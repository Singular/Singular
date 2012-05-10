/*****************************************
 *  Computer Algebra System SINGULAR      *
 *****************************************/
/*
 * ABSTRACT: class bigintmat: matrizes of big integers
 */
#include "bigintmat.h"

#include <misc/intvec.h>

//#include <kernel/mod2.h>
//#include <kernel/options.h>

#include <math.h>
#include <string.h>

#define BIMATELEM(M,I,J) (M)[ (M).index(I,J) ]


// Beginnt bei [0]
void bigintmat::set(int i, number n, const coeffs C)
{
  assume (C == NULL || C == basecoeffs());

  rawset(i, n_Copy(n, basecoeffs()), basecoeffs());
}

// Beginnt bei [1,1]
void bigintmat::set(int i, int j, number n, const coeffs C)
{
  assume (C == NULL || C == basecoeffs());
  assume (i > 0 && j > 0);
  assume (i <= rows() && j <= cols());

  set(index(i, j), n, C);
}

number bigintmat::get(int i) const
{
  assume (i >= 0);
  assume (i<rows()*cols());

  return n_Copy(v[i], basecoeffs());
}

number bigintmat::get(int i, int j) const
{
  assume (i > 0 && j > 0);
  assume (i <= rows() && j <= cols());

  return get(index(i, j));
}

// Überladener *=-Operator (für int und bigint)
// Frage hier: *= verwenden oder lieber = und * einzeln?
void bigintmat::operator*=(int intop)
{
  number iop = n_Init(intop, basecoeffs());

  inpMult(iop, basecoeffs());

  n_Delete(&iop, basecoeffs());
}

void bigintmat::inpMult(number bintop, const coeffs C)
{
  assume (C == NULL || C == basecoeffs());

  const int l = rows() * cols();

  for (int i=0; i < l; i++)
    n_InpMult(v[i], bintop, basecoeffs());
}

// Stimmen Parameter?
// Welche der beiden Methoden?
// Oder lieber eine comp-Funktion?

bool operator==(const bigintmat & lhr, const bigintmat & rhr)
{
  if (&lhr == &rhr) { return true; }
  if (lhr.cols() != rhr.cols()) { return false; }
  if (lhr.rows() != rhr.rows()) { return false; }
  if (lhr.basecoeffs() != rhr.basecoeffs()) { return false; }

  const int l = (lhr.rows())*(lhr.cols());

  for (int i=0; i < l; i++)
  {
    if (!n_Equal(lhr[i], rhr[i], lhr.basecoeffs())) { return false; }
  }

  return true;
}

bool operator!=(const bigintmat & lhr, const bigintmat & rhr)
{
  return !(lhr==rhr);
}

// Matrix-Add/-Sub/-Mult so oder mit operator+/-/* ?
bigintmat * bimAdd(bigintmat * a, bigintmat * b)
{
  if (a->cols() != b->cols()) return NULL;
  if (a->basecoeffs() != b->basecoeffs()) { return NULL; }

  const int mn = si_min(a->rows(),b->rows());
  const int ma = si_max(a->rows(),b->rows());

  const coeffs basecoeffs = a->basecoeffs();

  int i;

  if (a->cols() == 1)
  {
    bigintmat * bim = new bigintmat(ma, 1, basecoeffs);

    for (i=0; i<mn; i++)
      bim->rawset(i, n_Add((*a)[i], (*b)[i], basecoeffs), basecoeffs);

    if (ma > mn)
    {
      if (ma == a->rows())
        for(i=mn; i<ma; i++)
          bim->set(i, (*a)[i], basecoeffs);
      else
        for(i=mn; i<ma; i++)
          bim->set(i, (*b)[i], basecoeffs);
    }
    return bim;
  }

  if (mn != ma) return NULL;

  bigintmat * bim = new bigintmat(mn, a->cols(), basecoeffs);

  for (i=0; i<mn*a->cols(); i++)
    bim->rawset(i, n_Add((*a)[i], (*b)[i], basecoeffs), basecoeffs);

  return bim;
}
bigintmat * bimAdd(bigintmat * a, int b)
{

  const int mn = a->rows()*a->cols();

  const coeffs basecoeffs = a->basecoeffs();
  number bb=n_Init(b,basecoeffs);

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=0; i<mn; i++)
    bim->rawset(i, n_Add((*a)[i], bb, basecoeffs), basecoeffs);

  n_Delete(&bb,basecoeffs);
  return bim;
}

