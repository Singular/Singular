/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: mpr_numeric.cc,v 1.1 1999-06-28 12:48:16 wenk Exp $ */

/* 
* ABSTRACT - multipolynomial resultants - numeric stuff
*            ( root finder, vandermonde system solver, simplex )   
*/

#include "mod2.h"
//#ifdef HAVE_MPR

//#define mprDEBUG_ALL

//-> includes
#include "structs.h"
#include "febase.h"
#include "mmemory.h"
#include "numbers.h"
#include "polys.h"
#include "ideals.h"
#include "intvec.h"
#include "longalg.h"
#include "ring.h"
//#include "longrat.h"
#include "lists.h"

#include <math.h>
#include "mpr_numeric.h"

extern size_t gmp_output_digits;
//<-

extern void nPrint(number n);  // for debugging output

//-----------------------------------------------------------------------------
//-------------- vandermonde system solver ------------------------------------
//-----------------------------------------------------------------------------

//-> vandermonde::*
vandermonde::vandermonde( const long _cn, const long _n, const long _maxdeg, 
			  number *_p, const bool _homog )
  : n(_n), cn(_cn), maxdeg(_maxdeg), p(_p), homog(_homog)
{
  long j;
  l= (long)pow((double)maxdeg+1,(int)n);
  x= (number *)Alloc( cn * sizeof(number) );
  for ( j= 0; j < cn; j++ ) x[j]= nInit(1);
  init();
}

vandermonde::~vandermonde()
{
  int j;
  for ( j= 0; j < cn; j++ ) nDelete( x+j );
  Free( (ADDRESS)x, cn * sizeof( number ) );
}

void vandermonde::init()
{
  int j;
  long i,c,sum;
  number tmp,tmp1;

  c=0;
  sum=0;

  intvec exp( n );
  for ( j= 0; j < n; j++ ) exp[j]=0;

  for ( i= 0; i < l; i++ ) {
    if ( !homog || (sum == maxdeg) ) {
      for ( j= 0; j < n; j++ ) {
	nPower( p[j], exp[j], &tmp );
	tmp1 = nMult( tmp, x[c] );
	x[c]= tmp1;
	nDelete( &tmp );
      }
      c++;
    }
    exp[0]++;
    sum=0;
    for ( j= 0; j < n - 1; j++ ) {
      if ( exp[j] > maxdeg ) {
	exp[j]= 0;
	exp[j + 1]++;
	}
      sum+= exp[j];
    }
    sum+= exp[n - 1];
  }
}

poly vandermonde::numvec2poly( const number * q )
{
  int j;
  long i,c,sum;
  number tmp;

  poly pnew,pit=NULL;

  c=0;
  sum=0;

  Exponent_t *exp= (Exponent_t *) Alloc( (n+1) * sizeof(Exponent_t) );

  for ( j= 0; j < n+1; j++ ) exp[j]=0;

  for ( i= 0; i < l; i++ ) {
    if ( (!homog || (sum == maxdeg)) && q[i] && !nIsZero(q[i]) ) {
      pnew= pOne();
      pSetCoeff(pnew,q[i]);
      pSetExpV(pnew,exp);
      if ( pit ) {
	pNext(pnew)= pit;
	pit= pnew;
      } else {
	pit= pnew;
	pNext(pnew)= NULL;
      } 
      pSetm(pit);
    }
    exp[1]++;
    sum=0;
    for ( j= 1; j < n; j++ ) {
      if ( exp[j] > maxdeg ) {
	exp[j]= 0;
	exp[j + 1]++;
	}
      sum+= exp[j];
    }
    sum+= exp[n];
  }

  Free( (ADDRESS) exp, (n+1) * sizeof(Exponent_t) );
  
  pOrdPolyMerge(pit);
  return pit;
}

