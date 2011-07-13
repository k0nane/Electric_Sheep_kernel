/****************************************************************************

**

** COPYRIGHT(C) : Samsung Electronics Co.Ltd, 2006-2010 ALL RIGHTS RESERVED

**

****************************************************************************/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/miscdevice.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <linux/etherdevice.h>
#include <linux/random.h>
#include <linux/if_arp.h>
#include <linux/proc_fs.h>
#include <linux/freezer.h>
#include <linux/tty.h>
#include <linux/tty_driver.h>
#include <linux/tty_flip.h>
#include <linux/poll.h>
#include <linux/workqueue.h>

#include <linux/version.h>
#include <linux/vmalloc.h>
#include <linux/ip.h>
#include <linux/icmp.h>

#include <linux/time.h>


//#include "../common/pdptrace.h"

//currently no support for IPV6 for MBOX target cdma_kernel
//#ifdef MULTIPDP_IPV6
//#undef MULTIPDP_IPV6
//#endif
//#define MULTIPDP_IPV6	1	//Enabling support for IPV6

//#define __MULTIPDP_TEST__

//#define MULTIPDP_IPV6

/* Multiple PDP */
#ifdef MULTIPDP_IPV6
typedef enum _interface_type
{
	IPADDR_TYPE_IPV4 = 1,
	IPADDR_TYPE_IPV6,
        IPADDR_TYPE_IPV4V6,
        IPV6_TYPE_MAX
}interface_type_t;
#endif

typedef struct pdp_arg {
	unsigned char	id;
	char		ifname[16];
#ifdef MULTIPDP_IPV6
	char 		int_iden[8];
	interface_type_t en_interface_type;
#endif
} __attribute__ ((packed)) pdp_arg_t;

#if 0
#define IOC_MZ2_MAGIC		('h')
#define HN_PDP_ACTIVATE		_IOWR(IOC_MZ2_MAGIC, 0xf6, pdp_arg_t)
#define HN_PDP_DEACTIVATE	_IOW (IOC_MZ2_MAGIC, 0xf7, pdp_arg_t)
#define HN_PDP_ADJUST		_IOW (IOC_MZ2_MAGIC, 0xe2, int)
#else
#define IOC_MZ2_MAGIC		(0xC1)
#define HN_PDP_ACTIVATE		_IOWR(IOC_MZ2_MAGIC, 0xe0, pdp_arg_t)
#define HN_PDP_DEACTIVATE	_IOW(IOC_MZ2_MAGIC, 0xe1, pdp_arg_t)
#define HN_PDP_ADJUST		_IOW(IOC_MZ2_MAGIC, 0xe2, int)
#define HN_PDP_TXSTART		_IO(IOC_MZ2_MAGIC, 0xe3)
#define HN_PDP_TXSTOP		_IO(IOC_MZ2_MAGIC, 0xe4)
#define HN_PDP_SETRADIO		_IOW(IOC_MZ2_MAGIC, 0xe5,int)
#define HN_PDP_DATASTATUS	_IOW(IOC_MZ2_MAGIC, 0xe6,int)

#endif

//#include <asm/hardware.h>
//#include <asm/uaccess.h>
#include <mach/hardware.h>
#include <linux/uaccess.h>

/*
 * Driver macros
 */
#define DPRAM_POLL 0x01
#define ONEDRAM_POLL 0x02

#define MULTIPDP_ERROR			/* Define this for error messages */

//#define USE_LOOPBACK_PING		/* Use loopback ping test */
//#define LOOP_BACK_TEST			/* Use Loopback test via CH.31 */

#ifdef USE_LOOPBACK_PING
#include <linux/ip.h>
#include <linux/icmp.h>
#include <net/checksum.h>
#endif

#define ENABLE_MULTIPDP_DEBUG_PRINTS

#ifdef ENABLE_MULTIPDP_DEBUG_PRINTS

#define PRINT_READ_INFO
#define PRINT_WRITE_INFO

#define MULTIPDP_PRINT_ERROR		0x0001
#define MULTIPDP_PRINT_WARNING		0x0002
#define MULTIPDP_PRINT_INFO			0x0004
#define MULTIPDP_PRINT_WRITE		0x0008
#define MULTIPDP_PRINT_READ			0x0010

void multipdp_debug_print(u32 mask,  const char *fmt, ...);
void register_debug_control_attribute(void);
void deregister_debug_control_attribute(void);
u16 mulitpdp_debug_mask = MULTIPDP_PRINT_ERROR | MULTIPDP_PRINT_WARNING | MULTIPDP_PRINT_INFO;

#define MULTIPDP_LOG_ERR(s,arg...)			multipdp_debug_print(MULTIPDP_PRINT_ERROR, s, ## arg)
#define MULTIPDP_LOG_WARN(s,arg...)			multipdp_debug_print(MULTIPDP_PRINT_WARNING, s, ## arg)
#define MULTIPDP_LOG_INFO(s,arg...)			multipdp_debug_print(MULTIPDP_PRINT_INFO, s, ## arg)
#define MULTIPDP_LOG_READ(s,arg...)			multipdp_debug_print(MULTIPDP_PRINT_WRITE, s, ## arg)
#define MULTIPDP_LOG_WRITE(s,arg...)		multipdp_debug_print(MULTIPDP_PRINT_READ, s, ## arg)
#else
#define MULTIPDP_LOG_ERR(s,arg...)			do { } while (0)
#define MULTIPDP_LOG_WARN(s,arg...)			do { } while (0)
#define MULTIPDP_LOG_INFO(s,arg...)			do { } while (0)
#define MULTIPDP_LOG_READ(s,arg...)			do { } while (0)
#define MULTIPDP_LOG_WRITE(s,arg...)		do { } while (0)
#endif /*ENABLE_MULTIPDP_DEBUG_PRINTS*/


//#define PROFILE_ENABLE /* for PDP profiling/trace */

/* Maximum number of PDP context */
#define MAX_PDP_CONTEXT			12

/* Maximum PDP data length */
#define MAX_PDP_DATA_LEN		1550	

/* Maximum PDP packet length including header and start/stop bytes */
#define MAX_PDP_PACKET_LEN		(MAX_PDP_DATA_LEN + 6 + 2)

/* Prefix string of virtual network interface */
#define VNET_PREFIX				"pdp"

/* Device node name for application interface */
#define APP_DEVNAME				"multipdp"

/* onedram device node name */
#define ONEDRAM_DEVNAME			"/dev/onedram1"
#define DPRAM_DEVNAME 			"/dev/dpram1"

/* Device types */
#define DEV_TYPE_NET			0 /* network device for IP data */
#define DEV_TYPE_SERIAL			1 /* serial device for CSD */

/* Device flags */
#define DEV_FLAG_STICKY			0x1 /* Sticky */

/* Device major & minor number */
#define CSD_MAJOR_NUM			251
#define CSD_MINOR_NUM			0

/* Device Identification */
#define ONEDRAM_DEV 			0
#define DPRAM_DEV			1
#define UNKNOWN_DEV			2

/* Radio Tye Identification */
#define LTE			0
#define CDMA			1

/* Data Status */
#define SUSPEND			0
#define ACTIVE			1

#define __USE_ONLY_DPRAM

#ifdef LOOP_BACK_TEST


/**********************************************************************
	loop back test implementation		

	Usage :

	1. start test
	
	loopback test can be triggered by setting '/sys/class/misc/multipdp/loopback'

	"
	# cd /sys/class/misc/multipdp/
	# echo start > loopback
	
	2. get stastic result
	
	it shows some result value for this test)
	
	"
	# cat loopback

	3. stop test

	it stops loopback test

	"
	# echo stop > loopback
	(be careful, it does not show any result)


**********************************************************************/
#define LOOP_BACK_CHANNEL	31

int loopback_ongoing = 0;

char loopback_data[MAX_PDP_DATA_LEN];

char loopback_value[256];

struct loopback_result
{
	int nTransfered;
	int nPacketDataSize;
	struct timeval nStartTime;
	struct timeval nEndTime;
};

static struct loopback_result loopback_res;

static ssize_t show_loopback_value(struct device *dev, struct device_attribute *attr, char * buf)
{
	unsigned int nElapsedtime_s, total_size;
	if(!strncmp(loopback_value, "start", 5)) {
		// show elapsed value
		do_gettimeofday(&loopback_res.nEndTime);

		nElapsedtime_s = (loopback_res.nEndTime.tv_sec - loopback_res.nStartTime.tv_sec)
			+ (loopback_res.nEndTime.tv_usec - loopback_res.nStartTime.tv_usec)/1000000;

		total_size = loopback_res.nTransfered * loopback_res.nPacketDataSize;
		
		return sprintf(buf,
			"\n=====	LoopBack Test Result	=====\n\n"
			"Transfered Items = %d\n"
			"Packet Data Size = %d\n"
			"Total transfer size = %d\n"
			"Elapsed Time = %d (s)\n"
			"Mean Value = %d (byte/sec)\n"
			"\n=====================================\n"
			,
			loopback_res.nTransfered,
			loopback_res.nPacketDataSize,
			total_size,
			nElapsedtime_s,
			total_size/nElapsedtime_s
			);
						
	}
	else {
		// show test is not on going.
		return sprintf(buf, "loopback test is not on going\n");
	}
}

//declare fucntion first

static struct pdp_info * pdp_get_dev(u8 id);
static int pdp_mux(struct pdp_info *dev, const void *data, size_t len);

static send_loop_back_packet(const char* data, int size)
{
	struct pdp_info* dev = pdp_get_dev(LOOP_BACK_CHANNEL);

	if (loopback_ongoing) {
		//MULTIPDP_LOG_INFO("send loopback packet start [%d]\n",loopback_res.nTransfered);
		pdp_mux(dev, data, size);
		//MULTIPDP_LOG_INFO("send loopback packet end [%d]\n",loopback_res.nTransfered);
		loopback_res.nTransfered++;
		}
	
}

