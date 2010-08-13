#include "canonicalform.h"
#ifdef HAVE_BIFAC
#include "lgs.h"
#include "bifacConfig.h"

#define BIFAC_BASIS_OF_G_CHECK	1
void Reduce( bool );
CanonicalForm Bigcd( const CanonicalForm& f, const CanonicalForm& g);

CanonicalForm MyContent( const CanonicalForm& F, const Variable& x);
CFFList Mysqrfree( const CanonicalForm& F );


CanonicalForm MyGCDmod( const CanonicalForm & a,const CanonicalForm & b);
CFFList RelFactorize(const CanonicalForm & h);

//====== global definitions ===================
Variable x( 'x' );
Variable y( 'y' );
Variable z( 'z' );
Variable e( 'e' );


///////////////////////////////////////////////////////
// Class for storing polynomials as vectors.
// Enables fast access to a certain degree.
///////////////////////////////////////////////////////

//==================================================
class PolyVector 
//==================================================
{
public:
  PolyVector ( void ){
    m = -1;
  }
  virtual ~PolyVector( void ){
    if( m!= -1) delete[] value;
  }
  void init (CanonicalForm f){
    if( f.level()<0 )
    {
      m = 0;
      n = 0;
      value = new CanonicalForm[1];
      value[0] = f;
    }
    else
    {
      m = degree(f,x);
      n = degree(f,y);
      ASSERT( m>0 || n>0, "Input is not a polynomial");
      int correction = 1;  // univariate polynomials
      if( n==0) correction = n+1;
      
      value = new CanonicalForm[m*(n+1)+n+1];
      for(int i=0; i<=m*(n+1)+n; i++) value[i]=0;

      
      for ( CFIterator i = f; i.hasTerms(); i++ ) { 
	for ( CFIterator j = i.coeff(); j.hasTerms(); j++ ){
		if( i.coeff().mvar().level()< 0 ){ 
			value[ 0*(n+1) + i.exp()*correction ] = j.coeff();}
	    else{ 
			value[ j.exp()*(n+1) + i.exp()*correction ] = j.coeff();}}}
    }
  }

  void  push(int mm, int nn, CanonicalForm v){
    ASSERT( 0<=mm<=m && 0<=nn<=n, "Wrong Index in PolyVector");
    value[mm*(n+1)+nn] = v;
  }
  CanonicalForm get(int mm, int nn){

    if( 0<=mm && mm<=m && 0<=nn && nn<=n )
      return value[mm*(n+1)+nn];
    else
      return 0;
  }
#ifndef NOSTREAMIO
  friend OSTREAM & operator<< ( OSTREAM & s, const PolyVector& V ){
    for (int i=0;i<=V.m;i++)
    {
      s << "[";
      for (int j=0;j<=V.n;j++)
	s << V.value[i*(V.n+1)+j] << ", ";
      s << "]\n";
    }
    return s;
  }
#endif /* NOSTREAMIO */


private:
  int            m;       // Degree in x
  int            n;       // Degree in y
  CanonicalForm* value;   // Value: index = m*(n+1)+n
};
////////// END of PolyVector ///////////////////////////




/////////////////////////////////////////////////////////
//
//  Default class declarations
//
/////////////////////////////////////////////////////////



//--<>---------------------------------
BIFAC::BIFAC( void )// KONSTRUKTOR
//--<>---------------------------------
{
}

//--<>---------------------------------
BIFAC::~BIFAC( void )// DESTRUKTOR
//--<>---------------------------------
{
} 


/////////////////////////////////////////////////////////
//
//  Auxiliary functions
//
/////////////////////////////////////////////////////////

//  //--<>---------------------------------
//  void BIFAC::matrix_drucken( CFMatrix M )
//  //--<>---------------------------------
//  {
//    int i,j;
//    char* name="matrix.ppm";

//    // === Datei löschen ===
  
//    ofstream* aus = new ofstream(name, ios::out); 
//    delete aus;


//    // === Jetzt immer nur anhängen ===

//    aus  = new ofstream(name, ios::app); 
//    *aus << "// Zeilen Spalten\n"
//         << "// x-Koord. y-Koord.  Wert\n";

//    *aus  << M.rows()  << " " << M.columns() << endl;


//    // === Noch nicht bearbeitet Teile ===

//    for( i=0; i<M.rows(); i++)
//      for( j=0; j<M.columns(); j++)
//        *aus << i << " " << j << " " << M(i+1,j+1) << endl;;
//    delete aus;
//  }   

//=======================================================
void  BIFAC::passedTime()
//=======================================================
{
	;
}