bigintmat * bimSub(bigintmat * a, bigintmat * b)
{
  if (a->cols() != b->cols()) return NULL;
  if (a->basecoeffs() != b->basecoeffs()) { return NULL; }

  const int mn = si_min(a->rows(),b->rows());
  const int ma = si_max(a->rows(),b->rows());

  const coeffs basecoeffs = a->basecoeffs();

  int i;

  if (a->cols() == 1)
  {
    bigintmat * bim = new bigintmat(ma, 1, basecoeffs);

    for (i=0; i<mn; i++)
      bim->rawset(i, n_Sub((*a)[i], (*b)[i], basecoeffs), basecoeffs);

    if (ma > mn)
    {
      if (ma == a->rows())
      {
        for(i=mn; i<ma; i++)
          bim->set(i, (*a)[i], basecoeffs);
      }
      else
        for(i=mn; i<ma; i++)
        {
          number n = b->get(i);
          n = n_Neg(n, basecoeffs);
          bim->rawset(i, n, basecoeffs);
        }
    }
    return bim;
  }

  if (mn != ma) return NULL;

  bigintmat * bim = new bigintmat(mn, a->cols(), basecoeffs);

  for (i=0; i<mn*a->cols(); i++)
    bim->rawset(i, n_Sub((*a)[i], (*b)[i], basecoeffs), basecoeffs);

  return bim;
}

bigintmat * bimSub(bigintmat * a, int b)
{

  const int mn = a->rows()*a->cols();

  const coeffs basecoeffs = a->basecoeffs();
  number bb=n_Init(b,basecoeffs);

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=0; i<mn; i++)
    bim->rawset(i, n_Sub((*a)[i], bb, basecoeffs), basecoeffs);

  n_Delete(&bb,basecoeffs);
  return bim;
}

bigintmat * bimMult(bigintmat * a, bigintmat * b)
{
  const int ca = a->cols();
  const int cb = b->cols();

  const int ra = a->rows();
  const int rb = b->rows();

  if (ca != rb)
  {
#ifndef NDEBUG
    Werror("wrong bigintmat sizes at multiplication a * b: acols: %d != brows: %d\n", ca, rb);
#endif
    return NULL;
  }

  assume (ca == rb);

  if (a->basecoeffs() != b->basecoeffs()) { return NULL; }

  const coeffs basecoeffs = a->basecoeffs();

  int i, j, k;

  number sum;

  bigintmat * bim = new bigintmat(ra, cb, basecoeffs);

  for (i=0; i<ra; i++)
    for (j=0; j<cb; j++)
    {
      sum = n_Init(0, basecoeffs);

      for (k=0; k<ca; k++)
      {
        number prod = n_Mult( BIMATELEM(*a, i, k), BIMATELEM(*b, k, j), basecoeffs);

        number sum2 = n_Add(sum, prod, basecoeffs); // no inplace add :(

        n_Delete(&sum, basecoeffs); n_Delete(&prod, basecoeffs);

        sum = sum2;
      }
      bim->rawset(i+1, j+1, sum, basecoeffs);
    }
  return bim;
}

bigintmat * bimMult(bigintmat * a, int b)
{

  const int mn = a->rows()*a->cols();

  const coeffs basecoeffs = a->basecoeffs();
  number bb=n_Init(b,basecoeffs);

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=0; i<mn; i++)
    bim->rawset(i, n_Mult((*a)[i], bb, basecoeffs), basecoeffs);

  n_Delete(&bb,basecoeffs);
  return bim;
}

bigintmat * bimMult(bigintmat * a, number b, const coeffs cf)
{
  if (cf!=a->basecoeffs()) return NULL;

  const int mn = a->rows()*a->cols();

  const coeffs basecoeffs = a->basecoeffs();

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=0; i<mn; i++)
    bim->rawset(i, n_Mult((*a)[i], b, basecoeffs), basecoeffs);

  return bim;
}

// ----------------------------------------------------------------- //
// Korrekt?

intvec * bim2iv(bigintmat * b)
{
  intvec * iv = new intvec(b->rows(), b->cols(), 0);
  for (int i=0; i<(b->rows())*(b->cols()); i++)
    (*iv)[i] = n_Int((*b)[i], b->basecoeffs()); // Geht das so?
  return iv;
}

