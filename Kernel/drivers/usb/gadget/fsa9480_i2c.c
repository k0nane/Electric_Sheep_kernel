#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/i2c.h>
#include <linux/gpio.h>
#include <plat/pm.h>
#include <asm/irq.h>
#include <linux/delay.h>
#include <plat/gpio-cfg.h>
#include <mach/regs-gpio.h>
#include <mach/regs-clock.h>
#include <mach/param.h>
#include "fsa9480_i2c.h"
#include <linux/syscalls.h>
#include <linux/fcntl.h>
#include <asm/uaccess.h>
#include <linux/usb/composite.h>
#include <linux/usb/gadget.h>
#include <mach/max8998_function.h>
#include <linux/switch.h>


#define CHANGE_UART_TO_CP_ON_BOOT //Change UART to CP automatically on boot up

extern void otg_phy_init(void);
extern void otg_phy_off(void);

extern struct device *switch_dev;
extern int BOOTUP;

int askonstatus;
int inaskonstatus;


#if 1 //20100421_inchul
static int dpram_dump_on = 0;
#endif

#if 1 //20100601_inchul
extern unsigned int HWREV;
#endif

int mtp_mode_on = 0;

#if 1
	#define dmsg(arg,...)	printk("[USB_SWITCH] %s(%d): "arg,__FUNCTION__,__LINE__, ## __VA_ARGS__)
#else
	#define dmsg(arg,...) 	{}
#endif


#define FSA9480_UART 	1
#define FSA9480_USB 	2

#define log_usb_disable 	0
#define log_usb_enable 		1
#define log_usb_active		2
#define _SUPPORT_SAMSUNG_AUTOINSTALLER_

static u8 MicroUSBStatus=0;
static u8 MicroJigUSBOnStatus=0;
static u8 MicroJigUSBOffStatus=0;
static u8 MicroJigUARTOffStatus=0;
static u8 MicroJigUARTOnStatus=0;
u8 MicroTAstatus=0;
int oldusbstatus;


/* switch selection */
#define USB_SEL_MASK  				(1 << 0)
#define UART_SEL_MASK				(1 << 1)
#define USB_SAMSUNG_KIES_MASK		(1 << 2)
#define USB_UMS_MASK				(1 << 3)
#define USB_MTP_MASK				(1 << 4)
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
#define USB_VTP_MASK				(1 << 5)
#endif// ansari_L&T_FROYO_CL534716
#define USB_ASKON_MASK			(1 << 6)
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
#define USB_SAMSUNG_KIES_REAL_MASK	(1 << 10)
#endif
#define USB_SEL_2nd_MASK  			(1 << 11)
#define UART_SEL_2nd_MASK			(1 << 12)

#define DRIVER_NAME  "usb_mass_storage"

struct i2c_driver fsa9480_i2c_driver;
static struct i2c_client *fsa9480_i2c_client = NULL;

struct fsa9480_state {
	struct i2c_client *client;
};

static struct i2c_device_id fsa9480_id[] = {
	{"fsa9480", 0},
	{}
};

#if 1 // 20100630_inchul
extern struct switch_dev switch_dock_detection ;
#endif

static u8 fsa9480_device1 = 0, fsa9480_device2 = 0, fsa9480_adc = 0;
int usb_path = 0;
static int usb_state = 0;
int log_via_usb = log_usb_disable;
static int switch_sel = 0;

static wait_queue_head_t usb_detect_waitq;
static struct workqueue_struct *fsa9480_workqueue;
static struct work_struct fsa9480_work;
struct switch_dev indicator_dev;
struct delayed_work micorusb_init_work;

int currentusbstatus;
byte switchinginitvalue[12];
byte uart_message[6];
byte usb_message[5];
int factoryresetstatus=0;

int mtp_power_off = 0;

void UsbIndicator(u8 state);

extern int usb_mtp_select(int disable);
extern int usb_switch_select(int enable);
extern int askon_switch_select(int enable);
extern unsigned int charging_mode_get(void);

int samsung_kies_mtp_mode_flag;
void FSA9480_Enable_CP_USB(u8 enable);

FSA9480_DEV_TY1_TYPE FSA9480_Get_DEV_TYP1(void)
{
	return fsa9480_device1;
}
EXPORT_SYMBOL(FSA9480_Get_DEV_TYP1);


u8 FSA9480_Get_JIG_Status(void)
{
	if(MicroJigUSBOnStatus | MicroJigUSBOffStatus | MicroJigUARTOffStatus)
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_JIG_Status);


u8 FSA9480_Get_FPM_Status(void)
{
	if(fsa9480_adc == RID_FM_BOOT_ON_UART)
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_FPM_Status);


u8 FSA9480_Get_USB_Status(void)
{
	if( MicroUSBStatus | MicroJigUSBOnStatus | MicroJigUSBOffStatus )
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_USB_Status);


u8 FSA9480_Get_TA_Status(void)
{
	if(MicroTAstatus)
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_TA_Status);

u8 FSA9480_Get_JIG_UART_Status(void)
{
	if(MicroJigUARTOffStatus)
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_JIG_UART_Status);

u8 FSA9480_Get_JIG_OnOff_Status(void)	// hanapark (Factory purpose: Skip battery authentication when JIG cable is inserted.)
{
	if(MicroJigUARTOffStatus || MicroJigUARTOnStatus)
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_JIG_OnOff_Status);

int get_usb_cable_state(void)
{
	return usb_state;
}


static int fsa9480_read(struct i2c_client *client, u8 reg, u8 *data)
{
	int ret;

	ret = i2c_smbus_read_byte_data(client, reg);
	if (ret < 0)
		return -EIO;

	*data = ret & 0xff;
	return 0;
}


static int fsa9480_write(struct i2c_client *client, u8 reg, u8 data)
{
	return i2c_smbus_write_byte_data(client, reg, data);
}

#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
bool IsKiesCurrentUsbStatus(void)
{
	if( (currentusbstatus == USBSTATUS_SAMSUNG_KIES)
	|| (currentusbstatus == USBSTATUS_SAMSUNG_KIES_REAL)
	) {
		return true;
	}

	return false;
}
EXPORT_SYMBOL(IsKiesCurrentUsbStatus);
#endif




void ap_usb_power_on(int set_vaue)
{
 	byte reg_value=0;
	byte reg_address=0x0D;

	if(set_vaue){
		Get_MAX8998_PM_ADDR(reg_address, &reg_value, 1); // read 0x0D register
		reg_value = reg_value | (0x1 << 7);
		Set_MAX8998_PM_ADDR(reg_address,&reg_value,1);
		printk("[ap_usb_power_on]AP USB Power ON, askon: %d, mtp : %d\n",askonstatus,mtp_mode_on);
			if(mtp_mode_on == 1) {
				samsung_kies_mtp_mode_flag = 1;
				printk("************ [ap_usb_power_on] samsung_kies_mtp_mode_flag:%d, mtp:%d\n", samsung_kies_mtp_mode_flag, mtp_mode_on);
			}
			else {
				samsung_kies_mtp_mode_flag = 0;
				printk("!!!!!!!!!!! [ap_usb_power_on]AP samsung_kies_mtp_mode_flag%d, mtp:%d\n",samsung_kies_mtp_mode_flag, mtp_mode_on);
			}
		}
	else{
		Get_MAX8998_PM_ADDR(reg_address, &reg_value, 1); // read 0x0D register
		reg_value = reg_value & ~(0x1 << 7);
		Set_MAX8998_PM_ADDR(reg_address,&reg_value,1);
		printk("[ap_usb_power_on]AP USB Power OFF, askon: %d, mtp : %d\n",askonstatus,mtp_mode_on);
		}
}


void usb_api_set_usb_switch(USB_SWITCH_MODE usb_switch)
{
#if 1//hsjung USB switch is changed after rev 0.7
    if(usb_switch == USB_SW_LTE) // for LTE
    {
        gpio_set_value(GPIO_USB_SEL, GPIO_LEVEL_HIGH);
        FSA9480_Enable_CP_USB(1);
    }
    else
	if(usb_switch == USB_SW_CP)
	{
		//USB_SEL GPIO Set High => CP USB enable
		gpio_set_value(GPIO_USB_SEL, GPIO_LEVEL_LOW);
		FSA9480_Enable_CP_USB(1);
	}
	else
	{
		//USB_SEL GPIO Set Low => AP USB enable
        gpio_set_value(GPIO_USB_SEL, GPIO_LEVEL_LOW);// dont care
		FSA9480_Enable_CP_USB(0);
	}
#else
    if(usb_switch == USB_SW_LTE) // for LTE
    {
        gpio_set_value(GPIO_USB_SEL, GPIO_LEVEL_HIGH);
        FSA9480_Enable_CP_USB(0);
    }else
	if(usb_switch == USB_SW_CP)
	{
		//USB_SEL GPIO Set High => CP USB enable
		FSA9480_Enable_CP_USB(1);
	}
	else
	{
		//USB_SEL GPIO Set Low => AP USB enable
        gpio_set_value(GPIO_USB_SEL, GPIO_LEVEL_LOW);  // for LTE
		FSA9480_Enable_CP_USB(0);
	}
#endif	
}


