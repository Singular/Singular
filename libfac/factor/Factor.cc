///////////////////////////////////////////////////////////////////////////////
/*static const char * errmsg = "\nYou found a bug!\nPlease inform singular@mathematik.uni-kl.de\nPlease include above information and your input (the ideal/polynomial and characteristic) in your bug-report.\nThank you.";*/
///////////////////////////////////////////////////////////////////////////////
#include <string.h>
// FACTORY - Includes
#include <factory.h>
#ifndef NOSTREAMIO
#ifdef HAVE_IOSTREAM
#include <iostream>
#define CERR std::cerr
#define CIN std::cin
#elif defined(HAVE_IOSTREAM_H)
#include <iostream.h>
#define CERR cerr
#define CIN cin
#endif
#endif
// Factor - Includes
#include "tmpl_inst.h"
#include "SqrFree.h"
#include "helpstuff.h"
#include "MVMultiHensel.h"
#include "Truefactor.h"
#include "homogfactor.h"
#include "interrupt.h"
// some CC's need this:
#include "Factor.h"

#include "alg_factor.h"
void out_cf(const char *s1,const CanonicalForm &f,const char *s2);
void out_cff(CFFList &L);


#ifdef FACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(factorize_time);
TIMING_DEFINE_PRINT(sqrfree_time);
TIMING_DEFINE_PRINT(discr_time);
TIMING_DEFINE_PRINT(evaluate_time);
TIMING_DEFINE_PRINT(hensel_time);
TIMING_DEFINE_PRINT(truefactor_time);

/*
* a wrapper for factorize over algebraic extensions:
* does a sanity check and, if nec., a conversion
* before calling factorize(f,alpha)
* ( in factorize, alpha.level() must be < 0 )
*/
static
CFFList factorize2 ( const CanonicalForm & f,
                     const Variable & alpha, const CanonicalForm & mipo )
{
  if (alpha.level() <0)
  {
    return factorize(f,alpha);
  }
  else
  {
    //out_cf("f2 - factor:",f,"\n");
    //out_cf("f2 - ext:",alpha,"\n");
    //out_cf("f2 - mipo:",mipo,"\n");
    Variable X=rootOf(mipo);
    CanonicalForm F=f;
    F=replacevar(f,alpha,X);
    //out_cf("call - factor:",F,"\n");
    //out_cf("call - ext:",X,"\n");
    //out_cf("call - mipo:",getMipo(X,'A'),"\n");
    CFFList L=factorize(F,X);
    CFFListIterator i=L;
    {
      CFFList Outputlist;
      for(;i.hasItem(); i++ )
      {
        Outputlist.append(CFFactor(
        replacevar(i.getItem().factor(),X,alpha),
        i.getItem().exp()));
      }
      //out_cff(Outputlist);
      return Outputlist;
    }
  }
}
///////////////////////////////////////////////////////////////
// Choose a main variable if the user didn`t wish a          //
// special one. Returns level of main variable.              //
///////////////////////////////////////////////////////////////
static int
choose_main_variable( const CanonicalForm & f, int Mainvar=0){
  CanonicalForm remlc, newlc;
  int n= level(f), mainvar= Mainvar;

  if (mainvar != 0) return mainvar ; // We force use of the wished mainvar
  remlc= LC(f,n); mainvar = n;
  if ( totaldegree(remlc)==0 ){ remlc=f.genOne() ; }
  DEBOUTLN(CERR, "remlc= " , remlc);
  for ( int i=n-1; i>=1; i-- )
  {
    newlc= LC(f,i);
    if ( totaldegree(newlc)==0 ){ newlc=f.genOne() ; }
    DEBOUTLN(CERR, "newlc= " , newlc);
    if ( (remlc.isOne()) && (newlc.isOne()) ){ // take care of the degrees
      if ( degree(f,i) < degree(f,mainvar) ){
        remlc= newlc;
        mainvar= i;
      }
    }
    else  if ( (! remlc.isOne() ) && ( newlc.isOne() ) ){
      remlc= newlc;
      mainvar= i;
    }
  }
  return mainvar;
}

///////////////////////////////////////////////////////////////
// Check if the derivative is nonzero for oldmainvar.        //
// Returns the level of the chosen main variable.            //
///////////////////////////////////////////////////////////////
static int
necessary_condition( const CanonicalForm & F, int oldmainvar){
  CanonicalForm g;
  int n=level(F);

  g= swapvar(F,oldmainvar,n);
  g= g.deriv();
  if ( g.isZero() )
  {
    for ( int i=n; i>=1; i-- )
    {
      g= swapvar(F,i,n);
      g= g.deriv();
      if ( ! g.isZero() ) return i;
    }
  }
  return oldmainvar;
}

///////////////////////////////////////////////////////////////
// Make F monic. Return monic polynomial.                    //
///////////////////////////////////////////////////////////////
static CanonicalForm
make_monic( const CanonicalForm & F, const CanonicalForm & lt)
{
  CanonicalForm intermediatpoly,f;
  Variable x(level(F));

  if ( degree(lt) == 0 ) f= 1/lt * F ;
  else
  {
    intermediatpoly= power(lt,degree(F)-1);
    for ( int i=0; i<=degree(F); i++ )
      if ( ! F[i].isZero())
        f+= (F[i] * intermediatpoly*power(x,i))/power(lt,i);
  }
  return f;
}

