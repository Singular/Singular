#include <omalloc/omalloc.h>
#include <misc/auxiliary.h>

#include <coeffs/coeffs.h>
#include <coeffs/numbers.h>

#include <reporter/reporter.h>
#include <resources/feResource.h>

#ifdef HAVE_FACTORY
int initializeGMP(){ return 1; }
#endif

#include <polys/monomials/ring.h>
#include <polys/monomials/p_polys.h>


BOOLEAN Test(const n_coeffType type, void* param)
{
  BOOLEAN ret = TRUE;

  const coeffs r = nInitChar( type, param );

  if( r == NULL )
  {
    PrintS( "Test: could not get this coeff. domain" ); PrintLn();
    return FALSE;
  }

  assume( r->cfCoeffWrite != NULLp );

  if( r->cfCoeffWrite != NULL )
  {
    PrintS( "Coeff-domain: " ); n_CoeffWrite(r); PrintLn();
  }

  number t = n_Init(1, r);  
  ndInpAdd(t, t, r);  
  
  number two = n_Init(2, r);
  assume(n_Equal(two, t, r));
  ret = ret && n_Equal(two, t, r);
  n_Delete(&t, r);
  n_Delete(&two, r);
  

  const int N = 2; // number of vars
  char* n[N] = {"x", "y"}; // teir names
  
  ring R = rDefault( r, N, n);  // now r belongs to R!

  if( R == NULL )
  {
    PrintS( "Test: could not get a polynomial ring over this coeff. domain" ); PrintLn();
    nKillChar( r );
    return FALSE;
  }


  rWrite(R); PrintLn();
  #define RDEBUG
  #ifdef  RDEBUG
//    rDebugPrint(R); PrintLn();
  #endif

  const int exp[N] = {5, 8};
  
  poly p = p_ISet(1, R);
  assume( p != NULL );
  assume(pNext(p) == NULL);
  ret = ret && (pNext(p) == NULL);

  p_SetExp(p,1,exp[0],R);
  p_SetExp(p,2,exp[1],R);
  p_Setm(p, R);

  assume( p_GetExp(p, 1, R) == exp[0] );
  assume( p_GetExp(p, 2, R) == exp[1] );

  p = p_Add_q(p_Copy(p, R), p, R);  

  ret = ret && p_EqualPolys(p, p, R);

  
  poly p2 = p_ISet(2, R);
  assume( p2 != NULL );
  assume(pNext(p2) == NULL);
  ret = ret && (pNext(p2) == NULL);
  
  p_SetExp(p2,1,exp[0],R);
  p_SetExp(p2,2,exp[1],R);
  p_Setm(p, R);

  assume( p_GetExp(p2, 1, R) == exp[0] );
  assume( p_GetExp(p2, 2, R) == exp[1] );

  number s = p_GetCoeff(p, R);
  two = n_Init(2, r);
  assume(n_Equal(two, s, r));
  ret = ret && n_Equal(s, two, r);  
  n_Delete(&two, r);

  assume(p_EqualPolys(p2, p, R));
  ret = ret && p_EqualPolys(p, p, R);

  p_Delete(&p, R);
  
  p_Delete(&p2, R);


  rDelete(R);

  return ret;
}


BOOLEAN simple(const n_coeffType _type, cfInitCharProc p, void* param = NULLp)
{
  n_coeffType type = nRegister( _type, p);
  assume( type == _type ); // ?
  return ( Test(type, param) );
}

int main( int, char *argv[] ) 
{
  feInitResources(argv[0]);

  StringSetS("ressources in use (as reported by feStringAppendResources(0):\n");
  feStringAppendResources(0);
  PrintS(StringAppendS("\n"));

  extern BOOLEAN nlInitChar(coeffs, void*);
  extern BOOLEAN npInitChar(coeffs, void*);


  if( simple(n_Q, nlInitChar) )
    PrintS("Q: Test Passed!");
  else 
    PrintS("Q: Test: Failed!");
  PrintLn();

  if( simple(n_Zp, npInitChar, (void*)7) )
    PrintS("Zp: Test Passed!");
  else 
    PrintS("Zp: Test: Failed!");
  PrintLn();
}