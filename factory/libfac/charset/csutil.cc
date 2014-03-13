////////////////////////////////////////////////////////////
// emacs edit mode for this file is -*- C++ -*-
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
extern void out_cf(const char *s1,const CanonicalForm &f,const char *s2);
extern CanonicalForm alg_lc(const CanonicalForm &f);
extern int hasAlgVar(const CanonicalForm &f);

static bool
lowerRank ( const CanonicalForm & f, const CanonicalForm & g, int & ind )
{
  int df, dg;
  Variable vf = f.mvar(), vg = g.mvar();

  if ( f.inCoeffDomain() )
  {
    if ( g.inCoeffDomain() ) ind= 1;
    return true;//( vg > vf );
  }
  else if ( g.inCoeffDomain() ) return false;
  else if ( vf < vg ) return true;
  else if ( vf == vg )
  {
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
  while ( i.hasItem() )
  {
    //CERR << "comparing " << f << "  and " << i.getItem()
    // << " == " << lowerRank( i.getItem(), f, ind ) << "\n";
    if ( lowerRank( i.getItem(), f, ind ) )
    {
      if ( ind )
      {
        CFList Itemlist= get_Terms(i.getItem());
        CFList Flist= get_Terms(f);

        // Have to further compare number of terms!
        //CERR << "compare terms! f= " << Flist.length() << "  item= "
        //     << Itemlist.length() <<"\n";
        if ( Itemlist.length() < Flist.length()) f = i.getItem();
        ind=0;
      }
      else
      {
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
Prem ( const CanonicalForm &f, const CanonicalForm &g )
{
  CanonicalForm ff, gg, l, test, lu, lv, t, retvalue;
  int df, dg;
  bool reord;
  Variable vf, vg, v;

  if ( (vf = f.mvar()) < (vg = g.mvar()) ) return f;
  else
  {
    if ( vf == vg )
    {
      ff = f; gg = g;
      reord = false;
      v = vg;
    }
    else
    {
      v = Variable(level(f.mvar()) + 1);
      ff = swapvar(f,vg,v);
      gg = swapvar(g,vg,v);
      reord=true;
    }
    dg = degree( gg, v );
    df = degree( ff, v );
    if (dg <= df) {l=LC(gg); gg = gg -LC(gg)*power(v,dg);}
    else { l = 1; }
    while ( ( dg <= df  ) && ( !ff.isZero()) )
    {
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
    if ( reord )
    {
      retvalue= swapvar( ff, vg, v );
    }
    else
    {
      retvalue= ff;
    }
    return retvalue;
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

  //out_cf("divide f=",ff,"\n");
  //out_cf("divide g=",f,"\n");
  if (f.inCoeffDomain())
  {
    bool b=!isOn(SW_RATIONAL);
    On(SW_RATIONAL);
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

// This function allows as to be empty; in that case, it is equivalent
// to the previous version (treating no variables as algebraic).
static CanonicalForm
myfitting( const CanonicalForm &f, const CFList &as )
{
 CanonicalForm rem=f;

 if ( !(rem.isZero()) )
 {
   if ( getCharacteristic() > 0 )
     return num((rem/lc(rem)));
   else
   {
     bool isRat= isOn (SW_RATIONAL);
     if (!isRat)
       On(SW_RATIONAL);
     CanonicalForm temp= mapinto(rem);
//      CERR << "temp= " << temp << "\n";
//      CERR << "lc(temp)= " << lc(temp) << "\n";
//      CERR << "temp/lc(temp)= " << temp/lc(temp) << "\n";
//      CERR << "num(rem/lc(rem))= " << num(rem/lc(rem)) << "\n";

     // If as is of length 1, and its polynomial is level 1, then
     // we treat the first variable as algebraic and invert the leading
     // coefficient where this variable is part of the coefficient domain.

     if (as.length() == 1 && level(as.getFirst()) == 1)
     {
       CanonicalForm lcoeff = temp;
       while (level(lcoeff) > 1)
       {
         lcoeff = LC(lcoeff);
       }
       // out_cf("myfitting: lcoeff = ", lcoeff, "\n");

       CanonicalForm p = as.getFirst();
       // out_cf("myfitting: p = ", p, "\n");

       CanonicalForm unused, inverse;

       extgcd(lcoeff, p, inverse, unused);
       // out_cf("myfitting: inverse = ", inverse, "\n");

       // This may leave temp with non-integral coefficients,
       // which will be cleaned up below.
       temp = temp * inverse;
       // out_cf("myfitting: temp = ", temp, "\n");
     }

     temp= bCommonDen(temp/lc(temp))*(temp/lc(temp));
     if (!isRat)
       Off(SW_RATIONAL);
     rem= mapinto(temp);
     return rem;
   }
 }
 else
   return rem;
}

CanonicalForm
Prem( const CanonicalForm &f, const CFList &L )
{
  CanonicalForm rem = f;
  CFListIterator i = L;
  for ( i.lastItem(); i.hasItem(); i-- )
  {
    //CERR << "   PREM: Prem(" << rem << "," ;
    rem = Prem( rem, i.getItem() );
    //CERR << "   PREM: Prem(" << rem << "," << i.getItem() << ")  = " << rem << "\n";
  }
  return myfitting(rem, CFList());
}

CanonicalForm
Prem( const CanonicalForm &f, const CFList &L, const CFList &as )
{
  CanonicalForm rem = f;
  CFListIterator i = L;
  for ( i.lastItem(); i.hasItem(); i-- )
  {
    //CERR << "   PREM: Prem(" << rem << "," ;
    rem = Prem( rem, i.getItem() );
    //CERR << "   PREM: Prem(" << rem << "," << i.getItem() << ")  = " << rem << "\n";
  }
  return myfitting(rem, as);
}

CFList
Prem( const CFList &AS, const CFList &L )
{
  CFList Output;

  for ( CFListIterator i=AS; i.hasItem(); i++ )
    Output = Union(CFList(Prem(i.getItem(),L)), Output);

  return Output;
}

static CanonicalForm
premasb( const CanonicalForm & f, const CFList & as)
{
  CanonicalForm remd=f;
  CFList AS=as;

  if ( as.length() > 1 )
  {
    AS.removeFirst(); // get rid of first elem
    CanonicalForm elem;
    while ( ! AS.isEmpty() )
    { // thats true for at least the first iteration
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
remsetb( const CFList & ps, const CFList & as)
{
  CFList output;
  CanonicalForm elem;
  for (CFListIterator i=ps; i.hasItem(); i++)
  {
    elem= premasb(i.getItem(),as);
    if ( !elem.isZero() ) output.append(elem);
  }
  return output;
}

// for characteristic sets
//////////////////////////////////
// replace the power of factors of polys in as by 1 if any
static CFList
nopower( const CanonicalForm & init )
{
  CFFList sqrfreelist;// = Factorize(init);//SqrFree(init);
  CFList output;
  CanonicalForm elem;
  int count=0;

  for ( CFIterator j=init; j.hasTerms(); j++ )
    if (!(j.coeff().isOne()) ) count++;
  //  if ( init != 1 ){
  //  CERR << "nopower: f is " << init << "\n";
  //  CERR << "nopower: count is " << count << "\n";}
  if ( count > 1 ) sqrfreelist = CFFList( CFFactor(init,1));
  else
  {
    sqrfreelist = Factorize(init);
    //sqrfreelist.removeFirst();
  }
  for ( CFFListIterator i=sqrfreelist; i.hasItem(); i++ )
  {
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
removefactor( CanonicalForm & r , PremForm & Remembern)
{
  int test;
  CanonicalForm a,b,testelem;
  CFList testlist;
  int n=level(r);
  CFListIterator j ;

  for ( int J=1; J<= n ; J++ )
  {
    testlist.append(CanonicalForm(Variable(J)));
  }

  //  testlist = Union(Remembern.FS1, testlist); // add candidates

  // remove already removed factors
  for ( j = Remembern.FS2 ; j.hasItem(); j++ )
  {
    testelem = j.getItem();
    while ( 1 )
    {
      test = mydivremt(r,testelem,a,b);
      if ( test && b.isZero() ) r = a;
      else break;
    }
  }

  // Let's look if we have other canditates to remove
  for ( j = testlist ; j.hasItem(); j++ )
  {
    testelem = j.getItem();
//    if ( testelem != r && testelem != r.mvar() ){
    if ( testelem != r )
    {
      while ( 1 )
      {
        test = divremt(r,testelem,a,b);
        if ( test && b.isZero() )
        {
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
factorps( const CFList &ps )
{
  CFList qs;
  CFFList q;
  CanonicalForm elem;

  for ( CFListIterator i=ps; i. hasItem(); i++ )
  {
    q=Factorize(i.getItem());
    q.removeFirst();
    // Next can be simplified ( first (already removed) elem in q is the only constant
    for ( CFFListIterator j=q; j.hasItem(); j++ )
    {
      elem = j.getItem().factor();
      if ( getNumVars(elem) > 0 )
        qs= Union(qs, CFList(myfitting(elem, CFList())));
    }
  }
  return qs;
}

// the initial of poly f wrt to the order of the variables
static CanonicalForm
inital( const CanonicalForm &f )
{
  CanonicalForm leadcoeff;

  if ( cls(f) == 0 ) {return f.genOne(); }
  else
  {
    leadcoeff = LC(f,lvar(f));
    //    if ( leadcoeff != 0 )
    return myfitting(leadcoeff, CFList()); //num(leadcoeff/lc(leadcoeff));
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

// the set of nonconstant initials of Cset
// with certain repeated factors cancelled
CFList
initalset1(const CFList & Cset)
{
  CFList temp;
  CFList initals;
  CanonicalForm init;

  for ( CFListIterator i = Cset ; i.hasItem(); i++ )
  {
    initals= nopower( inital(i.getItem()) );
    //    init= inital(i.getItem());
    for ( CFListIterator j = initals; j.hasItem(); j++)
    {
      init = j.getItem();
      if ( cls(init) > 0 )
        temp= Union(temp, CFList(init));
    }
  }
  return temp;
}

// the set of nonconstant initials of Cset of those polys
// not having their cls higher than reducible
// with certain repeated factors cancelled
CFList
initalset2(const CFList & Cset, const CanonicalForm & reducible)
{
  CFList temp;
  CFList initals;
  CanonicalForm init;
  int clsred = cls(reducible);

  for ( CFListIterator i = Cset ; i.hasItem(); i++ )
  {
    init = i.getItem();
    if ( cls(init) < clsred )
    {
      initals= nopower( inital(init) );
      //    init= inital(i.getItem());
      for ( CFListIterator j = initals; j.hasItem(); j++)
      {
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
#include <libfac/factor/debug.h>
// examine the irreducibility of as for IrrCharSeries
int
irreducible( const CFList & AS)
{
// AS is given by AS = { A1, A2, .. Ar }, d_i = degree(Ai)

// 1) we test: if d_i > 1, d_j =1 for all j<>i, then AS is irreducible.
  bool deg1=true;
  for ( CFListIterator i = AS ; i.hasItem(); i++ )
  {
    if ( degree(i.getItem()) > 1 )
    {
      if ( deg1 ) deg1=0;
      else return 0; // found 2nd poly with deg > 1
    }
  }
  return 1;
}


// select an item from PS for irras
CFList
select( const ListCFList & PS)
{
  return PS.getFirst();
}

// divide list ppi in elems having length <= and > length
void
select( const ListCFList & ppi, int length, ListCFList & ppi1, ListCFList & ppi2)
{
  CFList elem;
  for ( ListCFListIterator i=ppi ; i.hasItem(); i++ )
  {
    elem = i.getItem();
    if ( ! elem.isEmpty() )
    {
      if ( length <= elem.length() ){ ppi2.append(elem); }
      else { ppi1.append(elem); }
    }
  }
}


//////////////////////////////////////////////////////////////
// help-functions for sets

// is f in F ?
static bool
member( const CanonicalForm &f, const CFList &F)
{
  for ( CFListIterator i=F; i.hasItem(); i++ )
    if ( i.getItem() == f ) return 1;
  return 0;
}

// are list A and B the same?
bool
same( const CFList &A, const CFList &B )
{
  CFListIterator i;

  for (i = A; i.hasItem(); i++)
    if (! member(i.getItem(), B) )  return 0;
  for (i = B; i.hasItem(); i++)
    if (! member(i.getItem(), A) )  return 0;
  return 1;
}


// is List cs contained in List of lists pi?
bool
member( const CFList & cs, const ListCFList & pi )
{
  ListCFListIterator i;
  CFList elem;

  for ( i=pi; i.hasItem(); i++)
  {
    elem = i.getItem();
    if ( same(cs,elem) ) return 1;
  }
  return 0;
}

// is PS a subset of Cset ?
bool
subset( const CFList &PS, const CFList &Cset )
{
  //  CERR << "subset: called with: " << PS << "   " << Cset << "\n";
  for ( CFListIterator i=PS; i.hasItem(); i++ )
    if ( ! member(i.getItem(), Cset) )
    {
      //      CERR << "subset: " << i.getItem() << "  is not a member of " << Cset << "\n";
      return 0;
    }
  return 1;
}

// Union of two List of Lists
ListCFList
MyUnion( const ListCFList & a, const ListCFList &b )
{
  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for ( i = a ; i.hasItem(); i++ )
  {
    elem=i.getItem();
    if ( (! elem.isEmpty()) && ( ! member(elem,output)) )
    {
      output.append(elem);
    }
  }

  for ( i = b ; i.hasItem(); i++ )
  {
    elem=i.getItem();
    if ( (! elem.isEmpty()) && ( ! member(elem,output)) )
    {
      output.append(elem);
    }
  }
  return output;
}

//if list b is member of the list of lists remove b and return the rest
ListCFList
MyDifference( const ListCFList & a, const CFList &b)
{
  ListCFList output;
  ListCFListIterator i;
  CFList elem;

  for ( i = a ; i.hasItem(); i++ )
  {
    elem=i.getItem();
    if ( (! elem.isEmpty()) && ( ! same(elem,b)) )
    {
      output.append(elem);
    }
  }
return output;
}

// remove all elements of b from list of lists a and return the rest
ListCFList
Minus( const ListCFList & a, const ListCFList & b)
{
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
  bool isRat= isOn (SW_RATIONAL);
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
  //out_cf("alg_gcd fff(",fff," \n ");
  //out_cf("ggg",ggg,")\n");
  int v= as.getLast().level();
  if (f.level() <= v || g.level() <= v)
    return 1;

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
  CanonicalForm c_f= alg_content (f, as);

  //printf("f.mvar=%d (%d), g.mvar=%d (%d)\n",f.level(),mvf,g.level(),mvg);
  if (mvf != mvg) // => mvf > mvg
  {
    res= alg_gcd (g, c_f, as);
    return res;
  }
  Variable x= f.mvar();
  // now: mvf==mvg, f.level()==g.level()
  // content of g
  CanonicalForm c_g= alg_content (g, as);

  int delta= degree (f) - degree (g);

  //f/=c_gcd;
  //g/=c_gcd;
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

  CanonicalForm r=1;

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

