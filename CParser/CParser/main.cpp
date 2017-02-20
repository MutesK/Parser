#include <iostream>
#include "Parser.h"

int main()
{

	CParser parser("Config.ini");

	while (1)
	{
		char part[256];
		char szName[256];

		int ivalue;
		float fvalue;
		char value[256];

		printf("찾을 명령 : ");
		scanf_s("%s", szName, 256);


		if (parser.GetValue(szName, &ivalue, part))
		{
			printf("%s::%s = %d \n", part, szName, ivalue);
		}

		if (parser.GetValue(szName, &fvalue, part))
		{
			printf("%s::%s = %f \n", part, szName, fvalue);
		}
		if (parser.GetValue(szName, value, part))
		{
			printf("%s::%s = %s \n", part, szName, value);
		}

	}
	
	

	return 0;
}