/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: class bigintmat: matrizes of big integers
*/
#include <kernel/mod2.h>
//#include <kernel/options.h>
#include "bigintmat.h"
#include <omalloc/omalloc.h>
#include <coeffs/longrat.h>
#include <misc/intvec.h>
#include <math.h>
#include <string.h>

#define BIGIMATELEM(M,I,J) (M)[(I-1)*(M).cols()+J-1]

// Beginnt bei [1,1]
void bigintmat::set(int i, int j, number n)
{
  nlDelete(&(v[(i-1)*col+j-1]), NULL);
  v[(i-1)*col+j-1] = nlCopy(n,NULL);
}

// Beginnt bei [0]
void bigintmat::set(int i, number n)
{
  if (i<row*col)
  {
    nlDelete(&(v[i]), NULL);
    v[i] = nlCopy(n,NULL);
  }
}

number bigintmat::get(int i, int j)
{
  return nlCopy(v[(i-1)*col+j-1],NULL);
}

number bigintmat::get(int i)
{
  return nlCopy(v[i],NULL);
}

bigintmat::bigintmat(int r, int c)
{
  row = r;
  col = c;
  int l = r*c;
  if (l>0) /*(r>0) && (c>0) */
    v = (number *)omAlloc(sizeof(number)*l);
  else
    v = NULL;
  for (int i=0; i<l; i++)
  {
    v[i] = nlInit(0, NULL);
  }
}

// Überladener *=-Operator (für int und bigint)
// Frage hier: *= verwenden oder lieber = und * einzeln?
void bigintmat::operator*=(int intop)
{
  for (int i=0; i<row*col; i++)
  {
          number iop = nlInit(intop, NULL);
          number prod = nlMult(v[i], iop,NULL);
          nlDelete(&(v[i]), NULL);
          nlDelete(&iop, NULL);
          v[i] = prod;
  }
}

void bigintmat::operator*=(number bintop)
{
  for (int i=0; i<row*col; i++)
  {
          number prod = nlMult(v[i], bintop,NULL);
          nlDelete(&(v[i]), NULL);
          v[i] = prod;
  }
}

// Stimmen Parameter?
// Welche der beiden Methoden?
// Oder lieber eine comp-Funktion?

bool operator==(bigintmat & lhr, bigintmat & rhr)
{
  if (&lhr == &rhr) { return true; }
  if (lhr.cols() != rhr.cols()) { return false; }
  if (lhr.rows() != rhr.rows()) { return false; }
  for (int i=0; i<(lhr.rows())*(lhr.cols()); i++)
  {
    if (!nlEqual(lhr[i], rhr[i],NULL)) { return false; }
  }
  return true;
}

bool operator!=(bigintmat & lhr, bigintmat & rhr)
{
  return !(lhr==rhr);
}

// Matrix-Add/-Sub/-Mult so oder mit operator+/-/* ?
bigintmat * bimAdd(bigintmat * a, bigintmat * b)
{
  bigintmat * bim;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    bim = new bigintmat(ma, 1);
    for (i=0; i<mn; i++)
    {
      number n = nlAdd((*a)[i], (*b)[i],NULL);
      bim->set(i, n);
      nlDelete(&n, NULL);
    }
    if (ma > mn)
    {
      if (ma == a->rows())
      {
        for(i=mn; i<ma; i++)
        {
          bim->set(i, (*a)[i]);
        }
      }
      else
      {
        for(i=mn; i<ma; i++)
        {
          bim->set(i, (*b)[i]);
        }
      }
    }
    return bim;
  }
  if (mn != ma) return NULL;
  bim = new bigintmat(mn, a->cols());
  for (i=0; i<mn*a->cols(); i++)
  {
    number n = nlAdd((*a)[i], (*b)[i],NULL);
    bim->set(i, n);
    nlDelete(&n, NULL);
  }
  return bim;
}