number * vandermonde::interpolateDense( const number * q )
{
  int i,j,k;
  number newnum,tmp1;
  number b,t,xx,s;
  number *c;
  number *w;

  b=t=xx=s=tmp1=NULL;

  w= (number *)Alloc( cn * sizeof(number) );
  c= (number *)Alloc( cn * sizeof(number) );
  for ( j= 0; j < cn; j++ ) {
    w[j]= nInit(0);
    c[j]= nInit(0);
  }

  if ( cn == 1 ) {
    nDelete( &w[0] );
    w[0]= nCopy(q[0]);
  } else {
    nDelete( &c[cn-1] );
    c[cn-1]= nCopy(x[0]);
    c[cn-1]= nNeg(c[cn-1]);              // c[cn]= -x[1]

    for ( i= 1; i < cn; i++ ) {              // i=2; i <= cn
      nDelete( &xx );
      xx= nCopy(x[i]);
      xx= nNeg(xx);               // xx= -x[i]

      for ( j= (cn-i-1); j <= (cn-2); j++) { // j=(cn+1-i); j <= (cn-1)
	nDelete( &tmp1 );
	tmp1= nMult( xx, c[j+1] );           // c[j]= c[j] + (xx * c[j+1])
	newnum= nAdd( c[j], tmp1 );
	nDelete( &c[j] );
	c[j]= newnum;
      }

      newnum= nAdd( xx, c[cn-1] );           // c[cn-1]= c[cn-1] + xx
      nDelete( &c[cn-1] );
      c[cn-1]= newnum;
    }

    for ( i= 0; i < cn; i++ ) {              // i=1; i <= cn
      nDelete( &xx );
      xx= nCopy(x[i]);                     // xx= x[i]

      nDelete( &t );          
      t= nInit( 1 );                         // t= b= 1
      nDelete( &b );
      b= nInit( 1 );         
      nDelete( &s );                         // s= q[cn-1]
      s= nCopy( q[cn-1] );

      for ( k= cn-1; k >= 1; k-- ) {         // k=cn; k >= 2
	nDelete( &tmp1 );
	tmp1= nMult( xx, b );                // b= c[k] + (xx * b)
	nDelete( &b );
	b= nAdd( c[k], tmp1 ); 

	nDelete( &tmp1 );
	tmp1= nMult( q[k-1], b );            // s= s + (q[k-1] * b)
	newnum= nAdd( s, tmp1 );
	nDelete( &s );
	s= newnum;

	nDelete( &tmp1 );
	tmp1= nMult( xx, t );                // t= (t * xx) + b
	newnum= nAdd( tmp1, b );
	nDelete( &t );
	t= newnum;
      }

      nDelete( &w[i] );                      // w[i]= s/t
      w[i]= nDiv( s, t );
      nNormalize( w[i] );

      mprSTICKYPROT(ST_VANDER_STEP);
    }
  }
  mprSTICKYPROT("\n");

  // free mem
  for ( j= 0; j < cn; j++ ) nDelete( c+j );
  Free( (ADDRESS)c, cn * sizeof( number ) );

  nDelete( &tmp1 );
  nDelete( &s );
  nDelete( &t );
  nDelete( &b );
  nDelete( &xx );

  // makes quotiens smaller
  for ( j= 0; j < cn; j++ ) nNormalize( w[j] );

  return w;
}
//<-

//-----------------------------------------------------------------------------
//-------------- rootContainer ------------------------------------------------
//-----------------------------------------------------------------------------

//-> definitions
#define MR       8        // never change this value
#define MT      20       
#define MAXIT   (MT*MR)   // max number of iterations in laguer root finder

// set these values according to gmp_default_prec_bits and gmp_equalupto_bits!
#define EPS     2.0e-34   // used by rootContainer::laguer_driver(), imag() == 0.0 ???
//<-

//-> rootContainer::rootContainer() 
rootContainer::rootContainer()
{
  rt=none;

  coeffs= NULL;
  ievpoint= NULL;
  theroots= NULL;

  found_roots= false;
}
//<-

//-> rootContainer::~rootContainer() 
rootContainer::~rootContainer()
{
  int i;
  int n= pVariables;

  // free coeffs, ievpoint
  if ( ievpoint != NULL ) {
    for ( i=0; i < anz+2; i++ ) nDelete( ievpoint + i );
    Free( (ADDRESS)ievpoint, (anz+2) * sizeof( number ) );
  }

  for ( i=0; i <= tdg; i++ ) nDelete( coeffs + i );
  Free( (ADDRESS)coeffs, (tdg+1) * sizeof( number ) );

  // theroots löschen
  for ( i=0; i < tdg; i++ ) delete theroots[i];
  Free( (ADDRESS) theroots, (tdg)*sizeof(gmp_complex*) );
  
  mprPROTnl("~rootContainer()");
}
//<-

