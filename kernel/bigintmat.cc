/*****************************************
*  Computer Algebra System SINGULAR      *
*****************************************/
/*
* ABSTRACT: class bigintmat: matrizes of big integers
*/
#include <kernel/mod2.h>
#include <kernel/febase.h>
#include <kernel/options.h>
#include <kernel/bigintmat.h>
#include <omalloc/omalloc.h>
#include <kernel/longrat.h>
#include <kernel/intvec.h>
#include <math.h>
#include <string.h>

#define BIGIMATELEM(M,I,J) (M)[(I-1)*(M).cols()+J-1]

// first entry is [1,1]
void bigintmat::set(int i, int j, number n)
{
  nlDelete(&(BIMATELEM(*this,i,j)), NULL);
  BIMATELEM(*this,i,j) = nlCopy(n);
}

// sees matrix as vector, first entry is [0]
void bigintmat::set(int i, number n)
{
  if (i<row*col)
  {
    nlDelete(&(v[i]), NULL);
    v[i] = nlCopy(n);
  }
}

number bigintmat::get(int i, int j)
{
  return nlCopy(v[(i-1)*col+j-1]);
}

number bigintmat::get(int i)
{
  return nlCopy(v[i]);
}

bigintmat::bigintmat(int r, int c)
{
  row = r;
  col = c;
  int l = r*c;
  if (l>0)
    v = (number *)omAlloc(sizeof(number)*l);
  else
    v = NULL;
  for (int i=0; i<l; i++)
  {
    v[i] = nlInit(0, NULL);
  }
}

void bigintmat::operator*=(int intop)
{
  number iop = nlInit(intop, NULL);
  for (int i=0; i<row*col; i++)
  {
          number prod = nlMult(v[i], iop);
          nlDelete(&(v[i]), NULL);
          v[i] = prod;
  }
  nlDelete(&iop, NULL);
}

void bigintmat::operator*=(number bintop)
{
  for (int i=0; i<row*col; i++)
  {
          number prod = nlMult(v[i], bintop);
          nlDelete(&(v[i]), NULL);
          v[i] = prod;
  }
}

void bigintmat::operator-=(int intop)
{
  number iop = nlInit(intop, NULL);
  for (int i=0; i<row*col; i++)
  {
          number diff = nlSub(v[i], iop);
          nlDelete(&(v[i]), NULL);
          v[i] = diff;
  }
  nlDelete(&iop, NULL);
}

void bigintmat::operator-=(number bintop)
{
  for (int i=0; i<row*col; i++)
  {
          number diff = nlSub(v[i], bintop);
          nlDelete(&(v[i]), NULL);
          v[i] = diff;
  }
}

void bigintmat::operator+=(int intop)
{
  number iop = nlInit(intop, NULL);
  for (int i=0; i<row*col; i++)
  {
          number sum = nlAdd(v[i], iop);
          nlDelete(&(v[i]), NULL);
          v[i] = sum;
  }
  nlDelete(&iop, NULL);
}