//=======================================================
long int  BIFAC::anz_terme(  CanonicalForm & f )
//=======================================================
{
  long int z=0;

  for ( CFIterator i = f; i.hasTerms(); i++ )
    for ( CFIterator j = i.coeff(); j.hasTerms(); j++ ) 
      z++;
  return( z );
}
 
//=======================================================
void BIFAC::biGanzMachen(  CanonicalForm & f )
//=======================================================
{
  CanonicalForm ggT;
  bool init = false;
  Off( SW_RATIONAL );

  for ( CFIterator i = f; i.hasTerms(); i++ )
    for ( CFIterator j = i.coeff(); j.hasTerms(); j++ ) 
    {
      if( !init )
      { 
	ggT = j.coeff();
	init = true;
      }
      else           
	ggT = gcd(j.coeff(), ggT);
    }
  f /= ggT;
  On( SW_RATIONAL );
}

//=======================================================
void  BIFAC::biNormieren( CanonicalForm & f ) 
//=======================================================
{
  if ( getCharacteristic() == 0 )
  {
    for ( CFIterator i = f; i.hasTerms(); i++ )
      for ( CFIterator j = i.coeff(); j.hasTerms(); j++ ) 
	if( j.coeff().den() != 1 )
	{
	  f  *= j.coeff().den();
	  biNormieren( f );
	}
    biGanzMachen( f );
  } 
  else
  {
    f /= LC(f);
  }
}


//=======================================================
//   * Convert the basis vectors of G into polynomials
//   * Validate the solutions
//=======================================================
CFList BIFAC::matrix2basis(CFMatrix A, int dim, int m, int n, CanonicalForm f)
//=======================================================
{
  Variable x('x'), y('y');
  int i,j,k;
  CanonicalForm g,h,ff;
  CFList Lg, Lh;

  // === Construction of the 'g's ====
  for(k=1; k<=dim; k++)
  {
    g=0;
    for(i=0; i<=m-1; i++)
      for(j=0; j<=n; j++)
	g += A(k, i*(n+1)+j+1)* power(x,i) * power(y,j);
    Lg.append(g);
  }

  ///////////  START VALIDATION ////////////////////////////////////
  if (BIFAC_BASIS_OF_G_CHECK)
  {

    // === Construction of the 'h's ====
    for(k=1; k<=dim; k++)
    {
      h=0;
      for(i=0; i<=m; i++)
	for(j=0; j<n; j++)
	  h += A(k, i*n+j+1 +m*(n+1))* power(x,i) * power(y,j);
      Lh.append(h);
    }
    
    // === Is the solution correct? ===
    CFListIterator itg=Lg;
    CFListIterator ith=Lh;
    for( ; itg.hasItem(); itg++, ith++)
    {
      g = itg.getItem();
      h = ith.getItem();
      ff = f*(deriv(g,y)-deriv(h,x)) +h*deriv(f,x) -g*deriv(f,y);
      if( !ff.isZero()) {  
      #ifndef NOSTREAMIO
	AUSGABE_ERR("* Falsche Polynome!"); 
	exit (1); 
      #else
        printf("wrong polys\n");
        break;
      #endif
      }
    } 
  }
  ///////////  END VALIDATION ////////////////////////////////////

  return (Lg);
}

/////////////////////////////////////////////////////////
//
//  Main functions
//
/////////////////////////////////////////////////////////

