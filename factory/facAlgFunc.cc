////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////

// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <tmpl_inst.h>
#include <helpstuff.h>
// Charset - Includes
#include "csutil.h"
#include "charset.h"
#include "reorder.h"
#include "algfactor.h"
// some CC's need this:
#include "alg_factor.h"

void out_cf(const char *s1,const CanonicalForm &f,const char *s2);

#ifdef ALGFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include <libfac/factor/debug.h>

///////////////////////////////////////////////////////////////
// generate a minpoly of degree degree_of_Extension in the   //
// field getCharacteristik()^Extension.                      //
///////////////////////////////////////////////////////////////
CanonicalForm
generate_mipo( int degree_of_Extension , const Variable & Extension )
{
  FFRandom gen;
  if (degree (Extension) < 0)
    factoryError("libfac: evaluate: Extension not inFF() or inGF() !");
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

  DEBOUTLN(CERR, "sqrf_norm_sub:      f= ", f);
  DEBOUTLN(CERR, "sqrf_norm_sub: Palpha= ", Palpha);
  myrandom.reset();   s=myrandom.item();   g=f;
  R= CanonicalForm(0);
  DEBOUTLN(CERR, "sqrf_norm_sub: myrandom s= ", s);

  // Norm, resultante taken with respect to y
  while ( !sqfreetest )
  {
    DEBOUTLN(CERR, "sqrf_norm_sub: Palpha= ", Palpha);
    R = resultante(Palpha, g, y); R= R* bCommonDen(R);
    DEBOUTLN(CERR, "sqrf_norm_sub: R= ", R);
    R /= content (R);
    // sqfree check ; R is a polynomial in K[x]
    if ( getCharacteristic() == 0 )
    {
      temp= gcd(R, R.deriv(vf));
      DEBOUTLN(CERR, "sqrf_norm_sub: temp= ", temp);
      if (degree(temp,vf) != 0 || temp == temp.genZero() )
        sqfreetest= 0;
      else
        sqfreetest= 1;
      DEBOUTLN(CERR, "sqrf_norm_sub: sqfreetest= ", sqfreetest);
    }
    else
    {
      DEBOUTMSG(CERR, "Starting SqrFreeTest(R)!");
      // Look at SqrFreeTest!
      // (z+a^5+w)^4 with z<w<a should not give sqfreetest=1 !
      // for now we use this workaround with Factorize...
      // ...but it should go away soon!!!!
      Variable X;
      if (hasFirstAlgVar(R,X))
        testlist=factorize( R, X );
      else
        testlist= factorize(R);
      DEBOUTLN(CERR, "testlist= ", testlist);
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
      DEBOUTLN(CERR, "SqrFreeTest(R)= ", sqfreetest);
    }
    if ( ! sqfreetest )
    {
      myrandom.next();
      DEBOUTLN(CERR, "sqrf_norm_sub generated new myrandom item: ", myrandom.item());
      if ( getCharacteristic() == 0 )
        t= CanonicalForm(mapinto(myrandom.item()));
      else
        t= CanonicalForm(myrandom.item());
      s= t;
      DEBOUTLN(CERR, "sqrf_norm_sub: testing s= ", s);
      g= f(f.mvar()-t*Palpha.mvar(), f.mvar());
      DEBOUTLN(CERR, "             gives g= ", g);
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

  DEBOUTLN(CERR, "sqrf_norm_sub:      f= ", f);
  DEBOUTLN(CERR, "sqrf_norm_sub: Palpha= ", Palpha);
  myrandom.reset();   s=myrandom.item();   g=f;
  R= CanonicalForm(0);
  DEBOUTLN(CERR, "sqrf_norm_sub: myrandom s= ", s);

  // Norm, resultante taken with respect to y
  while ( !sqfreetest )
  {
    DEBOUTLN(CERR, "sqrf_norm_sub: Palpha= ", Palpha);
    R = resultante(Palpha, g, y); R= R* bCommonDen(R);
    DEBOUTLN(CERR, "sqrf_norm_sub: R= ", R);
    R /= content (R);
    // sqfree check ; R is a polynomial in K[x]
    if ( getCharacteristic() == 0 )
    {
      temp= gcd(R, R.deriv(vf));
      DEBOUTLN(CERR, "sqrf_norm_sub: temp= ", temp);
      if (degree(temp,vf) != 0 || temp == temp.genZero() )
        sqfreetest= 0;
      else
        sqfreetest= 1;
      DEBOUTLN(CERR, "sqrf_norm_sub: sqfreetest= ", sqfreetest);
    }
    else
    {
      DEBOUTMSG(CERR, "Starting SqrFreeTest(R)!");
      // Look at SqrFreeTest!
      // (z+a^5+w)^4 with z<w<a should not give sqfreetest=1 !
      // for now we use this workaround with Factorize...
      // ...but it should go away soon!!!!
      Variable X;
      if (hasFirstAlgVar(R,X))
        testlist= factorize( R, X );
      else
        testlist= factorize(R);
      DEBOUTLN(CERR, "testlist= ", testlist);
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
      DEBOUTLN(CERR, "SqrFreeTest(R)= ", sqfreetest);
    }
    if ( ! sqfreetest )
    {
      myrandom.next();
      DEBOUTLN(CERR, "sqrf_norm_sub generated new myrandom item: ", myrandom.item());
      if ( getCharacteristic() == 0 )
        t= CanonicalForm(mapinto(myrandom.item()));
      else
        t= CanonicalForm(myrandom.item());
      s= t;
      DEBOUTLN(CERR, "sqrf_norm_sub: testing s= ", s);
      g= f(f.mvar()-t*Palpha.mvar(), f.mvar());
      DEBOUTLN(CERR, "             gives g= ", g);
    }
  }
}

static void
sqrf_norm( const CanonicalForm & f, const CanonicalForm & PPalpha,
           const Variable & Extension, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R)
{
  DEBOUTLN(CERR, "sqrf_norm:      f= ", f);
  DEBOUTLN(CERR, "sqrf_norm: Palpha= ", PPalpha);
  if ( getCharacteristic() == 0 )
  {
    IntGenerator myrandom;
    DEBOUTMSG(CERR, "sqrf_norm: no extension, char=0");
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
    DEBOUTLN(CERR, "sqrf_norm:      f= ", f);
    DEBOUTLN(CERR, "sqrf_norm: Palpha= ", PPalpha);
    DEBOUTLN(CERR, "sqrf_norm:      s= ", s);
    DEBOUTLN(CERR, "sqrf_norm:      g= ", g);
    DEBOUTLN(CERR, "sqrf_norm:      R= ", R);
  }
  else if ( degree(Extension) > 0 ) // working over Extensions
  {
    DEBOUTLN(CERR, "sqrf_norm: degree of extension is ", degree(Extension));
    AlgExtGenerator myrandom(Extension);
    sqrf_agnorm_sub(f,PPalpha, myrandom, s,g,R);
  }
  else
  {
    FFGenerator myrandom;
    DEBOUTMSG(CERR, "sqrf_norm: degree of extension is 0");
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
        DEBOUTLN(CERR, "getextension: gcd == 1, l=",l);
        if ( l==length ){ setCharacteristic(charac);  return k; }
      }
      else { DEBOUTMSG(CERR, "getextension: Next iteration"); break; }
    }
    k= k+1; l=0;
  }
  while ( 1 );
}


