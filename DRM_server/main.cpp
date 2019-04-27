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

	printf ("������Կ��");
	for (i = 0; i < 16; i++)
		printf ("%02X ", key[i]);
	printf ("\n");
		
	//���ܱ�׼�Ĳ�������
	sm4_setkey_enc (&ctx, key);//����key
	printf ("����ǰ���ݣ�");
	for (i = 0; i < 16; i++)
		printf ("%02X ", input[i]);
	printf ("\n");
	sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);
	printf ("���ܺ����ݣ�");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");

	//���ܲ���
	sm4_setkey_dec (&ctx, key);
	sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, output, output);
	printf ("���ܺ����ݣ�");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");
	printf ("�ļ����ܲ��ԣ�\n");
	flag = encryptfile ("test.txt", key, "encry.erc");
	if (flag)	printf ("�ļ����ܳɹ���\n");
	else
	{
		printf ("�ļ�����ʧ�ܣ�\n");
		return -1;
	}    
	flag = decryptfile ("encry.erc", key, "plain.txt");
	if (flag)	printf ("�ļ����ܳɹ���\n");

	//sm2����
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
	//16���ֽ�Ϊһ�飬�����ԣ�����ǵ÷������
	unsigned char input[17];
	unsigned char output[16];
	sm4_context ctx;//sm4������

	input[16] = '\0';
	sm4_setkey_enc (&ctx, key);//����key
	fp1 = fopen (in_fname, "rb");
	if (fp1 == NULL)
	{
		printf ("�ļ���ʧ�ܣ�������:%d\n.", GetLastError ());
		return FALSE;
	}
	fp2 = fopen (out_fname, "wb");
	if (fp2 == NULL)
	{
		printf ("�ļ���ʧ�ܣ�������:%d\n.", GetLastError ());
		return FALSE;
	}
	
	while (1)
	{
		/*ÿ�ζ���16���ֽڣ�ʹ��SM4�ԳƼ����㷨���м���*/
		for (i = 0; i < 16; i++)
		{
			if (feof (fp1)) break;	//��������ļ�β������ѭ��
			input[i] = fgetc (fp1);
		}
		//���ܺ����ĳ��Ȳ����������һ�鲻��16
		length = strlen ((const char *)input);
		sm4_crypt_ecb (&ctx, SM4_ENCRYPT, length, input, output);	//���ܣ������output����,����2Ϊ����ĺ꣬��־�Ǽ��ܻ��ǽ���
		for (i = 0; i < 16; i++)			
		{
			putc (output[i], fp2);		//�����ܽ��д�뵽�ļ���
			output[i] = '\0';			//���������
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
	sm4_context ctx;//sm4������

	sm4_setkey_dec (&ctx, key);//���ý���key
	fp1 = fopen (in_fname, "rb");
	if (fp1 == NULL)
	{
		printf ("�ļ���ʧ�ܣ�������:%d\n.", GetLastError ());
		return FALSE;
	}
	fp2 = fopen (out_fname, "wb");
	if (fp2 == NULL)
	{
		printf ("�ļ���ʧ�ܣ�������:%d\n.", GetLastError ());
		return FALSE;
	}
	while (1)
	{
		/*ÿ�ζ���16���ֽڣ�ʹ��SM4�ԳƼ����㷨���н�����*/
		for (i = 0; i < 16; i++)
		{
			if (feof (fp1)) break;	//��������ļ�β������ѭ��
			input[i] = fgetc (fp1);
		}
		if (feof (fp1)) break;	//����whileѭ��
		sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, input, output);
		for (i = 0; i < 16; i++)			//�����ܽ��д�뵽�ļ���
		{
			putc (output[i], fp2);
			//���������
			output[i] = '\0';
			input[i] = '\0';
		}
	}

	fclose (fp1);
	fclose (fp2);
	return TRUE;
}