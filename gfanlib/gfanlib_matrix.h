/*
 * lib_zmatrix.h
 *
 *  Created on: Sep 28, 2010
 *      Author: anders
 */

#ifndef LIB_ZMATRIX_H_
#define LIB_ZMATRIX_H_

#include <vector>
#include <algorithm>
#include "gfanlib_vector.h"

namespace gfan{

template <class typ> class Matrix{
  int width,height;
  std::vector<Vector<typ> > rows;
public:
  // rowIterator;
 // std::vector<Vector<typ> >::iterator rowsBegin(){return rows.begin();}
//  std::vector<Vector<typ> >::iterator rowsEnd(){return rows.end();}
  inline int getHeight()const{return height;};
  inline int getWidth()const{return width;};
  Matrix(const Matrix &a):width(a.getWidth()),height(a.getHeight()),rows(a.rows){
  }
  Matrix(int height_, int width_):width(width_),height(height_),rows(height_){
    assert(height>=0);
    assert(width>=0);
    for(int i=0;i<getHeight();i++)rows[i]=Vector<typ>(width);
  };
  ~Matrix(){
  };
  Matrix():width(0),height(0){
  };
  static Matrix rowVectorMatrix(Vector<typ> const &v)
  {
    Matrix ret(1,v.size());
    for(int i=0;i<v.size();i++)ret[0][i]=v[i];
    return ret;
  }
  Vector<typ> column(int i)const
    {
      assert(i>=0);
      assert(i<getWidth());
      Vector<typ> ret(getHeight());
      for(int j=0;j<getHeight();j++)ret[j]=rows[j][i];
      return ret;
    }
  Matrix transposed()const
    {
      Matrix ret(getWidth(),getHeight());
      for(int i=0;i<getWidth();i++)
        ret.rows[i]=column(i);
      return ret;
    }
  static Matrix identity(int n)
    {
      Matrix m(n,n);
      for(int i=0;i<n;i++)m.rows[i]=Vector<typ>::standardVector(n,i);
      return m;
    }
  void append(Matrix const &m)
    {
      for(int i=0;i<m.height;i++)
        {
          rows.push_back(m[i]);
        }
      height+=m.height;
    }
  void appendRow(Vector<typ> const &v)
    {
      assert((int)v.size()==width);
      rows.push_back(v);
      height++;
    }
  void eraseLastRow()
  {
    assert(rows.size()>0);
    rows.pop_back();
    height--;
  }
  /*IntegerVector vectormultiply(IntegerVector const &v)const
    {
      assert(v.size()==width);
      IntegerVector ret(height);
      for(int i=0;i<height;i++)
        ret[i]=dot(rows[i],v);
      return ret;
    }*/
  /**
   * Decides if v is in the kernel of the matrix.
   */
/*  bool inKernel(IntegerVector const &v)const
    {
      assert(v.size()==width);
      for(int i=0;i<height;i++)
          if(dotLong(rows[i],v)!=0)return false;
      return true;
    }
*/
  friend Matrix operator*(const typ &s, const Matrix& q)
    {
      Matrix p=q;
      for(int i=0;i<q.height;i++)p[i]=s*(q[i]);
      return p;
    }
  friend Matrix operator*(const Matrix& a, const Matrix& b)
    {
      assert(a.width==b.height);
      Matrix ret(b.width,a.height);
      for(int i=0;i<b.width;i++)
        ret[i]=a.vectormultiply(b.column(i));
      return ret.transposed();
    }
  /*  template<class T>
    Matrix<T>(const Matrix<T>& c):v(c.size()){
    for(int i=0;i<size();i++)v[i]=typ(c[i]);}
  */
  friend Matrix operator-(const Matrix &b)
  {
    Matrix ret(b.height,b.width);
    for(int i=0;i<b.height;i++)ret[i]=-b[i];
    return ret;
  }

  /**
     Returns the specified submatrix. The endRow and endColumn are not included.
   */
  Matrix submatrix(int startRow, int startColumn, int endRow, int endColumn)const
  {
    assert(startRow>=0);
    assert(startColumn>=0);
    assert(endRow>=startRow);
    assert(endColumn>=startColumn);
    assert(endRow<=height);
    assert(endColumn<=width);
    Matrix ret(endRow-startRow,endColumn-startColumn);
    for(int i=startRow;i<endRow;i++)
      for(int j=startColumn;j<endColumn;j++)
        ret[i-startRow][j-startColumn]=rows[i][j];
    return ret;
  }
  const Vector<typ>& operator[](int n)const{assert(n>=0 && n<getHeight());return (rows[n]);}
// Bugfix for gcc4.5 (passing assertion to the above operator):
  Vector<typ>& operator[](int n){if(!(n>=0 && n<getHeight())){(*(const Matrix<typ>*)(this))[n];}return (rows[n]);}


  bool operator<(const Matrix & b)const
  {
    if(getWidth()<b.getWidth())return true;
    if(b.getWidth()<getWidth())return false;
    if(getHeight()<b.getHeight())return true;
    if(b.getHeight()<getHeight())return false;

    for(int i=0;i<getHeight();i++)
      {
        if((*this)[i]<b[i])return true;
        if(b[i]<(*this)[i])return false;
      }
    return false;
  }
  /**
     Adds a times the i th row to the j th row.
  */
  void madd(int i, typ a, int j)
  {
    assert(i!=j);
    assert(i>=0 && i<height);
    assert(j>=0 && j<height);

    if(!a.isZero())
    for(int k=0;k<width;k++)
      if(!rows[i][k].isZero())
        rows[j][k].madd(rows[i][k],a);
  }

  friend std::ostream &operator<<(std::ostream &f, Matrix const &a){
    f<<"{";
    for(int i=0;i<a.getHeight();i++)
      {
        if(i)f<<","<<std::endl;
        f<<a.rows[i];
      }
    f<<"}"<<std::endl;
    return f;
  }

  std::string toString()const
  {
	  std::stringstream f;
	  f<<*this;
	  return f.str();
  }

  /**
     Swaps the i th and the j th row.
   */
  void swapRows(int i, int j)
  {
    std::swap(rows[i],rows[j]);
  }
  /**
     This method is used for iterating through the pivots in a matrix
     in row echelon form. To find the first pivot put i=-1 and
     j=-1 and call this routine. The (i,j) th entry of the matrix is a
     pivot. Call the routine again to get the next pivot. When no more
     pivots are found the routine returns false.
  */
  bool nextPivot(int &i, int &j)const
  {
    i++;
    if(i>=height)return false;
    while(++j<width)
      {
        if(!rows[i][j].isZero()) return true;
      }
    return false;
  }
  /**
     Returns the indices of the columns containing a pivot.
     The returned list is sorted.
     The matrix must be in row echelon form.
   */
  std::vector<int> pivotColumns()const
  {
    std::vector<int> ret;
    int pivotI=-1;
    int pivotJ=-1;
    while(nextPivot(pivotI,pivotJ))ret.push_back(pivotJ);
    return ret;
  }
  /**
     Returns the indices of the columns not containing a pivot.
     The returned list is sorted.
     The matrix must be in row echelon form.
   */
  std::vector<int> nonPivotColumns()const
  {
    std::vector<int> ret;
    int pivotI=-1;
    int pivotJ=-1;
    int firstPossiblePivot=0;
    while(nextPivot(pivotI,pivotJ))
      {
        for(int j=firstPossiblePivot;j<pivotJ;j++)
          ret.push_back(j);
        firstPossiblePivot=pivotJ+1;
      }
    for(int j=firstPossiblePivot;j<getWidth();j++)
      ret.push_back(j);

    return ret;
  }
  /**
     This routine removes the zero rows of the matrix.
   */
  void removeZeroRows()
  {
    int nonZeros=0;
    for(int i=0;i<height;i++)if(!(*this)[i].isZero())nonZeros++;
    if(nonZeros==height)return;

    Matrix b(nonZeros,width);

    int j=0;
    for(int i=0;i<height;i++)
      {
        if(!(*this)[i].isZero())
          {
            b[j]=(*this)[i];
            j++;
          }
      }
    *this=b;
  }
  /**
     Returns the index of a row whose index is at least currentRow and
     which has a non-zero element on the column th entry. If no such
     row exists then -1 is returned. This routine is used in the Gauss
     reduction. To make the reduction more efficient the routine
     chooses its row with as few non-zero entries as possible.
   */
  int findRowIndex(int column, int currentRow)const
  {
    int best=-1;
    int bestNumberOfNonZero=0;
    for(int i=currentRow;i<height;i++)
      if(!rows[i][column].isZero())
        {
          int nz=0;
          for(int k=column+1;k<width;k++)
            if(!rows[i][k].isZero())nz++;
          if(best==-1)
            {
              best=i;
              bestNumberOfNonZero=nz;
            }
          else if(nz<bestNumberOfNonZero)
            {
              best=i;
              bestNumberOfNonZero=nz;
            }
        }
    return best;
  }
  /**
     Performs a Gauss reduction and returns the number of row swaps (and negative scalings)
     done. The result is a matrix in row echelon form. The pivots may
     not be all 1.  In terms of Groebner bases, what is computed is a
     minimal (not necessarily reduced) Groebner basis of the linear
     ideal generated by the rows.  The number of swaps is need if one
     wants to compute the determinant afterwards. In this case it is
     also a good idea to set the flag returnIfZeroDeterminant which
     make the routine terminate before completion if it discovers that
     the determinant is zero.
  */
  int reduce(bool returnIfZeroDeterminant=false, bool integral=false, bool makePivotsOne=false)
  {
    assert(integral || typ::isField());
    assert(!makePivotsOne || !integral);

    int retSwaps=0;
    int currentRow=0;

    for(int i=0;i<width;i++)
      {
        int s=findRowIndex(i,currentRow);

        if(s!=-1)
          {
            if(s!=currentRow)
              {
                swapRows(currentRow,s);
                retSwaps++;
              }
            if(makePivotsOne)
              {//THE PIVOT SHOULD BE SET TO ONE IF INTEGRAL IS FALSE
                if(rows[currentRow][i].sign()>=0)retSwaps++;
                typ inverse=typ(1)/rows[currentRow][i];
                //                if(!rows[currentRow][i].isOne())
                  {
                    for(int k=0;k<width;k++)
                      if(!rows[currentRow][k].isZero())
                        rows[currentRow][k]*=inverse;
                  }
              }
            for(int j=currentRow+1;j<height;j++)
              if(integral)
                {
                  if(!rows[j][i].isZero())
                    {
                      typ s;
                      typ t;

                      typ g=typ::gcd(rows[currentRow][i],rows[j][i],s,t);
                      typ u=-rows[j][i]/g;
                      typ v=rows[currentRow][i]/g;
                        /* We want the (s,t) vector to be as small as possible.
                         * We are allowed to adjust by multiples of (u,v).
                         * The following computes the correct multiplier (in most cases).
                         */
/*                        {
                          FieldElement multiplier=(s*u+t*v)*((u*u+v*v).inverse());
                          double d=mpq_get_d(*(multiplier.getGmpRationalTemporaryPointer()));
                          multiplier=multiplier.getField()->zHomomorphism(-(((int)(d+10000.5))-10000));
                          s.madd(multiplier,u);
                          t.madd(multiplier,v);
                        }*/
                        for(int k=0;k<width;k++)
                          {
                            typ A=rows[currentRow][k];
                            typ B=rows[j][k];

                            rows[currentRow][k]=s*A+t*B;
                            rows[j][k]=u*A+v*B;
                          }
                      }
                  }
                else
                  {
                    if(!rows[j][i].isZero())
                      madd(currentRow,-rows[j][i]/rows[currentRow][i],j);
                  }
              currentRow++;
            }
          else
            if(returnIfZeroDeterminant)return -1;
        }

      return retSwaps;
    }
  /**
     Computes a reduced row echelon form from a row echelon form. In
     Groebner basis terms this is the same as tranforming a minimal
     Groebner basis to a reduced one except that we do not force
     pivots to be 1 unless the scalePivotsToOne parameter is set.
   */
  int REformToRREform(bool scalePivotsToOne=false)
  {
    int ret=0;
    int pivotI=-1;
    int pivotJ=-1;
    while(nextPivot(pivotI,pivotJ))
      {
        if(scalePivotsToOne)
          rows[pivotI]=rows[pivotI]/rows[pivotI][pivotJ];
        for(int i=0;i<pivotI;i++)
          if(!rows[i][pivotJ].isZero())
            madd(pivotI,-rows[i][pivotJ]/rows[pivotI][pivotJ],i);
      }
    return ret;
  }
  /**
     This function may be called if the FieldMatrix is in Row Echelon
     Form. The input is a FieldVector which is rewritten modulo the
     rows of the matrix. The result is unique and is the same as the
     normal form of the input vector modulo the Groebner basis of the
     linear ideal generated by the rows of the matrix.
  */
  Vector<typ> canonicalize(Vector<typ> v)const
  {
    assert(typ::isField());
    assert((int)v.size()==getWidth());

    int pivotI=-1;
    int pivotJ=-1;

    while(nextPivot(pivotI,pivotJ))
      if(!v[pivotJ].isZero())
      {
        typ s=-v[pivotJ]/rows[pivotI][pivotJ];

        for(int k=0;k<width;k++)
          if(!rows[pivotI][k].isZero())
            v[k].madd(rows[pivotI][k],s);
      }
    return v;
  }
  /**
     Calls reduce() and constructs matrix whose rows forms a basis of
     the kernel of the linear map defined by the original matrix. The
     return value is the new matrix.
   */
  Matrix reduceAndComputeKernel()
  {
    Matrix ret(width-reduceAndComputeRank(),width);

    REformToRREform();

    int k=0;
    int pivotI=-1;
    int pivotJ=-1;
    bool pivotExists=nextPivot(pivotI,pivotJ);
    for(int j=0;j<width;j++)
      {
        if(pivotExists && (pivotJ==j))
          {
            pivotExists=nextPivot(pivotI,pivotJ);
            continue;
          }
        int pivot2I=-1;
        int pivot2J=-1;
        while(nextPivot(pivot2I,pivot2J))
          {
            ret[k][pivot2J]=rows[pivot2I][j]/rows[pivot2I][pivot2J];
          }
        ret[k][j]=typ(-1);
        k++;
      }
    return ret;
  }
  /**
     Assumes that the matrix has a kernel of dimension 1.
     Calls reduce() and returns a non-zero vector in the kernel.
     If the matrix is an (n-1)x(n) matrix then the returned vector has
     the property that if it was appended as a row to the original matrix
     then the determinant of that matrix would be positive. Of course
     this property, as described here, only makes sense for ordered fields.
     Only allowed for fields at the moment.
   */
  Vector<typ> reduceAndComputeVectorInKernel()
  {
    assert(typ::isField());
    // TODO: (optimization) if the field is ordered, then it is better to just keep track of signs rather than
    // multiplying by sign*diagonalProduct*lastEntry at the end.
    int nswaps=this->reduce();
    typ sign=typ(1-2*(nswaps&1));
    int rank=reduceAndComputeRank();
    assert(rank+1==width);

    REformToRREform();

    Vector<typ> ret(width);

    typ diagonalProduct(1);
    {
      int pivot2I=-1;
      int pivot2J=-1;
      while(nextPivot(pivot2I,pivot2J))
        {
          diagonalProduct*=rows[pivot2I][pivot2J];
        }
    }
    {
      int j=nonPivotColumns().front();
      int pivot2I=-1;
      int pivot2J=-1;
      ret[j]=typ(-1);
      // Pretend that we are appending ret to the matrix, and reducing this
      // new row by the previous ones. The last entry of the resulting matrix
      // is lastEntry.
      typ lastEntry=ret[j];
      while(nextPivot(pivot2I,pivot2J))
        {
          ret[pivot2J]=rows[pivot2I][j]/rows[pivot2I][pivot2J];
          lastEntry-=ret[pivot2J]*ret[pivot2J];
        }
      ret=(sign*(diagonalProduct*lastEntry))*ret;
    }

    return ret;
  }

  /**
     Calls reduce() and returns the rank of the matrix.
   */
  int reduceAndComputeRank()
  {
    reduce(false,!typ::isField(),false);
    int ret=0;
    int pivotI=-1;
    int pivotJ=-1;
    while(nextPivot(pivotI,pivotJ))ret++;
    return ret;
  }
  /**
   * Sort the rows of the matrix.
   */
  void sortRows()
  {
    std::sort(rows.begin(),rows.end());
  }
  /**
   * Sort the rows of the matrix and remove duplicate rows.
   */
  void sortAndRemoveDuplicateRows()
  {
    sortRows();
    if(getHeight()==0)return;
    Matrix B(0,getWidth());
    B.appendRow((*this)[0]);
    for(int i=1;i<getHeight();i++)
      if(rows[i]!=rows[i-1])B.appendRow((*this)[i]);
    *this=B;
  }
  /**
     Takes two matrices with the same number of columns and construct
     a new matrix which has the rows of the matrix top on the top and
     the rows of the matrix bottom at the bottom. The return value is
     the constructed matrix.
   */
  friend Matrix combineOnTop(Matrix const &top, Matrix const &bottom)
  {
    assert(bottom.getWidth()==top.getWidth());
    Matrix ret(top.getHeight()+bottom.getHeight(),top.getWidth());
    for(int i=0;i<top.getHeight();i++)ret.rows[i]=top.rows[i];
    for(int i=0;i<bottom.getHeight();i++)ret.rows[i+top.getHeight()]=bottom.rows[i];

    return ret;
  }
  /**
     Takes two matrices with the same number of rows and construct
     a new matrix which has the columns of the matrix left on the left and
     the columns of the matrix right on the right. The return value is
     the constructed matrix.
   */
  friend Matrix combineLeftRight(Matrix const &left, Matrix const &right)
  {
    assert(left.getHeight()==right.getHeight());
    Matrix ret(left.getHeight(),left.getWidth()+right.getWidth());
    for(int i=0;i<left.getHeight();i++)
      {
        for(int j=0;j<left.getWidth();j++)ret.rows[i][j]=left.rows[i][j];
        for(int j=0;j<right.getWidth();j++)ret.rows[i][j+left.getWidth()]=right.rows[i][j];
      }
    return ret;
  }
};

typedef Matrix<Integer> ZMatrix;
typedef Matrix<Rational> QMatrix;
typedef Matrix<int> IntMatrix;

inline QMatrix ZToQMatrix(ZMatrix const &m)
{
  QMatrix ret(m.getHeight(),m.getWidth());
  for(int i=0;i<m.getHeight();i++)ret[i]=ZToQVector(m[i]);
  return ret;
}

inline ZMatrix QToZMatrixPrimitive(QMatrix const &m)
{
  ZMatrix ret(m.getHeight(),m.getWidth());
  for(int i=0;i<m.getHeight();i++)ret[i]=QToZVectorPrimitive(m[i]);
  return ret;
}


inline IntMatrix ZToIntMatrix(ZMatrix const &m)
{
  IntMatrix ret(m.getHeight(),m.getWidth());
  for(int i=0;i<m.getHeight();i++)ret[i]=ZToIntVector(m[i]);
  return ret;
}


inline ZMatrix IntToZMatrix(IntMatrix const &m)
{
  ZMatrix ret(m.getHeight(),m.getWidth());
  for(int i=0;i<m.getHeight();i++)ret[i]=IntToZVector(m[i]);
  return ret;
}

inline QMatrix canonicalizeSubspace(QMatrix const &m)
{
  QMatrix temp=m;
  temp.reduce();
  temp.REformToRREform();
  temp.removeZeroRows();
  return temp;
}

inline ZMatrix canonicalizeSubspace(ZMatrix const &m)
{
  return QToZMatrixPrimitive(canonicalizeSubspace(ZToQMatrix(m)));
}


inline QMatrix kernel(QMatrix const &m)
{
  QMatrix temp=m;
  return temp.reduceAndComputeKernel();
}

inline ZMatrix kernel(ZMatrix const &m)
{
  return QToZMatrixPrimitive(kernel(ZToQMatrix(m)));
}

}


#endif /* LIB_ZMATRIX_H_ */
