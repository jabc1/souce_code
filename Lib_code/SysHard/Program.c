/***************************Copyright BestFu 2014-05-14*************************
文	件：    Program.c
说	明：    STM32_Flash烧入
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-11-21
修　改：	暂无
*******************************************************************************/
#include "stm32f10x.h"
#include "program.h"
#include "BF_type.h"

#define FLASH_DEFAULT		(0xFFFFFFFF)
#define STM_SECTOR_SIZE     (PAGE_SIZE)
#define STM32_FLASH_BASE    (0x08000000)      //STM32 FLASH基地址

/*******************************************************************************
函 数 名：	Program_Erase
功能说明： 	擦除要编程的位置内容
参	  数： 	addr:   写入数据的首地址
            size:   要写入数据的大小
返 回 值：	擦除结果TRUE/FALSE
*******************************************************************************/
u8 Program_Erase(u32 addr, u32 size)
{   
	Flash_Unlock();
    while (FLASH_ErasePage(addr)&&(size > PAGE_SIZE))
    {
        size -= PAGE_SIZE;
        addr += PAGE_SIZE;
    }
  	Flash_lock();

    return (size > PAGE_SIZE ? FALSE : TRUE);
}

/*******************************************************************************
函 数 名：	Program_Write
功能说明： 	写入编程
参	  数： 	addr:    写入数据的首地址
            size:   要写入数据的大小
            data:   写入数据内容
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/
u8 Program_Write(u32 addr, u32 len, u8 *data)
{
	u8 flag;
	
	if (len & 0x01)		//内部为半字节写入
	{
		len++;
	}
	Flash_Unlock();
	flag = Flash_Write(addr, len, data);
	Flash_lock();

	return flag;
}

/*******************************************************************************
函 数 名：	Program_Read
功能说明： 	读出编程数据内容
参	  数： 	addr:   读出数据的首地址
            size:   要读出数据的大小
            data:   读出数据内容
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/
u8 Program_Read(u32 addr, u32 len, u8 *data)
{
    while (len > 0)
    {
        *(vu32*)data = *(vu32*)addr;
        data += 4;
        addr += 4;
        len  -= 4;
    }
    
	return TRUE;
}

/*******************************************************************************
函 数 名：	u8 Program_Read_Ext(u32 addr, u32 len, u8 *data) 
功能说明： 	带扩展的读出编程数据内容
参	  数： 	addr:   读出数据的首地址
            size:   要读出数据的大小
            data:   读出数据内容
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/
u8 Program_Read_Ext(u32 addr, u32 len, u8 *data)   				/**> 增加flash编程扩展接口，Jay Add 2015.11.13 **/
{
    while ((len--) > 0)
    {
        *(vu8*)data = *(vu8*)addr;
        data += 1;
        addr += 1;
    }   
	return TRUE;
}

/*******************************************************************************
函 数 名：	Program_Write_Ext
功能说明： 	带扩展的写入编程
参	  数： 	addr:    写入数据的首地址
            size:   要写入数据的大小
            data:   写入数据内容
返 回 值：	写入结果TRUE/FALSE
*******************************************************************************/
u8 Program_Write_Ext(u32 addr, u32 len, u8 *data)			  /**> 增加flash编程扩展接口，Jay Add 2015.11.13 **/
{
	u8 flag;
    u32 secpos;	                                              /**>  扇区地址                           **/
	u16 secoff;	                                              /**>  扇区内偏移地址                     **/
	u16 secremain;                                            /**>  扇区内剩余地址                     **/	   
 	u16 i;    
	u32 offaddr;                                              /**>  去掉0X08000000后的地址             **/
    u8 STMFLASH_BUF[STM_SECTOR_SIZE];
    
	if (len & 0x01)		                                      /**>  内部为半字节写入                   **/
	{
		len++;
	}
	Flash_Unlock();
    if((addr < STM32_FLASH_BASE)||                            /**> 若地址越界，直接返回错误            **/
       (addr >= (STM32_FLASH_BASE+1024*STM_SECTOR_SIZE)))  return 0;
  
    offaddr   = addr-STM32_FLASH_BASE;		                  /**>  实际偏移地址                       **/
	secpos    = offaddr/STM_SECTOR_SIZE;			          /**>  扇区地址  0~127 for STM32F103cBT6  **/
	secoff    = offaddr%STM_SECTOR_SIZE;		              /**>  在扇区内的偏移                     **/
	secremain = STM_SECTOR_SIZE-secoff;		                  /**>  扇区剩余空间大小                   **/   
	if(len <= secremain)  secremain = len;                    /**>  不大于该扇区范围                   **/
    while(1) 
	{	
		Program_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, /**>  读出整个扇区的内容                 **/
                     STM_SECTOR_SIZE,
                     STMFLASH_BUF);
		for(i=0;i<secremain;i++)                              /**>  校验数据                           **/
		{
			if((u8)FLASH_DEFAULT != STMFLASH_BUF[secoff+i]) break;             	  
		}
		if(i<secremain)                                       /**>  需要擦除                           **/  
		{
            flag=FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);
			for(i=0;i<secremain;i++)  
			{
				STMFLASH_BUF[i+secoff] = data[i];	  
			}
            flag=Flash_Write(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE, STM_SECTOR_SIZE, STMFLASH_BUF);
		}else flag=Flash_Write(addr,secremain,data);         /**>  写已经擦除了的,直接写入扇区剩余区间**/ 				   
		if(len==secremain)break;                             /**>  写入结束了                         **/
		else                                                 /**>  写入未结束                         **/
		{
			secpos++;                                        /**>  扇区地址增1                        **/
			secoff=0;				                         /**>  偏移位置为0 	                      **/ 
            data += secremain;                               /**>  指针偏移                           **/
			addr += secremain;	                             /**>  写地址偏移                         **/  
            len  -= secremain;	                             /**>  字节数递减                         **/
			if(len>(STM_SECTOR_SIZE)) secremain=STM_SECTOR_SIZE;/**>  下一个扇区还是写不完               **/
			else secremain = len;                            /**>  下一个扇区可以写完了               **/
		}	 
	};
	Flash_lock();
	return flag;
}

#if (FLASH_ENCRYPTION_EN > 0u)
/*******************************************************************************
函 数 名：	void FlashEncryptionInit(void)
功能说明： 	Flash加密初始化
参	  数： 	无
返 回 值：	无
*******************************************************************************/
void FlashEncryptionInit(void)
{
	u32 encryption_info[2] = {ENCRYPTION_FLAG,0};
	RCC->AHBENR |= RCC_AHBENR_CRCEN;
	CRC->CR	  |= CRC_CR_RESET;
	CRC->IDR	  |= CRC_IDR_IDR;
	if(ENCRYPTION_FLAG != *(vu32*)(ENCRYPTION_FLAG_ADDR))
	{
		encryption_info[1] = FlashEncryptionFunc(RE_DEVICE_ID);
		Program_Write_Ext(ENCRYPTION_FLAG_ADDR,sizeof(encryption_info),(u8*)(encryption_info));
	}
}
#endif

/*******************************************************************************
函 数 名：	Flash_DefaultValue
功能说明： 	False默认值
参	  数： 	void
返 回 值：	默认状态下Flashw值
*******************************************************************************/
u32 Flash_DefaultValue(void)
{
	return (FLASH_DEFAULT);
}

/**************************Copyright BestFu 2014-05-14*************************/
