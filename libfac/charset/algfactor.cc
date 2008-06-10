////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
/* $Id: algfactor.cc,v 1.13 2008-06-10 14:49:14 Singular Exp $ */
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <tmpl_inst.h>
#include <Factor.h>
#include <helpstuff.h>
// Charset - Includes
#include "csutil.h"
#include "charset.h"
#include "reorder.h"
#include "alg_factor.h"
// some CC's need this:
#include "algfactor.h"

#ifdef ALGFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(newfactoras_time);

int hasVar(const CanonicalForm &f, const Variable &v);

static CFFList
myminus( const CFFList & Inputlist, const CFFactor & TheFactor){
  CFFList Outputlist ;
  CFFactor copy;

  for ( CFFListIterator i=Inputlist ; i.hasItem() ; i++ ){
    copy = i.getItem();
    if ( copy.factor() != TheFactor.factor() ){
      Outputlist.append( copy );
    }
  }
  return Outputlist;
}

static CFFList
myDifference(const CFFList & Inputlist1,const CFFList & Inputlist2){
  CFFList Outputlist=Inputlist1;

  for ( CFFListIterator i=Inputlist2 ; i.hasItem() ; i++ )
    Outputlist = myminus(Outputlist, i.getItem() );

  return Outputlist;
}


// return 1 if as is quasilinear; 0 if not.
static int
isquasilinear( const CFList & as ){
  if (as.length() <= 1) { return 1; }
  else {
    CFList AS=as;
    AS.removeFirst(); // we are not interested in the first elem
    for ( CFListIterator i=AS; i.hasItem(); i++)
      if ( degree(i.getItem()) > 1 ) return 0;
  }
return 1;
}

#ifdef CHARSETNADEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"
static CFList
charsetnA(const CFList & AS, const CFList &PS, PremForm & Remembern, const Variable & vf ){
  CFList QS = PS, RS = PS, Cset;
  int nas= AS.length() +1;

  DEBOUTLN(CERR, "charsetnA: called with ps= ", PS);
  while ( ! RS.isEmpty() ) {
    Cset = BasicSet( QS );
    DEBOUTLN(CERR, "charsetnA: Cset= ", Cset);
    Cset=Union(Cset,AS);
    DEBOUTLN(CERR, "charsetnA: Cset= ", Cset);
    Remembern.FS1 = Union(Remembern.FS1, initalset1(Cset));
    DEBOUTLN(CERR, "charsetnA: Remembern.FS1= ", Remembern.FS1);
    DEBOUTLN(CERR, "charsetnA: Remembern.FS2= ", Remembern.FS2);
    RS = CFList();
    if ( Cset.length() == nas && degree(Cset.getLast(),vf) > 0 ) {
      CFList D = Difference( QS, Cset );
      DEBOUT(CERR, "charsetnA: Difference( ", QS);
      DEBOUT(CERR, " , ", Cset);
      DEBOUTLN(CERR, " ) = ", D);
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
        CanonicalForm r = Prem( i.getItem(), Cset );
        DEBOUT(CERR,"charsetnA: Prem(", i.getItem()  );
        DEBOUT(CERR, ",", Cset);
        DEBOUTLN(CERR,") = ", r);
        if ( r != 0 ){
          //removefactor( r, Remembern );
            RS=Union(RS,CFList(r));
        }
      }
      if ( ! checkok(RS,Remembern.FS2)) return CFList(CanonicalForm(1));
      DEBOUTLN(CERR, "charsetnA: RS= ", RS);
      //QS = Union( QS, RS );
      QS=Union(AS,RS); QS.append(Cset.getLast());
      DEBOUTLN(CERR, "charsetnA: QS= Union(QS,RS)= ", QS);
    }
    else{ return CFList(CanonicalForm(1)); }
  }
  DEBOUTLN(CERR, "charsetnA: Removed factors: ", Remembern.FS2);
  DEBOUTLN(CERR, "charsetnA: Remembern.FS1: ", Remembern.FS1);

  return Cset;
}

