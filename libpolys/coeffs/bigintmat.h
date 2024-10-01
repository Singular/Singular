/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: class bigintmat: matrices of number
*
* Matrices are stored as 1-dim c-arrays but interpreted 2-dim as matrices.
* Both modes of addressing are supported, note however, that the 1-dim
* addressing starts at 0, the 2-dim at 1.
*
* Matrices are meant to represent column modules, thus the default
* operations are always by column.
*
* While basic operations are supported over any ring (coeff), some more
* advanced ones require more special rings: eg. echelon forms, solving
* of linear equations is only effective over principal ideal or even
* Euclidean rings.
*
* Be careful with the get/set/view/rawset functions to understand which
* arguments are copied/ deleted or only assigned.
*/

#ifndef BIGINTMAT_H
#define BIGINTMAT_H

#include "coeffs/coeffs.h"

/**
 * @class bigintmat bigintmat.h <coeffs/bigintmat.h>
 *
 * Matrices of numbers
 *
 * Matrices are stored as 1-dim c-arrays but interpreted 2-dim as matrices.
 * Both modes of addressing are supported, note however, that the 1-dim
 * addressing starts at 0, the 2-dim at 1.
 *
 * Matrices are meant to represent column modules, thus the default
 * operations are always by column.
 *
 * While basic operations are supported over any ring (coeff), some more
 * advanced ones require more special rings: eg. echelon forms, solving
 * of linear equations is only effective over principal ideal or even
 * Euclidean rings.
 *
 * Be careful with the get/set/view/rawset functions to understand which
 * arguments are copied/ deleted or only assigned.
 *
 * @Note: no reference counting here!
 */
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

    /// transpose in place
    void inpTranspose();


    /// constructor: the r times c zero-matrix. Beware that the creation
    /// of a large zero matrix is expensive in terms of time and memory.
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

    /// copy constructor
    bigintmat(const bigintmat *m): m_coeffs(m->basecoeffs()), v(NULL), row(m->rows()), col(m->cols())
    {
      const int l = row*col;

      if (l > 0)
      {
        assume (rows() > 0);
        assume (cols() > 0);

        assume (m->v != NULL);

        v = (number *)omAlloc(sizeof(number)*row*col);

        assume (basecoeffs() != NULL);

        for (int i = l-1; i>=0; i--)
        {
          v[i] = n_Copy((*m)[i], basecoeffs());
        }
      }
    }
    /// dubious: 1-dim access to 2-dim array. Entries are read row by row.
    inline number& operator[](int i)
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong bigintmat index:%d\n",i);
      }
      assume ( !((i<0)||(i>=row*col)) );
#endif
      return v[i];  // Hier sollte imho kein nlCopy rein...
    }
    inline const number& operator[](int i) const
    {
#ifndef SING_NDEBUG
      if((i<0)||(i>=row*col))
      {
        Werror("wrong bigintmat index:%d\n",i);
      }
      assume ( !((i<0)||(i>=row*col)) );
#endif
      return v[i];
    }
