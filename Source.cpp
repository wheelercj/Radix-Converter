#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <boost/lexical_cast.hpp>
using namespace std;

class Error {};
void loadSettings(string&, int&);
void readBases(int&, int&);
string readStartNum(int, bool&);
bool detectNumeralsForm(string, string, int, int);
vector<unsigned long> stringToVector(string, string, int, bool);
unsigned long long toDecimal(vector<unsigned long>, int);
void mantissaToDecimal(vector<unsigned long>&, int);
string fromDecimal(string, int, int, bool);
string mantissaFromDecimal(vector<unsigned long>, string, int, bool, int);
int charToInt(string, char, int);
bool zeroVector(vector<unsigned long>);
void printResult(string, string, int, int);
bool invalid_int_input();
int mainMenu(string&, int&);
void settings(string&, int&);
void saveSettings(string, int);
void defaultSettings(string&, int&);
void editStandardDigits(string&, int&);
bool invalidNewDigit(string);
int getCharElement(string);
void listDigits(string);
void help(string);

int main()
{
	int startBase,
		endBase;
	int endDigitCount; // result precision. Truncation occurs after base conversion.
	string standardDigits; // the digits that can be used in standard form
	bool negative,
		numeralsStart,
		numeralsEnd;

	cout << "* * * * * * * * * *"
		  "\n* RADIX CONVERTER *"
		  "\n* * * * * * * * * *";

	loadSettings(standardDigits, endDigitCount);

	while (true)
	{
		try
		{
			int menuChoice = mainMenu(standardDigits, endDigitCount);
			if (menuChoice == 2)
				readBases(startBase, endBase);

			string str = readStartNum(startBase, negative),
				mantissa = ""; // the fractional part of a number

			numeralsStart = detectNumeralsForm(str, standardDigits, startBase, endBase);
			if (endBase > standardDigits.length())
				numeralsEnd = true;
			else
				numeralsEnd = false;

			// if there is a mantissa, store it in its own string
			for (int i = 0; i < str.length(); i++)
			{
				if (str[i] == '.')
				{
					mantissa = str.substr(i + 1, string::npos);
					str.erase(i, string::npos);
				}
			}
			
			// convert the string(s) to number vectors
			vector<unsigned long> vect = stringToVector(str, standardDigits, startBase, numeralsStart),
				mantVect;
			if (!mantissa.empty())
				mantVect = stringToVector(mantissa, standardDigits, startBase, numeralsStart);

			reverse(vect.begin(), vect.end()); // to make the following calculations easier

			// convert to decimal
			unsigned long long decimalNum = toDecimal(vect, startBase);
			if (!mantVect.empty() && (startBase != 10 || numeralsStart && !numeralsEnd))
				mantissaToDecimal(mantVect, startBase);

			// convert to a non-decimal base if desired
			if (endBase != 10 || numeralsEnd)
			{
				str = fromDecimal(standardDigits, decimalNum, endBase, numeralsEnd);
				if (!mantVect.empty())
					mantissa = mantissaFromDecimal(mantVect, standardDigits, endBase, numeralsEnd, endDigitCount);
			}
			else
			{
				str = to_string(decimalNum);

				// if the standard digit settings were edited, change each str digit to its equivalent standard digit
				for (int i = 0; i < str.length(); i++)
					str[i] = standardDigits[str[i] - '0'];

				if (!mantVect.empty())
				{
					// put mantVect into the mantissa string
					mantissa = "";
					for (int i = 0; i < mantVect.size(); i++)
					{
						if (mantVect[i] >= endBase)
						{
							cout << "\n Error: numerals-only fractional digit overflow.";
							throw Error();
						}

						mantissa.resize(i + 1);
						mantissa[i] = standardDigits[mantVect[i]];
					}
				}
			}

			if (str.empty())
				str = "0";
			if (negative)
				str.insert(0, "-");

			printResult(str, mantissa, endBase, endDigitCount);
		}
		catch (Error)
		{
		}
	}
}

void loadSettings(string& standardDigits, int& endDigitCount)
{
	// determine whether a settings file already exists
	fstream settingsFile("RadixSettings.txt", ios::in);
	if (settingsFile.fail())
	{
		// no settings file found. Create a new file with default settings
		settingsFile.open("RadixSettings.txt", ios::out);
		defaultSettings(standardDigits, endDigitCount);
		settingsFile.close();
	}
	else
	{
		// read settings from the file. All but the last input for standardDigits are just for file formatting
		for (int i = 0; i < 2; i++)
			settingsFile >> standardDigits;		
		settingsFile >> endDigitCount;
		for (int i = 0; i < 4; i++)
			settingsFile >> standardDigits;
		settingsFile.close();

		// validate the settings. If invalid settings are found, an error message will be displayed but the program will continue
		invalidNewDigit(standardDigits);
	}
}