static ssize_t store_loopback_value(struct device *dev, struct device_attribute *attr, char * buf, size_t count)
{
	int i;

	// we can send various size of data by setting this value as mutiple of 10
	int data_size = 1500;
	
	char temp_str[10] = "0123456789";
	if ( !strncmp(buf, "start", 5))
		{
		sscanf(buf, "%s", loopback_value);

		// initialize stastics value
		loopback_res.nTransfered = 0;
		loopback_res.nPacketDataSize = data_size;
		
		// make data
		for (i = 0; i < (data_size/10); i++) {
			memcpy((loopback_data + i*10), temp_str, 10);
		}

		loopback_ongoing = 1;
		
		do_gettimeofday(&loopback_res.nStartTime);
		
		send_loop_back_packet(loopback_data, data_size);
	}
	else if (!strncmp(buf, "stop", 4)) {
		sscanf(buf, "%s", loopback_value);
		
		loopback_ongoing = 0;
		
		do_gettimeofday(&loopback_res.nEndTime);
	}
	return strnlen(buf, 256);
}

static DEVICE_ATTR(loopback, S_IRUGO|S_IWUSR, show_loopback_value, store_loopback_value);
#endif


/*
 * Variable types
 */
#ifdef PROFILE_ENABLE
typedef struct
{
	struct sk_buff* skb;
	int fwd_pkt_number;
}WorkQ_data;
WorkQ_data wq_data;

#endif
/* PDP data packet header format */
struct pdp_hdr_onedram {
	u32	len;		/* Data length *///to Support LTE MODEM BINARY
	u8	id;			/* Channel ID */
	u8	control;	/* Control field */
} __attribute__ ((packed));

struct pdp_hdr_dpram {
	u16	len;		/* Data length */// to Support  CDMA MODEM BINARY
	u8	id;			/* Channel ID */
	u8	control;	/* Control field */
} __attribute__ ((packed));

/* Virtual network interface */
struct vnet_struct{
	struct net_device	*net;
	struct net_device_stats	stats;
	struct work_struct	xmit_work_struct;
	struct sk_buff* 	skb_ptr;
	u8	    netq_init;				//0:netif_start_queue called    1: netif_start_queue() not called
	u8	    netq_active;			//0: don't call netif_wake_queue    1: Call netif_wake_queue
	struct semaphore netq_sem;	// sem to protect netq_init and netq_active
};

/* PDP information type */
struct pdp_info {
	/* PDP context ID */
	u8		id;
        /* onedram or dpram interface */
	atomic_t		intf_dev; 

	/* Device type */
	unsigned		type;

	/* Device flags */
	unsigned		flags;

	/* Tx packet buffer */
	u8		*tx_buf;

	/* App device interface */
	union {
		/* Virtual network interface */
		struct vnet_struct vnet_u;

		/* Virtual serial interface */
		struct {
            // skumar@wtl one more tty_driver is needed for ttyETS
#ifdef LOOP_BACK_TEST
			// ttyCDMA, ttyEFS, ttyGPS, ttyKTRA, ttySMD, ttyPCM, ttySCRN, ttyCSD, ttyETS, ttyLOBK
			struct tty_driver	tty_driver[11];
#else
			struct tty_driver	tty_driver[10];	// ttyCDMA, ttyEFS, ttyGPS, ttyKTRA, ttySMD, ttyPCM, ttySCRN, ttyCSD, ttyETS
#endif
			//int			refcount;
			atomic_t		refcount;
			struct tty_struct	*tty_table[1];
			struct ktermios		*termios[1];
			struct ktermios		*termios_locked[1];
			char			tty_name[16];
			struct tty_struct	*tty;
			struct semaphore	write_lock;
		} vs_u;
	} dev_u;
#define vn_dev		dev_u.vnet_u
#define vs_dev		dev_u.vs_u
};

/*
 * Global variables
 */

/* PDP information table */
static struct pdp_info *pdp_table[MAX_PDP_CONTEXT];
static DECLARE_MUTEX(pdp_lock);

/* onedram-related stuffs */
static struct task_struct *poll_task;
static struct file *onedram_filp;
static struct file *dpram_filp;

static DECLARE_COMPLETION(poll_complete);

static int g_adjust = 9;
static int g_radio = CDMA;		// Temp change to support EHRPD  LTE;//MULTIRIL
static int g_datastatus = ACTIVE;//To Suspend or resume data
static unsigned long workqueue_data = 0;
static unsigned char pdp_rx_buf[MAX_PDP_DATA_LEN];

static int pdp_tx_flag = 0;
/*
 * Function declarations
 */
static int pdp_mux(struct pdp_info *dev, const void *data, size_t len);
//static int pdp_demux(void);
static int pdp_demux(struct file *,int devtype);

static inline struct pdp_info * pdp_get_serdev(const char *name);
/* 
	dpram IO functions	
*/
static inline struct file * dpram_open(void)
{

	int ret =0;
	struct file *filp;
	struct termios termios;
	mm_segment_t oldfs;
	filp = filp_open(DPRAM_DEVNAME, O_RDWR|O_NONBLOCK, 0);
	if (IS_ERR(filp)) {
		MULTIPDP_LOG_ERR("filp_open() failed~!: %ld \n", PTR_ERR(filp));
		return NULL;
	}
	MULTIPDP_LOG_INFO(" dpram_open successd !!!! \n");

#if 1	// hobac.
	oldfs = get_fs(); set_fs(get_ds());

	ret = filp->f_op->unlocked_ioctl(filp, 
			TCGETA, (unsigned long)&termios);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("dpram->ioctl() failed: %d\n", ret);
		filp_close(filp, current->files);
		return NULL;
	}
#endif

	set_fs(oldfs);

	termios.c_cflag = CS8 | CREAD | HUPCL | CLOCAL | B115200;
	termios.c_iflag = IGNBRK | IGNPAR;
	termios.c_lflag = 0;
	termios.c_oflag = 0;
	termios.c_cc[VMIN] = 1;
	termios.c_cc[VTIME] = 1;

	oldfs = get_fs(); set_fs(get_ds());
	ret = filp->f_op->unlocked_ioctl(filp, 
			TCSETA, (unsigned long)&termios);

	set_fs(oldfs);
	if (ret < 0) 
	{
		MULTIPDP_LOG_ERR("dpram->ioctl() failed for termios with: %d\n", ret);
		filp_close(filp, current->files);
		return NULL;
	}	
	return filp;

}


/*
 * onedram I/O functions
 */
#ifndef __USE_ONLY_DPRAM
static inline struct file *onedram_open(void)
{
	int ret;
	struct file *filp;
	struct termios termios;
	mm_segment_t oldfs;

	filp = filp_open(ONEDRAM_DEVNAME, O_RDWR|O_NONBLOCK, 0);
	if (IS_ERR(filp)) {
		MULTIPDP_LOG_ERR("onedram filp_open() failed~!: %ld\n", PTR_ERR(filp));
		return NULL;
	}

	MULTIPDP_LOG_INFO("Open OneDram File from Multipdp init \n");

#if 1	// hobac.
	oldfs = get_fs(); set_fs(get_ds());

	ret = filp->f_op->unlocked_ioctl(filp, 
				TCGETA, (unsigned long)&termios);
	set_fs(oldfs);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("onedram f_op->ioctl() failed: %d\n", ret);
		filp_close(filp, current->files);
		return NULL;
	}

	termios.c_cflag = CS8 | CREAD | HUPCL | CLOCAL | B115200;
	termios.c_iflag = IGNBRK | IGNPAR;
	termios.c_lflag = 0;
	termios.c_oflag = 0;
	termios.c_cc[VMIN] = 1;
	termios.c_cc[VTIME] = 1;

	oldfs = get_fs(); set_fs(get_ds());
	ret = filp->f_op->unlocked_ioctl(filp, 
				TCSETA, (unsigned long)&termios);
	set_fs(oldfs);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("onedram f_op->ioctl()for termios failed with: %d\n", ret);
		filp_close(filp, current->files);
		return NULL;
	}
#endif
	return filp;
}
#endif

static inline void dpram_close(struct file *dpram_filp)
{
	filp_close(dpram_filp, current->files);
	
}

static inline int multipdp_poll(struct file *d_filp)
{
	int ret;
	unsigned int dpram_mask;
    struct poll_wqueues wait_table;
	mm_segment_t oldfs;

	MULTIPDP_LOG_INFO("Starting multipdp_poll !!!\n");

	poll_initwait(&wait_table);

	while(1) {
		//MULTIPDP_LOG_INFO("Polling OneDram TTY Device \n");
		set_current_state(TASK_INTERRUPTIBLE);
		oldfs = get_fs(); set_fs(get_ds());
		dpram_mask = d_filp->f_op->poll(d_filp, &wait_table.pt);
		set_fs(oldfs);
		if (dpram_mask & POLLIN) {
			/* got data */
			ret = DPRAM_POLL;
	        MULTIPDP_LOG_INFO("multipdp_poll data available in dpram\n");
            break;
		}
		if (wait_table.error) {
			
			MULTIPDP_LOG_ERR("multipdp_poll ==> Polling wait table error \n");
			ret = wait_table.error;
			break;
		}

		if (signal_pending(current)) {
			/* got signal */
			ret = -ERESTARTSYS;
			MULTIPDP_LOG_ERR("multipdp_poll ==> Polling ERESTARTSYS error \n");
			break;
		}

		schedule();
	}
	set_current_state(TASK_RUNNING);
	poll_freewait(&wait_table);

	return ret;
}

static inline int multipdp_write(struct file *filp, const void *buf, size_t count,
			      int nonblock)
{
#ifdef PROFILE_ENABLE
	log_fwd_sniff_info(SNIFF_POINT_ID_MULTIPDP_WRITE , buf, count);
#endif	
	int ret, n = 0;
	mm_segment_t oldfs;

	MULTIPDP_LOG_INFO("In multipdp_write %d \n", count);

	if(pdp_tx_flag) {
		return -EAGAIN;
	}

	while (count) {
		if (!filp) {
			MULTIPDP_LOG_ERR("NULL filp is passed in multipdp_write\n");
			return -ENODEV;
		}
		filp->f_flags |= O_NONBLOCK;
		oldfs = get_fs(); set_fs(get_ds());
		ret = filp->f_op->write(filp, buf + n, count, &filp->f_pos);
		set_fs(oldfs);
		filp->f_flags &= ~O_NONBLOCK;
		if (ret < 0) {
			if (ret == -EAGAIN && !nonblock) {
				continue;
			}
			MULTIPDP_LOG_ERR("multipdp_write ==> f_op->write() failed: %d\n", ret);
			return ret;
		}
		n += ret;
		count -= ret;
	}
	return n;
}

