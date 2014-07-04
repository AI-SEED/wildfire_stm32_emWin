/**
  ******************************************************************************
  * @file    main.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   emWin�ļ�ϵͳ
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
#include "bsp_led.h"
#include "GUI.h"
#include "diskio.h"
#include "bsp_touch.h"
#include "bsp_SysTick.h"
#include "bsp_usart1.h"
#include "bsp_sdio_sdcard.h"
#include "bsp_fsmc_sram.h"


extern void Touch_MainTask(void);
extern void Fatfs_MainTask(void);

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */
int main(void)
{	
	/* LED �˿ڳ�ʼ�� */
	LED_GPIO_Config();	
	
	/* ��ʼ������ */
	Touch_Init();
	
	/* ��ʼ����ʱ�� */
	SysTick_Init();
  
  /* ���� FSMC Bank1 NOR/SRAM3 */
  FSMC_SRAM_Init();
	
	/*��ʼ��sd��*/
	disk_initialize(0);  
	
	/*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	
	/* ��ʼ��GUI */
	GUI_Init();
	
	/* ��ʼ������*/
	USART1_Config();
	
	DEBUG("\r\n wildfire ISO board emWin test \r\n");
	
	GUI_Delay (20);
	
	#if 0
	/* ����У׼demo */
	Touch_MainTask();
	#else
	Fatfs_MainTask();
	#endif

}


/*********************************************END OF FILE**********************/