void readBases(int& startBase, int& endBase)
{
	while (true)
	{
		cout << "\n Starting base: ";
		cin >> startBase;
		cout << " Target base: ";
		cin >> endBase;

		if (invalid_int_input())
			cout << "\n Please enter the bases as numbers.\n";
		else if (startBase < 1 || endBase < 1)
			cout << "\n Error: invalid base(s) entered.";
		else
			return;
	}
}

string readStartNum(int startBase, bool& negative)
{
	string str;
	cout << "\n Starting number (base " << startBase << "): ";
	cin >> str;

	// trailing period?
	if (str[str.length() - 1] == '.')
		str.erase(str.length() - 1, 1);

	// negative?
	if (str[0] == '-')
	{
		negative = true;
		str.erase(0, 1);
	}
	else
		negative = false;

	// hexadecimal prefix?
	if (startBase == 16 && str[0] == '0' && tolower(str[1]) == 'x')
		str.erase(0, 2);

	return str;
}

bool detectNumeralsForm(string str, string standardDigits, int startBase, int endBase)
{
	if (startBase > standardDigits.length())
		return true;

	// allow entering numbers of bases < maxNormalBase in numerals-only form, indicated by the use of one or more commas
	for (int i = str.length() - 1; i >= 0; i--)
	{
		if (str[i] == ',')
			return true;
	}

	return false;
}

vector<unsigned long> stringToVector(string strng, string standardDigits, int startBase, bool numeralsStart)
{
	vector<unsigned long> vect;

	if (!numeralsStart)
	{
		for (int i = 0; i < strng.length(); i++)
			vect.push_back(charToInt(standardDigits, strng[i], startBase));
	}
	else
	{
		for (int i = 0, j = 0; i < strng.length(); i++)
		{
			if (strng[i] == ',')
			{
				if (j != i)
					vect.push_back(stoi(strng.substr(j, i)));
				else // allow consecutive commas to be entered, meaning a hidden zero is between them
					vect.push_back(0);
				if (i == strng.length() - 1) // if the last char is a comma, there is a hidden zero at the end
					vect.push_back(0);

				j = i + 1;
			}
			else if (strng[i] == '.')
			{
				cout << "\n Error: more than one period entered.";
				throw Error();
			}
			else if (strng[i] > '9' || strng[i] < '0')
			{
				cout << "\n Error: non-numeral digit entered: " << strng[i];
				throw Error();
			}
			else if (i == strng.length() - 1)
				vect.push_back(stoi(strng.substr(j, i + 1)));
		}
	}

	return vect;
}

unsigned long long toDecimal(vector<unsigned long> vect, int startBase)
{
	unsigned long long decimalNum = 0;

	for (int i = 0; i < vect.size(); i++)
		decimalNum += vect[i] * (unsigned long)pow(startBase, i);

	return decimalNum;
}

void mantissaToDecimal(vector<unsigned long>& mantVect, int startBase)
{
	vector<unsigned long> denominators;
	int	lcd,
		total = 0;

	// when converting a non-decimal mantissa to decimal, each digit becomes fractional. To avoid rounding
	// errors, store the numerators and denominators of each digit seperately while converting
	for (int i = 0; i < mantVect.size(); i++)
		denominators.push_back((int)pow(startBase, i + 1));

	// the last denominator is the lowest common denominator of all the fractions
	lcd = denominators[mantVect.size() - 1];

	// multiply the numerators so that they all have the same denominator (the lcd) and find their sum
	for (int i = 0; i < mantVect.size(); i++)
		total += lcd / denominators[i] * mantVect[i];

	// divide the total by the lcd to get the mantissa in decimal, but in floating point form
	string temp = boost::lexical_cast<string>((double)total / lcd);

	// erase the preceding zero and period
	if (startBase != 1)
		temp.erase(0, 2);

	// put the mantissa into mantVect and return by reference
	mantVect.clear();
	for (int i = 0; i < temp.length(); i++)
		mantVect.push_back(temp[i] - '0');
}