///////////////////////////////////////////////////////////////
// Decide whether num/denum (num,denum both from the         //
// FiniteFielddomain)  lies in the RationalDomain.           //
// If false, return num/denum else return the zero poly from //
// the FiniteFielddomain.                                    //
///////////////////////////////////////////////////////////////
static CanonicalForm
is_rational( const CanonicalForm & num, const CanonicalForm & denum ){
  CanonicalForm a, b;
  int retvalue;

  retvalue= mydivremt(num,denum,a,b);
  if ( retvalue && b == num.genZero() ) // num/denum from FFdomain
    return a;
  else // num/denum is rational
    return num.genZero();
}

///////////////////////////////////////////////////////////////
// lt_is_product returns 1 iff lt is a product, 0 iff lt is  //
// a sum.                                                    //
///////////////////////////////////////////////////////////////
static int
lt_is_product( const CanonicalForm & lt ){
  CFList result;

  result= get_Terms(lt);
  if ( result.length() > 1 ) return 0;
  else return 1;
}

///////////////////////////////////////////////////////////////
// Reverse the make_monic transformation.                    //
// Return the list of factors.                               //
///////////////////////////////////////////////////////////////
static CFFList
not_monic( const CFFList & TheList, const CanonicalForm & ltt, const CanonicalForm & F, int levelF)
{
  CFFList Returnlist,IntermediateList;
  CFFListIterator i;
  CanonicalForm intermediate,lt= ltt,savelc;
  CanonicalForm numerator,denumerator,test,a,b;
  Variable x(level(F));
  int test1;

  if ( lt.isOne() ) return TheList; // the poly was already monic
  if ( TheList.length() <= 1 ) // only one factor to substitute back
  {
    if ( totaldegree(lt) == 0 ) // lt is type numeric
      Returnlist.append( CFFactor(lt*TheList.getFirst().factor(),
                                  TheList.getFirst().exp()) );
    else
    {
      intermediate = F(x*lt, levelF)/power(lt,degree(F,levelF)-1);
      Returnlist.append(CFFactor(intermediate,TheList.getFirst().exp()));
    }
  }
  else // more then one factor
  {
    IntermediateList= TheList;
    if ( totaldegree(lt) == 0 ){ // lt is type numeric;(SqrFree-use, see above)
      Returnlist.append( CFFactor(lt*IntermediateList.getFirst().factor()
                                  , IntermediateList.getFirst().exp()) );
      IntermediateList.removeFirst();
      Returnlist= Union(Returnlist,IntermediateList);
    }
    else // lt is a) a product or b) a sum of terms
    {
      if ( lt_is_product(lt) ) // case a)
      {
        DEBOUTLN(CERR, "lt_is_product: ", lt);
        savelc= content(lt) ; // can we simplify to savelc= lc(lt); ?
        while ( getNumVars(savelc) != 0 )
          savelc= content(savelc);
        for ( i=TheList; i.hasItem();i++ )
        {
          numerator= i.getItem().factor();
          numerator= numerator(x*lt,levelF); // x <- x*lt
          denumerator= power(lt,degree(F,levelF)-1); // == lt^(1-degree(F,x)
          while (numerator.genZero() == is_rational(numerator, denumerator))
            numerator*= lt;
          intermediate= is_rational(numerator,denumerator);

          Returnlist.append( CFFactor(lc(content(intermediate))*intermediate/content(intermediate), i.getItem().exp() ) );
        }
        // Now we add a test. If product(factors)/F is a multiple of
        // savelc, we have to add 1/multiplicity to the factors
        IntermediateList= Returnlist;
        intermediate= 1;
        for ( CFFListIterator j=IntermediateList; j.hasItem(); j++)
          intermediate*= j.getItem().factor();
        test1= mydivremt( intermediate,F,a,b);
        if ( test1 && b == intermediate.genZero() ) // Yupp!
        {
          IntermediateList.append(CFFactor(1/a,1));
          Returnlist= IntermediateList;
        }
        else { Returnlist= IntermediateList; }
      }
      else // case b)
      {
        DEBOUTLN(CERR, "lt_is_sum: ", lt);
        CanonicalForm save_denumerator= 1;
        for ( i=TheList; i.hasItem(); i++ )
        {
          numerator= i.getItem().factor();
          numerator= numerator(x*lt,levelF); // x <- x*lt
          denumerator= power(lt,degree(numerator,levelF)); // == lt^(-degree(numerator,x)
          test= content(numerator,x);
          test1= mydivremt(denumerator,test,a,b);
          if ( test1 && b == numerator.genZero() ) // Yupp!
          {
            save_denumerator*= a;
            Returnlist.append(CFFactor(numerator/test ,1));
          }
          else
          {
            factoryError("libfac: ERROR: not_monic1: case lt is a sum.");
          }
        }
        // Now we add a test if we did the right thing:
        // save_denumerator should be a multiple of the leading coeff
        test1= mydivremt(save_denumerator,lt,a,b);
        if ( test1 && b == save_denumerator.genZero() ) // Yupp!
          // We have to multiply one of the factors with
          // the multiplicity of the save_denumerator <-> lc
          // the following will do what we want
          Returnlist= myUnion( CFFList(CFFactor(1/a,1)),Returnlist) ;
        else
        {
          factoryError("libfac: ERROR: not_monic2: case lt is a sum.");
        }
      }
    }
  }
  DEBOUTLN(CERR,"Returnlist: ", Returnlist);
  return Returnlist;
}

///////////////////////////////////////////////////////////////
// Substitute the (Variable,Value)-Pair(s) from Substitution-//
// list into the polynomial F. Returns the resulting poly.   //
///////////////////////////////////////////////////////////////
static CanonicalForm
substitutePoly( const CanonicalForm & F, const SFormList & Substitutionlist){
  CanonicalForm f= F;

  for ( SFormListIterator i=Substitutionlist; i.hasItem(); i++ )
    f= f(i.getItem().exp(),level(i.getItem().factor()));
  return f;
}

