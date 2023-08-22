/*****************************************
 *  Computer Algebra System SINGULAR      *
 *****************************************/
/*
 * ABSTRACT: class bigintmat: matrices of numbers.
 * a few functinos might be limited to bigint or euclidean rings.
 */


#include "misc/auxiliary.h"

#include "coeffs/bigintmat.h"
#include "misc/intvec.h"

#include "coeffs/rmodulon.h"

#include <cmath>

#ifdef HAVE_RINGS
///create Z/nA of type n_Zn
static coeffs numbercoeffs(number n, coeffs c) // TODO: FIXME: replace with n_CoeffRingQuot1
{
  mpz_t p;
  number2mpz(n, c, p);
  ZnmInfo *pp = new ZnmInfo;
  pp->base = p;
  pp->exp = 1;
  coeffs nc = nInitChar(n_Zn, (void*)pp);
  mpz_clear(p);
  delete pp;
  return nc;
}
#endif

//#define BIMATELEM(M,I,J) (M)[ (M).index(I,J) ]

bigintmat * bigintmat::transpose()
{
  bigintmat * t = new bigintmat(col, row, basecoeffs());
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      t->set(j, i, BIMATELEM(*this,i,j));
    }
  }
  return t;
}

void bigintmat::inpTranspose()
{
  int n = row,
      m = col,
      nm = n<m?n : m; // the min, describing the square part of the matrix
  //CF: this is not optimal, but so far, it seems to work

#define swap(_i, _j)        \
  int __i = (_i), __j=(_j); \
  number c = v[__i];        \
  v[__i] = v[__j];          \
  v[__j] = c                \

  for (int i=0; i< nm; i++)
    for (int j=i+1; j< nm; j++)
    {
      swap(i*m+j, j*n+i);
    }
  if (n<m)
    for (int i=nm; i<m; i++)
      for(int j=0; j<n; j++)
      {
        swap(j*n+i, i*m+j);
      }
  if (n>m)
    for (int i=nm; i<n; i++)
      for(int j=0; j<m; j++)
      {
        swap(i*m+j, j*n+i);
      }
#undef swap
  row = m;
  col = n;
}


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

number bigintmat::view(int i) const
{
  assume (i >= 0);
  assume (i<rows()*cols());

  return v[i];
}

number bigintmat::get(int i, int j) const
{
  assume (i > 0 && j > 0);
  assume (i <= rows() && j <= cols());

  return get(index(i, j));
}

number bigintmat::view(int i, int j) const
{
  assume (i >= 0 && j >= 0);
  assume (i <= rows() && j <= cols());

  return view(index(i, j));
}
// Ueberladener *=-Operator (für int und bigint)
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
  if (a->rows() != b->rows()) return NULL;
  if (a->basecoeffs() != b->basecoeffs()) { return NULL; }

  const coeffs basecoeffs = a->basecoeffs();

  int i;

  bigintmat * bim = new bigintmat(a->rows(), a->cols(), basecoeffs);

  for (i=a->rows()*a->cols()-1;i>=0; i--)
    bim->rawset(i, n_Add((*a)[i], (*b)[i], basecoeffs), basecoeffs);

  return bim;
}
bigintmat * bimAdd(bigintmat * a, long b)
{

  const int mn = si_min(a->rows(),a->cols());

  const coeffs basecoeffs = a->basecoeffs();
  number bb=n_Init(b,basecoeffs);

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=1; i<=mn; i++)
    BIMATELEM(*bim,i,i)=n_Add(BIMATELEM(*a,i,i), bb, basecoeffs);

  n_Delete(&bb,basecoeffs);
  return bim;
}

bigintmat * bimSub(bigintmat * a, bigintmat * b)
{
  if (a->cols() != b->cols()) return NULL;
  if (a->rows() != b->rows()) return NULL;
  if (a->basecoeffs() != b->basecoeffs()) { return NULL; }

  const coeffs basecoeffs = a->basecoeffs();

  int i;

  bigintmat * bim = new bigintmat(a->rows(), a->cols(), basecoeffs);

  for (i=a->rows()*a->cols()-1;i>=0; i--)
    bim->rawset(i, n_Sub((*a)[i], (*b)[i], basecoeffs), basecoeffs);

  return bim;
}

bigintmat * bimSub(bigintmat * a, long b)
{
  const int mn = si_min(a->rows(),a->cols());

  const coeffs basecoeffs = a->basecoeffs();
  number bb=n_Init(b,basecoeffs);

  int i;

  bigintmat * bim = new bigintmat(a->rows(),a->cols() , basecoeffs);

  for (i=1; i<=mn; i++)
    BIMATELEM(*bim,i,i)=n_Sub(BIMATELEM(*a,i,i), bb, basecoeffs);

  n_Delete(&bb,basecoeffs);
  return bim;
}

