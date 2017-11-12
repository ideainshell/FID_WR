#ifndef __FINGERPRINT_H_
#define __FINGERPRINT_H_
#include "includes.h"
//指令集
#define CMD_GetImage   	 0x01   //从传感器上读入图像存于图像缓冲区
#define CMD_GenChar      0x02   //根据原始图像生成指纹特征存于CharBuffer1或CharBuffer2
#define CMD_Match        0x03   //精确比对CharBuffer1与CharBuffer2中的特征文件
#define CMD_Search       0x04   //以CharBuffer1或CharBuffer2中的特征文件搜索整个或部分指
#define CMD_RegModel     0x05   //将CharBuffer1与CharBuffer2中的特征文件合并生成模板存于CharBuffer2
#define CMD_StoreChar  	 0x06   //将特征缓冲区中的文件储存到flash指纹库中
#define CMD_LoadChar     0x07   //从flash指纹库中读取一个模板到特征缓冲区
#define CMD_UpChar       0x08   //将特征缓冲区中的文件上传给上位机
#define CMD_DownChar     0x09   //从上位机下载一个特征文件到特征缓冲区
#define CMD_UpImage      0x0A   //上传原始图像
#define CMD_DownImage    0x0B   //下载原始图像
#define CMD_DeletChar    0x0C   //删除flash指纹库中的一个特征文件
#define CMD_Empty	       0x0D   //清空flash指纹库
#define CMD_WriteReg     0x0E   //写SOC系统寄存器
#define CMD_ReadSysPara  0x0F   //读系统基本参数

/*-----------------------------------------*/
#define CMD_Identify     0x11   //验证指纹
#define CMD_SetPwd       0x12    //设置握手设备口令
#define CMD_VfyPwd       0x13    //验证握手设备口令	
#define CMD_WriteNotepad 0x18    //写记事本
#define CMD_ReadNotepad  0x19    //读记事本

//包头
#define Frame_Fst       0xEF   //帧界符第1位
#define Frame_Sed       0x01   //帧界符第2位

//包标识
#define BagID_Order     0x01   //命令包标识
#define BagID_Res       0x07   //应答包标识
#define BagID_Data      0x02   //数据包标识
#define BagID_Finish    0x08   //结束包标识

//确认码
#define F_OK            0x00   //表示指令执行完毕或OK
#define F_Error         0x01   //表示数据包接收错误
#define F_Nofinger      0x02   //表示传感器上没有手指
#define F_Wet           0x05   //表示指纹图像太湿、太糊而生不成特征
#define F_Disorde       0x06   //表示指纹图像太乱而生不成特征
#define F_Nochar        0x07   //特征点太少（或面积太小）而生不成特征
#define F_Noavail       0x15   //表示缓冲区内没有有效原始图而生不成特征
#define F_Unitelost     0x0a   //表示特征合并失败
#define F_Flashlost     0x18   //表示读写FLASH出错
#define F_Nomatch       0x08   //表示指纹不匹配
#define F_Nosearch      0x09   //表示没搜索到指纹
#define F_Dellost       0x11   //表示清空指纹库失败
#define F_Onedellost    0x10   //表示删除模板失败

void get_Image(void);
void genchar_buff1(void);
void genchar_buff2(void);
void get_search(void);
void get_ModelNum(void);
void get_regmodel(void);
void get_storemodel(int ID);
uint8_t IS_OK(u8 *temp);
#endif
