#include "ProgramDefinition.h"

#ifdef PLAYER
#include "Player.h"
int main()
{
	Player player;
	return 0;
}
#endif

#ifdef MENU
#include "Menu.h"
int main()
{
	Menu menu;
	return 0;
}
#endif


#ifdef FILE_TEST
#include <fstream>
#include <iostream>

int main()
{
	//prinzip zum lesen und schreiben /funktioniert gleichzeitig/
	char a;
	std::cin >> a;
	if (a == 'o')
	{
		while (true)
		{
			std::fstream file{ "out.gtmp", std::ios::in | std::ios::app };
			std::string output;
			file >> output;
			std::cout << output << std::endl;
			file.close();
		}
	}
	else
	{
		while (true)
		{
			std::string input;
			std::getline(std::cin, input);
			std::fstream file{ "out.gtmp", std::ios::out | std::fstream::trunc };
			file << input;
			file.close();
		}
	}
	return 0;
}
#endif
