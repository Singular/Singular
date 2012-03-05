/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
 * ABSTRACT - multipolynomial resultants - resultant matrices
 *            ( sparse, dense, u-resultant solver )
 */

#include <math.h>

#include <kernel/mod2.h>

#include <omalloc/mylimits.h>
#include <omalloc/omalloc.h>

//-> includes
#include <kernel/options.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>
#include <kernel/matpol.h>
#include <kernel/numbers.h>
#ifdef HAVE_FACTORY
#include <kernel/clapsing.h>
#endif
#include <kernel/sparsmat.h>

#include <kernel/mpr_global.h>
#include <kernel/mpr_base.h>
#include <kernel/mpr_numeric.h>
//<-

extern void nPrint(number n);  // for debugging output

//%s
//-----------------------------------------------------------------------------
//-------------- sparse resultant matrix --------------------------------------
//-----------------------------------------------------------------------------

//-> definitions

//#define mprTEST
//#define mprMINKSUM

#define MAXPOINTS      10000
#define MAXINITELEMS   256
#define LIFT_COOR      50000   // siRand() % LIFT_COOR gives random lift value
#define SCALEDOWN      100.0  // lift value scale down for linear program
#define MINVDIST       0.0
#define RVMULT         0.0001 // multiplicator for random shift vector
#define MAXRVVAL       50000
#define MAXVARS        100
//<-

//-> sparse resultant matrix

/* set of points */
class pointSet;



/* sparse resultant matrix class */
class resMatrixSparse : virtual public resMatrixBase
{
public:
  resMatrixSparse( const ideal _gls, const int special = SNONE );
  ~resMatrixSparse();

  // public interface according to base class resMatrixBase
  const ideal getMatrix();

  /** Fills in resMat[][] with evpoint[] and gets determinant
   * uRPos[i][1]: row of matrix
   * uRPos[i][idelem+1]: col of u(0)
   *  uRPos[i][2..idelem]: col of u(1) .. u(n)
   *  i= 1 .. numSet0
   */
  const number getDetAt( const number* evpoint );

  const poly getUDet( const number* evpoint );

private:
  resMatrixSparse( const resMatrixSparse & );

  void randomVector( const int dim, mprfloat shift[] );

  /** Row Content Function
   * Finds the largest i such that F[i] is a point, F[i]= a[ij] in A[i] for some j.
   * Returns -1 iff the point vert does not lie in a cell
   */
  int RC( pointSet **pQ, pointSet *E, int vert, mprfloat shift[] );

  /* Remaps a result of LP to the according point set Qi.
   * Returns false iff remaping was not possible, otherwise true.
   */
  bool remapXiToPoint( const int indx, pointSet **pQ, int *set, int *vtx );

  /** create coeff matrix
   * uRPos[i][1]: row of matrix
   * uRPos[i][idelem+1]: col of u(0)
   *  uRPos[i][2..idelem]: col of u(1) .. u(n)
   *  i= 1 .. numSet0
   * Returns the dimension of the matrix or -1 in case of an error
   */
  int createMatrix( pointSet *E );

  pointSet * minkSumAll( pointSet **pQ, int numq, int dim );
  pointSet * minkSumTwo( pointSet *Q1, pointSet *Q2, int dim );

private:
  ideal gls;

  int n, idelem;     // number of variables, polynoms
  int numSet0;       // number of elements in S0
  int msize;         // size of matrix

  intvec *uRPos;

  ideal rmat;        // sparse matrix representation

  simplex * LP;      // linear programming stuff
};
//<-

//-> typedefs and structs
poly monomAt( poly p, int i );

typedef unsigned int Coord_t;

struct setID
{
  int set;
  int pnt;
};

struct onePoint
{
  Coord_t * point;             // point[0] is unused, maxial dimension is MAXVARS+1
  setID rc;                    // filled in by Row Content Function
  struct onePoint * rcPnt;     // filled in by Row Content Function
};

typedef struct onePoint * onePointP;

/* sparse matrix entry */
struct _entry
{
  number num;
  int col;
  struct _entry * next;
};

typedef struct _entry * entry;
//<-

//-> class pointSet
class pointSet
{
private:
  onePointP *points;     // set of onePoint's, index [1..num], supports of monoms
  bool lifted;

public:
  int num;               // number of elements in points
  int max;               // maximal entries in points, i.e. allocated mem
  int dim;               // dimension, i.e. valid coord entries in point
  int index;             // should hold unique identifier of point set

  pointSet( const int _dim, const int _index= 0, const int count= MAXINITELEMS );
   ~pointSet();

  // pointSet.points[i] equals pointSet[i]
  inline const onePointP operator[] ( const int index );

  /** Adds a point to pointSet, copy vert[0,...,dim] ot point[num+1][0,...,dim].
   * Returns false, iff additional memory was allocated ( i.e. num >= max )
   * else returns true
   */
  bool addPoint( const onePointP vert );

  /** Adds a point to pointSet, copy vert[0,...,dim] ot point[num+1][0,...,dim].
   * Returns false, iff additional memory was allocated ( i.e. num >= max )
   * else returns true
   */
  bool addPoint( const int * vert );

  /** Adds a point to pointSet, copy vert[0,...,dim] ot point[num+1][0,...,dim].
   * Returns false, iff additional memory was allocated ( i.e. num >= max )
   * else returns true
   */
  bool addPoint( const Coord_t * vert );

  /* Removes the point at intex indx */
  bool removePoint( const int indx );

  /** Adds point to pointSet, iff pointSet \cap point = \emptyset.
   * Returns true, iff added, else false.
   */
  bool mergeWithExp( const onePointP vert );

  /** Adds point to pointSet, iff pointSet \cap point = \emptyset.
   * Returns true, iff added, else false.
   */
  bool mergeWithExp( const int * vert );

  /* Adds support of poly p to pointSet, iff pointSet \cap point = \emptyset. */
  void mergeWithPoly( const poly p );

  /* Returns the row polynom multiplicator in vert[] */
  void getRowMP( const int indx, int * vert );

  /* Returns index of supp(LT(p)) in pointSet. */
  int getExpPos( const poly p );

  /** sort lex
   */
  void sort();

  /** Lifts the point set using sufficiently generic linear lifting
   * homogeneous forms l[1]..l[dim] in Z. Every l[i] is of the form
   * L1x1+...+Lnxn, for generic L1..Ln in Z.
   *
   * Lifting raises dimension by one!
   */
  void lift( int *l= NULL );     // !! increments dim by 1
  void unlift() { dim--; lifted= false; }

private:
  pointSet( const pointSet & );

  /** points[a] < points[b] ? */
  inline bool smaller( int, int );

  /** points[a] > points[b] ? */
  inline bool larger( int, int );

  /** Checks, if more mem is needed ( i.e. num >= max ),
   * returns false, if more mem was allocated, else true
   */
  inline bool checkMem();
};
//<-

//-> class convexHull
/* Compute convex hull of given exponent set */
class convexHull
{
public:
  convexHull( simplex * _pLP ) : pLP(_pLP) {}
  ~convexHull() {}

  /** Computes the point sets of the convex hulls of the supports given
   * by the polynoms in gls.
   * Returns Q[].
   */
  pointSet ** newtonPolytopesP( const ideal gls );
  ideal newtonPolytopesI( const ideal gls );

private:
  /** Returns true iff the support of poly pointPoly is inside the
   * convex hull of all points given by the  support of poly p.
   */
  bool inHull(poly p, poly pointPoly, int m, int site);

private:
  pointSet **Q;
  int n;
  simplex * pLP;
};
//<-

//-> class mayanPyramidAlg
/* Compute all lattice points in a given convex hull */
class mayanPyramidAlg
{
public:
  mayanPyramidAlg( simplex * _pLP ) : n(pVariables), pLP(_pLP) {}
  ~mayanPyramidAlg() {}

  /** Drive Mayan Pyramid Algorithm.
   * The Alg computes conv(Qi[]+shift[]).
   */
  pointSet * getInnerPoints( pointSet **_q_i, mprfloat _shift[] );

private:

  /** Recursive Mayan Pyramid algorithm for directly computing MinkowskiSum
   * lattice points for (n+1)-fold MinkowskiSum of given point sets Qi[].
   * Recursively for range of dim: dim in [0..n); acoords[0..var) fixed.
   * Stores only MinkowskiSum points of udist > 0: done by storeMinkowskiSumPoints.
   */
  void runMayanPyramid( int dim );

  /**  Compute v-distance via Linear Programing
   * Linear Program finds the v-distance of the point in accords[].
   * The v-distance is the distance along the direction v to boundary of
   * Minkowski Sum of Qi (here vector v is represented by shift[]).
   * Returns the v-distance or -1.0 if an error occured.
   */
  mprfloat vDistance( Coord_t * acoords, int dim );

  /** LP for finding min/max coord in MinkowskiSum, given previous coors.
   * Assume MinkowskiSum in non-negative quadrants
   * coor in [0,n); fixed coords in acoords[0..coor)
   */
  void mn_mx_MinkowskiSum( int dim, Coord_t *minR, Coord_t *maxR );

  /**  Stores point in E->points[pt], iff v-distance != 0
   * Returns true iff point was stored, else flase
   */
  bool storeMinkowskiSumPoint();

private:
  pointSet **Qi;
  pointSet *E;
  mprfloat *shift;

  int n,idelem;

  Coord_t acoords[MAXVARS+2];

  simplex * pLP;
};
//<-

//-> debug output stuff
#if defined(mprDEBUG_PROT) || defined(mprDEBUG_ALL)
void print_mat(mprfloat **a, int maxrow, int maxcol)
{
  int i, j;

  for (i = 1; i <= maxrow; i++)
  {
    PrintS("[");
    for (j = 1; j <= maxcol; j++) Print("% 7.2f, ", a[i][j]);
    PrintS("],\n");
  }
}
void print_bmat(mprfloat **a, int nrows, int ncols, int N, int *iposv)
{
  int i, j;

  printf("Output matrix from LinProg");
  for (i = 1; i <= nrows; i++)
  {
    printf("\n[ ");
    if (i == 1) printf("  ");
    else if (iposv[i-1] <= N) printf("X%d", iposv[i-1]);
    else printf("Y%d", iposv[i-1]-N+1);
    for (j = 1; j <= ncols; j++) printf(" %7.2f ",(double)a[i][j]);
    printf(" ]");
  } printf("\n");
  fflush(stdout);
}

void print_exp( const onePointP vert, int n )
{
  int i;
  for ( i= 1; i <= n; i++ )
  {
    Print(" %d",vert->point[i] );
#ifdef LONG_OUTPUT
    if ( i < n ) PrintS(", ");
#endif
  }
}
void print_matrix( matrix omat )
{
  int i,j;
  int val;
  Print(" matrix m[%d][%d]=(\n",MATROWS( omat ),MATCOLS( omat ));
  for ( i= 1; i <= MATROWS( omat ); i++ )
  {
    for ( j= 1; j <= MATCOLS( omat ); j++ )
    {
      if ( (MATELEM( omat, i, j)!=NULL)
      && (!nIsZero(pGetCoeff( MATELEM( omat, i, j)))))
      {
        val= n_Int(pGetCoeff( MATELEM( omat, i, j) ), currRing);
        if ( i==MATROWS(omat) && j==MATCOLS(omat) )
        {
          Print("%d ",val);
        }
        else
        {
          Print("%d, ",val);
        }
      }
      else
      {
        if ( i==MATROWS(omat) && j==MATCOLS(omat) )
        {
          PrintS("  0");
        }
        else
        {
          PrintS("  0, ");
        }
      }
    }
    PrintLn();
  }
  PrintS(");\n");
}
#endif
//<-

//-> pointSet::*
pointSet::pointSet( const int _dim, const int _index, const int count )
  : num(0), max(count), dim(_dim), index(_index)
{
  int i;
  points = (onePointP *)omAlloc( (count+1) * sizeof(onePointP) );
  for ( i= 0; i <= max; i++ )
  {
    points[i]= (onePointP)omAlloc( sizeof(onePoint) );
    points[i]->point= (Coord_t *)omAlloc0( (dim+2) * sizeof(Coord_t) );
  }
  lifted= false;
}

