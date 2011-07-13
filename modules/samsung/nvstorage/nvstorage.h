#ifndef _NVSTORAGE_H_
#define _NVSTORAGE_H_


#define MAX_NV_DATA_SIZE        (512 * 1024)    // 0x80000

#define NV_NAME                 "nvs"
#define NV_MINOR                136

#define NV_PAGE_SIZE            4096
#define NV_BLOCK_SIZE           262144          // 256*1024
#define NV_BLK_PAGE_NUM         64              // (NV_BLOCK_SIZE/NV_PAGE_SIZE)

#define NV_BLK_NUM              (MAX_NV_DATA_SIZE/NV_BLOCK_SIZE)

#define NV_IDXBLK_MAGIC         'idxc'
#define NV_LOGBLK_MAGIC         'logb'

#define NV_IDXBLK_NUM           2
#define NV_LOGBLK_NUM           2
#define NV_BLOCK_NUM            4

#define NV_INDEX_NUM 64

#define BLOCK2PAGE(blk)         ((blk)*NV_BLK_PAGE_NUM)
#define PAGE2BLOCK(pgs)         ((pgs)/NV_BLK_PAGE_NUM)

#define NV_INDEX_BLOCK_START    0
#define NV_LOG_BLOCK_START      (NV_INDEX_BLOCK_START+NV_IDXBLK_NUM)
#define NV_MAIN_BLOCK_START     (NV_LOG_BLOCK_START+NV_LOGBLK_NUM)
#define NV_BACKUP_BLOCK_START   (NV_MAIN_BLOCK_START+NV_BLOCK_NUM)

#define NV_INDEX_PAGE_START     BLOCK2PAGE(NV_INDEX_BLOCK_START)
#define NV_LOG_PAGE_START       BLOCK2PAGE(NV_LOG_BLOCK_START)
#define NV_MAIN_PAGE_START      BLOCK2PAGE(NV_MAIN_BLOCK_START)
#define NV_BACKUP_PAGE_START    BLOCK2PAGE(NV_BACKUP_BLOCK_START)
#define NV_MAX_FLASH_PARTITIONS 20
#define NV_PARTI_ID             13

#define NV_LOAD_SUCCESS         'cont'
#define NV_NEED_RESTORE         'rest'
#define NV_META_INITIALIZE      'init'

#define MIN(a, b)               (a > b)? b: a

#if 0
#define LOGE(s, args...) printk(KERN_ERR "[NV/Err] <%s:%d> " s, __func__, __LINE__, ##args) // Error log
#define LOGA(s, args...) printk(KERN_ERR "[NV] <%s:%d> " s, __func__, __LINE__, ##args)     // Alway printf
#define LOG(s, args...) printk("[NV] <%s:%d> " s, __func__, __LINE__, ##args)
#else
#define LOGE(s, args...) printk(KERN_ERR "[NV/Err] <%s:%d> " s, __func__, __LINE__, ##args) // Error log
#define LOGA(s, args...)
#define LOG(...)
#endif

#endif
