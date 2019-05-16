#include <string.h>
#include <stdio.h>
#include "sm4.h"
#include "sm2.h"
#include <Windows.h>
#include <tchar.h>
#include "fileinfo.h"
#define UNICODE
#define _UNICODE


BOOL encryptfile (const char *in_fname, unsigned char *key, const char *out_fname);
BOOL decryptfile (const char *in_fname, unsigned char *key, const char *out_fname);
BOOL embed_info_in_pdf (const char infilename[20], const char *info, const char outfilename[20]);
char *extract_info_in_pdf (const char *stegofile);
void add_file_struct_info_to_file (const char *ercfile, Pdf_File_info pdf_file_info, const char *rarfile);

void my_fputs (char *Buffer, FILE *fp);
void my_fgets (char *Buffer, FILE *fp);
int getNumofObj (FILE *fp);
char *getOffset (char *readline, int value);
unsigned char*ConvertHardwareId (char *hex_string);
int tolower (int c);
int htoi (char *s);


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
		
	////加密标准的测试数据
	//sm4_setkey_enc (&ctx, key);//设置key
	//printf ("加密前数据：");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", input[i]);
	//printf ("\n");
	//sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);
	//printf ("加密后数据：");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", output[i]);
	//printf ("\n");

	////解密测试
	//sm4_setkey_dec (&ctx, key);
	//sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, output, output);
	//printf ("解密后数据：");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", output[i]);
	//printf ("\n");
	printf ("[+] PDF隐写测试\n");
	embed_info_in_pdf ("test.pdf", "Copyright 2019-05-04 Spwpun\n", "stego.pdf");
	extract_info_in_pdf ("stego.pdf");

	printf ("[+] 文件加密测试：\n");
	flag = encryptfile ("stego.pdf", key, "encry_pdf.erc");
	if (flag)	printf ("文件加密成功！\n");
	else
	{
		printf ("文件加密失败！\n");
		return -1;
	}
	printf ("添加文件信息测试：\n");
	Pdf_File_info pdf_file_info;
	strncpy ((char *)pdf_file_info.user, "Leeping", 10); //改为10自动填充'\0'
	strncpy ((char *)pdf_file_info.creator, "Spwpun", 10);
	char hardware[] = "254CC182224D74ED96E2482BC63DA1556E5A5333B62F8F3EE07F59E287CEE992";
	unsigned char *hardware_id = ConvertHardwareId (hardware);
	strncpy ((char *)pdf_file_info.user_hardware_id, (const char *)hardware_id, 32);
	add_file_struct_info_to_file ("encry_pdf.erc", pdf_file_info, "pkg_pdf.pkg");


	/*flag = decryptfile ("encry_pdf.erc", key, "decry.pdf");
	if (flag)	printf ("文件解密成功！\n");*/

	//sm2测试
	//main_test_sm2 ();
	
	system ("pause");
	return 0;
}

BOOL encryptfile (const char *in_fname, unsigned char *key, const char *out_fname)
{
	FILE *fp1, *fp2;
	int i = 0;
	int length = 0;
	unsigned char ch;
	//16个字节为一组，分组加密
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
		//加密函数的长度参数可能最后一组不是16，但加密之后的输出还是16
		sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);	//加密，输出到output数组,参数2为定义的宏，标志是加密还是解密
		for (i = 0; i < 16; i++)			
		{
			fputc (output[i], fp2);		//将加密结果写入到文件中
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
	unsigned char input[17];
	unsigned char output[17];
	sm4_context ctx; //sm4上下文

	input[16] = '\0';
	output[16] = '\0';
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
			if ((output[i] == 0xFF) && (output[i + 1] == 0x00) && (output[i+2] == 0x00))	//识别解密出多余的部分
			{
				//printf ("success！\n");
				break;
			}
			fputc (output[i], fp2);
			//清除缓冲区
			output[i] = '\0';
			input[i] = '\0';
		}
	}

	fclose (fp1);
	fclose (fp2);
	return TRUE;
}