//=======================================================
//   * Create the matrix belonging to G
//   * Compute a basis of the kernel
//=======================================================
CFList BIFAC::basisOfG(CanonicalForm f)
//=======================================================
{


  int m = degree(f,x);
  int n = degree(f,y);
  int r,s, ii,jj;
  

  // ======= Creation of the system of linear equations for G =============
  int rows    = 4*m*n;
  int columns = m*(n+1) + (m+1)*n;

  CFMatrix M(rows, columns); // Remember: The first index is (1,1) -- not (0,0)!
  
  for ( CFIterator i = f; i.hasTerms(); i++ )  // All coeffizients of y
  {
    for ( CFIterator j = i.coeff(); j.hasTerms(); j++ )  // All coeffizients of x
    {
      r = j.exp();  // x^r
      s = i.exp();  // y^s

      // Now we regard g_{ii,jj)
      for( ii=0; ii<m; ii++)
	for( jj=0; jj<=n; jj++)
	{
	  if(  s>= 1) M( (r+ii)*2*n +(jj+s-1)+1, ii*(n+1)+jj +1) += -j.coeff() * s;
	  if( jj>= 1) M( (r+ii)*2*n +(jj+s-1)+1, ii*(n+1)+jj +1) +=  j.coeff() * jj;
	}

      // Now we regard h_{ii,jj}
      for( ii=0; ii<=m; ii++)
	for( jj=0; jj<n; jj++)
	{
	  if(  r>= 1) M( (r+ii-1)*2*n +(jj+s)+1, (ii*n)+jj +m*(n+1) +1) += j.coeff() * r;
	  if( ii>= 1) M( (r+ii-1)*2*n +(jj+s)+1, (ii*n) +jj +m*(n+1) +1) +=  -j.coeff() * ii;
	}
    }
  }
  // ========= Solving the  system of linear equations for G =============

//  matrix_drucken(M); // **********************************

  LGS L(rows,columns);

  CFMatrix Z(1,columns);
  for( ii=1; ii<=rows; ii++)
  {
    for( jj=1; jj<=columns; jj++)
      Z(1,jj) = M(ii,jj);  // Copy the ii-th row
    L.new_row(Z);
  }

  if( L.corank() == 1 )
  {
    CFList Lg;
    Lg.append(f);
    return(Lg);
  }
//  L.print();
  CFMatrix basis = L.GetKernelBasis();

  // ============= TEST AUF KORREKTHEIT /start) ====
  CanonicalForm tmp;
  for(int k=1; k<= L.corank(); k++)
    for(int i=1; i<=rows; i++)
    {
      tmp =0;
      for(int j=1; j<=columns; j++)
	tmp += M(i,j) * basis(k,j);
      if( tmp!= 0) {
	exit(17);
      }
    }
  // ============= TEST AUF KORREKTHEIT (ende) ====
  return (  matrix2basis( basis, L.corank(), m,n,f ) );
}

//=======================================================
//   Compute a   r x r - matrix A=(a_ij) for 
//     gg_i = SUM a_ij * g_j * f_x (mod f)
//  Return a list consisting of 
//    r x (r+1) Matrix A
//    the last columns contains only the indices of the 
//    first r linear independent lines
// REMARK: this is used by BIFAC::createEg but NOT by createEgUni!!
//=======================================================
CFMatrix BIFAC::createA (CFList G, CanonicalForm f)
//=======================================================
{
  // === Declarations ===
  int m,n;
  int i,j,e;
  int r = G.length();  // number of factors

  LGS       L(r,r,true);      
//  LGS       L(r,r);      
  CFMatrix  Z(1,r);
  CFMatrix  A(r,r+2);  // the last two column contain the bi-degree

  CanonicalForm fx   = deriv(f,x);
  PolyVector*   gifx = new  PolyVector[r];

  // === Convert polynomials into vectors ===
  i=0;
  CanonicalForm q;

  for( CFListIterator it=G; it.hasItem(); it++, i++){ 

    gifx[i].init( (it.getItem()*fx)%f );
  }

  // === Search linear independent lines ===

  e=1; // row number of A
  n=0; // 
  m=0; //
  while (L.rank() != r )
  {
    for(j=0;j<r;j++)
      Z(1,j+1) = gifx[j].get(m,n);
    if( L.new_row(Z,0) )  // linear independent row?
    {
      ASSERT( e<=r, "Wrong index in matrix");
      A(e,r+1) = m;      // Degree in x
      A(e,r+2) = n;      // Degree in y
      e++;
    }
    if (m>n) n++;
    else     { m++; n=0; }
  }
  L.print();

  L.inverse(A);

  // === Clean up ==
  delete[] gifx;

  return A;
}

//=======================================================
CanonicalForm BIFAC::create_g (CFList G)
//=======================================================
{
  CanonicalForm g = 0;
  int i = 0;

  int r    = G.length();  // number of factors
  float SS =  10*( r*(r-1) / ( 2*( (100- (float) EgSeparable)/100)) );
  int S    = (int) SS +1;

  IntRandom RANDOM(S);

  int*        rand_coeff1 = new int[r];


  //  static for debugging
  // rand_coeff1[0] = 12; rand_coeff1[1] = 91; rand_coeff1[2] = 42;

  for( CFListIterator it=G; it.hasItem(); it++, i++)
    {
      rand_coeff1[i] =  RANDOM.generate().intval();

      g += rand_coeff1[i]  * it.getItem();
    }
  
  delete[] rand_coeff1;

  return g;
}

