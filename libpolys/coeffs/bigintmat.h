/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: class bigintmat: matrizes of big integers
*/

#ifndef BIGINTMAT_H
#define BIGINTMAT_H

#include <omalloc/omalloc.h>
#include <resources/feFopen.h>
#include <coeffs/coeffs.h>

/// matrix of numbers
/// NOTE: no reference counting!!!
class bigintmat
{
  private:
    coeffs m_coeffs;
    number *v;
    int row;
    int col;
  public:

    bigintmat(): m_coeffs(NULL), v(NULL), row(1), col(0){}

    bigintmat * transpose();

    bigintmat(int r, int c, const coeffs n): m_coeffs(n), v(NULL), row(r), col(c)
    {
      assume (rows() >= 0);
      assume (cols() >= 0);

      const int l = r*c;

      if (l>0) /*(r>0) && (c>0) */
      {
        v = (number *)omAlloc(sizeof(number)*l);

        assume (basecoeffs() != NULL);
        for (int i = l - 1; i>=0; i--)
        {
          v[i] = n_Init(0, basecoeffs());
        }
      }
    }

    bigintmat(const bigintmat *m): m_coeffs(m->basecoeffs()), v(NULL), row(m->rows()), col(m->cols())
    {
      const int l = row*col;

      if (l > 0)
      {
        assume (rows() >= 0);
        assume (cols() >= 0);

        assume (m->v != NULL);

        v = (number *)omAlloc(sizeof(number)*row*col);

        assume (basecoeffs() != NULL);

        for (int i = l-1; i>=0; i--)
        {
          v[i] = n_Copy((*m)[i], basecoeffs());
        }
      }
    }

    inline number& operator[](int i)
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong bigintmat index:%d\n",i);
      }
#endif
      assume ( !((i<0)||(i>=row*col)) );

      return v[i];  // Hier sollte imho kein nlCopy rein...
    }
    inline const number& operator[](int i) const
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong bigintmat index:%d\n",i);
      }
#endif
      assume ( !((i<0)||(i>=row*col)) );

      return v[i];
    }
#define BIMATELEM(M,I,J) (M)[(I-1)*(M).cols()+J-1]

    /// UEberladener *=-Operator (fuer int und bigint)
    /// Frage hier: *= verwenden oder lieber = und * einzeln?
    void operator*=(int intop);

    void inpMult(number bintop, const coeffs C = NULL);

    inline int length() { return col*row; }
    inline int  cols() const { return col; }
    inline int  rows() const { return row; }
    inline coeffs basecoeffs() const { return m_coeffs; }

    ~bigintmat()
    {
      if (v!=NULL)
      {
        for (int i=0; i<row*col; i++) { n_Delete(&(v[i]), basecoeffs()); }
        omFreeSize((ADDRESS)v, sizeof(number)*row*col);
        v=NULL;
      }
    }

    int index(int r, int c) const
    {
      assume (rows() >= 0 && cols() >= 0);

      assume (r > 0 && c > 0);
      assume (r <= rows() && c <= cols());

      const int index = ((r-1)*cols() + (c-1));

      assume (index >= 0 && index < rows() * cols());
      return index;
    }

    /// get a copy of an entry. NOTE: starts at [1,1]
    number get(int i, int j) const;

    /// get a copy of an entry. NOTE: starts at [0]
    number get(int i) const;

    /// replace an entry with a copy (delete old + copy new!).
    /// NOTE: starts at [1,1]
    void set(int i, int j, number n, const coeffs C = NULL);

    /// replace an entry with a copy (delete old + copy new!).
    /// NOTE: starts at [0]
    void set(int i, number n, const coeffs C = NULL);


    /// replace an entry with the given number n (only delete old).
    /// NOTE: starts at [0]
    inline void rawset(int i, number n, const coeffs C = NULL)
    {
      assume (C == NULL || C == basecoeffs());
      assume (i >= 0);
      const int l = rows() * cols();
      assume (i<l);

      if (i < l)
      {
        n_Delete(&(v[i]), basecoeffs()); v[i] = n;
      }
#ifndef SING_NDEBUG
      else
      {
        Werror("wrong bigintmat index:%d\n",i);
      }
#endif
    }

    inline void rawset(int i, int j, number n, const coeffs C = NULL)
    {
      rawset( index(i,j), n, C);
    }

    char * String();
/***
 * Returns a string as it would have been printed in the interpreter.
 * Used e.g. in print functions of various blackbox types.
 **/
    char * StringAsPrinted();
    void pprint(int maxwid);
    int compare(const bigintmat* op) const;
    int * getwid(int maxwid);
};

bool operator==(const bigintmat & lhr, const bigintmat & rhr);
bool operator!=(const bigintmat & lhr, const bigintmat & rhr);

/// Matrix-Add/-Sub/-Mult so oder mit operator+/-/* ?
/// NOTE: NULL as a result means an error (non-compatible matrices?)
bigintmat * bimAdd(bigintmat * a, bigintmat * b);
bigintmat * bimAdd(bigintmat * a, int b);
bigintmat * bimSub(bigintmat * a, bigintmat * b);
bigintmat * bimSub(bigintmat * a, int b);
bigintmat * bimMult(bigintmat * a, bigintmat * b);
bigintmat * bimMult(bigintmat * a, int b);
bigintmat * bimMult(bigintmat * a, number b, const coeffs cf);
bigintmat * bimCopy(const bigintmat * b);

int getShorter (int * a, int l, int j, int cols, int rows);
int findLongest(int * a, int length);
int intArrSum(int * a, int length);

class intvec;
intvec * bim2iv(bigintmat * b);
bigintmat * iv2bim(intvec * b, const coeffs C);
bigintmat * bimConcat(bigintmat * a, bigintmat * b, const coeffs C);
#endif // #ifndef BIGINTMAT_H
