////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
/* $Id$ */
/////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
#ifdef HAVE_IOSTREAM
#define CIN std::cin
#else
#define CIN cin
#endif
// Factor - Includes
#include <SqrFree.h>
#include <Factor.h>
#include <interrupt.h>
// Charset - Includes
#include "csutil.h"
#include "algfactor.h"
#include "alg_factor.h"
// Some CC's need this:
#include "charset.h"

#ifdef BASICSETDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(subfactorize_time);


// forward declarations:
static CFList     irras(CFList & AS, int &ja, CanonicalForm & reducible);

#ifdef BASICSETDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"

// the next computes a characteristic set (a basic set in Wang's sense)
CFList
BasicSet( const CFList &PS )
{
    CFList QS = PS, BS, RS;
    CanonicalForm b;
    int cb;

    DEBOUTLN(CERR, "BasicSet: called with ps= ", PS);
    if ( PS.length() < 2 ) return PS;
    while ( ! QS.isEmpty() ) {
        b = lowestRank( QS );
        cb = rank( b );
        DEBOUTLN(CERR, "BasicSet: choose b  = ", b);
        DEBOUTLN(CERR, "BasicSet: it's rank = ", cb);
        BS=Union(CFList(b),BS);//BS.append( b );
        if ( rank( b ) == 0 )
            return Union(PS, CFList(b)) ; // b should be the first elem!
        else {
            RS = CFList();
            // QS:= {q \in QS -{B} | q is reduced wrt b}
            // We can process whole QS, because b is not reduced wrt. b
            for ( CFListIterator i = QS; i.hasItem(); ++i )
                if ( degree( i.getItem(), cb ) < degree( b ) )
                    //RS.append( i.getItem() );
                    RS = Union(CFList(i.getItem()),RS);
            QS = RS;
        }
    }
    DEBOUTLN(CERR, "BasicSet: returning bs= ", BS);
    return BS;
}

int
checkok( const CFList & PS, CFList & FS2){
  CanonicalForm elem;

  for ( CFListIterator i=PS; i.hasItem(); i++){
    elem= i.getItem();
    for (CFListIterator j=FS2; j.hasItem(); j++){
      if (elem == j.getItem()){
         //        FS2= Difference(FS2,CFList(elem));
         return 0;
      }
    }
  }
  return 1;
}

#ifdef MCHARSETNDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"

// The modified CharSet (an extended characteristic set with certain factors
// canceled; this is a characteristic set in Wang's sense)
CFList
MCharSetN( const CFList &PS, PremForm & Remembern ){
  CFList QS = PS, RS = PS, CSet, OLDCS;

  DEBOUTLN(CERR, "MCharSetN: called with ps= ", PS);
  while ( ! RS.isEmpty() ) {
    CSet = BasicSet( QS );
    OLDCS=CSet;
    DEBOUTLN(CERR, "MCharSetN: CS= ", CSet);
//     if ( getNumVars(CSet.getFirst()) > 1 ){
//       //CSet = removecontent(CSet, Remembern);
// #ifdef MCHARSETNDEBUG
//       CERR << "MCharSetN: CSet= " << CSet << "\n";
// #endif
//     }
    Remembern.FS1 = Union(Remembern.FS1, initalset1(CSet));
    DEBOUTLN(CERR, "MCharSetN: Remembern.FS1= ", Remembern.FS1);
    DEBOUTLN(CERR, "MCharSetN: Remembern.FS2= ", Remembern.FS2);
    RS = CFList();
    if ( rank( CSet.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CSet );
      DEBOUT(CERR, "MCharSetN: Difference( ", QS);
      DEBOUT(CERR, " , ", CSet);
      DEBOUTLN(CERR, " ) = ", D);
//CERR << "MCharSetN: Difference( " << QS << " , " << CSet << " ) = " << D << "\n";
      //PremForm Oldremember=Remembern;
      //PremForm Newremember=Remembern;
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
        CanonicalForm r = Prem( i.getItem(), CSet );
        DEBOUT(CERR,"MCharSetN: Prem(", i.getItem()  );
        DEBOUT(CERR, ",", CSet);
        DEBOUTLN(CERR,") = ", r);
//CERR << "MCharSetN: Prem("<< i.getItem() << "," << CSet << ") = " << r << "\n";
        if ( r != 0 ){
          //removefactor( r, Newremember );
          removefactor( r, Remembern );
          //Remembern.FS2 = Union(Remembern.FS2, Newremember.FS2);
          //Newremember = Oldremember;
          //if ( cls(r) > 0 )
            //RS=Union(CFList(r),RS);//RS.append( r );
            RS=Union(RS,CFList(r));
        }
      }
      if ( ! checkok(RS,Remembern.FS2)) return CFList(CanonicalForm(1));
      DEBOUTLN(CERR, "MCharSetN: RS= ", RS);
      //QS = Union( QS, RS );
      QS = Union(OLDCS,RS);
      DEBOUTLN(CERR, "MCharSetN: QS= Union(QS,RS)= ", QS);
    }
    else{ return CFList(CanonicalForm(1)); }
  }
  DEBOUTLN(CERR, "MCharSetN: Removed factors: ", Remembern.FS2);
  DEBOUTLN(CERR, "MCharSetN: Remembern.FS1: ", Remembern.FS1);

  return CSet;
}


