/**
 * SAMSUNG MODEM IPC version 4
 *
 * Copyright (C) 2010 Samsung Electronics. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */

//#define DEBUG

#if defined(DEBUG)
//#define NOISY_DEBUG
#define PERF_DEBUG
#endif

#include "pdp.h"
#include "sipc.h"
#include "sipc4.h"

#define SVNET_CIRC
#if defined(SVNET_CIRC)
#include "circ.h"
#else
#include <linux/circ_buf.h>
#endif

#include <linux/workqueue.h>
#include <asm/errno.h>

#include <net/sock.h>
#include <linux/if_ether.h>
#include <linux/phonet.h>
#include <net/phonet/phonet.h>
#include <linux/skbuff.h>

#include <linux/onedram.h>

#define CONFIG_KERNEL_DEBUG_SEC_
#if defined(CONFIG_KERNEL_DEBUG_SEC_)
#include <linux/kernel_sec_common.h>
#define ERRMSG "LTE Crash"
static char cp_errmsg[65];
static void _go_dump(struct sipc *si);
#else
#define _go_dump(si) do { } while(0)
#endif

#if defined(NOISY_DEBUG) || defined(PERF_DEBUG)
static struct device *_dev;
#endif

#if defined(NOISY_DEBUG)
#define _dbg(format, arg...) dev_dbg(_dev, format, ## arg)
#else
#define _dbg(format, arg...) do { } while (0)
#endif

#if defined(PERF_DEBUG)
#define _pdbg(format, arg...) dev_dbg(_dev, format, ## arg)
#else
#define _pdbg(format, arg...) do { } while (0)
#endif

#if 0
#define dbg_loga(s, args...) printk(KERN_ERR "[SIPC4] <%s:%d> " s, __func__, __LINE__, ##args)
#define dbg_loge(s, args...) printk(KERN_ERR "[SIPC4/Err] <%s:%d> " s, __func__, __LINE__, ##args)
#define dbg_logi(s, args...) printk(KERN_ERR s, ##args)
#define dbg_log(s, args...)  printk(s, ##args)
#else
#define dbg_loga(s, args...)
#define dbg_loge(s, args...) printk(KERN_ERR "[SIPC4/Err] <%s:%d> " s, __func__, __LINE__, ##args)
#define dbg_logi(s, args...)
#define dbg_log(s, args...)
#endif

//#define LOOP_BACK_TEST
#ifdef LOOP_BACK_TEST
struct loopback_result
{
	int nTransfered;
	int nPacketDataSize;
	struct timeval nStartTime;
	struct timeval nEndTime;
};

extern struct loopback_result loopback_res;
extern int loopback_ongoing;

char loopback_data_priv [1500];
#endif

const char *sipc_version = "4.1";

static const char hdlc_start[1] = { HDLC_START };
static const char hdlc_end[1] = { HDLC_END };

struct mailbox_data {
	u16 mask_send;
	u16 mask_req_ack;
	u16 mask_res_ack;
};

static struct mailbox_data mb_data[IPCIDX_MAX] = {
	{
		.mask_send = MBD_SEND_FMT,
		.mask_req_ack = MBD_REQ_ACK_FMT,
		.mask_res_ack = MBD_RES_ACK_FMT,
	},
	{
		.mask_send = MBD_SEND_RAW,
		.mask_req_ack = MBD_REQ_ACK_RAW,
		.mask_res_ack = MBD_RES_ACK_RAW,
	},
	{
		.mask_send = MBD_SEND_RFS,
		.mask_req_ack = MBD_REQ_ACK_RFS,
		.mask_res_ack = MBD_RES_ACK_RFS,
	},
};

/* semaphore latency */
unsigned long long time_max_semlat;
//static volatile unsigned long *TCNT = (unsigned long *)0xF520000C;

struct sipc;
struct ringbuf;

struct ringbuf_info {
	unsigned int out_off;
	unsigned int out_size;
	unsigned int in_off;
	unsigned int in_size;
	int (*read)(struct sipc *si, int inbuf, struct ringbuf *rb);
};

struct ringbuf {
	unsigned char *out_base;
	unsigned char *in_base;
	struct ringbuf_mapped *out;
	struct ringbuf_mapped *in;
	struct ringbuf_info *info;
};
#define rb_in_size info->in_size
#define rb_out_size info->out_size
#define rb_read info->read
#define rb_out_head out->head
#define rb_out_tail out->tail
#define rb_in_head in->head
#define rb_in_tail in->tail


static int _read_fmt(struct sipc *si, int inbuf, struct ringbuf *rb);
static int _read_raw(struct sipc *si, int inbuf, struct ringbuf *rb);
static int _read_rfs(struct sipc *si, int inbuf, struct ringbuf *rb);

static struct ringbuf_info rb_info[IPCIDX_MAX] = {
	{
		.out_off = FMT_OUT,
		.out_size = FMT_OUT_SZ,
		.in_off = FMT_IN,
		.in_size = FMT_IN_SZ,
		.read = _read_fmt,
	},
	{
		.out_off = RAW_OUT,
		.out_size = RAW_OUT_SZ,
		.in_off = RAW_IN,
		.in_size = RAW_IN_SZ,
		.read = _read_raw,
	},
	{
		.out_off = RFS_OUT,
		.out_size = RFS_OUT_SZ,
		.in_off = RFS_IN,
		.in_size = RFS_IN_SZ,
		.read = _read_rfs,
	},
};

#define FRAG_BLOCK_MAX (PAGE_SIZE - sizeof(struct list_head) \
		- sizeof(u32) - sizeof(char *))
struct frag_block {
	struct list_head list;
	u32 len;
	char *ptr;
	char buf[FRAG_BLOCK_MAX];
};

struct frag_list {
	struct list_head list;
	u8 msg_id;
	u32 len;
	// timeout??
	struct list_head block_head;
};

struct frag_head {
	struct list_head head;
	unsigned long bitmap[FMT_ID_SIZE/BITS_PER_LONG];
};

struct frag_info {
	struct sk_buff *skb;
	unsigned int offset;
	u8 msg_id;
};

struct sipc {
	struct sipc_mapped *map;
	struct ringbuf rb[IPCIDX_MAX];

	struct resource *res;

	void (*queue)(u32, void *);
	void *queue_data;

	/* for fragmentation */
	u8 msg_id;
	char *frag_buf;
	struct frag_info frag;

	/* for merging */
	struct frag_head frag_map;

	int od_rel; /* onedram authority release */

	struct net_device *svndev;

	const struct attribute_group *group;

	struct sk_buff_head rfs_rx;
};

/* sizeof(struct phonethdr) + NET_SKB_PAD > SMP_CACHE_BYTES */
//#define RFS_MTU (PAGE_SIZE - sizeof(struct phonethdr) - NET_SKB_PAD)
/* SMP_CACHE_BYTES > sizeof(struct phonethdr) + NET_SKB_PAD */
#define RFS_MTU (PAGE_SIZE - SMP_CACHE_BYTES)
#define RFS_TX_RATE 4

/* TODO: move PDP related codes to other source file */
static DEFINE_MUTEX(pdp_mutex);
static struct net_device *pdp_devs[PDP_MAX];
static int pdp_cnt;
unsigned long pdp_bitmap[PDP_MAX/BITS_PER_LONG];

static void clear_pdp_wq(struct work_struct *work);
static DECLARE_WORK(pdp_work, clear_pdp_wq);

static ssize_t show_act(struct device *d,
		struct device_attribute *attr, char *buf);
static ssize_t show_deact(struct device *d,
		struct device_attribute *attr, char *buf);
static ssize_t store_act(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count);
static ssize_t store_deact(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count);

static ssize_t show_suspend(struct device *d,
		struct device_attribute *attr, char *buf);
static ssize_t store_suspend(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count);
static ssize_t store_resume(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count);

static DEVICE_ATTR(activate, S_IRUGO | S_IWUGO, show_act, store_act);
static DEVICE_ATTR(deactivate, S_IRUGO | S_IWUGO, show_deact, store_deact);
static DEVICE_ATTR(suspend, S_IRUGO | S_IWUGO, show_suspend, store_suspend);
static DEVICE_ATTR(resume, S_IRUGO | S_IWUGO, NULL, store_resume);

static struct attribute *pdp_attributes[] = {
	&dev_attr_activate.attr,
	&dev_attr_deactivate.attr,
	&dev_attr_suspend.attr,
	&dev_attr_resume.attr,
	NULL
};

static const struct attribute_group pdp_group = {
	.name = "pdp",
	.attrs = pdp_attributes,
};

extern int onedram_read_mailbox(u32 *mb);
extern int onedram_write_mailbox(u32 mb);
extern int onedram_get_auth(u32 cmd);
extern int onedram_put_auth(int release);
extern int onedram_rel_sem(void);
extern struct resource* onedram_request_region(resource_size_t start,
		resource_size_t n, const char *name);
extern void onedram_release_region(resource_size_t start, resource_size_t n);
extern int onedram_register_handler(void (*handler)(u32, void *), void *data);
extern int onedram_unregister_handler(void (*handler)(u32, void *));

#if defined(NOISY_DEBUG)
#define DUMP_LIMIT 32
static char dump_buf[64];
void _dbg_dump(u8 *buf, int size)
{
	int i;
	int len = 0;

	if (!buf)
		return;

	if (size > DUMP_LIMIT)
		size = DUMP_LIMIT;

	for (i=0;i<32 && i<size;i++) {
		len += sprintf(&dump_buf[len], "%02x ", buf[i]);
		if ((i & 0xf) == 0xf) {
			dump_buf[len] = '\0';
			_dbg("dump %04x [ %s]\n", (i>>4), dump_buf);
			len = 0;
		}
	}
	if (len) {
		dump_buf[len] = '\0';
		_dbg("dump %04x [ %s]\n", i, dump_buf);
	}
}
#else
#  define _dbg_dump(buf, size) do { } while(0)
#endif

//2 DK06 Dr.J ADD [[
#if 1
#include "ipc_v40.h"

#ifndef byte
#define byte unsigned char
#endif

typedef __packed struct {
   byte start_flag;
   byte len_packet[2];
   byte control;
   byte len[2];
   byte msg_seq;
   byte ack_seq;
   byte main_cmd;
   byte sub_cmd;
   byte cmd_type;
} format_hdr_type;

typedef __packed struct {
   byte len[2];
   byte msg_seq;
   byte ack_seq;
   byte main_cmd;
   byte sub_cmd;
   byte cmd_type;
} format_sipc_hdr_type;

static const char* getResponseTypeName( int cmd_type);
static const char* getCMDTypeName( int cmd_type);
static const char* getMainCommandName( int main_cmd);
static const char* getSubCommandName( int main_cmd, int sub_cmd);


void print_rx_fmt_frame(struct fmt_hdr *h, const u8 *psrc, u32 size)
{
    int              i = 0;
    int              loopcount = 0;
    int              fmt_len = h->len;
    format_hdr_type  fmt_hdr;
    int              fmt_hdr_size = sizeof(format_hdr_type);
    u8              *fmt_data = (psrc + 7);
    int              param_len = 0;

    memcpy(&fmt_hdr.len, psrc, 7);

    if ( (fmt_hdr.main_cmd == IPC_MISC_CMD) &&
         (fmt_hdr.sub_cmd == IPC_MISC_ME_VERSION) )
    {
        return;
    }

    dbg_log("\n");
    dbg_loga("(RX) Formatted HDLC packet (size %d) =\n", (fmt_len + 2));
    dbg_logi("[SIPC4] -------------------------------IPC FMT RX HEADER-------------------------------\n");
    dbg_logi("[SIPC4] (M)%s, (S)%s, (T)%s, len:%d, mseq:%d, aseq:%d\n",
              getMainCommandName((int)fmt_hdr.main_cmd),
              getSubCommandName((int)fmt_hdr.main_cmd, (int)fmt_hdr.sub_cmd),
              getResponseTypeName((int)fmt_hdr.cmd_type),
              fmt_len,
              fmt_hdr.msg_seq,
              fmt_hdr.ack_seq);
    dbg_logi("[SIPC4] --------------------------------IPC FMT RX DATA--------------------------------\n");
    if (size > 7)
        dbg_logi("[SIPC4] ");
    param_len = (size - 7);
    for (i = param_len; i > 0; i--)
    {
        loopcount++;
        dbg_log("%02x ", *fmt_data++);
        // Insert new line at every 8 hex numbers
        if (loopcount % 8 == 0)
        {
            dbg_log("\n");
            if (loopcount < param_len)
                dbg_logi("[SIPC4] ");
        }
    }
    if (loopcount % 8 != 0)
        dbg_log("\n");
    dbg_logi("[SIPC4] -------------------------------------------------------------------------------\n");
    dbg_logi("\n");
}

void print_tx_hdlc_frame(int q_type, const u8 *psrc, u32 size)
{
    u32              copied_size = 0;
    int              i = 0;
    int              loopcount = 0;
    u8              *fmt_pkt;
    int              fmt_len = 0;
    format_hdr_type  fmt_hdr;
    int              fmt_hdr_size = sizeof(format_hdr_type);

    if (q_type != IPCIDX_FMT)
    {
        dbg_log("\n");

        if (q_type == IPCIDX_RFS)
        {
            dbg_loga("RFS HDLC packet (size %d) =\n", size);
        }
        else //if (q_type == IPCIDX_RAW)
        {
            dbg_loga("RAW HDLC packet (size %d) =\n", size);
        }

        dbg_log("[SIPC4] ");
        for (i = 0; i < size; i++)
        {
            loopcount++;
            dbg_log("%02x ", *(psrc + i));
            // Insert new line at every 8 hex numbers
            if (loopcount % 8 == 0)
            {
                dbg_log("\n");
                if (loopcount < size)
                    dbg_log("[SIPC4] ");
            }
        }
        if (loopcount % 8 != 0)
            dbg_log("\n");

        dbg_log("\n");

        return;
    }

    fmt_pkt = psrc;
    copied_size = 0;
    while ((size - copied_size) > fmt_hdr_size)
    {
        memcpy(&fmt_hdr, fmt_pkt, fmt_hdr_size);

        if ( (fmt_hdr.main_cmd == IPC_MISC_CMD) &&
             (fmt_hdr.sub_cmd == IPC_MISC_ME_VERSION) )
        {
            break;
        }

        fmt_len = (fmt_hdr.len_packet[1] << 8) | fmt_hdr.len_packet[0];

        dbg_log("\n");
        dbg_loga("(TX) Formatted HDLC packet (size %d) =\n", (fmt_len + 2));
        dbg_logi("[SIPC4] -------------------------------IPC FMT TX HEADER-------------------------------\n");
        dbg_logi("[SIPC4] (M)%s, (S)%s, (T)%s, len:%d, mseq:%d, aseq:%d\n",
                  getMainCommandName((int)fmt_hdr.main_cmd),
                  getSubCommandName((int)fmt_hdr.main_cmd, (int)fmt_hdr.sub_cmd),
                  getCMDTypeName((int)fmt_hdr.cmd_type),
                  ((fmt_hdr.len[1] << 8) | fmt_hdr.len[0]),
                  fmt_hdr.msg_seq,
                  fmt_hdr.ack_seq);
        dbg_logi("[SIPC4] -------------------------------IPC FMT TX DATA--------------------------------\n");
        loopcount = 0;
        if ((fmt_len+1) > sizeof(format_hdr_type))
            dbg_logi("[SIPC4] ");
        for (i = sizeof(format_hdr_type); i < (fmt_len+1); i++)
        {
            loopcount++;
            dbg_log("%02x ", *(fmt_pkt + i));
            // Insert new line at every 8 hex numbers
            if (loopcount % 8 == 0)
            {
                dbg_log("\n");
                if (i < (fmt_len+1))
                    dbg_logi("[SIPC4] ");
            }
        }
        if (loopcount % 8 != 0)
            dbg_log("\n");
        dbg_logi("[SIPC4] -------------------------------------------------------------------------------\n");
        dbg_logi("\n");

        copied_size += (fmt_len + 2);  // 2 = start flag + end flag
        fmt_pkt = psrc + copied_size;
    }
}