#define BIMATELEM(M,I,J) (M)[(I-1)*(M).cols()+J-1]

    /// UEberladener *=-Operator (fuer int und bigint)
    /// Frage hier: *= verwenden oder lieber = und * einzeln?
    /// (coeffs in Singular are always commutative)
    void operator*=(int intop);

    /// inplace version of skalar mult. CHANGES input.
    void inpMult(number bintop, const coeffs C = NULL);

    inline int length() { return col*row; }
    inline int  cols() const { return col; }
    inline int  rows() const { return row; }
    inline coeffs basecoeffs() const { return m_coeffs; }

    /// canonical destructor.
    ~bigintmat()
    {
      if (v!=NULL)
      {
        for (int i=row*col-1;i>=0; i--) { n_Delete(&(v[i]), basecoeffs()); }
        omFreeSize((ADDRESS)v, sizeof(number)*row*col);
        v=NULL;
      }
    }

    /// helper function to map from 2-dim coordinates, starting by 1 to
    /// 1-dim coordinate, starting by 0
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
    /// view an entry an entry. NOTE: starts at [1,1]
    //do NOT delete.
    number view(int i, int j) const;

    /// get a copy of an entry. NOTE: starts at [0]
    number get(int i) const;
    /// view an entry. NOTE: starts at [0]
    number view(int i) const;

    /// replace an entry with a copy (delete old + copy new!).
    /// NOTE: starts at [1,1]
    void set(int i, int j, number n, const coeffs C = NULL);

    /// replace an entry with a copy (delete old + copy new!).
    /// NOTE: starts at [0]
    void set(int i, number n, const coeffs C = NULL);


    /// replace an entry with the given number n (only delete old).
    /// NOTE: starts at [0]. Should be named set_transfer
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

    /// as above, but the 2-dim version
    inline void rawset(int i, int j, number n, const coeffs C = NULL)
    {
      rawset( index(i,j), n, C);
    }

    ///IO: String returns a singular string containing the matrix, needs
    /// freeing afterwards
    char * String();
    ///IO: writes the matrix into the current internal string buffer which
    /// must be started/ allocated before (e.g. @ref StringSetS)
    void Write();
    ///IO: simply prints the matrix to the current output (screen?)
    void Print();

   /**
    * Returns a string as it would have been printed in the interpreter.
    * Used e.g. in print functions of various blackbox types.
    */
    char * StringAsPrinted();
    void pprint(int maxwid);
    int compare(const bigintmat* op) const;
    int * getwid(int maxwid);


    // Funktionen von Kira, Jan, Marco
    // !WICHTIG: Überall, wo eine number übergeben wird, und damit gearbeitet wird, die coeffs mitübergeben und erst
    // überprüfen, ob diese mit basecoeffs übereinstimmen. Falls nein: Breche ab!

    /// swap columns i and j
    void swap(int i, int j);

    /// swap rows i and j
    void swaprow(int i, int j);

    ///find index of 1st non-zero entry in row i
    int findnonzero(int i);

    ///find index of 1st non-zero entry in column j
    int findcolnonzero(int j);

    ///copies the j-th column into the matrix a - which needs to be pre-allocated with the correct size.
    void getcol(int j, bigintmat *a);

    ///copies the no-columns staring by j (so j...j+no-1) into the pre-allocated a
    void getColRange(int j, int no, bigintmat *a);

    void getrow(int i, bigintmat *a); ///< Schreibt i-te Zeile in Vektor (Matrix) a
    void setcol(int j, bigintmat *m); ///< Setzt j-te Spalte gleich übergebenem Vektor (Matrix) m
    void setrow(int i, bigintmat *m); ///< Setzt i-te Zeile gleich übergebenem Vektor (Matrix) m

    ///horizontally join the matrices, m <- m|a
    void appendCol (bigintmat *a);

    ///append i zero-columns to the matrix
    void extendCols (int i);

    bool add(bigintmat *b); ///< Addiert zur Matrix die Matrix b dazu. Return false => an error occurred
    bool sub(bigintmat *b); ///< Subtrahiert ...
    bool skalmult(number b, coeffs c); ///< Multipliziert zur Matrix den Skalar b hinzu
    bool addcol(int i, int j, number a, coeffs c); ///< addiert a-faches der j-ten Spalte zur i-ten dazu
    bool addrow(int i, int j, number a, coeffs c); ///< ... Zeile ...
    void colskalmult(int i, number a, coeffs c); ///< Multipliziert zur i-ten Spalte den Skalar a hinzu
    void rowskalmult(int i, number a, coeffs c); ///< ... Zeile ...
    void coltransform(int i, int j, number a, number b, number c, number d); ///<  transforms cols (i,j) using the 2x2 matrix ((a,b)(c,d)) (hopefully)
    void concatrow(bigintmat *a, bigintmat *b); ///< Fügt zwei Matrixen untereinander/nebeneinander in gegebene Matrix ein, bzw spaltet gegebenen Matrix auf
    void concatcol(bigintmat *a, bigintmat *b);
    void splitrow(bigintmat *a, bigintmat *b); ///< Speichert in Matrix a den oberen, in b den unteren Teil der Matrix, vorausgesetzt die Dimensionen stimmen überein
    void splitcol(bigintmat *a, bigintmat *b); ///< ... linken ... rechten ...
    void splitcol(bigintmat *a, int i); ///< Speichert die ersten i Spalten als Teilmatrix in a
    void splitrow(bigintmat *a, int i); ///< ... Zeilen ...
    bool copy(bigintmat *b); ///< Kopiert Einträge von b auf Bigintmat
    void copySubmatInto(bigintmat *, int sr, int sc, int nr, int nc, int tr, int tc);
    void one(); ///< Macht Matrix (Falls quadratisch) zu Einheitsmatrix
    int isOne(); ///< is matrix is identity
    void zero(); ///< Setzt alle Einträge auf 0
    int isZero();
    int colIsZero(int i);
    bigintmat *elim(int i, int j); ///< Liefert Streichungsmatrix (i-te Zeile und j-te Spalte gestrichen) zurück
    number pseudoinv(bigintmat *a); ///< Speichert in Matrix a die Pseudoinverse, liefert den Nenner zurück
    number trace(); ///< the trace ....
    number det(); ///< det (via LaPlace in general, hnf for euc. rings)
    number hnfdet(); ///< det via HNF
    /// Primzahlen als long long int, müssen noch in number umgewandelt werden?
    void hnf(); ///< transforms INPLACE to HNF
    void howell(); ///<dito, but Howell form (only different for zero-divsors)
    void swapMatrix(bigintmat * a);
    bigintmat * modhnf(number p, coeffs c); ///< computes HNF(this | p*I)
    bigintmat * modgauss(number p, coeffs c);
    void skaldiv(number b); ///< Macht Ganzzahldivision aller Matrixeinträge mit b
    void colskaldiv(int j, number b); ///< Macht Ganzzahldivision aller j-ten Spalteneinträge mit b
    void mod(number p); ///< Reduziert komplette Matrix modulo p
    bigintmat* inpmod(number p, coeffs c); ///< Liefert Kopie der Matrix zurück, allerdings im Ring Z modulo p
    number content(); ///<the content, the gcd of all entries. Only makes sense for Euclidean rings (or possibly constructive PIR)
    void simplifyContentDen(number *den); ///< ensures that Gcd(den, content)=1
    ///< enden hier wieder
};

