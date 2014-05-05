/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file facAlgFunc.cc
 *
 * This file provides functions to factorize polynomials over alg. function
 * fields
 * 
 * ABSTRACT: Descriptions can be found in B. Trager "Algebraic Factoring and
 * Rational Function Integration" and A. Steel "Conquering Inseparability:
 * Primary decomposition and multivariate factorization over algebraic function
 * fields of positive characteristic"
 *
 * @authors Martin Lee, Michael Messollen
 *
 **/
/*****************************************************************************/

#include "config.h"

#include "cf_assert.h"
#include "debug.h"

#include "algext.h"
#include "cf_random.h"
#include "cf_generator.h"
#include "cf_irred.h"
#include "cf_iter.h"
#include "cf_util.h"
#include "cf_algorithm.h"
#include "cf_map.h"
#include "cfModResultant.h"
#include "cfCharSets.h"
#include "facAlgFunc.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

static
CanonicalForm
Prem (const CanonicalForm& F, const CanonicalForm& G)
{
  CanonicalForm f, g, l, test, lu, lv, t, retvalue;
  int degF, degG, levelF, levelG;
  bool reord;
  Variable v, vg= G.mvar();

  if ( (levelF= F.level()) < (levelG= G.level()))
    return F;
  else
  {
    if ( levelF == levelG )
    {
      f= F;
      g= G;
      reord= false;
      v= F.mvar();
    }
    else
    {
      v= Variable (levelF + 1);
      f= swapvar (F, vg, v);
      g= swapvar (G, vg, v);
      reord= true;
    }
    degG= degree (g, v );
    degF= degree (f, v );
    if (degG <= degF)
    {
      l= LC (g);
      g= g - l*power (v, degG);
    }
    else
      l= 1;
    while ((degG <= degF) && (!f.isZero()))
    {
      test= gcd (l, LC(f));
      lu= l / test;
      lv= LC(f) / test;

      t= power (v, degF - degG)*g*lv;

      if (degF == 0)
        f= 0;
      else
        f= f - LC (f)*power (v, degF);

      f= lu*f - t;
      degF= degree (f, v);
    }

    if (reord)
      retvalue= swapvar (f, vg, v);
    else
      retvalue= f;

    return retvalue;
  }
}

static
CanonicalForm
Prem( const CanonicalForm &f, const CFList &L)
{
  CanonicalForm rem= f;
  CFListIterator i= L;

  for (i.lastItem(); i.hasItem(); i--)
    rem= normalize (Prem (rem, i.getItem()));

  return rem;
}

static
CanonicalForm
Prem (const CanonicalForm &f, const CFList &L, const CFList &as)
{
  CanonicalForm rem = f;
  CFListIterator i = L;
  for ( i.lastItem(); i.hasItem(); i-- )
    rem = Prem( rem, i.getItem() );
  return normalize (rem); //TODO better normalize in case as.length() == 1 && as.getFirst().level() == 1 ???
}

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

///////////////////////////////////////////////////////////////
// generate a minpoly of degree degree_of_Extension in the   //
// field getCharacteristik()^Extension.                      //
///////////////////////////////////////////////////////////////
CanonicalForm
generate_mipo( int degree_of_Extension , const Variable & Extension )
{
  FFRandom gen;
  /*if (degree (Extension) < 0)
    factoryError("libfac: evaluate: Extension not inFF() or inGF() !");*/
  return find_irreducible( degree_of_Extension, gen, Variable(1) );
}

static Varlist
Var_is_in_AS(const Varlist & uord, const CFList & Astar);

////////////////////////////////////////////////////////////////////////
// This implements the algorithm of Trager for factorization of
// (multivariate) polynomials over algebraic extensions and so called
// function field extensions.
////////////////////////////////////////////////////////////////////////

// // missing class: IntGenerator:
bool IntGenerator::hasItems() const
{
    return 1;
}

CanonicalForm IntGenerator::item() const
//int IntGenerator::item() const
{
  //return current; //CanonicalForm( current );
  return mapinto(CanonicalForm( current ));
}

void IntGenerator::next()
{
    current++;
}

int hasAlgVar(const CanonicalForm &f, const Variable &v)
{
  if (f.inBaseDomain()) return 0;
  if (f.inCoeffDomain())
  {
    if (f.mvar()==v) return 1;
    return hasAlgVar(f.LC(),v);
  }
  if (f.inPolyDomain())
  {
    if (hasAlgVar(f.LC(),v)) return 1;
    for( CFIterator i=f; i.hasTerms(); i++)
    {
      if (hasAlgVar(i.coeff(),v)) return 1;
    }
  }
  return 0;
}

int hasVar(const CanonicalForm &f, const Variable &v)
{
  if (f.inBaseDomain()) return 0;
  if (f.inCoeffDomain())
  {
    if (f.mvar()==v) return 1;
    return hasAlgVar(f.LC(),v);
  }
  if (f.inPolyDomain())
  {
    if (f.mvar()==v) return 1;
    if (hasVar(f.LC(),v)) return 1;
    for( CFIterator i=f; i.hasTerms(); i++)
    {
      if (hasVar(i.coeff(),v)) return 1;
    }
  }
  return 0;
}

int hasAlgVar(const CanonicalForm &f)
{
  if (f.inBaseDomain()) return 0;
  if (f.inCoeffDomain())
  {
    if (f.level()!=0)
      return 1;
    return hasAlgVar(f.LC());
  }
  if (f.inPolyDomain())
  {
    if (hasAlgVar(f.LC())) return 1;
    for( CFIterator i=f; i.hasTerms(); i++)
    {
      if (hasAlgVar(i.coeff())) return 1;
    }
  }
  return 0;
}

