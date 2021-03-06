#include <stdio.h>
#include <winsock2.h>
#include <windows.h>
#include <iphlpapi.h>
#include <tchar.h>
#include <string.h>
#include "getinfo.h"
#pragma comment(lib, "IPHLPAPI.lib")
#pragma comment(lib, "user32.lib")


char *GetCPUInfo (char *output)
{
	SYSTEM_INFO siSysInfo;
	FILE *fp;
	const char *pProcessor = NULL;
	const char *pType = NULL;
	char sCpuid[17];
	char cpuinfo[60];
	char ch[2];
	int i = 0;

	GetSystemInfo (&siSysInfo);
	//原来的doemID在官方文档里面不推荐使用，只是一个保留字段，为了兼容性
	switch (siSysInfo.wProcessorArchitecture)
	{
		case PROCESSOR_ARCHITECTURE_AMD64:
			pProcessor = "PROCESSOR_ARCHITECTURE_AMD64";
			break;
		case PROCESSOR_ARCHITECTURE_ARM64:
			pProcessor = "PROCESSOR_ARCHITECTURE_ARM64";
			break;
		case PROCESSOR_ARCHITECTURE_INTEL:
			pProcessor = "PROCESSOR_ARCHITECTURE_INTEL";
			break;
		case PROCESSOR_ARCHITECTURE_MIPS:
			pProcessor = "PROCESSOR_ARCHITECTURE_MIPS";
			break;
		default:
			pProcessor = "PROCESSOR_ARCHITECTURE_OTHER";
			break;
	}
	switch (siSysInfo.dwProcessorType)
	{
		case PROCESSOR_INTEL_386:
			pType = "INTEL_386";
			break;
		case PROCESSOR_INTEL_486:
			pType = "INTEL_486";
			break;
		case PROCESSOR_INTEL_PENTIUM:
			pType = "INTEL_PENTIUM";
			break;
		case PROCESSOR_INTEL_IA64:
			pType = "INTEL_IA64";
			break;
		case PROCESSOR_AMD_X8664:
			pType = "AMD_X8664";
			break;
		case PROCESSOR_MIPS_R4000:
			pType = "MIPS_R4000";
			break;
		default:
			pType = "UNKNOWN";
			break;
	}
	//测试时取消注释
	printf ("\n设备生产商: %s\n", pProcessor);
	printf ("处理器数量: %u\n", siSysInfo.dwNumberOfProcessors);
	printf ("页数的大小: %u\n", siSysInfo.dwPageSize);
	printf ("处理器类型: %s\n", pType);
	printf ("Cpuid has been stored into cpuid.txt!\n");
	system ("wmic cpu get processorid > cpuid.txt");
	fp = fopen ("cpuid.txt", "rb");
	if (fp == NULL)
	{
		printf ("文件打开失败，错误码:%d\n.", GetLastError ());
		return FALSE;
	}
	ch[0] = fgetc (fp);
	ch[1] = '\0';
	sCpuid[0] = '\0';
	while (!feof (fp))
	{
		if (i >= 0x2A && i <= 0x48 && i % 2 == 0)
		{
			strcat (sCpuid, ch);
		}
		ch[0] = fgetc (fp);
		i++;
	}
	sCpuid[16] = '\0';	//结束符
	puts (sCpuid);	//输出CPUID测试时使用
	fclose (fp);
	strcpy (cpuinfo, pProcessor);
	strcat (cpuinfo, pType);
	strcat (cpuinfo, sCpuid);
	printf ("\n处理器信息：%s\n", cpuinfo);		//测试输出信息
	strcpy (output, cpuinfo);

	return output;
}

char *Byte2char (BYTE num, char *output)
{
	int num_tmp = int (num);
	if (num_tmp < 16)
	{
		output[0] = '0';
		itoa (num_tmp, &output[1], 16);
	}
	else
		itoa (num_tmp, output, 16);
	return output;
}

