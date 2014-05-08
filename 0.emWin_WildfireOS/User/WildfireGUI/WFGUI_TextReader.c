#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_SDView.h"
#include "WFGUI_TextReader.h"

static WM_CALLBACK*     _pcbOldSDViewWin = NULL;


/**
  * @brief  _cbTextReaderWin framewin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbTextReaderWin(WM_MESSAGE * pMsg)
{
	int  Id;
	
		switch (pMsg->MsgId) {			//消息类型

#if 1			//奇怪，必须要有这个项
		case WM_NOTIFY_PARENT:		//通知		
			break;//通知代码break
#endif	
			
			case WM_DELETE:
				/* 删除app句柄链表里的记录 */	
				App_Delete(pMsg->hWin);
			
				/* 发送消息通知ctrl窗口*/		
				WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	
			
			
				break;
		
			default:		
      WM_DefaultProc(pMsg);
			
		}//MsgId
	
	if (_pcbOldSDViewWin) {
			(*_pcbOldSDViewWin)(pMsg);	//调用旧的回调函数处理部分未处理过程
		}	
	
}

/**
  * @brief  文本阅读器
  * @param  要打开的文件名  
  * @retval 无
  */
void TextReader(char *file_name )
{	
	FIL hFile;
	unsigned int rw_num;			//已读或已写的字节数
	FRESULT fres;							//返回结果
	char* read_buffer; 
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	read_buffer = (char * ) malloc(300* sizeof(char));  					//为存储目录名的指针分配空间
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

	/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldSDViewWin = WM_SetCallback(hFrame, _cbTextReaderWin);	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);
	
	DEBUG("file_name =%s",file_name);	
	
	fres = f_open (&hFile, file_name, FA_READ ); 				//打开要阅读的文件
	
	hMultiEdit=MULTIEDIT_CreateEx(0,0,WM_GetWindowSizeX(hFrameC),WM_GetWindowSizeY(hFrameC),hFrameC,WM_CF_SHOW,MULTIEDIT_CF_READONLY|MULTIEDIT_CF_AUTOSCROLLBAR_V,GUI_ID_MULTIEDIT0,0,0);
	MULTIEDIT_SetFont(hMultiEdit,GUI_FONT_13B_ASCII);
	MULTIEDIT_SetWrapWord(hMultiEdit);

	while(hFile.fptr!=hFile.fsize) 		//如果指针指向了文件尾，表示数据全部读完
	{
		fres = f_read(&hFile, read_buffer, 300, &rw_num);
		if(rw_num<300)
			read_buffer[rw_num]='\0';		//到了文件尾加上结束符
		
		MULTIEDIT_AddText(hMultiEdit,read_buffer);		
	}
	fres = f_close (&hFile);	
	
	DEBUG("%s",read_buffer);
	
	free(read_buffer);								//释放malloc空间		
	read_buffer = NULL;



}




/**
  * @brief  WFGUI_TextReader,WFGUI textReader 主函数
  * @param  none
  * @retval none
  */
void WFGUI_TextReader(void)
{

	Fill_TreeView(TEXTFILE,TEXT_LIST_PATH); 

}










