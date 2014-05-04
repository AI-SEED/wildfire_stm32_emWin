#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_TextReader.h"

static WM_CALLBACK*     _pcbOldSDViewWin = NULL;



/**
  * @brief  _cbTextReaderWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbTextReaderWin(WM_MESSAGE * pMsg)
{
	int  Id;
	
		switch (pMsg->MsgId) {			//��Ϣ����

#if 1			//��֣�����Ҫ�������
		case WM_NOTIFY_PARENT:		//֪ͨ		
			break;//֪ͨ����break
#endif	
			
			case WM_DELETE:
				/* ɾ��app���������ļ�¼ */	
				App_Delete(pMsg->hWin);
			
				/* ������Ϣ֪ͨctrl����*/		
				WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
			
			
				break;
		
			default:		
      WM_DefaultProc(pMsg);
			
		}//MsgId
	
	if (_pcbOldSDViewWin) {
			(*_pcbOldSDViewWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}

/**
  * @brief  �ı��Ķ���
  * @param  Ҫ�򿪵��ļ���  
  * @retval ��
  */
void TextReader(char *file_name )
{	
	FIL hFile;
	unsigned int rw_num;			//�Ѷ�����д���ֽ���
	FRESULT fres;							//���ؽ��
	char* read_buffer; 
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	read_buffer = (char * ) malloc(300* sizeof(char));  					//Ϊ�洢Ŀ¼����ָ�����ռ�
	*read_buffer ='\0';

	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,
															GUI_ID_FRAMEWIN1,
															"eBook Reader",
															0);															

	/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldSDViewWin = WM_SetCallback(hFrame, _cbTextReaderWin);	//��ȡ�ɵĻص�����ָ��

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);
	
	DEBUG("file_name =%s",file_name);	
	
	fres = f_open (&hFile, file_name, FA_READ ); 				//��Ҫ�Ķ����ļ�
	
	hMultiEdit=MULTIEDIT_CreateEx(0,0,WM_GetWindowSizeX(hFrameC),WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,MULTIEDIT_CF_READONLY|MULTIEDIT_CF_AUTOSCROLLBAR_V,GUI_ID_MULTIEDIT0,0,0);
	MULTIEDIT_SetFont(hMultiEdit,GUI_FONT_13B_ASCII);
	MULTIEDIT_SetWrapWord(hMultiEdit);

	while(hFile.fptr!=hFile.fsize) 		//���ָ��ָ�����ļ�β����ʾ����ȫ������
	{
		fres = f_read(&hFile, read_buffer, 300, &rw_num);
		if(rw_num<300)
			read_buffer[rw_num]='\0';		//�����ļ�β���Ͻ�����
		
		MULTIEDIT_AddText(hMultiEdit,read_buffer);		
	}
	fres = f_close (&hFile);	
	
	DEBUG("%s",read_buffer);
	
	free(read_buffer);								//�ͷ�malloc�ռ�		
	read_buffer = NULL;



}














