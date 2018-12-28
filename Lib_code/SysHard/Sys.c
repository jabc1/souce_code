/***************************Copyright BestFu 2014-05-14*************************
文	件：    Sys.c
说	明：    STM32内核相关设备函数
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-08-24
修　改：	暂无
*******************************************************************************/
	 
#include "stm32f10x.h"
#include "BF_type.h"
#include "sys.h"

//禁止全部中断
__asm void DisableIRQ(void)
{
	CPSID I;
	BX   LR;
}
//使能全部中断
__asm void EnableIRQ(void)
{
	CPSIE I; 
	BX   LR;
}
/*******************************************************************************
函 数 名：void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)
功能说明：设置向量表偏移地址
参	  数：NVIC_VectTab:基址	 Offset:偏移量
返 回 值：无
*******************************************************************************/
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//设置NVIC的向量表偏移寄存器
	//用于标识向量表是在CODE区还是在RAM区
}

/*******************************************************************************
函 数 名：void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	
功能说明：设置NVIC分组
参	  数：NVIC_Group:NVIC分组 0~4 总共5组 
返 回 值：无
*******************************************************************************/
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}

/*******************************************************************************
函 数 名：void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)		
功能说明：设置NVIC 
参	  数：NVIC_PreemptionPriority:抢占优先级
			NVIC_SubPriority       :响应优先级
			NVIC_Channel           :中断编号
			NVIC_Group             :中断分组 0~4
			注意优先级不能超过设定的组的范围!否则会有意想不到的错误
			组划分:
			组0:0位抢占优先级,4位响应优先级
			组1:1位抢占优先级,3位响应优先级
			组2:2位抢占优先级,2位响应优先级
			组3:3位抢占优先级,1位响应优先级
			组4:4位抢占优先级,0位响应优先级
			NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先
返 回 值：无
*******************************************************************************/
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 tmppriority = 0x00u;
	
	/* Compute the Corresponding IRQ Priority --------------------------------*/ 
	MY_NVIC_PriorityGroupConfig(NVIC_Group);					//设置分组
	tmppriority	= NVIC_PreemptionPriority << (0x4 - NVIC_Group);//抢占优先级设置
	tmppriority|= NVIC_SubPriority & (0x0f >> NVIC_Group);		//响应优先级设置
	tmppriority <<= 0x04;
	NVIC->IP[NVIC_Channel] = tmppriority;
	/* Enable the Selected IRQ Channels --------------------------------------*/
	NVIC->ISER[NVIC_Channel >> 0x05] = 						
		(u32)0x01 << (NVIC_Channel & (u8)0x1F); //使能中断位(要清除的话,相反操作就OK)	
}



/*******************************************************************************
函 数 名：void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
功能说明：外部中断配置函数
参	  数：只针对GPIOA~G;不包括PVD,RTC和USB唤醒这三个
			参数:GPIOx:0~6,代表GPIOA~G;BITx:需要使能的位;TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
			该函数一次只能配置1个IO口,多个IO口,需多次调用
			该函数会自动开启对应中断,以及屏蔽?  
返 回 值：无
*******************************************************************************/
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//得到中断寄存器组的编号
	EXTOFFSET=(BITx%4)*4;
	RCC->APB2ENR    |= 1<<0;//使能SYSCFG APB2 Clock.
	
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//清除原来设置！！！
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx


	EXTI->IMR|=1<<BITx;//  开启line BITx上的中断
	//EXTI->EMR|=1<<BITx;//不屏蔽line BITx上的事件 (如果不屏蔽这句,在硬件上是可以的,但是在软件仿真的时候无法进入中断!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx上事件下降沿触发
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx上事件上升降沿触发
} 

///*******************************************************************
//函 数 名：void BKP_Write(u8 reg,u16 dat)
//功能说明：后备寄存器写入操作
//参	  数：reg:寄存器编号
//		  reg:要写入的数值 
//返 回 值：无
//*******************************************************************/	  
////void BKP_Write(u8 reg,u16 dat)
////{
////  RCC->APB1ENR|=1<<28;     //使能电源时钟	    
////	RCC->APB1ENR|=1<<27;     //使能备份时钟	    
////	PWR->CR|=1<<8;           //取消备份区写保护 
////	switch(reg)
////	{
////		case 1:
////			BKP->DR1=dat;
////			break;
////		case 2:
////			BKP->DR2=dat;
////			break;
////		case 3:
////			BKP->DR3=dat;
////			break; 
////		case 4:
////			BKP->DR4=dat;
////			break;
////		case 5:
////			BKP->DR5=dat;
////			break;
////		case 6:
////			BKP->DR6=dat;
////			break;
////		case 7:
////			BKP->DR7=dat;
////			break;
////		case 8:
////			BKP->DR8=dat;
////			break;
////		case 9:
////			BKP->DR9=dat;
////			break;
////		case 10:
////			BKP->DR10=dat;
////			break;
////	} 
////}	

///*******************************************************************
//函 数 名：void Sys_Soft_Reset(void)
//功能说明：系统软复位
//参	  数：无
//返 回 值：无
//*******************************************************************/	     
//__asm void Sys_Soft_Reset(void)
//{
//	MOV R0, #1           //; 
//	MSR FAULTMASK, R0    //; 清除FAULTMASK 禁止一切中断产生
//	LDR R0, =0xE000ED0C  //;
//	LDR R1, =0x05FA0004  //; 
//	STR R1, [R0]         //; 系统软件复位
//deadloop
//    B deadloop        //; 死循环使程序运行不到下面的代码
//}

/*******************************************************************
函 数 名：	 __set_MSP
功能说明：	设置堆栈指针地址
参	  数：	无
返 回 值：	无
*******************************************************************/
__asm void Set_MSP(unsigned int mainStackPointer)
{
	msr msp, r0
	bx lr
}

/*******************************************************************
函 数 名：	Jump_To_Code
功能说明：	跳转到地址对应的程序区运行
参	  数：	addr:	对代码存储区首地址
返 回 值：	无
*******************************************************************/
void JumpToCode(u32 ApplicationAddress)
{
	volatile u32* p = (volatile u32*)ApplicationAddress;
	if (((*(vu32 *)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{
		Set_MSP(*p);		//取出初始的MSP的值
		((void (*)(void))(*(p + 1)))();
	}
}

///*******************************************************************
//函 数 名：void JTAG_Set(u8 mode)
//功能说明：JTAG模式设置,用于设置JTAG的模式
//参	  数：mode:jtag,swd模式设置;00,全使能;01,使能SWD;10,全关闭;
//返 回 值：无
//*******************************************************************/	  
//void JTAG_Set(u8 mode)
//{
//	u32 temp;
//	temp=mode;
//	temp<<=25;
//	RCC->APB2ENR|=1<<0;     //开启辅助时钟	   
////	AFIO->MAPR&=0XF8FFFFFF; //清除MAPR的[26:24]
////	AFIO->MAPR|=temp;       //设置jtag模式
//} 

/**< 恢复中断 */
__asm void CPU_SR_Restore(u32 cpu_sr)
{
    MSR     PRIMASK, R0
    BX      LR
}

/**< 保存中断状态，关中断 */
__asm u32 CPU_SR_Save(void)
{
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}

/**************************Copyright BestFu 2014-05-14*************************/ 

