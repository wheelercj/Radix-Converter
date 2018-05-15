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
		maxStandardBase = 36; // the greatest base the numbers of which can be printed in standard form.
	int startBase,
		endBase,
		again;
	bool negative;

	cout << "\n Only use commas when entering a number in numerals-only form. Numbers of a"
		"\n base greater than " << maxStandardBase << " must be entered in numerals-only form, e.g."
		"\n 5,24.30 (base 60) = 5 * 60^1 + 24 * 60^0 + 30 * 60^-1 (base 10) "
		"\n\t\t\t\t\t\t = 324.5 (base 10).\n";

	while (true)
	{
		do
		{
			cout << "\n Starting base: ";
			cin >> startBase;
			cout << " Target base: ";
			cin >> endBase;

			if (startBase < 1 || endBase < 1 || cin.fail())
			{
				cout << "\n Invalid base entered. Please enter the bases as numbers.\n";
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
			bool approx = false,
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
				if (startBase > maxStandardBase)
					numeralsOnlyStart = true;
				if (endBase > maxStandardBase)
					numeralsOnlyEnd = true;
				for (int i = str.length() - 1; i >= 0; i--) // allow entering numbers of bases < maxNormalBase in numerals-only form
				{
					if (str[i] == ',')
					{
						numeralsOnlyStart = true;
						break;
					}
				} // numerals-only numbers of bases < maxNormalBase with no commas will not be treated correctly.
				  // Recommend typing a zero and a comma in front of the number.

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
					str = '0';

				if (numeralsOnlyStart) // make each piece a separate int
				{
					int j = 0;

					for (int i = str.length() - 1; i >= 0; i--)
					{
						if (str[i] == ',')
						{
							if (i < str.length() - 1)
								numerals[j] = stoi(str.substr(i + 1, string::npos));
							str.erase(i, string::npos);
							j++;
							if (j == arraySize)
								throw Overflow();
						}
						else if (str[i] > '9' || str[i] < '0')
						{
							digit = str[i];
							throw InvalidDigit();
						}
						else if (i == 0)
						{
							numerals[j] = stoi(str.substr(0, string::npos));
							str.erase(0, string::npos);
						}
					}

					j = 0;

					for (int i = 0; i < mantissa.length(); i++)
					{
						if (mantissa[i] == ',')
						{
							mantissaNumerals[j] = stoi(mantissa.substr(0, i)); // what if i = 0?
							mantissa.erase(0, i + 1);
							j++;
							if (j == arraySize)
							{
								approx = true;
								break;
							}
						}
						else if (mantissa[i] > '9' || mantissa[i] < '0')
						{
							digit = mantissa[i];
							throw InvalidDigit();
						}
						else if (i == mantissa.length() - 1)
						{
							mantissaNumerals[j] = stoi(mantissa.substr(0, i + 1));
							mantissa = "0"; // mantissa is not cleared here because the program would skip the mantissa conversion block in if(startBase != 10)
						}
					}
				}

				// convert non-decimal numbers to decimal
				if (startBase != 10 || (numeralsOnlyStart && startBase == 10))
				{
					if (!numeralsOnlyStart)
						for (int i = str.length() - 1; i >= 0; i--)
							tempNum += convertCharToInt(str[i], startBase) * (int)pow(startBase, str.length() - i - 1);
					else
						for (int i = 0; i < arraySize; i++)
							tempNum += numerals[i] * (int)pow(startBase, i);

					if (!mantissa.empty())
					{
						int tempArraySize = 0,
							numerators[arraySize],
							denominators[arraySize],
							lcd,
							total = 0;

						if (!numeralsOnlyStart)
						{
							if (mantissa.length() > arraySize)
							{
								mantissa.erase(arraySize, string::npos);
								approx = true;
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
							// find the number of elements used
							for (int i = arraySize - 1; i >= 0; i--)
							{
								if (mantissaNumerals[i] != 0)
								{
									tempArraySize = i + 1;
									break;
								}
							}

							for (int i = 0; i < tempArraySize; i++)
							{
								numerators[i] = mantissaNumerals[i];
								denominators[i] = (int)pow(startBase, i + 1);
							}
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
					tempNum = stoull(str);

				str = "";

				// convert to a non-decimal base if desired
				if (endBase != 10 || (numeralsOnlyEnd && endBase == 10)) // currently there is no way for both the second and third conditions to both be true
				{
					for (int i = 0; tempNum > 0; i++)
					{
						digit = tempNum % endBase;
						tempNum /= endBase;

						if (!numeralsOnlyEnd)
						{
							str.resize(i + 1);
							if (digit > 9)
								str[i] = digit + 'A' - 10;
							else
								str[i] = digit + '0';
						}
						else
						{
							str.insert(0, ",");
							str.insert(0, to_string(digit));
						}
					}

					if (!numeralsOnlyEnd)
						reverse(str.begin(), str.end());
					else
						str.erase(str.length() - 1, 1); // erase the trailing comma

					if (!mantissa.empty())
					{
						string tempMantissa = mantissa;
						mantissa = "";

						for (int i = 0; !zeroString(tempMantissa); i++)
						{
							int carry = 0;

							for (int j = tempMantissa.length() - 1; j >= 0; j--)
							{
								digit = (tempMantissa[j] - '0') * endBase + carry;

								if (digit > 9)
								{
									carry = digit / 10;
									digit %= 10;
								}
								else
									carry = 0;

								tempMantissa[j] = digit + '0';
							}

							if (!numeralsOnlyEnd)
							{
								mantissa.resize(i + 1);

								if (carry > 9)
									mantissa[i] = carry + 'A' - 10;
								else
									mantissa[i] = carry + '0';
							}
							else
							{
								mantissa.append(to_string(carry));
								mantissa.append(",");
							}

							// a number that is rational in one base can be irrational in another, so truncate long numbers
							if (i == endMantissaDigits && !zeroString(tempMantissa))
							{
								approx = true;
								break;
							}
						}

						if (numeralsOnlyEnd)
							mantissa.erase(mantissa.length() - 1, 1); // erase the trailing comma
					}
				}
				else
				{
					str = to_string(tempNum);
					if (mantissa.length() >= endMantissaDigits)
					{
						mantissa.erase(endMantissaDigits, string::npos);
						approx = true;
					}
				}

				if (str.empty())
					str = "0";
				if (negative)
					str.insert(0, "-");

				// print the result
				cout << "\n  = " << str;
				if (!mantissa.empty())
					cout << '.' << mantissa;
				cout << " (base " << endBase << ')';
				if (approx)
					cout << " approx.";
			}
			catch (InvalidDigit)
			{
				cout << "\n\n ERROR: Invalid digit entered: " << digit;
			}
			catch (Overflow)
			{
				cout << "\n\n ERROR: Overflow.";
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
	digit = toupper(ch);

	if (digit >= 'A' && digit <= 'Z')
		digit -= 'A' - 10;
	else
		digit -= '0';

	if (digit >= startBase && startBase != 1)
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
