/***************************Copyright BestFu 2014-05-14*************************
文	件：    SI4432.c
说	明：    SI4432驱动文件
编	译：    Keil uVision4 V4.54.0.0
版	本：    v1.0
编	写：    Unarty
日	期：    2013-09-30
修　改：	2014.07.23	Seven  amend	兼容旧库
            **2014.07.25  Seven Amed:
	        最大发送数据包长度：250  (注意内存空间,防止溢出)
			2014.10.08 UnartyChen 	增加指令发送之间的固定间隔时长
			2014.10.17 UnartyChen	修改设备上报，与防止无线假死逻辑
			2014.11.13 UnartyChen	去队数据接收CRC异常记录（因为CRC出错概			率非常高，此时记录数据已没有意义）。
*******************************************************************************/

#include "si4432.h"
#include "GPIO.h"
#include "sys.h"
#include "delay.h"
#include "SPI.h"
#include "cmdProcess.h"
#include "BF_type.h"
#include "Updata.h"
#include "SysHard.h"
#include "Thread.h"
#include "FaultManage.h"
#include "SysTick.h"
#include "Sign433.h"

#define OVERTIME	0x1000	//发送超时时间 
#define BUSYTIME	1000	//连续最大忙检测时间
#define IDLETIME	100		//连续最大空闲时间
#define SPACETIME	200		//指令发送间隔时长，单位：ms
#define CENTER_FREQ  (82) //基准频率（82-64）*10+240+(3) = 423MHZ
#define Crystal_C         0x79////Cint = 1.8 pF + 0.085 pF x xlc[6:0] + 3.7 pF x xtalshift

static u8 SI4432_RSSI;			//
static u8 SI4432SendEn;		//发送使能标识，2014.10.08 Unarty Add
#ifndef SI4432_THRESHOLD
	static u8 Threshold = 160;			//主机门槛值
#endif 
//周旺添加，测试RSSI变量定义
u8 TestRecRSSI[MAX_FRAME_NUM];
u8 TestRecFlag = 0;

//Freq_433M_920KHz
const u8 Si4432Cfg_Tab[21] = {0x2B,0x40,0x0A,0x03,0x68,0x01,0x3A,0x93,0x02,0xF6,
							  0x1D,0x80,0x60,0x4E,0xA5,0x2D,0x23,0x0D,0x53,0x62,0x00};

static u8 Check_433M_Busy(u8 level);
static void Si4432_PortInit(void);				  
static u8   SPI_RW_Reg(u8 addr , u8 data);
static void Si4432_PortInit(void);
static void SetMode_RX(void);
static void SetMode_TX(void);
#ifndef SI4432_THRESHOLD
static u8 Si4432_Threshold(void);
#endif
static void Si4432_Fault(u8 event);
static void SI4432_SendEn(void);	//2014.10.08 Unarty Add
						
//信号指示灯
#if SIGNAL == 1
	#define TX		(0x55)
	#define RX		(0xaa)
	static void Si4332_Signal_Insturct(u8 state);
	static void Signal_Run(void);
	/*******************************************************************************
	函 数 名:  	Signal_Run
	功能说明: 	信号指示
	参    数:  	无		
	返 回 值:  	无
	*******************************************************************************/
	void Signal_Run(void)
	{
		GPIOx_Rvrs(SIGNAL_PORT, SIGNAL_PIN);
	}
	/*******************************************************************************
	函 数 名:  	SI4332_Signal_Insturct
	功能说明: 	433无线信号指示
	参    数:  	state：	433模块状态枚举值			
	返 回 值:  	无
	*******************************************************************************/
	static void Si4332_Signal_Insturct(u8 state)
	{
		SIGNAL_OFF;
		if (state == TX)
		{
			SIGNAL_ON;
			Thread_Login(ONCEDELAY, 0, 500, &Signal_Run);
		}
		else
		{
			Thread_Login(MANY, 20, 25, &Signal_Run);
		}		
	}
#endif

