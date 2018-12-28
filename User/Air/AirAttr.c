/***************************Copyright BestFu 2014-05-14*************************
文	件：	Air_Attr.c
说	明：	空调属性接口原代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v2.0
编	写：	Unarty
日	期：	2014.09.17 
修　改：	暂无
*******************************************************************************/
#include "AirAttr.h"
#include "AirUnit.h"
#include "DeviceRelate.h"
#include "AirCondition.h"
#include "KeyUnit.h"
#include "Backlight.h"
#include "Thread.h"
#include "LCD_Show.h"
#include "usart.h"
#include "Polling.h"
#include <stdlib.h>

extern Air_Para_t gAirConfigPara;

const AttrExe_st AirAttrTab[] =
{
	{0x02, LEVEL_1, NULL			        	, Get_AirBrand_Attr        	},
	{0x03, LEVEL_1, Set_AirStatus_Attr      	, Get_AirStatus_Attr     	},
	{0x04, LEVEL_1, Set_AirStatus_Attr      	, Get_AirStatus_Attr       	},
	{0x05, LEVEL_1, Set_AirStatus_Attr      	, Get_AirStatus_Attr		},
	{0x06, LEVEL_1, Set_AirStatus_Attr      	, Get_AirStatus_Attr   		},
	{0x07, LEVEL_1, Set_AirStatus_Attr     		, Get_AirStatus_Attr		},
	{0x09, LEVEL_0, Set_AirStatus_Attr     		, NULL       				},
	{0x0a, LEVEL_0, Set_AirStatus_Attr     		, NULL       				},
	{0x0b, LEVEL_0, Set_AirStatus_Attr			, NULL       				},
	{0x0c, LEVEL_0, Set_AirStatus_Attr			, NULL       				},
	{0x0d, LEVEL_0, Set_AirStatus_Attr     		, NULL       				},
	{0x0e, LEVEL_0, Set_AirStatus_Attr     		, NULL       				},
	{0x0F, LEVEL_0, NULL			     		, Get_AirStatus_Attr		},
	{0x10, LEVEL_0, Set_AirTimer_Attr     		, Get_AirStatus_Attr		},
	
	{0x11, LEVEL_0, Set_BindObject_Attr     	, Get_BindObject_Attr		}, //Add zyh 20150429
	{0x13, LEVEL_0, Set_MaxMinTemperature_Attr  , Get_MaxMinTemperature_Attr}, //Add zyh 20150429

	{0x14, LEVEL_0, Set_Correction_Temperature_Attr     	, Get_Correction_Temperature_Attr		}, //Add 20150907
	{0x15, LEVEL_0, Set_Air_Type_Attr  , Get_Air_Type_Attr}, //Add 20150907
	{0x16, LEVEL_0, Set_Air_Addr_Attr  , Get_Air_Addr_Attr}, //Add 20150907
	{0x17, LEVEL_0, Set_Air_List_Addr_Attr  , Get_Air_List_Addr_Attr}, //Add 20150907
	
	{0x50, LEVEL_1, Set_AirStatus_Attr     	    , Get_AirStatus_Attr      	},
	{0x80, LEVEL_0, Set_DeviceRelate_Attr       , Get_DeviceRelate_Attr      },   //读写设备关联属性
  {0x81, LEVEL_0, Set_DeviceRelateSwitch_Attr , Get_DeviceRelateSwitch_Attr},   //使能/禁能 数据发送
	{0x82, LEVEL_0, Set_RS232Cfg_Attr 			, Get_RS232Cfg_Attr},   //Add zyh 20150429
};
//volatile static u8 show_flag = 0;
static void Air_StatusShow(void);
static void Air_DataCheck(void);

