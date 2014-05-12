/*
*********************************************************************************************************
*
*	模块名称 : TFT液晶显示器驱动模块
*	文件名称 : bsp_ili9341_lcd.c
*	版    本 : V1.0
*	说    明 : 驱动芯片的访问地址为:  0x60000000
*				
*	修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2011-10-31   WildFire Team  ST固件库版本 V3.5.0版本。
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
/* XPT2046 内部ADC通道号 */
#define ADC_CH_X	1		/* X通道，测量X位置 */
#define ADC_CH_Y	5		/* Y通道，测量Y位置 */
#endif

/* XPT2046 内部ADC通道号 */
#define ADC_CH_X	0x90		/* X通道，测量X位置 */
#define ADC_CH_Y	0xD0	/* Y通道，测量Y位置 */

/* 触屏模块用到的全局变量 */
TOUCH_T g_tTP;

int32_t TOUCH_Abs(int32_t x);



/* 
 * 触摸模拟SPI IO 和 中断 IO 初始化
 */
void Touch_Init(void)
{
  GPIO_SPI_Config();
}

/*
 * us 级别延时，不是很精确
 */
static void DelayUS(vu32 cnt)
{
    uint16_t i;
    for(i = 0;i<cnt;i++)
    {
        uint8_t us = 12; /* 设置值为12，大约延1微秒 */    
        while (us--)     /* 延1微秒	*/
        {
            ;   
        }
    }
}

/*
*********************************************************************************************************
*	函 数 名: XPT2046_WriteCMD
*	功能说明: 写命令
*	形    参：CHX 	0x90 	//通道Y+的选择控制字 CHY 	0xd0	//通道X+的选择控制字
*	返 回 值: 无
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
*	函 数 名: XPT2046_ReadCMD
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: TSC2046_ReadAdc
*	功能说明: 选择一个模拟通道，启动ADC，并返回ADC采样结果
*	形    参：_ucCh = 0x90 表示Y通道； 0xd0 表示X通道
*	返 回 值: 12位ADC值
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
*	函 数 名: TOUCH_DataFilter
*	功能说明: 读取一个坐标值(x或者y)
*             连续读取XPT2046_READ_TIMES次数据,对这些数据升序排列,
*             然后去掉最低和最高XPT2046_LOST_VAL个数,取平均值 
*	形    参：无
*	返 回 值: 读到的数据
*********************************************************************************************************
*/
/* 读取次数 */
#define XPT2046_READ_TIMES    5 
/* 丢弃值  */	
#define XPT2046_LOST_VAL      1	  	
uint16_t TOUCH_DataFilter(uint8_t _ucCh)
{
	uint16_t i, j;
	uint16_t buf[XPT2046_READ_TIMES];
	uint16_t usSum;
	uint16_t usTemp;

	/* 读取READ_TIMES次数据*/
	for(i=0; i < XPT2046_READ_TIMES; i++)
	{
		buf[i] = XPT2046_ReadAdc(_ucCh);
	}
	
	/* 升序排列 */		 		    
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

	/*求和 */
	for(i = XPT2046_LOST_VAL; i < XPT2046_READ_TIMES - XPT2046_LOST_VAL; i++)
	{
		usSum += buf[i];
	}
	/*求平均 */
	usTemp = usSum / (XPT2046_READ_TIMES - 2 * XPT2046_LOST_VAL);

	return usTemp;   
} 

/*
*********************************************************************************************************
*	函 数 名: TOUCH_ReadAdcXY
*	功能说明: 连续2次读取触摸屏IC,且这两次的偏差不能超过
*             ADC_ERR_RANGE,满足条件,则认为读数正确,否则读数错误.	   
*             该函数能大大提高准确度
*	形    参：x,y:读取到的坐标值
*	返 回 值: 0,失败;1,成功
*********************************************************************************************************
*/
/* 误差范围 */  
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
	
	/* 前后两次采样在+-ERR_RANGE内 */  
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
*	函 数 名: TOUCH_Scan
*	功能说明: 触摸板事件检测程序。该函数被周期性调用，
*						每10ms调用1次.由systick中断服务程序调用GUI_TOUCH_Exec触发
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void TOUCH_Scan(void)
{
	static uint8_t init = 0 ;
	
	//刚上电的前5次默认执行（否则有时显示字体模糊，原因不明），之后检测到触摸状态才执行（提高gui速度）
	if(INT_IN_2046 == 0 || init < 5 ) 
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
		init ++;
		
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
*	函 数 名: TOUCH_Abs
*	功能说明: 计算绝对值
*	形    参：无
*	返 回 值: 无
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
*	函 数 名: TOUCH_SaveParam
*	功能说明: 保存校准参数	s_usAdcX1 s_usAdcX2 s_usAdcY1 s_usAdcX2
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void TOUCH_SaveParam(void)
{

	/* 保存下面的4个变量即可 */
	uint16_t usBuf[5];

	usBuf[0] = g_tTP.usAdcX1;
	usBuf[1] = g_tTP.usAdcY1;

	usBuf[2] = g_tTP.usAdcX2;
	usBuf[3] = g_tTP.usAdcY2;

	usBuf[4] = g_tTP.XYChange;

}

/*
*********************************************************************************************************
*	函 数 名: TOUCH_LoadParam
*	功能说明: 读取校准参数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
static void TOUCH_LoadParam(void)
{

	/* 保存下面的5个变量即可 */
	uint16_t usBuf[5];

	/* ReadParamToBuf() */

	g_tTP.usAdcX1 = usBuf[0];
	g_tTP.usAdcY1 = usBuf[1];

	g_tTP.usAdcX2 = usBuf[2];
	g_tTP.usAdcY2 = usBuf[3];


	g_tTP.XYChange = usBuf[4];

}
#endif
