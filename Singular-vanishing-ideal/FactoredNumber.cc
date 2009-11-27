#include "mod2.h"

#ifdef HAVE_VANISHING_IDEAL

#include "factory.h"
#include <list>
#include "FactoredNumber.h"

using namespace std;

void FactoredNumber::insertPrime (list<int>& primes, list<int>& exponents, const int p)
{
  list<int>::iterator itP = primes.begin();
  list<int>::iterator itExp = exponents.begin();
  while ((itP != primes.end()) && (*itP < p))
  {
    itP++;
    itExp++;
  }
  if (*itP == p)
  {
    /* increment exponent of p at current position */
    int exp = *itExp;
    itExp = exponents.erase(itExp);
    exponents.insert(itExp, exp + 1);
  }
  else
  {
    /* we have itP == primes.end();
       insert p^1 at the end */
    primes.insert(itP, p);
    exponents.insert(itExp, 1);
  }
}

/* assumes n >= 1 */
FactoredNumber::FactoredNumber (const int n)
{
  m_primes.clear();
  m_exponents.clear();
  int nn = n;
  int i = 0;
  int p;
  while (nn != 1)
  {
    if (nn % 2 == 0)
    {
      insertPrime(m_primes, m_exponents, 2);
      nn = nn / 2;
    }
    else
    {
      i = 0;
      p = cf_getSmallPrime(0);
      while (nn % p != 0) {
        i++;
        p = cf_getSmallPrime(i);
      }
      insertPrime(m_primes, m_exponents, p);
      nn = nn / p;
    }
  }
}

FactoredNumber::FactoredNumber (const int p, const int exponent)
{
  m_primes.clear();
  m_exponents.clear();
  m_primes.insert(m_primes.begin(), p);
  m_exponents.insert(m_exponents.begin(), exponent);
}

FactoredNumber::~FactoredNumber ()
{
  m_primes.clear();
  m_exponents.clear();
}

FactoredNumber& FactoredNumber::operator* (const FactoredNumber& fn)
{
  return *this;
}

FactoredNumber& FactoredNumber::cancel (const FactoredNumber& fn)
{
  return *this;
}

FactoredNumber& FactoredNumber::gcd (const FactoredNumber& fn)
{
  return *this;
}

FactoredNumber& FactoredNumber::lcm (const FactoredNumber& fn)
{
  return *this;
}

FactoredNumber& FactoredNumber::operator= (const FactoredNumber& fn)
{
  return *this;
}

bool FactoredNumber::divides (const FactoredNumber& fn)
{
  return false;
}

int FactoredNumber::getInt ()
{
  return 0;
}

char* FactoredNumber::toString ()
{
  char* h = new char[1000];
  if (m_primes.size() == 0)
  {
    strcpy(h, "1");
  }
  else
  {
    strcpy(h, "");
    list<int>::iterator itP;
    list<int>::iterator itExp = m_exponents.begin();
    bool firstTime = true;
    for (itP = m_primes.begin(); itP != m_primes.end(); itP++)
    {
      if (!firstTime) strcat(h, " * ");
      char s[10];
      sprintf(s, "%d^%d", *itP, *itExp);
      strcat(h, s);
      itExp++;
      firstTime = false;
    }
  }
  return h;
}

FactoredNumber& FactoredNumber::smarandache (const int n)
{
  return *this;
}

void FactoredNumber::print ()
{
  char* temp = this->toString();
  printf("%s", temp);
  delete temp;
}

void FactoredNumber::printLn ()
{
  char* temp = this->toString();
  printf("%s\n", temp);
  delete temp;
}

#endif
/* HAVE_VANISHING_IDEAL */