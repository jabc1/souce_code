/***************************Copyright BestFu 2014-05-14*************************
文	件：	Upload.h
说	明：	设备主动上报相关函数头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.07.03 
修　改：	暂无
*******************************************************************************/
#ifndef __UPLOAD_H
#define __UPLOAD_H

typedef enum
{
	RESTART			= 0,	//上电/重启
	USER_DATA_RESET	= 1,	//用户数据重置/恢复出厂设置
	EEPROM_FAIL 	= 2,	//用户数据存储区域异常	
	WIFI_433		= 3, 	//433无线通信失败
}Upload_t;

void Upload_Fault(Upload_t upload);

#endif //Upload.h end
/**************************Copyright BestFu 2014-05-14*************************/