bool operator==(const bigintmat & lhr, const bigintmat & rhr);
bool operator!=(const bigintmat & lhr, const bigintmat & rhr);

/// Matrix-Add/-Sub/-Mult so oder mit operator+/-/* ?
/// @Note: NULL as a result means an error (non-compatible matrices?)
bigintmat * bimAdd(bigintmat * a, bigintmat * b);
bigintmat * bimAdd(bigintmat * a, long b);
bigintmat * bimSub(bigintmat * a, bigintmat * b);
bigintmat * bimSub(bigintmat * a, long b);
bigintmat * bimMult(bigintmat * a, bigintmat * b);
bigintmat * bimMult(bigintmat * a, long b);
bigintmat * bimMult(bigintmat * a, number b, const coeffs cf);

///same as copy constructor - apart from it being able to accept NULL as input
bigintmat * bimCopy(const bigintmat * b);

class intvec;
intvec * bim2iv(bigintmat * b);
bigintmat * iv2bim(intvec * b, const coeffs C);

// Wieder von Kira, Jan, Marco
bigintmat * bimChangeCoeff(bigintmat *a, coeffs cnew); ///< Liefert Kopier von Matrix a zurück, mit coeffs cnew statt den ursprünglichen
void bimMult(bigintmat *a, bigintmat *b, bigintmat *c); ///< Multipliziert Matrix a und b und speichert Ergebnis in c

///solve Ax=b*d. x needs to be pre-allocated to the same number of columns as b.
/// the minimal denominator d is returned. Currently available for Z, Q and Z/nZ (and possibly for all fields: d=1 there)
///Beware that the internal functions can find the kernel as well - but the interface is lacking.
number solveAx(bigintmat *A, bigintmat *b, bigintmat *x); // solves Ax=b*d for a minimal denominator d. if x needs to have as many cols as b

///a basis for the nullspace of a mod p: only used internally in Round2.
/// Don't use it.
int kernbase (bigintmat *a, bigintmat *c, number p, coeffs q);
bool nCoeffs_are_equal(coeffs r, coeffs s);
// enden wieder
void diagonalForm(bigintmat *a, bigintmat **b, bigintmat **c);

#endif /* #ifndef BIGINTMAT_H */