BOOL embed_info_in_pdf (const char infilename[20], const char *info, const char outfilename[20])
{
	FILE *infile, *outfile;
	char readline[4096]; //假设一行最大的字节数为4096个
	int iNumObj = 0;	//pdf文件中对象的个数
	char insertpoint[20];
	int len_info = 0;	//计算版权信息的字符串长度，以便修改Obj偏移
	char *newOffsetString = NULL;

	infile = fopen (infilename, "rb");
	outfile = fopen (outfilename, "wb");
	if (infile == NULL || outfile == NULL)
	{
		printf ("文件无法打开：%s\n", infilename);
		return FALSE;
	}
	iNumObj = getNumofObj (infile);
	itoa (iNumObj-1, insertpoint, 10); //减1是因为是从对象0开始
	strcat (insertpoint, " 0 obj\n");
	len_info = strlen (info);
	//printf ("插入点：%s", insertpoint);
	//pdf文件的格式比较明确，每个对象的开头和结尾都独占一行，所以只要一行一行读文件，匹配到对象头时，先插入版权信息即可
	while (1)
	{
		//TODO:需要找到PDF文件对象的数目，<</Size 17..即表示有17个对象，将版权信息嵌入最后一个对象，16 0 obj
		my_fgets (readline, infile);
		if (strncmp (readline, "%%EOF", 5) == 0)
		{
			printf ("END\n");
			my_fputs (readline, outfile);
			break;
		}
		//找到插入点，嵌入版权信息，这里嵌入的是明文的版权信息，后续自个儿改进可以将版权信息转换文20,09的十六进制数据，便于隐藏
		else if (strcmp (readline, insertpoint) == 0)	
		{
			printf ("已嵌入版权信息！\n");
			my_fputs ((char *)info, outfile);
			my_fputs (readline, outfile);
		}
		//修改对象偏移量
		else if (strncmp (readline, "trailer", 7) == 0)
		{
			fseek (infile, -28L, SEEK_CUR); //文件指针向前移动28个字符
			fseek (outfile, -20L, SEEK_CUR); //此时输出文件还未写入trailer
			memset (readline, 0, sizeof (readline));
			my_fgets (readline, infile);
			newOffsetString = getOffset (readline, len_info); //从一行数据中提取offset
			my_fputs (newOffsetString, outfile);
			//读取trailer这一行并写入
			memset (readline, 0, sizeof (readline));
			my_fgets (readline, infile);
			my_fputs (readline, outfile);
		}
		else
		{
			my_fputs (readline, outfile);
		}
		//清空字符串以便下一次写入新的行
		memset (readline, 0, sizeof (readline));	
	}
	fclose (infile);
	fclose (outfile);
	return TRUE;
}

char *extract_info_in_pdf (const char *stegofile)
{
	FILE *fpStegofile;
	char readline[4096];
	char *CopyRight_str = NULL;
	int flag = 0;
	
	fpStegofile = fopen (stegofile, "rb");
	if (fpStegofile == NULL)
	{
		printf ("无法打开Stego文件\n");
		return NULL;
	}
	while (1)
	{
		memset (readline, 0, sizeof (readline)); //清除缓存区
		my_fgets (readline, fpStegofile);
		if (strncmp (readline, "Copyright", 9) == 0)
		{
			CopyRight_str = readline;
			flag = 1;
			break;
		}
		if (feof (fpStegofile)) break;
	}
	if (!flag)
	{
		printf ("未找到版权信息！\n");
		fclose (fpStegofile);
		return NULL;
	}
	else
	{
		//测试输出
		printf ("文件版权信息：%s\n", CopyRight_str);
		fclose (fpStegofile);
		return CopyRight_str;
	}
}

void my_fgets (char *Buffer, FILE *fp)
{
	int ch;
	for (int i = 0; i < 4096; i++)
	{
		ch = fgetc (fp);
		if (ch == '\n')
		{
			Buffer[i] = ch;
			break;
		}
		if (ch == EOF)	
		{
			break;
		}
		Buffer[i] = ch;
	}
}

void my_fputs (char *Buffer, FILE *fp)
{
	int i = 0;
	//单独判断文件结尾
	if (strcmp (Buffer, "%%EOF") == 0)
	{
		fputs (Buffer, fp);
		return;
	}
	while (Buffer[i] != '\n')
	{
		fputc ((int)Buffer[i], fp);
		i++;
	}
	fputc ('\n', fp);
}

