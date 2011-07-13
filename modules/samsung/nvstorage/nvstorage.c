#include <linux/init.h>
#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fcntl.h>

#include <asm/uaccess.h>
#include <linux/fsr/FSR.h>
#include <linux/fsr/FSR_BML.h>
#include <linux/fsr/FSR_LLD_4K_OneNAND.h>
#include <linux/vmalloc.h>
#include <linux/delay.h>

#include "nvstorage.h"

struct nv_index_tab_struct {
	unsigned int jobid;
	unsigned int logbpn;
	unsigned int logb_num;
	unsigned int magic;
};

struct nv_index_struct {
	unsigned int magic;
	unsigned int pad[3];	/* reserved */
	struct nv_index_tab_struct itab[NV_INDEX_NUM];	
};

struct log_blk_struct {
	unsigned int magic;
	unsigned int logid;
	unsigned int job_page_num;
	loff_t offs;
	size_t size;
	char data[0];
};

static struct nv_struct {
	unsigned int spp; /* start page of partition */
	unsigned int sbp; /* start block of partition */

	unsigned int jobid;
	unsigned int logbpn;
	unsigned int logb_num;
	unsigned int index_blk;
	int index_num;

	char *blk_buf;

    unsigned int is_init;
    unsigned int ref_count;
} g_blk_nv;

extern FSRPartI   *fsr_get_part_spec(u32 volume);
/*
 * Help Functions
 */
static inline int _blk_nv_read_pgs(unsigned int nVpn, unsigned int nNumOfPgs, unsigned char *pMBuf)
{
	struct nv_struct *nv = &g_blk_nv;

	LOG("read page(0x%x) num(0x%x)", nVpn+nv->spp, nNumOfPgs);
	return FSR_BML_Read(0, nVpn+nv->spp, nNumOfPgs, pMBuf, NULL, FSR_BML_FLAG_ECC_OFF );
}

static inline int _blk_nv_write_pgs(unsigned int nVpn, unsigned int nNumOfPgs, unsigned char *pMBuf)
{
	struct nv_struct *nv = &g_blk_nv;

	LOG("write page(0x%x) num(0x%x)", nVpn+nv->spp, nNumOfPgs);
	return  FSR_BML_Write(0, nVpn+nv->spp, nNumOfPgs, pMBuf, NULL, FSR_BML_FLAG_ECC_OFF);
}

static inline int _blk_nv_erase(unsigned int nVun, unsigned int nNumOfUnits)
{
	struct nv_struct *nv = &g_blk_nv;
	unsigned int unit_num = nv->sbp + nVun;

	LOG("unit_num:%d, nNumofUnit:%d", unit_num, nNumOfUnits);
	return FSR_BML_Erase(0, &unit_num, nNumOfUnits, FSR_BML_FLAG_NONE);
}

/*
 * major functions
 */