void Ap_Cp_Switch_Config(u16 ap_cp_mode)
{
   printk(KERN_ERR "Ap_Cp_Switch_Config mode() :%d", ap_cp_mode);

	switch (ap_cp_mode) {
		case AP_USB_MODE:
			usb_path=1;
			usb_api_set_usb_switch(USB_SW_AP);
			break;
		case AP_UART_MODE:
			gpio_set_value(GPIO_UART_SEL, GPIO_LEVEL_HIGH);
			s3c_gpio_slp_cfgpin(GPIO_UART_SEL, S3C_GPIO_SLP_OUT1);
			s3c_gpio_slp_setpull_updown(GPIO_UART_SEL, S3C_GPIO_PULL_NONE);
			break;
		case CP_USB_MODE:
			usb_path=2;
			usb_api_set_usb_switch(USB_SW_CP);
			break;
		case CP_UART_MODE:
			gpio_set_value(GPIO_UART_SEL, GPIO_LEVEL_LOW);
			s3c_gpio_slp_cfgpin(GPIO_UART_SEL, S3C_GPIO_SLP_OUT0);
			s3c_gpio_slp_setpull_updown(GPIO_UART_SEL, S3C_GPIO_PULL_NONE);
			gpio_set_value(GPIO_UART_SEL1, GPIO_LEVEL_LOW);
			s3c_gpio_slp_cfgpin(GPIO_UART_SEL1, S3C_GPIO_SLP_OUT0);
			s3c_gpio_slp_setpull_updown(GPIO_UART_SEL1, S3C_GPIO_PULL_NONE);
			break;
      case LTE_USB_MODE: // for LTE
         usb_path=3;
         usb_api_set_usb_switch(USB_SW_LTE);
         break;
      case LTE_UART_MODE: // for LTE
         gpio_set_value(GPIO_UART_SEL, GPIO_LEVEL_LOW);
         s3c_gpio_slp_cfgpin(GPIO_UART_SEL, S3C_GPIO_SLP_OUT0);
         s3c_gpio_slp_setpull_updown(GPIO_UART_SEL, S3C_GPIO_PULL_NONE);
         gpio_set_value(GPIO_UART_SEL1, GPIO_LEVEL_HIGH);
         s3c_gpio_slp_cfgpin(GPIO_UART_SEL1, S3C_GPIO_SLP_OUT0);
         s3c_gpio_slp_setpull_updown(GPIO_UART_SEL1, S3C_GPIO_PULL_NONE);
         break;
		default:
			printk("Ap_Cp_Switch_Config error");
	}

}

#if 1 //20100421_inchul
u32 set_phone_dump_stat(int state)
{
    dpram_dump_on = state;
}

EXPORT_SYMBOL(set_phone_dump_stat);
#endif



/* MODEM USB_SEL Pin control */
/* 1 : PDA, 2 : MODEM */
#define SWITCH_PDA		1
#define SWITCH_MODEM		2
#define SWITCH_LTE  		3 // for LTE

void usb_switching_value_update(int value)
{
    if(value == SWITCH_LTE) // for LTE
        usb_message[0] = 'L';
    else
	if(value == SWITCH_PDA)
		usb_message[0] = 'A';
	else
		usb_message[0] = 'C';

	usb_message[1] = 'P';
	usb_message[2] = 'U';
	usb_message[3] = 'S';
	usb_message[4] = 'B';

}

void uart_switching_value_update(int value)
{
    if(value == SWITCH_LTE) // LTE
        usb_message[0] = 'L';
    else
	if(value == SWITCH_PDA)
		uart_message[0] = 'A';
	else
		uart_message[0] = 'C';

	uart_message[1] = 'P';
	uart_message[2] = 'U';
	uart_message[3] = 'A';
	uart_message[4] = 'R';
	uart_message[5] = 'T';

}


void switching_value_update(void)
{
	int index;

	for(index=0;index<5;index++)
		switchinginitvalue[index] = usb_message[index];

	for(index=5;index<11;index++)
		switchinginitvalue[index] = uart_message[index-5];

	switchinginitvalue[11] = '\0';

}


static ssize_t factoryreset_value_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
	if(strncmp(buf, "FACTORYRESET", 12) == 0 || strncmp(buf, "factoryreset", 12) == 0)
		factoryresetstatus = 0xAE;

	return size;
}

static DEVICE_ATTR(FactoryResetValue, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, NULL, factoryreset_value_store);


/* for sysfs control (/sys/class/sec/switch/usb_sel) */
static ssize_t usb_sel_show(struct device *dev, struct device_attribute *attr, char *buf)
{
//	struct i2c_client *client = fsa9480_i2c_client;
//	u8 i, pData;
    switch(usb_path)
    {
        case SWITCH_PDA:
            sprintf(buf, "USB Switch : PDA\n");
            break;
        case SWITCH_MODEM:
            sprintf(buf, "USB Switch : MODEM\n");
            break;
        default: //case SWITCH_LTE:
            sprintf(buf, "USB Switch : LTE\n");
            break;
    }
	//sprintf(buf, "USB Switch : %s\n", usb_path==SWITCH_PDA?"PDA":"MODEM");

//    	for(i = 0; i <= 0x14; i++)
//		fsa9480_read(client, i, &pData);

	return sprintf(buf, "%s\n", buf);
}


void usb_switch_mode(int sel)
{
	if (sel == SWITCH_PDA)
	{
		DEBUG_FSA9480("[FSA9480] %s: Path = PDA\n", __func__);
		Ap_Cp_Switch_Config(AP_USB_MODE);
	}
	else if (sel == SWITCH_MODEM)
	{
		DEBUG_FSA9480("[FSA9480] %s: Path = MODEM\n", __func__);
		Ap_Cp_Switch_Config(CP_USB_MODE);
	}
    else if (sel == SWITCH_LTE)
    {
        DEBUG_FSA9480("[FSA9480] %s: Path = LTE\n", __func__);
		Ap_Cp_Switch_Config(LTE_USB_MODE);
    }
	else
		DEBUG_FSA9480("[FSA9480] Invalid mode...\n");
}
EXPORT_SYMBOL(usb_switch_mode);


void microusb_uart_status(int status)
{
	int uart_sel;
	int usb_sel;

	if(!FSA9480_Get_JIG_UART_Status())
		return;

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	uart_sel = (switch_sel & (int)(UART_SEL_MASK)) >> 1;
	usb_sel = switch_sel & (int)(USB_SEL_MASK);

	if(status){
		if(uart_sel)
			Ap_Cp_Switch_Config(AP_UART_MODE);
		else
			Ap_Cp_Switch_Config(CP_UART_MODE);
		}
	else{
		if(!usb_sel)
			Ap_Cp_Switch_Config(AP_USB_MODE);
		}
}


static ssize_t usb_sel_store(
		struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	DEBUG_FSA9480("[FSA9480]%s\n ", __func__);

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	if(strncmp(buf, "PDA", 3) == 0 || strncmp(buf, "pda", 3) == 0)
	{
		usb_switch_mode(SWITCH_PDA);
		usb_switching_value_update(SWITCH_PDA);
		switch_sel |= USB_SEL_MASK;
      switch_sel &= ~USB_SEL_2nd_MASK;
	}

	if(strncmp(buf, "MODEM", 5) == 0 || strncmp(buf, "modem", 5) == 0)
	{
		usb_switch_mode(SWITCH_MODEM);
		usb_switching_value_update(SWITCH_MODEM);
		switch_sel &= ~USB_SEL_MASK;
      switch_sel &= ~USB_SEL_2nd_MASK;
	}

    else if(strncmp(buf, "LTE", 3) == 0 || strncmp(buf, "lte", 3) == 0) {
		usb_switch_mode(SWITCH_LTE);
		usb_switching_value_update(SWITCH_LTE);
      switch_sel &= ~USB_SEL_MASK;
      switch_sel |= USB_SEL_2nd_MASK;
    }
	switching_value_update();

	if (sec_set_param_value)
		sec_set_param_value(__SWITCH_SEL, &switch_sel);

	// microusb_uart_status(0);

	return size;
}

static DEVICE_ATTR(usb_sel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, usb_sel_show, usb_sel_store);


int connectivity_switching_init_state=0;
void usb_switch_state(void)
{
	int usb_sel = 0;
    int usb_2nd_sel = 0;

	if(!connectivity_switching_init_state)
		return;
#if 1 //20100421_inchul
		 if(dpram_dump_on)
		 {
		     usb_switch_mode(SWITCH_MODEM);
		     return;
		 }
#endif

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	usb_sel = switch_sel & (int)(USB_SEL_MASK);
    usb_2nd_sel = switch_sel & (int)(USB_SEL_2nd_MASK);

    if(usb_2nd_sel)
    {
		usb_switch_mode(SWITCH_LTE);
		usb_switching_value_update(SWITCH_LTE);
    }
    else
	if(usb_sel)
	{
		usb_switch_mode(SWITCH_PDA);
		usb_switching_value_update(SWITCH_PDA);
	}
	else
	{
		usb_switch_mode(SWITCH_MODEM);
		usb_switching_value_update(SWITCH_MODEM);
	}
}


