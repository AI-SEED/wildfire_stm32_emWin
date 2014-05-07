
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_ImageReader.h"






//	TBD 把这些图片显示函数整合在一起

/* 直接定义成char类型，有可能地址不对齐，导致DMA传输出错 */
//char _acBuffer[1024*4];

/* 定义成4字节对齐，防止DMA传输出错 */
__align(4) char _acBuffer[1024*4] ;		

/*也可以这样定义,定义成整型，从而编译器自动分配到4字节对齐的空间*/
//int _acIntBuffer[1024];


static WM_CALLBACK*     _pcbOldImageWin = NULL;
static __IO int 				ImageNum				=	0;

static void Image_Display(int sel_num,WM_HWIN hParent);





/**
  * @brief  _GetData用于读取外存储器图像的函数 适用于bmp jpg gif文件
  * @param   p           - Pointer to application defined data.
	*					 NumBytesReq - Number of bytes requested.	
	*  				 ppData      - Pointer to data pointer. This pointer should be set to
	*                 				a valid location.
	*  				 StartOfFile - If this flag is 1, the data pointer should be set to the
	*                 				beginning of the data stream.

  * @retval 读取到的字节数
  */
	
	
static int _GetData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  UINT NumBytesRead;
  FIL * phFile;
	FRESULT fres;	
	
	//char* _acBuffer =(char *)_acIntBuffer;
	
  phFile = (FIL *)p;
	
	
  /*
  * Check buffer size
  */
  if (NumBytesReq > sizeof(_acBuffer)) {
    NumBytesReq = sizeof(_acBuffer);
  }

  /*
  * Set file pointer to the required position
  */ 
	fres =  f_lseek (phFile, Off);

	/*
  * Read data into buffer
  */	
	#if 0	/* TBD 直接读取数据，在32、24位图片时有问题 */
		fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead);
	#else
		/* 不知为何，读取24、32位图的时候，
		*	每次请求超过512字节的时候会出错，分开两次读取没问题，
		*	可能是底层有问题,但在其它情况下文件系统都没问题 。估计也是地址空间对齐的问题*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//求出有多少个数据块
		 unsigned int   remain = NumBytesReq%512;		//不足一块的字节数目
		 
		 for(i=0;i<count;i++)
		 {
			 /* 读取512字节 */
			fres = f_read(phFile, _acBuffer + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)/* 读取剩余字节 */
		 {
			fres = f_read(phFile, _acBuffer + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* 小于512字节的情况 */
		fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead); 

 #endif

  /*
  * Set data pointer to the beginning of the buffer
  */
  *ppData = (const U8 *)_acBuffer;
	
  /*
  * Return number of available bytes
  */
  return NumBytesRead;
}


/**
  * @brief  _GetPngData用于读取外存储器图像的函数 适用于gng文件
  * @param  
	*		
  * @retval 无
  */
static int _GetPNGData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
  UINT NumBytesRead;
  FIL * phFile;
	FRESULT fres;
  U8 * pData;

  pData  = (U8 *)*ppData;
  phFile = (FIL *)p;
  //
  // Set file pointer to the required position
  //
  f_lseek(phFile, Off);
  //
  // Read data into buffer

	#if 0	/* TBD 直接读取数据，在32、24位图片时有问题 */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead);
	#else
		/* 不知为何，读取24、32位图的时候，
		*	每次请求超过512字节的时候会出错，分开两次读取没问题，
		*	可能是底层有问题,但在其它情况下文件系统都没问题 。估计也是地址空间对齐的问题*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//求出有多少个数据块
		 unsigned int   remain = NumBytesReq%512;		//不足一块的字节数目
		 
		 for(i=0;i<count;i++)
		 {
			 /* 读取512字节 */
			fres = f_read(phFile, pData + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)/* 读取剩余字节 */
		 {
			fres = f_read(phFile, pData + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* 小于512字节的情况 */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead); 

 #endif
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}



