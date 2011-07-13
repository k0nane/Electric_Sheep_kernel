#ifndef __GPIO_ARIES_H_
#define __GPIO_ARIES_H_

//#include <mach/gpio.h>

#define GPIO_LEVEL_LOW          0
#define GPIO_LEVEL_HIGH         1
#define GPIO_LEVEL_NONE         2
#define GPIO_INPUT              0
#define GPIO_OUTPUT             1

/* HW Requeset Fixed keypad led on issue  when AP is sleep wakeup*/
#define GPIO_MAIN_KEY_LED_NEW   S5PV210_MP04(0)

#define GPIO_BT_UART_RXD        S5PV210_GPA0(0)
#define GPIO_BT_UART_RXD_AF     2
#define GPIO_BT_UART_TXD        S5PV210_GPA0(1)
#define GPIO_BT_UART_TXD_AF     2
#define GPIO_BT_UART_CTS        S5PV210_GPA0(2)
#define GPIO_BT_UART_CTS_AF     2
#define GPIO_BT_UART_RTS        S5PV210_GPA0(3)
#define GPIO_BT_UART_RTS_AF     2

#define GPIO_ACC_INT            S5PV210_GPA0(4)
#define GPIO_KEYSCAN6           S5PV210_GPA0(5)
#define GPIO_CAM_MEGA_EN        S5PV210_GPA0(6)
#define GPIO_TA_CURRENT_SEL_AP  S5PV210_GPA0(7)

#define GPIO_AP_RXD             S5PV210_GPA1(0)
#define GPIO_AP_RXD_AF          2
#define GPIO_AP_TXD             S5PV210_GPA1(1)
#define GPIO_AP_TXD_AF          2

#define GPIO_CP_RST             S5PV210_GPA1(2)
#define GPIO_QSC_RESET_N        S5PV210_GPA1(2)
#define GPIO_QSC_RESET_N_AF     1
#define GPIO_KEYSCAN7           S5PV210_GPA1(3)
#define GPIO_KEYSCAN7_AF        3

#define GPIO_LTE_SPI_CLK        S5PV210_GPB(0)      /* CMC_CLK_2.8V */
#define GPIO_LTE_SPI_CLK_AF     2
#define GPIO_LTE_SPI_CS         S5PV210_GPB(1)      /* CMC_CS_2.8V */
#define GPIO_LTE_SPI_CS_AF      2
#define GPIO_LTE_SPI_MISO       S5PV210_GPB(2)      /* CMC_DO_2.8V */
#define GPIO_LTE_SPI_MISO_AF    2
#define GPIO_LTE_SPI_MOSI       S5PV210_GPB(3)      /* CMC_DI_2.8V */
#define GPIO_LTE_SPI_MOSI_AF    2

#define GPIO_BL_SCL_28V         S5PV210_GPB(4)
#define GPIO_BL_SCL_28V_AF      2
#define GPIO_BL_SDA_28V         S5PV210_GPB(5)
#define GPIO_BL_SDA_28V_AF      2

#define GPIO_BUCK_1_EN_A        S5PV210_GPB(6)
#define GPIO_BUCK_2_EN          S5PV210_GPB(7)

#define GPIO_GPC00              S5PV210_GPC0(0)
#define GPIO_DISPLAY_CLK        S5PV210_GPC0(1)
#define GPIO_DISPLAY_CLK_AF     S3C_GPIO_SFN(1)
#define GPIO_DISPLAY_ID         S5PV210_GPC0(2)
#define GPIO_DISPLAY_SI         S5PV210_GPC0(3)
#define GPIO_DISPLAY_SI_AF      S3C_GPIO_SFN(1)
#define GPIO_DISPLAY_RST        S5PV210_GPC0(4)
#define GPIO_MLCD_RST           GPIO_DISPLAY_RST

#define GPIO_GPC10              S5PV210_GPC1(0)
// TODO:FORTE Temp_code  EARPATH_SEL not used
#define GPIO_EARPATH_SEL        S5PV210_GPC1(0)
#define GPIO_GPC11              S5PV210_GPC1(1)
#define GPIO_GPC12              S5PV210_GPC1(2)
#define GPIO_GPC13              S5PV210_GPC1(3)
#define GPIO_GPC14              S5PV210_GPC1(4)