/// pseudo division of f and g wrt. x s.t. multiplier*f=q*g+r
void
psqr (const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q, 
      CanonicalForm & r, CanonicalForm& multiplier, const Variable& x)
{
    ASSERT( x.level() > 0, "type error: polynomial variable expected" );
    ASSERT( ! g.isZero(), "math error: division by zero" );

    // swap variables such that x's level is larger or equal
    // than both f's and g's levels.
    Variable X;
    if (f.level() > g.level())
      X= f.mvar();
    else
      X= g.mvar();
    if (X.level() < x.level())
      X= x;
    CanonicalForm F= swapvar (f, x, X);
    CanonicalForm G= swapvar (g, x, X);

    // now, we have to calculate the pseudo remainder of F and G
    // w.r.t. X
    int fDegree= degree (F, X);
    int gDegree= degree (G, X);
    if (fDegree < 0 || fDegree < gDegree)
    {
      q= 0;
      r= f;
    }
    else
    {
      CanonicalForm LCG= LC (G, X);
      multiplier= power (LCG, fDegree - gDegree + 1);
      divrem (multiplier*F, G, q, r);
      q= swapvar (q, x, X);
      r= swapvar (r, x, X);
    }
}

static CanonicalForm
Sprem ( const CanonicalForm &f, const CanonicalForm &g, CanonicalForm & m, CanonicalForm & q )
{
  CanonicalForm ff, gg, l, test, retvalue;
  int df, dg,n;
  bool reord;
  Variable vf, vg, v;

  if ( (vf = f.mvar()) < (vg = g.mvar()) )
  {
    m=CanonicalForm(0); q=CanonicalForm(0);
    return f;
  }
  else
  {
    if ( vf == vg )
    {
      ff = f; gg = g;
      reord = false;
      v = vg; // == x
    }
    else
    {
      v = Variable(level(f.mvar()) + 1);
      ff = swapvar(f,vg,v); // == r
      gg = swapvar(g,vg,v); // == v
      reord=true;
    }
    dg = degree( gg, v ); // == dv
    df = degree( ff, v ); // == dr
    if (dg <= df) {l=LC(gg); gg = gg -LC(gg)*power(v,dg);}
    else { l = 1; }
    n= 0;
    while ( ( dg <= df  ) && ( !ff.isZero()) )
    {
      test= power(v,df-dg) * gg * LC(ff);
      if ( df == 0 ){ff= ff.genZero();}
      else {ff= ff - LC(ff)*power(v,df);}
      ff = l*ff-test;
      df= degree(ff,v);
      n++;
    }
    if ( reord )
    {
      retvalue= swapvar( ff, vg, v );
    }
    else
    {
      retvalue= ff;
    }
    m= power(l,n);
    if ( fdivides(g,m*f-retvalue) )
      q= (m*f-retvalue)/g;
    else
      q= CanonicalForm(0);
    return retvalue;
  }
}

CanonicalForm
divide( const CanonicalForm & ff, const CanonicalForm & f, const CFList & as)
{
  CanonicalForm r,m,q;

  if (f.inCoeffDomain())
  {
    bool isRat=isOn(SW_RATIONAL);
    if (getCharacteristic() == 0)
      On(SW_RATIONAL);
    q=ff/f;
    if (!isRat && getCharacteristic() == 0)
      Off(SW_RATIONAL);
  }
  else
    r= Sprem(ff,f,m,q);

  r= Prem(q,as);
  return r;
}

CanonicalForm
alg_content (const CanonicalForm& f, const CFList& as)
{
  if (!f.inCoeffDomain())
  {
    CFIterator i= f;
    CanonicalForm result= abs (i.coeff());
    i++;
    while (i.hasTerms() && !result.isOne())
    {
      result= alg_gcd (i.coeff(), result, as);
      i++;
    }
    return result;
  }

  return abs (f);
}

CanonicalForm alg_gcd(const CanonicalForm & fff, const CanonicalForm &ggg,
                      const CFList &as)
{
  if (fff.inCoeffDomain() || ggg.inCoeffDomain())
    return 1;
  CanonicalForm f=fff;
  CanonicalForm g=ggg;
  f=Prem(f,as);
  g=Prem(g,as);
  if ( f.isZero() )
  {
    if ( g.lc().sign() < 0 ) return -g;
    else                     return g;
  }
  else  if ( g.isZero() )
  {
    if ( f.lc().sign() < 0 ) return -f;
    else                     return f;
  }

  int v= as.getLast().level();
  if (f.level() <= v || g.level() <= v)
    return 1;

  CanonicalForm res;

  // does as appear in f and g ?
  bool has_alg_var=false;
  for ( CFListIterator j=as;j.hasItem(); j++ )
  {
    Variable v=j.getItem().mvar();
    if (hasVar (f, v))
      has_alg_var=true;
    if (hasVar (g, v))
      has_alg_var=true;
  }
  if (!has_alg_var)
  {
    /*if ((hasAlgVar(f))
    || (hasAlgVar(g)))
    {
      Varlist ord;
      for ( CFListIterator j=as;j.hasItem(); j++ )
        ord.append(j.getItem().mvar());
      res=algcd(f,g,as,ord);
    }
    else*/
    if (!hasAlgVar (f) && !hasAlgVar (g))
      return res=gcd(f,g);
  }

  int mvf=f.level();
  int mvg=g.level();
  if (mvg > mvf)
  {
    CanonicalForm tmp=f; f=g; g=tmp;
    int tmp2=mvf; mvf=mvg; mvg=tmp2;
  }
  if (g.inBaseDomain() || f.inBaseDomain())
    return CanonicalForm(1);

  CanonicalForm c_f= alg_content (f, as);

  if (mvf != mvg)
  {
    res= alg_gcd (g, c_f, as);
    return res;
  }
  Variable x= f.mvar();

  // now: mvf==mvg, f.level()==g.level()
  CanonicalForm c_g= alg_content (g, as);

  int delta= degree (f) - degree (g);

  f= divide (f, c_f, as);
  g= divide (g, c_g, as);

  // gcd of contents
  CanonicalForm c_gcd= alg_gcd (c_f, c_g, as);
  CanonicalForm tmp;

  if (delta < 0)
  {
    tmp= f;
    f= g;
    g= tmp;
    delta= -delta;
  }

  CanonicalForm r= 1;

  while (degree (g, x) > 0)
  {
    r= Prem (f, g, as);
    r= Prem (r, as);
    if (!r.isZero())
    {
      r= divide (r, alg_content (r,as), as);
      r /= vcontent (r,Variable (v+1));
    }
    f= g;
    g= r;
  }

  if (degree (g, x) == 0)
    return c_gcd;

  c_f= alg_content (f, as);

  f= divide (f, c_f, as);

  f *= c_gcd;
  f /= vcontent (f, Variable (v+1));

  return f;
}

