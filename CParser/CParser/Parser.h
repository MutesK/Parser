#pragma once
class CParser
{
private:
	char *buffer;
	int fileSize;

	bool SkipNoneCommand(char **_buff);
	bool GetNextWord(char **_buff_index, int *ipLength);
	bool isFloat(char *_buff, int len);
	bool isInt(char *_buff, int len);
public:
	CParser(char *fileName);
	~CParser();


	bool GetValue(char *szName, int *ipValue, char *part);
	bool GetValue(char *szName, char *ipValue, char *part);
	bool GetValue(char *szName, float *ipValue, char *part);

};