static int _load_log_block(struct nv_struct *nv_structs)
{
	struct nv_struct *nv = nv_structs;
	struct nv_index_struct *idx_block;
	struct log_blk_struct *logb;
	int ret=0;
	unsigned int i_blk=0, i_tab=0;
	unsigned int* buf;

	if(nv->blk_buf == NULL){
		LOGE("no block buffer\n");
		ret =-1;
		goto CleanUp;
	}
	idx_block = (struct nv_index_struct *)nv->blk_buf;
	/* find current index block -- 0th or 1st */
	for(i_blk=NV_INDEX_BLOCK_START; i_blk<NV_LOG_BLOCK_START; i_blk++)
	{
		memset(idx_block, 0, NV_PAGE_SIZE);
		ret = _blk_nv_read_pgs((BLOCK2PAGE(i_blk+1)-1), 1, (unsigned char*)idx_block);
		if(ret !=  FSR_BML_SUCCESS)	{
			LOGE("BML error 10(0x%x)\n", ret);
			goto CleanUp;
		}
		if(idx_block->magic == NV_IDXBLK_MAGIC) {
			break;
		}
	}
	/* find current index table */
	for(i_tab=0;i_tab<NV_INDEX_NUM;i_tab++){
		if(idx_block->itab[i_tab].magic != NV_IDXBLK_MAGIC)
			break;
	}
	LOGA("idx found index_blk(%d), index_num(%d), jobid(0x%x) logbpn(0x%x) \n", 
		i_blk, i_tab-1, idx_block->itab[i_tab-1].jobid, idx_block->itab[i_tab-1].logbpn);

	if(i_blk >= NV_LOG_BLOCK_START){	/* can't find valid index */
		LOGE("Can't find the log block\n");
		ret = _blk_nv_erase(NV_INDEX_BLOCK_START, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error 20(0x%x)\n", ret);
			goto CleanUp;
		}
		ret = _blk_nv_erase(NV_LOG_BLOCK_START, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error 21(0x%x)\n", ret);
			goto CleanUp;
		}

		nv->index_blk = NV_INDEX_BLOCK_START;
		nv->index_num = -1;
		nv->logbpn = NV_LOG_PAGE_START;
		nv->logb_num = 0;
		nv->jobid = 0;
		ret = NV_META_INITIALIZE;
		goto CleanUp;
	} else {
		nv->index_blk = i_blk;
		nv->index_num = i_tab-1; /* index_num is next wirte index table id */
		nv->logbpn = idx_block->itab[i_tab-1].logbpn;
		nv->logb_num = idx_block->itab[i_tab-1].logb_num;
		nv->jobid = idx_block->itab[i_tab-1].jobid;
	}

	LOGA("logbpn(0x%x), jobid(0x%x), index_blk(0x%x), index_num(0x%x)\n", 
		nv->logbpn, nv->jobid, nv->index_blk, nv->index_num);
	logb = (struct log_blk_struct*)nv->blk_buf;

	/*read next log block for restore */
	ret = _blk_nv_read_pgs(nv->logbpn+nv->logb_num, 1, nv->blk_buf);
	if(ret !=  FSR_BML_SUCCESS)	{
		LOGE("BML error 30(0x%x)\n", ret);
		goto CleanUp;
	}
	buf=(unsigned int *)nv->blk_buf;
	LOGA("[%08d] %08x %08x %08x %08x | %08x %08x %08x %08x\n",
		 nv->logbpn+nv->logb_num, *buf, *(buf+1), *(buf+2), *(buf+3), *(buf+4), *(buf+5), *(buf+6), *(buf+7));
	
	LOGA("next log block page_num(0x%x) magic=(0x%x) jobid=(0x%x)\n",
		nv->logbpn+nv->logb_num,logb->magic, logb->logid );
	if(logb->magic == NV_LOGBLK_MAGIC /*&& idx_block->jobid+1 == logb->logid*/) {
		LOGE("need to restore\n");
		nv->logbpn +=1;
		ret = NV_NEED_RESTORE;
		/*find the free log blocks*/
		goto CleanUp;
	}
	ret = NV_LOAD_SUCCESS;

	LOGA("Logb num=%d,  jobid(0x%x)\n", nv->logbpn,  nv->jobid);
	
CleanUp:
	return ret;
	
}