/*******************************************************************************
函 数 名:  	SetRecTestStatus
功能说明: 	设置无线测试标志
参    数:  	flag为非0时，准备好标志；flag为0时，未准备好标志	
返 回 值:  	无
*******************************************************************************/
void SetRecTestStatus(u8 flag)
{
	TestRecFlag = flag ? 1 : 0;
}
/*******************************************************************************
函 数 名:  GetRSSIPara
功能说明: 	获取RSSI参数
参    数:  	无
返 回 值:  	RSSI数据存储首地址
*******************************************************************************/
u8  *GetRSSIPara(void)
{
	return TestRecRSSI;
}
/*******************************************************************************
函 数 名:  GetThresholdPara
功能说明: 	获取通信门槛值
参    数:  	无
返 回 值:  通信门槛值
*******************************************************************************/
u8  GetThresholdPara(void)
{
#ifndef SI4432_THRESHOLD
	return Threshold ;
#else
	return SI4432_THRESHOLD;//主机门槛值
#endif 	
}
/*******************************************************************************
函 数 名:  	u8 SPI_RW_Reg(u8 addr,u8 data)
功能说明:  	通过SPI发送两个字节
参    数:  	u8 addr - 寄存器地址
			u8 data - 寄存器设定值 			
返 回 值:  	无
*******************************************************************************/
static u8 SPI_RW_Reg(u8 addr,u8 data)
{
	u8 status;
    
	nSEL(0);	//开433片选 
	SPI2_ReadWriteByte(addr);
	status = SPI2_ReadWriteByte(data);
	nSEL(1);    //关433片选
        
	return status;
}

/*******************************************************************************
函 数 名:  	void Si4432_PortInit(void)
功能说明: 	初始化si4432需要用到的GPIO，SPI管脚
参    数:  	无 			
返 回 值:  	无
*******************************************************************************/
static void Si4432_PortInit(void)
{
	EXTI->IMR &= ~(nIRQ_PIN);
	GPIOx_Cfg(SDN_PORT, SDN_PIN, OUT_PP_2M);
    GPIOx_Cfg(nSEL_PORT, nSEL_PIN, OUT_PP_2M);
    GPIOx_Cfg(nIRQ_PORT, nIRQ_PIN, IN_FLOATING);
    GPIOx_Cfg(nGPIO2_PORT, nGPIO2_PIN, IN_FLOATING);
    GPIOx_Set(nIRQ_PORT, nIRQ_PIN);		//上拉
    GPIOx_Set(nGPIO2_PORT, nGPIO2_PIN);	//上拉
#if SIGNAL == 1
	GPIOx_Cfg(SIGNAL_PORT, SIGNAL_PIN, OUT_PP_2M);
	SIGNAL_OFF;
#endif
#if	RF_SWITCH == 1
	GPIOx_Cfg(RF_SWITCH_PORT, RF_SWITCH_PIN, OUT_PP_2M);
	RF_SWITCH_ON;
#endif
	
	SPI2_Init();
	SPI2_SetSpeed(4);		//32M/(1<<(n+1)) 	
}