static inline int multipdp_read(struct file *filp, void *buf, size_t count)
{
	int ret, n = 0;
	mm_segment_t oldfs;
	unsigned long start_time = jiffies; 
    struct timeval elapsedtime_tv = {0};

#ifdef PROFILE_ENABLE
	log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_READ_START,NULL,0,0);
#endif	
	
	MULTIPDP_LOG_INFO("In multipdp_read with count = %d\n", count);
	while (count) {
		if (!filp) {
			MULTIPDP_LOG_ERR("NULL filp is passed in multipdp_read\n");
			return -ENODEV;
		}
		filp->f_flags |= O_NONBLOCK;
		oldfs = get_fs(); set_fs(get_ds());
		ret = filp->f_op->read(filp, buf + n, count, &filp->f_pos);

		MULTIPDP_LOG_INFO("multipdp_read ==> f_op->read returned  %d \n", ret);
		set_fs(oldfs);
		filp->f_flags &= ~O_NONBLOCK;
		if (ret < 0) {
			if (ret == -EAGAIN) {
				continue;
			}
			MULTIPDP_LOG_ERR("multipdp_read ==> f_op->read() failed: %d\n", ret);
			return ret;
		}
		n += ret;
		count -= ret;
#ifdef PRINT_READ_INFO
		MULTIPDP_LOG_READ("[%s] ret: %d, count: %d n: %d\n", __func__, ret, count, n);
#endif
		jiffies_to_timeval( jiffies - start_time, &elapsedtime_tv );
		if (elapsedtime_tv.tv_sec > 1)
		{
			return -1;
		}

	}

#ifdef PROFILE_ENABLE
	log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_READ_END,buf,n,0);
#endif	

	return n;
}

static inline int multipdp_flush_rx(struct file *filp, size_t count)
{
	int ret, n = 0;
	char *buf;
	mm_segment_t oldfs;

	buf = vmalloc(count);
	if (buf == NULL) return -ENOMEM;

	while (count) {
		filp->f_flags |= O_NONBLOCK;
		oldfs = get_fs(); set_fs(get_ds());
		ret = filp->f_op->read(filp, buf + n, count, &filp->f_pos);
		set_fs(oldfs);
		filp->f_flags &= ~O_NONBLOCK;
		if (ret < 0) {
			if (ret == -EAGAIN) continue;
			MULTIPDP_LOG_ERR("multipdp_flush_rx ==> f_op->read() failed: %d\n", ret);
			vfree(buf);
			return ret;
		}
		n += ret;
		count -= ret;

		break; // We should break, otherwise we will loop. Also there is no point in reading fruther as we already flushed what we have
	}
	vfree(buf);
	return n;
}


static int poll_thread(void *data)
{
	int ret = 0;
	//unsigned long flags;
	struct file *filp;
    struct file *dpram_thread_filp;

	int devtype;
	poll_task = current;
	MULTIPDP_LOG_INFO("%s called\n",__FUNCTION__);
	daemonize("poll_thread");
	//reparent_to_init();  // for 2.6 kernel porting : this seems not to be used in driver
	// current->tty = NULL; // for 2.6 kernel porting
	
	strcpy(current->comm, "multipdp");

#if 0
	/* scheduler policy and priority */
	current->policy = SCHED_OTHER;
	current->nice = -10;
	current->rt_priority = 1;
#endif
	/* set signals to accept */
	//spin_lock_irqsave(&current->sigmask_lock, flags); // for 2.6 kernel proting
	siginitsetinv(&current->blocked, sigmask(SIGUSR1));
	//recalc_sigpending(current);
	recalc_sigpending();
	//spin_unlock_irqrestore(&current->sigmask_lock, flags); // for 2.6 kernel proting

	dpram_thread_filp = dpram_open();
	if (dpram_thread_filp == NULL) {
		MULTIPDP_LOG_WARN("[%s] dpram_open failed!!\n", __FUNCTION__);
		goto out;
	}
	dpram_filp = dpram_thread_filp;
	/* send start signal */
	complete(&poll_complete);

	while (1) {
		MULTIPDP_LOG_INFO("multipdp_poll loop\n");
        ret = multipdp_poll(dpram_thread_filp);
		if(ret & DPRAM_POLL)
		{
			filp = dpram_thread_filp;
			devtype =  DPRAM_DEV;				
			MULTIPDP_LOG_INFO("poll_thread ==> data available in dpram\n");
			char ch;
			multipdp_read(filp, &ch, sizeof(ch));
			if (ch == 0x7f) {
				MULTIPDP_LOG_INFO("poll_thread ==> got the header, calling pdp_demux \n");
                pdp_demux(filp,devtype);
			}
		}
		if (ret == -ERESTARTSYS) {
			if (sigismember(&current->pending.signal, SIGUSR1)) {
				sigdelset(&current->pending.signal, SIGUSR1);
				recalc_sigpending();
				ret = 0;
				break;
			}
		}
		
		if (ret < 0) 
		{
			//MULTIPDP_LOG_ERR("multipdp_poll() failed, ret < 0\n");
			//break;
		}
		
		try_to_freeze();
#ifdef PROFILE_ENABLE
		log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_POLLTHREAD_END,  NULL,0,0);
#endif

	}
	dpram_close(dpram_thread_filp);

	dpram_filp = NULL;
	onedram_filp = NULL;

out:
	poll_task = NULL;
	dpram_thread_filp = NULL;
	/* send finish signal and exit */
	complete_and_exit(&poll_complete, ret);
}

/*
 * Virtual Network Interface functions
 */

static int vnet_open(struct net_device *net)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	struct pdp_info *dev = (struct pdp_info *)net->ml_priv;
#else
	struct pdp_info *dev = (struct pdp_info *)net->priv;
#endif
	INIT_WORK(&dev->vn_dev.xmit_work_struct, NULL);

	down(&dev->vn_dev.netq_sem);
	netif_start_queue(net);
	if(dev->vn_dev.netq_active == SUSPEND)
		netif_stop_queue(net);
	dev->vn_dev.netq_init = 1;
	up(&dev->vn_dev.netq_sem);

	return 0;
}

static int vnet_stop(struct net_device *net)
{
	netif_stop_queue(net);
	flush_scheduled_work(); /* flush any pending tx tasks */

	return 0;
}

static void vnet_defer_xmit(struct work_struct *data)
{
	struct vnet_struct *vnet_ptr = NULL;
	struct sk_buff *skb=NULL;

#ifdef PROFILE_ENABLE
	WorkQ_data* wq_data_dfxmit;
	wq_data_dfxmit = (struct WorkQ_data*)workqueue_data;
	skb = wq_data_dfxmit->skb;
	follow_fwd_packet_number = wq_data_dfxmit->fwd_pkt_number;
#else
	vnet_ptr = container_of(data, struct vnet_struct, xmit_work_struct);
	skb = vnet_ptr->skb_ptr;
#endif

	
	struct net_device *net = (struct net_device *)skb->dev;
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	struct pdp_info *dev = (struct pdp_info *)net->ml_priv;
#else
	struct pdp_info *dev = (struct pdp_info *)net->priv;
#endif
#ifdef PROFILE_ENABLE
	log_fwd_sniff_info(SNIFF_POINT_ID_VENT_DEFER_XMIT,skb->data, skb->len );
#endif

	int ret = 0;

	MULTIPDP_LOG_INFO("In vnet_defer_xmit\n");
#ifdef PRINT_WRITE_INFO	
	MULTIPDP_LOG_WRITE("WRITE vnet_defer_xmit ==> Len %d \n", skb->len);
#endif
	ret = pdp_mux(dev, skb->data, skb->len);

	if (ret < 0) {
		dev->vn_dev.stats.tx_dropped++;
	}
	
	else {
		net->trans_start = jiffies;
		dev->vn_dev.stats.tx_bytes += skb->len;
		dev->vn_dev.stats.tx_packets++;
	}

	dev_kfree_skb_any(skb);
	down(&dev->vn_dev.netq_sem);
	if (dev->vn_dev.netq_active == ACTIVE){
		netif_wake_queue(net);
	}
	up(&dev->vn_dev.netq_sem);
}

static int vnet_start_xmit(struct sk_buff *skb, struct net_device *net)
{
int i;
	MULTIPDP_LOG_INFO("In vnet_start_xmit\n");
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	struct pdp_info *dev = (struct pdp_info *)net->ml_priv;
#else
	struct pdp_info *dev = (struct pdp_info *)net->priv;
#endif

#ifdef USE_LOOPBACK_PING

	int ret;
	struct sk_buff *skb2;
	struct icmphdr *icmph;
	struct iphdr *iph;
#endif
#ifdef PROFILE_ENABLE

fwd_packet_number++;
wq_data.fwd_pkt_number = fwd_packet_number;
wq_data.skb = skb;
//log_fwd_sniff_info(byte sniff_id, byte pktlength, char * packet)
log_fwd_sniff_info(SNIFF_POINT_ID_VENT_XMIT,  skb->data,skb->len);
#endif

#ifdef USE_LOOPBACK_PING


	MULTIPDP_LOG_INFO("vnet_start_xmit ==> buffer Len %d \n", skb->len);

	//for(i=0; i<skb->len; i++)
		//MULTIPDP_LOG_INFO(" %c ", skb->data[i]);
	
	dev->vn_dev.stats.tx_bytes += skb->len;
	dev->vn_dev.stats.tx_packets++;
	MULTIPDP_LOG_INFO("vnet_start_xmit called in loopback \n");
	skb2 = alloc_skb(skb->len, GFP_ATOMIC);
	if (skb2 == NULL) {
		MULTIPDP_LOG_ERR("vnet_start_xmit ==>alloc_skb() failed\n");
		dev_kfree_skb_any(skb);
		return -ENOMEM;
	}

	memcpy(skb2->data, skb->data, skb->len);
	skb_put(skb2, skb->len);
	dev_kfree_skb_any(skb);

	icmph = (struct icmphdr *)(skb2->data + sizeof(struct iphdr));
	iph = (struct iphdr *)skb2->data;

	icmph->type = __constant_htons(ICMP_ECHOREPLY);

	ret = iph->daddr;
	iph->daddr = iph->saddr;
	iph->saddr = ret;
	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);

	skb2->dev = net;
	skb2->protocol = __constant_htons(ETH_P_IP);

	netif_rx(skb2);

	MULTIPDP_LOG_INFO("vnet_start_xmit enabled \n");
	dev->vn_dev.stats.rx_packets++;
	dev->vn_dev.stats.rx_bytes += skb->len;