static CanonicalForm
resultante( const CanonicalForm & f, const CanonicalForm& g, const Variable & v )
{
  bool on_rational = isOn(SW_RATIONAL);
  if (!on_rational && getCharacteristic() == 0)
    On(SW_RATIONAL);
  CanonicalForm cd = bCommonDen( f );
  CanonicalForm fz = f * cd;
  cd = bCommonDen( g );
  CanonicalForm gz = g * cd;
  if (!on_rational && getCharacteristic() == 0)
    Off(SW_RATIONAL);
  CanonicalForm result;
  if (getCharacteristic() == 0)
    result= resultantZ (fz, gz,v);
  else
    result= resultant (fz,gz,v);

  return result;
}

// sqr-free routine for algebraic extensions
// we need it! Ex.: f=c^2+2*a*c-1; as=[a^2+1]; f=(c+a)^2
//static CFFList alg_sqrfree( const CanonicalForm & f )
//{
//  CFFList L;
//
//  L.append(CFFactor(f,1));
//  return L;
//}

// Calculates a square free norm
// Input: f(x, alpha) a square free polynomial over K(alpha),
// alpha is defined by the minimal polynomial Palpha
// K has more than S elements (S is defined in thesis; look getextension)
static void
sqrf_norm_sub( const CanonicalForm & f, const CanonicalForm & PPalpha,
           CFGenerator & myrandom, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R)
{
  Variable y=PPalpha.mvar(),vf=f.mvar();
  CanonicalForm temp, Palpha=PPalpha, t;
  int sqfreetest=0;
  CFFList testlist;
  CFFListIterator i;

  myrandom.reset();   s=myrandom.item();   g=f;
  R= CanonicalForm(0);

  // Norm, resultante taken with respect to y
  while ( !sqfreetest )
  {
    R = resultante(Palpha, g, y); R= R* bCommonDen(R);
    R /= content (R);
    // sqfree check ; R is a polynomial in K[x]
    if ( getCharacteristic() == 0 )
    {
      temp= gcd(R, R.deriv(vf));
      if (degree(temp,vf) != 0 || temp == temp.genZero() )
        sqfreetest= 0;
      else
        sqfreetest= 1;
    }
    else
    {
      Variable X;
      if (hasFirstAlgVar(R,X))
        testlist=factorize( R, X );
      else
        testlist= factorize(R);

      if (testlist.getFirst().factor().inCoeffDomain())
        testlist.removeFirst();
      sqfreetest=1;
      for ( i=testlist; i.hasItem(); i++)
      {
        if ( i.getItem().exp() > 1 && degree(i.getItem().factor(), R.mvar()) > 0)
        {
          sqfreetest=0;
          break;
        }
      }
    }
    if ( ! sqfreetest )
    {
      myrandom.next();
      if ( getCharacteristic() == 0 )
        t= CanonicalForm(mapinto(myrandom.item()));
      else
        t= CanonicalForm(myrandom.item());
      s= t;
      g= f(f.mvar()-t*Palpha.mvar(), f.mvar());
    }
  }
}
static void
sqrf_agnorm_sub( const CanonicalForm & f, const CanonicalForm & PPalpha,
           AlgExtGenerator & myrandom, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R)
{
  Variable y=PPalpha.mvar(),vf=f.mvar();
  CanonicalForm temp, Palpha=PPalpha, t;
  int sqfreetest=0;
  CFFList testlist;
  CFFListIterator i;

  myrandom.reset();   s=myrandom.item();   g=f;
  R= CanonicalForm(0);

  // Norm, resultante taken with respect to y
  while ( !sqfreetest )
  {
    R = resultante(Palpha, g, y); R= R* bCommonDen(R);
    R /= content (R);
    // sqfree check ; R is a polynomial in K[x]
    if ( getCharacteristic() == 0 )
    {
      temp= gcd(R, R.deriv(vf));
      if (degree(temp,vf) != 0 || temp == temp.genZero() )
        sqfreetest= 0;
      else
        sqfreetest= 1;
    }
    else
    {
      Variable X;
      if (hasFirstAlgVar(R,X))
        testlist= factorize( R, X );
      else
        testlist= factorize(R);
      if (testlist.getFirst().factor().inCoeffDomain())
        testlist.removeFirst();
      sqfreetest=1;
      for ( i=testlist; i.hasItem(); i++)
      {
        if ( i.getItem().exp() > 1 && degree(i.getItem().factor(), R.mvar()) > 0)
        {
          sqfreetest=0;
          break;
        }
      }
    }
    if ( ! sqfreetest )
    {
      myrandom.next();
      if ( getCharacteristic() == 0 )
        t= CanonicalForm(mapinto(myrandom.item()));
      else
        t= CanonicalForm(myrandom.item());
      s= t;
      g= f(f.mvar()-t*Palpha.mvar(), f.mvar());
    }
  }
}

static void
sqrf_norm( const CanonicalForm & f, const CanonicalForm & PPalpha,
           const Variable & Extension, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R)
{
  if ( getCharacteristic() == 0 )
  {
    IntGenerator myrandom;
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
  }
  else if ( degree(Extension) > 0 ) // working over Extensions
  {
    AlgExtGenerator myrandom(Extension);
    sqrf_agnorm_sub(f,PPalpha, myrandom, s,g,R);
  }
  else
  {
    FFGenerator myrandom;
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
  }
}

static Varlist
Var_is_in_AS(const Varlist & uord, const CFList & Astar){
  Varlist output;
  CanonicalForm elem;
  Variable x;

  for ( VarlistIterator i=uord; i.hasItem(); i++)
  {
    x=i.getItem();
    for ( CFListIterator j=Astar; j.hasItem(); j++ )
    {
      elem= j.getItem();
      if ( degree(elem,x) > 0 ) // x actually occures in Astar
      {
        output.append(x);
        break;
      }
    }
  }
  return output;
}

// Look if Minimalpolynomials in Astar define seperable Extensions
// Must be a power of p: i.e. y^{p^e}-x
static int
inseperable(const CFList & Astar)
{
  CanonicalForm elem;
  int Counter= 1;

  if ( Astar.length() == 0 ) return 0;
  for ( CFListIterator i=Astar; i.hasItem(); i++)
  {
    elem= i.getItem();
    if ( elem.deriv() == elem.genZero() ) return Counter;
    else Counter += 1;
  }
  return 0;
}