/*******************************************************************************
函 数 名:  void Si4432_Init(void)
功能说明:  初始化SI4432，包括MCU管脚设置，将SI4432通信速率设置为9600bps
参    数:  无
返 回 值:  无
*******************************************************************************/
void Si4432_Init(void)
{
    Si4432_PortInit();
    nSEL(1);
    SDN(1);			//433模块寄存器清零
	delay_ms(2);
	SDN(0);
	delay_ms(50);
	SPI_RW_Reg(R_REG | 0x03, 0x00);	 //清RF模块中断	
	SPI_RW_Reg(R_REG | 0x04, 0x00);	 
	SPI_RW_Reg(W_REG | 0x07, 0x80);  //Si4432所有寄存器恢复初始值 
	delay_ms(2);
	SPI_RW_Reg(W_REG | 0x07, 0x00);  //Set sRFStandBy mode
	SPI_RW_Reg(W_REG | 0x05, 0x00);	 //Disenable all Interrupt1
	SPI_RW_Reg(W_REG | 0x06, 0x00);  //Disenable all Interrupt2
    SPI_RW_Reg(R_REG | 0x03, 0x00);	 
	SPI_RW_Reg(R_REG | 0x04, 0x00);	 //清RF模块中断	
	
	SPI_RW_Reg(W_REG | 0x09, Crystal_C);  //负载电容  Cint = 1.8 pF + 0.085 pF x xlc[6:0] + 3.7 pF x xtalshift
	SPI_RW_Reg(W_REG | 0x0a, 0x06);  //GPIOclock = 1M
	SPI_RW_Reg(W_REG | 0x0b, 0xea);  //GPIO 0 当做普通输出口
	SPI_RW_Reg(W_REG | 0x0c, 0xea);  //GPIO 1 当做普通输出口
	SPI_RW_Reg(W_REG | 0x0d, 0xfc);  //Clear Channel Assessment	
	SPI_RW_Reg(W_REG | 0x0e, 0x00);	 //设置GPIO管脚输出状态
	
	SPI_RW_Reg(W_REG | 0x1C, Si4432Cfg_Tab[0]);  //IFFilterBandwidth
	SPI_RW_Reg(W_REG | 0x1D, Si4432Cfg_Tab[1]);  //AFCLoopGearshiftOverride
	SPI_RW_Reg(W_REG | 0x1E, Si4432Cfg_Tab[2]);  //AFCTimingControl
	SPI_RW_Reg(W_REG | 0x1F, Si4432Cfg_Tab[3]);  //ClockRecoveryGearshiftOverride
	SPI_RW_Reg(W_REG | 0x20, Si4432Cfg_Tab[4]);  //ClockRecoveryOversamplingRatio
	SPI_RW_Reg(W_REG | 0x21, Si4432Cfg_Tab[5]);  //ClockRecoveryOffset2
	SPI_RW_Reg(W_REG | 0x22, Si4432Cfg_Tab[6]);  //ClockRecoveryOffset1
	SPI_RW_Reg(W_REG | 0x23, Si4432Cfg_Tab[7]);  //ClockRecoveryOffset0
	SPI_RW_Reg(W_REG | 0x24, Si4432Cfg_Tab[8]);  //ClockRecoveryTimingLoopGain1
	SPI_RW_Reg(W_REG | 0x25, Si4432Cfg_Tab[9]);  //ClockRecoveryTimingLoopGain0
	SPI_RW_Reg(W_REG | 0x2A, Si4432Cfg_Tab[10]); //AFCLimiter
	
	//物理层数据包：Preamble| Syn Word | Tx Header| Packlength| Data |CRC
	SPI_RW_Reg(W_REG | 0x30, 0xAC);	 //CRC Data Only Enable CCITT
	SPI_RW_Reg(W_REG | 0x32, 0x00);  //No broadcast address enable / No Received Header check
	SPI_RW_Reg(W_REG | 0x33, 0x02);  //No TX/RX header / Synchronization Word 3 and 2
//	SPI_RW_Reg(W_REG | 0x32, 0xff);  //byte 0,1,2,3 作为头码
//	SPI_RW_Reg(W_REG | 0x33, 0x42);  //使用Header byte 0,1,2,3 为头码  /  同步字使用 3,2 

	SPI_RW_Reg(W_REG | 0x34, 16);	 //设置发送 报头Preamble  16*4bit = 64bit  = 8byte
	SPI_RW_Reg(W_REG | 0x35, 5<<3);  //设置接收 报头Preamble 检测数目,半字:   5*4bit=20bit=2.5byte

	//同步字为 0x2DD4
	SPI_RW_Reg(W_REG | 0x36, 0x2D);  //Synchronization Word 3   OK
	SPI_RW_Reg(W_REG | 0x37, 0xD4);  //Synchronization Word 2   OK
	SPI_RW_Reg(W_REG | 0x38, 0x00);  //Synchronization Word 1   NULL
	SPI_RW_Reg(W_REG | 0x39, 0x00);	 //Synchronization Word 0   NULL
	//头码长度已关闭 -- 此无效
	SPI_RW_Reg(W_REG | 0x3a, 'B');   //发射的头码为： “BEST"
	SPI_RW_Reg(W_REG | 0x3b, 'E');
	SPI_RW_Reg(W_REG | 0x3c, 'S');
	SPI_RW_Reg(W_REG | 0x3d, 'T');
	SPI_RW_Reg(W_REG | 0x3f, 'B');   //需要校验的头码为：”BEST"
	SPI_RW_Reg(W_REG | 0x40, 'E');
	SPI_RW_Reg(W_REG | 0x41, 'S');
	SPI_RW_Reg(W_REG | 0x42, 'T');

	SPI_RW_Reg(W_REG | 0x43, 0xff);  //头码1,2,3,4 的所有位都需要校验
	SPI_RW_Reg(W_REG | 0x44, 0xff);  // 
	SPI_RW_Reg(W_REG | 0x45, 0xff);  // 
	SPI_RW_Reg(W_REG | 0x46, 0xff);  // 
	
	SPI_RW_Reg(W_REG | 0x58, Si4432Cfg_Tab[11]);  //ChargepumpCurrentTrimming_Override
	SPI_RW_Reg(W_REG | 0x69, Si4432Cfg_Tab[12]);  //AGCOverride1
	SPI_RW_Reg(W_REG | 0x6E, Si4432Cfg_Tab[13]);  //TXDataRate1
	SPI_RW_Reg(W_REG | 0x6F, Si4432Cfg_Tab[14]);  //TXDataRate0
	SPI_RW_Reg(W_REG | 0x70, Si4432Cfg_Tab[15]);  //ModulationModeControl1
	SPI_RW_Reg(W_REG | 0x71, Si4432Cfg_Tab[16]);  //ModulationModeControl2
	SPI_RW_Reg(W_REG | 0x72, Si4432Cfg_Tab[17]);  //FrequencyDeviation	
	
	SPI_RW_Reg(W_REG | 0x75, CENTER_FREQ); //基准频率423MHZ
#ifdef SI4432_CH
	SPI_RW_Reg(W_REG | 0x79, (SI4432_CH-73)*10);
#else
	SPI_RW_Reg(W_REG | 0x79, (Channel_Get()-73)*10);  //频率设置 434
#endif
	SPI_RW_Reg(W_REG | 0x76, Si4432Cfg_Tab[19]);  //NominalCarrierFrequency1
	SPI_RW_Reg(W_REG | 0x77, Si4432Cfg_Tab[20]);  //NominalCarrierFrequency0

	SPI_RW_Reg(W_REG | 0x73, 0x00);  //FrequencyOffset1
	SPI_RW_Reg(W_REG | 0x74, 0x00);  //FrequencyOffset2
	SPI_RW_Reg(W_REG | 0x7A, 0x0A);  //FrequencyHoppingStepSize 100KHZ
	SPI_RW_Reg(W_REG | 0x6D, 0x07);	 //发送功率（0～7） --max
#ifdef SI4432_THRESHOLD 	
	SPI_RW_Reg(W_REG | 0x27, SI4432_THRESHOLD);    //检测通道是否忙的信号强度门槛	
#else
	SPI_RW_Reg(W_REG | 0x27, Threshold);
	Thread_Login(RESULT, 0, 400, &Si4432_Threshold);	//重新计算信号门槛
#endif

    SPI_RW_Reg(W_REG | 0x7c, 30);   //TX FIFO Almost Full Threshold   = 30
	SPI_RW_Reg(W_REG | 0x7d, 12);   //TX FIFO Almost empty Threshold  = 12
    SPI_RW_Reg(W_REG | 0x7E, 50);   //RX FIFO Almost Full Threshold   = 50

	Ex_NVIC_Config(nIRQ_PORT, 10, FTIR); 	//下降沿触发 PB10 - nIRQ
	MY_NVIC_Init(2, 0, EXTI15_10_IRQn, 2);	//抢占2，子优先级0，组2	  
	EXTI->IMR &= ~(nIRQ_PIN);

	Si4432_Idle();
	SetMode_RX();
	SI4432_SendEn();
}

