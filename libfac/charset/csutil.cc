/* Copyright 1996 Michael Messollen. All rights reserved. */
////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
static char * rcsid = "$Id: csutil.cc,v 1.12 2006-05-16 14:46:49 Singular Exp $";
/////////////////////////////////////////////////////////////
// FACTORY - Includes
#include <factory.h>
// Factor - Includes
#include <tmpl_inst.h>
#include <Factor.h>
#include <SqrFree.h>
#include <helpstuff.h>
#include <homogfactor.h>
// Charset - Includes
#include "csutil.h"
extern void out_cf(char *s1,const CanonicalForm &f,char *s2);
extern CanonicalForm alg_lc(const CanonicalForm &f);
extern int hasAlgVar(const CanonicalForm &f);

static bool
lowerRank ( const CanonicalForm & f, const CanonicalForm & g, int & ind )
{
  int df, dg;
  Variable vf = f.mvar(), vg = g.mvar();

  if ( f.inCoeffDomain() ) {
    if ( g.inCoeffDomain() ) ind= 1;
    return true;//( vg > vf );
  }
  else if ( g.inCoeffDomain() ) return false;
  else if ( vf < vg ) return true;
  else if ( vf == vg ) {
    df = degree( f ); dg = degree( g );
    if ( df < dg ) return true;
    else if ( df == dg ) return lowerRank( LC( f ), LC( g ) , ind);
    else return false;
  }
  return false;
}

CanonicalForm
lowestRank( const CFList & F )
{
  CFListIterator i = F;
  CanonicalForm f;
  int ind=0;
  if ( ! i.hasItem() )        return f;
  f = i.getItem(); ++i;
  while ( i.hasItem() ) {
    //CERR << "comparing " << f << "  and " << i.getItem()
    // << " == " << lowerRank( i.getItem(), f, ind ) << "\n";
    if ( lowerRank( i.getItem(), f, ind ) ) {
      if ( ind ){
        CFList Itemlist= get_Terms(i.getItem());
        CFList Flist= get_Terms(f);

        // Have to further compare number of terms!
        //CERR << "compare terms! f= " << Flist.length() << "  item= "
        //     << Itemlist.length() <<"\n";
        if ( Itemlist.length() < Flist.length()) f = i.getItem();
        ind=0;
      }
      else{
        f = i.getItem();
      }
    }
    ++i;
  }
  return f;
}

// old version
// CanonicalForm
// prem ( const CanonicalForm &f, const CanonicalForm &g )
// {
//   CanonicalForm ff, gg, cg;
//   int df, dg;
//   bool reord;
//   Variable vf, vg, v;
//
//   if ( (vf = f.mvar()) < (vg = g.mvar()) ) return f;
//   else {
//     if ( vf == vg ) {
//       ff = f; gg = g;
//       reord = false;
//       v = vg;
//     }
//     else {
//       v = Variable(level(f.mvar()) + 1);
//       ff = swapvar(f,vg,v);
//       gg = swapvar(g,vg,v);
//       reord=true;
//     }
//     cg = ini( gg, v );
//     dg = degree( gg, v );
//     while ( ( df = degree( ff, v ) ) >= dg )
//       ff = cg * ff - power( v, df-dg ) * gg * LC( ff, v );
//     if ( reord ) {
//       return swapvar( ff, vg, v );
//     }
//     else
//       return ff;
//   }
// }

