#ifndef __WFGUI_ImageReader_H
#define __WFGUI_ImageReader_H


#define DRAW_SCALE 		1		//��������ͼ���ܣ����ٶȱȽ�����  1��ʹ�� 0����ֹ



void BMPReader(char * file_name);
void JPEGReader(char * file_name);
void PNGReader(char * file_name);
void GIFReader(char * file_name);

void WFGUI_ImageReader(void);




#endif

