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
 * ��������Task_Start
 * ����  : ��������
 *			   ���ȼ�Ϊ5��
 *		     ��ʼ����������������
 * ����  ����
 * ���  : ��
 */
void Task_Start(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;	
	
	/* �弶��ʼ�� */
	BSP_Init();		
	
	//��������LED	
	OSTaskCreate((OS_TCB     *)&LED_TCB,					            // ������ƿ�ָ��          
               (CPU_CHAR   *)"LED",						            	// ��������
               (OS_TASK_PTR )Task_LED,					            // �������ָ��
               (void       *)0,							                // ���ݸ�����Ĳ���parg
               (OS_PRIO     )TASK_LED_PRIO,				        	// �������ȼ�
               (CPU_STK    *)&LED_Stk[0],				          	// �����ջ����ַ
               (CPU_STK_SIZE)TASK_LED_STK_SIZE/10,		      // ��ջʣ�ྯ����
               (CPU_STK_SIZE)TASK_LED_STK_SIZE,			      	// ��ջ��С
               (OS_MSG_QTY  )0,							                // �ɽ��յ������Ϣ������
               (OS_TICK     )0,							                // ʱ��Ƭ��תʱ��
               (void       *)0,							                // ������ƿ���չ��Ϣ
               (OS_OPT      )(OS_OPT_TASK_STK_CHK | 
                              OS_OPT_TASK_STK_CLR),	        // ����ѡ��
               (OS_ERR     *)&err);						              // ����ֵ

	
	//��������GUI	
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
					
		//��������GUI_Touch	
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
							 
   
  //����ɾ���Լ�	
	OSTaskDel(&StartUp_TCB,&err);							 
}


/* 
 * ��������Task_LED
 * ����  : LED����1��
 *		     ���ȼ�Ϊ4��
 *		     ��200ms�ļ���������ر�LED
 * ����  ����
 * ���  : ��
 */
void Task_LED(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;                	

  while (1)
  {
    LED1( ON );
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	//��ʱ����200ms		
    LED1( OFF);  		
    LED2( ON );
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	 //��ʱ����200ms
    LED2( OFF);	
    LED3( ON );
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);	 //��ʱ����200ms
    LED3( OFF);
		
    OSTimeDlyHMSM(0, 0,0,200,OS_OPT_TIME_HMSM_STRICT,&err);		

  }
}



/* 
 * ��������Task_GUI
 * ����  : GUI����
 *			   ���ȼ�Ϊ6��
 *		     ��ʾGUI����
 * ����  �������������
 * ���  : ��
 */
void Task_GUI(void *p_arg)
{
  OS_ERR err;
  (void)p_arg;		
	
	GUIDEMO_Main();
	
	//��ʱ
	while(1)
	{
		OSTimeDly(1,OS_OPT_TIME_DLY,&err);
		}
}


/**
  * @brief  Task_GUI_Touch ÿ10msִ��һ�Σ�GUI������������
  * @param  p_arg
  * @retval ��
  */
void Task_GUI_Touch(void *p_arg)
{
	OS_ERR err;
  (void)p_arg;
	
	while(1)
	{
		GUI_TOUCH_Exec();										//10msһ��GUI���ô�����
		OSTimeDly(10,OS_OPT_TIME_DLY,&err);	
	}
}
 
/* -------------------------------------end of file------------------------------------ */