/*******************************************************************************
函 数 名:  Si4432_Idle
功能说明:  设置433模块进入空闲状态
参    数:  无
返 回 值:  无
*******************************************************************************/
void Si4432_Idle(void)
{
	EXTI->IMR &= ~(nIRQ_PIN);
	SPI_RW_Reg(W_REG | 0x0e, 0x00);				//关闭天线切换开关   GPIO0=0,GPIO1=0
	SPI_RW_Reg(W_REG | 0x07, 0x01); 			//Ready mode(Xtal is on)
	SPI_RW_Reg(W_REG | 0x05, 0x00);	 			//清中断
	SPI_RW_Reg(W_REG | 0x06, 0x00);
	SPI_RW_Reg(R_REG | 0x03,0x00);	
	SPI_RW_Reg(R_REG | 0x04,0x00);
	delay_ms(2);
}

/*******************************************************************************
函 数 名:  SetMode_RX
功能说明:  设置433模块进入接收状态
参    数:  无
返 回 值:  无
*******************************************************************************/
static void SetMode_RX(void)
{
	SPI_RW_Reg(W_REG | 0x07, 0x05); //RXon=1  Xton=1  开启接收模式
	SPI_RW_Reg(W_REG | 0x0e, 0x02); //切换接收  GPIO0=0,GPIO1=1
	SPI_RW_Reg(W_REG | 0x08, 0x02); 
	SPI_RW_Reg(W_REG | 0x08, 0x00); //Clear RX FIFO			
	SPI_RW_Reg(W_REG | 0x05, 0x13);	//Enable RX FIFO Almost Full/Valid Packet Received/CRC Error
	SPI_RW_Reg(W_REG | 0x06, 0x80); //Enable Sync Word Detected.
	SPI_RW_Reg(R_REG | 0x03, 0x00);	
	SPI_RW_Reg(R_REG | 0x04, 0x00); //Clear IRQ	flag
	EXTI->IMR |= (nIRQ_PIN);
}