//TODO: make special versions for certain rings!
bigintmat * bimMult(bigintmat * a, bigintmat * b)
{
  const int ca = a->cols();
  const int cb = b->cols();

  const int ra = a->rows();
  const int rb = b->rows();

  if (ca != rb)
  {
#ifndef SING_NDEBUG
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

  for (i=1; i<=ra; i++)
    for (j=1; j<=cb; j++)
    {
      sum = n_Init(0, basecoeffs);

      for (k=1; k<=ca; k++)
      {
        number prod = n_Mult( BIMATELEM(*a, i, k), BIMATELEM(*b, k, j), basecoeffs);

        n_InpAdd(sum, prod, basecoeffs);

        n_Delete(&prod, basecoeffs);
      }
      bim->rawset(i, j, sum, basecoeffs);
    }
  return bim;
}

bigintmat * bimMult(bigintmat * a, long b)
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

#ifndef SING_NDEBUG
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

void bigintmat::Write()
{
  int n = cols(), m=rows();

  for(int i=1; i<= m; i++)
  {
    for(int j=1; j< n; j++)
    {
      n_Write(v[(i-1)*n+j-1], basecoeffs());
      StringAppendS(", ");
    }
    if (n) n_Write(v[i*n-1], basecoeffs());
    if (i<m)
    {
      StringAppendS(", ");
    }
  }
}

char* bigintmat::String()
{
  StringSetS("");
  Write();
  return StringEndS();
}

void bigintmat::Print()
{
  char * s = String();
  PrintS(s);
  omFree(s);
}


char* bigintmat::StringAsPrinted()
{
  if ((col==0) || (row==0))
    return NULL;
  else
  {
    int * colwid = getwid(80);
    if (colwid == NULL)
    {
      WerrorS("not enough space to print bigintmat");
      WerrorS("try string(...) for a unformatted output");
      return NULL;
    }
    char * ps;
    int slength = 0;
    for (int j=0; j<col; j++)
      slength += colwid[j]*row;
    slength += col*row+row;
    ps = (char*) omAlloc0(sizeof(char)*(slength));
    int pos = 0;
    for (int i=0; i<col*row; i++)
    {
      StringSetS("");
      n_Write(v[i], basecoeffs());
      char * ts = StringEndS();
      const int _nl = strlen(ts);
      int cj = i%col;
      if (_nl > colwid[cj])
      {
        StringSetS("");
        int ci = i/col;
        StringAppend("[%d,%d]", ci+1, cj+1);
        char * ph = StringEndS();
        int phl = strlen(ph);
        if (phl > colwid[cj])
        {
          for (int j=0; j<colwid[cj]-1; j++)
            ps[pos+j] = ' ';
          ps[pos+colwid[cj]-1] = '*';
        }
        else
        {
          for (int j=0; j<colwid[cj]-phl; j++)
            ps[pos+j] = ' ';
          for (int j=0; j<phl; j++)
            ps[pos+colwid[cj]-phl+j] = ph[j];
        }
        omFree(ph);
    }
    else  // Mit Leerzeichen auffüllen und zahl reinschreiben
    {
      for (int j=0; j<(colwid[cj]-_nl); j++)
        ps[pos+j] = ' ';
      for (int j=0; j<_nl; j++)
        ps[pos+colwid[cj]-_nl+j] = ts[j];
    }
    // ", " und (evtl) "\n" einfügen
    if ((i+1)%col == 0)
    {
      if (i != col*row-1)
      {
        ps[pos+colwid[cj]] = ',';
        ps[pos+colwid[cj]+1] = '\n';
        pos += colwid[cj]+2;
      }
    }
    else
    {
      ps[pos+colwid[cj]] = ',';
      pos += colwid[cj]+1;
    }
    omFree(ts);  // Hier ts zerstören
  }
  return(ps);
  // omFree(ps);
}
}

static int intArrSum(int * a, int length)
{
  int sum = 0;
  for (int i=0; i<length; i++)
    sum += a[i];
  return sum;
}

static int findLongest(int * a, int length)
{
  int l = 0;
  int index;
  for (int i=0; i<length; i++)
  {
    if (a[i] > l)
    {
      l = a[i];
      index = i;
    }
  }
  return index;
}

static int getShorter (int * a, int l, int j, int cols, int rows)
{
  int sndlong = 0;
  int min;
  for (int i=0; i<rows; i++)
  {
    int index = cols*i+j;
    if ((a[index] > sndlong) && (a[index] < l))
    {
      min = floor(log10((double)cols))+floor(log10((double)rows))+5;
      if ((a[index] < min) && (min < l))
        sndlong = min;
      else
        sndlong = a[index];
    }
  }
  if (sndlong == 0)
  {
    min = floor(log10((double)cols))+floor(log10((double)rows))+5;
    if (min < l)
      sndlong = min;
    else
      sndlong = 1;
  }
  return sndlong;
}


int * bigintmat::getwid(int maxwid)
{
  int const c = /*2**/(col-1)+1;
  int * wv = (int*)omAlloc(sizeof(int)*col*row);
  int * cwv = (int*)omAlloc(sizeof(int)*col);
  for (int j=0; j<col; j++)
  {
    cwv[j] = 0;
    for (int i=0; i<row; i++)
    {
      StringSetS("");
      n_Write(v[col*i+j], basecoeffs());
      char * tmp = StringEndS();
      const int _nl = strlen(tmp);
      wv[col*i+j] = _nl;
      if (_nl > cwv[j]) cwv[j]=_nl;
      omFree(tmp);
    }
  }

  // Groesse verkleinern, bis < maxwid
  if (intArrSum(cwv, col)+c > maxwid)
  {
    int j = findLongest(cwv, col);
    cwv[j] = getShorter(wv, cwv[j], j, col, row);
  }
  omFree(wv);
  return cwv;
}

void bigintmat::pprint(int maxwid)
{
  if ((col==0) || (row==0))
    PrintS("");
  else
  {
    int * colwid = getwid(maxwid);
    char * ps;
    int slength = 0;
    for (int j=0; j<col; j++)
      slength += colwid[j]*row;
    slength += col*row+row;
    ps = (char*) omAlloc0(sizeof(char)*(slength));
    int pos = 0;
    for (int i=0; i<col*row; i++)
    {
      StringSetS("");
      n_Write(v[i], basecoeffs());
      char * ts = StringEndS();
      const int _nl = strlen(ts);
      int cj = i%col;
      if (_nl > colwid[cj])
      {
        StringSetS("");
        int ci = i/col;
        StringAppend("[%d,%d]", ci+1, cj+1);
        char * ph = StringEndS();
        int phl = strlen(ph);
        if (phl > colwid[cj])
        {
          for (int j=0; j<colwid[cj]-1; j++)
            ps[pos+j] = ' ';
          ps[pos+colwid[cj]-1] = '*';
        }
        else
        {
          for (int j=0; j<colwid[cj]-phl; j++)
            ps[pos+j] = ' ';
          for (int j=0; j<phl; j++)
            ps[pos+colwid[cj]-phl+j] = ph[j];
        }
          omFree(ph);
      }
      else  // Mit Leerzeichen auffüllen und zahl reinschreiben
      {
        for (int j=0; j<colwid[cj]-_nl; j++)
          ps[pos+j] = ' ';
        for (int j=0; j<_nl; j++)
          ps[pos+colwid[cj]-_nl+j] = ts[j];
      }
      // ", " und (evtl) "\n" einfügen
      if ((i+1)%col == 0)
      {
        if (i != col*row-1)
        {
          ps[pos+colwid[cj]] = ',';
          ps[pos+colwid[cj]+1] = '\n';
          pos += colwid[cj]+2;
        }
      }
      else
      {
        ps[pos+colwid[cj]] = ',';
        pos += colwid[cj]+1;
      }

      omFree(ts);  // Hier ts zerstören
    }
    PrintS(ps);
    omFree(ps);
  }
}


//swaps columns i and j
void bigintmat::swap(int i, int j)
{
  if ((i <= col) && (j <= col) && (i>0) && (j>0))
  {
    number tmp;
    number t;
    for (int k=1; k<=row; k++)
    {
      tmp = get(k, i);
      t = view(k, j);
      set(k, i, t);
      set(k, j, tmp);
      n_Delete(&tmp, basecoeffs());
    }
  }
  else
    WerrorS("Error in swap");
}

void bigintmat::swaprow(int i, int j)
{
  if ((i <= row) && (j <= row) && (i>0) && (j>0))
  {
    number tmp;
    number t;
    for (int k=1; k<=col; k++)
    {
      tmp = get(i, k);
      t = view(j, k);
      set(i, k, t);
      set(j, k, tmp);
      n_Delete(&tmp, basecoeffs());
    }
  }
  else
    WerrorS("Error in swaprow");
}

int bigintmat::findnonzero(int i)
{
  for (int j=1; j<=col; j++)
  {
    if (!n_IsZero(view(i,j), basecoeffs()))
    {
      return j;
    }
  }
  return 0;
}

int bigintmat::findcolnonzero(int j)
{
  for (int i=row; i>=1; i--)
  {
    if (!n_IsZero(view(i,j), basecoeffs()))
    {
      return i;
    }
  }
  return 0;
}

void bigintmat::getcol(int j, bigintmat *a)
{
  assume((j<=col) && (j>=1));
  if (((a->rows() != row) || (a->cols() != 1)) && ((a->rows() != 1) || (a->cols() != row)))
  {
    assume(0);
    WerrorS("Error in getcol. Dimensions must agree!");
    return;
  }
  if (!nCoeffs_are_equal(basecoeffs(), a->basecoeffs()))
  {
    nMapFunc f = n_SetMap(basecoeffs(), a->basecoeffs());
    number t1, t2;
    for (int i=1; i<=row;i++)
    {
      t1 = get(i,j);
      t2 = f(t1, basecoeffs(), a->basecoeffs());
      a->set(i-1,t1);
      n_Delete(&t1, basecoeffs());
      n_Delete(&t2, a->basecoeffs());
    }
    return;
  }
  number t1;
  for (int i=1; i<=row;i++)
  {
    t1 = view(i,j);
    a->set(i-1,t1);
  }
}

void bigintmat::getColRange(int j, int no, bigintmat *a)
{
  number t1;
  for(int ii=0; ii< no; ii++)
  {
    for (int i=1; i<=row;i++)
    {
      t1 = view(i, ii+j);
      a->set(i, ii+1, t1);
    }
  }
}

void bigintmat::getrow(int i, bigintmat *a)
{
  if ((i>row) || (i<1))
  {
    WerrorS("Error in getrow: Index out of range!");
    return;
  }
  if (((a->rows() != 1) || (a->cols() != col)) && ((a->rows() != col) || (a->cols() != 1)))
  {
    WerrorS("Error in getrow. Dimensions must agree!");
    return;
  }
  if (!nCoeffs_are_equal(basecoeffs(), a->basecoeffs()))
  {
    nMapFunc f = n_SetMap(basecoeffs(), a->basecoeffs());
    number t1, t2;
    for (int j=1; j<=col;j++)
    {
      t1 = get(i,j);
      t2 = f(t1, basecoeffs(), a->basecoeffs());
      a->set(j-1,t2);
      n_Delete(&t1, basecoeffs());
      n_Delete(&t2, a->basecoeffs());
    }
    return;
  }
  number t1;
  for (int j=1; j<=col;j++)
  {
    t1 = get(i,j);
    a->set(j-1,t1);
    n_Delete(&t1, basecoeffs());
  }
}

void bigintmat::setcol(int j, bigintmat *m)
{
   if ((j>col) || (j<1))
   {
    WerrorS("Error in setcol: Index out of range!");
    return;
  }
  if (((m->rows() != row) || (m->cols() != 1)) && ((m->rows() != 1) || (m->cols() != row)))
  {
    WerrorS("Error in setcol. Dimensions must agree!");
    return;
  }
  if (!nCoeffs_are_equal(basecoeffs(), m->basecoeffs()))
  {
    nMapFunc f = n_SetMap(m->basecoeffs(), basecoeffs());
    number t1,t2;
    for (int i=1; i<=row; i++)
    {
      t1 = m->get(i-1);
      t2 = f(t1, m->basecoeffs(),basecoeffs());
      set(i, j, t2);
      n_Delete(&t2, basecoeffs());
      n_Delete(&t1, m->basecoeffs());
    }
    return;
  }
  number t1;
  for (int i=1; i<=row; i++)
  {
    t1 = m->view(i-1);
    set(i, j, t1);
  }
}

void bigintmat::setrow(int j, bigintmat *m)
{
  if ((j>row) || (j<1))
  {
    WerrorS("Error in setrow: Index out of range!");
    return;
  }
  if (((m->rows() != 1) || (m->cols() != col)) && ((m->rows() != col) || (m->cols() != 1)))
  {
    WerrorS("Error in setrow. Dimensions must agree!");
    return;
  }
  if (!nCoeffs_are_equal(basecoeffs(), m->basecoeffs()))
  {
    nMapFunc f = n_SetMap(m->basecoeffs(), basecoeffs());
    number tmp1,tmp2;
    for (int i=1; i<=col; i++)
    {
      tmp1 = m->get(i-1);
      tmp2 = f(tmp1, m->basecoeffs(),basecoeffs());
      set(j, i, tmp2);
      n_Delete(&tmp2, basecoeffs());
      n_Delete(&tmp1, m->basecoeffs());
    }
    return;
  }
  number tmp;
  for (int i=1; i<=col; i++)
  {
    tmp = m->view(i-1);
    set(j, i, tmp);
  }
}

bool bigintmat::add(bigintmat *b)
{
  if ((b->rows() != row) || (b->cols() != col))
  {
    WerrorS("Error in bigintmat::add. Dimensions do not agree!");
    return false;
  }
  if (!nCoeffs_are_equal(basecoeffs(), b->basecoeffs()))
  {
    WerrorS("Error in bigintmat::add. coeffs do not agree!");
    return false;
  }
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      rawset(i, j, n_Add(b->view(i,j), view(i,j), basecoeffs()));
    }
  }
  return true;
}