#ifdef ALGFACTORDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"
// compute the GCD of f and g over the algebraic field having
// adjoing ascending set as
CanonicalForm
algcd(const CanonicalForm & F, const CanonicalForm & g, const CFList & as, const Varlist & order){
  CanonicalForm f=F;
  int nas= as.length()+1; // the length the new char. set should have!
  Variable vf=f.mvar();

//   for ( VarlistIterator i=order; i.hasItem() ; i++ ){
//     vf= i.getItem();
//     nas--;
//     if ( nas==0 ) break;
//   }
//   nas= as.length()+1;

  DEBOUTLN(CERR, "algcd called with f= ", f);
  DEBOUTLN(CERR, "                  g= ", g);
  DEBOUTLN(CERR, "                 as= ", as);
  DEBOUTLN(CERR, "              order= ", order);
  DEBOUTLN(CERR, "         choosen vf= ", vf);

  // check trivial case:
  if ( degree(f, order.getLast())==0 || degree(g, order.getLast())==0)
  {
    DEBOUTLN(CERR, "algcd Result= ", 1);
    return CanonicalForm(1);
  }

  CFList bs; bs.append(f); bs.append(g);
  PremForm Remembern;
  CFList cs=charsetnA(as,bs,Remembern,vf);
  DEBOUTLN(CERR, "CharSetA((as,bs))= ", cs);

//    for ( VarlistIterator i=order; i.hasItem() ; i++ ){
//      DEBOUTLN(CERR, "vf= ", i.getItem());
//      DEBOUTLN(CERR, "CharSetA((as,bs))= " , charsetnA(as,bs,Remembern,i.getItem()));
//    }

  CanonicalForm result;
  if (cs.length()==nas)
  {
    result= cs.getLast();
    CanonicalForm c=vcontent(result,Variable(1));
    //CanonicalForm c=result.Lc();
    result/=c;
    for(CFListIterator i=as;i.hasItem(); i++ )
    {
      if(hasVar(result,i.getItem().mvar()))
      {
        c=vcontent(result,Variable(i.getItem().level()+1));
        result/=c;
      }
    }
  }
  else result= CanonicalForm(1);
  DEBOUTLN(CERR, "algcd Result= ", result);
  return result;
}