/**
  * @brief  _cbImageWin framewin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbImageWin(WM_MESSAGE * pMsg)
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
	
	if (_pcbOldImageWin) {
			(*_pcbOldImageWin)(pMsg);	//调用旧的回调函数处理部分未处理过程
		}	
	
}



/**
  * @brief  GUI_JPEG_GetXSizeEx JPEG格式图片的获取X大小函数
  * @param  
	*		
  * @retval 无
  */
static int GUI_JPEG_GetXSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p)
{
	GUI_JPEG_INFO JPEGInfo;
	
  GUI_JPEG_GetInfoEx(_GetData, p, &JPEGInfo);                     /* Get JPEG info structure */
	
	return JPEGInfo.XSize;
}

/**
  * @brief  GUI_JPEG_GetYSizeEx JPEG格式图片的获取Y大小函数
  * @param  
	*		
  * @retval 无
  */
static int GUI_JPEG_GetYSizeEx(GUI_GET_DATA_FUNC * pfGetData, void * p)
{
	GUI_JPEG_INFO JPEGInfo;
	
  GUI_JPEG_GetInfoEx(_GetData, p, &JPEGInfo);                     /* Get JPEG info structure */
	
	return JPEGInfo.YSize;
}


/**
  * @brief  GUI_PNG_DrawScaledEx 按比例显示PNG图片，由于PNG没有按比例显示图片的函数，这里是直接显示的
  * @param  
	*		
  * @retval 无
  */
static int GUI_PNG_DrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom)
{
		GUI_PNG_DrawEx( pfGetData,  p, 0, 0);

}

/**
  * @brief  GUI_GIF_ActiveDrawEx 直接显示GIF动态图片
  * @param  
	*		
  * @retval 无
  */
static int GUI_GIF_ActiveDrawEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0)
{
	int j;
	GUI_GIF_INFO       GifInfo   = {0}; /* Info structure of GIF file */
  GUI_GIF_IMAGE_INFO ImageInfo = {0}; /* Info structure of one particular GIF image of the GIF file */

	
	GUI_GIF_GetInfoEx(pfGetData, p, &GifInfo);                     /* Get GIF info structure */

	for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubEx(pfGetData,p, x0, y0,j);            /* Draw sub image */
					GUI_GIF_GetImageInfoEx(pfGetData, p, &ImageInfo, j);       /* Get sub image information */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
				}		

}


/**
  * @brief  GUI_GIF_ActiveDrawScaledEx 按比例显示GIF动态图片
  * @param  
	*		
  * @retval 无
  */
static int GUI_GIF_ActiveDrawScaledEx(GUI_GET_DATA_FUNC * pfGetData, void * p, int x0, int y0, int Num, int Denom)
{
	int j;
	GUI_GIF_INFO       GifInfo   = {0}; /* Info structure of GIF file */
  GUI_GIF_IMAGE_INFO ImageInfo = {0}; /* Info structure of one particular GIF image of the GIF file */

	
	GUI_GIF_GetInfoEx(pfGetData, p, &GifInfo);                     /* Get GIF info structure */

	for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubScaledEx(pfGetData,p, x0, y0,j,Num,Denom);            /* Draw sub image */
					GUI_GIF_GetImageInfoEx(pfGetData, p, &ImageInfo, j);       /* Get sub image information */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
				}		

}


/**
  * @brief  ImageReader_Raw 显示图片
  * @param  file_name 图片文件名 ，hParent 父窗口句柄
	*		
  * @retval 无
  */
