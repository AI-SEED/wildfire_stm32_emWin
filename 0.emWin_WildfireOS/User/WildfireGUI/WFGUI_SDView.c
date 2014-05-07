
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ff.h"
#include "DIALOG.h"

#include "WFGUI_Common.h"
#include "WFGUI_SDView.h"
#include "WFGUI_TextReader.h"
#include "WFGUI_ImageReader.h"


static FRESULT scan_files (char* path,char*file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode) ;
static void OpenFileProcess(int sel_num);



 //char file_name[FILE_NAME_LEN];

	
//static		FIL	file;								//文件句柄	
//static  	FRESULT fres;							//返回结果
//static		unsigned int rw_num;			//已读或已写的字节数
static WM_CALLBACK*     _pcbOldFrame = NULL;
static WM_CALLBACK*     _pcbOldSDViewWin = NULL;

int xSize,ySize;

/**
  * @brief  对话框用到的小工具资源表
	*					
  * @param  none
  * @retval none
  */
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { FRAMEWIN_CreateIndirect, "Message",    0,      20,  90, 200, 100, FRAMEWIN_CF_MOVEABLE },
  { TEXT_CreateIndirect,     "Do you sure to open the file?",     	GUI_ID_TEXT0,    10,  10, 180,  80 },
  { BUTTON_CreateIndirect,   "Yes",   	GUI_ID_YES,     10, 50,  60,  18 },
  { BUTTON_CreateIndirect,   "No", 			GUI_ID_NO,  120, 50,  60,  18 },
};

/*********************************************************************
*
*       对话框回调函数
*/
static void _cbDlgWin(WM_MESSAGE * pMsg) {
  int NCode, Id;
  WM_HWIN hDlg, hItem;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:									//初始化对话框时
			hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
			TEXT_SetFont(hItem,GUI_FONT_16_ASCII);	//设置text的字体
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {

      case WM_NOTIFICATION_RELEASED:      /* React only if released */
        if (Id == GUI_ID_YES) {           /* Yes Button */     
					GUI_EndDialog(hDlg, 0);					//结束对话框，返回0
        }
        if (Id == GUI_ID_NO) {        		/* No Button */
          GUI_EndDialog(hDlg, 1);					//结束对话框，返回1
        }
        break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}




/**
  * @brief  _cbSDViewWin framewin框架的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWin(WM_MESSAGE * pMsg)
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
  * @brief  _cbSDViewWinC framewin用户窗口的回调函数
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWinC(WM_MESSAGE * pMsg)
{
	TREEVIEW_ITEM_INFO ItemInfo;
	TREEVIEW_ITEM_Handle hNode;
	
  switch (pMsg->MsgId) {
		
		case WM_NOTIFY_PARENT:	 //通知代码
			
			switch(pMsg->Data.v)	//通知代码类型
			{
				case WM_NOTIFICATION_RELEASED:	//释放了
					DEBUG("\r\n release");
				
				/* 查看选中了哪个项目 */
				hNode = TREEVIEW_GetSel(pMsg->hWinSrc);		
				/* 获取该项目的信息 */
				TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
				
				if(ItemInfo.IsNode == 0)//叶子
				{
					DEBUG("\r\n leaf num =%ld",hNode);					

					/* 官方手册说切勿在回调函数中调用阻塞对话框，但这里调用了也没错误，就先这样用吧 */
					/* 不在回调函数调用的话就可另外作标记了 */
					/* 选yes 返回0 选no返回1*/
//					if(	GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDlgWin, WinPara.hWinMain, 0, 0) == 0)
//					{
//									//open file
//							DEBUG("\r\n open file");
							OpenFileProcess(hNode);
//						}
						
					//设置选定为该叶子的父结点，防止误触发
					//TREEVIEW_SetSel(pMsg->hWinSrc,TREEVIEW_GetItem(pMsg->hWinSrc,hNode,TREEVIEW_GET_PARENT));

				}
				else										//结点
				{					
					DEBUG("\r\n node num =%ld",hNode);
				}
				break;				
				
			}	
			
		
    default:		
      WM_DefaultProc(pMsg);
  }

}



/**
  * @brief  OpenFileProcess打开文件	// TBD jpg显示不正常 
	*					
  * @param  none
  * @retval none
  */
static void OpenFileProcess(int sel_num)
{
	char* file_name;
	char* read_buffer; 
	FIL hFile;
	FRESULT fres;							//返回结果
	unsigned int rw_num;			//已读或已写的字节数

	
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	/* 执行对话框，确定是否要打开文件，返回0表示确定 */
	if(	GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDlgWin, WinPara.hWinMain, 0, 0) == 0)
	{
			//打开文件
		
			file_name 	= (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //为存储目录名的指针分配空间
			
			fres = f_open (&hFile, FILE_LIST_PATH, FA_READ ); 		//打开创建索引文件
			fres = f_lseek (&hFile, sel_num*FILE_NAME_LEN);				//根据索引值查找将要打开文件的路径
			fres = f_read(&hFile, file_name, FILE_NAME_LEN, &rw_num);
			fres = f_close (&hFile);
			
			if(strstr(file_name,".txt")||strstr(file_name,".TXT")
					||strstr(file_name,".c")||strstr(file_name,".c")
						||strstr(file_name,".cpp")||strstr(file_name,".CPP")
								||strstr(file_name,".h")||strstr(file_name,".h"))//判断是否可以使用txt格式打开
			{
				//	TBD file_name 是malloc来的，作为形参传入函数，小心出错
				TextReader(file_name);
				
			}
			else if(strstr(file_name,".bmp")||strstr(file_name,".BMP"))
			{
				BMPReader(file_name);
			}
			else if(strstr(file_name,".jpg")||strstr(file_name,".JPG"))
			{
				JPEGReader(file_name);
			}
			else if(strstr(file_name,".png")||strstr(file_name,".PNG"))
			{
				PNGReader(file_name);
			}
			else if(strstr(file_name,".gif")||strstr(file_name,".GIF"))
			{
				GIFReader(file_name);
			}
			else if(strstr(file_name,".mp3")||strstr(file_name,".MP3")||strstr(file_name,".wav")||strstr(file_name,".WAV"))
			{
				GUI_MessageBox("Sorry,do not support open the file!","Sorry",GUI_MESSAGEBOX_CF_MOVEABLE);
			}
			else
			{				
				GUI_MessageBox("Sorry,do not support open the file!","Sorry",GUI_MESSAGEBOX_CF_MOVEABLE);

				}

			
			free(file_name);								//释放malloc空间		
			file_name = NULL;
				
		}	

}