bool bigintmat::sub(bigintmat *b)
{
 if ((b->rows() != row) || (b->cols() != col))
 {
   WerrorS("Error in bigintmat::sub. Dimensions do not agree!");
   return false;
  }
  if (!nCoeffs_are_equal(basecoeffs(), b->basecoeffs()))
  {
    WerrorS("Error in bigintmat::sub. coeffs do not agree!");
    return false;
  }
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      rawset(i, j, n_Sub(view(i,j), b->view(i,j), basecoeffs()));
    }
  }
  return true;
}

bool bigintmat::skalmult(number b, coeffs c)
{
  if (!nCoeffs_are_equal(c, basecoeffs()))
  {
    WerrorS("Wrong coeffs\n");
    return false;
  }
  number t1, t2;
  if ( n_IsOne(b,c)) return true;
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      t1 = view(i, j);
      t2 = n_Mult(t1, b, basecoeffs());
      rawset(i, j, t2);
    }
  }
  return true;
}

bool bigintmat::addcol(int i, int j, number a, coeffs c)
{
  if ((i>col) || (j>col) || (i<1) || (j<1))
  {
    WerrorS("Error in addcol: Index out of range!");
    return false;
  }
  if (!nCoeffs_are_equal(c, basecoeffs()))
  {
    WerrorS("Error in addcol: coeffs do not agree!");
    return false;
  }
  number t1, t2, t3;
  for (int k=1; k<=row; k++)
  {
    t1 = view(k, j);
    t2 = view(k, i);
    t3 = n_Mult(t1, a, basecoeffs());
    n_InpAdd(t3, t2, basecoeffs());
    rawset(k, i, t3);
  }
  return true;
}

bool bigintmat::addrow(int i, int j, number a, coeffs c)
{
  if ((i>row) || (j>row) || (i<1) || (j<1))
  {
    WerrorS("Error in addrow: Index out of range!");
    return false;
  }
  if (!nCoeffs_are_equal(c, basecoeffs()))
  {
    WerrorS("Error in addrow: coeffs do not agree!");
    return false;
  }
  number t1, t2, t3;
  for (int k=1; k<=col; k++)
  {
    t1 = view(j, k);
    t2 = view(i, k);
    t3 = n_Mult(t1, a, basecoeffs());
    n_InpAdd(t3, t2, basecoeffs());
    rawset(i, k, t3);
  }
  return true;
}

void bigintmat::colskalmult(int i, number a, coeffs c)
{
  if ((i>=1) && (i<=col) && (nCoeffs_are_equal(c, basecoeffs())))
  {
    number t, tmult;
    for (int j=1; j<=row; j++)
    {
      t = view(j, i);
      tmult = n_Mult(a, t, basecoeffs());
      rawset(j, i, tmult);
    }
  }
  else
    WerrorS("Error in colskalmult");
}

void bigintmat::rowskalmult(int i, number a, coeffs c)
{
  if ((i>=1) && (i<=row) && (nCoeffs_are_equal(c, basecoeffs())))
  {
    number t, tmult;
    for (int j=1; j<=col; j++)
    {
      t = view(i, j);
      tmult = n_Mult(a, t, basecoeffs());
      rawset(i, j, tmult);
    }
  }
  else
    WerrorS("Error in rowskalmult");
}

void bigintmat::concatrow(bigintmat *a, bigintmat *b)
{
  int ay = a->cols();
  int ax = a->rows();
  int by = b->cols();
  int bx = b->rows();
  number tmp;
  if (!((col == ay) && (col == by) && (ax+bx == row)))
  {
    WerrorS("Error in concatrow. Dimensions must agree!");
    return;
  }
  if (!(nCoeffs_are_equal(a->basecoeffs(), basecoeffs()) && nCoeffs_are_equal(b->basecoeffs(), basecoeffs())))
  {
    WerrorS("Error in concatrow. coeffs do not agree!");
    return;
  }
  for (int i=1; i<=ax; i++)
  {
    for (int j=1; j<=ay; j++)
    {
      tmp = a->get(i,j);
      set(i, j, tmp);
      n_Delete(&tmp, basecoeffs());
    }
  }
  for (int i=1; i<=bx; i++)
  {
    for (int j=1; j<=by; j++)
    {
      tmp = b->get(i,j);
      set(i+ax, j, tmp);
      n_Delete(&tmp, basecoeffs());
    }
  }
}

void bigintmat::extendCols(int i)
{
  bigintmat * tmp = new bigintmat(rows(), i, basecoeffs());
  appendCol(tmp);
  delete tmp;
}

void bigintmat::appendCol (bigintmat *a)
{
  coeffs R = basecoeffs();
  int ay = a->cols();
  int ax = a->rows();
  assume(row == ax);

  assume(nCoeffs_are_equal(a->basecoeffs(), R));

  bigintmat * tmp = new bigintmat(rows(), cols() + ay, R);
  tmp->concatcol(this, a);
  this->swapMatrix(tmp);
  delete tmp;
}

void bigintmat::concatcol (bigintmat *a, bigintmat *b) {
  int ay = a->cols();
  int ax = a->rows();
  int by = b->cols();
  int bx = b->rows();
  number tmp;

  assume(row==ax && row == bx && ay+by ==col);

  assume(nCoeffs_are_equal(a->basecoeffs(), basecoeffs()) && nCoeffs_are_equal(b->basecoeffs(), basecoeffs()));

  for (int i=1; i<=ax; i++)
  {
    for (int j=1; j<=ay; j++)
    {
      tmp = a->view(i,j);
      set(i, j, tmp);
    }
  }
  for (int i=1; i<=bx; i++)
  {
    for (int j=1; j<=by; j++)
    {
      tmp = b->view(i,j);
      set(i, j+ay, tmp);
    }
  }
}

void bigintmat::splitrow(bigintmat *a, bigintmat *b)
{
  int ay = a->cols();
  int ax = a->rows();
  int by = b->cols();
  int bx = b->rows();
  number tmp;
  if (!(ax + bx == row))
  {
    WerrorS("Error in splitrow. Dimensions must agree!");
  }
  else if (!((col == ay) && (col == by)))
  {
    WerrorS("Error in splitrow. Dimensions must agree!");
  }
  else if (!(nCoeffs_are_equal(a->basecoeffs(), basecoeffs()) && nCoeffs_are_equal(b->basecoeffs(), basecoeffs())))
  {
    WerrorS("Error in splitrow. coeffs do not agree!");
  }
  else
  {
    for(int i = 1; i<=ax; i++)
    {
      for(int j = 1; j<=ay;j++)
      {
        tmp = get(i,j);
        a->set(i,j,tmp);
        n_Delete(&tmp, basecoeffs());
      }
    }
    for (int i =1; i<=bx; i++)
    {
      for (int j=1;j<=col;j++)
      {
        tmp = get(i+ax, j);
        b->set(i,j,tmp);
        n_Delete(&tmp, basecoeffs());
      }
    }
  }
}

void bigintmat::splitcol(bigintmat *a, bigintmat *b)
{
  int ay = a->cols();
  int ax = a->rows();
  int by = b->cols();
  int bx = b->rows();
  number tmp;
  if (!((row == ax) && (row == bx)))
  {
    WerrorS("Error in splitcol. Dimensions must agree!");
  }
  else if (!(ay+by == col))
  {
    WerrorS("Error in splitcol. Dimensions must agree!");
  }
  else if (!(nCoeffs_are_equal(a->basecoeffs(), basecoeffs()) && nCoeffs_are_equal(b->basecoeffs(), basecoeffs())))
  {
    WerrorS("Error in splitcol. coeffs do not agree!");
  }
  else
  {
    for (int i=1; i<=ax; i++)
    {
      for (int j=1; j<=ay; j++)
      {
        tmp = view(i,j);
        a->set(i,j,tmp);
      }
    }
    for (int i=1; i<=bx; i++)
    {
      for (int j=1; j<=by; j++)
      {
        tmp = view(i,j+ay);
        b->set(i,j,tmp);
      }
    }
  }
}

void bigintmat::splitcol(bigintmat *a, int i)
{
  number tmp;
  if ((a->rows() != row) || (a->cols()+i-1 > col) || (i<1))
  {
    WerrorS("Error in splitcol. Dimensions must agree!");
    return;
  }
  if (!(nCoeffs_are_equal(a->basecoeffs(), basecoeffs())))
  {
    WerrorS("Error in splitcol. coeffs do not agree!");
    return;
  }
  int width = a->cols();
  for (int j=1; j<=width; j++)
  {
    for (int k=1; k<=row; k++)
    {
      tmp = get(k, j+i-1);
      a->set(k, j, tmp);
      n_Delete(&tmp, basecoeffs());
    }
  }
}

