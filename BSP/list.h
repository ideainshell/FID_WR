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

//��ʼ������
void List_Init(LIST *plist);
//����Ϊ��
bool IsListEmpty(LIST *plist);
//����Ϊ��
bool IsListFull(LIST *plist);

//�����
bool ADD_Record(M_TIME ptime,uint8_t ID,LIST *plist);
//���������
bool List_Traverse(uint8_t id,LIST *plist);
//�������
void Clr_List(LIST *plist);
//ɾ����
bool Del_Item(LIST *plist);





#endif
