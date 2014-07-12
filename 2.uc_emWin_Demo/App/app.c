#include "app.h"
#include "GUIDEMO.h"

extern	OS_TCB	StartUp_TCB;					

static  OS_TCB	LED_TCB;		   				
static	CPU_STK	LED_Stk[TASK_LED_STK_SIZE];	   

static  OS_TCB	GUI_TCB;		   					
static	CPU_STK	GUI_Stk[TASK_GUI_STK_SIZE];

static  OS_TCB	GUI_Touch_TCB;		   					
static	CPU_STK	GUI_Touch_Stk[TASK_GUI_Touch_STK_SIZE];	



/* 
 * 函数名：Task_Start
 * 描述  : 启动任务，
 *			   优先级为5，
 *		     初始化及创建其它任务
 * 输入  ：无
 * 输出  : 无
 */
void Task_Start(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;	
	
	/* 板级初始化 */
	BSP_Init();		
	
	//创建任务LED	
	OSTaskCreate((OS_TCB     *)&LED_TCB,					            // 任务控制块指针          
               (CPU_CHAR   *)"LED",						            	// 任务名称
               (OS_TASK_PTR )Task_LED,					            // 任务代码指针
               (void       *)0,							                // 传递给任务的参数parg
               (OS_PRIO     )TASK_LED_PRIO,				        	// 任务优先级
               (CPU_STK    *)&LED_Stk[0],				          	// 任务堆栈基地址
               (CPU_STK_SIZE)TASK_LED_STK_SIZE/10,		      // 堆栈剩余警戒线
               (CPU_STK_SIZE)TASK_LED_STK_SIZE,			      	// 堆栈大小
               (OS_MSG_QTY  )0,							                // 可接收的最大消息队列数
               (OS_TICK     )0,							                // 时间片轮转时间
               (void       *)0,							                // 任务控制块扩展信息
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        // 任务选项
               (OS_ERR     *)&err);						              // 返回值

	
	//创建任务GUI	
	OSTaskCreate((OS_TCB     *)&GUI_TCB,					  
               (CPU_CHAR   *)"GUI",						
               (OS_TASK_PTR )Task_GUI,				
               (void       *)0,							
               (OS_PRIO     )TASK_GUI_PRIO,				
               (CPU_STK    *)&GUI_Stk[0],				
               (CPU_STK_SIZE)TASK_GUI_STK_SIZE/10,		
               (CPU_STK_SIZE)TASK_GUI_STK_SIZE,			
               (OS_MSG_QTY  )0,						
               (OS_TICK     )0,							
               (void       *)0,							
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	 
               (OS_ERR     *)&err);	
					
		//创建任务GUI_Touch	
	OSTaskCreate((OS_TCB     *)&GUI_Touch_TCB,					  
               (CPU_CHAR   *)"GUI_Touch",						
               (OS_TASK_PTR )Task_GUI_Touch,				
               (void       *)0,							
               (OS_PRIO     )TASK_GUI_TOUCH_PRIO,				
               (CPU_STK    *)&GUI_Touch_Stk[0],				
               (CPU_STK_SIZE)TASK_GUI_Touch_STK_SIZE/10,		
               (CPU_STK_SIZE)TASK_GUI_Touch_STK_SIZE,			
               (OS_MSG_QTY  )0,						
               (OS_TICK     )0,							
               (void       *)0,							
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	 
               (OS_ERR     *)&err);						
							 
   
  //任务删除自己	
	OSTaskDel(&StartUp_TCB,&err);							 
}


/* 
 * 函数名：Task_LED
 * 描述  : LED任务1，
 *		     优先级为4，
 *		     以200ms的间隔点亮、关闭LED
 * 输入  ：无
 * 输出  : 无
 */
void Task_LED(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;                	

  while (1)
  {
    LED1( ON );
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	//延时阻塞200ms		
    LED1( OFF);  		
    LED2( ON );
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	 //延时阻塞200ms
    LED2( OFF);	
    LED3( ON );
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	 //延时阻塞200ms
    LED3( OFF);
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);		

  }
}



/* 
 * 函数名：Task_GUI
 * 描述  : GUI任务，
 *			   优先级为6，
 *		     显示GUI操作
 * 输入  ：任务输入参数
 * 输出  : 无
 */
void Task_GUI(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;		
	
	GUIDEMO_Main();
	
	//延时
	while(1)
	{
		OSTimeDly(1,OS_OPT_TIME_DLY,&err);
		}
}


/**
  * @brief  Task_GUI_Touch 每10ms执行一次，GUI触屏触发函数
  * @param  p_arg
  * @retval 无
  */
void Task_GUI_Touch(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;
	
	while(1)
	{
		GUI_TOUCH_Exec();										//10ms一次GUI调用处理触屏
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
	}
}
 
/* -------------------------------------end of file------------------------------------ */
