#include <string.h>
#include <stdio.h>
#include "sm4.h"
#include "sm2.h"
#define UNICODE
#define _UNICODE

#include <Windows.h>
#include <tchar.h>
BOOL encryptfile (const char *in_fname, unsigned char *key, const char *out_fname);
BOOL decryptfile (const char *in_fname, unsigned char *key, const char *out_fname);

int main ()
{
	unsigned char key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	unsigned char input[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	unsigned char output[16];
	sm4_context ctx;
	unsigned long i;
	bool flag = FALSE;

	printf ("加密密钥：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", key[i]);
	printf ("\n");
		
	//加密标准的测试数据
	sm4_setkey_enc (&ctx, key);//设置key
	printf ("加密前数据：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", input[i]);
	printf ("\n");
	sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);
	printf ("加密后数据：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");

	//解密测试
	sm4_setkey_dec (&ctx, key);
	sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, output, output);
	printf ("解密后数据：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");
	printf ("文件加密测试：\n");
	flag = encryptfile ("test.txt", key, "encry.erc");
	if (flag)	printf ("文件加密成功！\n");
	else
	{
		printf ("文件加密失败！\n");
		return -1;
	}    
	flag = decryptfile ("encry.erc", key, "plain.txt");
	if (flag)	printf ("文件解密成功！\n");

	//sm2测试
	main_test_sm2 ();
	system ("pause");
	return 0;
}

BOOL encryptfile (const char *in_fname, unsigned char *key, const char *out_fname)
{
	FILE *fp1, *fp2;
	int i = 0;
	int length = 0;
	unsigned char ch;
	//16个字节为一组，先试试，最后还是得分组加密
	unsigned char input[17];
	unsigned char output[16];
	sm4_context ctx;//sm4上下文

	input[16] = '\0';
	sm4_setkey_enc (&ctx, key);//设置key
	fp1 = fopen (in_fname, "rb");
	if (fp1 == NULL)
	{
		printf ("文件打开失败，错误码:%d\n.", GetLastError ());
		return FALSE;
	}
	fp2 = fopen (out_fname, "wb");
	if (fp2 == NULL)
	{
		printf ("文件打开失败，错误码:%d\n.", GetLastError ());
		return FALSE;
	}
	
	while (1)
	{
		/*每次读入16个字节，使用SM4对称加密算法进行加密*/
		for (i = 0; i < 16; i++)
		{
			if (feof (fp1)) break;	//如果读到文件尾则跳出循环
			input[i] = fgetc (fp1);
		}
		//加密函数的长度参数可能最后一组不是16
		length = strlen ((const char *)input);
		sm4_crypt_ecb (&ctx, SM4_ENCRYPT, length, input, output);	//加密，输出到output数组,参数2为定义的宏，标志是加密还是解密
		for (i = 0; i < 16; i++)			
		{
			putc (output[i], fp2);		//将加密结果写入到文件中
			output[i] = '\0';			//清除缓冲区
			input[i] = '\0';
		}
		if (feof (fp1)) break;
	}
		
	fclose (fp1);
	fclose (fp2);
	return TRUE;
}

BOOL decryptfile (const char *in_fname, unsigned char *key, const char *out_fname)
{
	FILE *fp1, *fp2;
	int i = 0;
	char ch;
	unsigned char input[16];
	unsigned char output[16];
	sm4_context ctx;//sm4上下文

	sm4_setkey_dec (&ctx, key);//设置解密key
	fp1 = fopen (in_fname, "rb");
	if (fp1 == NULL)
	{
		printf ("文件打开失败，错误码:%d\n.", GetLastError ());
		return FALSE;
	}
	fp2 = fopen (out_fname, "wb");
	if (fp2 == NULL)
	{
		printf ("文件打开失败，错误码:%d\n.", GetLastError ());
		return FALSE;
	}
	while (1)
	{
		/*每次读入16个字节，使用SM4对称加密算法进行解密密*/
		for (i = 0; i < 16; i++)
		{
			if (feof (fp1)) break;	//如果读到文件尾则跳出循环
			input[i] = fgetc (fp1);
		}
		if (feof (fp1)) break;	//跳出while循环
		sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, input, output);
		for (i = 0; i < 16; i++)			//将解密结果写入到文件中
		{
			putc (output[i], fp2);
			//清除缓冲区
			output[i] = '\0';
			input[i] = '\0';
		}
	}

	fclose (fp1);
	fclose (fp2);
	return TRUE;
}