static int _save_index_page(struct nv_struct *nv_structs)
{
	struct nv_struct *nv = nv_structs;
	struct nv_index_struct *idx_page =(struct nv_index_struct *)nv->blk_buf;
	int erase_blk=-1;
	int ret=0;
	
	nv->index_num++;
	memset(nv->blk_buf, 0xFF, NV_PAGE_SIZE);
	if(nv->index_num == NV_INDEX_NUM-1){
		erase_blk = (int)nv->index_blk;
		nv->index_blk = (nv->index_blk+1)%NV_IDXBLK_NUM + NV_INDEX_BLOCK_START;
		nv->index_num = 0;		
	}
	idx_page->magic = NV_IDXBLK_MAGIC;
	idx_page->itab[nv->index_num].jobid = nv->jobid;
	idx_page->itab[nv->index_num].logbpn = nv->logbpn;
	idx_page->itab[nv->index_num].logb_num = nv->logb_num;
	idx_page->itab[nv->index_num].magic = NV_IDXBLK_MAGIC;

	LOGA("write jobid(0x%x), log page(0x%x), log page num(0x%x), index_blk(%d), index_num(%d)\n",
		nv->jobid, nv->logbpn, nv->logb_num, nv->index_blk, nv->index_num);

	ret = _blk_nv_write_pgs((BLOCK2PAGE(nv->index_blk+1)-1), 1, (unsigned char*)idx_page);
	if(ret !=  FSR_BML_SUCCESS)	{
		LOGE("BML error - save log (0x%x)\n", ret);
		goto CleanUp;
	}
	LOGA("index save success blk(%d) num(%d) \n", nv->index_blk, nv->index_num);
	/* erase next block */
	if(erase_blk>=0){
		LOGA("erase prv block\n");
		ret = _blk_nv_erase(erase_blk, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error 1 (0x%x)\n", ret);
			goto CleanUp;
		}
	}
	
CleanUp:
	return ret;
	
}

static int _save_log_block(struct nv_struct *nv_structs, const char *buf, size_t count, loff_t *f_pos)
{
	struct nv_struct *nv = nv_structs;
	struct log_blk_struct *logblk=(struct log_blk_struct *)nv->blk_buf;
	int page_num = (count+sizeof(struct log_blk_struct)) / NV_PAGE_SIZE +1;
	int ret=0, i, overlap=0, logbpn_write=0, logbpn_write_end=0;

	if(nv->blk_buf == NULL){
		LOGE("no block buffer\n");
		ret =-1;
		goto CleanUp;
	}

	memset(logblk, 0x00, NV_PAGE_SIZE*page_num);
	logblk->magic = NV_LOGBLK_MAGIC;
	logblk->logid = nv->jobid;
	logblk->job_page_num = page_num;
	logblk->size = count;
	logblk->offs = *f_pos;
	copy_from_user(logblk->data, buf, count);

	logbpn_write = nv->logbpn + nv->logb_num;
	logbpn_write_end = logbpn_write + page_num;
	LOG("logbpn 0x%x, write 0x%x\n", nv->logbpn, logbpn_write);
	if(logbpn_write_end >= NV_MAIN_PAGE_START) {
		logbpn_write = NV_LOG_PAGE_START;
		logbpn_write_end = logbpn_write + page_num;
		ret = _blk_nv_erase(NV_LOG_BLOCK_START, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - log first block erase (0x%x)\n", ret);
			goto CleanUp;
		}
	}
	/* erase next block */
	if(PAGE2BLOCK(logbpn_write) != PAGE2BLOCK(logbpn_write_end)){
		LOGA("erase next block blk(%d)\n", PAGE2BLOCK(logbpn_write_end));
		ret = _blk_nv_erase(PAGE2BLOCK(logbpn_write_end), 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error (0x%x)\n", ret);
			goto CleanUp;
		}
	}

	LOG("wpage(0x%x) pages(0x%x)\n", logbpn_write, page_num);
	ret = _blk_nv_write_pgs(logbpn_write, page_num, (unsigned char *)logblk);
	if(ret !=  FSR_BML_SUCCESS)	{
		LOGE("BML error - save log (0x%x)\n", ret);
		goto CleanUp;
	}
	nv->logbpn = logbpn_write;
	nv->logb_num = page_num;
		
CleanUp:
	return ret;
}

int _debug_print_page_header(void)
{
	int i, j, ret;
	struct nv_struct *nv = &g_blk_nv;
	unsigned int *buf;
	
	for(i=NV_INDEX_PAGE_START;i<NV_MAIN_PAGE_START;i++){
		memset(nv->blk_buf, 0XFFFFFFFF, NV_PAGE_SIZE);
		ret = _blk_nv_read_pgs(i, 1, nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS)	{
			LOGE("BML error (0x%x)\n", ret);
			goto CleanUp;
		}
		buf=(unsigned int *)nv->blk_buf;

		LOGA("[p%03d] %08x %08x %08x %08x | %08x %08x %08x %08x\n",
			i, *buf, *(buf+1), *(buf+2), *(buf+3), *(buf+4), *(buf+5), *(buf+6), *(buf+7));
	}
CleanUp:
	return ret;
}


