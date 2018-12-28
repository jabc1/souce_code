/***************************Copyright BestFu ***********************************
**  文    件：  Sign433.c
**  功    能：  设备433信号强度测试
**  编    译：  Keil uVision5 V4.72
**  版    本：  V1.1
**  编    写：  Seven
**  创建日期：  2014.10.08
**  修改日期：  2014.10.28
**  说    明：  >>V1.1  防止低功耗设备测试过程中休眠
*******************************************************************************/
#include "Sign433.h"
#include "Thread.h"
#include "MsgPackage.h"
#include "SI4432.h"

#ifdef STM32L151
extern  void StandbyCountReset(void);
#endif

#define MASTER_REC_FRAME_STREAM 0XB5        //主机接收测试数据流
#define MASTER_REC_TEST_REPORT  0XB6        //主机接收测试报告

Sign433Comm_t gSign433Comm;     //433信号测试信息结构体
u8 Sign433_State=0;             // 0 空闲   1 发送测试   2接收测试
static u8 SendCount=0;          //发送计数

/*******************************************************************************
**函    数： Sign433_SendTestFrame()
**功    能： 发送测试数据帧
********************************************************************************/
void Sign433_SendTestFrame(void)
{
    u8 pData[255];   //测试数据随机 
    u8 len = gSign433Comm.TestPkgInfo.Length;    
    
#if STM32L151		//专用于低功耗产品，休眠时间重置
			StandbyCountReset();
#endif 
     
    pData[0] = 0XBB;            //测试帧包
    pData[1] = SendCount++;
    
    if(SendCount > gSign433Comm.TestPkgInfo.Number)        //结束帧包
    {
        pData[0] = 0xFF; 
        len = 2;
    }
    //目录类型/组/场景          目标ID       组、场景号   单元号   属性号   数据长度   数据  
    //(ObjectType_t objecttype, u32 objectID, u16 actNum, u8 unit, u8 cmd, u8 len, u8 *data);
    Msg_Send( SINGLE_ACT , gSign433Comm.MasterID , 0 , 0x01 , MASTER_REC_FRAME_STREAM , len , pData);
}

/*******************************************************************************
**函    数： Sign433_SendTestFrameStream()
**功    能： 发送测试数据流
********************************************************************************/
void Sign433_SendTestFrameStream(void)
{
    SendCount = 0;
    Thread_Login(MANY, gSign433Comm.TestPkgInfo.Number+2,   //补发结束帧
                       gSign433Comm.TestPkgInfo.TimeSpace, &Sign433_SendTestFrame);  // 注册线程 按要求发送测试数据包
}
/*******************************************************************************
**函    数： ClearRecTestStatus()
**功    能： 清除接收RSSI标志
**参    数： 无
**返    回： 无
**说    明： 防止接收中断产生433接收死锁
********************************************************************************/
void ClearRecTestStatus(void)
{
	SetRecTestStatus(0);
}
/*******************************************************************************
**函    数： Sign433_SendTestReport()
**功    能： 发送测试报告帧
********************************************************************************/
void Sign433_SendTestReport(void)
{
	u8 *pRSSI=NULL,i;
	pRSSI=GetRSSIPara();
	for(i=0 ;i<2;i++)
	{
	  Msg_Send( SINGLE_ACT , gSign433Comm.MasterID , 0 , 0x01 ,  \
              MASTER_REC_TEST_REPORT , gSign433Comm.TestPkgInfo.Number+1, pRSSI);//81包，补充发送干扰帧
	}		   
}

/***************************Copyright BestFu **********************************/ 