pointSet::~pointSet()
{
  int i;
  int fdim= lifted ? dim+1 : dim+2;
  for ( i= 0; i <= max; i++ )
  {
    omFreeSize( (ADDRESS) points[i]->point, fdim * sizeof(Coord_t) );
    omFreeSize( (ADDRESS) points[i], sizeof(onePoint) );
  }
  omFreeSize( (ADDRESS) points, (max+1) * sizeof(onePointP) );
}

inline const onePointP pointSet::operator[] ( const int index_i )
{
  assume( index_i > 0 && index_i <= num );
  return points[index_i];
}

inline bool pointSet::checkMem()
{
  if ( num >= max )
  {
    int i;
    int fdim= lifted ? dim+1 : dim+2;
    points= (onePointP*)omReallocSize( points,
                                 (max+1) * sizeof(onePointP),
                                 (2*max + 1) * sizeof(onePointP) );
    for ( i= max+1; i <= max*2; i++ )
    {
      points[i]= (onePointP)omAlloc( sizeof(struct onePoint) );
      points[i]->point= (Coord_t *)omAlloc0( fdim * sizeof(Coord_t) );
    }
    max*= 2;
    mprSTICKYPROT(ST_SPARSE_MEM);
    return false;
  }
  return true;
}

bool pointSet::addPoint( const onePointP vert )
{
  int i;
  bool ret;
  num++;
  ret= checkMem();
  points[num]->rcPnt= NULL;
  for ( i= 1; i <= dim; i++ ) points[num]->point[i]= vert->point[i];
  return ret;
}

bool pointSet::addPoint( const int * vert )
{
  int i;
  bool ret;
  num++;
  ret= checkMem();
  points[num]->rcPnt= NULL;
  for ( i= 1; i <= dim; i++ ) points[num]->point[i]= (Coord_t) vert[i];
  return ret;
}

bool pointSet::addPoint( const Coord_t * vert )
{
  int i;
  bool ret;
  num++;
  ret= checkMem();
  points[num]->rcPnt= NULL;
  for ( i= 0; i < dim; i++ ) points[num]->point[i+1]= vert[i];
  return ret;
}

bool pointSet::removePoint( const int indx )
{
  assume( indx > 0 && indx <= num );
  if ( indx != num )
  {
    onePointP tmp;
    tmp= points[indx];
    points[indx]= points[num];
    points[num]= tmp;
  }
  num--;

  return true;
}

bool pointSet::mergeWithExp( const onePointP vert )
{
  int i,j;

  for ( i= 1; i <= num; i++ )
  {
    for ( j= 1; j <= dim; j++ )
      if ( points[i]->point[j] != vert->point[j] ) break;
    if ( j > dim ) break;
  }

  if ( i > num )
  {
    addPoint( vert );
    return true;
  }
  return false;
}

bool pointSet::mergeWithExp( const int * vert )
{
  int i,j;

  for ( i= 1; i <= num; i++ )
  {
    for ( j= 1; j <= dim; j++ )
      if ( points[i]->point[j] != (Coord_t) vert[j] ) break;
    if ( j > dim ) break;
  }

  if ( i > num )
  {
    addPoint( vert );
    return true;
  }
  return false;
}

void pointSet::mergeWithPoly( const poly p )
{
  int i,j;
  poly piter= p;
  int * vert;
  vert= (int *)omAlloc( (dim+1) * sizeof(int) );

  while ( piter )
  {
    pGetExpV( piter, vert );

    for ( i= 1; i <= num; i++ )
    {
      for ( j= 1; j <= dim; j++ )
        if ( points[i]->point[j] != (Coord_t) vert[j] ) break;
      if ( j > dim ) break;
    }

    if ( i > num )
    {
      addPoint( vert );
    }

    pIter( piter );
  }
  omFreeSize( (ADDRESS) vert, (dim+1) * sizeof(int) );
}

int pointSet::getExpPos( const poly p )
{
  int * vert;
  int i,j;

  // hier unschoen...
  vert= (int *)omAlloc( (dim+1) * sizeof(int) );

  pGetExpV( p, vert );
  for ( i= 1; i <= num; i++ )
  {
    for ( j= 1; j <= dim; j++ )
      if ( points[i]->point[j] != (Coord_t) vert[j] ) break;
    if ( j > dim ) break;
  }
  omFreeSize( (ADDRESS) vert, (dim+1) * sizeof(int) );

  if ( i > num ) return 0;
  else return i;
}

void pointSet::getRowMP( const int indx, int * vert )
{
  assume( indx > 0 && indx <= num && points[indx]->rcPnt );
  int i;

  vert[0]= 0;
  for ( i= 1; i <= dim; i++ )
    vert[i]= (int)(points[indx]->point[i] - points[indx]->rcPnt->point[i]);
}

inline bool pointSet::smaller( int a, int b )
{
  int i;

  for ( i= 1; i <= dim; i++ )
  {
    if ( points[a]->point[i] > points[b]->point[i] )
    {
      return false;
    }
    if ( points[a]->point[i] < points[b]->point[i] )
    {
      return true;
    }
  }

 return false; // they are equal
}

inline bool pointSet::larger( int a, int b )
{
  int i;

  for ( i= 1; i <= dim; i++ )
  {
    if ( points[a]->point[i] < points[b]->point[i] )
    {
      return false;
    }
    if ( points[a]->point[i] > points[b]->point[i] )
    {
      return true;
    }
  }

 return false; // they are equal
}

void pointSet::sort()
{
  int i;
  bool found= true;
  onePointP tmp;

  while ( found )
  {
    found= false;
    for ( i= 1; i < num; i++ )
    {
      if ( larger( i, i+1 ) )
      {
        tmp= points[i];
        points[i]= points[i+1];
        points[i+1]= tmp;

        found= true;
      }
    }
  }
}

void pointSet::lift( int l[] )
{
  bool outerL= true;
  int i, j;
  int sum;

  dim++;

  if ( l==NULL )
  {
    outerL= false;
    l= (int *)omAlloc( (dim+1) * sizeof(int) ); // [1..dim-1]

    for(i = 1; i < dim; i++)
    {
      l[i]= 1 + siRand() % LIFT_COOR;
    }
  }
  for ( j=1; j <= num; j++ )
  {
    sum= 0;
    for ( i=1; i < dim; i++ )
    {
      sum += (int)points[j]->point[i] * l[i];
    }
    points[j]->point[dim]= sum;
  }

#ifdef mprDEBUG_ALL
  PrintS(" lift vector: ");
  for ( j=1; j < dim; j++ ) Print(" %d ",l[j] );
  PrintLn();
#ifdef mprDEBUG_ALL
  PrintS(" lifted points: \n");
  for ( j=1; j <= num; j++ )
  {
    Print("%d: <",j);print_exp(points[j],dim);PrintS(">\n");
  }
  PrintLn();
#endif
#endif

  lifted= true;

  if ( !outerL ) omFreeSize( (ADDRESS) l, (dim+1) * sizeof(int) );
}
//<-

//-> global functions
// Returns the monom at pos i in poly p
poly monomAt( poly p, int i )
{
  assume( i > 0 );
  poly iter= p;
  for ( int j= 1; (j < i) && (iter!=NULL); j++ ) iter= pIter(iter);
  return iter;
}
//<-

//-> convexHull::*
bool convexHull::inHull(poly p, poly pointPoly, int m, int site)
{
  int i, j, col;

  pLP->m = n+1;
  pLP->n = m;                // this includes col of cts

  pLP->LiPM[1][1] = +0.0;
  pLP->LiPM[1][2] = +1.0;        // optimize (arbitrary) var
  pLP->LiPM[2][1] = +1.0;
  pLP->LiPM[2][2] = -1.0;         // lambda vars sum up to 1

  for ( j=3; j <= pLP->n; j++)
  {
    pLP->LiPM[1][j] = +0.0;
    pLP->LiPM[2][j] = -1.0;
  }

  for( i= 1; i <= n; i++) {        // each row constraints one coor
    pLP->LiPM[i+2][1] = (mprfloat)pGetExp(pointPoly,i);
    col = 2;
    for( j= 1; j <= m; j++ )
    {
      if( j != site )
      {
        pLP->LiPM[i+2][col] = -(mprfloat)pGetExp( monomAt(p,j), i );
        col++;
      }
    }
  }

#ifdef mprDEBUG_ALL
  PrintS("Matrix of Linear Programming\n");
  print_mat( pLP->LiPM, pLP->m+1,pLP->n);
#endif

  pLP->m3= pLP->m;

  pLP->compute();

  return (pLP->icase == 0);
}

// mprSTICKYPROT:
// ST_SPARSE_VADD: new vertex of convex hull added
// ST_SPARSE_VREJ: point rejected (-> inside hull)
pointSet ** convexHull::newtonPolytopesP( const ideal gls )
{
  int i, j, k;
  int m;  // Anzahl der Exponentvektoren im i-ten Polynom (gls->m)[i] des Ideals gls
  int idelem= IDELEMS(gls);
  int * vert;

  n= pVariables;
  vert= (int *)omAlloc( (idelem+1) * sizeof(int) );

  Q = (pointSet **)omAlloc( idelem * sizeof(pointSet*) );        // support hulls
  for ( i= 0; i < idelem; i++ )
    Q[i] = new pointSet( pVariables, i+1, pLength((gls->m)[i])+1 );

  for( i= 0; i < idelem; i++ )
  {
    k=1;
    m = pLength( (gls->m)[i] );

    poly p= (gls->m)[i];
    for( j= 1; j <= m; j++) {  // für jeden Exponentvektor
      if( !inHull( (gls->m)[i], p, m, j ) )
      {
        pGetExpV( p, vert );
        Q[i]->addPoint( vert );
        k++;
        mprSTICKYPROT(ST_SPARSE_VADD);
      }
      else
      {
        mprSTICKYPROT(ST_SPARSE_VREJ);
      }
      pIter( p );
    } // j
    mprSTICKYPROT("\n");
  } // i

  omFreeSize( (ADDRESS) vert, (idelem+1) * sizeof(int) );

#ifdef mprDEBUG_PROT
  PrintLn();
  for( i= 0; i < idelem; i++ )
  {
    Print(" \\Conv(Qi[%d]): #%d\n", i,Q[i]->num );
    for ( j=1; j <= Q[i]->num; j++ )
    {
      Print("%d: <",j);print_exp( (*Q[i])[j] , pVariables );PrintS(">\n");
    }
    PrintLn();
  }
#endif

  return Q;
}

// mprSTICKYPROT:
// ST_SPARSE_VADD: new vertex of convex hull added
// ST_SPARSE_VREJ: point rejected (-> inside hull)
ideal convexHull::newtonPolytopesI( const ideal gls )
{
  int i, j;
  int m;  // Anzahl der Exponentvektoren im i-ten Polynom (gls->m)[i] des Ideals gls
  int idelem= IDELEMS(gls);
  ideal id;
  poly p,pid;
  int * vert;

  n= pVariables;
  vert= (int *)omAlloc( (idelem+1) * sizeof(int) );
  id= idInit( idelem, 1 );

  for( i= 0; i < idelem; i++ )
  {
    m = pLength( (gls->m)[i] );

    p= (gls->m)[i];
    for( j= 1; j <= m; j++) {  // für jeden Exponentvektor
      if( !inHull( (gls->m)[i], p, m, j ) )
      {
        if ( (id->m)[i] == NULL )
        {
          (id->m)[i]= pHead(p);
          pid=(id->m)[i];
        }
        else
        {
          pNext(pid)= pHead(p);
          pIter(pid);
          pNext(pid)= NULL;
        }
        mprSTICKYPROT(ST_SPARSE_VADD);
      }
      else
      {
        mprSTICKYPROT(ST_SPARSE_VREJ);
      }
      pIter( p );
    } // j
    mprSTICKYPROT("\n");
  } // i

  omFreeSize( (ADDRESS) vert, (idelem+1) * sizeof(int) );

#ifdef mprDEBUG_PROT
  PrintLn();
  for( i= 0; i < idelem; i++ )
  {
  }
#endif

  return id;
}
//<-