///////////////////////////////////////////////////////////////
// Find specialization values for the poly F. Returns 0 if   //
// procedure failed, 1 otherwise. On success Substitutionlist//
// holds (Variable,Value)-pairs. On failure we only have a   //
// partitial list.                                           //
///////////////////////////////////////////////////////////////
//      *** This is the version with extensions ***          //
///////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////
// is CF g ok?                                               //
///////////////////////////////////////////////////////////////
static int
various_tests( const CanonicalForm & g, int deg, int vars_left)
{
  CFMap m;

  if ( degree(g) == deg ) // degrees match
    if ( level(compress(g,m)) == (vars_left) ) // exactly one variable less
      if ( SqrFreeTest(g,1) ) // poly is sqrfree
        if ( gcd(g,g.deriv()).isOne() ) // Discriminante != 0
           return 1;
  return 0;
}

///////////////////////////////////////////////////////////////
// specialize one variable over the given field.             //
///////////////////////////////////////////////////////////////
// substitutes in poly f of degree deg with former
// former_nr_of_variables variables the variable nr_of_variable ;
// this is done in the field of Char getCharacteristic() and
// Extension given by Extgenerator.
///////////////////////////////////////////////////////////////
static int
specialize_variable( CanonicalForm & f, int deg, SFormList & Substitutionlist, int nr_of_variable,
                     int former_nr_of_variables, CFGenerator & Extgenerator ){
  CanonicalForm g;
  Variable x(nr_of_variable);

  DEBOUTLN(CERR, "specialize_variable: called with: ", f);
  for ( Extgenerator.reset(); Extgenerator.hasItems(); Extgenerator.next() ){
    DEBOUTLN(CERR, "  specialize_variable: trying:  ", Extgenerator.item());
    g= f( Extgenerator.item(), x );
    DEBOUTLN(CERR, "  specialize_variable: resulting g= ", g);
    if ( various_tests(g,deg,former_nr_of_variables - nr_of_variable ) ){
      Substitutionlist.insert(SForm(x,Extgenerator.item())); // append (Var,value) pair
      f= g;
      return 1;
    }
  }
  return 0;
}
static int
specialize_agvariable( CanonicalForm & f, int deg, SFormList & Substitutionlist, int nr_of_variable,
                     int former_nr_of_variables, AlgExtGenerator & Extgenerator ){
  CanonicalForm g;
  Variable x(nr_of_variable);

  DEBOUTLN(CERR, "specialize_variable: called with: ", f);
  for ( Extgenerator.reset(); Extgenerator.hasItems(); Extgenerator.next() ){
    DEBOUTLN(CERR, "  specialize_variable: trying:  ", Extgenerator.item());
    g= f( Extgenerator.item(), x );
    DEBOUTLN(CERR, "  specialize_variable: resulting g= ", g);
    if ( various_tests(g,deg,former_nr_of_variables - nr_of_variable ) ){
      Substitutionlist.insert(SForm(x,Extgenerator.item())); // append (Var,value) pair
      f= g;
      return 1;
    }
  }
  return 0;
}

///////////////////////////////////////////////////////////////
// generate a minpoly of degree degree_of_Extension in the   //
// field getCharacteristik()^Extension.                      //
///////////////////////////////////////////////////////////////
CanonicalForm
generate_mipo( int degree_of_Extension , const Variable & Extension ){
  FFRandom gen;
  if (degree (Extension) < 0)
    factoryError("libfac: evaluate: Extension not inFF() or inGF() !");
  return find_irreducible( degree_of_Extension, gen, Variable(1) );
}

///////////////////////////////////////////////////////////////
// Try to find a specialization for f over the field of char //
// f.getCharacteristic() and (possible) extension defined by //
// the variable Extension .                                  //
// Returns 1 iff specialisation was found, 0 otherwise.      //
// If 0 is returned there are variables left to substitute.  //
// We check if Substitutionlist.length() > 0, i.e. we        //
// previously tried to find specialization values for some   //
// values. We take them and work with the resulting poly.    //
///////////////////////////////////////////////////////////////
static int
try_specializePoly(const CanonicalForm & f, const Variable & Extension, int deg, SFormList & Substitutionlist, int ii,int j)
{
  int ok,i= ii;
  CanonicalForm ff= f;

  if ( Substitutionlist.length() > 0 ){ // we formerly tried to specialize
    ff= substitutePoly(f,Substitutionlist); // substitute found values
    i= Substitutionlist.length() + 1;
  }

  if ( degree(Extension) > 0 )
  { // working over Extensions
    DEBOUTLN(CERR, "try_specializePoly: working over Extensions: ", Extension);
    if (Extension.level() > 0)
    {
    //  AlgExtGenerator g(Extension,minpoly );
    //  for ( int k=i ; k<j ; k++ ) // try to find specialization for all
    //  {                           // variables (# = k ) beginning with the
    //                             // starting value i
    //    ok= specialize_agvariable( ff, deg, Substitutionlist, k, j, g );
    //    if ( ! ok ) return 0; // we failed
    //  }
      #ifndef NDEBUG
        //printf("libfac: try_specializePoly: extension level >0\n");
      #endif
      return 0; // we failed
    }
    else
    {
      AlgExtGenerator g(Extension);
      for ( int k=i ; k<j ; k++ ) // try to find specialization for all
      {                           // variables (# = k ) beginning with the
                                 // starting value i
        ok= specialize_agvariable( ff, deg, Substitutionlist, k, j, g );
        if ( ! ok ) return 0; // we failed
      }
    }
  }
  else{ // working over the ground-field
    FFGenerator g;
    DEBOUTMSG(CERR, "try_specializePoly: working over the ground-field.");
    for ( int k=i ; k<j ; k++ ){
      ok= specialize_variable( ff, deg, Substitutionlist, k, j, g );
      if ( ! ok ) return 0; // we failed
    }
  }
  return 1;
}

