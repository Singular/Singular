/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: fac_sqrfree.cc,v 1.9 2008-01-25 14:17:59 Singular Exp $ */

#include <config.h>

#include "assert.h"

#include "cf_defs.h"
#include "cf_map.h"
#include "canonicalform.h"
#include "fac_sqrfree.h"

static int divexp = 1;

static void divexpfunc ( CanonicalForm &, int & e )
{
    e /= divexp;
}

static int compareFactors( const CFFactor & f, const CFFactor & g )
{
    return f.exp() > g.exp();
}

CFFList sortCFFList( CFFList & F )
{
    F.sort( compareFactors );

    int exp;
    CanonicalForm f;
    CFFListIterator I = F;
    CFFList result;

    // join elements with the same degree
    while ( I.hasItem() )
    {
        f = I.getItem().factor();
        exp = I.getItem().exp();
        I++;
        while ( I.hasItem() && I.getItem().exp() == exp )
        {
            f *= I.getItem().factor();
            I++;
        }
        result.append( CFFactor( f, exp ) );
    }

    return result;
}

CFFList appendCFFL( const CFFList & Inputlist, const CFFactor & TheFactor)
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

CFFList UnionCFFL(const CFFList & Inputlist1,const CFFList & Inputlist2)
{
  CFFList Outputlist;
  CFFListIterator i;

  for ( i=Inputlist1 ; i.hasItem() ; i++ )
    Outputlist = appendCFFL(Outputlist, i.getItem() );
  for ( i=Inputlist2 ; i.hasItem() ; i++ )
    Outputlist = appendCFFL(Outputlist, i.getItem() );

  return Outputlist;
}

CFFList sqrFreeFp_univ ( const CanonicalForm & f )
{
    if (getNumVars(f) == 0) return CFFactor(f,1); 
    CanonicalForm t0 = f, t, v, w, h;
    CanonicalForm leadcf = t0.lc();
    Variable x = f.mvar();
    CFFList F;
    int p = getCharacteristic();
    int k, e = 1;

    if ( ! leadcf.isOne() )
        t0 /= leadcf;

    divexp = p;
    while ( t0.degree(x) > 0 )
    {
        t = gcd( t0, t0.deriv() );
        v = t0 / t;
        k = 0;
        while ( v.degree(x) > 0 )
        {
            k = k+1;
            if ( k % p == 0 )
            {
                t /= v;
                k = k+1;
            }
            w = gcd( t, v );
            h = v / w;
            v = w;
            t /= v;
            if ( h.degree(x) > 0 )
                F.append( CFFactor( h/h.lc(), e*k ) );
        }
        t0 = apply( t, divexpfunc );
        e = p * e;
    }
    if ( ! leadcf.isOne() )
    {
        if ( F.getFirst().exp() == 1 )
        {
            leadcf = F.getFirst().factor() * leadcf;
            F.removeFirst();
        }
        F.insert( CFFactor( leadcf, 1 ) );
    }
    return F;
}
static inline CFFactor Powerup( const CFFactor & F , int exp=1)
{
  return CFFactor(F.factor(), exp*F.exp()) ;
}

static CFFList Powerup( const CFFList & Inputlist , int exp=1 )
{
  CFFList Outputlist;

  for ( CFFListIterator i=Inputlist; i.hasItem(); i++ )
    Outputlist.append(Powerup(i.getItem(), exp));
  return Outputlist ;
}
int Powerup( const int base , const int exp)
{
  int retvalue=1;
  if ( exp == 0 )  return retvalue ;
  else for ( int i=1 ; i <= exp; i++ ) retvalue *= base ;
    
  return retvalue;
}

///////////////////////////////////////////////////////////////
// Compute the Pth root of a polynomial in characteristic p  //
// f must be a polynomial which we can take the Pth root of. //
// Domain is q=p^m , f a uni/multivariate polynomial         //
///////////////////////////////////////////////////////////////
static CanonicalForm PthRoot( const CanonicalForm & f )
{
  CanonicalForm RES, R = f;
  int n= getNumVars(R), p= getCharacteristic();

  if (level(R)>n) n=level(R);

  if (n==0)
  { // constant
    if (R.inExtension()) // not in prime field; f over |F(q=p^k)
    {
      R = power(R,Powerup(p,getGFDegree() - 1)) ;
    }  
    // if f in prime field, do nothing
    return R;
  }
  // we assume R is a Pth power here
  RES = R.genZero();
  Variable x(n);
  for (int i=0; i<= (int) (degree(R,level(R))/p) ; i++)
    RES += PthRoot( R[i*p] ) * power(x,i);
  return RES;
}

