#include <string.h>
#include <stdlib.h>

#include "WFGUI_Phone.h"

#include "WFGUI_Message.h"
#include "WFGUI_Keypad.h"


#include "DIALOG.h"

#include "sim900a.h"
#include "bsp_usart2.h"


#define PHONEKEY_BORDER 10
#define PHONEKEY_PITCH  2
#define PHONEKEY_WIDTH	70
#define PHONEKEY_HEIGHT 35

#define GUI_KEY_COLOR 	GUI_WHITE


static const BUTTON_DATA _aButtonData[] = {
	
	/*	��һ�Ű��� */
  { PHONEKEY_BORDER	,PHONEKEY_BORDER	,  PHONEKEY_WIDTH		,PHONEKEY_HEIGHT, "1"   ,GUI_KEY_COLOR },
  { PHONEKEY_BORDER +	 PHONEKEY_PITCH + PHONEKEY_WIDTH		,PHONEKEY_BORDER		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "2",GUI_KEY_COLOR },
  { PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_WIDTH)*2	,PHONEKEY_BORDER		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "3",GUI_KEY_COLOR },
	
	/*	�ڶ��Ű��� */
  { PHONEKEY_BORDER	,PHONEKEY_BORDER + PHONEKEY_PITCH + PHONEKEY_HEIGHT 	,  PHONEKEY_WIDTH		,PHONEKEY_HEIGHT, "4"   ,GUI_KEY_COLOR },
  { PHONEKEY_BORDER +	 PHONEKEY_PITCH + PHONEKEY_WIDTH		,PHONEKEY_BORDER + PHONEKEY_PITCH + PHONEKEY_HEIGHT 	,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "5",GUI_KEY_COLOR },
  { PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_WIDTH)*2	,PHONEKEY_BORDER + PHONEKEY_PITCH + PHONEKEY_HEIGHT		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "6",GUI_KEY_COLOR },

	/*	�����Ű��� */
  { PHONEKEY_BORDER	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*2 	,  PHONEKEY_WIDTH		,PHONEKEY_HEIGHT, "7"   ,GUI_KEY_COLOR },
  { PHONEKEY_BORDER +	 PHONEKEY_PITCH + PHONEKEY_WIDTH		,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*2 	,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "8",GUI_KEY_COLOR },
  { PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_WIDTH)*2	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*2		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "9",GUI_KEY_COLOR },

	/*	�����Ű��� */
  { PHONEKEY_BORDER	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*3 	,  PHONEKEY_WIDTH		,PHONEKEY_HEIGHT, "*"   ,GUI_KEY_COLOR },
  { PHONEKEY_BORDER +	 PHONEKEY_PITCH + PHONEKEY_WIDTH		,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*3 	,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "0",GUI_KEY_COLOR },
  { PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_WIDTH)*2	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*3		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "#",GUI_KEY_COLOR },

	/*	�����Ű��� */
  { PHONEKEY_BORDER	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*4 	,  PHONEKEY_WIDTH		,PHONEKEY_HEIGHT, "Add"   ,GUI_KEY_COLOR },
  { PHONEKEY_BORDER +	 PHONEKEY_PITCH + PHONEKEY_WIDTH		,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*4 	,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "Call",GUI_KEY_COLOR },
  { PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_WIDTH)*2	,PHONEKEY_BORDER + (PHONEKEY_PITCH + PHONEKEY_HEIGHT)*4		,  PHONEKEY_WIDTH		, PHONEKEY_HEIGHT, "<--",GUI_KEY_COLOR },

};



static void Phone_Calling(WM_HWIN Parent,char *num);




/**
  * @brief  _cbPhone,�绰���ڻص�����
  * @param  none
  * @retval none
  */
static void _cbPhone(WM_MESSAGE * pMsg)
{
	
	WM_HWIN hWin;
	int xSize,ySize;
	
	
	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE:
		
	
		break;
	
	case WM_DELETE:
		
		/* ɾ��app���������ļ�¼ */	
	//	App_Delete(pMsg->hWin);
	
		/* ������Ϣ֪ͨctrl����*/		
	//	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	

	
		break;
	
	case WM_PAINT:
		//
    // Draw background
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    GUI_SetColor(GUI_WHITE);
	
		GUI_FillRect(0,0,xSize,ySize);
	
		break;
	
	}	
}


