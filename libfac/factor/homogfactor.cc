/* Copyright 1997 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// $Id: homogfactor.cc,v 1.11 2008-04-09 09:41:48 Singular Exp $ 
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
#ifdef HAVE_IOSTREAM
#include <iostream>
#define OSTREAM std::ostream
#define ISTREAM std::istream
#define CERR std::cerr
#define CIN std::cin
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define OSTREAM ostream
#define ISTREAM istream
#define CERR cerr
#define CIN cin
#endif

// Factor - Includes
#include "tmpl_inst.h"
#include "Factor.h"
// some CC's need it:
#include "homogfactor.h"

void out_cf(char *s1,const CanonicalForm &f,char *s2);

CFFList Factorize2(CanonicalForm F, const CanonicalForm & minpoly );

#ifdef HFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(hfactorize_time);

#if 0
///////////////////////////////////////////////////////////////
// get_Terms: Split the polynomial in the containing terms.  //
// getTerms: the real work is done here.                     // 
///////////////////////////////////////////////////////////////
static void 
getTerms( const CanonicalForm & f, const CanonicalForm & t, CFList & result ){

  if ( getNumVars(f) == 0 ) result.append(f*t);
  else{ 
    Variable x(level(f));
    for ( CFIterator i=f; i.hasTerms(); i++ )
      getTerms( i.coeff(), t*power(x,i.exp()), result);
  }
}

CFList 
get_Terms( const CanonicalForm & f ){
  CFList result,dummy,dummy2;
  CFIterator i;
  CFListIterator j;

  if ( getNumVars(f) == 0 ) result.append(f);
  else{
    Variable _x(level(f));
    for ( i=f; i.hasTerms(); i++ ){
      getTerms(i.coeff(), 1, dummy);
      for ( j=dummy; j.hasItem(); j++ )
	result.append(j.getItem() * power(_x, i.exp()));

      dummy= dummy2; // have to initalize new
    }
  }
  return result;
}
#endif

#if 0
///////////////////////////////////////////////////////////////
// is_homogeneous returns 1 iff f is homogeneous, 0 otherwise//
///////////////////////////////////////////////////////////////
bool
is_homogeneous( const CanonicalForm & f){
  CFList termlist= get_Terms(f);
  CFListIterator i;
  int deg= totaldegree(termlist.getFirst());

  for ( i=termlist; i.hasItem(); i++ )
    if ( totaldegree(i.getItem()) != deg ) return 0;
  return 1;
  // now: return f.isHomogeneous();
}
#endif

#if 0
///////////////////////////////////////////////////////////////
// get_max_degree_Variable returns Variable with             //
// highest degree. We assume f is *not* a constant!          //
///////////////////////////////////////////////////////////////
static Variable
get_max_degree_Variable(const CanonicalForm & f){
  ASSERT( ( ! f.inCoeffDomain() ), "no constants" );
  int max=0, maxlevel=0, n=level(f);
  for ( int i=1; i<=n; i++ )
    if (degree(f,Variable(i)) >= max) {
      max= degree(f,Variable(i)); maxlevel= i;
    }
  return Variable(maxlevel);
}
#endif

#if 0
///////////////////////////////////////////////////////////////
// homogenize homogenizes f with Variable x                  //
///////////////////////////////////////////////////////////////
static CanonicalForm
homogenize( const CanonicalForm & f, const Variable & x){
  CFList Newlist, Termlist= get_Terms(f); 
  int maxdeg=totaldegree(f), deg;
  CFListIterator i;
  CanonicalForm elem, result=f.genZero();
  
  for (i=Termlist; i.hasItem(); i++){
    elem= i.getItem();
    deg = totaldegree(elem);
    if ( deg < maxdeg )
      Newlist.append(elem * power(x,maxdeg-deg));
    else
      Newlist.append(elem);
  }
  for (i=Newlist; i.hasItem(); i++) // rebuild 
    result += i.getItem();
    
  return result;
}
#endif

// we assume g is square-free
CFFList
HomogFactor( const CanonicalForm & g, const CanonicalForm  & minpoly, const int Mainvar )
{
  DEBINCLEVEL(CERR, "HomogFactor");
  Variable xn = get_max_degree_Variable(g);
  //out_cf("HomogFactor:",g,"\n");
  int d_xn = degree(g,xn);
  CanonicalForm F = g(1,xn);

  DEBOUTLN(CERR, "xn= ", xn);
  DEBOUTLN(CERR, "d_xn=   ", d_xn);
  DEBOUTLN(CERR, "F= ", F);  
  //out_cf("HomogFactor:subst ",F,"\n");

  // should we do this for low degree polys g ? e.g. quadratic?
  // 
  CFFList Intermediatelist;
  CFFList Homoglist;
  CFFListIterator j;
  if ( getCharacteristic() > 0 )
  {
     CFMap n;
     if (minpoly.isZero())
       Intermediatelist = Factorize(compress(F,n), 1);
     else
       Intermediatelist = Factorized(compress(F,n), minpoly, Mainvar);
     for ( j=Intermediatelist; j.hasItem(); j++ )
       Homoglist.append(CFFactor( n(j.getItem().factor()), j.getItem().exp()) );
  }
  else
  {
     if (minpoly.isZero())
       Homoglist = factorize(F);
     else
       Homoglist = Factorize2(F,minpoly);
  }
  // Now we have uncompressed factors in Homoglist
  DEBOUTLN(CERR, "F factors as: ", Homoglist);
  CFFList Unhomoglist;
  CanonicalForm unhomogelem;
  if ((!(minpoly.isZero())) &&(getCharacteristic() == 0) )
  {
    for ( j=Homoglist; j.hasItem(); j++ )
    {
      // assume that minpoly.level() < level of all "real" variables of g
      DEBOUTLN(CERR, "Homogenizing ",j.getItem().factor()); 
      unhomogelem= homogenize(j.getItem().factor(),xn,
                         Variable(minpoly.level()+1),g.mvar());
      DEBOUTLN(CERR, "      that is ", unhomogelem);
      Unhomoglist.append(CFFactor(unhomogelem,j.getItem().exp()));
      d_xn -= degree(unhomogelem,xn)*j.getItem().exp();
    }
  }
  else
  {
    for ( j=Homoglist; j.hasItem(); j++ )
    {
      DEBOUTLN(CERR, "Homogenizing ",j.getItem().factor()); 
      unhomogelem= homogenize(j.getItem().factor(),xn);
      DEBOUTLN(CERR, "      that is ", unhomogelem);
      //out_cf("unhomogelem:",unhomogelem,"\n");
      Unhomoglist.append(CFFactor(unhomogelem,j.getItem().exp()));
      d_xn -= degree(unhomogelem,xn)*j.getItem().exp();
    }
  }
  DEBOUTLN(CERR, "Power of xn to append is ", d_xn);
  if ( d_xn != 0 ) // have to append xn^(d_xn)
    Unhomoglist.append(CFFactor(CanonicalForm(xn),d_xn));

  DEBDECLEVEL(CERR, "HomogFactor");
  return Unhomoglist;
}

/*
$Log: not supported by cvs2svn $
Revision 1.10  2008/04/08 16:19:10  Singular
*hannes: removed rcsid

Revision 1.9  2008/03/18 17:46:16  Singular
*hannes: gcc 4.2

Revision 1.8  2007/10/25 14:45:41  Singular
*hannes: homgfactor for alg.ext of Q

Revision 1.7  2006/05/16 14:46:50  Singular
*hannes: gcc 4.1 fixes

Revision 1.6  2005/12/09 08:36:11  Singular
*hannes: stuff for homog. polys ->factory

Revision 1.5  2005/12/05 15:47:32  Singular
*hannes: is_homogeneous -> factory: isHomogeneous

Revision 1.4  2002/08/19 11:11:34  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.3  1997/09/12 07:19:57  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:35:20  michael
changed cerr and cout messages for use with Singular
Version for libfac-0.2.1

Revision 1.1  1997/01/17 05:05:48  michael
Initial revision

*/