//-> void rootContainer::fillContainer( ... ) 
void rootContainer::fillContainer( number *_coeffs, number *_ievpoint, 
				   const int _var, const int _tdg, 
				   const rootType  _rt, const int _anz )
{
  int i;
  number nn= nInit(0);
  var=_var;
  tdg=_tdg;
  coeffs=_coeffs;
  rt=_rt;
  anz=_anz;

  for ( i=0; i <= tdg; i++ ) { 
    if ( nEqual(coeffs[i],nn) ) { 
      nDelete( &coeffs[i] );
      coeffs[i]=NULL;
    }
  }
  nDelete( &nn );

  if ( rt == cspecialmu && _ievpoint ) { // copy ievpoint
    ievpoint= (number *)Alloc( (anz+2) * sizeof( number ) );
    for (i=0; i < anz+2; i++) ievpoint[i]= nCopy( _ievpoint[i] );
  } 

  theroots= NULL;
  found_roots= false;
}
//<-

//-> poly rootContainer::getPoly() 
poly rootContainer::getPoly()
{
  int i;

  poly result= NULL;
  poly ppos;

  if ( (rt == cspecial) || ( rt == cspecialmu ) ) {
    for ( i= tdg; i >= 0; i-- ) {
      if ( coeffs[i] ) {
	poly p= pOne();
	//pSetExp( p, var+1, i);
	pSetExp( p, 1, i);
	pSetCoeff( p, nCopy( coeffs[i] ) );
	pSetm( p );
	if (result) {
	  ppos->next=p;
	  ppos=ppos->next;
	} else {
	  result=p;
	  ppos=p;
	}
      
      }
    }
    pSetm( result );
  } 

  return result;
}
//<-

//-> const gmp_complex & rootContainer::opterator[] ( const int i ) 
// this is now inline!
//  gmp_complex & rootContainer::operator[] ( const int i )
//  {
//    if ( found_roots && ( i >= 0) && ( i < tdg ) ) {
//      return *theroots[i];
//    } 
//    // warning
//    Werror("rootContainer::getRoot: Wrong index %d, found_roots %s",i,found_roots?"true":"false");
//    gmp_complex *tmp= new gmp_complex();
//    return *tmp;
//  }
//<-

//-> gmp_complex & rootContainer::evPointCoord( int i ) 
gmp_complex & rootContainer::evPointCoord( const int i )
{
  if (! ((i >= 0) && (i < anz+2) ) )
    Werror("rootContainer::evPointCoord: index out of range");
  if (ievpoint == NULL)
    Werror("rootContainer::evPointCoord: ievpoint == NULL");

  if ( (rt == cspecialmu) && found_roots ) {  // FIX ME
    if ( ievpoint[i] != NULL ) {
      gmp_complex *tmp= new gmp_complex();
      *tmp= numberToGmp_Complex(ievpoint[i]);
      return *tmp;
    } else {
      Werror("rootContainer::evPointCoord: NULL index %d",i);
    }
  }
   
  // warning
  Werror("rootContainer::evPointCoord: Wrong index %d, found_roots %s",i,found_roots?"true":"false");
  gmp_complex *tmp= new gmp_complex();
  return *tmp;
}
//<-

//-> bool rootContainer::changeRoots( int from, int to ) 
bool rootContainer::swapRoots( const int from, const int to )
{
  if ( found_roots && ( from >= 0) && ( from < tdg ) && ( to >= 0) && ( to < tdg ) ) {
    if ( to != from ) {
      gmp_complex tmp( *theroots[from] );
      *theroots[from]= *theroots[to];
      *theroots[to]= tmp;
    }
    return true;
  } 
    
  // warning
  Werror(" rootContainer::changeRoots: Wrong index %d, %d",from,to);
  return false;
}
//<-