#else

#ifdef PRINT_WRITE_INFO	
	MULTIPDP_LOG_WRITE("WRITE vnet_start_xmit ==> Buff Len %d \n", skb->len);
#endif
#ifdef PROFILE_ENABLE
	workqueue_data = (unsigned long)&wq_data;
#else
	dev->vn_dev.skb_ptr = skb;
#endif
	PREPARE_WORK(&dev->vn_dev.xmit_work_struct,vnet_defer_xmit);
#ifdef PRINT_WRITE_INFO	
	MULTIPDP_LOG_WRITE("WRITE vnet_start_xmit ==> schedule write work item \n");
#endif	
	schedule_work(&dev->vn_dev.xmit_work_struct);
	netif_stop_queue(net);
#endif

	return 0;
}

static int vnet_recv(struct pdp_info *dev, size_t len,int devtype)
{
	struct sk_buff *skb;
	int ret;
	unsigned char first_byte;

#ifdef PROFILE_ENABLE
		log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_VNETRECV_START,NULL,0,0);
#endif

	/* @LDK@ for multiple pdp.. , ex) email & streaming.. by hobac. */
	if (!dev) {
		return 0;
	}

	MULTIPDP_LOG_INFO("In vnet_recv \n");

	if (!netif_running(dev->vn_dev.net)) {
		MULTIPDP_LOG_ERR("%s(id: %u) is not running\n", 
			dev->vn_dev.net->name, dev->id);
		return -ENODEV;
	}

	skb = alloc_skb(len, GFP_ATOMIC);

	if (skb == NULL) {
		MULTIPDP_LOG_ERR("vnet_recv ==> alloc_skb() failed\n");
		return -ENOMEM;
	}

	if(devtype == DPRAM_DEV )
	{
		ret = multipdp_read(dpram_filp, skb->data, len);
	}
	else if(devtype == ONEDRAM_DEV )
	{
		ret = multipdp_read(onedram_filp, skb->data, len);
	}

	//ret = multipdp_read(onedram_filp, skb->data, len);

	if (ret < 0) {
		MULTIPDP_LOG_ERR("vnet_recv ==>multipdp_read failed: %d\n", ret);
		dev_kfree_skb_any(skb);
		return ret;
	}

	skb_put(skb, ret);

	skb->dev = dev->vn_dev.net;
#ifdef MULTIPDP_IPV6
	// Added kushagra for giving IPv6 support
	first_byte = skb->data[0];
	first_byte >>= 4;
	if(first_byte == 0x06)
		skb->protocol = __constant_htons(ETH_P_IPV6);
	else
#endif
	skb->protocol = __constant_htons(ETH_P_IP);
	// Added kushagra for giving IPv6 support

	netif_rx(skb);

	dev->vn_dev.stats.rx_packets++;
	dev->vn_dev.stats.rx_bytes += skb->len;
#ifdef PROFILE_ENABLE
	log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_VNETRECV_END,NULL,0,0);
#endif	
	return 0;
}

static struct net_device_stats *vnet_get_stats(struct net_device *net)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	struct pdp_info *dev = (struct pdp_info *)net->ml_priv;
#else
	struct pdp_info *dev = (struct pdp_info *)net->priv;
#endif
	return &dev->vn_dev.stats;
}

static void vnet_tx_timeout(struct net_device *net)
{
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	struct pdp_info *dev = (struct pdp_info *)net->ml_priv;
#else
	struct pdp_info *dev = (struct pdp_info *)net->priv;
#endif

	net->trans_start = jiffies;
	dev->vn_dev.stats.tx_errors++;

	down(&dev->vn_dev.netq_sem);
	if (dev->vn_dev.netq_active == ACTIVE){
		netif_wake_queue(net);
	}
	up(&dev->vn_dev.netq_sem);
	MULTIPDP_LOG_ERR("vnet_tx_timeout ==>Tx timed out\n");
}

static const struct net_device_ops pdp_netdev_ops = {
	.ndo_open		= vnet_open,
	.ndo_stop		= vnet_stop,
	.ndo_start_xmit	= vnet_start_xmit,
	.ndo_get_stats	= vnet_get_stats,
	.ndo_tx_timeout	= vnet_tx_timeout,		
};


static void vnet_setup(struct net_device *dev)
{
    dev->netdev_ops = &pdp_netdev_ops;
	dev->type		= ARPHRD_PPP;
	dev->hard_header_len 	= 0;
	dev->mtu		= MAX_PDP_DATA_LEN;
	dev->addr_len		= 0;
	dev->tx_queue_len	= 1000;
	dev->flags		= IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	dev->watchdog_timeo	= 5 * HZ;
}

#ifdef MULTIPDP_IPV6
static void vnet_setup_v6(struct net_device *dev)
{
    dev->netdev_ops = &pdp_netdev_ops;
	dev->type		= ARPHRD_PPP;
	dev->hard_header_len 	= 0;
	dev->mtu		= MAX_PDP_DATA_LEN;
	dev->addr_len		= 6;
	dev->tx_queue_len	= 1000;
	dev->flags		= IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	dev->watchdog_timeo	= 5 * HZ;
}
#endif

static struct net_device *vnet_add_dev(void *priv)
{
	int ret;
	struct net_device *dev;

	MULTIPDP_LOG_INFO(" %s\n",__FUNCTION__);
	dev = alloc_netdev(0, "hrpd%d", vnet_setup);
	if (dev == NULL) {
		MULTIPDP_LOG_ERR("vnet_add_dev ==> alloc_netdev failed\n");
		return NULL;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	dev->ml_priv		= priv;
#else
	dev->priv		= priv;
#endif

	ret = register_netdev(dev);

	if (ret != 0) {
		MULTIPDP_LOG_ERR("vnet_add_dev ==> register_netdevice failed: %d\n", ret);
		kfree(dev);
		return NULL;
	}
	return dev;
}

#ifdef MULTIPDP_IPV6
// Added kushagra for ipv6
static struct net_device *vnet_add_dev_v6(void *priv)
{
	int ret;
	struct net_device *dev;

	MULTIPDP_LOG_INFO(" %s\n",__FUNCTION__);
	dev = alloc_netdev(0, "hrpd%d", vnet_setup_v6);
	if (dev == NULL) {
		MULTIPDP_LOG_ERR(" vnet_add_dev_v6 ==> out of memory\n");
		return NULL;
	}
#if (LINUX_VERSION_CODE >= KERNEL_VERSION(2, 6, 29))
	dev->ml_priv		= priv;
#else
	dev->priv		= priv;
#endif

	ret = register_netdev(dev);

	if (ret != 0) {
		MULTIPDP_LOG_ERR("vnet_add_dev_v6 ==> register_netdevice failed: %d\n", ret);
		kfree(dev);
		return NULL;
	}
	return dev;
}
#endif
// Added kushagra for ipv6
#if 0
static struct net_device *vnet_add_dev(void *priv)
{
	int ret;
	struct net_device *net;

	net = kmalloc(sizeof(*net), GFP_KERNEL);

	if (net == NULL) {
		MULTIPDP_LOG_ERR("vnet_add_dev ==> out of memory, kmalloc failed \n");
		return NULL;
	}

	memset(net, 0, sizeof(*net));

//	SET_MODULE_OWNER(net);
	strcpy(net->name, VNET_PREFIX "%d");
	net->open		= vnet_open;
	net->stop		= vnet_stop;
	net->hard_start_xmit	= vnet_start_xmit;
	net->get_stats		= vnet_get_stats;
	net->tx_timeout		= vnet_tx_timeout;
	net->type		= ARPHRD_PPP;
	net->hard_header_len 	= 0;
	net->mtu		= MAX_PDP_DATA_LEN;
	net->addr_len		= 0;
	net->tx_queue_len	= 1000;
	net->flags		= IFF_POINTOPOINT | IFF_NOARP | IFF_MULTICAST;
	net->watchdog_timeo	= 5 * HZ;
	net->priv		= priv;

	ret = register_netdev(net);

	if (ret != 0) {
		MULTIPDP_LOG_ERR("vnet_add_dev ==> register_netdevice failed: %d\n", ret);
		kfree(net);
		return NULL;
	}

