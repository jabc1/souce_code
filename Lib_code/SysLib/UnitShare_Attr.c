/***************************Copyright BestFu 2014-05-14*************************
文	件：	UnitShare_Attr.c
说	明：	个单元共用属性实现函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014-04-08
修　改：	Unarty 2014.10.28,修改所有单元验证接口异常Bug。
			2014.12.28 Unarty 修改单元属性执行边界条件判断值，防止多单元执行时属性越界
*******************************************************************************/
#include "UnitShare_Attr.h"
#include "cmdProcess.h"
#include "Instruct.h"
#include "Thread.h"


/*******************************************************************************
函 数 名:  	Set_ManyUnit_Attr
功能说明:  	设置多个单元属性
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_ManyUnit_Attr(UnitPara_t *pData)
{
	if (pData->len > 0)	//核对参数长度
	{
		MsgResult_t result; 
		u8 buf[20];
		u8 paraLen = pData->len;	//参数长度
		u8 *pPara = (u8*)&pData->data;	//参数区指针
		
		while (pPara < &pData->data[paraLen]) //参数区没有使用完
		{
			if (pPara[2] > 16)	//如果属性参数长度大于20
			{
				return PARA_LEN_ERR;
			}
			buf[0] = WRITEWITHACK;
			memcpy(&buf[1], pPara, (3 + pPara[2]));
			result = Msg_Execute((CMDPara_t*)buf);
			
			if (result != COMPLETE)	//指令执行不正确
			{
				*(u16*)&pData->data[0] = *(u16*)&buf[1];  //赋值单元号与属性号
				*(u8*)&pData->data[2] = (u8)result;
				pData->len = 3;
				return OWN_RETURN_PARA;
			}
			pPara += (3 + pPara[2]);
		}
		return COMPLETE;		
	}
	
	return PARA_LEN_ERR;
}
							
/*******************************************************************************
函 数 名:  	Get_ManyUnit_Attr
功能说明:  	获取多个单元属性值
参    数:  	pData->unit:	操作单元
			pData->cmd:		操作指令
			pData->len：	参数长度
			pData->data:	参数内容
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Get_ManyUnit_Attr(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	*rlen = 0;	//返回数据长度清零
	
	if (pData->len > 0)	//获取某些单元的属性
	{
		u8 para[255]; //参数区
		u8 paraLen = pData->len;
		u8 *pPara = &para[paraLen];	//参数区指针
		u8 *aimPara = para;			//下传消息参数
		
		memcpy(aimPara, pData->data, paraLen);
		
		while (aimPara < &para[paraLen])
		{
			((CMDPara_t*)pPara)->msgType = READWITHACK;
			((CMDPara_t*)pPara)->unit = *aimPara++;
			((CMDPara_t*)pPara)->cmd = *aimPara++;
			((CMDPara_t*)pPara)->len = 0;		//不往下传参 
			if (COMPLETE == Msg_Execute((CMDPara_t*)pPara))	//如果消息完成
			{
				rpara[(*rlen)++] = ((CMDPara_t*)pPara)->unit;	//单元号
				rpara[(*rlen)++] = ((CMDPara_t*)pPara)->cmd;	//属性号				
				rpara[(*rlen)++] = --((CMDPara_t*)pPara)->len; //参数长度，（因为每次获取，最前面的一个参数为：属性执行结果。结果已做判断，所以在此移除）
				memcpy(&rpara[*rlen], &pPara[sizeof(CMDPara_t)+1], ((CMDPara_t*)pPara)->len); //将获取的单元属性内容放置到返回参数区中
				*rlen += ((CMDPara_t*)pPara)->len;
				if (*rlen > MAX_ATTR_NUM)	//如果获取的返回参数内容比较大
				{
					break;
				}
			}
		}
	}
	else	//批量获取单元的属性
	{
		u8 i, data[4]; 	//参数区
		const AttrExe_st *pAttrExe;
		
		i = UnitCnt_Get();
		while (i--)
		{	
			pAttrExe = UnitTab[i].attrExe;
			do {
				if (LEVEL_1 == pAttrExe->level)	//如果属性等级较高
				{
					data[0] = i;			//单元	
					data[1] = pAttrExe->cmd;//属性
					data[2] = 0;			//参数长度
					if ((pAttrExe->pGet_Attr != NULL)
						&& (COMPLETE == pAttrExe->pGet_Attr((UnitPara_t*)data, &data[3], &rpara[(*rlen)+3]))	//如果获取属性内容完成
						)
					{
						rpara[(*rlen)++] = i;				//单元
						rpara[(*rlen)++] = pAttrExe->cmd;	//属性
						rpara[(*rlen)++] = data[3];			//参数长度
						*rlen += data[3];
						
						if (*rlen > MAX_ATTR_NUM)	//如果获取的返回参数内容比较大
						{
							break;
						}
					}
				}
			}while ((pAttrExe->cmd < (++pAttrExe)->cmd)		//2014.12.28 Unarty Add
					&& (pAttrExe->cmd < 0x0100));
		}
	}
	
	return COMPLETE;	
}

/*******************************************************************************
函 数 名:  	Set_UnitChecking_Attr
功能说明:  	设置单元验证
参    数:  	data[0]: 目标单元
			data[1]: 参数长度
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t Set_UnitChecking_Attr(UnitPara_t *pData)
{
	u32 i;
	
	i = UnitCnt_Get();	//获取有效单元数
	
	if (1 != pData->len)
	{
		return PARA_LEN_ERR;	//参数长度错误
	}
	else if (pData->data[0] >= i)
	{
		return PARA_MEANING_ERR;//参数意义错误	
	}
	
	if (0 == pData->data[0])	//验证所有单元
	{
		for (i = UnitCnt_Get(); i > 0;)
		{
			i--;
			if (UnitTab[i].Instruct != NULL)
			{
				UnitTab[i].Instruct(); //注册单元验证
			}
		}
	}
	else
	{
		if (UnitTab[pData->data[0]].Instruct != NULL)
		{
			 UnitTab[pData->data[0]].Instruct(); //注册单元验证
		}
	}
	
	Thread_Login(MANY, 200, 50, &Instruct_Run);	//注册设备验证
	
	return COMPLETE;	
}

/**************************Copyright BestFu 2014-05-14*************************/
