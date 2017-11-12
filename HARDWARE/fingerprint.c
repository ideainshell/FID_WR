#include "fingerprint.h"
                       //��ͷ      ��ַ               ָ���  ����    ָ��      У��
uint8_t GR_GetModelNum[12]  ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1d,0x00,0x21};//��ȡģ������
uint8_t GR_Image[12]        ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};       //¼��ͼ��
uint8_t GR_Genchar_buff1[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};  //����������buff1
uint8_t GR_Genchar_buff2[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};  //����������buff2
uint8_t GR_Match[12]        ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x03,0x00,0x07};       //�Ա�buff1��buff2����������
uint8_t GR_Search[17]       ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x64,0x00,0x72};
	        //GR_Storechar[10]  �������ţ�GR_Storechar[11]��GR_Storechar[12] ��ʼҳ GR_Storechar[13]��GR_Storechar[14]ҳ��
uint8_t GR_RegModel[12]     ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09};       //��������buff����ģ��
uint8_t GR_Storechar[15]    ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x0E};//�洢ģ��	
	                      //GR_Storechar[10] buffid  GR_Storechar[11] GR_Storechar[12] ���

void oled_showerror(int err)
{
	switch(err)
	{
		case  F_OK        :  OLED_ShowString(12,32,"       OK       ",12);  break; //��ʾָ��ִ����ϻ�OK
    case  F_Error     :  OLED_ShowString(12,32,"  Package Error ",12);  break; //��ʾ���ݰ����մ���
    case  F_Nofinger  :  OLED_ShowString(12,32,"    Finger NO   ",12);  break; //��ʾ��������û����ָ
    case  F_Wet       :  OLED_ShowString(12,32,"   Finger Wet   ",12);  break; //��ʾָ��ͼ��̫ʪ��̫��������������
		case  F_Disorde   :  OLED_ShowString(12,32,"  Finger Mixed  ",12);  break; //��ʾָ��ͼ��̫�Ҷ�����������
		case  F_Nochar    :  OLED_ShowString(12,32,"   Finger less  ",12);  break; //������̫�٣������̫С��������������
		case  F_Noavail   :  OLED_ShowString(12,32," Finger invaild ",12);  break; //��ʾ��������û����Чԭʼͼ������������
		case  F_Unitelost :  OLED_ShowString(12,32," RegModel Failed",12);  break; //��ʾ�����ϲ�ʧ��
		case  F_Flashlost :  OLED_ShowString(12,32,"   Flash Error  ",12);  break; //��ʾ��дFLASH����
		case  F_Nomatch   :  OLED_ShowString(12,32," Finger Nomatch ",12);  break; //��ʾָ�Ʋ�ƥ��
		case  F_Nosearch  :  OLED_ShowString(12,32,"Finger Nosearch ",12);  break; //��ʾû������ָ��
		case  F_Dellost   :  OLED_ShowString(12,32,"  Clean Failed  ",12);  break; //��ʾ���ָ�ƿ�ʧ��
		case  F_Onedellost:  OLED_ShowString(12,32,"  Delete Failed ",12);  break; //��ʾɾ��ģ��ʧ��
		default:break;
	}
	OLED_Refresh_Gram();//������ʾ��OLED	
}

//��ȡģ������
//ͨѶ����:EF 01|FF FF FF FF|07|00 05|XX|nn nn|SUM SUM 
void get_ModelNum(void)
{
	USART1_SendData(GR_GetModelNum,sizeof(GR_GetModelNum));
}
//��ȡͼ��
//ͨѶ����:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_Image(void)
{
	USART1_SendData(GR_Image,sizeof(GR_Image));
}

uint8_t IS_OK(u8 *temp)
{
	oled_showerror((int)temp[9]);
	if(temp[8]==7&&temp[9]==F_OK&&temp[13]>90&&temp[11]==99)//����Աָ��
	{
		OLED_ShowString(0,44,"    MASTER     ",16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 3;
	}
	else if(temp[8]==7&&temp[9]==F_OK&&temp[13]>90)//�������û�ָ�ƣ����Ŷȴﵽ90
	{
		OLED_ShowString(0,44,"     ID:",16);
		OLED_ShowNum(64,44,(temp[10]<<8)|temp[11],2,16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 2;
	}
	else if(temp[8]==7&&temp[9]==F_Nosearch)//δ������ָ��
	{
		OLED_ShowString(0,44,"     TO ADD    ",16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 4;
	}
	else if(temp[8]==5&&temp[9]==F_OK)//����ok��ģ������
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 5;
	}
	else if(temp[8]==3&&temp[9]==F_OK)//����ok
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 1;
	}
	else
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//������ʾ��OLED	
		return 0;
	}
}


//������������buff1
//ͨѶ����:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void genchar_buff1(void)
{
	USART1_SendData(GR_Genchar_buff1,sizeof(GR_Genchar_buff1));
}
//������������buff2
//ͨѶ����:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void genchar_buff2(void)
{
	USART1_SendData(GR_Genchar_buff2,sizeof(GR_Genchar_buff2));
}
//�Ա�B1��B2�ļ�
//ͨѶ����:EF 01|FF FF FF FF|07|00 05|XX|XX|SUM SUM 
void get_match(void)
{
	USART1_SendData(GR_Match,sizeof(GR_Match));
}
//����ָ��
//ͨѶ����:EF 01|FF FF FF FF|07|00 07|XX|ID ID|S S|SUM SUM
void get_search(void)
{
	USART1_SendData(GR_Search,sizeof(GR_Search));
}
//����ģ��
//ͨѶ����:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_regmodel(void)
{
	USART1_SendData(GR_RegModel,sizeof(GR_RegModel));
}
//����ģ��
//ͨѶ����:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_storemodel(int ID)
{
	uint8_t sendstore[15]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0x00};
	sendstore[12]=GR_Storechar[12]+ID;
	sendstore[13]=0x00;
	sendstore[14]=GR_Storechar[14]+ID;
	USART1_SendData(sendstore,sizeof(sendstore));
}


