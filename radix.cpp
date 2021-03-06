#include "radix.h"
#include <iostream>
#include <boost/lexical_cast.hpp> // Boost license: https://www.boost.org/LICENSE_1_0.txt
#include <numeric> // accumulate

// Numerals-only form: a number of a base greater than decimal may be represented with only
// numerals (0 - 9) by placing a comma between each digit so that each digit can be multiple
// numerals long. For example, the hexadecimal number 7A3F2 in standard form can be written
// as 7,10,3,15,2 in numerals-only form.

// In this program, bases must be entered in decimal, and if a number has digits of varying
// bases, those bases must be entered in numerals-only form.

std::string standardDigits = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
// const std::string reservedDigits = ".,- ";

void setStandard(std::string newDigits)
{
	standardDigits = newDigits;
}

std::string changeBase(std::string startNum, std::string startBase, std::string endBase)
{
	Number number;

	while (true)
	{
		try
		{
			number.set(startNum, startBase);
			number.changeBase(endBase);
			return number.toString();
		}
		catch (const char* error)
		{
			return error;
		}
		catch (std::bad_alloc)
		{
			return "Error: insufficient memory.";
		}
	}
}

int charToInt(char ch)
{
	std::size_t pos = standardDigits.find(ch);

	if (pos != std::string::npos)
		return pos;
	else
		throw "Error: non-standard digit used.";
}

Vectors splitNumeralsString(const std::string str) // copy each comma-separated set of numerals into separate ints
{
	Vectors vects;

	// split the part before the period, if there is one
	int i = 0, j = 0;
	strToInts(str, vects.whole, i, j);

	if (i == str.size())
	{
		vects.whole.push_back(stoi(str.substr(j, i - j)));
		return vects;
	}

	// split the part after the period, if there is one
	strToInts(str, vects.fraction, i, j);

	if (str[i] == '.')
		throw "Error: too many periods entered.";
	if (i == str.size())
		vects.fraction.push_back(stoi(str.substr(j, i - j)));
	return vects;
}

void strToInts(const std::string str, std::vector<int>& vect, int& i, int& j) // only for numerals-only form
{
	for (; i < str.size() && str[i] != '.'; i++)
	{
		if (str[i] == ',')
		{
			vect.push_back(stoi(str.substr(j, i - j)));
			j = i;
		}
	}
}

Vectors splitString(std::string str)
{
	Vectors vects;

	int i = 0;
	for (; i < str.size() && str[i] != '.'; i++)
		vects.whole.push_back(charToInt(str[i]));
	for (++i; i < str.size() && str[i] != '.'; i++)
		vects.fraction.push_back(charToInt(str[i]));
	if (i < str.size() && str[i] == '.')
		throw "Error: too many periods entered.";

	return vects;
}

int lcm(int a, int b)
{
	int temp = gcd(a, b);
	return temp ? (a / temp * b) : 0;
}

int gcd(int a, int b)
{
	while (true)
	{
		if (!a)
			return b;
		b %= a;
		if (!b)
			return a;
		a %= b;
	}
}

/*
	Vectors members
*/

int Vectors::size()
{
	return whole.size() + fraction.size();
}

int Vectors::operator[](int index)
{
	if (index >= whole.size())
		return fraction[index - whole.size() - 1];
	return whole[index];
}

/*
	Digit members
*/

Digit::Digit()
{
	digit = 0;
	base = 10;
}

Digit::Digit(int newDigit, int newBase)
{
	digit = newDigit;
	base = newBase;
}

int Digit::get()
{
	return digit;
}

void Digit::set(int newDigit)
{
	digit = newDigit;
}

int Digit::getBase()
{
	return base;
}

void Digit::setBase(int newBase)
{
	base = newBase;
}

/*
	Part members
*/

Part::Part()
{
}

Part::~Part()
{
}

Digit Part::operator[](int index)
{
	return part[index];
}

int Part::size()
{
	return part.size();
}

