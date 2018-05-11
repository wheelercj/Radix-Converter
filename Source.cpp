#include <iostream>
#include <string>
#include <cmath>     // pow()
#include <algorithm> // reverse()
#include <sstream>   // ostringstream
using namespace std;

class InvalidDigit {};
class Overflow {};
int digit;
int convertCharToInt(char, int);
bool zeroString(string);

int main()
{
	string str,
		mantissa; // the fractional part of a number
	const int arraySize = 50, // the number of digits or numerals of the mantissa and/or the whole part of a numerals-only number. Truncation occurs during calculations.
		endMantissaDigits = 20, // the number of mantissa digits that will be printed. Truncation occurs after calculations.
		maxNormalBase = 36; // the greatest base that can be printed in non-numerals-only form.
	int startBase,
		endBase,
		again;
	bool negative;

	cout << "\n Only use commas when entering a number in numerals-only form."
		"\n Numbers of a base greater than " << maxNormalBase << " must be entered in numerals-only form, e.g."
		"\n 5,24.30 (base 60) = 5 * 60^1 + 24 * 60^0 + 30 * 60^-1 (base 10) = 324.5 (base 10).";

	while (true)
	{
		do
		{
			cout << "\n Starting base: ";
			cin >> startBase;
			cout << " Ending base: ";
			cin >> endBase;

			if (startBase < 2 || endBase < 2 || cin.fail())
			{
				cout << "\n Invalid base entered. Please enter the bases in number form.\n";
				cin.ignore(numeric_limits<int>::max(), '\n');
				cin.clear();
			}
			else
				break;
		} while (true);

		again = 1;
		while (again != 2)
		{
			int numerals[arraySize] = { 0 },
				mantissaNumerals[arraySize] = { 0 };
			unsigned long long tempNum = 0;
			bool approximate = false,
				numeralsOnlyStart = false,
				numeralsOnlyEnd = false;

			try
			{
				cout << "\n Starting number (base " << startBase << "): ";
				cin >> str;

				// remove any spaces
				for (int i = 0; i < str.length(); i++)
					if (str[i] == ' ')
						str.erase(i, 1);

				// remove any preceding positive sign
				if (str[0] == '+')
					str.erase(0, 1);

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

				// numerals-only form?
				if (startBase > maxNormalBase)
					numeralsOnlyStart = true;
				if (endBase > maxNormalBase)
					numeralsOnlyEnd = true;
				for (int i = str.length() - 1; i >= 0; i--) // allow entering numbers of low bases in numerals-only form
				{
					if (str[i] == ',')
					{
						numeralsOnlyStart = true;
						break;
					}
				} // numerals-only numbers of a base < maxNormalBase with only one piece (no comma) will not be treated appropriately. Suggest typing a zero and comma before the number?

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

				if (str.empty())
					str.resize(1, '0');

				if (numeralsOnlyStart) // make each piece a separate int
				{
					int j = 0;
					for (int i = str.length() - 1; i >= 0; i--)
					{
						if ((str[i] > '9' || str[i] < '0') && str[i] != ',')
						{
							digit = str[i];
							throw InvalidDigit();
						}

						if (str[i] == ',')
						{
							if (i < str.length() - 1)
								numerals[j] = stoi(str.substr(i + 1, string::npos));
							str.erase(i, string::npos);
							j++;
							if (j == arraySize)
								throw Overflow();
						}
					}

					j = 0;

					for (int i = 0; i < mantissa.length(); i++)
					{
						if (mantissa[i] == ',')
						{
							numeralsOnlyStart = true;
							mantissaNumerals[j] = stoi(mantissa.substr(0, i)); // what if i=0 ?
							mantissa.erase(0, i + 1);
							j++;
						} // truncation?
					}
				}

				// convert non-decimal numbers to decimal
				if (startBase != 10)
				{
					if (!numeralsOnlyStart)
						for (int i = str.length() - 1; i >= 0; i--)
							tempNum += convertCharToInt(str[i], startBase) * (int)pow(startBase, str.length() - i - 1);
					else
						for (int i = 0; i < arraySize; i++)
							tempNum += numerals[i] * (int)pow(startBase, i);

					if (!mantissa.empty())
					{
						int tempArraySize,
							numerators[arraySize],
							denominators[arraySize],
							lcd,
							total = 0;

						if (!numeralsOnlyStart)
						{
							if (mantissa.length() > arraySize) // truncate long numbers to prevent errors
							{
								mantissa.erase(arraySize, string::npos);
								approximate = true;
							}

							// when converting a non-decimal mantissa to decimal, each digit becomes fractional. To avoid rounding
							// errors again, store the numerators and denominators of each digit seperately while converting
							for (int i = 0; i < mantissa.length(); i++)
							{
								numerators[i] = convertCharToInt(mantissa[i], startBase);
								denominators[i] = (int)pow(startBase, i + 1);
							}

							tempArraySize = mantissa.length();
						}
						else
						{
							for (int i = 0; i < arraySize; i++)
							{
								numerators[i] = numerals[i];
								denominators[i] = (int)pow(startBase, i + 1);
							}

							// find the number of elements used
							for (int i = arraySize - 1; i >= 0; i--)
								if (numerators[i] != 0)
									tempArraySize = i + 1;
						}

						// the last denominator is the lowest common denominator of all the fractions
						lcd = denominators[tempArraySize - 1];

						// multiply the numerators so that they all have the same denominator (the lcd) and find their sum
						for (int i = 0; i < tempArraySize; i++)
							total += lcd / denominators[i] * numerators[i];

						// divide the total by the lcd to get the mantissa in decimal, but in floating point form
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
						digit = tempNum % endBase;
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
								digit = (tempMantissa[j] - '0') * endBase + carry;

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
							if (i == endMantissaDigits && !zeroString(tempMantissa))
							{
								approximate = true;
								break;
							}
						}
					}
				}
				else
				{
					str = to_string(tempNum);
					if (mantissa.length() >= endMantissaDigits)
					{
						mantissa.erase(endMantissaDigits, string::npos);
						approximate = true;
					}
				}

				if (negative)
					str.insert(0, "-");

				if (endBase <= maxNormalBase)
				{
					cout << "\n  = " << str;
					if (!mantissa.empty())
						cout << '.' << mantissa;
					cout << " (base " << endBase << ')';
					if (approximate)
						cout << " approx.";
				}
			}
			catch (InvalidDigit)
			{
				cout << "\n\n ERROR: Invalid digit entered: " << digit;
			}
			catch (Overflow)
			{
				cout << "\n\n ERROR: Overflow.";
			}
			/*catch (...)
			{
				cout << "\n\n ERROR \n ";
			}*/

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
	digit = toupper(ch);

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