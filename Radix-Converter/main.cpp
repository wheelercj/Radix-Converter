#include "radix.h"
#include <iostream>
#include <fstream>
using std::cout;
using std::cin;
using std::string;
using std::ifstream;
using std::ofstream;

void runConverter();
void runSettingsMenu();
bool validStandardDigits(string input);
bool isInteger(string input);
void setStandardDigits();
void setResultPrecision();
void resetSettingsToDefaults();
void loadSettings();
void saveSettings();
bool removeSpaces(std::string& input);

int main()
{
	cout << "\n Radix Converter\n"
		<< "\n For examples and instructions, see https://github.com/wheelercj/Radix-Converter\n";
	loadSettings();
	
	while (true)
	{
		string input;
		cout << "\n main menu:"
			<< "\n 1. convert"
			<< "\n 2. settings"
			<< "\n 3. exit"
			<< "\n > ";
		getline(cin, input);
		removeSpaces(input);
		if (input == "1")
			runConverter();
		else if (input == "2")
			runSettingsMenu();
		else if (input == "3")
			exit(0);
	}
}

void runConverter()
{
	cout << "\n Enter nothing for the starting number to return to the main menu.";
	string number, startBase, endBase;

	while (true)
	{
		cout << "\n Enter the starting number: ";
		getline(cin, number);
		removeSpaces(number);
		if (number.empty())
			return;
		do {
			cout << "\n Enter the starting base: ";
			getline(cin, startBase);
			removeSpaces(startBase);
		} while (startBase.empty());
		do {
			cout << "\n Enter the target base: ";
			getline(cin, endBase);
			removeSpaces(endBase);
		} while (endBase.empty());

		number = changeBase(number, startBase, endBase);

		cout << "\n  = " << number << "\n";
	}
}

void runSettingsMenu()
{
	cout << "\n settings:"
		<< "\n standard digits: " << settings::getStandardDigits()
		<< "\n result precision: " << settings::getResultPrecision();

	while (true)
	{
		string input;
		cout << "\n\n settings menu:"
			<< "\n 1. change standard digits"
			<< "\n 2. change result precision"
			<< "\n 3. reset settings to defaults"
			<< "\n 4. return"
			<< "\n > ";
		getline(cin, input);
		removeSpaces(input);
		if (input == "1")
			setStandardDigits();
		else if (input == "2")
			setResultPrecision();
		else if (input == "3")
			resetSettingsToDefaults();
		else if (input == "4")
			return;
		saveSettings();
	}
}

bool validStandardDigits(string input)
{
	if (input.empty())
		return false;
	for (int i = 0; i < input.size() - 1; i++)
	{
		// Check for use of reserved digits.
		if (string::npos != settings::RESERVED_DIGITS.find(input[i]))
		{
			cout << "\n Error: you cannot use \"" << input[i] << "\" as a digit.";
			return false;
		}
		// Check for duplicates.
		for (int j = i + 1; j < input.size(); j++)
		{
			if (input[i] == input[j])
			{
				cout << "\n Error: the standard digits must not have any duplicates.";
				return false;
			}
		}
	}
	return true;
}

bool isInteger(string input)
{
	if (input.empty())
		return false;
	for (char ch : input)
	{
		if (!std::isdigit(ch))
			return false;
	}
	return true;
}

void setStandardDigits()
{
	string input;
	cout << "\n Enter the new standard digits: ";
	getline(cin, input);
	if (removeSpaces(input))
		cout << "\n Warning: space(s) removed from the standard digits.";
	if (validStandardDigits(input))
		settings::setStandardDigits(input);
}

void setResultPrecision()
{
	string input;
	cout << "\n Enter the new precision: ";
	getline(cin, input);
	removeSpaces(input);
	if (isInteger(input))
		settings::setResultPrecision(std::stoi(input));
	else
		cout << "\n Error: the precision must be a whole number.";
}

void resetSettingsToDefaults()
{
	settings::setStandardDigits(settings::DEFAULT_STANDARD_DIGITS);
	settings::setResultPrecision(settings::DEFAULT_RESULT_PRECISION);
}

void loadSettings()
{
	ifstream file("settings.txt");
	if (file.is_open())
	{
		string input;
		getline(file, input);
		if (removeSpaces(input))
			cout << "\n Warning: space(s) removed from the standard digits.";
		if (validStandardDigits(input))
			settings::setStandardDigits(input);
		else
			cout << "\n Settings file parsing error. Using default standard digits: " << settings::getStandardDigits();
		
		getline(file, input);
		removeSpaces(input);
		if (isInteger(input))
			settings::setResultPrecision(stoi(input));
		else
			cout << "\n Settings file parsing error. Using default precision of " << settings::getResultPrecision();
		file.close();
	}
}

void saveSettings()
{
	ofstream file("settings.txt");
	file << settings::getStandardDigits() << std::endl
		<< settings::getResultPrecision() << std::endl;
	file.close();
}

bool removeSpaces(std::string& input)
{
	std::string output = "";
	bool foundSpace = false;
	for (char ch : input)
	{
		if (ch != ' ')
			output += ch;
		else
			foundSpace = true;
	}
	if (foundSpace)
		input = output;
	return foundSpace;
}