std::vector<Digit>::iterator Part::begin()
{
	return part.begin();
}

std::vector<Digit>::iterator Part::end()
{
	return part.end();
}

void Part::addPart(std::vector<int> number, std::vector<int> base)
{
	for (int i = 0, j = 0; i < number.size(); i++)
	{
		addDigit(number[i], base[j]);
		if (j != base.size() - 1)
			j++; // if there are more number digits than base digits, use the last base digit repeatedly
	}
}

void Part::addDigit(int newDigit, int newBase)
{
	part.push_back(Digit(newDigit, newBase));
}

/*
	Whole members
*/

unsigned long long Whole::toDecimal() // should have no effect on decimal numbers
{
	unsigned long long decimalNum = 0;

	for (int i = 0; i < part.size(); i++)
		decimalNum += part[i].get() * (long long)pow(part[i].getBase(), i);

	return decimalNum;
}

void Whole::fromDecimal(unsigned long long decimalNum, std::vector<int> newBase)
{
	part.clear();

	// Divide the decimal number by the target base repeatedly while storing the remainders of each division until the
	// decimal number is zero. The remainders in reverse order are the number in the target base.
	for (int i = 0; decimalNum > 0;)
	{
		int temp = decimalNum % newBase[i];
		decimalNum /= newBase[i];

		part.push_back(Digit(temp, newBase[i]));

		if (i != newBase.size() - 1)
			i++; // if there are more number digits than base digits, use the last base digit repeatedly
	}

	std::reverse(part.begin(), part.end());
}

void Whole::changeBase(std::vector<int> newBase)
{
	if (part.empty())
		return;

	// First change the number to decimal regardless of the start and target bases to simplify calculations.
	// The whole part is easier to temporarily work with as an integer datatype.
	unsigned long long decimalWhole = toDecimal();

	// Next, change the number to the target base
	fromDecimal(decimalWhole, newBase);
}

/*
	Fraction members
*/

void Fraction::toDecimal()
{
	// When converting the fractional part of a number to decimal, each digit becomes fractional. To avoid
	// rounding errors, store the numerators and denominators of each digit seperately while converting.
	// The numerators are what is already in Fraction::part.

	std::vector<int> denominators;

	for (int i = 0; i < size(); i++)
		denominators.push_back((int)pow(part[i].getBase(), i + 1));

	// find the lcd, which might not be the last denominator in a mixed-base number
	int	lcd = std::accumulate(denominators.begin(), denominators.end(), 0, lcm);

	// multiply the numerators so that they all have the same denominator (the lcd), and find their sum
	unsigned long long total = 0;
	for (int i = 0; i < size(); i++)
		total += lcd / denominators[i] * part[i].get();

	// divide the total by the lcd to get the fractional part in decimal
	Vectors vects = splitString(boost::lexical_cast<std::string>((long double)total / lcd));

	// copy the result back into Fraction::part
	part.clear();
	for (int i = 0; i < vects.fraction.size(); i++)
		part.push_back(Digit(vects.fraction[i], 10));
}

void Fraction::fromDecimal(std::vector<int> newBase)
{
	// For each digit of the fractional part from right to left, multiply by the target base and add the carry from the
	// previous digit if there is one. If the value of the digit goes above 9, carry all but the ones place to the next
	// digit. After this calculation is made for each digit, store the overall carry, or 0 if there isn't a carry, in
	// the next available element of a separate array. Repeat until the fractional part equals zero. The separate array
	// now holds the number in the target base.

	std::vector<Digit> newFraction;

	for (int i = 0; size(); i++)
	{
		int carry = 0;

		for (int j = size() - 1; j >= 0; j--)
		{
			int digit = part[j].get() * newBase[j] + carry;

			if (digit > 9)
			{
				carry = digit / 10;
				digit %= 10;
			}
			else
				carry = 0;

			part[j].set(digit);
		}

		newFraction.push_back(Digit(i + 1, newBase[i]));
	}

	part.clear();
	part = newFraction;
}

