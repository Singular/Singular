#ifndef LLL_CC
#define LLL_CC

#include "LLL.h"

/// subroutines for the LLL-algorithms

void REDI_KB(const short& k, const short& l, BigInt** b,
             const short& number_of_vectors, const short& vector_dimension,
             BigInt** H, BigInt* d, BigInt** lambda)
/// the REDI procedure for relations(...) (to compute the Kernel Basis,
/// algorithm 2.7.2 in Cohen's book)
{
#ifdef GMP
  if(abs(BigInt(2)*lambda[k][l])<=d[l+1])
#else   /// GMP
  if(labs(2*lambda[k][l])<=d[l+1])
    /// labs is the abs-function for long ints
#endif  /// GMP
    return;

#ifdef GMP
  BigInt q=(BigInt(2)*lambda[k][l]+d[l+1])/(BigInt(2)*d[l+1]);
#else   /// GMP
  long q=(long int) floor(((float)(2*lambda[k][l]+d[l+1]))/(2*d[l+1]));
#endif  /// GMP

  /// q is the integer quotient of the division
  /// (2*lambda[k][l]+d[l+1])/(2*d[l+1]).
  /// Because of the rounding mode (always towards zero) of GNU C++,
  /// we cannot use the built-in integer division
  /// here; it causes errors when dealing with negative numbers. Therefore
  /// the complicated casts: The divident is first casted to a float which
  /// causes the division result to be a float. This result is explicitly
  /// rounded downwards. As the floor-function returns a double (for range
  /// reasons), this has to be casted to an integer again.

  for(short n=0;n<number_of_vectors;n++)
    H[k][n]-=q*H[l][n];
  /// H[k]=H[k]-q*H[l]

  for(short m=0;m<vector_dimension;m++)
    b[k][m]-=q*b[l][m];
  /// b[k]=b[k]-q*b[l]

  lambda[k][l]-=q*d[l+1];

  for(short i=0;i<=l-1;i++)
    lambda[k][i]-=q*lambda[l][i];
}




void REDI_IL(const short& k, const short& l, BigInt** b,
             const short& vector_dimension, BigInt* d, BigInt** lambda)
/// the REDI procedure for the integer LLL algorithm (algorithm 2.6.7 in
/// Cohen's book)
{
#ifdef GMP
  if(abs(BigInt(2)*lambda[k][l])<=d[l+1])
#else   /// GMP
  if(labs(2*lambda[k][l])<=d[l+1])
    /// labs is the abs-function for long ints
#endif  /// GMP
    return;

#ifdef GMP
  BigInt q=(BigInt(2)*lambda[k][l]+d[l+1])/(BigInt(2)*d[l+1]);
#else   /// GMP
  long q=(long int) floor(((float)(2*lambda[k][l]+d[l+1]))/(2*d[l+1]));
#endif  /// GMP

  /// q is the integer quotient of the division
  /// (2*lambda[k][l]+d[l+1])/(2*d[l+1]).
  /// Because of the rounding mode (always towards zero) of GNU C++,
  /// we cannot use the built-in integer division
  /// here; it causes errors when dealing with negative numbers. Therefore
  /// the complicated casts: The divident is first casted to a float which
  /// causes the division result to be a float. This result is explicitly
  /// rounded downwards. As the floor-function returns a double (for range
  /// reasons), this has to be casted to an integer again.

  for(short m=0;m<vector_dimension;m++)
    b[k][m]-=q*b[l][m];
  /// b[k]=b[k]-q*b[l]

  lambda[k][l]-=q*d[l+1];

  for(short i=0;i<=l-1;i++)
    lambda[k][i]-=q*lambda[l][i];
}




void SWAPI(const short& k, const short& k_max, BigInt** b, BigInt* d,
           BigInt** lambda)