/// pseudo division of f and g wrt. x s.t. multiplier*f=q*g+r
/// but only if the leading coefficient of g is of level lower than coeffLevel
void
psqr (const CanonicalForm & f, const CanonicalForm & g, CanonicalForm & q,
      CanonicalForm & r, CanonicalForm& multiplier, const Variable& x,
      int coeffLevel)
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
    if (LCG.level() < coeffLevel)
    {
      multiplier= power (LCG, fDegree - gDegree + 1);
      divrem (multiplier*F, G, q, r);
      q= swapvar (q, x, X);
      r= swapvar (r, x, X);
    }
    else
    {
      q= 0;
      r= f;
    }
  }
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

  DEBOUTLN(CERR, "simpleextension: Astar= ", Astar);
  DEBOUTLN(CERR, "simpleextension:     R= ", R);
  DEBOUTLN(CERR, "simpleextension: Extension= ", Extension);
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

      DEBOUTLN(CERR, "simpleextension: g= ", g);
      DEBOUTLN(CERR, "simpleextension: s= ", s);
      DEBOUTLN(CERR, "simpleextension: R= ", R);
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
  //assert(f.inCoeffDomain());
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
#ifndef DEBUGOUTPUT
static CFFList
alg_factor( const CanonicalForm & F, const CFList & Astar, const Variable & vminpoly, const Varlist /*& oldord*/, const CFList & as, bool isFunctionField)
#else
static CFFList
alg_factor( const CanonicalForm & F, const CFList & Astar, const Variable & vminpoly, const Varlist & oldord, const CFList & as, bool isFunctionField)
#endif
{
  bool isRat= isOn (SW_RATIONAL);
  CFFList L, Factorlist;
  CanonicalForm R, Rstar, s, g, h, f= F;
  CFList substlist, backSubsts;

  DEBINCLEVEL(CERR,"alg_factor");
  DEBOUTLN(CERR, "alg_factor: f= ", f);

  substlist= simpleextension (backSubsts, Astar, vminpoly, isFunctionField, Rstar);
  DEBOUTLN(CERR, "alg_factor: substlist= ", substlist);
  DEBOUTLN(CERR, "alg_factor: minpoly Rstar= ", Rstar);
  DEBOUTLN(CERR, "alg_factor: vminpoly= ", vminpoly);

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
  //out_cf("sqrf_norm R:",R,"\n");
  //out_cf("sqrf_norm s:",s,"\n");
  //out_cf("sqrf_norm g:",g,"\n");
  DEBOUTLN(CERR, "alg_factor: g= ", g);
  DEBOUTLN(CERR, "alg_factor: s= ", s);
  DEBOUTLN(CERR, "alg_factor: R= ", R);
  Variable X;
  if (hasFirstAlgVar(R,X))
  {
    // factorize R over alg.extension with X
    //CERR << "alg: "<< X << " mipo=" << getMipo(X,Variable('X')) <<"\n";
    DEBOUTLN(CERR, "alg_factor: factorize( ", R);
    Factorlist =  factorize( R, X );
  }
  else
  {
    // factor R over k
    DEBOUTLN(CERR, "alg_factor: Factorize( ", R);
    Factorlist = factorize(R);
  }

  DEBOUTLN(CERR, "alg_factor: Factorize(R)= ", Factorlist);
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
    DEBOUTLN(CERR, "alg_factor: g= ", g);
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

      DEBOUTLN(CERR, "alg_factor: fnew= ", fnew);

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
  //CFFListIterator i=LL;
  //for(;i.hasItem(); i++ )
  //  out_cf("end alg_f:",i.getItem().factor(),"\n");
  //printf("end alg_factor\n");
  DEBOUTLN(CERR, "alg_factor: L= ", LL);
  DEBDECLEVEL(CERR,"alg_factor");
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
  PremForm rem;
  asnew.append (F);
  asnew= mcharset (asnew, rem); // TODO use modCharSet

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
    rem= PremForm();
    transform= transBack;
    CanonicalForm factor= k.getItem().factor();
    factor= M (factor);
    transform.append (factor);
    transform= mcharset (transform, rem); //TODO use modCharSet
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
  DEBINCLEVEL(CERR, "newfactoras");
  DEBOUTLN(CERR, "newfactoras called with f= ", f);
  DEBOUTLN(CERR, "               content(f)= ", content(f));
  DEBOUTLN(CERR, "                       as= ", as);
  DEBOUTLN(CERR, "newfactoras: cls(vf)= ", cls(vf));
  DEBOUTLN(CERR, "newfactoras: cls(as.getLast())= ", cls(as.getLast()));
  DEBOUTLN(CERR, "newfactoras: degree(f,vf)= ", degree(f,vf));

// F1: [Test trivial cases]
// 1) first trivial cases:
  if (cls(vf) <= cls(as.getLast()))
  {
// ||( (as.length()==1) && (degree(f,vf)==3) && (degree(as.getFirst()==2)) )
    DEBDECLEVEL(CERR,"newfactoras");
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
  DEBOUTLN(CERR, "Astar is: ", Astar);
  DEBOUTLN(CERR, "ord is: ", ord);
  DEBOUTLN(CERR, "uord is: ", uord);

// 3) second trivial cases: we already proved irr. of f over no extensions
  if ( Astar.length() == 0 ){
    DEBDECLEVEL(CERR,"newfactoras");
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
  DEBOUTLN(CERR, "newuord is: ", newuord);

  CFFList Factorlist;
  Varlist gcdord= Union(ord,newuord); gcdord.append(f.mvar());
  bool isFunctionField= (newuord.length() > 0);

  // This is for now. we need alg_sqrfree implemented!
  CanonicalForm Fgcd= 0;
  if (isFunctionField)
    Fgcd= alg_gcd(f,f.deriv(),Astar);

  if ( Fgcd == 0 ) {DEBOUTMSG(CERR, "WARNING: p'th root ?");}

  bool derivZero= f.deriv().isZero();
  if (isFunctionField && (degree (Fgcd, f.mvar()) > 0) && !derivZero)
  {
    DEBOUTLN(CERR, "Nontrivial GCD found of ", f);
    CanonicalForm Ggcd= divide(f, Fgcd,Astar);
    if (getCharacteristic() == 0)
    {
      CFFList result= newfactoras (Ggcd,as,success); //Ggcd is the squarefree part of f
      multiplicity (result, f, Astar);
      if (!isRat && getCharacteristic() == 0)
        Off (SW_RATIONAL);
      return result;
    }
    DEBOUTLN(CERR, "  split into ", Fgcd);
    DEBOUTLN(CERR, "         and ", Ggcd);
    Fgcd= pp(Fgcd); Ggcd= pp(Ggcd);
    DEBDECLEVEL(CERR,"newfactoras");
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
    DEBOUTLN(CERR, "Extension needed of degree ", extdeg);

    // Now the real stuff!
    if ( newuord.length() == 0 ) // no transzendentals
    {
      DEBOUTMSG(CERR, "No transzendentals!");
      if ( extdeg > 1 ){
        CanonicalForm MIPO= generate_mipo( extdeg, vminpoly);
        DEBOUTLN(CERR, "Minpoly produced ", MIPO);
        vminpoly= rootOf(MIPO);
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
      DEBDECLEVEL(CERR,"newfactoras");
      return Factorlist;
    }
    else if ( inseperable(Astar) > 0 || derivZero) // Look if extensions are seperable
    {
      DEBOUTMSG(CERR, "Inseperable extensions! Using Endler!");
      Factorlist= SteelTrager(f, Astar, newuord);
      DEBOUTLN(CERR, "Endler gives: ", Factorlist);
      return Factorlist;
    }
    else{ // we are on the save side: Use trager
      DEBOUTMSG(CERR, "Only seperable extensions!");
      if (extdeg > 1 ){
        CanonicalForm MIPO=generate_mipo(extdeg, vminpoly );
        vminpoly= rootOf(MIPO);
        DEBOUTLN(CERR, "Minpoly generated: ", MIPO);
        DEBOUTLN(CERR, "vminpoly= ", vminpoly);
        DEBOUTLN(CERR, "degree(vminpoly)= ", degree(vminpoly));
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
      DEBDECLEVEL(CERR,"newfactoras");
      return Factorlist;
    }
  }
  else{ // char=0 apply trager directly
    DEBOUTMSG(CERR, "Char=0! Apply Trager!");
    Variable vminpoly;
    Factorlist= alg_factor(f, Astar, vminpoly, oldord, as, isFunctionField);
    DEBDECLEVEL(CERR,"newfactoras");
    if (!isRat && getCharacteristic() == 0)
      Off (SW_RATIONAL);
    return Factorlist;
  }

  DEBDECLEVEL(CERR,"newfactoras");
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
  if ( cls(f) <= cls(as.getLast()) )
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