/*******************************************************************************
函 数 名:  SetMode_TX
功能说明:  设置433模块进入发送状态
参    数:  无
返 回 值:  无
*******************************************************************************/
static void SetMode_TX(void)
{	
	SPI_RW_Reg(W_REG | 0x08, 0x03); 
	SPI_RW_Reg(W_REG | 0x08, 0x08); //Clear TX FIFO and set auto send
	SPI_RW_Reg(W_REG | 0x0e, 0x01); //切换发送  GPIO0=1,GPIO1=0
}

/*******************************************************************************
函 数 名:  SI4432_SendData
功能说明:  数据发送
参    数:  *cptBuf    ：数据缓冲区指针
            sendLength：要发送的数据长度 最大MAX_PAGE(0xf3)字节长度
            uiOvertime ：发送超时时间设定值
返 回 值:   无
*******************************************************************************/
static void SI4432_Send_All_Data(u8 *cptBuf, u8 sendLength, u16 uiOvertime)
{
	u32 i, t = 0; 

	Si4432_Idle();
	SetMode_TX(); 						
#if SIGNAL == 1
	Si4332_Signal_Insturct(TX);
#endif 
    SPI_RW_Reg(W_REG | 0x3E, sendLength); //写入发送数据长度
	while (sendLength) //等待数据发送完全成
	{
		for (t = OVERTIME; (!(SPI_RW_Reg(R_REG | 0x03,0x00)&0x20)) && t; t--) //等待TX FIFO 快为空
		{
			delay_us(20);
		}
		/* 433FIFo大小为64个字节。当发送数据大于64时，后面的数据将覆盖前面数据
			虽然启用FIFO快满自动发送，但测试结果是后面数据还是会覆盖前面数据 */
		for (i = 0; (i < 50) && sendLength; i++)//
		{
	    	SPI_RW_Reg(W_REG | 0x7F, *cptBuf++);//要发送的数据写入FIFO
		   	sendLength--;
		}
	}
	SPI_RW_Reg(W_REG | 0x07, 0x09); //发送数据
	delay_us(400);                              
	for (t = OVERTIME; (!(SPI_RW_Reg(R_REG | 0x03,0x00)&0x04))&&t; )//等待数据发送完毕
	{
		if (!(--t))
		{
			Si4432_Fault( SI4432_TX_ERR);
			break;
		}
	}
	Si4432_Idle();
	SetMode_RX();      //进入接收状态
}