/////////////////////////////////////////////////////////////
// This subroutine creates the polynomials Eg(x) and g
// by using the 'bivariate' methode'.
// REMARK: There is a 'univariate methode' as well
//         which ought to be faster!
////////////////////////////////////////////////////////////
//=======================================================
CFList BIFAC::createEg (CFList G, CanonicalForm f)
//=======================================================
{


  CFMatrix NEU = createA(G,f);
//  passedTime();

  bool suitable1 = false; // Is Eg by chance unsuitable?
  bool suitable2 = false;  // Is on of g*g_i or g_i*f_x zero? 

  // === (0) Preparation ===
  CanonicalForm g;
  CanonicalForm Eg;
  CanonicalForm fx = deriv(f,x);

  int i,j,e;
  int r = G.length();  // number of factors
//    float SS =  ( r*(r-1) / ( 2*( (100- (float) EgSeparable)/100)) );
//    int S = (int) SS +1;

//    IntRandom RANDOM(S);
//    int*        rand_coeff = new int[r];
  CFMatrix  A(r,r);
  CanonicalForm*     gi  = new CanonicalForm[r];
  CanonicalForm*    ggi  = new CanonicalForm[r];
  PolyVector*     v_ggi  = new PolyVector   [r];



  i=0;
  for( CFListIterator it=G; it.hasItem(); it++, i++)
    gi[i] =  it.getItem();

  while ( !suitable1 )
  {

     suitable2 = false; 
    // === (1) Creating g ===
    while ( !suitable2 )
    {
//        i=0;
//        g=0;
//        for( CFListIterator it=G; it.hasItem(); it++, i++)
//        {
//  	gi[i] =  it.getItem();
//  	rand_coeff[i] =  RANDOM.generate().intval();
//  	g += rand_coeff[i] * it.getItem();
//        }
      g = create_g( G );
    
      // === (2) Computing g_i * g ===
	  //
      for(i=0; i<r; i++){ 

	  ggi[i]  = (g*gi[i])%f;   // seite 10
      }
      
      // ===  Check if all polynomials are <> 0  ===
      suitable2 = true;    // It should be fine, but ...
      if( g.isZero() ) 
	suitable2 = false;
//        else
//  	for(i=0; i<r; i++)
//  	  if(  ggi[i].isZero() )
//  	    suitable2 = false;

    } // end of  ´while ( !suitable2 )´
    
    // === (3) Computing Eg(x) ===
   
    for(i=0;i<r;i++)  // Get Polynomials as vectors
      v_ggi[i].init(ggi[i]);

    // Matrix A
    for(i=1; i<=r; i++) 
      for( j=1; j<=r; j++)
      {
	A(i,j) = 0;
	for( e=1; e<=r; e++)
	{


   A(i,j) += ( NEU(j,e ) * v_ggi[i-1].get(NEU(e,r+1).intval(),(NEU(e,r+2).intval() )));


//

	}
      }

    for(j=1; j<=r; j++)
      A(j,j) -= x;
    Eg = determinant(A,r);
//    exit(1);
    // === (4) Is Eg(x) suitable? ===
    if( MyGCDmod(Eg, deriv(Eg,x)) == 1 )
      suitable1 = true;
    else
    {
    }
  } // end of  ´while ( !suitable1 )´

  // Delete trash



  delete[] v_ggi;
  delete[] gi;
  delete[] ggi;
  //  delete[] rand_coeff;

  CFList LL;
  LL.append(Eg);
  LL.append(g);
  return (LL);
}
//  /////////////////////////////////////////////////////////////
//  // It is possible to take univariate polynomials
//  // with y:=c for a suitable c.
//  // c is suitable iff gcd( f(x,c), f_x(x,c)) = 1.
//  ////////////////////////////////////////////////////////////
//
//=======================================================
CFList BIFAC::createEgUni (CFList G, CanonicalForm f)
//=======================================================
{

  int i,ii,k;
  CanonicalForm ff, ffx,g, gg, Eg;

  
  bool suitable1 = false;  // Is Eg unsuitable?
  bool suitable2 = false;  // Is on of g*g_i or g_i*f_x zero? 
  bool suitable3 = false;  // Is 'konst' unsuitable?

  // ========================
  // =   (0) Preparation    =
  // ========================
  int konst = 0;
  CanonicalForm fx = deriv(f,x);
  int m = degree(f,x);
  int r = G.length();            // number of factors
  int S =  (int) ((float) ( r*(r-1) / ( 2*( (100- (float) EgSeparable)/100)) )+1);


  int*  rand_coeff      = new int[r];
  CanonicalForm*     gi  = new CanonicalForm[r];
  CanonicalForm*    ggi  = new CanonicalForm[r];

  CFMatrix  A(r,r);     // We have to find the matrix A,
  CFMatrix  Z(1,r);     // `Vector` for data transportation
  CFMatrix  AA(m,r);    // but first we generate AA.
  CFMatrix  AI(r,r+1);  // 
  LGS       L(r,r,true);  
  IntRandom RANDOM(S);


  // ==========================================================
  // =  (1) Find a suitable constant to make bivariate        =
  // =      polynomials univariate. Try the following numbers =
  // =      0, 1, -1, 2, -2, 3,...                            =
  // ==========================================================

  while ( !suitable3 )
  {
    ff  = f(konst,'y');
    ffx = fx(konst,'y');
    
    if( gcd(ff, ffx) == 1)
      suitable3 = true;
    else
    {
      konst *= -1;
      if( konst >= 0 )
	konst++;
    }
  }


  // ===============================================
  // =    (2) Make g_i univariate                  =
  // ===============================================
  i=0;
  for( CFListIterator it=G; it.hasItem(); it++, i++)
  {
    gi[i] =  it.getItem()(konst,'y'); 
  }

  // ===============================================
  // =   (3) Compute the matrices 'AA' and 'AI'    =
  // ===============================================
  

  for( i=0; i<r; i++) // First store all coeffizients in AA.
  {
    ggi[i] = (gi[i]*ffx)%ff;   // now we have degree < m.
    //biNormieren(ggi[i]);
    for ( CFIterator j = ggi[i]; j.hasTerms(); j++ )
      AA( j.exp()+1, i+1) = j.coeff();  
  }


  // Now find the lin. indep. rows.
  i  = 1;
  ii = 1; // row number of A
  while (L.rank() != r )
  {
    ASSERT( i<=m, "Too few linear independent rows!");

    for (k=1; k<=r; k++) 
      Z(1,k) =  AA(i,k);
    if( L.new_row(Z,0) )  // linear independent row?
    {
      ASSERT( ii<=r, "Wrong index in matrix");
      AI(ii,r+1) = i;      // Degree in x
      ii++;
    }
    i++;
    L.print();
  }
  L.inverse(AI);


  // ==============================================
  // =   (4) Big loop to find a suitable 'Eg(x)   =
  // ==============================================
   
  while ( !suitable1 )    // Is Eg(x) suitable? -> Check at the end of this procedure!
  {
    suitable2 = false;   // In case we need a second loop

    // ================================================
    // =    (4a) Find a suitable 'g'                  =
    // ================================================
//    rand_coeff[0] = 0;
//    rand_coeff[1] = 4;
    

    while ( !suitable2 )
    {
      // === (i) Creating g ===
      i=0;
      g=0;
      for( CFListIterator it=G; it.hasItem(); it++, i++)
      {
 	rand_coeff[i] =  RANDOM.generate().intval();
	g += rand_coeff[i] * it.getItem();
      }
      gg = g(konst,'y');   // univariate!
      for(i=0; i<r; i++)  ggi[i] = (gi[i]*gg)%ff; // !! Redefinition of ggi !!

      // ===  (ii) Check if all polynomials are <> 0  ===
      suitable2 = true;    // It should be fine, but ...
      if( gg.isZero() ) 
	suitable2 = false;
//        else
//  	for(i=0; i<r; i++)
//  	  if(  ggi[i].isZero() )
//  	    suitable2 = false;
    } // end of  ´while ( !suitable2 )´
  
//    createRg(g,f);

    // ===============================================
    // =    (b) Compute matrix 'A'                   =
    // ===============================================
    for(i=1; i<=r; i++) 
    {
      for( ii=1; ii<=m; ii++) 
	AA (ii,1) = 0;  // !! Redefinition of AA !!
      for ( CFIterator j = ggi[i-1]; j.hasTerms(); j++ )
	AA( j.exp()+1, 1) = j.coeff();  

      for( ii=1; ii<=r; ii++)
      {
	A(i,ii) = 0;
	for( k=1; k<=r; k++)
	  A(i,ii) += ( AI(ii,k ) *  AA( AI(k, r+1 ).intval(),1) ); 
      }
    }
    for(i=1; i<=r; i++)
      A(i,i) -= x;

    // ===============================================
    // =    (c) Compute Eg(x) and check it           =
    // ===============================================
 
    Eg = determinant(A,r);
    if( gcd(Eg, deriv(Eg,x)) == 1 )
    {
      suitable1 = true;
    }
  } // end of  ´while ( !suitable1 )´

  
  // ==============================================
  // =   (5) Prepare for leaving                  =
  // ==============================================

  delete[] gi;
  delete[] ggi;
  delete[] rand_coeff;
  
  CFList LL;
  LL.append(Eg);
  LL.append(g);

  return (LL);
}
/////////////////////////////////////////////////////////////
// This subroutine creates the polynomials Rg(x)
// which can be used instead of Eg(x).
// No basis of G is neccessary, only one element
////////////////////////////////////////////////////////////
//=======================================================
CFList BIFAC::createRg (CFList G, CanonicalForm f)
//=======================================================
{

//  cerr << "* Was ist wenn g versagt???? -> Ausbauen\n";

  CanonicalForm fx = deriv(f,x);
  CanonicalForm Rg;
  CanonicalForm g = create_g(G);


  // ===============================================
  // =   (1) Find a suitable constant              =
  // ===============================================

  CanonicalForm alpha=1; 

  while(  resultant( f, fx, x)(alpha) == 0 ) 
  {
	//while( resultant( f, fx, x)(alpha).inCoeffDomain() != true ) 
    //alpha +=1;
  }


  // ===============================================
  // =   (2) Find a suitable constant              =
  // ===============================================
  
  Rg = resultant( f(alpha,y), g(alpha,y)-z*fx(alpha,y), x);
  

  CFList LL;
  LL.append(Rg(x,z));
  LL.append(g);
  return (LL);
}
/////////////////////////////////////////////////////////
// Compute the absolute and rational factorization of
// the univariate polynomial 'ff^grad'.
//=======================================================
void BIFAC::unifac (CanonicalForm ff, int grad)
//=======================================================
{

  CFFList factorsUni;
  CFFList factorsAbs;
  CanonicalForm tmp;

  factorsUni = AbsFactorize(ff); 

  for( CFFListIterator l=factorsUni; l.hasItem(); l++)
    if( ! l.getItem().factor().inBaseDomain() )
    {
      gl_RL.append( CFFactor( l.getItem().factor(),l.getItem().exp()*grad) );
    }


}