static int
specializePoly(const CanonicalForm & f, Variable & Extension, int deg, SFormList & Substitutionlist, int i,int j)
{
  Variable minpoly= Extension;
  int ok,extended= degree(Extension), working_over_extension;

  // Remember if we are working over an extension-field
  if ( extended >= 2 )    { working_over_extension = 1; }
  else                    { working_over_extension = 0; extended = 1; }
  // First try:
  ok = try_specializePoly(f,minpoly,deg,Substitutionlist,i,j);
  while ( ! ok ) // we have to extend!
  {
    SFormList origS=Substitutionlist;
    extended+= 1;
    if ( ! working_over_extension )
    {
      if (!hasMipo(Extension))
        minpoly= rootOf (generate_mipo (extended, Extension));
      else
      {
        setReduce (Extension, false);
        setMipo (minpoly, generate_mipo ( extended, Extension));
        setReduce (Extension, true);
      }
      Extension= minpoly;
      ok= try_specializePoly(f,minpoly,deg,Substitutionlist,i,j);
      if (!ok)
        Substitutionlist=origS;
    }
    else
    {
      factoryError("libfac: spezializePoly ERROR: Working over given extension-field not yet implemented!");
      return 0;
    }
  }
  return 1;
}


// This is a procedure to play with: lot's of parameters!
// returns: 0  iff no success (possibly because Extension isn't great enough
//          >0 iff g (univariate) splits into n factors;
// if n>0 BestEvaluationpoint contains the choice of values for the variables
//
// tries to find at least maxtries evaluation points
// if g factored sametries into the same number of poly's the procedure stops
// if we tried failtries evaluations not found valid, we stop. Perhaps
// Extension isn't big enough!
static int
evaluate( int maxtries, int sametries, int failtries, const CanonicalForm &f , const Variable & Extension, const CanonicalForm &mipo, SFormList & BestEvaluationpoint, CFFList & BestFactorisation ){
  int minfactors=degree(f),degf=degree(f),n=level(f.mvar())-1;
  SFormList minEvaluation;
  CFFList minFactorisation;
  int samefactors=0, failedfactor=0, tried=0;
  FFRandom gen;
  CFFList unilist;

  if ( degree(Extension) >0 ) GFRandom gen;
  else
  {
    if ( degree(Extension) == 0 ) FFRandom gen;
    else
    {
      factoryError("libfac: evaluate: Extension not inFF() or inGF() !");
    }
  }
  REvaluation k(1,n,gen);
  k.nextpoint();
  for ( int i=1; i<=maxtries ; i++)
  {
    // k.nextpoint();
    SFormList Substitutionlist;
    for ( int j=1; j<=n; j++ )
     Substitutionlist.insert(SForm(Variable(j),k[j]));
    k.nextpoint();
    CanonicalForm g=substitutePoly(f,Substitutionlist);
    if ( various_tests(g, degf,1) )
    { // found a valid point
      failedfactor = 0; tried += 1;
      if ( degree(Extension) == 0   )
        unilist = factorize(g,1); // poly is sqr-free!
      else
      {
        unilist = factorize2(g,Extension,mipo);
      }
      if (unilist.length() <= minfactors )
      {
        minfactors=unilist.length();
        minEvaluation=Substitutionlist;
        minFactorisation=unilist;
      }
      else samefactors +=1;

      if (unilist.length() == 1 ) // wow! we found f is irreducible!
      {
        BestEvaluationpoint=minEvaluation;
        BestFactorisation=minFactorisation;
        return 1;
      }

      if ( samefactors >= sametries ) // now we stop ( maybe polynomial *has*
                                      // minfactors factors? )
      {
        BestEvaluationpoint=minEvaluation;
        BestFactorisation=minFactorisation;
        return minfactors;
      }

    }
    else
      failedfactor += 1;

    if ( failedfactor >= failtries ) // now we stop ( perhaps Extension isn't
                                     // big enough )
    {
      if ( tried == 0 )
        return 0;
      else
      {
        BestEvaluationpoint=minEvaluation;
        BestFactorisation=minFactorisation;
        return minfactors;
      }
    }
  }
  BestEvaluationpoint=minEvaluation;
  BestFactorisation=minFactorisation;
  return minfactors;
}

