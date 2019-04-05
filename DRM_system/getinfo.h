#include <wtypes.h>
#include <winnt.h>
#include <minwindef.h>
#pragma once
#ifndef __GETINFO_H__
#define __GETINFO_H__

/*---------------结构体定义--------------*/

struct Atrr_docId {
	unsigned long Ad_cid;	//创建者ID
	unsigned char Ad_did;	//文档ID
	DATE Ad_ctime;			//创建日期
};

struct Attr_auth_lim {
	unsigned int Al_count;	//文档使用次数
	DATE Al_start_time;		//开始时间
	DATE Al_end_time;		//截止时间
};

struct Attr_access_right {
	bool Ar_read;			//是否可读
	bool Ar_copy;			//是否可以复制
	bool Ar_print;			//是否可以打印
	bool Ar_modify;			//是否可以修改
	bool Ar_save_as;		//是否可以另存为
};

struct User_signature {
	unsigned char Us_sig[20];		//用户签名
	unsigned short int Us_auth_lim;	//用户的操作权限
};

struct User_Auth_License {
	unsigned long UA_user_id;		//用户的序列号，由硬件信息hash而得
	unsigned long UA_license_id;	//许可证的序列号
};

/*----------函数声明------------*/
//函数的前向声明
char *GetCPUInfo (char *);
char *GetAdapterInfo (char *);
int GetHardDriveSerialNumber (PCHAR pszIDBuff, int nBuffLen, int nDriveID);
char * flipAndCodeBytes (const char * str, int pos, int flip, char * buf);
char *GetAllHDSerial (char *output);
char *Byte2char (BYTE num, char *output);



#endif // !__GETINFO_H__