	return net;
}
#endif
static void vnet_del_dev(struct net_device *net)
{
	unregister_netdev(net);
	kfree(net);
}

/*
 * Virtual Serial Interface functions
 */
#if 0
static int vs_open(struct tty_struct *tty, struct file *filp)
{
	struct pdp_info *dev;

	dev = pdp_get_serdev(tty->driver->name); // 2.6 kernel porting

	if (dev == NULL) {
		return -ENODEV;
	}

	tty->driver_data = (void *)dev;
	tty->low_latency = 1;
	dev->vs_dev.tty = tty;

	MULTIPDP_LOG_INFO("vs_open : %s\n", tty->driver->name); 
	return 0;
}

static void vs_close(struct tty_struct *tty, struct file *filp)
{
	// TODO..
	struct pdp_info *dev;
	dev = pdp_get_serdev(tty->driver->name); // 2.6 kernel porting

	if (dev == NULL) {
		return -ENODEV;
	}
	dev->vs_dev.tty = NULL;	

	MULTIPDP_LOG_INFO("vs_close : %s\n", tty->driver->name); 
}
#endif

static int vs_open(struct tty_struct *tty, struct file *filp)
{
	struct pdp_info *dev;
	int rcnt;

	dev = pdp_get_serdev(tty->driver->name); // 2.6 kernel porting
	if (dev == NULL) {
		return -ENODEV;
	}

	rcnt = atomic_read(&dev->vs_dev.refcount);
/*
	if (rcnt > 0){
	    	MULTIPDP_LOG_INFO("[%s] %s, refcount: %d \n", __func__, tty->driver->name, rcnt);
		return -ENODEV;
	}
*/
	tty->driver_data = (void *)dev;
	tty->low_latency = 1;
	dev->vs_dev.tty = tty;
	MULTIPDP_LOG_INFO("[%s] %s \n", __func__, tty->driver->name);
	atomic_inc(&dev->vs_dev.refcount);
	return 0;
}

static void vs_close(struct tty_struct *tty, struct file *filp)
{
	struct pdp_info *dev;
	int rcnt;

	dev = pdp_get_serdev(tty->driver->name); 
	if (!dev )
		return;
	rcnt = atomic_read(&dev->vs_dev.refcount);
	if (rcnt == 0){
		MULTIPDP_LOG_INFO("[%s] %s: Nothing to close\n", __func__, tty->driver->name);
		return;
	}
	atomic_dec(&dev->vs_dev.refcount);
	if (rcnt == 1)		//This means dev->vs_dev.refcount is now 0
		dev->vs_dev.tty = NULL;	
	
	MULTIPDP_LOG_INFO("[%s] %s: Closed \n", __func__, tty->driver->name, dev->vs_dev.refcount);
	// TODO..
	return;
}

static int vs_write(struct tty_struct *tty,
		const unsigned char *buf, int count)
{
	int ret;
	struct pdp_info *dev = (struct pdp_info *)tty->driver_data;

	ret = pdp_mux(dev, buf, count);

	if (ret == 0) {
		ret = count;
	}

	return ret;
}

static int vs_write_room(struct tty_struct *tty) 
{
//	return TTY_FLIPBUF_SIZE;
	return 8192*2;
}

static int vs_chars_in_buffer(struct tty_struct *tty) 
{
	return 0;
}

static int vs_read(struct pdp_info *dev, size_t len, int devtype)
{
	int ret = 0;
	int ref_cnt;

	if (!dev) {
		return 0;
	}
	MULTIPDP_LOG_INFO("Entered %s \n", __FUNCTION__);
	if(len > 1500) {
		unsigned char *prx_buf = kzalloc(len, GFP_ATOMIC);

		if(prx_buf == NULL)
			return 0;
		if(devtype == DPRAM_DEV )
		{
			MULTIPDP_LOG_INFO("vs_read ==> reading from DPRAM_DEV\n");
			ret = multipdp_read(dpram_filp, prx_buf, len);
		}
		else if(devtype == ONEDRAM_DEV )
		{
			MULTIPDP_LOG_INFO("vs_read ==> reading from ONEDRAM_DEV\n");
			ret = multipdp_read(onedram_filp, prx_buf, len);
		}
		MULTIPDP_LOG_INFO("vs_read ==> multipdp_read read %d\n", ret);
		if(ret != len)
			return ret;
		
		ref_cnt = atomic_read(&dev->vs_dev.refcount);
		if ((dev->vs_dev.tty == NULL) || (ref_cnt == 0)){
			MULTIPDP_LOG_ERR("vs_read ==> dev->vs_dev.tty == NULL\n");
		}
		else {
			if (ret > 0) {
				ret = tty_insert_flip_string(dev->vs_dev.tty, prx_buf, ret);
				tty_flip_buffer_push(dev->vs_dev.tty);
			}
		}
		kfree(prx_buf);
	}
	else {

		/* pdp data length.. */
		if(devtype == DPRAM_DEV )
		{		
			MULTIPDP_LOG_INFO("vs_read ==> reading from DPRAM_DEV\n");
			ret = multipdp_read(dpram_filp, pdp_rx_buf, len);
		}
		else if(devtype == ONEDRAM_DEV )
		{
			MULTIPDP_LOG_INFO("vs_read ==> reading from ONEDRAM_DEV\n");
			ret = multipdp_read(onedram_filp, pdp_rx_buf, len);
		}
		MULTIPDP_LOG_INFO("vs_read ==> multipdp_read read %d\n", ret);

		if (ret != len) {
			return ret;
		}

#ifdef LOOP_BACK_TEST
		if (dev->id == LOOP_BACK_CHANNEL) {
			// compare and resend , update stastic data
			//printk("receive loopback packet[%d]\n",loopback_res.nTransfered);
			//printk("read data : %x %x %x %x %x %x\n",pdp_rx_buf[0],pdp_rx_buf[1],pdp_rx_buf[2],pdp_rx_buf[3],pdp_rx_buf[4],pdp_rx_buf[5]);
			//printk("write data : %x %x %x %x %x %x\n",loopback_data[0],loopback_data[1],loopback_data[2],loopback_data[3],loopback_data[4],loopback_data[5]);
			if (loopback_ongoing) {
				if (strncmp(pdp_rx_buf, loopback_data, loopback_res.nPacketDataSize)){
					//printk("receive packet is not identical to that sent\n");
				}
				else {
					send_loop_back_packet(loopback_data, loopback_res.nPacketDataSize);
				}
			}
			else {
				//do nothing
				//printk("loopback channel has gotten data, but test is no ongoing\n");
			}
			
		}
		else if (ret > 0 && dev->vs_dev.tty != NULL) {	//hobac.
			MULTIPDP_LOG_INFO("%s tty push \n", __FUNCTION__);
			tty_insert_flip_string(dev->vs_dev.tty, pdp_rx_buf, ret);
			MULTIPDP_LOG_INFO("%s tty written data size %d \n", __FUNCTION__,ret);
			tty_flip_buffer_push(dev->vs_dev.tty);
		}
#else
		ref_cnt = atomic_read(&dev->vs_dev.refcount);
		if ((dev->vs_dev.tty == NULL) || (ref_cnt == 0)){
			MULTIPDP_LOG_INFO("vs_read : #2 vs_dev.tty is NULL n");
		}
		else{
			if (ret > 0) {	//hobac.
				tty_insert_flip_string(dev->vs_dev.tty, pdp_rx_buf, ret);
				tty_flip_buffer_push(dev->vs_dev.tty);
			}
		}

		
#endif		
	}
	return 0;
}

static int vs_ioctl(struct tty_struct *tty, struct file *file, 
		    unsigned int cmd, unsigned long arg)
{
	return -ENOIOCTLCMD;
}

static void vs_break_ctl(struct tty_struct *tty, int break_state)
{
}

static struct tty_operations multipdp_tty_ops = {
	.open 		= vs_open,
	.close 		= vs_close,
	.write 		= vs_write,
	.write_room = vs_write_room,
	.ioctl 		= vs_ioctl,
	.chars_in_buffer = vs_chars_in_buffer,
//	.break_ctl	= vs_break_ctl,

