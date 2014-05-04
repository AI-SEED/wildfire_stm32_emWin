
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

#define FILE_NAME_LEN 	150			//�ļ������ȣ������⵽�ļ�������50 ��������ļ� 
#define PATH_LEN				100
#define MUSIC_NAME_LEN 	24			//LCD��ʾ���ļ�������󳤶�
#define _DF1S	0x81

#define FILE_LIST_PATH "0:WF_OS/filelist.txt"

 //char file_name[FILE_NAME_LEN];

	
//static		FIL	file;								//�ļ����	
//static  	FRESULT fres;							//���ؽ��
//static		unsigned int rw_num;			//�Ѷ�����д���ֽ���
static WM_CALLBACK*     _pcbOldFrame = NULL;
static WM_CALLBACK*     _pcbOldSDViewWin = NULL;

int xSize,ySize;

/**
  * @brief  �Ի����õ���С������Դ��
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
*       �Ի���ص�����
*/
static void _cbDlgWin(WM_MESSAGE * pMsg) {
  int NCode, Id;
  WM_HWIN hDlg, hItem;
  hDlg = pMsg->hWin;
  switch (pMsg->MsgId) {
    case WM_INIT_DIALOG:									//��ʼ���Ի���ʱ
			hItem = WM_GetDialogItem(hDlg, GUI_ID_TEXT0);
			TEXT_SetFont(hItem,GUI_FONT_16_ASCII);	//����text������
      break;
    case WM_NOTIFY_PARENT:
      Id    = WM_GetId(pMsg->hWinSrc);    /* Id of widget */
      NCode = pMsg->Data.v;               /* Notification code */
      switch (NCode) {

      case WM_NOTIFICATION_RELEASED:      /* React only if released */
        if (Id == GUI_ID_YES) {           /* Yes Button */     
					GUI_EndDialog(hDlg, 0);					//�����Ի��򣬷���0
        }
        if (Id == GUI_ID_NO) {        		/* No Button */
          GUI_EndDialog(hDlg, 1);					//�����Ի��򣬷���1
        }
        break;
      }
      break;
    default:
      WM_DefaultProc(pMsg);
  }
}




/**
  * @brief  _cbSDViewWin framewin��ܵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWin(WM_MESSAGE * pMsg)
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
			(*_pcbOldSDViewWin)(pMsg);	//���þɵĻص�������������δ��������
		}	
	
}

/**
  * @brief  _cbSDViewWinC framewin�û����ڵĻص�����
	*					
  * @param  none
  * @retval none
  */
