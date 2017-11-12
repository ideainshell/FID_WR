#ifndef __FINGERPRINT_H_
#define __FINGERPRINT_H_
#include "includes.h"
//ָ�
#define CMD_GetImage   	 0x01   //�Ӵ������϶���ͼ�����ͼ�񻺳���
#define CMD_GenChar      0x02   //����ԭʼͼ������ָ����������CharBuffer1��CharBuffer2
#define CMD_Match        0x03   //��ȷ�ȶ�CharBuffer1��CharBuffer2�е������ļ�
#define CMD_Search       0x04   //��CharBuffer1��CharBuffer2�е������ļ����������򲿷�ָ
#define CMD_RegModel     0x05   //��CharBuffer1��CharBuffer2�е������ļ��ϲ�����ģ�����CharBuffer2
#define CMD_StoreChar  	 0x06   //�������������е��ļ����浽flashָ�ƿ���
#define CMD_LoadChar     0x07   //��flashָ�ƿ��ж�ȡһ��ģ�嵽����������
#define CMD_UpChar       0x08   //�������������е��ļ��ϴ�����λ��
#define CMD_DownChar     0x09   //����λ������һ�������ļ�������������
#define CMD_UpImage      0x0A   //�ϴ�ԭʼͼ��
#define CMD_DownImage    0x0B   //����ԭʼͼ��
#define CMD_DeletChar    0x0C   //ɾ��flashָ�ƿ��е�һ�������ļ�
#define CMD_Empty	       0x0D   //���flashָ�ƿ�
#define CMD_WriteReg     0x0E   //дSOCϵͳ�Ĵ���
#define CMD_ReadSysPara  0x0F   //��ϵͳ��������

/*-----------------------------------------*/
#define CMD_Identify     0x11   //��ָ֤��
#define CMD_SetPwd       0x12    //���������豸����
#define CMD_VfyPwd       0x13    //��֤�����豸����	
#define CMD_WriteNotepad 0x18    //д���±�
#define CMD_ReadNotepad  0x19    //�����±�

//��ͷ
#define Frame_Fst       0xEF   //֡�����1λ
#define Frame_Sed       0x01   //֡�����2λ

//����ʶ
#define BagID_Order     0x01   //�������ʶ
#define BagID_Res       0x07   //Ӧ�����ʶ
#define BagID_Data      0x02   //���ݰ���ʶ
#define BagID_Finish    0x08   //��������ʶ

//ȷ����
#define F_OK            0x00   //��ʾָ��ִ����ϻ�OK
#define F_Error         0x01   //��ʾ���ݰ����մ���
#define F_Nofinger      0x02   //��ʾ��������û����ָ
#define F_Wet           0x05   //��ʾָ��ͼ��̫ʪ��̫��������������
#define F_Disorde       0x06   //��ʾָ��ͼ��̫�Ҷ�����������
#define F_Nochar        0x07   //������̫�٣������̫С��������������
#define F_Noavail       0x15   //��ʾ��������û����Чԭʼͼ������������
#define F_Unitelost     0x0a   //��ʾ�����ϲ�ʧ��
#define F_Flashlost     0x18   //��ʾ��дFLASH����
#define F_Nomatch       0x08   //��ʾָ�Ʋ�ƥ��
#define F_Nosearch      0x09   //��ʾû������ָ��
#define F_Dellost       0x11   //��ʾ���ָ�ƿ�ʧ��
#define F_Onedellost    0x10   //��ʾɾ��ģ��ʧ��

void get_Image(void);
void genchar_buff1(void);
void genchar_buff2(void);
void get_search(void);
void get_ModelNum(void);
void get_regmodel(void);
void get_storemodel(int ID);
uint8_t IS_OK(u8 *temp);
#endif
