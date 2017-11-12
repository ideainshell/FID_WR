#ifndef __TIME_H_
#define __TIME_H_
#include "includes.h"

typedef struct m_time 
{
	uint32_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}M_TIME;

typedef struct m_date
{
	uint32_t year;
	uint8_t month;
	uint8_t day;
}M_DATE;

typedef struct m_day
{
	uint8_t hour;
	uint8_t min;
	uint8_t sec;
}M_DAY;


//初始化时间
void Time_Init(uint16_t year,uint16_t MaD,uint32_t day);
//加一秒
void Time_add_Day(void);
//获取时间
M_TIME Time_Get(void);

void Time_Str_Get( char *Stime);
void Date_Str_Get( char *Stime);
void day_Str_Get( char *Stime);
void T2Str(M_TIME *ptime,char *Stime);
void D2Str(M_TIME *ptime,uint8_t *id,char *Stime);
#endif
