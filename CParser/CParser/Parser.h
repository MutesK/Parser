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


	// Parameter : 구역 이름
	bool ProvideArea(char *_AreaName);

	
	// GetValue : 원하는 값을 얻음. 
	// Parameters : ini 파일안의 변수 이름, 저장될 문자열, (in) 버퍼길이 (out) 문자열 길이
	// Parameters : ini 파일안의 변수 이름, 저장될 변수
	bool GetValue(char *_name, char *_value, int *ipBufSize);
	bool GetValue(char *_name, int *ipValue);
	bool GetValue(char *_name, float *ipValue);

};

