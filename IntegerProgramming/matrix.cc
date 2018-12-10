// matrix.cc

// implementation of class matrix

#ifndef MATRIX_CC
#define MATRIX_CC

#include "matrix.h"

////////////// constructors and destructor //////////////////////////////////
typedef Integer* IntegerP;
typedef BigInt* BigIntP;

matrix::matrix(const int& row_number, const int& column_number)
    :rows(row_number),columns(column_number)
{
  _kernel_dimension=-2;
  // LLL-algorithm not yet performed

  // argument check
  if((rows<=0)||(columns<=0))
    // bad input, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(const int&, const int&):\n"
      "argument out of range"<<endl;
    columns=-1;
    return;
  }

  // memory allocation and initialization

  coefficients=new IntegerP[rows];
  for(int i=0;i<rows;i++)
    coefficients[i]=new Integer[columns];
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
      coefficients[i][j]=0;
}

matrix::matrix(const int& row_number, const int& column_number,
               Integer** entries)
    :rows(row_number),columns(column_number)
{
  _kernel_dimension=-2;
  // LLL-algorithm not yet performed

  // argument check
  if((rows<=0)||(columns<=0))
    // bad input, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(const int&, const int&, Integr**):\n"
      "argument out of range"<<endl;
    columns=-1;
    return;
  }

  // memory allocation and initialization

  coefficients=new IntegerP[rows];
  for(int i=0;i<rows;i++)
    coefficients[i]=new Integer[columns];
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
      coefficients[i][j]=entries[i][j];
  // coefficients[i] is the i-th row vector
}




matrix::matrix(ifstream& input)
{
  _kernel_dimension=-2;
  // LLL-algorithm not yet performed

  input>>rows;
  if(!input)
    // input failure, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(ifstream&): input failure"<<endl;
    columns=-2;
    return;
  }

  input>>columns;
  if(!input)
    // input failure, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(ifstream&): input failure"<<endl;
    columns=-2;
    return;
  }

  if((rows<=0)||(columns<=0))
    // bad input, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(ifstream&): bad input"<<endl;
    columns=-1;
    return;
  }

  coefficients=new IntegerP[rows];
  for(int i=0;i<rows;i++)
    coefficients[i]=new Integer[columns];
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
    {
      input>>coefficients[i][j];
      if(!input)
        // bad input, set error flag
      {
        cerr<<"\nWARNING: matrix::matrix(ifstream&): input failure"<<endl;
        columns=-2;
        return;
      }
    }
}




matrix::matrix(const int& m, const int& n, ifstream& input)
{
  _kernel_dimension=-2;
  // LLL-algorithm not yet performed

  // argument check
  if((m<=0) || (n<=0))
    // bad input, set "error flag"
  {
    cerr<<"\nWARNING: matrix::matrix(const int&, const int&, ifstream&):\n"
      "argument out of range"<<endl;
    columns=-1;
    return;
  }

  rows=m;
  columns=n;

  // memory allocation and initialization

  coefficients=new IntegerP[rows];
  for(int i=0;i<rows;i++)
    coefficients[i]=new Integer[columns];
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
    {
      input>>coefficients[i][j];
      if(!input)
        // bad input, set error flag
      {
        columns=-2;
        return;
      }
    }
}




matrix::matrix(const matrix& A)
    :rows(A.rows),columns(A.columns),_kernel_dimension(A._kernel_dimension)
{

  if(columns<0)
  {
    cerr<<"\nWARNING: matrix::matrix(const matrix&):\n"
      "Building a matrix from a corrupt one"<<endl;
    return;
  }

  // memory allocation and initialization (also for H)

  coefficients=new IntegerP[rows];
  for(int i=0;i<rows;i++)
    coefficients[i]=new Integer[columns];
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
      coefficients[i][j]=A.coefficients[i][j];

  if(_kernel_dimension>0)
  {
    H=new BigIntP[_kernel_dimension];
    for(int k=0;k<_kernel_dimension;k++)
      H[k]=new BigInt[columns];
    for(int k=0;k<_kernel_dimension;k++)
      for(int j=0;j<columns;j++)
        H[k][j]=(A.H)[k][j];
  }
}




matrix::~matrix()
{
  for(int i=0;i<rows;i++)
    delete[] coefficients[i];
  delete[] coefficients;

  if(_kernel_dimension>0)
    // LLL-algorithm performed
  {
    for(int i=0;i<_kernel_dimension;i++)
      delete[] H[i];
    delete[] H;
  }
}




//////////////////// object properties //////////////////////////////////////