CFList
mcharset( const CFList &PS, PremForm & Remembern ){
  CFList cs= MCharSetN(PS, Remembern );
  CFList rs= remsetb(Difference(PS,cs),cs);

  DEBOUTLN(CERR, "mcharset: cs= ", cs);
  DEBOUTLN(CERR, "mcharset: rs= ", rs);
  if ( rs.length() > 0 )
    cs= mcharset(Union(PS,Union(cs,rs)), Remembern);
  return cs;
}

// the "original" extended characteristic set
CFList
CharSet( const CFList &PS ){
  CFList QS = PS, RS = PS, CSet;

  while ( ! RS.isEmpty() ) {
    CSet = BasicSet( QS );
    DEBOUTLN(CERR, "CharSet: CSet= ", CSet);
    RS = CFList();
    if ( rank( CSet.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CSet );
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
        CanonicalForm r = Prem( i.getItem(), CSet );
        if ( r != 0 )  RS=Union(CFList(r),RS);//RS.append( r );
      }
      QS = Union( QS, RS );
    }
  }
  return CSet;
}

static CFList
charseta( const CFList & PS ){
  CFList QS = PS, RS = PS, CSet;

  while ( ! RS.isEmpty() ) {
    CSet = CharSet( QS );
    RS = CFList();
    if ( rank( CSet.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CSet );
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
        CanonicalForm r = Prem( i.getItem(), CSet );
        if ( r != 0 )  RS=Union(CFList(r),RS);//RS.append( r );
      }
      QS = Union(CSet,Union( QS, RS ));
    }
    else return CFList(CanonicalForm(1));
  }
  return CSet;
}

static bool
contractsub( const CFList & cs1, const CFList & cs2 ){
  CFListIterator i;

  for ( i=cs1; i.hasItem(); i++ )
    if ( Prem(i.getItem(),cs2 ) != 0 ) return false;
  CFList is=initalset1(cs1);
  for ( i=is; i.hasItem(); i++ )
    if ( Prem(i.getItem(),cs2 ) == 0 ) return false;
  return true;
}

static ListCFList
contract( const ListCFList & cs){
  ListCFList mem,ts;
  CFList iitem,jitem;

  if ( cs.length() < 2 ) return cs;

  for ( ListCFListIterator i=cs; i.hasItem(); i++ ){
    iitem=i.getItem();
    if ( ! member(iitem, mem))
      for ( ListCFListIterator j=i; j.hasItem(); j++){
        jitem=j.getItem();
        if ( ! same( iitem, jitem ) )
          if ( ! member(jitem, mem))
            if ( contractsub(iitem, jitem) ){
              ts.append(jitem); mem.append(jitem);
            }
            else
              if ( contractsub(jitem, iitem) ){
                ts.append(iitem);
              }
      }
  }
  return Minus(cs,ts);
}

