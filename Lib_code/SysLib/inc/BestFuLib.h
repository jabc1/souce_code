/***************************Copyright BestFu 2014-05-14*************************
文	件：	BestFulib.h
说	明：	BestFu自建Lib库头文件
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014-05-30
*******************************************************************************/
#ifndef __BESTFULIB_H
#define __BESTFULIB_H

/*容错宏*/
#define ERRR(conf, ret)      do              \
                            {                \
                                if (conf)    \
                                {            \
                                    ret;     \
                                }            \
                            } while(0) 
							
#define ABS(i, j)		(((i) > (j)) ? ((i) - (j)) : ((j) - (i))) 		//获取绝对值

                                
void memcpy(void *dest, const void *src, unsigned int num); //内存考备
char memcmp(void *dest,  const void *src, unsigned int num);	//内存比较
void memmove(void *dest,  const void *src, unsigned int num);	//内存移动
void memset(void *dest, unsigned char const val, unsigned int num);	//内存设置 
void* memfind(void *dest, unsigned int dLen, void *src, unsigned int sLen);							
void memcharget(void *dest, unsigned char *len, void const *src, unsigned int size);

#endif //BestFuLib.h end
/**************************Copyright BestFu 2014-05-14*************************/
							
