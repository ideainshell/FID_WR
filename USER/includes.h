#ifndef  __INCLUDES_H__
#define  __INCLUDES_H__

#include "stm32f10x.h"
#include  "os.h"  		//uC/OS-IIϵͳ����ͷ�ļ�
#include  "os_type.h"  		//uC/OS-IIϵͳ����ͷ�ļ�
#include  "BSP.h"			//�뿪������صĺ���
#include 	"app.h"			//�û�������
#include 	"led.h"			//LED��������
#include "usart.h"    //������������
#include "fingerprint.h" //ָ��ģ����������
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
