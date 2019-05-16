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

	printf ("������Կ��");
	for (i = 0; i < 16; i++)
		printf ("%02X ", key[i]);
	printf ("\n");
		
	////���ܱ�׼�Ĳ�������
	//sm4_setkey_enc (&ctx, key);//����key
	//printf ("����ǰ���ݣ�");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", input[i]);
	//printf ("\n");
	//sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);
	//printf ("���ܺ����ݣ�");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", output[i]);
	//printf ("\n");

	////���ܲ���
	//sm4_setkey_dec (&ctx, key);
	//sm4_crypt_ecb (&ctx, SM4_DECRYPT, 16, output, output);
	//printf ("���ܺ����ݣ�");
	//for (i = 0; i < 16; i++)
	//	printf ("%02X ", output[i]);
	//printf ("\n");
	printf ("[+] PDF��д����\n");
	embed_info_in_pdf ("test.pdf", "Copyright 2019-05-04 Spwpun\n", "stego.pdf");
	extract_info_in_pdf ("stego.pdf");

	printf ("[+] �ļ����ܲ��ԣ�\n");
	flag = encryptfile ("stego.pdf", key, "encry_pdf.erc");
	if (flag)	printf ("�ļ����ܳɹ���\n");
	else
	{
		printf ("�ļ�����ʧ�ܣ�\n");
		return -1;
	}
	printf ("����ļ���Ϣ���ԣ�\n");
	Pdf_File_info pdf_file_info;
	strncpy ((char *)pdf_file_info.user, "Leeping", 10); //��Ϊ10�Զ����'\0'
	strncpy ((char *)pdf_file_info.creator, "Spwpun", 10);
	char hardware[] = "254CC182224D74ED96E2482BC63DA1556E5A5333B62F8F3EE07F59E287CEE992";
	unsigned char *hardware_id = ConvertHardwareId (hardware);
	strncpy ((char *)pdf_file_info.user_hardware_id, (const char *)hardware_id, 32);
	add_file_struct_info_to_file ("encry_pdf.erc", pdf_file_info, "pkg_pdf.pkg");


	/*flag = decryptfile ("encry_pdf.erc", key, "decry.pdf");
	if (flag)	printf ("�ļ����ܳɹ���\n");*/

	//sm2����
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
	//16���ֽ�Ϊһ�飬�������
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
		//���ܺ����ĳ��Ȳ����������һ�鲻��16��������֮����������16
		sm4_crypt_ecb (&ctx, SM4_ENCRYPT, 16, input, output);	//���ܣ������output����,����2Ϊ����ĺ꣬��־�Ǽ��ܻ��ǽ���
		for (i = 0; i < 16; i++)			
		{
			fputc (output[i], fp2);		//�����ܽ��д�뵽�ļ���
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
	unsigned char input[17];
	unsigned char output[17];
	sm4_context ctx; //sm4������

	input[16] = '\0';
	output[16] = '\0';
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
			if ((output[i] == 0xFF) && (output[i + 1] == 0x00) && (output[i+2] == 0x00))	//ʶ����ܳ�����Ĳ���
			{
				//printf ("success��\n");
				break;
			}
			fputc (output[i], fp2);
			//���������
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
	char readline[4096]; //����һ�������ֽ���Ϊ4096��
	int iNumObj = 0;	//pdf�ļ��ж���ĸ���
	char insertpoint[20];
	int len_info = 0;	//�����Ȩ��Ϣ���ַ������ȣ��Ա��޸�Objƫ��
	char *newOffsetString = NULL;

	infile = fopen (infilename, "rb");
	outfile = fopen (outfilename, "wb");
	if (infile == NULL || outfile == NULL)
	{
		printf ("�ļ��޷��򿪣�%s\n", infilename);
		return FALSE;
	}
	iNumObj = getNumofObj (infile);
	itoa (iNumObj-1, insertpoint, 10); //��1����Ϊ�ǴӶ���0��ʼ
	strcat (insertpoint, " 0 obj\n");
	len_info = strlen (info);
	//printf ("����㣺%s", insertpoint);
	//pdf�ļ��ĸ�ʽ�Ƚ���ȷ��ÿ������Ŀ�ͷ�ͽ�β����ռһ�У�����ֻҪһ��һ�ж��ļ���ƥ�䵽����ͷʱ���Ȳ����Ȩ��Ϣ����
	while (1)
	{
		//TODO:��Ҫ�ҵ�PDF�ļ��������Ŀ��<</Size 17..����ʾ��17�����󣬽���Ȩ��ϢǶ�����һ������16 0 obj
		my_fgets (readline, infile);
		if (strncmp (readline, "%%EOF", 5) == 0)
		{
			printf ("END\n");
			my_fputs (readline, outfile);
			break;
		}
		//�ҵ�����㣬Ƕ���Ȩ��Ϣ������Ƕ��������ĵİ�Ȩ��Ϣ�������Ը����Ľ����Խ���Ȩ��Ϣת����20,09��ʮ���������ݣ���������
		else if (strcmp (readline, insertpoint) == 0)	
		{
			printf ("��Ƕ���Ȩ��Ϣ��\n");
			my_fputs ((char *)info, outfile);
			my_fputs (readline, outfile);
		}
		//�޸Ķ���ƫ����
		else if (strncmp (readline, "trailer", 7) == 0)
		{
			fseek (infile, -28L, SEEK_CUR); //�ļ�ָ����ǰ�ƶ�28���ַ�
			fseek (outfile, -20L, SEEK_CUR); //��ʱ����ļ���δд��trailer
			memset (readline, 0, sizeof (readline));
			my_fgets (readline, infile);
			newOffsetString = getOffset (readline, len_info); //��һ����������ȡoffset
			my_fputs (newOffsetString, outfile);
			//��ȡtrailer��һ�в�д��
			memset (readline, 0, sizeof (readline));
			my_fgets (readline, infile);
			my_fputs (readline, outfile);
		}
		else
		{
			my_fputs (readline, outfile);
		}
		//����ַ����Ա���һ��д���µ���
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
		printf ("�޷���Stego�ļ�\n");
		return NULL;
	}
	while (1)
	{
		memset (readline, 0, sizeof (readline)); //���������
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
		printf ("δ�ҵ���Ȩ��Ϣ��\n");
		fclose (fpStegofile);
		return NULL;
	}
	else
	{
		//�������
		printf ("�ļ���Ȩ��Ϣ��%s\n", CopyRight_str);
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
	//�����ж��ļ���β
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
					//�������
					//printf ("pdf�ļ��Ķ�����Ϊ%d��\n", iNumObj);
					rewind (fp);	//���ļ�ָ���Ƶ��ļ�ͷ
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
			offset = offset + value; //����Ƕ�����Ϣ���ȣ�һ���С������ı�offset��λ��
			sprintf (pLineString, "%010d 00000 n\n", offset);
			//�������
			printf ("ƫ���ַ�����%s\nƫ������%d\n", pLineString, offset);
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
	//���ṹ����ϢǶ���ļ���,rarfile��������ɹ����ļ���
	FILE *fpErcfile;
	FILE *fpRarfile;
	int size = 0;
	int ch;

	fpErcfile = fopen (ercfile, "rb");
	fpRarfile = fopen (rarfile, "wb");
	if (fpErcfile == NULL || fpRarfile == NULL)
	{
		printf ("[+] WARNING:�ļ��޷��򿪣�\n");
		return;
	}
	//����Ƕ��RAR�ļ�ͷ��
	fwrite (&pdf_file_info, sizeof (Pdf_File_info), 1, fpRarfile);
	//TODO:��ERC�ļ�׷�ӵ�RAR�ļ�����
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

//��16������ʽ���ַ���ת��Ϊunsigned char���飬����"6e7f"-->"\x6e\x7f"
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
		//�������
		printf ("%02X", hardware_id[i]);
	}
	return hardware_id;
}

/* ����д��ĸת����Сд��ĸ */
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


/* ��ʮ�������ַ���ת����ʮ�������� */
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