/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
static char * rcsid = "$Id: charset.cc,v 1.1.1.1 1997-05-02 17:00:44 Singular Exp $";
/////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <SqrFree.h>
#include <Factor.h>
// Charset - Includes
#include "csutil.h"

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

CFList
BasicSet( const CFList &PS )
{
    CFList QS = PS, BS, RS;
    CanonicalForm b;
    int cb;

    DEBOUTLN(cout, "BasicSet: called with ps= ", PS);
    if ( PS.length() < 2 ) return PS;
    while ( ! QS.isEmpty() ) {
	b = lowestRank( QS );
	cb = rank( b );
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
    DEBOUTLN(cout, "BasicSet: returning bs= ", BS);
    return BS;
}

 int
 checkok( const CFList & PS, CFList & FS2){
   CanonicalForm elem;

   for ( CFListIterator i=PS; i.hasItem(); i++){
     elem= i.getItem();
     for (CFListIterator j=FS2; j.hasItem(); j++){
       if (elem == j.getItem()){
 	//	FS2= Difference(FS2,CFList(elem));
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

// The modified CharSet
CFList
MCharSetN( const CFList &PS, PremForm & Remembern ){
  CFList QS = PS, RS = PS, CS, OLDCS;

  DEBOUTLN(cout, "MCharSetN: called with ps= ", PS);
  while ( ! RS.isEmpty() ) {
    CS = BasicSet( QS );
    OLDCS=CS;
    DEBOUTLN(cout, "MCharSetN: CS= ", CS);
//     if ( getNumVars(CS.getFirst()) > 1 ){
//       //CS = removecontent(CS, Remembern);
// #ifdef MCHARSETNDEBUG
//       cout << "MCharSetN: CS= " << CS << endl;
// #endif
//     }
    Remembern.FS1 = Union(Remembern.FS1, initalset1(CS));
    DEBOUTLN(cout, "MCharSetN: Remembern.FS1= ", Remembern.FS1);
    DEBOUTLN(cout, "MCharSetN: Remembern.FS2= ", Remembern.FS2);
    RS = CFList();
    if ( rank( CS.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CS );
      DEBOUT(cout, "MCharSetN: Difference( ", QS);
      DEBOUT(cout, " , ", CS);
      DEBOUTLN(cout, " ) = ", D);
//cout << "MCharSetN: Difference( " << QS << " , " << CS << " ) = " << D << endl;
      //PremForm Oldremember=Remembern;
      //PremForm Newremember=Remembern;
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
	CanonicalForm r = Prem( i.getItem(), CS );
	DEBOUT(cout,"MCharSetN: Prem(", i.getItem()  );
	DEBOUT(cout, ",", CS);
	DEBOUTLN(cout,") = ", r); 
//cout << "MCharSetN: Prem("<< i.getItem() << "," << CS << ") = " << r << endl;
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
      DEBOUTLN(cout, "MCharSetN: RS= ", RS);
      //QS = Union( QS, RS );
      QS = Union(OLDCS,RS);
      DEBOUTLN(cout, "MCharSetN: QS= Union(QS,RS)= ", QS);
    }
    else{ return CFList(CanonicalForm(1)); }
  }
  DEBOUTLN(cout, "MCharSetN: Removed factors: ", Remembern.FS2);
  DEBOUTLN(cout, "MCharSetN: Remembern.FS1: ", Remembern.FS1);

  return CS;
}

CFList
CharSet( const CFList &PS ){
  CFList QS = PS, RS = PS, CS;

  while ( ! RS.isEmpty() ) {
    CS = BasicSet( QS );
    RS = CFList();
    if ( rank( CS.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CS );
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
	CanonicalForm r = Prem( i.getItem(), CS );
	if ( r != 0 )  RS=Union(CFList(r),RS);//RS.append( r );
      }
      QS = Union( QS, RS );
    }
  }
  return CS;
}

static CFList
charseta( const CFList & PS ){
  CFList QS = PS, RS = PS, CS;

  while ( ! RS.isEmpty() ) {
    CS = CharSet( QS );
    RS = CFList();
    if ( rank( CS.getFirst() ) != 0 ) {
      CFList D = Difference( QS, CS );
      for ( CFListIterator i = D; i.hasItem(); ++i ) {
	CanonicalForm r = Prem( i.getItem(), CS );
	if ( r != 0 )  RS=Union(CFList(r),RS);//RS.append( r );
      }
      QS = Union(CS,Union( QS, RS ));
    }
    else return CFList(CanonicalForm(1));
  }
  return CS;
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

#ifdef EXPERIMENTAL
static CFList 
getItemNr( int nr, const ListCFList & copy){
  int i =1;
  CFList elem;

  for ( ListCFListIterator j=copy; j.hasItem(); j++ )
    if ( i == nr ) { elem=j.getItem(); break; }
    else { i+= 1; }
  return elem;
}

static ListCFList
msort( const ListCFList & list_to_sort ){
  int nr, number = list_to_sort.length();
  ListCFList output;

  cout << "Sort: list to sort is: " <<  list_to_sort << endl; 
  for (int i=1; i<= number; i++){
    cout << " Next elem = "; cin >> nr;
    output.append(getItemNr(nr,list_to_sort));
  }
  return output;
}
#endif

#ifdef IRRCHARSERIESDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"

ListCFList
IrrCharSeries( const CFList &PS ){
  CanonicalForm reducible,reducible2;
  CFList qs,cs,factorset,is,ts;
  ListCFList pi,ppi,qqi,qsi,iss,qhi= ListCFList(PS);
  int nr_of_iteration=0,ts2,highestlevel=0;

  DEBOUTMSG(cout, rcsid);
  for ( CFListIterator Ps=PS; Ps.hasItem(); Ps++ )
    if ( level(Ps.getItem() ) > highestlevel ) highestlevel = level(Ps.getItem()) ;

  while ( ! qhi.isEmpty() ) {
    qhi=sort(qhi);
    DEBOUTLN(cout, "qhi is: ", qhi);
    qs=qhi.getFirst();
    DEBOUTLN(cout, "qs  is: ", qs);
    DEBOUTLN(cout, "ppi is: ", ppi);
    ListCFList ppi1,ppi2;
    select(ppi,qs.length(),ppi1,ppi2);
    DEBOUTLN(cout, "ppi1 is: ", ppi1);
    DEBOUTLN(cout, "ppi2 is: ", ppi2);
    qqi = MyUnion(ppi2,qqi);
    DEBOUTLN(cout, "qqi is: ", qqi);
    if ( nr_of_iteration == 0 ){ nr_of_iteration += 1; ppi = ListCFList(); }
    else{ nr_of_iteration += 1; ppi = MyUnion(ListCFList(qs),ppi1); }
    DEBOUTLN(cout,"ppi is: ", ppi);
    PremForm Remembern;
    cs = MCharSetN(qs,Remembern);
    DEBOUTLN(cout, "cs is: ", cs);
    DEBOUTLN(cout, "factorset is: ", Remembern.FS2);
    cs = removecontent(cs,Remembern);
    factorset=Remembern.FS2;
    DEBOUTLN(cout, "cs (after removecontent) is: ", cs);
    DEBOUTLN(cout, "factorset is: ", factorset);
    // Hier: removecontent einfuegen!!!!
    if ( cls(cs.getFirst()) > 0 ){
      ts = irras(cs,ts2, reducible);
      DEBOUTLN(cout, "ts is: ", ts);
      DEBOUTLN(cout, "ts2 is: ", ts2);
      // next is preliminary: should be ==0
      if ( ts2 <= 0 ){ //irreducible
	if ( ! subset(cs,qs) ){ 
	  DEBOUTMSG(cout, "cs is not a subset of qs");
	  cs = charseta(Union(qs,cs));
	  DEBOUTLN(cout, "new cs is: ", cs);
	}
	if ( ! member(cs,pi) ){ 
	  pi = MyUnion(pi, ListCFList(cs));
	  DEBOUTMSG(cout, "cs is not a member of pi");
	  DEBOUTLN(cout, "pi is: ", pi);
	  if ( cls(cs.getFirst()) > 0 ){
	    ts = irras(cs,ts2,reducible);
	    DEBOUTLN(cout, "ts is: ", ts);
	    DEBOUTLN(cout, "ts2 is: ", ts2);
	    // next is preliminary: should be ==0
	    if ( ts2 <= 0 ){ //irreducible
	      qsi = MyUnion(qsi,ListCFList(cs));
	      DEBOUTLN(cout, "qsi is: ", qsi);
	      if ( cs.length() == highestlevel ){
		DEBOUTLN(cout, "cs.length() == nops(ord) :", cs.length());
		is = factorps(factorset);
	      }
	      else{
		DEBOUT(cout,"cs.length() != nops(ord) :", cs.length());
		DEBOUTLN(cout, "  nops(ord)= ", highestlevel);
		is = Union(initalset1(cs),factorps(factorset));
	      }
	      DEBOUTLN(cout, "is is: ", is);
	      iss = adjoin(is,qs,qqi);
	      DEBOUTLN(cout, "iss is: ", iss);
	    }
	  }
	  else{ iss = adjoin(factorps(factorset),qs,qqi); }
	}
	else{ 
	  DEBOUTMSG(cout, "cs is a member of pi");
	  iss = adjoin(factorps(factorset),qs,qqi); }
	DEBOUTLN(cout, "iss is: ", iss);
	DEBOUTLN(cout, "   factorps(factorset)= ", factorps(factorset));
	DEBOUTLN(cout, "   qs= ", qs);
	DEBOUTLN(cout, "   qqi= ", qqi);
      }
      // next is preliminary: should be !=0
      if ( ts2 > 0 ){
	is = factorps(factorset);
	DEBOUTLN(cout, "is is: ", is);
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
	DEBOUTLN(cout, "iss is: ", iss);
      }
    }
    else{ 
      iss = adjoin(factorps(factorset),qs,qqi); 
      DEBOUTMSG(cout, "case: cs is a constant.");
      DEBOUTLN(cout, "  qs = ", qs);
      DEBOUTLN(cout, "  qqi = ", qqi);
      DEBOUTLN(cout, "  iss = adjoin(factorps(factorset),qs,qqi) = ",iss);
    }
    if ( qhi.length() > 1 ){ qhi.removeFirst(); qhi = MyUnion(qhi,iss); }
    else{ qhi = iss; }
    DEBOUTLN(cout, "iss is: ", iss);
  }
  if ( ! qsi.isEmpty() ){ 
    return contract( qsi ); 
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
irras( CFList & AS, int & ja, CanonicalForm & reducible){
  CFFList qs;
  CFList ts,as;
  CanonicalForm elem;
  int ind=1,nr=0;

  ja = 0;
  DEBOUTLN(cout, "irras: called with: AS= ", AS);
  for ( CFListIterator i=AS; i.hasItem(); i++ ){
    elem = i.getItem();
    nr += 1;
    DEBOUT(cout, "irras: factoring: ", elem);
    qs = Factorize(elem);
    qs.removeFirst();
    DEBOUTLN(cout, "  = ", qs);
    //    if ( num(qs.getFirst().factor() / LC(qs.getFirst().factor())) !=
    //         num(elem / LC(elem))){
    if ( (qs.length() >= 2 ) || (qs.getFirst().exp() > 1)){
      DEBOUTLN(cout, "irras: Setting ind=0, ja= ", nr);
      ja=nr; ind=0; reducible= elem; 
      // return(qs); // ist das nicht genauso gut?
      break;
    }
    else{ as.append(elem) ; }
  }
  AS=as; // warum das?
  if ( (ind == 1) && ( AS.length() > 1) ){
    if ( irreducible(AS) ) ja = 0;
    else {
#ifdef HAVE_SINGULAR
      extern void WerrorS(char *);
      WerrorS("libfac: Factorization over algebraic function field required!");
#else 
      cerr << "Factorization over algebraic function field required!" << endl;
#endif
      ja = -1;
    }
  }
  for ( CFFListIterator k=qs; k.hasItem();k++)
    ts.append(k.getItem().factor());
  return ts;
}
///////////////////////////////////////////////////////////////////////////////
/*
$Log: not supported by cvs2svn $
Revision 1.2  1997/04/25 22:52:28  michael
changed cerr and cout messages for use with Singular
Version for libfac-0.2.1

*/
