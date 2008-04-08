/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// This is really experimental. What is the best order of the variables for
// an ideal? Clearly, if a variable only occures in one and only one polynomial
// this variable should be assigned the highest level.
// But what for the others?
//
// The strategy used here is o.k. for *most* of the examples. But there are
// examples where this strategy doesn't give the best possible answer.
// So, use this at your own risk! The order given may increase or decrease
// computing time for your example!
/////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
/* $Id: reorder.cc,v 1.6 2008-04-08 16:19:10 Singular Exp $ */
////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <tmpl_inst.h>
#include "homogfactor.h"
// Charset - Includes

// some CC's need this:
#include "reorder.h"

#ifdef REORDERDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif

#include "debug.h"
#include "timing.h"
TIMING_DEFINE_PRINT(neworder_time);

#define __ARRAY_INIT__ -1

// the maximal degree of polys in PS wrt. variable x
static int
degpsmax( const CFList & PS, const Variable & x, 
	  Intarray & A, Intarray & C){
  int varlevel = level(x);
  if ( A[varlevel] != __ARRAY_INIT__ ) return A[varlevel];
  int max=0,temp, count=0;
  
  for ( CFListIterator i=PS; i.hasItem();i++){
    temp = degree(i.getItem(),x);
    if ( temp > max ) {max = temp; count = 0; }
    if ( temp == max) { count += max; } // we count the number of polys
  }
  A[varlevel] = max; C[varlevel] = count;
  return max;
}

// the minimal non-zero degree of polys in PS wrt. x
// returns 0 if variable x doesn't occure in any of the polys
static int
degpsmin( const CFList & PS, const Variable & x, Intarray & A, Intarray & B,
	  Intarray & C, Intarray & D){
  int varlevel = level(x);
  if ( B[varlevel] != __ARRAY_INIT__ ) return B[varlevel];
  int min=degpsmax(PS,x,A,C), temp, count=0;

  if (min==0) {
    B[varlevel] = min; D[varlevel] = min;
    return min;
  }
  else
    for ( CFListIterator i=PS; i.hasItem();i++){
      temp = degree(i.getItem(),x);
      if ( temp < min && temp != 0 ){ min = temp; count = 0; }
      if ( temp == min) { count += min;} // we count the number of polys
    }
  B[varlevel] = min; D[varlevel] = count;
  return min;
}

// the minimal total degree of lcoeffs of polys in PS wrt. x
// for those polys having degree degpsmin in x.
// F will be assigned the minimal number of terms of those lcoeffs
static int
Tdeg( const CFList & PS, const Variable & x, Intarray & A, Intarray & B,
      Intarray & C, Intarray & D, Intarray & E, Intarray & F){
  int k= degpsmin(PS,x,A,B,C,D), 
    varlevel= level(x), min=0;

  if ( E[varlevel] != __ARRAY_INIT__ ) return E[varlevel];
  if (k == 0){
    E[varlevel]= 0; F[varlevel]= 0;
  }
  else{
    int nopslc=0;
    CFList LCdegList;
    CanonicalForm elem;
    CFListIterator i;

    for ( i=PS; i.hasItem(); i++ ){
      elem = i.getItem();
      if ( degree(elem,x) == k ) LCdegList.append(LC(elem,x));
    }
    
    if ( LCdegList.length() > 0 ){
      CFList TermList;
      int newmin, newnopslc;

      min= totaldegree(LCdegList.getFirst());
      TermList= get_Terms(LCdegList.getFirst()); 
      nopslc= TermList.length();
      for ( i=LCdegList; i.hasItem(); i++){
	elem= i.getItem();
	newmin= totaldegree(elem);
	TermList= get_Terms(elem);
	newnopslc= TermList.length();
	if ( newmin < min ) min= newmin; 
	if ( newnopslc < nopslc ) nopslc= newnopslc;
      }
      
    }
    E[varlevel]= min;
    F[varlevel]= nopslc;
  }
  return min;
}

