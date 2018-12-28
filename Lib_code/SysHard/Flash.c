/***************************Copyright BestFu 2014-05-14*************************
文	件：	Flash.c
说	明：	STM32F103_Flash操作函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2013-11-21
修  改:     暂无
*******************************************************************************/
#include "stm32f10x.h"
#include "Flash.h"
#include "BF_type.h"


/* FLASH Keys */
#define RDP_Key                  ((u16)0x00A5)
#define FLASH_KEY1               ((u32)0x45670123)
#define FLASH_KEY2               ((u32)0xCDEF89AB)

#define EraseTime 		((u32)0x2000)
#define ProgramTime		((u32)0x3000)

static u8 Flash_EOP(void);
static u8 Flash_Status(u32 timeOut);

/*******************************************************************************
函 数 名：	Flash_Unlock
功能说明： 	STM32_Flash解锁
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Flash_Unlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*******************************************************************************
函 数 名：	Flash_Unlock
功能说明： 	STM32_Flash上锁
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void Flash_lock(void)
{
    FLASH->CR |= CR_LOCK;
}

/*******************************************************************************
函 数 名：	FLASH_ErasePage
功能说明： 	Flash页擦除
参	  数： 	page_add:  业地址
返 回 值：  TRUE/FALSE
*******************************************************************************/
u8 FLASH_ErasePage(u32 page_add)
{
    u8 flag = Flash_Status(EraseTime);

    if (!flag)//没有flash错误
    {
        FLASH->CR |= CR_PER; //使能页擦除
        FLASH->AR = page_add;//要擦除页地址
        FLASH->CR |= CR_STRT;//开启一次页擦除 
        
        flag = Flash_Status(EraseTime);
        FLASH->CR &= ~CR_PER;   //清除页擦除
    }
    if (!flag)
    {
        flag  = Flash_EOP();
    }
    
    return flag;
}

/*******************************************************************************
函 数 名：	Flash_Status
功能说明： 	获取Flash状态寄存器值
参	  数：  timeOut:    检测超时时间
返 回 值：	Flash_status
*******************************************************************************/
static u8 Flash_Status(u32 timeOut)
{
    for (;(timeOut > 0)&&(FLASH->SR&BUSY); timeOut--)//等待Flash空闲
    {}
    if (!timeOut)//等待超时
    {
        return BUSY;
    }
    if (FLASH->SR&(PGERR|WRITEERR))
	{
		FLASH->SR = PGERR|WRITEERR;
		return 1;
	}

    return 0;
}

/*******************************************************************************
函 数 名：	Flash_ProgramHalfWord
功能说明： 	写入半个字节
参	  数：  add:    写入地址
            data:   写入数据内容
返 回 值：	TRUE/FLASE
*******************************************************************************/
u8 Flash_ProgramHalfWord(u32 add, u16 data)
{
    u8 flag = Flash_Status(ProgramTime);

    if (!flag)
    {
        FLASH->CR |= CR_PG;
        *(vu16*)add = (u16)data;
        flag = Flash_Status(ProgramTime);
    
        FLASH->CR &= ~CR_PG;
    }
    if (!flag)
    {
        if (Flash_EOP())
        {
            flag = TRUE; 
        }
    }

    return flag;
}