string fromDecimal(string standardDigits, int decimalNum, int endBase, bool numeralsEnd)
{
	// Divide the decimal number by the target base repeatedly while storing the remainders of each division until the
	// decimal number is zero. The remainders in reverse order are the number in the target base.

	string str = "";

	for (int i = 0; decimalNum > 0; i++)
	{
		int digit = decimalNum % endBase;
		decimalNum /= endBase;

		if (!numeralsEnd)
		{
			str.resize(i + 1);
			str[i] = standardDigits[digit];
		}
		else
		{
			str.insert(0, ",");
			str.insert(0, to_string(digit));
		}
	}

	if (!numeralsEnd)
		reverse(str.begin(), str.end());
	else
		str.erase(str.length() - 1, 1); // erase the trailing comma

	return str;
}

string mantissaFromDecimal(vector<unsigned long> mantVect, string standardDigits, int endBase, bool numeralsEnd, int endDigitCount)
{
	// For each digit of the mantissa from right to left, multiply by the target base and add the carry from the previous digit if there
	// is one. If the value of the digit goes above 9, carry all but the ones place to the next digit. After this calculation is
	// made for each digit, store the leftover carry, or 0 if there isn't a carry, in the next available element of a separate array.
	// Repeat until the mantissa is zero. The separate array now holds the number in the target base.

	string mantissa = "";

	for (int i = 0; !zeroVector(mantVect) && i < endDigitCount; i++)
	{
		int carry = 0;

		for (int j = mantVect.size() - 1; j >= 0; j--)
		{
			int digit = mantVect[j] * endBase + carry;

			if (digit > 9)
			{
				carry = digit / 10;
				digit %= 10;
			}
			else
				carry = 0;

			mantVect[j] = digit;
		}

		if (!numeralsEnd)
		{
			mantissa.resize(i + 1);
			mantissa[i] = standardDigits[carry];
		}
		else
		{
			mantissa.append(to_string(carry));
			mantissa.append(",");
		}
	}

	if (numeralsEnd)
		mantissa.erase(mantissa.length() - 1, 1); // erase the trailing comma

	return mantissa;
}

int charToInt(string standardDigits, char ch, int startBase)
{
	if (ch == '.')
	{
		cout << "\n Error: more than one period entered.";
		throw Error();
	}

	int digit = standardDigits.find(ch);

	if (digit >= startBase && startBase != 1)
	{
		cout << "\n Error: digit invalid in base " << startBase << ": " << ch;
		throw Error();
	}
	else if (digit == string::npos)
	{
		cout << "\n Error: digit with no assigned value entered: " << ch;
		throw Error();
	}

	return digit;
}

bool zeroVector(vector<unsigned long> mantVect) // returns true if every element of the vector holds a zero
{
	for (int i = 0; i < mantVect.size(); i++)
	{
		if (mantVect[i] != 0)
			return false;
	}

	return true;
}

void printResult(string str, string mantissa, int endBase, int endDigitCount)
{
	cout << "\n  = " << str;

	if (!mantissa.empty())
	{
		cout << '.';
		for (int i = 0; i < mantissa.length() && i < endDigitCount; i++)
			cout << mantissa[i];
	}

	cout << " (base " << endBase << ')';
}

bool invalid_int_input()
{
	if (cin.fail())
	{
		cin.clear();
		cin.ignore(numeric_limits<int>::max(), '\n');
		return true;
	}

	return false;
}

int mainMenu(string& standardDigits, int& endDigitCount)
{
	int menuChoice;
	static bool canContinue = false;

	do
	{
		if (canContinue)
			cout << "\n\n Continue(1) ";
		else
			cout << "\n\n ----------- ";

		cout << "/ New Bases(2) / List Digits(3) / Settings(4) / Help(5) / Quit(0) \n> ";
		cin >> menuChoice;

		if (menuChoice == 0)
			exit(0);
		if (menuChoice == 2)
			canContinue = true;
		else if (menuChoice == 3)
			listDigits(standardDigits);
		else if (menuChoice == 4)
			settings(standardDigits, endDigitCount);
		else if (menuChoice == 5)
			help(standardDigits);

	} while (invalid_int_input() || menuChoice < 1 || menuChoice > 2 || !canContinue && menuChoice == 1);

	return menuChoice;
}