// The number of the poly in which Variable x first occures
static int
nr_of_poly( const CFList & PS, const Variable & x, Intarray & G){
  int min=0, varlevel=level(x);
  if ( G[varlevel] != __ARRAY_INIT__ ) return G[varlevel];
  for ( CFListIterator i=PS; i.hasItem(); i++ ){
    min += 1;
    if ( degree(i.getItem(),x) > 0 ) break;
  }
  G[varlevel]= min;
  return min;
}

static int
degord( const Variable & x, const Variable & y, const CFList & PS,
	Intarray & A, Intarray & B, Intarray & C, Intarray & D,
	Intarray & E, Intarray & F, Intarray & G){
  int xlevel= level(x), ylevel= level(y);

  if      (degpsmax(PS,y,A,C) < degpsmax(PS,x,A,C))         return 1;
  else if (degpsmax(PS,x,A,C) < degpsmax(PS,y,A,C) )        return 0;
  else if (C[ylevel] < C[xlevel])                           return 1;
  else if (C[xlevel] < C[ylevel])                           return 0;
  else if (degpsmin(PS,x,A,B,C,D) < degpsmin(PS,y,A,B,C,D)) return 1;
  else if (degpsmin(PS,y,A,B,C,D) < degpsmin(PS,x,A,B,C,D)) return 0;
  else if (D[ylevel] < D[xlevel])                           return 1;
  else if (D[xlevel] < D[ylevel])                           return 0;
  else if (Tdeg(PS,y,A,B,C,D,E,F) < Tdeg(PS,x,A,B,C,D,E,F)) return 1;
  else if (Tdeg(PS,x,A,B,C,D,E,F) < Tdeg(PS,y,A,B,C,D,E,F)) return 0;
  else if (F[ylevel] < F[xlevel])                           return 1;
  else if (F[xlevel] < F[ylevel])                           return 0;
  else if (nr_of_poly(PS,x,G) <= nr_of_poly(PS,y,G))        return 1;
  else return 0;

}

// determine the highest variable of all involved Variables in PS
// NOTE:
//    this doesn't give always the correct answer:
//    If a variable is assigned the highest level in the definition of the
//    original ring, but doesn't occure in any of the 
//    polynomials, get_max_var returns the variable with a level lower than 
//    the highest level.
//    Is there a workaround?
// But for the redefinition of the ring this doesn't matter due to the
// implementation of neworder().

static Variable
get_max_var(const CFList & PS){
  Variable x=PS.getFirst().mvar(), y;
  for (CFListIterator i=PS; i.hasItem(); i++){
    y = i.getItem().mvar();
    if ( y > x ) x=y;
  }
  return x;
}


// determine if variable x is in one and only one of the polynomials in PS
// first criterion for neworder
static CFList
only_in_one( const CFList & PS , const Variable & x){
  CFList output;

  for ( CFListIterator i=PS; i.hasItem(); i++ ){
    if ( degree(i.getItem(),x) >= 1 ) output.insert(i.getItem());
    if ( output.length() >= 2 ) break;
  }
  return output;
}

// initialize all Arrays (of same range) with __ARRAY_INIT__
static void
initArray(const int highest_level, Intarray & A, Intarray & B, Intarray & C, 
	  Intarray & D, Intarray & E, Intarray & F, Intarray & G){ 

  for ( int i=1 ; i <= highest_level; i ++){
    A[i] = __ARRAY_INIT__; B[i] = __ARRAY_INIT__; 
    C[i] = __ARRAY_INIT__; D[i] = __ARRAY_INIT__;
    E[i] = __ARRAY_INIT__; F[i] = __ARRAY_INIT__;
    G[i] = __ARRAY_INIT__;
  }
}

