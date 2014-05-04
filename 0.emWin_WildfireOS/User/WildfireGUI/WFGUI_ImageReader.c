
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "bsp_usart1.h"
#include "WFGUI_Common.h"
#include "WFGUI_ImageReader.h"

//	TBD ����ЩͼƬ��ʾ����������һ��

/* ֱ�Ӷ����char���ͣ��п��ܵ�ַ�����룬����DMA������� */
//char _acBuffer[1024*4];

/* �����4�ֽڶ��룬��ֹDMA������� */
__align(4) char _acBuffer[1024*4] ;		

/*Ҳ������������,��������ͣ��Ӷ��������Զ����䵽4�ֽڶ���Ŀռ�*/
//int _acIntBuffer[1024];


static WM_CALLBACK*     _pcbOldImageWin = NULL;






/**
  * @brief  _GetData���ڶ�ȡ��洢��ͼ��ĺ��� ������bmp jpg gif�ļ�
  * @param   p           - Pointer to application defined data.
	*					 NumBytesReq - Number of bytes requested.	
	*  				 ppData      - Pointer to data pointer. This pointer should be set to
	*                 				a valid location.
	*  				 StartOfFile - If this flag is 1, the data pointer should be set to the
	*                 				beginning of the data stream.

  * @retval ��ȡ�����ֽ���
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
	#if 0	/* TBD ֱ�Ӷ�ȡ���ݣ���32��24λͼƬʱ������ */
		fres =  f_read(phFile, _acBuffer, NumBytesReq, &NumBytesRead);
	#else
		/* ��֪Ϊ�Σ���ȡ24��32λͼ��ʱ��
		*	ÿ�����󳬹�512�ֽڵ�ʱ�������ֿ����ζ�ȡû���⣬
		*	�����ǵײ�������,��������������ļ�ϵͳ��û���� ������Ҳ�ǵ�ַ�ռ���������*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//����ж��ٸ����ݿ�
		 unsigned int   remain = NumBytesReq%512;		//����һ����ֽ���Ŀ
		 
		 for(i=0;i<count;i++)
		 {
			 /* ��ȡ512�ֽ� */
			fres = f_read(phFile, _acBuffer + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)/* ��ȡʣ���ֽ� */
		 {
			fres = f_read(phFile, _acBuffer + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* С��512�ֽڵ���� */
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
  * @brief  _GetPngData���ڶ�ȡ��洢��ͼ��ĺ��� ������gng�ļ�
  * @param  
	*		
  * @retval ��
  */
static int _GetPngData(void * p, const U8 ** ppData, unsigned NumBytesReq, U32 Off) {
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

	#if 0	/* TBD ֱ�Ӷ�ȡ���ݣ���32��24λͼƬʱ������ */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead);
	#else
		/* ��֪Ϊ�Σ���ȡ24��32λͼ��ʱ��
		*	ÿ�����󳬹�512�ֽڵ�ʱ�������ֿ����ζ�ȡû���⣬
		*	�����ǵײ�������,��������������ļ�ϵͳ��û���� ������Ҳ�ǵ�ַ�ռ���������*/
	 if(NumBytesReq>512)		
	 {	 
		 unsigned char  i;
		 unsigned int		read_count = 0;
		 unsigned char  count = NumBytesReq/512;		//����ж��ٸ����ݿ�
		 unsigned int   remain = NumBytesReq%512;		//����һ����ֽ���Ŀ
		 
		 for(i=0;i<count;i++)
		 {
			 /* ��ȡ512�ֽ� */
			fres = f_read(phFile, pData + i*512, 512, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 if(remain)/* ��ȡʣ���ֽ� */
		 {
			fres = f_read(phFile, pData + i*512, remain, &NumBytesRead);
			read_count += NumBytesRead;

		 }
		 
		NumBytesRead =  read_count;
	 }
	 else	/* С��512�ֽڵ���� */
		fres =  f_read(phFile, pData, NumBytesReq, &NumBytesRead); 

 #endif
  //
  // Return number of available bytes
  //
  return NumBytesRead;
}



/**
  * @brief  _cbImageWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbImageWin(WM_MESSAGE * pMsg)
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
	
	if (_pcbOldImageWin) {
			(*_pcbOldImageWin)(pMsg);	//���þɵĻص�����������δ�������
		}	
	
}

/**
  * @brief  BMPReader��ʾsd���ϵ�bmpͼƬ
  * @param  
	*		
  * @retval ��
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
	
		/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//��ȡ�ɵĻص�����ָ��

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);

	/* �����ػ洰�ڣ�����ͼƬ���ڴ����ػ�ʱ��ˢ�� */
	WM_PaintWindowAndDescs(hFrame);
	//WM_Paint(hFrameC);	

	//ѡ�����Ӵ������ڻ��Ʋ���
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
		/* ���ݴ��ڰ���������ͼƬ */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* ��������ʾͼƬ */
			GUI_BMP_DrawScaledEx(_GetData,&hFile,0,YPos,XSizeWin,XSize);
		}
		else
		{
			
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* ��������ʾͼƬ */
			GUI_BMP_DrawScaledEx(_GetData,&hFile,XPos,0,YSizeWin,YSize);	
		}			
	}
	else
	{
		/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
		GUI_BMP_DrawEx(_GetData, &hFile, XPos, YPos); 
	}
	
#else
	
			/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
	#if 1		
		//ֱ����ʾ���ͻ�����
		//_ShowBMP(file_name); //ʹ������ͼ����ص��洢���ķ�ʽ
	
		GUI_BMP_DrawEx(_GetData, &hFile, XPos, YPos); 	
	#else		
		//ʹ��Image�ؼ���ʾ,�о��ô����󣬲��ܰ���������
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetBMPEx(hImage,_GetData, &hFile);
		
		GUI_Delay(10);
	#endif
#endif	
	
  f_close(&hFile);
	

}




/**
  * @brief  JPEGRead��ʾsd���ϵ�jpegͼƬ
  * @param  file_name �ļ�·��
	*		
  * @retval ��
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
	
		/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//��ȡ�ɵĻص�����ָ��


	FRAMEWIN_SetText(hFrame,file_name);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);

	/* �����ػ洰�ڣ�����ͼƬ���ڴ����ػ�ʱ��ˢ�� */
	WM_PaintWindowAndDescs(hFrame);

	//ѡ�����Ӵ������ڻ��Ʋ���
	WM_SelectWindow(hFrameC);
	
	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);
	
	/* ��ձ��� */
	GUI_ClearRect(0, 0,XSizeWin, YSizeWin);

  fres = f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING | FA_OPEN_ALWAYS); 
	
	/* ��ȡͼ����Ϣ */
  GUI_JPEG_GetInfoEx(_GetData, &hFile, &Info); 

  XSize = Info.XSize ;
  YSize = Info.YSize ;
	
	
