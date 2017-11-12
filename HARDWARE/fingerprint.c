#include "fingerprint.h"
                       //包头      地址               指令包  长度    指令      校检
uint8_t GR_GetModelNum[12]  ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x1d,0x00,0x21};//获取模板数量
uint8_t GR_Image[12]        ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x01,0x00,0x05};       //录入图像
uint8_t GR_Genchar_buff1[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x01,0x00,0x08};  //生成特征到buff1
uint8_t GR_Genchar_buff2[13]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x04,0x02,0x02,0x00,0x09};  //生成特征到buff2
uint8_t GR_Match[12]        ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x03,0x00,0x07};       //对比buff1和buff2的特征数据
uint8_t GR_Search[17]       ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x08,0x04,0x01,0x00,0x00,0x00,0x64,0x00,0x72};
	        //GR_Storechar[10]  缓存区号，GR_Storechar[11]，GR_Storechar[12] 开始页 GR_Storechar[13]，GR_Storechar[14]页数
uint8_t GR_RegModel[12]     ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x03,0x05,0x00,0x09};       //根据两个buff生成模板
uint8_t GR_Storechar[15]    ={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0x00,0x00,0x00,0x0E};//存储模板	
	                      //GR_Storechar[10] buffid  GR_Storechar[11] GR_Storechar[12] 编号

void oled_showerror(int err)
{
	switch(err)
	{
		case  F_OK        :  OLED_ShowString(12,32,"       OK       ",12);  break; //表示指令执行完毕或OK
    case  F_Error     :  OLED_ShowString(12,32,"  Package Error ",12);  break; //表示数据包接收错误
    case  F_Nofinger  :  OLED_ShowString(12,32,"    Finger NO   ",12);  break; //表示传感器上没有手指
    case  F_Wet       :  OLED_ShowString(12,32,"   Finger Wet   ",12);  break; //表示指纹图像太湿、太糊而生不成特征
		case  F_Disorde   :  OLED_ShowString(12,32,"  Finger Mixed  ",12);  break; //表示指纹图像太乱而生不成特征
		case  F_Nochar    :  OLED_ShowString(12,32,"   Finger less  ",12);  break; //特征点太少（或面积太小）而生不成特征
		case  F_Noavail   :  OLED_ShowString(12,32," Finger invaild ",12);  break; //表示缓冲区内没有有效原始图而生不成特征
		case  F_Unitelost :  OLED_ShowString(12,32," RegModel Failed",12);  break; //表示特征合并失败
		case  F_Flashlost :  OLED_ShowString(12,32,"   Flash Error  ",12);  break; //表示读写FLASH出错
		case  F_Nomatch   :  OLED_ShowString(12,32," Finger Nomatch ",12);  break; //表示指纹不匹配
		case  F_Nosearch  :  OLED_ShowString(12,32,"Finger Nosearch ",12);  break; //表示没搜索到指纹
		case  F_Dellost   :  OLED_ShowString(12,32,"  Clean Failed  ",12);  break; //表示清空指纹库失败
		case  F_Onedellost:  OLED_ShowString(12,32,"  Delete Failed ",12);  break; //表示删除模板失败
		default:break;
	}
	OLED_Refresh_Gram();//更新显示到OLED	
}

//获取模板数量
//通讯返回:EF 01|FF FF FF FF|07|00 05|XX|nn nn|SUM SUM 
void get_ModelNum(void)
{
	USART1_SendData(GR_GetModelNum,sizeof(GR_GetModelNum));
}
//获取图像
//通讯返回:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_Image(void)
{
	USART1_SendData(GR_Image,sizeof(GR_Image));
}

uint8_t IS_OK(u8 *temp)
{
	oled_showerror((int)temp[9]);
	if(temp[8]==7&&temp[9]==F_OK&&temp[13]>90&&temp[11]==99)//管理员指纹
	{
		OLED_ShowString(0,44,"    MASTER     ",16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 3;
	}
	else if(temp[8]==7&&temp[9]==F_OK&&temp[13]>90)//搜索到用户指纹，置信度达到90
	{
		OLED_ShowString(0,44,"     ID:",16);
		OLED_ShowNum(64,44,(temp[10]<<8)|temp[11],2,16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 2;
	}
	else if(temp[8]==7&&temp[9]==F_Nosearch)//未搜索到指纹
	{
		OLED_ShowString(0,44,"     TO ADD    ",16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 4;
	}
	else if(temp[8]==5&&temp[9]==F_OK)//正常ok，模板数量
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 5;
	}
	else if(temp[8]==3&&temp[9]==F_OK)//正常ok
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 1;
	}
	else
	{
		OLED_ShowString(0,44,"                ",16);
		OLED_Refresh_Gram();//更新显示到OLED	
		return 0;
	}
}


//生成特征存在buff1
//通讯返回:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void genchar_buff1(void)
{
	USART1_SendData(GR_Genchar_buff1,sizeof(GR_Genchar_buff1));
}
//生成特征存在buff2
//通讯返回:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void genchar_buff2(void)
{
	USART1_SendData(GR_Genchar_buff2,sizeof(GR_Genchar_buff2));
}
//对比B1，B2文件
//通讯返回:EF 01|FF FF FF FF|07|00 05|XX|XX|SUM SUM 
void get_match(void)
{
	USART1_SendData(GR_Match,sizeof(GR_Match));
}
//搜索指纹
//通讯返回:EF 01|FF FF FF FF|07|00 07|XX|ID ID|S S|SUM SUM
void get_search(void)
{
	USART1_SendData(GR_Search,sizeof(GR_Search));
}
//生成模板
//通讯返回:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_regmodel(void)
{
	USART1_SendData(GR_RegModel,sizeof(GR_RegModel));
}
//生成模板
//通讯返回:EF 01|FF FF FF FF|07|00 03|XX|SUM SUM 
void get_storemodel(int ID)
{
	uint8_t sendstore[15]={0xEF,0x01,0xFF,0xFF,0xFF,0xFF,0x01,0x00,0x06,0x06,0x01,0x00};
	sendstore[12]=GR_Storechar[12]+ID;
	sendstore[13]=0x00;
	sendstore[14]=GR_Storechar[14]+ID;
	USART1_SendData(sendstore,sizeof(sendstore));
}