/// the SWAPI procedure of algorithm 2.6.7
{

  /// exchange b[k] and b[k-1]
  /// This can be done efficiently by swapping pointers (not entries).
  BigInt* swap=b[k];
  b[k]=b[k-1];
  b[k-1]=swap;

  if(k>1)
    for(short j=0;j<=k-2;j++)
    {
      /// exchange lambda[k][j] and lambda[k-1][j]
      BigInt swap=lambda[k][j];
      lambda[k][j]=lambda[k-1][j];
      lambda[k-1][j]=swap;
    }

  BigInt _lambda=lambda[k][k-1];

  BigInt B=(d[k-1]*d[k+1] + _lambda*_lambda)/d[k];
  /// It might be better to choose another evaluation order for this formula,
  /// see below.

  for(short i=k+1;i<=k_max;i++)
  {
    BigInt t=lambda[i][k];
    lambda[i][k]=(d[k+1]*lambda[i][k-1] - _lambda*t)/d[k];
    lambda[i][k-1]=(B*t + _lambda*lambda[i][k])/d[k+1];
  }

  d[k]=B;
}




void SWAPK(const short& k, const short& k_max, BigInt** b, BigInt** H,
           char *f, BigInt* d, BigInt** lambda)
/// the SWAPK procedure of algorithm 2.7.2
{
  /// exchange H[k] and H[k-1]
  /// This can be done efficiently by swapping pointers (not entries).
  BigInt *swap=H[k];
  H[k]=H[k-1];
  H[k-1]=swap;

  /// exchange b[k] and b[k-1] by the same method
  swap=b[k];
  b[k]=b[k-1];
  b[k-1]=swap;

  if(k>1)
    for(short j=0;j<=k-2;j++)
    {
      /// exchange lambda[k][j] and lambda[k-1][j]
      BigInt swap=lambda[k][j];
      lambda[k][j]=lambda[k-1][j];
      lambda[k-1][j]=swap;
    }

  BigInt _lambda=lambda[k][k-1];

  if(_lambda==BigInt(0))
  {
    d[k]=d[k-1];
    f[k-1]=0;
    f[k]=1;
    lambda[k][k-1]=0;
    for(short i=k+1;i<=k_max;i++)
    {
      lambda[i][k]=lambda[i][k-1];
      lambda[i][k-1]=0;
    }
  }
  else
    /// lambda!=0
  {
    for(short i=k+1;i<=k_max;i++)
      lambda[i][k-1]=(_lambda*lambda[i][k-1])/d[k];

    /// Multiplie lambda[i][k-1] by _lambda/d[k].
    /// One could also write
    ///   lambda[i][k-1]*=(lambda/d[k]);   (*)
    /// Without a BigInt class, this can prevent overflows when computing
    /// _lambda*lambda[i][k-1].
    /// But examples show that lambda/d[k] is in general not an integer.
    /// So (*) could lead to problems due to the inexact floating point
    /// arithmetic...
    /// Therefore, we chose the secure evaluation order in all such cases.

    BigInt t=d[k+1];
    d[k]=(_lambda*_lambda)/d[k];
    d[k+1]=d[k];

    for(short j=k+1;j<=k_max-1;j++)
      for(short i=j+1;i<=k_max;i++)
        lambda[i][j]=(lambda[i][j]*d[k])/t;

    for(short j=k+1;j<=k_max;j++)
      d[j+1]=(d[j+1]*d[k])/t;
  }

}

typedef BigInt* BigIntP;