//-> mayanPyramidAlg::*
pointSet * mayanPyramidAlg::getInnerPoints( pointSet **_q_i, mprfloat _shift[] )
{
  int i;

  Qi= _q_i;
  shift= _shift;

  E= new pointSet( Qi[0]->dim ); // E has same dim as Qi[...]

  for ( i= 0; i < MAXVARS+2; i++ ) acoords[i]= 0;

  runMayanPyramid(0);

  mprSTICKYPROT("\n");

  return E;
}

mprfloat mayanPyramidAlg::vDistance( Coord_t * acoords_a, int dim )
{
  int i, ii, j, k, col, r;
  int numverts, cols;

  numverts = 0;
  for( i=0; i<=n; i++)
  {
    numverts += Qi[i]->num;
  }
  cols = numverts + 2;

  //if( dim < 1 || dim > n )
  //  WerrorS("mayanPyramidAlg::vDistance: Known coords dim off range");

  pLP->LiPM[1][1] = 0.0;
  pLP->LiPM[1][2] = 1.0;        // maximize
  for( j=3; j<=cols; j++) pLP->LiPM[1][j] = 0.0;

  for( i=0; i <= n; i++ )
  {
    pLP->LiPM[i+2][1] = 1.0;
    pLP->LiPM[i+2][2] = 0.0;
  }
  for( i=1; i<=dim; i++)
  {
    pLP->LiPM[n+2+i][1] = (mprfloat)(acoords_a[i-1]);
    pLP->LiPM[n+2+i][2] = -shift[i];
  }

  ii = -1;
  col = 2;
  for ( i= 0; i <= n; i++ )
  {
    ii++;
    for( k= 1; k <= Qi[ii]->num; k++ )
    {
      col++;
      for ( r= 0; r <= n; r++ )
      {
        if ( r == i ) pLP->LiPM[r+2][col] = -1.0;
        else pLP->LiPM[r+2][col] = 0.0;
      }
      for( r= 1; r <= dim; r++ )
        pLP->LiPM[r+n+2][col] = -(mprfloat)((*Qi[ii])[k]->point[r]);
    }
  }

  if( col != cols)
    Werror("mayanPyramidAlg::vDistance:"
           "setting up matrix for udist: col %d != cols %d",col,cols);

  pLP->m = n+dim+1;
  pLP->m3= pLP->m;
  pLP->n=cols-1;

#ifdef mprDEBUG_ALL
  Print("vDistance LP, known koords dim=%d, constr %d, cols %d, acoords= ",
        dim,pLP->m,cols);
  for( i= 0; i < dim; i++ )
    Print(" %d",acoords_a[i]);
  PrintLn();
  print_mat( pLP->LiPM, pLP->m+1, cols);
#endif

  pLP->compute();

#ifdef mprDEBUG_ALL
  PrintS("LP returns matrix\n");
  print_bmat( pLP->LiPM, pLP->m+1, cols+1-pLP->m, cols, pLP->iposv);
#endif

  if( pLP->icase != 0 )
  {  // check for errors
    WerrorS("mayanPyramidAlg::vDistance:");
    if( pLP->icase == 1 )
      WerrorS(" Unbounded v-distance: probably 1st v-coor=0");
    else if( pLP->icase == -1 )
      WerrorS(" Infeasible v-distance");
    else
      WerrorS(" Unknown error");
    return -1.0;
  }

  return pLP->LiPM[1][1];
}

void  mayanPyramidAlg::mn_mx_MinkowskiSum( int dim, Coord_t *minR, Coord_t *maxR )
{
  int i, j, k, cols, cons;
  int la_cons_row;

  cons = n+dim+2;

  // first, compute minimum
  //

  // common part of the matrix
  pLP->LiPM[1][1] = 0.0;
  for( i=2; i<=n+2; i++)
  {
    pLP->LiPM[i][1] = 1.0;        // 1st col
    pLP->LiPM[i][2] = 0.0;        // 2nd col
  }

  la_cons_row = 1;
  cols = 2;
  for( i=0; i<=n; i++)
  {
    la_cons_row++;
    for( j=1; j<= Qi[i]->num; j++)
    {
      cols++;
      pLP->LiPM[1][cols] = 0.0;        // set 1st row 0
      for( k=2; k<=n+2; k++)
      {  // lambdas sum up to 1
        if( k != la_cons_row) pLP->LiPM[k][cols] = 0.0;
        else pLP->LiPM[k][cols] = -1.0;
      }
      for( k=1; k<=n; k++)
        pLP->LiPM[k+n+2][cols] = -(mprfloat)((*Qi[i])[j]->point[k]);
    } // j
  } // i

  for( i= 0; i < dim; i++ )
  {                // fixed coords
    pLP->LiPM[i+n+3][1] = acoords[i];
    pLP->LiPM[i+n+3][2] = 0.0;
  }
  pLP->LiPM[dim+n+3][1] = 0.0;


  pLP->LiPM[1][2] = -1.0;                        // minimize
  pLP->LiPM[dim+n+3][2] = 1.0;

#ifdef mprDEBUG_ALL
  Print("\nThats the matrix for minR, dim= %d, acoords= ",dim);
  for( i= 0; i < dim; i++ )
    Print(" %d",acoords[i]);
  PrintLn();
  print_mat( pLP->LiPM, cons+1, cols);
#endif

  // simplx finds MIN for obj.fnc, puts it in [1,1]
  pLP->m= cons;
  pLP->n= cols-1;
  pLP->m3= cons;

  pLP->compute();

  if ( pLP->icase != 0 )
  { // check for errors
    if( pLP->icase < 0)
      WerrorS(" mn_mx_MinkowskiSum: LinearProgram: minR: infeasible");
    else if( pLP->icase > 0)
      WerrorS(" mn_mx_MinkowskiSum: LinearProgram: minR: unbounded");
  }

  *minR = (Coord_t)( -pLP->LiPM[1][1] + 1.0 - SIMPLEX_EPS );

  // now compute maximum
  //

  // common part of the matrix again
  pLP->LiPM[1][1] = 0.0;
  for( i=2; i<=n+2; i++)
  {
    pLP->LiPM[i][1] = 1.0;
    pLP->LiPM[i][2] = 0.0;
  }
  la_cons_row = 1;
  cols = 2;
  for( i=0; i<=n; i++)
  {
    la_cons_row++;
    for( j=1; j<=Qi[i]->num; j++)
    {
      cols++;
      pLP->LiPM[1][cols] = 0.0;
      for( k=2; k<=n+2; k++)
      {
        if( k != la_cons_row) pLP->LiPM[k][cols] = 0.0;
        else pLP->LiPM[k][cols] = -1.0;
      }
      for( k=1; k<=n; k++)
        pLP->LiPM[k+n+2][cols] = -(mprfloat)((*Qi[i])[j]->point[k]);
    } // j
  }  // i

  for( i= 0; i < dim; i++ )
  {                // fixed coords
    pLP->LiPM[i+n+3][1] = acoords[i];
    pLP->LiPM[i+n+3][2] = 0.0;
  }
  pLP->LiPM[dim+n+3][1] = 0.0;

  pLP->LiPM[1][2] = 1.0;                      // maximize
  pLP->LiPM[dim+n+3][2] = 1.0;                // var = sum of pnt coords

#ifdef mprDEBUG_ALL
  Print("\nThats the matrix for maxR, dim= %d\n",dim);
  print_mat( pLP->LiPM, cons+1, cols);
#endif

  pLP->m= cons;
  pLP->n= cols-1;
  pLP->m3= cons;

  // simplx finds MAX for obj.fnc, puts it in [1,1]
  pLP->compute();

  if ( pLP->icase != 0 )
  {
    if( pLP->icase < 0)
      WerrorS(" mn_mx_MinkowskiSum: LinearProgram: maxR: infeasible");
    else if( pLP->icase > 0)
      WerrorS(" mn_mx_MinkowskiSum: LinearProgram: maxR: unbounded");
  }

  *maxR = (Coord_t)( pLP->LiPM[1][1] + SIMPLEX_EPS );

#ifdef mprDEBUG_ALL
  Print("  Range for dim=%d: [%d,%d]\n", dim, *minR, *maxR);
#endif
}

// mprSTICKYPROT:
// ST_SPARSE_VREJ: rejected point
// ST_SPARSE_VADD: added point to set
bool mayanPyramidAlg::storeMinkowskiSumPoint()
{
  mprfloat dist;

  // determine v-distance of point pt
  dist= vDistance( &(acoords[0]), n );

  // store only points with v-distance > minVdist
  if( dist <= MINVDIST + SIMPLEX_EPS )
  {
    mprSTICKYPROT(ST_SPARSE_VREJ);
    return false;
  }

  E->addPoint( &(acoords[0]) );
  mprSTICKYPROT(ST_SPARSE_VADD);

  return true;
}

// mprSTICKYPROT:
// ST_SPARSE_MREC1: recurse
// ST_SPARSE_MREC2: recurse with extra points
// ST_SPARSE_MPEND: end
void mayanPyramidAlg::runMayanPyramid( int dim )
{
  Coord_t minR, maxR;
  mprfloat dist;

  // step 3
  mn_mx_MinkowskiSum( dim, &minR, &maxR );

#ifdef mprDEBUG_ALL
  int i;
  for( i=0; i <= dim; i++) Print("acoords[%d]=%d ",i,(int)acoords[i]);
  Print(":: [%d,%d]\n", minR, maxR);
#endif

  // step 5 -> terminate
  if( dim == n-1 )
  {
    int lastKilled = 0;
    // insert points
    acoords[dim] = minR;
    while( acoords[dim] <= maxR )
    {
      if( !storeMinkowskiSumPoint() )
        lastKilled++;
      acoords[dim]++;
    }
    mprSTICKYPROT(ST_SPARSE_MPEND);
    return;
  }

  // step 4 -> recurse at step 3
  acoords[dim] = minR;
  while ( acoords[dim] <= maxR )
  {
    if ( (acoords[dim] > minR) && (acoords[dim] <= maxR) )
    {     // acoords[dim] >= minR  ??
      mprSTICKYPROT(ST_SPARSE_MREC1);
      runMayanPyramid( dim + 1 );         // recurse with higer dimension
    }
    else
    {
      // get v-distance of pt
      dist= vDistance( &(acoords[0]), dim + 1 );// dim+1 == known coordinates

      if( dist >= SIMPLEX_EPS )
      {
        mprSTICKYPROT(ST_SPARSE_MREC2);
        runMayanPyramid( dim + 1 );       // recurse with higer dimension
      }
    }
    acoords[dim]++;
  } // while
}
//<-

//-> resMatrixSparse::*
bool resMatrixSparse::remapXiToPoint( const int indx, pointSet **pQ, int *set, int *pnt )
{
  int i,nn= pVariables;
  int loffset= 0;
  for ( i= 0; i <= nn; i++ )
  {
    if ( (loffset < indx) && (indx <= pQ[i]->num + loffset) )
    {
      *set= i;
      *pnt= indx-loffset;
      return true;
    }
    else loffset+= pQ[i]->num;
  }
  return false;
}

