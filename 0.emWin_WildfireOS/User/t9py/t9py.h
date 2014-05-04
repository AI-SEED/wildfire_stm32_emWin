#ifndef  __t9py_h
#define  __t9py_h

#define  UTF8_CODE  1

#define   T9BD        0x00	  //标点输入
#define   T9DX        0x01	  //大写英文
#define   T9XX        0x02	  //小写英文
#define   T9PY        0x03	  //T9拼音输入
#define   T9SZ        0x04	  //T9数字输入

#define   T9NULL      0x00    //查询结果为空



#define   SRF_X       0x00    // 显示输入法的X坐标
#define   SRF_Y       0X00    // 显示输入法的Y坐标
#define   STR_X       0X00    // 汉字输入首地址X
#define   STR_Y       16	  // 汉字输入首地址Y
#define   T9PY_X      0		  // 拼音显示首地址X
#define   T9PY_Y      223	  // 拼音显示首地址Y
#define   INDEX_X     70	  // 索引显示首地址X
#define   INDEX_Y     223	  // 索引显示首地址Y


#if 0
extern  uint16   straddrx;		//字符显示位置
extern  uint16	 straddry;
extern  uint16   t9pyaddrx;	    //拼音显示位置
extern  uint16   t9pyaddry;
extern  uint16   indexaddrx;	//索引显示位置
extern  uint16   indexaddry;

extern  uint8    hzbuff[512];   //输入256个汉字
#endif

extern  struct t9py_struct t9pyfun;
extern const struct t9PY_index  t9PY_sz[];

extern void clrindex(void);

extern  void py_index_sub(void);            //输入法索引程序

extern  void dispsf(void);                  //显示输入的内容

extern  void T9pyTask(unsigned int key);



#endif    //end of __t9py_h

