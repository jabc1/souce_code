/***************************Copyright BestFu 2014-05-14*************************
文	件：	Eeprom.c
说	明：	对Eeprom操作,包括全局参数与EEPROM映射关系，独立读写EEPROM入口源代码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Joey
日	期：	2013.7.23   
修  改:     Unart(2014.04.12增加换页写入时的忙检测)
*******************************************************************************/
#include "EEPROM.h"
#include "I2C.h"
#include "FaultManage.h"

/*******************************************************************************
函 数 名：	ReadDataFromEEPROM
功能说明：	把对应EEPROM中数据读取到RAM中
参	  数：	addr-EEPROM中的实际地址
            len-长度
            data-读取到的数据存放于该指针
返 回 值：	1表示成功，0表示失败
*******************************************************************************/ 
u8 ReadDataFromEEPROM(u32 addr, u32 len, u8 *data)
{
	u32 count;
	while (len)
	{
		if(addr >= EEPROMENDADDR || len > EEPROMSIZE)
		{
			return FALSE;
		}
			
		count = (len > (PAGESIZE-(addr%PAGESIZE))) ?
					(PAGESIZE - (addr%PAGESIZE)) : len;	
		if (!IIC_Read(EEPROMSLAVEADDR, addr, data, count))
		{
			break;
		}
		
		addr += count;
		len  -= count;
		data += count;
		if (!IIC_GetState(EEPROMSLAVEADDR, 0xfffffFFF)) //??EEPROM???? 
		{
			break;
		}
	}

	if (0 == len)
	{
		return TRUE;
	}
	else
	{
		Fault_Upload(FAULT_0, EEPROM_W_ERR, NULL);
		return FALSE;
	}	
}

/*******************************************************************************
函 数 名：	WriteDataToEEPROM
功能说明：	把RAM中的数据写入到对应的EEPROM中
参	  数：	addr-EEPROM中的实际地址
            len-长度
            data-读取到的数据存放于该指针
返 回 值：	1表示成功，0表示失败
*******************************************************************************/ 
u8 WriteDataToEEPROM(u32 addr, u32 len, u8 *data)
{
	u32 count;

	while (len)
	{
		if(addr >= EEPROMENDADDR || len > EEPROMSIZE)
		{
			return FALSE;
		}
		
		count = (len > (PAGESIZE-(addr%PAGESIZE))) ?
					(PAGESIZE - (addr%PAGESIZE)) : len;		
		if(!IIC_Write(EEPROMSLAVEADDR, addr, data, count))
		{
			break;
		}

		addr += count;
		len  -= count;
		data += count;
		if (!IIC_GetState(EEPROMSLAVEADDR, 0xfffffFF)) //??EEPROM???? 
		{
			break;
		}
	}

	if (0 == len)
	{
		return TRUE;
	}
	else
	{
		Fault_Upload(FAULT_0, EEPROM_W_ERR, NULL);
		return FALSE;
	}		
}

/*******************************************************************************
函 数 名：	WriteDataToEEPROMEx
功能说明：	把相同数据写入到对应的EEPROM中
参	  数：	addr-EEPROM中的实际地址
            len-长度
            data-要写入的相同数据
返 回 值：	1表示成功，0表示失败
*******************************************************************************/ 
u8 WriteDataToEEPROMEx(u32 addr, u32 len, u8 data)
{
	u32 count;

	while (len)
	{
		if(addr >= EEPROMENDADDR || len > EEPROMSIZE)
		{
			return FALSE;
		}
		
		count = (len > (PAGESIZE-(addr%PAGESIZE))) ?
					(PAGESIZE - (addr%PAGESIZE)) : len;		
		if(!IIC_WriteSame(EEPROMSLAVEADDR, addr, data, count))
		{
			break;
		}

		addr += count;
		len  -= count;
		if (!IIC_GetState(EEPROMSLAVEADDR, 0xfffffFF)) //??EEPROM???? 
		{
			break;
		}
	}
	if (0 == len)
	{
		return TRUE;
	}
	else
	{
		Fault_Upload(FAULT_0, EEPROM_W_ERR, NULL);
		return FALSE;
	}	
}

/**************************Copyright BestFu 2014-05-14*************************/