// mprSTICKYPROT
// ST_SPARSE_RC: point added
int resMatrixSparse::RC( pointSet **pQ, pointSet *E, int vert, mprfloat shift[] )
{
  int i, j, k,c ;
  int size;
  bool found= true;
  mprfloat cd;
  int onum;
  int bucket[MAXVARS+2];
  setID *optSum;

  LP->n = 1;
  LP->m = n + n + 1;   // number of constrains

  // fill in LP matrix
  for ( i= 0; i <= n; i++ )
  {
    size= pQ[i]->num;
    for ( k= 1; k <= size; k++ )
    {
      LP->n++;

      // objective funtion, minimize
      LP->LiPM[1][LP->n] = - ( (mprfloat) (*pQ[i])[k]->point[pQ[i]->dim] / SCALEDOWN );

      // lambdas sum up to 1
      for ( j = 0; j <= n; j++ )
      {
        if ( i==j )
          LP->LiPM[j+2][LP->n] = -1.0;
        else
          LP->LiPM[j+2][LP->n] = 0.0;
      }

      // the points
      for ( j = 1; j <= n; j++ )
      {
        LP->LiPM[j+n+2][LP->n] =  - ( (mprfloat) (*pQ[i])[k]->point[j] );
      }
    }
  }

  for ( j = 0; j <= n; j++ ) LP->LiPM[j+2][1] = 1.0;
  for ( j= 1; j <= n; j++ )
  {
    LP->LiPM[j+n+2][1]= (mprfloat)(*E)[vert]->point[j] - shift[j];
  }
  LP->n--;

  LP->LiPM[1][1] = 0.0;

#ifdef mprDEBUG_ALL
  PrintLn();
  Print(" n= %d, LP->m=M= %d, LP->n=N= %d\n",n,LP->m,LP->n);
  print_mat(LP->LiPM, LP->m+1, LP->n+1);
#endif

  LP->m3= LP->m;

  LP->compute();

  if ( LP->icase < 0 )
  {
    // infeasibility: the point does not lie in a cell -> remove it
    return -1;
  }

  // store result
  (*E)[vert]->point[E->dim]= (int)(-LP->LiPM[1][1] * SCALEDOWN);

#ifdef mprDEBUG_ALL
  Print(" simplx returned %d, Objective value = %f\n", LP->icase, LP->LiPM[1][1]);
  //print_bmat(LP->LiPM, NumCons + 1, LP->n+1-NumCons, LP->n+1, LP->iposv); // ( rows= M+1, cols= N+1-m3 )
  //print_mat(LP->LiPM, NumCons+1, LP->n);
#endif

#if 1
  // sort LP results
  while (found)
  {
    found=false;
    for ( i= 1; i < LP->m; i++ )
    {
      if ( LP->iposv[i] > LP->iposv[i+1] )
      {

        c= LP->iposv[i];
        LP->iposv[i]=LP->iposv[i+1];
        LP->iposv[i+1]=c;

        cd=LP->LiPM[i+1][1];
        LP->LiPM[i+1][1]=LP->LiPM[i+2][1];
        LP->LiPM[i+2][1]=cd;

        found= true;
      }
    }
  }
#endif

#ifdef mprDEBUG_ALL
  print_bmat(LP->LiPM, LP->m + 1, LP->n+1-LP->m, LP->n+1, LP->iposv);
  PrintS(" now split into sets\n");
#endif


  // init bucket
  for ( i= 0; i <= E->dim; i++ ) bucket[i]= 0;
  // remap results of LP to sets Qi
  c=0;
  optSum= (setID*)omAlloc( (LP->m) * sizeof(struct setID) );
  for ( i= 0; i < LP->m; i++ )
  {
    //Print("% .15f\n",LP->LiPM[i+2][1]);
    if ( LP->LiPM[i+2][1] > 1e-12 )
    {
      if ( !remapXiToPoint( LP->iposv[i+1], pQ, &(optSum[c].set), &(optSum[c].pnt) ) )
      {
        Werror(" resMatrixSparse::RC: Found bad solution in LP: %d!",LP->iposv[i+1]);
        WerrorS(" resMatrixSparse::RC: remapXiToPoint faild!");
        return -1;
      }
      bucket[optSum[c].set]++;
      c++;
    }
  }

  onum= c;
  // find last min in bucket[]: maximum i such that Fi is a point
  c= 0;
  for ( i= 1; i < E->dim; i++ )
  {
    if ( bucket[c] >= bucket[i] )
    {
      c= i;
    }
  }
  // find matching point set
  for ( i= onum - 1; i >= 0; i-- )
  {
    if ( optSum[i].set == c )
      break;
  }
  // store
  (*E)[vert]->rc.set= c;
  (*E)[vert]->rc.pnt= optSum[i].pnt;
  (*E)[vert]->rcPnt= (*pQ[c])[optSum[i].pnt];
  // count
  if ( (*E)[vert]->rc.set == linPolyS ) numSet0++;

#ifdef mprDEBUG_PROT
  Print("\n Point E[%d] was <",vert);print_exp((*E)[vert],E->dim-1);Print(">, bucket={");
  for ( j= 0; j < E->dim; j++ )
  {
    Print(" %d",bucket[j]);
  }
  PrintS(" }\n optimal Sum: Qi ");
  for ( j= 0; j < LP->m; j++ )
  {
    Print(" [ %d, %d ]",optSum[j].set,optSum[j].pnt);
  }
  Print(" -> i= %d, j = %d\n",(*E)[vert]->rc.set,optSum[i].pnt);
#endif

  // clean up
  omFreeSize( (ADDRESS) optSum, (LP->m) * sizeof(struct setID) );

  mprSTICKYPROT(ST_SPARSE_RC);

  return (int)(-LP->LiPM[1][1] * SCALEDOWN);
}

// create coeff matrix
int resMatrixSparse::createMatrix( pointSet *E )
{
  // sparse matrix
  //    uRPos[i][1]: row of matrix
  //    uRPos[i][idelem+1]: col of u(0)
  //    uRPos[i][2..idelem]: col of u(1) .. u(n)
  //    i= 1 .. numSet0
  int i,epos;
  int rp,cp;
  poly rowp,epp;
  poly iterp;
  int *epp_mon, *eexp;

  epp_mon= (int *)omAlloc( (n+2) * sizeof(int) );
  eexp= (int *)omAlloc0((pVariables+1)*sizeof(int));

  totDeg= numSet0;

  mprSTICKYPROT2(" size of matrix: %d\n", E->num);
  mprSTICKYPROT2("  resultant deg: %d\n", numSet0);

  uRPos= new intvec( numSet0, pLength((gls->m)[0])+1, 0 );

  // sparse Matrix represented as a module where
  // each poly is column vector ( pSetComp(p,k) gives the row )
  rmat= idInit( E->num, E->num );    // cols, rank= number of rows
  msize= E->num;

  rp= 1;
  rowp= NULL;
  epp= pOne();
  for ( i= 1; i <= E->num; i++ )
  {       // for every row
    E->getRowMP( i, epp_mon );           // compute (p-a[ij]), (i,j) = RC(p)
    pSetExpV( epp, epp_mon );

    //
    rowp= ppMult_qq( epp, (gls->m)[(*E)[i]->rc.set] );  // x^(p-a[ij]) * f(i)

    cp= 2;
    // get column for every monomial in rowp and store it
    iterp= rowp;
    while ( iterp!=NULL )
    {
      epos= E->getExpPos( iterp );
      if ( epos == 0 )
      {
        // this can happen, if the shift vektor or the lift funktions
        // are not generically chosen.
        Werror("resMatrixSparse::createMatrix: Found exponent not in E, id %d, set [%d, %d]!",
               i,(*E)[i]->rc.set,(*E)[i]->rc.pnt);
        return i;
      }
      pSetExpV(iterp,eexp);
      pSetComp(iterp, epos );
      pSetm(iterp);
      if ( (*E)[i]->rc.set == linPolyS )
      { // store coeff positions
        IMATELEM(*uRPos,rp,cp)= epos;
        cp++;
      }
      pIter( iterp );
    } // while
    if ( (*E)[i]->rc.set == linPolyS )
    {   // store row
      IMATELEM(*uRPos,rp,1)= i-1;
      rp++;
    }
    (rmat->m)[i-1]= rowp;
  } // for

  pDelete( &epp );
  omFreeSize( (ADDRESS) epp_mon, (n+2) * sizeof(int) );
  omFreeSize( (ADDRESS) eexp, (pVariables+1)*sizeof(int));

#ifdef mprDEBUG_ALL
  if ( E->num <= 40 )
  {
    matrix mout= idModule2Matrix( idCopy(rmat) );
    print_matrix(mout);
  }
  for ( i= 1; i <= numSet0; i++ )
  {
    Print(" row  %d contains coeffs of f_%d\n",IMATELEM(*uRPos,i,1),linPolyS);
  }
  PrintS(" Sparse Matrix done\n");
#endif

  return E->num;
}

// find a sufficiently generic and small vector
void resMatrixSparse::randomVector( const int dim, mprfloat shift[] )
{
  int i,j;
  i= 1;

  while ( i <= dim )
  {
    shift[i]= (mprfloat) (RVMULT*(siRand()%MAXRVVAL)/(mprfloat)MAXRVVAL);
    i++;
    for ( j= 1; j < i-1; j++ )
    {
      if ( (shift[j] < shift[i-1] + SIMPLEX_EPS) && (shift[j] > shift[i-1] - SIMPLEX_EPS) )
      {
        i--;
        break;
      }
    }
  }
}

pointSet * resMatrixSparse::minkSumTwo( pointSet *Q1, pointSet *Q2, int dim )
{
  pointSet *vs;
  onePoint vert;
  int j,k,l;

  vert.point=(Coord_t*)omAlloc( (pVariables+2) * sizeof(Coord_t) );

  vs= new pointSet( dim );

  for ( j= 1; j <= Q1->num; j++ )
  {
    for ( k= 1; k <= Q2->num; k++ )
    {
      for ( l= 1; l <= dim; l++ )
      {
        vert.point[l]= (*Q1)[j]->point[l] + (*Q2)[k]->point[l];
      }
      vs->mergeWithExp( &vert );
      //vs->addPoint( &vert );
    }
  }

  omFreeSize( (ADDRESS) vert.point, (pVariables+2) * sizeof(Coord_t) );

  return vs;
}

pointSet * resMatrixSparse::minkSumAll( pointSet **pQ, int numq, int dim )
{
  pointSet *vs,*vs_old;
  int j;

  vs= new pointSet( dim );

  for ( j= 1; j <= pQ[0]->num; j++ ) vs->addPoint( (*pQ[0])[j] );

  for ( j= 1; j < numq; j++ )
  {
    vs_old= vs;
    vs= minkSumTwo( vs_old, pQ[j], dim );

    delete vs_old;
  }

  return vs;
}

//----------------------------------------------------------------------------------------

