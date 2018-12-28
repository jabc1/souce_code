/***************************Copyright BestFu 2014-05-14*************************
文	件：	Flash.h
说	明：	STM32F103_Flash操作头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-11-21
修  改:     暂无
*******************************************************************************/
#ifndef __FLASH_H
#define __FLASH_H

#include "BF_type.h"

/*********************************配置功能*************************************/
#define FLASH_ENCRYPTION_EN				(1u)		//使能flash加密功能
#define DEVICE_ADDR_RESET_FEATURE_EN	(1u)		//使能地址重设功能

/*******************************数据类型定义***********************************/
typedef enum
{
    BUSY     = 0x01,
    PGERR    = 0x04,
    WRITEERR = 0x10,
    EOP      = 0x20
}Flash_SR;

/* Flash Control Register bits */
#define CR_PG                ((u32)0x00000001)
#define CR_PER               ((u32)0x00000002)
#define CR_MER               ((u32)0x00000004)
#define CR_OPTPG             ((u32)0x00000010)
#define CR_OPTER             ((u32)0x00000020)
#define CR_STRT              ((u32)0x00000040)
#define CR_LOCK              ((u32)0x00000080)

#define DEVICE_ID			 (*(vu32*)(0x1ffff7f0))									//设备ID取自IC全球唯一ID   F103
#define CHIPID_ADDR			 (0X1FFFF7E8)    										//STM32F103CX全球96位唯一芯片ID存储首地址

//#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
/*******************************地址重设功能定义**********************************/
/*重设地址功能数据格式为->重设地址标记(2bytes 0xaa55) + 重设地址(4bytes) + 重设地址反码(4bytes) */
#define RE_DEVICE_ID_START_ADDR		(0x8000380)	                            		//重设地址存放STM32 Flash的首地址,注意4bytes对齐
#define RE_DEVICE_ID_FLAG_SIZE		(2)												//重设地址标记占用空间的大小(单位byte)
#define RE_DEVICE_ID_SIZE			(4)												//重设地址占用的空间大小(单位byte)
#define RE_DEVICE_ID_ANTI_SIZE		(4)												//重设地址反码占用的空间大小(单位byte)
#define RE_DEVICE_ID_FLAG_ADDR		(RE_DEVICE_ID_START_ADDR)						//重设地址标记存储的首地址
#define RE_DEVICE_ID_ADDR			(RE_DEVICE_ID_START_ADDR+RE_DEVICE_ID_FLAG_SIZE)//重设地址存储的首地址
#define RE_DEVICE_ID_ANTI_ADDR		(RE_DEVICE_ID_ADDR + RE_DEVICE_ID_SIZE)			//重设地址反码存储的首地址
#define RE_DEVICE_ID_END_ADDR		(RE_DEVICE_ID_ANTI_ADDR+RE_DEVICE_ID_ANTI_SIZE)	//重设地址存放STM32 Flash的末地址

#define RE_DEVICE_ID		        (*(vu32*)(RE_DEVICE_ID_ADDR))					//重设地址存放STM32 Flash的首地址
#define RE_DEVICE_ID_ANTI		    (*(vu32*)(RE_DEVICE_ID_ANTI_ADDR)) 				//重设地址反码存放STM32 Flash的首地址
#define RE_DEVICE_ID_FLAG           (0XAA55)                                		//重设地址标志
//判断地址的有效性
#define DEVICE_ID_IS_OK				(((RE_DEVICE_ID_FLAG == (*(u16 *)(RE_DEVICE_ID_START_ADDR)))&&\
									(RE_DEVICE_ID != 0xffffffff)&&\
									(RE_DEVICE_ID != 0x00000000)&&\
									(RE_DEVICE_ID != 0x44444444)&&\
									(RE_DEVICE_ID != 0x88888888)&&\
									(RE_DEVICE_ID != 0x54534542)&&\
									(RE_DEVICE_ID == ~RE_DEVICE_ID_ANTI)))
//#endif
									
#if (FLASH_ENCRYPTION_EN > 0u)
/*****************************Flash加密相关函数********************************/
#define ENCRYPTION_FLAG			((u32)(0x5ABD42A5))								//用于存储加密标记
#define BYTE_4_ALIGNMENT(addr)	(((addr)&(0x03))?(((addr)&(~(0x03)))+4):(addr))	//用于Flash地址的4byte对齐
#define ENCRYPTION_FLAG_SIZE	(4)												//存储标记占用空间
#define ENCRYPTION_SIZE			(4)												//存储加密信息占用空间
#define ENCRYPTION_FLAG_ADDR	(BYTE_4_ALIGNMENT(RE_DEVICE_ID_END_ADDR))		//用于存储加密标记,用于Flash的4bytes对齐
#define ENCRYPTION_ADDR			(ENCRYPTION_FLAG_ADDR + ENCRYPTION_FLAG_SIZE)	//用于存储加密值的地址
#define ENCRYPTION_END_ADDR		(ENCRYPTION_ADDR + ENCRYPTION_SIZE)				//用于存储加密值的结束地址
#define CRC_BASE_DR				((u32)(0x40023000))								//CRC_DR寄存器的首地址
#define CRC_BASE_CR				((u32)(0x40023008))								//CRC_CR寄存器的首地址
__inline u32 FlashEncryptionFunc(u32 NewAddr)	//计算加密值使用
{
	*((vu32*)(CRC_BASE_CR))|= (u32)(0x01);
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x00));
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x04));
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x08));
	*((vu32*)(CRC_BASE_DR)) = NewAddr;
	return (*((vu32*)(CRC_BASE_DR)));
}
__inline u32 DecryptionFlash(void)				//使用加密值在函数中混淆使用
{
	*((vu32*)(CRC_BASE_CR))|=  (u32)(0x01);
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x00));
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x04));
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(CHIPID_ADDR + 0x08));
	*((vu32*)(CRC_BASE_DR)) = RE_DEVICE_ID;
	*((vu32*)(CRC_BASE_DR)) = *((vu32*)(ENCRYPTION_ADDR));
	return (*((vu32*)(CRC_BASE_DR)));
}
#endif

/*******************************函数声明**************************************/
void Flash_Unlock(void);
void Flash_lock(void);
u8 FLASH_ErasePage(u32 page_add);
u8 Flash_ProgramHalfWord(u32 add, u16 data);
u8 Flash_Write(u32 add, u32 len, u8 *data);
u8 Flash_WriteProtect(u32 addr, u8 sta);
u8 Flash_EraseOptionByte(void);
u8 Flash_WriteOptionByte(u32 addr, u8 data);

#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
void Get_UniqueDeviceID(u8* chipidtable);
#endif

#endif /*FLASH_H */

/**************************Copyright BestFu 2014-05-14*************************/
