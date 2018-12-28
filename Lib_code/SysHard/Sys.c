/***************************Copyright BestFu 2014-05-14*************************
��	����    Sys.c
˵	����    STM32�ں�����豸����
��	�룺    Keil uVision4 V4.54.0.0
��	����    v1.0
��	д��    Unarty
��	�ڣ�    2013-08-24
�ޡ��ģ�	����
*******************************************************************************/
	 
#include "stm32f10x.h"
#include "BF_type.h"
#include "sys.h"

//��ֹȫ���ж�
__asm void DisableIRQ(void)
{
	CPSID I;
	BX   LR;
}
//ʹ��ȫ���ж�
__asm void EnableIRQ(void)
{
	CPSIE I; 
	BX   LR;
}
/*******************************************************************************
�� �� ����void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)
����˵��������������ƫ�Ƶ�ַ
��	  ����NVIC_VectTab:��ַ	 Offset:ƫ����
�� �� ֵ����
*******************************************************************************/
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset)	 
{  	 
	SCB->VTOR = NVIC_VectTab|(Offset & (u32)0x1FFFFF80);//����NVIC��������ƫ�ƼĴ���
	//���ڱ�ʶ����������CODE��������RAM��
}

/*******************************************************************************
�� �� ����void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	
����˵��������NVIC����
��	  ����NVIC_Group:NVIC���� 0~4 �ܹ�5�� 
�� �� ֵ����
*******************************************************************************/
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}

/*******************************************************************************
�� �� ����void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)		
����˵��������NVIC 
��	  ����NVIC_PreemptionPriority:��ռ���ȼ�
			NVIC_SubPriority       :��Ӧ���ȼ�
			NVIC_Channel           :�жϱ��
			NVIC_Group             :�жϷ��� 0~4
			ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
			�黮��:
			��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
			��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
			��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
			��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
			��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
			NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����
�� �� ֵ����
*******************************************************************************/
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 tmppriority = 0x00u;
	
	/* Compute the Corresponding IRQ Priority --------------------------------*/ 
	MY_NVIC_PriorityGroupConfig(NVIC_Group);					//���÷���
	tmppriority	= NVIC_PreemptionPriority << (0x4 - NVIC_Group);//��ռ���ȼ�����
	tmppriority|= NVIC_SubPriority & (0x0f >> NVIC_Group);		//��Ӧ���ȼ�����
	tmppriority <<= 0x04;
	NVIC->IP[NVIC_Channel] = tmppriority;
	/* Enable the Selected IRQ Channels --------------------------------------*/
	NVIC->ISER[NVIC_Channel >> 0x05] = 						
		(u32)0x01 << (NVIC_Channel & (u8)0x1F); //ʹ���ж�λ(Ҫ����Ļ�,�෴������OK)	
}



/*******************************************************************************
�� �� ����void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
����˵�����ⲿ�ж����ú���
��	  ����ֻ���GPIOA~G;������PVD,RTC��USB����������
			����:GPIOx:0~6,����GPIOA~G;BITx:��Ҫʹ�ܵ�λ;TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
			�ú���һ��ֻ������1��IO��,���IO��,���ε���
			�ú������Զ�������Ӧ�ж�,�Լ�����?  
�� �� ֵ����
*******************************************************************************/
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR=BITx/4;//�õ��жϼĴ�����ı��
	EXTOFFSET=(BITx%4)*4;
	RCC->APB2ENR    |= 1<<0;//ʹ��SYSCFG APB2 Clock.
	
	AFIO->EXTICR[EXTADDR]&=~(0x000F<<EXTOFFSET);//���ԭ�����ã�����
	AFIO->EXTICR[EXTADDR]|=GPIOx<<EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx


	EXTI->IMR|=1<<BITx;//  ����line BITx�ϵ��ж�
	//EXTI->EMR|=1<<BITx;//������line BITx�ϵ��¼� (������������,��Ӳ�����ǿ��Ե�,��������������ʱ���޷������ж�!)
 	if(TRIM&0x01)EXTI->FTSR|=1<<BITx;//line BITx���¼��½��ش���
	if(TRIM&0x02)EXTI->RTSR|=1<<BITx;//line BITx���¼��������ش���
} 

///*******************************************************************
//�� �� ����void BKP_Write(u8 reg,u16 dat)
//����˵�����󱸼Ĵ���д�����
//��	  ����reg:�Ĵ������
//		  reg:Ҫд�����ֵ 
//�� �� ֵ����
//*******************************************************************/	  
////void BKP_Write(u8 reg,u16 dat)
////{
////  RCC->APB1ENR|=1<<28;     //ʹ�ܵ�Դʱ��	    
////	RCC->APB1ENR|=1<<27;     //ʹ�ܱ���ʱ��	    
////	PWR->CR|=1<<8;           //ȡ��������д���� 
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
//�� �� ����void Sys_Soft_Reset(void)
//����˵����ϵͳ��λ
//��	  ������
//�� �� ֵ����
//*******************************************************************/	     
//__asm void Sys_Soft_Reset(void)
//{
//	MOV R0, #1           //; 
//	MSR FAULTMASK, R0    //; ���FAULTMASK ��ֹһ���жϲ���
//	LDR R0, =0xE000ED0C  //;
//	LDR R1, =0x05FA0004  //; 
//	STR R1, [R0]         //; ϵͳ�����λ
//deadloop
//    B deadloop        //; ��ѭ��ʹ�������в�������Ĵ���
//}

/*******************************************************************
�� �� ����	 __set_MSP
����˵����	���ö�ջָ���ַ
��	  ����	��
�� �� ֵ��	��
*******************************************************************/
__asm void Set_MSP(unsigned int mainStackPointer)
{
	msr msp, r0
	bx lr
}

/*******************************************************************
�� �� ����	Jump_To_Code
����˵����	��ת����ַ��Ӧ�ĳ���������
��	  ����	addr:	�Դ���洢���׵�ַ
�� �� ֵ��	��
*******************************************************************/
void JumpToCode(u32 ApplicationAddress)
{
	volatile u32* p = (volatile u32*)ApplicationAddress;
	if (((*(vu32 *)ApplicationAddress) & 0x2FFE0000 ) == 0x20000000)
	{
		Set_MSP(*p);		//ȡ����ʼ��MSP��ֵ
		((void (*)(void))(*(p + 1)))();
	}
}

///*******************************************************************
//�� �� ����void JTAG_Set(u8 mode)
//����˵����JTAGģʽ����,��������JTAG��ģʽ
//��	  ����mode:jtag,swdģʽ����;00,ȫʹ��;01,ʹ��SWD;10,ȫ�ر�;
//�� �� ֵ����
//*******************************************************************/	  
//void JTAG_Set(u8 mode)
//{
//	u32 temp;
//	temp=mode;
//	temp<<=25;
//	RCC->APB2ENR|=1<<0;     //��������ʱ��	   
////	AFIO->MAPR&=0XF8FFFFFF; //���MAPR��[26:24]
////	AFIO->MAPR|=temp;       //����jtagģʽ
//} 

/**< �ָ��ж� */
__asm void CPU_SR_Restore(u32 cpu_sr)
{
    MSR     PRIMASK, R0
    BX      LR
}

/**< �����ж�״̬�����ж� */
__asm u32 CPU_SR_Save(void)
{
    MRS     R0, PRIMASK
    CPSID   I
    BX      LR
}

/**************************Copyright BestFu 2014-05-14*************************/ 