static ListCFList
adjoin(const CFList & is, const CFList & qs, const ListCFList & qh ){
  ListCFList iss,qhi;
  ListCFListIterator j;
  CFList iscopy,itt;
  CFListIterator i;
  CanonicalForm elem;
  int ind, length;

  for ( i=is ; i.hasItem(); i++ ){
    elem=i.getItem();
    if ( cls(elem) > 0 ) iscopy=Union(CFList(elem),iscopy);
  }
  if ( iscopy.isEmpty() ) return iss;
  qhi = MyDifference(qh,qs);
  length = qhi.length();
  for ( i =iscopy; i.hasItem(); i++){
    itt = Union(qs,CFList(i.getItem()));
    ind = 0;
    if ( length > 0 )
      for ( j=qhi; j.hasItem(); j++ )
        if ( subset(j.getItem(),itt )) ind=1;
    if ( ind == 0 ) iss.append(itt);
  }
  return iss;
}

static ListCFList
adjoinb(const CFList & is, const CFList & qs, const ListCFList & qh ,const CFList & cs){
  ListCFList iss,qhi;
  ListCFListIterator j;
  CFList iscopy,itt;
  CFListIterator i;
  CanonicalForm elem;
  int ind, length;

  for ( i=is ; i.hasItem(); i++ ){
    elem=i.getItem();
    if ( cls(elem) > 0 ) iscopy=Union(CFList(elem),iscopy);
  }
  if ( iscopy.isEmpty() ) return iss;
  qhi = MyDifference(qh,qs);
  length = qhi.length();
  for ( i =iscopy; i.hasItem(); i++){
    itt = Union(Union(qs,CFList(i.getItem())),cs);
    ind = 0;
    if ( length > 0 )
      for ( j=qhi; j.hasItem(); j++ )
        if ( subset(j.getItem(),itt )) ind=1;
    if ( ind == 0 ) {iss.append(itt);}
  }
  return iss;
}

static ListCFList
sort( const ListCFList & list_to_sort ){
  ListCFList output,copy=list_to_sort;
  CFList l,qs1,elem;

  l = copy.getLast(); copy.removeLast();
  if ( copy.length() == 0 ){ return ListCFList(l); }
  for ( ListCFListIterator i=copy ; i.hasItem(); i++ ){
    elem = i.getItem();
    if ( elem.length() > l.length() ) {
      output = MyUnion( ListCFList(l), output);
      l= elem;
    }
    else{ output = MyUnion(output, ListCFList(elem) ); }
  }
  //output = MyUnion(ListCFList(l),sort(output));
  output = MyUnion(ListCFList(l),output);
  return output;
}

#ifdef IRRCHARSERIESDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"