static int blk_nv_read(char *buf, size_t count, loff_t *f_pos)
{
	struct nv_struct *nv = &g_blk_nv;
	unsigned int pn, nop; /* page number, num of pages */
	loff_t soffs;
	int ret=0;
	char *p=buf;

	LOG("buf(0x%x), size(0x%x), offs(0x%x)\n", buf, count, *f_pos);

	if(nv->blk_buf == NULL){
		LOGE("no block buffer\n");
		ret =-1;
		goto CleanUp;
	}
	if(count > NV_BLOCK_SIZE) {
		LOGE("data largger than block size\n");
		ret =-1;
		goto CleanUp;
	}

	soffs = *f_pos % NV_PAGE_SIZE;
	pn = *f_pos / NV_PAGE_SIZE;
	nop = (count+soffs)/NV_PAGE_SIZE +((count+soffs)%NV_PAGE_SIZE?1:0);

	/* Check the block boundary */
	LOGA("start page(0x%x) page num(0x%x)\n", pn+NV_MAIN_PAGE_START, nop);
	ret = _blk_nv_read_pgs(pn+NV_MAIN_PAGE_START, nop, nv->blk_buf);
	if(ret !=  FSR_BML_SUCCESS)	{
		LOGE("BML error (0x%x)\n", ret);
		goto CleanUp;
	}
	p += copy_to_user(p, nv->blk_buf+soffs, count);

	LOG("NV Read Success\n");
	
CleanUp:
	return ret;
}


/* static int _blk_nv_write(const char *buf, size_t count, loff_t *f_pos)
  Flash device write operation
  <1. Read Main block> <2. add the offest data> <3. erase backup block> <4. write data to bakcup> 
  <5. erase main block> <6. write main block>
*/
static int blk_nv_write(const char *buf, size_t count, loff_t *f_pos)
{
	struct nv_struct *nv = &g_blk_nv;
	loff_t offs=*f_pos;
	unsigned char *p=buf;
	int blk_offs, blk_sbn, blk_nob, i, ret;

	LOGA("buf(0x%x), size(0x%x), offs(0x%x)\n", buf, count, *f_pos);
	
	if(count > NV_BLOCK_SIZE) {
		LOGE("data largger than block size\n");
		ret =-1;
		goto CleanUp;
	}
		
	if(nv->blk_buf == NULL){
		LOGE("no block buffer\n");
		ret =-1;
		goto CleanUp;
	}
	
	nv->jobid++;
	/* 1. save the data to log block */
	_save_log_block(nv, buf, count, f_pos);
/*case 1 - only logging data*/
/*	LOGE("wait 5sec - loged\n");
	msleep(5000);
	LOGE("wait done\n");
*/
	blk_offs = offs%NV_BLOCK_SIZE;
	blk_sbn = offs/NV_BLOCK_SIZE;
	blk_nob = (blk_offs + count)/NV_BLOCK_SIZE + ((blk_offs + count)%NV_BLOCK_SIZE?1:0);

	LOGA("blk_offs(0x%x), blk_sbn(0x%x), blk_nob(0x%x)\n", blk_offs, blk_sbn, blk_nob);

	for(i=0;i<blk_nob;i++){

		/*read old block to buf*/
		memset(nv->blk_buf, 0x00, NV_BLOCK_SIZE);
		ret = _blk_nv_read_pgs(BLOCK2PAGE(blk_sbn+i)+NV_MAIN_PAGE_START,
			NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - read main block (0x%x)\n", ret);
			goto CleanUp;
		}

		/*copy to buffer*/
		if(i==0)
			p += copy_from_user(nv->blk_buf + blk_offs, p, MIN(count, NV_BLOCK_SIZE-blk_offs) );
		else
			p += copy_from_user(nv->blk_buf, p,  MIN(count, NV_BLOCK_SIZE));

		/*Erase write BACK-UP block*/
		ret = _blk_nv_erase(blk_sbn+i + NV_BACKUP_BLOCK_START, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - erase backup block (0x%x)\n", ret);
			goto CleanUp;
		}
/*case 2 - backup block was erased*/
/*		LOGE("wait 5sec - backup erased\n");
		msleep(5000);
		LOGE("wait done\n");
*/
		ret = _blk_nv_write_pgs(BLOCK2PAGE(blk_sbn+i)+NV_BACKUP_PAGE_START, 
			NV_BLK_PAGE_NUM, nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - write backup block (0x%x)\n", ret);
			goto CleanUp;
		}
		/*Erase write MAIN block*/	
		ret = _blk_nv_erase(blk_sbn+i + NV_MAIN_BLOCK_START, 1);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - erase main block (0x%x)\n", ret);
			goto CleanUp;
		}
/*case 3 - main block was erased*/
/*		LOGE("wait 5sec - main erased\n");
		msleep(5000);
		LOGE("wait done\n");
*/
		ret = _blk_nv_write_pgs(BLOCK2PAGE(blk_sbn+i)+NV_MAIN_PAGE_START, 
			NV_BLK_PAGE_NUM, nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - write main block (0x%x)\n", ret);
			goto CleanUp;
		}
	}
	_save_index_page(nv);

	//_debug_print_page_header();
	
	LOGA("NV Write Sucess\n");