// calculate big enough extension for finite fields
// Idea: first calculate k, such that q^k > S (->thesis, -> getextension)
// Second, search k with gcd(k,m_i)=1, where m_i is the degree of the i'th
// minimal polynomial. Then the minpoly f_i remains irrd. over q^k and we
// have enough elements to plug in.
static int
getextension( IntList & degreelist, int n)
{
  int charac= getCharacteristic();
  setCharacteristic(0); // need it for k !
  int k=1, m=1, length=degreelist.length();
  IntListIterator i;

  for (i=degreelist; i.hasItem(); i++) m= m*i.getItem();
  int q=charac;
  while (q <= ((n*m)*(n*m)/2)) { k= k+1; q= q*charac;}
  int l=0;
  do {
    for (i=degreelist; i.hasItem(); i++){
      l= l+1;
      if ( igcd(k,i.getItem()) == 1){
        if ( l==length ){ setCharacteristic(charac);  return k; }
      }
      else { break; }
    }
    k= k+1; l=0;
  }
  while ( 1 );
}

CanonicalForm
QuasiInverse (const CanonicalForm& f, const CanonicalForm& g,
              const Variable& x)
{
  CanonicalForm pi, pi1, q, t0, t1, Hi, bi, pi2;
  bool isRat= isOn (SW_RATIONAL);
  CanonicalForm m,tmp;
  if (isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);
  pi= f/content (f,x);
  pi1= g/content (g,x);

  t0= 0;
  t1= 1;
  bi= 1;

  int delta= degree (f, x) - degree (g, x);
  Hi= power (LC (pi1, x), delta);
  if ( (delta+1) % 2 )
      bi = 1;
  else
      bi = -1;

  while (degree (pi1,x) > 0)
  {
    psqr( pi, pi1, q, pi2, m, x);
    pi2 /= bi;

    tmp= t1;
    t1= t0*m - t1*q;
    t0= tmp;
    t1 /= bi;
    pi = pi1; pi1 = pi2;
    if ( degree ( pi1, x ) > 0 )
    {
      delta = degree( pi, x ) - degree( pi1, x );
      if ( (delta+1) % 2 )
        bi = LC( pi, x ) * power( Hi, delta );
      else
        bi = -LC( pi, x ) * power( Hi, delta );
      Hi = power( LC( pi1, x ), delta ) / power( Hi, delta-1 );
    }
  }
  t1 /= gcd (pi1, t1);
  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);
  return t1;
}

CanonicalForm
evaluate (const CanonicalForm& f, const CanonicalForm& g, const CanonicalForm& h, const CanonicalForm& powH)
{
  if (f.inCoeffDomain())
    return f;
  CFIterator i= f;
  int lastExp = i.exp();
  CanonicalForm result = i.coeff()*powH;
  i++;
  while (i.hasTerms())
  {
    int i_exp= i.exp();
    if ((lastExp - i_exp) == 1)
    {
      result *= g;
      result /= h;
    }
    else
    {
      result *= power (g, lastExp - i_exp);
      result /= power (h, lastExp - i_exp);
    }
    result += i.coeff()*powH;
    lastExp = i_exp;
    i++;
  }
  if (lastExp != 0)
  {
    result *= power (g, lastExp);
    result /= power (h, lastExp);
  }
  return result;
}


/// evaluate f at g/h at v such that powH*f is integral i.e. powH is assumed to be h^degree(f,v)
CanonicalForm
evaluate (const CanonicalForm& f, const CanonicalForm& g,
          const CanonicalForm& h, const CanonicalForm& powH,
          const Variable& v)
{
  if (f.inCoeffDomain())
  {
    return f*powH;
  }

  Variable x = f.mvar();
  if ( v > x )
    return f*powH;
  else  if ( v == x )
    return evaluate (f, g, h, powH);

  // v is less than main variable of f
  CanonicalForm result= 0;
  for (CFIterator i= f; i.hasTerms(); i++)
    result += evaluate (i.coeff(), g, h, powH, v)*power (x, i.exp());
  return result;
}