#define GPIO_BOOT_MODE          S5PV210_GPD0(0)
#define GPIO_VIBTONE_PWM        S5PV210_GPD0(1)
#define GPIO_MSENSE_nRST        S5PV210_GPD0(2)
#define GPIO_TOUCH_INT          S5PV210_GPD0(3)     /* TSP_INT  */

#define GPIO_CAM_SDA_28V        S5PV210_GPD1(0)
#define GPIO_CAM_SDA_28V_AF     2
#define GPIO_CAM_SCL_28V        S5PV210_GPD1(1)
#define GPIO_CAM_SCL_28V_AF     2

#define GPIO_AP_SDA_28V         S5PV210_GPD1(2)
#define AP_I2C_SDA_28V          S5PV210_GPD1(2)
#define AP_I2C_SDA_28V_AF       2
#define GPIO_AP_SCL_28V         S5PV210_GPD1(3)
#define AP_I2C_SCL_28V          S5PV210_GPD1(3)
#define AP_I2C_SCL_28V_AF       2

#define GPIO_TSP_SDA_28V        S5PV210_GPD1(4)
#define GPIO_TSP_SDA_28V_AF     2
#define GPIO_TSP_SCL_28V        S5PV210_GPD1(5)
#define GPIO_TSP_SCL_28V_AF     2

////////////////////////////////////////////////////////////////////////////////
#define GPIO_CAM_PCLK           S5PV210_GPE0(0)
#define GPIO_CAM_PCLK_AF        2
#define GPIO_CAM_VSYNC          S5PV210_GPE0(1)
#define GPIO_CAM_VSYNC_AF       2
#define GPIO_CAM_HSYNC          S5PV210_GPE0(2)
#define GPIO_CAM_HSYNC_AF       2
#define GPIO_CAM_D0             S5PV210_GPE0(3)
#define GPIO_CAM_D0_AF          2
#define GPIO_CAM_D1             S5PV210_GPE0(4)
#define GPIO_CAM_D1_AF          2
#define GPIO_CAM_D2             S5PV210_GPE0(5)
#define GPIO_CAM_D2_AF          2
#define GPIO_CAM_D3             S5PV210_GPE0(6)
#define GPIO_CAM_D3_AF          2
#define GPIO_CAM_D4             S5PV210_GPE0(7)
#define GPIO_CAM_D4_AF          2
#define GPIO_CAM_D5             S5PV210_GPE1(0)
#define GPIO_CAM_D5_AF          2
#define GPIO_CAM_D6             S5PV210_GPE1(1)
#define GPIO_CAM_D6_AF          2
#define GPIO_CAM_D7             S5PV210_GPE1(2)
#define GPIO_CAM_D7_AF          2

#define GPIO_CAM_MCLK           S5PV210_GPE1(3)
#define GPIO_CAM_MCLK_AF        2
#define GPIO_CAM_MEGA_nRST      S5PV210_GPE1(4) /* CAM_RST */

#define GPIO_DISPLAY_HSYNC      S5PV210_GPF0(0)
#define GPIO_DISPLAY_HSYNC_AF   2
#define GPIO_DISPLAY_VSYNC      S5PV210_GPF0(1)
#define GPIO_DISPLAY_VSYNC_AF   2
#define GPIO_DISPLAY_DE         S5PV210_GPF0(2)
#define GPIO_DISPLAY_DE_AF      2
#define GPIO_DISPLAY_PCLK       S5PV210_GPF0(3)
#define GPIO_DISPLAY_PCLK_AF    2

