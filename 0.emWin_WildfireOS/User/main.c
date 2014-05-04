/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   测试led
  ******************************************************************************
  * @attention
  *
  * 实验平台:野火 iSO STM32 开发板 
  * 论坛    :http://www.chuxue123.com
  * 淘宝    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"
#include "GUI.h"
#include "diskio.h"
#include "ff.h"

#include "bsp_led.h"
#include "bsp_touch.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_fsmc_sram.h"
#include "bsp_rtc.h"
#include "bsp_exti.h"

#include "WFGUI_DeskTop.h"
#include "WFGUI_Common.h"

extern void Touch_MainTask(void);
//extern void WFGUI_MainTask(void);
/*时间结构体*/
struct rtc_time systmtime;
//char path[500]="0:";
	FATFS fsys;

void BSP_Init(void)
{
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/* 初始化触屏 */
	Touch_Init();
	
	/* 初始化定时器 */
	SysTick_Init();
	
	/* 配置 FSMC Bank1 NOR/SRAM3 */
  FSMC_SRAM_Init();
	
	/* Sdio Interrupt Config */
	//NVIC_Configuration();	
	
	/*初始化sd卡*/
	disk_initialize(0);
	
	f_mount(0,&fsys);								//注册文件系统工作区	// TBD 增加sd卡检测

	/* Enable the CRC Module */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);//不知道为什么一定要开crc时钟
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
	
	
	/* 初始化GUI */
	GUI_Init();

	/* 初始化串口*/
	USART1_Config();
	
	/* 配置RTC秒中断优先级 */
	RTC_NVIC_Config();

	RTC_CheckAndConfig(&systmtime);
	
	EXTI_Key_Config();
	

	
	DEBUG("\r\n wildfire ISO board emWin test \r\n");


}


extern uint8_t key_flag;
/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	
	BSP_Init();
	
	GUI_Delay (20);
	
	#if 0
	/* 触摸校准demo */
	Touch_MainTask();
	
	#else	
	//f_mount(0,&fsys);								//注册文件系统工作区	// TBD 增加sd卡检测

	WFGUI_MainTask();	
	
	while(1)
				{

    GUI_Delay(50); //窗口在gui delay期间自动重绘，必须要有gui_delay
  }
	#endif

}


/*********************************************END OF FILE**********************/
