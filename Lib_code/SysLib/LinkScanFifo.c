/***************************Copyright BestFu 2014-05-14*************************
文	件：	LinkScanFifo.c
说	明：	属性变化，引发联动入口
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.22 
修　改：	Unarty 2014.11.18 修改设备禁能状态下特殊指令（200～220）不做禁能
*******************************************************************************/

#include "linkall.h"
#if 1
FIFO_t  propfifo;             // 属性变化实列
unsigned char propbuf[PROPBUFSIZE]; // 属性变化缓冲区

/*******************************************************************************
函 数 名：	PropFifoInit
功能说明： 	队列初始化
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void PropFifoInit(void)
{
	fifo_Init(&propfifo, propbuf, PROPBUFSIZE);
}
/*******************************************************************************
函 数 名：	PropChangeScanAndExe
功能说明： 	属性变化扫描并执行
参	  数： 	void
返 回 值：	void
*******************************************************************************/
unsigned char PropChangeScanAndExe(void)
{
	PropFifoItem proptmp;

	if(fifo_gets(&propfifo, (unsigned char *)&proptmp, sizeof(PropFifoItem)) != 0)
	{
		LinkSrcExe(proptmp.UnitID, proptmp.PropID, proptmp.Type, proptmp.Value);
		return 1;
	}
	PropFifoInit();
	
	return 0;	
}
/*******************************************************************************
函 数 名：	PropEventFifo
功能说明： 	属性变化事件压入队列
参	  数： 	unitID--单元号
            propID--属性号
            type--类型号
            value--值
返 回 值：	void
*******************************************************************************/
void PropEventFifo(unsigned char unitID, unsigned char propID, 
					unsigned char type, int value)
{
	if ((gUnitData[unitID].able)	//单元使能
		|| ((199 < propID) && (propID < 221))	//单元特殊属性不做禁能 Unarty 2014.11.18 Add
		)
	{
		PropFifoItem proptmp;
		proptmp.UnitID = unitID;
		proptmp.PropID = propID;
		proptmp.Type   = type;
		proptmp.Value  = value;
		fifo_puts(&propfifo, (unsigned char *)&proptmp, sizeof(PropFifoItem));

		Thread_Login( RESULT, 0, 200, &PropChangeScanAndExe );
	}
	else
	{
		Upload();
	}
}

#endif
/**************************Copyright BestFu 2014-05-14*************************/