static void _cbSDViewWinC(WM_MESSAGE * pMsg)
{
	TREEVIEW_ITEM_INFO ItemInfo;
	TREEVIEW_ITEM_Handle hNode;
	
  switch (pMsg->MsgId) {
		
		case WM_NOTIFY_PARENT:	 //֪ͨ����
			
			switch(pMsg->Data.v)	//֪ͨ��������
			{
				case WM_NOTIFICATION_RELEASED:	//�ͷ���
					DEBUG("\r\n release");
				
				/* �鿴ѡ�����ĸ���Ŀ */
				hNode = TREEVIEW_GetSel(pMsg->hWinSrc);		
				/* ��ȡ����Ŀ����Ϣ */
				TREEVIEW_ITEM_GetInfo(hNode,&ItemInfo);
				
				if(ItemInfo.IsNode == 0)//Ҷ��
				{
					DEBUG("\r\n leaf num =%ld",hNode);					

					/* �ٷ��ֲ�˵�����ڻص������е��������Ի��򣬵����������Ҳû���󣬾��������ð� */
					/* ���ڻص��������õĻ��Ϳ������������ */
					/* ѡyes ����0 ѡno����1*/
//					if(	GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDlgWin, WinPara.hWinMain, 0, 0) == 0)
//					{
//									//open file
//							DEBUG("\r\n open file");
							OpenFileProcess(hNode);
//						}
						
					//����ѡ��Ϊ��Ҷ�ӵĸ���㣬��ֹ�󴥷�
					//TREEVIEW_SetSel(pMsg->hWinSrc,TREEVIEW_GetItem(pMsg->hWinSrc,hNode,TREEVIEW_GET_PARENT));

				}
				else										//���
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
  * @brief  OpenFileProcess���ļ�	// TBD jpg��ʾ������ 
	*					
  * @param  none
  * @retval none
  */
static void OpenFileProcess(int sel_num)
{
	char* file_name;
	char* read_buffer; 
	FIL hFile;
	FRESULT fres;							//���ؽ��
	unsigned int rw_num;			//�Ѷ�����д���ֽ���

	
	WM_HWIN hFrame;
	WM_HWIN hFrameC;
	WM_HWIN hMultiEdit;
	
	/* ִ�жԻ���ȷ���Ƿ�Ҫ���ļ�������0��ʾȷ�� */
	if(	GUI_ExecDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbDlgWin, WinPara.hWinMain, 0, 0) == 0)
	{
			//���ļ�
		
			file_name 	= (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //Ϊ�洢Ŀ¼����ָ�����ռ�
			
			fres = f_open (&hFile, FILE_LIST_PATH, FA_READ ); 		//�򿪴��������ļ�
			fres = f_lseek (&hFile, sel_num*FILE_NAME_LEN);				//��������ֵ���ҽ�Ҫ���ļ���·��
			fres = f_read(&hFile, file_name, FILE_NAME_LEN, &rw_num);
			fres = f_close (&hFile);
			
			if(strstr(file_name,".txt")||strstr(file_name,".TXT")
					||strstr(file_name,".c")||strstr(file_name,".c")
						||strstr(file_name,".cpp")||strstr(file_name,".CPP")
								||strstr(file_name,".h")||strstr(file_name,".h"))//�ж��Ƿ����ʹ��txt��ʽ��
			{
				//	TBD file_name ��malloc���ģ���Ϊ�βδ��뺯����С�ĳ���
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

			
			free(file_name);								//�ͷ�malloc�ռ�		
			file_name = NULL;
				
		}	

}


/**
  * @brief  scan_files �ݹ�ɨ��sd���ڵĸ����ļ�
  * @param  path:��ʼɨ��·��  hTree Ŀ¼�� hNode Ŀ¼���
  * @retval result:�ļ�ϵͳ�ķ���ֵ
  */
static FRESULT scan_files (char* path,char* file_name,FIL *hFile,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode) 
{ 
		
    FRESULT res; 		//�����ڵݹ���̱��޸ĵı���������ȫ�ֱ���	
    FILINFO fno; 
		unsigned int rw_num;			//�Ѷ�����д���ֽ���

    DIR dir; 
    int i; 
    char *fn; 
	
		TREEVIEW_ITEM_Handle hItem;

	
#if _USE_LFN 
    static char lfn[_MAX_LFN * (_DF1S ? 2 : 1) + 1]; 	//���ļ���֧��
    fno.lfname = lfn; 
    fno.lfsize = sizeof(lfn); 
#endif 

    res = f_opendir(&dir, path); //��Ŀ¼
    if (res == FR_OK) 
			{ 
        i = strlen(path); 
        for (;;) 
				{ 
            res = f_readdir(&dir, &fno); 										//��ȡĿ¼�µ�����
            if (res != FR_OK || fno.fname[0] == 0) break; 	//Ϊ��ʱ��ʾ������Ŀ��ȡ��ϣ�����
#if _USE_LFN 
            fn = *fno.lfname ? fno.lfname : fno.fname; 
#else 
            fn = fno.fname; 
#endif 
            if (*fn == '.') continue; 											//���ʾ��ǰĿ¼������			
            if (fno.fattrib & AM_DIR) 
						{ 																							//Ŀ¼���ݹ��ȡ
								hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_NODE,fn,0);						//Ŀ¼���������
								TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//�ѽ����뵽Ŀ¼����
							
							  sprintf(&path[i], "/%s", fn); 							//�ϳ�����Ŀ¼��
                res = scan_files(path,file_name,hFile,hTree,hItem);					//�ݹ���� 
                if (res != FR_OK) 
									break; 																		//��ʧ�ܣ�����ѭ��
                path[i] = 0; 
            } 
						else 
						{ 
							DEBUG("%s/%s hItem = %d  \r\n", path, fn,(int)hItem);								//����ļ���	
							hItem = TREEVIEW_ITEM_Create(TREEVIEW_ITEM_TYPE_LEAF,fn,0);						//�ļ���������Ҷ
							TREEVIEW_AttachItem(hTree,hItem,hNode,TREEVIEW_INSERT_FIRST_CHILD);		//����Ҷ���ӵ�Ŀ¼��
							
							if (strlen(path)+strlen(fn)<FILE_NAME_LEN)
							{
								sprintf(file_name, "%s/%s", path,fn); 	
								//�洢�ļ�����filelist(��·��)										
								res = f_lseek (hFile, hItem*FILE_NAME_LEN);  
								res = f_write (hFile, file_name, FILE_NAME_LEN, &rw_num);						
							}								
           }//else
        } //for
    } 

    return res; 
} 


/**
  * @brief  Fill_TreeView�����ǵݹ���̣�Ȼ����õݹ麯��scan_filesɨ��Ŀ¼
	*					
  * @param  path:��ʼɨ��·��
  * @retval none
  */
static void Fill_TreeView(char* path,char* record_file,WM_HWIN hTree, TREEVIEW_ITEM_Handle hNode)
{
	char * p_path;									//Ŀ¼�� ָ��
	char * file_name;								//���ڴ洢��Ŀ¼�ļ�����
	FIL		hFile;										//�ļ����	
	FRESULT fres;										//���ؽ��
	
	
	fres = f_unlink(record_file);//ɾ���ɵ�filelist		// TBD �����Խ�Ŀ¼

	p_path = (char * ) malloc(PATH_LEN* sizeof(char));  //Ϊ�洢Ŀ¼����ָ�����ռ�
	file_name = (char * ) malloc(FILE_NAME_LEN* sizeof(char));  //Ϊ�洢Ŀ¼����ָ�����ռ�
	
	fres = f_open (&hFile, record_file, FA_READ|FA_WRITE|FA_CREATE_ALWAYS ); //�򿪴��������ļ�

	strcpy(p_path,path);						//����Ŀ¼����ָ��
	
	fres = scan_files(p_path,file_name,&hFile,hTree,hNode);			//�ݹ�ɨ������ļ�		
	
	fres = f_close (&hFile);					//�ر������ļ�		

	free(p_path);										//�ͷ�malloc�ռ�		
	p_path = NULL;
	
	free(file_name);								//�ͷ�malloc�ռ�		
	file_name = NULL;

	

}

/**
  * @brief  SDView_MainTask�����ǵݹ���̣�Ȼ����õݹ麯��scan_filesɨ��Ŀ¼
	*					
  * @param  path:��ʼɨ��·��
  * @retval none
  */
 void SDView_MainTask(void )
{ 

	WM_HWIN hFrame;									//sdview���ھ��
  WM_HWIN hFrameC;								//�Ӵ��ھ��
	
	WM_HWIN hTree;	  							//Ŀ¼�����
  TREEVIEW_ITEM_Handle hNode;			//�����

  //
  // ���ϵͳ��Ϣ
  //
//	WM_SelectWindow(WinPara.hWinMain);
//  GUI_DrawGradientV(0, 0, WinPara.xSizeWin,WinPara.ySizeWin, GUI_BLUE, GUI_BLACK);
//  GUI_SetFont(GUI_FONT_20F_ASCII);
//  GUI_DispStringHCenterAt("scanning sd Card...", WinPara.xSizeWin >> 1, WinPara.ySizeWin / 3);
  //GUI_X_Delay(100);

	hFrame = FRAMEWIN_CreateEx(0,0,WinPara.xSizeWin,WinPara.ySizeWin,WinPara.hWinMain,WM_CF_SHOW,FRAMEWIN_CF_ACTIVE|FRAMEWIN_CF_MOVEABLE,GUI_ID_FRAMEWIN0,"SD View",0);

	FRAMEWIN_SetResizeable(hFrame,1);
	/* �������ڰ�ť */
  FRAMEWIN_AddCloseButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 0);
  FRAMEWIN_AddMaxButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 1);
  FRAMEWIN_AddMinButton(hFrame, FRAMEWIN_BUTTON_RIGHT, 2);
	
	/* ��app����������� */
	App_Insert(hFrame);
	WM_SendMessageNoPara(WinPara.hWinCtrl,MY_MESSAGE_CTRLCHANGE);
	
	//WM_SetCallback(hFrame,_cbSDViewWin);

	//	TBD ʹ�ûص�����������޷��ƶ���ܴ��ڵ����
	_pcbOldSDViewWin = WM_SetCallback(hFrame, _cbSDViewWin);	//��ȡ�ɵĻص�����ָ��
	
	/* ��ȡ��ܴ����û����ľ�� */
	hFrameC = WM_GetClientWindow(hFrame);

  //
  // ����Ŀ¼��
  //
  hTree = TREEVIEW_CreateEx(0, 0, WM_GetWindowSizeX(hFrameC),WM_GetWindowSizeY(hFrameC), hFrameC, WM_CF_SHOW, TREEVIEW_CF_AUTOSCROLLBAR_H, GUI_ID_TREEVIEW0);
  TREEVIEW_SetAutoScrollV(hTree, 1);								//ʹ���Զ�������
  TREEVIEW_SetSelMode(hTree, TREEVIEW_SELMODE_ROW);

	WM_SetCallback(hFrameC,_cbSDViewWinC); //����Ŀ¼���ĸ�����(��������)�Ļص�����
  
	// ���Ŀ¼��,������һ��Ŀ¼���
  //
  hNode = TREEVIEW_InsertItem(hTree, TREEVIEW_ITEM_TYPE_NODE, 0, 0, "sd Card");
	
	/* ɨ��sd�������Ŀ¼��*/
	Fill_TreeView("0:",FILE_LIST_PATH,hTree,hNode);
	

}
	