static void  ImageReader_Raw(char *file_name ,WM_HWIN hParent)
{
	FIL hFile ;
	FRESULT fres;
	
	enum IMAGE_TYPE ImageType = GIF;
	IMAGE_FUN ImageFun;
	
  int XSize, YSize, XPos, YPos,XSizeWin,YSizeWin;

	/* 立即重绘窗口，否则图片会在窗口重绘时被刷掉 */
	WM_PaintWindowAndDescs(hParent);

	//选择其窗口用于绘制操作
	WM_SelectWindow(hParent);
	
	XSizeWin = WM_GetWindowSizeX(hParent);
	YSizeWin = WM_GetWindowSizeY(hParent);
	
	GUI_ClearRect(0, 0,XSizeWin, YSizeWin);	
	
	if(strstr(file_name,".bmp")||strstr(file_name,".BMP"))
	{
	
				ImageFun.GetXSizeEx 	= GUI_BMP_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_BMP_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_BMP_DrawEx;
				ImageFun.DrawScaledEx = GUI_BMP_DrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
		
	}
	else if(strstr(file_name,".jpg")||strstr(file_name,".JPG"))
	{
		
				ImageFun.GetXSizeEx 	= GUI_JPEG_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_JPEG_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_JPEG_DrawEx;
				ImageFun.DrawScaledEx = GUI_JPEG_DrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
		
	}
	else if(strstr(file_name,".gif")||strstr(file_name,".GIF"))
	{
				ImageFun.GetXSizeEx 	= GUI_GIF_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_GIF_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_GIF_ActiveDrawEx;
				ImageFun.DrawScaledEx = GUI_GIF_ActiveDrawScaledEx;
				ImageFun.GetDataEx		= _GetData;
	
	}
	else if(strstr(file_name,".png")||strstr(file_name,".PNG"))
	{
			
				ImageFun.GetXSizeEx 	= GUI_PNG_GetXSizeEx;
				ImageFun.GetYSizeEx		= GUI_PNG_GetYSizeEx;
				ImageFun.DrawEx     	= GUI_PNG_DrawEx;
				ImageFun.DrawScaledEx = GUI_PNG_DrawScaledEx;								//PNG没有scaled画法，
				ImageFun.GetDataEx		= _GetPNGData;
	}
	else 
		return; //不支持的格式，跳出函数	
	
	fres = f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING| FA_OPEN_ALWAYS );
	DEBUG("\r\n open extsting fres = %d",fres); 	
	
	XSize = ImageFun.GetXSizeEx(ImageFun.GetDataEx, &hFile);
  YSize = ImageFun.GetYSizeEx(ImageFun.GetDataEx, &hFile);
	
	DEBUG("\r\nXSize = %d ,YSize= %d ",XSize,YSize); 	

#if DRAW_SCALE		//按照比例缩放图片，使图片完整地在屏幕显示
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* 根据窗口按比例缩放图片 */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* 计算图片位置，使图片居中 */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* 按比例显示图片 */
			//while (WM_IsWindow(hParent)) 
			{
				ImageFun.DrawScaledEx(ImageFun.GetDataEx,&hFile,0,YPos,XSizeWin,XSize);
				GUI_Delay(10);	
				}	
			}
		else
		{
			
			/* 计算图片位置，使图片居中 */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* 按比例显示图片 */
			//while (WM_IsWindow(hParent)) 
			{
				ImageFun.DrawScaledEx(ImageFun.GetDataEx,&hFile,XPos,0,YSizeWin,YSize);	
				GUI_Delay(10);
			}		
		}			
	}
	else
	{
		/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
		//while (WM_IsWindow(hParent) ) 
		{
			ImageFun.DrawEx(ImageFun.GetDataEx, &hFile, XPos, YPos);
			GUI_Delay(10);
		}			
	}
	
#else
	
			/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
		//直接显示到客户窗口
		//while (WM_IsWindow(hParent)) 
		{		
			ImageFun.DrawEx(ImageFun.GetDataEx, &hFile, XPos, YPos); 
			GUI_Delay(10);	
		}			

#endif	
	
  f_close(&hFile);
	
}


/**
  * @brief  ImageReader_FrameWin 使用framewin显示图片
  * @param  file_name 图片文件名
	*		
  * @retval 无
  */