/**
  * @brief  scan_files 递归扫描sd卡内的歌曲文件
  * @param  path:初始扫描路径  hTree 目录树 hNode 目录结点
  * @retval result:文件系统的返回值
  */
static FRESULT scan_files (char* path,char* file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode) 
{ 
		
    FRESULT res; 		//部分在递归过程被修改的变量，不用全局变量	
    FILINFO fno; 
		unsigned int rw_num;			//已读或已写的字节数

    DIR dir; 
    int i; 
    char *fn; 
	
		TREEVIEW_ITEM_Handle hItem;

	
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
								hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);						//目录，创建结点
								TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把结点加入到目录树中
							
							  sprintf(&path[i], "/%s", fn); 							//合成完整目录名
                res = scan_files(path,file_name,hFile,hTree,hItem);					//递归遍历 
                if (res != FR_OK) 
									break; 																		//打开失败，跳出循环
                path[i] = 0; 
            } 
						else 
						{ 
							DEBUG("%s/%s hItem = %d  \r\n", path, fn,(int)hItem);								//输出文件名	
							hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);						//文件，创建树叶
							TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//把树叶添加到目录树
							
							if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
							{
								sprintf(file_name, "%s/%s", path,fn); 	
								//存储文件名到filelist(含路径)										
								res = f_lseek (hFile, hItem*FILE_NAME_LEN);  
								res = f_write (hFile, file_name, FILE_NAME_LEN, &rw_num);						
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
static void Fill_TreeView(char* path,char* record_file,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode)
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
	
	fres = scan_files(p_path,file_name,&hFile,hTree,hNode);			//递归扫描歌曲文件		
	
	fres = f_close (&hFile);					//关闭索引文件		

	free(p_path);										//释放malloc空间		
	p_path = NULL;
	
	free(file_name);								//释放malloc空间		
	file_name = NULL;

	

}

/**
  * @brief  SDView_MainTask处理非递归过程，然后调用递归函数scan_files扫描目录
	*					
  * @param  path:初始扫描路径
  * @retval none
  */
 void SDView_MainTask(void )
{ 

	WM_HWIN hFrame;									//sdview窗口句柄
  WM_HWIN hFrameC;								//子窗口句柄
	
	WM_HWIN hTree;	  							//目录树句柄
  TREEVIEW_ITEM_Handle hNode;			//结点句柄

  //
  // 输出系统信息
  //
//	WM_SelectWindow(WinPara.hWinMain);
//  GUI_DrawGradientV(0, 0, WinPara.xSizeWin,WinPara.ySizeWin, GUI_BLUE, GUI_BLACK);
//  GUI_SetFont(GUI_FONT_20F_ASCII);
//  GUI_DispStringHCenterAt("scanning sd Card...", WinPara.xSizeWin >> 1, WinPara.ySizeWin / 3);
  //GUI_X_Delay(100);

	hFrame = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,GUI_ID_FRAMEWIN0,"SD View",0);

	FRAMEWIN_SetResizeable(hFrame,1);
	/* 创建窗口按钮 */
  FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
  FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
  FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* 把app句柄插入链表 */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame,_cbSDViewWin);

	//	TBD 使用回调函数会出现无法移动框架窗口的情况
	_pcbOldSDViewWin = WM_SetCallback(hFrame, _cbSDViewWin);	//获取旧的回调函数指针
	
	/* 获取框架窗口用户区的句柄 */
	hFrameC = WM_GetClientWindow(hFrame);

  //
  // 创建目录树
  //
  hTree = TREEVIEW_CreateEx(0, 0, WM_GetWindowSizeX(hFrameC),WM_GetWindowSizeY(hFrameC), hFrameC, WM_CF_SHOW, TREEVIEW_CF_AUTOSCROLLBAR_H, GUI_ID_TREEVIEW0);
  TREEVIEW_SetAutoScrollV(hTree, 1);								//使用自动滑动条
  TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);

	WM_SetCallback(hFrameC,_cbSDViewWinC); //设置目录树的父窗口(背景窗口)的回调函数
  
	// 填充目录树,创建第一个目录结点
  //
  hNode = TREEVIEW_InsertItem(hTree, TREEVIEW_ITEM_TYPE_NODE, 0, 0, "sd Card");
	
	/* 扫描sd卡，填充目录树*/
	Fill_TreeView("0:",FILE_LIST_PATH,hTree,hNode);
	

}
	


