#include <iostream>
#include "Parser.h"
using namespace std;
int main()
{

	CParser parser("Config.ini");
	float f;

	parser.ProvideArea("NETWORK");

	parser.GetValue("Version", &f);
	cout << f << endl;
	

	return 0;
}