bigintmat * bimSub(bigintmat * a, bigintmat * b)
{
  bigintmat * bim;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    bim = new bigintmat(ma, 1);
    for (i=0; i<mn; i++)
    {
      number n = nlSub((*a)[i], (*b)[i],NULL);
      bim->set(i, n);
      nlDelete(&n, NULL);
    }
    if (ma > mn)
    {
      if (ma == a->rows())
      {
        for(i=mn; i<ma; i++)
        {
          bim->set(i, (*a)[i]);
        }
      }
      else
      {
        for(i=mn; i<ma; i++)
        {
          bim->set(i, (*b)[i]);
        }
      }
    }
    return bim;
  }
  if (mn != ma) return NULL;
  bim = new bigintmat(mn, a->cols());
  for (i=0; i<mn*a->cols(); i++)
  {
    number n = nlSub((*a)[i], (*b)[i],NULL);
    bim->set(i, n);
    nlDelete(&n, NULL);
  }
  return bim;
}

bigintmat * bimMult(bigintmat * a, bigintmat * b)
{
  int i, j, k,
      ra = a->rows(), ca = a->cols(),
      rb = b->rows(), cb = b->cols();
  number sum;
  bigintmat * bim;
  if (ca != rb) return NULL;
  bim = new bigintmat(ra, cb);
  for (i=0; i<ra; i++)
  {
    for (j=0; j<cb; j++)
    {
      sum = nlInit(0, NULL);
      for (k=0; k<ca; k++)
      {
        number prod = nlMult((*a)[i*ca+k], (*b)[k*cb+j],NULL);
        number sum2 = nlAdd(sum, prod,NULL);
        nlDelete(&sum, NULL);
        sum = sum2;
        nlDelete(&prod, NULL);
      }
      bim->set(i+1, j+1, sum);
      nlDelete(&sum, NULL);
    }
  }
  return bim;
}

// Korrekt?

intvec * bim2iv(bigintmat * b)
{
  intvec * iv;
  iv = new intvec(b->rows(), b->cols(), 0);
  for (int i=0; i<(b->rows())*(b->cols()); i++) { (*iv)[i] = nlInt((*b)[i], NULL); } // Geht das so?
  return iv;
}

bigintmat * iv2bim(intvec * b)
{
  bigintmat * bim;
  bim = new bigintmat(b->rows(), b->cols());
  for (int i=0; i<(b->rows())*(b->cols()); i++)
  {
    number n = nlInit((*b)[i], NULL);
    bim->set(i, n);
    nlDelete(&n, NULL);
  }
  return bim;
}

int bigintmat::compare(const bigintmat* op) const
{
  if ((col!=1) ||(op->cols()!=1))
  {
    if((col!=op->cols())
    || (row!=op->rows()))
      return -2;
  }
  int i;
  for (i=0; i<si_min(row*col,op->rows()*op->cols()); i++)
  {
    if (nlGreater(v[i], (*op)[i],NULL))
      return 1;
    else if (!nlEqual(v[i], (*op)[i],NULL))
      return -1;
  }

  for (; i<row; i++)
  {
    if (nlGreaterZero(v[i],NULL))
      return 1;
    else if (!nlIsZero(v[i],NULL))
      return -1;
  }
  for (; i<op->rows(); i++)
  {
    if (nlGreaterZero((*op)[i],NULL))
      return -1;
    else if (!nlIsZero((*op)[i],NULL))
      return 1;
  }
  return 0;
}


bigintmat * bimCopy(const bigintmat * b)
{
  bigintmat * a=NULL;
  if (b!=NULL)
  {
    a = new bigintmat(b->rows(), b->cols());
    for (int i=0; i<(b->rows())*(b->cols()); i++)
    {
      a->set(i, (*b)[i]);
    }
  }
  return a;
}