void bigintmat::splitrow(bigintmat *a, int i)
{
  number tmp;
  if ((a->cols() != col) || (a->rows()+i-1 > row) || (i<1))
  {
    WerrorS("Error in Marco-splitrow");
    return;
  }

  if (!(nCoeffs_are_equal(a->basecoeffs(), basecoeffs())))
  {
    WerrorS("Error in splitrow. coeffs do not agree!");
    return;
  }
  int height = a->rows();
  for (int j=1; j<=height; j++)
  {
    for (int k=1; k<=col; k++)
    {
      tmp = view(j+i-1, k);
      a->set(j, k, tmp);
    }
  }
}

bool bigintmat::copy(bigintmat *b)
{
  if ((b->rows() != row) || (b->cols() != col))
  {
    WerrorS("Error in bigintmat::copy. Dimensions do not agree!");
    return false;
  }
  if (!nCoeffs_are_equal(basecoeffs(), b->basecoeffs()))
  {
    WerrorS("Error in bigintmat::copy. coeffs do not agree!");
    return false;
  }
  number t1;
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      t1 = b->view(i, j);
      set(i, j, t1);
    }
  }
  return true;
}

/// copy the submatrix of b, staring at (a,b) having n rows, m cols into
/// the given matrix at pos. (c,d)
/// needs c+n, d+m <= rows, cols
///       a+n, b+m <= b.rows(), b.cols()
void bigintmat::copySubmatInto(bigintmat *B, int a, int b, int n, int m, int c, int d)
{
  number t1;
  for (int i=1; i<=n; i++)
  {
    for (int j=1; j<=m; j++)
    {
      t1 = B->view(a+i-1, b+j-1);
      set(c+i-1, d+j-1, t1);
    }
  }
}

int bigintmat::isOne()
{
  coeffs r = basecoeffs();
  if (row==col)
  {
    for (int i=1; i<=row; i++)
    {
      for (int j=1; j<=col; j++)
      {
        if (i==j)
        {
          if (!n_IsOne(view(i, j), r))
            return 0;
        }
        else
        {
          if (!n_IsZero(view(i,j), r))
            return 0;
        }
      }
    }
  }
  return 1;
}

void bigintmat::one()
{
  if (row==col)
  {
    number one = n_Init(1, basecoeffs()),
           zero = n_Init(0, basecoeffs());
    for (int i=1; i<=row; i++)
    {
      for (int j=1; j<=col; j++)
      {
        if (i==j)
        {
          set(i, j, one);
        }
        else
        {
          set(i, j, zero);
        }
      }
    }
    n_Delete(&one, basecoeffs());
    n_Delete(&zero, basecoeffs());
  }
}

void bigintmat::zero()
{
  number tmp = n_Init(0, basecoeffs());
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      set(i, j, tmp);
    }
  }
  n_Delete(&tmp,basecoeffs());
}

int bigintmat::isZero()
{
  for (int i=1; i<=row; i++) {
    for (int j=1; j<=col; j++) {
      if (!n_IsZero(view(i,j), basecoeffs()))
        return FALSE;
    }
  }
  return TRUE;
}
//****************************************************************************
//
//****************************************************************************

//used in the det function. No idea what it does.
//looks like it return the submatrix where the i-th row
//and j-th column has been removed in the LaPlace generic
//determinant algorithm
bigintmat *bigintmat::elim(int i, int j)
{
  if ((i<=0) || (i>row) || (j<=0) || (j>col))
    return NULL;
  int cx, cy;
  cx=1;
  cy=1;
  number t;
  bigintmat *b = new bigintmat(row-1, col-1, basecoeffs());
  for (int k=1; k<=row; k++) {
    if (k!=i)
    {
      cy=1;
      for (int l=1; l<=col; l++)
      {
        if (l!=j)
        {
          t = get(k, l);
          b->set(cx, cy, t);
          n_Delete(&t, basecoeffs());
          cy++;
        }
      }
      cx++;
    }
  }
  return b;
}


//returns d such that a/d is the inverse of the input
//TODO: make work for p not prime using the euc stuff.
//long term: rewrite for Z using p-adic lifting
//and Dixon. Possibly even the sparse recent Storjohann stuff
number bigintmat::pseudoinv(bigintmat *a) {

  // Falls Matrix über reellen Zahlen nicht invertierbar, breche ab
 assume((a->rows() == row) && (a->rows() == a->cols()) && (row == col));

 number det = this->det(); //computes the HNF, so should e reused.
 if ((n_IsZero(det, basecoeffs())))
    return det;

 // Hänge Einheitsmatrix über Matrix und wendet HNF an. An Stelle der Einheitsmatrix steht im Ergebnis die Transformationsmatrix dazu
  a->one();
  bigintmat *m = new bigintmat(2*row, col, basecoeffs());
  m->concatrow(a,this);
  m->hnf();
  // Arbeite weiterhin mit der zusammengehängten Matrix
  // Laufe durch die Diagonalelemente, und multipliziere jede Spalte rechts davon damit, speichere aber den alten Eintrag der Spalte, temp, der in der Zeile des Diagonalelements liegt, zwischen. Dann addiere das -temp-Fache der Diagonalspalte zur entsprechenenden Spalte rechts davon. Dadurch entsteht überall rechts der Diagonalen eine 0
  number diag;
  number temp, ttemp;
  for (int i=1; i<=col; i++) {
    diag = m->get(row+i, i);
    for (int j=i+1; j<=col; j++) {
      temp = m->get(row+i, j);
      m->colskalmult(j, diag, basecoeffs());
      temp = n_InpNeg(temp, basecoeffs());
      m->addcol(j, i, temp, basecoeffs());
      n_Delete(&temp, basecoeffs());
    }
    n_Delete(&diag, basecoeffs());
  }
  // Falls wir nicht modulo n arbeiten, können wir die Spalten durch den ggT teilen, um die Einträge kleiner zu bekommen
  // Bei Z/n sparen wir uns das, da es hier sinnlos ist
  number g;
  number gcd;
  for (int j=1; j<=col; j++) {
    g = n_Init(0, basecoeffs());
    for (int i=1; i<=2*row; i++) {
      temp = m->get(i,j);
      gcd = n_Gcd(g, temp, basecoeffs());
      n_Delete(&g, basecoeffs());
      n_Delete(&temp, basecoeffs());
      g = n_Copy(gcd, basecoeffs());
      n_Delete(&gcd, basecoeffs());
    }
    if (!(n_IsOne(g, basecoeffs())))
      m->colskaldiv(j, g);
    n_Delete(&g, basecoeffs());
  }

  // Nun müssen die Diagonalelemente durch Spaltenmultiplikation gleich gesett werden. Bei Z können wir mit dem kgV arbeiten, bei Z/n bringen wir jedes Diagonalelement auf 1 (wir arbeiten immer mit n = Primzahl. Für n != Primzahl muss noch an anderen Stellen etwas geändert werden)

  g = n_Init(0, basecoeffs());
  number prod = n_Init(1, basecoeffs());
  for (int i=1; i<=col; i++) {
    gcd = n_Gcd(g, m->get(row+i, i), basecoeffs());
    n_Delete(&g, basecoeffs());
    g = n_Copy(gcd, basecoeffs());
    n_Delete(&gcd, basecoeffs());
    ttemp = n_Copy(prod, basecoeffs());
    temp = m->get(row+i, i);
    n_Delete(&prod, basecoeffs());
    prod = n_Mult(ttemp, temp, basecoeffs());
    n_Delete(&ttemp, basecoeffs());
    n_Delete(&temp, basecoeffs());
  }
  number lcm;
  lcm = n_Div(prod, g, basecoeffs());
  for (int j=1; j<=col; j++) {
    ttemp = m->get(row+j,j);
    temp = n_QuotRem(lcm, ttemp, NULL, basecoeffs());
    m->colskalmult(j, temp, basecoeffs());
    n_Delete(&ttemp, basecoeffs());
    n_Delete(&temp, basecoeffs());
  }
  n_Delete(&lcm, basecoeffs());
  n_Delete(&prod, basecoeffs());

  number divisor = m->get(row+1, 1);
  m->splitrow(a, 1);
  delete m;
  n_Delete(&det, basecoeffs());
  return divisor;
}

number bigintmat::trace()
{
  assume (col == row);
  number t = get(1,1),
         h;
  coeffs r = basecoeffs();
  for(int i=2; i<= col; i++) {
    h = n_Add(t, view(i,i), r);
    n_Delete(&t, r);
    t = h;
  }
  return t;
}