CanonicalForm
Prem ( const CanonicalForm &f, const CanonicalForm &g ){
  CanonicalForm ff, gg, l, test, lu, lv, t, retvalue;
  int df, dg;
  bool reord;
  Variable vf, vg, v;

  if ( (vf = f.mvar()) < (vg = g.mvar()) ) return f;
  else {
    if ( vf == vg ) {
      ff = f; gg = g;
      reord = false;
      v = vg;
    }
    else {
      v = Variable(level(f.mvar()) + 1);
      ff = swapvar(f,vg,v);
      gg = swapvar(g,vg,v);
      reord=true;
    }
    dg = degree( gg, v );
    df = degree( ff, v );
    if (dg <= df) {l=LC(gg); gg = gg -LC(gg)*power(v,dg);}
    else { l = 1; }
    while ( ( dg <= df  ) && ( ff != ff.genZero()) ){
      // CERR << "Start gcd..." << "\n";
      test = gcd(l,LC(ff));
      //CERR << "gcd(" << l << "," << LC(ff) << ")= " << test << "\n";
      lu = l/test; lv = LC(ff)/test;
      t = power(v,df-dg) * gg * lv;
      if ( df == 0 ){ ff = ff.genZero(); }
      else { ff = ff - LC(ff)*power(v,df); }
      ff = lu*ff - t;
      df = degree( ff, v );
    }
    if ( reord ) {
     retvalue= swapvar( ff, vg, v );
    }
    else {
     retvalue= ff;
    }
    return retvalue;
  }
}