resMatrixSparse::resMatrixSparse( const ideal _gls, const int special )
  : resMatrixBase(), gls( _gls )
{
  pointSet **Qi; // vertices sets of Conv(Supp(f_i)), i=0..idelem
  pointSet *E;   // all integer lattice points of the minkowski sum of Q0...Qn
  int i,k;
  int pnt;
  int totverts;                // total number of exponent vectors in ideal gls
  mprfloat shift[MAXVARS+2];   // shiftvector delta, index [1..dim]

  if ( pVariables > MAXVARS )
  {
    WerrorS("resMatrixSparse::resMatrixSparse: Too many variables!");
    return;
  }

  rmat= NULL;
  numSet0= 0;

  if ( special == SNONE ) linPolyS= 0;
  else linPolyS= special;

  istate= resMatrixBase::ready;

  n= pVariables;
  idelem= IDELEMS(gls);  // should be n+1

  // prepare matrix LP->LiPM for Linear Programming
  totverts = 0;
  for( i=0; i < idelem; i++) totverts += pLength( (gls->m)[i] );

  LP = new simplex( idelem+totverts*2+5, totverts+5 ); // rows, cols

  // get shift vector
#ifdef mprTEST
  shift[0]=0.005; shift[1]=0.003; shift[2]=0.008; shift[3]=0.005; shift[4]=0.002;
  shift[5]=0.1; shift[6]=0.3; shift[7]=0.2; shift[8]=0.4; shift[9]=0.2;
#else
  randomVector( idelem, shift );
#endif
#ifdef mprDEBUG_PROT
  PrintS(" shift vector: ");
  for ( i= 1; i <= idelem; i++ ) Print(" %.12f ",(double)shift[i]);
  PrintLn();
#endif

  // evaluate convex hull for supports of gls
  convexHull chnp( LP );
  Qi= chnp.newtonPolytopesP( gls );

#ifdef mprMINKSUM
  E= minkSumAll( Qi, n+1, n);
#else
  // get inner points
  mayanPyramidAlg mpa( LP );
  E= mpa.getInnerPoints( Qi, shift );
#endif

#ifdef mprDEBUG_PROT
#ifdef mprMINKSUM
  PrintS("(MinkSum)");
#endif
  PrintS("\n E = (Q_0 + ... + Q_n) \\cap \\N :\n");
  for ( pnt= 1; pnt <= E->num; pnt++ )
  {
    Print("%d: <",pnt);print_exp( (*E)[pnt], E->dim );PrintS(">\n");
  }
  PrintLn();
#endif

#ifdef mprTEST
  int lift[5][5];
  lift[0][1]=3; lift[0][2]=4; lift[0][3]=8;  lift[0][4]=2;
  lift[1][1]=6; lift[1][2]=1; lift[1][3]=7;  lift[1][4]=4;
  lift[2][1]=2; lift[2][2]=5; lift[2][3]=9;  lift[2][4]=6;
  lift[3][1]=2; lift[3][2]=1; lift[3][3]=9;  lift[3][4]=5;
  lift[4][1]=3; lift[4][2]=7; lift[4][3]=1;  lift[4][4]=5;
  // now lift everything
  for ( i= 0; i <= n; i++ ) Qi[i]->lift( lift[i] );
#else
  // now lift everything
  for ( i= 0; i <= n; i++ ) Qi[i]->lift();
#endif
  E->dim++;

  // run Row Content Function for every point in E
  for ( pnt= 1; pnt <= E->num; pnt++ )
  {
    RC( Qi, E, pnt, shift );
  }

  // remove points not in cells
  k= E->num;
  for ( pnt= k; pnt > 0; pnt-- )
  {
    if ( (*E)[pnt]->rcPnt == NULL )
    {
      E->removePoint(pnt);
      mprSTICKYPROT(ST_SPARSE_RCRJ);
    }
  }
  mprSTICKYPROT("\n");

#ifdef mprDEBUG_PROT
  PrintS(" points which lie in a cell:\n");
  for ( pnt= 1; pnt <= E->num; pnt++ )
  {
    Print("%d: <",pnt);print_exp( (*E)[pnt], E->dim );PrintS(">\n");
  }
  PrintLn();
#endif

  // unlift to old dimension, sort
  for ( i= 0; i <= n; i++ ) Qi[i]->unlift();
  E->unlift();
  E->sort();

#ifdef mprDEBUG_PROT
  Print(" points with a[ij] (%d):\n",E->num);
  for ( pnt= 1; pnt <= E->num; pnt++ )
  {
    Print("Punkt p \\in E[%d]: <",pnt);print_exp( (*E)[pnt], E->dim );
    Print(">, RC(p) = (i:%d, j:%d), a[i,j] = <",(*E)[pnt]->rc.set,(*E)[pnt]->rc.pnt);
    //print_exp( (Qi[(*E)[pnt]->rc.set])[(*E)[pnt]->rc.pnt], E->dim );PrintS("> = <");
    print_exp( (*E)[pnt]->rcPnt, E->dim );PrintS(">\n");
  }
#endif

  // now create matrix
  if (E->num <1)
  {
    WerrorS("could not handle a degenerate situation: no inner points found");
    goto theEnd;
  }
  if ( createMatrix( E ) != E->num )
  {
    // this can happen if the shiftvector shift is to large or not generic
    istate= resMatrixBase::fatalError;
    WerrorS("resMatrixSparse::resMatrixSparse: Error in resMatrixSparse::createMatrix!");
    goto theEnd;
  }

 theEnd:
  // clean up
  for ( i= 0; i < idelem; i++ )
  {
    delete Qi[i];
  }
  omFreeSize( (ADDRESS) Qi, idelem * sizeof(pointSet*) );

  delete E;

  delete LP;
}

//----------------------------------------------------------------------------------------

resMatrixSparse::~resMatrixSparse()
{
  delete uRPos;
  idDelete( &rmat );
}

const ideal resMatrixSparse::getMatrix()
{
  int i,j,cp;
  poly pp,phelp,piter,pgls;

  // copy original sparse res matrix
  ideal rmat_out= idCopy(rmat);

  // now fill in coeffs of f0
  for ( i= 1; i <= numSet0; i++ )
  {

    pgls= (gls->m)[0]; // f0

    // get matrix row and delete it
    pp= (rmat_out->m)[IMATELEM(*uRPos,i,1)];
    pDelete( &pp );
    pp= NULL;
    phelp= pp;
    piter= NULL;

    // u_1,..,u_k
    cp=2;
    while ( pNext(pgls)!=NULL )
    {
      phelp= pOne();
      pSetCoeff( phelp, nCopy(pGetCoeff(pgls)) );
      pSetComp( phelp, IMATELEM(*uRPos,i,cp) );
      pSetmComp( phelp );
      if ( piter!=NULL )
      {
        pNext(piter)= phelp;
        piter= phelp;
      }
      else
      {
        pp= phelp;
        piter= phelp;
      }
      cp++;
      pIter( pgls );
    }
    // u0, now pgls points to last monom
    phelp= pOne();
    pSetCoeff( phelp, nCopy(pGetCoeff(pgls)) );
    //pSetComp( phelp, IMATELEM(*uRPos,i,idelem+1) );
    pSetComp( phelp, IMATELEM(*uRPos,i,pLength((gls->m)[0])+1) );
    pSetmComp( phelp );
    if (piter!=NULL) pNext(piter)= phelp;
    else pp= phelp;
    (rmat_out->m)[IMATELEM(*uRPos,i,1)]= pp;
  }

  return rmat_out;
}

// Fills in resMat[][] with evpoint[] and gets determinant
//    uRPos[i][1]: row of matrix
//    uRPos[i][idelem+1]: col of u(0)
//    uRPos[i][2..idelem]: col of u(1) .. u(n)
//    i= 1 .. numSet0
const number resMatrixSparse::getDetAt( const number* evpoint )
{
  int i,cp;
  poly pp,phelp,piter;

  mprPROTnl("smCallDet");

  for ( i= 1; i <= numSet0; i++ )
  {
    pp= (rmat->m)[IMATELEM(*uRPos,i,1)];
    pDelete( &pp );
    pp= NULL;
    phelp= pp;
    piter= NULL;
    // u_1,..,u_n
    for ( cp= 2; cp <= idelem; cp++ )
    {
      if ( !nIsZero(evpoint[cp-1]) )
      {
        phelp= pOne();
        pSetCoeff( phelp, nCopy(evpoint[cp-1]) );
        pSetComp( phelp, IMATELEM(*uRPos,i,cp) );
        pSetmComp( phelp );
        if ( piter )
        {
          pNext(piter)= phelp;
          piter= phelp;
        }
        else
        {
          pp= phelp;
          piter= phelp;
        }
      }
    }
    // u0
    phelp= pOne();
    pSetCoeff( phelp, nCopy(evpoint[0]) );
    pSetComp( phelp, IMATELEM(*uRPos,i,idelem+1) );
    pSetmComp( phelp );
    pNext(piter)= phelp;
    (rmat->m)[IMATELEM(*uRPos,i,1)]= pp;
  }

  mprSTICKYPROT(ST__DET); // 1

  poly pres= smCallDet( rmat );
  number numres= nCopy( pGetCoeff( pres ) );
  pDelete( &pres );

  mprSTICKYPROT(ST__DET); // 2

  return ( numres );
}

// Fills in resMat[][] with evpoint[] and gets determinant
//    uRPos[i][1]: row of matrix
//    uRPos[i][idelem+1]: col of u(0)
//    uRPos[i][2..idelem]: col of u(1) .. u(n)
//    i= 1 .. numSet0
const poly resMatrixSparse::getUDet( const number* evpoint )
{
  int i,cp;
  poly pp,phelp,piter;

  mprPROTnl("smCallDet");

  for ( i= 1; i <= numSet0; i++ )
  {
    pp= (rmat->m)[IMATELEM(*uRPos,i,1)];
    pDelete( &pp );
    phelp= NULL;
    piter= NULL;
    for ( cp= 2; cp <= idelem; cp++ )
    { // u1 .. un
      if ( !nIsZero(evpoint[cp-1]) )
      {
        phelp= pOne();
        pSetCoeff( phelp, nCopy(evpoint[cp-1]) );
        pSetComp( phelp, IMATELEM(*uRPos,i,cp) );
        //pSetmComp( phelp );
        pSetm( phelp );
        //Print("comp %d\n",IMATELEM(*uRPos,i,cp));
        #if 0
        if ( piter!=NULL )
        {
          pNext(piter)= phelp;
          piter= phelp;
        }
        else
        {
          pp= phelp;
          piter= phelp;
        }
        #else
        pp=pAdd(pp,phelp);
        #endif
      }
    }
    // u0
    phelp= pOne();
    pSetExp(phelp,1,1);
    pSetComp( phelp, IMATELEM(*uRPos,i,idelem+1) );
    //    Print("comp %d\n",IMATELEM(*uRPos,i,idelem+1));
    pSetm( phelp );
    #if 0
    pNext(piter)= phelp;
    #else
    pp=pAdd(pp,phelp);
    #endif
    pTest(pp);
    (rmat->m)[IMATELEM(*uRPos,i,1)]= pp;
  }

  mprSTICKYPROT(ST__DET); // 1

  poly pres= smCallDet( rmat );

  mprSTICKYPROT(ST__DET); // 2

  return ( pres );
}
//<-

//-----------------------------------------------------------------------------
//-------------- dense resultant matrix ---------------------------------------
//-----------------------------------------------------------------------------

//-> dense resultant matrix
//
class resVector;

/* dense resultant matrix */
class resMatrixDense : virtual public resMatrixBase
{
public:
  /**
   * _gls: system of multivariate polynoms
   * special: -1 -> resMatrixDense is a symbolic matrix
   *    0,1, ... -> resMatrixDense ist eine u-Resultante, wobei special das
   *                        lineare u-Polynom angibt
   */
  resMatrixDense( const ideal _gls, const int special = SNONE );
  ~resMatrixDense();

  /** column vector of matrix, index von 0 ... numVectors-1 */
  resVector *getMVector( const int i );

  /** Returns the matrix M in an usable presentation */
  const ideal getMatrix();

  /** Returns the submatrix M' of M in an usable presentation */
  const ideal getSubMatrix();

  /** Evaluate the determinant of the matrix M at the point evpoint
   * where the ui's are replaced by the components of evpoint.
   * Uses singclap_det from factory.
   */
  const number getDetAt( const number* evpoint );

  /** Evaluates the determinant of the submatrix M'.
   * Since the matrix is numerically, no evaluation point is needed.
   * Uses singclap_det from factory.
   */
  const number getSubDet();

private:
  /** deactivated copy constructor */
  resMatrixDense( const resMatrixDense & );

  /** Generate the "matrix" M. Each column is presented by a resVector
   * holding all entries for this column.
   */
  void generateBaseData();

  /** Generates needed set of monoms, split them into sets S0, ... Sn and
   * check if reduced/nonreduced and calculate size of submatrix.
   */
  void generateMonomData( int deg, intvec* polyDegs , intvec* iVO );

  /** Recursively generate all homogeneous monoms of
   * pVariables variables of degree deg.
   */
  void generateMonoms( poly m, int var, int deg );

  /** Creates quadratic matrix M of size numVectors for later use.
   * u0, u1, ...,un are replaced by 0.
   * Entries equal to 0 are not initialized ( == NULL)
   */
  void createMatrix();

private:
  resVector *resVectorList;

  int veclistmax;
  int veclistblock;
  int numVectors;
  int subSize;

  matrix m;
};
//<-

//-> struct resVector
/* Holds a row vector of the dense resultant matrix */
struct resVector
{
public:
  void init()
  {
    isReduced = FALSE;
    elementOfS = SFREE;
    mon = NULL;
  }
  void init( const poly m )
  {
    isReduced = FALSE;
    elementOfS = SFREE;
    mon = m;
  }

  /** index von 0 ... numVectors-1 */
  poly getElem( const int i );

  /** index von 0 ... numVectors-1 */
  number getElemNum( const int i );

  // variables
  poly mon;
  poly dividedBy;
  bool isReduced;

  /** number of the set S mon is element of */
  int elementOfS;

  /** holds the index of u0, u1, ..., un, if (elementOfS == linPolyS)
   *  the size is given by pVariables
   */
  int *numColParNr;

  /** holds the column vector if (elementOfS != linPolyS) */
  number *numColVector;

  /** size of numColVector */
  int numColVectorSize;

  number *numColVecCopy;
};
//<-

