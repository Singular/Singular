/**
 * @file cfGcdUtil.h
 *
 * coprimality check and change of representation mod n
**/

#ifndef CF_GCD_UTIL_H
#define CF_GCD_UTIL_H
bool
gcd_test_one ( const CanonicalForm & f, const CanonicalForm & g, bool swap, int & d );

CanonicalForm
balance_p ( const CanonicalForm & f, const CanonicalForm & q, const CanonicalForm & qh );

CanonicalForm
balance_p ( const CanonicalForm & f, const CanonicalForm & q );
#endif