// calculate a "primitive element"
// K must have more than S elements (->thesis, -> getextension)
static CFList
simpleextension(CFList& backSubst, const CFList & Astar,
                const Variable & Extension, bool& isFunctionField,
                CanonicalForm & R)
{
  CFList Returnlist, Bstar=Astar;
  CanonicalForm s, g, ra, rb, oldR, h, denra, denrb=1;
  Variable alpha;
  CFList tmp;

  bool isRat= isOn (SW_RATIONAL);

  CFListIterator j;
  if (Astar.length() == 1)
  {
    R= Astar.getFirst();
    rb= R.mvar();
  }
  else
  {
    R=Bstar.getFirst();
    Bstar.removeFirst();
    for (CFListIterator i=Bstar; i.hasItem(); i++)
    {
      j= i;
      j++;
      if (getCharacteristic() == 0)
        Off (SW_RATIONAL);
      R /= icontent (R);
      if (getCharacteristic() == 0)
        On (SW_RATIONAL);
      oldR= R;
      sqrf_norm (i.getItem(), R, Extension, s, g, R);

      backSubst.insert (s);

      if (getCharacteristic() == 0)
        Off (SW_RATIONAL);
      R /= icontent (R);

      if (getCharacteristic() == 0)
        On (SW_RATIONAL);

      if (!isFunctionField)
      {
        alpha= rootOf (R);
        h= replacevar (g, g.mvar(), alpha);
        if (getCharacteristic() == 0)
          On (SW_RATIONAL); //needed for GCD
        h= gcd (h, oldR);
        h /= Lc (h);
        ra= -h[0];
        ra= replacevar(ra, alpha, g.mvar());
        rb= R.mvar()-s*ra;
        for (; j.hasItem(); j++)
        {
          j.getItem()= j.getItem() (ra, oldR.mvar());
          j.getItem()= j.getItem() (rb, i.getItem().mvar());
        }
      }
      else
      {
        if (getCharacteristic() == 0)
          On (SW_RATIONAL);
        h= swapvar (g, g.mvar(), oldR.mvar());
        tmp= CFList (swapvar (R, g.mvar(), oldR.mvar()));
        h= alg_gcd (h, swapvar (oldR, g.mvar(), oldR.mvar()), tmp);
        CanonicalForm hh= replacevar (h, oldR.mvar(), alpha);

        CanonicalForm numinv, deninv;
        numinv= QuasiInverse (tmp.getFirst(), LC (h), tmp.getFirst().mvar());

        if (getCharacteristic() == 0)
          Off (SW_RATIONAL);
        h *= numinv;
        h= reduce (h, tmp.getFirst());
        deninv= LC(h);

        ra= -h[0];
        denra= gcd (ra, deninv);
        ra /= denra;
        denra= deninv/denra;
        denra= replacevar (denra, ra.mvar(), g.mvar());
        ra= replacevar(ra, ra.mvar(), g.mvar());
        rb= R.mvar()*denra-s*ra;
        denrb= denra;
        for (; j.hasItem(); j++)
        {
          CanonicalForm powdenra= power (denra, degree (j.getItem(), oldR.mvar()));
          j.getItem()= evaluate (j.getItem(),ra, denra, powdenra, oldR.mvar());
          powdenra= power (denra, degree (j.getItem(), i.getItem().mvar()));
          j.getItem()= evaluate (j.getItem(), rb, denrb, powdenra, i.getItem().mvar());
        }
      }

      Returnlist.append (ra);
      if (isFunctionField)
        Returnlist.append (denra);
    }
  }
  Returnlist.append (rb);
  if (isFunctionField)
    Returnlist.append (denrb);

  if (isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  else if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  return Returnlist;
}

CanonicalForm alg_lc(const CanonicalForm &f)
{
  if (f.level()>0)
  {
    return alg_lc(f.LC());
  }

  return f;
}

CanonicalForm alg_LC (const CanonicalForm& f, int lev)
{
  CanonicalForm result= f;
  while (result.level() > lev)
    result= LC (result);
  return result;
}

CanonicalForm
subst (const CanonicalForm& f, const CFList& a, const CFList& b,
       const CanonicalForm& Rstar, bool isFunctionField)
{
  if (isFunctionField)
    ASSERT (2*a.length() == b.length(), "wrong length of lists");
  else
    ASSERT (a.length() == b.length(), "lists of equal length expected");
  CFListIterator j= b;
  CanonicalForm result= f, tmp, powj;
  for (CFListIterator i= a; i.hasItem() && j.hasItem(); i++, j++)
  {
    if (!isFunctionField)
      result= result (j.getItem(), i.getItem().mvar());
    else
    {
      tmp= j.getItem();
      j++;
      powj= power (j.getItem(), degree (result, i.getItem().mvar()));
      result= evaluate (result, tmp, j.getItem(), powj, i.getItem().mvar());

      if (fdivides (powj, result, tmp))
        result= tmp;

      result /= vcontent (result, Variable (i.getItem().level() + 1));
    }
  }
  result= reduce (result, Rstar);
  result /= vcontent (result, Variable (Rstar.level() + 1));
  return result;
}

CanonicalForm
backSubst (const CanonicalForm& F, const CFList& a, const CFList& b)
{
  ASSERT (a.length() == b.length() - 1, "wrong length of lists in backSubst");
  CanonicalForm result= F;
  Variable tmp;
  CFList tmp2= b;
  tmp= tmp2.getLast().mvar();
  tmp2.removeLast();
  for (CFListIterator iter= a; iter.hasItem(); iter++)
  {
    result= result (tmp+iter.getItem()*tmp2.getLast().mvar(), tmp);
    tmp= tmp2.getLast().mvar();
    tmp2.removeLast();
  }
  return result;
}

void deflateDegree (const CanonicalForm & F, int & pExp, int n)
{
  if (n == 0 || n > F.level())
  {
    pExp= -1;
    return;
  }
  if (F.level() == n)
  {
    ASSERT (F.deriv().isZero(), "derivative of F is not zero");
    int termCount=0;
    CFIterator i= F;
    for (; i.hasTerms(); i++)
    {
      if (i.exp() != 0)
        termCount++;
    }

    int j= 1;
    i= F;
    for (;j < termCount; j++, i++)
      ;

    int exp= i.exp();
    int count= 0;
    int p= getCharacteristic();
    while ((exp >= p) && (exp != 0) && (exp % p == 0))
    {
      exp /= p;
      count++;
    }
    pExp= count;
  }
  else
  {
    CFIterator i= F;
    deflateDegree (i.coeff(), pExp, n);
    i++;
    int tmp= pExp;
    for (; i.hasTerms(); i++)
    {
      deflateDegree (i.coeff(), pExp, n);
      if (tmp == -1)
        tmp= pExp;
      else if (tmp != -1 && pExp != -1)
        pExp= (pExp < tmp) ? pExp : tmp;
      else
        pExp= tmp;
    }
  }
}

CanonicalForm deflatePoly (const CanonicalForm & F, int exp)
{
  if (exp == 0)
    return F;
  int p= getCharacteristic();
  int pToExp= ipower (p, exp);
  Variable x=F.mvar();
  CanonicalForm result= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
    result += i.coeff()*power (x, i.exp()/pToExp);
  return result;
}

CanonicalForm deflatePoly (const CanonicalForm & F, int exps, int n)
{
  if (n == 0 || exps <= 0 || F.level() < n)
    return F;
  if (F.level() == n)
    return deflatePoly (F, exps);
  else
  {
    CanonicalForm result= 0;
    for (CFIterator i= F; i.hasTerms(); i++)
      result += deflatePoly (i.coeff(), exps, n)*power(F.mvar(), i.exp());
    return result;
  }
}

CanonicalForm inflatePoly (const CanonicalForm & F, int exp)
{
  if (exp == 0)
    return F;
  int p= getCharacteristic();
  int pToExp= ipower (p, exp);
  Variable x=F.mvar();
  CanonicalForm result= 0;
  for (CFIterator i= F; i.hasTerms(); i++)
    result += i.coeff()*power (x, i.exp()*pToExp);
  return result;
}

CanonicalForm inflatePoly (const CanonicalForm & F, int exps, int n)
{
  if (n == 0 || exps <= 0 || F.level() < n)
    return F;
  if (F.level() == n)
    return inflatePoly (F, exps);
  else
  {
    CanonicalForm result= 0;
    for (CFIterator i= F; i.hasTerms(); i++)
      result += inflatePoly (i.coeff(), exps, n)*power(F.mvar(), i.exp());
    return result;
  }
}

// the heart of the algorithm: the one from Trager
static CFFList
alg_factor( const CanonicalForm & F, const CFList & Astar, const Variable & vminpoly, const Varlist & oldord, const CFList & as, bool isFunctionField)
{
  bool isRat= isOn (SW_RATIONAL);
  CFFList L, Factorlist;
  CanonicalForm R, Rstar, s, g, h, f= F;
  CFList substlist, backSubsts;

  substlist= simpleextension (backSubsts, Astar, vminpoly, isFunctionField, Rstar);

  f= subst (f, Astar, substlist, Rstar, isFunctionField);

  Variable alpha;
  if (!isFunctionField)
  {
    alpha= rootOf (Rstar);
    g= replacevar (f, Rstar.mvar(), alpha);

    if (!isRat && getCharacteristic() == 0)
      On (SW_RATIONAL);
    Factorlist= factorize (g, alpha);

    for (CFFListIterator i= Factorlist; i.hasItem(); i++)
    {
      h= i.getItem().factor();
      if (!h.inCoeffDomain())
      {
        h= replacevar (h, alpha, Rstar.mvar());
        h *= bCommonDen(h);
        h= backSubst (h, backSubsts, Astar);
        h= Prem (h, as);
        L.append (CFFactor (h, i.getItem().exp()));
      }
    }
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return L;
  }
  // after here we are over an extension of a function field

  // make quasi monic
  CFList Rstarlist= CFList (Rstar);
  int algExtLevel= Astar.getLast().level(); //highest level of algebraic variables
  CanonicalForm numinv;
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  numinv= QuasiInverse (Rstar, alg_LC(f, algExtLevel), Rstar.mvar());

  f *= numinv;
  f= Prem (f, Rstarlist);
  f /= vcontent (f, Rstar.mvar());
  // end quasi monic

  if (degree (f) == 1)
  {
    f= backSubst (f, backSubsts, Astar);
    f *= bCommonDen (f);
    f= Prem (f, as);
    f /= vcontent (f, as.getFirst().mvar());

    return CFFList (CFFactor (f, 1));
  }

  sqrf_norm(f, Rstar, vminpoly, s, g, R );

  Variable X;
  if (hasFirstAlgVar(R,X))
  {
    // factorize R over alg.extension with X
    Factorlist =  factorize( R, X );
  }
  else
  {
    // factor R over k
    Factorlist = factorize(R);
  }

  if ( !Factorlist.getFirst().factor().inCoeffDomain() )
    Factorlist.insert(CFFactor(1,1));
  if ( Factorlist.length() == 2 && Factorlist.getLast().exp()== 1)
  { // irreduzibel (first entry is a constant)
    f= backSubst (f, backSubsts, Astar);
    f *= bCommonDen (f);
    f= Prem (f, as);
    f /= vcontent (f, as.getFirst().mvar());

    L.append(CFFactor(f,1));
  }
  else
  {
    g= f;
    for ( CFFListIterator i=Factorlist; i.hasItem(); i++)
    {
      CanonicalForm fnew=i.getItem().factor();
      if (fnew.level() < Rstar.level()) //factor is a constant from the function field
        continue;
      else
      {
        fnew= fnew (g.mvar()+s*Rstar.mvar(), g.mvar());
        fnew= reduce (fnew, Rstar);
      }

      h= alg_gcd (g, fnew, Rstarlist);
      numinv= QuasiInverse(Rstar, alg_LC(h, algExtLevel), Rstar.mvar());
      h *= numinv;
      h= Prem (h, Rstarlist);
      h /= vcontent (h, Rstar.mvar());

      if (h.level() >= Rstar.level())
      {
        g= divide (g, h, Rstarlist);
        h= backSubst (h, backSubsts, Astar);
        h= Prem (h, as);
        h *= bCommonDen (h);
        h /= vcontent (h, as.getFirst().mvar());
        L.append (CFFactor (h, 1));
      }
    }
    // we are not interested in a
    // constant (over K_r, which can be a polynomial!)
    if (degree(g, f.mvar())>0){ L.append(CFFactor(g,1)); }
  }
  CFFList LL;
  if (getCharacteristic()>0)
  {
    CFFListIterator i=L;
    CanonicalForm c_fac=1;
    CanonicalForm c;
    for(;i.hasItem(); i++ )
    {
      CanonicalForm ff=i.getItem().factor();
      c=alg_lc(ff);
      int e=i.getItem().exp();
      ff/=c;
      if (!ff.isOne()) LL.append(CFFactor(ff,e));
      while (e>0) { c_fac*=c;e--; }
    }
    if (!c_fac.isOne()) LL.insert(CFFactor(c_fac,1));
  }
  else
  {
    LL=L;
  }

  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);

  return LL;
}