//-> resVector::*
poly resVector::getElem( const int i ) // inline ???
{
  assume( 0 < i || i > numColVectorSize );
  poly out= pOne();
  pSetCoeff( out, numColVector[i] );
  pTest( out );
  return( out );
}

number resVector::getElemNum( const int i ) // inline ??
{
  assume( i >= 0 && i < numColVectorSize );
  return( numColVector[i] );
}
//<-

//-> resMatrixDense::*
resMatrixDense::resMatrixDense( const ideal _gls, const int special )
  : resMatrixBase()
{
  int i;

  sourceRing=currRing;
  gls= idCopy( _gls );
  linPolyS= special;
  m=NULL;

  // init all
  generateBaseData();

  totDeg= 1;
  for ( i= 0; i < IDELEMS(gls); i++ )
  {
    totDeg*=pTotaldegree( (gls->m)[i] );
  }

  mprSTICKYPROT2("  resultant deg: %d\n",totDeg);

  istate= resMatrixBase::ready;
}

resMatrixDense::~resMatrixDense()
{
  int i,j;
  for (i=0; i < numVectors; i++)
  {
    pDelete( &resVectorList[i].mon );
    pDelete( &resVectorList[i].dividedBy );
    for ( j=0; j < resVectorList[i].numColVectorSize; j++ )
    {
        nDelete( resVectorList[i].numColVector+j );
    }
    // OB: ????? (solve_s.tst)
    if (resVectorList[i].numColVector!=NULL)
      omfreeSize( (ADDRESS)resVectorList[i].numColVector,
                numVectors * sizeof( number ) );
    if (resVectorList[i].numColParNr!=NULL)
      omfreeSize( (ADDRESS)resVectorList[i].numColParNr,
                (pVariables+1) * sizeof(int) );
  }

  omFreeSize( (ADDRESS)resVectorList, veclistmax*sizeof( resVector ) );

  // free matrix m
  if ( m != NULL )
  {
    idDelete((ideal *)&m);
  }
}

// mprSTICKYPROT:
// ST_DENSE_FR: found row S0
// ST_DENSE_NR: normal row
void resMatrixDense::createMatrix()
{
  int k,i,j;
  resVector *vecp;

  m= mpNew( numVectors, numVectors );

  for ( i= 1; i <= MATROWS( m ); i++ )
    for ( j= 1; j <= MATCOLS( m ); j++ )
    {
      MATELEM(m,i,j)= pInit();
      pSetCoeff0( MATELEM(m,i,j), nInit(0) );
    }


  for ( k= 0; k <= numVectors - 1; k++ )
  {
    if ( linPolyS == getMVector(k)->elementOfS )
    {
      mprSTICKYPROT(ST_DENSE_FR);
      for ( i= 0; i < pVariables; i++ )
      {
        MATELEM(m,numVectors-k,numVectors-(getMVector(k)->numColParNr)[i])= pInit();
      }
    }
    else
    {
      mprSTICKYPROT(ST_DENSE_NR);
      vecp= getMVector(k);
      for ( i= 0; i < numVectors; i++)
      {
        if ( !nIsZero( vecp->getElemNum(i) ) )
        {
          MATELEM(m,numVectors - k,i + 1)= pInit();
          pSetCoeff0( MATELEM(m,numVectors - k,i + 1), nCopy(vecp->getElemNum(i)) );
        }
      }
    }
  } // for
  mprSTICKYPROT("\n");

#ifdef mprDEBUG_ALL
  for ( k= numVectors - 1; k >= 0; k-- )
  {
    if ( linPolyS == getMVector(k)->elementOfS )
    {
      for ( i=0; i < pVariables; i++ )
      {
        Print(" %d ",(getMVector(k)->numColParNr)[i]);
      }
      PrintLn();
    }
  }
  for (i=1; i <= numVectors; i++)
  {
    for (j=1; j <= numVectors; j++ )
    {
      pWrite0(MATELEM(m,i,j));PrintS("  ");
    }
    PrintLn();
  }
#endif
}

// mprSTICKYPROT:
// ST_DENSE_MEM: more mem allocated
// ST_DENSE_NMON: new monom added
void resMatrixDense::generateMonoms( poly mm, int var, int deg )
{
  if ( deg == 0 )
  {
    poly mon = pCopy( mm );

    if ( numVectors == veclistmax )
    {
      resVectorList= (resVector * )omReallocSize( resVectorList,
                                            (veclistmax) * sizeof( resVector ),
                                            (veclistmax + veclistblock) * sizeof( resVector ) );
      int k;
      for ( k= veclistmax; k < (veclistmax + veclistblock); k++ )
        resVectorList[k].init();
      veclistmax+= veclistblock;
      mprSTICKYPROT(ST_DENSE_MEM);

    }
    resVectorList[numVectors].init( mon );
    numVectors++;
    mprSTICKYPROT(ST_DENSE_NMON);
    return;
  }
  else
  {
    if ( var == pVariables+1 ) return;
    poly newm = pCopy( mm );
    while ( deg >= 0 )
    {
      generateMonoms( newm, var+1, deg );
      pIncrExp( newm, var );
      pSetm( newm );
      deg--;
    }
    pDelete( & newm );
  }

  return;
}

void resMatrixDense::generateMonomData( int deg, intvec* polyDegs , intvec* iVO )
{
  int i,j,k;

  // init monomData
  veclistblock= 512;
  veclistmax= veclistblock;
  resVectorList= (resVector *)omAlloc( veclistmax*sizeof( resVector ) );

  // Init resVector()s
  for ( j= veclistmax - 1; j >= 0; j-- ) resVectorList[j].init();
  numVectors= 0;

  // Generate all monoms of degree deg
  poly start= pOne();
  generateMonoms( start, 1, deg );
  pDelete( & start );

  mprSTICKYPROT("\n");

  // Check for reduced monoms
  // First generate polyDegs.rows() monoms
  //  x(k)^(polyDegs[k]),  0 <= k < polyDegs.rows()
  ideal pDegDiv= idInit( polyDegs->rows(), 1 );
  for ( k= 0; k < polyDegs->rows(); k++ )
  {
    poly p= pOne();
    pSetExp( p, k + 1, (*polyDegs)[k] );
    pSetm( p );
    (pDegDiv->m)[k]= p;
  }

  // Now check each monom if it is reduced.
  // A monom monom is called reduced if there exists
  // exactly one x(k)^(polyDegs[k]) that divides the monom.
  int divCount;
  for ( j= numVectors - 1; j >= 0; j-- )
  {
    divCount= 0;
    for ( k= 0; k < IDELEMS(pDegDiv); k++ )
      if ( pLmDivisibleByNoComp( (pDegDiv->m)[k], resVectorList[j].mon ) )
        divCount++;
    resVectorList[j].isReduced= (divCount == 1);
  }

  // create the sets S(k)s
  // a monom x(i)^deg, deg given, is element of the set S(i)
  // if all x(0)^(polyDegs[0]) ... x(i-1)^(polyDegs[i-1]) DONT divide
  // x(i)^deg and only x(i)^(polyDegs[i]) divides x(i)^deg
  bool doInsert;
  for ( k= 0; k < iVO->rows(); k++)
  {
    //mprPROTInl(" ------------ var:",(*iVO)[k]);
    for ( j= numVectors - 1; j >= 0; j-- )
    {
      //mprPROTPnl("testing monom",resVectorList[j].mon);
      if ( resVectorList[j].elementOfS == SFREE )
      {
        //mprPROTnl("\tfree");
        if ( pLmDivisibleByNoComp( (pDegDiv->m)[ (*iVO)[k] ], resVectorList[j].mon ) )
        {
          //mprPROTPnl("\tdivisible by ",(pDegDiv->m)[ (*iVO)[k] ]);
          doInsert=TRUE;
          for ( i= 0; i < k; i++ )
          {
            //mprPROTPnl("\tchecking db ",(pDegDiv->m)[ (*iVO)[i] ]);
            if ( pLmDivisibleByNoComp( (pDegDiv->m)[ (*iVO)[i] ], resVectorList[j].mon ) )
            {
              //mprPROTPnl("\t and divisible by",(pDegDiv->m)[ (*iVO)[i] ]);
              doInsert=FALSE;
              break;
            }
          }
          if ( doInsert )
          {
            //mprPROTInl("\t------------------> S ",(*iVO)[k]);
            resVectorList[j].elementOfS= (*iVO)[k];
            resVectorList[j].dividedBy= pCopy( (pDegDiv->m)[ (*iVO)[i] ] );
          }
        }
      }
    }
  }

  // size of submatrix M', equal to number of nonreduced monoms
  // (size of matrix M is equal to number of monoms=numVectors)
  subSize= 0;
  int sub;
  for ( i= 0; i < polyDegs->rows(); i++ )
  {
    sub= 1;
    for ( k= 0; k < polyDegs->rows(); k++ )
      if ( i != k ) sub*= (*polyDegs)[k];
    subSize+= sub;
  }
  subSize= numVectors - subSize;

  // pDegDiv wieder freigeben!
  idDelete( &pDegDiv );

#ifdef mprDEBUG_ALL
  // Print a list of monoms and their properties
  PrintS("// \n");
  for ( j= numVectors - 1; j >= 0; j-- )
  {
    Print("// %s, S(%d),  db ",
          resVectorList[j].isReduced?"reduced":"nonreduced",
          resVectorList[j].elementOfS);
    pWrite0(resVectorList[j].dividedBy);
    PrintS("  monom ");
    pWrite(resVectorList[j].mon);
  }
  Print("// size: %d, subSize: %d\n",numVectors,subSize);
#endif
}

void resMatrixDense::generateBaseData()
{
  int k,j,i;
  number matEntry;
  poly pmatchPos;
  poly pi,factor,pmp;

  // holds the degrees of F0, F1, ..., Fn
  intvec polyDegs( IDELEMS(gls) );
  for ( k= 0; k < IDELEMS(gls); k++ )
    polyDegs[k]= pTotaldegree( (gls->m)[k] );

  // the internal Variable Ordering
  // make sure that the homogenization variable goes last!
  intvec iVO( pVariables );
  if ( linPolyS != SNONE )
  {
    iVO[pVariables - 1]= linPolyS;
    int p=0;
    for ( k= pVariables - 1; k >= 0; k-- )
    {
      if ( k != linPolyS )
      {
        iVO[p]= k;
        p++;
      }
    }
  }
  else
  {
    linPolyS= 0;
    for ( k= 0; k < pVariables; k++ )
      iVO[k]= pVariables - k - 1;
  }

  // the critical degree d= sum( deg(Fi) ) - n
  int sumDeg= 0;
  for ( k= 0; k < polyDegs.rows(); k++ )
    sumDeg+= polyDegs[k];
  sumDeg-= polyDegs.rows() - 1;

  // generate the base data
  generateMonomData( sumDeg, &polyDegs, &iVO );

  // generate "matrix"
  for ( k= numVectors - 1; k >= 0; k-- )
  {
    if ( resVectorList[k].elementOfS != linPolyS )
    {
      // column k is a normal column with numerical or symbolic entries
      // init stuff
      resVectorList[k].numColParNr= NULL;
      resVectorList[k].numColVectorSize= numVectors;
      resVectorList[k].numColVector= (number *)omAlloc( numVectors*sizeof( number ) );
      for ( i= 0; i < numVectors; i++ ) resVectorList[k].numColVector[i]= nInit(0);

      // compute row poly
      poly pi= ppMult_qq( (gls->m)[ resVectorList[k].elementOfS ] , resVectorList[k].mon );
      pi= pDivideM( pCopy( pi ), pCopy( resVectorList[k].dividedBy ) );

      // fill in "matrix"
      while ( pi != NULL )
      {
        matEntry= nCopy(pGetCoeff(pi));
        pmatchPos= pLmInit( pi );
        pSetCoeff0( pmatchPos, nInit(1) );

        for ( i= 0; i < numVectors; i++)
          if ( pLmEqual( pmatchPos, resVectorList[i].mon ) )
            break;

        resVectorList[k].numColVector[numVectors - i - 1] = nCopy(matEntry);

        pDelete( &pmatchPos );
        nDelete( &matEntry );

        pIter( pi );
      }
      pDelete( &pi );
    }
    else
    {
      // column is a special column, i.e. is generated by S0 and F0
      // safe only the positions of the ui's in the column
      //mprPROTInl(" setup of numColParNr ",k);
      resVectorList[k].numColVectorSize= 0;
      resVectorList[k].numColVector= NULL;
      resVectorList[k].numColParNr= (int *)omAlloc0( (pVariables+1) * sizeof(int) );

      pi= (gls->m)[ resVectorList[k].elementOfS ];
      factor= pDivideM( pCopy( resVectorList[k].mon ), pCopy( resVectorList[k].dividedBy ) );

      j=0;
      while ( pi  != NULL )
      { // fill in "matrix"
        pmp= pMult( pCopy( factor ), pHead( pi ) );
        pTest( pmp );

        for ( i= 0; i < numVectors; i++)
          if ( pLmEqual( pmp, resVectorList[i].mon ) )
            break;

        resVectorList[k].numColParNr[j]= i;
        pDelete( &pmp );
        pIter( pi );
        j++;
      }
      pDelete( &pi );
      pDelete( &factor );
    }
  } // for ( k= numVectors - 1; k >= 0; k-- )

  mprSTICKYPROT2(" size of matrix:    %d\n",numVectors);
  mprSTICKYPROT2(" size of submatrix: %d\n",subSize);

  // create the matrix M
  createMatrix();

}

