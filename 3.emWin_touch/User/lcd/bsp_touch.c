/*
*********************************************************************************************************
*
*	ģ������ : TFTҺ����ʾ������ģ��
*	�ļ����� : bsp_ili9341_lcd.c
*	��    �� : V1.0
*	˵    �� : ����оƬ�ķ��ʵ�ַΪ:  0x60000000
*				
*	�޸ļ�¼ :
*		�汾��    ����          ����                 ˵��
*		v1.0    2011-10-31   WildFire Team  ST�̼���汾 V3.5.0�汾��
*
*	Copyright (C), 2012-2013, WildFire Team
*
*********************************************************************************************************
*/
#include "bsp_touch.h"
#include "bsp_gpio_spi.h"


#define XPT2046_CS_0()      GPIOB->BRR  =  GPIO_Pin_7
#define XPT2046_CS_1()      GPIOB->BSRR =  GPIO_Pin_7
#define TOUCH_PressValid    GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6)

#if 0
/* XPT2046 �ڲ�ADCͨ���� */
#define ADC_CH_X	1		/* Xͨ��������Xλ�� */
#define ADC_CH_Y	5		/* Yͨ��������Yλ�� */
#endif

/* XPT2046 �ڲ�ADCͨ���� */
#define ADC_CH_X	0x90		/* Xͨ��������Xλ�� */
#define ADC_CH_Y	0xD0	/* Yͨ��������Yλ�� */

/* ����ģ���õ���ȫ�ֱ��� */
TOUCH_T g_tTP;

int32_t TOUCH_Abs(int32_t x);



/* 
 * ����ģ��SPI IO �� �ж� IO ��ʼ��
 */
void Touch_Init(void)
{
  GPIO_SPI_Config();
}

/*
 * us ������ʱ�����Ǻܾ�ȷ
 */
