#ifndef __cxxtest__Root_cpp__
#define __cxxtest__Root_cpp__

//
// This file holds the "root" of CxxTest, i.e.
// the parts that must be in a source file file.
//

#include <cxxtest/Descriptions.cpp>
#include <cxxtest/DummyDescriptions.cpp>
#include <cxxtest/GlobalFixture.cpp>
#include <cxxtest/LinkedList.cpp>
#include <cxxtest/RealDescriptions.cpp>
#include <cxxtest/TestSuite.cpp>
#include <cxxtest/TestTracker.cpp>
#include <cxxtest/ValueTraits.cpp>


template char* CxxTest::numberToString<double>(double, char*, double, unsigned int, unsigned int);
template char* CxxTest::numberToString<unsigned int>(unsigned int, char*, unsigned int, unsigned int, unsigned int);
template char* CxxTest::numberToString<unsigned long>(unsigned long, char*, unsigned long, unsigned int, unsigned int);
template std::basic_ostream<char, std::char_traits<char> >& std::operator<< <std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&, char const*);
template void CxxTest::doAssertDelta<float, float, float>(char const*, unsigned int, char const*, float, char const*, float, char const*, float, char const*);
template bool CxxTest::delta<float, float, float>(float, float, float);


#endif // __cxxtest__Root_cpp__