CleanUp:
	return ret;
}


/* restore case 
 1. backup block erase
 1-2. backup block writing
 3. main block erase
 3-2. main block writing
*/
int block_nv_restore(struct nv_struct *nv_structs)
{
	struct nv_struct *nv = nv_structs;
	struct log_blk_struct *logb;
	int blk_offs, blk_sbn, blk_nob, ret;
	unsigned int backup_page_head, main_page_head, *buf, blk_hd, blk_tail;

	logb=(struct log_blk_struct *)vmalloc(NV_PAGE_SIZE*(nv->logb_num));
	if(logb == NULL){
		LOGE("get block buffer fail\n");
		ret= -1;
		goto CleanUp;
	}
	memset(nv->blk_buf, 0x00, NV_BLOCK_SIZE);
	ret = _blk_nv_read_pgs(nv->logbpn, nv->logb_num, logb);
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error (0x%x)\n", ret);
		goto CleanUp;
	}
	buf=(unsigned int *)logb->data;
	LOGA("[p%03d] %08x %08x %08x %08x | %08x %08x %08x %08x\n",
		nv->logbpn, *buf, *(buf+1), *(buf+2), *(buf+3), *(buf+4), *(buf+5), *(buf+6), *(buf+7));	
	
	/* Check the logging data validate...*/
	if(*((unsigned int*)(logb->data + logb->size))==0xFFFFFFFF){
		/*drop resetore,, devices was reset while writing log data*/
		LOGE("NV Restore is Fail,, drop restore logging page's last data is 0xFFFF_FFFFF \n");
		goto CleanUp;
	}
 	blk_offs = logb->offs%NV_BLOCK_SIZE;
	blk_sbn = logb->offs/NV_BLOCK_SIZE;
	blk_nob = (blk_offs + logb->size)/NV_BLOCK_SIZE + (blk_offs?1:0);

	/* load first page of backup block */
	ret = _blk_nv_read_pgs(BLOCK2PAGE(blk_sbn)+NV_BACKUP_PAGE_START, 1, nv->blk_buf); // first page
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error (0x%x)\n", ret);
		goto CleanUp;
	}
	blk_hd = *((unsigned int *)nv->blk_buf);
	/* load last page of backup block */
	ret = _blk_nv_read_pgs(BLOCK2PAGE(blk_sbn+1)+NV_BACKUP_PAGE_START-1, 1, nv->blk_buf); // last page
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error (0x%x)\n", ret);
		goto CleanUp;
	}
	blk_tail = *((unsigned int *)nv->blk_buf);
	
	if(blk_hd==0xFFFFFFFF || blk_tail==0xFFFFFFFF){
		/* load main block */
		LOGA("Backup page was broken, restore main pages\n");
		ret = _blk_nv_read_pgs(BLOCK2PAGE(blk_sbn)+NV_MAIN_PAGE_START,
			NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - read main block (0x%x)\n", ret);
			goto CleanUp;
		}
	}
	else {
		/* load backup block */
		LOGA("Backup page load\n");
		ret = _blk_nv_read_pgs(BLOCK2PAGE(blk_sbn)+NV_BACKUP_PAGE_START,
			NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
		if(ret !=  FSR_BML_SUCCESS){
			LOGE("BML error - read main block (0x%x)\n", ret);
			goto CleanUp;
		}
	}
	/* copy the logged data to backup data buf */
	memcpy(nv->blk_buf+blk_offs, logb->data, logb->size);
	ret = _blk_nv_erase(blk_sbn + NV_BACKUP_BLOCK_START, 1);
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error - erase backup block (0x%x)\n", ret);
		goto CleanUp;
	}
	ret = _blk_nv_write_pgs(BLOCK2PAGE(blk_sbn)+NV_BACKUP_PAGE_START, 
		NV_BLK_PAGE_NUM, nv->blk_buf);
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error - write backup block (0x%x)\n", ret);
		goto CleanUp;
	}
	ret = _blk_nv_erase(blk_sbn + NV_MAIN_BLOCK_START, 1);
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error - erase main block (0x%x)\n", ret);
		goto CleanUp;
	}
	ret = _blk_nv_write_pgs(BLOCK2PAGE(blk_sbn)+NV_MAIN_PAGE_START, 
		NV_BLK_PAGE_NUM, nv->blk_buf);
	if(ret !=  FSR_BML_SUCCESS){
		LOGE("BML error - write main block (0x%x)\n", ret);
		goto CleanUp;
	}
	_save_index_page(nv);
	