void uart_insert_switch_state(void)
{
	int usb_sel = 0;

	if(!connectivity_switching_init_state)
		return;

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	usb_sel = switch_sel & (int)(USB_SEL_MASK);

	if(!usb_sel)
		Ap_Cp_Switch_Config(AP_USB_MODE);
}


void uart_remove_switch_state(void)
{
	int usb_sel = 0;

	if(!connectivity_switching_init_state)
		return;

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	usb_sel = switch_sel & (int)(USB_SEL_MASK);

	if(usb_sel)
		Ap_Cp_Switch_Config(AP_USB_MODE);
	else
		Ap_Cp_Switch_Config(CP_USB_MODE);

}


/**********************************************************************
*    Name         : usb_state_show()
*    Description : for sysfs control (/sys/class/sec/switch/usb_state)
*                        return usb state using fsa9480's device1 and device2 register
*                        this function is used only when NPS want to check the usb cable's state.
*    Parameter   :
*
*
*    Return        : USB cable state's string
*                        USB_STATE_CONFIGURED is returned if usb cable is connected
***********************************************************************/
static ssize_t usb_state_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	int cable_state = get_usb_cable_state();

	sprintf(buf, "%s\n", (cable_state & (CRB_JIG_USB<<8 | CRA_USB<<0 ))?"USB_STATE_CONFIGURED":"USB_STATE_NOTCONFIGURED");

	return sprintf(buf, "%s\n", buf);
}


/**********************************************************************
*    Name         : usb_state_store()
*    Description : for sysfs control (/sys/class/sec/switch/usb_state)
*                        noting to do.
*    Parameter   :
*
*
*    Return        : None
*
***********************************************************************/
static ssize_t usb_state_store(
		struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
	DEBUG_FSA9480("[FSA9480]%s\n ", __func__);
	return 0;
}

/*sysfs for usb cable's state.*/
static DEVICE_ATTR(usb_state, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, usb_state_show, usb_state_store);


static int uart_current_owner = 0;
static ssize_t uart_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if ( uart_current_owner == 2 )
		return sprintf(buf, "%s[UART Switch] Current UART owner = LTE \n", buf);
	else
	if (uart_current_owner)
		return sprintf(buf, "%s[UART Switch] Current UART owner = PDA \n", buf);
	else
		return sprintf(buf, "%s[UART Switch] Current UART owner = MODEM \n", buf);
}

static ssize_t uart_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	if (strncmp(buf, "PDA", 3) == 0 || strncmp(buf, "pda", 3) == 0)	{
		Ap_Cp_Switch_Config(AP_UART_MODE);
		uart_switching_value_update(SWITCH_PDA);
		uart_current_owner = 1;
		switch_sel |= UART_SEL_MASK;
        switch_sel &= ~UART_SEL_2nd_MASK;
		printk("[UART Switch] Path : PDA\n");
	}

	if (strncmp(buf, "MODEM", 5) == 0 || strncmp(buf, "modem", 5) == 0) {
		Ap_Cp_Switch_Config(CP_UART_MODE);
		uart_switching_value_update(SWITCH_MODEM);
		uart_current_owner = 0;
		switch_sel &= ~UART_SEL_MASK;
        switch_sel &= ~UART_SEL_2nd_MASK;
		printk("[UART Switch] Path : MODEM\n");
	}
    if (strncmp(buf, "LTE", 3) == 0 || strncmp(buf, "lte", 3) == 0) {
		Ap_Cp_Switch_Config(LTE_UART_MODE);
		uart_switching_value_update(SWITCH_LTE);
        uart_current_owner = 2;
        switch_sel &= ~UART_SEL_MASK;
        switch_sel |= UART_SEL_2nd_MASK;
        printk("[UART Switch] Path : LTE\n");
    }

	switching_value_update();

	if (sec_set_param_value)
		sec_set_param_value(__SWITCH_SEL, &switch_sel);

	return size;
}

static DEVICE_ATTR(uart_sel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, uart_switch_show, uart_switch_store);


void FSA9480_ChangePathToAudio(u8 enable)
{
	struct i2c_client *client = fsa9480_i2c_client;
	u8 manualsw1;

	if(enable)
	{
		mdelay(10);
		fsa9480_write(client, REGISTER_MANUALSW1, 0x48);

		mdelay(10);
		fsa9480_write(client, REGISTER_CONTROL, 0x1A);

		fsa9480_read(client, REGISTER_MANUALSW1, &manualsw1);
		printk("Fsa9480 ManualSW1 = 0x%x\n",manualsw1);
	}
	else
	{
		mdelay(10);
		fsa9480_write(client, REGISTER_CONTROL, 0x1E);
	}
}
EXPORT_SYMBOL(FSA9480_ChangePathToAudio);


// TODO : need these?
#if 1
static ssize_t DMport_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{

		return sprintf(buf, "%s[UsbMenuSel test] ready!! \n", buf);
}

static ssize_t DMport_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
#if 0
	int fd;
	int i;
	char dm_buf[] = "DM port test sucess\n";
	int old_fs = get_fs();
	set_fs(KERNEL_DS);

		if (strncmp(buf, "DMport", 5) == 0 || strncmp(buf, "dmport", 5) == 0) {

				fd = sys_open("/dev/dm", O_RDWR, 0);
				if(fd < 0){
				printk("Cannot open the file");
				return fd;}
				for(i=0;i<5;i++)
				{
				sys_write(fd,dm_buf,sizeof(dm_buf));
				mdelay(1000);
				}
		sys_close(fd);
		set_fs(old_fs);
		}

		if ((strncmp(buf, "logusb", 6) == 0)||(log_via_usb == log_usb_enable)) {
			log_via_usb = log_usb_active;
				printk("denis_test_prink_log_via_usb_1\n");
				mdelay(1000);
				printk(KERN_INFO"%s: 21143 10baseT link beat good.\n", "denis_test");
			set_fs(old_fs);
			}
		return size;
#else
		if (strncmp(buf, "ENABLE", 6) == 0)
		{
			usb_switch_select(USBSTATUS_DM);
		}

		if (strncmp(buf, "DISABLE", 6) == 0)
		{
			usb_switch_select(USBSTATUS_SAMSUNG_KIES);
		}

	return size;
#endif
}


static DEVICE_ATTR(DMport, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, DMport_switch_show, DMport_switch_store);


//denis
static ssize_t DMlog_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{

		return sprintf(buf, "%s[DMlog test] ready!! \n", buf);
}

static ssize_t DMlog_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
		if (strncmp(buf, "CPONLY", 6) == 0)
		{
			printk("DMlog selection : CPONLY\n");
}

		if (strncmp(buf, "APONLY", 6) == 0)
		{
			printk("DMlog selection : APONLY\n");
		}

		if (strncmp(buf, "CPAP", 4) == 0)
		{
			printk("DMlog selection : AP+CP\n");
		}


	return size;
}


static DEVICE_ATTR(DMlog, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, DMlog_switch_show, DMlog_switch_store);
#endif