static void DelayUS(vu32 cnt)
{
    uint16_t i;
    for(i = 0;i<cnt;i++)
    {
        uint8_t us = 12; /* ����ֵΪ12����Լ��1΢�� */    
        while (us--)     /* ��1΢��	*/
        {
            ;   
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: XPT2046_WriteCMD
*	����˵��: д����
*	��    �Σ�CHX 	0x90 	//ͨ��Y+��ѡ������� CHY 	0xd0	//ͨ��X+��ѡ�������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void XPT2046_WriteCMD(unsigned char cmd) 
{
    unsigned char buf;
    unsigned char i;
//     TP_CS(1);
    TP_DIN(0);
    TP_DCLK(0);
//     TP_CS(0);
    for(i=0;i<8;i++) 
    {
        buf=(cmd>>(7-i))&0x1;
        TP_DIN(buf);
        //Delayus(5);
      DelayUS(5);
        TP_DCLK(1);
        //Delayus(5);
      DelayUS(5);
        TP_DCLK(0);
    }
}

/*
*********************************************************************************************************
*	�� �� ��: XPT2046_ReadCMD
*	����˵��: ѡ��һ��ģ��ͨ��������ADC��������ADC�������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/

static unsigned short XPT2046_ReadCMD(void) 
{
    unsigned short buf=0,temp;
    unsigned char i;
    TP_DIN(0);
    TP_DCLK(1);
    for(i=0;i<12;i++) 
    {
        TP_DCLK(0);          
        temp= (TP_DOUT) ? 1:0;
        buf|=(temp<<(11-i));
        TP_DCLK(1);
    }
    buf&=0x0fff;

    return(buf);
}

/*
*********************************************************************************************************
*	�� �� ��: TSC2046_ReadAdc
*	����˵��: ѡ��һ��ģ��ͨ��������ADC��������ADC�������
*	��    �Σ�_ucCh = 0x90 ��ʾYͨ���� 0xd0 ��ʾXͨ��
*	�� �� ֵ: 12λADCֵ
*********************************************************************************************************
*/
uint16_t XPT2046_ReadAdc(uint8_t _ucCh)
{
	  //uint16_t usAdc;

    XPT2046_WriteCMD(_ucCh);

	return 	XPT2046_ReadCMD();
}


/*
*********************************************************************************************************
*	�� �� ��: TOUCH_DataFilter
*	����˵��: ��ȡһ������ֵ(x����y)
*             ������ȡXPT2046_READ_TIMES������,����Щ������������,
*             Ȼ��ȥ����ͺ����XPT2046_LOST_VAL����,ȡƽ��ֵ 
*	��    �Σ���
*	�� �� ֵ: ����������
*********************************************************************************************************
*/
/* ��ȡ���� */
#define XPT2046_READ_TIMES    5 
/* ����ֵ  */	
#define XPT2046_LOST_VAL      1	  	
uint16_t TOUCH_DataFilter(uint8_t _ucCh)
{
	uint16_t i, j;
	uint16_t buf[XPT2046_READ_TIMES];
	uint16_t usSum;
	uint16_t usTemp;

	/* ��ȡREAD_TIMES������*/
	for(i=0; i < XPT2046_READ_TIMES; i++)
	{
		buf[i] = XPT2046_ReadAdc(_ucCh);
	}
	
	/* �������� */		 		    
	for(i = 0; i < XPT2046_READ_TIMES - 1; i++)
	{
		for(j = i + 1; j < XPT2046_READ_TIMES; j++)
		{
			if(buf[i] > buf[j])
			{
				usTemp = buf[i];
				buf[i] = buf[j];
				buf[j] = usTemp;
			}
		}
	}
		  
	usSum = 0;

	/*��� */
	for(i = XPT2046_LOST_VAL; i < XPT2046_READ_TIMES - XPT2046_LOST_VAL; i++)
	{
		usSum += buf[i];
	}
	/*��ƽ�� */
	usTemp = usSum / (XPT2046_READ_TIMES - 2 * XPT2046_LOST_VAL);

	return usTemp;   
} 

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_ReadAdcXY
*	����˵��: ����2�ζ�ȡ������IC,�������ε�ƫ��ܳ���
*             ADC_ERR_RANGE,��������,����Ϊ������ȷ,�����������.	   
*             �ú����ܴ�����׼ȷ��
*	��    �Σ�x,y:��ȡ��������ֵ
*	�� �� ֵ: 0,ʧ��;1,�ɹ�
*********************************************************************************************************
*/
/* ��Χ */  
uint8_t ADC_ERR_RANGE = 5; 
uint8_t TOUCH_ReadAdcXY(uint16_t *_usX, uint16_t *_usY) 
{
	uint16_t iX1, iY1;
 	uint16_t iX2, iY2;
	uint16_t iX, iY;

    iX1 = TOUCH_DataFilter(ADC_CH_Y);
	iY1 = TOUCH_DataFilter(ADC_CH_X);
	iX2 = TOUCH_DataFilter(ADC_CH_Y);
	iY2 = TOUCH_DataFilter(ADC_CH_X);
	
	iX = TOUCH_Abs(iX1 - iX2);
	iY = TOUCH_Abs(iY1 - iY2); 
	
	/* ǰ�����β�����+-ERR_RANGE�� */  
    if ((iX <= ADC_ERR_RANGE) && (iY <= ADC_ERR_RANGE))
    {       
		*_usX = (iX1 + iX2) / 2;
        *_usY = (iY1 + iY2) / 2;

        return 1;
    }
	else 
	{
		return 0;
	}	  
} 

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_Scan
*	����˵��: �������¼������򡣸ú����������Ե��ã�
*						ÿ10ms����1��.��systick�жϷ���������GUI_TOUCH_Exec����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void TOUCH_Scan(void)
{
	//��⵽����״̬��ִ�У����gui�ٶȣ�
	if(INT_IN_2046 == 0 ) 
	{	
		#if 0
		while(!TOUCH_ReadAdcXY(&g_tTP.usAdcNowX, &g_tTP.usAdcNowY)&&s_invalid_count<5)
		{
			s_invalid_count++;
		}
		if(s_invalid_count >= 5)
		{
			g_tTP.usAdcNowX = 0;
			g_tTP.usAdcNowY = 0;	
		}
		#else
		TOUCH_ReadAdcXY(&g_tTP.usAdcNowX, &g_tTP.usAdcNowY);
		
		#endif
	}
	else
	{
		g_tTP.usAdcNowX = 0;
		g_tTP.usAdcNowY = 0;	
	}

}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_Abs
*	����˵��: �������ֵ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
int32_t TOUCH_Abs(int32_t x)
{
	if (x >= 0)
	{
		return x;
	}
	else
	{
		return -x;
	}
}

#if 0
/*
*********************************************************************************************************
*	�� �� ��: TOUCH_SaveParam
*	����˵��: ����У׼����	s_usAdcX1 s_usAdcX2 s_usAdcY1 s_usAdcX2
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TOUCH_SaveParam(void)
{

	/* ���������4���������� */
	uint16_t usBuf[5];

	usBuf[0] = g_tTP.usAdcX1;
	usBuf[1] = g_tTP.usAdcY1;

	usBuf[2] = g_tTP.usAdcX2;
	usBuf[3] = g_tTP.usAdcY2;

	usBuf[4] = g_tTP.XYChange;

}

/*
*********************************************************************************************************
*	�� �� ��: TOUCH_LoadParam
*	����˵��: ��ȡУ׼����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void TOUCH_LoadParam(void)
{

	/* ���������5���������� */
	uint16_t usBuf[5];

	/* ReadParamToBuf() */

	g_tTP.usAdcX1 = usBuf[0];
	g_tTP.usAdcY1 = usBuf[1];

	g_tTP.usAdcX2 = usBuf[2];
	g_tTP.usAdcY2 = usBuf[3];


	g_tTP.XYChange = usBuf[4];

}
#endif
