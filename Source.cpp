#include <iostream>
#include <string>
#include <vector>
#include <cmath>     // pow()
#include <algorithm> // reverse()
#include <boost/lexical_cast.hpp>
using namespace std;

class InvalidDigit {};
void readBases(int&, int&);
string readStartNum(int, bool&);
void detectNumeralsForm(string, int, int, int, bool&, bool&);
vector<unsigned long> stringToVector(string, int, bool, int&);
unsigned long long toDecimal(vector<unsigned long>, int);
void mantissaToDecimal(vector<unsigned long>&, int);
string fromDecimal(int, int, bool);
string mantissaFromDecimal(vector<unsigned long>, int, bool, int);
int charToInt(char, int);
char intToChar(int);
bool zeroVector(vector<unsigned long>);
void printResult(string, string, int, int);
void settings(int&);

int main()
{
	int startBase,
		endBase,
		again;
	string mantissa = ""; // the fractional part of a number
	int endDigitCount = 20, // the greatest number of digits that can be printed in the result. Truncation occurs after conversion.
		maxStandardBase = 62; // the greatest base the numbers of which can be represented in standard form.

	cout << "\n Only use commas when entering a number in numerals-only form. Numbers of a"
		"\n base greater than " << maxStandardBase << " must be entered in numerals-only form.\n";

	while (true)
	{
		readBases(startBase, endBase);

		again = 1;
		while (again != 2)
		{
			bool negative = false,
				numeralsStart = false,
				numeralsEnd = false;

			try
			{
				string str = readStartNum(startBase, negative);
				detectNumeralsForm(str, startBase, endBase, maxStandardBase, numeralsStart, numeralsEnd);

				// put the starting number into a vector
				int dot = -1;
				vector<unsigned long> vect = stringToVector(str, startBase, numeralsStart, dot),
					mantVect;

				// if there is a mantissa, store it in a different vector
				if (dot != -1)
				{
					mantVect.assign(vect.begin() + dot, vect.end());
					vect.erase(vect.begin() + dot, vect.end());
					reverse(vect.begin(), vect.end());
				}

				// convert both vectors to decimal, and put vect into an unsigned long long
				unsigned long long decimalNum = toDecimal(vect, startBase);
				if (!mantVect.empty() && startBase != 10)
					mantissaToDecimal(mantVect, startBase);

				// convert to a non-decimal base if desired
				if (endBase != 10 || numeralsEnd)
				{
					str = fromDecimal(decimalNum, endBase, numeralsEnd);
					if (!mantVect.empty())
						mantissa = mantissaFromDecimal(mantVect, endBase, numeralsEnd, endDigitCount);
				}
				else
				{
					str = to_string(decimalNum);

					// put mantVect into the mantissa string
					mantissa = "";
					for (int i = 0; i < mantVect.size(); i++)
					{
						mantissa.resize(i + 1);
						mantissa[i] = intToChar(mantVect[i]);
					}
				}

				if (str.empty())
					str = "0";
				if (negative)
					str.insert(0, "-");

				printResult(str, mantissa, endBase, endDigitCount);
			}
			catch (InvalidDigit)
			{
				cout << "\n ERROR: Invalid digit entered.";
			}

			do
			{
				cout << "\n\n Continue(1) / New Bases(2) / Settings(3) / Exit(0) > ";
				cin >> again;

				if (!again)
					exit(0);

				if (again == 3)
					settings(endDigitCount);

				if (cin.fail())
				{
					cin.clear();
					cin.ignore(numeric_limits<int>::max(), '\n');
				}
			} while (again < 1 || again > 2);
		}
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

		if (startBase < 1 || endBase < 1 || cin.fail())
		{
			cout << "\n Invalid base entered. Please enter the bases as numbers.\n";
			cin.clear();
			cin.ignore(numeric_limits<int>::max(), '\n');
		}
		else
			return;
	}
}

string readStartNum(int startBase, bool& negative)
{
	string str;
	cout << "\n Starting number (base " << startBase << "): ";
	cin >> str;

	// remove any spaces
	for (int i = 0; i < str.length(); i++)
	{
		if (str[i] == ' ')
			str.erase(i, 1);
	}

	// remove any trailing period
	if (str[str.length() - 1] == '.')
		str.erase(str.length() - 1, 1);

	// negative?
	if (str[0] == '-')
	{
		negative = true;
		str.erase(0, 1);
	}

	// hexadecimal prefix?
	if (startBase == 16 && str[0] == '0' && tolower(str[1]) == 'x')
		str.erase(0, 2);

	return str;
}

void detectNumeralsForm(string str, int startBase, int endBase, int maxStandardBase, bool& numeralsStart, bool& numeralsEnd)
{
	if (startBase > maxStandardBase)
		numeralsStart = true;
	if (endBase > maxStandardBase)
		numeralsEnd = true;

	// allow entering numbers of bases < maxNormalBase in numerals-only form, indicated by the use of one or more commas
	for (int i = str.length() - 1; i >= 0; i--)
	{
		if (str[i] == ',')
		{
			numeralsStart = true;
			break;
		}
	}
}