static int UsbMenuSelStore(int sel)
{
	int switch_sel=0, ret=0;//PREVENT fixed

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	if(sel == 0){
		switch_sel &= ~(int)USB_UMS_MASK;
		switch_sel &= ~(int)USB_MTP_MASK;
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		switch_sel &= ~(int)USB_VTP_MASK;
#endif// ansari_L&T_FROYO_CL534716
		switch_sel &= ~(int)USB_ASKON_MASK;		
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		switch_sel &= ~(int)USB_SAMSUNG_KIES_REAL_MASK;
#endif
		switch_sel |= (int)USB_SAMSUNG_KIES_MASK;	
		}
	else if(sel == 1){
		switch_sel &= ~(int)USB_UMS_MASK;
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		switch_sel &= ~(int)USB_SAMSUNG_KIES_REAL_MASK;
#endif		
		switch_sel &= ~(int)USB_SAMSUNG_KIES_MASK;
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		switch_sel &= ~(int)USB_VTP_MASK;
#endif// ansari_L&T_FROYO_CL534716
		switch_sel &= ~(int)USB_ASKON_MASK;				
		switch_sel |= (int)USB_MTP_MASK;		
		}
	else if(sel == 2){
		switch_sel &= ~(int)USB_SAMSUNG_KIES_MASK;
		switch_sel &= ~(int)USB_MTP_MASK;
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		switch_sel &= ~(int)USB_VTP_MASK;
#endif// ansari_L&T_FROYO_CL534716
		switch_sel &= ~(int)USB_ASKON_MASK;				
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
	      switch_sel &= ~(int)USB_SAMSUNG_KIES_REAL_MASK;
#endif
		switch_sel |= (int)USB_UMS_MASK;
		}
	else if(sel == 3){
		switch_sel &= ~(int)USB_UMS_MASK;
		switch_sel &= ~(int)USB_MTP_MASK;
		switch_sel &= ~(int)USB_SAMSUNG_KIES_MASK;
		switch_sel &= ~(int)USB_ASKON_MASK;				
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		switch_sel &= ~(int)USB_SAMSUNG_KIES_REAL_MASK;
#endif		
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		switch_sel |= (int)USB_VTP_MASK;	
#endif// ansari_L&T_FROYO_CL534716
		}
	else if(sel == 4){
		switch_sel &= ~(int)USB_UMS_MASK;
		switch_sel &= ~(int)USB_MTP_MASK;
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		switch_sel &= ~(int)USB_VTP_MASK;
#endif// ansari_L&T_FROYO_CL534716
		switch_sel &= ~(int)USB_SAMSUNG_KIES_MASK;				
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		switch_sel &= ~(int)USB_SAMSUNG_KIES_REAL_MASK;
#endif
		switch_sel |= (int)USB_ASKON_MASK;	
		}
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		// KIES_REAL
		else if(sel == 5){
			switch_sel &= ~(int)USB_UMS_MASK;
			switch_sel &= ~(int)USB_MTP_MASK;
#if !defined(CONFIG_TARGET_LOCALE_NTT) // disable tethering xmoondash		
			switch_sel &= ~(int)USB_VTP_MASK;
#endif
			switch_sel &= ~(int)USB_ASKON_MASK; 	
			switch_sel &= ~(int)USB_SAMSUNG_KIES_MASK;	
			switch_sel |= (int)USB_SAMSUNG_KIES_REAL_MASK;	
		}
#endif
	

	if (sec_set_param_value)
		sec_set_param_value(__SWITCH_SEL, &switch_sel);

	printk("\n[WJ] %s, %s, switch_sel=%d\n", __FILE__, __FUNCTION__, switch_sel);

	// returns current USB Mode setting...
	ret = switch_sel;
	ret &= ~(UART_SEL_MASK|USB_SEL_MASK);

	dmsg("ret = 0x%x\n", ret);

	return ret;	
}

void PathSelStore(int sel)
{
	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);
    switch(sel)
    {
    	case AP_USB_MODE: // 10
            switch_sel |= USB_SEL_MASK;
            switch_sel &= ~USB_SEL_2nd_MASK;
            break;
    	case AP_UART_MODE: // 10
            switch_sel |= UART_SEL_MASK;
            switch_sel &= ~UART_SEL_2nd_MASK;
            break;
    	case CP_USB_MODE: // 00
            switch_sel &= ~USB_SEL_MASK;
            switch_sel &= ~USB_SEL_2nd_MASK;
            break;
    	case CP_UART_MODE: // 00
            switch_sel &= ~UART_SEL_MASK;
            switch_sel &= ~UART_SEL_2nd_MASK;
            break;
    	case LTE_USB_MODE: // 01
            switch_sel &= ~USB_SEL_MASK;
            switch_sel |= USB_SEL_2nd_MASK;
            break;
    	case LTE_UART_MODE: // 01
            switch_sel &= ~UART_SEL_MASK;
            switch_sel |= UART_SEL_2nd_MASK;
            break;
    }
#if 0
	if(sel == AP_USB_MODE){
		switch_sel |= USB_SEL_MASK;
		}
	else if(sel == CP_USB_MODE){
		switch_sel &= ~USB_SEL_MASK;
		}
	else if(sel == AP_UART_MODE){
		switch_sel |= UART_SEL_MASK;
		}
	else if(sel == CP_UART_MODE){
		switch_sel &= ~UART_SEL_MASK;
		}
#endif
	if (sec_set_param_value)
		sec_set_param_value(__SWITCH_SEL, &switch_sel);
}


static ssize_t UsbMenuSel_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
        if (usb_path == SWITCH_MODEM)
            return sprintf(buf,"[UsbMenuSel] MODEM\n");

		if (currentusbstatus == USBSTATUS_UMS)
			return sprintf(buf, "[UsbMenuSel] UMS\n");

#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		else if (currentusbstatus == USBSTATUS_SAMSUNG_KIES)
				return sprintf(buf, "[UsbMenuSel] UMS_CDFS\n");

			else if (currentusbstatus == USBSTATUS_SAMSUNG_KIES_REAL) 
				return sprintf(buf, "[UsbMenuSel] ACM_MTP\n");
#else
			else if (currentusbstatus == USBSTATUS_SAMSUNG_KIES)
				return sprintf(buf, "[UsbMenuSel] ACM_MTP\n");
#endif
		else if (currentusbstatus == USBSTATUS_MTPONLY)
			return sprintf(buf, "[UsbMenuSel] MTP\n");

		else if (currentusbstatus == USBSTATUS_ASKON)
			return sprintf(buf, "[UsbMenuSel] ASK\n");
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		else if (currentusbstatus == USBSTATUS_VTP)
			return sprintf(buf, "[UsbMenuSel] VTP\n");
#endif		// ansari_L&T_FROYO_CL534716
		else if (currentusbstatus == USBSTATUS_ADB)
			return sprintf(buf, "[UsbMenuSel] ACM_ADB_UMS\n");
}


static ssize_t UsbMenuSel_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		if (strncmp(buf, "KIES_REAL", 9) == 0)
		{
			//UsbMenuSelStore(0);		
			usb_switch_select(USBSTATUS_SAMSUNG_KIES_REAL);
		}
		else
#endif
		if (strncmp(buf, "KIES", 4) == 0)
		{
			UsbMenuSelStore(0);
			usb_switch_select(USBSTATUS_SAMSUNG_KIES);
		}

		if (strncmp(buf, "MTP", 3) == 0)
		{
			UsbMenuSelStore(1);
			usb_switch_select(USBSTATUS_MTPONLY);
		}

		if (strncmp(buf, "UMS", 3) == 0)
		{
			UsbMenuSelStore(2);
			usb_switch_select(USBSTATUS_UMS);
		}
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		if (strncmp(buf, "VTP", 3) == 0)
		{
			UsbMenuSelStore(3);
			usb_switch_select(USBSTATUS_VTP);
		}
#endif// ansari_L&T_FROYO_CL534716
		if (strncmp(buf, "ASKON", 5) == 0)
		{
			UsbMenuSelStore(4);
			usb_switch_select(USBSTATUS_ASKON);
		}

	return size;
}

static DEVICE_ATTR(UsbMenuSel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, UsbMenuSel_switch_show, UsbMenuSel_switch_store);


extern int inaskonstatus;
static ssize_t AskOnStatus_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(inaskonstatus || (usb_path == SWITCH_MODEM))
		return sprintf(buf, "%s\n", "Blocking");
	else
		return sprintf(buf, "%s\n", "NonBlocking");
}


static ssize_t AskOnStatus_switch_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	return size;
}

static DEVICE_ATTR(AskOnStatus, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, AskOnStatus_switch_show, AskOnStatus_switch_store);


static ssize_t AskOnMenuSel_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
		return sprintf(buf, "%s[AskOnMenuSel] Port test ready!! \n", buf);
}

static ssize_t AskOnMenuSel_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
		if (strncmp(buf, "KIES", 4) == 0)
		{
			askon_switch_select(USBSTATUS_SAMSUNG_KIES);
		}

		if (strncmp(buf, "MTP", 3) == 0)
		{
			askon_switch_select(USBSTATUS_MTPONLY);
		}

		if (strncmp(buf, "UMS", 3) == 0)
		{
			askon_switch_select(USBSTATUS_UMS);
		}
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
		if (strncmp(buf, "VTP", 3) == 0)
		{
			askon_switch_select(USBSTATUS_VTP);
		}
#endif// ansari_L&T_FROYO_CL534716
	return size;
}

static DEVICE_ATTR(AskOnMenuSel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, AskOnMenuSel_switch_show, AskOnMenuSel_switch_store);


static ssize_t Mtp_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
		return sprintf(buf, "%s[Mtp] MtpDeviceOn \n", buf);
}

static ssize_t Mtp_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
	if (strncmp(buf, "Mtp", 3) == 0)
		{
			if(mtp_mode_on)
				{
				printk("[Mtp_switch_store]AP USB power on. \n");
#ifdef VODA
				askon_switch_select(USBSTATUS_SAMSUNG_KIES);
#endif
				ap_usb_power_on(1);
				}
		}
	else if (strncmp(buf, "OFF", 3) == 0)
		{
				printk("[Mtp_switch_store]AP USB power off. \n");
				usb_mtp_select(1);
		}
	return size;
}

static DEVICE_ATTR(Mtp, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, Mtp_switch_show, Mtp_switch_store);

static int g_tethering;

static ssize_t tethering_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if (g_tethering)
		return sprintf(buf, "1\n");
	else			
		return sprintf(buf, "0\n");
}

