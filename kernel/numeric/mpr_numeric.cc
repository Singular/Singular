/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/


/*
* ABSTRACT - multipolynomial resultants - numeric stuff
*            ( root finder, vandermonde system solver, simplex )
*/

#include "kernel/mod2.h"

//#define mprDEBUG_ALL

#include "misc/options.h"

#include "coeffs/numbers.h"
#include "coeffs/mpr_global.h"

#include "polys/matpol.h"

#include "kernel/polys.h"

#include "mpr_base.h"
#include "mpr_numeric.h"

#include <cmath>
//<-

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
  x= (number *)omAlloc( cn * sizeof(number) );
  for ( j= 0; j < cn; j++ ) x[j]= nInit(1);
  init();
}

vandermonde::~vandermonde()
{
  int j;
  for ( j= 0; j < cn; j++ ) nDelete( x+j );
  omFreeSize( (void *)x, cn * sizeof( number ) );
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

  for ( i= 0; i < l; i++ )
  {
    if ( !homog || (sum == maxdeg) )
    {
      for ( j= 0; j < n; j++ )
      {
        nPower( p[j], exp[j], &tmp );
        tmp1 = nMult( tmp, x[c] );
        x[c]= tmp1;
        nDelete( &tmp );
      }
      c++;
    }
    exp[0]++;
    sum=0;
    for ( j= 0; j < n - 1; j++ )
    {
      if ( exp[j] > maxdeg )
      {
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
  long i,/*c,*/sum;

  poly pnew,pit=NULL;

  // c=0;
  sum=0;

  int *exp= (int *) omAlloc( (n+1) * sizeof(int) );

  for ( j= 0; j < n+1; j++ ) exp[j]=0;

  for ( i= 0; i < l; i++ )
  {
    if ( (!homog || (sum == maxdeg)) && q[i] && !nIsZero(q[i]) )
    {
      pnew= pOne();
      pSetCoeff(pnew,q[i]);
      pSetExpV(pnew,exp);
      if ( pit )
      {
        pNext(pnew)= pit;
        pit= pnew;
      }
      else
      {
        pit= pnew;
        pNext(pnew)= NULL;
      }
      pSetm(pit);
    }
    exp[1]++;
    sum=0;
    for ( j= 1; j < n; j++ )
    {
      if ( exp[j] > maxdeg )
      {
        exp[j]= 0;
        exp[j + 1]++;
        }
      sum+= exp[j];
    }
    sum+= exp[n];
  }

  omFreeSize( (void *) exp, (n+1) * sizeof(int) );

  pSortAdd(pit);
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

  w= (number *)omAlloc( cn * sizeof(number) );
  c= (number *)omAlloc( cn * sizeof(number) );
  for ( j= 0; j < cn; j++ )
  {
    w[j]= nInit(0);
    c[j]= nInit(0);
  }

  if ( cn == 1 )
  {
    nDelete( &w[0] );
    w[0]= nCopy(q[0]);
  }
  else
  {
    nDelete( &c[cn-1] );
    c[cn-1]= nCopy(x[0]);
    c[cn-1]= nInpNeg(c[cn-1]);              // c[cn]= -x[1]

    for ( i= 1; i < cn; i++ ) {              // i=2; i <= cn
      if (xx!=NULL) nDelete( &xx );
      xx= nCopy(x[i]);
      xx= nInpNeg(xx);               // xx= -x[i]

      for ( j= (cn-i-1); j <= (cn-2); j++) { // j=(cn+1-i); j <= (cn-1)
        if (tmp1!=NULL) nDelete( &tmp1 );
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

      if (t!=NULL) nDelete( &t );
      t= nInit( 1 );                        // t= b= 1
      if (b!=NULL) nDelete( &b );
      b= nInit( 1 );
      if (s!=NULL) nDelete( &s );           // s= q[cn-1]
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

      if (!nIsZero(t))
      {
        nDelete( &w[i] );                      // w[i]= s/t
        w[i]= nDiv( s, t );
        nNormalize( w[i] );
      }

      mprSTICKYPROT(ST_VANDER_STEP);
    }
  }
  mprSTICKYPROT("\n");

  // free mem
  for ( j= 0; j < cn; j++ ) nDelete( c+j );
  omFreeSize( (void *)c, cn * sizeof( number ) );

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
#define MT      5
#define MMOD    (MT*MR)
#define MAXIT   (5*MMOD)   // max number of iterations in laguer root finder


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
  // free coeffs, ievpoint
  if ( ievpoint != NULL )
  {
    for ( i=0; i < anz+2; i++ ) nDelete( ievpoint + i );
    omFreeSize( (void *)ievpoint, (anz+2) * sizeof( number ) );
  }

  for ( i=0; i <= tdg; i++ ) 
    if (coeffs[i]!=NULL) nDelete( coeffs + i );
  omFreeSize( (void *)coeffs, (tdg+1) * sizeof( number ) );

  // theroots löschen
  for ( i=0; i < tdg; i++ ) delete theroots[i];
  omFreeSize( (void *) theroots, (tdg)*sizeof(gmp_complex*) );

  //mprPROTnl("~rootContainer()");
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

  for ( i=0; i <= tdg; i++ )
  {
    if ( nEqual(coeffs[i],nn) )
    {
      nDelete( &coeffs[i] );
      coeffs[i]=NULL;
    }
  }
  nDelete( &nn );

  if ( rt == cspecialmu && _ievpoint )  // copy ievpoint
  {
    ievpoint= (number *)omAlloc( (anz+2) * sizeof( number ) );
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

  if ( (rt == cspecial) || ( rt == cspecialmu ) )
  {
    for ( i= tdg; i >= 0; i-- )
    {
      if ( coeffs[i] )
      {
        poly p= pOne();
        //pSetExp( p, var+1, i);
        pSetExp( p, 1, i);
        pSetCoeff( p, nCopy( coeffs[i] ) );
        pSetm( p );
        if (result)
        {
          ppos->next=p;
          ppos=ppos->next;
        }
        else
        {
          result=p;
          ppos=p;
        }

      }
    }
    if (result!=NULL) pSetm( result );
  }

  return result;
}
//<-

//-> const gmp_complex & rootContainer::opterator[] ( const int i )
// this is now inline!
//  gmp_complex & rootContainer::operator[] ( const int i )
//  {
//    if ( found_roots && ( i >= 0) && ( i < tdg ) )
//    {
//      return *theroots[i];
//    }
//    // warning
//    Warn("rootContainer::getRoot: Wrong index %d, found_roots %s",i,found_roots?"true":"false");
//    gmp_complex *tmp= new gmp_complex();
//    return *tmp;
//  }
//<-

//-> gmp_complex & rootContainer::evPointCoord( int i )
gmp_complex & rootContainer::evPointCoord( const int i )
{
  if (! ((i >= 0) && (i < anz+2) ) )
    WarnS("rootContainer::evPointCoord: index out of range");
  if (ievpoint == NULL)
    WarnS("rootContainer::evPointCoord: ievpoint == NULL");

  if ( (rt == cspecialmu) && found_roots ) // FIX ME
  {
    if ( ievpoint[i] != NULL )
    {
      gmp_complex *tmp= new gmp_complex();
      *tmp= numberToComplex(ievpoint[i], currRing->cf);
      return *tmp;
    }
    else
    {
      Warn("rootContainer::evPointCoord: NULL index %d",i);
    }
  }

  // warning
  Warn("rootContainer::evPointCoord: Wrong index %d, found_roots %s",i,found_roots?"true":"false");
  gmp_complex *tmp= new gmp_complex();
  return *tmp;
}
//<-

//-> bool rootContainer::changeRoots( int from, int to )
bool rootContainer::swapRoots( const int from, const int to )
{
  if ( found_roots && ( from >= 0) && ( from < tdg ) && ( to >= 0) && ( to < tdg ) )
  {
    if ( to != from )
    {
      gmp_complex tmp( *theroots[from] );
      *theroots[from]= *theroots[to];
      *theroots[to]= tmp;
    }
    return true;
  }

  // warning
  Warn(" rootContainer::changeRoots: Wrong index %d, %d",from,to);
  return false;
}
//<-

//-> void rootContainer::solver()
bool rootContainer::solver( const int polishmode )
{
  int i;

  // there are maximal tdg roots, so *roots ranges form 0 to tdg-1.
  theroots= (gmp_complex**)omAlloc( tdg*sizeof(gmp_complex*) );
  for ( i=0; i < tdg; i++ ) theroots[i]= new gmp_complex();

  // copy the coefficients of type number to type gmp_complex
  gmp_complex **ad= (gmp_complex**)omAlloc( (tdg+1)*sizeof(gmp_complex*) );
  for ( i=0; i <= tdg; i++ )
  {
    ad[i]= new gmp_complex();
    if ( coeffs[i] ) *ad[i] = numberToComplex( coeffs[i], currRing->cf );
  }

  // now solve
  found_roots= laguer_driver( ad, theroots, polishmode != 0 );
  if (!found_roots)
    WarnS("rootContainer::solver: No roots found!");

  // free memory
  for ( i=0; i <= tdg; i++ ) delete ad[i];
  omFreeSize( (void *) ad, (tdg+1)*sizeof(gmp_complex*) );

  return found_roots;
}
//<-

//-> gmp_complex* rootContainer::laguer_driver( bool polish )
bool rootContainer::laguer_driver(gmp_complex ** a, gmp_complex ** roots, bool polish )
{
  int i,j,k,its;
  gmp_float zero(0.0);
  gmp_complex x(0.0),o(1.0);
  bool ret= true, isf=isfloat(a), type=true;

  gmp_complex ** ad= (gmp_complex**)omAlloc( (tdg+1)*sizeof(gmp_complex*) );
  for ( i=0; i <= tdg; i++ ) ad[i]= new gmp_complex( *a[i] );

  k = 0;
  i = tdg;
  j = i-1;
  while (i>2)
  {
    // run laguer alg
    x = zero;
    laguer(ad, i, &x, &its, type);
    if ( its > MAXIT )
    {
      type = !type;
      x = zero;
      laguer(ad, i, &x, &its, type);
    }

    mprSTICKYPROT(ST_ROOTS_LGSTEP);
    if ( its > MAXIT )
    {  // error
      WarnS("Laguerre solver: Too many iterations!");
      ret= false;
      goto theend;
    }
    if ( polish )
    {
      laguer( a, tdg, &x, &its , type);
      if ( its > MAXIT )
      {  // error
        WarnS("Laguerre solver: Too many iterations in polish!");
        ret= false;
        goto theend;
      }
    }
    if ((!type)&&(!((x.real()==zero)&&(x.imag()==zero)))) x = o/x;
    if (x.imag() == zero)
    {
      *roots[k] = x;
      k++;
      divlin(ad,x,i);
      i--;
    }
    else
    {
      if(isf)
      {
        *roots[j] = x;
        *roots[j-1]= gmp_complex(x.real(),-x.imag());
        j -= 2;
        divquad(ad,x,i);
        i -= 2;
      }
      else
      {
        *roots[j] = x;
        j--;
        divlin(ad,x,i);
        i--;
      }
    }
    type = !type;
  }
  solvequad(ad,roots,k,j);
  sortroots(roots,k,j,isf);

theend:
  mprSTICKYPROT("\n");
  for ( i=0; i <= tdg; i++ ) delete ad[i];
  omFreeSize( (void *) ad, (tdg+1)*sizeof( gmp_complex* ));

  return ret;
}
//<-

//-> void rootContainer::laguer(...)
void rootContainer::laguer(gmp_complex ** a, int m, gmp_complex *x, int *its, bool type)
{
  int iter,j;
  gmp_float zero(0.0),one(1.0),deg(m);
  gmp_float abx_g, err_g, fabs;
  gmp_complex dx, x1, b, d, f, g, h, sq, gp, gm, g2;
  gmp_float frac_g[MR+1] = { 0.0, 0.5, 0.25, 0.75, 0.125, 0.375, 0.625, 0.875, 1.0 };

  gmp_float epss(0.1);
  mpf_pow_ui(*epss._mpfp(),*epss.mpfp(),gmp_output_digits);

  for ( iter= 1; iter <= MAXIT; iter++ )
  {
    mprSTICKYPROT(ST_ROOTS_LG);
    *its=iter;
    if (type)
      computefx(a,*x,m,b,d,f,abx_g,err_g);
    else
      computegx(a,*x,m,b,d,f,abx_g,err_g);
    err_g *= epss; // EPSS;

    fabs = abs(b);
    if (fabs <= err_g)
    {
      if ((fabs==zero) || (abs(d)==zero)) return;
      *x -= (b/d); // a last newton-step
      goto ende;
    }

    g= d / b;
    g2 = g * g;
    h= g2 - (((f+f) / b ));
    sq= sqrt(( ( h * deg ) - g2 ) * (deg - one));
    gp= g + sq;
    gm= g - sq;
    if (abs(gp)<abs(gm))
    {
      dx = deg/gm;
    }
    else
    {
      if((gp.real()==zero)&&(gp.imag()==zero))
      {
        dx.real(cos((mprfloat)iter));
        dx.imag(sin((mprfloat)iter));
        dx = dx*(one+abx_g);
      }
      else
      {
        dx = deg/gp;
      }
    }
    x1= *x - dx;

    if (*x == x1) goto ende;

    j = iter%MMOD;
    if (j==0) j=MT;
    if ( j % MT ) *x= x1;
    else *x -= ( dx * frac_g[ j / MT ] );
  }

  *its= MAXIT+1;
ende:
  checkimag(x,epss);
}

void rootContainer::checkimag(gmp_complex *x, gmp_float &e)
{
  if(abs(x->imag())<abs(x->real())*e)
  {
    x->imag(0.0);
  }
}

bool rootContainer::isfloat(gmp_complex **a)
{
  gmp_float z(0.0);
  gmp_complex *b;
  for (int i=tdg; i >= 0; i-- )
  {
    b = &(*a[i]);
    if (!(b->imag()==z))
      return false;
  }
  return true;
}

void rootContainer::divlin(gmp_complex **a, gmp_complex x, int j)
{
  int i;
  gmp_float o(1.0);

  if (abs(x)<o)
  {
    for (i= j-1; i > 0; i-- )
      *a[i] += (*a[i+1]*x);
    for (i= 0; i < j; i++ )
      *a[i] = *a[i+1];
  }
  else
  {
    gmp_complex y(o/x);
    for (i= 1; i < j; i++)
      *a[i] += (*a[i-1]*y);
  }
}

void rootContainer::divquad(gmp_complex **a, gmp_complex x, int j)
{
  int i;
  gmp_float o(1.0),p(x.real()+x.real()),
            q((x.real()*x.real())+(x.imag()*x.imag()));

  if (abs(x)<o)
  {
    *a[j-1] += (*a[j]*p);
    for (i= j-2; i > 1; i-- )
      *a[i] += ((*a[i+1]*p)-(*a[i+2]*q));
    for (i= 0; i < j-1; i++ )
      *a[i] = *a[i+2];
  }
  else
  {
    p = p/q;
    q = o/q;
    *a[1] += (*a[0]*p);
    for (i= 2; i < j-1; i++)
      *a[i] += ((*a[i-1]*p)-(*a[i-2]*q));
  }
}

void rootContainer::solvequad(gmp_complex **a, gmp_complex **r, int &k, int &j)
{
  gmp_float zero(0.0);

  if ((j>k)
  &&((!(*a[2]).real().isZero())||(!(*a[2]).imag().isZero())))
  {
    gmp_complex sq(zero);
    gmp_complex h1(*a[1]/(*a[2] + *a[2])), h2(*a[0] / *a[2]);
    gmp_complex disk((h1 * h1) - h2);
    if (disk.imag().isZero())
    {
      if (disk.real()<zero)
      {
        sq.real(zero);
        sq.imag(sqrt(-disk.real()));
      }
      else
        sq = (gmp_complex)sqrt(disk.real());
    }
    else
      sq = sqrt(disk);
    *r[k+1] = sq - h1;
    sq += h1;
    *r[k] = (gmp_complex)0.0-sq;
    if(sq.imag().isZero())
    {
      k = j;
      j++;
    }
    else
    {
      j = k;
      k--;
    }
  }
  else
  {
    if (((*a[1]).real().isZero()) && ((*a[1]).imag().isZero()))
    {
      WerrorS("precision lost, try again with higher precision");
    }
    else
    {
      *r[k]= (gmp_complex)0.0-(*a[0] / *a[1]);
      if(r[k]->imag().isZero())
        j++;
      else
        k--;
    }
  }
}

void rootContainer::sortroots(gmp_complex **ro, int r, int c, bool isf)
{
  int j;

  for (j=0; j<r; j++) // the real roots
    sortre(ro, j, r, 1);
  if (c>=tdg) return;
  if (isf)
  {
    for (j=c; j+2<tdg; j+=2) // the complex roots for a real poly
      sortre(ro, j, tdg-1, 2);
  }
  else
  {
    for (j=c; j+1<tdg; j++) // the complex roots for a general poly
      sortre(ro, j, tdg-1, 1);
  }
}

void rootContainer::sortre(gmp_complex **r, int l, int u, int inc)
{
  int pos,i;
  gmp_complex *x,*y;

  pos = l;
  x = r[pos];
  for (i=l+inc; i<=u; i+=inc)
  {
    if (r[i]->real()<x->real())
    {
      pos = i;
      x = r[pos];
    }
  }
  if (pos>l)
  {
    if (inc==1)
    {
      for (i=pos; i>l; i--)
        r[i] = r[i-1];
      r[l] = x;
    }
    else
    {
      y = r[pos+1];
      for (i=pos+1; i+1>l; i--)
        r[i] = r[i-2];
      if (x->imag()>y->imag())
      {
        r[l] = x;
        r[l+1] = y;
      }
      else
      {
        r[l] = y;
        r[l+1] = x;
      }
    }
  }
  else if ((inc==2)&&(x->imag()<r[l+1]->imag()))
  {
    r[l] = r[l+1];
    r[l+1] = x;
  }
}

void rootContainer::computefx(gmp_complex **a, gmp_complex x, int m,
                  gmp_complex &f0, gmp_complex &f1, gmp_complex &f2,
                  gmp_float &ex, gmp_float &ef)
{
  int k;

  f0= *a[m];
  ef= abs(f0);
  f1= gmp_complex(0.0);
  f2= f1;
  ex= abs(x);

  for ( k= m-1; k >= 0; k-- )
  {
    f2 = ( x * f2 ) + f1;
    f1 = ( x * f1 ) + f0;
    f0 = ( x * f0 ) + *a[k];
    ef = abs( f0 ) + ( ex * ef );
  }
}

void rootContainer::computegx(gmp_complex **a, gmp_complex x, int m,
                  gmp_complex &f0, gmp_complex &f1, gmp_complex &f2,
                  gmp_float &ex, gmp_float &ef)
{
  int k;

  f0= *a[0];
  ef= abs(f0);
  f1= gmp_complex(0.0);
  f2= f1;
  ex= abs(x);

  for ( k= 1; k <= m; k++ )
  {
    f2 = ( x * f2 ) + f1;
    f1 = ( x * f1 ) + f0;
    f0 = ( x * f0 ) + *a[k];
    ef = abs( f0 ) + ( ex * ef );
  }
}

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
    if ( !roots[i]->solver( howclean ) )
    {
      found_roots= false;
      return;
    }
  // find roots of polys given by coeffs in mu
  mc= mu[0]->getAnzElems();
  for ( i= 0; i < mc; i++ )
    if ( ! mu[i]->solver( howclean ) )
    {
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

  for ( xkoord= 0; xkoord < anzm; xkoord++ ) {    // für x1,x2, x1,x2,x3, x1,x2,...,xn
    gmp_float mprec(1.0/pow(10.0,(int)(gmp_output_digits/3)));
    for ( r= 0; r < anzr; r++ ) {                 // für jede Nullstelle
      // (x1-koordinate) * evp[1] + (x2-koordinate) * evp[2] +
      //                                  ... + (xkoord-koordinate) * evp[xkoord]
      tmp= gmp_complex();
      for ( xk =0; xk <= xkoord; xk++ )
      {
        tmp -= (*roots[xk])[r] * mu[xkoord]->evPointCoord(xk+1); //xk+1
      }
      found= false;
      do { // while not found
        for ( rtest= r; rtest < anzr; rtest++ ) {   // für jede Nullstelle
           zwerg = tmp - (*roots[xk])[rtest] * mu[xkoord]->evPointCoord(xk+1); // xk+1, xkoord+2
          for ( mtest= 0; mtest < anzr; mtest++ )
          {
            //          if ( tmp == (*mu[xkoord])[mtest] )
            //          {
            if ( ((zwerg.real() <= (*mu[xkoord])[mtest].real() + mprec) &&
                  (zwerg.real() >= (*mu[xkoord])[mtest].real() - mprec)) &&
                 ((zwerg.imag() <= (*mu[xkoord])[mtest].imag() + mprec) &&
                  (zwerg.imag() >= (*mu[xkoord])[mtest].imag() - mprec)) )
            {
              roots[xk]->swapRoots( r, rtest );
              found= true;
              break;
            }
          }
        } // rtest
        if (!found)
        {
          WarnS("rootArranger::arrange: precision lost");
          mprec*=10;
        }
      } while(!found);
#if 0
      if ( !found )
      {
        Warn("rootArranger::arrange: No match? coord %d, root %d.",xkoord,r);
//#ifdef mprDEBUG_PROT
        WarnS("One of these ...");
        for ( rtest= r; rtest < anzr; rtest++ )
        {
          tmp= gmp_complex();
          for ( xk =0; xk <= xkoord; xk++ )
          {
            tmp-= (*roots[xk])[r] * mu[xkoord]->evPointCoord(xk+1);
          }
          tmp-= (*roots[xk])[rtest] * mu[xkoord]->evPointCoord(xk+1); // xkoord+2
          Warn("  %s",complexToStr(tmp,gmp_output_digits+1),rtest);
        }
        WarnS(" ... must match to one of these:");
        for ( mtest= 0; mtest < anzr; mtest++ )
        {
          Warn("                  %s",complexToStr((*mu[xkoord])[mtest],gmp_output_digits+1));
        }
//#endif
      }
#endif
    } // r
  } // xkoord
}
//<-

//-----------------------------------------------------------------------------
//-------------- simplex ----- ------------------------------------------------
//-----------------------------------------------------------------------------

//  #ifdef mprDEBUG_PROT
//  #define error(a) a
//  #else
//  #define error(a)
//  #endif

#define error(a) a

#define MAXPOINTS      1000

//-> simplex::*
//
simplex::simplex( int rows, int cols )
   : LiPM_cols(cols), LiPM_rows(rows)
{
  int i;

  LiPM_rows=LiPM_rows+3;
  LiPM_cols=LiPM_cols+2;

  LiPM = (mprfloat **)omAlloc( LiPM_rows * sizeof(mprfloat *) );  // LP matrix
  for( i= 0; i < LiPM_rows; i++ )
  {
    // Mem must be allocated aligned, also for type double!
    LiPM[i] = (mprfloat *)omAlloc0Aligned( LiPM_cols * sizeof(mprfloat) );
  }

  iposv = (int *)omAlloc0( 2*LiPM_rows*sizeof(int) );
  izrov = (int *)omAlloc0( 2*LiPM_rows*sizeof(int) );

  m=n=m1=m2=m3=icase=0;

#ifdef mprDEBUG_ALL
  Print("LiPM size: %d, %d\n",LiPM_rows,LiPM_cols);
#endif
}

simplex::~simplex()
{
  // clean up
  int i;
  for( i= 0; i < LiPM_rows; i++ )
  {
    omFreeSize( (void *) LiPM[i], LiPM_cols * sizeof(mprfloat) );
  }
  omFreeSize( (void *) LiPM, LiPM_rows * sizeof(mprfloat *) );

  omFreeSize( (void *) iposv, 2*LiPM_rows*sizeof(int) );
  omFreeSize( (void *) izrov, 2*LiPM_rows*sizeof(int) );
}

BOOLEAN simplex::mapFromMatrix( matrix mm )
{
  int i,j;
//    if ( MATROWS( m ) > LiPM_rows ||  MATCOLS( m ) > LiPM_cols ) {
//      WarnS("");
//      return FALSE;
//    }

  number coef;
  for ( i= 1; i <= MATROWS( mm ); i++ )
  {
     for ( j= 1; j <= MATCOLS( mm ); j++ )
     {
        if ( MATELEM(mm,i,j) != NULL )
        {
           coef= pGetCoeff( MATELEM(mm,i,j) );
           if ( coef != NULL && !nIsZero(coef) )
              LiPM[i][j]= (double)(*(gmp_float*)coef);
           //#ifdef mpr_DEBUG_PROT
           //Print("%f ",LiPM[i][j]);
           //#endif
        }
     }
     //     PrintLn();
  }

  return TRUE;
}

matrix simplex::mapToMatrix( matrix mm )
{
  int i,j;
//    if ( MATROWS( mm ) < LiPM_rows-3 ||  MATCOLS( m ) < LiPM_cols-2 ) {
//      WarnS("");
//      return NULL;
//    }

//Print(" %d x %d\n",MATROWS( mm ),MATCOLS( mm ));

  number coef;
  gmp_float * bla;
  for ( i= 1; i <= MATROWS( mm ); i++ )
  {
    for ( j= 1; j <= MATCOLS( mm ); j++ )
    {
       pDelete( &(MATELEM(mm,i,j)) );
       MATELEM(mm,i,j)= NULL;
//Print(" %3.0f ",LiPM[i][j]);
       if ( LiPM[i][j] != 0.0 )
       {
          bla= new gmp_float(LiPM[i][j]);
          coef= (number)bla;
          MATELEM(mm,i,j)= pOne();
          pSetCoeff( MATELEM(mm,i,j), coef );
       }
    }
//PrintLn();
  }

  return mm;
}

intvec * simplex::posvToIV()
{
   int i;
   intvec * iv = new intvec( m );
   for ( i= 1; i <= m; i++ )
   {
      IMATELEM(*iv,i,1)= iposv[i];
   }
   return iv;
}

intvec * simplex::zrovToIV()
{
   int i;
   intvec * iv = new intvec( n );
   for ( i= 1; i <= n; i++ )
   {
      IMATELEM(*iv,i,1)= izrov[i];
   }
   return iv;
}

void simplex::compute()
{
  int i,ip,ir,is,k,kh,kp,m12,nl1,nl2;
  int *l1,*l2,*l3;
  mprfloat q1, bmax;

  if ( m != (m1+m2+m3) )
  {
    // error: bad input
    error(WarnS("simplex::compute: Bad input constraint counts!");)
    icase=-2;
    return;
  }

  l1= (int *) omAlloc0( (n+1) * sizeof(int) );
  l2= (int *) omAlloc0( (m+1) * sizeof(int) );
  l3= (int *) omAlloc0( (m+1) * sizeof(int) );

  nl1= n;
  for ( k=1; k<=n; k++ ) l1[k]=izrov[k]=k;
  nl2=m;
  for ( i=1; i<=m; i++ )
  {
    if ( LiPM[i+1][1] < 0.0 )
    {
      // error: bad input
      error(WarnS("simplex::compute: Bad input tableau!");)
      error(Warn("simplex::compute: in input Matrix row %d, column 1, value %f",i+1,LiPM[i+1][1]);)
      icase=-2;
      // free mem l1,l2,l3;
      omFreeSize( (void *) l3, (m+1) * sizeof(int) );
      omFreeSize( (void *) l2, (m+1) * sizeof(int) );
      omFreeSize( (void *) l1, (n+1) * sizeof(int) );
      return;
    }
    l2[i]= i;
    iposv[i]= n+i;
  }
  for ( i=1; i<=m2; i++) l3[i]= 1;
  ir= 0;
  if (m2+m3)
  {
    ir=1;
    for ( k=1; k <= (n+1); k++ )
    {
      q1=0.0;
      for ( i=m1+1; i <= m; i++ ) q1+= LiPM[i+1][k];
      LiPM[m+2][k]= -q1;
    }

    do
    {
      simp1(LiPM,m+1,l1,nl1,0,&kp,&bmax);
      if ( bmax <= SIMPLEX_EPS && LiPM[m+2][1] < -SIMPLEX_EPS )
      {
        icase= -1; // no solution found
        // free mem l1,l2,l3;
        omFreeSize( (void *) l3, (m+1) * sizeof(int) );
        omFreeSize( (void *) l2, (m+1) * sizeof(int) );
        omFreeSize( (void *) l1, (n+1) * sizeof(int) );
        return;
      }
      else if ( bmax <= SIMPLEX_EPS && LiPM[m+2][1] <= SIMPLEX_EPS )
      {
        m12= m1+m2+1;
        if ( m12 <= m )
        {
          for ( ip= m12; ip <= m; ip++ )
          {
            if ( iposv[ip] == (ip+n) )
            {
              simp1(LiPM,ip,l1,nl1,1,&kp,&bmax);
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
                LiPM[i+1][k] = -(LiPM[i+1][k]);
        break;
      }
      //#if DEBUG
      //print_bmat( a, m+2, n+3);
      //#endif
      simp2(LiPM,n,l2,nl2,&ip,kp,&q1);
      if ( ip == 0 )
      {
        icase = -1; // no solution found
        // free mem l1,l2,l3;
        omFreeSize( (void *) l3, (m+1) * sizeof(int) );
        omFreeSize( (void *) l2, (m+1) * sizeof(int) );
        omFreeSize( (void *) l1, (n+1) * sizeof(int) );
        return;
      }
    one: simp3(LiPM,m+1,n,ip,kp);
      // #if DEBUG
      // print_bmat(a,m+2,n+3);
      // #endif
      if ( iposv[ip] >= (n+m1+m2+1))
      {
        for ( k= 1; k <= nl1; k++ )
          if ( l1[k] == kp ) break;
        --nl1;
        for ( is=k; is <= nl1; is++ ) l1[is]= l1[is+1];
        ++(LiPM[m+2][kp+1]);
        for ( i= 1; i <= m+2; i++ ) LiPM[i][kp+1] = -(LiPM[i][kp+1]);
      }
      else
      {
        if ( iposv[ip] >= (n+m1+1) )
        {
          kh= iposv[ip]-m1-n;
          if ( l3[kh] )
          {
            l3[kh]= 0;
            ++(LiPM[m+2][kp+1]);
            for ( i=1; i<= m+2; i++ )
              LiPM[i][kp+1] = -(LiPM[i][kp+1]);
          }
        }
      }
      is= izrov[kp];
      izrov[kp]= iposv[ip];
      iposv[ip]= is;
    } while (ir);
  }
  /* end of phase 1, have feasible sol, now optimize it */
  loop
  {
    // #if DEBUG
    // print_bmat( a, m+1, n+5);
    // #endif
    simp1(LiPM,0,l1,nl1,0,&kp,&bmax);
    if (bmax <= /*SIMPLEX_EPS*/0.0)
    {
      icase=0; // finite solution found
      // free mem l1,l2,l3
      omFreeSize( (void *) l3, (m+1) * sizeof(int) );
      omFreeSize( (void *) l2, (m+1) * sizeof(int) );
      omFreeSize( (void *) l1, (n+1) * sizeof(int) );
      return;
    }
    simp2(LiPM,n,l2,nl2,&ip,kp,&q1);
    if (ip == 0)
    {
      //printf("Unbounded:");
      // #if DEBUG
      //       print_bmat( a, m+1, n+1);
      // #endif
      icase=1;                /* unbounded */
      // free mem
      omFreeSize( (void *) l3, (m+1) * sizeof(int) );
      omFreeSize( (void *) l2, (m+1) * sizeof(int) );
      omFreeSize( (void *) l1, (n+1) * sizeof(int) );
      return;
    }
    simp3(LiPM,m,n,ip,kp);
    is= izrov[kp];
    izrov[kp]= iposv[ip];
    iposv[ip]= is;
  }/*for ;;*/
}

void simplex::simp1( mprfloat **a, int mm, int ll[], int nll, int iabf, int *kp, mprfloat *bmax )
{
  int k;
  mprfloat test;

  if( nll <= 0)
  {                        /* init'tion: fixed */
    *bmax = 0.0;
    return;
  }
  *kp=ll[1];
  *bmax=a[mm+1][*kp+1];
  for (k=2;k<=nll;k++)
  {
    if (iabf == 0)
    {
      test=a[mm+1][ll[k]+1]-(*bmax);
      if (test > 0.0)
      {
        *bmax=a[mm+1][ll[k]+1];
        *kp=ll[k];
      }
    }
    else
    {                        /* abs values: have fixed it */
      test=fabs(a[mm+1][ll[k]+1])-fabs(*bmax);
      if (test > 0.0)
      {
        *bmax=a[mm+1][ll[k]+1];
        *kp=ll[k];
      }
    }
  }
}

void simplex::simp2( mprfloat **a, int nn, int l2[], int nl2, int *ip, int kp, mprfloat *q1 )
{
  int k,ii,i;
  mprfloat qp,q0,q;

  *ip= 0;
  for ( i=1; i <= nl2; i++ )
  {
    if ( a[l2[i]+1][kp+1] < -SIMPLEX_EPS )
    {
      *q1= -a[l2[i]+1][1] / a[l2[i]+1][kp+1];
      *ip= l2[i];
      for ( i= i+1; i <= nl2; i++ )
      {
        ii= l2[i];
        if (a[ii+1][kp+1] < -SIMPLEX_EPS)
        {
          q= -a[ii+1][1] / a[ii+1][kp+1];
          if (q - *q1 < -SIMPLEX_EPS)
          {
            *ip=ii;
            *q1=q;
          }
          else if (q - *q1 < SIMPLEX_EPS)
          {
            for ( k=1; k<= nn; k++ )
            {
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

void simplex::simp3( mprfloat **a, int i1, int k1, int ip, int kp )
{
  int kk,ii;
  mprfloat piv;

  piv= 1.0 / a[ip+1][kp+1];
  for ( ii=1; ii <= i1+1; ii++ )
  {
    if ( ii -1 != ip )
    {
      a[ii][kp+1] *= piv;
      for ( kk=1; kk <= k1+1; kk++ )
        if ( kk-1 != kp )
          a[ii][kk] -= a[ip+1][kk] * a[ii][kp+1];
    }
  }
  for ( kk=1; kk<= k1+1; kk++ )
    if ( kk-1 != kp ) a[ip+1][kk] *= -piv;
  a[ip+1][kp+1]= piv;
}
//<-

//-----------------------------------------------------------------------------

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***

