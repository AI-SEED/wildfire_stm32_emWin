/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   emWin文件系统
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
#include "bsp_led.h"
#include "GUI.h"
#include "diskio.h"
#include "bsp_touch.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_sdio_sdcard.h"


extern void Touch_MainTask(void);
extern void Fatfs_MainTask(void);

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	
	
	/* 初始化触屏 */
	Touch_Init();
	
	/* 初始化定时器 */
	SysTick_Init();
	
	/*初始化sd卡*/
	disk_initialize(0);  
	
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* 初始化GUI */
	GUI_Init();
	
	/* 初始化串口*/
	USART1_Config();
	
	DEBUG("\r\n wildfire ISO board emWin test \r\n");
	
	GUI_Delay (20);
	
	#if 0
	/* 触摸校准demo */
	Touch_MainTask();
	#else
	Fatfs_MainTask();
	#endif

}


/*********************************************END OF FILE**********************/