static ssize_t tethering_switch_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t size)
{
	int usbstatus;
	usbstatus = FSA9480_Get_USB_Status();
	dmsg("usbstatus = 0x%x, currentusbstatus = 0x%x\n", usbstatus, currentusbstatus);

	if (strncmp(buf, "1", 1) == 0)
	{
		dmsg("tethering On\n");

		g_tethering = 1;
		usb_switch_select(USBSTATUS_VTP);
		UsbIndicator(0);
	}
	else if (strncmp(buf, "0", 1) == 0)
	{
		dmsg("tethering Off\n");

		g_tethering = 0;
		usb_switch_select(oldusbstatus);
		if(usbstatus)
			UsbIndicator(1);
	}

	return size;
}

static DEVICE_ATTR(tethering, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, tethering_switch_show, tethering_switch_store);


static int mtpinitstatus=0;
static ssize_t MtpInitStatusSel_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(mtpinitstatus == 2)
		return sprintf(buf, "%s\n", "START");
	else
		return sprintf(buf, "%s\n", "STOP");
}

static ssize_t MtpInitStatusSel_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
	mtpinitstatus = mtpinitstatus + 1;

	return size;
}

static DEVICE_ATTR(MtpInitStatusSel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, MtpInitStatusSel_switch_show, MtpInitStatusSel_switch_store);


static int askinitstatus=0;
static ssize_t AskInitStatusSel_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(askinitstatus == 2)
		return sprintf(buf, "%s\n", "START");
	else
		return sprintf(buf, "%s\n", "STOP");
}

static ssize_t AskInitStatusSel_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
	askinitstatus = askinitstatus + 1;

	return size;
}

static DEVICE_ATTR(AskInitStatusSel, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, AskInitStatusSel_switch_show, AskInitStatusSel_switch_store);


// TODO : what should s1_froyo use?
#if 1 //P1_proyo
static ssize_t get_SwitchingInitValue(struct device *dev, struct device_attribute *attr,	char *buf)
{
	return snprintf(buf, 12, "%s\n", switchinginitvalue);
}
#else //S1
static ssize_t get_SwitchingInitValue(struct device *dev, struct device_attribute *attr,	const char *buf)
{
	return snprintf(buf, 12, "%d\n", switchinginitvalue);
}
#endif

static DEVICE_ATTR(SwitchingInitValue, S_IRUGO, get_SwitchingInitValue, NULL);


#if 1 //20100601_inchul
static ssize_t HWrevision_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return sprintf(buf, "%x\n", HWREV);
}

static ssize_t HWrevision_store(
		struct device *dev, struct device_attribute *attr,
		const char *buf, size_t size)
{
    DEBUG_FSA9480("[FSA9480]%s\n ", __func__);

	return 0;
}

/*sysfs for HW Board revision */
static DEVICE_ATTR(HWrevision, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, HWrevision_show, HWrevision_store);
#endif

#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
static int kies_status = 0;
static ssize_t KiesStatus_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	if(kies_status == 1)
		return sprintf(buf, "%s\n", "START");
	else if( kies_status == 2)
		return sprintf(buf, "%s\n", "STOP");
    else
        return sprintf(buf, "%s\n", "INIT");
}

static ssize_t KiesStatus_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
    dmsg("buf=%s\n", buf);

    if (strncmp(buf, "START", 5) == 0 )
    {
        kies_status = 1;
    }
	else if (strncmp(buf, "STOP", 4) == 0)
	{
	    kies_status = 2;
        UsbIndicator(2);
	}
    else if (strncmp(buf, "INIT", 4) == 0 )
    {
        kies_status = 0;
    }

    return size;
}

static DEVICE_ATTR(KiesStatus, S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, KiesStatus_switch_show, KiesStatus_switch_store);
#endif /* _SUPPORT_SAMSUNG_AUTOINSTALLER_ */


// [ lte_factory_test_101203
char lte_factory_test = FALSE;

static ssize_t LteFactoryTest_switch_show(struct device *dev, struct device_attribute *attr, char *buf)
{
   dmsg("LteFactoryTest_switch_show lte_factory_test=%d\n", lte_factory_test);
   
	return sprintf(buf,"%d\n",lte_factory_test);
}

static ssize_t LteFactoryTest_switch_store(struct device *dev, struct device_attribute *attr,	const char *buf, size_t size)
{
   dmsg("LteFactoryTest_switch_store buf=%s\n", buf);

   if (strncmp(buf, "0", 1) == 0 )
   {
      lte_factory_test = FALSE;
   }
   else
   {
      lte_factory_test = TRUE;
   }

   return size;
}

static DEVICE_ATTR(LteFactoryTest, 
                   S_IRUGO |S_IWUGO | S_IRUSR | S_IWUSR, 
                   LteFactoryTest_switch_show, 
                   LteFactoryTest_switch_store);
// lte_factory_test_101203 ]



int  FSA9480_PMIC_CP_USB(void)
{
	int usb_sel = 0;

	if (sec_get_param_value)
		sec_get_param_value(__SWITCH_SEL, &switch_sel);

	usb_sel = switch_sel & (int)(USB_SEL_MASK);
    // USB_SEL_MASK=0  USB_SEL_2nd_MASK=0
    usb_sel = switch_sel & (int)(USB_SEL_2nd_MASK);

	return usb_sel;
}

int  FSA9480_ALL_OPEN(int all_open)
{
   struct i2c_client *client = fsa9480_i2c_client;
   u8 reg_value;

   DEBUG_FSA9480("[FSA9480] %s all_open:%d\n", __func__, all_open);
#if 1


   if(all_open)
   {

      mdelay(10);
      fsa9480_write(client, REGISTER_MANUALSW1, 0x90); // TV-out
      mdelay(10);
      fsa9480_write(client, REGISTER_CONTROL, 0x1A);
   
   }
   else
   {


   }

#else
   fsa9480_read(client, REGISTER_CONTROL, &reg_value);
   mdelay(10);
   if(all_open)
      reg_value = reg_value&0x0F;
   else
      reg_value = reg_value|0x10;

   fsa9480_write(client, REGISTER_CONTROL, reg_value);
   mdelay(10);
#endif
}

int check_reg=0;
void FSA9480_Enable_CP_USB(u8 enable)
{
	struct i2c_client *client = fsa9480_i2c_client;
	byte reg_value=0;
	byte reg_address=0x0D;

	if(enable)
	{
		printk("[FSA9480_Enable_CP_USB] Enable CP USB\n");
		mdelay(10);
		Get_MAX8998_PM_ADDR(reg_address, &reg_value, 1); // read 0x0D register
		check_reg = reg_value;
		reg_value = ((0x2<<5)|reg_value);
		check_reg = reg_value;
		Set_MAX8998_PM_ADDR(reg_address,&reg_value,1);
		check_reg = reg_value;

		#if 1 //20100405_inchul
		/*
		    Added source for the below issue.

		    1. Connect  the UART cable
		    2. Enter '**87284 or *#7284#'
		    3. Select the USB path as Modem or PDA
		    4. If user chooses PDA, UART is disconnected
		*/

		   if(MicroJigUARTOffStatus)
		   {
		    		mdelay(10);
		    		fsa9480_write(client, REGISTER_CONTROL, 0x1E);
		   }
		   else
		   {
				mdelay(10);
				fsa9480_write(client, REGISTER_MANUALSW1, 0x48); // for FORTE 0x90->0x48
				mdelay(10);
				fsa9480_write(client, REGISTER_CONTROL, 0x1A);
		   }
		#else
				mdelay(10);
				fsa9480_write(client, REGISTER_MANUALSW1, 0x90);	// D+/- switching by V_Audio_L/R in HW03
				mdelay(10);
				fsa9480_write(client, REGISTER_CONTROL, 0x1A);;	//manual switching
#endif
	}
	else
	{
		printk("[FSA9480_Enable_AP_USB] Enable AP USB\n");
		Get_MAX8998_PM_ADDR(reg_address, &reg_value, 1); // read 0x0D register

		if(askonstatus||mtp_mode_on)
		ap_usb_power_on(0);
		else
		ap_usb_power_on(1);
		mdelay(10);
		fsa9480_write(client, REGISTER_CONTROL, 0x1E);
	}
}


void FSA9480_Enable_SPK(u8 enable)
{
	#if 0// 20101111_Not support car&desk dock
	struct i2c_client *client = fsa9480_i2c_client;
	u8 data = 0;
	byte reg_value=0;
	byte reg_address=0x0D;

	if(enable)
	{
		DEBUG_FSA9480("FSA9480_Enable_SPK --- enable\n");
		msleep(10);
		Get_MAX8998_PM_ADDR(reg_address, &reg_value, 1); // read 0x0D register
		check_reg = reg_value;
		reg_value = ((0x2<<5)|reg_value);
		check_reg = reg_value;
		Set_MAX8998_PM_ADDR(reg_address,&reg_value,1);
		check_reg = reg_value;

		msleep(10);
		fsa9480_write(client, REGISTER_MANUALSW1, 0x48);	// D+/- switching by V_Audio_L/R in HW03
		msleep(10);
		fsa9480_write(client, REGISTER_CONTROL, 0x1A);	//manual switching

	}
	#endif
}

void UsbIndicator(u8 state)
{
	switch_set_state(&indicator_dev, state);
}

