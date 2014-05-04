#include <stdio.h>

#include "DIALOG.h"
#include "MESSAGEBOX.h "
#include "TEXT.h"
#include "bsp_led.h"
#include "ff.h"


#include "WFGUI_Snapshot.h"




#define SNAPSHOT_PATH 	"0:WF_OS/Snapshot/test2.bmp"


/**
  * @brief  _WriteByte2File 作为屏幕截图的回调函数
  * @param  none
  * @retval none
  */
static void _WriteByte2File(U8 Data, void * p) {

		UINT rwb;
		
		LED1_TOGGLE;
		/* 写入数据 */
		f_write((FIL *)p,&Data,1,&rwb);	
		LED1_TOGGLE;
}


/**
  * @brief  WFGUI_Snapshot 对屏幕截图
  * @param  none
  * @retval none
  */
void WFGUI_Snapshot(void)
{

	FIL hFile;
	FRESULT res;
	char fpath[40];

	/* 根据当前时间创建截图文件名 */
	sprintf(fpath,"0:WF_OS/Snapshot/%04d%02d%02d_%02d%02d%02d.bmp",
					systmtime.tm_year,systmtime.tm_mon,systmtime.tm_mday,systmtime.tm_hour,systmtime.tm_min,systmtime.tm_sec ); 
	
	/* 打开文件 */
	res = f_open(&hFile,fpath,FA_CREATE_ALWAYS|FA_WRITE);
	if(res == FR_OK)
	{	
		/* 输出文件 */
		GUI_BMP_Serialize(_WriteByte2File, &hFile);
		
		/* 关闭文件 */
		f_close(&hFile);
		
		LED1_OFF;		
		
		/* 调用消息框显示信息 */
		MESSAGEBOX_Create("Snapshot succeed!","Snapshot",0);			

	}
	else
	{
		LED1_OFF;
		MESSAGEBOX_Create("Snapshot failed!","Snapshot",0);
		
		

	}
}