vector<unsigned long> stringToVector(string str, int startBase, bool numeralsStart, int& dot)
{
	vector<unsigned long> vect;
	int numeralCount = 0;

	for (int i = 0, j = 0; i < str.length(); i++)
	{
		if (!numeralsStart)
		{
			if (str[i] == '.')
			{
				j = i + 1;
				dot = i;
			}
			else
			{
				vect.push_back(charToInt(str[i], startBase));
			}
		}
		else
		{
			if (str[i] == '.')
			{
				vect.push_back(stoi(str.substr(j, i)));
				j = i + 1;
				dot = numeralCount + 1;
			}
			else if (str[i] == ',')
			{
				vect.push_back(stoi(str.substr(j, i)));
				j = i + 1;
				numeralCount++;
			}
			else if (str[i] > '9' || str[i] < '0')
			{
				throw InvalidDigit();
			}
			else if (i == str.length() - 1)
			{
				vect.push_back(stoi(str.substr(j, i)));
			}
		}
	}

	return vect;
}

unsigned long long toDecimal(vector<unsigned long> vect, int startBase)
{
	unsigned long long decimalNum = 0;

	for (int i = 0; i < vect.size(); i++)
		decimalNum += vect[i] * (int)pow(startBase, i);

	return decimalNum;
}

void mantissaToDecimal(vector<unsigned long>& mantVect, int startBase)
{
	vector<int> denominators;
	int	lcd,
		total = 0;

	// when converting a non-decimal mantissa to decimal, each digit becomes fractional. To avoid rounding
	// errors again, store the numerators and denominators of each digit seperately while converting
	for (int i = 0; i < mantVect.size(); i++)
		denominators.push_back((int)pow(startBase, i + 1));

	// the last denominator is the lowest common denominator of all the fractions
	lcd = denominators[mantVect.size() - 1];

	// multiply the numerators so that they all have the same denominator (the lcd) and find their sum
	for (int i = 0; i < mantVect.size(); i++)
		total += lcd / denominators[i] * mantVect[i];

	// divide the total by the lcd to get the mantissa in decimal, but in floating point form
	string temp = boost::lexical_cast<string>((double)total / lcd);

	// erase the preceding zero and the period
	temp.erase(0, 2);

	// erase any trailing zeros
	for (int i = temp.length() - 1; i >= 0; i--)
	{
		if (temp[i] != '0')
		{
			if (i < temp.length() - 1)
				temp.erase(i + 1, string::npos);
			break;
		}
	}

	// put the mantissa into mantVect and return by reference
	mantVect.clear();
	for (int i = 0; i < temp.length(); i++)
		mantVect.push_back(temp[i] - '0');
}

string fromDecimal(int decimalNum, int endBase, bool numeralsEnd)
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
			str[i] = intToChar(digit);
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

string mantissaFromDecimal(vector<unsigned long> mantVect, int endBase, bool numeralsEnd, int endDigitCount)
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
			mantissa[i] = intToChar(carry);
		}
		else
		{
			mantissa.append(to_string(carry));
			mantissa.append(",");
		}
	}

	// erase the trailing comma
	if (numeralsEnd)
		mantissa.erase(mantissa.length() - 1, 1);

	return mantissa;
}

int charToInt(char ch, int startBase)
{
	int digit = ch;

	if (digit >= 'A' && digit <= 'Z')
		digit -= 'A' - 10;
	else if (digit >= 'a' && digit <= 'z')
		digit -= 'a' - 36;
	else
		digit -= '0';

	if (digit >= startBase && startBase != 1)
		throw InvalidDigit();

	return digit;
}

char intToChar(int digit)
{
	if (digit > 35)
		return digit + 'a' - 36;
	else if (digit > 9)
		return digit + 'A' - 10;
	else
		return digit + '0';
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
	int digits = 0;

	cout << "\n  = ";

	for (int i = 0; i < str.length() && digits < endDigitCount; i++, digits++)
		cout << str[i];

	if (!mantissa.empty())
	{
		cout << '.';
		for (int i = 0; i < mantissa.length() && digits < endDigitCount; i++, digits++)
			cout << mantissa[i];
	}

	cout << " (base " << endBase << ')';
}

void settings(int& endDigitCount) //, int& maxStandardBase, string& standardDigits, bool& bothFormatsEnd)
{
	while (true)
	{
		cout << "\n Max number of digits shown in the result (default 20): ";
		cin >> endDigitCount;

		if (cin.fail())
		{
			cin.clear();
			cin.ignore(numeric_limits<int>::max(), '\n');
		}
		else
			break;
	}
}