/*****************************************************************************
函 数 名:  	Air_ParaInit
功能说明:  	空调相关上电参数初始化
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_ParaInit(u8 unitID)
{
	UART_Init(36);   //无绑定时串口驱动
	Air_UnitInit();
	AirCondition_Init();
	DeviceRelate_Init();
	BindObject_Init();
	KeyUnit_Init();
	Backlight_Init();
	LCD_Init();
	//增加3秒显示全部图标
	Air_DataCheck();
//	Thread_Login(FOREVER, 0, 2500, Air_StatusShow);	//后台更新状态显示
//	Thread_Login(FOREVER, 0, 10, Air_StatusShow);	//后台更新状态显示
}
/*****************************************************************************
函 数 名:  	Air_StatusShow_thread
功能说明:  	线程Air_StatusShow_thread
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_StatusShow_thread(void)
{
	Thread_Login(FOREVER, 0, 10, Air_StatusShow);	//后台更新状态显示
}
/*****************************************************************************
函 数 名:  AirEepromInit
功能说明:  空调EEPROM参数的默认值
参    数:  unitID-单元号
返 回 值:  无
*****************************************************************************/
void AirEepromInit(u8 unitID)
{
	DeviceRelate_EEPROMInit();
}

/*****************************************************************************
函 数 名:  	Air_StatusShow
功能说明:  	空调状态显示
参    数:  	无
返 回 值:  	无
*****************************************************************************/
static void Air_StatusShow(void)
{
	static AirStatus_t airStatus = {0x02};
	static u16 time = 0;
	
	if (0 == gAirStatus.power)	//空调关闭
	{
		if (airStatus.power != gAirStatus.power)	//空调电源状态有新的变化
		{
			airStatus.power = gAirStatus.power;
			LCD_ShowPowerOff();
			time = 500;
//			time = 1;
		}
	}
	else
	{
		if (1 == memcmp(&airStatus, &gAirStatus, sizeof(airStatus)))// | (show_flag & 0x02))	//如果空调设置有变化
		{
			time = 0;
			memcpy(&airStatus, &gAirStatus, sizeof(airStatus));
			
			LCD_ShowWindSpeed(gAirStatus.speed);
			LCD_ShowMode(gAirStatus.mode);
			LCD_ShowTemper(airStatus.temp, 0);	//显示空调设置温度
//			show_flag &= 0xFD;
		}
		LCD_ShowTime(gAirCondition.timer);	//更新剩余时间
	}
	
	if (++time > 500)	
//	if (++time > 1)	
	{	
		time = 0;	//每0.5s更新一次当前检测温度
		LCD_ShowTemper(gAirCondition.temp, 1);	//显示当前室内温度
//		show_flag |= 0x01;
	}
}
/*****************************************************************************
函 数 名:  	Air_DataCheck
功能说明:  	空调数据核对
参    数:  	无
返 回 值:  	无
注    意：	对空调输入的参数进行核对，会改变实际有效值
*****************************************************************************/
void Air_DataCheck(void)
{
	gAirStatus.power %= 2;	//空调电源只有两种状态
	gAirStatus.mode %= 5;	//空调模式只有5种
	gAirStatus.speed %= 4;	//风速只有四种
	gAirStatus.wind %= 2;	//风向只有四种
	
	if (gAirStatus.temp > gAirStatus.SetMaxTemp)	//温度范围在16～32之间
	{
		gAirStatus.temp  = gAirStatus.SetMaxTemp ;
	}
	else if (gAirStatus.temp < gAirStatus.SetMinTemp)
	{
		gAirStatus.temp = gAirStatus.SetMinTemp ;
	}
}
/*******************************************************************************
函 数 名:  	Get_AirBrand_Attr
功能说明:  	获取空调品牌
参    数:  	pData: 	单元号/属性号/参数长度/参数值
			*rlen-返回参数长度
			*rpara-返回参数存放地址
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_AirBrand_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rlen = Air_BrandGet(rpara);
/*
	*rlen = 1;
	rpara[0] = 1;
*/
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Set_AirTimer_Attr
功能说明:  	设置空调定时属性
参    数:  	pData->data: 	单元号/属性号/参数长度/参数值
返 回 值:  	操作结果
*******************************************************************************/
MsgResult_t Set_AirTimer_Attr(UnitPara_t *pData)
{
	if (pData->len != 1)	
	{
		return PARA_LEN_ERR;
	}
	if (pData->data[0] > 9)
	{
		return PARA_MEANING_ERR;
	}
	
	AirCondition_TimerSet(pData->data[0]);
	
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Set_AirStatus_Attr
功能说明:  	设置空调状态属性
参    数:  	pData->data: 	单元号/属性号/参数长度/参数值
返 回 值:  	操作结果
*******************************************************************************/
MsgResult_t Set_AirStatus_Attr(UnitPara_t *pData)
{
//	u32 temp;
	if(0x03 == pData->cmd)//电源开关	0：关，1：开
	{
		if (0 == pData->len)	//如果不带参数
		{
			gAirStatus.power ^= 0x01;
		}
		else
		{
			gAirStatus.power = pData->data[0];
		}
	}
//	else if(0x01 == gAirStatus.power)
	else
	{
		if(0x00 == gAirStatus.power)
		{
			gAirStatus.power |= 0x01;
		}
		switch (pData->cmd)
		{
			case 0x04:			//工作模式	0:自动1:冷气2:除湿3:送风4:暖气
				gAirStatus.mode = pData->data[0];
				break;
			case 0x05:			//是否扫风	0:关闭扫风1:打开扫风
				gAirStatus.wind = pData->data[0];
				break;
			case 0x06:			//风速		0:自动风速1:小风2:中风3:大风
				gAirStatus.speed = pData->data[0];
				break;
			case 0x07:			//温度
				gAirStatus.temp = pData->data[0];
				break;
			case 0x09:			//温度自加
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
					gAirStatus.temp = (gAirStatus.temp<gAirStatus.SetMaxTemp )\
									? ++gAirStatus.temp : gAirStatus.temp;
//				}
				break;
			case 0x0A:			//温度自减
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
					gAirStatus.temp = (gAirStatus.temp>gAirStatus.SetMinTemp ) \
									? --gAirStatus.temp : gAirStatus.temp;
//				}
				break;
			case 0x0B:			//模式自加
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
					gAirStatus.mode++;
					gAirStatus.mode %= 5;
//				}
				break;
			case 0x0C:			//模式自减
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
			
					gAirStatus.mode = (gAirStatus.mode)?(--gAirStatus.mode):(gAirStatus.mode = 4);
//						gAirStatus.mode--;
//				}
				break;
			case 0x0D:			//风速自加
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
					gAirStatus.speed++;
					gAirStatus.speed %= 4;
//				}
				break;
			case 0x0E:		//风速自减
//				if(show_flag & 0x01)
//				{
//					show_flag &= 0xFE;
//					show_flag |= 0x02;
//				}
//				else
//				{
					gAirStatus.speed = (gAirStatus.speed) ? (--gAirStatus.speed) : (gAirStatus.speed = 3);
//					gAirStatus.speed--;
//				}
				break;
			case 0x50:			//一起设置
				gAirStatus.power = pData->data[0];
				gAirStatus.temp  = pData->data[1];
				gAirStatus.mode  = pData->data[2];
				gAirStatus.speed = pData->data[3];
				gAirStatus.wind	 = pData->data[4];
				break;
			default: return CMD_EXE_ERR; 
		}
	}
//	else
//	{
//		return COMPLETE;
//	}
	if(gBindObject.ObjectType > 0X00 || TRUE == CheckBindAddr(gBindObject.Addr) ) //绑定对象不为万能红外时验证绑定地址
	{
		Air_DataCheck();
		Air_CmdDelaySend(1, rand()%500);
		return COMPLETE;
	}	
	else
	{
		return ID_ERR;
	}
	
	
	
}

/*******************************************************************************
函 数 名:  	Get_AirStatus_Attr
功能说明:  	获取空调状态属性
参    数:  	pData: 	单元号/属性号/参数长度/参数值
			*rlen-返回参数长度
			*rpara-返回参数存放地址
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_AirStatus_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rlen = 0;
	switch (pData->cmd)
	{
		case 0x03:			//电源开关
			rpara[(*rlen)++] = gAirStatus.power;
			break;
		case 0x04:			//工作模式
			rpara[(*rlen)++] = gAirStatus.mode;
			break;
		case 0x05:			//是否扫风
			rpara[(*rlen)++] = gAirStatus.wind;
			break;
		case 0x06:			//风速
			rpara[(*rlen)++] = gAirStatus.speed;
			break;
		case 0x07:			//温度
			rpara[(*rlen)++] = gAirStatus.temp;
			break;
		case 0x0F:			//实时温度
			rpara[(*rlen)++] = (u8)gAirCondition.temp;
			rpara[(*rlen)++] = (u8)(gAirCondition.temp >> 8);
			break;
		case 0x10:			//运行定时时间
			rpara[(*rlen)++] = gAirCondition.timer;
			break;
		case 0x50:			//一起获取
			rpara[(*rlen)++] = gAirStatus.power;
			rpara[(*rlen)++] = gAirStatus.temp;
			rpara[(*rlen)++] = gAirStatus.mode;
			rpara[(*rlen)++] = gAirStatus.speed;
			rpara[(*rlen)++] = gAirStatus.wind;
			break;
		default: return CMD_EXE_ERR; 
	}
	
	return COMPLETE;
}



/*******************************************************************************
函 数 名:  	Set_MaxMinTemperature_Attr
功能说明:  	设置温度可控范围
参    数:  	pData->data: 	单元号/属性号/参数长度/参数值
返 回 值:  	操作结果
编    写：	zyh
时    间：  20150429
*******************************************************************************/
MsgResult_t Set_MaxMinTemperature_Attr(UnitPara_t *pData)
{
	if((pData->data[0] > MAX_TEMP) || (pData->data[1] < MIN_TEMP ) || (pData->data[0] < pData->data[1]))
	{
		return PARA_MEANING_ERR;
	}
	gAirStatus.SetMaxTemp  = pData->data[0];
	gAirStatus.SetMinTemp  = pData->data[1];
	EEPROM_Write(AIR_STATUS_ADDR, sizeof(AirStatus_t), (u8*)&gAirStatus);	//存储最新状态
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_MaxMinTemperature_Attr
功能说明:  	获取设置温度可控范围
参    数:  	pData: 	单元号/属性号/参数长度/参数值
			*rlen-返回参数长度
			*rpara-返回参数存放地址
返 回 值:  	消息执行结果
编    写：	zyh
时    间：  20150429
*******************************************************************************/
MsgResult_t Get_MaxMinTemperature_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rlen = 2;
	rpara[0] = gAirStatus.SetMaxTemp ;
	rpara[1] = gAirStatus.SetMinTemp ;
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_Correction_Temperature_Attr
功能说明：  设置校正环境温度属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Set_Correction_Temperature_Attr(UnitPara_t *pData)
{
	gAirCondition.Correction_temperature = pData->data[0];  //绑定对象
	EEPROM_Write(CORRECTION_TEMPERATURE_ADDR, sizeof(gAirCondition.Correction_temperature), (u8*)&gAirCondition.Correction_temperature);//保存校正温度
  return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_Correction_Temperature_Attr
功能说明：  获取校正环境温度属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Get_Correction_Temperature_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
	*rlen = 1;
	*rpara++ = gAirCondition.Correction_temperature;
	return COMPLETE;
}


/*******************************************************************************
函 数 名：  Set_Air_Type_Attr
功能说明：  设置温控面板参数属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Set_Air_Type_Attr(UnitPara_t *pData)
{
	gAirConfigPara.Air_type = pData->data[0];
	EEPROM_Write(RS485_PARA_ADDR, sizeof(Air_Para_t), (u8*)&gAirConfigPara);//保存参数
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_Air_Type_Attr
功能说明：  获取温控面板参数属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Get_Air_Type_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
	*rlen = 1;
	*rpara++ = gAirConfigPara.Air_type;
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Set_Air_Addr_Attr
功能说明：  设置温控面板地址属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Set_Air_Addr_Attr(UnitPara_t *pData)
{
	gAirConfigPara.Local_Addr = pData->data[0];
	gAirConfigPara.Air_Indoor_Addr = pData->data[1];
	gAirConfigPara.Air_Indoor_Addr_Len = pData->data[2];
	EEPROM_Write(RS485_PARA_ADDR, sizeof(Air_Para_t), (u8*)&gAirConfigPara);//保存参数
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_Air_Addr_Attr
功能说明：  获取温控面板地址属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Get_Air_Addr_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
	*rlen = 3;
	*rpara++ = gAirConfigPara.Local_Addr;
	*rpara++ = gAirConfigPara.Air_Indoor_Addr;
	*rpara++ = gAirConfigPara.Air_Indoor_Addr_Len;
	return COMPLETE;
}



/*******************************************************************************
函 数 名：  Set_Air_List_Addr_Attr
功能说明：  设置温控面板地址列表属性
参    数：  *pdata     -- 输入参数   
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Set_Air_List_Addr_Attr(UnitPara_t *pData)
{
	u8 i,j,k = 0,m,temp[3] = {0};
	u16 data;
	for(i = 0; i< POLL_ADDR_LEN; i++)
	{
		data = 0;
		for(j = 0; ((j < 3) && ('#' != pData->data[k]));)
		{
			temp[j] = pData->data[k];
			j++;
			k++;
		}
		k++;
		for(m = 0;m < j;)
		{
			data = data * 10 + (temp[m] - '0');
			m++;
		}
		if(data <= 0xFF)
		{
			gAirConfigPara.Poll_Addr[i] = (u8)data;
		}
		else
		{
			gAirConfigPara.Poll_Addr[i] = 0;
		}
	}
	EEPROM_Write(RS485_PARA_ADDR, sizeof(Air_Para_t), (u8*)&gAirConfigPara);//保存参数
	return COMPLETE;
}

/*******************************************************************************
函 数 名：  Get_Air_List_Addr_Attr
功能说明：  获取温控面板地址列表属性
参    数：  *pdata     
            *rlen       -- 返回长度
            *rpara      -- 返回数据
返 回 值：  TURE / FLASE
编    写：  zyh
时	  间：  20150907
*******************************************************************************/
MsgResult_t Get_Air_List_Addr_Attr(UnitPara_t *pdata, u8 *rlen, u8 *rpara)  
{
	u8 i,j = 0,k,temp[2];
	u8 data[POLL_ADDR_LEN * 4] = {0};
	for(i = 0; i< POLL_ADDR_LEN;)
	{
		temp[0] = gAirConfigPara.Poll_Addr[i] /100;
		if(temp[0])
		{
			data[j] = temp[0] + '0';
			j++;
		}
		temp[1]  = (gAirConfigPara.Poll_Addr[i] - temp[0] * 100) / 10;
		if(temp[1])
		{
			data[j] = temp[1] + '0';
			j++;
		}
		data[j++] = (gAirConfigPara.Poll_Addr[i] % 10) + '0';
		data[j++] = '#';
		i++;
	}
	*rlen = j;
	for(k = 0; k <= j;)
	{
		*rpara++ = data[k++];
	}
	return COMPLETE;
}


/*******************************************************************************
函 数 名:  	Set_RS232Cfg_Attr
功能说明:  	设置串口配置参数
参    数:  	pData: 	单元号/属性号/参数长度/参数值
返 回 值:  	操作结果
*******************************************************************************/
MsgResult_t Set_RS232Cfg_Attr(UnitPara_t *pData)
{
   if (FALSE == UART_CfgCheck((UARTCfg_t*)pData->data))	//参数异常
   {
	   return PARA_MEANING_ERR;
   }
   
   UART_CfgSave((UARTCfg_t*)pData->data);	//保存设置参数
   Thread_Login(ONCEDELAY, 0, 10, UART_Init);	//随后重置433配置
    
   return COMPLETE;
}

/*******************************************************************************
函 数 名:  	Get_RS232Cfg_Attr
功能说明:  	获取串口配置参数
参    数:  	pData: 	单元号/属性号/参数长度/参数值
			*rlen-返回参数长度
			*rpara-返回参数存放地址
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_RS232Cfg_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rlen = sizeof(UARTCfg_t);
	UART_CfgGet((UARTCfg_t*)rpara);
		
    return COMPLETE;
}
/*****************************************************************************
函 数 名:  	Air_Instruct
功能说明:  	空调验证函数
参    数:  	无
返 回 值:  	无
*****************************************************************************/
void Air_Instruct(void)
{
	Thread_Login(MANY, 6, 800, Backlight_Instruct);
}

/**************************Copyright BestFu 2014-05-14*************************/