/*******************************************************************************
函 数 名:   RecDataPacket
功能说明:   接收数据放
参    数:   *cptBuf    ：数据缓冲区指针
            cLength    ：要发送的数据长度 最大255字节长度   
返 回 值:  	TRUE（数据接收OK)/FALSE
*******************************************************************************/
static void RecDataToFifo(FIFO_t *fifo)
{
	u8 resVal1, resVal2, i = 0,fifo_success = TRUE; //接收队列溢出标志
	static u8 seat = 0;
	
	while (!nIRQ)
	{ 
		if (i++ > 5)	//模块数据读取错误
		{
			Si4432_Fault( SI4432_RX_ERR);//数据接收错误	
			seat = 0;
			break;
		}
		resVal1 = SPI_RW_Reg(R_REG | 0x03,0x00);
		resVal2 = SPI_RW_Reg(R_REG | 0x04,0x00);  //读中断寄存器2
	
		if (resVal2&0x80)//核对中断状态寄存器2
		{
			SI4432_RSSI = SPI_RW_Reg(R_REG | 0x26, 0x00);
		}
		/*核对中断状态寄存器1的值*/
		if ((resVal1&0x01))  	//CRC Error
		{
			seat = 0;
//			Fault_Upload(FAULT_2, SI4432_CRC_ERR, NULL); 2014.11.13 Unarty 屏蔽，此异常记录已失去意义	
			break;
		}	
		if ((resVal1&0x10)) //RX FIFO almost full interrupt occured
		{
			u8 cLen;
			for (cLen = 0; cLen < 50; cLen++)//read 32bytes from the FIFO				
			{
				fifo_putc(fifo, SPI_RW_Reg(R_REG|0x7F, 0x00));
			}
			if (seat > (MAX_PAGE - 50))
			{
				seat = 0;
				break;
			}
			else
			{
				seat += 50;
			}
		}   //end  if((resVal&0x10)==0x10)
		
		if ((resVal1&0x02)) //packet received interrupt occured
		{
			if(TestRecFlag == 0)//中断时接收RSSI信号值保存，2015.6.11新的方法
			{
				resVal1 = SPI_RW_Reg(R_REG|0x4B,0x00);//读取接收包的长度
				for ( ; seat < resVal1; seat++)				
				{
					fifo_success = fifo_putc(fifo, SPI_RW_Reg(R_REG|0x7F, 0x00));					
				}
			}
			else
			{
				u8 data[64];
				Communication_t *pMsg = (Communication_t*)&data[HEAD_SIZE];
				UnitPara_t *pData =(UnitPara_t*)&pMsg->para.unit; //增加通讯结构体解析 yanhuan adding 2015/12/02
				
				if((resVal1 = SPI_RW_Reg(R_REG|0x4B,0x00)) < 50)//读取接收包的长度
				{
						for (seat = 0 ; seat < resVal1; seat++)				
						{
							data[seat] = SPI_RW_Reg(R_REG|0x7F, 0x00);
						}
						if(pMsg->para.cmd == 0xB2 && pData->data[0] == 0xBB \
							&& (Get_Sign433_MasterAddr()  == pMsg->aim.sourceID))
						{                                  
							if( pData->data[1]< MAX_FRAME_NUM)
							 TestRecRSSI[pData->data[1]] = SI4432_RSSI;
						}
						if(Get_Sign433_MasterAddr() != pMsg->aim.sourceID)
						{
							u8 temp = Get_Sign433_TestPackNum();
							TestRecRSSI[temp]++;  //NUM包测试数据，第NUM包存放干扰包数量
						}
						fifo_success = fifo_puts(fifo, data, resVal1);
				}
			}
			seat = 0;
			if(fifo_success == FALSE) //放入队列失败 yanhuan adding 2015/10/10
				Fault_Upload(FAULT_2 , RECV_433_FIFO_OVER , NULL);//暂时不做处理
		}//end if((resVal&0x02)==0x02)
	} //end if(nIRQ == 0)
	if (!seat)	//数据接收完毕
	{
		SetMode_RX();
	}
}

/*******************************************************************************
函 数 名:  void EXTI15_10_IRQHandler(void)
功能说明:  433模块中断事件产生
参    数:  无
返 回 值:  无
*******************************************************************************/
void EXTI15_10_IRQHandler(void)
{
	RecDataToFifo(&rcv433fifo);
#if SIGNAL == 1
	Si4332_Signal_Insturct(RX);
#endif
	EXTI->PR = 1<<10;  //清除LINE0上的中断标志位  
}    

/*******************************************************************************
函 数 名:   SI4432_SendEn
功能说明:   无线发送使能
参    数:   无
返 回 值:  	无
注    意：	修改全局变量SI4432SendEn = TRUE: 2014.10.08 Unarty Add
*******************************************************************************/
void SI4432_SendEn(void)
{
	SI4432SendEn = TRUE;
}

/*******************************************************************************
函 数 名:   SI4432_Send_Buf_AT_SpareTime
功能说明:   检测433M信道空闲状态，设定时间内出现空闲将数据发送出去,每间隔5ms检测一次
参    数:   u8 *cptBuf - 将要发送数据包指针
            u8 cLength - 将要发送数据包长度
		    u16 uiOvertime - 将要发送数据最大延时  单位：秒
		    RxTxStatus_btst *btstFlag - 发送标志
返 回 值:  	0 - 在设定时间内，未发送成功
			1 - 在设定时间内，发送成功
*******************************************************************************/
u8 Si4432_Send(u8 *cptBuf, u8 cLength, u8 level)             
{
	if (SI4432SendEn == TRUE
		&& Check_433M_Busy(level))	//2014.10.08 Unarty Add
	{
		SI4432_Send_All_Data(cptBuf, cLength, 0);
		SI4432SendEn = FALSE;
		Thread_Login(ONCEDELAY, 0, SPACETIME, SI4432_SendEn);
		
		return TRUE;
	}
	
    return FALSE;
}

