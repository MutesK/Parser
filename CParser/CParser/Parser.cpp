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
	//:, {, } ���� ã��
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

		// �齺���̽� 0x08
		// �� 0x09
		// ���� ��Ʈ 0x0a
		// ĳ���� ���� 0x0d
		// ���� 0x20
		if (*buf == 0x20 || *buf == 0x0d || *buf == 0x0a ||
			*buf == 0x09 || *buf == 0x08)
		{
			*buf = 0x20;
			++buf;

			++m_buffFocusPos;
		}
		// �ּ� '//' �����ڵ�
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

		// �ּ� '/**/'
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

	// ���ڿ� �б�
	if (**_buff_index == '"')
	{
		if (GetStringWord(_buff_index, ipLength))
			return true;
		else
			return false;
	}

	while (true)
	{
		// �ܾ��� ����
		// �ĸ� ','
		// ��ħǥ '.'
		// ����ǥ '"'
		// �����̽� 0x20
		// �齺���̽� 0x08
		// �� 0x0a
		// ���� ��Ʈ 0x0a
		// ĳ���� ���� 0x0d
		
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

	// ���ڿ� �б�
	if (**_buff_index != '"')
		return false;

	// ù ����Ǧ�� �ѱ��.
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
			// ���� ���ڿ� �ε�ȣ '='
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
			// ���� ���ڿ� �ε�ȣ '='
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
			// ���� ���ڿ� �ε�ȣ '='
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