char *GetAdapterInfo (char *macAdress)
{
	IP_ADAPTER_INFO *pAdapterInfo = NULL;
	ULONG ulOutBufLen;
	DWORD dwRetVal;
	PIP_ADAPTER_INFO pAdapter;
	char EthernetMac[256];
	char temp[5];
	static char loc_buf[1000];
	int adapterNum = 0;

	pAdapterInfo = (IP_ADAPTER_INFO *)malloc (sizeof (IP_ADAPTER_INFO));
	ulOutBufLen = sizeof (IP_ADAPTER_INFO);
	if (GetAdaptersInfo (pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		free (pAdapterInfo);
		pAdapterInfo = (IP_ADAPTER_INFO *)malloc (ulOutBufLen);
	}
	if (dwRetVal = GetAdaptersInfo (pAdapterInfo, &ulOutBufLen) != ERROR_SUCCESS)
	{
		printf ("Error!\n");
	}
	pAdapter = pAdapterInfo;
	while (pAdapter)
	{
		//测试时请取消下面两行的注释
		printf ("\n网络适配器名：%s\n", pAdapter->AdapterName);
		printf ("网络适配器描述：%s\n", pAdapter->Description);
		//过滤掉其他可变网卡的计算
		//strncmp (pAdapter->Description, "VMware", 6) == 0 
		//strncmp (pAdapter->Description, "VirtualBox", 10) == 0 
		if (strncmp(pAdapter->Description, "Realtek", 7) != 0)
		{
			//测试时取消注释
			printf ("易改变MAC的网卡，不计算！\n");
			pAdapter = pAdapter->Next;
			continue;
		}
		//测试时取消注释
		printf ("MAC地址：");
		for (int i = 0; i < (int)pAdapter->AddressLength; i++)
		{
			//将字节转换为16进制字符串存入temp中
			Byte2char ((int)pAdapter->Address[i], temp);
			if (i == (pAdapter->AddressLength - 1))
			{
				//测试时取消注释
				printf ("%02X\n", (int)pAdapter->Address[i]);
				EthernetMac[adapterNum + 3 * i] = temp[0];
				EthernetMac[adapterNum + 3 * i + 1] = temp[1];
				EthernetMac[adapterNum + 3 * i + 2] = ';';
			}
			else
			{
				EthernetMac[adapterNum + 3 * i] = temp[0];
				EthernetMac[adapterNum + 3 * i + 1] = temp[1];
				EthernetMac[adapterNum + 3 * i + 2] = ':';
				//测试时取消注释
				printf ("%.2X:", (int)pAdapter->Address[i]);
			}
		}	
		adapterNum = adapterNum + 18;
		pAdapter = pAdapter->Next;
	}
	EthernetMac[adapterNum] = '\0';
	//测试时取消注释
	for (int j = 0; EthernetMac[j] != '\0'; j++)
	{
		printf ("%c", EthernetMac[j]);
	}
	strcpy (macAdress, EthernetMac);

	return macAdress;
}

char * flipAndCodeBytes (const char * str,
	int pos,
	int flip,
	char * buf)
{
	int i;
	int j = 0;
	int k = 0;

	buf[0] = '\0';
	if (pos <= 0)
		return buf;

	if (!j)
	{
		char p = 0;

		// First try to gather all characters representing hex digits only.
		j = 1;
		k = 0;
		buf[k] = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = tolower (str[i]);
			//isspace判断是否为空白字符
			if (isspace (c))
				c = '0';

			++p;
			buf[k] <<= 4;

			if (c >= '0' && c <= '9')
				buf[k] |= (unsigned char)(c - '0');
			else if (c >= 'a' && c <= 'f')
				buf[k] |= (unsigned char)(c - 'a' + 10);
			else
			{
				j = 0;
				break;
			}

			if (p == 2)
			{
				if (buf[k] != '\0' && !isprint (buf[k]))
				{
					j = 0;
					break;
				}
				++k;
				p = 0;
				buf[k] = 0;
			}

		}
	}

	if (!j)
	{
		// There are non-digit characters, gather them as is.
		j = 1;
		k = 0;
		for (i = pos; j && str[i] != '\0'; ++i)
		{
			char c = str[i];

			if (!isprint (c))
			{
				j = 0;
				break;
			}

			buf[k++] = c;
		}
	}

	if (!j)
	{
		// The characters are not there or are not printable.
		k = 0;
	}

	buf[k] = '\0';

	if (flip)
		// Flip adjacent characters
		for (j = 0; j < k; j += 2)
		{
			char t = buf[j];
			buf[j] = buf[j + 1];
			buf[j + 1] = t;
		}

	// Trim any beginning and end space
	i = j = -1;
	for (k = 0; buf[k] != '\0'; ++k)
	{
		if (!isspace (buf[k]))
		{
			if (i < 0)
				i = k;
			j = k;
		}
	}

	if ((i >= 0) && (j >= 0))
	{
		for (k = i; (k <= j) && (buf[k] != '\0'); ++k)
			buf[k - i] = buf[k];
		buf[k - i] = '\0';
	}

	return buf;
}

