/*
**--------------文件信息------------------------------------------------------------------
**文   件   名: CRC.C
**创   建   人: 
**最后修改日期: 2012年11月15日
**描        述: 中间层
**
*/
/*
**--------------程序运行描述---------------------------------------------------------------

*/

#ifndef	__CRC16_H
#define __CRC16_H

#define	CreateTable			0							//创建多项式余数表 1 创建 0不创建	通过串口打印出来
#if	CreateTable == 1
#define		Polynomial			 0x8005 		//多项式(x^16 + x^15 + x^2 + 1)
#include "..\device\Uart\Uart.h"						//使用的串口头文件
#define		SendTable(X)		 Uart_SendByte[0](X)
#define		SendTableStr(X,Y)		 Uart_SendStr[0](X,Y)		
#endif

//0001 0000 0010 0001
/**********************************************************************
** 函数名称: CreateTableFuntion				
** 功能描述: 创建余数表(通过指定串口发送给终端)				
** 输　  入: 无				 	
** 输    出: 无		
** 函数说明: 创建余数表(通过指定串口发送给终端)	
***********************************************************************/
#if	CreateTable == 1
extern void CreateTableFuntion(void);
#endif



/**********************************************************************
** 函数名称: GetCRC_IBM_SDLC				
** 功能描述: 计算数据流的CRC值			
** 输　  入: unsigned char *p				数据流	
			 unsgined short len				数据流长度			 	
** 输    出: 无		
** 函数说明: 该函数不必初始化CRC寄存器
***********************************************************************/
#if	CreateTable == 0
extern unsigned short GetCRC_IBM_SDLC(unsigned char *p,unsigned short len);
#endif



/**********************************************************************
** 函数名称: GetCRC_IBM_SDLC_oneByte				
** 功能描述: 计算数据流的CRC值			
** 输　  入: puchMsg			本次要计算的数据
			 CRC				上次的CRC值			 	
** 输    出: 无		
** 函数说明: 
***********************************************************************/
#if	CreateTable == 0
extern unsigned short GetCRC_IBM_SDLC_oneByte(unsigned char puchMsg,unsigned short CRC);
#endif

#endif	//__CRC_H