//-> void rootContainer::solver() 
bool rootContainer::solver( const int polishmode )
{
  int i;

  // there are maximal tdg roots, so *roots ranges form 0 to tdg-1.
  theroots= (gmp_complex**)Alloc( (tdg)*sizeof(gmp_complex*) );
  for ( i=0; i < tdg; i++ ) theroots[i]= new gmp_complex();

  // copy the coefficients of type number to type gmp_complex
  gmp_complex **ad= (gmp_complex**)Alloc( (tdg+1)*sizeof(gmp_complex*) );
  for ( i=0; i <= tdg; i++ ) {
    ad[i]= new gmp_complex();
    if ( coeffs[i] ) *ad[i] = numberToGmp_Complex( coeffs[i] );
  }

  // now solve
  switch (polishmode) {
  case PM_NONE:
  case PM_POLISH:
    found_roots= laguer_driver( ad, theroots, polishmode == PM_POLISH );
    if (!found_roots) {
      Werror("rootContainer::solver: No roots found!");
      goto solverend;
    }
    break;
  case PM_CORRUPT:
    found_roots= laguer_driver( ad, theroots, false ); 
    // corrupt the roots
    for ( i= 0; i < tdg; i++ )
      *theroots[i]= *theroots[i] * (gmp_float)(1.0+0.01*(mprfloat)i);
    // and interpolate again      
    found_roots= laguer_driver( ad, theroots, true );
    if (!found_roots) {
      Werror("rootContainer::solver: No roots found!");
      goto solverend;
    }
    break;
  default:
    Werror("rootContainer::solver: Unsupported polish mode %d! Valid are [1,2,3].",polishmode);
    found_roots= false;
  } // switch

 solverend:
  for ( i=0; i <= tdg; i++ ) delete ad[i];
  Free( (ADDRESS) ad, (tdg+1)*sizeof(gmp_complex*) );

  return found_roots;
}
//<-

//-> gmp_complex* rootContainer::laguer_driver( bool polish ) 
bool rootContainer::laguer_driver(gmp_complex ** a, gmp_complex ** roots, bool polish )
{
  int i,j,jj;
  int its;
  gmp_complex x,b,c;
  bool ret= true;

  gmp_complex ** ad= (gmp_complex**)Alloc( (tdg+1)*sizeof(gmp_complex*) );
  for ( i=0; i <= tdg; i++ ) ad[i]= new gmp_complex( *a[i] );

  for ( j= tdg; j >= 1; j-- ) {
    x= gmp_complex();

    // run laguer alg
    laguer(ad, j, &x, &its);

    mprSTICKYPROT(ST_ROOTS_LGSTEP);
    if ( its > MAXIT ) {  // error
      Werror("rootContainer::laguer_driver: To many iterations!");
      ret= false;
      goto theend;
    }
    if ( abs(x.imag()) <= (gmp_float)(2.0*EPS)*abs(x.real())) {
      x= gmp_complex( x.real() );
    }
    *roots[j-1]= x;
    b= *ad[j];
    for ( jj= j-1; jj >= 0; jj-- ) {
      c= *ad[jj];
      *ad[jj]= b;
      b= ( x * b ) + c;
    }
  }

  if ( polish ) {
    mprSTICKYPROT(ST_ROOTS_LGPOLISH);
    for ( i=0; i <= tdg; i++ ) *ad[i]=*a[i];

    for ( j= 1; j <= tdg; j++ ) {
      // run laguer alg with corrupted roots
      laguer( ad, tdg, roots[j-1], &its );

      mprSTICKYPROT(ST_ROOTS_LGSTEP);
      if ( its > MAXIT ) {  // error
	Werror("rootContainer::laguer_driver: To many iterations!");
	ret= false;
	goto theend;
      }
    }
    for ( j= 2; j <= tdg; j++ ) { 
      // sort root by their absolute real parts by straight insertion
      x= *roots[j-1];
      for ( i= j-1; i >= 1; i-- ) {
	if ( abs(roots[i-1]->real()) <= abs(x.real()) ) break;
	*roots[i]= *roots[i-1];
      }
      *roots[i]= x;
    }
  }

 theend:
  mprSTICKYPROT("\n");
  for ( i=0; i <= tdg; i++ ) delete ad[i];
  Free( (ADDRESS) ad, (tdg+1)*sizeof( gmp_complex* ));

  return ret;
}
//<-