///////////////////////////////////////////////////////////////
// A factorization routine for a sqrfree polynomial.         //
// Returns the list of factors.                              //
///////////////////////////////////////////////////////////////
CFFList
Factorized( const CanonicalForm & F, const CanonicalForm & alpha, int Mainvar)
{
  CanonicalForm f,lt,ff,ffuni;
  Variable Extension=alpha.mvar();
  CFFList Outputlist,UnivariateFactorlist,Outputlist2;
  SFormList Substitutionlist, Evaluationpoint;
  CFFactor copy;
  int mainvar=Mainvar,success,oldmainvar;
  CFMap m;

  // INTERRUPTHANDLER
  if ( interrupt_handle() ) return CFFList() ;
  // INTERRUPTHANDLER

  if (F.inCoeffDomain()) {  return CFFList(CFFactor(F,1)); }
  if ( F.isUnivariate() ) // could have lost one Variable elsewhere
  {
    if ( degree(Extension) == 0 )
    {
      TIMING_START(evaluate_time);
      Outputlist = factorize(F,1); // poly is sqr-free
      TIMING_END(evaluate_time);
      return Outputlist;
    }
    else
    {
      if (Extension.level()<0)
      DEBOUTLN(CERR, "Univ. Factorization over extension of degree ",
               degree(getMipo(Extension,'x')) );
      else
      DEBOUTLN(CERR, "Univ. Factorization over extension of level ??",
                Extension.level());
      TIMING_START(evaluate_time);
      Outputlist = factorize2(F,Extension,alpha);
      TIMING_END(evaluate_time);
      return Outputlist;
    }
  }

  if ( Mainvar ) oldmainvar=Mainvar; else oldmainvar=level(F);
  // First choose a main variable; this may be revisted in the next step
  mainvar = choose_main_variable(F);
  // Let`s look if @f/@mainvar is nonzero
  mainvar = necessary_condition(F,mainvar);
  // Now we have definetly chosen a main variable
  // swap poly such that the mainvar has highest level
  f=swapvar(F,mainvar,level(F));

  // INTERRUPTHANDLER
  if ( interrupt_handle() ) return CFFList() ;
  // INTERRUPTHANDLER

  if ( oldmainvar != mainvar ){
    DEBOUTSL(CERR); DEBOUT(CERR,"Swapped poly ", F);
    DEBOUT(CERR, " in ", f); DEBOUTNL(CERR);
    DEBOUTSL(CERR); DEBOUT(CERR,"Swapped  ", oldmainvar );
    DEBOUT(CERR, " <-- ", mainvar ); DEBOUT(CERR, "  Mainvar= ", f.mvar());
    DEBOUTNL(CERR);
    ff = f.deriv();
    TIMING_START(discr_time);
    ffuni = gcd(f,ff);
    TIMING_END(discr_time);
    if ( !(ffuni.isOne()) ){ //discriminante nonzero: split poly
      DEBOUTLN(CERR,"Nontrivial GCD of f= ", f);
      DEBOUTLN(CERR,"             and @f= ", ff);
      DEBOUTLN(CERR,"          GCD(f,@f)= ", ffuni);
      ff=f/ffuni;
      CFFList Outputlist_a, Outputlist_b;
      Outputlist_a = Factorized(ff,alpha);
      DEBOUTLN(CERR, "Outputlist_a = ", Outputlist_a);
      Outputlist_b = Factorized(ffuni,alpha);
      DEBOUTLN(CERR, "Outputlist_b = ", Outputlist_b);
      Outputlist = myUnion(Outputlist_a, Outputlist_b);
      // have to back-swapvar the factors....
      for ( CFFListIterator i=Outputlist; i.hasItem(); i++ ){
        copy=i.getItem();
        Outputlist2.append(CFFactor(swapvar(copy.factor(),oldmainvar,mainvar),copy.exp()));
      }
      DEBOUTLN(CERR, "Outputlist2 (a+b swapped) (to return) = ", Outputlist2);
      return Outputlist2;
    }
  }

  // Check special cases
  for ( int i=1; i<=level(F); i++)
  {
    if ( degree(f,Variable(i) ) == 1 )
    //test trivial case; only true iff F is primitiv w.r.t every variable; else check (if F=ax+b) gcd(a,b)=1 ?
    {
      DEBOUTLN(CERR, "Trivial case: ", F);
      Outputlist.append(CFFactor(F,1));
      return Outputlist;
    }
  }

  // Look at the leading term:
  lt = LC(f);
  DEBOUTLN(CERR, "Leading term: ", lt);
  //if ( lt != f.genOne() )
  if ( !lt.isOne() )
  {
    // make the polynomial monic in the main variable
    ff = make_monic(f,lt); ffuni = ff;
    DEBOUTLN(CERR, "make_monic returned: ", ff);
  }
  else{ ff= f; ffuni= ff; }

  TIMING_START(evaluate_time);
  success=evaluate(min(10,max(degree(ff), 5)), min(degree(ff),3), min(degree(ff),3), ff, Extension, alpha, Substitutionlist,UnivariateFactorlist);
  DEBOUTLN(CERR,  "Returned from evaluate: success: ", success);
  for ( SFormListIterator ii=Substitutionlist; ii.hasItem(); ii++ )
  {
    DEBOUTLN(CERR, "Substituting ", ii.getItem().factor());
    DEBOUTLN(CERR, "       with value: ", ii.getItem().exp());
  }

  if ( success==0 ) // evalute wasn't successfull
  {
    success= specializePoly(ffuni,Extension,degree(ff),Substitutionlist,1,getNumVars(compress(ff,m)));
    DEBOUTLN(CERR,  "Returned from specializePoly: success: ", success);
    if (success == 0 ) // No spezialisation could be found
    {
      factoryError("libfac: Factorize: ERROR: Not able to find a valid specialization!");
      Outputlist.append(CFFactor(F,1));
      return Outputlist;
    }

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

    ffuni = substitutePoly(ff,Substitutionlist);
    // We now have an univariat poly; factorize that
    if ( degree(Extension) == 0   )
    {
      DEBOUTMSG(CERR, "Univ. Factorization over the ground field");
      UnivariateFactorlist = factorize(ffuni,1); // univ. poly is sqr-free!
    }
    else
    {
      DEBOUTLN(CERR, "Univ. Factorization over extension of degree ",
               degree(getMipo(Extension,'x')) );
      UnivariateFactorlist = factorize2(ffuni,Extension,alpha);
    }
  }
  else
  {
    ffuni = substitutePoly(ff,Substitutionlist);
  }
    TIMING_END(evaluate_time);
  if (UnivariateFactorlist.length() == 1)
  { // poly is irreduzibel
    DEBOUTLN(CERR, "Univ. poly is irreduzible: ", UnivariateFactorlist);
    Outputlist.append(CFFactor(F,1));
    return Outputlist;
  }
  else
  { // we have factors
    DEBOUTSL(CERR);
    DEBOUT(CERR, "Univariate poly has " , UnivariateFactorlist.length());
    DEBOUT(CERR, " factors:  ", ffuni);
    DEBOUT(CERR, " = ", UnivariateFactorlist); DEBOUTNL(CERR);

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

    TIMING_START(hensel_time);
    Outputlist = MultiHensel(ff,UnivariateFactorlist,Substitutionlist, alpha);
    DEBOUTLN(CERR, "Outputlist after MultiHensel: ", Outputlist);
    TIMING_END(hensel_time);

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

    TIMING_START(truefactor_time);
    Outputlist = Truefactors(ff, level(ff), Substitutionlist, Outputlist);
    DEBOUTLN(CERR, "Outputlist after Truefactors: ", Outputlist);
    TIMING_END(truefactor_time);

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

    //if ( lt != f.genOne() )
    if ( !lt.isOne() )
    {
      Outputlist = not_monic(Outputlist,lt,ff,level(ff));
      DEBOUTLN(CERR, "not_monic returned: ", Outputlist);
    }

    // have to back-swapvar the factors....
    for ( CFFListIterator i=Outputlist; i.hasItem(); i++ )
    {
      copy=i.getItem();
      Outputlist2.append(CFFactor(swapvar(copy.factor(),oldmainvar,mainvar),copy.exp()));
    }

    return Outputlist2;
  }
}

