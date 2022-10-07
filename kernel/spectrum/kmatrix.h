// ----------------------------------------------------------------------------
//  kmatrix.h
//  begin of file
//  Stephan Endrass, endrass@mathematik.uni-mainz.de
//  23.7.99
// ----------------------------------------------------------------------------

#ifndef KMATRIX_H
#define KMATRIX_H
#include <stdlib.h>

// ----------------------------------------------------------------------------
//  template class for matrices with coefficients in the field  K
//  K is a class representing elements of a field
//  The implementation of  K  is expected to have overloaded
//  the operators +, -, *, /, +=, -=, *= and /=.
//  The expressions  (K)0  and (K)1  should cast to the  0  and  1  of  K.
//  Additionally we use the following functions in class K:
//
//    member functions:
//
//      double  complexity( void );
//
//    friend functions:
//
//      friend  K   gcd( const K &a,const K &b );  // gcd(a,b)
//      friend  K   gcd( K* a,int k );             // gcd(a[0],...,a[k-1])
//
//  The complexity function should return a measure indicating
//  how complicated this number is in terms of memory usage
//  and arithmetic operations. For a rational p/q, one could
//  return max(|p|,|q|). This function is used for pivoting.
//
//  The gcd of two numbers a,b should be a number g such that
//  the complexities of a/g and b/g are less or equal than those
//  of a and b. For rationals p1/q1, p2/q2 one could return the
//  quotient of integer gcd's gcd(p1,p2)/gcd(q1,q2).
//
// ----------------------------------------------------------------------------

template<class K> class KMatrix
{

private:

    K    *a;                                // the entries of the matrix
    int rows;                               // number of rows
    int cols;                               // number of columns

public:

    KMatrix( );                             // init zero
    KMatrix( const KMatrix& );              // copy constructor
    KMatrix( int,int );                     // preallocate rows & columns

    ~KMatrix( );                            // destructor

    void    copy_delete ( void );           // delete associated memory
    void    copy_new    ( int  );           // allocate associated memory
    void    copy_zero   ( void );           // init zero
    void    copy_unit   ( int );            // init as unit matrix
    void    copy_shallow( KMatrix& );       // shallow copy
    void    copy_deep   ( const KMatrix& ); // deep copy

    K       get( int,int ) const;           // get an element
    void    set( int,int,const K& );        // set an element

    int     row_is_zero( int ) const;       // test if row is zero
    int     column_is_zero( int ) const;    // test if column is zero

    int     column_pivot( int,int ) const;

    int     gausseliminate( void );         // Gauss elimination
    int     rank( void ) const;             // compute the rank
    int     solve( K**,int* );              // solve Ax=b from (A|b)

    // elementary transformations

    K       multiply_row( int,const K& );
    K       add_rows( int,int,const K&,const K& );
    int     swap_rows( int,int );
    K       set_row_primitive( int );

    int     is_quadratic( void ) const;
    int     is_symmetric( void ) const;

    K       determinant( void ) const;

    #ifdef KMATRIX_DEBUG
        void    test_row( int ) const;
        void    test_col( int ) const;
    #endif

    #ifdef KMATRIX_PRINT
        friend  ostream & operator << ( ostream&,const KMatrix& );
    #endif
};

// ------------------------------------
//  inline functions for class KMatrix
// ------------------------------------

// ----------------------------------------------------------------------------
//  Delete memory associated to a  KMatrix
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_delete( void )
{
    if( a != (K*)NULL && rows > 0 && cols > 0 ) delete [] a;
    copy_zero( );
}

// ----------------------------------------------------------------------------
//  Allocate memory associated to a  KMatrix
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_new( int k )
{
    if( k > 0 )
    {
        a = new K[k];

        #ifndef SING_NDEBUG
        if( a == (K*)NULL )
        {
            #ifdef KMATRIX_PRINT
            #ifdef KMATRIX_IOSTREAM
                cerr << "void KMatrix::copy_new( int k )";
                cerr << ": no memory left ..." << endl;
            #else
                fprintf( stderr,"void KMatrix::copy_new( int k )" );
                fprintf( stderr,": no memory left ...\n" );
            #endif
            #endif

            exit( 1 );
        }
        #endif
    }
    else if( k == 0 )
    {
        a = (K*)NULL;
    }
    else
    {
        #ifdef KMATRIX_PRINT
        #ifdef KMATRIX_IOSTREAM
            cerr << "void KMatrix::copy_new( int k )";
            cerr << ": k < 0 ..." << endl;
        #else
            fprintf( stderr,"void KMatrix::copy_new( int k )" );
            fprintf( stderr,": k < 0 ...\n" );
        #endif
        #endif

        exit( 1 );
    }
}