void ImageReader_FrameWin( char * file_name) {
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	
	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN2,
															"Image Reader",
															0);
	
		/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);

	
	/* 在客户窗口显示图片 */
	ImageReader_Raw(file_name,hFrameC);

}




/*BMPReader JPEGReader GIFReader PNGReader这些函数已经不使用了，
*	但可以作为学习教程的示例，
*	因为每种格式都是独立的，
*	没有使用函数指针，相对简单，
*/
#if 0 
/**
  * @brief  BMPReader显示sd卡上的bmp图片
  * @param  
	*		
  * @retval 无
  */
void BMPReader( char * file_name) {
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hImage;
	
  int XSize, YSize, XPos, YPos,XSizeWin,YSizeWin;
	
  FIL hFile ;
	FRESULT fres;
	
	
	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN2,
															"BMP Reader",
															0);
	
		/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);

	/* 立即重绘窗口，否则图片会在窗口重绘时被刷掉 */
	WM_PaintWindowAndDescs(hFrame);
	//WM_Paint(hFrameC);	

	//选择其子窗口用于绘制操作
	WM_SelectWindow(hFrameC);
	


	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);
	
	
	fres = f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING| FA_OPEN_ALWAYS );
	DEBUG("\r\n open extsting fres = %d",fres); 		  
	
	
  GUI_ClearRect(0, 0,XSizeWin, YSizeWin);
  
	XSize = GUI_BMP_GetXSizeEx(_GetData, &hFile);
  YSize = GUI_BMP_GetYSizeEx(_GetData, &hFile);
	
	DEBUG("\r\nXSize = %d ,YSize= %d ",XSize,YSize); 	

#if DRAW_SCALE
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* 根据窗口按比例缩放图片 */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* 计算图片位置，使图片居中 */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* 按比例显示图片 */
			GUI_BMP_DrawScaledEx(_GetData,&hFile,0,YPos,XSizeWin,XSize);
		}
		else
		{
			
			/* 计算图片位置，使图片居中 */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* 按比例显示图片 */
			GUI_BMP_DrawScaledEx(_GetData,&hFile,XPos,0,YSizeWin,YSize);	
		}			
	}
	else
	{
		/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
		GUI_BMP_DrawEx(_GetData, &hFile, XPos, YPos); 
	}
	
#else
	
			/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
	#if 1		
		//直接显示到客户窗口
		//_ShowBMP(file_name); //使用整幅图像加载到存储器的方式
	
		GUI_BMP_DrawEx(_GetData, &hFile, XPos, YPos); 	
	#else		
		//使用Image控件显示,感觉用处不大，不能按比例调整
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetBMPEx(hImage,_GetData, &hFile);
		
		GUI_Delay(10);
	#endif
#endif	
	
  f_close(&hFile);
	

}


/**
  * @brief  JPEGRead显示sd卡上的jpeg图片
  * @param  file_name 文件路径
	*		
  * @retval 无
  */
void JPEGReader( char * file_name) {
  int XPos, YPos, XSize, YSize, XSizeWin,YSizeWin,nx, ny, n;
  FIL hFile;
  GUI_JPEG_INFO Info;
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN	hImage;
	
	FRESULT fres;
	int fre;
	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN3,
															"JPG Reader",
															0);
	
		/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//获取旧的回调函数指针


	FRAMEWIN_SetText(hFrame,file_name);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);

	/* 立即重绘窗口，否则图片会在窗口重绘时被刷掉 */
	WM_PaintWindowAndDescs(hFrame);

	//选择其子窗口用于绘制操作
	WM_SelectWindow(hFrameC);
	
	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);
	
	/* 清空背景 */
	GUI_ClearRect(0, 0,XSizeWin, YSizeWin);

  fres = f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING | FA_OPEN_ALWAYS); 
	
	/* 获取图像信息 */
  GUI_JPEG_GetInfoEx(_GetData, &hFile, &Info); 

  XSize = Info.XSize ;
  YSize = Info.YSize ;
	
	
