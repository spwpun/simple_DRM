#include <stdio.h>
#include "getinfo.h"
#include "sm3.h"
#include <windows.h>

//����ǰ������
unsigned char *GenerateSerial (unsigned char *output);	//�����Զ��庯���������к�

//��������ʼ
int main ()
{
	unsigned char digest[32] = "";
	GenerateSerial (digest);

	system ("pause");
}

unsigned char *GenerateSerial (unsigned char *output)
{
	char macAdress[256];
	char cpuInfo[256];
	char hdSerial[256];
	GetAdapterInfo (macAdress);
	GetCPUInfo (cpuInfo);
	GetAllHDSerial (hdSerial);
	//����Ϣ���ӳ�һ���ַ�����MacAdress�����ж��
	strcat (hdSerial, cpuInfo);
	strcat (hdSerial, macAdress);
	unsigned char digest[32];
	SM3Calc ((const unsigned char *)hdSerial, strlen (hdSerial), digest);
	printf ("\n�õ������к����£�\n");
	for (int i = 0; i < 32; i++)
	{
		if ((i + 1) % 4 == 0)
			printf ("%02X ", digest[i]);
		else
			printf ("%02X", digest[i]);
	}
	output = digest;
	return output;
}