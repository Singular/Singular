////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
////////////////////////////////////////////////////////////
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
  DEBOUTLN(CERR, "          chosen vf= ", vf);

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
