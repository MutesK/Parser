#pragma once
class CParser
{
private:
	char *m_buffer;
	int m_fileSize;

	int m_bufferAreaStart;
	int m_bufferAreaEnd;
	int m_buffFocusPos;

	bool SkipNoneCommand(void);
	bool GetNextWord(char **_buff_index, int *ipLength);
	bool GetStringWord(char **buff_index, int *ipLength);
public:
	CParser(char *fileName);
	~CParser();


	// Parameter : ���� �̸�
	bool ProvideArea(char *_AreaName);

	
	// GetValue : ���ϴ� ���� ����. 
	// Parameters : ini ���Ͼ��� ���� �̸�, ����� ���ڿ�, (in) ���۱��� (out) ���ڿ� ����
	// Parameters : ini ���Ͼ��� ���� �̸�, ����� ����
	bool GetValue(char *_name, char *_value, int *ipBufSize);
	bool GetValue(char *_name, int *ipValue);
	bool GetValue(char *_name, float *ipValue);

};