CleanUp:

	LOGA("CleanUp!!!\n");
/* block buffer will be released when driver close function was called.
	if(logb)
		vfree(logb);
*/	
}

static int _blk_init(struct nv_struct *nv_structs)
{
	int ret=0, i=0;
	FSRPartI *pi;
	struct nv_struct *nv = nv_structs;

	ret = FSR_BML_Open(0, FSR_BML_FLAG_NONE);
	if (ret != FSR_BML_SUCCESS) {
		LOGE("No such device or address(0x%x)", ret);
	}
	pi = fsr_get_part_spec(0);
	if(pi == NULL) {
		LOGE("fail get partition table\n");
		ret =-1;
		goto CleanUp;
	}
	/* find the EFS2 Partition with Partition ID13	*/
	for(i=0;i<NV_MAX_FLASH_PARTITIONS;i++){
		if(pi->stPEntry[i].nID==NV_PARTI_ID){
			nv->sbp = pi->stPEntry[i].n1stVun;
			nv->spp = BLOCK2PAGE(nv->sbp);
			LOGA("find EFS2 Partition with Partition ID(0x%x)\n block Start(0x%x), Page Start(0x%x)\n"
			,pi->stPEntry[i].nID, nv->sbp, nv->spp);
			break;
		}
	}
	if(i>= NV_MAX_FLASH_PARTITIONS){
		LOGE("No EFS2 Partition!!\n");
		ret = -1;
		goto CleanUp;
	}

	/* get block buffer */
	nv->blk_buf=vmalloc(NV_BLOCK_SIZE);
	if(nv->blk_buf == NULL){
		LOGE("get block buffer fail\n");
		ret= -1;
		goto CleanUp;
	}
	ret = _load_log_block(nv);
	switch(ret){
		case NV_LOAD_SUCCESS:
			LOGA("load logdate\n");
			break;
		case NV_NEED_RESTORE:
			block_nv_restore(nv);
			LOGA("nv restore done\n");
			break;
		case NV_META_INITIALIZE:
			LOGA("nv initalized\n");
			break;
	}

CleanUp: 
	return ret;
}

