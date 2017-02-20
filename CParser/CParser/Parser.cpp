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
		// 파일 크기 가져옴.
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
		// 주석 스킵
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

		// 반복문 탈출 조건
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

	// 찾고자 하는단어가 나올때까지 계속찾을것이므로, while문으로 검사
	while (GetNextWord(&buff, &iLength))
	{
		// WORD버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);

		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// 인자로 입력 받은 단어와 같은지 검사한다.
 		 else if (0 == strcmp(szName, chWord))
		{
			// 맞다면 바로 뒤에 =를 찾자.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// 다음 부분의 데이터 부분을 얻음.
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

	// 찾고자 하는단어가 나올때까지 계속찾을것이므로, while문으로 검사
	while (GetNextWord(&buff, &iLength))
	{
		// WORD버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);
		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// 인자로 입력 받은 단어와 같은지 검사한다.
		else if (0 == strcmp(szName, chWord))
		{
			// 맞다면 바로 뒤에 =를 찾자.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// 다음 부분의 데이터 부분을 얻음.
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
	// 찾고자 하는단어가 나올때까지 계속찾을것이므로, while문으로 검사
	while (GetNextWord(&buff, &iLength))
	{
		// WORD버퍼에 찾은 단어를 저장한다.
		memset(chWord, 0, 256);
		memcpy(chWord, buff, iLength);

		if (*(buff - 1) == ':')
			strcpy_s(partName, iLength + 1, chWord);
		
		// 인자로 입력 받은 단어와 같은지 검사한다.
		else if (0 == strcmp(szName, chWord))
		{
			// 맞다면 바로 뒤에 =를 찾자.
			if (GetNextWord(&buff, &iLength))
			{
				memset(chWord, 0, 256);
				memcpy(chWord, buff, iLength);

				if (0 == strcmp(chWord, "="))
				{
					// 다음 부분의 데이터 부분을 얻음.
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