#if DRAW_SCALE
	if((XSize > XSizeWin) || (YSize > YSizeWin))	
	{		
		/* ���ݴ��ڰ���������ͼƬ */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* ��������ʾͼƬ */
			GUI_JPEG_DrawScaledEx(_GetData,&hFile,0,YPos,XSizeWin,XSize);
		}
		else
		{
			
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* ��������ʾͼƬ */
			GUI_JPEG_DrawScaledEx(_GetData,&hFile,XPos,0,YSizeWin,YSize);	
		}			
	}
	else
	{
		/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
		GUI_JPEG_DrawEx(_GetData, &hFile, XPos, YPos); 
	}
	
#else
	
			/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//ֱ����ʾ���ͻ�����
		//_ShowJPG(file_name);	//����ͼƬ���ص�sram�ռ�����ʾ
		
		GUI_JPEG_DrawEx(_GetData, &hFile, XPos, YPos);
		GUI_Delay(10);
	#else		
		//ʹ��Image�ؼ���ʾ
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetJPEGEx(hImage,_GetData, &hFile);		
		GUI_Delay(10);
	
	#endif
	
#endif	

  f_close(&hFile);

}



/**
  * @brief  GIFReader��ʾsd���ϵ�gifͼƬ
  * @param  
	*		
  * @retval ��
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
	
		/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	
	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//��ȡ�ɵĻص�����ָ��

	
	FRAMEWIN_SetText(hFrame,file_name);
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);
	
	/* �����ػ洰�ڣ�����ͼƬ���ڴ����ػ�ʱ��ˢ�� */
	WM_PaintWindowAndDescs(hFrame);
	//WM_Paint(hFrameC);	
	
	//ѡ�����Ӵ������ڻ��Ʋ���
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
		/* ���ݴ��ڰ���������ͼƬ */
		if(XSize/XSizeWin > YSize/YSizeWin)
		{
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			YPos = (YSizeWin - YSize*XSizeWin/XSize )/ 2;
			
			/* ��������ʾͼƬ */
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
			
			/* ����ͼƬλ�ã�ʹͼƬ���� */
			XPos = (XSizeWin - XSize*YSizeWin/YSize )/ 2;
			
			/* ��������ʾͼƬ */
			
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
		/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
		
		/* ��ԭ������ʾͼƬ*/
		
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
	
			/* ����ͼƬλ�ã�ʹͼƬ���� */
		XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
		YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//ֱ����ʾ���ͻ�����		

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
		//ʹ��Image�ؼ���ʾ
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
  * @brief  PNGReader��ʾsd���ϵ�pngͼƬ
  * @param  file_name �ļ�·��
	*		
  * @retval ��
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
	
		/* �������ڰ�ť */
	FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
//	FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
//	FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);


	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldImageWin = WM_SetCallback(hFrame, _cbImageWin );	//��ȡ�ɵĻص�����ָ��

	
	FRAMEWIN_SetText(hFrame,file_name);
	
	/* �����ػ洰�ڣ�����ͼƬ���ڴ����ػ�ʱ��ˢ�� */
	WM_PaintWindowAndDescs(hFrame);
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);
	
	XSizeWin = WM_GetWindowSizeX(hFrameC);
	YSizeWin = WM_GetWindowSizeY(hFrameC);	
	
	//ѡ�����Ӵ������ڻ��Ʋ���
	WM_SelectWindow(hFrameC);
	
	GUI_ClearRect(0, 0, XSizeWin, YSizeWin);                           /* Clear the image area */

	
  f_open(&hFile,file_name,  FA_READ|FA_OPEN_EXISTING | FA_OPEN_ALWAYS);
	
  XSize = GUI_PNG_GetXSizeEx(_GetPngData, &hFile);
  YSize = GUI_PNG_GetYSizeEx(_GetPngData, &hFile);

	
		/* ����ͼƬλ�ã�ʹͼƬ���� */
	XPos = (XSize > XSizeWin) ?  0 : (XSizeWin - XSize )/ 2;
	YPos = (YSize > YSizeWin) ?  0 : (YSizeWin - YSize )/ 2;
	
	#if 1
		
		//ֱ����ʾ���ͻ�����
		
		GUI_PNG_DrawEx(_GetPngData, &hFile, XPos, YPos);
		GUI_Delay(10);
	#else		
		//ʹ��Image�ؼ���ʾ
		hImage = IMAGE_CreateEx(0,0,XSizeWin,YSizeWin,hFrameC,WM_CF_SHOW,0,GUI_ID_IMAGE0);
		IMAGE_SetPNGEx(hImage,_GetPngData, &hFile);		
		GUI_Delay(10);
	
	#endif

  f_close(&hFile);
}