static int _blk_deinit(struct nv_struct *nv_structs)
{
	struct nv_struct *nv = nv_structs;
	
	if(nv->blk_buf)
		vfree(nv->blk_buf);

	return 0;
}

/*
* Driver Interface functions
*/

int nv_open(struct inode *inode, struct file *filp)
{
	int num = MINOR(inode->i_rdev);
	struct nv_struct *nv = &g_blk_nv;
    int ret;

	LOGA("NV Device open Minor : %d, ref_count : %d\n", num, nv->ref_count);

    if (!nv->is_init) {
        ret = _blk_init(nv);
        if (ret < 0)
            return -1;

        nv->is_init = true;
    }

    nv->ref_count++;

	return 0;
}

int nv_close(struct inode *inode, struct file *filp)
{
	struct nv_struct *nv = &g_blk_nv;

    nv->ref_count--;

    if (nv->ref_count == 0) {
	    _blk_deinit(nv);
        nv->is_init = false;
    }

	LOGA("NV Device Close. ref_count : %d\n", nv->ref_count);

	return 0;
}

ssize_t nv_read(struct file *filp, char *buf, size_t count, loff_t *f_pos)
{
	return blk_nv_read(buf, count, f_pos);
}

ssize_t nv_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos)
{
	return blk_nv_write(buf, count, f_pos);
}

#define NV_ERASE_BLOCK		0xF0a1
#define NV_READ_BLOCK 			0xF0a2
#define NV_WRITE_BLOCK		0xF0a3
#define NV_READ_DATA_BLOCK 	0xF0a4
#define NV_WRITE_DATA_BLOCK	0xF0a5

#define NV_BLOCK_FIRST		0xF0a6
#define NV_ERASE_ALL		0xF0a7
#define NV_NOISY_BLOCK	0xF0a8
#define NV_INITIALIZED	'ined'
#define NV_INIT_MAGIC	'GIRE'

struct nv_ioctl_arg_struct {
	unsigned int blknum;
	char * buf;
};