#if 0// 20101111_Not support car&desk dock //20100630_inchul
void Usb_Dock_Detector(u8 state)
{
	switch_set_state(&switch_dock_detection, state);
}
#endif

extern void askon_gadget_disconnect(void);
extern int s3c_usb_cable(int connected);

extern void vps_status_change(int status);
extern void car_vps_status_change(int status);

int uUSB_check_finished = 0;
EXPORT_SYMBOL(uUSB_check_finished);


void FSA9480_ProcessDevice(u8 dev1, u8 dev2, u8 attach)
{
	DEBUG_FSA9480("[FSA9480] %s (dev1 : 0x%x, dev2 : 0x%x)\n", __func__, dev1, dev2);

	if(dev1)
	{
		switch(dev1)
		{
			case FSA9480_DEV_TY1_AUD_TY1:
				DEBUG_FSA9480("Audio Type1 ");
				if(attach & FSA9480_INT1_ATTACH)
					DEBUG_FSA9480("FSA9480_DEV_TY1_AUD_TY1 --- ATTACH\n");
				else
					DEBUG_FSA9480("FSA9480_DEV_TY1_AUD_TY1 --- DETACH\n");
				break;

			case FSA9480_DEV_TY1_AUD_TY2:
				DEBUG_FSA9480("Audio Type2 ");
				break;

			case FSA9480_DEV_TY1_USB:
				DEBUG_FSA9480("USB attach or detach: %d\n",attach);
				if(attach & FSA9480_INT1_ATTACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY1_USB --- ATTACH\n");
					MicroUSBStatus = 1;
					log_via_usb = log_usb_enable;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(1);
#endif
					usb_switch_state();

                    if(!askonstatus)
                        UsbIndicator(1);

                    inaskonstatus = 0;
					uUSB_check_finished = 1;  // finished
				}
				else if(attach & FSA9480_INT1_DETACH)
				{
					MicroUSBStatus = 0;
					inaskonstatus = 0;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(0);
#endif
					UsbIndicator(0);
					askon_gadget_disconnect();
					DEBUG_FSA9480("FSA9480_DEV_TY1_USB --- DETACH\n");
					uUSB_check_finished = 0;  // finished
				}
				break;

			case FSA9480_DEV_TY1_UART:
				DEBUG_FSA9480("UART\n");
				break;

			case FSA9480_DEV_TY1_CAR_KIT:
				DEBUG_FSA9480("Carkit\n");
				break;

			case FSA9480_DEV_TY1_USB_CHG:
				DEBUG_FSA9480("USB\n");
				break;

			case FSA9480_DEV_TY1_DED_CHG:
				{
					if(attach & FSA9480_INT1_ATTACH)
					{
						DEBUG_FSA9480("Dedicated Charger ATTACH\n");
						uUSB_check_finished = 1;  // finished
						//A9480_ChangePathToAudio(TRUE);
					}
					else if(attach & FSA9480_INT1_DETACH)
					{
						DEBUG_FSA9480("Dedicated Charger DETACH\n");
						uUSB_check_finished = 0;  // finished
						//A9480_ChangePathToAudio(FALSE);
					}
				}
				break;

			case FSA9480_DEV_TY1_USB_OTG:
				DEBUG_FSA9480("USB OTG\n");
				break;

			default:
				DEBUG_FSA9480("Unknown device\n");
				break;
		}

	}

	if(dev2)
	{
		switch(dev2)
		{
			case FSA9480_DEV_TY2_JIG_USB_ON:
				DEBUG_FSA9480("JIG USB ON attach or detach: %d",attach);
				if(attach & FSA9480_INT1_ATTACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_USB_ON --- ATTACH\n");
					MicroJigUSBOnStatus = 1;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(1);
#endif
					usb_switch_state();
					if(!askonstatus)
						UsbIndicator(1);
					else
						inaskonstatus = 0;
				}
				else if(attach & FSA9480_INT1_DETACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_USB_ON --- DETACH\n");
					MicroJigUSBOnStatus = 0;
					inaskonstatus = 0;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(0);
#endif
					UsbIndicator(0);
					askon_gadget_disconnect();
				}
				break;

			case FSA9480_DEV_TY2_JIG_USB_OFF:
				if(attach & FSA9480_INT1_ATTACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_USB_OFF --- ATTACH\n");
					MicroJigUSBOffStatus = 1;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(1);
#endif
					usb_switch_state();
					if(!askonstatus)
						UsbIndicator(1);
					else
						inaskonstatus = 0;
				}
				else if(attach & FSA9480_INT1_DETACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_USB_OFF --- DETACH\n");
					MicroJigUSBOffStatus = 0;
					inaskonstatus = 0;
#if 0
					if(connectivity_switching_init_state)
						s3c_usb_cable(0);
#endif
					UsbIndicator(0);
					askon_gadget_disconnect();
				}
				DEBUG_FSA9480("JIG USB OFF \n");
				break;

			case FSA9480_DEV_TY2_JIG_UART_ON:
				if(attach & FSA9480_INT1_ATTACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_UART_ON --- ATTACH\n");
#if 0// 20101111_Not support car&desk dock //20100630_inchul
					Usb_Dock_Detector(SEC_CAR_DOCK_DEVICE);
					car_vps_status_change(1);
#endif
				}
				else
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_UART_ON --- DETACH\n");
#if 0// 20101111_Not support car&desk dock //20100630_inchul
					Usb_Dock_Detector(SEC_DOCK_NO_DEVICE);
					car_vps_status_change(0);
#endif
				}
				DEBUG_FSA9480("JIG UART ON\n");
				break;

			case FSA9480_DEV_TY2_JIG_UART_OFF:
				if(attach & FSA9480_INT1_ATTACH)
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_UART_OFF --- ATTACH\n");
					MicroJigUARTOffStatus = 1;
					//uart_insert_switch_state();
				}
				else
				{
					DEBUG_FSA9480("FSA9480_DEV_TY2_JIG_UART_OFF --- DETACH\n");
					MicroJigUARTOffStatus = 0;
					//uart_remove_switch_state();
				}
				DEBUG_FSA9480("JIT UART OFF\n");
				break;

			case FSA9480_DEV_TY2_PDD:
				DEBUG_FSA9480("PPD \n");
				break;

			case FSA9480_DEV_TY2_TTY:
				DEBUG_FSA9480("TTY\n");
				break;

			case FSA9480_DEV_TY2_AV:
				DEBUG_FSA9480("AudioVideo\n");
				if(attach & FSA9480_INT1_ATTACH){
					DEBUG_FSA9480("FSA9480_enable_spk\n");
#if 0 // 20101111_Not support car&desk dock //20100630_inchul
					Usb_Dock_Detector(SEC_DESK_DOCK_DEVICE);
					vps_status_change(1);
					//FSA9480_Enable_SPK(1);
#endif
					}
				else {
					DEBUG_FSA9480("FSA9480_disable_spk\n");
#if 0 // 20101111_Not support car&desk dock //20100630_inchul
					Usb_Dock_Detector(SEC_DOCK_NO_DEVICE);
					vps_status_change(0);
#endif
					}
				break;

			default:
				DEBUG_FSA9480("Unknown device\n");
				break;
		}
	}

}

EXPORT_SYMBOL(FSA9480_ProcessDevice);


void FSA9480_ReadIntRegister(struct work_struct *work)
{
	struct i2c_client *client = fsa9480_i2c_client;
	u8 interrupt1 , device1, device2, temp;

	DEBUG_FSA9480("[FSA9480] %s\n", __func__);

	 fsa9480_read(client, REGISTER_INTERRUPT1, &interrupt1);
 	msleep(5);

	 fsa9480_read(client, REGISTER_DEVICETYPE1, &device1);
 	msleep(5);

	 fsa9480_read(client, REGISTER_DEVICETYPE2, &device2);

	usb_state = (device2 << 8) | (device1 << 0);

	if(interrupt1 & FSA9480_INT1_ATTACH)
	{
		fsa9480_device1 = device1;
		fsa9480_device2 = device2;

		if(fsa9480_device1 != FSA9480_DEV_TY1_DED_CHG){
			//DEBUG_FSA9480("FSA9480_enable LDO8\n");
			s3c_usb_cable(1);
		}

		if(fsa9480_device1&FSA9480_DEV_TY1_CAR_KIT)
		{
			msleep(5);
			fsa9480_write(client, REGISTER_CARKITSTATUS, 0x02);

			msleep(5);
			fsa9480_read(client, REGISTER_CARKITINT1, &temp);
		}
	}

 	msleep(5);

	 fsa9480_write(client, REGISTER_CONTROL, 0x1E);
	 fsa9480_write(client, REGISTER_INTERRUPTMASK1, 0xFC);

#if 1 //20100618_inchul... To block the A/V Charging interrupt
	 msleep(5);

	 fsa9480_write(client, REGISTER_INTERRUPTMASK2, 0xFF);
#endif



	 FSA9480_ProcessDevice(fsa9480_device1, fsa9480_device2, interrupt1);

	if(interrupt1 & FSA9480_INT1_DETACH)
	{
		if(fsa9480_device1 != FSA9480_DEV_TY1_DED_CHG){
			//DEBUG_FSA9480("FSA9480_disable LDO8\n");
			s3c_usb_cable(0);
		}

		fsa9480_device1 = 0;
		fsa9480_device2 = 0;
	}

	enable_irq(IRQ_FSA9480_INTB);
}
EXPORT_SYMBOL(FSA9480_ReadIntRegister);



