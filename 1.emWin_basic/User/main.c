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

#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"




/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */
int main(void)
{	
	/* LED 端口初始化 */
	LED_GPIO_Config();	
  
  /* 初始化定时器 */
	SysTick_Init();
	
	/*CRC和emWin没有关系，只是他们为了库的保护而做的，这样STemWin的库只能用在ST的芯片上面，别的芯片是无法使用的。 */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* 初始化GUI */
	GUI_Init();
	
	/* 初始化串口 */
	USART1_Config();
	
	/* 串口调试信息 */
	DEBUG("\r\n wildfire ISO board emWin test!\r\n");
 
	/* 显示测试 */
	GUI_DispString("wildfire ISO board emWin test!");
	
	/* 方法2，使用固件库控制IO */
	while (1)
	{
		LED1( ON );			  // 亮
    GUI_Delay(1000);
		LED1( OFF );		  // 灭

		LED2( ON );			  // 亮
    GUI_Delay(1000);
		LED2( OFF );		  // 灭

		LED3( ON );			  // 亮
    GUI_Delay(1000);
		LED3( OFF );		  // 灭	   
	}

}


/*********************************************END OF FILE**********************/