void print_tx_sipc_frame(int q_type, const u8 *psrc, u32 size)
{
    int                   i = 0;
    int                   loopcount = 0;
    u8                   *fmt_pkt;
    int                   fmt_len = 0;
    format_sipc_hdr_type  fmt_hdr;
    int                   fmt_hdr_size = sizeof(format_sipc_hdr_type);

    if (q_type != IPCIDX_FMT)
    {
        if (q_type == IPCIDX_RFS)
        {
            dbg_loga("RFS IPC packet (size %d) =\n", size);
            dbg_log("[SIPC4] ");
            for (i = 0; i < size; i++)
            {
                loopcount++;
                dbg_log("%02x ", *(psrc + i));
                // Insert new line at every 8 hex numbers
                if (loopcount % 8 == 0)
                {
                    dbg_log("\n");
                    if (loopcount < size)
                        dbg_log("[SIPC4] ");
                }
            }
            if (loopcount % 8 != 0)
                dbg_log("\n");
        }

        return;
    }

    loopcount = 0;

    fmt_pkt = psrc;
    memcpy(&fmt_hdr, fmt_pkt, fmt_hdr_size);

    fmt_len = (fmt_hdr.len[1] << 8) | fmt_hdr.len[0];

    dbg_log("\n");
    dbg_loga("(TX) Formatted IPC packet (size %d) =\n", size);
    dbg_log("[SIPC4] ------------------------------IPC FMT TX HEADER-------------------------------\n");
    dbg_log("[SIPC4] (M)%s, (S)%s, (T)%s, len:%d, mseq:%d, aseq:%d\n",
              getMainCommandName((int)fmt_hdr.main_cmd),
              getSubCommandName((int)fmt_hdr.main_cmd, (int)fmt_hdr.sub_cmd),
              getCMDTypeName((int)fmt_hdr.cmd_type),
              fmt_len,
              fmt_hdr.msg_seq,
              fmt_hdr.ack_seq);
    dbg_log("[SIPC4] -------------------------------IPC FMT TX DATA--------------------------------\n");
    if (size > sizeof(format_sipc_hdr_type))
        dbg_log("[SIPC4] ");
    for (i = sizeof(format_sipc_hdr_type); i < fmt_len; i++)
    {
        loopcount++;
        dbg_log("%02x ", *(fmt_pkt + i));
        // Insert new line at every 8 hex numbers
        if (loopcount % 8 == 0)
        {
            dbg_log("\n");
            if (i < fmt_len)
                dbg_log("[SIPC4] ");
        }
    }
    if (loopcount % 8 != 0)
        dbg_log("\n");
    dbg_log("[SIPC4] -------------------------------------------------------------------------------\n");
    dbg_log("\n");
}

const char* getResponseTypeName( int cmd_type)
{
    switch(cmd_type)
    {
        case IPC_CMD_INDI :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_INDI";
            break;
    
        case IPC_CMD_RESP :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_RESP";
            break;
    
        case IPC_CMD_NOTI :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_NOTI";
            break;
    
         default :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "CMD_TYPE_UNDEFINED";
            break;
    }
}

const char* getCMDTypeName( int cmd_type)
{
    switch(cmd_type)
    {
        case IPC_CMD_EXEC :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_EXEC";
            break;
    
        case IPC_CMD_GET :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_GET";
            break;
    
        case IPC_CMD_SET :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_SET";
            break;
    
        case IPC_CMD_CFRM :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_CFRM";
            break;
    
        case IPC_CMD_EVENT :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CMD_EVENT";
            break;
    
         default :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "CMD_TYPE_UNDEFINED";
            break;

    }
}

const char* getMainCommandName( int main_cmd)
{
    switch(main_cmd)
    {
        case IPC_PWR_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_PWR_CMD";
            break;
    
        case IPC_CALL_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CALL_CMD";
            break;
    
        case IPC_CDMA_DATA_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CDMA_DATA_CMD";
            break;
    
        case IPC_SMS_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SMS_CMD";
            break;
    
        case IPC_SEC_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SEC_CMD";
            break;
    
        case IPC_PB_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_PB_CMD";
            break;
    
        case IPC_DISP_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_DISP_CMD";
            break;
    
        case IPC_NET_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_NET_CMD";
            break;
    
        case IPC_SND_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SND_CMD";
            break;
    
        case IPC_MISC_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_MISC_CMD";
            break;
    
        case IPC_SVC_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SVC_CMD";
            break;
    
        case IPC_SS_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SS_CMD";
            break;
    
        case IPC_GPRS_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_GPRS_CMD";
            break;
    
        case IPC_SAT_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SAT_CMD";
            break;
    
        case IPC_CFG_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_CFG_CMD";
            break;
    
        case IPC_IMEI_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_IMEI_CMD";
            break;
    
        case IPC_GPS_CMD:
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_GPS_CMD";
            break;
    
        case IPC_SAP_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_SAP_CMD";
            break;
    
        case IPC_FACTORY_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_FACTORY_CMD";
            break;

        case IPC_OMADM_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_OMADM_CMD";
            break;
    
        case IPC_GEN_CMD :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "IPC_GEN_CMD";
            break;
    
        default :
            // ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
            return "MAIN_CMD_UNDEFINED";
            break; 
    }
    return 0;
}