/**
  * @brief  _cbCalling,�绰���ڻص�����
  * @param  none
  * @retval none
  */
static void _cbCalling(WM_MESSAGE * pMsg)
{
	WM_HWIN hWin;
	WM_HWIN hButton;
	
	int xSize,ySize;
	
	int Id,NCode;
	
	
	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  
		case WM_CREATE:
			
		 hButton = BUTTON_CreateEx(70,WM_GetWindowSizeY(hWin)-50,100, 40, 
                                hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, GUI_ID_BUTTON0);
      
			BUTTON_SetText(hButton, "Hang off");
			BUTTON_SetFont(hButton,GUI_FONT_20B_ASCII);	
			BUTTON_SetBkColor(hButton,BUTTON_CI_UNPRESSED,GUI_WHITE);
			BUTTON_SetBkColor(hButton,BUTTON_CI_PRESSED,GUI_BLUE);    		
	
		break;
		
	case WM_NOTIFY_PARENT:
		Id    = WM_GetId(pMsg->hWinSrc);
		NCode = pMsg->Data.v;
		
		if(NCode == WM_NOTIFICATION_RELEASED )
		{		
			if(Id == GUI_ID_BUTTON0)				
			{
				/* �ҵ绰 */
				SIM900A_HANGOFF();
				
				/* �رմ��� */		
				App_Delete(WM_GetParent(pMsg->hWin));
				
				WM_DeleteWindow(WM_GetParent(pMsg->hWin));
			
			}
			
		}			
	
	
		break;
		
	
	case WM_PAINT:
		//
    // Draw background
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    GUI_SetColor(GUI_DARKGRAY);
	
		GUI_FillRect(0,0,xSize,ySize);
	
		break;
	
	}	


}

/**
  * @brief  _cbPhoneKey,�绰���ڻص�����
  * @param  none
  * @retval none
  */
static void _cbPhoneKey(WM_MESSAGE * pMsg)
{
	
	WM_HWIN hWin;
	WM_HWIN hButton; 
	WM_HWIN hPhone;
	WM_HWIN hEdit;
	
	int xSize,ySize;
	
	int Id ;
	int NCode;	
	
	uint8_t i;
	
	
	hWin = pMsg->hWin;
	
  switch (pMsg->MsgId) {
		case WM_CREATE:
		
			/* �����绰���� */
			for (i = 0; i < GUI_COUNTOF(_aButtonData); i++) {
      hButton = BUTTON_CreateEx(_aButtonData[i].xPos, _aButtonData[i].yPos, _aButtonData[i].xSize, _aButtonData[i].ySize, 
                                hWin, WM_CF_SHOW | WM_CF_HASTRANS, 0, ID_BUTTON + i);
      
			BUTTON_SetText(hButton, _aButtonData[i].acLabel);
			BUTTON_SetFont(hButton,GUI_FONT_20B_ASCII);	
			BUTTON_SetBkColor(hButton,BUTTON_CI_UNPRESSED,_aButtonData[i].bkColor);
      BUTTON_SetFocussable(hButton, 0);
				
    }
	
		break;
		
		case WM_NOTIFY_PARENT:
			
			Id    = WM_GetId(pMsg->hWinSrc);
			NCode = pMsg->Data.v;
			
			if(NCode == WM_NOTIFICATION_RELEASED )
			{			
					/* ��ͨ���� */				
				if (Id - ID_BUTTON < 12) 
				{					
					char c = _aButtonData[Id - ID_BUTTON].acLabel[0];
					
					/* ���밴������ */
					GUI_StoreKeyMsg(c, 1);
					GUI_StoreKeyMsg(c, 0);
				
				}
				/* ��ӵ���ϵ�� TBD*/
				else if(Id - ID_BUTTON == 12)
				{
				}
				/* ȷ�ϲ���绰 */
				else if(Id - ID_BUTTON == 13)
				{
					char *  rebuff;		
					
					char* num;//�绰����
					int len;
					
					/* ���Phone ���ڵľ��*/
					hPhone = WM_GetParent(pMsg->hWin);
					
					/* ��ȡEdit �ľ�� */
					hEdit = WM_GetDialogItem(hPhone,GUI_ID_EDIT0);
					
					len = EDIT_GetNumChars(hEdit)+1;
					
					num = (char *)malloc(sizeof(char)*len);

					
					/* ��ȡ����ĵ绰���� */
					EDIT_GetText(hEdit,num,len);	

					/* ��ʾ���ڲ���绰�Ľ��� */	
					Phone_Calling(hPhone,num);	

					/* ����GSMģ�麯�� ����绰 */
					
					
					//����绰����
					sim900a_call(num);
					
					/* �ͷſռ� */
					free(num);
					
					#if 0
					rebuff = sim900a_waitask(0);
					switch(rebuff[3])
					{
							case 'B':   //BUSY
									DEBUG("\r\n�㲦��ĵ绰��æ�����Ժ��ٲ�");
									break;
							case '+':   //
									//��һ��:�ź��������Ϊ:"�������"
									DEBUG("\r\n�ѽ�ͨ");
									SIM900A_CLEAN_RX();             //������ջ�����
									rebuff = sim900a_waitask(0);     //���µȴ���Ϣ
									if(rebuff[3] == 'N')
									{
											DEBUG("\r\nͨ������");
									}
									
							
									break;
							case 'N':       //NO ANSWER
									DEBUG("\r\n�㲦��ĵ绰��ʱ���˽��������Ժ��ٲ�");
							
									break;
					}
					#endif				

				}
				/* ɾ�� */
				else if(Id - ID_BUTTON == 14)
				{
					/* �˸� */
					GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 1);
					GUI_StoreKeyMsg(GUI_KEY_BACKSPACE, 0);
				}
		}
		break;
	
	case WM_PAINT:
		//
    // Draw background
    //
    xSize = WM_GetWindowSizeX(hWin);
    ySize = WM_GetWindowSizeY(hWin);
    GUI_SetColor(GUI_WHITE);
	
		GUI_FillRect(0,0,xSize,ySize);
	
		break;
	
	}	
	

}
	

