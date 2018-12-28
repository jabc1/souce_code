
#ifndef _LCD_SHOW_H_
#define _LCD_SHOW_H_

#include "BF_type.h"
/*
#define AC_POWER_OFF     0      //电源关
#define AC_POWER_ON      1      //电源开

#define AC_MIN_TEMPER   16      //温度最小值
#define AC_MAX_TEMPER   30      //温度最大值

#define AC_MODE_MAX         4   //模式最大值
    #define AC_MODE_AUTO    0   //自动        自动
    #define AC_MODE_COOL    1   //制冷
    #define AC_MODE_WATER   2   //除湿
    #define AC_MODE_WIND    3   //送风
    #define AC_MODE_HOT     4   //制热

#define AC_WIND_MAX         3   //分模式最大值
    #define AC_WIND_AUTO    0   //自动风速
    #define AC_WIND_LOW     1   //低风速
    #define AC_WIND_MID     2   //中风速
    #define AC_WIND_HIGH    3   //高风速
*/

void LCD_Init(void);
void LCD_ShowLine(void);
void LCD_ShowTime(u8 t);
void LCD_ShowWindSpeed(u8 wind);
void LCD_ShowMode(u8 mode);
void LCD_ShowTemper(u8 T ,u8 flag);
void LCD_ShowPowerOff(void);
void LCD_ShowTimeLogo(u8 sta);
void LCD_ShowALL(void);
void Read_Air_Protocol(void);
void RS485_Gateway_Thread(void);
#endif