irqreturn_t fsa9480_interrupt(int irq, void *ptr)
{
	printk("%s\n", __func__);
	DEBUG_FSA9480("[FSA9480]----------DEBUG INT HENDLER--------------- %s\n", __func__);
        disable_irq_nosync(IRQ_FSA9480_INTB);

	uUSB_check_finished =0;  // reset

	queue_work(fsa9480_workqueue, &fsa9480_work);

	return IRQ_HANDLED;
}
EXPORT_SYMBOL(fsa9480_interrupt);


void fsa9480_interrupt_init(void)
{
	s3c_gpio_cfgpin(GPIO_JACK_nINT, S3C_GPIO_SFN(GPIO_JACK_nINT_AF));
	s3c_gpio_setpull(GPIO_JACK_nINT, S3C_GPIO_PULL_NONE);
	set_irq_type(IRQ_FSA9480_INTB, IRQ_TYPE_EDGE_FALLING);

	 if (request_irq(IRQ_FSA9480_INTB, fsa9480_interrupt, IRQF_DISABLED, "FSA9480 Detected", NULL))
		 DEBUG_FSA9480("[FSA9480]fail to register IRQ[%d] for FSA9480 USB Switch \n", IRQ_FSA9480_INTB);
}
EXPORT_SYMBOL(fsa9480_interrupt_init);


void fsa9480_chip_init(void)
{
	struct i2c_client *client = fsa9480_i2c_client;
	 fsa9480_write(client, HIDDEN_REGISTER_MANUAL_OVERRDES1, 0x01); //RESET

	 mdelay(10);

	 fsa9480_write(client, REGISTER_CONTROL, 0x1E);

	 mdelay(10);

	 fsa9480_write(client, REGISTER_INTERRUPTMASK1, 0xFC);
#if 1 //20100618_inchul... To block the A/V Charging interrupt
	 mdelay(10);

	 fsa9480_write(client, REGISTER_INTERRUPTMASK2, 0xFF);
#endif
	 mdelay(10);

	 fsa9480_read(client, REGISTER_DEVICETYPE1, &fsa9480_device1);

	 mdelay(10);

	 fsa9480_read(client, REGISTER_DEVICETYPE2, &fsa9480_device2);
}


// TODO : need it?
#if 0
static int fsa9480_modify(struct i2c_client *client, u8 reg, u8 data, u8 mask)
{
   u8 original_value, modified_value;

   fsa9480_read(client, reg, &original_value);

   mdelay(10);

   modified_value = ((original_value&~mask) | data);

   fsa9480_write(client, reg, modified_value);

   mdelay(10);

   return 0;
}


void fsa9480_init_status(void)
{
	u8 pData;

	fsa9480_modify(&fsa9480_i2c_client,REGISTER_CONTROL,~INT_MASK, INT_MASK);

	fsa9480_read(&fsa9480_i2c_client, 0x13, &pData);
}
#endif

u8 FSA9480_Get_I2C_USB_Status(void)
{
	u8 device1, device2;

	fsa9480_read(fsa9480_i2c_client, REGISTER_DEVICETYPE1, &device1);
  	msleep(5);
	fsa9480_read(fsa9480_i2c_client, REGISTER_DEVICETYPE2, &device2);

	if((device1==FSA9480_DEV_TY1_USB)||(device2==FSA9480_DEV_TY2_JIG_USB_ON)||(device2==FSA9480_DEV_TY2_JIG_USB_OFF))
		return 1;
	else
		return 0;
}
EXPORT_SYMBOL(FSA9480_Get_I2C_USB_Status);


void connectivity_switching_init(struct work_struct *ignored)
{
	int usb_sel,uart_sel,samsung_kies_sel,ums_sel,mtp_sel,vtp_sel,askon_sel;
	int lpm_mode_check = charging_mode_get();
	int usb_2nd_sel,uart_2nd_sel;

// [ lte_factory_test_101203
	char cmdline[12];
	char* pCmd = NULL;

   if(sec_get_param_value){
       sec_get_param_value(__PARAM_STR_4, cmdline);
       printk(KERN_ERR "[LTE_UART] PARAM_STR_4 : %s\n",cmdline);
       pCmd = strstr(cmdline, "LTE_FACTORY");
       if( pCmd ){            
           lte_factory_test = TRUE;
           memset(cmdline, 0x00, 12);
           sec_set_param_value(__PARAM_STR_4, cmdline);
           printk(KERN_ERR "pCmd (LTE_FACTORY) : lte_factory_test is TRUE\n");
       }
    }
// lte_factory_test_101203 ]

	if (sec_get_param_value){
		sec_get_param_value(__SWITCH_SEL, &switch_sel);
		cancel_delayed_work(&micorusb_init_work);
	}
	else{
		schedule_delayed_work(&micorusb_init_work, msecs_to_jiffies(200));
		return;
	}

	if(BOOTUP){
		BOOTUP = 0;
		otg_phy_init(); //USB Power on after boot up.
	}

	printk("[FSA9480]connectivity_switching_init = switch_sel : 0x%x\n",switch_sel);

	usb_sel = switch_sel & (int)(USB_SEL_MASK);
	uart_sel = (switch_sel & (int)(UART_SEL_MASK)) >> 1;
	samsung_kies_sel = (switch_sel & (int)(USB_SAMSUNG_KIES_MASK)) >> 2;
	ums_sel = (switch_sel & (int)(USB_UMS_MASK)) >> 3;
	mtp_sel = (switch_sel & (int)(USB_MTP_MASK)) >> 4;
	vtp_sel = (switch_sel & (int)(USB_VTP_MASK)) >> 5;
	askon_sel = (switch_sel & (int)(USB_ASKON_MASK)) >> 6;
    usb_2nd_sel = (switch_sel & (int)(USB_SEL_2nd_MASK)) >> 11;
    uart_2nd_sel = (switch_sel & (int)(UART_SEL_2nd_MASK)) >> 12;

	if((switch_sel == 0x1) || (factoryresetstatus == 0xAE)){
		PathSelStore(AP_USB_MODE);
		Ap_Cp_Switch_Config(AP_USB_MODE);
		usb_switching_value_update(SWITCH_PDA);

		PathSelStore(CP_UART_MODE);
		Ap_Cp_Switch_Config(CP_UART_MODE);
		uart_switching_value_update(SWITCH_MODEM);
#if 1 //20100531_inchul
        uart_current_owner = 0;
#endif

	}
	else{
        if(usb_sel == 0 && usb_2nd_sel == 1){
            Ap_Cp_Switch_Config(LTE_USB_MODE);
            usb_switching_value_update(SWITCH_LTE);
        }else
	    if(usb_sel) {
		    Ap_Cp_Switch_Config(AP_USB_MODE);
		    usb_switching_value_update(SWITCH_PDA);
	    }
	    else {
			if(MicroJigUARTOffStatus){
			    Ap_Cp_Switch_Config(AP_USB_MODE);
			}
		    else {
			    Ap_Cp_Switch_Config(CP_USB_MODE);
			    usb_switching_value_update(SWITCH_MODEM);
		    }
	    }
// [ lte_factory_test_101203
        if(lte_factory_test == TRUE)
        {
            printk(KERN_ERR "-- change LTE_UART_MODE !!!\n"); 
            Ap_Cp_Switch_Config(LTE_UART_MODE);
            uart_switching_value_update(SWITCH_LTE);
            uart_current_owner = 2;
        }else
        // LTE_Factory_test }}
        if(uart_sel == 0 && uart_2nd_sel == 1){
#ifndef CHANGE_UART_TO_CP_ON_BOOT
            Ap_Cp_Switch_Config(LTE_UART_MODE);
            uart_switching_value_update(SWITCH_LTE);
            uart_current_owner = 2;
#else //change uart to CP when LTE on boot up
            PathSelStore(CP_UART_MODE);
            
		    Ap_Cp_Switch_Config(CP_UART_MODE);
		    uart_switching_value_update(SWITCH_MODEM);
		    #if 1 //20100531_inchul
						uart_current_owner = 0;
			#endif
#endif            
        }else

	    if(uart_sel) {
		    Ap_Cp_Switch_Config(AP_UART_MODE);
		    uart_switching_value_update(SWITCH_PDA);
		    #if 1 //20100531_inchul
						uart_current_owner = 1;
			#endif

	    }
	    else {
		    Ap_Cp_Switch_Config(CP_UART_MODE);
		    uart_switching_value_update(SWITCH_MODEM);
		    #if 1 //20100531_inchul
						uart_current_owner = 0;
			#endif

	    }
	}

	/*Turn off usb power when LPM mode*/
	if(lpm_mode_check)
		otg_phy_off();

		switching_value_update();

	if((switch_sel == 1) || (factoryresetstatus == 0xAE)){
		usb_switch_select(USBSTATUS_SAMSUNG_KIES);
		mtp_mode_on = 1;
		ap_usb_power_on(0);
		UsbMenuSelStore(0);
	}
	else{
		if(usb_sel){
			if(samsung_kies_sel){
				usb_switch_select(USBSTATUS_SAMSUNG_KIES);

#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
					//mtp_mode_on = 1;
					//ap_usb_power_on(0);
#else				
				/*USB Power off till MTP Appl launching*/
				mtp_mode_on = 1;
				ap_usb_power_on(0);
#endif				
			}
			else if(mtp_sel){
				usb_switch_select(USBSTATUS_MTPONLY);
				/*USB Power off till MTP Appl launching*/
				mtp_mode_on = 1;
				ap_usb_power_on(0);
			}
			else if(ums_sel){
				usb_switch_select(USBSTATUS_UMS);
			}
#if !defined(CONFIG_ARIES_NTT) // disable tethering xmoondash
			else if(vtp_sel){
				usb_switch_select(USBSTATUS_VTP);
			}
#endif// ansari_L&T_FROYO_CL534716
			else if(askon_sel){
				usb_switch_select(USBSTATUS_ASKON);
			}
		}
	}

	if(!FSA9480_Get_USB_Status()) {
		s3c_usb_cable(1);
		mdelay(5);
		s3c_usb_cable(0);
	}

	printk("[FSA9480]connectivity_switching_init = switch_sel : 0x%x\n",switch_sel);
	// microusb_uart_status(1);

	connectivity_switching_init_state=1;
}


