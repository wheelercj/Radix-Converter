//#include "stdafx.h"
//#include "CppUnitTest.h"
//#include "../Radix Converter/radix.h"
//#include "../Radix Converter/radix.cpp"
//using namespace Microsoft::VisualStudio::CppUnitTestFramework;
//
//namespace UnitTest1
//{
//	void equal(int a, int b, int c, int d)
//	{
//		Assert::AreEqual(ToString(a), ToString(changeBase(std::to_string(b), std::to_string(c), std::to_string(d))));
//	}
//
//	void equal(std::string a, std::string b, std::string c, std::string d)
//	{
//		Assert::AreEqual(ToString(a), ToString(changeBase(b, c, d)));
//	}
//
//	//template <class A, class B, class C, class D>
//	//void equal(A a, B b, C c, D d)
//	//{
//	//	// convert ints to std::strings, but a (the variable) can remain an int
//	//	std::string bs = (typeid(B) == typeid(int) ? std::to_string((int)b) : b),
//	//		cs = (typeid(C) == typeid(int) ? std::to_string((int)c) : c),
//	//		ds = (typeid(D) == typeid(int) ? std::to_string((int)d) : d);
//
//	//	Assert::AreEqual(ToString(a), ToString(changeBase(bs, cs, ds)));
//	//}
//
//	TEST_CLASS(UnitTest1)
//	{
//	public:
//		
//		TEST_METHOD(TestMethod1)
//		{
//			equal(0, 0, 10, 16);
//		}
//		TEST_METHOD(TestMethod2)
//		{
//			equal(0, 0, 16, 10);
//		}
//		TEST_METHOD(TestMethod3)
//		{
//			equal(0, 0, 1, 16);
//		}
//		TEST_METHOD(TestMethod4)
//		{
//			equal(0, 0, 2, 16);
//		}
//		TEST_METHOD(TestMethod5)
//		{
//			equal("F", "15", "10", "16");
//		}
//		TEST_METHOD(TestMethod6)
//		{
//			equal("15", "F", "16", "10");
//		}
//		TEST_METHOD(TestMethod7)
//		{
//			equal("Error: invalid base entered.", "9", "10", "0");
//		}
//		TEST_METHOD(TestMethod8)
//		{
//			equal("Error: invalid base entered.", "9", "0", "10");
//		}
//		TEST_METHOD(TestMethod9)
//		{
//			equal("Error: too many periods entered.", "5.5.5", "10", "11");
//		}
//		TEST_METHOD(TestMethod10)
//		{
//		}
//		TEST_METHOD(TestMethod11)
//		{
//		}
//		TEST_METHOD(TestMethod12)
//		{
//		}
//		TEST_METHOD(TestMethod13)
//		{
//		}
//		TEST_METHOD(TestMethod14)
//		{
//		}
//	};
//}
