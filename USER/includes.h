#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include "stm32f10x.h"
#include  "os.h"  		//uC/OS-II系统函数头文件
#include  "os_type.h"  		//uC/OS-II系统函数头文件
#include  "BSP.h"			//与开发板相关的函数
#include 	"app.h"			//用户任务函数
#include 	"led.h"			//LED驱动函数
#include "usart.h"    //串口驱动程序
#include "fingerprint.h" //指纹模块驱动程序
#include "sys.h"
#include "stdlib.h"	 
#include "oled.h"
#include "sdio_sdcard.h"
#include "diskio.h"		/* FatFs lower layer API */
#include "ff.h"
#include "time.h"
#include "string.h"
#include "rtc.h"
#include "stdbool.h"
#include "list.h"

extern OS_TCB LedTaskTCB;
extern OS_TCB SfTaskTCB;
extern OS_TCB FmTaskTCB;
extern OS_TCB WriteTaskTCB;

extern OS_SEM share_Usart;
extern OS_SEM Re_Finger;
extern OS_SEM Write_SD;
extern OS_TMR systime;

extern FATFS fs;

extern OS_MEM F_Log;
#endif //__INCLUDES_H__
