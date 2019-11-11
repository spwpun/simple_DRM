#pragma once
#ifndef __FILEINFO_HEADER__
#define __FILEINFO_HEADER__
#include <wtypes.h>

/*-------------------struct define------------------*/
//�ýṹ���ɷ���˹��죬�����pdf�ļ�ͷ�����ͻ�����ȡpdf�ļ���ʱ�򽫴˲�����Ϣ�Թ�
typedef struct Pdf_File_info
{
	unsigned char user[10];	//ʹ����
	unsigned char creator[10];	//������
	unsigned char user_hardware_id[32]; //ʹ���ߵ�Ӳ�����кţ���������֤
	DATE create_time;	//��������
};

//�ýṹ�����Ϣ������Ϣ�����㷨���ص�pdf�ļ���ȥ����Ϊ��Ȩ����
typedef struct Rights_Claimed
{
	unsigned char Rights_info[64];	//��������Copyright by spwpun 2019-05-15
};

/*------------------Function Declared-----------------------*/



#endif // !__FILEINFO_HEADER__
