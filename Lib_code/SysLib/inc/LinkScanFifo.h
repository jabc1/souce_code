/***************************Copyright BestFu 2014-05-14*************************
文	件：	LinkScanFifo.h
说	明：	属性变化，引发联动入口
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	暂无
*******************************************************************************/
#ifndef    _linkscanfifo_h_
#define    _linkscanfifo_h_

#define PROPBUFSIZE       128
#pragma pack(1) //按1字节对齐
typedef struct
{
	unsigned char UnitID;
	unsigned char PropID;
	unsigned char Type;
	int           Value;
}PropFifoItem;
#pragma pack()  //取消按1字节对齐

void PropFifoInit(void);
unsigned char PropChangeScanAndExe(void);

#endif
/**************************Copyright BestFu 2014-05-14*************************/