short relations(BigInt **b, const short& number_of_vectors,
                const short& vector_dimension, BigInt**& H)
{

/// first check arguments

  if(number_of_vectors<0)
  {
    cerr<<"\nWARNING: short relations(BigInt**, const short&, const short&, "
      "BigInt**):\nargument number_of_vectors out of range"<<endl;
    return -1;
  }

  if(vector_dimension<=0)
  {
    cerr<<"\nWARNING: short relations(BigInt**, const short&, const short&, "
      "BigInt**):\nargument vector_dimension out of range"<<endl;
    return -1;
  }


/// consider special case

  if(number_of_vectors==1)
    /// Only one vector which has no relations if it is not zero,
    /// else relation 1.
  {
    short r=1;    /// Suppose the only column of the matrix is zero.

    for(short m=0;m<vector_dimension;m++)
      if(b[0][m]!=BigInt(0))
        /// nonzero entry detected
        r=0;

    if(r==1)
    {
      H=new BigIntP[1];
      H[0]=new BigInt[1];
      H[0][0]=1;
      /// This is the lattice basis of the relations...
    }

    return r;
  }


/// memory allocation

/// The names are chosen (as far as possible) according to Cohen's book.
/// However, for technical reasons, the indices do not run from 1 to
/// (e.g.) number_of_vectors, but from 0 to number_of_vectors-1.
/// Therefore all indices are shifted by -1 in comparison with this book,
/// except from the indices of the array d which has size
/// number_of_vectors+1.

  H=new BigIntP[number_of_vectors];
  for(short n=0;n<number_of_vectors;n++)
    H[n]=new BigInt[number_of_vectors];

  char* f=new char[number_of_vectors];

  BigInt* d=new BigInt[number_of_vectors+1];

  BigInt** lambda=new BigIntP[number_of_vectors];
  for(short n=1;n<number_of_vectors;n++)
    lambda[n]=new BigInt[n];
  /// We only need lambda[n][k] for n>k.



/// Step 1: Initialization

  short k=1;
  short k_max=0;
  /// for iteration

  d[0]=1;

  BigInt t=0;
  for(short m=0;m<vector_dimension;m++)
    t+=b[0][m]*b[0][m];
  /// Now, t is the scalar product of b[0] with itself.

  for(short n=0;n<number_of_vectors;n++)
    for(short l=0;l<number_of_vectors;l++)
      if(n==l)
        H[n][l]=1;
      else
        H[n][l]=0;
  /// Now, H equals the matrix I_(number_of_vectors).

  if(t!=BigInt(0))
  {
    d[1]=t;
    f[0]=1;
  }
  else
  {
    d[1]=1;
    f[0]=0;
  }



/// The other steps are not programmed with "goto" as in Cohen's book.
/// Instead, we enter a do-while-loop which terminates iff
/// k>=number_of_vectors.

  do
  {


/// Step 2: Incremental Gram-Schmidt

    if(k>k_max)
      /// else we can immediately go to step 3.
    {
      k_max=k;

      for(short j=0;j<=k;j++)
        if((f[j]==0) && (j<k))
          lambda[k][j]=0;
        else
        {
          BigInt u=0;

          /// compute scalar product of b[k] and b[j]
          for(short m=0;m<vector_dimension;m++)
            u+=b[k][m]*b[j][m];

          for(short i=0;i<=j-1;i++)
            if(f[i]!=0)
              u=(d[i+1]*u-lambda[k][i]*lambda[j][i])/d[i];

          if(j<k)
            lambda[k][j]=u;
          else
            /// j==k
            if(u!=BigInt(0))
            {
              d[k+1]=u;
              f[k]=1;
            }
            else
              /// u==0
            {
              d[k+1]=d[k];
              f[k]=0;
            }
        }
    }


/// Step 3: Test f[k]==0 and f[k-1]!=0

    do
    {
      if(f[k-1]!=0)
        REDI_KB(k,k-1,b,number_of_vectors,vector_dimension,H,d,lambda);

      if((f[k-1]!=0) && (f[k]==0))
      {
        SWAPK(k,k_max,b,H,f,d,lambda);

        if(k>1)
          k--;
        else
          k=1;
        /// k=max(1,k-1)
      }

      else
        break;
    }
    while(1);

    /// Now the conditions above are no longer satisfied.

    for(short l=k-2;l>=0;l--)
      if(f[l]!=0)
        REDI_KB(k,l,b,number_of_vectors,vector_dimension,H,d,lambda);
    k++;


/// Step 4: Finished?

  }
  while(k<number_of_vectors);



/// Now we have computed a lattice basis of the relations of the b[i].
/// Prepare the LLL-reduction.

  /// Compute the dimension r of the relations.
  short r=0;
  for(short n=0;n<number_of_vectors;n++)
    if(f[n]==0) /// n==r!!
      r++;
    else
      break;

  /// Delete the part of H that is no longer needed (especially the vectors
  /// H[r],...,H[number_of_vectors-1]).
  BigInt **aux=H;
  if(r>0)
    H=new BigIntP[r];
  for(short i=0;i<r;i++)
    H[i]=aux[i];

  for(short i=r;i<number_of_vectors;i++)
    delete[] aux[i];
  delete[] aux;

  delete[] f;

  delete[] d;

  for(short i=1;i<number_of_vectors;i++)
    delete[] lambda[i];
  delete[] lambda;

  integral_LLL(H,r,number_of_vectors);

  return r;

}