void Fraction::changeBase(std::vector<int> newBase)
{
	if (part.empty())
		return;

	// first, change the number to decimal regardless of the start and target bases to simplify calculations
	toDecimal();

	// then convert to the target base
	if (!(newBase.size() == 1 && newBase[0] == 10))
		fromDecimal(newBase);
}

/*
	Number members
*/

Digit Number::operator[](int index)
{
	int wholeSize = whole.size(); // the size of the whole part, not of the entire number

	if (index < wholeSize)
		return whole[index];
	else if (index < wholeSize + fraction.size())
		return fraction[index - wholeSize - 1];
	else
		throw "Error: subscript out of range.";
}

bool Number::detectNumerals(std::string startNum, Vectors bases)
{
	// numerals-only form indicated by the use of one or more commas in the starting number
	for (int i = 0; i < startNum.size(); i++)
	{
		if (startNum[i] == ',')
			return true;
	}

	// numerals-only form indicated by a base digit of a value greater than the max standard value
	for (int i = 0; i < bases.size(); i++)
	{
		if (bases[i] > standardDigits.length())
			return true;
	}

	return false;
}

Number::Number()
{
	resultPrecision = 20;
	negative = false;
	numeralsOnly = false;
}

void Number::set(std::string num, std::string base)
{
	// negative?
	negative = false;
	if (num[0] == '-')
	{
		negative = true;
		num.erase(0, 1);
	}

	// hexadecimal prefix?
	if (base == "16" && num[0] == '0' && tolower(num[1]) == 'x')
		num.erase(0, 2);

	// Prepare the base(s) to be entered into Digits by separating them into integers.
	// Bases must be entered in decimal, but a single number can have multiple bases
	// separated by commas (numerals-only form). If the number has more
	// digits than the base, the extra number digits will be in the closest base, e.g. if
	// startBase = "16", all digits of the starting number will be assumed to be in hexadecimal.
	Vectors bases = splitNumeralsString(base);

	// check for any invalid bases
	for (int i = 0; i < bases.size(); i++)
	{
		if (bases[i] < 1)
			throw "Error: invalid base entered.";
	}

	// numerals-only form?
	numeralsOnly = detectNumerals(num, bases);

	// split the number string into its whole and fractional parts
	Vectors vects;
	if (numeralsOnly)
		vects = splitNumeralsString(num);
	else
		vects = splitString(num);

	// convert all digits to Number Digits
	whole.addPart(vects.whole, bases.whole);
	fraction.addPart(vects.fraction, bases.fraction);

	// reverse the "whole" Part so that its indices coincide with the digit's power
	std::reverse(whole.begin(), whole.end());
}

int Number::size()
{
	return whole.size() + fraction.size();
}

void Number::changeBase(std::string newBase)
{
	Vectors newBases = splitNumeralsString(newBase);

	// check for any invalid bases
	for (int i = 0; i < newBases.size(); i++)
	{
		if (newBases[i] < 1)
			throw "Error: invalid base entered.";
	}

	// if one of the base vectors is empty, give it the closest digit from the other vector
	if (newBases.fraction.empty())
		newBases.fraction.push_back(newBases.whole[0]);
	else if (newBases.whole.empty())
		newBases.whole.push_back(newBases.fraction[0]);

	whole.changeBase(newBases.whole);
	fraction.changeBase(newBases.fraction);

	// if the whole part of the number is empty, assign zero to it
	if (!whole.size())
		whole.addDigit(0, 0);
}

std::string Number::toString()
{
	std::string temp = "";

	if (negative)
		temp += '-';
	
	// convert the digits to a string
	for (int i = 0; i < whole.size(); i++)
		temp += standardDigits[whole[i].get()];
	if (fraction.size())
	{
		temp += '.';
		for (int i = 0; i < fraction.size(); i++)
			temp += standardDigits[fraction[i].get()];
	}

	return temp;
}

void Number::setPrecision(int digitCount)
{
	resultPrecision = digitCount;
}