///////////////////////////////////////////////////////
// Compute the rational factor of f belonging to phi
//=======================================================
CanonicalForm BIFAC::RationalFactor (CanonicalForm phi, CanonicalForm ff, \
				     CanonicalForm fx, CanonicalForm g)
//=======================================================
{

  CanonicalForm h,hh;
//  CanonicalForm fx = deriv(f,x);

  for ( CFIterator it = phi; it.hasTerms(); it++ )
    h += it.coeff() * power(fx,phi.degree()-it.exp())*power(g,it.exp());


  hh = Bigcd(ff,  h);
  
  return(hh);
}
//=======================================================
void BIFAC::RationalFactorizationOnly (CFFList Phis, CanonicalForm f0, CanonicalForm g)
//=======================================================
{
  CanonicalForm h,ff;
  CanonicalForm fx = deriv(f0,x);

  for( CFFListIterator i=Phis; i.hasItem(); i++)
  {
    ASSERT( i.getItem().exp() == 1 , "Wrong factor of Eg"); // degree must be 1
    CanonicalForm phi = i.getItem().factor();
    
    if( ! phi.inBaseDomain())
    {
      h = RationalFactor(phi,f0,fx,g);
      gl_RL.append( CFFactor(h,exponent ));
      ff = f0;
      f0 /= h;
      ASSERT( f0*h==ff, "Wrong factor found");
    }
  }
}