int cmpCF( const CFFactor & f, const CFFactor & g );

///////////////////////////////////////////////////////////////
// The user front-end for a uni/multivariate factorization   //
// routine. F needs not to be SqrFree.                       //
// Option of * choosing a  main variable (n.y.i.)            //
//           * choosing an algebraic extension (n.y.u.)      //
//           * ensuring poly is sqrfree (n.y.i.)             //
// use Factorize(F,alpha,is_SqrFree) if not over Zp[x]/Q[x]  //
///////////////////////////////////////////////////////////////
int find_mvar(const CanonicalForm &f);
CFFList Factorize(const CanonicalForm & F, int is_SqrFree )
{
  //out_cf("Factorize ",F,"\n");
  CFFList Outputlist;

  // INTERRUPTHANDLER
  if ( interrupt_handle() ) return CFFList() ;
  // INTERRUPTHANDLER

  DEBINCLEVEL(CERR, "Factorize");
  DEBOUTLN(CERR, "Called with F= ", F);
  if (( getCharacteristic() == 0 ) || (F.isUnivariate()))
  { // char == 0
    TIMING_START(factorize_time);
    //CERR << "Factoring in char=0 of " << F << " = " << Outputlist << "\n";
    Outputlist= factorize(F);
    // Factorization in char=0 doesn't sometimes return at least two elements!!!
    if ( getNumVars(Outputlist.getFirst().factor()) != 0 )
      Outputlist.insert(CFFactor(1,1));
    //CERR << "  Factorize in char=0: returning with: " << Outputlist << "\n";
    TIMING_END(factorize_time);
    DEBDECLEVEL(CERR, "Factorize");
    TIMING_PRINT(factorize_time, "\ntime used for factorization   : ");
    return Outputlist;
  }
  CFFList SqrFreeList,Intermediatelist,Outputlist2;
  ListIterator<CFFactor> i,j;
  CanonicalForm g=1,unit=1,r=1;
  Variable minpoly; // dummy
  int exp;
  CFMap m;
  TIMING_START(factorize_time);
  // search an "optimal" main variavble
  int mv=F.level();
  if ((mv != LEVELBASE) /* && (! F.isUnivariate()) */)
  {
     mv=find_mvar(F);
     if (mv!=F.level())
     {
       swapvar(F,Variable(mv),F.mvar());
     }
  }

  ///////
  // Maybe it`s better to add a sqrfree-test before?
  // (If gcd is fast...)
  ///////
  //  if ( ! SqrFreeTest(F) ){
  if ( ! is_SqrFree )
  {
    TIMING_START(sqrfree_time);
    SqrFreeList = SqrFreeMV(F) ; // first sqrfree the polynomial
    // don't use sqrFree(F), factory's internal sqrFree for multiv.
    // Polynomials; it's wrong!! Ex.: char=p   f= x^p*(y+1);
    // SqrFreeMV(f)= ( y+1, (x)^p ), sqrFree(f)= ( y+1 ) .
    TIMING_END(sqrfree_time);

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

  }
  else
    SqrFreeList.append(CFFactor(F,1));

  DEBOUTLN(CERR, "SqrFreeMV= ", SqrFreeList);
  for ( i=SqrFreeList; i.hasItem(); i++ )
  {
    DEBOUTLN(CERR, "Factor under consideration: ", i.getItem().factor());
    // We need a compress on each list item ! Maybe we have less variables!
    g =compress(i.getItem().factor(),m);
    exp = i.getItem().exp();
    if ( getNumVars(g) ==0 ) // a constant; Exp==1
      Outputlist.append( CFFactor(g,1) ) ;
    else// a real polynomial
      if ( g.isUnivariate() )
      {
        //out_cf("univ. poly: ",g,"\n");
        Intermediatelist=factorize(g,1); // poly is sqr-free!
        for ( j=Intermediatelist; j.hasItem(); j++ )
          //Normally j.getItem().exp() should be 1
          Outputlist.append( CFFactor( m(j.getItem().factor()),exp*j.getItem().exp()));
      }
      else
      { // multivariate polynomial
        if ( g.isHomogeneous() )
        {
          DEBOUTLN(CERR, "Poly is homogeneous! : ", g);
          // Now we can substitute one variable to 1, factorize and then
          // look on the resulting factors and their monomials for
          // backsubstitution of the substituted variable.
          Intermediatelist = HomogFactor(g, minpoly, 0);
        }
        else // not homogeneous
          Intermediatelist = Factorized(g, minpoly, 0);

        // INTERRUPTHANDLER
        if ( interrupt_handle() ) return CFFList() ;
        // INTERRUPTHANDLER

        for ( j=Intermediatelist; j.hasItem(); j++ )
          //Normally j.getItem().exp() should be 1
          Outputlist= myappend( Outputlist, CFFactor(m(j.getItem().factor()),exp*j.getItem().exp()));
      }
  }
  g=1; unit=1;
  DEBOUTLN(CERR, "Outputlist is ", Outputlist);
  for ( i=Outputlist; i.hasItem(); i++ )
    if ( level(i.getItem().factor()) > 0 )
    {
      unit = lc(i.getItem().factor());
      if ( getNumVars(unit) == 0 )
      { // a constant; possibly 1
        Outputlist2.append(CFFactor(i.getItem().factor()/unit , i.getItem().exp()));
        g *=power(i.getItem().factor()/unit,i.getItem().exp());
      }
      else
      {
        Outputlist2.append(i.getItem());
        g *=power(i.getItem().factor(),i.getItem().exp());
      }
    }

  r=F/g;
  Outputlist2.insert(CFFactor(r,1));

  if ((mv!=F.level()) && (! F.isUnivariate() ))
  {
    CFFListIterator J=Outputlist2;
    for ( ; J.hasItem(); J++)
    {
      swapvar(J.getItem().factor(),Variable(mv),F.mvar());
    }
    swapvar(F,Variable(mv),F.mvar());
  }
  DEBDECLEVEL(CERR, "Factorize");
  TIMING_END(factorize_time);

  TIMING_PRINT(sqrfree_time, "\ntime used for sqrfree   : ");
  TIMING_PRINT(discr_time, "time used for discriminante   : ");
  TIMING_PRINT(evaluate_time, "time used for evaluation and univ. factorization  : ");
  TIMING_PRINT(hensel_time, "time used for hensel-lift   : ");
  TIMING_PRINT(truefactor_time, "time used for truefactors   : ");
  TIMING_PRINT(factorize_time, "\ntime used for factorization   : ");

  if(isOn(SW_USE_NTL_SORT)) Outputlist2.sort(cmpCF);

  return Outputlist2;
}

