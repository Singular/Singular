/*!
 * \file F5cData.cc
 * \author Christian Eder
 * \brief This file includes the inlined methods of the classes \c CPair
 * and \c Label.
 * \details This file includes the inlined methods of the class \c CPair,
 * representing critical pairs in the F5C Algorithm, and the class \c Label,
 * representing the labels in the F5C Algorithm. In this file the handling of
 * these data types is defined.
 */

#include <kernel/mod2.h>

#ifdef HAVE_F5C
#include <kernel/kutil.h>
#include <kernel/structs.h>
#include <omalloc.h>
#include <kernel/polys.h>
#include <kernel/p_polys.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/kstd1.h>
#include <kernel/khstd.h>
#include <kernel/kbuckets.h>
#include <kernel/weight.h>
#include <kernel/intvec.h>
#include <kernel/pInline1.h>
#include <kernel/f5c.h>
#include <kernel/F5cData.h>
#include <kernel/F5cLists.h>


// IMPLEMENTATIONS OF METHODS OF CLASS LABEL


// constructor / destructor of class Label
Label::Label(int* expVec) {
  m_pExpVec       = expVec;
  m_nShortExpVec  = computeShortExpVec(m_pExpVec);
}

Label::~Label() {
  delete &m_nShortExpVec;
  delete [] m_pExpVec;
}



// MISC of class Label
unsigned long Label::computeShortExpVec(int* expVec) {
  //if (p == NULL) return 0;
  unsigned long ev = 0; // short exponent vector
  unsigned int n = BIT_SIZEOF_LONG / currRing->N; // number of bits per exp
  unsigned int m1; // highest bit which is filled with (n+1)
  unsigned int i = 0, j=1;

  if (n == 0) {
    if (currRing->N <2*BIT_SIZEOF_LONG) {
      n=1;
      m1=0;
    }
    else {
      for (; j<=(unsigned long) currRing->N; j++) {
        if (expVec[j] > 0) i++;
        if (i == BIT_SIZEOF_LONG) break;
      }
      if (i>0)
      ev = ~((unsigned long)0) >> ((unsigned long) (BIT_SIZEOF_LONG - i));
      return ev;
    }
  }
  else {
    m1 = (n+1)*(BIT_SIZEOF_LONG - n*currRing->N);
  }

  n++;
  while (i<m1) {
    ev |= getBitFields(expVec[j], i, n);
    i += n;
    j++;
  }

  n--;
  while (i<BIT_SIZEOF_LONG) {
    ev |= getBitFields(expVec[j], i, n);
    i += n;
    j++;
  }
  return ev;
}

unsigned long Label::getBitFields(int e, unsigned int s, unsigned int n) {
#define Sy_bit_L(x)     (((unsigned long)1L)<<(x))
  unsigned int i = 0;
  unsigned long  ev = 0L;
  assume(n > 0 && s < BIT_SIZEOF_LONG);
  do {
    assume(s+i < BIT_SIZEOF_LONG);
    if (e > (int) i) ev |= Sy_bit_L(s+i);
    else break;
    i++;
  }
  while (i < n);
  return ev;
}

// END IMPLEMENTATIONS OF METHODS OF CLASS LABEL

#endif
// HAVE_F5C
