#include <iostream>
#include <string>
#include <cmath>     // pow()
#include <algorithm> // reverse()
#include <sstream>   // ostringstream
using namespace std;

class InvalidDigit {};
int convertCharToInt(char, int);
bool zeroString(string);

int main()
{
	int startBase,
		endBase,
		again;
	string str,
		mantissa; // the fractional part of a number
	unsigned long long tempNum;
	bool negative;

	cout << "\n Base converter for any bases up to 36. \n";

	while (true)
	{
		do
		{
			cout << "\n Starting base: ";
			cin >> startBase;
			cout << " Ending base: ";
			cin >> endBase;

			if (startBase < 2 || startBase > 36 || endBase < 2 || endBase > 36 || cin.fail())
			{
				cout << "\n Invalid base entered. Please enter the bases within 2 to 36 in number form.\n";
				cin.ignore(numeric_limits<int>::max(), '\n');
				cin.clear();
			}
			else
				break;
		} while (true);

		again = 1;
		while (again != 2)
		{
			tempNum = 0;
			bool approximate = false;

			try
			{
				cout << "\n Starting number (base " << startBase << "): ";
				cin >> str;

				// negative number?
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

				// fractional?
				for (int i = 0; i < str.length(); i++)
				{
					if (str[i] == '.')
					{
						if (i < str.length() - 1) // split the string into two strings
							mantissa = str.substr(i + 1, string::npos); // mantissa stored as a whole number to avoid rounding errors
						str.erase(i, string::npos);
						break;
					}
				}
				
				// convert non-decimal numbers to decimal
				if (startBase != 10)
				{
					for (int i = str.length() - 1; i >= 0; i--)
						tempNum += convertCharToInt(str[i], startBase) * (int)pow(startBase, str.length() - i - 1);

					if (!mantissa.empty())
					{
						int numerators[100],
							denominators[100],
							lcd,
							total = 0;

						if (mantissa.length() >= 100)
							mantissa.erase(100, string::npos);

						// when converting a non-decimal mantissa to decimal, each digit becomes fractional. To avoid rounding
						// errors again, store the numerators and denominators of each digit seperately while converting
						for (int i = 0; i < mantissa.length(); i++)
						{
							numerators[i] = convertCharToInt(mantissa[i], startBase);
							denominators[i] = (int)pow(startBase, i + 1);
						}

						// the last denominator is the lowest common denominator of all the fractions
						lcd = denominators[mantissa.length() - 1];

						// multiply the numerators so that they all have the same denominator (the lcd)
						for (int i = 0; i < mantissa.length(); i++)
							numerators[i] = lcd / denominators[i] * numerators[i];

						// find the sum of the numerators
						for (int i = 0; i < mantissa.length(); i++)
							total += numerators[i];

						// divide the total by the lcd to get the mantissa in floating point form
						mantissa.clear();
						ostringstream temp;
						temp << (double)total / lcd;
						mantissa = temp.str();
						
						// erase the preceding zero and the period to make the mantissa a whole number again
						mantissa.erase(0, 2);

						// erase any trailing zeros
						for (int i = mantissa.length() - 1; i >= 0; i--)
						{
							if (mantissa[i] != '0')
							{
								if (i < mantissa.length() - 1)
									mantissa.erase(i + 1, string::npos);
								break;
							}
						}
					}
				}
				else
					tempNum = stoi(str);

				str.clear();

				// convert to non-decimal if desired
				if (endBase != 10)
				{
					for (int i = 0; tempNum > 0; i++)
					{
						int digit = tempNum % endBase;
						tempNum /= endBase;

						str.resize(i + 1);

						if (digit > 9)
							str[i] = digit + 'A' - 10;
						else
							str[i] = digit + '0';
					}

					reverse(str.begin(), str.end());

					if (!mantissa.empty())
					{
						string tempMantissa = mantissa;
						mantissa.clear();

						for (int i = 0; !zeroString(tempMantissa); i++)
						{
							int carry = 0;

							for (int j = tempMantissa.length() - 1; j >= 0; j--)
							{
								int digit = (tempMantissa[j] - '0') * endBase + carry;

								if (digit > 9)
								{
									carry = digit;
									digit %= 10;
									carry /= 10;
								}
								else
									carry = 0;

								tempMantissa[j] = digit + '0';
							}

							mantissa.resize(i + 1);

							if (carry > 9)
								mantissa[i] = carry + 'A' - 10;
							else
								mantissa[i] = carry + '0';

							// a number that is rational in one base can be irrational in another, so truncate long numbers
							if (i == 20 && !zeroString(tempMantissa))
							{
								approximate = true;
								break;
							}
						}
					}
				}
				else
					str = to_string(tempNum);
				
				if (str.empty())
					str.resize(1, '0');
				if (negative)
					str.insert(0, "-");

				cout << "\n  = " << str;
				if (!mantissa.empty())
					cout << '.' << mantissa;
				cout << " (base " << endBase << ')';
				if (approximate)
					cout << " approximation";
			}
			catch (InvalidDigit)
			{
				cout << "\n\n ERROR: Invalid digit(s) entered. \n ";
			}
			catch (...)
			{
				cout << "\n\n ERROR \n ";
			}

			do
			{
				cout << "\n\n Continue(1) / New Bases(2) / Exit(0) > ";
				cin >> again;

				if (!again)
					exit(0);
			} while (again != 1 && again != 2);

			str.clear();
			mantissa.clear();
		}
	}
}

int convertCharToInt(char ch, int startBase)
{
	int digit = toupper(ch);

	if (digit >= 'A' && digit <= 'Z')
		digit -= 'A' - 10;
	else
		digit -= '0';

	if (digit >= startBase)
		throw InvalidDigit();

	return digit;
}

bool zeroString(string str) // returns true if every element holds a zero
{
	for (int i = 0; i < str.length(); i++)
		if (str[i] != '0')
			return false;

	return true;
}