///////////////////////////////////////////////////////////////
// The user front-end for a uni/multivariate factorization   //
// routine. F needs not to be SqrFree.                       //
// Option of * choosing a  main variable (n.y.i.)            //
//           * choosing an algebraic extension (n.y.u.)      //
//           * ensuring poly is sqrfree (n.y.i.)             //
///////////////////////////////////////////////////////////////
static bool fdivides2(const CanonicalForm &F, const CanonicalForm &G, const CanonicalForm &minpoly)
{
  if (!minpoly.isZero())
  {
  #if 0
    Variable Alpha=minpoly.mvar();
    Variable X=rootOf(minpoly);
    CanonicalForm rF=replacevar(F,Alpha,X);
    CanonicalForm rG=replacevar(G,Alpha,X);
    return fdivides(rF,rG);;
  #else
    if (degree(F,F.mvar()) > degree(G,F.mvar())) return false;
    return true;
    //CanonicalForm a,b;
    //mydivrem(G,F,a,b);
    //if (b.isZero()) return true;
    //else return false;
  #endif
  }
  else
   return fdivides(F,G);
}

CFFList
Factorize(const CanonicalForm & F, const CanonicalForm & minpoly, int is_SqrFree )
{
  //out_cf("Factorize: F=",F,"\n");
  //out_cf("           minpoly:",minpoly,"\n");
  CFFList Outputlist,SqrFreeList,Intermediatelist,Outputlist2;
  ListIterator<CFFactor> i,j;
  CanonicalForm g=1,unit=1,r=1;
  //Variable minpoly; // reserved (-> Factorisation over algebraic Extensions)
  int exp;
  CFMap m;

  // INTERRUPTHANDLER
  if ( interrupt_handle() ) return CFFList() ;
  // INTERRUPTHANDLER

  DEBINCLEVEL(CERR, "Factorize");
  DEBOUTLN(CERR, "Called with F= ", F);
  if ( getCharacteristic() == 0 )
  { // char == 0
    TIMING_START(factorize_time);
    //CERR << "Factoring in char=0 of " << F << " = " << Outputlist << "\n";
    #if 0
    // SHOULD: Outputlist= factorize(F,minpoly);
    Outputlist= factorize(F);
    #else
    if (!minpoly.isZero())
    {
      if ( F.isHomogeneous() )
      {
        DEBOUTLN(CERR, "Poly is homogeneous! : ", F);
        // Now we can substitute one variable to 1, factorize and then
        // look on the resulting factors and their monomials for
        // backsubstitution of the substituted variable.
        Outputlist=HomogFactor(F, minpoly, 0);
      }
      else
      {
        CFList as(minpoly);
        //CFFList sqF=sqrFree(F); // sqrFreeZ
        CFFList sqF=SqrFreeMV(F,minpoly);
        if (sqF.isEmpty()) sqF=sqrFree(F);
        CFFList G,H;
        CanonicalForm fac;
        int d;
        ListIterator<CFFactor> i,k;
        for ( i = sqF; i.hasItem(); ++i )
        {
          d = i.getItem().exp();
          fac = i.getItem().factor();
          int success=1;
          G = newfactoras( fac, as, success);
          for ( k = G; k.hasItem(); ++k )
          {
            fac = k.getItem().factor();
            int dd = k.getItem().exp();
            H.append( CFFactor( fac , d*dd ) );
          }
        }
        Outputlist = H;
      }
    }
    else // minpoly==0
      Outputlist=factorize(F);
    #endif
    // Factorization in char=0 doesn't sometimes return at least two elements!!!
    if ( getNumVars(Outputlist.getFirst().factor()) != 0 )
      Outputlist.insert(CFFactor(1,1));
    //CERR << "  Factorize in char=0: returning with: " << Outputlist << "\n";
    TIMING_END(factorize_time);
    DEBDECLEVEL(CERR, "Factorize");
    TIMING_PRINT(factorize_time, "\ntime used for factorization   : ");
    //out_cff(Outputlist);
    return Outputlist;
  }
  TIMING_START(factorize_time);
  // search an "optimal" main variavble
  int mv=F.level();
  if (mv != LEVELBASE && ! F.isUnivariate() )
  {
     mv=find_mvar(F);
     if (mv!=F.level())
     {
       swapvar(F,Variable(mv),F.mvar());
     }
  }

  ///////
  // Maybe it`s better to add a sqrfree-test before?
  // (If gcd is fast...)
  ///////
  //  if ( ! SqrFreeTest(F) ){
  if ( ! is_SqrFree )
  {
    TIMING_START(sqrfree_time);
    SqrFreeList = SqrFreeMV(F, minpoly) ; // first sqrfree the polynomial
    // don't use sqrFree(F), factory's internal sqrFree for multiv.
    // Polynomials; it's wrong!! Ex.: char=p   f= x^p*(y+1);
    // SqrFreeMV(f)= ( y+1, (x)^p ), sqrFree(f)= ( y+1 ) .
    TIMING_END(sqrfree_time);

    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFFList() ;
    // INTERRUPTHANDLER

  }
  else
    SqrFreeList.append(CFFactor(F,1));
  DEBOUTLN(CERR, "SqrFreeMV= ", SqrFreeList);
  for ( i=SqrFreeList; i.hasItem(); i++ )
  {
    DEBOUTLN(CERR, "Factor under consideration: ", i.getItem().factor());
    // We need a compress on each list item ! Maybe we have less variables!
    g =compress(i.getItem().factor(),m);
    exp = i.getItem().exp();
    if ( getNumVars(g) ==0 ) // a constant; Exp==1
      Outputlist.append( CFFactor(g,1) ) ;
    else// a real polynomial
    {
      if ( g.isUnivariate() )
      {
        Variable alpha=rootOf(minpoly);
        Intermediatelist=factorize2(g,alpha,minpoly); // poly is sqr-free!
        for ( j=Intermediatelist; j.hasItem(); j++ )
          //Normally j.getItem().exp() should be 1
          Outputlist.append(
           CFFactor( m(replacevar(j.getItem().factor(),alpha,minpoly.mvar())),
             exp*j.getItem().exp()));
      }
      else // multivariate polynomial
      {
        if ( g.isHomogeneous() )
        {
          DEBOUTLN(CERR, "Poly is homogeneous! : ", g);
          // Now we can substitute one variable to 1, factorize and then
          // look on the resulting factors and their monomials for
          // backsubstitution of the substituted variable.
          Intermediatelist = HomogFactor(g, minpoly, 0);
        }
        else // not homogeneous
          Intermediatelist = Factorized(g, minpoly, 0);

        // INTERRUPTHANDLER
        if ( interrupt_handle() ) return CFFList() ;
        // INTERRUPTHANDLER

        for ( j=Intermediatelist; j.hasItem(); j++ )
          //Normally j.getItem().exp() should be 1
          Outputlist= myappend( Outputlist, CFFactor(m(j.getItem().factor()),exp*j.getItem().exp()));
      }
    }
  }
  g=1; unit=1;
  DEBOUTLN(CERR, "Outputlist is ", Outputlist);
  for ( i=Outputlist; i.hasItem(); i++ )
    if ( level(i.getItem().factor()) > 0 )
    {
      unit = lc(i.getItem().factor());
      if ( getNumVars(unit) == 0 ){ // a constant; possibly 1
        Outputlist2.append(CFFactor(i.getItem().factor()/unit , i.getItem().exp()));
        g *=power(i.getItem().factor()/unit,i.getItem().exp());
      }
      else
      {
        Outputlist2.append(i.getItem());
        g *=power(i.getItem().factor(),i.getItem().exp());
      }
    }

  r=F/g;
  Outputlist2.insert(CFFactor(r,1));

  if ((mv!=F.level()) && (! F.isUnivariate() ))
  {
    CFFListIterator J=Outputlist2;
    for ( ; J.hasItem(); J++)
    {
      swapvar(J.getItem().factor(),Variable(mv),F.mvar());
    }
    swapvar(F,Variable(mv),F.mvar());
  }

  DEBDECLEVEL(CERR, "Factorize");
  TIMING_END(factorize_time);

  TIMING_PRINT(sqrfree_time, "\ntime used for sqrfree   : ");
  TIMING_PRINT(discr_time, "time used for discriminante   : ");
  TIMING_PRINT(evaluate_time, "time used for evaluation and univ. factorization  : ");
  TIMING_PRINT(hensel_time, "time used for hensel-lift   : ");
  TIMING_PRINT(truefactor_time, "time used for truefactors   : ");
  TIMING_PRINT(factorize_time, "\ntime used for factorization   : ");

  if(isOn(SW_USE_NTL_SORT)) Outputlist2.sort(cmpCF);

  //out_cff(Outputlist2);
  return Outputlist2;
}