bigintmat * iv2bim(intvec * b, const coeffs C)
{
  const int l = (b->rows())*(b->cols());
  bigintmat * bim = new bigintmat(b->rows(), b->cols(), C);

  for (int i=0; i < l; i++)
    bim->rawset(i, n_Init((*b)[i], C), C);

  return bim;
}

// ----------------------------------------------------------------- //

int bigintmat::compare(const bigintmat* op) const
{
  assume (basecoeffs() == op->basecoeffs() );

#ifndef NDEBUG
  if (basecoeffs() != op->basecoeffs() )
    WerrorS("wrong bigintmat comparison: different basecoeffs!\n");
#endif

  if ((col!=1) ||(op->cols()!=1))
  {
    if((col!=op->cols())
       || (row!=op->rows()))
      return -2;
  }

  int i;
  for (i=0; i<si_min(row*col,op->rows()*op->cols()); i++)
  {
    if ( n_Greater(v[i], (*op)[i], basecoeffs()) )
      return 1;
    else if (! n_Equal(v[i], (*op)[i], basecoeffs()))
      return -1;
  }

  for (; i<row; i++)
  {
    if ( n_GreaterZero(v[i], basecoeffs()) )
      return 1;
    else if (! n_IsZero(v[i], basecoeffs()) )
      return -1;
  }
  for (; i<op->rows(); i++)
  {
    if ( n_GreaterZero((*op)[i], basecoeffs()) )
      return -1;
    else if (! n_IsZero((*op)[i], basecoeffs()) )
      return 1;
  }
  return 0;
}


bigintmat * bimCopy(const bigintmat * b)
{
  if (b == NULL)
    return NULL;

  return new bigintmat(b);
}

char* bigintmat::String()
{
  assume (rows() > 0);
  assume (cols() > 0);

  StringSetS("");
  const int l = cols()*rows();

  n_Write(v[0], basecoeffs());
  for (int i = 1; i < l; i++)
  {
    StringAppendS(","); n_Write(v[i], basecoeffs());
  }
  /* if (i != col*row-1)
  {
  StringAppendS(",");
  if ((i+1)%col == 0)
  StringAppendS("\n");
  }   */
  return StringAppendS("");
}

int bigintmat::getwid(int maxwid)
{
  int wid=0;
  int colwid = floor((maxwid-2*(col-1))/col);
  for (int i=0; i<col*row; i++)
  {
    StringSetS("");
    n_Write(v[i], basecoeffs());
    char * tmp = StringAppendS("");
//    char * ts = omStrDup(tmp);
    const int _nl = strlen(tmp); // ts?
    if (_nl > wid)
    {
      if (_nl > colwid)
      {
        int phwid = floor(log10(row))+floor(log10(col))+5;
        if ((colwid > phwid) && (wid < phwid))
          wid = phwid;
      }
      else
        wid = _nl;
    }
  }
  return wid;
}

void bigintmat::pprint(int maxwid)
{
  if ((col==0) || (row==0))
    PrintS("");
  else
  {
    int colwid = getwid(maxwid);
    if (colwid*col+2*(col-1) > maxwid)
      colwid = floor((maxwid-2*(col-1))/col);
    char * ps;
    ps = (char*) omAlloc0(sizeof(char)*(colwid*col*row+2*(col-1)*row+row));
    int pos = 0;
    for (int i=0; i<col*row; i++)
    {
      StringSetS("");
      n_Write(v[i], basecoeffs());
      char * temp = StringAppendS("");
      char * ts = omStrDup(temp);
      const int _nl = strlen(ts);
      if (_nl > colwid)
      {
        StringSetS("");
        int cj = i%col;
        int ci = floor(i/col);
        StringAppend("[%d,%d]", ci+1, cj+1);
        char *tmp = StringAppendS("");
        char * ph = omStrDup(tmp);
        int phl = strlen(ph);
        if (phl > colwid)
        {
          for (int j=0; j<colwid; j++)
            ps[pos+j] = '*';
        }
        else
        {
          for (int j=0; j<colwid-phl; j++)
            ps[pos+j] = ' ';
          for (int j=0; j<phl; j++)
            ps[pos+colwid-phl+j] = ph[j];
        }
        omFree(ph);
      }
      else  // Mit Leerzeichen auffüllen und zahl reinschreiben
      {
        for (int j=0; j<colwid-_nl; j++)
          ps[pos+j] = ' ';
        for (int j=0; j<_nl; j++)
          ps[pos+colwid-_nl+j] = ts[j];
      }
      // ", " oder "\n" einfügen
      if ((i+1)%col == 0)
      {
        if (i != col*row-1)
        {
          ps[pos+colwid] = '\n';
          pos += colwid+1;
        }
      }
      else
      {
        ps[pos+colwid] = ',';
        ps[pos+colwid+1] = ' ';
        pos += colwid+2;
      }
      // Hier ts zerstören
    }
    PrintS(ps);
    omFree(ps);
  }
}

