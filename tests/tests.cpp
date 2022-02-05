#include "pch.h"
#include "CppUnitTest.h"
#include "../Radix-Converter/radix.h"
#include "../Radix-Converter/radix.cpp"
#include "../Radix-Converter/main.cpp"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace tests
{
    void equal(std::string startBase, std::string endBase, std::string startNum, std::string endNum)
    {
        Assert::AreEqual(ToString(endNum), ToString(changeBase(startNum, startBase, endBase)));
    }

    TEST_CLASS(converter)
    {
    public:

        TEST_METHOD(DecimalToDecimal)
        {
            equal("10", "10", "1234", "1234");
        }

        TEST_METHOD(DecimalToBinary)
        {
            equal("10", "2", "1341", "10100111101");
        }

        TEST_METHOD(DecimalToOctal)
        {
            equal("10", "8", "1341", "2475");
        }

        TEST_METHOD(DecimalToHexadecimal)
        {
            equal("10", "16", "1341", "53d");
        }

        TEST_METHOD(FractionalDecimalToHexadecimal)
        {
            equal("10", "16", "3315.3", "cf3.4cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc");
        }

        TEST_METHOD(FractionalOctalToDecimal)
        {
            equal("8", "10", "234.14", "156.1875");
        }

        TEST_METHOD(FractionalHexadecimalToDecimal)
        {
            equal("16", "10", "4b3.3", "1203.1875");
        }

        TEST_METHOD(BinaryToOctal)
        {
            equal("2", "8", "10110", "26");
        }

        TEST_METHOD(BinaryToHexadecimal)
        {
            equal("2", "16", "10110110", "b6");
        }

        TEST_METHOD(DecimalToBase7)
        {
            equal("10", "7", "513.03", "1332.013201320132013201320132013201320132013201320132013201320132013201320132013");
        }

        TEST_METHOD(MixedTimeNumeralsToMinutes)
        {
            equal("60", "10", "5,48.30", "348.5");
        }

        TEST_METHOD(NegativeDecimalToBase9)
        {
            equal("10", "9", "-15", "-16");
        }

        TEST_METHOD(NegativeFractionalMixedBaseToDecimal)
        {
            equal("10,10,16,20,5.16,10,10", "10", "-87eg4.c71", "-90908.821");
        }

        TEST_METHOD(AntiprimeNumeralsToDecimal)
        {
            equal("5040", "10", "3,2,6", "76214886");
        }

        TEST_METHOD(Base64NumeralsToBase36)
        {
            equal("64", "36", "35,12", "1qk");
        }

        TEST_METHOD(InvalidEndBase)
        {
            equal("10", "0", "9", "Error: invalid base entered.");
        }

        TEST_METHOD(InvalidStartBase)
        {
            equal("0", "10", "9", "Error: invalid base entered.");
        }

        TEST_METHOD(TooManyPeriods)
        {
            equal("10", "11", "5.5.5", "Error: too many periods entered.");
        }

        TEST_METHOD(NonStandardDigitUsed)
        {
            equal("5", "7", "32$4", "Error: non-standard digit used: \"$\"");
        }

        TEST_METHOD(NonNumeralsInNumeralsOnlyForm)
        {
            equal("16", "10", "3,A,3.5", "Error: expected only numerals but received \"A\"");
        }

        TEST_METHOD(UnaryToDecimal)
        {
            equal("1", "10", "37a", "20");
        }

        TEST_METHOD(NumeralsWithLeadingComma)
        {
            equal("16", "16", ",12.15", "c.f");
        }

        TEST_METHOD(NumeralsWithTrailingComma)
        {
            equal("16", "16", "12.15,", "c.f");
        }

        TEST_METHOD(IsInteger)
        {
            Assert::IsTrue(isInteger("387398"));
        }

        TEST_METHOD(IsNotInteger)
        {
            Assert::IsFalse(isInteger("387.398"));
        }
        
        TEST_METHOD(AssertInteger)
        {
            assertInteger("37983");
        }

        TEST_METHOD(AssertNotInteger)
        {
            auto f = [] { assertInteger("379B3"); };
            Assert::ExpectException<std::runtime_error>(f);
        }
    };

    TEST_CLASS(main)
    {
    public:
        
        TEST_METHOD(RemoveNoSpaces)
        {
            std::string actual = "nospaces";
            removeSpaces(actual);
            std::string expected = "nospaces";
            Assert::AreEqual(expected, actual);
        }

        TEST_METHOD(RemoveSpaces)
        {
            std::string actual = " s p a c e s ";
            removeSpaces(actual);
            std::string expected = "spaces";
            Assert::AreEqual(expected, actual);
        }

        TEST_METHOD(ValidStandardDigits)
        {
            Assert::IsTrue(validStandardDigits("9876543210"));
        }

        TEST_METHOD(InvalidStandardDigits)
        {
            Assert::IsFalse(validStandardDigits("01234567890123456789"));
        }
    };
}
