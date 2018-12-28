/***************************Copyright BestFu 2014-05-14*************************
文	件：    SI4432.h
说	明：    SI4432驱动头文件
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-09-30
修　改：	2014.07.23	Seven  amend	兼容旧库
*******************************************************************************/
#ifndef __SI4432_H
#define __SI4432_H

#include "BF_type.h"


//注意sts的值只能为1或0
#define SDN(sts)    STATUS##sts(SDN_PORT, SDN_PIN)
#define nSEL(sts)   STATUS##sts(nSEL_PORT, nSEL_PIN)

#define nIRQ        GPIOx_Get(nIRQ_PORT, nIRQ_PIN)
#define nGPIO2      GPIOx_Get(nGPIO2_PORT, nGPIO2_PIN)

#define R_REG       0x00    //读寄存器
#define W_REG       0x80    //写寄存器

#define MAX_PAGE	0xff	//最大发送243个字节，理论是255但受程序内部某些头尾判断u8变量的影响，所以最大为243

typedef enum
{
	SI4432_TXS 		= (1<<0),		//发送状态标识
	SI4432_RXS 		= (1<<1),		//接收状态标识
	SI4432_TXF  	= (1<<3),		//数据发送错误
	SI4432_RXF  	= (1<<4),		//数据接收错误
	SI4432_CRCF  	= (1<<5),		//数据CRC错误
	SI4432_BUSYF 	= (1<<6),		//信道忙错误
	SI4432_RSSIF	= (1<<7)		//模块通信错误
}SI4432State_t;


void Si4432_Init(void);
void Si4432_Idle(void);
u8 Si4432_Send(u8 *cptBuf, u8 cLength, u8 level);
extern u32 Get_Sign433_MasterAddr(void);//获取测试主机地址
extern u8  Get_Sign433_TestPackNum(void);//获取测试报数量
#endif //si4432.h
/**************************Copyright BestFu 2014-05-14*************************/

