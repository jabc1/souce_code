/***************************Copyright BestFu 2014-05-14*************************
文	件：	Object.c
说	明：	目标数据匹配相关函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.23 
修　改：	暂无
*******************************************************************************/
#include "Object.h"
#include "command.h"
#include "UserData.h"
#include "UnitCfg.h"
#include "BestFuLib.h"
#include "SysTick.h"
#include "CMDProcess.h"
#include "Updata.h"
#include "SysHard.h"

typedef u32 (*con)(u8 i);

u32 DelayID;	//用于通信匹配（Unarty 2014.08.07)
RepeatSt RData[REPEAT_MAX]={0};//周旺增加重复流水号，增加时间判断。2015.7.30

u8 Check_Area(const u8 *area, u32 *validUnit);           //区域核对
static u8 Check_Repeat(const CMDAim_t *aim);    //消息重复核对


/*******************************************************************************
函 数 名：	Object_Check
功能说明： 	目标核对
参	  数： 	aim:		消息目标内容
			*validUnit:	目标操作有效单元
返 回 值：	0(消息匹配成功）/ 1(用户号不同）/2(指令重复)/3 (目标不匹配）
            /4(区域不匹配)/5(消息类型越界)
*******************************************************************************/
MsgResult_t Object_Check(u8 *sdata, u8 len, CMDAim_t *aim, CMDPara_t *para, u8 *data)
{
	
	if ( aim->userID == gSysData.userID				//校验主板用户名	
		|| aim->userID == SUPER_USERID				//校验超级用户号
		|| gSysData.userID == DEFAULT_USERID				//用户号为默认值
		)
	{
		if (Check_Repeat(aim) == FALSE)  //指令重复
		{
				return MSG_REPEAT;	//返回指令已运行
		}
		else
		{
#ifndef STM32L151		//不是低功耗产品
			if(Check_BandAddr(aim))
			{
				//进行转发入队列
				fifo_puts(&send433fifo, sdata, len);
			}
#endif
		}			
		if (aim->objectType == SINGLE_ACT)	//单节点操作
		{
			if (aim->object.id != gSysData.deviceID)
			{
				return ID_ERR;	//返回目标匹配错误
			}
			DelayID = 0;	//用于通信匹配（Unarty 2014.08.07)
		}
		else if (aim->objectType == DEV_ALL_ACT && aim->object.id == 0)
		{
			DelayID = aim->sourceID;		//用于通信匹配（Unarty 2014.08.07)
		}
        else
		{
			u32 area, validUnit;
			u8 i, len;

#if (FLASH_ENCRYPTION_EN > 0u)
			u32 temp = 0x00;			
			temp = DecryptionFlash();		//增加代码混淆 jay add 2015.12.07
#endif
			DelayID = aim->sourceID;		//用于通信匹配（Unarty 2014.08.07)
			area = Check_Area(aim->object.area, &validUnit);	//匹配数据区域，获取区域对应的有效单元
#if (FLASH_ENCRYPTION_EN > 0u)
			validUnit <<= temp;area += temp;
#endif
			if (!validUnit)	//没有有效单元
			{
				return AREA_ERR;	//返回区域匹配错误
			}
			
			if (aim->objectType == GROUP_ACT) //组广播	
			{
				memmove(&data[3], &data[0], para->len); //参数据内容区后移
				memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
				len = para->len + 3;
				para->unit = 0;
				para->cmd = 0x00;
				para->len = 0;
				for (i = 1; i < UnitCnt_Get(); i++)
				{
					if ((validUnit&(1<<i))	//单元位于操作范围内
						&& GroupEEPROM_Get(i, area, aim->actNum)	//查找单元是否添加到组内 
						)
					{
						memcpy(&data[para->len], &data[0], len);
						data[para->len] = i;
						para->len += len;
					}
				}
			}
			else if (aim->objectType == SCENE_ACT) //场景广播
			{
				para->unit = 0;
				para->cmd = 0x00;
				para->len = 0;
				for (i = 1; i < UnitCnt_Get(); i++)
				{
					if ((validUnit&(1<<i)))	//单元位于操作范围内
					{
						SceneEEPROM_Get(i, area, aim->actNum, &data[para->len]);	//获取场景设置值
						if (data[para->len] != 0)	//如果场景设置内容为真
						{
							len = data[para->len];	//保存参数区长度
							memmove(&data[para->len], &data[para->len] + 1, len);
							para->len += len;
						} 
					}
				}
			}
			else if (aim->objectType == DEVIVCE_TYPE_ACT)//类型广播
			{
				if(0x00 == para->unit)
				{
					memmove(&data[3], &data[0], para->len); //参数据内容区后移
					memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
					len = para->len + 3;
					para->unit = 0;
					para->cmd = 0x00;
					para->len = 0;
					for (i = 1; i < UnitCnt_Get(); i++)
					{
						if ((validUnit&(1<<i))	//单元位于操作范围内
							&& (aim->actNum == (u16)SOFT->name))	//查找单元是否添加到组内 ,Jay Add 2015.11.14
						{
							memcpy(&data[para->len], &data[0], len);
							data[para->len] = i;
							para->len += len;
						}
					}
				}
				else
				{			
					if(para->unit > UnitCnt_Get() - 1) return PARA_MEANING_ERR;  //操作单元越界检查					
					if(!((validUnit&(1 << (para->unit)))&&			//单元位于操作范围内
						((aim->actNum == (u16)SOFT->name) ||		//专用于酒店门铃,Jay Add 2015.11.14
						((0x001D == (u16)SOFT->name)&&(0x001E == aim->actNum))))
					  )
					{
						return PARA_MEANING_ERR;
					}
				}
			}
			else if (aim->objectType == UNIT_USER_TYPE_ACT)//单元应用类型广播
			{
				memmove(&data[3], &data[0], para->len); //参数据内容区后移
				memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
				len = para->len + 3;
				para->unit = 0;
				para->cmd = 0x00;
				para->len = 0;
				for (i = 1; i < UnitCnt_Get(); i++)
				{
					if ((validUnit&(1<<i))	//单元位于操作范围内
						&& (aim->actNum == gUnitData[i].type)	//查找单元是否添加到组内 
						)
					{
						memcpy(&data[para->len], &data[0], len);
						data[para->len] = i;
						para->len += len;
					}
				}
			}
			else if (aim->objectType == UNIT_COMMON_TYPE_ACT)//单元应大类型广播
			{
				memmove(&data[3], &data[0], para->len); //参数据内容区后移
				memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
				len = para->len + 3;
				para->unit = 0;
				para->cmd = 0x00;
				para->len = 0;
				for (i = 1; i < UnitCnt_Get(); i++)
				{
					if ((validUnit&(1<<i))	//单元位于操作范围内
						&& (aim->actNum == gUnitData[i].common)	//查找单元是否添加到组内 
						)
					{
						memcpy(&data[para->len], &data[0], len);
						data[para->len] = i;
						para->len += len;
					}
				}
			}
			else if(aim->objectType == SPEC_AREA_DEV_TYPE_ACT)  //特定区域设备类型广播  Jay Add 2015.11.17
			{
				if(0x00 == para->unit)
				{
					memmove(&data[3], &data[0], para->len); //参数据内容区后移
					memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
					len = para->len + 3;
					para->unit = 0;
					para->cmd = 0x00;
					para->len = 0;
					for (i = 1; i < UnitCnt_Get(); i++)
					{
						if (((*(u32*)gUnitData[i].area) == aim->object.id)	//单元位于操作范围内
							&& (aim->actNum == (u16)SOFT->name))	//查找单元是否添加到组内 ,Jay Add 2015.11.14
						{
							memcpy(&data[para->len], &data[0], len);
							data[para->len] = i;
							para->len += len;
						}
					}
				}
				else
				{	
					if(para->unit > UnitCnt_Get() - 1) return PARA_MEANING_ERR;  //操作单元越界检查					
					if(!(((*(u32*)gUnitData[para->unit].area) == aim->object.id)&&			//单元位于操作范围内
						((aim->actNum == (u16)SOFT->name) ||		//专用于酒店门铃,Jay Add 2015.11.14
						((0x001D == (u16)SOFT->name)&&(0x001E == aim->actNum))))
					  )
					{
						return PARA_MEANING_ERR;
					}
				}
			}
			else if(aim->objectType == SPEC_AREA_UNIT_USER_TYPE_ACT) //特定区域单元应用类型广播  Jay Add 2015.11.17
			{
				memmove(&data[3], &data[0], para->len); //参数据内容区后移
				memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
				len = para->len + 3;
				para->unit = 0;
				para->cmd = 0x00;
				para->len = 0;
				for (i = 1; i < UnitCnt_Get(); i++)
				{
					if (((*(u32*)gUnitData[i].area) == aim->object.id)	//单元位于操作范围内
						&& (aim->actNum == gUnitData[i].type)	//查找单元是否添加到组内 
						)
					{
						memcpy(&data[para->len], &data[0], len);
						data[para->len] = i;
						para->len += len;
					}
				}
			}
			else
			{
				return ADDS_TYPE_ERR;
			}
		}
        
        aim->object.id = aim->sourceID;      //交换地址
		
        return COMPLETE;
	}

    return USER_ID_ERR;
}

