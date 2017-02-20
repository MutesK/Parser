#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "Parser.h"



CParser::CParser(char *fileName)
{
	FILE *fp;
	fopen_s(&fp, fileName, "rb");

	if (fp != nullptr)
	{
		// ���� ũ�� ������.
		fseek(fp, 0, SEEK_END);
		fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		buffer = new char[fileSize + 1];
		fread(buffer, fileSize + 1, 1, fp);
		buffer[fileSize] = '\0';

		fclose(fp);
	}

}

CParser::~CParser()
{
}

bool CParser::SkipNoneCommand(char **_buff)
{
	char *index = *_buff;

	while (true)
	{
		// �ּ� ��ŵ
		if (*index == '/' && *(index + 1) == '/')
		{
			while (*index != 0x0d)
				index++;
			index++;
		}

		if (*index == '/' && *(index + 1) == '*')
		{
			while ((*index != '*') && (*(index + 1) != '/'))
				index++;
			index++;
		}

		// �ݺ��� Ż�� ����
		if (*index == '\t' || *index == '"' || *index == 0x20 || *index == '\"' || *index == ':')
			break;

		if (index - buffer >= fileSize)
			return false;

		index++;
	}

	if (index - buffer >= fileSize)
		return false;
	else
	{
		*_buff = index + 1;
		return true;
	}
}

bool CParser::GetNextWord(char ** _buff, int * ipLength)
{
	if (SkipNoneCommand(_buff))
	{
		char *bufIndex = *_buff;

		int count = 0;
		if (bufIndex == '\0')
			return false;

		while (*bufIndex != '}')
		{
			if (*bufIndex == '\t' || *bufIndex == 0x20 || *bufIndex == '\r' || *bufIndex == '\n')
				break;

			bufIndex++;
			count++;
		}

		if (bufIndex - buffer >= fileSize)
			return false;

		*ipLength = count;
		return true;
	}

	return false;

}

bool CParser::isFloat(char *_buff, int len)
{
	int i = 0;
	int dotcount = 0;
	int numCount = 0;

	if (_buff[0] == '0')
		return false;

	while (i < len)
	{
		if (_buff[i] >= '0' && _buff[i] <= '9')
		{
			i++;
			numCount++;
			continue;
		}
		else if (_buff[i] == '.')
		{
			i++;
			dotcount++;
			continue;
		}
		else {
			return false;
		}

	}

	if (len - numCount == 1)
		return true;
	else 
		return false;
}

bool CParser::isInt(char *_buff, int len)
{
	int i = 0;

	if (_buff[0] == '0')
		return false;

	while (i < len)
	{
		if (_buff[i] >= '0' && _buff[i] <= '9')
		{
			i++;
			continue;
		}
		else {
			return false;
		}
	}

	return true;
}

bool CParser::GetValue(char *szName, int *ipValue, char *partName)
{
	char *buff, chWord[256];
	int iLength;

	buff = buffer;

	// ã���� �ϴ´ܾ ���ö����� ���ã�����̹Ƿ�, while������ �˻�
	while (GetNextWord(&buff, &iLength))
	{
		// WORD���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);

		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// ���ڷ� �Է� ���� �ܾ�� ������ �˻��Ѵ�.
 		 else if (0 == strcmp(szName, chWord))
		{
			// �´ٸ� �ٷ� �ڿ� =�� ã��.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// ���� �κ��� ������ �κ��� ����.
					if (GetNextWord(&buff, &iLength))
					{
						if (buff[0] != '\"')
						{
							memset(chWord, 0, 256);
							memcpy(chWord, buff, iLength);
							if (isInt(chWord, iLength))
							{
								*ipValue = atoi(chWord);
								return true;
							}
							return false;
						}
						return false;
					}

					return false;
				}
			}
			return false;
		}
	}
	return false;
}
bool CParser::GetValue(char *szName, char *ipValue, char *partName)
{
	char *buff, chWord[256];
	int iLength;

	buff = buffer;

	// ã���� �ϴ´ܾ ���ö����� ���ã�����̹Ƿ�, while������ �˻�
	while (GetNextWord(&buff, &iLength))
	{
		// WORD���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);
		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// ���ڷ� �Է� ���� �ܾ�� ������ �˻��Ѵ�.
		else if (0 == strcmp(szName, chWord))
		{
			// �´ٸ� �ٷ� �ڿ� =�� ã��.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// ���� �κ��� ������ �κ��� ����.
					if (GetNextWord(&buff, &iLength))
					{
						if (buff[0] != '\"')
						{
							memset(chWord, 0, 256);
							memcpy(chWord, buff, iLength);
							if (!isInt(chWord, iLength))
							{
								if (!isFloat(chWord, iLength))
								{
									chWord[iLength] = '\0';
									strcpy_s(ipValue, iLength + 1, chWord);
									return true;
								}
							}
							return false;
						}
						return false;
					}

					return false;
				}
			}
			return false;
		}
	}
	return false;
}
bool CParser::GetValue(char *szName, float *ipValue, char *partName)
{
	char *buff, chWord[256];
	int iLength;

	buff = buffer;
	// ã���� �ϴ´ܾ ���ö����� ���ã�����̹Ƿ�, while������ �˻�
	while (GetNextWord(&buff, &iLength))
	{
		// WORD���ۿ� ã�� �ܾ �����Ѵ�.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);

		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// ���ڷ� �Է� ���� �ܾ�� ������ �˻��Ѵ�.
		else if (0 == strcmp(szName, chWord))
		{
			// �´ٸ� �ٷ� �ڿ� =�� ã��.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// ���� �κ��� ������ �κ��� ����.
					if (GetNextWord(&buff, &iLength))
					{
						if (buff[0] != '\"')
						{

							memset(chWord, 0, 256);
							memcpy(chWord, buff, iLength);
							if (isFloat(chWord, iLength))
							{
								*ipValue = atof(chWord);
								return true;
							}
							return false;
						}
						return false;
					}

					return false;
				}
			}
			return false;
		}
	}

	return false;
}