///////////////////////////////////////////////////////////////
// Compute the Pth root of a polynomial in characteristic p  //
// f must be a polynomial which we can take the Pth root of. //
// Domain is q=p^m , f a uni/multivariate polynomial         //
///////////////////////////////////////////////////////////////
static CanonicalForm PthRoot( const CanonicalForm & f ,const CanonicalForm & mipo)
{
  CanonicalForm RES, R = f;
  int n= getNumVars(R), p= getCharacteristic();
  int mipodeg=-1;

  if (level(R)>n) n=level(R);

  if (f.level()==mipo.level()) mipodeg=mipo.degree();
  else if ((f.level()==1) &&(mipo.level()!=LEVELBASE))
  {
    Variable t;
    CanonicalForm tt=getMipo(mipo.mvar(),t);
    mipodeg=degree(tt,t);
  }

  if ((n==0)
  ||(mipodeg!=-1))
  { // constant
    if (R.inExtension()) // not in prime field; f over |F(q=p^k)
    {
      R = power(R,Powerup(p,getGFDegree() - 1)) ;
    }  
    else if ((f.level()==mipo.level())
    ||((f.level()==1) &&(mipo.level()!=LEVELBASE)))
    {
      R = power(R,Powerup(p,mipodeg - 1)) ;
      R=mod(R, mipo);
    }
    // if f in prime field, do nothing
    return R;
  }
  // we assume R is a Pth power here
  RES = R.genZero();
  Variable x(n);
  for (int i=0; i<= (int) (degree(R,level(R))/p) ; i++)
    RES += PthRoot( R[i*p], mipo ) * power(x,i);
  return RES;
}
CFFList sqrFreeFp ( const CanonicalForm & r, const CanonicalForm &mipo )
{
///////////////////////////////////////////////////////////////
// A uni/multivariate SqrFree routine.Works for polynomials  //
// which don\'t have a constant as the content.              //
// Works for charcteristic 0 and q=p^m                       //
// returns a list of polys each of sqrfree, but gcd(f_i,f_j) //
// needs not to be 1 !!!!!                                   //
///////////////////////////////////////////////////////////////
  CanonicalForm h, g, f = r;
  CFFList Outputlist;
  int n = level(f);

  if (getNumVars(f)==0 )
  { // just a constant; return it
    Outputlist= CFFactor(f,1);
    return Outputlist ;
  }

// We look if we do have a content; if so, SqrFreed the content
// and continue computations with pp(f)
  for (int k=1; k<=n; k++)
  {
    if ((mipo.level()==LEVELBASE)||(k!=1))
    {
      g = swapvar(f,k,n); g = content(g);
      if ( ! (g.isOne() || (-g).isOne() || degree(g)==0 ))
      {
        g = swapvar(g,k,n);
        Outputlist = UnionCFFL(sqrFreeFp(g,mipo),Outputlist); // should we add a
                                                // SqrFreeTest(g) first ?
        f /=g;
      }
    }  
  }

// Now f is primitive; Let`s look if f is univariate
  if ( f.isUnivariate() )
  {
    g = content(g);
    if ( ! (g.isOne() || (-g).isOne() ) )
    {
      Outputlist= appendCFFL(Outputlist,CFFactor(g,1)) ;
      f /= g;
    }
    Outputlist = Union(sqrFreeFp_univ(f),Outputlist) ;
    return Outputlist ;
  }

// Linear?
  if ( totaldegree(f) <= 1 )
  {
    Outputlist= appendCFFL(Outputlist,CFFactor(f,1)) ;
    return Outputlist ;
  }

// is it Pth root?
  n=level(f); // maybe less indeterminants
  g= f.deriv();
  if ( /*getCharacteristic() > 0 &&*/ g.isZero() )
  {  // Pth roots only apply to char > 0
    for (int k=1; k<=n; k++)
    {
      if ((mipo.level()==LEVELBASE)||(k!=1))
      {
        g=swapvar(f,k,n) ;
        g = g.deriv();

        if ( ! g.isZero() )
	{ // can`t be Pth root
          CFFList Outputlist2= sqrFreeFp(swapvar(f,k,n));
          for (CFFListIterator inter=Outputlist2; inter.hasItem(); inter++)
	  {
            Outputlist=appendCFFL(Outputlist,
               CFFactor(swapvar(inter.getItem().factor(),k,n), inter.getItem().exp()));
          }
          return Outputlist;
        }
      }	
      if ( k==n )
      { // really is Pth power
        CFMap m;
        g = compress(f,m);
	if (mipo.isZero())
          f = m(PthRoot(g));
	else
          f = m(PthRoot(g,mipo));
        // now : Outputlist union ( SqrFreed(f) )^getCharacteristic()
        Outputlist=UnionCFFL(Powerup(sqrFreeFp(f),getCharacteristic()),Outputlist);
        return Outputlist ;
      }
    }
  }
  g = f.deriv();
  h = gcd(f,pp(g));  h /= lc(h);
  if ( (h.isOne()) || ( h==f) || ((-h).isOne()) || getNumVars(h)==0 )
  { // no common factor
    Outputlist=appendCFFL(Outputlist,CFFactor(f,1)) ;
    return Outputlist ;
  }
  else  // we can split into two nontrivial pieces
  {
    f /= h; // Now we have split the poly into f and h
    g = lc(f);
    if ( (!g.isOne()) && getNumVars(g) == 0 )
    {
       Outputlist=appendCFFL(Outputlist,CFFactor(g,1)) ;
       f /= g;
    }
    // For char > 0 the polys f and h can have Pth roots; so we need a test
    // Test is disabled because timing is the same
    
    Outputlist=UnionCFFL(Outputlist, sqrFreeFp(f,mipo));
    Outputlist=UnionCFFL(Outputlist,sqrFreeFp(h,mipo));
    return Outputlist ;
  }
  return Outputlist; // for safety purpose
}

