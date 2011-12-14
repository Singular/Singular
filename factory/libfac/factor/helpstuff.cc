////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
// $Id$
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include "tmpl_inst.h"
// some CC's need this:
#include "helpstuff.h"

bool
mydivremt ( const CanonicalForm& f, const CanonicalForm& g, CanonicalForm& a, CanonicalForm& b )
{
  bool retvalue;
  CanonicalForm aa,bb;
  retvalue = divremt(f,g,a,bb);
  aa= f-g*a;
  if ( aa==bb ) { b=bb; }
  else { b=aa; }
  return retvalue;
}

// Now some procedures used in SqrFree and in Factor
///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
// We have to include a version of <CFFList>.append(CFFactor)//
// and Union( CFFList, CFFList)                              //
// because we have to look for multiplicities in SqrFree.    //
// e.g.: SqrFree( f^3 ) with char <> 3                       //
///////////////////////////////////////////////////////////////
CFFList
myappend( const CFFList & Inputlist, const CFFactor & TheFactor)
{
  CFFList Outputlist ;
  CFFactor copy;
  CFFListIterator i;
  int exp=0;

  for ( i=Inputlist ; i.hasItem() ; i++ )
  {
    copy = i.getItem();
    if ( copy.factor() == TheFactor.factor() )
      exp += copy.exp();
    else
      Outputlist.append(copy);
  }
  Outputlist.append( CFFactor(TheFactor.factor(), exp + TheFactor.exp()));
  return Outputlist;
}

CFFList
myUnion(const CFFList & Inputlist1,const CFFList & Inputlist2)
{
  CFFList Outputlist;
  CFFListIterator i;

  for ( i=Inputlist1 ; i.hasItem() ; i++ )
    Outputlist = myappend(Outputlist, i.getItem() );
  for ( i=Inputlist2 ; i.hasItem() ; i++ )
    Outputlist = myappend(Outputlist, i.getItem() );

  return Outputlist;
}

int
Powerup( const int base , const int exp)
{
  int retvalue=1;
  if ( exp == 0 )  return retvalue ;
  else for ( int i=1 ; i <= exp; i++ ) retvalue *= base ;

  return retvalue;
}

// Now some procedures used in MVMultiHensel and in Truefactors
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// mod_power: Return f mod I^k, where I is now the ideal     //
// {x_1, .. x_(level(f)-1)}. Computation mod I^k is simply   //
// done by dropping all the terms of degree >= k in          //
// x_1, .. x_(level(f)-1); e.g. x_1*x_2 == 0 mod I^2 .       //
// modpower: the real work is done here; called by mod_power //
///////////////////////////////////////////////////////////////
static void
modpower( const CanonicalForm & f, int k , int td,
          const CanonicalForm & t, CanonicalForm & result)
{

  if ( td >= k ) return;
  if ( getNumVars(f) == 0 ) result += f*t;
  else{
    Variable x(level(f));
    for ( CFIterator i=f; i.hasTerms(); i++)
      modpower(i.coeff(),k,td+i.exp(),t*power(x,i.exp()),result);
  }
}

CanonicalForm
mod_power( const CanonicalForm & f, int k, int levelU)
{
  CanonicalForm result,dummy;
  Variable x(levelU);

  if ( levelU > level(f) )
    modpower(f,k,0,1,result);
  else
  {
    for ( CFIterator i=f; i.hasTerms(); i++)
    {
      dummy = 0;
      modpower(i.coeff(),k,0,1,dummy);
      result += dummy * power(x,i.exp());
// the following works, but is slower
//    int degf=degree(f);
//    for ( int i=0; i<=degf;i++){
//      result+= mod_power(f[i],k,levelU)*power(x,i);
    }
  }

  return result;
}

///////////////////////////////////////////////////////////////
// Return the deg of F in the Variables x_1,..,x_(levelF-1)  //
///////////////////////////////////////////////////////////////
int
subvardegree( const CanonicalForm & F, int levelF )
{
  int n=0,m=degree(F,levelF),newn=0;

  for ( int k=0; k<=m; k++ )
  {
    newn = totaldegree( F[k] );
    if ( newn > n ) n=newn;
  }
  return n;
}

///////////////////////////////////////////////////////////////
// Change poly:  x_i <- x_i +- a_i    for i= 1,..,level(f)-1 //
///////////////////////////////////////////////////////////////
CanonicalForm
change_poly( const CanonicalForm & f , const SFormList & Substitutionlist ,int directionback )
{
  CanonicalForm F=f,g,k;
  int level_i;
//  Variable x;

  for ( SFormListIterator i=Substitutionlist; i.hasItem(); i++)
  {
  // now we can access: i.getItem().factor()  -> level(~) gives x_i
  //                    i.getItem().exp()     -> gives a_i
  // ==> g = x_i ; k = a_i
    level_i=level(i.getItem().factor());
    g = power(
                       Variable(level_i),1
                       );
    k= i.getItem().exp();
    if ( directionback )
    {
      if ( degree(F, level_i) != 0 )
        F=F(g-k, level_i /*level(i.getItem().factor())*/); // x_i <-- x_i - a_i
    }
    else
    {
      if ( degree(F, level_i) != 0 )
        F=F(g+k, level_i /*level(i.getItem().factor())*/); // x_i <-- x_i +a_i
    }
  }

  return F;
}