CFFList
factoras( const CanonicalForm & f, const CFList & as, int & success ){
  Variable vf=f.mvar();
  CFListIterator i;
  CFFListIterator jj;
  CFList reduceresult;
  CFFList result;

  DEBINCLEVEL(CERR, "factoras");
  DEBOUTLN(CERR, "factoras called with f= ", f);
  DEBOUTLN(CERR, "               content(f)= ", content(f));
  DEBOUTLN(CERR, "                       as= ", as);
  DEBOUTLN(CERR, "factoras: cls(vf)= ", cls(vf));
  DEBOUTLN(CERR, "factoras: cls(as.getLast())= ", cls(as.getLast()));
  DEBOUTLN(CERR, "factoras: degree(f,vf)= ", degree(f,vf));

// F1: [Test trivial cases]
// 1) first trivial cases:
  if ( (cls(vf) <= cls(as.getLast())) ||
       degree(f,vf)<=1
// ||( (as.length()==1) && (degree(f,vf)==3) && (degree(as.getFirst()==2)) )
       ){
    success=1;
    DEBDECLEVEL(CERR,"factoras");
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
  DEBOUTLN(CERR, "Astar is: ", Astar);
  DEBOUTLN(CERR, "ord is: ", ord);
  DEBOUTLN(CERR, "uord is: ", uord);

// 3) second trivial cases
  if ( Astar.length() == 0 ){
    success=1;
    DEBDECLEVEL(CERR,"factoras");
    return CFFList(CFFactor(f,1));
  }

// 4) Try to obtain a partial factorization using prop2 and prop3
  // Not yet implemented

// 5) choose (d_1, ..., d_s)
  FFRandom gen;
  if ( getCharacteristic() == 0 ) IntRandom gen(100);
  REvaluation D(1, ord.length(), gen);
  //D.nextpoint(); // first we use 0 as evaluation point

F2: //[Characteristic set computation]
// 6) Compute f^* (<- fstar)
  CanonicalForm substhin=CanonicalForm(vf), substback=CanonicalForm(vf);
  CanonicalForm monom;
  DEBOUTLN(CERR, "substhin= ", substhin);
  DEBOUTLN(CERR, "substback= ", substback);
  int j=1;
  for ( VarlistIterator jjj=ord; jjj.hasItem(); jjj++){
    if ( getCharacteristic() > 0 ) monom= jjj.getItem()*D[j];
    else  monom= mapinto(jjj.getItem()*mapinto(D[j]));
    j++;
    substhin-= monom; substback+= monom;
  }
  DEBOUTLN(CERR, "substhin= ", substhin);
  DEBOUTLN(CERR, "substback= ", substback);
  CanonicalForm fstar=f(substhin,vf);
  DEBOUTLN(CERR, "fstar= ", fstar);

// 7) Set up Variable ordering from ord,vf to vf,ord ! i.e. vf is the variable
//    with lowest level.
  Varlist nord=uord;
  nord.append(vf); nord= Union(ord,nord);
  DEBOUTLN(CERR, "          nord= ", nord);
  CFList Astarnord= Astar; Astarnord.insert(fstar);
  DEBOUTLN(CERR, "     Astarnord= ", Astarnord);
  Astarnord= reorder(nord,Astarnord);
  DEBOUTLN(CERR, "original Astar= ", Astar);
  DEBOUTLN(CERR, "reorderd Astar= ", Astarnord);
  DEBOUTLN(CERR, "             f= ", f);
  DEBOUTLN(CERR, "         fstar= ", fstar);

// 8) Compute Charset Cstar of Astar \cup { fstar } wrt. ordering {vf, ord}
  PremForm Remembern;
  CFList Cstar= MCharSetN(Astarnord, Remembern);
  DEBOUTLN(CERR, " Cstar= ", Cstar );
  DEBOUTLN(CERR, " Factors removed= ", Remembern.FS2 );
  DEBOUTLN(CERR, " Possible Factors considered:= ", Remembern.FS1 );
  DEBOUTLN(CERR, " Reorderd Cstar= ", reorder(nord,Cstar));

// 9) Compute Delta: the set of all irr. factors (over K_0) of initials of
//    Cstar
  CFList iniset= initalset1(Cstar);
  DEBOUTLN(CERR, "Set of initials: ", iniset);
  CFFList Delta;
  CFFList temp;
  for ( i=iniset; i.hasItem(); i++){
    Off(SW_RATIONAL);
    temp= Factorize(i.getItem());
    On(SW_RATIONAL);
    temp.removeFirst();
    Delta= Union(temp,Delta);
  }
  DEBOUTLN(CERR, "Delta= ", Delta);

// 10) Compute Psi: the irreduzible factors (over K_0) of the first polynomial
//     in Cstar, which are not factors of any polynomial in Delta
  Off(SW_RATIONAL);
  CFFList Psi=Factorize(Cstar.getFirst());
  On(SW_RATIONAL);
  Psi.removeFirst();
  Psi= myDifference(Psi,Delta);
  DEBOUTLN(CERR, "Psi= ", Psi);

// F3: [Test quasilinearity]
// If Cstar is quasilinear -> F4
// else if Psi.length() > 1 then Delta<- Delta \cup Psi; Psi<- \emptyset -> F4
// else D.nextpoint() -> F2
  if ( isquasilinear(Cstar) ) {
    DEBOUTLN(CERR, "Cstar is quasilinear; going to F4, Cstar= ", Cstar);
    goto F4;
  }
  else if ( Psi.length() > 1 ){
    Delta= Union(Psi,Delta);
    Psi= CFFList();
    DEBOUTMSG(CERR, "Psi.length() == 1; going to F4");
    goto F4;
  }
  else{
    D.nextpoint();
    DEBOUTMSG(CERR, "Choosing next evaluation point. going to F2");
    goto F2;
  }

F4: //[GCD Computation]
  CanonicalForm g=f;
  Delta= reorder(nord,Delta);
  Psi= reorder(nord,Psi);
  DEBOUTLN(CERR, "Reordered: Delta= ", Delta);
  DEBOUTLN(CERR, "             Psi= ", Psi);
  CanonicalForm fp;

  DEBOUTMSG(CERR, "Testing Psi: this gives irreducible Factors!");
  for (jj=Psi; jj.hasItem(); jj++){
    if ( degree(g,vf) == 1 ) { // g is linear
      break;
    }
    fp= jj.getItem().factor();
    DEBOUT(CERR, "Calculating fp= gcd(", g);
    DEBOUT(CERR, ",", fp(substback,vf));
    DEBOUT(CERR, ") over K_r wrt ", vf);
    fp=alg_gcd(g,fp(substback,vf), as);
    //fp= algcd(g,fp(substback,vf), as, oldord);
    DEBOUTLN(CERR, " = ", fp);
    if ( degree(fp,vf) > 0 ){ //otherwise it's a constant
      g= divide(g, fp,as);
      DEBOUTLN(CERR, "f/fp= ", g);
      result.append(CFFactor(fp,1));
    }
  }

  DEBOUTMSG(CERR, "Testing Delta: this gives Factors (not nec. irreduzible!)");
  for (jj=Delta; jj.hasItem(); jj++){
    if ( degree(g,vf) <= 1 ) { // g is linear (or a constant..)
      break;
    }
    fp= jj.getItem().factor();
    DEBOUT(CERR, "Calculating fp= gcd(", g);
    DEBOUT(CERR, ",", fp(substback,vf));
    DEBOUT(CERR, ") over K_r wrt ", vf);
    fp= alg_gcd(g,fp(substback,vf), as);
    //fp= algcd(g,fp(substback,vf), as, oldord);
    DEBOUTLN(CERR, " = ", fp);
    if ( degree(fp,vf) > 0 ){ //otherwise it's a constant
      g= divide(g, fp,as);
      DEBOUTLN(CERR, "f/fp= ", g);
//      reduceresult.append(fp); // a facctor but not nec. irreduzible
      result.append(CFFactor(fp,1));
      success=0;
    }
  }

//  if (reduceresult.length() > 0){ //we have factors;
//    for ( CFListIterator I=reduceresult; I.hasItem(); I++ ){
//      // try to obtain an irreducible factorization
//      result= Union( result,
//                     factoras(I.getItem(), Astar, success) );
//    }
//  }

  if ( result.length()==0 ){
    if ( isquasilinear(Cstar) ){ // Cstar quasilinear => f is irreduzible
      success=1;
      DEBDECLEVEL(CERR,"factoras");
      return CFFList(CFFactor(f,1));
    }
    else {
      DEBOUTMSG(CERR, "Going to F2!");
      D.nextpoint(); // choose a new set of int's.
      goto F2;
    }
  }
  // result.length() > 0 ==> we have factors!
  success=1;
  result.append(CFFactor(g,1)); //append the rest
  DEBDECLEVEL(CERR,"factoras");
  return result;
}

// for now: success=-1 no success, success=1   factored poly,
// success = 0 factored poly partially
CFFList
cfactor(const CanonicalForm & f, const CFList & as, int success ){
  Off(SW_RATIONAL);
  CFFList Output, output, Factors=Factorize(f); On(SW_RATIONAL);
  int csuccess=0;
  Factors.removeFirst();

  if ( as.length() == 0 ){ success=1; return Factors;}
  if ( cls(f) <= cls(as.getLast()) ) { success=1; return Factors;}

  success=1;
  for ( CFFListIterator i=Factors; i.hasItem(); i++ ){
    CFFList CERR=factoras(i.getItem().factor(),as,csuccess);
    success= min(success,csuccess);
    for ( CFFListIterator j=CERR; j.hasItem(); j++)
      Output = myappend(Output,CFFactor(j.getItem().factor(),j.getItem().exp()*i.getItem().exp()));
  }
  return Output;
}

// Gegenbeispiel: char(k)=0 (original maple von wang):
// f:= x^2*z^2-6909*x*z^2-3537*z^2-14344*x^2;
// as:= [x^3-9977*x^2+15570*x-961]
// ord:= [x,z]
// cfactor(f,as,ord)=
// -1/153600624478379*(-153600624478379*z^2+96784194432*x^2-246879593179080*x+
// 13599716437688)*(x^2-6909*x-3537)
//
// maple:
//  alias(alpha=RootOf(x^3-9977*x^2+15570*x-961,x));
//  factor(f,alpha); // faktorisierung in char=0
// x^2*z^2-6909*x*z^2-3537*z^2-14344*x^2


/*
$Log: not supported by cvs2svn $
Revision 1.12  2008/04/08 16:19:09  Singular
*hannes: removed rcsid

Revision 1.11  2008/03/18 17:46:14  Singular
*hannes: gcc 4.2

Revision 1.10  2008/03/17 17:44:16  Singular
*hannes: fact.tst

Revision 1.8  2006/06/19 13:37:46  Singular
*hannes: more CS renamed

Revision 1.7  2006/05/16 14:46:48  Singular
*hannes: gcc 4.1 fixes

Revision 1.6  2002/08/19 11:11:30  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.5  2001/06/27 13:58:05  Singular
*hannes/GP: debug newfactoras, char_series, ...

Revision 1.4  2001/06/21 14:57:04  Singular
*hannes/GP: Factorize, newfactoras, ...

Revision 1.3  2001/06/18 08:44:40  pfister
* hannes/GP/michael: factory debug, Factorize

Revision 1.2  1997/09/12 07:19:38  Singular
* hannes/michael: libfac-0.3.0

*/