int GetHardDriveSerialNumber (PCHAR pszIDBuff, int nBuffLen, int nDriveID)
{
	HANDLE hPhysicalDrive = INVALID_HANDLE_VALUE;
	ULONG ulSerialLen = 0;
	__try
	{
		//  Try to get a handle to PhysicalDrive IOCTL, report failure
		//  and exit if can't.
		TCHAR szDriveName[32];
		wsprintf (szDriveName, TEXT ("\\\\.\\PhysicalDrive%d"), nDriveID);

		//  Windows NT, Windows 2000, Windows XP - admin rights not required
		hPhysicalDrive = CreateFile (szDriveName, 0,
			FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
			OPEN_EXISTING, 0, NULL);
		if (hPhysicalDrive == INVALID_HANDLE_VALUE)
		{
			__leave;
		}
		STORAGE_PROPERTY_QUERY query;
		DWORD cbBytesReturned = 0;
		static char local_buffer[10000];

		memset ((void *)&query, 0, sizeof (query));
		query.PropertyId = StorageDeviceProperty;
		query.QueryType = PropertyStandardQuery;

		memset (local_buffer, 0, sizeof (local_buffer));

		if (DeviceIoControl (hPhysicalDrive, IOCTL_STORAGE_QUERY_PROPERTY,
			&query,
			sizeof (query),
			&local_buffer[0],
			sizeof (local_buffer),
			&cbBytesReturned, NULL))
		{
			STORAGE_DEVICE_DESCRIPTOR * descrip = (STORAGE_DEVICE_DESCRIPTOR *)& local_buffer;
			char serialNumber[1000];

			flipAndCodeBytes (local_buffer,
				descrip->SerialNumberOffset,
				1, serialNumber);

			if (isalnum (serialNumber[0]))
			{
				ULONG ulSerialLenTemp = strnlen (serialNumber, nBuffLen - 1);
				memcpy (pszIDBuff, serialNumber, ulSerialLenTemp);
				pszIDBuff[ulSerialLenTemp] = NULL;
				ulSerialLen = ulSerialLenTemp;
				__leave;
			}

		}
	}
	__finally
	{
		if (hPhysicalDrive != INVALID_HANDLE_VALUE)
		{
			CloseHandle (hPhysicalDrive);
		}
		return ulSerialLen;
	}

	return 0;
}

char *GetAllHDSerial (char *output)
{
	const int MAX_IDE_DRIVES = 16;
	static char szBuff[0x100];
	for (int nDriveNum = 0; nDriveNum < MAX_IDE_DRIVES; nDriveNum++)
	{
		ULONG ulLen = GetHardDriveSerialNumber (szBuff, sizeof (szBuff), nDriveNum);
		//测试时取消注释
		if (ulLen > 0)
		{
			_tprintf (TEXT ("\n第%d块硬盘的序列号为：%hs\n"), nDriveNum + 1, szBuff);
		}
	}
	strcpy (output, szBuff);

	return output;
}