#if DRAW_SCALE
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* 根据窗口按比例缩放图片 */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* 计算图片位置，使图片居中 */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* 按比例显示图片 */
			GUI_JPEG_DrawScaledEx(_GetData,&hFile,0,YPos,XSizeWin,XSize);
		}
		else
		{
			
			/* 计算图片位置，使图片居中 */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* 按比例显示图片 */
			GUI_JPEG_DrawScaledEx(_GetData,&hFile,XPos,0,YSizeWin,YSize);	
		}			
	}
	else
	{
		/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
		GUI_JPEG_DrawEx(_GetData, &hFile, XPos, YPos); 
	}
	
#else
	
			/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//直接显示到客户窗口
		//_ShowJPG(file_name);	//整幅图片加载到sram空间再显示
		
		GUI_JPEG_DrawEx(_GetData, &hFile, XPos, YPos);
		GUI_Delay(10);
	#else		
		//使用Image控件显示
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetJPEGEx(hImage,_GetData, &hFile);		
		GUI_Delay(10);
	
	#endif
	
#endif	

  f_close(&hFile);

}



/**
  * @brief  GIFReader显示sd卡上的gif图片
  * @param  
	*		
  * @retval 无
  */
void GIFReader( char * file_name) {
	
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hImage;
	FRESULT fres;
  int XSize, YSize, XSizeWin,YSizeWin, XPos, YPos,i,j;
  FIL hFile ;
	
  GUI_GIF_INFO       GifInfo   = {0}; /* Info structure of GIF file */
  GUI_GIF_IMAGE_INFO ImageInfo = {0}; /* Info structure of one particular GIF image of the GIF file */

	
	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW|WM_CF_MEMDEV,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN4,
															"GIF Reader",
															0);
	
		/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	
	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame,file_name);
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);
	
	/* 立即重绘窗口，否则图片会在窗口重绘时被刷掉 */
	WM_PaintWindowAndDescs(hFrame);
	//WM_Paint(hFrameC);	
	
	//选择其子窗口用于绘制操作
	WM_SelectWindow(hFrameC);
	
	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);	

  GUI_ClearRect(0, 0, XSizeWin, YSizeWin);                           /* Clear the image area */
	
  f_open(&hFile,file_name,FA_READ|FA_OPEN_EXISTING | FA_OPEN_ALWAYS);

  GUI_GIF_GetInfoEx(_GetData, &hFile, &GifInfo);                     /* Get GIF info structure */
	
	XSize = GifInfo.xSize;
	YSize = GifInfo.ySize;
	
		
#if DRAW_SCALE
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* 根据窗口按比例缩放图片 */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* 计算图片位置，使图片居中 */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* 按比例显示图片 */
			//GUI_JPEG_DrawScaledEx(_GetData,&hFile,0,YPos,XSizeWin,XSize);
			
			while (WM_IsWindow(hFrameC)) 
			{                                       /* Show the complete GIF  ... */
					for (j = 0; j < GifInfo.NumImages; j++) 
				{
						GUI_GIF_DrawSubScaledEx(_GetData, &hFile, 0, YPos,j,XSizeWin,XSize);            /* Draw sub image */
						GUI_GIF_GetImageInfoEx(_GetData, &hFile, &ImageInfo, j);       /* Get sub image information */
						GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
					}		
			}
		}
		else
		{
			
			/* 计算图片位置，使图片居中 */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* 按比例显示图片 */
			
			while (WM_IsWindow(hFrameC)) 
 
			{                                       /* Show the complete GIF ... */
					for (j = 0; j < GifInfo.NumImages; j++) 
				{
						GUI_GIF_DrawSubScaledEx(_GetData, &hFile, XPos, 0,j,YSizeWin,YSize);            /* Draw sub image */
						GUI_GIF_GetImageInfoEx(_GetData, &hFile, &ImageInfo, j);       /* Get sub image information */
						GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
					}		
			}
		}			
	}
	else
	{
		/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* 以原比例显示图片*/
		
		while (WM_IsWindow(hFrameC)) 

		{                                       /* Show the complete GIF  ... */
				for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubEx(_GetData, &hFile, XPos, YPos,j);            /* Draw sub image */
					GUI_GIF_GetImageInfoEx(_GetData, &hFile, &ImageInfo, j);       /* Get sub image information */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
				}		
			}
	}
	
