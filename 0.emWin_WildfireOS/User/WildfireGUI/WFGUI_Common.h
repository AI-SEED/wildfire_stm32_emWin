#ifndef __WFGUI_COMMON_H
#define __WFGUI_COMMON_H

#include "ICONVIEW.h"
#include "bsp_usart1.h"
#include "bsp_date.h"
#include "ff.h"

/* 用户定义的消息 */
#define MY_MESSAGE_RTC 				(WM_USER+0)
#define MY_MESSAGE_SNAPSHOT 	(WM_USER+1)
#define MY_MESSAGE_CTRLCHANGE	(WM_USER+2)

/* GUI ID */
#define GUI_ID_FRAMEWIN0 	(GUI_ID_USER+1)	
#define GUI_ID_FRAMEWIN1 	(GUI_ID_USER+2)	
#define GUI_ID_FRAMEWIN2 	(GUI_ID_USER+3)	
#define GUI_ID_FRAMEWIN3 	(GUI_ID_USER+4)	
#define GUI_ID_FRAMEWIN4 	(GUI_ID_USER+5)	
#define GUI_ID_FRAMEWIN5 	(GUI_ID_USER+6)	
#define GUI_ID_FRAMEWIN6 	(GUI_ID_USER+7)	
#define GUI_ID_FRAMEWIN7 	(GUI_ID_USER+8)	

#define GUI_ID_LISTWHEEL4 (GUI_ID_USER + 9)
#define GUI_ID_LISTWHEEL5 (GUI_ID_USER + 10)
#define GUI_ID_LISTWHEEL6 (GUI_ID_USER + 11)
#define GUI_ID_FRAMEWIN8 	(GUI_ID_USER+12)

#define GUI_ID_WINDOW0 	(GUI_ID_USER+13)

#define GUI_ID_FRAMEWIN9 	(GUI_ID_USER+14)
#define GUI_ID_FRAMEWIN10 	(GUI_ID_USER+15)

/* icon的页数 */
#define VIRTUAL_WIN_NUM					(2)

/* 每页的图标数*/
#define ICON_PAGE_NUM						(5)



typedef struct WIN_PARA{			//窗口使用到的用户定义参数，方便在回调函数中使用
  int xSizeLCD, ySizeLCD;			//LCD屏幕尺寸
  int xPosWin, yPosWin;				//窗口的起始位置
	int xSizeWin, ySizeWin;			//窗口尺寸
	int xSizeWinVirtual;				//虚拟窗口尺寸	
	
	unsigned char ctr_index;		//记录icorn窗口的索引
	unsigned char isDesktop;		//记录桌面窗口是否在最前  0：桌面窗口不在最前 1：桌面窗口在最前
	
	WM_HWIN hWinCtrl;						//控制窗口的句柄（底部）
	WM_HWIN hWinMain;						//主显示窗的句柄（中部）
	WM_HWIN hWinStatus;					//状态窗口的句柄（顶部）
	WM_HWIN hWinTime;
	
	WM_HWIN hApp[20];						//记录正在运行的App句柄
	
}WIN_PARA;

typedef struct {
  const GUI_BITMAP * pBitmap;
  const char       * pText;
  const char       * pExplanation;
} BITMAP_ITEM;


typedef struct
{
	FIL hFontFile;
	GUI_FONT     XFont;
	GUI_XBF_DATA XBF_Data; 	

}SD_FONT;


extern WIN_PARA WinPara;			//用户使用的窗口额外数据
extern SD_FONT SDFont;

/*时间结构体*/
extern struct rtc_time systmtime;

/* 这些数据文件存储在 WFGUI_Icon.c 文件*/

/***图标**/
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Logo;				//野火Logo，未按下状态
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_LogoPr;			//野火Logo，按下状态

extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Floder;			//文件管理器
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Clock;				//时间
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Message;			//短信
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Phone;				//电话
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Note;				//笔记
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Calculator;	//计算器
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Camera;			//摄像头
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Picture;			//图片浏览器
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Map;					//地图
extern GUI_CONST_STORAGE GUI_BITMAP bmWF_Temperature;	//温度计



/* 公有函数 */
extern void App_Insert(WM_HWIN NewApp);
extern void App_Delete(WM_HWIN NewApp);
extern WM_HWIN App_GetTopWin(void);



#endif