void bigintmat::operator+=(number bintop)
{
  for (int i=0; i<row*col; i++)
  {
          number sum = nlAdd(v[i], bintop);
          nlDelete(&(v[i]), NULL);
          v[i] = sum;
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
    if (!nlEqual(lhr[i], rhr[i])) { return false; }
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
  if (a->rows() != b->rows()) return NULL;
  bim = new bigintmat(a->rows(), a->cols());
  for (i=0; i<a->rows()*a->cols(); i++)
  {
    number n = nlAdd((*a)[i], (*b)[i]);
    nlDelete(&(*bim)[i], NULL);
    (*bim)[i]=n;
  }
  return bim;
}

bigintmat * bimSub(bigintmat * a, bigintmat * b)
{
  bigintmat * bim;
  int mn, ma, i;
  if (a->cols() != b->cols()) return NULL;
  if (a->rows() != b->rows()) return NULL;
  bim = new bigintmat(a->rows(), a->cols());
  for (i=0; i<a->rows()*a->cols(); i++)
  {
    number n = nlSub((*a)[i], (*b)[i]);
    nlDelete(&(*bim)[i], NULL);
    (*bim)[i]=n;
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
  for (i=1; i<=ra; i++)
  {
    for (j=1; j<=cb; j++)
    {
      sum = nlInit(0, NULL);
      for (k=1; k<=ca; k++)
      {
        number prod = nlMult(BIMATELEM((*a),i,k), BIMATELEM((*b),k,j));
        number sum2 = nlAdd(sum, prod);
        nlDelete(&sum, NULL);
        sum = sum2;
        nlDelete(&prod, NULL);
      }
      nlDelete(&BIMATELEM((*bim),i,j),NULL);
      BIMATELEM((*bim),i,j)=sum;
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
    if (nlGreater(v[i], (*op)[i]))
      return 1;
    else if (!nlEqual(v[i], (*op)[i]))
      return -1;
  }

  for (; i<row; i++)
  {
    if (nlGreaterZero(v[i]))
      return 1;
    else if (!nlIsZero(v[i]))
      return -1;
  }
  for (; i<op->rows(); i++)
  {
    if (nlGreaterZero((*op)[i]))
      return -1;
    else if (!nlIsZero((*op)[i]))
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

bigintmat * bigintmat::transpose()
{
  bigintmat * t = new bigintmat(col, row);
  for (int i=1; i<=row; i++)
  {
    for (int j=1; j<=col; j++)
    {
      t->set(j, i, BIMATELEM(*this,i,j));
    }
  }
  return t;
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
  if ((col!=0) && (row!=0))
    nlWrite(v[i], NULL);
   /* if (i != col*row-1)
    {
      StringAppendS(",");
      if ((i+1)%col == 0)
        StringAppendS("\n");
    }   */
  return StringEndS();
}

char* bigintmat::StringAsPrinted()
{
  if ((col==0) || (row==0))
    return NULL;
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
  slength += 2*(col-1)*row+2*row-1;
  ps = (char*) omAlloc0(sizeof(char)*(slength));
  int pos = 0;
  for (int i=0; i<col*row; i++)
  {
    StringSetS("");
    nlWrite(v[i], NULL);
    char * ts = StringEndS();
    int nl = strlen(ts);
    int cj = i%col;
    if (nl > colwid[cj])
    {
      StringSetS("");
      int ci = i/col;
      StringAppend("[%d,%d]", ci+1, cj+1);
      char *ph = StringEndS();
      int phl = strlen(ph);
      if (phl > colwid[cj])
      {
        for (int j=0; j<colwid[cj]; j++)
          ps[pos+j] = '*';
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
      for (int j=0; j<colwid[cj]-nl; j++)
        ps[pos+j] = ' ';
      for (int j=0; j<nl; j++)
        ps[pos+colwid[cj]-nl+j] = ts[j];
    }
    // ", " oder "\n" einfügen
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
      ps[pos+colwid[cj]+1] = ' ';
      pos += colwid[cj]+2;
    }
    omFree(ts);
  }
  return ps;
}

int intArrSum(int * a, int length)
{
  int sum = 0;
  for (int i=0; i<length; i++)
    sum += a[i];
  return sum;
}

// Eingabe muss erlaubt sein (kein Vektor mit nur 0)
int findLongest(int * a, int length)
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

int getShorter (int * a, int l, int j, int cols, int rows)
{
  int sndlong = 0;
  for (int i=0; i<rows; i++)
  {
    int index = cols*i+j;
    if ((a[index] > sndlong) && (a[index] < l))
    {
      int min = (int)(floor(log10((double)cols))+floor(log10((double)rows)))+5;
      if ((a[index] < min) && (min < l))
        sndlong = min;
      else
        sndlong = a[index];
    }
  }
  if (sndlong == 0)
  {
    int min = (int)(floor(log10((double)cols))+floor(log10((double)rows)))+5;
    if (min < l)
      sndlong = min;
    else
      sndlong = l-1;
  }
  return sndlong;
}

int * bigintmat::getwid(int maxwid)
{
  int const c = 2*(col-1)+1;
  if (col + c > maxwid-1) { return NULL; }
  int * wv = (int*)omAlloc(sizeof(int)*col*row);
  int * cwv = (int*)omAlloc(sizeof(int)*col);
  for (int j=0; j<col; j++)
  {
    cwv[j] = 0;
    for (int i=0; i<row; i++)
    {
      StringSetS("");
      nlWrite(v[col*i+j], NULL);
      char * ts = StringEndS();
      int nl = strlen(ts);
      wv[col*i+j] = nl;
      if (nl > cwv[j])
        cwv[j] = nl;
    }
  }

  // Größen verkleinern
  while (intArrSum(cwv, col)+c > maxwid)
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
    if (colwid == NULL)
    {
      WerrorS("not enough space to print bigintmat");
      WerrorS("try string(...) for a unformatted output");
      return;
    }
    char * ps;
    int slength = 0;
    for (int j=0; j<col; j++)
      slength += colwid[j]*row;
    slength += 2*(col-1)*row+2*row;
    ps = (char*) omAlloc0(sizeof(char)*(slength));
    int pos = 0;
    for (int i=0; i<col*row; i++)
    {
      StringSetS("");
      nlWrite(v[i], NULL);
      char * ts = StringEndS();
      int nl = strlen(ts);
      int cj = i%col;
      if (nl > colwid[cj])
      {
        StringSetS("");
        int ci = i/col;
        StringAppend("[%d,%d]", ci+1, cj+1);
        char *ph = StringEndS();
        int phl = strlen(ph);
        if (phl > colwid[cj])
        {
          for (int j=0; j<colwid[cj]; j++)
            ps[pos+j] = '*';
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
        for (int j=0; j<colwid[cj]-nl; j++)
          ps[pos+j] = ' ';
        for (int j=0; j<nl; j++)
          ps[pos+colwid[cj]-nl+j] = ts[j];
      }
      // ", " oder "\n" einfügen
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
        ps[pos+colwid[cj]+1] = ' ';
        pos += colwid[cj]+2;
      }
    omFree(ts);
    }
    PrintS(ps);
//    omFree(ps);
  }
}

// Ungetestet
static void bimRowContent(bigintmat *bimat, int rowpos, int colpos)
{
  number tgcd, m;
  int i=bimat->cols();

  loop
  {
    tgcd = nlCopy(BIMATELEM(*bimat,rowpos,i--));
    if (!nlIsZero(tgcd)) break;
    if (i<colpos) return;
  }
  if ((!nlGreaterZero(tgcd)) && (!nlIsZero(tgcd))) tgcd = nlNeg(tgcd);
  if (nlIsOne(tgcd)) return;
  loop
  {
    m = BIMATELEM(*bimat,rowpos,i--);
    if (!nlIsZero(m))
    {
      number tp1 = nlGcd(tgcd, m, NULL);
      nlDelete(&tgcd, NULL);
      tgcd = tp1;
    }
    if (nlIsOne(tgcd)) return;
    if (i<colpos) break;
  }
  for (i=bimat->cols();i>=colpos;i--)
  {
    number tp2 = nlDiv(BIMATELEM(*bimat,rowpos,i), tgcd);
    nlDelete(&BIMATELEM(*bimat,rowpos,i), NULL);
    BIMATELEM(*bimat,rowpos,i) = tp2;
  }
  nlDelete(&tgcd, NULL);
}

static void bimReduce(bigintmat *bimat, int rpiv, int colpos,
                     int ready, int all)
{
  number tgcd, ce, m1, m2;
  int j, i;
  number piv = BIMATELEM(*bimat,rpiv,colpos);

  for (j=all;j>ready;j--)
  {
    ce = nlCopy(BIMATELEM(*bimat,j,colpos));
    if (!nlIsZero(ce))
    {
      nlDelete(&BIMATELEM(*bimat,j,colpos), NULL);
      BIMATELEM(*bimat,j,colpos) = nlInit(0, NULL);
      m1 = nlCopy(piv);
      m2 = nlCopy(ce);
      tgcd = nlGcd(m1, m2, NULL);
      if (!nlIsOne(tgcd))
      {
        number tp1 = nlDiv(m1, tgcd);
        number tp2 = nlDiv(m2, tgcd);
        nlDelete(&m1, NULL);
        nlDelete(&m2, NULL);
        m1 = tp1;
        m2 = tp2;
      }
      for (i=bimat->cols();i>colpos;i--)
      {
        nlDelete(&BIMATELEM(*bimat,j,i), NULL);
        number tp1 = nlMult(BIMATELEM(*bimat,j,i), m1);
        number tp2 = nlMult(BIMATELEM(*bimat,rpiv,i), m2);
        BIMATELEM(*bimat,j,i) = nlSub(tp1, tp2);
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

// columnwise concatination of two bigintmats
bigintmat * bimConcat(bigintmat * a, bigintmat * b)
{
  int ac=a->cols();
  int r=si_max(a->rows(),b->rows());
  bigintmat * ab = new bigintmat(r,ac + b->cols());

  int i,j;
  for (i=1; i<=a->rows(); i++)
  {
    for (j=1; j<=ac; j++)
    {
      BIMATELEM(*ab,i,j)=nlCopy(BIMATELEM(*a,i,j));
    }
  }

  for (i=1; i<=b->rows(); i++)
  {
    for (j=1; j<=b->cols(); j++)
    {
      BIMATELEM(*ab,i,j+ac)=nlCopy(BIMATELEM(*b,i,j));
    }
  }

  return ab;
}

