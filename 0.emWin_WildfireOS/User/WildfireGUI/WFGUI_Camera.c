#include "WFGUI_Camera.h"
#include "dialog.h"
#include "bsp_ov7725.h"

extern char _acBuffer[1024*4];


/**
  * @brief  _cbCamera,����ͷ�ص�����
  * @param  none
  * @retval none
  */
static void _cbCamera(WM_MESSAGE * pMsg)
{
	
	WM_HWIN hWin;
	int xSize,ySize;
	
	
	hWin = pMsg->hWin;
  switch (pMsg->MsgId) {
  case WM_CREATE:
		
	
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
  * @brief  _GetData���ڶ�ȡ��洢��ͼ��ĺ��� ������bmp jpg gif�ļ�
  * @param  
	*		
  * @retval ��
  */
//static int _GetCameraData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
//  UINT NumBytesRead;
//  FIL * phFile;
//	FRESULT fres;

//  phFile = (FIL *)p;
//  /*
//  * Check buffer size
//  */
//  if (NumBytesReq > sizeof(_acBuffer)) {
//    NumBytesReq = sizeof(_acBuffer);
//  }
//  /*
//  * Set file pointer to the required position
//  */
// fres =  f_lseek (phFile, Off );

//  /*
//  * Read data into buffer
//  */
// fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead);
//  /*
//  * Set data pointer to the beginning of the buffer
//  */
//  *ppData = (U8 *)_acBuffer;
//	
//  /*
//  * Return number of available bytes
//  */
//  return NumBytesRead;
//}






/**
  * @brief  WFGUI_Camera,����ͷ
  * @param  none
  * @retval none
  */
void WFGUI_Camera(void)
{
	/*��ʾ��Ϣ����Ӧ�����Ӱ�ӵ������*/
	const char tips[] = {0xE8,0xAF,0xA5,0xE5,0xBA,0x94,0xE7,0x94,0xA8,0xE6,0xAD,0xA3,0xE5,0x8A,0xA0,0xE7,0x8F,0xAD,0xE5,0x8A,0xA0,0xE7,0x82,0xB9,0xE8,0xAE,0xBE,0xE8,0xAE,0xA1,0xE4,0xB8,0xAD,0x20};
	HANDLE_LIST *hCamera = hAPPLinkedList_NewNode();
	
	/* �����绰���� */
	hCamera->hAPP = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbCamera, 0);	
	
	/* ��¼��ǰ���� */
	/* ��ӽ�㵽���� */
	hAPPLinkedList_AddTail(hCamera);
	/* ��ctrl���ڷ�����Ϣ */
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	
	GUI_MessageBox(tips,"error",GUI_MESSAGEBOX_CF_MOVEABLE);

	
	/* ��ʼ������ͷ */
	//Camera_Init();

}