// ----------------------------------------------------------------------------
//  Initialize a  KMatrix  with  0
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_zero( void )
{
    a = (K*)NULL;
    rows = cols = 0;
}

// ----------------------------------------------------------------------------
//  Initialize a  KMatrix  with the unit matrix
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_unit( int rank )
{
    int r,n=rank*rank;
    copy_new( n );
    rows = cols = rank;

    for( r=0; r<n; a[r++]=(K)0 );

    for( r=0; r<rows; r++ )
    {
        a[r*cols+r] = (K)1;
    }
}

// ----------------------------------------------------------------------------
//  Shallow copy
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_shallow( KMatrix &m )
{
    a = m.a;
    rows = m.rows;
    cols = m.cols;
}

// ----------------------------------------------------------------------------
//  Deep copy
// ----------------------------------------------------------------------------

template<class K>
    inline  void    KMatrix<K>::copy_deep( const KMatrix &m )
{
    if( m.a == (K*)NULL )
    {
        copy_zero( );
    }
    else
    {
        int n=m.rows*m.cols;
        copy_new( n );
        rows = m.rows;
        cols = m.cols;

        for( int i=0; i<n; i++ )
        {
            a[i] = m.a[i];
        }
    }
}

// ----------------------------------------------------------------------------
//  Zero constructor
// ----------------------------------------------------------------------------

template<class K>
    inline  KMatrix<K>::KMatrix( )
{
    copy_zero( );
}

// ----------------------------------------------------------------------------
//  Copy constructor
// ----------------------------------------------------------------------------

template<class K>
    inline  KMatrix<K>::KMatrix( const KMatrix &m )
{
    copy_deep( m );
}

// ----------------------------------------------------------------------------
//  Zero r by c matrix constructor
// ----------------------------------------------------------------------------

template<class K>
    KMatrix<K>::KMatrix( int r,int c )
{
    int n = r*c;

    copy_new( n );

    rows = r;
    cols = c;

    for( int i=0; i<n; i++ )
    {
        a[i]=(K)0;
    }
}

// ----------------------------------------------------------------------------
//  Destructor
// ----------------------------------------------------------------------------

template<class K>
    KMatrix<K>::~KMatrix( )
{
    copy_delete( );
}

// -------------------------------------------------
//  non-inline template functions for class KMatrix
// -------------------------------------------------

// ----------------------------------------------------------------------------
//  Debugging functions
// ----------------------------------------------------------------------------

#ifdef KMATRIX_DEBUG

template<class K>
    void    KMatrix<K>::test_row( int r ) const
{
    if( r<0 || r>=rows )
    {
        #ifdef KMATRIX_PRINT
        #ifdef KMATRIX_IOSTREAM
            cerr << "KMatrix<K>::test_row( " << r << " )" << endl;
            cerr << "    rows = " << rows << endl;
            cerr << "    exiting...." << endl;
        #else
            fprintf( stderr,"KMatrix<K>::test_row( %d )\n",r );
            fprintf( stderr,"    rows = %d\n",rows );
            fprintf( stderr,"    exiting....\n" );
        #endif
        #endif

        exit( 1 );
    }
}

template<class K>
    void    KMatrix<K>::test_col( int c ) const
{
    if( c<0 || c>=cols )
    {
        #ifdef KMATRIX_PRINT
        #ifdef KMATRIX_IOSTREAM
            cerr << "KMatrix<K>::test_col( " << c << " )" << endl;
            cerr << "    cols = " << cols << endl;
            cerr << "    exiting...." << endl;
        #else
            fprintf( stderr,"KMatrix<K>::test_col( %d )\n",c );
            fprintf( stderr,"    cols = %d\n",cols );
            fprintf( stderr,"    exiting....\n" );
        #endif
        #endif

        exit( 1 );
    }
}

#endif

// ----------------------------------------------------------------------------
//  get coefficient at row  r  and column  c
//  return value: the coefficient
// ----------------------------------------------------------------------------

