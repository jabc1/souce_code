/***************************Copyright BestFu 2014-05-14*************************
文	件：	UserDataDownload.c
说	明：	用户数据下载实现原码
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.08.28 
修　改：	2014.12.25 Unarty 修改Flash用户数据存储区地址范围
*******************************************************************************/
#include "UserDataDownload.h"
#include "Program.h"

UserDataDownload_t UserDownload;

/*******************************************************************************
函 数 名：	UserDataDownload_Init
功能说明： 	用户数据下载标识初始化
参	  数： 	无
返 回 值:  	无
*******************************************************************************/
void UserDataDownload_Init(void)
{
	memset(&UserDownload, 0, sizeof(UserDownload));
	UserDownload.save = NULL;	
}

/*******************************************************************************
函 数 名：	UserDataDownload_Ready
功能说明： 	下载准备属性
参	  数： 	pData: 单元/属性/长度/内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t UserDataDownload_Ready(UnitPara_t *pData)
{
	u32 dataSize;
	
	if (pData->len < 10)	
	{
		return PARA_LEN_ERR;
	}
	
	dataSize = *(u32*)&pData->data[0];		//数据大小
	UserDownload.frameSize = pData->data[4];	//每帧数据大小
	UserDownload.addr = *(u32*)&pData->data[6];	//用户数据下载存储地址
	UserDownload.frameCount = dataSize/UserDownload.frameSize;//获取数据总包数
	if (0 != dataSize%UserDownload.frameSize)	//最后为一个不完整包
	{
		UserDownload.frameCount++;
	}
	memset(UserDownload.frameFlag, 0, sizeof(UserDownload.frameFlag));	//清空帧标识区
	
	if ((sizeof(UserDownload.frameFlag)*8) < UserDownload.frameCount)	//总包数过大
	{
		return PARA_MEANING_ERR;
	}
	else if (USER_FLASH == pData->data[5])	//存储到用户Flash区
	{
		u32 start, end;		//Flash用户数据存储启始/结束地址
		
		UserDownload.save = Program_Write;			//调用Flash写入函数
		
		switch (*(u16*)USER_FLASH_AREA)	//IC Flash大小
		{
			case 64:			//64K
				start = 0x08010000;
				end   = 0x08010000;
				break;
			case 128:			//128k
				start = 0x08018000;
				end   = 0x08020000;
				break;
			case 256:			//256k
				start = 0x08032800;
				end   = 0x08040000;
				break;
			case 512:			//512K
				start = 0x08064800;
				end   = 0x08080000;
				break;
			default : 
				start = 0;
				end   = 0;
				break;
		}
		
		if ((start <= UserDownload.addr)	//存储地址合法
			&& (end > (dataSize + UserDownload.addr))	//数据存储数据不会溢出
			)
		{	
			Program_Erase(UserDownload.addr, dataSize);	//擦除用户Flash区
			
			return COMPLETE;
		}		
	}
	else if (USER_EEPROM == pData->data[5])	//存储到用户EEPROM区
	{
		UserDownload.save = (Save_fun)EEPROM_Write;		//调用Flash写入函数
		if ((USER_EEPROM_START_ADDR <= UserDownload.addr)		//存储地址错误
			&& (USER_EEPROM_END_ADDR > (dataSize + UserDownload.addr))			//数据存储数据溢出
			)
		{
			return COMPLETE;
		}
	}
	
	UserDataDownload_Init();
	
	return PARA_MEANING_ERR;
}

/*******************************************************************************
函 数 名：	UserDataFrame_Save
功能说明： 	下载数据帧存储
参	  数： 	pData: 单元/属性/长度/内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t UserDataFrame_Save(UnitPara_t *pData)
{
	u32 addr;
	u16 id;
	
	id = *(u16*)&pData->data[0];	//数据帧ID
	pData->len -= sizeof(id);	//每帧用户数据内容长度
	
	if ((pData->len > UserDownload.frameSize)	//帧数据长度错误
		|| (NULL == UserDownload.save)	//数据存储函数为空
		)
	{
		return NO_CMD;
	}
	else if (id > UserDownload.frameCount)	//包ID错误
	{
		return PARA_MEANING_ERR;
	}
	
	if (!(UserDownload.frameFlag[id>>5]&(1<<(id&0x1f))))
	{
	    addr = id*UserDownload.frameSize + UserDownload.addr;
	    if (1 != UserDownload.save(addr, pData->len, (u8*)&pData->data[2]))	//存储失败
	    {
			return CMD_EXE_ERR;
	    }
		UserDownload.frameFlag[id>>5] |= (1<<(id&0x1f)); //接收包ID置1
	}
	
	return COMPLETE;
}

/*******************************************************************************
函 数 名:  	UserDataFrame_Check
功能说明:  	用户帧核对
参    数:  	pData: 单元/属性/长度/内容
			rLen:	返回参数长度
			rpara:	返回参数内容
返 回 值:  	消息执行结果
*******************************************************************************/
MsgResult_t UserDataFrame_Check(UnitPara_t *pData, u8 *rlen, u8 *rpara)
{
	u32 i;
	
	for (i = 0, *rlen = 0; i < UserDownload.frameCount; i++)
	{
		if (!(UserDownload.frameFlag[i>>5]&(1<<(i&0x1f))))//包未收到
        {
			*(u16*)&rpara[*rlen] = i;
			(*rlen) += 2;
			if ((*rlen) > 100)
			{
				break;
			}
			
        }
	}
	
	return ((0 == *rlen) ? COMPLETE : (MsgResult_t)0);
}
/**************************Copyright BestFu 2014-05-14*************************/