number bigintmat::det()
{
  assume (row==col);

  if (col == 1)
    return get(1, 1);
  // should work as well in Z/pZ of type n_Zp?
  // relies on XExtGcd and the other euc. functinos.
  if ( getCoeffType(basecoeffs())== n_Z || getCoeffType(basecoeffs() )== n_Zn) {
    return hnfdet();
  }
  number sum = n_Init(0, basecoeffs());
  number t1, t2, t3, t4;
  bigintmat *b;
  for (int i=1; i<=row; i++) {
    b = elim(i, 1);
    t1 = get(i, 1);
    t2 = b->det();
    t3 = n_Mult(t1, t2, basecoeffs());
    t4 = n_Copy(sum, basecoeffs());
    n_Delete(&sum, basecoeffs());
    if ((i+1)>>1<<1==(i+1))
      sum = n_Add(t4, t3, basecoeffs());
    else
      sum = n_Sub(t4, t3, basecoeffs());
    n_Delete(&t1, basecoeffs());
    n_Delete(&t2, basecoeffs());
    n_Delete(&t3, basecoeffs());
    n_Delete(&t4, basecoeffs());
  }
  return sum;
}

number bigintmat::hnfdet()
{
  assume (col == row);

  if (col == 1)
    return get(1, 1);
  bigintmat *m = new bigintmat(this);
  m->hnf();
  number prod = n_Init(1, basecoeffs());
  number temp, temp2;
  for (int i=1; i<=col; i++) {
    temp = m->get(i, i);
    temp2 = n_Mult(temp, prod, basecoeffs());
    n_Delete(&prod, basecoeffs());
    prod = temp2;
    n_Delete(&temp, basecoeffs());
  }
  delete m;
  return prod;
}

void bigintmat::swapMatrix(bigintmat *a)
{
  int n = rows(), m = cols();
  row = a->rows();
  col = a->cols();
  number * V = v;
  v = a->v;
  a->v = V;
  a->row = n;
  a->col = m;
}
int bigintmat::colIsZero(int j)
{
  coeffs R = basecoeffs();
  for(int i=1; i<=rows(); i++)
    if (!n_IsZero(view(i, j), R)) return FALSE;
  return TRUE;
}

void bigintmat::howell()
{
  coeffs R = basecoeffs();
  hnf(); // as a starting point...
  if (getCoeffType(R)== n_Z) return; //wrong, need to prune!

  int n = cols(), m = rows(), i, j, k;

  //make sure, the matrix has enough space. We need no rows+1 columns.
  //The resulting Howell form will be pruned to be at most square.
  bigintmat * t = new bigintmat(m, m+1, R);
  t->copySubmatInto(this, 1, n>m ? n-m+1 : 1, m, n>m ? m : n, 1, n>m ? 2 : m+2-n  );
  swapMatrix(t);
  delete t;
  for(i=1; i<= cols(); i++) {
    if (!colIsZero(i)) break;
  }
  assume (i>1);
  if (i>cols()) {
    t = new bigintmat(rows(), 0, R);
    swapMatrix(t);
    delete t;
    return; // zero matrix found, clearly normal.
  }

  int last_zero_col = i-1;
  for (int c = cols(); c>0; c--) {
    for(i=rows(); i>0; i--) {
      if (!n_IsZero(view(i, c), R)) break;
    }
    if (i==0) break; // matrix SHOULD be zero from here on
    number a = n_Ann(view(i, c), R);
    addcol(last_zero_col, c, a, R);
    n_Delete(&a, R);
    for(j = c-1; j>last_zero_col; j--) {
      for(k=rows(); k>0; k--) {
        if (!n_IsZero(view(k, j), R)) break;
        if (!n_IsZero(view(k, last_zero_col), R)) break;
      }
      if (k==0) break;
      if (!n_IsZero(view(k, last_zero_col), R)) {
        number gcd, co1, co2, co3, co4;
        gcd = n_XExtGcd(view(k, last_zero_col), view(k, j), &co1, &co2, &co3, &co4, R);
        if (n_Equal(gcd, view(k, j), R)) {
          number q = n_Div(view(k, last_zero_col), gcd, R);
          q = n_InpNeg(q, R);
          addcol(last_zero_col, j, q, R);
          n_Delete(&q, R);
        } else if (n_Equal(gcd, view(k, last_zero_col), R)) {
          swap(last_zero_col, k);
          number q = n_Div(view(k, last_zero_col), gcd, R);
          q = n_InpNeg(q, R);
          addcol(last_zero_col, j, q, R);
          n_Delete(&q, R);
        } else {
          coltransform(last_zero_col, j, co3, co4, co1, co2);
        }
        n_Delete(&gcd, R);
        n_Delete(&co1, R);
        n_Delete(&co2, R);
        n_Delete(&co3, R);
        n_Delete(&co4, R);
      }
    }
    for(k=rows(); k>0; k--) {
      if (!n_IsZero(view(k, last_zero_col), R)) break;
    }
    if (k) last_zero_col--;
  }
  t = new bigintmat(rows(), cols()-last_zero_col, R);
  t->copySubmatInto(this, 1, last_zero_col+1, rows(), cols()-last_zero_col, 1, 1);
  swapMatrix(t);
  delete t;
}

void bigintmat::hnf()
{
  // Laufen von unten nach oben und von links nach rechts
  // CF: TODO: for n_Z: write a recursive version. This one will
  //     have exponential blow-up. Look at Michianchio
  //     Alternatively, do p-adic det and modular method

#if 0
    char * s;
    ::PrintS("mat over Z is \n");
    ::Print("%s\n", s = nCoeffString(basecoeffs()));
    omFree(s);
    Print();
    ::Print("\n(%d x %d)\n", rows(), cols());
#endif

  int i = rows();
  int j = cols();
  number q = n_Init(0, basecoeffs());
  number one = n_Init(1, basecoeffs());
  number minusone = n_Init(-1, basecoeffs());
  number tmp1 = n_Init(0, basecoeffs());
  number tmp2 = n_Init(0, basecoeffs());
  number co1, co2, co3, co4;
  number ggt = n_Init(0, basecoeffs());

  while ((i>0) && (j>0))
  {
    // Falls erstes Nicht-Null-Element in Zeile i nicht existiert, oder hinter Spalte j vorkommt, gehe in nächste Zeile
    if ((findnonzero(i)==0) || (findnonzero(i)>j))
    {
      i--;
    }
    else
    {
      // Laufe von links nach rechts durch die Zeile:
      for (int l=1; l<=j-1; l++)
      {
        n_Delete(&tmp1, basecoeffs());
        tmp1 = get(i, l);
        // Falls Eintrag (im folgenden x genannt) gleich 0, gehe eine Spalte weiter. Ansonsten...
        if (!n_IsZero(tmp1, basecoeffs()))
        {
          n_Delete(&tmp2, basecoeffs());
          tmp2 = get(i, l+1);
          // Falls Eintrag (i.f. y g.) rechts daneben gleich 0, tausche beide Spalten, sonst...
          if (!n_IsZero(tmp2, basecoeffs()))
          {
            n_Delete(&ggt, basecoeffs());
            ggt = n_XExtGcd(tmp1, tmp2, &co1, &co2, &co3, &co4, basecoeffs());
            // Falls x=ggT(x, y), tausche die beiden Spalten und ziehe die (neue) rechte Spalte so häufig von der linken ab, dass an der ehemaligen Stelle von x nun eine 0 steht. Dazu:
            if (n_Equal(tmp1, ggt, basecoeffs()))
            {
              swap(l, l+1);
              n_Delete(&q, basecoeffs());
              q = n_Div(tmp2, ggt, basecoeffs());
              q = n_InpNeg(q, basecoeffs());
              // Dann addiere das -q-fache der (neuen) rechten Spalte zur linken dazu. Damit erhalten wir die gewünschte 0

              addcol(l, l+1, q, basecoeffs());
              n_Delete(&q, basecoeffs());
            }
            else if (n_Equal(tmp1, minusone, basecoeffs()))
            {
              // Falls x=-1, so ist x=-ggt(x, y). Dann gehe wie oben vor, multipliziere aber zuerst die neue rechte Spalte (die mit x) mit -1
              // Die Berechnung von q (=y/ggt) entfällt, da ggt=1
              swap(l, l+1);
              colskalmult(l+1, minusone, basecoeffs());
              tmp2 = n_InpNeg(tmp2, basecoeffs());
              addcol(l, l+1, tmp2, basecoeffs());
            }
            else
            {
              // CF: use the 2x2 matrix (co1, co2)(co3, co4) to
              // get the gcd in position and the 0 in the other:
#ifdef CF_DEB
              ::PrintS("applying trafo\n");
              StringSetS("");
              n_Write(co1, basecoeffs()); StringAppendS("\t");
              n_Write(co2, basecoeffs()); StringAppendS("\t");
              n_Write(co3, basecoeffs()); StringAppendS("\t");
              n_Write(co4, basecoeffs()); StringAppendS("\t");
              ::Print("%s\nfor l=%d\n", StringEndS(), l);
              {char * s = String();
              ::Print("to %s\n", s);omFree(s);};
#endif
              coltransform(l, l+1, co3, co4, co1, co2);
#ifdef CF_DEB
              {char * s = String();
              ::Print("gives %s\n", s);}
#endif
            }
            n_Delete(&co1, basecoeffs());
            n_Delete(&co2, basecoeffs());
            n_Delete(&co3, basecoeffs());
            n_Delete(&co4, basecoeffs());
          }
          else
          {
            swap(l, l+1);
          }
          // Dann betrachte die vormals rechte Spalte als neue linke, und die rechts daneben als neue rechte.
        }
      }

      #ifdef HAVE_RINGS
      // normalize by units:
      if (!n_IsZero(view(i, j), basecoeffs()))
      {
        number u = n_GetUnit(view(i, j), basecoeffs());
        if (!n_IsOne(u, basecoeffs()))
        {
          colskaldiv(j, u);
        }
        n_Delete(&u, basecoeffs());
      }
      #endif
      // Zum Schluss mache alle Einträge rechts vom Diagonalelement betragsmäßig kleiner als dieses
      for (int l=j+1; l<=col; l++)
      {
        n_Delete(&q, basecoeffs());
        q = n_QuotRem(view(i, l), view(i, j), NULL, basecoeffs());
        q = n_InpNeg(q, basecoeffs());
        addcol(l, j, q, basecoeffs());
      }
      i--;
      j--;
      // Dann betrachte die Zeile darüber und gehe dort wie vorher vor
    }
  }
  n_Delete(&q, basecoeffs());
  n_Delete(&tmp1, basecoeffs());
  n_Delete(&tmp2, basecoeffs());
  n_Delete(&ggt, basecoeffs());
  n_Delete(&one, basecoeffs());
  n_Delete(&minusone, basecoeffs());

#if 0
    ::PrintS("hnf over Z is \n");
    Print();
    ::Print("\n(%d x %d)\n", rows(), cols());
#endif
}