#else
	
			/* 计算图片位置，使图片居中 */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//直接显示到客户窗口		

		while (WM_IsWindow(hFrameC)) 
		{                                       /* Show the complete GIF  ... */
				for (j = 0; j < GifInfo.NumImages; j++) 
			{
					GUI_GIF_DrawSubEx(_GetData, &hFile, XPos, YPos,j);            /* Draw sub image */
					GUI_GIF_GetImageInfoEx(_GetData, &hFile, &ImageInfo, j);       /* Get sub image information */
					GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
				}		
			}
					
		GUI_Delay(10);
	#else		
		//使用Image控件显示
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
					
		while (WM_IsWindow(hFrameC)) 
			{                                       /* Show the complete GIF  ... */
					for (j = 0; j < GifInfo.NumImages; j++) 
				{
						GUI_GIF_DrawSubEx(_GetData, &hFile, 0, YPos,j);            /* Draw sub image */
						GUI_GIF_GetImageInfoEx(_GetData, &hFile, &ImageInfo, j);       /* Get sub image information */
						GUI_Delay(ImageInfo.Delay ? ImageInfo.Delay * 10 : 100);    /* Use the Delay member of the ImageInfo structure for waiting a while */
					}		
				}
				
		GUI_Delay(10);
	
	#endif
	
#endif	
}


/**
  * @brief  PNGReader显示sd卡上的png图片
  * @param  file_name 文件路径
	*		
  * @retval 无
  */
void PNGReader( char * file_name) {
  FIL hFile;
  int XSize, YSize, XSizeWin,YSizeWin, XPos, YPos;
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hImage;
	
	
	hFrame = FRAMEWIN_CreateEx(	0,
															0,
															WinPara.xSizeWin,
															WinPara.ySizeWin,
															WinPara.hWinMain,
															WM_CF_SHOW,
															FRAMEWIN_CF_ACTIVE/*|FRAMEWIN_CF_MOVEABLE*/,
															GUI_ID_FRAMEWIN3,
															"PNG Reader",
															0);
	
		/* 创建窗口按钮 */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//获取旧的回调函数指针

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* 立即重绘窗口，否则图片会在窗口重绘时被刷掉 */
	WM_PaintWindowAndDescs(hFrame);
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);
	
	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);	
	
	//选择其子窗口用于绘制操作
	WM_SelectWindow(hFrameC);
	
	GUI_ClearRect(0, 0, XSizeWin, YSizeWin);                           /* Clear the image area */

	
  f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING | FA_OPEN_ALWAYS);
	
  XSize = GUI_PNG_GetXSizeEx(_GetPNGData, &hFile);
  YSize = GUI_PNG_GetYSizeEx(_GetPNGData, &hFile);

	
		/* 计算图片位置，使图片居中 */
	XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
	YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//直接显示到客户窗口
		
		GUI_PNG_DrawEx(_GetPNGData, &hFile, XPos, YPos);
		GUI_Delay(10);
	#else		
		//使用Image控件显示
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetPNGEx(hImage,_GetPngData, &hFile);		
		GUI_Delay(10);
	
	#endif

  f_close(&hFile);
}


#endif



/**
  * @brief  _cbImageAPPWin,图片APP回调函数
  * @param  none
  * @retval none
  */
