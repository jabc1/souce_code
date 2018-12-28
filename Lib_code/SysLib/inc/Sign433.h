/***************************Copyright BestFu ***********************************
**  文    件：  Sign433.h
**  功    能：  433信号强度测试
**  编    译：  Keil uVision5 V4.72
**  版    本：  V1.1
**  编    写：  Seven
**  创建日期：  2014.10.08
**  修改日期：  2014.10.22
**  说    明：  >>
*******************************************************************************/
#ifndef _SIGN433_H_
#define _SIGN433_H_

#include "BF_type.H"
#define MAX_FRAME_NUM   100       //最大测试数据帧数目  100帧
typedef struct
{
    u8  Number;      //测试包数量
    u8  Length;      //测试包长度
    u16 TimeSpace;   //测试包时间间隔(ms)
}TestPkgInfo_t;      //433信号测试信息头

typedef struct
{
    u32  MasterID;                  //主机地址
    u32  SlaveID;                   //从机地址
    TestPkgInfo_t TestPkgInfo;      //433信号测试信息头
}Sign433Comm_t;                     //433信号测试通讯结构体


extern Sign433Comm_t gSign433Comm;
extern u8   RecRSSI[100];           //每帧信号强度值
extern u8   Sign433_State;          // FLASE 空闲   TURE 测试中

extern void Sign433_SendTestFrame(void);
extern void Sign433_SendTestFrameStream(void);
extern void ClearRecTestStatus(void);
extern void Sign433_SendTestReport(void);
//周旺添加，测试RSSI变量定义
extern void SetRecTestStatus(u8 flag);
extern u8 *GetRSSIPara(void);
//闫欢添加，设备通信门槛值获取
extern u8  GetThresholdPara(void);

#endif

/***************************Copyright BestFu **********************************/ 
