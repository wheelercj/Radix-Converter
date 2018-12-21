#ifndef RADIX_H
#define RADIX_H

#include <string>
#include <vector>

void setStandard(std::string);

struct Vectors
{
	std::vector<int> whole, fraction;
	int size();
	int operator[](int);
};

std::string changeBase(std::string, std::string, std::string);
int charToInt(char);
Vectors splitNumeralsString(const std::string);
void strToInts(const std::string, std::vector<int>&, int&, int&);
Vectors splitString(std::string);
int lcm(int, int);
int gcd(int, int);

class Digit
{
private:
	int digit;
	int base; // this program supports numbers of mixed bases
public:
	Digit();
	Digit(int, int);
	int get();
	void set(int);
	int getBase();
	void setBase(int);
};

class Part  // either the whole or fractional part of the number
{
protected:
	std::vector<Digit> part;
public:
	Part();
	~Part();
	Digit operator[](int); // goes through the Number's Digits starting from the digit closest to the period
	int size();
	std::vector<Digit>::iterator begin();
	std::vector<Digit>::iterator end();
	void addPart(std::vector<int>, std::vector<int>);
	void addDigit(int, int);
};

class Whole : public Part
{
private:
	unsigned long long toDecimal();
	void fromDecimal(unsigned long long, std::vector<int>);
public:
	void changeBase(std::vector<int>);
};

class Fraction : public Part
{
private:
	void toDecimal();
	void fromDecimal(std::vector<int>);
public:
	void changeBase(std::vector<int>);
};

class Number
{
private:
	Whole whole;
	Fraction fraction;
	int resultPrecision;
	bool negative;
	bool numeralsOnly;
	Digit operator[](int); // goes through the Number's Digits in reading order
	bool detectNumerals(std::string, Vectors);
public:
	Number();
	void set(std::string, std::string);
	int size();
	void changeBase(std::string);
	std::string toString();
	void setPrecision(int);
};

#endif // !RADIX_H
