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
		m_fileSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		m_buffer = new char[m_fileSize + 1];
		fread(m_buffer, m_fileSize + 1, 1, fp);
		m_buffer[m_fileSize] = '\0';

		m_bufferAreaStart = -1;
		m_bufferAreaEnd = m_fileSize;
		m_buffFocusPos = 0;

		fclose(fp);
	}
	else
		throw 1;
}

CParser::~CParser()
{
	delete[]m_buffer;
}

bool CParser::ProvideArea(char *_AreaName)
{
	char *buff, chBuff[256];
	int ilength;
	bool bAreaFlag = false;

	m_bufferAreaStart = -1;
	m_bufferAreaEnd = m_fileSize;
	m_buffFocusPos = 0;

	//-----------------------------------------
	//:, {, } 구역 찾음
	//-----------------------------------------
	while (GetNextWord(&buff, &ilength))
	{
		memset(chBuff, 0, 256);
		memcpy(chBuff, buff, ilength);

		if (chBuff[0] == ':')
		{
			if (0 == strcmp(chBuff + 1, _AreaName))
			{
				if (GetNextWord(&buff, &ilength))
				{
					memset(chBuff, 0, 256);
					memcpy(chBuff, buff, ilength);

					if (chBuff[0] == '{')
					{
						if (!SkipNoneCommand())
							return false;

						m_bufferAreaStart = m_buffFocusPos;
					}
				}
				else
					return false;
			}
		}
		else if (bAreaFlag && chBuff[0] == '}')
		{
			m_bufferAreaEnd = m_buffFocusPos - 1;
			return true;
		}
	}

	return false;



}
bool CParser::SkipNoneCommand()
{
	char *buf = m_buffer + m_buffFocusPos;

	while (true)
	{
		if (m_buffFocusPos > m_fileSize ||
			m_buffFocusPos > m_bufferAreaEnd)
			return false;

		// 백스페이스 0x08
		// 탭 0x09
		// 라인 피트 0x0a
		// 캐리지 리턴 0x0d
		// 공백 0x20
		if (*buf == 0x20 || *buf == 0x0d || *buf == 0x0a ||
			*buf == 0x09 || *buf == 0x08)
		{
			*buf = 0x20;
			++buf;

			++m_buffFocusPos;
		}
		// 주석 '//' 엔터코드
		else if (*buf == '/' && *(buf + 1) == '/')
		{
			while (*buf != 0x0d)
			{
				*buf = 0x20;
				++m_buffFocusPos;
				buf++;

				if (m_buffFocusPos > m_fileSize ||
					m_buffFocusPos > m_bufferAreaEnd)
					return false;
			}
		}

		// 주석 '/**/'
		else if (*buf == '/' && *(buf + 1) == '*')
		{
			while (!(*buf == '*' && *(buf + 1) == '/'))
			{
				*buf = 0x20;
				++m_buffFocusPos;
				buf++;

				if (m_buffFocusPos > m_fileSize ||
					m_buffFocusPos > m_bufferAreaEnd)
					return false;
			}

			*buf = 0x20;
			++buf;
			*buf = 0x20;
			++buf;

			m_buffFocusPos += 2;
		}
		else
		{
			break;
		}
	}
}
bool CParser::GetNextWord(char **_buff_index, int *ipLength)
{
	char *buffTemp;

	if (!SkipNoneCommand())
		return false;

	if (m_buffFocusPos > m_fileSize ||
		m_buffFocusPos > m_bufferAreaEnd)
		return false;

	buffTemp = *_buff_index = m_buffer + m_buffFocusPos;
	*ipLength = 0;

	// 문자열 읽기
	if (**_buff_index == '"')
	{
		if (GetStringWord(_buff_index, ipLength))
			return true;
		else
			return false;
	}

	while (true)
	{
		// 단어의 기준
		// 컴마 ','
		// 마침표 '.'
		// 따옴표 '"'
		// 스페이스 0x20
		// 백스페이스 0x08
		// 탭 0x0a
		// 라인 피트 0x0a
		// 캐리지 리턴 0x0d
		
		if (**_buff_index == ',' || **_buff_index == '"' ||
			**_buff_index == '.' || **_buff_index == 0x20 || **_buff_index == 0x08 || **_buff_index == 0x0a
			|| **_buff_index == 0x0d)
		{
			break;
		}

		m_buffFocusPos++;
		(*_buff_index)++;
		(*ipLength)++;

		if (m_buffFocusPos > m_fileSize ||
			m_buffFocusPos > m_bufferAreaEnd)
			return false;

	}
	*_buff_index = buffTemp;

	if (*ipLength == 0)
		return false;
	return true;
}
bool CParser::GetStringWord(char **_buff_index, int *ipLength)
{
	char *buffTemp;

	buffTemp = *_buff_index = m_buffer + m_buffFocusPos;
	*ipLength = 0;

	// 문자열 읽기
	if (**_buff_index != '"')
		return false;

	// 첫 따옴푠느 넘긴다.
	m_buffFocusPos++;
	(*_buff_index)++;
	(*buffTemp)++;

	while (1)
	{
		if (**_buff_index == '"' || **_buff_index == 0x0a || **_buff_index == 0x0d)
		{
			m_buffFocusPos++;
			break;
		}

		m_buffFocusPos++;
		(*_buff_index)++;
		(*ipLength)++;


		if (m_buffFocusPos > m_fileSize ||
			m_buffFocusPos > m_bufferAreaEnd)
			return false;
	}

	*_buff_index = buffTemp;

	if (*ipLength == 0)
		return false;
	return true;
}