void settings(string& standardDigits, int& endDigitCount)
{
	int menuChoice;
	char confirm;
	string newDigits;

	while (true)
	{
		do
		{
			cout << "\n 1. Change result precision (currently " << endDigitCount << " digits)"
				"\n 2. Edit the standard form digits (current max standard base: " << standardDigits.length() << ")"
				"\n 3. Reset all settings to their defaults"
				"\n 0. Back"
				"\n> ";
			cin >> menuChoice;
		} while (invalid_int_input() || menuChoice < 0 || menuChoice > 3);

		switch (menuChoice)
		{
		case 0: // back
			return;

		case 1: // change result precision
			do
			{
				cout << "\n Result precision: ";
				cin >> endDigitCount;
			} while (invalid_int_input());
			saveSettings(standardDigits, endDigitCount);
			break;

		case 2: // Edit the standard form digits
			editStandardDigits(standardDigits, endDigitCount);
			break;

		case 3: // reset all settings to their defaults
			do
			{
				cout << "\n Are you sure? [y/n] > ";
				cin >> confirm;
				confirm = tolower(confirm);
			} while (confirm != 'y' && confirm != 'n');
			if (confirm == 'y')
			{
				defaultSettings(standardDigits, endDigitCount);
				cout << "\n Default settings restored.\n";
			}
		}
	}
}

void saveSettings(string standardDigits, int endDigitCount)
{
	fstream settingsFile("RadixSettings.txt", ios::out);

	settingsFile << "Result precision: " << endDigitCount
		<< "\nStandard form digits: " << standardDigits
		<< "\n\nPlease be careful to not change the setting descriptions, or the program may not be able to correctly load the settings.";
	settingsFile.close();
}

void defaultSettings(string& standardDigits, int& endDigitCount)
{
	endDigitCount = 20;
	standardDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	saveSettings(standardDigits, endDigitCount);
}

void editStandardDigits(string& standardDigits, int& endDigitCount)
{
	int menuChoice,
		charElement;

	while (true)
	{
		cout << "\n Standard form digits:\n " << standardDigits << '\n';

		do
		{
			cout << "\n 1. Replace"
				"\n 2. Insert/Append"
				"\n 3. Delete"
				"\n 0. Back"
				"\n> ";
			cin >> menuChoice;
		} while (invalid_int_input() || menuChoice < 0 || menuChoice > 3);

		if (menuChoice == 0) // back
			return;
		else
		{
			charElement = getCharElement(standardDigits);
			if (charElement == -1) // character not found
				continue;

			if (menuChoice < 3) // replace and insert/append
			{
				string newDigits,
					testDigits = standardDigits;
				cout << "\n New digits: ";
				cin >> newDigits;

				if (menuChoice == 1) // replace
					testDigits.replace(charElement, newDigits.length(), newDigits);
				else // insert/append
					testDigits.insert(charElement + 1, newDigits);

				if (invalidNewDigit(testDigits))
					continue;

				standardDigits = testDigits;
			}
			else // delete
			{
				int eraseCount;

				do
				{
					cout << "\n Number of digits to delete: ";
					cin >> eraseCount;
				} while (invalid_int_input());

				standardDigits.erase(charElement, eraseCount);
			}
		}
		
		saveSettings(standardDigits, endDigitCount);
	}
}

bool invalidNewDigit(string testDigits) // validate the new standard digits
{
	string reservedDigits = ".,-<> ";

	for (int i = 0; i < testDigits.length(); i++)
	{
		// check newDigits for reserved digits
		if (reservedDigits.find(testDigits[i]) != string::npos)
		{
			cout << "\n Error: digit reserved for other uses: " << testDigits[i];
			return true;
		}

		// make sure none of the digits are used more than once
		for (int j = i + 1; j < testDigits.length(); j++)
		{
			if (testDigits[i] == testDigits[j])
			{
				cout << "\n Error: digit used more than once: " << testDigits[i];
				return true;
			}
		}
	}

	return false;
}

int getCharElement(string standardDigits)
{
	char selectChar;
	cout << "\n Digit to start editing from: ";
	cin >> selectChar;

	for (int i = 0; i < standardDigits.length(); i++)
	{
		if (standardDigits[i] == selectChar)
			return i;
	}

	cout << "\n Digit not found.\n";
	return -1;
}

void listDigits(string standardDigits)
{
	int j = 0;
	cout << "\n Value of each standard form digit: \n";

	for (int i = 0; i < standardDigits.length(); i++)
	{
		cout << ' ' << standardDigits[i] << ':' << i;

		if (i % 15 == 0 && i != 0)
			cout << '\n';
	}
}

void help(string standardDigits)
{
	cout << "\n Do not use commas except when entering a number in numerals-only form."
		"\n Numerals-only form is a system for representing numbers of any base using"
		"\n only numerals (0-9). In this form, each digit of a number is separated by"
		"\n a comma (or a period if the number is fractional), but each digit can have"
		"\n multiple numerals. Numbers of a base greater than the number of standard"
		"\n form digits must be entered in numerals-only form. Feedback welcome and"
		"\n more info available at github.com/wheelercj/Radix-Converter\n";
}