/// algorithm of A. Steel described in "Conquering Inseparability: Primary
/// decomposition and multivariate factorization over algebraic function fields
/// of positive characteristic" with the following modifications: we use
/// characteristic sets in IdealOverSubfield and Trager's primitive element
/// algorithm instead of FGLM
CFFList
SteelTrager (const CanonicalForm & f, const CFList & AS, const Varlist & uord)
{
  CanonicalForm F= f, lcmVars= 1;
  CFList asnew, as= AS;
  CFListIterator i, ii;

  bool derivZeroF= false, derivZero= false;
  int j, exp=0, expF= 0, tmpExp;
  CFFList varsMapLevel;
  CFFListIterator iter;

  // check if F is separable
  if (F.deriv().isZero())
  {
    derivZeroF= true;
    deflateDegree (F, expF, F.level());
  }

  CanonicalForm varsF= getVars (F);
  varsF /= F.mvar();

  lcmVars= lcm (varsF, lcmVars);

  if (derivZeroF)
    as.append (F);

  CFFList varsGMapLevel;
  CFFList tmp;
  CFFList * varsGMap= new CFFList [as.length()];
  for (j= 0; j < as.length(); j++)
    varsGMap[j]= CFFList();

  CanonicalForm varsG;
  j= 0;
  bool recurse= false;
  i= as;
  // make minimal polys and F separable
  while (i.hasItem())
  {
    derivZero= false;

    if (i.getItem().deriv() == 0)
    {
      deflateDegree (i.getItem(), exp, i.getItem().level());
      i.getItem()= deflatePoly (i.getItem(), exp, i.getItem().level());
     
      varsG= getVars (i.getItem());
      varsG /= i.getItem().mvar();

      lcmVars= lcm (varsG, lcmVars);

      while (!varsG.isOne())
      {
        if (i.getItem().deriv (varsG.level()).isZero())
        {
          deflateDegree (i.getItem(), tmpExp, varsG.level());
          if (exp >= tmpExp)
          {
            if (exp == tmpExp)
              i.getItem()= deflatePoly (i.getItem(), exp, varsG.level());
            else
            {
              if (j != 0)
                recurse= true;
              i.getItem()= deflatePoly (i.getItem(), tmpExp, varsG.level());
            }
            varsGMapLevel.insert (CFFactor (varsG.mvar(), exp - tmpExp));
          }
          else
          {
            i.getItem()= deflatePoly (i.getItem(), exp, varsG.level());
            varsGMapLevel.insert (CFFactor (varsG.mvar(), 0));
          }
        }
        else
        {
          if (j != 0)
            recurse= true;
          varsGMapLevel.insert (CFFactor (varsG.mvar(),exp));
        }
        varsG /= varsG.mvar();
      }

      if (recurse)
      {
        ii=as;
        for (; ii.hasItem(); ii++)
        {
          if (ii.getItem() == i.getItem())
            continue;
          for (iter= varsGMapLevel; iter.hasItem(); iter++)
            ii.getItem()= inflatePoly (ii.getItem(), iter.getItem().exp(),
                                       iter.getItem().factor().level());
        }
      }
      else
      {
        ii= i;
        ii++;
        for (; ii.hasItem(); ii++)
        {
          for (iter= varsGMapLevel; iter.hasItem(); iter++)
          {
            ii.getItem()= inflatePoly (ii.getItem(), iter.getItem().exp(),
                                       iter.getItem().factor().level());
          }
        }
      }
      if (varsGMap[j].isEmpty())
        varsGMap[j]= varsGMapLevel;
      else
      {
        if (!varsGMapLevel.isEmpty())
        {
          tmp= varsGMap[j];
          CFFListIterator iter2= varsGMapLevel;
          ASSERT (tmp.length() == varsGMapLevel.length(), "wrong length of lists");
          for (iter=tmp; iter.hasItem(); iter++, iter2++)
            iter.getItem()= CFFactor (iter.getItem().factor(),
                                  iter.getItem().exp() + iter2.getItem().exp());
          varsGMap[j]= tmp;
        }
      }
      varsGMapLevel= CFFList();
    }
    asnew.append (i.getItem());
    if (!recurse)
    {
      i++;
      j++;
    }
    else
    {
      i= as;
      j= 0;
      recurse= false;
      asnew= CFList();
    }
  }

  while (!lcmVars.isOne())
  {
    varsMapLevel.insert (CFFactor (lcmVars.mvar(), 0));
    lcmVars /= lcmVars.mvar();
  }

  // compute how to map variables in F
  for (j= 0; j < as.length(); j++)
  {
    if (varsGMap[j].isEmpty())
      continue;

    for (CFFListIterator iter2= varsGMap[j]; iter2.hasItem(); iter2++)
    {
      for (iter= varsMapLevel; iter.hasItem(); iter++)
      {
        if (iter.getItem().factor() == iter2.getItem().factor())
        {
          iter.getItem()= CFFactor (iter.getItem().factor(),
                                  iter.getItem().exp() + iter2.getItem().exp());
        }
      }
    }
  }

  delete [] varsGMap;

  if (derivZeroF)
  {
    as.removeLast();
    asnew.removeLast();
    F= deflatePoly (F, expF, F.level());
  }

  // map variables in F
  for (iter= varsMapLevel; iter.hasItem(); iter++)
  {
    if (expF > 0)
      tmpExp= iter.getItem().exp() - expF;
    else
      tmpExp= iter.getItem().exp();

    if (tmpExp > 0)
      F= inflatePoly (F, tmpExp, iter.getItem().factor().level());
    else if (tmpExp < 0)
      F= deflatePoly (F, -tmpExp, iter.getItem().factor().level());
  }

  // factor F with minimal polys given in asnew
  asnew.append (F);
  asnew= charSetViaModCharSet (asnew, false);

  F= asnew.getLast();
  F /= content (F); // should be obsolete if we use modCharSet

  asnew.removeLast();
  for (CFListIterator i= asnew; i.hasItem(); i++) // should be obsolete if we use modCharSet
    i.getItem() /= content (i.getItem());

  j= 0;
  tmp= newcfactor (F, asnew, j);

  // transform back
  j= 0;
  int p= getCharacteristic();
  CFList transBack;
  CFMap M;
  CanonicalForm g;

  for (iter= varsMapLevel; iter.hasItem(); iter++)
  {
    if (iter.getItem().exp() > 0)
    {
      j++;
      g= power (Variable (f.level() + j), ipower (p, iter.getItem().exp())) -
         iter.getItem().factor().mvar();
      transBack.append (g);
      M.newpair (iter.getItem().factor().mvar(), Variable (f.level() + j));
    }
  }

  for (i= asnew; i.hasItem(); i++)
    transBack.insert (M (i.getItem()));

  if (expF > 0)
    tmpExp= ipower (p, expF);

  CFFList result;
  CFList transform;

  for (CFFListIterator k= tmp; k.hasItem(); k++)
  {
    transform= transBack;
    CanonicalForm factor= k.getItem().factor();
    factor= M (factor);
    transform.append (factor);
    transform= charSetViaModCharSet (transform, false);
    for (i= transform; i.hasItem(); i++)
    {
      if (degree (i.getItem(), f.mvar()) > 0)
      {
        factor= i.getItem();
        break;
      }
    }

    factor /= content (factor); // should be superflous if we use modCharSet instead of CharSet

    if (expF > 0)
    {
      int mult= tmpExp/(degree (factor)/degree (k.getItem().factor()));
      result.append (CFFactor (factor, k.getItem().exp()*mult));
    }
    else
      result.append (CFFactor (factor, k.getItem().exp()));
  }

  return result;
}