	/* TODO: add more operations */
};

static int vs_add_dev(struct pdp_info *dev)
{
	struct tty_driver *tty_driver;

	switch (dev->id) {
		case 7:
			tty_driver = &dev->vs_dev.tty_driver[0];
			tty_driver->minor_start = CSD_MINOR_NUM;
			break;

		case 8:
			tty_driver = &dev->vs_dev.tty_driver[1];
			tty_driver->minor_start = 1;
			break;

		case 5:
			tty_driver = &dev->vs_dev.tty_driver[2];
			tty_driver->minor_start = 2;
			break;

		case 6:
			tty_driver = &dev->vs_dev.tty_driver[3];
			tty_driver->minor_start = 3;
			break;

		case 25:
			tty_driver = &dev->vs_dev.tty_driver[4];
			tty_driver->minor_start = 4;
			break;

		case 30:
			tty_driver = &dev->vs_dev.tty_driver[5];
			tty_driver->minor_start = 5;
			break;

        //skumar@wtl for ETS over DPRAM
		case 9:
			tty_driver = &dev->vs_dev.tty_driver[6];
			tty_driver->minor_start = 6;
			break;

		case 16:
			tty_driver = &dev->vs_dev.tty_driver[7];
			tty_driver->minor_start = 7;
			break;

		case 28:
			tty_driver = &dev->vs_dev.tty_driver[8];
			tty_driver->minor_start = 8;
			break;

		case 17:
			tty_driver = &dev->vs_dev.tty_driver[9];
			tty_driver->minor_start = 9;
			break;

#ifdef LOOP_BACK_TEST
		case 31:
			tty_driver = &dev->vs_dev.tty_driver[10];
			tty_driver->minor_start = 10;
			break;
#endif

		default:
			tty_driver = NULL;
	}

	if (!tty_driver) {
		MULTIPDP_LOG_ERR("vs_add_dev ==> tty driver is NULL!\n");
		return -1;
	}

	#if 1 //hobac.
	kref_init(&tty_driver->kref);
	#endif

	tty_driver->magic	= TTY_DRIVER_MAGIC;
	tty_driver->driver_name	= "multipdp";
	tty_driver->name	= dev->vs_dev.tty_name;
	tty_driver->major	= CSD_MAJOR_NUM;
//	tty_driver->minor_start = CSD_MINOR_NUM;
	tty_driver->num		= 1;
	tty_driver->type	= TTY_DRIVER_TYPE_SERIAL;
	tty_driver->subtype	= SERIAL_TYPE_NORMAL;
	tty_driver->flags	= TTY_DRIVER_REAL_RAW;
	tty_driver->ttys	= dev->vs_dev.tty_table; // 2.6 kernel porting
	tty_driver->termios	= dev->vs_dev.termios;
	tty_driver->termios_locked	= dev->vs_dev.termios_locked;
#if 0
	tty_driver->init_termios.c_cflag = CS8 | CREAD | HUPCL | CLOCAL | B115200;
	tty_driver->init_termios.c_iflag = IGNBRK | IGNPAR;
	tty_driver->init_termios.c_lflag = 0;
	tty_driver->init_termios.c_oflag = 0;
	tty_driver->init_termios.c_cc[VMIN] = 1;
	tty_driver->init_termios.c_cc[VTIME] = 1;
#endif
//	tty_driver->init_termios = tty_std_termios;

	tty_set_operations(tty_driver, &multipdp_tty_ops);
	return tty_register_driver(tty_driver);
}

static void vs_del_dev(struct pdp_info *dev)
{
	struct tty_driver *tty_driver = NULL;

	switch (dev->id) {
		case 7:
			tty_driver = &dev->vs_dev.tty_driver[0];
			break;

		case 8:
			tty_driver = &dev->vs_dev.tty_driver[1];
			break;

		case 5:
			tty_driver = &dev->vs_dev.tty_driver[2];
			break;

		case 6:
			tty_driver = &dev->vs_dev.tty_driver[3];
			break;

        //skumar@wtl for ETS over DPRAM
		case 9:
			tty_driver = &dev->vs_dev.tty_driver[6];
			break;

		case 16:
			tty_driver = &dev->vs_dev.tty_driver[7];
			break;

		case 28:
			tty_driver = &dev->vs_dev.tty_driver[8];
			break;

		case 17:
			tty_driver = &dev->vs_dev.tty_driver[9];
			break;
	}

	tty_unregister_driver(tty_driver);
}

/*
 * PDP context and mux/demux functions
 */

static inline struct pdp_info * pdp_get_dev(u8 id)
{
	int slot;

	for (slot = 0; slot < MAX_PDP_CONTEXT; slot++) {
		if (pdp_table[slot] && pdp_table[slot]->id == id) {
			return pdp_table[slot];
		}
	}
	return NULL;
}

static inline struct pdp_info * pdp_get_serdev(const char *name)
{
	int slot;
	struct pdp_info *dev;

	for (slot = 0; slot < MAX_PDP_CONTEXT; slot++) {
		dev = pdp_table[slot];
		if (dev && dev->type == DEV_TYPE_SERIAL &&
		    strcmp(name, dev->vs_dev.tty_name) == 0) {
			return dev;
		}
	}
	return NULL;
}

static inline int pdp_add_dev(struct pdp_info *dev)
{
	int slot;

	if (pdp_get_dev(dev->id)) {
		return -EBUSY;
	}

	for (slot = 0; slot < MAX_PDP_CONTEXT; slot++) {
		if (pdp_table[slot] == NULL) {
			pdp_table[slot] = dev;
			return slot;
		}
	}
	return -ENOSPC;
}

static inline struct pdp_info * pdp_remove_dev(u8 id)
{
	int slot;
	struct pdp_info *dev;

	for (slot = 0; slot < MAX_PDP_CONTEXT; slot++) {
		if (pdp_table[slot] && pdp_table[slot]->id == id) {
			dev = pdp_table[slot];
			pdp_table[slot] = NULL;
			return dev;
		}
	}
	return NULL;
}

static inline struct pdp_info * pdp_remove_slot(int slot)
{
	struct pdp_info *dev;

	dev = pdp_table[slot];
	pdp_table[slot] = NULL;
	return dev;
}

static int pdp_mux(struct pdp_info *dev, const void *data, size_t len   )
{
	int ret;
	size_t nbytes;
	u8 *tx_buf;
	struct pdp_hdr_onedram *hdr_onedram;
	struct pdp_hdr_dpram *hdr_dpram;
	u8 curr_dev;
		
	const u8 *buf;
#ifdef PROFILE_ENABLE
	log_fwd_sniff_info(SNIFF_POINT_ID_PDP_MUX, data, len);
#endif

	// Only reading the value of dev->intf_dev so that we don't 
	// write one hdr now and write some other data later.
	tx_buf = dev->tx_buf;
	if(atomic_read(&dev->intf_dev) == DPRAM_DEV)	
	{
	   curr_dev = DPRAM_DEV;
	   hdr_dpram = (struct pdp_hdr_dpram*)(tx_buf + 1);
	   hdr_dpram->id = dev->id;
	   hdr_dpram->control = 0;
	}
	else if(atomic_read(&dev->intf_dev) == ONEDRAM_DEV)	
	{
		curr_dev = ONEDRAM_DEV;
		hdr_onedram = (struct pdp_hdr_onedram*)(tx_buf + 1);
		hdr_onedram->id = dev->id;
	    hdr_onedram->control = 0;
	}
	else
	{
		curr_dev = UNKNOWN_DEV;
		MULTIPDP_LOG_ERR("pdp_mux => Unknown physical interface for device. Not Dpram/OneDram \n");
		return(-1);	
	}

	buf = data;

	MULTIPDP_LOG_INFO("pdp_mux => mux the data \n");

	while (len) {

		if (len > MAX_PDP_DATA_LEN) 
		{
			nbytes = MAX_PDP_DATA_LEN;
		} 
		else 
		{
			nbytes = len;
		}

		if(curr_dev == DPRAM_DEV)
		{
			hdr_dpram->len = nbytes + sizeof(struct pdp_hdr_dpram);
			tx_buf[0] = 0x7f;
			memcpy(tx_buf + 1 + sizeof(struct pdp_hdr_dpram), buf, nbytes);
			tx_buf[1 + hdr_dpram->len] = 0x7e;
#ifdef PRINT_WRITE_INFO	
			MULTIPDP_LOG_WRITE("WRITE hdr_dpram->id: %d, hdr->len: %d\n", hdr_dpram->id, hdr_dpram->len);
			MULTIPDP_LOG_WRITE("WRITE Writing data to dpram device\n");
#endif
			ret = multipdp_write(dpram_filp, tx_buf, hdr_dpram->len + 2, 
				  dev->type == DEV_TYPE_NET ? 1 : 0);
			if (ret < 0) {
				MULTIPDP_LOG_INFO("multipdp_write() failed for DPRAM_DEV with ret: %d\n", ret);
				return ret;
			}
		}
		else if(curr_dev == ONEDRAM_DEV)
		{
			hdr_onedram->len = nbytes + sizeof(struct pdp_hdr_onedram);
			tx_buf[0] = 0x7f;
			memcpy(tx_buf + 1 + sizeof(struct pdp_hdr_onedram), buf,  nbytes);
			tx_buf[1 + hdr_onedram->len] = 0x7e;
#ifdef PRINT_WRITE_INFO	
			MULTIPDP_LOG_WRITE("WRITE hdr_onedram->id: %d, hdr_onedram->len: %d\n", hdr_onedram->id, hdr_onedram->len);
			MULTIPDP_LOG_WRITE("WRITE Writing data to onedram device\n");
#endif
			ret = multipdp_write(onedram_filp, tx_buf, hdr_onedram->len + 2, 
					  dev->type == DEV_TYPE_NET ? 1 : 0);
			if (ret < 0) {
				MULTIPDP_LOG_ERR("multipdp_write() failed for ONEDRAM_DEV with ret: %d\n", ret);
				return ret;
			}

		}
	
		buf += nbytes;
		len -= nbytes;
	}

	return 0;
}

static int pdp_demux(struct file *filp,int devtype)
{
	int ret;
	u8 ch;
	size_t len;
	struct pdp_info *dev = NULL;
	//struct pdp_hdr hdr;
	struct pdp_hdr_onedram hdr_onedram;
	struct pdp_hdr_dpram hdr_dpram;
#ifdef PROFILE_ENABLE
	log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_PDPDEMUX_START, NULL,0,0);
#endif

	MULTIPDP_LOG_INFO("In pdp_demux\n");

	/* read header */
	if(devtype == ONEDRAM_DEV)
	{
		ret = multipdp_read(filp, &hdr_onedram, sizeof(struct pdp_hdr_onedram));
		if (ret < 0) {
			return ret;
		}

		len = hdr_onedram.len - sizeof(struct pdp_hdr_onedram);

		/* check header */
		down(&pdp_lock);
		dev = pdp_get_dev(hdr_onedram.id);
#ifdef PRINT_READ_INFO
		MULTIPDP_LOG_READ("READ hdr_onedram.id: %d, hdr_onedram.len: %d, len: %d\n", hdr_onedram.id, hdr_onedram.len, len);
#endif
		if (dev == NULL) {
			MULTIPDP_LOG_ERR("hdr_onedram.id : %u, there is no existing device.\n", hdr_onedram.id);
			ret = -ENODEV;
			goto err;
		}
	}
	else
	{
		ret = multipdp_read(filp, &hdr_dpram, sizeof(struct pdp_hdr_dpram));
		if (ret < 0) {
			return ret;
		}

		len = hdr_dpram.len - sizeof(struct pdp_hdr_dpram);

		/* check header */
		down(&pdp_lock);
		dev = pdp_get_dev(hdr_dpram.id);
#ifdef PRINT_READ_INFO
		MULTIPDP_LOG_READ("READ hdr_dpram.id: %d, hdr_dpram.len: %d, len: %d\n", hdr_dpram.id, hdr_dpram.len, len);
#endif
		if (dev == NULL) {
			MULTIPDP_LOG_ERR("hdr_dpram.id: %u, there is no existing device.\n", hdr_dpram.id);
			ret = -ENODEV;
			goto err;
		}
	}

	/* read data */
	switch (dev->type) {
		case DEV_TYPE_NET:
			ret = vnet_recv(dev, len, devtype );
			break;
		case DEV_TYPE_SERIAL:
			ret = vs_read(dev, len ,devtype);
			break;
		default:
			ret = -1;
	}

	if (ret < 0) {
		goto err;
	}
	up(&pdp_lock);

	/* check stop byte */
	ret = multipdp_read(filp, &ch, sizeof(ch));

	if (ret < 0 || ch != 0x7e) {
		return ret;
	}
#ifdef PROFILE_ENABLE
	log_rev_sniff_info(SNIFF_POINT_ID_MULTIPDP_PDPDEMUX_END,NULL,0,0);
#endif

	return 0;
err:
	up(&pdp_lock);

	/* flush the remaining data including stop byte. */
	multipdp_flush_rx(filp, len + 1);
	return ret;
}