#define GPIO_LCD_D0             S5PV210_GPF0(4)
#define GPIO_LCD_D0_AF          2
#define GPIO_LCD_D1             S5PV210_GPF0(5)
#define GPIO_LCD_D1_AF          2
#define GPIO_LCD_D2             S5PV210_GPF0(6)
#define GPIO_LCD_D2_AF          2
#define GPIO_LCD_D3             S5PV210_GPF0(7)
#define GPIO_LCD_D3_AF          2
#define GPIO_LCD_D4             S5PV210_GPF1(0)
#define GPIO_LCD_D4_AF          2
#define GPIO_LCD_D5             S5PV210_GPF1(1)
#define GPIO_LCD_D5_AF          2
#define GPIO_LCD_D6             S5PV210_GPF1(2)
#define GPIO_LCD_D6_AF          2
#define GPIO_LCD_D7             S5PV210_GPF1(3)
#define GPIO_LCD_D7_AF          2
#define GPIO_LCD_D8             S5PV210_GPF1(4)
#define GPIO_LCD_D8_AF          2
#define GPIO_LCD_D9             S5PV210_GPF1(5)
#define GPIO_LCD_D9_AF          2
#define GPIO_LCD_D10            S5PV210_GPF1(6)
#define GPIO_LCD_D10_AF         2
#define GPIO_LCD_D11            S5PV210_GPF1(7)
#define GPIO_LCD_D11_AF         2
#define GPIO_LCD_D12            S5PV210_GPF2(0)
#define GPIO_LCD_D12_AF         2
#define GPIO_LCD_D13            S5PV210_GPF2(1)
#define GPIO_LCD_D13_AF         2
#define GPIO_LCD_D14            S5PV210_GPF2(2)
#define GPIO_LCD_D14_AF         2
#define GPIO_LCD_D15            S5PV210_GPF2(3)
#define GPIO_LCD_D15_AF         2
#define GPIO_LCD_D16            S5PV210_GPF2(4)
#define GPIO_LCD_D16_AF         2
#define GPIO_LCD_D17            S5PV210_GPF2(5)
#define GPIO_LCD_D17_AF         2
#define GPIO_LCD_D18            S5PV210_GPF2(6)
#define GPIO_LCD_D18_AF         2
#define GPIO_LCD_D19            S5PV210_GPF2(7)
#define GPIO_LCD_D19_AF         2
#define GPIO_LCD_D20            S5PV210_GPF3(0)
#define GPIO_LCD_D20_AF         2
#define GPIO_LCD_D21            S5PV210_GPF3(1)
#define GPIO_LCD_D21_AF         2
#define GPIO_LCD_D22            S5PV210_GPF3(2)
#define GPIO_LCD_D22_AF         2
#define GPIO_LCD_D23            S5PV210_GPF3(3)
#define GPIO_LCD_D23_AF         2

#define GPIO_CODEC_LDO_EN       S5PV210_GPF3(4)
#define GPIO_LTE_WAKE_INT       S5PV210_GPF3(5)

#define GPIO_NAND_CLK           S5PV210_GPG0(0)
#define GPIO_NAND_CLK_AF        2
#define GPIO_NAND_CMD           S5PV210_GPG0(1)
#define GPIO_NAND_CMD_AF        2

#define GPIO_AP_PMIC_SCL        S5PV210_GPG0(2)
#define GPIO_AP_PMIC_SCL_AF     2

#define GPIO_NAND_D0            S5PV210_GPG0(3)
#define GPIO_NAND_D0_AF         2
#define GPIO_NAND_D1            S5PV210_GPG0(4)
#define GPIO_NAND_D1_AF         2
#define GPIO_NAND_D2            S5PV210_GPG0(5)
#define GPIO_NAND_D2_AF         2
#define GPIO_NAND_D3            S5PV210_GPG0(6)
#define GPIO_NAND_D3_AF         2

#define GPIO_WLAN_SDIO_CLK      S5PV210_GPG1(0)
#define GPIO_WLAN_SDIO_CLK_AF   2
#define GPIO_WLAN_SDIO_CMD      S5PV210_GPG1(1)
#define GPIO_WLAN_SDIO_CMD_AF   2
#define GPIO_WLAN_nRST          S5PV210_GPG1(2)
#define GPIO_WLAN_RST           GPIO_WLAN_nRST
#define GPIO_WLAN_nRST_AF       1
#define GPIO_WLAN_SDIO_D0       S5PV210_GPG1(3)
#define GPIO_WLAN_SDIO_D0_AF    2
#define GPIO_WLAN_SDIO_D1       S5PV210_GPG1(4)
#define GPIO_WLAN_SDIO_D1_AF    2
#define GPIO_WLAN_SDIO_D2       S5PV210_GPG1(5)
#define GPIO_WLAN_SDIO_D2_AF    2
#define GPIO_WLAN_SDIO_D3       S5PV210_GPG1(6)
#define GPIO_WLAN_SDIO_D3_AF    2

#define GPIO_T_FLASH_CLK        S5PV210_GPG2(0)
#define GPIO_T_FLASH_CLK_AF     2
#define GPIO_T_FLASH_CMD        S5PV210_GPG2(1)
#define GPIO_T_FLASH_CMD_AF     2

#define GPIO_DISPLAY_BL_EN      S5PV210_GPG2(2)

