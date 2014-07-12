#include "bsp.h"
#include "GUI.h"

/*
 * ��������BSP_Init
 * ����  ��ʱ�ӳ�ʼ����Ӳ����ʼ��
 * ����  ����
 * ���  ����
 */
void BSP_Init(void)
{
  SysTick_init();	
	
	/* Enable the CRC Module */
  /*CRC��emWinû�й�ϵ��ֻ������Ϊ�˿�ı��������ģ�
  ����STemWin�Ŀ�ֻ������ST��оƬ���棬���оƬ���޷�ʹ�õġ� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
	 
	/* LED �˿ڳ�ʼ�� */
  LED_GPIO_Config();
	
	/* ��ʼ������ */
	Touch_Init();
	
	/* ��ʼ��gui */
	GUI_Init();
	
}

/*
 * ��������SysTick_init
 * ����  ������SysTick��ʱ��
 * ����  ����
 * ���  ����
 */
void SysTick_init(void)
{
  /* ��ʼ����ʹ��SysTick��ʱ�� */
  SysTick_Config(SystemCoreClock/OS_CFG_TICK_RATE_HZ);
  
  /*  ����1ms �ж�һ�Σ���os��Ƶ��Ϊ1000hz */
	if (SysTick_Config(SystemCoreClock/OS_CFG_TICK_RATE_HZ))	
	{ 
		/* Capture error */ 
		while (1);
	}
}


/* --------------------------------------end of file--------------------------------------- */