static int pdp_activate(pdp_arg_t *pdp_arg, unsigned type, unsigned char intf_dev, unsigned flags )
{
	int ret;
	struct pdp_info *dev;
	struct net_device *net;
	MULTIPDP_LOG_INFO(" %s \n",__FUNCTION__);
	MULTIPDP_LOG_INFO("pdp_activate ==> id: %d\n", pdp_arg->id);

	dev = vmalloc(sizeof(struct pdp_info) + MAX_PDP_PACKET_LEN);
	if (dev == NULL) {
		MULTIPDP_LOG_ERR("pdp_activate ==> vmalloc failed, out of memory\n");
		return -ENOMEM;
	}
	memset(dev, 0, sizeof(struct pdp_info));

	/* @LDK@ added by gykim on 20070203 for adjusting IPC 3.0 spec. */
	if (type == DEV_TYPE_NET) {
		dev->id = pdp_arg->id + g_adjust;
	}

	else {
		dev->id = pdp_arg->id;
		atomic_set(&dev->vs_dev.refcount, 0);
	}
	/* @LDK@ added by gykim on 20070203 for adjusting IPC 3.0 spec. */
	atomic_set(&dev->intf_dev, intf_dev);
	dev->type = type;
	dev->flags = flags;
	dev->tx_buf = (u8 *)(dev + 1);

	if (type == DEV_TYPE_NET) {

		MULTIPDP_LOG_INFO("calling vnet_add_dev\n");
#ifdef MULTIPDP_IPV6
		// Added kushagra for ipv6 checking
		if(pdp_arg->en_interface_type == IPADDR_TYPE_IPV4)
			net = vnet_add_dev((void *)dev);
		else if((pdp_arg->en_interface_type == IPADDR_TYPE_IPV6) || (pdp_arg->en_interface_type == IPADDR_TYPE_IPV4V6))
			net = vnet_add_dev_v6((void *)dev);
		// Added kushagra for ipv6 checking
#else
		net = vnet_add_dev((void *)dev);
#endif
		if (net == NULL) {
			vfree(dev);
			return -ENOMEM;
		}

		dev->vn_dev.netq_init = 0;
		dev->vn_dev.netq_active = g_datastatus;
		init_MUTEX(&dev->vn_dev.netq_sem);

		dev->vn_dev.net = net;
		strcpy(pdp_arg->ifname, net->name);

		down(&pdp_lock);
		ret = pdp_add_dev(dev);
		if (ret < 0) {
			MULTIPDP_LOG_ERR("pdp_activate ==> pdp_add_dev() failed\n");
			up(&pdp_lock);
			vnet_del_dev(dev->vn_dev.net);
			vfree(dev);
			return ret;
		}
		up(&pdp_lock);
#ifdef MULTIPDP_IPV6
        	memcpy(net->int_iden, pdp_arg->int_iden, 8);
#endif
		MULTIPDP_LOG_INFO("%s(id: %u) network device created\n", net->name, dev->id);
	} else if (type == DEV_TYPE_SERIAL) {
		init_MUTEX(&dev->vs_dev.write_lock);
		strcpy(dev->vs_dev.tty_name, pdp_arg->ifname);

		ret = vs_add_dev(dev);
		if (ret < 0) {
			vfree(dev);
			return ret;
		}

		down(&pdp_lock);
		ret = pdp_add_dev(dev);
		if (ret < 0) {
			MULTIPDP_LOG_ERR("pdp_activate ==> pdp_add_dev() failed\n");
			up(&pdp_lock);
			vs_del_dev(dev);
			vfree(dev);
			return ret;
		}
		up(&pdp_lock);

		if (dev->id == 7) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[0].name, dev->id);
		}

		else if (dev->id == 8) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[1].name, dev->id);
		}
        //skumar@wtl for ttyETS (ETS over DPRAM)
		else if (dev->id == 9) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[6].name, dev->id);
		}
		else if (dev->id == 16) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[7].name, dev->id);
		}
		else if (dev->id == 28) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[8].name, dev->id);
		}
		else if (dev->id == 17) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[9].name, dev->id);
		}
#ifdef LOOP_BACK_TEST
		else if (dev->id == 31) {
			MULTIPDP_LOG_INFO("%s(id: %u) serial device is created.\n", dev->vs_dev.tty_driver[10].name, dev->id);
		}
#endif
	}
	return 0;
}

static int pdp_deactivate(pdp_arg_t *pdp_arg, int force)
{
	struct pdp_info *dev = NULL;

	MULTIPDP_LOG_INFO("pdp_deactivate ==> id: %d\n", pdp_arg->id);

	down(&pdp_lock);

	if (pdp_arg->id == 1) {
		MULTIPDP_LOG_WARN("Channel ID is 1, we will remove the network device (pdp) of channel ID: %d.\n",
				pdp_arg->id + g_adjust);
	}

	else {
		MULTIPDP_LOG_INFO("pdp_deactivate ==> Channel ID: %d\n", pdp_arg->id);
	}

	pdp_arg->id = pdp_arg->id + g_adjust;
	//pdp_arg->id += PDP_ID_ADJUST;
	MULTIPDP_LOG_INFO("pdp_deactivate ==> ID is adjusted, new ID: %d\n", pdp_arg->id);

	dev = pdp_get_dev(pdp_arg->id);

	if (dev == NULL) {
		MULTIPDP_LOG_ERR("pdp_deactivate ==> error not found id: %u\n", pdp_arg->id);
		up(&pdp_lock);
		return -EINVAL;
	}
	if (!force && dev->flags & DEV_FLAG_STICKY) {
		MULTIPDP_LOG_ERR("pdp_deactivate ==> sticky id: %u\n", pdp_arg->id);
		up(&pdp_lock);
		return -EACCES;
	}

	pdp_remove_dev(pdp_arg->id);
	up(&pdp_lock);

	if (dev->type == DEV_TYPE_NET) {
		MULTIPDP_LOG_WARN("%s(id: %u) network device removed\n", 
			dev->vn_dev.net->name, dev->id);
		vnet_del_dev(dev->vn_dev.net);
	} else if (dev->type == DEV_TYPE_SERIAL) {
		if (dev->id == 1) {
			MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
					dev->vs_dev.tty_driver[0].name, dev->id);
		}

		else if (dev->id == 8) {
			MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
					dev->vs_dev.tty_driver[1].name, dev->id);
		}

		else if (dev->id == 16) {
			MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
					dev->vs_dev.tty_driver[7].name, dev->id);
		}

		else if (dev->id == 28) {
			MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
					dev->vs_dev.tty_driver[8].name, dev->id);
		}

		else if (dev->id == 17) {
			MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
					dev->vs_dev.tty_driver[9].name, dev->id);
		}

		vs_del_dev(dev);
	}

	 vfree(dev);

	return 0;
}

static void __exit pdp_cleanup(void)
{
	int slot;
	struct pdp_info *dev;

	down(&pdp_lock);
	for (slot = 0; slot < MAX_PDP_CONTEXT; slot++) {
		dev = pdp_remove_slot(slot);
		if (dev) {
			if (dev->type == DEV_TYPE_NET) {
				MULTIPDP_LOG_WARN("%s(id: %u) network device removed\n", 
					dev->vn_dev.net->name, dev->id);
				vnet_del_dev(dev->vn_dev.net);
			} else if (dev->type == DEV_TYPE_SERIAL) {
				if (dev->id == 1) {
					MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n", 
							dev->vs_dev.tty_driver[0].name, dev->id);
				}

				else if (dev->id == 8) {
					MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
							dev->vs_dev.tty_driver[1].name, dev->id);
				}

				else if (dev->id == 16) {
					MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
							dev->vs_dev.tty_driver[7].name, dev->id);
				}

				else if (dev->id == 28) {
					MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
							dev->vs_dev.tty_driver[8].name, dev->id);
				}

				else if (dev->id == 17) {
					MULTIPDP_LOG_WARN("%s(id: %u) serial device removed\n",
							dev->vs_dev.tty_driver[9].name, dev->id);
				}

				vs_del_dev(dev);
			}
			vfree(dev);
		}
	}
	up(&pdp_lock);
}

static int pdp_adjust(const int adjust)
{
	g_adjust = adjust;
	MULTIPDP_LOG_INFO("pdp_adjust ==> adjusting value: %d\n", adjust);
	return 0;
}

static int pdp_setradiotype(const int radio)
{
	struct pdp_info *dev;
	int loopcount;	
	g_radio = radio;

	MULTIPDP_LOG_INFO("pdp_setradiotype  = %d \n ",g_radio);
	down(&pdp_lock);
	for (loopcount = 0; loopcount < MAX_PDP_CONTEXT; loopcount++) {
		dev = pdp_table[loopcount];
		if (dev!=NULL) 
		    {
		     	 if(dev->type == DEV_TYPE_NET)
		     	 {
					 if (g_radio == LTE)
					 	atomic_set(&dev->intf_dev, ONEDRAM_DEV);
					 else
					 	atomic_set(&dev->intf_dev, DPRAM_DEV);
				 }
			}
	}
	up(&pdp_lock);



	return 0;
}
static int pdp_datastatus(const int datastatus)
{
	struct pdp_info *dev;
	int loopcount;	
	
	

	down(&pdp_lock);
	for (loopcount = 0; loopcount < MAX_PDP_CONTEXT; loopcount++) {
		dev = pdp_table[loopcount];
		if ((dev!=NULL) && (dev->type == DEV_TYPE_NET)){
			down(&dev->vn_dev.netq_sem);
			if (SUSPEND == datastatus){
				if (dev->vn_dev.netq_init != 0){
					netif_stop_queue(dev->vn_dev.net);						
				}
				dev->vn_dev.netq_active = SUSPEND;
			}
			else{
				if ((dev->vn_dev.netq_init != 0) && (SUSPEND == dev->vn_dev.netq_active)){
					netif_wake_queue(dev->vn_dev.net);						
				}
				dev->vn_dev.netq_active = ACTIVE;
			}
			up(&dev->vn_dev.netq_sem);
		}
	}
	g_datastatus = datastatus;		//Not sure if we even need this global variable.
	MULTIPDP_LOG_INFO("pdp_datastatus = %d \n ",g_datastatus);
	up(&pdp_lock);
	
	return 0;
}

/*
 * App. Interfece Device functions
 */

static int multipdp_ioctl(struct inode *inode, struct file *file, 
			      unsigned int cmd, unsigned char *arg)
{
	int ret, adjust, radio, datastatus;
	pdp_arg_t pdp_arg;
	int loop = 0;

	switch (cmd) {
	case HN_PDP_ACTIVATE:
		{
			if (copy_from_user(&pdp_arg, (void *)arg, sizeof(pdp_arg)))
			return -EFAULT;
			if(g_radio == LTE)
			{
				ret = pdp_activate(&pdp_arg, DEV_TYPE_NET,ONEDRAM_DEV, 0);
			}
			else
			{
				ret = pdp_activate(&pdp_arg, DEV_TYPE_NET,DPRAM_DEV, 0);

			}
		if (ret < 0) {
			return ret;
		}
		}
		return copy_to_user((void *)arg, &pdp_arg, sizeof(pdp_arg));

	case HN_PDP_DEACTIVATE:
		if (copy_from_user(&pdp_arg, (void *)arg, sizeof(pdp_arg)))
			return -EFAULT;
		return pdp_deactivate(&pdp_arg, 0);

	case HN_PDP_ADJUST:
		if (copy_from_user(&adjust, (void *)arg, sizeof (int)))
			return -EFAULT;
		return pdp_adjust(adjust);

	case HN_PDP_SETRADIO:
			if (copy_from_user(&radio, (void *)arg, sizeof (int)))
			return -EFAULT;
		return pdp_setradiotype(radio);
	case HN_PDP_DATASTATUS:
			if (copy_from_user(&datastatus, (void *)arg, sizeof (int)))
				return -EFAULT;
			return pdp_datastatus(datastatus);
	case HN_PDP_TXSTART:
		pdp_tx_flag = 0;
		return 0;

	case HN_PDP_TXSTOP:
		pdp_tx_flag = 1;
		return 0;

	}

	return -EINVAL;
}