BOOLEAN matrix::is_nonnegative() const
{
  for(int i=0;i<rows;i++)
    for(int j=0;j<columns;j++)
      if(coefficients[i][j]<0)
        return FALSE;
  return TRUE;
}



int matrix::error_status() const
{
  if(columns<0)
    return columns;
  else
    return 0;
}



int matrix::row_number() const
{
  return rows;
}



int matrix::column_number() const
{
  return columns;
}




////////// special routines for the IP-algorithms /////////////////////////




int matrix::LLL_kernel_basis()
{

  // copy the column vectors of the actual matrix
  // (They are modified by the LLL-algorithm!)
  BigInt** b=new BigIntP[columns];
  for(int n=0;n<columns;n++)
    b[n]=new BigInt[rows];
  for(int n=0;n<columns;n++)
    for(int m=0;m<rows;m++)
      b[n][m]=coefficients[m][n];

  // compute a LLL-reduced basis of the relations of b[0],...,b[columns-1]
  _kernel_dimension=relations(b,columns,rows,H);

  // The kernel lattice basis is now stored in the member H (vectors
  // H[0],...,H[_kernel_dimension-1]).

  // delete auxiliary vectors
  for(int n=0;n<columns;n++)
    delete[] b[n];
  delete[] b;

  return _kernel_dimension;
}