//-> void rootContainer::laguer(...) 
void rootContainer::laguer(gmp_complex ** a, int m, gmp_complex *x, int *its)
{
  int iter,j;
  gmp_float abx_g, abp_g, abm_g, err_g;
  gmp_complex dx, x1, b, d, f, g, h, sq, gp, gm, g2;
  static gmp_float frac_g[MR+1] = { 0.0, 0.5, 0.25, 0.75, 0.13, 0.38, 0.62, 0.88, 1.0 };

  gmp_float epss(1.0/pow(10.0,(int)(gmp_output_digits+gmp_output_digits/4)));

  for ( iter= 1; iter <= MAXIT; iter++ ) {
    mprSTICKYPROT(ST_ROOTS_LG);

    *its=iter;

    b= *a[m];
    err_g= abs(b);  
    d= gmp_complex();
    f= gmp_complex();
    abx_g= abs(*x);  

    for ( j= m-1; j >= 0; j-- ) {
      f= ( *x * f ) + d;
      d= ( *x * d ) + b;
      b= ( *x * b ) + *a[j];
      err_g= abs( b ) + ( abx_g * err_g ); 
    }

    err_g *= epss; // EPSS;

    if ( abs(b) <= err_g ) return;  

    g= d / b;
    g2 = g * g;
    h= g2 - ( ( f / b ) * (gmp_float)2.0 );
    sq= sqrt(( ( h * (gmp_float)m ) - g2 ) * (gmp_float)(m - 1));
    gp= g + sq;
    gm= g - sq;
    abp_g= abs( gp );  
    abm_g= abs( gm );  

    if ( abp_g < abm_g ) gp= gm;

    dx = ( (max(abp_g,abm_g) > (gmp_float)0.0) 
	   ? ( gmp_complex( (mprfloat)m ) / gp ) 
	   : ( gmp_complex( cos((mprfloat)iter),sin((mprfloat)iter)) 
	       * exp(log((gmp_float)1.0+abx_g))) );
    
    x1= *x - dx;

    if ( (*x == x1) ) return;

    if ( iter % MT ) *x= x1;
    else *x -= ( dx * frac_g[ iter / MT ] );
  }

  *its= MAXIT+1;

  return;
}
//<-

//-----------------------------------------------------------------------------
//-------------- rootArranger -------------------------------------------------
//-----------------------------------------------------------------------------

//-> rootArranger::rootArranger(...) 
rootArranger::rootArranger( rootContainer ** _roots, 
			    rootContainer ** _mu, 
			    const int _howclean )
  : roots(_roots), mu(_mu), howclean(_howclean)
{
  found_roots=false;
}
//<-

//-> void rootArranger::solve_all()
void rootArranger::solve_all()
{
  int i;
  found_roots= true;

  // find roots of polys given by coeffs in roots
  rc= roots[0]->getAnzElems();
  for ( i= 0; i < rc; i++ ) 
    if ( !roots[i]->solver( howclean ) ) {
      found_roots= false;
      return;
    }
  // find roots of polys given by coeffs in mu
  mc= mu[0]->getAnzElems();
  for ( i= 0; i < mc; i++ ) 
    if ( ! mu[i]->solver( howclean ) ) {
      found_roots= false;
      return;
    }
}
//<-