static struct file_operations multipdp_fops = {
	.owner =	THIS_MODULE,
	.ioctl =	multipdp_ioctl,
	.llseek =	no_llseek,
};

static struct miscdevice multipdp_dev = {
	.minor =	132, //MISC_DYNAMIC_MINOR,
	.name =		APP_DEVNAME,
	.fops =		&multipdp_fops,
};

/*
 * /proc fs interface
 */

#ifdef CONFIG_PROC_FS
static int multipdp_proc_read(char *page, char **start, off_t off,
			      int count, int *eof, void *data)
{

	char *p = page;
	int len;

	down(&pdp_lock);

	p += sprintf(p, "modified multipdp driver on 20070205");
	for (len = 0; len < MAX_PDP_CONTEXT; len++) {
		struct pdp_info *dev = pdp_table[len];
		if (!dev) continue;

		p += sprintf(p,
			     "name: %s\t, id: %-3u, type: %-7s, flags: 0x%04x\n",
			     dev->type == DEV_TYPE_NET ? 
			     dev->vn_dev.net->name : dev->vs_dev.tty_name,
			     dev->id, 
			     dev->type == DEV_TYPE_NET ? "network" : "serial",
			     dev->flags);
	}
	up(&pdp_lock);

	len = (p - page) - off;
	if (len < 0)
		len = 0;

	*eof = (len <= count) ? 1 : 0;
	*start = page + off;

	return len;
}
#endif

/*
 * Module init/clanup functions
 */

static int __init multipdp_init(void)
{
	int ret;
	MULTIPDP_LOG_WARN("multipdp_init back is enabled \n");
	pdp_arg_t pdp_arg = { .id = 7, .ifname = "ttyCDMA", };
	pdp_arg_t efs_arg = { .id = 8, .ifname = "ttyEFS", };
	pdp_arg_t gps_arg = { .id = 5, .ifname = "ttyGPS", };
	pdp_arg_t xtra_arg = { .id = 6, .ifname = "ttyXTRA", };
	pdp_arg_t smd_arg = { .id = 25, .ifname = "ttySMD", };
	pdp_arg_t pcm_arg = { .id = 30, .ifname = "ttyPCM", } ;
	pdp_arg_t net_arg = { .id = 1, .ifname = "pdp", } ;	//for network interface
	pdp_arg_t scrn_arg = { .id = 16, .ifname = "ttySCRN", } ;	// for TRFB
	pdp_arg_t debug_arg = { .id = 28, .ifname = "ttyDEBUG", } ;
	pdp_arg_t atchnl_arg = { .id = 17, .ifname = "ttyCSD", } ; // sbaby@wtl - for AT command channel
#ifdef LOOP_BACK_TEST	
	pdp_arg_t loopback_arg = { .id = 31, .ifname = "ttyLOBK", };
#endif
    //skumar@wtl for ETS over DPRAM
	pdp_arg_t ets_arg = { .id = 9, .ifname = "ttyETS", };

//	printk(" Loop back is enabled%s line %d\n",__FUNCTION__,__LINE__);
	/* run onedram I/O thread */
	
	ret = kernel_thread(poll_thread, NULL, CLONE_FS | CLONE_FILES);
	MULTIPDP_LOG_INFO("kernel thread called\n");
	if (ret < 0) {
		MULTIPDP_LOG_ERR("kernel_thread() call failed\n");
		return ret;
	}
	MULTIPDP_LOG_INFO("wait for completion\n");
	wait_for_completion(&poll_complete);
	MULTIPDP_LOG_INFO("wait completed\n");
//TODO:
// 
//	MULTIPDP_LOG_INFO("%s onedram file flags", __FUNCTION__,onedram_filp->f_flags);	
	
	if (!poll_task) {
		MULTIPDP_LOG_ERR("onedram I/O thread error\n");
		return -EIO;
	}

	ret = pdp_activate(&pdp_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for ttyCDMA\n");
		goto err0;
	}
    //skumar@wtl for ETS over DPRAM
	ret = pdp_activate(&ets_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for ttyETS\n");
		goto err0_1;
	}

	ret = pdp_activate(&scrn_arg, DEV_TYPE_SERIAL, DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for ttySCRN\n");
		goto err0_5;
	}
	ret = pdp_activate(&debug_arg, DEV_TYPE_SERIAL, DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for ttyDEBUG\n");
		goto err0_7;
	}
	ret = pdp_activate(&atchnl_arg, DEV_TYPE_SERIAL, DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for ttyCSD\n");
		goto err0_6;
	}

#if 0	/* create serial device for Circuit Switched Data */
	ret = pdp_activate(&pdp_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for CSD\n");
		goto err0;
	}

	ret = pdp_activate(&efs_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for EFS\n");
		goto err1;
	}

	ret = pdp_activate(&gps_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for GPS\n");
		goto err2;
	}

	ret = pdp_activate(&xtra_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for XTRA\n");
		goto err3;
	}
	
	ret = pdp_activate(&smd_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for SMD\n");
		goto err4;
	}

	ret = pdp_activate(&pcm_arg, DEV_TYPE_SERIAL,DPRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for SMD\n");
		goto err5;
	}

#ifdef LOOP_BACK_TEST	
	
	MULTIPDP_LOG_INFO(" Loop back is enabled%s line %d\n",__FUNCTION__,__LINE__);
	ret = pdp_activate(&loopback_arg, DEV_TYPE_SERIAL, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for LoopBack\n");
		goto err6;
	}
#endif
#endif

#ifdef __MULTIPDP_TEST__

	MULTIPDP_LOG_INFO("network interface is enabled%s \n",__FUNCTION__);
	ret = pdp_activate(&net_arg, DEV_TYPE_NET,ONEDRAM_DEV, DEV_FLAG_STICKY);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("failed to create a serial device for LoopBack\n");
		goto err6;
	}

#endif	

	/* create app. interface device */  
	ret = misc_register(&multipdp_dev);
	if (ret < 0) {
		MULTIPDP_LOG_ERR("misc_register() failed\n");
		goto err7;
	}


#ifdef LOOP_BACK_TEST
	ret = device_create_file(multipdp_dev.this_device, &dev_attr_loopback);
#endif	

#ifdef ENABLE_MULTIPDP_DEBUG_PRINTS
	register_debug_control_attribute();
#endif

#ifdef CONFIG_PROC_FS
	create_proc_read_entry(APP_DEVNAME, 0, 0, 
			       multipdp_proc_read, NULL);
#endif

//	printk(KERN_INFO 
//	       "$Id: multipdp.c,v 1.10 2008/01/11 05:40:56 melonzz Exp $\n");
	return 0;
err7:
	pdp_deactivate(&net_arg,1);
err6:
#ifdef LOOP_BACK_TEST	
	pdp_deactivate(&loopback_arg, 1);
#endif	
err5:
#if	0
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&pcm_arg, 1);

err4:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&smd_arg, 1);

err3:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&xtra_arg, 1);
err2:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&gps_arg, 1);
#endif	/* 0 */
err0_6:
	/* undo serial device for ttyCSD */
	pdp_deactivate(&atchnl_arg, 1);
err1:
	/* undo serial device for TRFB */
	pdp_deactivate(&scrn_arg, 1);
err0_5:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&ets_arg, 1);
err0_1:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&pdp_arg, 1);
err0_7:
	/* undo serial device for Circuit Switched Data */
	pdp_deactivate(&debug_arg, 1);
err0:
	/* kill onedram I/O thread */
	if (poll_task) {
		send_sig(SIGUSR1, poll_task, 1);
		wait_for_completion(&poll_complete);
	}
	return ret;
}

static void __exit multipdp_exit(void)
{
#ifdef CONFIG_PROC_FS
	remove_proc_entry(APP_DEVNAME, 0);
#endif

#ifdef ENABLE_MULTIPDP_DEBUG_PRINTS
	deregister_debug_control_attribute();
#endif

	/* remove app. interface device */
	misc_deregister(&multipdp_dev);

	/* clean up PDP context table */
	pdp_cleanup();

	/* kill onedram I/O thread */
	if (poll_task) {
		send_sig(SIGUSR1, poll_task, 1);
		wait_for_completion(&poll_complete);
	}
}

#ifdef ENABLE_MULTIPDP_DEBUG_PRINTS
static ssize_t mulitpdp_store_debug_level(struct device *dev, struct device_attribute *attr, 
										  const char *buf, size_t len)
{
	u16 value;
	char *after;
	value = simple_strtoul(buf, &after, 10);	
	printk("mulitpdp_store_debug_level, value = %x\n", value);
	mulitpdp_debug_mask = value;
	mulitpdp_debug_mask |= (MULTIPDP_PRINT_ERROR | MULTIPDP_PRINT_WARNING); // ERROR/WARN are always enabled
	printk("mulitpdp_debug_mask = %x\n", mulitpdp_debug_mask);
	return 1;
}
static ssize_t mulitpdp_show_debug_level(struct device *dev, struct device_attribute *attr, char *buf)
{
	printk("mulitpdp_show_debug_level, mulitpdp_debug_mask %x\n", mulitpdp_debug_mask);
	return snprintf(buf, PAGE_SIZE, "%d\n", mulitpdp_debug_mask);
}

static DEVICE_ATTR(debug_ctrl_attr, S_IRUGO | S_IWUSR, mulitpdp_show_debug_level, mulitpdp_store_debug_level);
void register_debug_control_attribute(void)
{
    return device_create_file(multipdp_dev.this_device, &dev_attr_debug_ctrl_attr);

}
void deregister_debug_control_attribute(void)
{
	device_remove_file(multipdp_dev.this_device, &dev_attr_debug_ctrl_attr);
}
void multipdp_debug_print(u32 mask,  const char *fmt, ...)
{
	if (mulitpdp_debug_mask & mask)
	{
		static char s_buf[1024];
		va_list args;

		va_start(args, fmt);
		vsprintf(s_buf, fmt, args);
		va_end(args);

		printk("[MULTIPDP]");
		printk(s_buf); 
	}
}
#endif /*ENABLE_MULTIPDP_DEBUG_PRINTS*/

//module_init(multipdp_init);
late_initcall(multipdp_init);
module_exit(multipdp_exit);

MODULE_AUTHOR("SAMSUNG ELECTRONICS CO., LTD");
MODULE_DESCRIPTION("Multiple PDP Muxer / Demuxer");
MODULE_LICENSE("GPL");