void
multiplicity (CFFList& factors, const CanonicalForm& F, const CFList& as)
{
  CanonicalForm G= F;
  Variable x= F.mvar();
  CanonicalForm q, r;
  int count= -1;
  for (CFFListIterator iter=factors; iter.hasItem(); iter++)
  {
    count= -1;
    if (iter.getItem().factor().inCoeffDomain())
      continue;
    while (1)
    {
      psqr (G, iter.getItem().factor(), q, r, x);

      q= Prem (q, as);
      r= Prem (r, as);
      if (!r.isZero())
        break;
      count++;
      G= q;
    }
    iter.getItem()= CFFactor (iter.getItem().factor(), iter.getItem().exp()+count);
  }
}

// 1) prepares data
// 2) for char=p we distinguish 3 cases:
//           no transcendentals, seperable and inseperable extensions
CFFList
newfactoras( const CanonicalForm & f, const CFList & as, int &success)
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  Variable vf=f.mvar();
  CFListIterator i;
  CFFListIterator jj;
  CFList reduceresult;
  CFFList result;

  success=1;

// F1: [Test trivial cases]
// 1) first trivial cases:
  if (vf.level() <= as.getLast().level())
  {
// ||( (as.length()==1) && (degree(f,vf)==3) && (degree(as.getFirst()==2)) )
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return CFFList(CFFactor(f,1));
  }