template<class K>
    K    KMatrix<K>::get( int r,int c ) const
{
    #ifdef KMATRIX_DEBUG
        test_row( r );
        test_col( c );
    #endif

    return a[r*cols+c];
}

// ----------------------------------------------------------------------------
//  sets coefficient at row  r  and column  c  to  value
// ----------------------------------------------------------------------------

template<class K>
    void    KMatrix<K>::set( int r,int c,const K &value )
{
    #ifdef KMATRIX_DEBUG
        test_row( r );
        test_col( c );
    #endif

    a[r*cols+c] = value;
}

// ----------------------------------------------------------------------------
//  interchanges the rows  r1  and  r2
//  return value:  1 if r1==r2
//  return value: -1 if r1!=r2
//  caution: the determinant changes its sign by the return value
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::swap_rows( int r1,int r2 )
{
    #ifdef KMATRIX_DEBUG
        test_row( r1 );
        test_row( r2 );
    #endif

    if( r1 == r2 ) return 1;

    K   tmp;

    for( int c=0; c<cols; c++ )
    {
        tmp          = a[r1*cols+c];
        a[r1*cols+c] = a[r2*cols+c];
        a[r2*cols+c] = tmp;
    }

    return -1;
}

// ----------------------------------------------------------------------------
//  replaces row  r  by its multiple (row r)*factor
//  return value: factor
//  caution: the determinant changes by the return value
// ----------------------------------------------------------------------------

template<class K>
    K    KMatrix<K>::multiply_row( int r,const K &factor )
{
    #ifdef KMATRIX_DEBUG
        test_row( r );
    #endif

    int i_src = r*cols;

    for( int i=0; i<cols; i++,i_src++ )
    {
        a[i_src] *= factor;
    }
    return  factor;
}

// ----------------------------------------------------------------------------
//  replaces row  dest  by the linear combination
//      (row src)*factor_src + (row dest)*factor_dest
//  return value: factor_dest
//  caution: the determinant changes by the return value
// ----------------------------------------------------------------------------

template<class K>
    K   KMatrix<K>::add_rows(
        int src,int dest,const K &factor_src,const K &factor_dest )
{
    #ifdef KMATRIX_DEBUG
        test_row( src  );
        test_row( dest );
    #endif

    int i;
    int i_src  = src*cols;
    int i_dest = dest*cols;

    for( i=0; i<cols; i++,i_src++,i_dest++ )
    {
        a[i_dest] = a[i_src]*factor_src + a[i_dest]*factor_dest;
    }

    return factor_dest;
}

// ----------------------------------------------------------------------------
//  test if row  r  is zero
//  return value: TRUE  if zero
//                FALSE if not zero
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::row_is_zero( int r ) const
{
    #ifdef KMATRIX_DEBUG
        test_row( r );
    #endif

    for( int c=0; c<cols; c++ )
    {
        if( a[r*cols+c] != (K)0 ) return FALSE;
    }
    return TRUE;
}

// ----------------------------------------------------------------------------
//  test if column  c  is zero
//  return value: TRUE  if zero
//                FALSE if not zero
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::column_is_zero( int c ) const
{
    #ifdef KMATRIX_DEBUG
        test_col( c );
    #endif

    for( int r=0; r<rows; r++ )
    {
        if( a[r*cols+c] != (K)0 ) return FALSE;
    }
    return TRUE;
}

// ----------------------------------------------------------------------------
//  find the element of column  c  if smallest nonzero absolute value
//  consider only elements in row  r0  or below
//  return value: the row of the element
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::column_pivot( int r0,int c ) const
{
    #ifdef KMATRIX_DEBUG
        test_row( r0 );
        test_col( c  );
    #endif

    int r;
    //  find first nonzero entry in column  c

    for( r=r0; r<rows && a[r*cols+c]==(K)0; r++ );

    if( r == rows )
    {
        //  column is zero

        return -1;
    }
    else
    {
        double val     = a[r*cols+c].complexity( );
        double val_new = 0.0;
        int pivot   = r;

        for( ; r<rows; r++ )
        {
            if( a[r*cols+c] != (K)0 &&
                ( val_new = a[r*cols+c].complexity( ) ) < val )
            {
                val = val_new;
                pivot = r;
            }
        }
        return pivot;
    }
}

// ----------------------------------------------------------------------------
//  divide row  r  by the gcd of all elements
// ----------------------------------------------------------------------------

