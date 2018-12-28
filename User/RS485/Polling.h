/***************************Copyright BestFu 2014-05-14*************************
文件:	Polling.h
说明:	RS485轮询
编译:	Keil uVision4 V4.72.0.0
版本:	v1.0
编写:	Unarty
日期:	2015.09.07
修改:	暂无
*******************************************************************************/
#ifndef __POLLING_H
#define __POLLING_H	

#include "BF_type.h"
#include "SysTick.h"
#include "AirUnit.h"
/******************************************************************************/
#define	SEND_DELAY							300		//发完一条命令再发下一条命令的时间，ms
#define	POLL_ADDR_LEN						20
#define	SEND_DATA_LEN						100
#define	RECV_DATA_LEN						100
#define SEND_PROTOCOL_DATA_LEN 	80
#define ORDER_NUM							 	13
#define	abst(a,b)								((a)>(b)?((a)-(b)):((a)+(TIME_MAX-(b))))

/******************************************************************************/
typedef struct
{
	u8 Air_type;								//面板工作类型 0:485网关 1:温控面板与网关协议工作 2:面板独立工作，直接控制空调
	u8 Local_Addr;							//面板本身的地址
	u8 Air_Indoor_Addr;					//室内机地址
	u8 Air_Indoor_Addr_Len;			//室内机地址大小
	u8 Poll_Addr[POLL_ADDR_LEN];//面板做为网关时，轮询地址列表
}Air_Para_t;


typedef struct
{
	u8 En;						//使能，即协议是否有这部分的数据
	u8 Lenth;					//数据长度，即这部分数据的长度
	u8 CMD_data[4];		//这部分的数据，即数据的固定部分，比如有些数据标志，变化的数据不用理会
}CMD_t;
typedef struct
{
	u8 En;						//使能，即协议是否有这部分的数据
	u8 Lenth;					//数据长度，即这部分数据的长度
	u8 CMD_data[10];	//这部分的数据，即数据的固定部分，比如有些数据标志，变化的数据不用理会
}CMD_Mode_t;
typedef struct
{
	u8 En;						//使能，即协议是否有这部分的数据
	u8 Lenth;					//数据长度								
}CMD_DATA_t;
typedef struct
{
	CMD_t Soi;													//帧头标志
	CMD_t Addr;													//空调地址
	CMD_t Cmd;													//功能代码
	CMD_t Addr_sta;											//室内机起始地址
	CMD_t Addr_len;											//室内机寄存器数量
	CMD_t Fram_len;											//数据大小
	CMD_t On_Off_Data;									//开关机数据
	CMD_Mode_t Mode_Data;								//空调模式数据
	CMD_DATA_t Temperatue_Data;					//温度数据
	CMD_Mode_t Wind_Speed_Data;					//风速数据
	CMD_t Wind_Data;										//扫风数据
	CMD_DATA_t Check;										//校验
	CMD_t Eoi;													//结束标志
	u8 order[ORDER_NUM];
}CMD_fram_t;
typedef struct
{
	u8 En;															//接收检测使能
	CMD_t Soi;													//帧头标志
	CMD_t Addr;													//空调地址
	CMD_t Cmd;													//功能代码
	CMD_t Addr_sta;											//室内机起始地址
	CMD_t Addr_len;											//室内机寄存器数量
	CMD_t Fram_len;											//数据大小
	CMD_t On_Off_Data;									//开关机数据
	CMD_Mode_t Mode_Data;								//空调模式数据
	CMD_DATA_t Temperatue_Data;					//温度数据
	CMD_Mode_t Wind_Speed_Data;					//风速数据
	CMD_t Wind_Data;										//扫风数据
	CMD_DATA_t Check;										//校验
	CMD_t Eoi;													//结束标志
	u8 order[ORDER_NUM];
}Recv_CMD_fram_t;
typedef struct
{
	u8 name[16];			//品牌名
	u8 type[16];			//型号
	u8 Check_Mode;		//校验模式	0：CRC16	1：累加和
	u8 CRC_Mode;			//CRC采用那种主机序	0：小端		1：大端
	u8 Run_CMD[5];		//开关机，模式，温度，风速，扫风执行那一条命令，总5条
}RS485Explain_t;
typedef struct
{
	RS485Explain_t device_set;				//485协议库信息
	CMD_fram_t first_cmd;							//第一条命令规则
	CMD_fram_t second_cmd;						//第二条命令规则					
	CMD_fram_t third_cmd;							//第三条命令规则
	CMD_fram_t forth_cmd;							//第四条命令规则
	CMD_fram_t fifth_cmd;							//第五条命令规则
	Recv_CMD_fram_t Recv_first_cmd;		//第一条接收命令规则
	Recv_CMD_fram_t Recv_second_cmd;	//第二条接收命令规则					
	Recv_CMD_fram_t Recv_third_cmd;		//第三条接收命令规则
	Recv_CMD_fram_t Recv_forth_cmd;		//第四条接收命令规则
	Recv_CMD_fram_t Recv_fifth_cmd;		//第五条接收命令规则
}airlib_t;