/*******************************************************************************
函 数 名：	Check_Area
功能说明： 	核对设备区域号
参	  数： 	area:   区域地址内容
返 回 值：	当前匹配区域值
*******************************************************************************/
u8 Check_Area(const u8 *area, u32 *validUnit)
{
    u8 i, j, k;

   for (i = 4; i > 0; i--)
   {
        if (area[i-1] != AREA_VALUE)  //先核对子区域
        {
            break;
        }
	}  
	for (j = 0, *validUnit = 0xFFFFFFFF; j < i; j++)
	{
		if (area[j] != AREA_VALUE)//再核对总区域 
		{
			for (k = UnitCnt_Get(); k; )	
			{
				if (gUnitData[--k].area[j] != area[j])//核对单元所处区域
				{
					*validUnit &= ~(1<<k);
				}
			}
		}
	}

	return i;
}
#ifndef STM32L151		//不是低功耗产品
/*******************************************************************************
函 数 名：	Check_AreaEx
功能说明： 	核对设备区域号
参	  数： 	area:   区域地址内容
返 回 值：	当前匹配区域值
*******************************************************************************/
u8 Check_AreaEx(const u8 *area)
{
   u8 i;
   for (i = 0; i < 4 ; i++)
   {
			if(!(gUnitData[1].area[i] == area[i]
				|| area[i] == AREA_VALUE))
			{
				return FALSE;
			}
	} 
	return TRUE;
}
#endif
/*******************************************************************************
函 数 名：	Check_Repeat
功能说明： 	核对指令是否重复
参	  数： 	data:	要核对的指令内容
返 回 值：	TRUE(重复)/FALSE(不重复)
*******************************************************************************/
#if 0
static u8 Check_Repeat(const CMDAim_t *aim)
{
	static u32 srcID[10] = {0};
	static u8 serialN[10] = {0}, seat;
	u8 i;

	for (i = 0; i < sizeof(srcID)/sizeof(srcID[0]); i++)
	{
		if (srcID[i] == aim->sourceID)	//数据源地址相等
		{
			if (serialN[i] == aim->serialNum)	//流水号相等
			{
				return FALSE;	//数据已处理
			}
			else
			{
				serialN[i] = aim->serialNum;
				return TRUE;
			}
		}
	}
	seat = ++seat%(sizeof(srcID)/sizeof(srcID[0]));
	serialN[seat] = aim->serialNum;
	srcID[seat]  = aim->sourceID;
		
	return TRUE;
}
#endif
#if 0
static u8 Check_Repeat(const CMDAim_t *aim)
{
	static u8 seat;
	u8 i;

	for (i = 0; i < REPEAT_MAX; i++)
	{
		if (RData[i].srcID == aim->sourceID)	//数据源地址相等
		{
			if (RData[i].serial == aim->serialNum)	//流水号相等
			{
				return FALSE;	//数据已处理
			}
			else
			{
				RData[i].serial = aim->serialNum;
				RData[i].time   = Time_Get();
				return TRUE;
			}
		}
	}
	seat = ++seat%REPEAT_MAX;
	RData[seat].serial = aim->serialNum;
	RData[seat].srcID  = aim->sourceID;
	RData[i].time      = Time_Get();
	
	return TRUE;
}
#endif
static u8 Check_Repeat(const CMDAim_t *aim)
{
	static u8 seat;
	u8 i,j;

	for (i = 0; i < REPEAT_MAX; i++)
	{
		if (RData[i].srcID == aim->sourceID)	//数据源地址相等
		{
			for(j=0; j < 5; j++)
			{
				if(RData[i].serial[j] == aim->serialNum)
				{
					return FALSE;	//数据已处理
				}
			}
			
			RData[i].serialSeat = ++RData[i].serialSeat%5;   //流水号偏移位置
			RData[i].serial[RData[i].serialSeat] = aim->serialNum;	//记录流水号
			return TRUE;
		}
	}
	//原地址不相同
	seat = ++seat%REPEAT_MAX; //原地址偏移位置
	RData[seat].serial[0] = aim->serialNum; //流水号
	RData[seat].srcID     = aim->sourceID;  //原地址

	return TRUE;
}
#if 0
/*******************************************************************************
函 数 名：	ClearRepeatSerialNum
功能说明： 	核对重复指令的时间是否超时，超时则清除记录
参	  数： 	void
返 回 值：	void
*******************************************************************************/
void ClearRepeatSerialNum(void)
{
	u32 time=Time_Get();
	u8 i;

	for (i = 0; i < REPEAT_MAX; i++)
	{
		if ((RData[i].time - time > REPEAT_OVERT) 
			|| (time < RData[i].time))	//超时10秒，或是小于之前记录值
		{
			RData[i].serial = 0;
			RData[i].time   = 0;
		}
	}

	return;
}
#endif
#ifndef STM32L151		//不是低功耗产品
/*******************************************************************************
函 数 名：	Check_BandAddr
功能说明： 	核实aim是否有绑定转发地址，有则转发，没有则不转发
参	  数： 	aim
返 回 值：	TRUE(有)/FALSE(没有)
*******************************************************************************/
u8 Check_BandAddr(CMDAim_t *aim)
{
	u8 i;
	
	if(gSysData.BandAreaEn == 1) //区域转发功能已开启
	{
		if (aim->objectType >= GROUP_ACT) //如果是广播类型，所有的指令都进行转发
		{
			if(Check_AreaEx(aim->object.area))	//匹配区域
			{
				return TRUE;
			}
		}
	}
	
	for(i = 0; i < 5; i++)
	{
		if(!gSysData.BandAddr[i] || gSysData.BandAddr[i] == 0xFFFFFFFF)//为0的话，无绑定功能，查找下一个
		{
			continue;
		}

		if((aim->sourceID == gSysData.BandAddr[i])//判断第i个是否有路由转发功能？
			|| (aim->object.id == gSysData.BandAddr[i]))
		{
			return TRUE;
		}
	}
	return FALSE;
}
#endif 
/*******************************************************************************
//函 数 名：	Type_ActValue_
//功能说明： 	类型广播赋值
//参	  数： 	validUnit:	有效单元
//			*para:	传入指令参数
//			data:	参数内容
//			fun: 	核对函数
//返 回 值：	TRUE(重复)/FALSE(不重复)
*******************************************************************************/
//void Type_ActValue(u32 validUnit, CMDPara_t *para, u8 *data, con fun)
//{
//	u8 i, len;
//	
//	memmove(&data[3], &data[0], para->len); //参数据内容区后移
//	memcpy(&data[0], &para->unit, 3);		//用于放置参数描述区内容：单元号，属性号，参数长度
//	len = para->len + 3;
//	para->unit = 0;
//	para->cmd = 0x00;
//	para->len = 0;
//	for (i = 1; i < UnitCnt_Get(); i++)
//	{
//		if ((validUnit&(1<<i))	//单元位于操作范围内
//			&& (fun(i) == TRUE)	//查找单元是否添加到组内 
//			)
//		{
//			memcpy(&data[para->len], &data[0], len);
//			data[para->len] = i;
//			para->len += len;
//		}
//	}
//}

/*******************************************************************************
//函 数 名：	Group_Check
//功能说明： 	组广播核对
//参	  数： 	num: 组号
//			
//返 回 值：	TRUE(重复)/FALSE(不重复)
*******************************************************************************/

/**************************Copyright BestFu 2014-05-14*************************/