#define GPIO_T_FLASH_D0         S5PV210_GPG2(3)
#define GPIO_T_FLASH_D0_AF      2
#define GPIO_T_FLASH_D1         S5PV210_GPG2(4)
#define GPIO_T_FLASH_D1_AF      2
#define GPIO_T_FLASH_D2         S5PV210_GPG2(5)
#define GPIO_T_FLASH_D2_AF      2
#define GPIO_T_FLASH_D3         S5PV210_GPG2(6)
#define GPIO_T_FLASH_D3_AF      2

#define GPIO_FM_SDA_28V         S5PV210_GPG3(0)
#define GPIO_FM_SDA_AF          2

#define GPIO_MASSMEMORY_EN      S5PV210_GPG3(1)

#define GPIO_FM_SCL_28V         S5PV210_GPG3(2)
#define GPIO_FM_SCL_AF          2

#define GPIO_BT_nRST            S5PV210_GPG3(3)
#define GPIO_BT_RST             GPIO_BT_nRST
#define GPIO_BT_WAKE            S5PV210_GPG3(4)
#define GPIO_WLAN_BT_EN         S5PV210_GPG3(5)
#define GPIO_WLAN_BT_EN_AF		1

#define GPIO_AP_PMIC_SDA        S5PV210_GPG3(6)
#define GPIO_AP_PMIC_SDA_AF     2
#define GPIO_AP_PS_HOLD         S5PV210_GPH0(0)
#define GPIO_AP_PS_HOLD_AF      1
#define GPIO_PS_VOUT            S5PV210_GPH0(1)
#define GPIO_PS_VOUT_AF         0xFF

#define GPIO_3P_SEND_END        S5PV210_GPH0(2)
#define GPIO_3P_SEND_END_AF     0
#define GPIO_EAR_SEND_END_OPEN      GPIO_3P_SEND_END
#define GPIO_EAR_SEND_END_OPEN_AF   0xF

#define GPIO_WLAN_HOST_WAKE     S5PV210_GPH0(3)
#define GPIO_WLAN_HOST_WAKE_AF  0xF
#define GPIO_BT_HOST_WAKE       S5PV210_GPH0(4)
#define GPIO_HALL_SW            S5PV210_GPH0(5)
#define GPIO_EAR_DET            S5PV210_GPH0(6)
#define GPIO_EAR_DET_AF         0xFF
#define GPIO_AP_PMIC_IRQ        S5PV210_GPH0(7)
#define GPIO_AP_PMIC_IRQ_AF     0xFF

#define GPIO_LTE_ACTIVE         S5PV210_GPH1(0)
#define GPIO_LTE_ACTIVE_AF      0xFF
#define IRQ_LTE_ACTIVE          IRQ_EINT8

#define GPIO_C111_DPRAM_INT_N   S5PV210_GPH1(1)
#define GPIO_GAUGE_ALT          S5PV210_GPH1(2)

#define GPIO_nINT_ONEDRAM_AP    S5PV210_GPH1(3)
#define GPIO_nINT_ONEDRAM_AP_AF 0xF
#define IRQ_ONEDRAM_INT_N       IRQ_EINT11
//3
#define GPIO_LTE_RST_N          S5PV210_GPH1(4)     /* CMC_RST */
#define GPIO_LTE_RST_N_AF       1
#define GPIO_EAR_SEND_END       S5PV210_GPH1(5)
#define GPIO_EAR_SEND_END_AF    0xFF
#define GPIO_LTE_PWRON          S5PV210_GPH1(6)     /* 220_PMIC_PWRON */
#define GPIO_LTE_PWRON_AF       1
#define GPIO_PHONE_ACTIVE       S5PV210_GPH1(7)     /* QSC_ACTIVE   */
#define GPIO_PHONE_ACTIVE_AF    2

#define GPIO_QSC_ACTIVE         S5PV210_GPH1(7)
#define GPIO_QSC_ACTIVE_AF      0xFF
#define IRQ_QSC_ACTIVE          IRQ_EINT15

#define GPIO_KEYSCAN0           S5PV210_GPH2(0)
#define GPIO_KEYSCAN0_AF        3
#define GPIO_KEYSCAN1           S5PV210_GPH2(1)
#define GPIO_KEYSCAN1_AF        3
#define GPIO_KEYSCAN2           S5PV210_GPH2(2)
#define GPIO_KEYSCAN2_AF        3
#define GPIO_KEYSCAN3           S5PV210_GPH2(3)
#define GPIO_KEYSCAN3_AF        3
#define GPIO_KEYSCAN4           S5PV210_GPH2(4)
#define GPIO_KEYSCAN4_AF        3
#define GPIO_KEYSCAN5           S5PV210_GPH2(5)
#define GPIO_KEYSCAN5_AF        3