bigintmat * bimChangeCoeff(bigintmat *a, coeffs cnew)
{
  coeffs cold = a->basecoeffs();
  bigintmat *b = new bigintmat(a->rows(), a->cols(), cnew);
  // Erzeugt Karte von alten coeffs nach neuen
  nMapFunc f = n_SetMap(cold, cnew);
  number t1;
  number t2;
  // apply map to all entries.
  for (int i=1; i<=a->rows(); i++)
  {
    for (int j=1; j<=a->cols(); j++)
    {
      t1 = a->get(i, j);
      t2 = f(t1, cold, cnew);
      b->set(i, j, t2);
      n_Delete(&t1, cold);
      n_Delete(&t2, cnew);
    }
  }
  return b;
}

#ifdef HAVE_RINGS
//OK: a HNF of (this | p*I)
//so the result will always have FULL rank!!!!
//(This is different form a lift of the HNF mod p: consider the matrix (p)
//to see the difference. It CAN be computed as HNF mod p^2 usually..)
bigintmat * bigintmat::modhnf(number p, coeffs R)
{
  coeffs Rp = numbercoeffs(p, R); // R/pR
  bigintmat *m = bimChangeCoeff(this, Rp);
  m->howell();
  bigintmat *a = bimChangeCoeff(m, R);
  delete m;
  bigintmat *C = new bigintmat(rows(), rows(), R);
  int piv = rows(), i = a->cols();
  while (piv)
  {
    if (!i || n_IsZero(a->view(piv, i), R))
    {
      C->set(piv, piv, p, R);
    }
    else
    {
      C->copySubmatInto(a, 1, i, rows(), 1, 1, piv);
      i--;
    }
    piv--;
  }
  delete a;
  return C;
}
#endif


//exactly divide matrix by b
void bigintmat::skaldiv(number b)
{
  number tmp1, tmp2;
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      tmp1 = view(i, j);
      tmp2 = n_Div(tmp1, b, basecoeffs());
      rawset(i, j, tmp2);
    }
  }
}

//exactly divide col j by b
void bigintmat::colskaldiv(int j, number b)
{
  number tmp1, tmp2;
  for (int i=1; i<=row; i++)
  {
    tmp1 = view(i, j);
    tmp2 = n_Div(tmp1, b, basecoeffs());
    rawset(i, j, tmp2);
  }
}

// col(j, k) <- col(j,k)*matrix((a, c)(b, d))
// mostly used internally in the hnf and Howell stuff
void bigintmat::coltransform(int j, int k, number a, number b, number c, number d)
{
  number tmp1, tmp2, tmp3, tmp4;
  for (int i=1; i<=row; i++)
  {
    tmp1 = get(i, j);
    tmp2 = get(i, k);
    tmp3 = n_Mult(tmp1, a, basecoeffs());
    tmp4 = n_Mult(tmp2, b, basecoeffs());
    n_InpAdd(tmp3, tmp4, basecoeffs());
    n_Delete(&tmp4, basecoeffs());

    n_InpMult(tmp1, c, basecoeffs());
    n_InpMult(tmp2, d, basecoeffs());
    n_InpAdd(tmp1, tmp2, basecoeffs());
    n_Delete(&tmp2, basecoeffs());

    set(i, j, tmp3);
    set(i, k, tmp1);
    n_Delete(&tmp1, basecoeffs());
    n_Delete(&tmp3, basecoeffs());
  }
}



//reduce all entries mod p. Does NOT change the coeffs type
void bigintmat::mod(number p)
{
  // produce the matrix in Z/pZ
  number tmp1, tmp2;
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      tmp1 = get(i, j);
      tmp2 = n_IntMod(tmp1, p, basecoeffs());
      n_Delete(&tmp1, basecoeffs());
      set(i, j, tmp2);
    }
  }
}

void bimMult(bigintmat *a, bigintmat *b, bigintmat *c)
{
  if (!nCoeffs_are_equal(a->basecoeffs(), b->basecoeffs()))
  {
    WerrorS("Error in bimMult. Coeffs do not agree!");
    return;
  }
  if ((a->rows() != c->rows()) || (b->cols() != c->cols()) || (a->cols() != b->rows()))
  {
    WerrorS("Error in bimMult. Dimensions do not agree!");
    return;
  }
  bigintmat *tmp = bimMult(a, b);
  c->copy(tmp);

  delete tmp;
}

static void reduce_mod_howell(bigintmat *A, bigintmat *b, bigintmat * eps, bigintmat *x)
{
  //write b = Ax + eps where eps is "small" in the sense of bounded by the
  //pivot entries in H. H does not need to be Howell (or HNF) but need
  //to be triagonal in the same direction.
  //b can have multiple columns.
#if 0
  PrintS("reduce_mod_howell: A:\n");
  A->Print();
  PrintS("\nb:\n");
  b->Print();
#endif

  coeffs R = A->basecoeffs();
  assume(x->basecoeffs() == R);
  assume(b->basecoeffs() == R);
  assume(eps->basecoeffs() == R);
  if (!A->cols())
  {
    x->zero();
    eps->copy(b);

#if 0
    PrintS("\nx:\n");
    x->Print();
    PrintS("\neps:\n");
    eps->Print();
    PrintS("\n****************************************\n");
#endif
    return;
  }

  bigintmat * B = new bigintmat(b->rows(), 1, R);
  for(int i=1; i<= b->cols(); i++)
  {
    int A_col = A->cols();
    b->getcol(i, B);
    for(int j = B->rows(); j>0; j--)
    {
      number Ai = A->view(A->rows() - B->rows() + j, A_col);
      if (n_IsZero(Ai, R) &&
          n_IsZero(B->view(j, 1), R))
      {
        continue; //all is fine: 0*x = 0
      }
      else if (n_IsZero(B->view(j, 1), R))
      {
        x->rawset(x->rows() - B->rows() + j, i, n_Init(0, R));
        A_col--;
      }
      else if (n_IsZero(Ai, R))
      {
        A_col--;
      }
      else
      {
        // "solve" ax=b, possibly enlarging d
        number Bj = B->view(j, 1);
        number q = n_Div(Bj, Ai, R);
        x->rawset(x->rows() - B->rows() + j, i, q);
        for(int k=j; k>B->rows() - A->rows(); k--)
        {
          //B[k] = B[k] - x[k]A[k][j]
          number s = n_Mult(q, A->view(A->rows() - B->rows() + k, A_col), R);
          B->rawset(k, 1, n_Sub(B->view(k, 1), s, R));
          n_Delete(&s, R);
        }
        A_col--;
      }
      if (!A_col)
      {
        break;
      }
    }
    eps->setcol(i, B);
  }
  delete B;
#if 0
  PrintS("\nx:\n");
  x->Print();
  PrintS("\neps:\n");
  eps->Print();
  PrintS("\n****************************************\n");
#endif
}

static bigintmat * prependIdentity(bigintmat *A)
{
  coeffs R = A->basecoeffs();
  bigintmat *m = new bigintmat(A->rows()+A->cols(), A->cols(), R);
  m->copySubmatInto(A, 1, 1, A->rows(), A->cols(), A->cols()+1, 1);
  number one = n_Init(1, R);
  for(int i=1; i<= A->cols(); i++)
    m->set(i,i,one);
  n_Delete(&one, R);
  return m;
}

