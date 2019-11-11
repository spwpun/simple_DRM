#pragma once
#ifndef __FILEINFO_HEADER__
#define __FILEINFO_HEADER__
#include <wtypes.h>

/*-------------------struct define------------------*/
//该结构体由服务端构造，添加在pdf文件头部，客户端提取pdf文件的时候将此部分信息略过
typedef struct Pdf_File_info
{
	unsigned char user[10];	//使用者
	unsigned char creator[10];	//创建者
	unsigned char user_hardware_id[32]; //使用者的硬件序列号，可用来验证
	DATE create_time;	//创建日期
};

//该结构体的信息利用信息隐藏算法隐藏到pdf文件中去，作为版权声明
typedef struct Rights_Claimed
{
	unsigned char Rights_info[64];	//声明例：Copyright by spwpun 2019-05-15
};

/*------------------Function Declared-----------------------*/



#endif // !__FILEINFO_HEADER__