#define GPIO_nPOWER             S5PV210_GPH2(6)
#define GPIO_N_POWER            GPIO_nPOWER
#define GPIO_N_POWER_AF         2

#define GPIO_JACK_nINT          S5PV210_GPH2(7)
#define GPIO_JACK_nINT_AF       0xFF

#define GPIO_KEYSENSE0          S5PV210_GPH3(0)
#define GPIO_KEYSENSE0_AF       3
#define GPIO_KEYSENSE1          S5PV210_GPH3(1)
#define GPIO_KEYSENSE1_AF       3
#define GPIO_KEYSENSE2          S5PV210_GPH3(2)
#define GPIO_KEYSENSE2_AF       3
#define GPIO_KEYSENSE3          S5PV210_GPH3(3)
#define GPIO_KEYSENSE3_AF       3
#define GPIO_KEYSENSE4          S5PV210_GPH3(4)
#define GPIO_KEYSENSE4_AF       3
#define GPIO_KEYSENSE5          S5PV210_GPH3(5)
#define GPIO_KEYSENSE5_AF       3
#define GPIO_KEYSENSE6          S5PV210_GPH3(6)
#define GPIO_KEYSENSE6_AF       3

#define GPIO_T_FLASH_DETECT     S5PV210_GPH3(7)

#define GPIO_CODEC_I2S_CLK      S5PV210_GPI(0)      /* AP_I2S_CLK */
#define GPIO_CODEC_I2S_CLK_AF   2

#define GPIO_GPI1               S5PV210_GPI(1)

#define GPIO_CODEC_I2S_WS       S5PV210_GPI(2)      /* AP_I2S_SYNC */
#define GPIO_CODEC_I2S_WS_AF    2
#define GPIO_CODEC_I3S_DI       S5PV210_GPI(3)      /* AP_I3S_DI    */
#define GPIO_CODEC_I3S_DI_AF    2
#define GPIO_CODEC_I3S_DO       S5PV210_GPI(4)      /* AP_I3S_DO    */
#define GPIO_CODEC_I3S_DO_AF    2

#define GPIO_GPI5               S5PV210_GPI(5)
#define GPIO_GPI6               S5PV210_GPI(6)

#define GPIO_POPUP_WN_EN        S5PV210_GPJ0(0)     /* not used? */

// ??
#define GPIO_MSENSE_SCL_28V     GPIO_GPI5   // TEMP CODE
#define GPIO_MSENSE_SCL_28V_AF  2
#define GPIO_MSENSE_SDA_28V     GPIO_GPI6   // TEMP CODE
#define GPIO_MSENSE_SDA_28V_AF  2

#define GPIO_BUCK_1_EN_B        S5PV210_GPJ0(1)

#define GPIO_GPJ02              S5PV210_GPJ0(2)
#define GPIO_GPJ03              S5PV210_GPJ0(3)
#define GPIO_GPJ04              S5PV210_GPJ0(4)
#define GPIO_GPJ05              S5PV210_GPJ0(5)
#define GPIO_GPJ06              S5PV210_GPJ0(6)
#define GPIO_GPJ07              S5PV210_GPJ0(7)

#define GPIO_PHONE_ON           S5PV210_GPJ1(0)
#define GPIO_QSC_PHONE_ON       GPIO_PHONE_ON

#define GPIO_VIBTONE_EN1        S5PV210_GPJ1(1)     /* GPIO_VIBTONE_EN  */
#define GPIO_MICBIAS_EN         S5PV210_GPJ1(2)
#define GPIO_TOUCH_EN           S5PV210_GPJ1(3)     /* TSP_LDO_ON */
#define GPIO_TOUCH_EN_AF        1

#define GPIO_GPJ14              S5PV210_GPJ1(4)
#define GPIO_GPJ15              S5PV210_GPJ1(5)