ListCFList
IrrCharSeries( const CFList &PS, int opt ){
  CanonicalForm reducible,reducible2;
  CFList qs,cs,factorset,is,ts;
  ListCFList pi,ppi,qqi,qsi,iss,qhi= ListCFList(PS);
  int nr_of_iteration=0,ts2,highestlevel=0;

//  CERR << getCharacteristic() << "\n";
  for ( CFListIterator Ps=PS; Ps.hasItem(); Ps++ )
    if ( level(Ps.getItem() ) > highestlevel ) highestlevel = level(Ps.getItem()) ;
//  for ( int xx=1; xx <= highestlevel; xx++)
//   CERR << Variable(xx) ;
//  CERR << "\n";
//  for ( CFListIterator Ps=PS; Ps.hasItem(); Ps++ )
//    CERR << Ps.getItem() << ", " ;//<< "\n";
//  CERR <<  "\n";
  while ( ! qhi.isEmpty() ) {
    qhi=sort(qhi);
    DEBOUTLN(CERR, "qhi is: ", qhi);
    qs=qhi.getFirst();
    DEBOUTLN(CERR, "qs  is: ", qs);
    DEBOUTLN(CERR, "ppi is: ", ppi);
    ListCFList ppi1,ppi2;
    select(ppi,qs.length(),ppi1,ppi2);
    DEBOUTLN(CERR, "ppi1 is: ", ppi1);
    DEBOUTLN(CERR, "ppi2 is: ", ppi2);
    qqi = MyUnion(ppi2,qqi);
    DEBOUTLN(CERR, "qqi is: ", qqi);
    if ( nr_of_iteration == 0 ){ nr_of_iteration += 1; ppi = ListCFList(); }
    else{ nr_of_iteration += 1; ppi = MyUnion(ListCFList(qs),ppi1); }
    DEBOUTLN(CERR,"ppi is: ", ppi);
    PremForm Remembern;
    cs = MCharSetN(qs,Remembern);
    DEBOUTLN(CERR, "cs is: ", cs);
    DEBOUTLN(CERR, "factorset is: ", Remembern.FS2);
    cs = removecontent(cs,Remembern);
    factorset=Remembern.FS2;
    DEBOUTLN(CERR, "cs (after removecontent) is: ", cs);
    DEBOUTLN(CERR, "factorset is: ", factorset);
    // Hier: removecontent einfuegen!!!!
    if ( cls(cs.getFirst()) > 0 ){
      ts = irras(cs,ts2, reducible);

      // INTERRUPTHANDLER
      if ( interrupt_handle() ) return ListCFList() ;
      // INTERRUPTHANDLER

      DEBOUTLN(CERR, "ts is: ", ts);
      DEBOUTLN(CERR, "ts2 is: ", ts2);
      // next is preliminary: should be ==0
      if ( ts2 <= 0 ){ //irreducible
        if ( ! subset(cs,qs) ){
          DEBOUTMSG(CERR, "cs is not a subset of qs");
          cs = charseta(Union(qs,cs));
          DEBOUTLN(CERR, "new cs is: ", cs);
        }
        if ( ! member(cs,pi) ){
          pi = MyUnion(pi, ListCFList(cs));
          DEBOUTMSG(CERR, "cs is not a member of pi");
          DEBOUTLN(CERR, "pi is: ", pi);
          if ( cls(cs.getFirst()) > 0 ){
            ts = irras(cs,ts2,reducible);

            // INTERRUPTHANDLER
            if ( interrupt_handle() ) return ListCFList() ;
            // INTERRUPTHANDLER

            DEBOUTLN(CERR, "ts is: ", ts);
            DEBOUTLN(CERR, "ts2 is: ", ts2);
            // next is preliminary: should be ==0
            if ( ts2 <= 0 ){ //irreducible
              qsi = MyUnion(qsi,ListCFList(cs));
              DEBOUTLN(CERR, "qsi is: ", qsi);
              if ( cs.length() == highestlevel ){
                DEBOUTLN(CERR, "cs.length() == nops(ord) :", cs.length());
                is = factorps(factorset);
              }
              else{
                DEBOUT(CERR,"cs.length() != nops(ord) :", cs.length());
                DEBOUTLN(CERR, "  nops(ord)= ", highestlevel);
                is = Union(initalset1(cs),factorps(factorset));
              }
              DEBOUTLN(CERR, "is is: ", is);
              iss = adjoin(is,qs,qqi);
              DEBOUTLN(CERR, "iss is: ", iss);
            }
          }
          else{ iss = adjoin(factorps(factorset),qs,qqi); }
        }
        else{
          DEBOUTMSG(CERR, "cs is a member of pi");
          iss = adjoin(factorps(factorset),qs,qqi); }
        DEBOUTLN(CERR, "iss is: ", iss);
        DEBOUTLN(CERR, "   factorps(factorset)= ", factorps(factorset));
        DEBOUTLN(CERR, "   qs= ", qs);
        DEBOUTLN(CERR, "   qqi= ", qqi);
      }
      // next is preliminary: should be !=0
      if ( ts2 > 0 ){
        is = factorps(factorset);
        DEBOUTLN(CERR, "is is: ", is);
        if ( ts2 > 1 ){
          // setup cst: need it later for adjoinb
          CFList cst;
          for ( CFListIterator i=cs ; i.hasItem(); i++){
            if ( i.getItem() == reducible ) { break; }
            else { cst.append(i.getItem()); }
          }
          is = Union(initalset1(cst), is);
          iss = MyUnion(adjoin(is,qs,qqi), adjoinb(ts,qs,qqi,cst));
        }
        else{ iss = adjoin(Union(is,ts),qs,qqi); }
        DEBOUTLN(CERR, "iss is: ", iss);
      }
    }
    else{
      iss = adjoin(factorps(factorset),qs,qqi);
      DEBOUTMSG(CERR, "case: cs is a constant.");
      DEBOUTLN(CERR, "  qs = ", qs);
      DEBOUTLN(CERR, "  qqi = ", qqi);
      DEBOUTLN(CERR, "  iss = adjoin(factorps(factorset),qs,qqi) = ",iss);
    }
    if ( qhi.length() > 1 ){ qhi.removeFirst(); qhi = MyUnion(qhi,iss); }
    else{ qhi = iss; }
    DEBOUTLN(CERR, "iss is: ", iss);
  }
  if ( ! qsi.isEmpty() ){
    DEBOUTLN(CERR, "qsi before contract= ", qsi);
    if ( opt == 0 ){
       return contract( qsi );
    }
    else { return qsi; }
  }
  else{ return ListCFList() ; }
}

