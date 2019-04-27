#include <stdio.h>
#include "getinfo.h"
#include "sm3.h"
#include <windows.h>

//函数前向声明
unsigned char *GenerateSerial (unsigned char *output);	//调用自定义函数产生序列号

//主函数开始
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
	//将信息连接成一个字符串，MacAdress可能有多个
	strcat (hdSerial, cpuInfo);
	strcat (hdSerial, macAdress);
	unsigned char digest[32];
	SM3Calc ((const unsigned char *)hdSerial, strlen (hdSerial), digest);
	printf ("\n该电脑序列号如下：\n");
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