static ssize_t print_switch_name(struct switch_dev *sdev, char *buf)
{
	return sprintf(buf, "%s\n", DRIVER_NAME);
}

static ssize_t print_switch_state(struct switch_dev *sdev, char *buf)
{
	int usbstatus;

	usbstatus = FSA9480_Get_USB_Status();

//TODO : each platform require different noti
#if 0 //froyo default
    if(usbstatus){
        return sprintf(buf, "%s\n", "online");
    }
    else{
        return sprintf(buf, "%s\n", "offline");
    }
#elif 1 //P1
    if(usbstatus){
        if((currentusbstatus== USBSTATUS_UMS) || (currentusbstatus== USBSTATUS_ADB))
            return sprintf(buf, "%s\n", "ums online");
        else
            return sprintf(buf, "%s\n", "InsertOffline");
    }
    else{
        if((currentusbstatus== USBSTATUS_UMS) || (currentusbstatus== USBSTATUS_ADB))
            return sprintf(buf, "%s\n", "ums offline");
        else
            return sprintf(buf, "%s\n", "RemoveOffline");
    }
#else //S1
	if(usbstatus){
		if((currentusbstatus== USBSTATUS_UMS) || (currentusbstatus== USBSTATUS_ADB))
			return sprintf(buf, "%s\n", "InsertOnline");
		else
			return sprintf(buf, "%s\n", "InsertOffline");
	}
	else{
		if((currentusbstatus== USBSTATUS_UMS) || (currentusbstatus== USBSTATUS_ADB))
			return sprintf(buf, "%s\n", "RemoveOnline");
		else
			return sprintf(buf, "%s\n", "RemoveOffline");
	}
#endif
}


static int fsa9480_codec_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct fsa9480_state *state;
	struct device *dev = &client->dev;
	u8 pData;

	DEBUG_FSA9480("[FSA9480] %s\n", __func__);

	s3c_gpio_cfgpin(GPIO_USB_SCL_28V, S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(GPIO_USB_SCL_28V, S3C_GPIO_PULL_NONE);

	s3c_gpio_cfgpin(GPIO_USB_SDA_28V, S3C_GPIO_OUTPUT);
	s3c_gpio_setpull(GPIO_USB_SDA_28V, S3C_GPIO_PULL_NONE);

	s3c_gpio_cfgpin(GPIO_UART_SEL, S3C_GPIO_OUTPUT );
	s3c_gpio_setpull(GPIO_UART_SEL, S3C_GPIO_PULL_NONE);

	s3c_gpio_cfgpin(GPIO_UART_SEL1, S3C_GPIO_OUTPUT );
	s3c_gpio_setpull(GPIO_UART_SEL1, S3C_GPIO_PULL_NONE);

   s3c_gpio_cfgpin( GPIO_USB_SEL, S3C_GPIO_OUTPUT);
   s3c_gpio_setpull( GPIO_USB_SEL, S3C_GPIO_PULL_NONE);

	if (device_create_file(switch_dev, &dev_attr_uart_sel) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_uart_sel.attr.name);

	if (device_create_file(switch_dev, &dev_attr_usb_sel) < 0)
		DEBUG_FSA9480("[FSA9480]Failed to create device file(%s)!\n", dev_attr_usb_sel.attr.name);

	if (device_create_file(switch_dev, &dev_attr_usb_state) < 0)
		DEBUG_FSA9480("[FSA9480]Failed to create device file(%s)!\n", dev_attr_usb_state.attr.name);

#if 1
	if (device_create_file(switch_dev, &dev_attr_DMport) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_DMport.attr.name);

	if (device_create_file(switch_dev, &dev_attr_DMlog) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_DMlog.attr.name);
#endif

	if (device_create_file(switch_dev, &dev_attr_UsbMenuSel) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_UsbMenuSel.attr.name);

	if (device_create_file(switch_dev, &dev_attr_AskOnMenuSel) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_AskOnMenuSel.attr.name);

	if (device_create_file(switch_dev, &dev_attr_Mtp) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_Mtp.attr.name);

	if (device_create_file(switch_dev, &dev_attr_SwitchingInitValue) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_SwitchingInitValue.attr.name);

	if (device_create_file(switch_dev, &dev_attr_FactoryResetValue) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_FactoryResetValue.attr.name);

	if (device_create_file(switch_dev, &dev_attr_AskOnStatus) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_AskOnStatus.attr.name);
#if 1
	if (device_create_file(switch_dev, &dev_attr_HWrevision) < 0)
		pr_err("Failed to create device file(%s)!\n", dev_attr_HWrevision.attr.name);
#endif

if (device_create_file(switch_dev, &dev_attr_tethering) < 0)
	dmsg("Failed to create device file(%s)!\n", dev_attr_tethering.attr.name);

#ifdef _SUPPORT_SAMSUNG_AUTOINSTALLER_
		 if (device_create_file(switch_dev, &dev_attr_KiesStatus) < 0)
			 dmsg("Failed to create device file(%s)!\n", dev_attr_KiesStatus.attr.name);
#endif
// [ lte_factory_test_101203
   if (device_create_file(switch_dev, &dev_attr_LteFactoryTest) < 0)
      dmsg("Failed to create device file(%s)!\n", dev_attr_LteFactoryTest.attr.name);
// lte_factory_test_101203 ]


	 init_waitqueue_head(&usb_detect_waitq);
	 INIT_WORK(&fsa9480_work, FSA9480_ReadIntRegister);
	 fsa9480_workqueue = create_singlethread_workqueue("fsa9480_workqueue");

	 state = kzalloc(sizeof(struct fsa9480_state), GFP_KERNEL);
	 if(!state) {
		 dev_err(dev, "%s: failed to create fsa9480_state\n", __func__);
		 return -ENOMEM;
	 }

	indicator_dev.name = DRIVER_NAME;
	indicator_dev.print_name = print_switch_name;
	indicator_dev.print_state = print_switch_state;
	switch_dev_register(&indicator_dev);

#if 1 //20100630_inchul
		 switch_dev_register(&switch_dock_detection);
#endif

	state->client = client;
	fsa9480_i2c_client = client;

	i2c_set_clientdata(client, state);
	if(!fsa9480_i2c_client)
	{
		dev_err(dev, "%s: failed to create fsa9480_i2c_client\n", __func__);
		return -ENODEV;
	}

	 /*clear interrupt mask register*/
	fsa9480_read(fsa9480_i2c_client, REGISTER_CONTROL, &pData);
	fsa9480_write(fsa9480_i2c_client, REGISTER_CONTROL, pData & ~INT_MASK);

	 fsa9480_interrupt_init();
	 fsa9480_chip_init();
	INIT_DELAYED_WORK(&micorusb_init_work, connectivity_switching_init);
	schedule_delayed_work(&micorusb_init_work, msecs_to_jiffies(200));

	 return 0;
}


static int __devexit fsa9480_remove(struct i2c_client *client)
{
	struct fsa9480_state *state = i2c_get_clientdata(client);
	kfree(state);
	return 0;
}


struct i2c_driver fsa9480_i2c_driver = {
	.driver = {
		.owner	= THIS_MODULE,
		.name	= "fsa9480",
	},
	.id_table	= fsa9480_id,
	.probe	= fsa9480_codec_probe,
	.remove	= __devexit_p(fsa9480_remove),
	.command = NULL,
};