static number bimFarey(bigintmat *A, number N, bigintmat *L)
{
  coeffs Z = A->basecoeffs(),
         Q = nInitChar(n_Q, 0);
  number den = n_Init(1, Z);
  nMapFunc f = n_SetMap(Q, Z);

  for(int i=1; i<= A->rows(); i++)
  {
    for(int j=1; j<= A->cols(); j++)
    {
      number ad = n_Mult(den, A->view(i, j), Z);
      number re = n_IntMod(ad, N, Z);
      n_Delete(&ad, Z);
      number q = n_Farey(re, N, Z);
      n_Delete(&re, Z);
      if (!q)
      {
        n_Delete(&ad, Z);
        n_Delete(&den, Z);
        return NULL;
      }

      number d = n_GetDenom(q, Q),
             n = n_GetNumerator(q, Q);

      n_Delete(&q, Q);
      n_Delete(&ad, Z);
      number dz = f(d, Q, Z),
             nz = f(n, Q, Z);
      n_Delete(&d, Q);
      n_Delete(&n, Q);

      if (!n_IsOne(dz, Z))
      {
        L->skalmult(dz, Z);
        n_InpMult(den, dz, Z);
#if 0
        PrintS("den increasing to ");
        n_Print(den, Z);
        PrintLn();
#endif
      }
      n_Delete(&dz, Z);
      L->rawset(i, j, nz);
    }
  }

  nKillChar(Q);
  PrintS("bimFarey worked\n");
#if 0
  L->Print();
  PrintS("\n * 1/");
  n_Print(den, Z);
  PrintLn();
#endif
  return den;
}

#ifdef HAVE_RINGS
static number solveAx_dixon(bigintmat *A, bigintmat *B, bigintmat *x, bigintmat *kern) {
  coeffs R = A->basecoeffs();

  assume(getCoeffType(R) == n_Z);

  number p = n_Init(536870909, R); // PreviousPrime(2^29); not clever
  coeffs Rp = numbercoeffs(p, R); // R/pR
  bigintmat *Ap = bimChangeCoeff(A, Rp),
            *m = prependIdentity(Ap),
            *Tp, *Hp;
  delete Ap;

  m->howell();
  Hp = new bigintmat(A->rows(), A->cols(), Rp);
  Hp->copySubmatInto(m, A->cols()+1, 1, A->rows(), A->cols(), 1, 1);
  Tp = new bigintmat(A->cols(), A->cols(), Rp);
  Tp->copySubmatInto(m, 1, 1, A->cols(), A->cols(), 1, 1);

  int i, j;

  for(i=1; i<= A->cols(); i++)
  {
    for(j=m->rows(); j>A->cols(); j--)
    {
      if (!n_IsZero(m->view(j, i), Rp)) break;
    }
    if (j>A->cols()) break;
  }
//  Print("Found nullity (kern dim) of %d\n", i-1);
  bigintmat * kp = new bigintmat(A->cols(), i-1, Rp);
  kp->copySubmatInto(Tp, 1, 1, A->cols(), i-1, 1, 1);
  kp->howell();

  delete m;

  //Hp is the mod-p howell form
  //Tp the transformation, mod p
  //kp a basis for the kernel, in howell form, mod p

  bigintmat * eps_p = new bigintmat(B->rows(), B->cols(), Rp),
            * x_p   = new bigintmat(A->cols(), B->cols(), Rp),
            * fps_p = new bigintmat(kp->cols(), B->cols(), Rp);

  //initial solution

  number zero = n_Init(0, R);
  x->skalmult(zero, R);
  n_Delete(&zero, R);

  bigintmat * b = new bigintmat(B);
  number pp = n_Init(1, R);
  i = 1;
  do
  {
    bigintmat * b_p = bimChangeCoeff(b, Rp), * s;
    bigintmat * t1, *t2;
    reduce_mod_howell(Hp, b_p, eps_p, x_p);
    delete b_p;
    if (!eps_p->isZero())
    {
      PrintS("no solution, since no modular solution\n");

      delete eps_p;
      delete x_p;
      delete Hp;
      delete kp;
      delete Tp;
      delete b;
      n_Delete(&pp, R);
      n_Delete(&p, R);
      nKillChar(Rp);

      return NULL;
    }
    t1 = bimMult(Tp, x_p);
    delete x_p;
    x_p = t1;
    reduce_mod_howell(kp, x_p, x_p, fps_p);  //we're not all interested in fps_p
    s = bimChangeCoeff(x_p, R);
    t1 = bimMult(A, s);
    t2 = bimSub(b, t1);
    t2->skaldiv(p);
    delete b;
    delete t1;
    b = t2;
    s->skalmult(pp, R);
    t1 = bimAdd(x, s);
    delete s;
    x->swapMatrix(t1);
    delete t1;

    if(kern && i==1)
    {
      bigintmat * ker = bimChangeCoeff(kp, R);
      t1 = bimMult(A, ker);
      t1->skaldiv(p);
      t1->skalmult(n_Init(-1, R), R);
      b->appendCol(t1);
      delete t1;
      x->appendCol(ker);
      delete ker;
      x_p->extendCols(kp->cols());
      eps_p->extendCols(kp->cols());
      fps_p->extendCols(kp->cols());
    }

    n_InpMult(pp, p, R);

    if (b->isZero())
    {
      //exact solution found, stop
      delete eps_p;
      delete fps_p;
      delete x_p;
      delete Hp;
      delete kp;
      delete Tp;
      delete b;
      n_Delete(&pp, R);
      n_Delete(&p, R);
      nKillChar(Rp);

      return n_Init(1, R);
    }
    else
    {
      bigintmat *y = new bigintmat(x->rows(), x->cols(), R);
      number d = bimFarey(x, pp, y);
      if (d)
      {
        bigintmat *c = bimMult(A, y);
        bigintmat *bd = new bigintmat(B);
        bd->skalmult(d, R);
        if (kern)
        {
          bd->extendCols(kp->cols());
        }
        if (*c == *bd)
        {
          x->swapMatrix(y);
          delete y;
          delete c;
          if (kern)
          {
            y = new bigintmat(x->rows(), B->cols(), R);
            c = new bigintmat(x->rows(), kp->cols(), R);
            x->splitcol(y, c);
            x->swapMatrix(y);
            delete y;
            kern->swapMatrix(c);
            delete c;
          }

          delete bd;

          delete eps_p;
          delete fps_p;
          delete x_p;
          delete Hp;
          delete kp;
          delete Tp;
          delete b;
          n_Delete(&pp, R);
          n_Delete(&p, R);
          nKillChar(Rp);

          return d;
        }
        delete c;
        delete bd;
        n_Delete(&d, R);
      }
      delete y;
    }
    i++;
  } while (1);
  delete eps_p;
  delete fps_p;
  delete x_p;
  delete Hp;
  delete kp;
  delete Tp;
  n_Delete(&pp, R);
  n_Delete(&p, R);
  nKillChar(Rp);
  return NULL;
}
#endif

//TODO: re-write using reduce_mod_howell
static number solveAx_howell(bigintmat *A, bigintmat *b, bigintmat *x, bigintmat *kern)
{
  // try to solve Ax=b, more precisely, find
  //   number    d
  //   bigintmat x
  // sth. Ax=db
  // where d is small-ish (divides the determinant of A if this makes sense)
  // return 0 if there is no solution.
  //
  // if kern is non-NULL, return a basis for the kernel

  //Algo: we do row-howell (triangular matrix). The idea is
  //  Ax = b <=>  AT T^-1x = b
  //  y := T^-1 x, solve AT y = b
  //  and return Ty.
  //Howell does not compute the trafo, hence we need to cheat:
  //B := (I_n | A^t)^t, then the top part of the Howell form of
  //B will give a useful trafo
  //Then we can find x by back-substitution and lcm/gcd to find the denominator
  //The defining property of Howell makes this work.

  coeffs R = A->basecoeffs();
  bigintmat *m = prependIdentity(A);
  m->howell(); // since m contains the identity, we'll have A->cols()
               // many cols.
  number den = n_Init(1, R);

  bigintmat * B = new bigintmat(A->rows(), 1, R);
  for(int i=1; i<= b->cols(); i++)
  {
    int A_col = A->cols();
    b->getcol(i, B);
    B->skalmult(den, R);
    for(int j = B->rows(); j>0; j--)
    {
      number Ai = m->view(m->rows()-B->rows() + j, A_col);
      if (n_IsZero(Ai, R) &&
          n_IsZero(B->view(j, 1), R))
      {
        continue; //all is fine: 0*x = 0
      }
      else if (n_IsZero(B->view(j, 1), R))
      {
        x->rawset(x->rows() - B->rows() + j, i, n_Init(0, R));
        A_col--;
      }
      else if (n_IsZero(Ai, R))
      {
        delete m;
        delete B;
        n_Delete(&den, R);
        return 0;
      }
      else
      {
        // solve ax=db, possibly enlarging d
        // so x = db/a
        number Bj = B->view(j, 1);
        number g = n_Gcd(Bj, Ai, R);
        number xi;
        if (n_Equal(Ai, g, R))
        { //good: den stable!
          xi = n_Div(Bj, Ai, R);
        }
        else
        { //den <- den * (a/g), so old sol. needs to be adjusted
          number inc_d = n_Div(Ai, g, R);
          n_InpMult(den, inc_d, R);
          x->skalmult(inc_d, R);
          B->skalmult(inc_d, R);
          xi = n_Div(Bj, g, R);
          n_Delete(&inc_d, R);
        } //now for the back-substitution:
        x->rawset(x->rows() - B->rows() + j, i, xi);
        for(int k=j; k>0; k--)
        {
          //B[k] = B[k] - x[k]A[k][j]
          number s = n_Mult(xi, m->view(m->rows()-B->rows() + k, A_col), R);
          B->rawset(k, 1, n_Sub(B->view(k, 1), s, R));
          n_Delete(&s, R);
        }
        n_Delete(&g, R);
        A_col--;
      }
      if (!A_col)
      {
        if (B->isZero()) break;
        else
        {
          delete m;
          delete B;
          n_Delete(&den, R);
          return 0;
        }
      }
    }
  }
  delete B;
  bigintmat *T = new bigintmat(A->cols(), A->cols(), R);
  T->copySubmatInto(m, 1, 1, A->cols(), A->cols(), 1, 1);
  if (kern)
  {
    int i, j;
    for(i=1; i<= A->cols(); i++)
    {
      for(j=m->rows(); j>A->cols(); j--)
      {
        if (!n_IsZero(m->view(j, i), R)) break;
      }
      if (j>A->cols()) break;
    }
    Print("Found nullity (kern dim) of %d\n", i-1);
    bigintmat * ker = new bigintmat(A->rows(), i-1, R);
    ker->copySubmatInto(T, 1, 1, A->rows(), i-1, 1, 1);
    kern->swapMatrix(ker);
    delete ker;
  }
  delete m;
  bigintmat * y = bimMult(T, x);
  x->swapMatrix(y);
  delete y;
  x->simplifyContentDen(&den);
#if 0
  PrintS("sol = 1/");
  n_Print(den, R);
  PrintS(" *\n");
  x->Print();
  PrintLn();
#endif
  return den;
}

