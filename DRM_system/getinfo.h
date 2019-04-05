#include <wtypes.h>
#include <winnt.h>
#include <minwindef.h>
#pragma once
#ifndef __GETINFO_H__
#define __GETINFO_H__

/*---------------�ṹ�嶨��--------------*/

struct Atrr_docId {
	unsigned long Ad_cid;	//������ID
	unsigned char Ad_did;	//�ĵ�ID
	DATE Ad_ctime;			//��������
};

struct Attr_auth_lim {
	unsigned int Al_count;	//�ĵ�ʹ�ô���
	DATE Al_start_time;		//��ʼʱ��
	DATE Al_end_time;		//��ֹʱ��
};

struct Attr_access_right {
	bool Ar_read;			//�Ƿ�ɶ�
	bool Ar_copy;			//�Ƿ���Ը���
	bool Ar_print;			//�Ƿ���Դ�ӡ
	bool Ar_modify;			//�Ƿ�����޸�
	bool Ar_save_as;		//�Ƿ�������Ϊ
};

struct User_signature {
	unsigned char Us_sig[20];		//�û�ǩ��
	unsigned short int Us_auth_lim;	//�û��Ĳ���Ȩ��
};

struct User_Auth_License {
	unsigned long UA_user_id;		//�û������кţ���Ӳ����Ϣhash����
	unsigned long UA_license_id;	//���֤�����к�
};

/*----------��������------------*/
//������ǰ������
char *GetCPUInfo (char *);
char *GetAdapterInfo (char *);
int GetHardDriveSerialNumber (PCHAR pszIDBuff, int nBuffLen, int nDriveID);
char * flipAndCodeBytes (const char * str, int pos, int flip, char * buf);
char *GetAllHDSerial (char *output);
char *Byte2char (BYTE num, char *output);



#endif // !__GETINFO_H__