// 2) List of variables:
// 2a) Setup list of those polys in AS having degree(AS[i], AS[i].mvar()) > 1
// 2b) Setup variableordering
  CFList Astar;
  Variable x;
  CanonicalForm elem;
  Varlist ord, uord,oldord;
  for ( int ii=1; ii< level(vf) ; ii++ ) { uord.append(Variable(ii));  }
  oldord= uord; oldord.append(vf);

  for ( i=as; i.hasItem(); i++ ){
    elem= i.getItem();
    x= elem.mvar();
    if ( degree(elem,x) > 1){ // otherwise it's not an extension
      Astar.append(elem);
      ord.append(x);
    }
  }
  uord= Difference(uord,ord);

// 3) second trivial cases: we already proved irr. of f over no extensions
  if ( Astar.length() == 0 ){
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return CFFList(CFFactor(f,1));
  }

// 4) Try to obtain a partial factorization using prop2 and prop3
//    Use with caution! We have to proof these propositions first!
  // Not yet implemented

// 5) Look if elements in uord actually occure in any of the minimal
//    polynomials. If no element of uord occures in any of the minimal
//   polynomials, we don't have transzendentals.
  Varlist newuord=Var_is_in_AS(uord,Astar);

  CFFList Factorlist;
  Varlist gcdord= Union(ord,newuord); gcdord.append(f.mvar());
  bool isFunctionField= (newuord.length() > 0);

  // This is for now. we need alg_sqrfree implemented!
  CanonicalForm Fgcd= 0;
  if (isFunctionField)
    Fgcd= alg_gcd(f,f.deriv(),Astar);

  bool derivZero= f.deriv().isZero();
  if (isFunctionField && (degree (Fgcd, f.mvar()) > 0) && !derivZero)
  {
    CanonicalForm Ggcd= divide(f, Fgcd,Astar);
    if (getCharacteristic() == 0)
    {
      CFFList result= newfactoras (Ggcd,as,success); //Ggcd is the squarefree part of f
      multiplicity (result, f, Astar);
      if (!isRat && getCharacteristic() == 0)
        Off (SW_RATIONAL);
      return result;
    }

    Fgcd= pp(Fgcd); Ggcd= pp(Ggcd);
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return myUnion(newfactoras(Fgcd,as,success) , newfactoras(Ggcd,as,success));
  }

  if ( getCharacteristic() > 0 )
  {
    // First look for extension!
    IntList degreelist;
    Variable vminpoly;
    for (i=Astar; i.hasItem(); i++){degreelist.append(degree(i.getItem()));}
    int extdeg= getextension(degreelist, degree(f));

    // Now the real stuff!
    if ( newuord.length() == 0 ) // no transzendentals
    {
      if ( extdeg > 1 ){
        CanonicalForm MIPO= generate_mipo( extdeg, vminpoly);
        vminpoly= rootOf(MIPO);
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
      return Factorlist;
    }
    else if ( inseperable(Astar) > 0 || derivZero) // Look if extensions are seperable
    {
      Factorlist= SteelTrager(f, Astar, newuord);
      return Factorlist;
    }
    else{ // we are on the save side: Use trager
      if (extdeg > 1 ){
        CanonicalForm MIPO=generate_mipo(extdeg, vminpoly );
        vminpoly= rootOf(MIPO);
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
      return Factorlist;
    }
  }
  else{ // char=0 apply trager directly
    Variable vminpoly;
    Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return Factorlist;
  }

  return CFFList(CFFactor(f,1));
}

CFFList
newcfactor(const CanonicalForm & f, const CFList & as, int & success )
{
  bool isRat= isOn (SW_RATIONAL);
  if (!isRat && getCharacteristic() == 0)
    On (SW_RATIONAL);
  CFFList Output, output, Factors= factorize(f);
  if (Factors.getFirst().factor().inCoeffDomain())
    Factors.removeFirst();

  if ( as.length() == 0 )
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    success=1;
    return Factors;
  }
  if (f.level() <= as.getLast().level())
  {
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    success=1;
    return Factors;
  }

  success=1;
  for ( CFFListIterator i=Factors; i.hasItem(); i++ )
  {
    if (i.getItem().factor().level() > as.getLast().level())
    {
      output=newfactoras(i.getItem().factor(),as, success);
      for ( CFFListIterator j=output; j.hasItem(); j++)
        Output = myappend(Output,CFFactor(j.getItem().factor(),j.getItem().exp()*i.getItem().exp()));
    }
  }

  if (!isRat && getCharacteristic() == 0)
    Off (SW_RATIONAL);
  return Output;
}