short integral_LLL(BigInt** b, const short& number_of_vectors,
                  const short& vector_dimension)
{

/// first check arguments

  if(number_of_vectors<0)
  {
    cerr<<"\nWARNING: short integral_LL(BigInt**, const short&, const short&):"
      "\nargument number_of_vectors out of range"<<endl;
    return -1;
  }

  if(vector_dimension<=0)
  {
    cerr<<"\nWARNING: short integral_LLL(BigInt**, const short&, const "
      "short&):\nargument vector_dimension out of range"<<endl;
    return -1;
  }


/// consider special case

  if(number_of_vectors<=1)
    /// 0 or 1 input vector, nothing to be done
    return 0;


/// memory allocation

/// The names are chosen (as far as possible) according to Cohen's book.
/// However, for technical reasons, the indices do not run from 1 to
/// (e.g.) number_of_vectors, but from 0 to number_of_vectors-1.
/// Therefore all indices are shifted by -1 in comparison with this book,
/// except from the indices of the array d which has size
/// number_of_vectors+1.

  BigInt* d=new BigInt[number_of_vectors+1];

  BigInt** lambda=new BigIntP[number_of_vectors];
  for(short s=1;s<number_of_vectors;s++)
    lambda[s]=new BigInt[s];
  /// We only need lambda[n][k] for n>k.



/// Step 1: Initialization

  short k=1;
  short k_max=0;
  /// for iteration
  d[0]=1;

  d[1]=0;
  for(short n=0;n<vector_dimension;n++)
    d[1]+=b[0][n]*b[0][n];
  /// Now, d[1] is the scalar product of b[0] with itself.


/// The other steps are not programmed with "goto" as in Cohen's book.
/// Instead, we enter a do-while-loop which terminates iff k>r.

  do
  {


/// Step 2: Incremental Gram-Schmidt

    if(k>k_max)
      /// else we can immediately go to step 3.
    {
      k_max=k;

      for(short j=0;j<=k;j++)
      {
        BigInt u=0;
        /// compute scalar product of b[k] and b[j]
        for(short n=0;n<vector_dimension;n++)
          u+=b[k][n]*b[j][n];

        for(short i=0;i<=j-1;i++)
        {
          u*=d[i+1];
          u-=lambda[k][i]*lambda[j][i];
          u/=d[i];

          ///u=(d[i+1]*u-lambda[k][i]*lambda[j][i])/d[i];
        }

        if(j<k)
          lambda[k][j]=u;
        else
          /// j==k
          d[k+1]=u;
      }

      if(d[k+1]==BigInt(0))
      {
        cerr<<"\nERROR: void integral_LLL(BigInt**, const short&, const "
          "short&):\ninput vectors must be linearly independent"<<endl;
        return -1;
      }
    }


/// Step 3: Test LLL-condition

    do
    {
      REDI_IL(k,k-1,b,vector_dimension,d,lambda);

      ///if(4*d[k+1]*d[k-1] < 3*d[k]*d[k] - lambda[k][k-1]*lambda[k][k-1])
      if((BigInt(4))*d[k+1]*d[k-1]
          < (BigInt(3))*d[k]*d[k] - lambda[k][k-1]*lambda[k][k-1])
      {
        SWAPI(k,k_max,b,d,lambda);
        if(k>1)
          k--;
        /// k=max(1,k-1)
      }
      else
        break;

    }
    while(1);

    /// Now the condition above is no longer satisfied.

    for(short l=k-2;l>=0;l--)
      REDI_IL(k,l,b,vector_dimension,d,lambda);
    k++;



/// Step 4: Finished?

  }
  while(k<number_of_vectors);


/// Now, b contains the LLL-reduced lattice basis.
/// Memory cleanup.

  delete[] d;

  for(short i=1;i<number_of_vectors;i++)
    delete[] lambda[i];
  delete[] lambda;

  return 0;

}
#endif  /// LLL_CC