// Ungetestet
static void bimRowContent(bigintmat *bimat, int rowpos, int colpos)
{
  const coeffs basecoeffs = bimat->basecoeffs();

  number tgcd, m;
  int i=bimat->cols();

  loop
  {
    tgcd = n_Copy(BIMATELEM(*bimat,rowpos,i--), basecoeffs);
    if (! n_IsZero(tgcd, basecoeffs)) break;
    if (i<colpos) return;
  }
  if ((! n_GreaterZero(tgcd, basecoeffs)) && (! n_IsZero(tgcd, basecoeffs))) tgcd = n_Neg(tgcd, basecoeffs);
  if ( n_IsOne(tgcd,basecoeffs)) return;
  loop
  {
    m = n_Copy(BIMATELEM(*bimat,rowpos,i--), basecoeffs);
    if (! n_IsZero(m,basecoeffs))
    {
      number tp1 = n_Gcd(tgcd, m, basecoeffs);
      n_Delete(&tgcd, basecoeffs);
      tgcd = tp1;
    }
    if ( n_IsOne(tgcd,basecoeffs)) return;
    if (i<colpos) break;
  }
  for (i=bimat->cols();i>=colpos;i--)
  {
    number tp2 = n_Div(BIMATELEM(*bimat,rowpos,i), tgcd,basecoeffs);
    n_Delete(&BIMATELEM(*bimat,rowpos,i), basecoeffs);
    BIMATELEM(*bimat,rowpos,i) = tp2;
  }
  n_Delete(&tgcd, basecoeffs);
  n_Delete(&m, basecoeffs);
}

static void bimReduce(bigintmat *bimat, int rpiv, int colpos,
                      int ready, int all)
{
  const coeffs basecoeffs = bimat->basecoeffs();

  number tgcd, ce, m1, m2;
  int j, i;
  number piv = BIMATELEM(*bimat,rpiv,colpos);

  for (j=all;j>ready;j--)
  {
    ce = n_Copy(BIMATELEM(*bimat,j,colpos),basecoeffs);
    if (! n_IsZero(ce, basecoeffs))
    {
      n_Delete(&BIMATELEM(*bimat,j,colpos), basecoeffs);
      BIMATELEM(*bimat,j,colpos) = n_Init(0, basecoeffs);
      m1 = n_Copy(piv,basecoeffs);
      m2 = n_Copy(ce,basecoeffs);
      tgcd = n_Gcd(m1, m2, basecoeffs);
      if (! n_IsOne(tgcd,basecoeffs))
      {
        number tp1 = n_Div(m1, tgcd,basecoeffs);
        number tp2 = n_Div(m2, tgcd,basecoeffs);
        n_Delete(&m1, basecoeffs);
        n_Delete(&m2, basecoeffs);
        m1 = tp1;
        m2 = tp2;
      }
      for (i=bimat->cols();i>colpos;i--)
      {
        n_Delete(&BIMATELEM(*bimat,j,i), basecoeffs);
        number tp1 = n_Mult(BIMATELEM(*bimat,j,i), m1,basecoeffs);
        number tp2 = n_Mult(BIMATELEM(*bimat,rpiv,i), m2,basecoeffs);
        BIMATELEM(*bimat,j,i) = n_Sub(tp1, tp2,basecoeffs);
        n_Delete(&tp1, basecoeffs);
        n_Delete(&tp2, basecoeffs);
      }
      bimRowContent(bimat, j, colpos+1);
      n_Delete(&m1, basecoeffs);
      n_Delete(&m2, basecoeffs);
    }
    n_Delete(&ce, basecoeffs);
  }
}