/*******************************************************************************
函 数 名：	Flash_Write
功能说明： 	flash写入数据
参	  数：  add:    写入首地址
            len:    写入数据大小
            data:   写入数据内容
返 回 值：	TRUE/FLASE
*******************************************************************************/
u8 Flash_Write(u32 add, u32 len, u8 *data)
{
    if (!Flash_Status(ProgramTime))
    {
        FLASH->CR |= CR_PG;  //进入编程
        while (len > 0)
        {
            *(vu16*)add = *(u16*)data;
            data += 2;
            add  += 2;
            len  -= 2;
            if (!Flash_Status(ProgramTime))//未超时
            {
                if (!Flash_EOP())
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    
        FLASH->CR &= ~CR_PG;
    } 

    return (len == 0 ? TRUE : FALSE);
}

/*******************************************************************************
函 数 名：	Flash_EOP
功能说明： 	获取Flash操作结果标志位
参	  数：  无
返 回 值：	FALSE/TRUE (Flash写入或擦除操作结果)
*******************************************************************************/
static u8 Flash_EOP(void)
{
    if (FLASH->SR&EOP)//操作成功
    {
        FLASH->SR = EOP; //清除标志
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
函 数 名：	Flash_eraseOptionByte
功能说明： 	擦除整个Flash选项字节
参	  数：  无
返 回 值：	TRUE/FALSE (操作结果)
*******************************************************************************/
u8 Flash_EraseOptionByte(void)
{
    u8 flag = FALSE;
    
    if (!Flash_Status(ProgramTime))
    {
        Flash_Unlock();
        FLASH->CR |= CR_OPTER;
        FLASH->CR |= CR_STRT;
        if (!Flash_Status(ProgramTime))
        {
            if (Flash_EOP())
            {
                flag = TRUE;
            }
        }
          FLASH->CR &= ~CR_OPTER;
        
    }
    return flag;
}


/*******************************************************************************
函 数 名：	Flash_WriteOptionByte
功能说明： 	写入Flash选项字节
参	  数：  addr:   选项字节地址
            data:   内容
返 回 值：	TRUE/FALSE (操作结果)
*******************************************************************************/
u8 Flash_WriteOptionByte(u32 addr, u8 data)
{
    u8 flag = FALSE;
    
    if (!Flash_Status(ProgramTime))
    {
        Flash_Unlock();
        FLASH->CR |= CR_OPTPG;
        FLASH->CR |= CR_STRT;
        *(vu8*)addr = data;
        if (!Flash_Status(ProgramTime))
        {
            if (Flash_EOP())
            {
                flag = TRUE;
            }
        }
        FLASH->CR &= ~CR_OPTER;    
    }
    return flag;
}

/*******************************************************************************
//函 数 名：	Flash_WriteProtect
//功能说明： 	写入Flash选项字节
//参	  数：  addr:   保护的区域地址
//            sta:    0(NO protect)/(~0)(protect)
//返 回 值：	TRUE/FALSE (操作结果)
//注    意:   size <= 32Kb
*******************************************************************************/
//u8 Flash_WriteProtect(u32 addr, u8 sta)
//{
//    u8 seat, flag;
//    
//    addr -= 0x08000000;
//    seat = (u8)(addr>>12)&(0x1f);
//        
//
////    if (sta)
////    {
////        flag = Flash_WriteOptionByte((u32)(&OB->WRP0 + (seat>>3))
////                              ,&OB->WRP0 + ((seat>>3)|(1<<(seat&0x07))));
////    }
////    else
////    {
////       flag = Flash_WriteOptionByte((u32)(&OB->WRP0 + (seat>>3))
////                              ,&OB->WRP0 + ((seat>>3)&(~(1<<(seat&0x07)))));
////    }
//
//    return flag;
//}

#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
/*******************************************************************************
函 数 名：	void Get_UniqueDeviceID(u8* chipidtable)
功能说明： 	获取stm32 flash全球唯一ID
参	  数：  chipidtable:存储96位全球唯一ID的首地址
返 回 值：	无
*******************************************************************************/
void Get_UniqueDeviceID(u8* chipidtable)
{
	*(u32*)(&chipidtable[0]) = *((vu32*)(CHIPID_ADDR + 0x00));
	*(u32*)(&chipidtable[4]) = *((vu32*)(CHIPID_ADDR + 0x04));
	*(u32*)(&chipidtable[8]) = *((vu32*)(CHIPID_ADDR + 0x08));
}
#endif

/**************************Copyright BestFu 2014-05-14*************************/


