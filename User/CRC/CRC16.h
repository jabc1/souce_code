/*
**--------------�ļ���Ϣ------------------------------------------------------------------
**��   ��   ��: CRC.C
**��   ��   ��: 
**����޸�����: 2012��11��15��
**��        ��: �м��
**
*/
/*
**--------------������������---------------------------------------------------------------

*/

#ifndef	__CRC16_H
#define __CRC16_H

#define	CreateTable			0							//��������ʽ������ 1 ���� 0������	ͨ�����ڴ�ӡ����
#if	CreateTable == 1
#define		Polynomial			 0x8005 		//����ʽ(x^16 + x^15 + x^2 + 1)
#include "..\device\Uart\Uart.h"						//ʹ�õĴ���ͷ�ļ�
#define		SendTable(X)		 Uart_SendByte[0](X)
#define		SendTableStr(X,Y)		 Uart_SendStr[0](X,Y)		
#endif

//0001 0000 0010 0001
/**********************************************************************
** ��������: CreateTableFuntion				
** ��������: ����������(ͨ��ָ�����ڷ��͸��ն�)				
** �䡡  ��: ��				 	
** ��    ��: ��		
** ����˵��: ����������(ͨ��ָ�����ڷ��͸��ն�)	
***********************************************************************/
#if	CreateTable == 1
extern void CreateTableFuntion(void);
#endif



/**********************************************************************
** ��������: GetCRC_IBM_SDLC				
** ��������: ������������CRCֵ			
** �䡡  ��: unsigned char *p				������	
			 unsgined short len				����������			 	
** ��    ��: ��		
** ����˵��: �ú������س�ʼ��CRC�Ĵ���
***********************************************************************/
#if	CreateTable == 0
extern unsigned short GetCRC_IBM_SDLC(unsigned char *p,unsigned short len);
#endif



/**********************************************************************
** ��������: GetCRC_IBM_SDLC_oneByte				
** ��������: ������������CRCֵ			
** �䡡  ��: puchMsg			����Ҫ���������
			 CRC				�ϴε�CRCֵ			 	
** ��    ��: ��		
** ����˵��: 
***********************************************************************/
#if	CreateTable == 0
extern unsigned short GetCRC_IBM_SDLC_oneByte(unsigned char puchMsg,unsigned short CRC);
#endif

#endif	//__CRC_H