static void _cbImageAPPWin(WM_MESSAGE * pMsg)
{
	WM_HWIN   hWin;
	WM_HWIN		hButton;
	WM_HWIN 	hImage;
	WM_HWIN 	hText ;


	
	int        NCode;
  int        Id;
	int        Sel;
	
	char 			 i;
	
	static int ImageIndex = 0;
	
	hWin = pMsg->hWin;	

	switch (pMsg->MsgId) 
	{
		case WM_CREATE:	
			/* 创建按键 */
			hButton = BUTTON_CreateEx(WinPara.xSizeWin-40,WinPara.ySizeWin-20,20, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_STAYONTOP,0, GUI_ID_BUTTON0);
		
			BUTTON_SetText(hButton, "<");
		
		
			hButton = BUTTON_CreateEx(WinPara.xSizeWin-20,WinPara.ySizeWin-20,20, 20, 
																		pMsg->hWin, WM_CF_SHOW|WM_CF_STAYONTOP,0, GUI_ID_BUTTON1);
		
			BUTTON_SetText(hButton, ">");
		
			/* 文件路径 */
			hText = TEXT_CreateEx(0,WinPara.ySizeWin-20,200,20,hWin,WM_CF_SHOW|WM_CF_HASTRANS,TEXT_CF_HCENTER|TEXT_CF_VCENTER,GUI_ID_TEXT0,NULL);
	
			TEXT_SetFont(hText,&GUI_Font16_ASCII);
			TEXT_SetTextColor(hText,GUI_WHITE);			
		
			break;			
		
		case WM_NOTIFY_PARENT:
				Id    = WM_GetId(pMsg->hWinSrc);      // 控件的ID
				NCode = pMsg->Data.v;                 // 通知代码
			 
			 if(NCode == WM_NOTIFICATION_RELEASED)
				 {
				 
					 if(Id == GUI_ID_BUTTON0)
					 {					 
						 if(ImageIndex != 0)
						{					
							
							 Image_Display(--ImageIndex,hWin);
						 }
						 					
					 } 
					 else if(Id == GUI_ID_BUTTON1)
					 {						 
							if(ImageIndex<ImageNum )
							{
								
								Image_Display(++ImageIndex,hWin);
							}
						}
						 
				 }
			
			
			
		break;
			
		
		
		case WM_DELETE:			
				
			/* 删除app句柄链表里的记录 */	
			App_Delete(pMsg->hWin);
		
			/* 发送消息通知ctrl窗口*/		
			WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
			break;
		
		case WM_PAINT:
			
			GUI_SetColor(GUI_BLACK);
			GUI_FillRect(0,0,WM_GetWindowSizeX(pMsg->hWin),WM_GetWindowSizeY(pMsg->hWin));
		
			break;
		
		default:
			break;
		
	}
	
	
}





/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径  hTree 目录树 hNode 目录结点
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path,char* file_name,FIL *hFile  ) 
{ 
		
    FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
		unsigned int rw_num;			//已读或已写的字节数

    DIR dir; 
    int i; 
    char *fn; 
	
	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//长文件名支持
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path); //打开目录
    if (res == FR_OK) 
			{ 
        i = strlen(path); 
        for (;;) 
				{ 
            res = f_readdir(&dir, &fno); 										//读取目录下的内容
            if (res != FR_OK || fno.fname[0] == 0) break; 	//为空时表示所有项目读取完毕，跳出
#if _USE_LFN 
            fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
            fn = fno.fname; 
#endif 
            if (*fn == '.') continue; 											//点表示当前目录，跳过			
            if (fno.fattrib & AM_DIR) 
						{ 																							//目录，递归读取		
							  sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path,file_name,hFile);					//递归遍历 
                if (res != FR_OK) 
									break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
						else 
						{ 
							if(strstr(fn,".bmp")||strstr(fn,".BMP")||
										strstr(fn,".jpg")||strstr(fn,".JPG")||
											strstr(fn,".gif")||strstr(fn,".GIF")||
												strstr(fn,".png")||strstr(fn,".PNG"))//判断是否mp3或wav文件
								{
									DEBUG("%s/%s  \r\n", path, fn);								//输出文件名	
									
									if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
									{
										sprintf(file_name, "%s/%s", path,fn); 	
										//存储文件名到filelist(含路径)										
										res = f_lseek (hFile, ImageNum*FILE_NAME_LEN);  
										res = f_write (hFile, file_name, FILE_NAME_LEN, &rw_num);						
									
										ImageNum++;	

										DEBUG(" imagenum =%d  \r\n", ImageNum);								//输出文件名	
										
										}		

								}							
           }//else
        } //for
    } 

    return res; 
} 