//=======================================================
CFList BIFAC::getAbsoluteFactors (CanonicalForm f1, CanonicalForm phi)
//=======================================================
{
  CanonicalForm fac;
  CanonicalForm root;
  CFList AbsFac;

  CFFList Fac = factorize(phi,e);
  for( CFFListIterator i=Fac; i.hasItem(); i++)
  {
    fac = i.getItem().factor();
    if( taildegree(fac) > 0 )  // case: phi = a * x
      root = 0;
    else
      root = -tailcoeff(fac)/lc(fac);
 
 
    AbsFac.append( f1(root,e) );
    AbsFac.append( i.getItem().exp() * exponent);
    AbsFac.append( phi ); // Polynomial of the field extension
  }
  return AbsFac;
}
//=======================================================
void BIFAC::AbsoluteFactorization (CFFList Phis, CanonicalForm ff, CanonicalForm g)
//=======================================================
{

  int ii;
  if( getCharacteristic() == 0 )
  {
    //cerr << "* Charcteristic 0 is not yet implemented! => Aborting!\n";
    exit(1);
  }


  CFList  AbsFac;
  CanonicalForm phi;
  CanonicalForm h, h_abs, h_res, h_rat;
  CanonicalForm fx = deriv(ff,x);
  

  for( CFFListIterator i=Phis; i.hasItem(); i++)
  {
    ASSERT( i.getItem().exp() == 1 , "Wrong factor of Eg"); // degree must be 1
    phi = i.getItem().factor();
    
    if( ! phi.inBaseDomain())
    {

      // === Case 1:  phi has degree 1 ===
      if( phi.degree() == 1 )
      {
	if( taildegree(phi) > 0 )  // case: phi = a * x
	  h = gcd( ff,g );
	else                       // case: phi = a * x + c
	{
	  h =  gcd( ff, g+tailcoeff(phi)/lc(phi)*fx);
	}
	
	//biNormieren( h );
	gl_AL.append(h); // Factor of degree 1 
 	gl_AL.append(exponent); // Multiplicity (exponent)
	gl_AL.append(0); // No field extension
      } else
      {
	// === Case 2:  phi has degree > 1 ===
	e=rootOf(phi, 'e');
	h =  gcd( ff, g-e*fx);
	//biNormieren( h );

	AbsFac = getAbsoluteFactors(h, phi);
	for( CFListIterator l=AbsFac; l.hasItem(); l++)
	  gl_AL.append( l.getItem() );


	// ===  (1) Get the rational factor by multi-  ===
	// ===      plication of the absolute factor.  ===
	h_abs=1;
	ii = 0;
	
	for( CFListIterator l=AbsFac; l.hasItem(); l++)
	{
	  ii++;
	  if (ii%3 == 1 )
	    h_abs *= l.getItem();
	}
	//biNormieren( h_abs );


	// === (2) Compute the rational factor  ===
	// ===     by using the resultant.      ===
	h_res =  resultant(phi(z,x), h(z,e), z);
	//biNormieren( h_res );


	// === (3) Compute the rational factor by ignoring  ===
	// ===     all knowledge of absolute factors.       === 
	h_rat = RationalFactor(phi, ff,fx, g); 
	//biNormieren( h_rat );

	ASSERT(  (h_abs == h_res) && (h_res == h_rat), "Wrong rational factor ?!?");
	h = h_abs;
      }
      // End of absolute factorization.
      gl_RL.append(CFFactor( h,exponent )); // Save the rational factor
      ff/=h;
    }
  }
}


