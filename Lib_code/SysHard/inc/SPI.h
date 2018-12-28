/***************************Copyright BestFu 2014-05-14*************************
文	件：    SPI.h
说	明：    SPI外设寄存器配置
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-08-24
修　改：	暂无
*******************************************************************************/
#ifndef __SPI_H
#define __SPI_H
#include "sys.h"

				    
// SPI总线速度设置 
#define SPI_SPEED_2   		0
#define SPI_SPEED_4   		1
#define SPI_SPEED_8   		2
#define SPI_SPEED_16  		3
#define SPI_SPEED_32 		4
#define SPI_SPEED_64 		5
#define SPI_SPEED_128 		6
#define SPI_SPEED_256 		7
						  	    													  
void SPI2_Init(void);			 //初始化SPI2口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI2速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI2总线读写一个字节
		 
#endif
/**************************Copyright BestFu 2014-05-14*************************/
