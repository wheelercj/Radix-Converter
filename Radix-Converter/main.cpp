#include "radix.h"
#include <iostream>
using std::cout;
using std::cin;
using std::string;

int main()
{
	string number, startBase, endBase;

	while (true)
	{
		cout << "\n Enter the starting number: ";
		cin >> number;
		cout << "\n Enter the starting base: ";
		cin >> startBase;
		cout << "\n Enter the target base: ";
		cin >> endBase;

		number = changeBase(number, startBase, endBase);

		cout << "\n  = " << number << "\n";
	}
}
