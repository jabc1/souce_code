/***************************Copyright BestFu 2014-05-14*************************
文	件：    command.c
说	明:	    从队列中获取一条指令
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-10-14
修  改:     暂无
*******************************************************************************/
#include "command.h"
#include "fifo.h"
#include "crc.h"

const u8 HeadCMD[HEAD_SIZE] = {0xF1, 0xF1, 0xF1, 0xF2, 0xF1, 0xF1};
const u8 TailCMD[TAIL_SIZE] = {0xF2, 0xF2, 0xF2, 0xF1, 0xF2, 0xF2};

/*******************************************************************************
函 数 名：  cmd_get
功能说明：  从队列中获取一条指令
参	  数：  fifo:	获取源
            cmdBuf: 指令存储缓冲区	
返 回 值：  指令长度。为零，没有可读指令
*******************************************************************************/
u32 cmd_get(u8 *cmdBuf, FIFO_t *fifo)
{
	u32 i;
     
	ERRR(fifo == NULL, goto ERR1);
	
	ERRR(fifo_empty(fifo) == TRUE, goto ERR1);        //FIFO为空
	i = fifo_find(fifo, HeadCMD, sizeof(HeadCMD));
	fifo->front = i;
	ERRR(fifo_empty(fifo) == TRUE, goto ERR1);        //FIFO为空

	i = sizeof(Communication_t) + sizeof(HeadCMD);
	ERRR(i > fifo_validSize(fifo), goto ERR1); //数据空间不够

	i += fifo->data[(fifo->front + i - ARGE_SEAT)%fifo->size] + CRC_SIZE + sizeof(TailCMD);
	ERRR(i > CMD_SIZE, goto ERR2);	//数据指令长度错误
	ERRR(i > fifo_validSize(fifo), goto ERR1); //数据空间不够
	
	ERRR(fifo_cmp(fifo, (fifo->front + i - sizeof(TailCMD))%fifo->size
					, TailCMD, sizeof(TailCMD)) == FALSE, goto ERR2);	//核对数据尾
					
	ERRR(fifo_gets(fifo, cmdBuf, i) == FALSE, goto ERR1);	//数据内容出队
	ERRR(ChkCrcValue(&cmdBuf[sizeof(HeadCMD)], (i - sizeof(HeadCMD) - sizeof(TailCMD))), goto ERR1);	//CRC错误
	
	return i;

ERR2:	fifo->front = (fifo->front+sizeof(HeadCMD))%fifo->size; //数据错误将头移出
ERR1:	return 0;	
}

/*******************************************************************************
函 数 名：  cmd_put
功能说明：  放置一条指令到FIFO中
参	  数：  fifo:	目标地址
            cmdBuf: 指令存储缓冲区	
			cmdLen:	放置内容长度
返 回 值：  指令长度。为零，没有可读指令
*******************************************************************************/
u32 cmd_put(FIFO_t *fifo, u8 *cmdBuf, u32 cmdLen)
{
	u16 crc;
	
	ERRR(fifo == NULL, goto ERR1);
	ERRR(fifo_puts(fifo, (u8*)HeadCMD, sizeof(HeadCMD)) == FALSE, goto ERR1);	//放置通信标志头
	crc = ChkCrcValue(cmdBuf, cmdLen);	//获取CRC
	ERRR(fifo_puts(fifo, cmdBuf, cmdLen) == FALSE, goto ERR1);	//放置参数区
	ERRR(fifo_putc(fifo, (u8)(crc>>8)) == FALSE, goto ERR1);	//放置CRC高位	
	ERRR(fifo_putc(fifo, (u8)(crc)) == FALSE, goto ERR1);		//放置CRC底位	
	ERRR(fifo_puts(fifo, (u8*)TailCMD, sizeof(TailCMD)) == FALSE, goto ERR1);	//放置通信标尾
	
	return TRUE;
ERR1:	return FALSE;
}

/**************************Copyright BestFu 2014-05-14*************************/
