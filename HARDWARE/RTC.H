
/* ������������������Ļ�,PC13�ͻ����Ƶ��ΪRTC Clock/64��ʱ�� */   
//#define RTCClockOutput_Enable  /* RTC Clock/64 is output on tamper pin(PC.13) */
#ifndef __RTC_H_
#define __RTC_H_
#include "includes.h"
void RTC_Configuration(void);
void Time_Adjust(uint32_t Tmp_HH,uint32_t Tmp_MM,uint32_t Tmp_SS);
#endif