//-> void rootArranger::arrange() 
void rootArranger::arrange()
{
  gmp_complex tmp,zwerg;
  int anzm= mu[0]->getAnzElems();
  int anzr= roots[0]->getAnzRoots();
  int xkoord, r, rtest, xk, mtest;
  bool found;
  //gmp_complex mprec(1.0/pow(10,gmp_output_digits-5),1.0/pow(10,gmp_output_digits-5));
  gmp_float mprec(1.0/pow(10.0,(int)(gmp_output_digits/3)));

  for ( xkoord= 0; xkoord < anzm; xkoord++ ) {    // für x1,x2, x1,x2,x3, x1,x2,...,xn
    for ( r= 0; r < anzr; r++ ) {                 // für jede Nullstelle 
      // (x1-koordinate) * evp[1] + (x2-koordinate) * evp[2] + 
      //                                  ... + (xkoord-koordinate) * evp[xkoord]
      tmp= gmp_complex();
      for ( xk =0; xk <= xkoord; xk++ ){ 
	tmp -= (*roots[xk])[r] * mu[xkoord]->evPointCoord(xk+1); //xk+1
      }
      found= false;
      for ( rtest= r; rtest < anzr; rtest++ ) {   // für jede Nullstelle 
	zwerg = tmp - (*roots[xk])[rtest] * mu[xkoord]->evPointCoord(xk+1); // xk+1, xkoord+2
        for ( mtest= 0; mtest < anzr; mtest++ ) {
	  //	  if ( tmp == (*mu[xkoord])[mtest] ) {
	  if ( ((zwerg.real() <= (*mu[xkoord])[mtest].real() + mprec) && 
		(zwerg.real() >= (*mu[xkoord])[mtest].real() - mprec)) &&
	       ((zwerg.imag() <= (*mu[xkoord])[mtest].imag() + mprec) && 
		(zwerg.imag() >= (*mu[xkoord])[mtest].imag() - mprec)) ) {
	    roots[xk]->swapRoots( r, rtest );
	    found= true;
	    break;
	  }
	}
      } // rtest
      if ( !found ) {
	Werror("rootArranger::arrange: No match? coord %d, root %d.",xkoord,r);
#ifdef mprDEBUG_PROT
	Werror("One of these ...");
	for ( rtest= r; rtest < anzr; rtest++ ) { 
	  tmp= gmp_complex();
	  for ( xk =0; xk <= xkoord; xk++ ){ 
	    tmp-= (*roots[xk])[r] * mu[xkoord]->evPointCoord(xk+1);
	  }
	  tmp-= (*roots[xk])[rtest] * mu[xkoord]->evPointCoord(xk+1); // xkoord+2
	  Werror("  %s",complexToStr(tmp,gmp_output_digits+1),rtest);
	}
	Werror(" ... must match to one of these:");
	for ( mtest= 0; mtest < anzr; mtest++ ) {
	  Werror("                  %s",complexToStr((*mu[xkoord])[mtest],gmp_output_digits+1));
	}
#endif
      }
    } // r
  } // xkoord
}
//<-

//-> lists rootArranger::listOfRoots( int oprec ) 
lists rootArranger::listOfRoots( const unsigned int oprec )
{
  int i,j,tr;
  int count= roots[0]->getAnzRoots(); // number of roots
  int elem= roots[0]->getAnzElems();  // number of koordinates per root

  lists listofroots= (lists)Alloc( sizeof(slists) ); // must be done this way!

  if ( found_roots ) {
    listofroots->Init( count );
    
    for (i=0; i < count; i++) {
      lists onepoint= (lists)Alloc(sizeof(slists)); // must be done this way!
      onepoint->Init(elem);
      for ( j= 0; j < elem; j++ ) {
	if ( !rField_is_long_C() ) {
	  onepoint->m[j].rtyp=STRING_CMD;
	  onepoint->m[j].data=(void *)complexToStr((*roots[j])[i],oprec);
	} else {
	  onepoint->m[j].rtyp=NUMBER_CMD;
	  onepoint->m[j].data=(void *)nCopy((number)(roots[j]->getRoot(i)));
	}
	onepoint->m[j].next= NULL;
	onepoint->m[j].name= NULL;
      }
      listofroots->m[i].rtyp=LIST_CMD;
      listofroots->m[i].data=(void *)onepoint;
      listofroots->m[j].next= NULL;
      listofroots->m[j].name= NULL;
    }

  } else {
    listofroots->Init( 0 );
  }

  return listofroots;
}
//<-

//-----------------------------------------------------------------------------
//-------------- ludcmp/lubksb ------------------------------------------------
//-----------------------------------------------------------------------------

//#define error(a) a
#define error(a)

//-> simplex
//
void simp1( mprfloat **a, int mm, int ll[], int nll, int iabf, int *kp, mprfloat *bmax );
void simp2( mprfloat **a, int n, int l2[], int nl2, int *ip, int kp, mprfloat *q1 );
void simp3( mprfloat **a, int i1, int k1, int ip, int kp );