// now for the second criterion; a little more complex
//
// idea: set up seven arrays of lenth highest_level 
//       (of which some entries may be empty, because of only_in_one!)
//   A saves maxdegree of Variable x in A(level(x))
//   B saves mindegree of Variable x in B(level(x))
//   C saves the number of polys in PS which have degree A(level(x)) in 
//     D(level(x))
//   D saves the number of polys in PS which have degree B(level(x)) in 
//     D(level(x))
//   E saves the minimal total degree of lcoeffs of polys wrt x in E(level(x))
//   F saves the minimal number of terms of lcoeffs of E(level(x)) in F(~)
//   G saves nr of poly Variable x has first deg <> 0

#define __INIT_GAP__ 3
static Varlist
reorderb(const Varlist & difference, const CFList & PS, 
	 const int highest_level){
  Intarray A(1, highest_level), B(1, highest_level), C(1, highest_level), 
    D(1, highest_level), E(1, highest_level), F(1, highest_level),
    G(1, highest_level);
  initArray(highest_level,A,B,C,D,E,F,G);
  int i=0, j, n=difference.length(), gap=1+__INIT_GAP__;
  Variable temp;
  Array<Variable> v(0,n);
  VarlistIterator J;

  for (J= difference; J.hasItem(); J++ ){ 
    v[i]= J.getItem(); 
    i++ ; 
  }
  
  while ( gap <= n) gap = __INIT_GAP__ * gap+1;
  gap /= __INIT_GAP__;
  DEBOUTLN(CERR, "gap is ", gap);
  
  while ( gap > 0 ){
    for ( i=gap; i<= n-1; i++){
      temp = v[i];
      for ( j= i-gap; j >=0 ; j-=gap){
	if (degord(v[j],temp, PS, A,B,C,D,E,F,G))  break;
	v[j+gap] = v[j];
	//CERR << "v[" << j+gap << "]= " << v[j+gap] << "\n";
      }
      v[j+gap] = temp;
      //CERR << "v[" << j+gap << "]= " << v[j+gap] << "\n";
    }
    gap /= __INIT_GAP__;
  }
  Varlist output;
  for (i=0; i<= n-1; i++)
    output.append(v[i]);
  DEBOUTLN(CERR, "A= ", A);  DEBOUTLN(CERR, "B= ", B);  
  DEBOUTLN(CERR, "C= ", C);  DEBOUTLN(CERR, "D= ", D);
  DEBOUTLN(CERR, "E= ", E);  DEBOUTLN(CERR, "F= ", F);
  DEBOUTLN(CERR, "G= ", G);
  return output;
}

// set up a new orderd list of Variables.
// we try to reorder the variables heuristically optimal.
Varlist
neworder( const CFList & PolyList ){
  CFList PS= PolyList, PS1=PolyList;
  Varlist oldorder, reorder, difference;

  DEBINCLEVEL(CERR, "neworder");
  TIMING_START(neworder_time);

  int highest_level= level(get_max_var(PS));
  DEBOUTLN(CERR, "neworder: highest_level=   ", highest_level);
  DEBOUTLN(CERR, "neworder: that is variable ", Variable(highest_level));

  // set up oldorder and first criterion: only_in_one
  for (int i=highest_level; i>=1; i--){
    oldorder.insert(Variable(i));
    CFList is_one= only_in_one(PS1, Variable(i)); 
    if ( is_one.length() == 1 ){
      DEBOUTLN(CERR, "Found a variable which is in only one Polynomial: ", 
	       Variable(i));
      DEBOUTLN(CERR, ".... in the Polynomial: ", is_one.getFirst());
      reorder.insert(Variable(i));
      PS1 = Difference(PS1, is_one);
      DEBOUTLN(CERR, "New cutted list is: ", PS1);
    }
    else if ( is_one.length() == 0 ){
      DEBOUTLN(CERR, "Found a variable which is in no polynomial: ", 
	       Variable(i));
      DEBOUTMSG(CERR, "... assigning it the highest level.");
      reorder.append(Variable(i)); // assigne it the highest level
      PS1 = Difference(PS1, is_one); 
    }
  }
  difference = Difference(oldorder,reorder);
  DEBOUTLN(CERR, "Missing variables are: ", difference);
  // rearrange the ordering of the variables!
  difference = reorderb(difference, PS, highest_level);
  DEBOUTLN(CERR, "second criterion gives: ", difference);
  reorder = Union(reorder, difference);
  DEBOUTLN(CERR, "old order was:     ", oldorder);
  DEBOUTLN(CERR, "New order will be: ", Union(reorder, Difference(oldorder,reorder)));
  DEBDECLEVEL(CERR, "neworder");
  TIMING_END(neworder_time);

  TIMING_PRINT(neworder_time, "\ntime used for neworder   : ");


  return Union(reorder,Difference(oldorder,reorder));
}