number solveAx(bigintmat *A, bigintmat *b, bigintmat *x)
{
#if 0
  PrintS("Solve Ax=b for A=\n");
  A->Print();
  PrintS("\nb = \n");
  b->Print();
  PrintS("\nx = \n");
  x->Print();
  PrintLn();
#endif

  coeffs R = A->basecoeffs();
  assume (R == b->basecoeffs());
  assume (R == x->basecoeffs());
  assume ((x->cols() == b->cols()) && (x->rows() == A->cols()) && (A->rows() == b->rows()));

  switch (getCoeffType(R))
  {
  #ifdef HAVE_RINGS
    case n_Z:
      return solveAx_dixon(A, b, x, NULL);
    case n_Zn:
    case n_Znm:
    case n_Z2m:
      return solveAx_howell(A, b, x, NULL);
  #endif
    case n_Zp:
    case n_Q:
    case n_GF:
    case n_algExt:
    case n_transExt:
      WarnS("have field, should use Gauss or better");
      break;
    default:
      if (R->cfXExtGcd && R->cfAnn)
      { //assume it's Euclidean
        return solveAx_howell(A, b, x, NULL);
      }
      WerrorS("have no solve algorithm");
      break;
  }
  return NULL;
}

void diagonalForm(bigintmat *A, bigintmat ** S, bigintmat ** T)
{
  bigintmat * t, *s, *a=A;
  coeffs R = a->basecoeffs();
  if (T)
  {
    *T = new bigintmat(a->cols(), a->cols(), R),
    (*T)->one();
    t = new bigintmat(*T);
  }
  else
  {
    t = *T;
  }

  if (S)
  {
    *S = new bigintmat(a->rows(), a->rows(), R);
    (*S)->one();
    s = new bigintmat(*S);
  }
  else
  {
    s = *S;
  }

  int flip=0;
  do
  {
    bigintmat * x, *X;
    if (flip)
    {
      x = s;
      X = *S;
    }
    else
    {
      x = t;
      X = *T;
    }

    if (x)
    {
      x->one();
      bigintmat * r = new bigintmat(a->rows()+a->cols(), a->cols(), R);
      bigintmat * rw = new bigintmat(1, a->cols(), R);
      for(int i=0; i<a->cols(); i++)
      {
        x->getrow(i+1, rw);
        r->setrow(i+1, rw);
      }
      for (int i=0; i<a->rows(); i++)
      {
        a->getrow(i+1, rw);
        r->setrow(i+a->cols()+1, rw);
      }
      r->hnf();
      for(int i=0; i<a->cols(); i++)
      {
        r->getrow(i+1, rw);
        x->setrow(i+1, rw);
      }
      for(int i=0; i<a->rows(); i++)
      {
        r->getrow(i+a->cols()+1, rw);
        a->setrow(i+1, rw);
      }
      delete rw;
      delete r;

#if 0
      Print("X: %ld\n", X);
      X->Print();
      Print("\nx: %ld\n", x);
      x->Print();
#endif
      bimMult(X, x, X);
#if 0
      Print("\n2:X: %ld %ld %ld\n", X, *S, *T);
      X->Print();
      Print("\n2:x: %ld\n", x);
      x->Print();
      PrintLn();
#endif
    }
    else
    {
      a->hnf();
    }

    int diag = 1;
    for(int i=a->rows(); diag && i>0; i--)
    {
      for(int j=a->cols(); j>0; j--)
      {
        if ((a->rows()-i)!=(a->cols()-j) && !n_IsZero(a->view(i, j), R))
        {
          diag = 0;
          break;
        }
      }
    }
#if 0
    PrintS("Diag ? %d\n", diag);
    a->Print();
    PrintLn();
#endif
    if (diag) break;

    a = a->transpose(); // leaks - I need to write inpTranspose
    flip = 1-flip;
  } while (1);
  if (flip)
    a = a->transpose();

  if (S) *S = (*S)->transpose();
  if (s) delete s;
  if (t) delete t;
  A->copy(a);
}

#ifdef HAVE_RINGS
//a "q-base" for the kernel of a.
//Should be re-done to use Howell rather than smith.
//can be done using solveAx now.
int kernbase (bigintmat *a, bigintmat *c, number p, coeffs q)
{
#if 0
  PrintS("Kernel of ");
  a->Print();
  PrintS(" modulo ");
  n_Print(p, q);
  PrintLn();
#endif

  coeffs coe = numbercoeffs(p, q);
  bigintmat *m = bimChangeCoeff(a, coe), *U, *V;
  diagonalForm(m, &U, &V);
#if 0
  PrintS("\ndiag form: ");
  m->Print();
  PrintS("\nU:\n");
  U->Print();
  PrintS("\nV:\n");
  V->Print();
  PrintLn();
#endif

  int rg = 0;
#undef MIN
#define MIN(a,b) (a < b ? a : b)
  for(rg=0; rg<MIN(m->rows(), m->cols()) && !n_IsZero(m->view(m->rows()-rg,m->cols()-rg), coe); rg++);

  bigintmat * k = new bigintmat(m->cols(), m->rows(), coe);
  for(int i=0; i<rg; i++)
  {
    number A = n_Ann(m->view(m->rows()-i, m->cols()-i), coe);
    k->set(m->cols()-i, i+1, A);
    n_Delete(&A, coe);
  }
  for(int i=rg; i<m->cols(); i++)
  {
    k->set(m->cols()-i, i+1-rg, n_Init(1, coe));
  }
  bimMult(V, k, k);
  c->copy(bimChangeCoeff(k, q));
  return c->cols();
}
#endif

bool nCoeffs_are_equal(coeffs r, coeffs s)
{
  if ((r == NULL) || (s == NULL))
    return false;
  if (r == s)
    return true;
  if ((getCoeffType(r)==n_Z) && (getCoeffType(s)==n_Z))
    return true;
  if ((getCoeffType(r)==n_Zp) && (getCoeffType(s)==n_Zp))
  {
    if (r->ch == s->ch)
      return true;
    else
      return false;
  }
  // n_Zn stimmt wahrscheinlich noch nicht
  if ((getCoeffType(r)==n_Zn) && (getCoeffType(s)==n_Zn))
  {
    if (r->ch == s->ch)
      return true;
    else
      return false;
  }
  if ((getCoeffType(r)==n_Q) && (getCoeffType(s)==n_Q))
    return true;
  // FALL n_Zn FEHLT NOCH!
    //if ((getCoeffType(r)==n_Zn) && (getCoeffType(s)==n_Zn))
  return false;
}

number bigintmat::content()
{
  coeffs r = basecoeffs();
  number g = get(1,1), h;
  int n=rows()*cols();
  for(int i=1; i<n && !n_IsOne(g, r); i++)
  {
    h = n_Gcd(g, view(i), r);
    n_Delete(&g, r);
    g=h;
  }
  return g;
}
void bigintmat::simplifyContentDen(number *d)
{
  coeffs r = basecoeffs();
  number g = n_Copy(*d, r), h;
  int n=rows()*cols();
  for(int i=0; i<n && !n_IsOne(g, r); i++)
  {
    h = n_Gcd(g, view(i), r);
    n_Delete(&g, r);
    g=h;
  }
  *d = n_Div(*d, g, r);
  if (!n_IsOne(g, r))
  skaldiv(g);
}