void simplx( mprfloat **a, int m, int n, int m1, int m2, int m3, int *icase, int izrov[], int iposv[] )
{
  int i,ip,ir,is,k,kh,kp,m12,nl1,nl2;
  int *l1,*l2,*l3;
  mprfloat q1, bmax; 

  if ( m != (m1+m2+m3)) {
    // error: bad input
    error(Werror(" bad input constraint counts in simplex ");)
    *icase=-2;
    return;
  }

  l1= (int *) Alloc0( (n+1) * sizeof(int) );
  l2= (int *) Alloc0( (m+1) * sizeof(int) );
  l3= (int *) Alloc0( (m+1) * sizeof(int) );

  nl1= n;
  for ( k=1; k<=n; k++ ) l1[k]=izrov[k]=k;
  nl2=m;
  for ( i=1; i<=m; i++ ) {
    if ( a[i+1][1] < 0.0 ) {
      // error: bad input
      error(Werror(" bad input tableau in simplex ");)
      *icase=-2;
      // free mem l1,l2,l3;
      Free( (ADDRESS) l3, (m+1) * sizeof(int) );
      Free( (ADDRESS) l2, (m+1) * sizeof(int) );
      Free( (ADDRESS) l1, (n+1) * sizeof(int) );
      return;
    }
    l2[i]= i;
    iposv[i]= n+i;
  }
  for ( i=1; i<=m2; i++) l3[i]= 1;
  ir= 0;
  if (m2+m3) {
    ir=1;
    for ( k=1; k <= (n+1); k++ ) {
      q1=0.0;
      for ( i=m1+1; i <= m; i++ ) q1+= a[i+1][k];
      a[m+2][k]= -q1;
    }

    do {
      simp1(a,m+1,l1,nl1,0,&kp,&bmax);
      if ( bmax <= SIMPLEX_EPS && a[m+2][1] < -SIMPLEX_EPS ) {
	*icase= -1; // no solution found
	// free mem l1,l2,l3;
	Free( (ADDRESS) l3, (m+1) * sizeof(int) );
	Free( (ADDRESS) l2, (m+1) * sizeof(int) );
	Free( (ADDRESS) l1, (n+1) * sizeof(int) );
	return;
      } else if ( bmax <= SIMPLEX_EPS && a[m+2][1] <= SIMPLEX_EPS ) {
	m12= m1+m2+1;
	if ( m12 <= m ) {
	  for ( ip= m12; ip <= m; ip++ ) {
	    if ( iposv[ip] == (ip+n) ) {
	      simp1(a,ip,l1,nl1,1,&kp,&bmax);
	      if ( fabs(bmax) >= SIMPLEX_EPS)
		goto one;
	    }
	  }
	}
	ir= 0;
	--m12;
	if ( m1+1 <= m12 )
	  for ( i=m1+1; i <= m12; i++ ) 
	    if ( l3[i-m1] == 1 ) 
	      for ( k=1; k <= n+1; k++ ) 
		a[i+1][k] = -a[i+1][k];
	break;
      }
      //#if DEBUG
      //print_bmat( a, m+2, n+3);
      //#endif
      simp2(a,n,l2,nl2,&ip,kp,&q1);
      if ( ip == 0 ) {
	*icase = -1; // no solution found
	// free mem l1,l2,l3;
	Free( (ADDRESS) l3, (m+1) * sizeof(int) );
	Free( (ADDRESS) l2, (m+1) * sizeof(int) );
	Free( (ADDRESS) l1, (n+1) * sizeof(int) );
	return;
      }
    one: simp3(a,m+1,n,ip,kp);
      // #if DEBUG
      // print_bmat(a,m+2,n+3);
      // #endif
      if ( iposv[ip] >= (n+m1+m2+1)) {
	for ( k=1; k<= nl1; k++ ) 
	  if ( l1[k] == kp ) break;
	--nl1;
	for ( is=k; is <= nl1; is++ ) l1[is]= l1[is+1];
	++a[m+2][kp+1];
	for ( i= 1; i <= m+2; i++ ) a[i][kp+1] = -a[i][kp+1];
      } else {
	if ( iposv[ip] >= (n+m1+1) ) {
	  kh= iposv[ip]-m1-n;
	  if ( l3[kh] ) {
	    l3[kh]= 0;
	    ++a[m+2][kp+1];
	    for ( i=1; i<= m+2; i++ )
	      a[i][kp+1] = -a[i][kp+1];
	  }
	}
      }
      is= izrov[kp];
      izrov[kp]= iposv[ip];
      iposv[ip]= is;
    } while (ir);
  }
  /* end of phase 1, have feasible sol, now optimize it */
  for (;;) {
    // #if DEBUG
    // print_bmat( a, m+1, n+5);
    // #endif
    simp1(a,0,l1,nl1,0,&kp,&bmax);
    if (bmax <= /*SIMPLEX_EPS*/0.0) {
      *icase=0; // finite solution found
      // free mem l1,l2,l3
      Free( (ADDRESS) l3, (m+1) * sizeof(int) );
      Free( (ADDRESS) l2, (m+1) * sizeof(int) );
      Free( (ADDRESS) l1, (n+1) * sizeof(int) );
      return;
    }
    simp2(a,n,l2,nl2,&ip,kp,&q1);
    if (ip == 0) {
      //printf("Unbounded:");
      // #if DEBUG
      //       print_bmat( a, m+1, n+1);
      // #endif
      *icase=1;		/* unbounded */
      // free mem
      Free( (ADDRESS) l3, (m+1) * sizeof(int) );
      Free( (ADDRESS) l2, (m+1) * sizeof(int) );
      Free( (ADDRESS) l1, (n+1) * sizeof(int) );
      return;
    }
    simp3(a,m,n,ip,kp);
    is= izrov[kp];
    izrov[kp]= iposv[ip];
    iposv[ip]= is;	
  }/*for ;;*/
}

