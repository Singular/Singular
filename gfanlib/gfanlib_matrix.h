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
//  std::vector<Vector<typ> > rows;
  std::vector<typ> data;
public:
  // rowIterator;
 // std::vector<Vector<typ> >::iterator rowsBegin(){return rows.begin();}
//  std::vector<Vector<typ> >::iterator rowsEnd(){return rows.end();}
  inline int getHeight()const{return height;};
  inline int getWidth()const{return width;};
  Matrix(const Matrix &a):width(a.getWidth()),height(a.getHeight()),data(a.data){
  }
  Matrix(int height_, int width_):width(width_),height(height_),data(width_*height_){
    assert(height>=0);
    assert(width>=0);
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
      for(int j=0;j<getHeight();j++)ret[j]=(*this)[j][i];
      return ret;
    }
  Matrix transposed()const
    {
      Matrix ret(getWidth(),getHeight());
      for(int i=0;i<getWidth();i++)
          for(int j=0;j<getHeight();j++)
                  ret[i][j]=(*this)[j][i];
      return ret;
    }
  static Matrix identity(int n)
    {
      Matrix m(n,n);
      for(int i=0;i<n;i++)m[i][i]=typ(1);
      return m;
    }
  void append(Matrix const &m)
    {
      assert(m.getWidth()==width);
          data.resize((height+m.height)*width);
          int oldHeight=height;
      height+=m.height;
      for(int i=0;i<m.height;i++)
        {
          for(int j=0;j<m.width;j++)
                  (*this)[i+oldHeight][j]=m[i][j];
        }
    }
  void appendRow(Vector<typ> const &v)
    {
          assert(v.size()==width);
          data.resize((height+1)*width);
          height++;
          for(int j=0;j<width;j++)
                  (*this)[height-1][j]=v[j];
    }
  void eraseLastRow()
  {
    assert(height>0);
    data.resize((height-1)*width);
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
      for(int i=0;i<q.height;i++)
          for(int j=0;j<q.width;j++)
                  p[i][j]=s*(q[i][j]);
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
        ret[i-startRow][j-startColumn]=(*this)[i][j];
    return ret;
  }

  class RowRef;
  class const_RowRef{
    int rowNumM;
    Matrix const &matrix;
    friend class RowRef;
  public:
  inline const_RowRef(const Matrix  &matrix_, int rowNum_)__attribute__((always_inline)):
    rowNumM(rowNum_*matrix_.width),
      matrix(matrix_)
      {
      }
  inline typ const &operator[](int j)const __attribute__((always_inline))
    {
        assert(j>=0);
        assert(j<matrix.width);
        return matrix.data[rowNumM+j];
    }
  inline typ const &UNCHECKEDACCESS(int j)const __attribute__((always_inline))
    {
        return matrix.data[rowNumM+j];
    }
    const Vector<typ> toVector()const
    {
      Vector<typ> ret(matrix.width);
      for(int j=0;j<matrix.width;j++)
              ret[j]=matrix.data[rowNumM+j];
      return ret;
    }
    operator Vector<typ>()const
                {
                        return toVector();
                }
    bool operator==(Vector<typ> const &b)const
                {
                        return toVector()==b;
                }
/*    typ dot(Vector<typ> const &b)const
                {
                        return dot(toVector(),b);
                }*/
    Vector<typ> operator-()const
    {
            return -toVector();
    }
  };
  class RowRef{
    int rowNumM;
    Matrix &matrix;
  public:
  inline RowRef(Matrix &matrix_, int rowNum_):
    rowNumM(rowNum_*matrix_.width),
      matrix(matrix_)
      {
      }
    inline typ &operator[](int j) __attribute__((always_inline))
      {
            assert(j>=0);
            assert(j<matrix.width);
            return matrix.data[rowNumM+j];
      }
    inline typ &UNCHECKEDACCESS(int j)
      {
            return matrix.data[rowNumM+j];
      }
    RowRef &operator=(Vector<typ> const &v)
    {
        assert(v.size()==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]=v[j];

            return *this;
    }
    RowRef &operator=(RowRef const &v)
    {
        assert(v.matrix.width==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]=v.matrix.data[v.rowNumM+j];

            return *this;
    }
/*    RowRef &operator+=(Vector<typ> const &v)
    {
        assert(v.size()==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]+=v.v[j];

            return *this;
    }*/
    RowRef &operator+=(RowRef const &v)
    {
        assert(v.matrix.width==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]+=v.matrix.data[v.rowNumM+j];

            return *this;
    }
    RowRef &operator+=(const_RowRef const &v)
    {
        assert(v.matrix.width==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]+=v.matrix.data[v.rowNumM+j];

            return *this;
    }
    RowRef &operator=(const_RowRef const &v)
    {
        assert(v.matrix.width==matrix.width);
        for(int j=0;j<matrix.width;j++)
                matrix.data[rowNumM+j]=v.matrix.data[v.rowNumM+j];

            return *this;
    }
    const Vector<typ> toVector()const
    {
      Vector<typ> ret(matrix.width);
      for(int j=0;j<matrix.width;j++)
              ret[j]=matrix.data[rowNumM+j];
      return ret;
    }
    operator Vector<typ>()const
                {
                        return toVector();
                }
/*    typ dot(Vector<typ> const &b)const
                {
                        return dot(toVector(),b);
                }*/
    bool isZero()const
        {
          for(int j=0;j<matrix.width;j++)if(!(matrix.data[rowNumM+j].isZero()))return false;
          return true;
        }
  };


  inline RowRef operator[](int i) __attribute__((always_inline))
  {
    assert(i>=0);
    assert(i<height);
    return RowRef(*this,i);
  }
  inline const_RowRef operator[](int i)const __attribute__((always_inline))
  {
    assert(i>=0);
    assert(i<height);
    return const_RowRef(*this,i);
  }


  const typ& UNCHECKEDACCESS(int i,int j)const __attribute__((always_inline)){
/*            assert(i>=0);
            assert(i<height);
            assert(j>=0);
            assert(j<width);*/
          return data[i*width+j];}
  typ& UNCHECKEDACCESS(int i,int j) __attribute__((always_inline)){
/*            assert(i>=0);
            assert(i<height);
            assert(j>=0);
            assert(j<width);*/
            return data[i*width+j];}



  bool operator<(const Matrix & b)const
  {
    if(getWidth()<b.getWidth())return true;
    if(b.getWidth()<getWidth())return false;
    if(getHeight()<b.getHeight())return true;
    if(b.getHeight()<getHeight())return false;

    for(int i=0;i<getHeight();i++)
      {
        if((*this)[i].toVector()<b[i].toVector())return true;
        if(b[i].toVector()<(*this)[i].toVector())return false;
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
      if(!(*this)[i][k].isZero())
              (*this)[j][k].madd((*this)[i][k],a);
  }

  friend std::ostream &operator<<(std::ostream &f, Matrix const &a){
    f<<"{";
    for(int i=0;i<a.getHeight();i++)
      {
        if(i)f<<","<<std::endl;
        f<<a[i].toVector();
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
    for(int a=0;a<width;a++)std::swap((*this)[i][a],(*this)[j][a]);
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
        if(!(*this)[i][j].isZero()) return true;
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
      if(!(*this)[i][column].isZero())
        {
          int nz=0;
          for(int k=column+1;k<width;k++)
            if(!(*this)[i][k].isZero())nz++;
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
                if((*this)[currentRow][i].sign()>=0)retSwaps++;
                typ inverse=typ(1)/(*this)[currentRow][i];
                //                if(!rows[currentRow][i].isOne())
                  {
                    for(int k=0;k<width;k++)
                      if(!(*this)[currentRow][k].isZero())
                        (*this)[currentRow][k]*=inverse;
                  }
              }
            for(int j=currentRow+1;j<height;j++)
              if(integral)
                {
                  if(!(*this)[j][i].isZero())
                    {
                      typ s;
                      typ t;

                      typ g=typ::gcd((*this)[currentRow][i],(*this)[j][i],s,t);
                      typ u=-(*this)[j][i]/g;
                      typ v=(*this)[currentRow][i]/g;
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
                            typ A=(*this)[currentRow][k];
                            typ B=(*this)[j][k];

                            (*this)[currentRow][k]=s*A+t*B;
                            (*this)[j][k]=u*A+v*B;
                          }
                      }
                  }
                else
                  {
                    if(!(*this)[j][i].isZero())
                      madd(currentRow,-(*this)[j][i]/(*this)[currentRow][i],j);
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
     Groebner basis terms this is the same as transforming a minimal
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
          (*this)[pivotI]=(*this)[pivotI].toVector()/(*this)[pivotI][pivotJ];
        for(int i=0;i<pivotI;i++)
          if(!(*this)[i][pivotJ].isZero())
            madd(pivotI,-(*this)[i][pivotJ]/(*this)[pivotI][pivotJ],i);
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
        typ s=-v[pivotJ]/(*this)[pivotI][pivotJ];

        for(int k=0;k<width;k++)
          if(!(*this)[pivotI][k].isZero())
            v[k].madd((*this)[pivotI][k],s);
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
            ret[k][pivot2J]=(*this)[pivot2I][j]/(*this)[pivot2I][pivot2J];
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
          diagonalProduct*=(*this)[pivot2I][pivot2J];
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
          ret[pivot2J]=(*this)[pivot2I][j]/(*this)[pivot2I][pivot2J];
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
  struct rowComparer{
    bool operator()(std::pair<Matrix*,int> i, std::pair<Matrix*,int> j) {return ((*i.first)[i.second].toVector()<(*j.first)[j.second].toVector());}
  } theRowComparer;
  void sortRows()
  {
          std::vector<std::pair<Matrix*,int> > v;
          for(int i=0;i<height;i++)v.push_back(std::pair<Matrix*,int>(this,i));
          std::sort(v.begin(),v.end(),theRowComparer);
          Matrix result(height,width);
          for(int i=0;i<height;i++)
                  result[i]=(*this)[v[i].second].toVector();
          data=result.data;
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
      if((*this)[i].toVector()!=(*this)[i-1].toVector())B.appendRow((*this)[i].toVector());
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
    for(int i=0;i<top.getHeight();i++)ret[i]=top[i];
    for(int i=0;i<bottom.getHeight();i++)ret[i+top.getHeight()]=bottom[i];

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
        for(int j=0;j<left.getWidth();j++)ret[i][j]=left[i][j];
        for(int j=0;j<right.getWidth();j++)ret[i][j+left.getWidth()]=right[i][j];
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