#define GPIO_CP_EBI2_AD0        S5PV210_GPJ2(0)
#define GPIO_CP_EBI2_AD1        S5PV210_GPJ2(1)
#define GPIO_CP_EBI2_AD2        S5PV210_GPJ2(2)
#define GPIO_CP_EBI2_AD3        S5PV210_GPJ2(3)
#define GPIO_CP_EBI2_AD4        S5PV210_GPJ2(4)
#define GPIO_CP_EBI2_AD5        S5PV210_GPJ2(5)
#define GPIO_CP_EBI2_AD6        S5PV210_GPJ2(6)
#define GPIO_CP_EBI2_AD7        S5PV210_GPJ2(7)
#define GPIO_CP_EBI2_AD8        S5PV210_GPJ3(0)
#define GPIO_CP_EBI2_AD9        S5PV210_GPJ3(1)
#define GPIO_CP_EBI2_AD10       S5PV210_GPJ3(2)
#define GPIO_CP_EBI2_AD11       S5PV210_GPJ3(3)
#define GPIO_CP_EBI2_AD12       S5PV210_GPJ3(4)
#define GPIO_CP_EBI2_AD13       S5PV210_GPJ3(5)
#define GPIO_CP_EBI2_AD14       S5PV210_GPJ3(6)
#define GPIO_CP_EBI2_AD15       S5PV210_GPJ3(7)

#define GPIO_CP_DPRAM_CS_N      S5PV210_GPJ4(0)
#define GPIO_CP_EBI2_WE_N       S5PV210_GPJ4(1)
#define GPIO_CP_EBI2_OE_N       S5PV210_GPJ4(2)
#define GPIO_DPRAM_INT_CP_N     S5PV210_GPJ4(3)
#define GPIO_CP_EBI2_ADV_N      S5PV210_GPJ4(4)
#define GPIO_MAIN_KEY_LED_EN    S5PV210_MP01(0)

#define GPIO_DISPLAY_CS         S5PV210_MP01(1)     /* LCD_CS   */
#define GPIO_DISPLAY_CS_AF      S3C_GPIO_SFN(1)
#define GPIO_LTE_OP_MODE_0      S5PV210_MP01(2)
#define GPIO_UART_SEL1          S5PV210_MP01(3)
#define GPIO_AP_NANDCS          S5PV210_MP01(4)
#define GPIO_AP_NANDCS_AF       5
#define GPIO_LTE_OP_MODE_1      S5PV210_MP01(5)

#define GPIO_MP016              S5PV210_MP01(6)
#define GPIO_MP017              S5PV210_MP01(7)

#define GPIO_ALS_EN             S5PV210_MP02(0)
#define GPIO_PS_ON              GPIO_ALS_EN
#define GPIO_USB_SEL            S5PV210_MP02(1)
#define GPIO_VCC_18V_PDA        S5PV210_MP02(2)

#define GPIO_MP023              S5PV210_MP02(3)
#define GPIO_MP030              S5PV210_MP03(0)
#define GPIO_MP031              S5PV210_MP03(1)
#define GPIO_MP032              S5PV210_MP03(2)

#define GPIO_PDA_ACTIVE         S5PV210_MP03(3)
#define GPIO_PDA_ACTIVE_AF      1

#define GPIO_VCC_28V_PDA        S5PV210_MP03(4)

#define GPIO_MP035              S5PV210_MP03(5)
#define GPIO_MP036              S5PV210_MP03(6)
#define GPIO_MP037              S5PV210_MP03(7)

#define GPIO_MP040              S5PV210_MP04(0)
#define GPIO_MP041              S5PV210_MP04(1)
#define GPIO_MP042              S5PV210_MP04(2)
#define GPIO_MP043              S5PV210_MP04(3)
#define GPIO_MP044              S5PV210_MP04(4)
#define GPIO_MP045              S5PV210_MP04(5)
#define GPIO_MP046              S5PV210_MP04(6)
#define GPIO_MP047              S5PV210_MP04(7)

#define FUEL_SCL_18V            S5PV210_MP05(0)  /* GPIO_FUEL_SCL_18V   */
#define FUEL_SCL_18V_AF         3
#define FUEL_SDA_18V            S5PV210_MP05(1)  /* GPIO_FUEL_SDA_18V   */
#define FUEL_SDA_18V_AF         3

#define GPIO_AP_SDA_18V         S5PV210_MP05(2)
#define GPIO_AP_SCL_18V         S5PV210_MP05(3)
#define GPIO_MP054              S5PV210_MP05(4)
#define GPIO_LTE_DP_INT         S5PV210_MP05(5)
#define GPIO_MP056              S5PV210_MP05(6)
#define GPIO_UART_SEL           S5PV210_MP05(7)

