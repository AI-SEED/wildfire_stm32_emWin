#ifndef	_APP_H_
#define	_APP_H_

#include "bsp.h" 
#include "os.h"  		
#include "os_type.h"
#include "GUI.h"

/*******************设置任务优先级*******************/
#define STARTUP_TASK_PRIO       3
#define	TASK_LED_PRIO			    	5   
#define	TASK_GUI_PRIO				    4 
#define	TASK_BSP_INIT_PRIO			3

/************设置栈大小（单位为 OS_STK ）************/
#define STARTUP_TASK_STK_SIZE   100 
#define	TASK_LED_STK_SIZE		  	80  
#define	TASK_GUI_STK_SIZE		  	300

/**************** 用户任务声明 *******************/
void Task_Start(void *p_arg);
void Task_LED(void *p_arg);
void Task_GUI(void *p_arg);
void Task_Bsp_Init(void *p_arg);

#endif	//_APP_H_
