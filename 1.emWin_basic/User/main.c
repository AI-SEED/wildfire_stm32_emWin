/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   ����led
  ******************************************************************************
  * @attention
  *
  * ʵ��ƽ̨:Ұ�� iSO STM32 ������ 
  * ��̳    :http://www.chuxue123.com
  * �Ա�    :http://firestm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "stm32f10x.h"

#include "GUI.h"

#include "bsp_usart1.h"
#include "bsp_SysTick.h"
#include "bsp_led.h"




/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
  
  /* ��ʼ����ʱ�� */
	SysTick_Init();
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
	GUI_Init();
	
	/* ��ʼ������ */
	USART1_Config();
	
	/* ���ڵ�����Ϣ */
	DEBUG("\r\n wildfire ISO board emWin test!\r\n");
 
	/* ��ʾ���� */
	GUI_DispString("wildfire ISO board emWin test!");
	
	/* ����2��ʹ�ù̼������IO */
	while (1)
	{
		LED1( ON );			  // ��
    GUI_Delay(1000);
		LED1( OFF );		  // ��

		LED2( ON );			  // ��
    GUI_Delay(1000);
		LED2( OFF );		  // ��

		LED3( ON );			  // ��
    GUI_Delay(1000);
		LED3( OFF );		  // ��	   
	}

}


/*********************************************END OF FILE**********************/
