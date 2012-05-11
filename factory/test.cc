#include <factory/factory.h>

int mmInit(void) {
#ifdef SINGULAR
// this is a hack to make linker baheave on Mac OS X 10.6.8 / 64 bit
// since otherwise both debug and release DYNAMIC tests failed there with:
/*
dyld: lazy symbol binding failed: Symbol not found: __Z7feFopenPKcS0_
Referenced from: ...BUILDDIR/factory/.libs/libfactory_g-3.1.3.dylib
Expected in: flat namespace
    
dyld: Symbol not found: __Z7feFopenPKcS0_
Referenced from: ...BUILDDIR/factory/.libs/libfactory_g-3.1.3.dylib
Expected in: flat namespace
*/
extern FILE * feFopen ( const char * path, const char * mode);
const int f = (int)(long)(void*)feFopen;
return (f ^ f) + 1; 
#else
return 1; 
#endif
}

int test2 (int p)
{
  int ret = 1;
  setCharacteristic (p);
  printf ("p: %d, i: %d", p, 0);
  CanonicalForm d1= Variable (1) + Variable (2) + 1;
  CanonicalForm f1= Variable (1) - Variable (2) - 2;
  CanonicalForm g1= Variable (1) + Variable (2) + 2;
  CanonicalForm d= power (d1, 2);
  CanonicalForm f= d* power (f1, 2);
  CanonicalForm g= d* power (g1, 2);
  CanonicalForm h= gcd (f,g);
  h /= Lc (h);
  printf (", h==d? %d\n", (h == d));
  if (h != d)
    ret = -1;
  for (int i= 3; i <= 11; i++)
  {
    printf ("p: %d, i: %d", p, i);
    d1 += power (Variable (i), i);
    f1 -= power (Variable (i), i);
    g1 += power (Variable (i), i);
    d= power (d1, 2);
    f= d*power (f1, 2);
    g= d*power (g1, 2);
    h= gcd (f,g);
    h /= Lc (h);
    printf (", h==d? %d\n", (h == d));
    if (h != d)
      ret = -1;
  }
  return ret;
}

/*int test5 (int p)
{
  setCharacteristic (p);
  Variable x= Variable (1);
  CanonicalForm d1= x + 1;
  CanonicalForm d2= Variable (2) + 1;
  CanonicalForm f1= x - 2;
  CanonicalForm f2= Variable (2) - 2;
  CanonicalForm g1= x + 2;
  CanonicalForm g2= Variable (2) + 2;
  CanonicalForm d= d1*d2 - 2;
  CanonicalForm f= f1*f2 + 2;
  CanonicalForm g= g1*g2 - 2;
  f *= d;
  g *= d;
  CanonicalForm h= gcd (f, g);
  h /= Lc (h);
  if (h != d)
    return - 1;
  printf ("h==d? %d\n", (h == d));
  for (int i= 3; i <= 11; i++)
  {
    d2 *= power (Variable (i), i) + 1;
    f2 *= power (Variable (i), i) - 2;
    g2 *= power (Variable (i), i) + 2;

    d= d1*d2 - 2;
    f= f1*f2 + 2;
    g= g1*g2 - 2;
    f *= d;
    g *= d;
    h= gcd (f, g);
    h /= Lc (h);
    if (h != d)
      return -1;
    printf ("h==d? %d\n", (h == d));
  }
  return 1;
}*/

extern void feInitResources(const char* argv0 = NULL);

int main( int, char *argv[] )
{
  int ret = 0;
  
  feInitResources(argv[0]);

//  On (SW_USE_EZGCD); On (SW_USE_EZGCD_P); // TODO&NOTE: these switches lead to failed tests (with nonzero p)!

  Off (SW_USE_EZGCD);
  Off (SW_USE_CHINREM_GCD);

  int t= test2 (0);
  if (t < 0)
    ret = t;
  /*t= test5 (0);
  if (t < 0)
    return t;
  t= test5 (3);
  if (t < 0)
    return t;
  t= test5 (101);
  if (t < 0)
    return t;
  t= test5 (43051);
  if (t < 0)
    return t;*/
  t= test2 (3);
  if (t < 0)
    ret += t;
  
  t= test2 (101);
  if (t < 0)
    ret += t;
  
  t= test2 (43051);
  if (t < 0)
    ret += t;
  
  return ret;
}
