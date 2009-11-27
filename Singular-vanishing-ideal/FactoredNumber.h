#ifndef FACTORED_NUMBER_H
#define FACTORED_NUMBER_H

#ifdef HAVE_VANISHING_IDEAL

#include <list>

class FactoredNumber
{
  private:
    std::list<int> m_primes;    // prime factors sorted in ascending order
    std::list<int> m_exponents; // corresponding exponents in prime factorization
    static void insertPrime (std::list<int>& primes, std::list<int>& exponents, const int p);
  public:
    FactoredNumber (const int n);
    FactoredNumber (const int p, const int exponent);
    ~FactoredNumber ();
    FactoredNumber& operator* (const FactoredNumber& fn);
    FactoredNumber& cancel (const FactoredNumber& fn);
    FactoredNumber& operator= (const FactoredNumber& fn);
    FactoredNumber& gcd (const FactoredNumber& fn);
    FactoredNumber& lcm (const FactoredNumber& fn);
    bool divides (const FactoredNumber& fn);
    FactoredNumber& smarandache (const int n);
    int getInt ();
    char* toString ();
    void print ();
    void printLn ();
};

#endif
/* HAVE_VANISHING_IDEAL */

#endif
/* FACTORED_NUMBER_H */