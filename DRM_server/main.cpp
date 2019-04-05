#include <string.h>
#include <stdio.h>
#include "sm4.h"
#include <Windows.h>

int main ()
{
	unsigned char key[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	unsigned char input[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0xfe, 0xdc, 0xba, 0x98, 0x76, 0x54, 0x32, 0x10 };
	unsigned char output[16];
	sm4_context ctx;
	unsigned long i;

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
	sm4_crypt_ecb (&ctx, 1, 16, input, output);
	printf ("加密后数据：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");

	//decrypt testing
	sm4_setkey_dec (&ctx, key);
	sm4_crypt_ecb (&ctx, 0, 16, output, output);
	printf ("解密后数据：");
	for (i = 0; i < 16; i++)
		printf ("%02X ", output[i]);
	printf ("\n");

	//decrypt 1M times testing vector based on standards.
	/*i = 0;
	sm4_setkey_enc (&ctx, key);
	while (i < 1000000)
	{
		sm4_crypt_ecb (&ctx, 1, 16, input, input);
		i++;
	}
	for (i = 0; i < 16; i++)
		printf ("%02x ", input[i]);
	printf ("\n");*/

	system ("pause");
	return 0;
}