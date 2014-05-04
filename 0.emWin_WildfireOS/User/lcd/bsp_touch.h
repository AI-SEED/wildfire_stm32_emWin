
#if 0

#ifndef __BSP_TOUCH_H
#define	__BSP_TOUCH_H

#include "stm32f10x.h"

/* 液晶坐标结构体 */
typedef	struct POINT 
{
   uint16_t x;		
   uint16_t y;
}Coordinate;

/*  校正系数结构体 */
typedef struct Parameter 
{						
long double An,  			 //注:sizeof(long double) = 8
            Bn,     
            Cn,   
            Dn,    
            En,    
            Fn,     
            Divider ;
}Parameter ;

extern volatile unsigned char touch_flag;
extern uint8_t cal_flag;
extern long double cal_p[6];

extern Coordinate ScreenSample[4];
extern Coordinate DisplaySample[4];
extern Parameter touch_para ;
extern Coordinate  display ;

#define	CHX 	0x90 	//通道Y+的选择控制字	
#define	CHY 	0xd0	//通道X+的选择控制字


void Touch_Init(void);
int Touch_Calibrate(void);
void Palette_Init(void);
FunctionalState Get_touch_point(Coordinate * displayPtr,
                                Coordinate * screenPtr,
                                Parameter * para );
Coordinate *Read_2046_2(void);
void Palette_draw_point(uint16_t x, uint16_t y);
uint16_t XPT2046_ReadAdc(uint8_t _ucCh);

#endif /* __BSP_TOUCH_H */

#else

#include "stm32f10x.h"
	typedef struct
	{
		uint16_t usMaxAdc;
		uint16_t usAdcNowX;
		uint16_t usAdcNowY;
	}TOUCH_T;



extern TOUCH_T g_tTP;

/* 触摸事件 */
enum
{
	TOUCH_NONE = 0,		/* 无触摸 */
	TOUCH_DOWN = 1,		/* 按下 */
	TOUCH_MOVE = 2,		/* 移动 */
	TOUCH_RELEASE = 3	/* 释放 */
};

/* 供外部调用的函数声明 */
uint8_t TOUCH_GetKey(int16_t *_pX, int16_t *_pY);

uint8_t TOUCH_InRect(uint16_t _usX, uint16_t _usY,
uint16_t _usRectX, uint16_t _usRectY, uint16_t _usRectH, uint16_t _usRectW);

void bsp_InitTouch(void);
void Touch_Init(void);

uint16_t XPT2046_ReadAdc(uint8_t _ucCh);

void TOUCH_Scan(void);


#endif