// tests for characteristic sets
//////////////////////////////////
#ifdef IRRASDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"

static CFList
irras( CFList & AS, int & ja, CanonicalForm & reducible)
{
  CFFList qs;
  CFList ts,as;
  CanonicalForm elem;
  int ind=1,nr=0, success=-1;
  CFListIterator i;

  ja = 0;
  DEBOUTLN(CERR, "irras: called with: AS= ", AS);
  for ( i=AS; i.hasItem(); i++ )
  {
    elem = i.getItem();
    nr += 1;
    DEBOUT(CERR, "irras: factoring: ", elem);
    if ( degree(elem) > 1 ) // linear poly's are irreduzible
    {
      qs = Factorize(elem);
      // remove a constant
      if (qs.getFirst().factor().degree()==0) qs.removeFirst();
    }
    else
    {
      qs=(CFFactor(elem,1));
    }
    DEBOUTLN(CERR, "  = ", qs);
    // INTERRUPTHANDLER
    if ( interrupt_handle() ) return CFList() ;
    // INTERRUPTHANDLER

    if ( (qs.length() >= 2 ) || (qs.getFirst().exp() > 1))
    {
      DEBOUTLN(CERR, "irras: Setting ind=0, ja= ", nr);
      ja=nr; ind=0; reducible= elem;
      break;
    }
    //    else{ as.append(elem) ; }
  }
  //  CERR << "ind= " << ind << "\n";
  if ( (ind == 1) ) //&& ( as.length() > 1) )
  {
    if ( irreducible(AS) )
    { // as quasilinear? => irreducible!
      ja = 0;
      DEBOUTLN(CERR, "as is irreducible. as= ", AS);
    }
    else
    {
      i=AS;
      for ( nr=1; nr< AS.length(); nr++)
      {
        as.append(i.getItem());
        i++;
        if ( degree(i.getItem()) > 1 )
        {  // search for a non linear elem
          elem=i.getItem();
          //if (as.length()==1)
          //  qs = Factorize2(elem,as.getFirst());
          //else
            qs= newfactoras(elem,as,success);
          if ( qs.length() > 1 || qs.getFirst().exp() > 1 )
          { //found elem is reducible
            reducible=elem;
            ja=nr+1;
            break;
          }
        }
      }
    }
  }
  for ( CFFListIterator k=qs; k.hasItem();k++)
    ts.append(k.getItem().factor());
  return ts;
}

///////////////////////////////////////////////////////////////////////////////