template<class K>
    K    KMatrix<K>::set_row_primitive( int r )
{
    #ifdef KMATRIX_DEBUG
        test_row( r );
    #endif

    K   g = gcd( &(a[r*cols]),cols );

    for( int c=0; c<cols; c++ )
    {
        a[r*cols+c] /= g;
    }
    return  g;
}

// ----------------------------------------------------------------------------
//  convert the matrix to upper triangular form
//  return value: rank of the matrix
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::gausseliminate( void )
{
    int r,c,rank = 0;
    K g;

    //  make sure that the elements of each row have gcd=1
    //  this is useful for pivoting

    for( r=0; r<rows; r++ )
    {
        set_row_primitive( r );
    }

    //  search a pivoting element in each column
    //  perform Gauss elimination

    for( c=0; c<cols && rank<rows; c++ )
    {
        if( ( r = column_pivot( rank,c )) >= 0 )
        {
            swap_rows( rank,r );

            for( r=rank+1; r<rows; r++ )
            {
                if( a[r*cols+c] != (K)0 )
                {
                    g = gcd( a[r*cols+c],a[rank*cols+c] );
                    add_rows( rank,r,-a[r*cols+c]/g,a[rank*cols+c]/g );
                    set_row_primitive( r );
                }
            }
            rank++;
        }
    }
    return rank;
}

// ----------------------------------------------------------------------------
//  solve the linear system of equations given by
//    (x1,...,xn,-1)*(*this) = 0
//  return value: rank of the matrix
//  k is set to the number of variables
//  rat[0],...,rat[k-1] are set to the solutions
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::solve( K **solution,int *k )
{
    int r,c,rank = 0;
    K g;

    // ----------------------------------------------------
    //  make sure that the elements of each row have gcd=1
    //  this is useful for pivoting
    // ----------------------------------------------------

    for( r=0; r<rows; r++ )
    {
        set_row_primitive( r );
    }

    // ------------------------------------------
    //  search a pivoting element in each column
    //  perform Gauss elimination
    // ------------------------------------------

    for( c=0; c<cols && rank < rows; c++ )
    {
        if( ( r = column_pivot( rank,c )) >= 0 )
        {
            swap_rows( rank,r );

            for( r=0; r<rank; r++ )
            {
                if( a[r*cols+c] != (K)0 )
                {
                    g = gcd( a[r*cols+c],a[rank*cols+c] );
                    add_rows( rank,r,-a[r*cols+c]/g,a[rank*cols+c]/g );
                    set_row_primitive( r );
                }
            }

            for( r=rank+1; r<rows; r++ )
            {
                if( a[r*cols+c] != (K)0 )
                {
                    g = gcd( a[r*cols+c],a[rank*cols+c] );
                    add_rows( rank,r,-a[r*cols+c]/g,a[rank*cols+c]/g );
                    set_row_primitive( r );
                }
            }

            rank++;
        }
    }

    if( rank < cols )
    {
        // ----------------------
        //  equation is solvable
        //  copy solutions
        // ----------------------

        *solution = new K[cols-1];
        *k        = cols - 1;

        for( c=0; c<cols-1; c++ )
        {
            (*solution)[c] = (K)0;
        }

        for( r=0; r<rows; r++ )
        {
            for( c=0; c<cols && a[r*cols+c] == (K)0; c++ );

            if( c < cols-1 )
            {
                (*solution)[c] = ((K)a[(r+1)*cols-1])/a[r*cols+c];
            }
        }
    }
    else
    {
        // --------------------------
        //  equation is not solvable
        // --------------------------

        *solution = (K*)NULL;
        *k   = 0;
    }

    return rank;
}

// ----------------------------------------------------------------------------
//  compute the rank of the matrix
//  return value: rank of the matrix
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::rank( void ) const
{
    KMatrix<K> dummy( *this );

    return dummy.gausseliminate( );
}

// ----------------------------------------------------------------------------
//  print the matrix
//  return value: the output stream used
// ----------------------------------------------------------------------------

#ifdef KMATRIX_PRINT

template<class K>
    static
        void    print_rational( ostream &s,int digits,const K &n )
{
    unsigned int num = digits - n.length( );

    for( unsigned int i=0; i < num; i++ )
    {
        #ifdef KMATRIX_IOSTREAM
            s << " ";
        #else
            fprintf( stdout," " );
        #endif
    }

    s << n;
}

