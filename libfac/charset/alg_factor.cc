/* Copyright 1997 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
static char * rcsid = "$Id: alg_factor.cc,v 1.10 2002-08-19 11:11:29 Singular Exp $";
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <tmpl_inst.h>
#include <Factor.h>
#include <SqrFree.h>
#include <helpstuff.h>
// Charset - Includes
#include "csutil.h"
#include "charset.h"
#include "reorder.h"
#include "algfactor.h"
// some CC's need this:
#include "alg_factor.h"

//void out_cf(char *s1,const CanonicalForm &f,char *s2);

#ifdef ALGFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(newfactoras_time);

static Varlist
Var_is_in_AS(const Varlist & uord, const CFList & Astar);

int getAlgVar(const CanonicalForm &f, Variable &X)
{
  if (f.inBaseDomain()) return 0;
  if (f.inCoeffDomain())
  {
    if (f.level()!=0)
    {
      X= f.mvar();
      return 1;
    }
    return getAlgVar(f.LC(),X);
  }
  if (f.inPolyDomain())
  {
    if (getAlgVar(f.LC(),X)) return 1;
    for( CFIterator i=f; i.hasTerms(); i++)
    {
      if (getAlgVar(i.coeff(),X)) return 1;
    }
  }
  return 0;
}

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

// replacement for factory's broken psr
static CanonicalForm
mypsr ( const CanonicalForm &rr, const CanonicalForm &vv, const Variable & x ){
  CanonicalForm r=rr, v=vv, l, test, lu, lv, t, retvalue;
  int dr, dv, d,n=0;


  dr = degree( r, x );
  dv = degree( v, x );
  if (dv <= dr) {l=LC(v,x); v = v -l*power(x,dv);}
  else { l = 1; }
  d= dr-dv+1;
  while ( ( dv <= dr  ) && ( r != r.genZero()) ){
    test = power(x,dr-dv)*v*LC(r,x);
    if ( dr == 0 ) { r= CanonicalForm(0); }
    else { r= r - LC(r,x)*power(x,dr); }
    r= l*r -test;
    dr= degree(r,x);
    n+=1;
  }
  r= power(l, d-n)*r;
  return r;
}

// replacement for factory's broken resultant
static CanonicalForm
resultante( const CanonicalForm & f, const CanonicalForm& g, const Variable & v ){
return resultant(f,g,v);

  CanonicalForm h, beta, help, F, G;
  int delta;

  DEBOUTLN( cout, "resultante: called  f= ", f);
  DEBOUTLN( cout, "resultante: called  g= ", g);
  DEBOUTLN( cout, "resultante: called  v= ", v);
  if ( f.mvar() < v || g.mvar() < v ){
    DEBOUTMSG(cout, "resultante: f.mvar() < v || g.mvar() < v");
    return 1;
  }

  if ( f.degree( v ) < 1 || g.degree( v ) < 1 ){
    DEBOUTMSG(cout, "resultante: f.degree( v ) < 1 || g.degree( v ) < 1");
    // If deg(F,v) == 0 , then resultante(F,G,v) = F^n, where n=deg(G,v)
    if ( f.degree( v ) < 1 ) return power(f,degree(g,v));
    else return power(g,degree(f,v));
  }

   if ( f.degree( v ) >= g.degree( v ) ) { F = f; G = g; }
   else { G = f; F = g; }

  h = CanonicalForm(1);
  while ( G != G.genZero() ) {
     delta= degree(F,v) -degree(G,v);
     beta = power(CanonicalForm(-1), delta+1) * LC(F,v)* power(h, delta);
     h= (h * power(LC(G,v), delta)) / power(h, delta);
     help= G;
     G= mypsr(F,G,v);
     G= G/beta;
     F=help;
   }
   if ( degree(F,v) != 0 ) F= CanonicalForm(0);
   return F;
}

// sqr-free routine for algebraic extensions
// we need it! Ex.: f=c^2+2*a*c-1; as=[a^2+1]; f=(c+a)^2
static CFFList
alg_sqrfree( const CanonicalForm & f ){
  CFFList L;

  L.append(CFFactor(f,1));
  return L;
}

// Calculates a square free norm
// Input: f(x, alpha) a square free polynomial over K(alpha),
// alpha is defined by the minimal polynomial Palpha
// K has more than S elements (S is defined in thesis; look getextension)
static void
sqrf_norm_sub( const CanonicalForm & f, const CanonicalForm & PPalpha,
           CFGenerator & myrandom, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R){
  Variable y=PPalpha.mvar(),vf=f.mvar();
  CanonicalForm temp, Palpha=PPalpha, t;
  int sqfreetest=0;
  CFFList testlist;
  CFFListIterator i;

  DEBOUTLN(cout, "sqrf_norm_sub:      f= ", f);
  DEBOUTLN(cout, "sqrf_norm_sub: Palpha= ", Palpha);
  myrandom.reset();   s=f.mvar()-myrandom.item()*Palpha.mvar();   g=f;
  R= CanonicalForm(0);
  DEBOUTLN(cout, "sqrf_norm_sub: myrandom s= ", s);

  // Norm, resultante taken with respect to y
  while ( !sqfreetest ){
    DEBOUTLN(cout, "sqrf_norm_sub: Palpha= ", Palpha);
    R = resultante(Palpha, g, y); R= R* bCommonDen(R);
    DEBOUTLN(cout, "sqrf_norm_sub: R= ", R);
    // sqfree check ; R is a polynomial in K[x]
    if ( getCharacteristic() == 0 )
    {
      temp= gcd(R, R.deriv(vf));
      DEBOUTLN(cout, "sqrf_norm_sub: temp= ", temp);
      if (degree(temp,vf) != 0 || temp == temp.genZero() ){ sqfreetest= 0; }
      else { sqfreetest= 1; }
      DEBOUTLN(cout, "sqrf_norm_sub: sqfreetest= ", sqfreetest);
    }
    else{
      DEBOUTMSG(cout, "Starting SqrFreeTest(R)!");
      // Look at SqrFreeTest!
      // (z+a^5+w)^4 with z<w<a should not give sqfreetest=1 !
      // for now we use this workaround with Factorize...
      // ...but it should go away soon!!!!
      Variable X;
      if (getAlgVar(R,X))
      { 
        if (R.isUnivariate())
	  testlist=factorize( R, X );
	else  
          testlist= Factorize(R, X, 0);
      }
      else 
        testlist= Factorize(R);
      DEBOUTLN(cout, "testlist= ", testlist);
      testlist.removeFirst();
      sqfreetest=1;
      for ( i=testlist; i.hasItem(); i++)
        if ( i.getItem().exp() > 1 && degree(i.getItem().factor(), R.mvar()) > 0) { sqfreetest=0; break; }
      DEBOUTLN(cout, "SqrFreeTest(R)= ", sqfreetest);
    }
    if ( ! sqfreetest ){
      myrandom.next();
      DEBOUTLN(cout, "sqrf_norm_sub generated new myrandom item: ", myrandom.item());
      if ( getCharacteristic() == 0 ) t= CanonicalForm(mapinto(myrandom.item()));
      else t= CanonicalForm(myrandom.item());
      s= f.mvar()+t*Palpha.mvar(); // s defines backsubstitution
      DEBOUTLN(cout, "sqrf_norm_sub: testing s= ", s);
      g= f(f.mvar()-t*Palpha.mvar(), f.mvar());
      DEBOUTLN(cout, "             gives g= ", g);
    }
  }
}

static void
sqrf_norm( const CanonicalForm & f, const CanonicalForm & PPalpha,
           const Variable & Extension, CanonicalForm & s,  CanonicalForm & g,
           CanonicalForm & R){

  DEBOUTLN(cout, "sqrf_norm:      f= ", f);
  DEBOUTLN(cout, "sqrf_norm: Palpha= ", PPalpha);
  if ( getCharacteristic() == 0 ) {
    IntGenerator myrandom;
    DEBOUTMSG(cout, "sqrf_norm: no extension, char=0");
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
    DEBOUTLN(cout, "sqrf_norm:      f= ", f);
    DEBOUTLN(cout, "sqrf_norm: Palpha= ", PPalpha);
    DEBOUTLN(cout, "sqrf_norm:      s= ", s);
    DEBOUTLN(cout, "sqrf_norm:      g= ", g);
    DEBOUTLN(cout, "sqrf_norm:      R= ", R);
  }
  else if ( degree(Extension) > 0 ){ // working over Extensions
    DEBOUTLN(cout, "sqrf_norm: degree of extension is ", degree(Extension));
    AlgExtGenerator myrandom(Extension);
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
  }
  else{
    FFGenerator myrandom;
    DEBOUTMSG(cout, "sqrf_norm: degree of extension is 0");
    sqrf_norm_sub(f,PPalpha, myrandom, s,g,R);
  }
}

static Varlist
Var_is_in_AS(const Varlist & uord, const CFList & Astar){
  Varlist output;
  CanonicalForm elem;
  Variable x;

  for ( VarlistIterator i=uord; i.hasItem(); i++){
    x=i.getItem();
    for ( CFListIterator j=Astar; j.hasItem(); j++ ){
      elem= j.getItem();
      if ( degree(elem,x) > 0 ){ // x actually occures in Astar
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
inseperable(const CFList & Astar){
  CanonicalForm elem;
  int Counter= 1;

  if ( Astar.length() == 0 ) return 0;
  for ( CFListIterator i=Astar; i.hasItem(); i++){
    elem= i.getItem();
    if ( elem.deriv() == elem.genZero() ) return Counter;
    else Counter += 1;
  }
  return 0;
}

// calculate gcd of f and g in char=0
static CanonicalForm
gcd0( CanonicalForm f, CanonicalForm g ){
  int charac= getCharacteristic();
  setCharacteristic(0); Off(SW_RATIONAL);
  CanonicalForm ff= mapinto(f), gg= mapinto(g);
  CanonicalForm result= gcd(ff,gg);
  setCharacteristic(charac); On(SW_RATIONAL);
  return mapinto(result);
}

// calculate big enough extension for finite fields
// Idea: first calculate k, such that q^k > S (->thesis, -> getextension)
// Second, search k with gcd(k,m_i)=1, where m_i is the degree of the i'th
// minimal polynomial. Then the minpoly f_i remains irrd. over q^k and we
// have enough elements to plug in.
static int
getextension( IntList & degreelist, int n){
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
      if ( gcd0(k,i.getItem()) == 1){
        DEBOUTLN(cout, "getextension: gcd == 1, l=",l);
        if ( l==length ){ setCharacteristic(charac);  return k; }
      }
      else { DEBOUTMSG(cout, "getextension: Next iteration"); break; }
    }
    k= k+1; l=0;
  }
  while ( 1 );
}

// calculate a "primitive element"
// K must have more than S elements (->thesis, -> getextension)
static CFList
simpleextension(const CFList & Astar, const Variable & Extension,
                CanonicalForm & R){
  CFList Returnlist, Bstar=Astar;
  CanonicalForm s, g;

  DEBOUTLN(cout, "simpleextension: Astar= ", Astar);
  DEBOUTLN(cout, "simpleextension:     R= ", R);
  DEBOUTLN(cout, "simpleextension: Extension= ", Extension);
  if ( Astar.length() == 1 ){ R= Astar.getFirst();}
  else{
    R=Bstar.getFirst(); Bstar.removeFirst();
    for ( CFListIterator i=Bstar; i.hasItem(); i++){
      DEBOUTLN(cout, "simpleextension: f(x)= ", i.getItem());
      DEBOUTLN(cout, "simpleextension: P(x)= ", R);
      sqrf_norm(i.getItem(), R, Extension, s, g, R);
      // spielt die Repraesentation eine Rolle?
      // muessen wir die Nachfolger aendern, wenn s != 0 ?
      DEBOUTLN(cout, "simpleextension: g= ", g);
      if ( s != 0 ) DEBOUTLN(cout, "simpleextension: s= ", s);
      else DEBOUTLN(cout, "simpleextension: s= ", s);
      DEBOUTLN(cout, "simpleextension: R= ", R);
      Returnlist.insert(s);
    }
  }

  return Returnlist;
}

CanonicalForm alg_lc(const CanonicalForm &f)
{
  if (f.inCoeffDomain()) return f;
  if (f.level()>0)
  {
    return alg_lc(f.LC());
  }
}  

// the heart of the algorithm: the one from Trager
static CFFList
alg_factor( const CanonicalForm & f, const CFList & Astar, const Variable & vminpoly, const Varlist & oldord, const CFList & as){
  CFFList L, Factorlist;
  CanonicalForm R, Rstar, s, g, h;
  CFList substlist;

  DEBINCLEVEL(cout,"alg_factor");
  DEBOUTLN(cout, "alg_factor: f= ", f);
  //out_cf("start alg_factor:",f,"\n");
  substlist= simpleextension(Astar, vminpoly, Rstar);
  DEBOUTLN(cout, "alg_factor: substlist= ", substlist);
  DEBOUTLN(cout, "alg_factor: minpoly Rstar= ", Rstar);
  DEBOUTLN(cout, "alg_factor: vminpoly= ", vminpoly);

  sqrf_norm(f, Rstar, vminpoly, s, g, R );
  //out_cf("sqrf_norm R:",R,"\n");
  //out_cf("sqrf_norm s:",s,"\n");
  //out_cf("sqrf_norm g:",g,"\n");
  DEBOUTLN(cout, "alg_factor: g= ", g);
  DEBOUTLN(cout, "alg_factor: s= ", s);
  DEBOUTLN(cout, "alg_factor: R= ", R);
  Off(SW_RATIONAL);
  Variable X;
  if (getAlgVar(R,X))
  {
    // factorize R over alg.extension with X
//cout << "alg: "<< X << " mipo=" << getMipo(X,Variable('X')) <<endl;
    if (R.isUnivariate())
      Factorlist =  factorize( R, X );
    else
    {
      #if 1
      Variable XX;
      CanonicalForm mipo=getMipo(X,XX);
      CFList as(mipo);
      Factorlist = newfactoras(R, as , 1);
      #else
      // factor R over k
      Factorlist = Factorize(R);
      #endif
    }
  }
  else
  {
    // factor R over k
    Factorlist = Factorize(R);
  }
  On(SW_RATIONAL);
  DEBOUTLN(cout, "alg_factor: Factorize(R)= ", Factorlist);
  if ( !Factorlist.getFirst().factor().inCoeffDomain() )
    Factorlist.insert(CFFactor(1,1));
  if ( Factorlist.length() == 2 && Factorlist.getLast().exp()== 1){ // irreduzibel (first entry is a constant)
    L.append(CFFactor(f,1));
  }
  else{
    DEBOUTLN(cout, "alg_factor: g= ", g);
    CanonicalForm gnew= g(s,s.mvar());
    DEBOUTLN(cout, "alg_factor: gnew= ", gnew);
    g=gnew;
    for ( CFFListIterator i=Factorlist; i.hasItem(); i++){
      CanonicalForm fnew=i.getItem().factor();
      fnew= fnew(s,s.mvar());
      DEBOUTLN(cout, "alg_factor: fnew= ", fnew);
      DEBOUTLN(cout, "alg_factor: substlist= ", substlist);
      for ( CFListIterator ii=substlist; ii.hasItem(); ii++){
        DEBOUTLN(cout, "alg_factor: item= ", ii.getItem());
        fnew= fnew(ii.getItem(), ii.getItem().mvar());
        DEBOUTLN(cout, "alg_factor: fnew= ", fnew);
      }
      if (degree(i.getItem().factor()) > 0 ){
        // undo linear transformation!!!! and then gcd!
        //cout << "algcd(" << g << "," << fnew << ",as" << as << ")" << endl;
	//out_cf("algcd g=",g,"\n");
	//out_cf("algcd fnew=",fnew,"\n");
        //h= algcd(g,fnew, as, oldord);
	//if (as.length() >1)
        //  h= algcd(g,fnew, as, oldord);
	//else
	  h=alg_gcd(g,fnew,as);
	//out_cf(" -> algcd=",algcd(g,fnew, as, oldord),"\n");
	//out_cf(" -> alg_gcd=",alg_gcd(g,fnew,as),"\n");
        //cout << "algcd result:" << h << endl;
        DEBOUTLN(cout, "  alg_factor: h= ", h);
        DEBOUTLN(cout, "  alg_factor: oldord= ", oldord);
        if ( degree(h) > 0 ){ //otherwise it's a constant
          g= divide(g, h,as);
          DEBOUTLN(cout, "alg_factor: g/h= ", g);
          DEBOUTLN(cout, "alg_factor: s= ", s);
          DEBOUTLN(cout, "alg_factor: substlist= ", substlist);
          L.append(CFFactor(h,1));
        }
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
  DEBOUTLN(cout, "alg_factor: L= ", LL);
  DEBDECLEVEL(cout,"alg_factor");
  return LL;
}

static CFFList
endler( const CanonicalForm & f, const CFList & AS, const Varlist & uord ){
  CanonicalForm F=f, g, q,r;
  CFFList Output;
  CFList One, Two, asnew, as=AS;
  CFListIterator i,ii;
  VarlistIterator j;
  Variable vg;

  for (i=as; i.hasItem(); i++){
    g= i.getItem();
    if (g.deriv() == 0 ){
      DEBOUTLN(cout, "Inseperable extension detected: ", g);
      for (j=uord; j.hasItem(); j++){
        if ( degree(g,j.getItem()) > 0 ) vg= j.getItem();
      }
      // Now we have the highest transzendental in vg;
      DEBOUTLN(cout, "Transzendental is ", vg);
      CanonicalForm gg=-1*g[0];
      divrem(gg,vg,q,r); r= gg-q*vg;   gg= gg-r;
      //DEBOUTLN(cout, "q= ", q); DEBOUTLN(cout, "r= ", r);
      DEBOUTLN(cout, "  that is ", gg);
      DEBOUTLN(cout, "  maps to ", g+gg);
      One.insert(gg); Two.insert(g+gg);
      // Now transform all remaining polys in as:
      int x=0;
      for (ii=i; ii.hasItem(); ii++){
        if ( x != 0 ){
          divrem(ii.getItem(), gg, q,r);
//          cout << ii.getItem() << " divided by " << gg << endl;
          DEBOUTLN(cout, "q= ", q); DEBOUTLN(cout, "r= ", r);
          ii.append(ii.getItem()+q*g); ii.remove(1);
          DEBOUTLN(cout, "as= ", as);
        }
        x+= 1;
      }
      // Now transform F:
      divrem(F, gg, q,r);
      F= F+q*g;
      DEBOUTLN(cout, "new F= ", F);
    }
    else{ asnew.append(i.getItem());  }// just the identity
  }
  // factor F with minimal polys given in asnew:
  DEBOUTLN(cout, "Factor F=  ", F);
  DEBOUTLN(cout, "  with as= ", asnew);
  int success=0;
  CFFList factorlist= newcfactor(F,asnew, success);
  DEBOUTLN(cout, "  gives =  ", factorlist);
  DEBOUTLN(cout, "One= ", One);
  DEBOUTLN(cout, "Two= ", Two);

  // Transform back:
  for ( CFFListIterator k=factorlist; k.hasItem(); k++){
    CanonicalForm factor= k.getItem().factor();
    ii=One;
    for (i=Two; i.hasItem(); i++){
      DEBOUTLN(cout, "Mapping ", i.getItem());
      DEBOUTLN(cout, "     to ", ii.getItem());
      DEBOUTLN(cout, "     in ", factor);
      divrem(factor,i.getItem(),q,r); r=factor -q*i.getItem();
      DEBOUTLN(cout, "q= ", q); DEBOUTLN(cout, "r= ", r);
      factor= ii.getItem()*q +r; //
      ii++;
    }
    Output.append(CFFactor(factor,k.getItem().exp()));
  }

  return Output;
}


// 1) prepares data
// 2) for char=p we distinguish 3 cases:
//           no transcendentals, seperable and inseperable extensions
CFFList
newfactoras( const CanonicalForm & f, const CFList & as, int success){
  Variable vf=f.mvar();
  CFListIterator i;
  CFFListIterator jj;
  CFList reduceresult;
  CFFList result;

  success=1;
  DEBINCLEVEL(cout, "newfactoras");
  DEBOUTMSG(cerr, rcsid);
  DEBOUTLN(cout, "newfactoras called with f= ", f);
  DEBOUTLN(cout, "               content(f)= ", content(f));
  DEBOUTLN(cout, "                       as= ", as);
  DEBOUTLN(cout, "newfactoras: cls(vf)= ", cls(vf));
  DEBOUTLN(cout, "newfactoras: cls(as.getLast())= ", cls(as.getLast()));
  DEBOUTLN(cout, "newfactoras: degree(f,vf)= ", degree(f,vf));

// F1: [Test trivial cases]
// 1) first trivial cases:
  if ( (cls(vf) <= cls(as.getLast())) ||  degree(f,vf)<=1 ){
// ||( (as.length()==1) && (degree(f,vf)==3) && (degree(as.getFirst()==2)) )
    DEBDECLEVEL(cout,"newfactoras");
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
      //if ( degree(f,x) > 0 ){ // does it occure in f? RICHTIG?
        Astar.append(elem);
        ord.append(x);
        //}
    }
  }
  uord= Difference(uord,ord);
  DEBOUTLN(cout, "Astar is: ", Astar);
  DEBOUTLN(cout, "ord is: ", ord);
  DEBOUTLN(cout, "uord is: ", uord);

// 3) second trivial cases: we already prooved irr. of f over no extensions
  if ( Astar.length() == 0 ){
    DEBDECLEVEL(cout,"newfactoras");
    return CFFList(CFFactor(f,1));
  }

// 4) Try to obtain a partial factorization using prop2 and prop3
//    Use with caution! We have to proof these propositions first!
  // Not yet implemented

// 5) Look if elements in uord actually occure in any of the minimal
//    polynomials. If no element of uord occures in any of the minimal
//   polynomials, we don't have transzendentals.
  Varlist newuord=Var_is_in_AS(uord,Astar);
  DEBOUTLN(cout, "newuord is: ", newuord);

  CFFList Factorlist;
  Varlist gcdord= Union(ord,newuord); gcdord.append(f.mvar());
  // This is for now. we need alg_sqrfree implemented!
  //cout << "algcd(" << f << "," << f.deriv() << " as:" << Astar <<endl;
  //CanonicalForm Fgcd= algcd(f,f.deriv(),Astar,gcdord);
  CanonicalForm Fgcd;
	//if (Astar.length() >1)
        //  Fgcd= algcd(f,f.deriv(),Astar,gcdord);
	//else
	  Fgcd= alg_gcd(f,f.deriv(),Astar);
	//out_cf("algcd:",algcd(f,f.deriv(),Astar,gcdord),"\n");
	//out_cf("alg_gcd:",alg_gcd(f,f.deriv(),Astar),"\n");
 // cout << "algcd result:"  << Fgcd << endl;
  if ( Fgcd == 0 ) DEBOUTMSG(cerr, "WARNING: p'th root ?");
  if ( degree(Fgcd, f.mvar()) > 0 ){
    DEBOUTLN(cout, "Nontrivial GCD found of ", f);
    CanonicalForm Ggcd= divide(f, Fgcd,Astar);
    DEBOUTLN(cout, "  split into ", Fgcd);
    DEBOUTLN(cout, "         and ", Ggcd);
    Fgcd= pp(Fgcd); Ggcd= pp(Ggcd);
    DEBDECLEVEL(cout,"newfactoras");
    return myUnion(newfactoras(Fgcd,as,success) , newfactoras(Ggcd,as,success));
  }
  if ( getCharacteristic() > 0 ){

    // First look for extension!
    IntList degreelist;
    Variable vminpoly;
    for (i=Astar; i.hasItem(); i++){degreelist.append(degree(i.getItem()));}
    int extdeg= getextension(degreelist, degree(f));
    DEBOUTLN(cout, "Extension needed of degree ", extdeg);

    // Now the real stuff!
    if ( newuord.length() == 0 ){ // no transzendentals
      DEBOUTMSG(cout, "No transzendentals!");
      if ( extdeg > 1 ){
        CanonicalForm MIPO= generate_mipo( extdeg, vminpoly);
        DEBOUTLN(cout, "Minpoly produced ", MIPO);
        vminpoly= rootOf(MIPO);
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as);
      DEBDECLEVEL(cout,"newfactoras");
      return Factorlist;
    }
    else if ( inseperable(Astar) > 0 ){ // Look if extensions are seperable
      // a) Use Endler
      DEBOUTMSG(cout, "Inseperable extensions! Using Endler!");
      CFFList templist= endler(f,Astar, newuord);
      DEBOUTLN(cout, "Endler gives: ", templist);
      return templist;
    }
    else{ // we are on the save side: Use trager
      DEBOUTMSG(cout, "Only seperable extensions!");
      if (extdeg > 1 ){
        CanonicalForm MIPO=generate_mipo(extdeg, vminpoly );
        vminpoly= rootOf(MIPO);
        DEBOUTLN(cout, "Minpoly generated: ", MIPO);
        DEBOUTLN(cout, "vminpoly= ", vminpoly);
        DEBOUTLN(cout, "degree(vminpoly)= ", degree(vminpoly));
      }
      Factorlist= alg_factor(f, Astar, vminpoly, oldord, as);
      DEBDECLEVEL(cout,"newfactoras");
      return Factorlist;
    }
  }
  else{ // char=0 apply trager directly
    DEBOUTMSG(cout, "Char=0! Apply Trager!");
    Variable vminpoly;
    Factorlist= alg_factor(f, Astar, vminpoly, oldord, as);
      DEBDECLEVEL(cout,"newfactoras");
      return Factorlist;
  }

  DEBDECLEVEL(cout,"newfactoras");
  return CFFList(CFFactor(f,1));
}

CFFList
newcfactor(const CanonicalForm & f, const CFList & as, int success ){
  Off(SW_RATIONAL);
  CFFList Output, output, Factors=Factorize(f); On(SW_RATIONAL);
  Factors.removeFirst();

  if ( as.length() == 0 ){ success=1; return Factors;}
  if ( cls(f) <= cls(as.getLast()) ) { success=1; return Factors;}

  success=1;
  for ( CFFListIterator i=Factors; i.hasItem(); i++ ){
    output=newfactoras(i.getItem().factor(),as, success);
    for ( CFFListIterator j=output; j.hasItem(); j++)
      Output = myappend(Output,CFFactor(j.getItem().factor(),j.getItem().exp()*i.getItem().exp()));
  }
  return Output;
}

/*
$Log: not supported by cvs2svn $
Revision 1.9  2002/07/30 15:16:19  Singular
*hannes: fix for alg. extension

Revision 1.8  2001/08/06 08:32:53  Singular
* hannes: code cleanup

Revision 1.7  2001/06/27 13:58:05  Singular
*hannes/GP: debug newfactoras, char_series, ...

Revision 1.6  2001/06/21 14:57:04  Singular
*hannes/GP: Factorize, newfactoras, ...

Revision 1.5  2001/06/18 08:44:39  pfister
* hannes/GP/michael: factory debug, Factorize

Revision 1.4  1998/05/25 18:32:38  obachman
1998-05-25  Olaf Bachmann  <obachman@mathematik.uni-kl.de>

        * charset/alg_factor.cc: replaced identifiers 'random' by
        'myrandom' to avoid name conflicts with the built-in (stdlib)
        library function 'random' which might be a macro -- and, actually
        is  under gcc v 2.6.3

Revision 1.3  1998/03/12 12:34:24  schmidt
        * charset/csutil.cc, charset/alg_factor.cc: all references to
          `common_den()' replaced by `bCommonDen()'

Revision 1.2  1997/09/12 07:19:37  Singular
* hannes/michael: libfac-0.3.0

*/