typedef struct
{
	u8 Addr;													//空调地址
	u8 Cmd;														//功能代码
	u8 len;														//数据大小
	u8 data;													//数据
	u16 crc;													//CRC校验
}Poll_t;

typedef enum
{
	RS485_POLL_ASK = 0x01,				//轮询询问是否有数据发送
	RS485_Data_Ready = 0x02,			//数据已准备好，可以发送
	RS485_Data_Nothing = 0x03,		//无数据可以发送
	RS485_Send_Allow = 0x04,			//允许发送数据
	RS485_Send_Finish = 0x05,			//数据发送完成
	RS485_RESERVE = 0xFF,					//保留
}CommMeaning_t;									//网关与面板通信协议数据

typedef enum
{
	Air_RS485_Gateway = 0x00,			//RS485网关
	Air_Slave = 0x01,							//温控面板，与网关一起协调工作
	Air_Control = 0x02,						//温控面板，独立工作，直接控制空调
	Air_RESERVE = 0xFF,						//保留
}Air_type_t;	
/******************************************************************************/
extern volatile u8 Device_type;
extern airlib_t air_config;
extern CMD_fram_t send_data_cmd;
extern u8 Send_Protocol_Data[5][SEND_PROTOCOL_DATA_LEN];
extern u8 Send_Protocol_Cnt[5];
extern u8 Send_Communication_Data[SEND_DATA_LEN];
extern u8 Recv_Communication_Data[RECV_DATA_LEN];
extern u8 list_order;
extern volatile u8 Send_Data_Flag;//有无数据发送标志
extern AirStatus_t gAirStatus_bak;
extern Air_Para_t gAirConfigPara;
extern volatile u8 Recv_Finish_Flag;
extern u8 Air_Indoor_Addr;
extern volatile u32 UPST;
extern volatile u32 Send_Data_UPST;
extern volatile u32 GW_Recv_Data_UPST;
/******************************************************************************/
extern void RS485_Gateway(void);
extern void Device_Polling(void);
extern void Comm_gateway(void);
extern void Send_Protocol_CMD(CMD_fram_t cmd, u8 num);
extern void Send_Device_CMD(u8 addr,u8 cmd,u8 len,u8* data);
extern void Send_Data_To_RS485(u8 *data, u8 len);
extern void MS_Device_Polling(void);
extern void Recv_Device_Ack(u8* data);
extern void Device_Polling_Send(void);
extern void _SetTimeing_ms(u32 *T,unsigned int time);
extern int _CheckTime(u32 *T);
extern u8 Check_Recv_Data(Poll_t data);
extern void Config_Data_Init(void);
extern void Clr_Memery(u8 *data, u8 cnt);
extern u8 Next_addr(u8 order);
extern void Select_CMD(u8 num);
extern void AIR_LibNameGet(airlib_t const *explain, u8 *len, u8 *data);
#endif

/**************************Copyright BestFu 2015-09-07*************************/
