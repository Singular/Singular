/* ===================================================================
    Dateiname:          lgs.cc
   =================================================================== */

#include "lgs.h"
#include "bifacConfig.h"

#ifdef HAVE_BIFAC

//--<>---------------------------------
LGS::LGS( int r, int c, bool inv )// KONSTRUKTOR
//--<>---------------------------------
{
  INVERSE = inv;

  max_columns = c;
  max_rows    = (r>c) ? c : r ;

  if( INVERSE )
  {
    c *=2;
  }

  CFMatrix  AA(max_rows, c);
  CFMatrix  bb(max_rows, 1);
  A=AA;
  b=bb;
  pivot = new int [max_columns+1]; // We start with index 1 (easier)
  for( int i=0; i<=max_columns; i++)    pivot[i]=0;
  now_row = 1;   // We start counting in the first row
}


//--<>---------------------------------
LGS::~LGS( void )// DESTRUKTOR
//--<>---------------------------------
{
  delete[] pivot;
}

//--<>---------------------------------
void LGS::reset(void)
//--<>---------------------------------
{ // Clear the matrix for a new computation
  now_row=1;
  for( int i=0; i<=max_columns; i++)
    pivot[i]=0;
}

//--<>---------------------------------
bool LGS::new_row( const CFMatrix Z, const CanonicalForm bb)
//--<>---------------------------------
{ // Insert a new row
  ASSERT ( (1 <= now_row && now_row <=max_rows), "wrong number of rows => Matrix has max. rank");
  int i;

//    if (INVERSE)
//    cout << "* Integriere Zeile "<<now_row << " (max " <<max_rows<<" x "
//         <<  max_columns << ")\n"
//         << "Z = " << Z << "\nb = " << bb << endl << flush;




  // === Insert a new row ===
  for(i=1; i<=max_columns; i++)
    A(now_row, i) = Z(1,i);
  b(now_row, 1) = bb;


  //  cout << "* reduzierte Matrix (vor lin_dep) " << A << endl;
  // === check linear dependency ===
  if ( ! lin_dep() )
    now_row++;
  else
    return(false);

  // === Reduce the previous rows ===
  for(i=1; i<now_row-1; i++)
    reduce(now_row-1,i );


  //  cout << "\n* Zeile Z =" << Z << " ist integriert!\n" << A << flush;

//    if( INVERSE ) cout << A;
//  cout << "* Verlasse new_row!\n" << "* Z = " << Z << endl << flush;
  return(true); // row was linear independent
}


//--<>---------------------------------
bool LGS::lin_dep( void )
//--<>---------------------------------
{ // check if new row is linear dependent of the former ones

  int i;

  // === Reduce the actual row ===
  for(i=1; i<now_row; i++)
    reduce(i, now_row);

//  cerr << "* Bin noch in [lin_dep]\n" << flush;

  // === Quest for a pivot ===
  for ( i=1; i<=max_columns; i++)
    if( A(now_row,i) != 0 )
    {
      pivot[now_row] = i;
      break;//      i = max_columns;

    }
//  cout << "* pivot["<<now_row<<"] = " << pivot[now_row] << endl << flush;

  // === Is the reduced row (0,...,0)? ====
  if( pivot[now_row] == 0 )
  {
    if( INVERSE )
      for ( i=1; i<=max_columns; i++)
        A(now_row, max_columns+i) = 0;
    return (true);
  }
  // === The row is linear independent ====
//  cout << "* reduzierte Matrix (1) ist " << A << endl;
  if( INVERSE )   // Identity matrix
    A(now_row, now_row+max_columns) = 1;
//  cout << "* reduzierte Matrix (2) ist " << A << endl;
  return(false);
}


