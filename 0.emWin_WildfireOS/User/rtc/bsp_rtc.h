

#ifndef __RTC_H
#define	__RTC_H


#include "stm32f10x.h"
#include "bsp_calendar.h"
#include "bsp_date.h"
#include "bsp_usart1.h"

/* ������������������Ļ�,PC13�ͻ����Ƶ��ΪRTC Clock/64��ʱ�� */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */

void RTC_NVIC_Config(void);
void RTC_Configuration(void);
void Time_Regulate(struct rtc_time *tm);
void Time_Adjust(struct rtc_time *tm);
void Time_Display(uint32_t TimeVar,struct rtc_time *tm);
void Time_Show(struct rtc_time *tm);
static uint8_t USART_Scanf(uint32_t value);
void RTC_CheckAndConfig(struct rtc_time *tm);
void RTC_TimeCovr(struct rtc_time *tm);

void Time_Adjust_LCD(struct rtc_time *DecTm,struct rtc_time *SrcTm);

#endif /* __XXX_H */