char* bigintmat::String()
{
  StringSetS("");
  int i;
  for (i=0; i<col*row-1; i++)
  {
    nlWrite(v[i], NULL);
    StringAppendS(",");
  }
  nlWrite(v[i], NULL);
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
    nlWrite(v[i], NULL);
    char * tmp = StringAppendS("");
    char * ts = omStrDup(tmp);
    int nl = strlen(ts);
    if (nl > wid)
    {
      if (nl > colwid)
      {
        int phwid = floor(log10(row))+floor(log10(col))+5;
        if ((colwid > phwid) && (wid < phwid))
          wid = phwid;
      }
      else
        wid = nl;
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
      nlWrite(v[i], NULL);
      char * temp = StringAppendS("");
      char * ts = omStrDup(temp);
      int nl = strlen(ts);
      if (nl > colwid)
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
        for (int j=0; j<colwid-nl; j++)
          ps[pos+j] = ' ';
        for (int j=0; j<nl; j++)
          ps[pos+colwid-nl+j] = ts[j];
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
  number tgcd, m;
  int i=bimat->cols();

  loop
  {
    tgcd = nlCopy(BIMATELEM(*bimat,rowpos,i--),NULL);
    if (!nlIsZero(tgcd,NULL)) break;
    if (i<colpos) return;
  }
  if ((!nlGreaterZero(tgcd,NULL)) && (!nlIsZero(tgcd,NULL))) tgcd = nlNeg(tgcd,NULL);
  if (nlIsOne(tgcd,NULL)) return;
  loop
  {
    m = nlCopy(BIMATELEM(*bimat,rowpos,i--),NULL);
    if (!nlIsZero(m,NULL))
    {
      number tp1 = nlGcd(tgcd, m, NULL);
      nlDelete(&tgcd, NULL);
      tgcd = tp1;
    }
    if (nlIsOne(tgcd,NULL)) return;
    if (i<colpos) break;
  }
  for (i=bimat->cols();i>=colpos;i--)
  {
    number tp2 = nlDiv(BIMATELEM(*bimat,rowpos,i), tgcd,NULL);
    nlDelete(&BIMATELEM(*bimat,rowpos,i), NULL);
    BIMATELEM(*bimat,rowpos,i) = tp2;
  }
  nlDelete(&tgcd, NULL);
  nlDelete(&m, NULL);
}

static void bimReduce(bigintmat *bimat, int rpiv, int colpos,
                     int ready, int all)
{
  number tgcd, ce, m1, m2;
  int j, i;
  number piv = BIMATELEM(*bimat,rpiv,colpos);

  for (j=all;j>ready;j--)
  {
    ce = nlCopy(BIMATELEM(*bimat,j,colpos),NULL);
    if (!nlIsZero(ce,NULL))
    {
      nlDelete(&BIMATELEM(*bimat,j,colpos), NULL);
      BIMATELEM(*bimat,j,colpos) = nlInit(0, NULL);
      m1 = nlCopy(piv,NULL);
      m2 = nlCopy(ce,NULL);
      tgcd = nlGcd(m1, m2, NULL);
      if (!nlIsOne(tgcd,NULL))
      {
        number tp1 = nlDiv(m1, tgcd,NULL);
        number tp2 = nlDiv(m2, tgcd,NULL);
        nlDelete(&m1, NULL);
        nlDelete(&m2, NULL);
        m1 = tp1;
        m2 = tp2;
      }
      for (i=bimat->cols();i>colpos;i--)
      {
        nlDelete(&BIMATELEM(*bimat,j,i), NULL);
        number tp1 = nlMult(BIMATELEM(*bimat,j,i), m1,NULL);
        number tp2 = nlMult(BIMATELEM(*bimat,rpiv,i), m2,NULL);
        BIMATELEM(*bimat,j,i) = nlSub(tp1, tp2,NULL);
        nlDelete(&tp1, NULL);
        nlDelete(&tp2, NULL);
      }
      bimRowContent(bimat, j, colpos+1);
      nlDelete(&m1, NULL);
      nlDelete(&m2, NULL);
    }
    nlDelete(&ce, NULL);
  }
}