int nv_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int ret=0, i=0;//PREVENT fixed
	unsigned int temp, *out_int;
	unsigned char *outbuf;
	struct nv_struct *nv = &g_blk_nv;
	struct nv_ioctl_arg_struct *blkinfo;

	LOGA(" NV ioctl -> cmd : %08X, arg : %08X \n", cmd, arg);
	switch (cmd) {
		case NV_READ_BLOCK:
			blkinfo = (struct nv_ioctl_arg_struct *)arg;
			LOGA("num(0x%x) addr(0x%x)\n", blkinfo->blknum, nv->blk_buf);
			ret = _blk_nv_read_pgs(BLOCK2PAGE(blkinfo->blknum),	NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			copy_to_user(blkinfo->buf, nv->blk_buf, NV_BLOCK_SIZE);
			break;

		case NV_ERASE_BLOCK:
			blkinfo = (struct nv_ioctl_arg_struct *)arg;
			ret = _blk_nv_erase(blkinfo->blknum, 1);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			break;
			
		case NV_WRITE_BLOCK:
			blkinfo = (struct nv_ioctl_arg_struct *)arg;
			LOGA("num(0x%x) addr(0x%x)\n", blkinfo->blknum, nv->blk_buf);
			copy_from_user(nv->blk_buf, blkinfo->buf, NV_BLOCK_SIZE);
			ret = _blk_nv_erase(blkinfo->blknum, 1);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			ret = _blk_nv_write_pgs(BLOCK2PAGE(blkinfo->blknum), NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			break;

		case NV_READ_DATA_BLOCK:
			blkinfo = (struct nv_ioctl_arg_struct *)arg;
			LOGA("num(0x%x) addr(0x%x)\n", blkinfo->blknum+NV_MAIN_BLOCK_START, nv->blk_buf);
			ret = _blk_nv_read_pgs(BLOCK2PAGE(blkinfo->blknum)+NV_MAIN_PAGE_START,	NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			copy_to_user(blkinfo->buf, nv->blk_buf, NV_BLOCK_SIZE);
			break;

		case NV_WRITE_DATA_BLOCK:
			blkinfo = (struct nv_ioctl_arg_struct *)arg;
			LOGA("num(0x%x) addr(0x%x)\n", blkinfo->blknum, nv->blk_buf);
			copy_from_user(nv->blk_buf, blkinfo->buf, NV_BLOCK_SIZE);

			ret = _blk_nv_erase(blkinfo->blknum+NV_MAIN_BLOCK_START, 1);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			ret = _blk_nv_write_pgs(BLOCK2PAGE(blkinfo->blknum)+NV_MAIN_PAGE_START, NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			ret = _blk_nv_erase(blkinfo->blknum+NV_BACKUP_BLOCK_START, 1);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			ret = _blk_nv_write_pgs(BLOCK2PAGE(blkinfo->blknum)+NV_BACKUP_PAGE_START, NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			break;

		case NV_BLOCK_FIRST:
			out_int = (unsigned int*)arg;
			temp = NV_INITIALIZED;
			ret = _blk_nv_read_pgs(NV_MAIN_PAGE_START, 1, (unsigned char *)nv->blk_buf);
			if(ret !=  FSR_BML_SUCCESS){
				LOGE("BML error (0x%x)\n", ret);
				goto CleanUp;
			}
			LOGA("## NV Magic  0x%x -(if 0x47495245 is ok!)\n", *((unsigned int *)nv->blk_buf) );
			if(*((unsigned int *)nv->blk_buf) != NV_INIT_MAGIC ) {
				temp = 0xFFFFFFFF;
			}
			LOGA("check index head=(0x%x)\n", temp);
			*out_int = temp;
			break;

		case NV_ERASE_ALL:
			for(i=0;i<12;i++){
				ret = _blk_nv_erase(NV_INDEX_BLOCK_START+i, 1);
				if(ret !=  FSR_BML_SUCCESS){
					LOGE("BML error (0x%x)\n", ret);
					goto CleanUp;
				}
			}
			break;
			
		case NV_NOISY_BLOCK:
			memset(nv->blk_buf, 0x5A5A5A5A, NV_BLOCK_SIZE);
			for(i=0;i<8;i++){
				ret = _blk_nv_erase(4+i, 1);
				if(ret !=  FSR_BML_SUCCESS){
					LOGE("BML error (0x%x)\n", ret);
					goto CleanUp;
				}
				ret = _blk_nv_write_pgs(BLOCK2PAGE(4+i), NV_BLK_PAGE_NUM, (unsigned char *)nv->blk_buf);
				if(ret !=  FSR_BML_SUCCESS){
					LOGE("BML error (0x%x)\n", ret);
					goto CleanUp;
				}			
			}
			break;

		default:
			break;
	}

CleanUp:
	return ret;
}

static struct file_operations nv_fops = 
{
	.owner   = THIS_MODULE,
	.open    = nv_open,
	.release = nv_close,
	.read    = nv_read,
	.write   = nv_write,
	.ioctl   = nv_ioctl,
};

static struct miscdevice nv_device = {
	.minor  = NV_MINOR,		/* 136 */
	.name   = NV_NAME,		/* "nvs" */
	.fops   = &nv_fops,
	.mode   = 0x1b0,
};

/* init & cleanup. */
static int __init nv_init(void)
{
	int result;

	LOGA("nv device init\n");

	result = misc_register(&nv_device);
	if (result <0) 
		return result;

	return 0;
}

static void __exit nv_exit(void)
{
	LOGA("Storage device exit\n");
	misc_deregister(&nv_device);
}

module_init(nv_init);
module_exit(nv_exit);

MODULE_AUTHOR("SAMSUNG ELECTRONICS CO., LTD");
MODULE_DESCRIPTION("BaseBend Remote fs  API Driver.");
MODULE_LICENSE("Samsung Proprietary");