template<class K>
    ostream &    operator << ( ostream &s,const KMatrix<K> &m )
{
    int i,r,c,digits=0,tmp;

    for( i=0; i<m.rows*m.cols; i++ )
    {
        tmp = m.a[i].length( );

        if( tmp > digits ) digits = tmp;
    }

    for( r=0; r<m.rows; r++ )
    {
        if( m.rows == 1 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << "<";
            #else
                fprintf( stdout,"<" );
            #endif
        }
        else if( r == 0 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << "/";
            #else
                fprintf( stdout,"/" );
            #endif
        }
        else if( r == m.rows - 1 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << "\\";
            #else
                fprintf( stdout,"\\" );
            #endif
        }
        else
        {
            #ifdef KMATRIX_IOSTREAM
                s << "|";
            #else
                fprintf( stdout,"|" );
            #endif
        }

        for( c=0; c<m.cols; c++ )
        {
            #ifdef KMATRIX_IOSTREAM
                s << " ";
            #else
                fprintf( stdout," " );
            #endif

            print_rational( s,digits,m.a[r*m.cols+c] );
        }

        if( m.rows == 1 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << " >";
            #else
                fprintf( stdout," >" );
            #endif
        }
        else if( r == 0 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << " \\" << endl;
            #else
                fprintf( stdout," \\\n" );
            #endif
        }
        else if( r == m.rows - 1 )
        {
            #ifdef KMATRIX_IOSTREAM
                s << " /";
            #else
                fprintf( stdout," /" );
            #endif
        }
        else
        {
            #ifdef KMATRIX_IOSTREAM
                s << " |" << endl;
            #else
                fprintf( stdout," |\n" );
            #endif
        }
    }
    return s;
}

#endif

// ----------------------------------------------------------------------------
//  test if the matrix is quadratic
//  return value: TRUE or FALSE
// ----------------------------------------------------------------------------

template<class K>
    int     KMatrix<K>::is_quadratic( void ) const
{
    return ( rows == cols ? TRUE : FALSE );
}

// ----------------------------------------------------------------------------
//  test if the matrix is symmetric
//  return value: TRUE or FALSE
// ----------------------------------------------------------------------------

template<class K>
     int     KMatrix<K>::is_symmetric( void ) const
{
    if( is_quadratic( ) )
    {
        int r,c;

        for( r=1; r<rows; r++ )
        {
            for( c=0; c<r; c++ )
            {
                if( a[r*cols+c] != a[c*cols+r] )
                {
                    return  FALSE;
                }
            }
        }
        return  TRUE;
    }
    else
    {
        return  FALSE;
    }
}

// ----------------------------------------------------------------------------
//  compute the determinant
//  return value: the determinant
// ----------------------------------------------------------------------------

template<class K> K KMatrix<K>::determinant( void ) const
{
    if( !is_quadratic( ) )
    {
        return 0;
    }

    KMatrix<K> dummy( *this );

    int r,c,rank = 0;
    K g;
    K frank,fr;
    K det = 1;

    //  make sure that the elements of each row have gcd=1
    //  this is useful for pivoting

    for( r=0; r<dummy.rows; r++ )
    {
        det *= dummy.set_row_primitive( r );
    }

    //  search a pivoting element in each column
    //  perform Gauss elimination

    for( c=0; c<cols && rank<dummy.rows; c++ )
    {
        if( ( r = dummy.column_pivot( rank,c )) >= 0 )
        {
            det *= dummy.swap_rows( rank,r );

            for( r=rank+1; r<dummy.rows; r++ )
            {
                if( dummy.a[r*cols+c] != (K)0 )
                {
                    g = gcd( dummy.a[r*cols+c],dummy.a[rank*cols+c] );

                    frank = -dummy.a[r*cols+c]/g;
                    fr    = dummy.a[rank*cols+c]/g;

                    det /= dummy.add_rows( rank,r,frank,fr );
                    det *= dummy.set_row_primitive( r );
                }
            }
            rank++;
        }
    }

    if( rank != dummy.rows )
    {
        return 0;
    }

    for( r=0; r<dummy.rows; r++ )
    {
        det *= dummy.a[r*cols+r];
    }

    return  det;
}

#endif /* KMATRIX_H */

// ----------------------------------------------------------------------------
//  kmatrix.h
//  end of file
// ----------------------------------------------------------------------------