bool CParser::GetValue(char *_name, char *_value, int *ipBufSize)
{
	char *buff, chBuff[256];
	int ilength;
	
	m_buffFocusPos = m_bufferAreaStart;

	while (GetNextWord(&buff, &ilength))
	{
		memset(chBuff, 0, 256);
		memcpy(chBuff, buff, ilength);

		if (0 == strcmp(_name, chBuff))
		{
			// 다음 문자열 부등호 '='
			if (GetNextWord(&buff, &ilength))
			{
				memset(chBuff, 0, 256);
				memcpy(chBuff, buff, ilength);

				if (0 == strcmp(chBuff, "="))
				{
					if (GetNextWord(&buff, &ilength))
					{
						memset(chBuff, 0, 256);
						memcpy(chBuff, buff, ilength);

						strcpy_s(_value, ilength, buff);
						*ipBufSize = ilength;
						return true;
					}
					return false;
				}
				return false;
			}
			return false;
		}
	}

}
bool CParser::GetValue(char *_name, int *ipValue)
{
	char *buff, chBuff[256];
	int ilength;

	m_buffFocusPos = m_bufferAreaStart;

	while (GetNextWord(&buff, &ilength))
	{
		memset(chBuff, 0, 256);
		memcpy(chBuff, buff, ilength);

		if (0 == strcmp(_name, chBuff))
		{
			// 다음 문자열 부등호 '='
			if (GetNextWord(&buff, &ilength))
			{
				memset(chBuff, 0, 256);
				memcpy(chBuff, buff, ilength);

				if (0 == strcmp(chBuff, "="))
				{
					if (GetNextWord(&buff, &ilength))
					{
						memset(chBuff, 0, 256);
						memcpy(chBuff, buff, ilength);

						*ipValue = atoi(chBuff);
						return true;
					}
					return false;
				}
				return false;
			}
			return false;
		}
	}
}
bool CParser::GetValue(char *_name, float *ipValue)
{
	char *buff, chBuff[256];
	int ilength;

	m_buffFocusPos = m_bufferAreaStart;

	while (GetNextWord(&buff, &ilength))
	{
		memset(chBuff, 0, 256);
		memcpy(chBuff, buff, ilength);

		if (0 == strcmp(_name, chBuff))
		{
			// 다음 문자열 부등호 '='
			if (GetNextWord(&buff, &ilength))
			{
				memset(chBuff, 0, 256);
				memcpy(chBuff, buff, ilength);

				if (0 == strcmp(chBuff, "="))
				{
					if (GetNextWord(&buff, &ilength))
					{
						memset(chBuff, 0, 256);
						memcpy(chBuff, buff, ilength);

						*ipValue = atof(chBuff);
						return true;
					}
					return false;
				}
				return false;
			}
			return false;
		}
	}
}