// the same as above, only returning a list of CanonicalForms
CFList
newordercf(const CFList & PolyList ){
  Varlist reorder= neworder(PolyList);
  CFList output;

  for (VarlistIterator i=reorder; i.hasItem(); i++)
    output.append(CanonicalForm(i.getItem()));

  return output;
}

// the same as above, only returning a list of ints
IntList
neworderint(const CFList & PolyList ){
  Varlist reorder= neworder(PolyList);
  IntList output;

  for (VarlistIterator i=reorder; i.hasItem(); i++)
    output.append(level(i.getItem()));

  return output;
}

// swapvar a whole list of CanonicalForms
static CFList
swapvar( const CFList & PS, const Variable & x, const Variable & y){
  CFList ps;

  for (CFListIterator i= PS; i.hasItem(); i++)
    ps.append(swapvar(i.getItem(),x,y));
  return ps;
}

static CFFList
swapvar( const CFFList & PS, const Variable & x, const Variable & y){
  CFFList ps;

  for (CFFListIterator i= PS; i.hasItem(); i++)
    ps.append(CFFactor(swapvar(i.getItem().factor(),x,y),i.getItem().exp()));
  return ps;
}

// a library function: we reorganize the global variable ordering
CFList
reorder( const Varlist & betterorder, const CFList & PS){
  int i=1, n = betterorder.length();
  Intarray v(1,n);
  CFList ps=PS;

  //initalize:
  for (VarlistIterator j = betterorder; j.hasItem(); j++){
    v[i]= level(j.getItem()); i++;
  }
  DEBOUTLN(CERR, "reorder: Original ps=  ", ps);
  // reorder:
  for (i=1; i <= n; i++)
    ps=swapvar(ps,Variable(v[i]),Variable(n+i));
  DEBOUTLN(CERR, "reorder: Reorganized ps= ", ps); 
  return ps;
}

CFFList
reorder( const Varlist & betterorder, const CFFList & PS){
  int i=1, n = betterorder.length();
  Intarray v(1,n);
  CFFList ps=PS;

  //initalize:
  for (VarlistIterator j = betterorder; j.hasItem(); j++){
    v[i]= level(j.getItem()); i++;
  }
  DEBOUTLN(CERR, "reorder: Original ps=  ", ps);
  // reorder:
  for (i=1; i <= n; i++)
    ps=swapvar(ps,Variable(v[i]),Variable(n+i));
  DEBOUTLN(CERR, "reorder: Reorganized ps= ", ps); 
  return ps;
}

ListCFList
reorder(const Varlist & betterorder, const ListCFList & Q){
  ListCFList Q1;

  for ( ListCFListIterator i=Q; i.hasItem(); i++)
    Q1.append(reorder(betterorder, i.getItem()));
  return Q1;
}
/*
$Log: not supported by cvs2svn $
Revision 1.5  2008/03/18 17:46:15  Singular
*hannes: gcc 4.2

Revision 1.4  2006/05/16 14:46:49  Singular
*hannes: gcc 4.1 fixes

Revision 1.3  1997/09/12 07:19:44  Singular
* hannes/michael: libfac-0.3.0

Revision 1.2  1997/04/25 22:53:25  michael
changed cerr and cout messages for use with Singular
Version for libfac-0.2.1

Revision 1.1  1996/12/19 03:46:05  michael
Initial revision

*/   