const char* getSubCommandName( int main_cmd, int sub_cmd)
{
    switch(main_cmd)
    {
        case IPC_PWR_CMD :
            switch(sub_cmd)
                {
                    case IPC_PWR_PHONE_PWR_UP :
                        return "IPC_PWR_PHONE_PWR_UP";
                        break;
                    
                    case IPC_PWR_PHONE_PWR_OFF :
                        return "IPC_PWR_PHONE_PWR_OFF";
                        break;
                    
                    case IPC_PWR_PHONE_RESET :
                        return "IPC_PWR_PHONE_RESET";
                        break;
                    
                    case IPC_PWR_BATT_STATUS :
                        return "IPC_PWR_BATT_STATUS";
                        break;
                    
                    case IPC_PWR_BATT_TYPE :
                        return "IPC_PWR_BATT_TYPE";
                        break;
                    
                    case IPC_PWR_BATT_COMP:
                        return "IPC_PWR_BATT_COMP";
                        break;
                    
                    case IPC_PWR_PHONE_STATE:
                        return "IPC_PWR_PHONE_STATE";
                        break;
                    
                    default :
                        return "PWR_SUB_CMD_UNDEFINED";
                        break;            
                }
            break;
    
        case IPC_CALL_CMD :
            switch(sub_cmd)
            {
                case IPC_CALL_OUTGOING :
                    return "IPC_CALL_OUTGOING";
                    break;
                
                case IPC_CALL_INCOMING :
                    return "IPC_CALL_INCOMING";
                    break;
                
                case IPC_CALL_RELEASE :
                    return "IPC_CALL_RELEASE";
                    break;
                
                case IPC_CALL_ANSWER :
                    return "IPC_CALL_ANSWER";
                    break;
                
                case IPC_CALL_STATUS :
                    return "IPC_CALL_STATUS";
                    break;
                
                case IPC_CALL_LIST :
                    return "IPC_CALL_LIST";
                    break;
                
                case IPC_CALL_BURST_DTMF :
                    return "IPC_CALL_BURST_DTMF";
                    break;
                
                case IPC_CALL_CONT_DTMF :
                    return "IPC_CALL_CONT_DTMF";
                    break;
                
                case IPC_CALL_WAITING :
                    return "IPC_CALL_WAITING";
                    break;
                
                case IPC_CALL_LINE_ID :
                    return "IPC_CALL_LINE_ID";
                    break;
                
                case IPC_CALL_SIGNAL :
                    return "IPC_CALL_SIGNAL";
                    break;
                
                case IPC_CALL_VOICE_PRIVACY :
                    return "IPC_CALL_VOICE_PRIVACY";
                    break;
                
                case IPC_CALL_CALL_TIME_COUNT :
                    return "IPC_CALL_CALL_TIME";
                    break;
                
                case IPC_CALL_OTA_PROGRESS :
                    return "IPC_CALL_OTA_PROGRESS_IND";
                    break;
                
                case IPC_CALL_DIAG_OUTGOING :
                    return "IPC_CALL_DIAG_OUTGOING";
                    break;
                
                case IPC_CALL_E911_CB_MODE :
                    return "IPC_CALL_E911_CB_MODE";
                    break;

                case IPC_CALL_FLASH_INFO :
                    return "IPC_CALL_FLASH_INFO";
                    break;
                
                default :
                    return "CALL_SUB_CMD_UNDEFINED";
                    break;
            }
            break;		

 
        case IPC_CDMA_DATA_CMD :		
            
            switch(sub_cmd)
            {
                case IPC_CDMA_DATA_TE2_STATUS :
                    return "IPC_CDMA_DATA_TE2_STATUS";
                    break;
                
                case IPC_CDMA_DATA_BYTE_COUNTER :
                    return "IPC_CDMA_DATA_BYTE_COUNTER";
                    break;
                
                case IPC_CDMA_DATA_INCOMING_CALL_TYPE :
                    return "IPC_CDMA_DATA_INCOMING_CALL_TYPE";
                    break;
                
                case IPC_CDMA_DATA_TE2_DIALING_INFO :
                    return "IPC_CDMA_DATA_TE2_DIALING_INFO";
                    break;
                
                case IPC_CDMA_DATA_TE2_DATA_RATE_INFO :
                    return "IPC_CDMA_DATA_TE2_DATA_RATE_INFO";
                    break;
                
                case IPC_CDMA_DATA_PACKET_DATA_CALL_CFG :
                    return "IPC_CDMA_DATA_PACKET_DATA_CALL_CFG";
                    break;
                    
                case IPC_CDMA_DATA_DS_BAUD_RATE :
                    return "IPC_CDMA_DATA_DS_BAUD_RATE";
                    break;
                    
                case IPC_CDMA_DATA_MOBILE_IP_NAI :
                    return "IPC_CDMA_DATA_MOBILE_IP_NAI";
                    break;
                    
                case IPC_CDMA_DATA_CURRENT_NAI_INDEX :
                    return "IPC_CDMA_DATA_CURRENT_NAI_INDEX";
                    break;
                    
                case IPC_CDMA_DATA_DORMANT_CONFIG :
                    return "IPC_CDMA_DATA_DORMANT_CONFIG";
                    break;
                    
                case IPC_CDMA_DATA_MIP_NAI_CHANGED :
                    return "IPC_CDMA_DATA_MIP_NAI_CHANGED";
                    break;
                    
                case IPC_CDMA_DATA_SIGNEDIN_STATE :
                    return "IPC_CDMA_DATA_SIGNEDIN_STATE";
                    break;
                    
                case IPC_CDMA_DATA_RESTORE_NAI :
                    return "IPC_CDMA_DATA_RESTORE_NAI";
                    break;
                    
                case IPC_CDMA_DATA_MIP_CONNECT_STATUS :
                    return "IPC_CDMA_DATA_MIP_CONNECT_STATUS";
                    break;
                    
                case IPC_CDMA_DATA_DORMANT_MODE_STATUS :
                    return "IPC_CDMA_DATA_DORMANT_MODE_STATUS";
                    break;
                    
                case IPC_CDMA_DATA_R_SCH_CONFIG :
                    return "IPC_CDMA_DATA_R_SCH_CONFIG";
                    break;
                    
                case IPC_CDMA_DATA_HDR_SESSION_CLEAR :
                    return "IPC_CDMA_DATA_HDR_SESSION_CLEAR";
                    break;
                    
                case IPC_CDMA_DATA_SESSION_CLOSE_TIMER_EXPIRED :
                    return "IPC_CDMA_DATA_SESSION_CLOSE_TIMER_EXPIRED";
                    break;
                    
                case IPC_CDMA_DATA_KEEPALIVETIMER_VALUE :
                    return "IPC_CDMA_DATA_KEEPALIVETIMER_VALUE";
                    break;
                    
                case IPC_CDMA_DATA_DDTMMODE_CONFIG :
                    return "IPC_CDMA_DATA_DDTMMODE_CONFIG";
                    break;
                    
                case IPC_CDMA_DATA_ROAM_GUARD :
                    return "IPC_CDMA_DATA_ROAM_GUARD";
                    break;
                    
                case IPC_CDMA_DATA_MODEM_NAI :
                    return "IPC_CDMA_DATA_MODEM_NAI";
                    break;
                    
                case IPC_CDMA_DATA_KOREA_MODE :
                    return "IPC_CDMA_DATA_KOREA_MODE";
                    break;
                case IPC_CDMA_DATA_DATA_SERVICE_TYPE :
                    return "IPC_CDMA_DATA_DATA_SERVICE_TYPE";
                    break;
                
                case IPC_CDMA_DATA_FORCE_REV_A_MODE :
                    return "IPC_CDMA_DATA_FORCE_REV_A_MODE";
                    break;
                
                case IPC_CDMA_DATA_CUSTOM_CONFIG_MODE :
                    return "IPC_CDMA_DATA_CUSTOM_CONFIG_MODE";
                    break;
                
                case IPC_CDMA_DATA_NAI_SETTING_MODE :
                    return "IPC_CDMA_DATA_NAI_SETTING_MODE";
                    break;
                
                case IPC_CDMA_DATA_PIN_CTRL :
                    return "IPC_CDMA_DATA_PIN_CTRL";
                    break;
                
                default :
                    return "DATA_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;		
  
        case IPC_SMS_CMD :    
            switch(sub_cmd)
            {
                case IPC_SMS_SEND_MSG :
                    return "IPC_SMS_SEND_MSG";
                    break;
                
                case IPC_SMS_INCOMING_MSG :
                    return "IPC_SMS_INCOMING_MSG";
                    break;
                
                case IPC_SMS_READ_MSG :
                    return "IPC_SMS_READ_MSG";
                    break;
                
                case IPC_SMS_SAVE_MSG :
                    return "IPC_SMS_SAVE_MSG";
                    break;
                
                case IPC_SMS_DEL_MSG :
                    return "IPC_SMS_DEL_MSG";
                    break;
                
                case IPC_SMS_DELIVER_REPORT :
                    return "IPC_SMS_DELIVER_REPORT";
                    break;
                
                case IPC_SMS_DEVICE_READY :
                    return "IPC_SMS_DEVICE_READY";
                    break;
                
                case IPC_SMS_SEL_MEM :
                    return "IPC_SMS_SEL_MEM";
                    break;
                
                case IPC_SMS_STORED_MSG_COUNT :
                    return "IPC_SMS_STORED_MSG_COUNT";
                    break;
                
                case IPC_SMS_SVC_CENTER_ADDR :
                    return "IPC_SMS_SVC_CENTER_ADDR";
                    break;
                
                case IPC_SMS_SVC_OPTION :
                    return "IPC_SMS_SVC_OPTION";
                    break;
                
                case IPC_SMS_MEM_STATUS :
                    return "IPC_SMS_MEM_STATUS";
                    break;
                
                case IPC_SMS_CBS_MSG:
                    return "IPC_SMS_CBS_MSG";
                    break;
                
                case IPC_SMS_CBS_CFG :
                    return "IPC_SMS_CBS_CFG";
                    break;
                
                case IPC_SMS_STORED_MSG_STATUS :
                    return "IPC_SMS_STORED_MSG_STATUS";
                    break;
                
                case IPC_SMS_PARAM_COUNT :
                    return "IPC_SMS_PARAM_COUNT";
                    break;
                
                case IPC_SMS_PARAM :
                    return "IPC_SMS_PARAM";
                    break;
            
                case IPC_SMS_STATUS :
                    return "IPC_SMS_STATUS";
                    break;
                
                default :
                    return "SMS_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;		
  
        case IPC_SEC_CMD :
            switch(sub_cmd)
            {
                case IPC_SEC_PIN_STATUS :
                    return "IPC_SEC_PIN_STATUS";
                    break;
                
                case IPC_SEC_PHONE_LOCK :
                    return "IPC_SEC_PHONE_LOCK";
                    break;
                
                case IPC_SEC_CHANGE_LOCKING_PW :
                    return "IPC_SEC_CHANGE_LOCKING_PW";
                    break;
                
                case IPC_SEC_SIM_LANG :
                    return "IPC_SEC_SIM_LANG";
                    break;
                
                case IPC_SEC_RSIM_ACCESS :
                    return "IPC_SEC_RSIM_ACCESS";
                    break;
                
                case IPC_SEC_GSIM_ACCESS :
                    return "IPC_SEC_GSIM_ACCESS";
                    break;
                
                case IPC_SEC_SIM_ICC_TYPE :
                    return "IPC_SEC_SIM_ICC_TYPE";
                    break;
                
                case IPC_SEC_LOCK_INFOMATION :
                    return "IPC_SEC_LOCK_INFOMATION";
                    break;
                
                case IPC_SEC_IMS_AUTH :
                    return "IPC_SEC_IMS_AUTH";
                    break;  
            
                default :
                return "SEC_SUB_CMD_UNDEFINED";
                break; 
            }
            break;
			
	
        case IPC_PB_CMD :    
            switch(sub_cmd)
            {
                case IPC_PB_ACCESS :
                    return "IPC_PB_ACCESS";
                    break;
                
                case IPC_PB_STORAGE :
                    return "IPC_PB_STORAGE";
                    break;
                
                case IPC_PB_STORAGE_LIST :
                    return "IPC_PB_STORAGE_LIST";
                    break;
                
                case IPC_PB_ENTRY_INFO :
                    return "IPC_PB_ENTRY_INFO";
                    break;
                
                case IPC_PB_3GPB_CAPA:
                    return "IPC_PB_3GPB_CAPA";
                    break;
                
                default :
                    return "PB_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;
	
	
        case IPC_DISP_CMD :    
            switch(sub_cmd)
            {
                case IPC_DISP_ICON_INFO :
                    return "IPC_DISP_ICON_INFO";
                    break;
                
                case IPC_DISP_HOMEZONE_INFO:
                    return "IPC_DISP_HOMEZONE_INFO";
                    break;
            
                case IPC_DISP_PHONE_FATAL_INFO:
                    return "IPC_DISP_PHONE_FATAL_INFO";
                    break;
            
                case IPC_DISP_EXT_ROAM_INFO:
                    return "IPC_DISP_EXT_ROAM_INFO";
                    break;
            
                case IPC_DISP_USER_INDICATION:
                    return "IPC_DISP_USER_INDICATION";
                    break;	
                
                default :
                    return "DISP_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;

	
        case IPC_NET_CMD :    
            switch(sub_cmd)
            {
                case IPC_NET_PREF_PLMN :
                    return "IPC_NET_PREF_PLMN";
                    break;
                
                case IPC_NET_PLMN_SEL :
                    return "IPC_NET_PLMN_SEL";
                    break;    
                
                case IPC_NET_SERVING_NETWORK :
                    return "IPC_NET_SERVING_NETWORK";
                    break;
                
                case IPC_NET_PLMN_LIST :
                    return "IPC_NET_PLMN_LIST";
                    break;
                
                case IPC_NET_REGIST :
                    return "IPC_NET_REGIST";
                    break;
                
                case IPC_NET_SUBSCRIBER_NUM :
                    return "IPC_NET_SUBSCRIBER_NUM";
                    break;
                
                case IPC_NET_BAND_SEL :
                    return "IPC_NET_BAND_SEL";
                    break;
                
                case IPC_NET_SERVICE_DOMAIN_CONFIG :
                    return "IPC_NET_SERVICE_DOMAIN_CONFIG";
                    break;
                
                case IPC_NET_POWERON_ATTACH :
                    return "IPC_NET_POWERON_ATTACH";
                    break;
                
                case IPC_NET_MODE_SEL :
                    return "IPC_NET_MODE_SEL";
                    break;
                
                case IPC_NET_ACQ_ORDER:
                    return "IPC_NET_ACQ_ORDER";
                    break;
                
                case IPC_NET_IDENTITY:
                    return "IPC_NET_IDENTITY";
                    break;
            
                case IPC_NET_PREFERRED_NETWORK_INFO:
                    return "IPC_NET_PREFERRED_NETWORK_INFO";
                    break;
            
                case IPC_NET_HYBRID_MODE:
                    return "IPC_NET_HYBRID_MODE";
                    break;
            
                default :
                    return "NET_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;		


        case IPC_SND_CMD :    
            switch(sub_cmd)
            {
                case IPC_SND_KEY_TONE :
                    return "IPC_SND_KEY_TONE";
                    break;
                
                case IPC_SND_NOTI_TONE :
                    return "IPC_SND_NOTI_TONE";
                    break;
                
                case IPC_SND_LED_CTRL :
                    return "IPC_SND_LED_CTRL";
                    break;
                
                case IPC_SND_VIB_CTRL :
                    return "IPC_SND_VIB_CTRL";
                    break;
                
                case IPC_SND_SPKR_VOLUME_CTRL :
                    return "IPC_SND_SPKR_VOLUME_CTRL";
                    break;
                
                case IPC_SND_MIC_GAIN_CTRL :
                    return "IPC_SND_MIC_GAIN_CTRL";
                    break;
                
                case IPC_SND_MIC_MUTE_CTRL :
                    return "IPC_SND_MIC_MUTE_CTRL";
                    break;
            
                case IPC_SND_SPKR_PHONE_CTRL :
                    return "IPC_SND_SPKR_PHONE_CTRL";
                    break;
            	
                case IPC_SND_HFK_AUDIO_STARTSTOP :
                    return "IPC_SND_HFK_AUDIO_STARTSTOP";
                    break;
            
                case IPC_SND_VOICECALL_RECORD_REPORT :
                    return "IPC_SND_VOICECALL_RECORD_REPORT";
                    break;
                
                case IPC_SND_AUDIO_PATH_CTRL :
                    return "IPC_SND_AUDIO_PATH_CTRL";
                    break;
                
                case IPC_SND_AUDIO_SOURCE_CTRL :
                    return "IPC_SND_AUDIO_SOURCE_CTRL";
                    break;
                
                case IPC_SND_USER_SND_CONFIG :
                    return "IPC_SND_USER_SND_CONFIG";
                    break;
            
                case IPC_SND_GAIN_CTRL :
                    return "IPC_SND_GAIN_CTRL";
                    break;
            
                case IPC_SND_QUIET_MODE_CTRL :
                    return "IPC_SND_QUIET_MODE_CTRL";
                    break;
            
                case IPC_SND_DYVE_MODE_CTRL :
                    return "IPC_SND_DYVE_MODE_CTRL";
                    break;
            	
                default :
                    return "SND_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;
	
    
        case IPC_MISC_CMD :    
            switch(sub_cmd)
            {
                case IPC_MISC_ME_VERSION :
                    return "IPC_MISC_ME_VERSION";
                    break;
                
                case IPC_MISC_ME_IMSI :
                    return "IPC_MISC_ME_IMSI";
                    break;
                
                case IPC_MISC_ME_SN :
                    return "IPC_MISC_ME_SN";
                    break;
            
                case IPC_MISC_KEY_EVENT_PROCESS :
                    return "IPC_MISC_KEY_EVENT_PROCESS";
                    break;
            
                case IPC_MISC_TIME_INFO :
                    return "IPC_MISC_TIME_INFO";
                    break;
            
                case IPC_MISC_NAM_INFO :
                    return "IPC_MISC_NAM_INFO";
                    break;
            
                case IPC_MISC_VCALL_CHANNEL_ID :
                    return "IPC_MISC_VCALL_CHANNEL_ID";
                    break;
            
                case IPC_MISC_PHONE_DEBUG :
                    return "IPC_MISC_PHONE_DEBUG";
                    break;	
            
                case IPC_MISC_FUS :
                    return "IPC_MISC_FUS";
                    break;	
            
                default :
                    return "MISC_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;
    

        case IPC_SVC_CMD :    
            switch(sub_cmd)
            {
                case IPC_SVC_ENTER :
                    return "IPC_SVC_ENTER";
                    break;
                
                case IPC_SVC_END :
                    return "IPC_SVC_END";
                    break;
                
                case IPC_SVC_PRO_KEYCODE :
                    return "IPC_SVC_PRO_KEYCODE";
                    break;
                
                case IPC_SVC_SCREEN_CFG :
                    return "IPC_SVC_SCREEN_CFG";
                    break;
                
                case IPC_SVC_DISPLAY_SCREEN :
                    return "IPC_SVC_DISPLAY_SCREEN";
                    break;
                
                case IPC_SVC_CHANGE_SVC_MODE:
                    return "IPC_SVC_CHANGE_SVC_MODE";
                    break;
                
                case IPC_SVC_DEVICE_TEST:
                    return "IPC_SVC_DEVICE_TEST";
                    break;
                
                default :
                    return "SVC_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;		


        case IPC_SS_CMD :    
            switch(sub_cmd)
            {
                case IPC_SS_WAITING :
                    return "IPC_SS_WAITING";
                    break;
                
                case IPC_SS_CLI :
                    return "IPC_SS_CLI";
                    break;
                
                case IPC_SS_BARRING :
                    return "IPC_SS_BARRING";
                    break;
                
                case IPC_SS_BARRING_PW :
                    return "IPC_SS_BARRING_PW";
                    break;
                
                case IPC_SS_FORWARDING :
                    return "IPC_SS_FORWARDING";
                    break;
                
                case IPC_SS_INFO :
                    return "IPC_SS_INFO";
                    break;
                
                case IPC_SS_MANAGE_CALL :
                    return "IPC_SS_MANAGE_CALL";
                    break;
                
                case IPC_SS_USSD :
                    return "IPC_SS_USSD";
                    break;
                
                case IPC_SS_AOC :
                    return "IPC_SS_AOC";
                    break;
                
                case IPC_SS_RELEASE_COMPLETE :
                    return "IPC_SS_RELEASE_COMPLETE";
                    break;
                
                default :
                    return "SS_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;
	
    
        case IPC_GPRS_CMD:
            switch(sub_cmd)
            {
                case IPC_GPRS_DEFINE_PDP_CONTEXT:
                    return "IPC_GPRS_DEFINE_PDP_CONTEXT";
                    break;
                
                case IPC_GPRS_QOS:
                    return "IPC_GPRS_QOS";
                    break;
				
                case IPC_GPRS_PS:
                    return "IPC_GPRS_PS";
                    break;
                
                case IPC_GPRS_PDP_CONTEXT:
                    return "IPC_GPRS_PDP_CONTEXT";
                    break;
                
                case IPC_GPRS_ENTER_DATA:
                    return "IPC_GPRS_ENTER_DATA";
                    break;
                
                case IPC_GPRS_SHOW_PDP_ADDR:
                    return "IPC_GPRS_SHOW_PDP_ADDR";
                    break;
                
                case IPC_GPRS_MS_CLASS:
                    return "IPC_GPRS_MS_CLASS";
                    break;
                
                case IPC_GPRS_3G_QUAL_SRVC_PROFILE:
                    return "IPC_GPRS_3G_QUAL_SRVC_PROFILE";
                    break;
                
                case IPC_GPRS_IP_CONFIGURATION:
                    return "IPC_GPRS_IP_CONFIGURATION";
                    break;
                
                case IPC_GPRS_DEFINE_SEC_PDP_CONTEXT:
                    return "IPC_GPRS_DEFINE_SEC_PDP_CONTEXT";
                    break;
                
                case IPC_GPRS_TFT:
                    return "IPC_GPRS_TFT";
                    break;
                
                case IPC_GPRS_HSDPA_STATUS:
                    return "IPC_GPRS_HSDPA_STATUS";
                    break;
                
                case IPC_GPRS_CURRENT_SESSION_DATA_COUNTER:
                    return "IPC_GPRS_CURRENT_SESSION_DATA_COUNTER";
                    break;
                
                case IPC_GPRS_DATA_DORMANT:
                    return "IPC_GPRS_DATA_DORMANT";
                    break;
            
                case IPC_GPRS_PIN_CTRL:
                    return "IPC_GPRS_PIN_CTRL";
                    break;
                
                case IPC_GPRS_CALL_STATUS:
                    return "IPC_GPRS_CALL_STATUS";
                    break;
                
                default:
                    return "GPRS_SUB_CMD_UNDEFINED";
                    break;
            }
            break;


        case IPC_SAT_CMD :    
            switch(sub_cmd)
            {
                case IPC_SAT_PROFILE_DOWNLOAD :
                    return "IPC_SAT_PROFILE_DOWNLOAD";
                    break;
                
                case IPC_SAT_ENVELOPE_CMD:
                    return "IPC_SAT_ENVELOPE_CMD";
                    break;
                
                case IPC_SAT_PROACTIVE_CMD:
                    return "IPC_SAT_PROACTIVE_CMD";
                    break;
                
                case IPC_SAT_TERMINATE_USAT_SESSION :
                    return "IPC_SAT_TERMINATE_USAT_SESSION";
                    break;
                
                case IPC_SAT_EVENT_DOWNLOAD :
                    return "IPC_SAT_EVENT_DOWNLOAD";
                    break;
                
                case IPC_SAT_PROVIDE_LOCAL_INFO:
                    return "IPC_SAT_PROVIDE_LOCAL_INFO";
                    break;
                
                case IPC_SAT_POLLING :
                    return "IPC_SAT_POLLING";
                    break;
                
                case IPC_SAT_REFRESH :
                    return "IPC_SAT_REFRESH";
                    break;
                
                case IPC_SAT_SETUP_EVENT_LIST :
                    return "IPC_SAT_SETUP_EVENT_LIST";
                    break;
                
                case IPC_SAT_CALL_CONTROL_RESULT:
                    return "IPC_SAT_CALL_CONTROL_RESULT";
                    break;
                
                case IPC_SAT_IMAGE_CLUT:
                    return "IPC_SAT_IMAGE_CLUT";
                    break;
            
                case IPC_SAT_SETUP_CALL_PROCESSING:
                    return "IPC_SAT_SETUP_CALL_PROCESSING";
                    break;
                
                case IPC_SAT_SIM_INITIATE_MESSAGE:
                    return "IPC_SAT_SIM_INITIATE_MESSAGE";
                    break;
                
                default :
                    return "SAT_SUB_CMD_UNDEFINED";
                    break;
            }
            break;
        
        case IPC_CFG_CMD :    
            switch(sub_cmd)
            {
                case IPC_CFG_DEFAULT_CONFIG :
                    return "IPC_CFG_DEFAULT_CONFIG";
                    break;
                
                case IPC_CFG_EXTERNAL_DEVICE :
                    return "IPC_CFG_EXTERNAL_DEVICE";
                    break;
                
                case IPC_CFG_MAC_ADDRESS :
                    return "IPC_CFG_MAC_ADDRESS";
                    break;
                
                case IPC_CFG_CONFIGURATION_ITEM :
                    return "IPC_CFG_CONFIGURATION_ITEM";
                    break;

                case IPC_CFG_TTY :
                    return "IPC_CFG_TTY";
                    break;

                case IPC_CFG_HSDPA_TMP_SETTING :
                    return "IPC_CFG_HSDPA_TMP_SETTING";
                    break;

                case IPC_CFG_HSDPA_PERM_SETTING :
                    return "IPC_CFG_HSDPA_PERM_SETTING";
                    break;

                case IPC_CFG_SIO_MODE :
                    return "IPC_CFG_SIO_MODE";
                    break;

                case IPC_CFG_AKEY_VERIFY :
                    return "IPC_CFG_AKEY_VERIFY";
                    break;

                case IPC_CFG_MSL_INFO :
                    return "IPC_CFG_MSL_INFO";
                    break;

                case IPC_CFG_USER_LOCK_CODE :
                    return "IPC_CFG_USER_LOCK_CODE";
                    break;

                case IPC_CFG_USB_PATH :
                    return "IPC_CFG_USB_PATH";
                    break;

                case IPC_CFG_CURRENT_SVC_CARRIER :
                    return "IPC_CFG_CURRENT_SVC_CARRIER";
                    break;

                case IPC_CFG_RADIO_CONFIG :
                    return "IPC_CFG_RADIO_CONFIG";
                    break;

                case IPC_CFG_VOCODER_OPTION :
                    return "IPC_CFG_VOCODER_OPTION";
                    break;

                case IPC_CFG_TEST_SYS :
                    return "IPC_CFG_TEST_SYS";
                    break;

                case IPC_CFG_RECONDITIONED_DATE :
                    return "IPC_CFG_RECONDITIONED_DATE";
                    break;

                case IPC_CFG_PROTOCOL_REVISION :
                    return "IPC_CFG_PROTOCOL_REVISION";
                    break;

                case IPC_CFG_SLOT_MODE :
                    return "IPC_CFG_SLOT_MODE";
                    break;

                case IPC_CFG_ACTIVATION_DATE :
                    return "IPC_CFG_ACTIVATION_DATE";
                    break;

                case IPC_CFG_CURRENT_UATI :
                    return "IPC_CFG_CURRENT_UATI";
                    break;

                case IPC_CFG_QUICK_PAGING :
                    return "IPC_CFG_QUICK_PAGING";
                    break;

                case IPC_CFG_LMSC_INFO :
                    return "IPC_CFG_LMSC_INFO";
                    break;

                case IPC_CFG_TAS_INFO :
                    return "IPC_CFG_TAS_INFO";
                    break;

                case IPC_CFG_AUTH_INFO :
                    return "IPC_CFG_AUTH_INFO";
                    break;

                case IPC_CFG_HIDDEN_MENU_ACCESS :
                    return "IPC_CFG_HIDDEN_MENU_ACCESS";
                    break;

                case IPC_CFG_UTS_SMS_SEND :
                    return "IPC_CFG_UTS_SMS_SEND";
                    break;
                    
                case IPC_CFG_UTS_SMS_COUNT :
                    return "IPC_CFG_UTS_SMS_COUNT";
                    break;
                    
                case IPC_CFG_UTS_SMS_MSG :
                    return "IPC_CFG_UTS_SMS_MSG";
                    break;
                    
                case IPC_CFG_SCM_INFO :
                    return "IPC_CFG_SCM_INFO";
                    break;
                    
                case IPC_CFG_SCI_INFO :
                    return "IPC_CFG_SCI_INFO";
                    break;
                    
                case IPC_CFG_ACCOLC_INFO :
                    return "IPC_CFG_ACCOLC_INFO";
                    break;

                case IPC_CFG_MOBTERM_INFO :
                    return "IPC_CFG_MOBTERM_INFO";
                    break;
                    
                case IPC_CFG_1X_EVDO_DIVERSITY_CONFIG :
                    return "IPC_CFG_1X_EVDO_DIVERSITY_CONFIG";
                    break;
                    
                case IPC_CFG_DEVICE_CONFIGURATION :
                    return "IPC_CFG_DEVICE_CONFIGURATION";
                    break;
                    
                case IPC_CFG_USER_LOCK_CODE_STATUS :
                    return "IPC_CFG_USER_LOCK_CODE_STATUS";
                    break;
                    
                default :
                    return "CFG_SUB_CMD_UNDEFINED";
                    break; 
            }
            break;

        case IPC_IMEI_CMD :    
            switch(sub_cmd)
            {
                case IPC_IMEI_START :
                    return "IPC_IMEI_START";
                    break;
                
                case IPC_IMEI_CHECK_DEVICE_INFO :
                    return "IPC_IMEI_CHECK_DEVICE_INFO";
                    break;
                
                case IPC_IMEI_PRE_CONFIG :
                    return "IPC_IMEI_PRE_CONFIG";
                    break;
                
                case IPC_IMEI_WRITE_ITEM :
                    return "IPC_IMEI_WRITE_ITEM";
                    break;
                
                case IPC_IMEI_REBOOT :
                    return "IPC_IMEI_REBOOT";
                    break;
                
                case IPC_IMEI_VERIFY_FACTORY_RESET :
                    return "IPC_IMEI_VERIFY_FACTORY_RESET";
                    break;
                
                case IPC_IMEI_COMPARE_ITEM :
                    return "IPC_IMEI_COMPARE_ITEM";
                    break;

                case IPC_IMEI_MASS_STORAGE_INFO :
                    return "IPC_IMEI_MASS_STORAGE_INFO";
                    break;    

                default :
                    return "IMEI_SUB_CMD_UNDEFINED";
                    break; 
            }	
            break;
    
        case IPC_GPS_CMD:    
            switch(sub_cmd)
            {
                case IPC_GPS_OPEN:
                    return "IPC_GPS_OPEN";
                    break;
                
                case IPC_GPS_CLOSE:
                    return "IPC_GPS_CLOSE";
                    break;  

                case IPC_GPS_START:
                    return "IPC_GPS_START";
                    break;
                
                case IPC_GPS_DEVICE_STATE:
                    return "IPC_GPS_DEVICE_STATE";
                    break;
                
                case IPC_GPS_VERIFICATION:
                    return "IPC_GPS_VERIFICATION";
                    break;
                
                case IPC_GPS_OPTION:
                    return "IPC_GPS_OPTION";
                    break;
                
                case IPC_GPS_TTFF:
                    return "IPC_GPS_TTFF";
                    break;

		case IPC_GPS_LOCK_MODE:
		    return "IPC_GPS_LOCK_MODE";

		case IPC_GPS_SECURITY_UPDATE:
		    return "IPC_GPS_SECURITY_UPDATE";

		case IPC_GPS_SSD:
		    return "IPC_GPS_SSD";

		case IPC_GPS_SECURITY_UPDATE_RATE:
		    return "IPC_GPS_SECURITY_UPDATE_RATE";

		case IPC_GPS_FIX_REQ:
		    return "IPC_GPS_FIX_REQ";

		case IPC_GPS_POSITION_RESULT:
		    return "IPC_GPS_POSITION_RESULT";

		case IPC_GPS_EXT_POSITION_RESULT:
		    return "IPC_GPS_EXT_POSITION_RESULT";

		case IPC_GPS_EXT_STATUS_INFO:
		    return "IPC_GPS_EXT_STATUS_INFO";

		case IPC_GPS_PD_CMD_CB:
		    return "IPC_GPS_PD_CMD_CB";

		case IPC_GPS_DLOAD_STATUS:
		    return "IPC_GPS_DLOAD_STATUS";

		case IPC_GPS_END_SESSION:
		    return "IPC_GPS_END_SESSION";

		case IPC_GPS_FAILURE_INFO:
		    return "IPC_GPS_FAILURE_INFO";
                
                case IPC_GPS_DISPLAY_SUPLFLOW:
                    return "IPC_GPS_DISPLAY_SUPLFLOW";
                    break;

                case IPC_GPS_XTRA_SET_TIME_INFO:
                    return "IPC_GPS_XTRA_SET_TIME_INFO";
                    break;

                case IPC_GPS_XTRA_SET_DATA:
                    return "IPC_GPS_XTRA_SET_DATA";
                    break;

                case IPC_GPS_XTRA_CLIENT_INIT_DOWNLOAD:
                    return "IPC_GPS_XTRA_CLIENT_INIT_DOWNLOAD";
                    break;

                case IPC_GPS_XTRA_QUERY_DATA_VALIDITY:
                    return "IPC_GPS_XTRA_QUERY_DATA_VALIDITY";
                    break;

                case IPC_GPS_XTRA_SET_AUTO_DOWNLOAD:
                    return "IPC_GPS_XTRA_SET_AUTO_DOWNLOAD";
                    break;

                case IPC_GPS_XTRA_SET_XTRA_ENABLE:
                    return "IPC_GPS_XTRA_SET_XTRA_ENABLE";
                    break;

                case IPC_GPS_XTRA_DOWNLOAD:
                    return "IPC_GPS_XTRA_DOWNLOAD";
                    break;

                case IPC_GPS_XTRA_VALIDITY_STATUS:
                    return "IPC_GPS_XTRA_VALIDITY_STATUS";
                    break;

                case IPC_GPS_XTRA_TIME_EVENT:
                    return "IPC_GPS_XTRA_TIME_EVENT";
                    break;

                case IPC_GPS_XTRA_DATA_INJECTION_STATUS:
                    return "IPC_GPS_XTRA_DATA_INJECTION_STATUS";
                    break;

                case IPC_GPS_XTRA_USE_SNTP:
                    return "IPC_GPS_XTRA_USE_SNTP";
                    break;
                
                case IPC_GPS_AGPS_PDP_CONNECTION:
                    return "IPC_GPS_AGPS_PDP_CONNECTION";
                    break;
                
                case IPC_GPS_AGPS_DNS_QUERY:
                    return "IPC_GPS_AGPS_DNS_QUERY";
                    break;
                
                case IPC_GPS_AGPS_SSL:
                    return "IPC_GPS_AGPS_SSL";
                    break;
                
                case IPC_GPS_AGPS_MODE:
                    return "IPC_GSP_AGPS_MODE";
                    break;

                default :
                    return "GPS_SUB_CMD_UNDEFINED";
                    break;
            }
            break;


        case IPC_SAP_CMD:
            switch(sub_cmd)
            {
                case IPC_SAP_CONNECT:
                    return "IPC_SAP_CONNECT";
                    break;

                case IPC_SAP_STATUS:
                    return "IPC_SAP_STATUS";
                    break;

                case IPC_SAP_TRANSFER_ATR:
                    return "IPC_SAP_TRANSFER_ATR";
                    break;

                case IPC_SAP_TRANSFER_APDU:
                    return "IPC_SAP_TRANSFER_APDU";
                    break;

                case IPC_SAP_TRANSPORT_PROTOCOL:
                    return "IPC_SAP_TRANSPORT_PROTOCOL";
                    break;

                case IPC_SAP_SIM_POWER:
                    return "IPC_SAP_SIM_POWER";
                    break;

                case IPC_SAP_TRANSFER_CARD_READER_STATUS:
                    return "IPC_SAP_TRANSFER_CARD_READER_STATUS";
                    break;
                
                default :
                    return "GPS_SUB_CMD_UNDEFINED";
                    break;
            }
            break;


        case IPC_FACTORY_CMD:
            switch(sub_cmd)
            {
                case IPC_FACTORY_AUTO_TEST_CDMA:
                    return "IPC_FACTORY_AUTO_TEST_CDMA";
                    break;

                case IPC_FACTORY_AUTO_TEST_GSM:
                    return "IPC_FACTORY_AUTO_TEST_CDMA";
                    break;

                case IPC_FACTORY_PDA_FORCE_SLEEP:
                    return "IPC_FACTORY_PDA_FORCE_SLEEP";
                    break;

                case IPC_FACTORY_BT_TEST:
                    return "IPC_FACTORY_BT_TEST";
                    break;

                case IPC_FACTORY_WIFI_TEST:
                    return "IPC_FACTORY_WIFI_TEST";
                    break;

                case IPC_FACTORY_FM_RADIO_TEST:
                    return "IPC_FACTORY_FM_RADIO_TEST";
                    break;

                case IPC_FACTORY_MEM_FORMAT_TEST:
                    return "IPC_FACTORY_MEM_FORMAT_TEST";
                    break;

                case IPC_FACTORY_LINE_MODE_SMS_COUNT:
                    return "IPC_FACTORY_LINE_MODE_SMS_COUNT";
                    break;

                case IPC_FACTORY_LINE_MODE_SMS_READ:
                    return "IPC_FACTORY_LINE_MODE_SMS_READ";
                    break;

                case IPC_FACTORY_G_SENSOR_TEST:
                    return "IPC_FACTORY_G_SENSE_TEST";
                    break;

                case IPC_FACTORY_MEM_SIZE_CHECK:
                    return "IPC_FACTORY_MEM_SIZE_CHECK";
                    break;

                case IPC_FACTORY_MEM_FILE_NUM_CHECK:
                    return "IPC_FACTORY_MEM_FILE_NUM_CHECK";
                    break;
/*
                case IPC_FACTORY_CONTENT_COPY:
                    return "IPC_FACTORY_CONTENT_COPY";
                    break;

                case IPC_FACTORY_DIAG_KEY_COE:
                    return "IPC_FACTORY_DIAG_KEY_COE";
                    break;
*/
                case IPC_FACTORY_DIAG_TOUCH_TEST:
                    return "IPC_FACTORY_DIAG_TOUCH_TEST";
                    break;
                
                case IPC_FACTORY_SHIPMENT_TEST:
                    return "IPC_FACTORY_SHIPMENT_TEST";
                    break;
                
                case IPC_FACTORY_AP_THERMISTOR:
                    return "IPC_FACTORY_AP_THERMISTOR";
                    break;

                case IPC_FACTORY_AP_BATT_CALIBRATION:
                    return "IPC_FACTORY_AP_BATT_CALIBRATION";
                    break;
/* SMS Input / Read Data   Pineone hgwoo*/
				case IPC_FACTORY_SMS_INPUT:
					return "IPC_FACTORY_SMS_INPUT";
					break;

                case IPC_READ_AUTO_FULL_DATA_CMD:  
                    return "IPC_READ_AUTO_FULL_DATA_CMD";
                    break;

// gideonj_dc02 HQRL_ED11 - STA MODE [[
    //            case IPC_FACTORY_STA_MODE:
     //               return "IPC_FACTORY_STA_MODE";
     //               break;
// gideonj_dc02 HQRL_ED11 - STA MODE]]
					
// [[ SecFeature.SPRINT_DFT : keytrace pineon_dsseo Start 2010_02_19
                case IPC_FACTORY_KEYTRACE:   //etinum.keytrace
                    return "IPC_FACTORY_KEYTRACE";
                    break;
// pineon_dsseo End ]] 

//SecFeature.SPRINT_DFT : PINEONE_ADS29 SD Card Check 
				case IPC_FACTORY_SD_CHECK:
					return "IPC_FACTORY_CHECK_SDCARD_CMD";

// gideonj_dc07 HQRL - [factory process check] ui check [[
				case IPC_FACTORY_PROCESS_TEST:
					return "IPC_FACTORY_PROCESS_TEST";
					break;
// gideonj_dc07 HQRL - [factory process check] ui check ]]

				case IPC_FACTORY_FACTORY_RESET_COMPLETE:
					return "IPC_FACTORY_FACTORY_RESET_COMPLETE";
					
                default :
                    return "GPS_SUB_CMD_UNDEFINED";
                    break;
            }
            break;

       case IPC_OMADM_CMD:
            switch(sub_cmd)
            {
                case IPC_OMADM_PRL_SIZE:
                    return "IPC_OMADM_PRL_SIZE";
                    break;

                case IPC_OMADM_MODEL_NAME:
                    return "IPC_OMADM_MODEL_NAME";
                    break;

                case IPC_OMADM_OEM_NAME:
                    return "IPC_OMADM_OEM_NAME";
                    break;

                case IPC_OMADM_SW_VER:
                    return "IPC_OMADM_SW_VER";
                    break;

                case IPC_OMADM_IS683_DATA:
                    return "IPC_OMADM_IS683_DATA";
                    break;

                case IPC_OMADM_PRL_READ:
                    return "IPC_OMADM_PRL_READ";
                    break;

                case IPC_OMADM_PRL_WRITE:
                    return "IPC_OMADM_PRL_WRITE";
                    break;

                case IPC_OMADM_PUZL_DATA:
                    return "IPC_OMADM_PUZL_DATA";
                    break;

                case IPC_OMADM_ROOTCERT_READ:
                    return "IPC_OMADM_ROOTCERT_READ";
                    break;

                case IPC_OMADM_ROOTCERT_WRITE:
                    return "IPC_OMADM_ROOTCERT_WRITE";
                    break;

                case IPC_OMADM_MMC_OBJECT:
                    return "IPC_OMADM_MMC_OBJECT";
                    break;

                case IPC_OMADM_MIP_NAI_OBJECT:
                    return "IPC_OMADM_MIP_NAI_OBJECT";
                    break;

                case IPC_OMADM_CURRENT_NAI_INDEX:
                    return "IPC_OMADM_CURRENT_NAI_INDEX";
                    break;

                case IPC_OMADM_MIP_AUTH_ALGO:
                    return "IPC_OMADM_MIP_AUTH_ALGO";
                    break;

                case IPC_OMADM_NAM_INFO:
                    return "IPC_OMADM_NAM_INFO";
                    break;

                case IPC_OMADM_START_CIDC:
                    return "IPC_OMADM_START_CIDC";
                    break;

                case IPC_OMADM_START_CIFUMO:
                    return "IPC_OMADM_START_CIFUMO";
                    break;

                case IPC_OMADM_START_CIPRL:
                    return "IPC_OMADM_START_CIPRL";
                    break;

                case IPC_OMADM_START_HFA:
                    return "IPC_OMADM_START_HFA";
                    break;

                case IPC_OMADM_START_REG_HFA:
                    return "IPC_OMADM_START_REG_HFA";
                    break;

                case IPC_OMADM_SETUP_SESSION:
                    return "IPC_OMADM_SETUP_SESSION";
                    break;

                case IPC_OMADM_SERVER_START_SESSION:
                    return "IPC_OMADM_SERVER_START_SESSION";
                    break;

                case IPC_OMADM_CLIENT_START_SESSION:
                    return "IPC_OMADM_CLIENT_START_SESSION";
                    break;

                case IPC_OMADM_SEND_DATA:
                    return "IPC_OMADM_SEND_DATA";
                    break;

                case IPC_OMADM_ENABLE_HFA:
                    return "IPC_OMADM_ENABLE_HFA";
                    break;
					
               default :
                    return "OMADM_SUB_CMD_UNDEFINED";
                    break;
            }
            break;
            
    
        case IPC_GEN_CMD :    
            switch(sub_cmd)
            {
                case IPC_GEN_PHONE_RES :
                    return "IPC_GEN_PHONE_RES";
                    break;
                
                default :
                    return "GEN_SUB_CMD_UNDEFINED";
                    break;
            }
            break;
    
        default :
            return "MAIN_AND_SUB_CMD_UNDEFINED";
            break; 
    
    }
    return 0;
}



#endif
//2 ]]

static u8 _get_msg_id(struct sipc *si)
{
	if (!si)
		return 0;

	si->msg_id = (si->msg_id + 1) & FMT_ID_MASK;

	return si->msg_id;
}

static int _get_auth(void)
{
	int r;
	unsigned long long t, d;

	t = cpu_clock(smp_processor_id());

	r = onedram_get_auth(MB_CMD(MBC_REQ_SEM)); // wait for completion

	d = cpu_clock(smp_processor_id()) - t;
	_pdbg("Get auth %llu ns\n", d);
	if (d > time_max_semlat)
		time_max_semlat = d;

	return r;
}

static void _put_auth(struct sipc *si, u32 mailbox)
{
	if (!si)
		return;

	onedram_put_auth(0);

	if (si->od_rel && !onedram_rel_sem()) {
		if (mailbox == 0)
			onedram_write_mailbox(MB_CMD(MBC_RES_SEM));
		si->od_rel = 0;
	}
}

static inline void _req_rel_auth(struct sipc *si)
{
	si->od_rel = 1;
}

static int _get_auth_try(void)
{
	return onedram_get_auth(0);
}

static void _check_buffer(struct sipc *si)
{
	int i;
	u32 mailbox;

	i = _get_auth_try();
	if (i)
		return;

	mailbox = 0;

	for (i=0;i<IPCIDX_MAX;i++) {
		int inbuf;
		struct ringbuf *rb;

		rb = &si->rb[i];
		inbuf = CIRC_CNT(rb->rb_in_head, rb->rb_in_tail, rb->rb_in_size);
		if (!inbuf)
			continue;

		mailbox |= mb_data[i].mask_send;
	}
	_put_auth(si, 0);

	if (mailbox)
		si->queue(MB_DATA(mailbox), si->queue_data);
}

static void _do_command(struct sipc *si, u32 mailbox)
{
	int r;
	u32 cmd = (mailbox & MBC_MASK) & ~(MB_CMD(0));

//	dev_dbg(&si->svndev->dev, "Command: %x\n", cmd);

	switch(cmd) {
	case MBC_REQ_SEM:
		r = onedram_rel_sem();
		if (r) {
			dev_dbg(&si->svndev->dev, "onedram in use, "
					"defer releasing semaphore\n");
			_req_rel_auth(si);
		}
		else
			onedram_write_mailbox(MB_CMD(MBC_RES_SEM));
		break;
	case MBC_RES_SEM:
		/* do nothing */
		break;
	case MBC_PHONE_START:
		onedram_write_mailbox(MB_CMD(MBC_INIT_END) | CP_BOOT_AIRPLANE
				| AP_OS_ANDROID);
		break;
	case MBC_RESET:
		si->queue(SIPC_RESET_MB, si->queue_data);
		break;
	case MBC_ERR_DISPLAY:
		si->queue(SIPC_EXIT_MB, si->queue_data);
		break;
	/* TODO : impletment other commands... */
	default:
		/* do nothing */

		break;
	}
}

void sipc_handler(u32 mailbox, void *data)
{
	struct sipc *si = (struct sipc *)data;

	if (!si || !si->queue)
		return;

	dev_dbg(&si->svndev->dev, "recv mailbox %x\n", mailbox);

	if ((mailbox & MB_VALID) == 0) {
		dev_err(&si->svndev->dev, "Invalid mailbox message: %x\n", mailbox);
		return;
	}

	if (mailbox & MB_COMMAND) {
		_check_buffer(si);  // check buffer for missing interrupt
		_do_command(si, mailbox);
		return;
	}

	si->queue(mailbox, si->queue_data);
}

static inline void _init_data(struct sipc *si, unsigned char *base)
{
	int i;

	si->map = (struct sipc_mapped *)base;
	si->map->magic = 0x0;
	si->map->access = 0x0;

	for (i=0;i<IPCIDX_MAX;i++) {
		struct ringbuf *r = &si->rb[i];
		struct ringbuf_info *info = &rb_info[i];

		r->out = (struct ringbuf_mapped *)(base + info->out_off);
		r->in = (struct ringbuf_mapped *)(base + info->in_off);
		r->out_base = r->out + 1;
		r->in_base = r->in + 1;
		r->info = info;

		r->rb_out_head = 0;
		r->rb_out_tail = 0;
		r->rb_in_head = 0;
		r->rb_in_tail = 0;
	}
}

static void _init_proc(struct sipc *si)
{
	u32 mailbox;
	int r;

	r = onedram_read_mailbox(&mailbox);
	if (r)
		return;

	sipc_handler(mailbox, si);
}

struct sipc* sipc_open(void (*queue)(u32, void*), struct net_device *ndev)
{
	struct sipc *si;
	struct resource *res;
	int r;

    dbg_loga("Invoked!!!\n");

	if (!queue || !ndev)
		return ERR_PTR(-EINVAL);

	si = kzalloc(sizeof(struct sipc), GFP_KERNEL);
	if (!si)
		return ERR_PTR(-ENOMEM);

	/* If FMT_OUT_SZ grown up, MUST be changed!! */
	si->frag_buf = kmalloc(FMT_OUT_SZ, GFP_KERNEL);
	if (!si->frag_buf) {
		sipc_close(&si);
        dbg_loge("kmalloc() ... Fail!!!\n");
		return ERR_PTR(-ENOMEM);
	}

	INIT_LIST_HEAD(&si->frag_map.head);

	res = onedram_request_region(0, SIPC_MAP_SIZE, SIPC_NAME);
	if (!res) {
		sipc_close(&si);
        dbg_loge("onedram_request_region() ... Fail!!!\n");
		return ERR_PTR(-EBUSY);
	}
	si->res = res;
	si->queue = queue;
	si->queue_data = ndev;
	si->svndev = ndev;

	/* TODO: need?? */
	if (work_pending(&pdp_work))
		flush_work(&pdp_work);

	r = sysfs_create_group(&si->svndev->dev.kobj, &pdp_group);
	if (r) {
		sipc_close(&si);
        dbg_loge("sysfs_create_group() ... Fail!!!\n");
		return ERR_PTR(r);
	}
	si->group = &pdp_group;

#if defined(NOISY_DEBUG) || defined(PERF_DEBUG)
	_dev = &si->svndev->dev;
#endif

	_init_data(si, (unsigned char *)res->start);
	skb_queue_head_init(&si->rfs_rx);

	/* process init message */
	_init_proc(si);

	r = onedram_register_handler(sipc_handler, si);
	if (r) {
		sipc_close(&si);
		return ERR_PTR(r);
	}

    dbg_loga("Complete!!!\n");

	return si;
}

static void clear_pdp_wq(struct work_struct *work)
{
	int i;

	mutex_lock(&pdp_mutex);

	for (i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
		if (pdp_devs[i]) {
			destroy_pdp(&pdp_devs[i]);
			clear_bit(i, pdp_bitmap);
		}
	}
	pdp_cnt = 0;

	mutex_unlock(&pdp_mutex);
}

void sipc_exit(void)
{
	if (work_pending(&pdp_work))
		flush_work(&pdp_work);
	else
		clear_pdp_wq(NULL);
}

void sipc_close(struct sipc **psi)
{
	struct sipc *si;

	if (!psi || !*psi)
		return;

	si = *psi;

	if (si->group && si->svndev) {
		int i;
		sysfs_remove_group(&si->svndev->dev.kobj, si->group);

		mutex_lock(&pdp_mutex);
		for (i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
			if (pdp_devs[i])
				netif_stop_queue(pdp_devs[i]);
		}
		mutex_unlock(&pdp_mutex);
		schedule_work(&pdp_work);
	}

	if (si->frag_buf)
		kfree(si->frag_buf);

	if (si->queue)
		onedram_unregister_handler(sipc_handler);

	if (si->res)
		onedram_release_region(0, SIPC_MAP_SIZE);

	kfree(si);
	*psi = NULL;
}

static inline void _wake_queue(int idx)
{
	mutex_lock(&pdp_mutex);

	if (pdp_devs[idx] && !test_bit(idx, pdp_bitmap))
		netif_wake_queue(pdp_devs[idx]);

	mutex_unlock(&pdp_mutex);
}

static int __write(struct ringbuf *rb, u8 *buf, unsigned int size)
{
	int c;
	int len = 0;

	// no check space

	_dbg("%s b: size %u head %u tail %u\n", __func__, size, rb->rb_out_head, rb->rb_out_tail);
	_dbg_dump(buf, size);

	while(1) {
		c = CIRC_SPACE_TO_END(rb->rb_out_head, rb->rb_out_tail, rb->rb_out_size);
		if(size < c)
			c = size;
		if(c <= 0)
			break;
		memcpy(rb->out_base + rb->rb_out_head, buf, c);
#if defined(SVNET_CIRC)
		rb->rb_out_head = rb->rb_out_head + c;
		if (rb->rb_out_head >= rb->rb_out_size)
			rb->rb_out_head -= rb->rb_out_size;
#else
		rb->rb_out_head = (rb->rb_out_head + c) & (rb->rb_out_size - 1);
#endif
		buf += c;
		size -= c;
		len += c;
	}

	_dbg("%s a: size %u head %u tail %u\n", __func__, len, rb->rb_out_head, rb->rb_out_tail);

	return len;
}

static inline void _set_raw_hdr(struct raw_hdr *h, int res,
		unsigned int len, int control)
{
	h->len = len;
	h->channel = CHID(res);
	h->control = 0;
}

static int _write_raw_buf(struct ringbuf *rb, int res, struct sk_buff *skb)
{
	int len;
	struct raw_hdr h;

	_dbg("%s: packet %p res 0x%02x\n", __func__, skb, res);

	len = skb->len + sizeof(h);

	_set_raw_hdr(&h, res, len, 0);

	len  = __write(rb, (u8 *)hdlc_start, sizeof(hdlc_start));
	len += __write(rb, (u8 *)&h, sizeof(h));
	len += __write(rb, skb->data, skb->len);
	len += __write(rb, (u8 *)hdlc_end, sizeof(hdlc_end));

	return len;
}

#ifdef LOOP_BACK_TEST
int _write_raw_buffer(struct sipc * si,/*struct ringbuf *rb,*/ int res, u8 *buff,int length)
{
	int len =0, space =0;
	struct raw_hdr h;
	struct ringbuf *rb = &si->rb[1];	
	onedram_get_auth(MB_CMD(MBC_REQ_SEM));
	space = CIRC_SPACE((rb->rb_out_head), (rb->rb_out_tail), (rb->rb_out_size));
	
	if(space < length + sizeof(struct raw_hdr)
			+ sizeof(hdlc_start) + sizeof(hdlc_end))
		{
	//	_put_auth(si, mailbox);
		return -ENOSPC;
		}

	len = length + sizeof(h);
	_set_raw_hdr(&h, res, len, 0);
	
	len  = __write(rb, (u8 *)hdlc_start, sizeof(hdlc_start));
	len += __write(rb, (u8 *)&h, sizeof(h));
	len += __write(rb, buff, length);
	len += __write(rb, (u8 *)hdlc_end, sizeof(hdlc_end));

	onedram_put_auth(MB_CMD(MBC_RES_SEM));
	onedram_write_mailbox(MB_CMD(MBD_SEND_RAW));	

	return len;
}
EXPORT_SYMBOL(_write_raw_buffer);
#endif

static int _write_raw_skb(struct ringbuf *rb, int res, struct sk_buff *skb)
{
	char         *b;
	unsigned int  len;

	_dbg("%s: packet %p res 0x%02x\n", __func__, skb, res);

	b = skb_put(skb, sizeof(hdlc_end));
	memcpy(b, hdlc_end, sizeof(hdlc_end));

	b = skb_push(skb, sizeof(struct raw_hdr) + sizeof(hdlc_start));
	memcpy(b, hdlc_start, sizeof(hdlc_start));

	b += sizeof(hdlc_start);

    len = skb->len - sizeof(hdlc_start) - sizeof(hdlc_end);
	_set_raw_hdr((struct raw_hdr *)b, res, len, 0);

	return __write(rb, skb->data, skb->len);
}

static int _write_raw(struct ringbuf *rb, struct sk_buff *skb, int res)
{
	int len;
	int space;

	space = CIRC_SPACE(rb->rb_out_head, rb->rb_out_tail, rb->rb_out_size);
	if(space < skb->len + sizeof(struct raw_hdr)
			+ sizeof(hdlc_start) + sizeof(hdlc_end))
		return -ENOSPC;

	if(skb_headroom(skb) > (sizeof(struct raw_hdr) + sizeof(hdlc_start))
			&& skb_tailroom(skb) > sizeof(hdlc_end)) {
		len = _write_raw_skb(rb, res, skb);
	} else {
		len = _write_raw_buf(rb, res, skb);
	}

	if (res >= PN_PDP_START && res <= PN_PDP_END)
		_wake_queue(PDP_ID(res));
	else
		netif_wake_queue(skb->dev);

	return len;
}

static int _write_rfs_buf(struct ringbuf *rb, struct sk_buff *skb)
{
	int len;

	_dbg("%s: packet %p\n", __func__, skb);
	len  = __write(rb, (u8 *)hdlc_start, sizeof(hdlc_start));
	len += __write(rb, skb->data, skb->len);
	len += __write(rb, (u8 *)hdlc_end, sizeof(hdlc_end));

	return len;
}

static int _write_rfs_skb(struct ringbuf *rb, struct sk_buff *skb)
{
	char *b;

	_dbg("%s: packet %p\n", __func__, skb);
	b = skb_put(skb, sizeof(hdlc_end));
	memcpy(b, hdlc_end, sizeof(hdlc_end));

	b = skb_push(skb, sizeof(hdlc_start));
	memcpy(b, hdlc_start, sizeof(hdlc_start));

#if 0
    print_tx_hdlc_frame(IPCIDX_RFS, b, skb->len);
#endif

	return __write(rb, skb->data, skb->len);
}

static int _write_rfs(struct ringbuf *rb, struct sk_buff *skb)
{
	int len;
	int space;

	space = CIRC_SPACE(rb->rb_out_head, rb->rb_out_tail, rb->rb_out_size);
	if(space < skb->len + sizeof(hdlc_start) + sizeof(hdlc_end))
		return -ENOSPC;

	if(skb_headroom(skb) > sizeof(hdlc_start)
			&& skb_tailroom(skb) > sizeof(hdlc_end)) {
		len = _write_rfs_skb(rb, skb);
	} else {
		len = _write_rfs_buf(rb, skb);
	}

	netif_wake_queue(skb->dev);
	return len;
}

static int _write_fmt_buf(char *frag_buf, struct ringbuf *rb,
		struct sk_buff *skb, struct frag_info *fi, int wlen,
		u8 control)
{
	char *buf = frag_buf;
	struct fmt_hdr *h;

	memcpy(buf, hdlc_start, sizeof(hdlc_start));
	buf += sizeof(hdlc_start);

	h = (struct fmt_hdr *)buf;
	h->len = sizeof(struct fmt_hdr) + wlen;
	h->control = control;
	buf += sizeof(struct fmt_hdr);

	memcpy(buf, skb->data + fi->offset, wlen);
	buf += wlen;

	memcpy(buf, hdlc_end, sizeof(hdlc_end));
	buf += sizeof(hdlc_end);

    print_tx_hdlc_frame(IPCIDX_FMT, frag_buf, (wlen + 5));

	return __write(rb, frag_buf, buf - frag_buf);
}

static int _write_fmt(struct sipc *si, struct ringbuf *rb, struct sk_buff *skb)
{
	int len;
	int space;
	int remain;
	struct frag_info *fi = &si->frag;

	if (skb != fi->skb) {
		/* new packet */
		fi->skb = skb;
//		fi->msg_id = _get_msg_id(si);
		fi->offset = 0;
	}

	len = 0;
	remain = skb->len - fi->offset;

	_dbg("%s: packet %p length %d sent %d\n", __func__, skb, skb->len, fi->offset);

	while (remain > 0) {
		int wlen;
		u8 control;

		space = CIRC_SPACE(rb->rb_out_head, rb->rb_out_tail, rb->rb_out_size);
		space -= sizeof(struct fmt_hdr)
			+ sizeof(hdlc_start) + sizeof(hdlc_end);
		if (space < FMT_TX_MIN)
			return -ENOSPC;

		if (remain > space) {
			/* multiple frame */
			wlen = space;
			control = 0x1 | FMT_MB_MASK;
		} else {
			wlen = remain;
			if (fi->offset == 0) {
				/* single frame */
				control = 0x0;
			} else {
				/* last frmae */
				control = 0x1;
			}
		}

		wlen = _write_fmt_buf(si->frag_buf, rb, skb, fi, wlen, control);
		if (wlen < 0)
			return wlen;

		len += wlen;

		wlen -= sizeof(hdlc_start) + sizeof(struct fmt_hdr)
			+ sizeof(hdlc_end);

		fi->offset += wlen;
		remain -= wlen;
	}

	if (len > 0) {
		fi->skb = NULL;
		fi->offset = 0;
	}

	netif_wake_queue(skb->dev);
	return len; /* total write bytes */
}

static int _write(struct sipc *si, int res, struct sk_buff *skb, u32 *mailbox)
{
	int r;
	int rid;

	rid = res_to_ridx(res);
	if(rid < 0 || rid >= IPCIDX_MAX)
		return -EINVAL;

	switch (rid)
	{
    	case IPCIDX_FMT:
    		r = _write_fmt(si, &si->rb[rid], skb);
    		break;
    	case IPCIDX_RAW:
    		r = _write_raw(&si->rb[rid], skb, res);
    		break;
    	case IPCIDX_RFS:
    		r = _write_rfs(&si->rb[rid], skb);
    		break;
    	default:
    		/* do nothing */
    		r = 0;
    		break;
	}

	if (r > 0)
		*mailbox |= mb_data[rid].mask_send;
	else if (r == -ENOSPC)
		*mailbox = mb_data[rid].mask_req_ack;

	_dbg("%s: return %d\n", __func__, r);
	return r;
}

static inline void _update_stat(struct net_device *ndev, unsigned int len)
{
	if(!ndev)
		return;

	ndev->stats.tx_bytes += len;
	ndev->stats.tx_packets++;
}

static inline int _write_pn(struct sipc *si, struct sk_buff *skb, u32 *mb)
{
	int r;
	struct phonethdr *ph;

	ph = pn_hdr(skb);
	skb_pull(skb, sizeof(struct phonethdr) + 1); // 1 is addr len

	r = _write(si, ph->pn_res, skb, mb);
	if (r < 0)
		skb_push(skb, sizeof(struct phonethdr) + 1);

	return r;
}

int sipc_write(struct sipc *si, struct sk_buff_head *sbh)
{
	int r;
	u32 mailbox;
	struct sk_buff *skb;

	if (!sbh)
		return -EINVAL;

	if (!si) {
		skb_queue_purge(sbh);
		return -ENXIO;
	}

	r = _get_auth();
	if (r)
		return r;

	r = mailbox = 0;
	skb = skb_dequeue(sbh);
	while (skb)
	{
		struct net_device *ndev = skb->dev;
		int len = skb->len;

		dev_dbg(&si->svndev->dev, "write packet %p\n", skb);

		if (skb->protocol != __constant_htons(ETH_P_PHONET)) {
			struct pdp_priv *priv;
			priv = netdev_priv(ndev);
			r = _write(si, PN_PDP(priv->channel), skb, &mailbox);
		} else
			r = _write_pn(si, skb, &mailbox);

		if (r < 0)
			break;

		_update_stat(ndev, len);
		dev_kfree_skb_any(skb);

		skb = skb_dequeue(sbh);
	}

//	_req_rel_auth(si);
	_put_auth(si, mailbox);

	if (mailbox)
		onedram_write_mailbox(MB_DATA(mailbox));

	if (r < 0) {
		if (r == -ENOSPC) {
			dev_err(&si->svndev->dev,
					"write nospc queue %p\n", skb);
			skb_queue_head(sbh, skb);
			netif_stop_queue(skb->dev);
		} else {
			dev_err(&si->svndev->dev,
					"write err %d, drop %p\n", r, skb);
			dev_kfree_skb_any(skb);
		}
	}

	return r;
}

extern int __read(struct ringbuf *rb, unsigned char *buf, unsigned int size)
{
	int c;
	int len = 0;
	unsigned char *p = buf;

	_dbg("%s b: size %u head %u tail %u\n", __func__, size, rb->rb_in_head, rb->rb_in_tail);

	if(rb->rb_in_head < 0 || rb->rb_in_head >= rb->rb_in_size) {
		printk(KERN_ERR "head is wrong!! %d\n", rb->rb_in_head);
		return len;
	}

	while(1)
	{
		c = CIRC_CNT_TO_END(rb->rb_in_head, rb->rb_in_tail, rb->rb_in_size);
		if(size < c)
			c = size;
		if(c <= 0)
			break;
		if (p) {
			memcpy(p, rb->in_base + rb->rb_in_tail, c);
			p += c;
		}

#if defined(SVNET_CIRC)
		rb->rb_in_tail = rb->rb_in_tail + c;
		if (rb->rb_in_tail >= rb->rb_in_size)
			rb->rb_in_tail -= rb->rb_in_size;
#else
		rb->rb_in_tail = (rb->rb_in_tail + c) & (rb->rb_in_size - 1);
#endif

		size -= c;
		len += c;
	}

	_dbg("%s a: size %u head %u tail %u\n", __func__, len, rb->rb_in_head, rb->rb_in_tail);
	_dbg_dump(buf, len);

	return len;
}

static inline void _get_raw_hdr(struct raw_hdr *h, int *res,
		unsigned int *len, int *control)
{
	if(res)
		*res = PN_RAW(h->channel);
	if(len)
		*len = h->len;
	if(control)
		*control = h->control;
}

static inline void _phonet_rx(struct net_device *ndev,
		struct sk_buff *skb, int res)
{
	int r;
	struct phonethdr *ph;

	skb->protocol = __constant_htons(ETH_P_PHONET);

	ph = (struct phonethdr *)skb_push(skb, sizeof(struct phonethdr));
	ph->pn_rdev = ndev->dev_addr[0];
	ph->pn_sdev = 0;
	ph->pn_res = res;
	ph->pn_length = __cpu_to_be16(skb->len + 2 - sizeof(*ph));
	ph->pn_robj = 0;
	ph->pn_sobj = 0;

	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += skb->len;

	skb_reset_mac_header(skb);

	r = netif_rx_ni(skb);
	if (r != NET_RX_SUCCESS)
		dev_err(&ndev->dev, "phonet rx error: %d\n", r);

	_dbg("%s: res 0x%02x packet %p len %d\n", __func__, res, skb, skb->len);
}

static int _read_pn(struct net_device *ndev, struct ringbuf *rb, int len, int res)
{
	struct sk_buff *skb;
	char           *p;
	int             r;
	int             read_len = len + sizeof(hdlc_end);

	_dbg("%s: res 0x%02x data %d\n", __func__, res, len);

	skb = netdev_alloc_skb(ndev, read_len + sizeof(struct phonethdr));
	if (unlikely(!skb))
		return -ENOMEM;

	skb_reserve(skb, sizeof(struct phonethdr));

	p = skb_put(skb, len);
	r = __read(rb, p, read_len);
	if (r != read_len) {
		kfree_skb(skb);
		return -EBADMSG;
	}

	_phonet_rx(ndev, skb, res);

	return r;
}

static inline struct sk_buff* _alloc_phskb(struct net_device *ndev, int len)
{
	struct sk_buff *skb;

	skb = netdev_alloc_skb(ndev, len + sizeof(struct phonethdr));
	if (likely(skb))
		skb_reserve(skb, sizeof(struct phonethdr));

	return skb;
}

static inline int _alloc_rfs(struct net_device *ndev,
		struct sk_buff_head *list, int len)
{
	int r = 0;
	struct sk_buff *skb;

	__skb_queue_head_init(list);

	while (len > 0) {
		skb = _alloc_phskb(ndev, RFS_MTU);
		if (unlikely(!skb)) {
			r = -ENOMEM;
			break;
		}
		__skb_queue_tail(list, skb);
		len -= RFS_MTU;
	}

	return r;
}
static void _free_rfs(struct sk_buff_head *list)
{
	struct sk_buff *skb;

	skb = __skb_dequeue(list);
	while (skb) {
		__kfree_skb(skb);
		skb = __skb_dequeue(list);
	}
}

static inline int _read_rfs_rb(struct ringbuf *rb, int len,
		struct sk_buff_head *list)
{
	int r;
	int read_len;
	struct sk_buff *skb;
	char *p;

	read_len = 0;
	skb = list->next;
	while (skb != (struct sk_buff *)list) {
		int rd = RFS_MTU;

		if (skb == list->next) /* first sk has header */
			rd -= sizeof(struct rfs_hdr);

		if (len < rd)
			rd = len;

		p = skb_put(skb, rd);
		r = __read(rb, p, rd);
		if (r != rd)
			return -EBADMSG;

		len -= r;
		read_len += r;
		skb = skb->next;
	}

	return read_len;
}

static int _read_rfs_data(struct sipc *si, struct ringbuf *rb, int len,
		struct rfs_hdr *h)
{
	int r;
	struct sk_buff_head list;
	struct sk_buff *skb;
	char *p;
	int read_len;
	struct net_device *ndev = si->svndev;

	_dbg("%s: %d bytes\n", __func__, len);

	/* alloc sk_buffs */
	r = _alloc_rfs(ndev, &list, len + sizeof(struct rfs_hdr));
	if (r)
		goto free_skb;

	skb = list.next;
	p = skb_put(skb, sizeof(struct rfs_hdr));
	memcpy(p, h, sizeof(struct rfs_hdr));

	/* read data all */
	r = _read_rfs_rb(rb, len, &list);
	if (r < 0)
		goto free_skb;

	read_len = r;

	/* move to rfs_rx queue */
	skb = __skb_dequeue(&list);
	while (skb) {
//		_phonet_rx(ndev, skb, PN_RFS);
		skb_queue_tail(&si->rfs_rx, skb);
		skb = __skb_dequeue(&list);
	}

	/* remove hdlc_end */
	read_len += __read(rb, NULL, sizeof(hdlc_end));

	return read_len;

free_skb:
	_free_rfs(&list);
	return r;
}

static int _read_pdp(struct ringbuf *rb, int len,
		int res)
{
	int r;
	struct sk_buff *skb;
	char *p;
	int read_len = len + sizeof(hdlc_end);
	struct net_device *ndev;
	unsigned char first_byte;

	_dbg("%s: res 0x%02x data %d\n", __func__, res, len);

	mutex_lock(&pdp_mutex);

	ndev = pdp_devs[PDP_ID(res)];
	if (!ndev) {
		// drop data
		r = __read(rb, NULL, read_len);
		mutex_unlock(&pdp_mutex);
		return r;
	}

	/* HBG Rollback the Ethernet packet changes
	 for Forte Not required */
#ifdef SVNET_PDP_ETHER
	skb = netdev_alloc_skb(ndev, read_len + sizeof(struct ethhdr));
#else
    skb = netdev_alloc_skb(ndev, read_len);
#endif
	if (unlikely(!skb)) {
		mutex_unlock(&pdp_mutex);
		return -ENOMEM;
	}

	//skb_reserve(skb, sizeof(struct ethhdr));

	/* HBG Rollback the Ethernet packet changes
	for Forte Not required */
#ifdef SVNET_PDP_ETHER
	p = skb_put(skb, len + sizeof(struct ethhdr));
	eHdr = (struct ethhdr *)p;
	memcpy(eHdr->h_dest, dest, ETH_ALEN);
	memcpy(eHdr->h_source, source, ETH_ALEN);
	eHdr->h_proto = (first_byte == 0x06)? __constant_htons(ETH_P_IPV6): __constant_htons(ETH_P_IP);

	p = p + sizeof(struct ethhdr);
#else
    p = skb_put(skb, len); 
#endif

	r = __read(rb, p, read_len);
	if (r != read_len) {
		mutex_unlock(&pdp_mutex);
		kfree_skb(skb);
		return -EBADMSG;
	}
	ndev->stats.rx_packets++;
	ndev->stats.rx_bytes += skb->len;

	mutex_unlock(&pdp_mutex);

	read_len = r;

	/* yoonsung.nam support Ipv6 
	 * TODO: this should be re-implemented */
	
	/* HBG Rollback the Ethernet packet changes
	for Forte Not required */
#ifdef SVNET_PDP_ETHER
	first_byte = skb->data[14];
#else
    first_byte = skb->data[0];
#endif
	first_byte >>= 4;
	if(first_byte == 0x06)
		skb->protocol = __constant_htons(ETH_P_IPV6);
	else
		skb->protocol = __constant_htons(ETH_P_IP);
	skb_reset_mac_header(skb);

#ifdef SVNET_PDP_ETHER
	skb->ip_summed = CHECKSUM_UNNECESSARY;
#endif
	_dbg("%s: pdp packet %p len %d\n", __func__, skb, skb->len);

	/* HBG Rollback the Ethernet packet changes
	for Forte Not required */
#ifdef SVNET_PDP_ETHER
	skb_pull(skb, sizeof(struct ethhdr));
#endif	
	r = netif_rx_ni(skb);
	if (r != NET_RX_SUCCESS)
		dev_err(&ndev->dev, "pdp rx error: %d\n", r);

	return read_len;
}

static int _read_raw(struct sipc *si, int inbuf, struct ringbuf *rb)
{
	int r;
	char buf[sizeof(struct raw_hdr) + sizeof(hdlc_start)];
	int res, data_len;
	u32 tail;

	while (inbuf > 0) {
		tail = rb->rb_in_tail;

		r = __read(rb, buf, sizeof(buf));
		if (r < sizeof(buf) ||
				strncmp(buf, hdlc_start, sizeof(hdlc_start))) {
			dev_err(&si->svndev->dev, "Bad message: "
				"read %d byte [%x %x %x %x %x %x %x]\n", r
				, (buf[0] ? buf[0] : 0)
				, (buf[1] ? buf[1] : 0)
				, (buf[2] ? buf[2] : 0)
				, (buf[3] ? buf[3] : 0)
				, (buf[4] ? buf[4] : 0)
				, (buf[5] ? buf[5] : 0)
				, (buf[6] ? buf[6] : 0)	);
			dev_err(&si->svndev->dev, "Head %d Tail %d\n",
				rb->rb_in_head, rb->rb_in_tail);
			return -EBADMSG;
		}
		inbuf -= r;

		_get_raw_hdr((struct raw_hdr *)&buf[sizeof(hdlc_start)],
				&res, &data_len, NULL);

		data_len -= sizeof(struct raw_hdr);
#ifdef LOOP_BACK_TEST
		res = CHID(res);
		if (res == 31)
		{
			r = __read(rb, loopback_data_priv,data_len );
			
			if (r != data_len)
			{
				return -EBADMSG;
			}
			r = __read(rb,NULL,sizeof(hdlc_end));
			
			if(loopback_ongoing)
			{
    			if(mb_data[1].mask_req_ack)
    			{
    				_put_auth(si,MB_DATA(mb_data[1].mask_res_ack));
    				onedram_write_mailbox(MB_DATA(mb_data[1].mask_res_ack));
			    }
			    _write_raw_buffer(si,res, loopback_data_priv,data_len);

    			loopback_res.nTransfered++;
    			r = _get_auth();
			}
		}
		else 
        {
		    if (res >= PN_PDP_START && res <= PN_PDP_END)
			    r = _read_pdp(rb, data_len, res);
    		else
    			r = _read_pn(si->svndev, rb, data_len, res);
		}
#else
	    if (res >= PN_PDP_START && res <= PN_PDP_END)
		    r = _read_pdp(rb, data_len, res);
		else
			r = _read_pn(si->svndev, rb, data_len, res);
#endif
		
		if (r < 0) {
			if (r == -ENOMEM)
				rb->rb_in_tail = tail;

			return r;
		}

		inbuf -= r;
	}

	return 0;
}

static int _read_rfs(struct sipc *si, int inbuf, struct ringbuf *rb)
{
	int r;
	char buf[sizeof(struct rfs_hdr) + sizeof(hdlc_start)];
	int data_len;
	u32 tail;
	struct rfs_hdr *h;

	h = (struct rfs_hdr *)&buf[sizeof(hdlc_start)];
	while (inbuf > 0) {
		tail = rb->rb_in_tail;

		r = __read(rb, buf, sizeof(buf));
		if (r < sizeof(buf) ||
				strncmp(buf, hdlc_start, sizeof(hdlc_start))) {
			dev_err(&si->svndev->dev, "Bad message: "
				"read %d byte [%x %x %x %x %x %x %x]\n", r
				, (buf[0] ? buf[0] : 0)
				, (buf[1] ? buf[1] : 0)
				, (buf[2] ? buf[2] : 0)
				, (buf[3] ? buf[3] : 0)
				, (buf[4] ? buf[4] : 0)
				, (buf[5] ? buf[5] : 0)
				, (buf[6] ? buf[6] : 0)	);
			dev_err(&si->svndev->dev, "Head %d Tail %d\n",
				rb->rb_in_head, rb->rb_in_tail);
			return -EBADMSG;
		}
		inbuf -= r;

/* clarify the length field according to the RFS spec */
		data_len = h->len - sizeof(struct rfs_hdr);
//		data_len = h->len;

		r = _read_rfs_data(si, rb, data_len, h);
		if (r < 0) {
			if (r == -ENOMEM)
				rb->rb_in_tail = tail;

			return r;
		}

		inbuf -= r;
	}

	return 0;
}


static struct frag_list* _find_frag_list(u8 control, struct frag_head *fh)
{
	struct frag_list *fl;
	u8 msg_id = control & FMT_ID_MASK;

	if (!test_bit(msg_id, fh->bitmap))
		return NULL;

	list_for_each_entry(fl, &fh->head, list) {
		if (fl->msg_id == msg_id)
			break;
	}

	return fl;
}

static int _fill_skb(struct sk_buff *skb, struct frag_list *fl)
{
	struct frag_block *fb, *n;
	int offset = 0;
	char *p;

	list_for_each_entry_safe(fb, n, &fl->block_head, list) {
		p = skb_put(skb, fb->len);
		memcpy(p, fb->buf, fb->len);
		offset += fb->len;
		list_del(&fb->list);
		kfree(fb);
	}

	return offset;
}

static void _destroy_frag_list(struct frag_list *fl, struct frag_head *fh)
{
	struct frag_block *fb, *n;

	if (!fl || !fh)
		return;

	list_for_each_entry_safe(fb, n, &fl->block_head, list) {
		kfree(fb);
	}

	clear_bit(fl->msg_id, fh->bitmap);
	list_del(&fl->list);
	kfree(fl);
}

static struct frag_list* _create_frag_list(u8 control, struct frag_head *fh)
{
	struct frag_list *fl;
	u8 msg_id = control & FMT_ID_MASK;

	if (test_bit(msg_id, fh->bitmap)) {
		fl = _find_frag_list(control, fh);
		_destroy_frag_list(fl, fh);
	}

	fl = kmalloc(sizeof(struct frag_list), GFP_KERNEL);
	if (!fl)
		return NULL;

	INIT_LIST_HEAD(&fl->block_head);
	fl->msg_id = msg_id;
	fl->len = 0;
	list_add(&fl->list, &fh->head);
	set_bit(msg_id, fh->bitmap);

	return fl;
}

static inline struct frag_block* _create_frag_block(struct frag_list *fl)
{
	struct frag_block *fb;

	fb = kmalloc(sizeof(struct frag_block), GFP_KERNEL);
	if (!fb)
		return NULL;

	fb->len = 0;
	fb->ptr = fb->buf;
	list_add_tail(&fb->list, &fl->block_head);

	return fb;
}

static struct frag_block* _prepare_frag_block(struct frag_list *fl, int size)
{
	struct frag_block *fb;

	if (size > FRAG_BLOCK_MAX)
		BUG();

	if (list_empty(&fl->block_head)) {
		fb = _create_frag_block(fl);
	} else {
		fb = list_entry(fl->block_head.prev, struct frag_block, list);
		if (size > FRAG_BLOCK_MAX - fb->len)
			fb = _create_frag_block(fl);
	}

	return fb;
}

static int _read_fmt_frag(struct frag_head *fh, struct fmt_hdr *h,
		struct ringbuf *rb)
{
	int r;
	int data_len;
	int read_len;
	struct frag_list *fl;
	struct frag_block *fb;

	data_len = h->len - sizeof(struct fmt_hdr);
	read_len = data_len + sizeof(hdlc_end);

	_dbg("%s: data %d\n", __func__, data_len);

	fl = _find_frag_list(h->control, fh);
	if (!fl)
		fl = _create_frag_list(h->control, fh);

	if (!fl)
		return -ENOMEM;

	fb = _prepare_frag_block(fl, read_len);
	if (!fb) {
		if (fl->len == 0)
			_destroy_frag_list(fl, fh);

		return -ENOMEM;
	}

	r = __read(rb, fb->ptr, read_len);
	if (r != read_len) {
		_destroy_frag_list(fl, fh);
		return -EBADMSG;
	}

	fb->ptr += data_len;
	fb->len += data_len;
	fl->len += data_len;

	_dbg("%s: fl %p len %d fb %p ptr %p len %d\n", __func__,
			fl, fl->len, fb, fb->ptr, fb->len);

	return r;
}

static int _read_fmt_last(struct frag_head *fh, struct fmt_hdr *h,
		struct ringbuf *rb, struct net_device *ndev)
{
	int r;
	int data_len;
	int read_len;
	int total_len;
	struct sk_buff *skb;
	struct frag_list *fl;
	char *p;

	total_len = data_len = h->len - sizeof(struct fmt_hdr);
	read_len = data_len + sizeof(hdlc_end);

	fl = _find_frag_list(h->control & FMT_ID_MASK, fh);
	if (fl)
		total_len += fl->len;

	_dbg("%s: total %d data %d\n", __func__, total_len, data_len);

	skb = netdev_alloc_skb(ndev, total_len
			+ sizeof(struct phonethdr) + sizeof(hdlc_end));
	if (unlikely(!skb))
		return -ENOMEM;

	skb_reserve(skb, sizeof(struct phonethdr));

	if (fl)
		_fill_skb(skb, fl);

	_destroy_frag_list(fl, fh);

	p = skb_put(skb, data_len);
	r = __read(rb, p, read_len);
	if (r != read_len) {
		kfree_skb(skb);
		return -EBADMSG;
	}

    print_rx_fmt_frame(h, p, (read_len - 1));

	_phonet_rx(ndev, skb, PN_FMT);

	return r;
}

static int _read_fmt(struct sipc *si, int inbuf, struct ringbuf *rb)
{
	int r;
	char buf[sizeof(struct fmt_hdr) + sizeof(hdlc_start)];
	struct fmt_hdr *h;
	u32 tail;
	struct net_device *ndev = si->svndev;

	h = (struct fmt_hdr *)&buf[sizeof(hdlc_start)];
	while (inbuf > 0) {
		tail = rb->rb_in_tail;

		r = __read(rb, buf, sizeof(buf));
		if (r < sizeof(buf) ||
				strncmp(buf, hdlc_start, sizeof(hdlc_start))) {
			dev_err(&si->svndev->dev, "Bad message: "
				"read %d byte [%x %x %x %x %x %x %x]\n", r
				, (buf[0] ? buf[0] : 0)
				, (buf[1] ? buf[1] : 0)
				, (buf[2] ? buf[2] : 0)
				, (buf[3] ? buf[3] : 0)
				, (buf[4] ? buf[4] : 0)
				, (buf[5] ? buf[5] : 0)
				, (buf[6] ? buf[6] : 0)	);
			dev_err(&si->svndev->dev, "Head %d Tail %d\n",
				rb->rb_in_head, rb->rb_in_tail);
			return -EBADMSG;
		}
		inbuf -= r;

		if (is_fmt_last(h->control))
			r = _read_fmt_last(&si->frag_map, h, rb, ndev);
		else
			r = _read_fmt_frag(&si->frag_map, h, rb);

		if (r < 0) {
			if (r == -ENOMEM)
				rb->rb_in_tail = tail;

			return r;
		}

		inbuf -= r;
	}

	return 0;
}

static inline int check_mailbox(u32 mailbox, int idx)
{
	return mailbox & mb_data[idx].mask_send;
}

static inline void purge_buffer(struct ringbuf *rb)
{
	rb->rb_in_tail = rb->rb_in_head;
}

int sipc_read(struct sipc *si, u32 mailbox, int *cond)
{
	int r = 0;
	int i;
	u32 res = 0;

	if (!si)
		return -EINVAL;

	r = _get_auth();
	if (r)
		return r;

	for (i = 0; i < IPCIDX_MAX; i++)
	{
		int inbuf;
		struct ringbuf *rb;

//		if (!check_mailbox(mailbox, i))
//			continue;

		rb = &si->rb[i];
		inbuf = CIRC_CNT(rb->rb_in_head, rb->rb_in_tail, rb->rb_in_size);
		if (!inbuf)
			continue;

		_pdbg("%s:1 %d bytes in %d\n", __func__, inbuf, i);

		r = rb->rb_read(si, inbuf, rb);
		_pdbg("%s:2 %d bytes in %d\n", __func__, inbuf, i);
		if (r < 0)
		{
			if (r == -EBADMSG)
				purge_buffer(rb);

			dev_err(&si->svndev->dev, "read err %d\n", r);
			break;
		}
#ifdef LOOP_BACK_TEST
		if (mailbox & (i != 1) & mb_data[i].mask_req_ack)
			res = mb_data[i].mask_res_ack;
#else
		if (mailbox & mb_data[i].mask_req_ack)
			res = mb_data[i].mask_res_ack;
#endif
	}

//	_req_rel_auth(si);
	_put_auth(si, res);

	if (res)
		onedram_write_mailbox(MB_DATA(res));

	*cond =	skb_queue_len(&si->rfs_rx);

	return r;
}

int sipc_rx(struct sipc *si)
{
	int tx_cnt;
	struct sk_buff *skb;

	if (!si)
		return -EINVAL;

	if (skb_queue_len(&si->rfs_rx) == 0)
		return 0;

	tx_cnt = 0;
	skb = skb_dequeue(&si->rfs_rx);
	while (skb) {
		_phonet_rx(si->svndev, skb, PN_RFS);
		tx_cnt++;
		if (tx_cnt > RFS_TX_RATE)
			break;
		skb = skb_dequeue(&si->rfs_rx);
	}

	return skb_queue_len(&si->rfs_rx);
}

static inline ssize_t _debug_show_buf(struct sipc *si, char *buf)
{
	int i;
	int r;
	int inbuf, outbuf;
	char *p = buf;

	r = _get_auth();
	if (r) {
		p += sprintf(p, "\nGet authority: timed out!\n");
		return p - buf;
	}

	p += sprintf(p, "\nHeader info ---------\n");

	for (i=0;i<IPCIDX_MAX;i++) {
		struct ringbuf *rb = &si->rb[i];
		inbuf = CIRC_CNT(rb->rb_in_head, rb->rb_in_tail, rb->rb_in_size);
		outbuf = CIRC_CNT(rb->rb_out_head, rb->rb_out_tail, rb->rb_out_size);
		p += sprintf(p, "%d\tSize\t%8u(in)\t%8u(out)\n"
				"\tIn\t%8u\t%8u\t%8u\n\tOut\t%8u\t%8u\t%8u\n",
				i, rb->rb_in_size, rb->rb_out_size,
				rb->rb_in_head, rb->rb_in_tail, inbuf,
				rb->rb_out_head, rb->rb_out_tail, outbuf);
	}
	_put_auth(si, 0);

	return p - buf;
}

static inline ssize_t _debug_show_pdp(struct sipc *si, char *buf)
{
	int i;
	char *p = buf;

	p += sprintf(p, "\nPDP count: %d\n", pdp_cnt);

	mutex_lock(&pdp_mutex);
	for (i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
		if (pdp_devs[i])
			p += sprintf(p, "pdp%d: %d", i,
					netif_queue_stopped(pdp_devs[i]));
	}
	mutex_unlock(&pdp_mutex);

	return p - buf;
}

ssize_t sipc_debug_show(struct sipc *si, char *buf)
{
	char *p = buf;

	if (!si || !buf)
		return 0;

	p += _debug_show_buf(si, p);

	p += _debug_show_pdp(si, p);

	return p - buf;
}

int sipc_debug(struct sipc *si, const char *buf)
{
#if 0
	int r;

	if (!si || !buf)
		return -EINVAL;

	r = _get_auth();
	if (r)
		return r;

	switch (buf[0]) {
	case 'R':
		/* do something */
		break;
	default:
		/* do nothing */
		break;
	}
	_put_auth(si, 0);
#endif
	return 0;
}

int sipc_check_skb(struct sipc *si, struct sk_buff *skb)
{
	struct phonethdr *ph;

	ph = pn_hdr(skb);

	if (ph->pn_res == PN_CMD)
		return 1;

	return 0;
}

int sipc_do_cmd(struct sipc *si, struct sk_buff *skb)
{
	if (!si)
		return -EINVAL;

	skb_pull(skb, sizeof(struct phonethdr) + 1);

	// TODO: 
	if (!strncmp("PHONE_ON", skb->data, sizeof("PHONE_ON"))) {

		return 0;
	}

	return 0;
}

static int pdp_activate(struct net_device *svndev, int channel)
{
	int idx;
	struct net_device *ndev;

	if (!svndev || channel < 1 || channel > PDP_MAX)
		return -EINVAL;

	idx = channel - 1; /* start from 0 */

	mutex_lock(&pdp_mutex);

	if (pdp_devs[idx]) {
		mutex_unlock(&pdp_mutex);
		return -EBUSY;
	}

	ndev = create_pdp(channel, svndev);
	if (IS_ERR(ndev)) {
		mutex_unlock(&pdp_mutex);
		return PTR_ERR(ndev);
	}

	pdp_devs[idx] = ndev;
	pdp_cnt++;

	mutex_unlock(&pdp_mutex);

	return 0;
}

static int pdp_deactivate(int channel)
{
	int idx;

	if (channel < 1 || channel > PDP_MAX)
		return -EINVAL;

	idx = channel - 1; /* start from 0 */

	mutex_lock(&pdp_mutex);

	if (!pdp_devs[idx]) {
		mutex_unlock(&pdp_mutex);
		return -EBUSY;
	}

	destroy_pdp(&pdp_devs[idx]);
	clear_bit(idx, pdp_bitmap);
	pdp_cnt--;

	mutex_unlock(&pdp_mutex);

	return 0;
}

static ssize_t show_act(struct device *d,
		struct device_attribute *attr, char *buf)
{
	int i;
	char *p = buf;

	mutex_lock(&pdp_mutex);

	for (i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
		if (pdp_devs[i])
			p += sprintf(p, "%d\n", (i+1));
	}

	mutex_unlock(&pdp_mutex);

	return p - buf;
}

static ssize_t show_deact(struct device *d,
		struct device_attribute *attr, char *buf)
{
	int i;
	char *p = buf;

	mutex_lock(&pdp_mutex);

	for(i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
		if (!pdp_devs[i])
			p += sprintf(p, "%d\n", (i+1));
	}

	mutex_unlock(&pdp_mutex);

	return p - buf;
}

static ssize_t store_act(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int r;
	unsigned long chan;
	struct net_device *ndev = to_net_dev(d);

	if (!ndev)
		return count;

	r = strict_strtoul(buf, 10, &chan);
	if (!r)
		r = pdp_activate(ndev, chan);

	if (r)
		dev_err(&ndev->dev, "Failed to activate pdp "
				" channel %lu: %d\n", chan, r);

	return count;
}

static ssize_t store_deact(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int r;
	unsigned long chan;
	struct net_device *ndev = to_net_dev(d);

	if (!ndev)
		return count;

	r = strict_strtoul(buf, 10, &chan);
	if (!r)
		r = pdp_deactivate(chan);

	if (r)
		dev_err(&ndev->dev, "Failed to deactivate pdp"
				" channel %lu: %d\n", chan, r);

	return count;
}

static ssize_t show_suspend(struct device *d,
		struct device_attribute *attr, char *buf)
{
	int i;
	char *p = buf;

	mutex_lock(&pdp_mutex);

	for (i=0;i<sizeof(pdp_devs)/sizeof(pdp_devs[0]);i++) {
		if (test_bit(i, pdp_bitmap))
			p += sprintf(p, "%d\n", (i+1));
	}

	mutex_unlock(&pdp_mutex);

	return p - buf;
}

static ssize_t store_suspend(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int r;
	unsigned long chan;
	int id;

	r = strict_strtoul(buf, 10, &chan);
	if (r)
		return count;

	if (chan < 1 || chan > PDP_MAX)
		return count;

	id = chan - 1;

	mutex_lock(&pdp_mutex);

	set_bit(id, pdp_bitmap);

	if (pdp_devs[id])
		netif_stop_queue(pdp_devs[id]);

	mutex_unlock(&pdp_mutex);

	return count;
}

static ssize_t store_resume(struct device *d,
		struct device_attribute *attr, const char *buf, size_t count)
{
	int r;
	unsigned long chan;
	int id;

	r = strict_strtoul(buf, 10, &chan);
	if (r)
		return count;

	if (chan < 1 || chan > PDP_MAX)
		return count;

	id = chan - 1;

	mutex_lock(&pdp_mutex);

	clear_bit(id, pdp_bitmap);

	if (pdp_devs[id])
		netif_wake_queue(pdp_devs[id]);

	mutex_unlock(&pdp_mutex);

	return count;
}

void sipc_ramdump(struct sipc *si)
{
	_go_dump(si);
}

#if defined(CONFIG_KERNEL_DEBUG_SEC_)
static void _go_dump(struct sipc *si)
{
	t_kernel_sec_mmu_info mmu_info;

	memset(cp_errmsg, 0, sizeof(cp_errmsg));
	strcpy(cp_errmsg, ERRMSG);

#if 0
	r = _get_auth();
	if (r)
		strcpy(cp_errmsg, ERRMSG);
	else {
		char *p;
		p = (char *)si->map + FATAL_DISP;
		memcpy(cp_errmsg, p, sizeof(cp_errmsg));
	}
#endif

	printk("CP Dump Cause - %s\n", cp_errmsg);

	kernel_sec_set_cause_strptr(cp_errmsg, sizeof(cp_errmsg));
	kernel_sec_set_upload_magic_number();
	kernel_sec_get_mmu_reg_dump(&mmu_info);
	kernel_sec_set_upload_cause(UPLOAD_CAUSE_CP_ERROR_FATAL);
	kernel_sec_hw_reset(false);

	// Never Return!!!
}
#endif