int getNumofObj (FILE *fp)
{
	char readline[4096];
	int iNumObj = 0;
	char numStr[10] = { 0 };
	while (1)
	{
		my_fgets (readline, fp);
		if (strncmp (readline, "<</Size", 7) == 0)
		{
			for (int i = 0; i < 10; i++)
			{
				if (!isalnum (readline[i + 8]))
				{
					iNumObj = atoi ((const char *)numStr);
					//测试输出
					//printf ("pdf文件的对象数为%d个\n", iNumObj);
					rewind (fp);	//将文件指针移到文件头
					return iNumObj;
				}
				else
				{
					numStr[i] = readline[i + 8];
				}
			}
		}
	}
}

char *getOffset (char *readline, int value)
{
	char *pLineString = NULL;
	char offsetStr[15] = { 0 };
	int offset = 0;

	pLineString = (char *)malloc (20);
	for (int i = 0; i < 15; i++)
	{
		if (!isalnum (readline[i]))
		{
			offset = atoi (offsetStr);
			offset = offset + value; //加上嵌入的消息长度，一般很小，不会改变offset的位数
			sprintf (pLineString, "%010d 00000 n\n", offset);
			//输出测试
			printf ("偏移字符串：%s\n偏移量：%d\n", pLineString, offset);
			break;
		}
		else
		{
			offsetStr[i] = readline[i];
		}
	}

	return pLineString;
}

void add_file_struct_info_to_file (const char *ercfile, Pdf_File_info pdf_file_info, const char *rarfile)
{
	//将结构体信息嵌入文件中,rarfile是最后打包成功的文件名
	FILE *fpErcfile;
	FILE *fpRarfile;
	int size = 0;
	int ch;

	fpErcfile = fopen (ercfile, "rb");
	fpRarfile = fopen (rarfile, "wb");
	if (fpErcfile == NULL || fpRarfile == NULL)
	{
		printf ("[+] WARNING:文件无法打开！\n");
		return;
	}
	//按块嵌入RAR文件头部
	fwrite (&pdf_file_info, sizeof (Pdf_File_info), 1, fpRarfile);
	//TODO:将ERC文件追加到RAR文件后面
	ch = fgetc (fpErcfile);
	while (ch != EOF)
	{
		fputc (ch, fpRarfile);
		ch = fgetc (fpErcfile);
	}

	fclose (fpErcfile);
	fclose (fpRarfile);
	return;
}

//将16进制形式的字符串转换为unsigned char数组，例："6e7f"-->"\x6e\x7f"
unsigned char*ConvertHardwareId(char *hex_string)
{
	unsigned char hardware_id[32];
	char tmp_str[2];

	memset (hardware_id, 0, 32);
	//Test output
	printf ("Test for Hardware ID convert:\n");
	for (int i = 0; i < 32; i++)
	{
		tmp_str[0] = hex_string[2 * i];
		tmp_str[1] = hex_string[2 * i + 1];
		hardware_id[i] = htoi (tmp_str);
		//测试输出
		printf ("%02X", hardware_id[i]);
	}
	return hardware_id;
}

/* 将大写字母转化成小写字母 */
int tolower (int c)
{
	if (c >= 'A' && c <= 'Z')
	{
		return c + 'a' - 'A';
	}
	else
	{
		return c;
	}
}


/* 将十六进制字符串转化成十进制整数 */
int htoi (char *s)
{
	int i;
	int n = 0;
	if (s[0] == '0' && (s[1] == 'x' || s[1] == 'X'))
	{
		i = 2;
	}
	else
	{
		i = 0;
	}
	for (; (s[i] >= '0' && s[i] <= '9') || (s[i] >= 'a' && s[i] <= 'z') || (s[i] >= 'A' && s[i] <= 'Z'); ++i)
	{
		if (tolower (s[i]) > '9')
		{
			n = 16 * n + (10 + tolower (s[i]) - 'a');
		}
		else
		{
			n = 16 * n + (tolower (s[i]) - '0');
		}
	}
	return n;
}