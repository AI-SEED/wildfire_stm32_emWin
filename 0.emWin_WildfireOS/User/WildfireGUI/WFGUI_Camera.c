#include "WFGUI_Camera.h"
#include "dialog.h"
#include "bsp_ov7725.h"

extern char _acBuffer[1024*4];


/**
  * @brief  _cbCamera,摄像头回调函数
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
  * @brief  _GetData用于读取外存储器图像的函数 适用于bmp jpg gif文件
  * @param  
	*		
  * @retval 无
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
  * @brief  WFGUI_Camera,摄像头
  * @param  none
  * @retval none
  */
void WFGUI_Camera(void)
{
	WM_HWIN hCamera;
	
	/* 创建电话窗口 */
	hCamera = WM_CreateWindowAsChild(0, 0, WinPara.xSizeWin,WinPara.ySizeWin ,WinPara.hWinMain , WM_CF_SHOW | WM_CF_STAYONTOP, _cbCamera, 0);	
	
	/* 记录当前窗口 */
	App_Insert(hCamera);
	
	/* 初始化摄像头 */
	//Camera_Init();

}