/*******************************************************************************
函 数 名:  u8 Check_433M_Busy(void)
功能说明:  检测433M信道空闲状态
参    数:  	level:	随机等待基数
返 回 值:  	1 - BUSY	忙碌
			0 - SPARE	空闲
*******************************************************************************/
static u8 Check_433M_Busy(u8 level)
{
	const static u16 randWait_Tab[] = {5, 5, 5, 5};
	static u16 busyTime = 0;		//连续检测忙的时间
	static u16 idleTime = 0;		//连续检测空闲时间		
	static u16 rand = 0;			//数据发送随机等待时间
	
	if (!rand)
	{
		if (level >= sizeof(randWait_Tab))
		{
			level = 1;
		}
		rand = (Time_Get()%randWait_Tab[level] + 2);
	}
	
	if(!nGPIO2)	//外界信号不忙	
	{
		if(!(--rand))		//连续空闲时间
		{
			if (++idleTime > IDLETIME)
			{
				idleTime = 0;
#ifndef SI4432_THRESHOLD
				Thread_Login(RESULT, 0, 400, &Si4432_Threshold);	//重新计算信号门槛
#else
				Si4432_Fault( SI4432_TX_ERR);
#endif
			}
			rand = 0;
			busyTime = 0;
			return 1;	
		}
	}
	else
	{
		rand = 0;
		idleTime = 0;
		if (++busyTime > BUSYTIME)
		{
			busyTime = 0;
#ifndef SI4432_THRESHOLD
			Thread_Login(RESULT, 0, 400, &Si4432_Threshold);	//重新计算信号门槛
#else
			Si4432_Fault( SI4432_TX_ERR);
#endif
		}
	}
		
	return 0;
}

#ifndef SI4432_THRESHOLD 

/*******************************************************************************
函 数 名:  	Si4432_Threshold
功能说明: 	Si4332_模块动态门槛设置
参    数:  	void
返 回 值:  	0（门槛设置完成）/1（门槛设置中）
*******************************************************************************/
static u8 Si4432_Threshold(void)
{
	static u8 buf[4] = {0};
	static u32 time = 0;	//计算时间值
	static u32 calculTime = 0;	 //上次计算时间
	u8 i;
	
	i = SPI_RW_Reg(R_REG | 0x26, 0x00);
	if (ABS(i , buf[0]) < 8)	//如果当前采样值较正常
	{
		i >>= 1;
		buf[0] = (buf[0] >> 1) + i;			//统计平均值
		if (++buf[3] > 20)		//计算统计完成
		{
			if (buf[0] < 10|| buf[0] > 220)	//取值平均异常
			{
				Si4432_Fault( SI4432_TX_ERR);
			}
			else
			{
				Threshold = buf[0] + 20;
  				SPI_RW_Reg(W_REG | 0x27, Threshold);    //检测通道是否忙的信号强度门槛	
				if (0 != calculTime)	//不是第一次计算
				{
					if (ABS(calculTime, Time_Get()) < 60000*7)	//两次计算时间间隔没有超过5分钟
					{
						Si4432_Fault( SI4432_TX_ERR);	//重置无线模块
						calculTime = 0;	
					}
					else
					{
						calculTime = Time_Get();	
					}
				}
				else
				{
					calculTime = Time_Get();	
				}	
			}
			time = 0;
			     
			return 0;
		}
	}
	else if (ABS(i, buf[1]) < 50)	//如果与异常状态值相近
	{
		if (++buf[2] > 4)
		{
			buf[0] = i;					//将新采样值入队
			buf[3] = 0;					//统计次数清零
			buf[2] = 0;
		}
	}
	else
	{
		buf[1] = i;
		buf[2] = 0;
	}
	
	if (++time > 600)	//每400ms进来一次， 总计算时间不能超过2分钟
	{
		time = 0;
		Si4432_Fault( SI4432_TX_ERR);
		
		return 0;
	}
	
	return 1;
}

#endif		

/*******************************************************************************
函 数 名:  	Si4432_Fault
功能说明: 	Si4332异常
参    数:  	event:	异常原因
返 回 值:  	无
*******************************************************************************/
static void Si4432_Fault(u8 event)
{
	Fault_Upload(FAULT_2, (Fault_t)event, &Si4432_Init);
}
	
/**************************Copyright BestFu 2014-05-14*************************/