//--<>---------------------------------
void LGS::reduce(int fix, int row)
//--<>---------------------------------
{ // Reduce the `row´ by means of row `fix´

  if( A(row, pivot[fix]) == 0 ) return;

  CanonicalForm mul =  A(row, pivot[fix]) / A(fix, pivot[fix]);

//    cout << "* Multiplikationsfaktor ist " << mul << endl;
//    cout << "* Aktuelle Matrix ist " << A
//         << "\n  b = " << b << endl;
//    cout << "max_columns = "<< max_columns << endl;
//    cout << "* now_row = " << now_row <<", row=" << row << endl << flush;
//    cout << "* Pivot[" << row << "] = " << pivot[row] << endl << flush;
//    cout << "* Pivot[" << fix << "] = " << pivot[fix] << endl << flush;


  for (int i=1; i<=max_columns; i++)
    if( A(fix,i) != 0 )
      A(row, i) -= mul* A(fix,i);
  if( b(fix,1) != 0 )
    b(row,1) -= mul * b(fix,1);


  if ( INVERSE )
    for (int i=max_columns+1; i<=2*max_columns; i++)
      if( A(fix,i) != 0 )
        A(row, i) -= (mul* A(fix,i));
//      A(row,i) = 777;

//   cout << "* reduzierte Matrix ist " << A << endl;
//    cout << "* Verlasse [reduce]\n" << flush;

}
///////////////////////////////////////////////////////
// The Matrix (A | I) has maximal rank and
// the ´A´ part has to be normalized only
//--<>---------------------------------
void LGS::inverse( CFMatrix & I )
//--<>---------------------------------
{
  int i,j;
  CanonicalForm mul;

//  cout << "* PRÄ-Invers:" << A << endl;
  for(i=1; i<=max_rows; i++)
  {
    for( j=max_columns+1; j<=2*max_columns; j++)
      if( A(i,j)!=0 )
        A(i,j) /= A( i, pivot[i]);
    A( i, pivot[i]) = 1;
  }
// cout << "* Inverse Matrix ist " << A << endl;

  for(i=1; i<=max_rows; i++)
  {
//    cout << "pivot["<<i<<"] = " << pivot[i]<< endl;
    for(j=1; j<=max_columns; j++)
      I(pivot[i],j) = A(i, j+max_columns);
  }
}
//--<>---------------------------------
int LGS::rank(void)
//--<>---------------------------------
{ // Return the current rank of the matrix
  return( now_row-1);
}


//--<>---------------------------------
void LGS::print(void)
//--<>---------------------------------
{ // Return the current rank of the matrix
#ifndef NOSTREAMIO
  cout << "A = " << A << "\nb = " << b << endl;
#endif
}




//--<>---------------------------------
int LGS::corank(void)
//--<>---------------------------------
{ // Return the current rank of the matrix
  return(  ( (max_rows < max_columns ) ? max_rows : max_columns )-now_row+1 );
}

//--<>---------------------------------
int LGS::ErgCol(int row, int basis[])
//--<>---------------------------------
{
  bool state = false;

  for( int i=1; i<=max_columns; i++)
  {
    if( A(row,i) != 0 )
    {
      state = true;
      for( int j=1; j<=basis[0]; j++)
        if( i==basis[j] )
          state = false;
      if( state == true )
        return (i);
    }
  }
  // A row contains only pivot entry/-ies
  for( int j=1; j<=basis[0]; j++)
    if(  A(row, basis[j]) != 0 )
      return( -basis[j] );


  AUSGABE_LGS("Zeile ist " << row << endl);
  AUSGABE_ERR("* Mistake in [lgs.cc]! Impossible result. Aborting!\n");
  exit (1);
}


//--<>---------------------------------
CFMatrix LGS::GetKernelBasis(void)
//--<>---------------------------------
{
  int i,z;
  int dim = corank();

  bool* tmp_vec = new bool[max_columns+1];
  int*  basis   = new int [dim+1];
  basis[0]=1;
  CFMatrix erg  (dim, max_columns); // Each row is one solution

  // === Searching free parameters ===
  for( i=1; i<=max_rows; i++)
    tmp_vec[i]=false;
  for( i=1; i<now_row; i++)
    if( pivot[i] != 0 )
      tmp_vec[pivot[i]] = true;

  for( i=1; i<=max_columns; i++)
    if(tmp_vec[i] == false)
    {
      basis[basis[0]]=i;
      basis[0]++;
    }
  ASSERT(basis[0]-1==dim, "Wrong dimensions");


//    cout << "* Freie Parameter sind:";
//    for ( int k=1; k<basis[0]; k++) cout << basis[k] << ", ";
//    cout << endl;

// === Creating the basis vectors ===
  for(z=1; z<now_row; z++)
    for(i=1; i<=dim; i++)
    {
      erg(i, basis[i]) = 1;
      erg(i, pivot[z]) = -(A(z, basis[i]) / A(z,pivot[z]));
    }

//  cout << "Kernbasis ist " << erg;
  return erg;
}

//--<>---------------------------------
CFMatrix LGS::GetSolutionVector(void)
//--<>---------------------------------
{ // Ax=b has exactly one solution vector x
  int z;
  CFMatrix erg  (1, max_columns);

  // === Creating the basis vectors ===
  for(z=1; z<=max_columns; z++)
  {
    erg(1,pivot[z])  = b(z,1);         // Vector b
    erg(1,pivot[z]) /= A(z,pivot[z]);
  }
  return erg;
}
#endif
