#include "list.h"
#define UC_MEM_ON 1
int test=0;
//初始化链表
void List_Init(LIST *plist)
{
	plist->head = NULL;
	plist->count = 0;
	plist->tail = NULL;
}

//链表为空
bool IsListEmpty(LIST *plist)
{
	if (plist->count == 0)
		return true;
	return false;
}

//链表为满
bool IsListFull(LIST *plist)
{
	NODE * ptemp;
	ptemp = (NODE *)malloc(sizeof(NODE));
	if (ptemp == NULL)
	{
		free(ptemp);
		return true;
	}
	free(ptemp);
	return false;
}

//添加项
bool ADD_Record(M_TIME ptime,uint8_t ID,LIST *plist)
{
	NODE * ptemp;
		OS_ERR err;
#ifndef UC_MEM_ON
		if (IsListFull(plist))
	{
		return false;
	}
	ptemp = (NODE *)malloc(sizeof(NODE));
	if (ptemp == NULL)
		return false;
#else
	ptemp = (NODE *)OSMemGet(&F_Log,&err);
	if (err != OS_ERR_NONE)
	return false;
#endif
	ptemp->item.S_Time.day=ptime.day;
	ptemp->item.S_Time.hour=ptime.hour;
	ptemp->item.S_Time.min=ptime.min;
	ptemp->item.S_Time.month=ptime.month;
	ptemp->item.S_Time.sec=ptime.sec;
	ptemp->item.S_Time.year=ptime.year;
	
	ptemp->item.ID=ID;
	ptemp->next = NULL;
	if (plist->head == NULL)
	{
		plist->head = ptemp;
	}
	else
		plist->tail->next = ptemp;
	plist->tail = ptemp;
	plist->count++;
	return true;
}

//删除项
bool Del_Item(LIST *plist)
{
	if (IsListEmpty(plist))
		return false;
	NODE *ptemp;
	ptemp = plist->head;
	if (ptemp->next == NULL)
	{
		plist->tail = NULL;
	}
	plist->head = plist->head->next;
#ifndef UC_MEM_ON
	free(ptemp);
#else
	OS_ERR err;
	OSMemPut(&F_Log,ptemp,&err);
#endif
	plist->count--;
	return true;

}

//遍历项查找
bool List_Traverse(uint8_t id,LIST *plist)
{
	NODE * ptemp;
	ptemp = plist->head;
	while (ptemp != NULL)
	{
		if (ptemp->item.ID==id)
			return true;
		ptemp = ptemp->next;
	}
	return false;

}

//清空链表
void Clr_List(LIST *plist)
{
	while(!IsListEmpty(plist))
	{
		Del_Item(plist);
	}
}
