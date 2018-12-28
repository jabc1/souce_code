/***************************Copyright BestFu 2014-05-14*************************
文件:	Polling.c
说明:	RS485轮询
编译:	Keil uVision4 V4.72.0.0
版本:	v1.0
编写:	Unarty
日期:	2015.09.07
修改:	暂无
*******************************************************************************/
#include "Polling.h"
#include "CRC16.h"
#include "AirUnit.h"
#include "DeviceRelate.h"
#include "usart.h"
#include "config.h"
#include "test.h"
#include "KeyUnit.h"
/******************************************************************************/
volatile u8 Device_type;
airlib_t air_config;
CMD_fram_t send_data_cmd;
u8 Send_Protocol_Data[5][SEND_PROTOCOL_DATA_LEN];
u8 Send_Protocol_Cnt[5];
u8 Send_Communication_Data[SEND_DATA_LEN];
u8 Recv_Communication_Data[RECV_DATA_LEN];
u8 Air_Indoor_Addr;
u8 list_order = 0;
volatile u8 Send_Data_Flag = 0;//有无数据发送标志
static volatile u8 Poll_Dev_Addr = 0;	//被轮询的地址
AirStatus_t gAirStatus_bak = {0x0F,0x0,0x0F,0x0F,0x0F,16,35};
Air_Para_t gAirConfigPara;
volatile u8 Recv_Finish_Flag = 0;
volatile u32 UPST = 0;
volatile u32 Gateway_UPST = 0;
volatile u32 Recv_Data_UPST = 0;
volatile u32 Send_Data_UPST = 0;
volatile u32 Uart_Send_Data_UPST = 0;
volatile u32 GW_Recv_Data_UPST = 0;
volatile u32 Send_Data_Flag_UPST = 0;
/******************************************************************************/
/*******************************************************************************
函 数 名:	RS485_Gateway
功能说明: 485网关轮询设备，工作过程
参		数: 无
返 回 值:	无
*******************************************************************************/
void RS485_Gateway(void)
{
	u8 data[5] = {0};
//	static u32 time;
	static volatile u8 Gateway_status = 0, Send_Time_Cnt = 0;
	switch(Gateway_status)
	{
		case 0:				//读取地址列表，轮询地址,询问面板是否有数据发送
		{
			list_order %= POLL_ADDR_LEN;
			Poll_Dev_Addr = gAirConfigPara.Poll_Addr[list_order];
			if(0 != Poll_Dev_Addr)
			{
				data[0] = RS485_POLL_ASK;
				Send_Device_CMD(Poll_Dev_Addr, 0x03, 1, data);
				Send_Time_Cnt++;
				Gateway_status++;
			}
			else
			{
				list_order = Next_addr(list_order);
			}
		}
			break;
		case 1:				//等待面板返回信息，超时或询问3次，无应答或无消息发送将返回
			if((Recv_Finish_Flag & 0x01) && (Send_Time_Cnt <= POLL_TIME))
			{	
				Poll_t recv_poll_data;
				memcpy(&recv_poll_data, Recv_Communication_Data,6);
				Clr_Memery(Recv_Communication_Data, 6);
				if((Poll_Dev_Addr == recv_poll_data.Addr) && (0x03 == recv_poll_data.Cmd) && (0x01 == recv_poll_data.len) && Check_Recv_Data(recv_poll_data))
				{
					if(RS485_Data_Ready == recv_poll_data.data)
					{
						Gateway_status++;
					}
					else if(RS485_Data_Nothing == recv_poll_data.data)
					{
						Gateway_status = 0;
						list_order = Next_addr(list_order);
					}
					else
					{
						Gateway_status = 0;
					}
				}
				else
				{
					Gateway_status = 0;
					list_order = Next_addr(list_order);
				}
				Send_Time_Cnt = 0;
				Recv_Finish_Flag &= 0xFE;
			}
			else
			{
				if(Send_Time_Cnt >= POLL_TIME)
				{
					Send_Time_Cnt = 0;
					list_order = Next_addr(list_order);
				}
				Gateway_status = 0;
			}
			
			break;
		case 2:				//允许面板发送信息
		{
			data[0] = RS485_Send_Allow;
			Send_Device_CMD(Poll_Dev_Addr,0x03,1,data);
			Gateway_status++;
			_SetTimeing_ms((u32*)&Gateway_UPST,8000);
		}
			break;
		case 3:				//等待发送完数据，计时，超时总线空闲，此面板不再发送消息，返回轮询下一面板
			if(Recv_Finish_Flag & 0x01)
			{
				Poll_t recv_poll_data;
				memcpy(&recv_poll_data, Recv_Communication_Data,6);
				Clr_Memery(Recv_Communication_Data, 6);
				if((Poll_Dev_Addr == recv_poll_data.Addr) && (0x03 == recv_poll_data.Cmd) && (0x01 == recv_poll_data.len) )//判断是否是发给自己
				{
					if((RS485_Send_Finish == recv_poll_data.data) && Check_Recv_Data(recv_poll_data))
					{
						Gateway_status = 0;
						list_order = Next_addr(list_order);
					}
				}
				Recv_Finish_Flag &= 0xFE;
			}
			if(0 == _CheckTime((u32 *)&Gateway_UPST))
			{
				Gateway_status = 0;
				list_order = Next_addr(list_order);
			}
			break;
		default:
		{
			Gateway_status = 0;
		}
			break;
	}
}
/*******************************************************************************
函 数 名:	Next_addr
功能说明: 下一扫描地址
参		数: 无
返 回 值:	无
*******************************************************************************/
u8 Next_addr(u8 order)
{
	u8 i = (order + 1) % POLL_ADDR_LEN;
	for(;i <= POLL_ADDR_LEN; )
	{
		if(gAirConfigPara.Poll_Addr[i % POLL_ADDR_LEN])
		{
			return (i % POLL_ADDR_LEN);
		}
		else
		{
			i++;
		}
	}
	return 0;
}
/*******************************************************************************
函 数 名:	MS_Device_Polling
功能说明: 主从模式下温控面板工作过程
参		数: 无
返 回 值:	无
*******************************************************************************/
void MS_Device_Polling(void)
{
	static volatile u8 MS_DevPol_Status = 0;
	switch(MS_DevPol_Status)
	{
		case 0:				//与网关通信
//			if((check_flag & 0x01) && (0 == _CheckTime((u32 *)&UPST)))
			if((Recv_Finish_Flag & 0x02) && (0 == (Recv_Finish_Flag & 0x10)))
			{
				Comm_gateway();
				Recv_Finish_Flag &= 0xFD;
//				check_flag &= 0xFE;
			}
			if(Recv_Finish_Flag & 0x10)
			{
				if((0 == air_config.device_set.Run_CMD[0]))
				{
					if (0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
					{
						Send_Data_To_RS485(Send_Protocol_Data[0], Send_Protocol_Cnt[0]);	//第1条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
						MS_DevPol_Status++;
						Recv_Finish_Flag &= 0xEF;
						Recv_Finish_Flag &= 0xFD;
					}
				}
				else
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xEF;
					Recv_Finish_Flag &= 0xFD;
				}
				_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
			}
			break;
		case 1:
			if((air_config.Recv_first_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
			{	
				if(Recv_Finish_Flag & 0x02)
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					//接收数据校验代码
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			
			break;
		case 2:				//接收空调返回的信息，无返回，超时，跳到下一步
			if((1 == air_config.device_set.Run_CMD[1]))
			{
				if (0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
				{
					Send_Data_To_RS485(Send_Protocol_Data[1], Send_Protocol_Cnt[1]);	//第2条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 3:
			if((air_config.Recv_second_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
			{	
				if(Recv_Finish_Flag & 0x02)
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					//接收数据校验代码
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			
			break;
		case 4:				//接收空调返回的信息，无返回，超时，跳到下一步
			if((2 == air_config.device_set.Run_CMD[2]) )
			{
				if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
				{
					Send_Data_To_RS485(Send_Protocol_Data[2], Send_Protocol_Cnt[2]);	//第3条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 5:
			if((air_config.Recv_third_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
			{
				if(Recv_Finish_Flag & 0x02)
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					//接收数据校验代码
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 6:				//接收空调返回的信息，无返回，超时，跳到下一步
			if((3 == air_config.device_set.Run_CMD[3]))
			{
				if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
				{
					Send_Data_To_RS485(Send_Protocol_Data[3], Send_Protocol_Cnt[3]);	//第4条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 7:
			if((air_config.Recv_forth_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
			{
				if(Recv_Finish_Flag & 0x02)
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					//接收数据校验代码
					
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 8:				//接收空调返回的信息，无返回，超时，跳到下一步
			if((4 == air_config.device_set.Run_CMD[4]))
			{
				if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
				{
					Send_Data_To_RS485(Send_Protocol_Data[4], Send_Protocol_Cnt[4]);	//第5条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 9:
			if((air_config.Recv_fifth_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
			{
				if(Recv_Finish_Flag & 0x02)
				{
					MS_DevPol_Status++;
					Recv_Finish_Flag &= 0xFD;
					//接收数据校验代码
					
					_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
				}
			}
			else
			{
				MS_DevPol_Status++;
			}
			break;
		case 10:				//接收空调返回的信息，无返回，超时，跳到下一步
			if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
			{
				u8 data[5] = {0};
				data[0] = RS485_Send_Finish;
				Send_Device_CMD(gAirConfigPara.Local_Addr, 0x03, 1, data);						//数据发送完成
				MS_DevPol_Status = 0;
//				_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
			}
//			else
//			{
//				MS_DevPol_Status = 0;
//			}
			Send_Data_Flag &= 0xFC;
			break;
		default:
			MS_DevPol_Status = 0;
			Send_Data_Flag &= 0xFC;
			Recv_Finish_Flag &= 0xFD;
			break;
	}
}
/*******************************************************************************
函 数 名:	Device_Polling_Send
功能说明: 无主从模式下温控面板发送数据
参		数: 无
返 回 值:	无
*******************************************************************************/
void Device_Polling_Send(void)
{
	static u8 Device_Polling_Status = 0;
	if(Send_Data_Flag & 0x01)
	{
		switch(Device_Polling_Status)
		{
			case 0:	
				if(0 == air_config.device_set.Run_CMD[0])
				{
					if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
					{
						Send_Data_To_RS485(Send_Protocol_Data[0], Send_Protocol_Cnt[0]);	//第1条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 1:
				if((air_config.Recv_first_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
				{
					if(Recv_Finish_Flag & 0x04)
					{
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 2:				//接收空调返回的信息，无返回，超时，跳到下一步
				if(1 == air_config.device_set.Run_CMD[1])
				{
					if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
					{
						Send_Data_To_RS485(Send_Protocol_Data[1], Send_Protocol_Cnt[1]);	//第2条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 3:
				if((air_config.Recv_second_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
				{	
					if(Recv_Finish_Flag & 0x04)
					{
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 4:				//接收空调返回的信息，无返回，超时，跳到下一步
				if(2 == air_config.device_set.Run_CMD[2])
				{
					if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
					{
						Send_Data_To_RS485(Send_Protocol_Data[2], Send_Protocol_Cnt[2]);	//第3条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 5:
				if((air_config.Recv_third_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
				{
					if(Recv_Finish_Flag & 0x04)
					{
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 6:				//接收空调返回的信息，无返回，超时，跳到下一步
				if(3 == air_config.device_set.Run_CMD[3])
				{
					if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
					{
						Send_Data_To_RS485(Send_Protocol_Data[3], Send_Protocol_Cnt[3]);	//第4条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 7:
				if((air_config.Recv_forth_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
				{
					if(Recv_Finish_Flag & 0x04)
					{
						Device_Polling_Status++;
						Recv_Finish_Flag &= 0xFB;
						_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
					}
				}
				else
				{
					Device_Polling_Status++;
				}
				break;
			case 8:				//接收空调返回的信息，无返回，超时，跳到下一步
					if(4 == air_config.device_set.Run_CMD[4])
					{
						if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
						{
							Send_Data_To_RS485(Send_Protocol_Data[4], Send_Protocol_Cnt[4]);	//第5条命令发送，所有数据或开关机，模式，温度，风速，扫风命令,最多5条命令
							Device_Polling_Status++;
							Recv_Finish_Flag &= 0xFB;
							_SetTimeing_ms((u32*)&Send_Data_UPST,SEND_DELAY);
						}
					}
					else
					{
						Device_Polling_Status++;
					}
				break;
			case 9:
				if((air_config.Recv_fifth_cmd.En) && (_CheckTime((u32 *)&Send_Data_UPST)))
				{
					if(Recv_Finish_Flag & 0x04)
					{
						Device_Polling_Status = 0;
						Recv_Finish_Flag &= 0xFB;
					}
				}
				else
				{
					Device_Polling_Status = 0;
				}
				Send_Data_Flag &= 0xFC;
				break;
			default:
				Device_Polling_Status = 0;
				Recv_Finish_Flag &= 0xFB;
				Send_Data_Flag &= 0xFC;
				break;
		}
	}
}
/*******************************************************************************
函 数 名:	Device_Polling
功能说明: 生成协议数据
参		数: 无
返 回 值:	无
*******************************************************************************/
void Device_Polling(void)
{
	if(0 == (Send_Data_Flag & 0x02))
	{
		Send_Data_Flag |= 0x02;
		_SetTimeing_ms((u32*)&Send_Data_Flag_UPST,2000);
//	air_config.first_cmd = AIR_EXPLAIN->first_cmd;
		if(gAirStatus_bak.power != gAirStatus.power)
		{
//		Send_Protocol_CMD(air_config.first_cmd, air_config.device_set.Run_CMD[0]);
			Select_CMD(air_config.device_set.Run_CMD[0]);
			gAirStatus_bak.power = gAirStatus.power;
		}
		if(gAirStatus_bak.mode != gAirStatus.mode)
		{
//		Send_Protocol_CMD(air_config.second_cmd, air_config.device_set.Run_CMD[1]);
			Select_CMD(air_config.device_set.Run_CMD[1]);
			gAirStatus_bak.mode = gAirStatus.mode;
		}
		if(gAirStatus_bak.temp != gAirStatus.temp)
		{
//		Send_Protocol_CMD(air_config.third_cmd , air_config.device_set.Run_CMD[2]);
			Select_CMD(air_config.device_set.Run_CMD[2]);
			gAirStatus_bak.temp = gAirStatus.temp;
		}
		if(gAirStatus_bak.speed != gAirStatus.speed)
		{
//		Send_Protocol_CMD(air_config.forth_cmd, air_config.device_set.Run_CMD[3]);
			Select_CMD(air_config.device_set.Run_CMD[3]);
			gAirStatus_bak.speed = gAirStatus.speed;
		}
		if(gAirStatus_bak.wind != gAirStatus.wind)
		{
//		Send_Protocol_CMD(air_config.forth_cmd, air_config.device_set.Run_CMD[4]);
			Select_CMD(air_config.device_set.Run_CMD[4]);
			gAirStatus_bak.wind = gAirStatus.wind;
		}
	}
	else
	{
		if(0 == _CheckTime((u32 *)&Send_Data_Flag_UPST))
		{
			Send_Data_Flag &= 0xFD;
		}
	}
}
/*******************************************************************************
函 数 名:	Select_CMD
功能说明: 选择执行程序
参		数: 无
返 回 值:	无
*******************************************************************************/
void Select_CMD(u8 num)
{
	switch(num)
	{
		case 0x00:
			Send_Protocol_CMD(air_config.first_cmd, num);
			break;
		case 0x01:
			Send_Protocol_CMD(air_config.second_cmd, num);
			break;
		case 0x02:
			Send_Protocol_CMD(air_config.third_cmd , num);
			break;
		case 0x03:
			Send_Protocol_CMD(air_config.forth_cmd, num);
			break;
		case 0x04:
			Send_Protocol_CMD(air_config.forth_cmd, num);
			break;
		default:
			break;
	}
}
/*******************************************************************************
函 数 名:	Comm_gateway
功能说明: 面板与485网关通信
参		数: 无
返 回 值:	无
*******************************************************************************/
void Comm_gateway(void)
{
	u8 data[5] = {0};
	static volatile u8 Comm_gateway_status = 0;
	switch(Comm_gateway_status)
	{
		case 0:					//接收到网关询问消息		
		{
			Poll_t recv_poll_data;
			memcpy(&recv_poll_data, Recv_Communication_Data,6);
			Clr_Memery(Recv_Communication_Data,6);
			if((gAirConfigPara.Local_Addr == recv_poll_data.Addr) && (0x03 == recv_poll_data.Cmd) && (0x01 == recv_poll_data.len))
			{
				if(RS485_POLL_ASK == recv_poll_data.data)
				{
					if(Send_Data_Flag & 0x01)
					{
						data[0] = RS485_Data_Ready;
						Comm_gateway_status++;
						_SetTimeing_ms((u32*)&GW_Recv_Data_UPST,2000);
					}
					else
					{
						data[0] = RS485_Data_Nothing;
						Comm_gateway_status = 0;
					}
					Send_Data_Flag |= 0x10;
					Send_Data_Flag &= 0xFC;
					Send_Device_CMD(gAirConfigPara.Local_Addr,0x03,1,data);
				}
			}
		}
			break;
		case 1:					//接收到允许发送消息
			if(_CheckTime((u32 *)&GW_Recv_Data_UPST))
			{
				Poll_t recv_poll_data;
				memcpy(&recv_poll_data, Recv_Communication_Data,6);
				Clr_Memery(Recv_Communication_Data,6);
				if((gAirConfigPara.Local_Addr == recv_poll_data.Addr) && (0x03 == recv_poll_data.Cmd) && (0x01 == recv_poll_data.len))//判断是否是发给自己
				{
					if(RS485_Send_Allow == recv_poll_data.data)
					{
						//发送空调命令数据
						Recv_Finish_Flag |= 0x10;
					}
					recv_poll_data.data = 0;
					Send_Data_Flag &= 0xEF;
					Comm_gateway_status = 0;
				}
			}
			else
			{
				Comm_gateway_status = 0;
			}
			break;
		case 0xFF:
			Comm_gateway_status = 0;
			break;
		default:
			Comm_gateway_status = 0;
			break;
	}
}
/*******************************************************************************
函 数 名:	Send_Protocol_CMD
功能说明: 生成发送数据到空调
参		数: 无
返 回 值:	无
*******************************************************************************/
void Send_Protocol_CMD(CMD_fram_t cmd, u8 num)
{
	u8 i,temp;
	u8 Send_Protocol_State,Send_Protocol_Num = 0;
	u16 Check;
	Send_Protocol_Cnt[num] = 0;
	Send_Protocol_State = cmd.order[Send_Protocol_Num];
	if(0 == air_config.device_set.Check_Mode)
	{
		Check = 0xFFFF;						//CRC16校验
	}
	else
	{
		Check = 0;								//累加和校验
	}
	while(Send_Protocol_Num < ORDER_NUM)
	{
		switch(Send_Protocol_State)
		{
			case 0:
				if(cmd.Soi.En & 0x01)							//帧头
				{
					for(i = 0; i < cmd.Soi.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Soi.CMD_data[i];
						if(cmd.Soi.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(cmd.Soi.CMD_data[i],Check);
							}
							else
							{
								Check += cmd.Soi.CMD_data[i];
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 1:
				if(cmd.Addr.En & 0x01)							//空调地址
				{
					for(i = 0; i < cmd.Addr.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = gBindObject.Addr;
						if(cmd.Addr.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(gBindObject.Addr,Check);
							}
							else
							{
								Check += gBindObject.Addr;
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 2:
				if(cmd.Cmd.En & 0x01)							//命令代码
				{
					for(i = 0; i < cmd.Cmd.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Cmd.CMD_data[i];
						if(cmd.Cmd.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(cmd.Cmd.CMD_data[i],Check);
							}
							else
							{
								Check += cmd.Cmd.CMD_data[i];
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 3:
				if(cmd.Addr_sta.En & 0x01)							//起始地址
				{
					for(i = 0; i < cmd.Addr_sta.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Addr_sta.CMD_data[i] + gAirConfigPara.Air_Indoor_Addr * gAirConfigPara.Air_Indoor_Addr_Len;
						if(cmd.Addr_sta.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(cmd.Addr_sta.CMD_data[i],Check);
							}
							else
							{
								Check += cmd.Addr_sta.CMD_data[i];
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 4:
				if(cmd.Addr_len.En & 0x01)							//寄存器数量
				{
					for(i = 0; i < cmd.Addr_len.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Addr_len.CMD_data[i];
						if(cmd.Addr_len.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(cmd.Addr_len.CMD_data[i],Check);
							}
							else
							{
								Check += cmd.Addr_len.CMD_data[i];
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 5:
				if(cmd.Fram_len.En & 0x01)							//数据大小
				{
					for(i = 0; i < cmd.Fram_len.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Fram_len.CMD_data[i];
						if(cmd.Fram_len.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(cmd.Fram_len.CMD_data[i],Check);
							}
							else
							{
								Check += cmd.Fram_len.CMD_data[i];
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 6:
				if(cmd.On_Off_Data.En & 0x01)							//开关机数据
				{
					if(cmd.On_Off_Data.Lenth <= 2)
					{
						for(i = 0; i < cmd.On_Off_Data.Lenth; i++)
						{
							if(1 == gAirStatus.power)
							{
								temp = cmd.On_Off_Data.CMD_data[i+2];
							}
							else
							{
								temp = cmd.On_Off_Data.CMD_data[i];
							}
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = temp;
							if(cmd.On_Off_Data.En & 0x02)
							{
								if(0 == air_config.device_set.Check_Mode)
								{
									Check = GetCRC_IBM_SDLC_oneByte(temp,Check);
								}
								else
								{
									Check += temp;
								}
							}
							Send_Protocol_Cnt[num]++;
						}
					}
				}
				break;
			case 7:
				if(cmd.Mode_Data.En & 0x01)							//空调运行模式
				{
					if(cmd.Mode_Data.Lenth <= 2)
					{
						for(i = 0; i < cmd.Mode_Data.Lenth; i++)
						{
							if(1 == gAirStatus.mode)
							{
								temp = cmd.Mode_Data.CMD_data[i+2];
							}
							else if(2 == gAirStatus.mode)
							{
								temp = cmd.Mode_Data.CMD_data[i+4];
							}
							else if(3 == gAirStatus.mode)
							{
								temp = cmd.Mode_Data.CMD_data[i+6];
							}
							else if(4 == gAirStatus.mode)
							{
								temp = cmd.Mode_Data.CMD_data[i+8];
							}
							else
							{
								temp = cmd.Mode_Data.CMD_data[i];
							}
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = temp;
							if(cmd.Mode_Data.En & 0x02)
							{
								if(0 == air_config.device_set.Check_Mode)
								{
									Check = GetCRC_IBM_SDLC_oneByte(temp,Check);
								}
								else
								{
									Check += temp;
								}
							}
							Send_Protocol_Cnt[num]++;
						}
					}
				}
				break;
			case 8:
				if(cmd.Temperatue_Data.En & 0x01)							//空调设置温度
				{
					temp = gAirStatus.temp;
					for(i = 0; i < cmd.Temperatue_Data.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = temp;
						if(cmd.Temperatue_Data.En & 0x02)
						{
							if(0 == air_config.device_set.Check_Mode)
							{
								Check = GetCRC_IBM_SDLC_oneByte(temp,Check);
							}
							else
							{
								Check += temp;
							}
						}
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			case 9:
				if(cmd.Wind_Speed_Data.En & 0x01)							//空调风速设置
				{
					if(cmd.Wind_Speed_Data.Lenth <= 2)
					{
						for(i = 0; i < cmd.Wind_Speed_Data.Lenth; i++)
						{
							if(1 == gAirStatus.speed)
							{
								temp = cmd.Wind_Speed_Data.CMD_data[i+2];
							}
							else if(2 == gAirStatus.speed)
							{
								temp = cmd.Wind_Speed_Data.CMD_data[i+4];
							}
							else if(3 == gAirStatus.speed)
							{
								temp = cmd.Wind_Speed_Data.CMD_data[i+6];
							}
							else
							{
								temp = cmd.Wind_Speed_Data.CMD_data[i];
							}
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = temp;
							if(cmd.Wind_Speed_Data.En & 0x02)
							{
								if(0 == air_config.device_set.Check_Mode)
								{
									Check = GetCRC_IBM_SDLC_oneByte(temp,Check);
								}
								else
								{
									Check += temp;
								}
							}
							Send_Protocol_Cnt[num]++;
						}
					}
				}
				break;
			case 10:
				if(cmd.Wind_Data.En & 0x01)							//扫风数据
				{
					if(cmd.Wind_Data.Lenth <= 2)
					{
						for(i = 0; i < cmd.Wind_Data.Lenth; i++)
						{
							if(1 == gAirStatus.wind)
							{
								temp = cmd.Wind_Data.CMD_data[i+2];
							}
							else
							{
								temp = cmd.Wind_Data.CMD_data[i];
							}
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = temp;
							if(cmd.Wind_Data.En & 0x02)
							{
								if(0 == air_config.device_set.Check_Mode)
								{
									Check = GetCRC_IBM_SDLC_oneByte(temp,Check);
								}
								else
								{
									Check += temp;
								}
							}
							Send_Protocol_Cnt[num]++;
						}
					}
				}
				break;
			case 11:
				if(cmd.Check.En & 0x01)							//校验
				{
					if(0 == air_config.device_set.Check_Mode)
					{
						if(0 == air_config.device_set.CRC_Mode)
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)Check;
						}
						else
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)(Check >> 8);
						}
						Send_Protocol_Cnt[num]++;
						if(0 == air_config.device_set.CRC_Mode)
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)(Check >> 8);
						}
						else
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)Check;
						}
						Send_Protocol_Cnt[num]++;				
					}
					else
					{
						if(0 == air_config.device_set.CRC_Mode)
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)Check;
						}
						else
						{
							Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)(Check >> 8);
						}
						Send_Protocol_Cnt[num]++;
						if(2 == cmd.Check.Lenth)
						{
							if(0 == air_config.device_set.CRC_Mode)
							{
								Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)(Check >> 8);
							}
							else
							{
								Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = (u8)Check;
							}
							Send_Protocol_Cnt[num]++;	
						}				
					}
				}
				break;
			case 12:
				if(cmd.Eoi.En & 0x01)							//结束标志
				{
					for(i = 0; i < cmd.Eoi.Lenth; i++)
					{
						Send_Protocol_Data[num][Send_Protocol_Cnt[num]] = cmd.Eoi.CMD_data[i];
						Send_Protocol_Cnt[num]++;
					}
				}
				break;
			default:
				break;
		}
		Send_Protocol_Num++;
		Send_Protocol_State = cmd.order[Send_Protocol_Num];
	}
	Send_Data_Flag |= 0x01;
}
/*******************************************************************************
函 数 名:	Send_Device_CMD
功能说明: 网关与面板通信
参		数: 无
返 回 值:	无
*******************************************************************************/
void Send_Device_CMD(u8 addr,u8 cmd,u8 len,u8* data)
{
	u16 Check = 0xFFFF;
	u8 i = 0,j;
	Send_Communication_Data[i++] = addr;						//设备地址
	Check = GetCRC_IBM_SDLC_oneByte(addr,Check);
	Send_Communication_Data[i++] = cmd;							//命令代码
	Check = GetCRC_IBM_SDLC_oneByte(cmd,Check);
	Send_Communication_Data[i++] = len;							//数据大小
	Check = GetCRC_IBM_SDLC_oneByte(len,Check);
	for(j = 0; j < len; j++)
	{
		Send_Communication_Data[i++] = data[j];				//数据
		Check = GetCRC_IBM_SDLC_oneByte(data[j],Check);
	}
	if(0 == air_config.device_set.CRC_Mode)
	{
		Send_Communication_Data[i++] = (u8)Check;
	}
	else
	{
		Send_Communication_Data[i++] = (u8)(Check >> 8);
	}
	if(0 == air_config.device_set.CRC_Mode)
	{
		Send_Communication_Data[i++] = (u8)(Check >> 8);
	}
	else
	{
		Send_Communication_Data[i++] = (u8)Check;
	}
	
	Send_Data_To_RS485(Send_Communication_Data, i);//发送数据
}
/*******************************************************************************
函 数 名:	Recv_Device_Ack
功能说明: 验证空调应答数据
参		数: 无
返 回 值:	无
*******************************************************************************/
void Recv_Device_Ack(u8* data)
{
}
/*******************************************************************************
函 数 名:	Send_Data_To_RS485
功能说明: 发送数据到485总线
参		数: 无
返 回 值:	无
*******************************************************************************/
void Send_Data_To_RS485(u8 *data, u8 len)
{
//	if(0 == _CheckTime((u32 *)&Uart_Send_Data_UPST))
	{
		UART_Send(data, len, 0);
		_SetTimeing_ms((u32*)&Uart_Send_Data_UPST, 20);
	}
							//延时10ms或发送前检测上次发送结束已间隔10ms以上
}
/*******************************************************************************
函 数 名:	Check_Recv_Data
功能说明: 检测接收到的数据的正确性
参		数: 无
返 回 值:	无
*******************************************************************************/
u8 Check_Recv_Data(Poll_t data)
{
	unsigned short CRC_Temp = 0xFFFF;
	CRC_Temp = GetCRC_IBM_SDLC_oneByte(data.Addr, CRC_Temp);
	CRC_Temp = GetCRC_IBM_SDLC_oneByte(data.Cmd, CRC_Temp);
	CRC_Temp = GetCRC_IBM_SDLC_oneByte(data.len, CRC_Temp);
	CRC_Temp = GetCRC_IBM_SDLC_oneByte(data.data, CRC_Temp);
	if(((CRC_Temp & 0x00ff) == (data.crc >> 8)) && ((CRC_Temp >> 8) == (data.crc & 0x00ff)))
	{
		return 1;
	}
	return 0;
}

/****************************************************************************
函 数 名：Config_Data_Init()
功能说明：配置数据初始化
参		数：无
返 回 值：无
****************************************************************************/
void Config_Data_Init(void)
{
	air_config.device_set = AIR_EXPLAIN->device_set;
	air_config.first_cmd = AIR_EXPLAIN->first_cmd;
	air_config.second_cmd = AIR_EXPLAIN->second_cmd;
	air_config.third_cmd = AIR_EXPLAIN->third_cmd;
	air_config.forth_cmd = AIR_EXPLAIN->forth_cmd;
	_SetTimeing_ms((u32*)&UPST, 10);
	_SetTimeing_ms((u32*)&Gateway_UPST, 10);
	_SetTimeing_ms((u32*)&Recv_Data_UPST, 10);
	_SetTimeing_ms((u32*)&Send_Data_UPST, 10);
	_SetTimeing_ms((u32*)&Uart_Send_Data_UPST, 10);
	_SetTimeing_ms((u32*)&GW_Recv_Data_UPST, 10);
	_SetTimeing_ms((u32*)&Send_Data_Flag_UPST, 10);
	_SetTimeing_ms((u32*)&EEprom_Save_UPST, 10);
}
/*****************************************************************************
名    称:  	AIR_LibNameGet
功能说明:  	空调库名获取
参		数:  	explain:	库存储地址
返 回 值:  	无
*****************************************************************************/
void AIR_LibNameGet(airlib_t const *explain, u8 *len, u8 *data)
{
	u8 i;
	memcharget(data, &i, explain->device_set.name, sizeof(explain->device_set.name));
	data[i] = '(';	//结束尾零改为括号，便于上位机识别
	*len = i;
	memcharget(&data[i], &i, explain->device_set.type, sizeof(explain->device_set.type));
	*len += i;
	data[*len] = ')';	//添加结束括号
}
/****************************************************************************
函 数 名：Clr_Memery()
功能说明：清除内存
参		数：无
返 回 值：无
****************************************************************************/
void Clr_Memery(u8 *data, u8 cnt)
{
	u8 i;
	for(i = 0; i < cnt; i++)
	{
		data[i] = 0;
	}
}
/****************************************************************************
函 数 名：_SetTimeing_ms()
功能说明：设置定时时间
参		数：T	定时器的代号
					time 定时时间,Xms
返 回 值：无
****************************************************************************/
void _SetTimeing_ms(u32 *T,unsigned int time)
{
	*T = (Time_Get()+time) % TIME_MAX;
}
/****************************************************************************
名    称：_CheckTime()
功能说明：查询是否超时
参		数：T	定时器的代号
返 回 值：0定时时间未到，1已超时
****************************************************************************/
int _CheckTime(u32 *T)
{
	if((*T) > Time_Get())
	{
		return 1;
	}
	return 0;
}

/**************************Copyright BestFu 2015-09-07*************************/