int matrix::compute_nonzero_kernel_vector()
{

  if(_kernel_dimension==-2)
    // lattice basis not yet computed
    LLL_kernel_basis();

  if(_kernel_dimension==-1)
  {
    cerr<<"\nWARNING: int matrix::compute_non_zero_kernel_vector(BigInt*&):"
      "\nerror in kernel basis, cannot compute the desired vector"<<endl;
    return 0;
  }

  if(_kernel_dimension==0)
  {
    cerr<<"\nWARNING: int matrix::compute_non_zero_kernel_vector(BigInt*&): "
      "\nkernel dimension is zero"<<endl;
    return 0;
  }

  // Now, the kernel dimension is positive.

  BigInt *M=new BigInt[_kernel_dimension];
  // M stores a number by which the algorithm decides which vector to
  // take next.


// STEP 1: Determine the vector with the least zero components (if it is not
// unique, choose the smallest).

  // determine number of zero components
  for(int i=0;i<_kernel_dimension;i++)
  {
    M[i]=0;
    for(int j=0;j<columns;j++)
      if(H[i][j]==BigInt(0))
        M[i]++;
  }

  // determine minimal number of zero components
  BigInt min=columns;
  // columns is an upper bound (not reached because the kernel basis cannot
  // contain the zero vector)
  for(int i=0;i<_kernel_dimension;i++)
    if(M[i]<min)
      min=M[i];

  // add the square of the norm to the vectors with the least zero components
  // and discard the others (the norm computation is why we have chosen the
  // M[i] to be BigInts)
  for(int i=0;i<_kernel_dimension;i++)
    if(M[i]!=min)
      M[i]=-1;
    else
      for(int j=0;j<columns;j++)
        M[i]+=H[i][j]*H[i][j];
  // As the lattice basis does not contain the zero vector, at least one M[i]
  // is positive!

  // determine the start vector, i.e. the one with least zero components, but
  // smallest possible (euclidian) norm
  int min_index=-1;
  for(int i=0;i<_kernel_dimension;i++)
    if(M[i]>BigInt(0))
    {
      if(min_index==-1)
        min_index=i;
      else if(M[i]<M[min_index])
        min_index=i;
    }

  // Now, H[min_index] is the vector to be transformed into a nonnegative one.
  // For a better overview, it is swapped with the first vector
  // (only pointers).

  if(min_index!=0)
  {
    BigInt* swap=H[min_index];
    H[min_index]=H[0];
    H[0]=swap;
  }


// Now construct the desired vector.
// This is done by adding a linear combination of
// H[1],...,H[_kernel_dimension-1] to H[0]. It is important that the final
// result, written as a linear combination of
// H[0],...,H[_kernel_dimension-1], has coefficient 1 or -1 at H[0]
// (to make sure that it is together with H[1],...,H[_kernel_dimension]
// still a  l a t t i c e   basis).

  for(int current_position=1;current_position<columns;current_position++)
    // in fact, this loop will terminate before the condition in the
    // for-statement is satisfied...
  {


// STEP 2: Nonnegative vector already found?

    BOOLEAN found=TRUE;
    for(int j=0;j<columns;j++)
      if(H[0][j]==BigInt(0))
        found=FALSE;

    if(found==TRUE)
      // H[0] has only positive entries,
      return 1;
    // else there are further zero components


// STEP 3: Can a furhter zero component be "eliminated"?
// If this is the case, find a basis vector that can do this.

    // determine number of components in each remaining vector that are zero
    // in the vector itself as well as in the already constructed vector
    for(int i=current_position;i<_kernel_dimension;i++)
      M[i]=0;

    int remaining_zero_components=0;

    for(int j=0;j<columns;j++)
      if(H[0][j]==BigInt(0))
      {
        remaining_zero_components++;
        for(int i=current_position;i<_kernel_dimension;i++)
          if(H[i][j]==BigInt(0))
            M[i]++;
      }

    // determine minimal number of such components
    min=remaining_zero_components;
    // this is the number of zero components in H[0] and an upper bound
    // for the M[i]
    for(int i=current_position;i<_kernel_dimension;i++)
      if(M[i]<min)
        min=M[i];

    if(min==(const BigInt&)remaining_zero_components)
      // all zero components in H[0] are zero in each remaining vector
      // => desired vector does not exist
      return 0;

    // add the square of the norm to the vectors with the least common zero
    // components
    // discard the others
    for(int i=current_position;i<_kernel_dimension;i++)
      if(M[i]!=min)
        M[i]=-1;
      else
        for(int j=0;j<columns;j++)
          M[i]+=H[i][j]*H[i][j];
    //  Again, at least one M[i] is positive!

    // determine vector to proceed with
    // This is the vector with the least common zero components with respect
    // to H[0], but the smallest possible norm.
    int min_index=0;
    for(int i=current_position;i<_kernel_dimension;i++)
      if(M[i]>BigInt(0))
      {
        if(min_index==0)
          min_index=i;
        else if(M[i]<M[min_index])
          min_index=i;
      }

    // Now, a multiple of H[min_index] will be added to the already constructed
    // vector H[0].
    // For a better handling, it is swapped with the vector at current_position
    // (only pointers).

    if(min_index!=current_position)
    {
      BigInt* swap=H[min_index];
      H[min_index]=H[current_position];
      H[current_position]=swap;
    }


// STEP 4: Choose a convenient multiple of H[current_position] to add to H[0].
// The number of factors "mult" that have to be tested is bounded by the
// number of nonzero components in H[0] (for each such components, there is at
// most one such factor that will eliminate it in the linear combination
// H[0] + mult*H[current_position].

    found=FALSE;

    for(int mult=1;found==FALSE;mult++)
    {
      found=TRUE;

      // check if any component !=0 of H[0] becomes zero by adding
      // mult*H[current_position]
      for(int j=0;j<columns;j++)
        if(H[0][j]!=BigInt(0))
          if(H[0][j]+(const BigInt&)mult*H[current_position][j]
            ==BigInt(0))
            found=FALSE;

      if(found==TRUE)
        for(int j=0;j<columns;j++)
          H[0][j]+=(const BigInt&)mult*H[current_position][j];
      else
        // try -mult
      {

        found=TRUE;

        // check if any component !=0 of H[0] becomes zero by subtracting
        // mult*H[current_position]
        for(int j=0;j<columns;j++)
          if(H[0][j]!=BigInt(0))
            if(H[0][j]-(const BigInt&)mult*H[current_position][j]
              ==BigInt(0))
              found=FALSE;

        if(found==TRUE)
          for(int j=0;j<columns;j++)
            H[0][j]-=(const BigInt&)mult*H[current_position][j];
      }
    }
  }

// When reaching this line, an error must have occurred.
  cerr<<"FATAL ERROR in int matrix::compute_nonnegative_vector()"<<endl;
  abort();
}

int matrix::compute_flip_variables(int*& F)
{
  // first compute nonzero vector
  int okay=compute_nonzero_kernel_vector();

  if(!okay)
  {
    cout<<"\nWARNING: int matrix::compute_flip_variables(int*&):\n"
      "kernel of the matrix contains no vector with nonzero components,\n"
      "no flip variables computed"<<endl;
    return -1;
  }

  // compute variables to flip; these might either be those corresponding
  // to the positive components of the kernel vector without zero components
  // or those corresponding to the negative ones

  int r=0;
  // number of flip variables

  for(int j=0;j<columns;j++)
    if(H[0][j]<BigInt(0))
      r++;
  // remember that all components of H[0] are !=0

  if(r==0)
    // no flip variables
    return 0;

  if(2*r>columns)
    // more negative than positive components in H[0]
    // all variables corresponding to positive components will be flipped
  {
    r=columns-r;
    F=new int[r];
    memset(F,0,r*sizeof(int));
    int counter=0;
    for(int j=0;j<columns;j++)
      if(H[0][j]> BigInt(0))
      {
        F[counter]=j;
        counter++;
      }
  }
  else
    // more (or as many) positive than negative components in v
    // all variables corresponding to negative components will be flipped
  {
    F=new int[r];
    memset(F,0,r*sizeof(int));
    int counter=0;
    for(int j=0;j<columns;j++)
      if(H[0][j]< BigInt(0))
      {
        F[counter]=j;
        counter++;
      }
  }

  return r;
}