/**
  * @brief  Fill_TreeView处理非递归过程，然后调用递归函数scan_files扫描目录
	*					
  * @param  path:初始扫描路径
  * @retval none
  */
static void Image_scanf(char* path,char* record_file)
{
	char * p_path;									//目录名 指针
	char * file_name;								//用于存储的目录文件名，
	FIL		hFile;										//文件句柄	
	FRESULT fres;										//返回结果
	
	
	fres = f_unlink(record_file);//删除旧的filelist		// TBD 增加自建目录

	p_path = (char * ) malloc(PATH_LEN* sizeof(char));  //为存储目录名的指针分配空间
	file_name = (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //为存储目录名的指针分配空间
	
	fres = f_open (&hFile, record_file, FA_READ|FA_WRITE|FA_CREATE_ALWAYS ); //打开创建索引文件

	strcpy(p_path,path);						//复制目录名到指针
	
	fres = scan_files(p_path,file_name,&hFile);			//递归扫描歌曲文件		
	
	fres = f_close (&hFile);					//关闭索引文件		

	free(p_path);										//释放malloc空间		
	p_path = NULL;
	
	free(file_name);								//释放malloc空间		
	file_name = NULL;

	

}

/**
  * @brief  Image_Display 显示图片
	*					
  * @param  none
  * @retval none
  */
static void Image_Display(int sel_num ,WM_HWIN hParent)
{
		WM_HWIN hText;
	
		char* file_name;
		FIL hFile;
		FRESULT fres;							//返回结果		
		unsigned int rw_num;			//已读或已写的字节数

		//打开文件
		
		file_name 	= (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //为存储目录名的指针分配空间
		
		fres = f_open (&hFile, IMAGE_LIST_PATH, FA_READ ); 		//打开创建索引文件
		fres = f_lseek (&hFile, sel_num*FILE_NAME_LEN);				//根据索引值查找将要打开文件的路径
		fres = f_read(&hFile, file_name, FILE_NAME_LEN, &rw_num);
		fres = f_close (&hFile);	
		
		/* 显示图片路径 */
		hText = WM_GetDialogItem(hParent,GUI_ID_TEXT0);	
		TEXT_SetText(hText,file_name);

		DEBUG("\r\n filename = %s\r\n",file_name);
	

		/* 显示图片到窗口 */
		ImageReader_Raw(file_name,hParent);	
	
		free(file_name);								//释放malloc空间		
		file_name = NULL;

}




/**
  * @brief  WFGUI_ImageReader 图片浏览器主函数
	*					
  * @param  none
  * @retval none
  */
void WFGUI_ImageReader(void)
{
	
	WM_HWIN hImage;	
	
	/* 创建图片浏览器窗口 */
	hImage = WM_CreateWindowAsChild(0,
																	0,
																	WinPara.xSizeWin,
																	WinPara.ySizeWin ,
																	WinPara.hWinMain ,
																	WM_CF_SHOW , 
																	_cbImageAPPWin,
																	0);	
	
		/* 把app句柄插入链表 */
	App_Insert(hImage);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);	

	/* 扫描图片文件 */
	Image_scanf("0:",IMAGE_LIST_PATH);
	
	GUI_Delay(10);	

	/* 显示第一幅图像 */
	Image_Display(0,hImage);


}