void simp1( mprfloat **a, int mm, int ll[], int nll, int iabf, int *kp, mprfloat *bmax )
{
  int k;
  mprfloat test;

  if( nll <= 0) {			/* init'tion: fixed */
    *bmax = 0.0;
    return;
  }
  *kp=ll[1];
  *bmax=a[mm+1][*kp+1];
  for (k=2;k<=nll;k++) {
    if (iabf == 0) {
      test=a[mm+1][ll[k]+1]-(*bmax);
      if (test > 0.0) {
	*bmax=a[mm+1][ll[k]+1];
	*kp=ll[k];
      }
    } else {			/* abs values: have fixed it */
      test=fabs(a[mm+1][ll[k]+1])-fabs(*bmax);
      if (test > 0.0) {
	*bmax=a[mm+1][ll[k]+1];
	*kp=ll[k];
      }
    }
  }
}

void simp2( mprfloat **a, int n, int l2[], int nl2, int *ip, int kp, mprfloat *q1 )
{
  int k,ii,i;
  mprfloat qp,q0,q;

  *ip= 0;
  for ( i=1; i <= nl2; i++ ) {
    if ( a[l2[i]+1][kp+1] < -SIMPLEX_EPS ) {
      *q1= -a[l2[i]+1][1] / a[l2[i]+1][kp+1];
      *ip= l2[i];
      for ( i= i+1; i <= nl2; i++ ) {
	ii= l2[i];
	if (a[ii+1][kp+1] < -SIMPLEX_EPS) {
	  q= -a[ii+1][1] / a[ii+1][kp+1];
	  if (q - *q1 < -SIMPLEX_EPS) {
	    *ip=ii;
	    *q1=q;
	  } else if (q - *q1 < SIMPLEX_EPS) {
	    for ( k=1; k<= n; k++ ) {
	      qp= -a[*ip+1][k+1]/a[*ip+1][kp+1];
	      q0= -a[ii+1][k+1]/a[ii+1][kp+1];
	      if ( q0 != qp ) break;
	    }
	    if ( q0 < qp ) *ip= ii;
	  }
	}
      }
    }
  }
}

void simp3( mprfloat **a, int i1, int k1, int ip, int kp )
{
  int kk,ii;
  mprfloat piv;

  piv= 1.0 / a[ip+1][kp+1];
  for ( ii=1; ii <= i1+1; ii++ ) 
    if ( ii -1 != ip ) {
      a[ii][kp+1] *= piv;
      for ( kk=1; kk <= k1+1; kk++ )  
	if ( kk-1 != kp ) 
	  a[ii][kk] -= a[ip+1][kk] * a[ii][kp+1];
    }
  for ( kk=1; kk<= k1+1; kk++ ) 
    if ( kk-1 != kp ) a[ip+1][kk] *= -piv;
  a[ip+1][kp+1]= piv;
}
//<-

//-----------------------------------------------------------------------------

//#endif // HAVE_MPR

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***



