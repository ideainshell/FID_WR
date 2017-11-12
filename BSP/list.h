#ifndef __LIST_H_
#define __LIST_H_
#include "includes.h"

typedef struct rec
{
		uint8_t  ID;
	M_TIME S_Time;
}REC;

typedef struct node
{
	REC item;
	struct node *next;
}NODE;

struct list
{
	NODE * head;
	NODE * tail;
	uint16_t count;
};
typedef struct list LIST;

//初始化链表
void List_Init(LIST *plist);
//链表为空
bool IsListEmpty(LIST *plist);
//链表为满
bool IsListFull(LIST *plist);

//添加项
bool ADD_Record(M_TIME ptime,uint8_t ID,LIST *plist);
//遍历项查找
bool List_Traverse(uint8_t id,LIST *plist);
//清空链表
void Clr_List(LIST *plist);
//删除项
bool Del_Item(LIST *plist);





#endif