//====================================================== 
//  Factorization of a squarefree bivariate polynomial
//  in which every factor appears only once.
//  Do we need a complete factorization ('absolute' is true)
//  or only a rational factorization ('absolute' false)?
//====================================================== 
void BIFAC::bifacSqrFree(CanonicalForm ff)
//=======================================================
{

  int anz=0;                  // number of factors without field elements

  CFList G   = basisOfG(ff);

  CFList LL;
  CanonicalForm Eg,g;



  // Case 1: There is only one rational & absolute factor ===
  if( G.length() == 1 ){                        // There is only one
    gl_RL.append( CFFactor(ff, exponent));      // rational factor
    gl_AL.append( ff );
    gl_AL.append( exponent );
    gl_AL.append( 0 );
  }
  else // Case 2: There is more than  one absolute factor ===
  {
//    LL  = createEg(G,ff);
//   LL = createEgUni(G,ff); // Hier ist noch ein FEHLER !!!!
        
   LL = createRg( G, ff);  // viel langsamer als EgUni
   
   
    Eg  =  LL.getFirst();
	Eg  =  Eg/LC(Eg); 
   
   g   =  LL.getLast();

//      g = G.getFirst();

    
    CFFList PHI = AbsFactorize( Eg ); 

	CFFListIterator J=PHI; 
	CanonicalForm Eg2=1;
 	for ( ; J.hasItem(); J++)
	{ Eg2 = Eg2 * J.getItem().factor(); }

    // === Is Eg(x) irreducible ? ===
    anz=0;

	// PHI =  AbsFactorize( Eg) ; 
	//
	
    for( CFFListIterator i=PHI; i.hasItem(); i++) { 
      if( !i.getItem().factor().inBaseDomain())
	anz++;
	 }

    /* if( absolute ) // Only for a absolute factorization
      AbsoluteFactorization( PHI,ff, g);
    else         // only for a rational factorization  
    { */
      if( anz==1 ){ ;
	gl_RL.append( CFFactor(ff,exponent));}
      else          
	RationalFactorizationOnly( PHI,ff, g);
   /* } */
  }
}

/////////////////////////////////////////////
// Main procedure for the factorization
// of the bivariate polynomial 'f'.
// REMARK: 'f' might be univariate, too.
//--<>---------------------------------
void BIFAC::bifacMain(CanonicalForm  f)
//--<>---------------------------------
{


  CanonicalForm ff, ggT;

  // ===============================================
  // =    (1) Trivial case: Input is a constant    =
  // ===============================================

  if( f.inBaseDomain() )
  {
    gl_AL.append(f); // store polynomial
    gl_AL.append(1); // store exponent
    gl_AL.append(0); // store ´polynomial´ for field extension

    gl_RL.append( CFFactor(f,1) ); // store polynomial
    return;
  }

  // ===============================================
  // =       STEP: Squarefree decomposition        =
  // ===============================================

 
	CFFList Q =Mysqrfree(f);
//	
//	cout << Q << endl;
//



  // =========================================================
  // =  STEP: Factorization of the squarefree decomposition  =
  // =========================================================


  for( CFFListIterator i=Q; i.hasItem(); i++)
  {

	if( i.getItem().factor().level() < 0 ) ;
	else
	{
    if( ( degree(i.getItem().factor(),x) == 0 || degree( i.getItem().factor(),y) == 0) ) {
      // case: univariate
      unifac( i.getItem().factor(), i.getItem().exp()  ); }
    else // case: bivariate
    {
      exponent =  i.getItem().exp();       // global variable  
	  CanonicalForm dumm = i.getItem().factor();
	  dumm = dumm.LC();
	  if( dumm.level() > 0 ){ dumm =  1;  }
      bifacSqrFree(i.getItem().factor()/dumm ); 
    }
  }}


}