/*

CFFList sqrFreeZ ( const CanonicalForm & a )
{
    CanonicalForm b = a.deriv(), c = gcd( a, b );
    CanonicalForm y, z, w = a / c;
    int i = 1;
    CFFList F;

    while ( ! c.degree() == 0 ) {
        y = gcd( w, c ); z = w / y;
        if ( degree( z ) > 0 )
            if ( lc( z ).sign() < 0 )
                F.append( CFFactor( -z, i ) );
            else
                F.append( CFFactor( z, i ) );
        i++;
        w = y; c = c / y;
    }
    if ( degree( w ) > 0 )
        if ( lc( w ).sign() < 0 )
            F.append( CFFactor( -w, i ) );
        else
            F.append( CFFactor( w, i ) );
    return F;
}
*/

CFFList sqrFreeZ ( const CanonicalForm & a, const CanonicalForm & mipo )
{
    if ( a.inCoeffDomain() )
        return CFFactor( a, 1 );
    CanonicalForm cont = content( a );
    CanonicalForm aa = a / cont;
    CanonicalForm b = aa.deriv(), c = gcd( aa, b );
    CanonicalForm y, z, w = aa / c;
    int i = 1;
    CFFList F;
    Variable v = aa.mvar();
    while ( ! c.degree(v) == 0 )
    {
        y = gcd( w, c ); z = w / y;
        if ( degree( z, v ) > 0 )
            if ( lc( z ).sign() < 0 )
                F.append( CFFactor( -z, i ) );
            else
                F.append( CFFactor( z, i ) );
        i++;
        w = y; c = c / y;
    }
    if ( degree( w,v ) > 0 )
        if ( lc( w ).sign() < 0 )
            F.append( CFFactor( -w, i ) );
        else
            F.append( CFFactor( w, i ) );
    if ( ! cont.isOne() )
        F = Union( F, sqrFreeZ( cont ) );
    if ( lc( a ).sign() < 0 ) {
        if ( F.getFirst().exp() == 1 )
        {
            CanonicalForm f = F.getFirst().factor();
            CFFListIterator(F).getItem() = CFFactor( -f, 1 );
        }
        else
            F.insert( CFFactor( -1, 1 ) );
    }
    return F;
}