/**
  * @brief  Phone_Calling,���ڲ���绰
  * @param  none
  * @retval none
  */
static void Phone_Calling(WM_HWIN Parent,char *num)
{
	//WM_HWIN hPhone;
	WM_HWIN hText;
	WM_HWIN hKeypad;
	WM_HWIN hButton;	
	
	HANDLE_LIST hPhone;
	
	char DispText[60];
	
	/* �����绰���� */
	hPhone.hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbPhone, 0);	

	sprintf(DispText,"Calling %s",num);
	/* �������봰�� */	
	hText	= TEXT_CreateEx(0,0,240,80,hPhone.hAPP,WM_CF_SHOW,TEXT_CF_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT0,DispText);	
	
	/* �����ı��򱳾� */
	TEXT_SetBkColor(hText,GUI_DARKGRAY);
	
	/* �����ı����� */
	TEXT_SetFont(hText,GUI_FONT_24B_ASCII);
	
	/* ����������ɫ */
	TEXT_SetTextColor(hText,GUI_WHITE);
	
	/* �����ı����뷽ʽ */
	TEXT_SetTextAlign(hText,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
		
	

	/* ������������ */
	hKeypad = WM_CreateWindowAsChild(0, 80, WinPara.xSizeWin,WinPara.ySizeWin-80 ,hPhone.hAPP , WM_CF_SHOW | WM_CF_STAYONTOP, _cbCalling, 0);		
	
	/* ��¼��ǰ���� */
	//App_Insert(hPhone);
	
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);


}

 //����app_info����ṹ
 typedef struct application_info
 {
     uint32_t  app_id;
     uint32_t  up_flow;
     uint32_t  down_flow;
     struct    list_head app_info_node;//����ڵ�
 }app_info;
 
 
 app_info* get_app_info(uint32_t app_id, uint32_t up_flow, uint32_t down_flow)
 {
     app_info *app = (app_info*)malloc(sizeof(app_info));
     if (app == NULL)
     {
//     fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",
//         errno, strerror(errno));
     return NULL;
     }
     app->app_id = app_id;
     app->up_flow = up_flow;
     app->down_flow = down_flow;
     return app;
 }
 static void for_each_app(const struct list_head *head)
 {
     struct list_head *pos;
     app_info *app;
     //��������
     list_for_each(pos, head)
     {
     app = list_entry(pos, app_info, app_info_node); 
     printf("ap_id: %u\tup_flow: %u\tdown_flow: %u\n",
         app->app_id, app->up_flow, app->down_flow);
 
     }
 }
 
 void destroy_app_list(struct list_head *head)
 {
     struct list_head *pos = head->next;
     struct list_head *tmp = NULL;
     while (pos != head)
     {
     tmp = pos->next;
     list_del(pos);
     pos = tmp;
     }
 }
	
	
	