///////////////////////////////////////////////////////
// Find the least prime so that the factorization
// works.
///////////////////////////////////////////////////////

//=======================================================
int BIFAC::findCharacteristic(CanonicalForm f)
//=======================================================
{
  int min = (2*degree(f,'x')-1)*degree(f,'y');
  int nr=0;

  if( min >= 32003 ) return ( 32003 ); // this is the maximum
  
  // Find the smallest poosible prime
  while ( cf_getPrime(nr) < min)  { nr++;  }
  return ( cf_getPrime(nr) );
}

/////////////////////////////////////////////////////////
//
//  PUBLIC functions
//
/////////////////////////////////////////////////////////

// convert the result of the factorization from
// the intern storage type into the public one.
// Also, check the correctness of the solution
// and, if neccessary, change the characteristic.
//--<>---------------------------------
void BIFAC::convertResult(CanonicalForm & f, int ch, int sw)
//--<>---------------------------------
{

  CanonicalForm ff = 1;
  CanonicalForm c;

  CFFList aL;

  //cout << gl_RL<<endl;

	if( sw )
	{
		Variable W('W');
		for( CFFListIterator i=gl_RL; i.hasItem(); i++)
	    {
			c = i.getItem().factor();
			c = c(W,y); 
			c = c(y,x); 
			c = c(x,W);
			aL.append( CFFactor( c, i.getItem().exp() ));
		}

		f = f(W,y); f=f(y,x); f=f(x,W);
	}
	else aL = gl_RL;

	gl_RL = aL;

	//cout << aL;



  // ==========  OUTPUT  =====================
/*  for( CFFListIterator i=aL; i.hasItem(); i++)
  {
    cout << "(" << i.getItem().factor() << ")";
    if( i.getItem().exp() != 1 )
      cout << "^" << i.getItem().exp();
    cout << " * ";
  } */


//  cout << "\n* Test auf Korrektheit ...";
  
 
  for( CFFListIterator i=aL; i.hasItem(); i++)
    {
      ff *= power(i.getItem().factor(),  i.getItem().exp() );
      //      cout << " ff = " << ff
      //	   << "\n a^b = " << i.getItem().factor() << "  ^ " <<   i.getItem().exp() << endl;
    }
  c = f.LC()/ff.LC();

  ff *= c;


//   cout << "\n\nOriginal f = " << f << "\n\nff = " << ff
//	   << "\n\nDiff = " << f-ff << endl << "Quot "<< f/ff <<endl;
//  cout << "degree 0: " << c << endl;
  
  
#ifndef NOSTREAMIO
  if( f != ff ) cout << "\n\nOriginal f = " << f << "\n\nff = " << ff 
		     << "\n\nDiff = " << f-ff << endl << "Quot "<< f/ff <<endl;
#endif
  ASSERT( f==ff, "Wrong rational factorization. Abborting!");
//  cout << "  [OK]\n";
   
}
//--<>---------------------------------
void BIFAC::bifac(CanonicalForm f, bool abs)
//--<>---------------------------------
{
  absolute = 1;      // global variables
  CFList  factors;
  int ch =  getCharacteristic();
  int ch2;


  ASSERT( ch==0 && !isOn(SW_RATIONAL), "Integer numbers not allowed" );
  

  // === Check the characteristic ===
  if( ch != 0 ) 
  {
    ch2 = findCharacteristic(f);
    if( ch <  ch2 )
    {
//      setCharacteristic( ch2 );
      f = mapinto(f);

      // PROVISORISCH
      //cerr << "\n Characteristic is too small!"
	 //  << "\n The result might be wrong!\n\n";
      exit(1);

    } else ;
  }

 	Variable W('W');
  	CanonicalForm l;
	int sw = 0;

	if( degree(f,x) < degree(f,y) ) {
		f = f(W,x);   f = f(x,y); f=f(y,W);  
		sw = 1;
	}
		l = f.LC();

		if( l.level()<0 ) { f = f/f.LC(); gl_RL.append( CFFactor(l,1) ); }


  bifacMain(f);                   // start the computation

  convertResult(f,ch, sw) ; // and convert the result
}

// ==============  end of 'bifac.cc'  ==================
#endif
