#ifndef __WFGUI_ImageReader_H
#define __WFGUI_ImageReader_H


#define DRAW_SCALE 		1		//按比例画图功能，（速度比较慢）  1：使能 0：禁止



void BMPReader(char * file_name);
void JPEGReader(char * file_name);
void PNGReader(char * file_name);
void GIFReader(char * file_name);

void WFGUI_ImageReader(void);




#endif