int matrix::hosten_shapiro(int*& sat_var)
{

  if(_kernel_dimension==-2)
    // lattice basis not yet computed
    LLL_kernel_basis();

  if(_kernel_dimension==-1)
  {
    cerr<<"\nWARNING: int matrix::hosten_shapiro(int*&):\n"
      "error in kernel basis, cannot compute the saturation variables"<<endl;
    return 0;
  }

  if(_kernel_dimension==0)
    // the toric ideal corresponding to the kernel lattice is the zero ideal,
    // no saturation variables necessary
    return 0;

  // Now, the kernel dimension is positive.

  if(columns==1)
    // matrix consists of one zero column, kernel is generated by the vector
    // (1) corresponding to the toric ideal <x-1> which is already staurated
    return 0;

  int number_of_sat_var=0;
  sat_var=new int[columns/2];
  memset(sat_var,0,sizeof(int)*(columns/2));

  BOOLEAN* ideal_saturated_by_var=new BOOLEAN[columns];
  // auxiliary array used to remember by which variables the ideal has still to
  // be saturated
  for(int j=0;j<columns;j++)
    ideal_saturated_by_var[j]=FALSE;

  for(int k=0;k<_kernel_dimension;k++)
  {
    // determine number of positive and negative components in H[k]
    // corresponding to variables by which the ideal has still to be saturated
    int pos_sat_var=0;
    int neg_sat_var=0;

    for(int j=0;j<columns;j++)
    {
      if(ideal_saturated_by_var[j]==FALSE)
      {
        if(H[k][j]> BigInt(0))
          pos_sat_var++;
        else
          if(H[k][j]< BigInt(0))
            neg_sat_var++;
      }
    }


    // now add the smaller set to the saturation variables
    if(pos_sat_var<=neg_sat_var)
    {
      for(int j=0;j<columns;j++)
        if(ideal_saturated_by_var[j]==FALSE)
        {
          if(H[k][j]> BigInt(0))
            // ideal has to be saturated by the variables corresponding
            // to positive components
          {
            sat_var[number_of_sat_var]=j;
            ideal_saturated_by_var[j]=TRUE;
            number_of_sat_var++;
          }
          else if(H[k][j]< BigInt(0))
              // then the ideal is automatically saturated by the variables
              // corresponding to negative components
              ideal_saturated_by_var[j]=TRUE;
        }
    }
    else
    {
      for(int j=0;j<columns;j++)
        if(ideal_saturated_by_var[j]==FALSE)
        {
          if(H[k][j]< BigInt(0))
            // ideal has to be saturated by the variables corresponding
            // to negative components
          {
            sat_var[number_of_sat_var]=j;
            ideal_saturated_by_var[j]=TRUE;
            number_of_sat_var++;
          }
          else if(H[k][j]> BigInt(0))
              // then the ideal is automatically saturated by the variables
              // corresponding to positive components
              ideal_saturated_by_var[j]=TRUE;
        }
    }
  }

  // clean up memory
  delete[] ideal_saturated_by_var;

  return number_of_sat_var;
}




//////////////////// output ///////////////////////////////////////////////




void matrix::print() const
{
  printf("\n%3d x %3d\n",rows,columns);

  for(int i=0;i<rows;i++)
  {
    for(int j=0;j<columns;j++)
      printf("%6d",coefficients[i][j]);
    printf("\n");
  }
}


void matrix::print(FILE *output) const
{
  fprintf(output,"\n%3d x %3d\n",rows,columns);

  for(int i=0;i<rows;i++)
  {
    for(int j=0;j<columns;j++)
      fprintf(output,"%6d",coefficients[i][j]);
    fprintf(output,"\n");
  }
}


void matrix::print(ofstream& output) const
{
  output<<endl<<setw(3)<<rows<<" x "<<setw(3)<<columns<<endl;

  for(int i=0;i<rows;i++)
  {
    for(int j=0;j<columns;j++)
      output<<setw(6)<<coefficients[i][j];
    output<<endl;
  }
}
#endif  // matrix.cc