#define GPIO_MP060              S5PV210_MP06(0)

#define GPIO_MP061              S5PV210_MP06(1)
#define GPIO_MP062              S5PV210_MP06(2)
#define GPIO_MP063              S5PV210_MP06(3)
#define GPIO_MP064              S5PV210_MP06(4)
#define GPIO_MP065              S5PV210_MP06(5)
#define GPIO_MP066              S5PV210_MP06(6)
#define GPIO_MP067              S5PV210_MP06(7)

#define GPIO_MP070              S5PV210_MP07(0)
#define GPIO_MP071              S5PV210_MP07(1)
#define GPIO_MP072              S5PV210_MP07(2)
#define GPIO_MP073              S5PV210_MP07(3)
#define GPIO_MP074              S5PV210_MP07(4)
#define GPIO_MP075              S5PV210_MP07(5)
#define GPIO_MP076              S5PV210_MP07(6)
#define GPIO_MP077              S5PV210_MP07(7)

// ???
#define GPIO_SUB_KEY_LED_EN     GPIO_GPC14   // TEMP CODE
/* uart 0~3 */
#define GPIO_BT_RXD             GPIO_BT_UART_RXD
#define GPIO_BT_RXD_AF          2
#define GPIO_BT_TXD             GPIO_BT_UART_TXD
#define GPIO_BT_TXD_AF          2
#define GPIO_BT_CTS             GPIO_BT_UART_CTS
#define GPIO_BT_CTS_AF          2
#define GPIO_BT_RTS             GPIO_BT_UART_RTS
#define GPIO_BT_RTS_AF          2

#define _3_GPIO_TOUCH_INT       GPIO_TOUCH_INT
#define _3_GPIO_TOUCH_EN        GPIO_TOUCH_EN
#define _3_GPIO_TOUCH_EN_AF     GPIO_TOUCH_EN_AF
#define _3_GPIO_TOUCH_CE        GPIO_TOUCH_EN
#define _3_GPIO_TOUCH_ST_AF     GPIO_TOUCH_EN_AF

#define GPIO_ALS_SCL_28V        GPIO_FM_SCL_28V
#define GPIO_ALS_SCL_28V_AF     2
#define GPIO_ALS_SDA_28V        GPIO_FM_SDA_28V
#define GPIO_ALS_SDA_28V_AF     2

#define GPIO_DET_35             GPIO_EAR_DET
#define GPIO_DET_35_AF          0xFF
#define GPIO_EAR_BIAS_EN        GPIO_MICBIAS_EN
#define GPIO_EAR_BIAS_EN_AF     1

// dummies
#define GPIO_DUMMY              GPIO_GPC14
#define GPIO_GPS_RXD            GPIO_DUMMY
#define GPIO_GPS_RXD_AF         2
#define GPIO_GPS_TXD            GPIO_DUMMY
#define GPIO_GPS_TXD_AF         2
#define GPIO_GPS_CTS            GPIO_DUMMY
#define GPIO_GPS_CTS_AF         2
#define GPIO_GPS_RTS            GPIO_DUMMY
#define GPIO_GPS_RTS_AF         2

#define GPIO_FLM_RXD            GPIO_DUMMY
#define GPIO_FLM_RXD_AF         2
#define GPIO_FLM_TXD            GPIO_DUMMY
#define GPIO_FLM_TXD_AF         2

#define GPIO_USB_SDA_28V        AP_I2C_SDA_28V
#define GPIO_USB_SCL_28V        AP_I2C_SCL_28V
#define _3_TOUCH_SDA_28V        GPIO_TSP_SDA_28V
#define _3_TOUCH_SCL_28V        GPIO_TSP_SCL_28V

#define GPIO_GPS_nRST           GPIO_DUMMY
#define GPIO_GPS_PWR_EN         GPIO_DUMMY
#define GPIO_GPS_CLK_INT        GPIO_DUMMY
#define GPIO_CAM_VGA_nSTBY      GPIO_DUMMY
#define GPIO_CAM_VGA_nRST       GPIO_DUMMY
#define GPIO_GPB7               GPIO_DUMMY
#define GPIO_TV_EN              GPIO_DUMMY
#define GPIO_BATT_ID            GPIO_DUMMY
#define GPIO_PCM_SEL            GPIO_DUMMY

#endif  /* ARIES.H */
