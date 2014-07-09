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
#include <coeffs/rmodulon.h>

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
    void inpTranspose();

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
    /// view an entry an entry. NOTE: starts at [1,1]
    //do NOT delete.
    number view(int i, int j) const;

    /// get a copy of an entry. NOTE: starts at [0]
    number get(int i) const;
    number view(int i) const;

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
    void Write();
    void Print();
/***
 * Returns a string as it would have been printed in the interpreter.
 * Used e.g. in print functions of various blackbox types.
 **/
    char * StringAsPrinted();
    void pprint(int maxwid);
    int compare(const bigintmat* op) const;
    int * getwid(int maxwid);


    // Funktionen von Kira, Jan, Marco
    // !WICHTIG: Überall, wo eine number übergeben wird, und damit gearbeitet wird, die coeffs mitübergeben und erst
    // überprüfen, ob diese mit basecoeffs übereinstimmen. Falls nein: Breche ab!
    // Genauere Beschreibung der Funktionen in der Funktionen.odt
    void swap(int i, int j); // Vertauscht i-te und j-te Spalte
    void swaprow(int i, int j); // Vertauscht i-te und j-te Zeile
    int findnonzero(int i); // liefert Index des ersten nicht-Null Eintrages in Zeile i
    int findcolnonzero(int j); // ---"--- Spalte j
    void getcol(int j, bigintmat *a); // Schreibt j-te Spalte in Vektor (Matrix) a
    void getColRange(int j, int no, bigintmat *a); // copies col j..j+no-1 into cols 1..no of a
    void getrow(int i, bigintmat *a); // Schreibt i-te Zeile in Vektor (Matrix) a
    void setcol(int j, bigintmat *m); // Setzt j-te Spalte gleich übergebenem Vektor (Matrix) m
    void setrow(int i, bigintmat *m); // Setzt i-te Zeile gleich übergebenem Vektor (Matrix) m
    void appendCol (bigintmat *a);
    void extendCols (int i);
    bool add(bigintmat *b); // Addiert zur Matrix die Matrix b dazu. Return false => an error occured
    bool sub(bigintmat *b); // Subtrahiert ...
    bool skalmult(number b, coeffs c); // Multipliziert zur Matrix den Skalar b hinzu
    bool addcol(int i, int j, number a, coeffs c); // addiert a-faches der j-ten Spalte zur i-ten dazu
    bool addrow(int i, int j, number a, coeffs c); // ... Zeile ...
    void colskalmult(int i, number a, coeffs c); // Multipliziert zur i-ten Spalte den Skalar a hinzu
    void coltransform(int i, int j, number a, number b, number c, number d); //  transforms cols (i,j) using the 2x2 matrix ((a,b)(c,d)) (hopefully)
    void rowskalmult(int i, number a, coeffs c); // ... Zeile ...
    void concatrow(bigintmat *a, bigintmat *b); // Fügt zwei Matrixen untereinander/nebeneinander in gegebene Matrix ein, bzw spaltet gegebenen Matrix auf
    void concatcol(bigintmat *a, bigintmat *b);
    void splitrow(bigintmat *a, bigintmat *b); // Speichert in Matrix a den oberen, in b den unteren Teil der Matrix, vorausgesetzt die Dimensionen stimmen überein
    void splitcol(bigintmat *a, bigintmat *b); // ... linken ... rechten ...
    void splitcol(bigintmat *a, int i); // Speichert die ersten i Spalten als Teilmatrix in a
    void splitrow(bigintmat *a, int i); // ... Zeilen ...
    bool copy(bigintmat *b); // Kopiert Einträge von b auf Bigintmat
    void copySubmatInto(bigintmat *, int sr, int sc, int nr, int nc, int tr, int tc);
    void one(); // Macht Matrix (Falls quadratisch) zu Einheitsmatrix
    int isOne(); // is matrix is identity
    void zero(); // Setzt alle Einträge auf 0
    int isZero();
    int colIsZero(int i);
    bigintmat *elim(int i, int j); // Liefert Streichungsmatrix (i-te Zeile und j-te Spalte gestrichen) zurück
    number pseudoinv(bigintmat *a); // Speichert in Matrix a die Pseudoinverse, liefert den Nenner zurück
    number trace(); // the trace ....
    number det(); // det (via LaPlace in general, hnf for euc. rings)
    number hnfdet(); // det via HNF
    // Primzahlen als long long int, müssen noch in number umgewandelt werden?
    void hnf(); // transforms INPLACE to HNF
    void howell(); //dito, but Howell form (only different for zero-divsors)
    void swapMatrix(bigintmat * a);
    bigintmat * modhnf(number p, coeffs c); // computes HNF(this | p*I)
    bigintmat * modgauss(number p, coeffs c);
    void skaldiv(number b); // Macht Ganzzahldivision aller Matrixeinträge mit b
    void colskaldiv(int j, number b); // Macht Ganzzahldivision aller j-ten Spalteneinträge mit b
    void mod(number p, coeffs c); // Reduziert komplette Matrix modulo p
    bigintmat* inpmod(number p, coeffs c); // Liefert Kopie der Matrix zurück, allerdings im Ring Z modulo p
    number content();  //the content
    void simplifyContentDen(number *den); // ensures that Gcd(den, content)=1
    // enden hier wieder
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

class intvec;
intvec * bim2iv(bigintmat * b);
bigintmat * iv2bim(intvec * b, const coeffs C);

// Wieder von Kira, Jan, Marco
bigintmat * bimChangeCoeff(bigintmat *a, coeffs cnew); // Liefert Kopier von Matrix a zurück, mit coeffs cnew statt den ursprünglichen
void bimMult(bigintmat *a, bigintmat *b, bigintmat *c); // Multipliziert Matrix a und b und speichert Ergebnis in c
number solveAx(bigintmat *A, bigintmat *b, bigintmat *x); // solves Ax=b*d for a minimal denominator d. if x needs to have as many cols as b
int kernbase (bigintmat *a, bigintmat *c, number p, coeffs q);
coeffs numbercoeffs(number n, coeffs c);
bool nCoeffs_are_equal(coeffs r, coeffs s);
// Geklaut aus anderer Datei:
static inline void number2mpz(number n, coeffs c, mpz_t m){ n_MPZ(m, n, c); }
static inline number mpz2number(mpz_t m, coeffs c){ return n_InitMPZ(m, c); }
// enden wieder
void diagonalForm(bigintmat *a, bigintmat **b, bigintmat **c);

#endif // #ifndef BIGINTMAT_H