resVector *resMatrixDense::getMVector(const int i)
{
  assume( i >= 0 && i < numVectors );
  return &resVectorList[i];
}

const ideal resMatrixDense::getMatrix()
{
  int i,j;

  // copy matrix
  matrix resmat= mpNew(numVectors,numVectors);
  poly p;
  for (i=1; i <= numVectors; i++)
  {
    for (j=1; j <= numVectors; j++ )
    {
      p=MATELEM(m,i,j);
      if (( p!=NULL)
      && (!nIsZero(pGetCoeff(p)))
      && (pGetCoeff(p)!=NULL)
      )
      {
        MATELEM(resmat,i,j)= pCopy( p );
      }
    }
  }
  for (i=0; i < numVectors; i++)
  {
    if ( resVectorList[i].elementOfS == linPolyS )
    {
      for (j=1; j <= pVariables; j++ )
      {
        if ( MATELEM(resmat,numVectors-i,
                     numVectors-resVectorList[i].numColParNr[j-1])!=NULL )
          pDelete( &MATELEM(resmat,numVectors-i,numVectors-resVectorList[i].numColParNr[j-1]) );
        MATELEM(resmat,numVectors-i,numVectors-resVectorList[i].numColParNr[j-1])= pOne();
        // FIX ME
        if ( FALSE )
        {
          pSetCoeff( MATELEM(resmat,numVectors-i,numVectors-resVectorList[i].numColParNr[j-1]), nPar(j) );
        }
        else
        {
          pSetExp( MATELEM(resmat,numVectors-i,numVectors-resVectorList[i].numColParNr[j-1]), j, 1 );
          pSetm(MATELEM(resmat,numVectors-i,numVectors-resVectorList[i].numColParNr[j-1]));
        }
      }
    }
  }

  // obachman: idMatrix2Module frees resmat !!
  ideal resmod= idMatrix2Module(resmat);
  return resmod;
}

const ideal resMatrixDense::getSubMatrix()
{
  int k,i,j,l;
  resVector *vecp;

  // generate quadratic matrix resmat of size subSize
  matrix resmat= mpNew( subSize, subSize );

  j=1;
  for ( k= numVectors - 1; k >= 0; k-- )
  {
    vecp= getMVector(k);
    if ( vecp->isReduced ) continue;
    l=1;
    for ( i= numVectors - 1; i >= 0; i-- )
    {
      if ( getMVector(i)->isReduced ) continue;
      if ( !nIsZero(vecp->getElemNum(numVectors - i - 1)) )
      {
        MATELEM(resmat,j,l)= pCopy( vecp->getElem(numVectors-i-1) );
      }
      l++;
    }
    j++;
  }

  // obachman: idMatrix2Module frees resmat !!
  ideal resmod= idMatrix2Module(resmat);
  return resmod;
}

const number resMatrixDense::getDetAt( const number* evpoint )
{
  int k,i;

  // copy evaluation point into matrix
  // p0, p1, ..., pn replace u0, u1, ..., un
  for ( k= numVectors - 1; k >= 0; k-- )
  {
    if ( linPolyS == getMVector(k)->elementOfS )
    {
      for ( i= 0; i < pVariables; i++ )
      {
        pSetCoeff( MATELEM(m,numVectors-k,numVectors-(getMVector(k)->numColParNr)[i]),
                   nCopy(evpoint[i]) );
      }
    }
  }

  mprSTICKYPROT(ST__DET);

  // evaluate determinant of matrix m using factory singclap_det
#ifdef HAVE_FACTORY
  poly res= singclap_det( m );
#else
  poly res= NULL;
#endif

  // avoid errors for det==0
  number numres;
  if ( (res!=NULL)  && (!nIsZero(pGetCoeff( res ))) )
  {
    numres= nCopy( pGetCoeff( res ) );
  }
  else
  {
    numres= nInit(0);
    mprPROT("0");
  }
  pDelete( &res );

  mprSTICKYPROT(ST__DET);

  return( numres );
}

const number resMatrixDense::getSubDet()
{
  int k,i,j,l;
  resVector *vecp;

  // generate quadratic matrix mat of size subSize
  matrix mat= mpNew( subSize, subSize );

  for ( i= 1; i <= MATROWS( mat ); i++ )
  {
    for ( j= 1; j <= MATCOLS( mat ); j++ )
    {
      MATELEM(mat,i,j)= pInit();
      pSetCoeff0( MATELEM(mat,i,j), nInit(0) );
    }
  }
  j=1;
  for ( k= numVectors - 1; k >= 0; k-- )
  {
    vecp= getMVector(k);
    if ( vecp->isReduced ) continue;
    l=1;
    for ( i= numVectors - 1; i >= 0; i-- )
    {
      if ( getMVector(i)->isReduced ) continue;
      if ( vecp->getElemNum(numVectors - i - 1) && !nIsZero(vecp->getElemNum(numVectors - i - 1)) )
      {
        pSetCoeff(MATELEM(mat, j , l ), nCopy(vecp->getElemNum(numVectors - i - 1)));
      }
      /* else
      {
           MATELEM(mat, j , l )= pOne();
           pSetCoeff(MATELEM(mat, j , l ), nInit(0) );
      }
      */
      l++;
    }
    j++;
  }

#ifdef HAVE_FACTORY
  poly res= singclap_det( mat );
#else
  poly res= NULL;
#endif

  number numres;
  if ((res != NULL) && (!nIsZero(pGetCoeff( res ))) )
  {
    numres= nCopy(pGetCoeff( res ));
  }
  else
  {
    numres= nInit(0);
  }
  pDelete( &res );
  return numres;
}
//<--

//-----------------------------------------------------------------------------
//-------------- uResultant ---------------------------------------------------
//-----------------------------------------------------------------------------

#define MAXEVPOINT 1000000 // 0x7fffffff
//#define MPR_MASI

//-> unsigned long over(unsigned long n,unsigned long d)
// Calculates (n+d \over d) using gmp functionality
//
unsigned long over( const unsigned long n , const unsigned long d )
{ // (d+n)! / ( d! n! )
  mpz_t res;
  mpz_init(res);
  mpz_t m,md,mn;
  mpz_init(m);mpz_set_ui(m,1);
  mpz_init(md);mpz_set_ui(md,1);
  mpz_init(mn);mpz_set_ui(mn,1);

  mpz_fac_ui(m,n+d);
  mpz_fac_ui(md,d);
  mpz_fac_ui(mn,n);

  mpz_mul(res,md,mn);
  mpz_tdiv_q(res,m,res);

  mpz_clear(m);mpz_clear(md);mpz_clear(mn);

  unsigned long result = mpz_get_ui(res);
  mpz_clear(res);

  return result;
}
//<-

//-> uResultant::*
uResultant::uResultant( const ideal _gls, const resMatType _rmt, BOOLEAN extIdeal )
  : rmt( _rmt )
{
  if ( extIdeal )
  {
    // extend given ideal by linear poly F0=u0x0 + u1x1 +...+ unxn
    gls= extendIdeal( _gls, linearPoly( rmt ), rmt );
    n= IDELEMS( gls );
  }
  else
    gls= idCopy( _gls );

  switch ( rmt )
  {
  case sparseResMat:
    resMat= new resMatrixSparse( gls );
    break;
  case denseResMat:
#ifdef HAVE_FACTORY
    resMat= new resMatrixDense( gls );
    break;
#endif
  default:
    WerrorS("uResultant::uResultant: Unknown resultant matrix type chosen!");
  }
}

uResultant::~uResultant( )
{
  delete resMat;
}

ideal uResultant::extendIdeal( const ideal igls, poly linPoly, const resMatType rrmt )
{
  ideal newGls= idCopy( igls );
  newGls->m= (poly *)omReallocSize( newGls->m,
                              IDELEMS(igls) * sizeof(poly),
                              (IDELEMS(igls) + 1) * sizeof(poly) );
  IDELEMS(newGls)++;

  switch ( rrmt )
  {
  case sparseResMat:
  case denseResMat:
    {
      int i;
      for ( i= IDELEMS(newGls)-1; i > 0; i-- )
      {
        newGls->m[i]= newGls->m[i-1];
      }
      newGls->m[0]= linPoly;
    }
    break;
  default:
    WerrorS("uResultant::extendIdeal: Unknown resultant matrix type chosen!");
  }

  return( newGls );
}

poly uResultant::linearPoly( const resMatType rrmt )
{
  int i;

  poly newlp= pOne();
  poly actlp, rootlp= newlp;

  for ( i= 1; i <= pVariables; i++ )
  {
    actlp= newlp;
    pSetExp( actlp, i, 1 );
    pSetm( actlp );
    newlp= pOne();
    actlp->next= newlp;
  }
  actlp->next= NULL;
  pDelete( &newlp );

  if ( rrmt == sparseResMat )
  {
    newlp= pOne();
    actlp->next= newlp;
    newlp->next= NULL;
  }
  return ( rootlp );
}