static CanonicalForm
Sprem ( const CanonicalForm &f, const CanonicalForm &g, CanonicalForm & m, CanonicalForm & q ){
  CanonicalForm ff, gg, l, test, retvalue;
  int df, dg,n;
  bool reord;
  Variable vf, vg, v;

  if ( (vf = f.mvar()) < (vg = g.mvar()) ) {
    m=CanonicalForm(0); q=CanonicalForm(0);
    return f;
  }
  else {
    if ( vf == vg ) {
      ff = f; gg = g;
      reord = false;
      v = vg; // == x
    }
    else {
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
    while ( ( dg <= df  ) && ( ff != ff.genZero()) ){
      test= power(v,df-dg) * gg * LC(ff);
      if ( df == 0 ){ff= ff.genZero();}
      else {ff= ff - LC(ff)*power(v,df);}
      ff = l*ff-test;
      df= degree(ff,v);
      n++;
    }
    if ( reord ) {
     retvalue= swapvar( ff, vg, v );
    }
    else {
     retvalue= ff;
    }
    m= power(l,n);
    if ( fdivides(g,m*f-retvalue) )
      q= (m*f-retvalue)/g;
    else {
      q= CanonicalForm(0);
    }
    return retvalue;
  }
}

CanonicalForm
divide( const CanonicalForm & ff, const CanonicalForm & f, const CFList & as){
  CanonicalForm r,m,q;

  //out_cf("divide f=",ff,"\n");
  //out_cf("divide g=",f,"\n");
  if (f.inCoeffDomain())
  {
    bool b=false;	  
    if (!isOn(SW_RATIONAL)) { b=true;On(SW_RATIONAL); }
    q=ff/f;
    if (b) Off(SW_RATIONAL);
  }  
  else
    r= Sprem(ff,f,m,q); //result in q, ignore r,m
  //CERR << "r= " << r << "  , m= " << m << "  , q= " << q << "\n";
  r= Prem(q,as);
  //CERR << "r= " << r << "\n";
  //out_cf(" ->",r,"\n");
  return r;
}

static CanonicalForm
myfitting( const CanonicalForm &f ){
 CanonicalForm rem=f;

 if ( !(rem.isZero()) ){
   if ( getCharacteristic() > 0 )
     return num((rem/lc(rem)));
   else{
     On(SW_RATIONAL);
     CanonicalForm temp= mapinto(rem);
//      CERR << "temp= " << temp << "\n";
//      CERR << "lc(temp)= " << lc(temp) << "\n";
//      CERR << "temp/lc(temp)= " << temp/lc(temp) << "\n";
//      CERR << "num(rem/lc(rem))= " << num(rem/lc(rem)) << "\n";
     temp= bCommonDen(temp/lc(temp))*(temp/lc(temp));
     Off(SW_RATIONAL);
     rem= mapinto(temp);
     return rem;
   }
 }
 else
   return rem;
}

CanonicalForm
Prem( const CanonicalForm &f, const CFList &L ){
  CanonicalForm rem = f;
  CFListIterator i = L;
  for ( i.lastItem(); i.hasItem(); i-- ){
//CERR << "   PREM: Prem(" << rem << "," ;
    rem = Prem( rem, i.getItem() );
//CERR << "   PREM: Prem(" << rem << "," << i.getItem() << ")  = " << rem << "\n";
  }
  return myfitting(rem);
}

CFList
Prem( const CFList &AS, const CFList &L ){
  CFList Output;

  for ( CFListIterator i=AS; i.hasItem(); i++ )
    Output = Union(CFList(Prem(i.getItem(),L)), Output);

  return Output;
}

static CanonicalForm
premasb( const CanonicalForm & f, const CFList & as){
  CanonicalForm remd=f;
  CFList AS=as;

  if ( as.length() > 1 ){
    AS.removeFirst(); // get rid of first elem
    CanonicalForm elem;
    while ( ! AS.isEmpty() ){ // thats true for at least the first iteration
      elem= AS.getLast();
      remd= Prem(remd,elem);
      AS.removeLast();
    }
  }
  CanonicalForm a,b;
  if ( mydivremt(remd, as.getFirst(), a,b )){ remd= remd.genZero();}
  else { remd= Prem(remd, as.getFirst()); }

  return remd;
}

CFList
remsetb( const CFList & ps, const CFList & as){
  CFList output;
  CanonicalForm elem;
  for (CFListIterator i=ps; i.hasItem(); i++){
    elem= premasb(i.getItem(),as);
    if ( elem != elem.genZero() ) output.append(elem);
  }
  return output;
}

// for characteristic sets
//////////////////////////////////
// replace the power of factors of polys in as by 1 if any
static CFList
nopower( const CanonicalForm & init ){
  CFFList sqrfreelist;// = Factorize(init);//SqrFree(init);
  CFList output;
  CanonicalForm elem;
  int count=0;

  for ( CFIterator j=init; j.hasTerms(); j++ )
    if (!(j.coeff().isOne()) ) count += 1;
  //  if ( init != 1 ){
  //  CERR << "nopower: f is " << init << "\n";
  //  CERR << "nopower: count is " << count << "\n";}
  if ( count > 1 ) sqrfreelist = CFFList( CFFactor(init,1));
  else {
    sqrfreelist = Factorize(init);
    //sqrfreelist.removeFirst();
  }
  for ( CFFListIterator i=sqrfreelist; i.hasItem(); i++ ){
    elem=i.getItem().factor();
    if ( cls(elem) > 0 ) output.append(elem);
  }
  return output;
}

// remove the content of polys in PS; add the removed content to
// Remembern.FS2 ( the set of removed factors )
CFList
removecontent ( const CFList & PS, PremForm & Remembern )
{
  CFListIterator i=PS;
  if ((!i.hasItem()) || ( cls(PS.getFirst()) == 0 )) return PS;

  CFList output;
  CanonicalForm cc,elem;

  for (; i.hasItem(); i++)
  {
    elem = i.getItem();
    cc = content(elem, elem.mvar());
    if ( cls(cc) > 0 )
    {
      output.append(elem/cc);
      Remembern.FS2 = Union(CFList(cc), Remembern.FS2);
    }
    else{ output.append(elem); }
  }
  return output;
}

// remove possible factors in Remember.FS1 from poly r
// Remember.FS2 contains all factors removed before
void
removefactor( CanonicalForm & r , PremForm & Remembern){
  int test;
  CanonicalForm a,b,testelem;
  CFList testlist;
  int n=level(r);
  CFListIterator j ;

  for ( int J=1; J<= n ; J++ ){
    testlist.append(CanonicalForm(Variable(J)));
  }

  //  testlist = Union(Remembern.FS1, testlist); // add candidates

  // remove already removed factors
  for ( j = Remembern.FS2 ; j.hasItem(); j++ ){
    testelem = j.getItem();
    while ( 1 ){
      test = mydivremt(r,testelem,a,b);
      if ( test && b == r.genZero() ) r = a;
      else break;
    }
  }

  // Let's look if we have other canditates to remove
  for ( j = testlist ; j.hasItem(); j++ ){
    testelem = j.getItem();
//    if ( testelem != r && testelem != r.mvar() ){
    if ( testelem != r ){
      while ( 1 ){
        test = divremt(r,testelem,a,b);
        if ( test && b == r.genZero() ){
          Remembern.FS2= Union(Remembern.FS2, CFList(testelem));
          r = a;
          if ( r == 1 ) break;
        }
        else break;
      }
    }
  }
  //  CERR << "Remembern.FS1 = " << Remembern.FS1 << "\n";
  //  CERR << "Remembern.FS2 = " << Remembern.FS2 << "\n";
  //  Remembern.FS1 = Difference(Remembern.FS1, Remembern.FS2);
  //  CERR << "  New Remembern.FS1 = " << Remembern.FS1 << "\n";
}


// all irreducible nonconstant factors of a set of polynomials
CFList
factorps( const CFList &ps ){
  CFList qs;
  CFFList q;
  CanonicalForm elem;

  for ( CFListIterator i=ps; i. hasItem(); i++ ){
    q=Factorize(i.getItem());
    q.removeFirst();
    // Next can be simplified ( first (already removed) elem in q is the only constant
    for ( CFFListIterator j=q; j.hasItem(); j++ ){
      elem = j.getItem().factor();
      if ( getNumVars(elem) > 0 )
        qs= Union(qs, CFList(myfitting(elem)));
    }
  }
  return qs;
}

// the initial of poly f wrt to the order of the variables
static CanonicalForm
inital( const CanonicalForm &f ){
  CanonicalForm leadcoeff;

  if ( cls(f) == 0 ) {return f.genOne(); }
  else {
    leadcoeff = LC(f,lvar(f));
    //    if ( leadcoeff != 0 )
    return myfitting(leadcoeff); //num(leadcoeff/lc(leadcoeff));
    //    else return leadcoeff;
  }
}

// the set of all nonconstant factors of initals of polys in as
// CFList
// initalset(const CFList &as){
//   CanonicalForm elem;
//   CFList is, iss,iniset;

//   for ( CFListIterator i=as ; i.hasItem(); i++ ){
//     elem = inital(i.getItem());
//     if ( cls(elem) > 0 ) is.append(elem);
//   }
//   iss = factorps(is);
//   for ( CFListIterator j=iss; j.hasItem();j++ ){
//     elem = j.getItem();
//     if ( cls(elem) > 0 ) iniset.append(num(elem/lc(elem)));
//   }
//   return iniset;
// }

// the set of nonconstant initials of CS
// with certain repeated factors cancelled
CFList
initalset1(const CFList & CS){
  CFList temp;
  CFList initals;
  CanonicalForm init;

  for ( CFListIterator i = CS ; i.hasItem(); i++ ){
    initals= nopower( inital(i.getItem()) );
    //    init= inital(i.getItem());
    for ( CFListIterator j = initals; j.hasItem(); j++){
      init = j.getItem();
      if ( cls(init) > 0 )
        temp= Union(temp, CFList(init));
    }
  }
  return temp;
}

// the set of nonconstant initials of CS of those polys
// not having their cls higher than reducible
// with certain repeated factors cancelled
CFList
initalset2(const CFList & CS, const CanonicalForm & reducible){
  CFList temp;
  CFList initals;
  CanonicalForm init;
  int clsred = cls(reducible);

  for ( CFListIterator i = CS ; i.hasItem(); i++ ){
    init = i.getItem();
    if ( cls(init) < clsred ){
      initals= nopower( inital(init) );
      //    init= inital(i.getItem());
      for ( CFListIterator j = initals; j.hasItem(); j++){
        init = j.getItem();
        if ( cls(init) > 0 )
          temp= Union(temp, CFList(init));
      }
    }
  }
  return temp;
}

//replace the power of factors of poly in CF init by 1 if any
// and return the sqrfree poly
// static CanonicalForm
// nopower1( const CanonicalForm & init ){
//   CFFList returnlist=Factorize(init);
//   CanonicalForm elem, test=init.genOne();
//   for ( CFFListIterator i= returnlist; i.hasItem(); i++){
//     elem = i.getItem().factor();
//     if ( cls(elem)>0 ) test *= elem;
//   }
//   return test;
// }

// the sequence of distinct factors of f
//CF pfactor( ..... )

// //////////////////////////////////////////
// // for IrrCharSeries

#ifdef IRRCHARSERIESDEBUG
#  define DEBUGOUTPUT
#else
#  undef DEBUGOUTPUT
#endif
#include "debug.h"
// examine the irreducibility of as for IrrCharSeries
int
irreducible( const CFList & AS){
// AS is given by AS = { A1, A2, .. Ar }, d_i = degree(Ai)

  DEBOUTMSG(CERR, rcsid);
// 1) we test: if d_i > 1, d_j =1 for all j<>i, then AS is irreducible.
  bool deg1=1;
  for ( CFListIterator i = AS ; i.hasItem(); i++ ){
    if ( degree(i.getItem()) > 1 ){
      if ( deg1 ) deg1=0;
      else return 0; // found 2nd poly with deg > 1
    }
  }
  return 1;
}


// select an item from PS for irras
CFList
select( const ListCFList & PS){

  return PS.getFirst();
}

// divide list ppi in elems having length <= and > length
void
select( const ListCFList & ppi, int length, ListCFList & ppi1, ListCFList & ppi2){
  CFList elem;
  for ( ListCFListIterator i=ppi ; i.hasItem(); i++ ){
    elem = i.getItem();
    if ( ! elem.isEmpty() )
      if ( length <= elem.length() ){ ppi2.append(elem); }
      else { ppi1.append(elem); }
  }
}


//////////////////////////////////////////////////////////////
// help-functions for sets

// is f in F ?
static bool
member( const CanonicalForm &f, const CFList &F){

  for ( CFListIterator i=F; i.hasItem(); i++ )
    if ( i.getItem() == f ) return 1;
  return 0;
}

// are list A and B the same?
bool
same( const CFList &A, const CFList &B ){
  CFListIterator i;

  for (i = A; i.hasItem(); i++)
    if (! member(i.getItem(), B) )  return 0;
  for (i = B; i.hasItem(); i++)
    if (! member(i.getItem(), A) )  return 0;
  return 1;
}


// is List cs contained in List of lists pi?
bool
member( const CFList & cs, const ListCFList & pi ){
  ListCFListIterator i;
  CFList elem;

  for ( i=pi; i.hasItem(); i++){
    elem = i.getItem();
    if ( same(cs,elem) ) return 1;
  }
  return 0;
}

// is PS a subset of CS ?
bool
subset( const CFList &PS, const CFList &CS ){

  //  CERR << "subset: called with: " << PS << "   " << CS << "\n";
  for ( CFListIterator i=PS; i.hasItem(); i++ )
    if ( ! member(i.getItem(), CS) ) {
      //      CERR << "subset: " << i.getItem() << "  is not a member of " << CS << "\n";
      return 0;
    }
  return 1;
}

// Union of two List of Lists
ListCFList
MyUnion( const ListCFList & a, const ListCFList &b ){
  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for ( i = a ; i.hasItem(); i++ ){
    elem=i.getItem();
    // if ( ! member(elem,output) ){
    if ( (! elem.isEmpty()) && ( ! member(elem,output)) ){
      output.append(elem);
    }
  }

  for ( i = b ; i.hasItem(); i++ ){
    elem=i.getItem();
    // if ( ! member(elem,output) ){
    if ( (! elem.isEmpty()) && ( ! member(elem,output)) ){
      output.append(elem);
    }
  }
  return output;
}

//if list b is member of the list of lists remove b and return the rest
ListCFList
MyDifference( const ListCFList & a, const CFList &b){
  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for ( i = a ; i.hasItem(); i++ ){
    elem=i.getItem();
    if ( (! elem.isEmpty()) && ( ! same(elem,b)) ){
      output.append(elem);
    }
  }
return output;
}

// remove all elements of b from list of lists a and return the rest
ListCFList
Minus( const ListCFList & a, const ListCFList & b){
  ListCFList output=a;

  for ( ListCFListIterator i=b; i.hasItem(); i++ )
    output = MyDifference(output, i.getItem() );

  return output;
}

#if 0
static CanonicalForm alg_lc(const CanonicalForm &f, const CFList as)
{
  for(CFListIterator i=as; i.hasItem(); i++)
  {
    if (f.mvar()==i.getItem().mvar()) return f;
  }
  if (f.level()>0)
  {
    return alg_lc(f.LC(),as);
  }
  return CanonicalForm(1);
}
#endif

CanonicalForm alg_gcd(const CanonicalForm & fff, const CanonicalForm &ggg,
                      const CFList &as)
{
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
  //out_cf("alg_gcd(",fff," , ");
  //out_cf("",ggg,")\n");
  CanonicalForm res;
  // does as appear in f and g ?
  bool has_alg_var=false;
  for ( CFListIterator j=as;j.hasItem(); j++ )
  {
    Variable v=j.getItem().mvar();
    if (hasVar(f,v)) {has_alg_var=true; /*break;*/}
    if (hasVar(g,v)) {has_alg_var=true; /*break;*/}
    //out_cf("as:",j.getItem(),"\n");
  }
  if (!has_alg_var)
  {
    if ((hasAlgVar(f))
    || (hasAlgVar(g)))
    {
      Varlist ord;
      for ( CFListIterator j=as;j.hasItem(); j++ )
        ord.append(j.getItem().mvar());
      res=algcd(f,g,as,ord);
    }
    else
      res=gcd(f,g);
    //out_cf("gcd=",res,"\n");
    //out_cf("of f=",fff," , ");
    //out_cf("and g=",ggg,"\n");

    return res;
  }

  int mvf=f.level();
  int mvg=g.level();
  if (mvg > mvf)
  {
    CanonicalForm tmp=f; f=g; g=tmp;
    int tmp2=mvf; mvf=mvg; mvg=tmp2;
  }
  if (g.inBaseDomain() || f.inBaseDomain())
  {
    //printf("const\n");
    //out_cf("of f=",fff," , ");
    //out_cf("and g=",ggg,"\n");
    return CanonicalForm(1);
  }

  // gcd of all coefficients:
  CFIterator i=f;
  CanonicalForm c_gcd=i.coeff(); i++;
  while( i.hasTerms())
  {
    c_gcd=alg_gcd(i.coeff(),c_gcd,as);
    if (c_gcd.inBaseDomain()) break;
    i++;
  }
  //printf("f.mvar=%d (%d), g.mvar=%d (%d)\n",f.level(),mvf,g.level(),mvg);
  if (mvf!=mvg) // => mvf > mvg
  {
    res=alg_gcd(g,c_gcd,as);
    //out_cf("alg_gcd1=",res,"\n");
    //out_cf("of f=",fff," , ");
    //out_cf("and g=",ggg,"\n");
    return res;
  }
  // now: mvf==mvg, f.level()==g.level()
  if (!c_gcd.inBaseDomain())
  {
    i=g;
    while( i.hasTerms())
    {
      c_gcd=alg_gcd(i.coeff(),c_gcd,as);
      if (c_gcd.inBaseDomain()) break;
      i++;
    }
  }

  //f/=c_gcd;
  //g/=c_gcd;
  if (!c_gcd.isOne())
  {	  
    f=divide(f,c_gcd,as);
    g=divide(g,c_gcd,as);
  }

  CFList gg;
  CanonicalForm r=1;
  while (1)
  {
    //printf("f.mvar=%d, g.mvar=%d\n",f.level(),g.level());
    gg=as;
    if (!g.inCoeffDomain()) gg.append(g);
    //out_cf("Prem(",f," , ");
    //out_cf("",g,")\n");
    if (g.inCoeffDomain()|| g.isZero())
    {
      //printf("in coeff domain:");
      if (g.isZero()) { //printf("0\n");
        i=f;
        CanonicalForm f_gcd=i.coeff(); i++;
        while( i.hasTerms())
        {
          f_gcd=alg_gcd(i.coeff(),f_gcd,as);
          if (f_gcd.inBaseDomain()) break;
          i++;
        }  
	//out_cf("g=0 -> f:",f,"\n");
	//out_cf("f_gcd:",f_gcd,"\n");
	//out_cf("c_gcd:",c_gcd,"\n");
        //f/=f_gcd;  
	f=divide(f,f_gcd,as);
	//out_cf("f/f_gcd:",f,"\n");
	f*=c_gcd;
	//out_cf("f*c_gcd:",f,"\n");
        CanonicalForm r_lc=alg_lc(f);
	//out_cf("r_lc:",r_lc,"\n");
	//f/=r_lc;
	f=divide(f,r_lc,as);
	//out_cf(" -> gcd:",f,"\n");
        return f;
      }
      else { //printf("c\n"); 
	return c_gcd;}
    }  
    else if (g.level()==f.level()) r=Prem(f,gg);
    else
    {
      //printf("diff. level: %d, %d\n", f.level(), g.level());
      // g and f have different levels
      //out_cf("c_gcd=",c_gcd,"\n");
    //out_cf("of f=",fff," , ");
    //out_cf("and g=",ggg,"\n");
      return c_gcd;
    }
    //out_cf("->",r,"\n");
    f=g;
    g=r;
  }
  if (degree(f,Variable(mvg))==0)
  {
  // remark: mvf == mvg == f.level() ==g.level()
    //out_cf("c_gcd=",c_gcd,"\n");
    //out_cf("of f=",fff," , ");
    //out_cf("and g=",ggg,"\n");
    return c_gcd;
  }
  //out_cf("f=",f,"\n");
  i=f;
  CanonicalForm k=i.coeff(); i++;
  //out_cf("k=",k,"\n");
  while( i.hasTerms())
  {
    if (k.inCoeffDomain()) break;
    k=alg_gcd(i.coeff(),k,as);
    //out_cf("k=",k,"\n");
    i++;
  }
  //out_cf("end-k=",k,"\n");
  f/=k;
  //out_cf("f/k=",f,"\n");
  res=c_gcd*f;
  CanonicalForm r_lc=alg_lc(res);
  res/=r_lc;
  //CanonicalForm r_lc=alg_lc(res,as);
  //res/=r_lc;
  //out_cf("alg_gcd2=",res,"\n");
  //  out_cf("of f=",fff," , ");
  //  out_cf("and g=",ggg,"\n");
  //return res;
  //if (res.level()<fff.level() && res.level() < ggg.level())
  //  return alg_gcd(alg_gcd(fff,res,as),ggg,as);
  //else
    return res;
}

/*
$Log: not supported by cvs2svn $
Revision 1.11  2006/05/16 13:48:13  Singular
*hannes: gcc 4.1 fix: factory changes

Revision 1.10  2006/04/28 13:45:29  Singular
*hannes: better tests for 0, 1

Revision 1.9  2003/05/28 11:52:52  Singular
*pfister/hannes: newfactoras, alg_gcd, divide (see bug_33)

Revision 1.8  2002/10/24 17:22:22  Singular
* hannes: factoring in alg.ext., alg_gcd, NTL stuff

Revision 1.7  2002/08/19 11:11:31  Singular
* hannes/pfister: alg_gcd etc.

Revision 1.6  2002/01/21 09:11:07  Singular
* hannes: handle empty set in removecontent

Revision 1.5  2001/06/21 14:57:04  Singular
*hannes/GP: Factorize, newfactoras, ...

Revision 1.4  1998/03/12 12:34:35  schmidt
        * charset/csutil.cc, charset/alg_factor.cc: all references to
          `common_den()' replaced by `bCommonDen()'

Revision 1.3  1997/09/12 07:19:41  Singular
* hannes/michael: libfac-0.3.0

Revision 1.3  1997/04/25 22:55:00  michael
Version for libfac-0.2.1

Revision 1.2  1996/12/13 05:53:42  michael
fixed a typo in nopower

*/