/**
  * @brief  WFGUI_Phone,�绰����
  * @param  none
  * @retval none
  */	
void WFGUI_Phone(void)
{
	//WM_HWIN hPhone;
	WM_HWIN hEdit;
	WM_HWIN hKeypad;
	WM_HWIN hButton;	
	
	HANDLE_LIST *hPhone = hAPPLinkedList_NewNode();

	
	/* �����绰���� */
	hPhone->hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbPhone, 0);	

	/* �������봰�� */	
	hEdit	= EDIT_CreateEx(0,0,240,80,hPhone->hAPP,WM_CF_SHOW,0,GUI_ID_EDIT0,50);	
	
	/* �����ı��򱳾� */
	EDIT_SetBkColor(hEdit,EDIT_CI_ENABLED,GUI_DARKGRAY);
	
	/* �����ı����� */
	EDIT_SetFont(hEdit,GUI_FONT_24B_ASCII);
	
	/* ����������ɫ */
	EDIT_SetTextColor(hEdit,EDIT_CI_ENABLED,GUI_WHITE);
	
	/* �����ı����뷽ʽ */
	EDIT_SetTextAlign(hEdit,TEXT_CF_HCENTER|TEXT_CF_VCENTER);
	
	/* ���ô��ھ۽� */
	WM_SetFocus(hEdit);	


	/* ������������ */
	hKeypad = WM_CreateWindowAsChild(0, 80, WinPara.xSizeWin,WinPara.ySizeWin-80 ,hPhone->hAPP , WM_CF_SHOW | WM_CF_STAYONTOP, _cbPhoneKey, 0);	

	/* ��¼��ǰ���� */
	#if 0
	App_Insert(hPhone->hAPP);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	#else
	hAPPLinkedList_AddTail(hPhone);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);

	#endif
	/* ��ʼ�������ģ�� */
	if (sim900a_init()!= 0)
	{
		GUI_MessageBox("\r\n No detected SIM900A module! \r\n","error",GUI_MESSAGEBOX_CF_MOVEABLE);
		}
		
	

	WM_SetFocus(hEdit);		
		
//	{	
//		
//		struct list_head *head;
//		 app_info *app;
//	 //����һ��app_info
//     app_info * app_info_list = (app_info*)malloc(sizeof(app_info));
//    
//     if (app_info_list == NULL)
//     {
////     fprintf(stderr, "Failed to malloc memory, errno:%u, reason:%s\n",
////         errno, strerror(errno));
//     return ;
//     }
//     //��ʼ������ͷ��
//     head = &app_info_list->app_info_node;
//     INIT_LIST_HEAD(head);
//     //��������app_info
//     app = get_app_info(1001, 100, 200);
//    list_add_tail(&app->app_info_node, head);
//     app = get_app_info(1002, 80, 100);
//     list_add_tail(&app->app_info_node, head);
//     app = get_app_info(1003, 90, 120);
//     list_add_tail(&app->app_info_node, head);
//     printf("After insert three app_info: \n");
//     for_each_app(head);
//     //����һ���ڵ��Ƶ�ĩβ
//     printf("Move first node to tail:\n");
//     list_move_tail(head->next, head);
//     for_each_app(head);
//     //ɾ�����һ���ڵ�
//     printf("Delete the last node:\n");
//     list_del(head->prev);
//     for_each_app(head);
//     destroy_app_list(head);
//     free(app_info_list);

//}
		

	
}