poly uResultant::interpolateDense( const number subDetVal )
{
  int i,j,p;
  long tdg;

  // D is a Polynom homogeneous in the coeffs of F0 and of degree tdg = d0*d1*...*dn
  tdg= resMat->getDetDeg();

  // maximum number of terms in polynom D (homogeneous, of degree tdg)
  // long mdg= (facul(tdg+n-1) / facul( tdg )) / facul( n - 1 );
  long mdg= over( n-1, tdg );

  // maximal number of terms in a polynom of degree tdg
  long l=(long)pow( (double)(tdg+1), n );

#ifdef mprDEBUG_PROT
  Print("// total deg of D: tdg %ld\n",tdg);
  Print("// maximum number of terms in D: mdg: %ld\n",mdg);
  Print("// maximum number of terms in polynom of deg tdg: l %ld\n",l);
#endif

  // we need mdg results of D(p0,p1,...,pn)
  number *presults;
  presults= (number *)omAlloc( mdg * sizeof( number ) );
  for (i=0; i < mdg; i++) presults[i]= nInit(0);

  number *pevpoint= (number *)omAlloc( n * sizeof( number ) );
  number *pev= (number *)omAlloc( n * sizeof( number ) );
  for (i=0; i < n; i++) pev[i]= nInit(0);

  mprPROTnl("// initial evaluation point: ");
  // initial evaluatoin point
  p=1;
  for (i=0; i < n; i++)
  {
    // init pevpoint with primes 3,5,7,11, ...
    p= nextPrime( p );
    pevpoint[i]=nInit( p );
    nTest(pevpoint[i]);
    mprPROTNnl(" ",pevpoint[i]);
  }

  // evaluate the determinant in the points pev^0, pev^1, ..., pev^mdg
  mprPROTnl("// evaluating:");
  for ( i=0; i < mdg; i++ )
  {
    for (j=0; j < n; j++)
    {
      nDelete( &pev[j] );
      nPower(pevpoint[j],i,&pev[j]);
      mprPROTN(" ",pev[j]);
    }
    mprPROTnl("");

    nDelete( &presults[i] );
    presults[i]=resMat->getDetAt( pev );

    mprSTICKYPROT(ST_BASE_EV);
  }
  mprSTICKYPROT("\n");

  // now interpolate using vandermode interpolation
  mprPROTnl("// interpolating:");
  number *ncpoly;
  {
    vandermonde vm( mdg, n, tdg, pevpoint );
    ncpoly= vm.interpolateDense( presults );
  }

  if ( subDetVal != NULL )
  {   // divide by common factor
    number detdiv;
    for ( i= 0; i <= mdg; i++ )
    {
      detdiv= nDiv( ncpoly[i], subDetVal );
      nNormalize( detdiv );
      nDelete( &ncpoly[i] );
      ncpoly[i]= detdiv;
    }
  }

#ifdef mprDEBUG_ALL
  PrintLn();
  for ( i=0; i < mdg; i++ )
  {
    nPrint(ncpoly[i]); PrintS(" --- ");
  }
  PrintLn();
#endif

  // prepare ncpoly for later use
  number nn=nInit(0);
  for ( i=0; i < mdg; i++ )
  {
    if ( nEqual(ncpoly[i],nn) )
    {
      nDelete( &ncpoly[i] );
      ncpoly[i]=NULL;
    }
  }
  nDelete( &nn );

  // create poly presenting the determinat of the uResultant
  intvec exp( n );
  for ( i= 0; i < n; i++ ) exp[i]=0;

  poly result= NULL;

  long sum=0;
  long c=0;

  for ( i=0; i < l; i++ )
  {
    if ( sum == tdg )
    {
      if ( !nIsZero(ncpoly[c]) )
      {
        poly p= pOne();
        if ( rmt == denseResMat )
        {
          for ( j= 0; j < n; j++ ) pSetExp( p, j+1, exp[j] );
        }
        else if ( rmt == sparseResMat )
        {
          for ( j= 1; j < n; j++ ) pSetExp( p, j, exp[j] );
        }
        pSetCoeff( p, ncpoly[c] );
        pSetm( p );
        if (result!=NULL) result= pAdd( result, p );
        else result=  p;
      }
      c++;
    }
    sum=0;
    exp[0]++;
    for ( j= 0; j < n - 1; j++ )
    {
      if ( exp[j] > tdg )
      {
        exp[j]= 0;
        exp[j + 1]++;
      }
      sum+=exp[j];
    }
    sum+=exp[n-1];
  }

  pTest( result );

  return result;
}

rootContainer ** uResultant::interpolateDenseSP( BOOLEAN matchUp, const number subDetVal )
{
  int i,p,uvar;
  long tdg;
  int loops= (matchUp?n-2:n-1);

  mprPROTnl("uResultant::interpolateDenseSP");

  tdg= resMat->getDetDeg();

  // evaluate D in tdg+1 distinct points, so
  // we need tdg+1 results of D(p0,1,0,...,0) =
  //              c(0)*u0^tdg + c(1)*u0^tdg-1 + ... + c(tdg-1)*u0 + c(tdg)
  number *presults;
  presults= (number *)omAlloc( (tdg + 1) * sizeof( number ) );
  for ( i=0; i <= tdg; i++ ) presults[i]= nInit(0);

  rootContainer ** roots;
  roots= (rootContainer **) omAlloc( loops * sizeof(rootContainer*) );
  for ( i=0; i < loops; i++ ) roots[i]= new rootContainer(); // 0..n-2

  number *pevpoint= (number *)omAlloc( n * sizeof( number ) );
  for (i=0; i < n; i++) pevpoint[i]= nInit(0);

  number *pev= (number *)omAlloc( n * sizeof( number ) );
  for (i=0; i < n; i++) pev[i]= nInit(0);

  // now we evaluate D(u0,-1,0,...0), D(u0,0,-1,0,...,0), ..., D(u0,0,..,0,-1)
  // or D(u0,k1,k2,0,...,0), D(u0,k1,k2,k3,0,...,0), ..., D(u0,k1,k2,k3,...,kn)
  // this gives us n-1 evaluations
  p=3;
  for ( uvar= 0; uvar < loops; uvar++ )
  {
    // generate initial evaluation point
    if ( matchUp )
    {
      for (i=0; i < n; i++)
      {
        // prime(random number) between 1 and MAXEVPOINT
        nDelete( &pevpoint[i] );
        if ( i == 0 )
        {
          //p= nextPrime( p );
          pevpoint[i]= nInit( p );
        }
        else if ( i <= uvar + 2 )
        {
          pevpoint[i]=nInit(1+siRand()%MAXEVPOINT);
          //pevpoint[i]=nInit(383);
        }
        else
          pevpoint[i]=nInit(0);
        mprPROTNnl(" ",pevpoint[i]);
      }
    }
    else
    {
      for (i=0; i < n; i++)
      {
        // init pevpoint with  prime,0,...0,1,0,...,0
        nDelete( &pevpoint[i] );
        if ( i == 0 )
        {
          //p=nextPrime( p );
          pevpoint[i]=nInit( p );
        }
        else
        {
          if ( i == (uvar + 1) ) pevpoint[i]= nInit(-1);
          else pevpoint[i]= nInit(0);
        }
        mprPROTNnl(" ",pevpoint[i]);
      }
    }

    // prepare aktual evaluation point
    for (i=0; i < n; i++)
    {
      nDelete( &pev[i] );
      pev[i]= nCopy( pevpoint[i] );
    }
    // evaluate the determinant in the points pev^0, pev^1, ..., pev^tdg
    for ( i=0; i <= tdg; i++ )
    {
      nDelete( &pev[0] );
      nPower(pevpoint[0],i,&pev[0]);          // new evpoint

      nDelete( &presults[i] );
      presults[i]=resMat->getDetAt( pev );   // evaluate det at point evpoint

      mprPROTNnl("",presults[i]);

      mprSTICKYPROT(ST_BASE_EV);
      mprPROTL("",tdg-i);
    }
    mprSTICKYPROT("\n");

    // now interpolate
    vandermonde vm( tdg + 1, 1, tdg, pevpoint, FALSE );
    number *ncpoly= vm.interpolateDense( presults );

    if ( subDetVal != NULL )
    {  // divide by common factor
      number detdiv;
      for ( i= 0; i <= tdg; i++ )
      {
        detdiv= nDiv( ncpoly[i], subDetVal );
        nNormalize( detdiv );
        nDelete( &ncpoly[i] );
        ncpoly[i]= detdiv;
      }
    }

#ifdef mprDEBUG_ALL
    PrintLn();
    for ( i=0; i <= tdg; i++ )
    {
      nPrint(ncpoly[i]); PrintS(" --- ");
    }
    PrintLn();
#endif

    // save results
    roots[uvar]->fillContainer( ncpoly, pevpoint, uvar+1, tdg,
                                (matchUp?rootContainer::cspecialmu:rootContainer::cspecial),
                                loops );
  }

  // free some stuff: pev, presult
  for ( i=0; i < n; i++ ) nDelete( pev + i );
  omFreeSize( (ADDRESS)pev, n * sizeof( number ) );

  for ( i=0; i <= tdg; i++ ) nDelete( presults+i );
  omFreeSize( (ADDRESS)presults, (tdg + 1) * sizeof( number ) );

  return roots;
}

rootContainer ** uResultant::specializeInU( BOOLEAN matchUp, const number subDetVal )
{
  int i,p,uvar;
  long tdg;
  poly pures,piter;
  int loops=(matchUp?n-2:n-1);
  int nn=n;
  if (loops==0) { loops=1;nn++;}

  mprPROTnl("uResultant::specializeInU");

  tdg= resMat->getDetDeg();

  rootContainer ** roots;
  roots= (rootContainer **) omAlloc( loops * sizeof(rootContainer*) );
  for ( i=0; i < loops; i++ ) roots[i]= new rootContainer(); // 0..n-2

  number *pevpoint= (number *)omAlloc( nn * sizeof( number ) );
  for (i=0; i < nn; i++) pevpoint[i]= nInit(0);

  // now we evaluate D(u0,-1,0,...0), D(u0,0,-1,0,...,0), ..., D(u0,0,..,0,-1)
  // or D(u0,k1,k2,0,...,0), D(u0,k1,k2,k3,0,...,0), ..., D(u0,k1,k2,k3,...,kn)
  p=3;
  for ( uvar= 0; uvar < loops; uvar++ )
  {
    // generate initial evaluation point
    if ( matchUp )
    {
      for (i=0; i < n; i++)
      {
        // prime(random number) between 1 and MAXEVPOINT
        nDelete( &pevpoint[i] );
        if ( i <= uvar + 2 )
        {
          pevpoint[i]=nInit(1+siRand()%MAXEVPOINT);
          //pevpoint[i]=nInit(383);
        }
        else pevpoint[i]=nInit(0);
        mprPROTNnl(" ",pevpoint[i]);
      }
    }
    else
    {
      for (i=0; i < n; i++)
      {
        // init pevpoint with  prime,0,...0,-1,0,...,0
        nDelete( &(pevpoint[i]) );
        if ( i == (uvar + 1) ) pevpoint[i]= nInit(-1);
        else pevpoint[i]= nInit(0);
        mprPROTNnl(" ",pevpoint[i]);
      }
    }

    pures= resMat->getUDet( pevpoint );

    number *ncpoly= (number *)omAlloc( (tdg+1) * sizeof(number) );

#ifdef MPR_MASI
    BOOLEAN masi=true;
#endif

    piter= pures;
    for ( i= tdg; i >= 0; i-- )
    {
      //if ( piter ) Print("deg %d, pDeg(piter) %d\n",i,pTotaldegree(piter));
      if ( piter && pTotaldegree(piter) == i )
      {
        ncpoly[i]= nCopy( pGetCoeff( piter ) );
        pIter( piter );
#ifdef MPR_MASI
        masi=false;
#endif
      }
      else
      {
        ncpoly[i]= nInit(0);
      }
      mprPROTNnl("", ncpoly[i] );
    }
#ifdef MPR_MASI
    if ( masi ) mprSTICKYPROT("MASI");
#endif

    mprSTICKYPROT(ST_BASE_EV); // .

    if ( subDetVal != NULL )  // divide by common factor
    {
      number detdiv;
      for ( i= 0; i <= tdg; i++ )
      {
        detdiv= nDiv( ncpoly[i], subDetVal );
        nNormalize( detdiv );
        nDelete( &ncpoly[i] );
        ncpoly[i]= detdiv;
      }
    }

    pDelete( &pures );

    // save results
    roots[uvar]->fillContainer( ncpoly, pevpoint, uvar+1, tdg,
                                (matchUp?rootContainer::cspecialmu:rootContainer::cspecial),
                                loops );
  }

  mprSTICKYPROT("\n");

  // free some stuff: pev, presult
  for ( i=0; i < n; i++ ) nDelete( pevpoint + i );
  omFreeSize( (ADDRESS)pevpoint, n * sizeof( number ) );

  return roots;
}

int uResultant::nextPrime( const int i )
{
  int init=i;
  int ii=i+2;
  int j= IsPrime( ii );
  while ( j <= init )
  {
    ii+=2;
    j= IsPrime( ii );
  }
  return j;
}
//<-

//-----------------------------------------------------------------------------

//-> loNewtonPolytope(...)
ideal loNewtonPolytope( const ideal id )
{
  simplex * LP;
  int i;
  int n,totverts,idelem;
  ideal idr;

  n= pVariables;
  idelem= IDELEMS(id);  // should be n+1

  totverts = 0;
  for( i=0; i < idelem; i++) totverts += pLength( (id->m)[i] );

  LP = new simplex( idelem+totverts*2+5, totverts+5 ); // rows, cols

  // evaluate convex hull for supports of id
  convexHull chnp( LP );
  idr = chnp.newtonPolytopesI( id );

  delete LP;

  return idr;
}
//<-

//%e

//-----------------------------------------------------------------------------

// local Variables: ***
// folded-file: t ***
// compile-command-1: "make installg" ***
// compile-command-2: "make install" ***
// End: ***

// in folding: C-c x
// leave fold: C-c y
//   foldmode: F10
