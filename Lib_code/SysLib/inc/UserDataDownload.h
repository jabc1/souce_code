/***************************Copyright BestFu 2014-05-14*************************
文	件：	UserDataDownload.h
说	明：	用户数据下载实现原码头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.08.28 
修　改：	暂无
*******************************************************************************/
#ifndef __USERDATADOWNLOAD_H
#define __USERDATADOWNLOAD_H

#include "UnitCfg.h"

#define USER_FLASH					(0x5A)			//用户Flash区

#ifdef 	STM32L151
	#define USER_FLASH_AREA			(0X1FF8004C)
#else
	#define USER_FLASH_AREA			(0x1FFFF7E0)	//F103
#endif



#define USER_EEPROM					(0xA5)			//用户EEPRM区
#define USER_EEPROM_START_ADDR		(0x3000)		//用户EEPROM地址启始地址
#define USER_EEPROM_END_ADDR		(0x8000)		//用户EEPROM地址结束地址


typedef u8 (*Save_fun)(u32 addr, u32 len, u8 *data);

typedef struct
{
	u16 frameSize;		//每帧数据大小
	u16 frameCount;		//总帧数
	u32 addr;			//数据存储地址
	Save_fun save;		//存储函数
	u32 frameFlag[32];	//帧接收标记
}UserDataDownload_t;	//用户数据下载描述结构体

void UserDataDownload_Init(void);
MsgResult_t UserDataDownload_Ready(UnitPara_t *pData);
MsgResult_t UserDataFrame_Save(UnitPara_t *pData);
MsgResult_t UserDataFrame_Check(UnitPara_t *pData, u8 *rlen, u8 *rpara);

#endif //UserDataDownload.h end
/**************************Copyright BestFu 2014-05-14*************************/
