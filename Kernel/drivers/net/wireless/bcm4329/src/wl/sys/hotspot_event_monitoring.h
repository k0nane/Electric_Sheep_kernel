#include <linux/wait.h>
#include <linux/sched.h>

//#define HOTSPOT_MAX_QUEUE_CNT			128
extern int hotspot_event_detect_complete(char *msg);
#if 0
unsigned char hotspot_event_readQ[HOTSPOT_MAX_QUEUE_CNT];
unsigned long hotspot_readQ_cnt = 0;
unsigned long hotspot_readQ_head = 0;
unsigned long hotspot_readQ_tail = 0;
char hotspot_str_buf[HOTSPOT_MAX_QUEUE_CNT];
int hotspot_event_flag;
int hotspot_polling_flag = 0;


DECLARE_WAIT_QUEUE_HEAD(Hotspot_Waitqueue);
#endif