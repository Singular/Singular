/* Probleme:
3) Ausgabe als String fehlt noch
5) Welche der eingebundenen Dateien werden wirklich benötigt?

in Makefile.in
Dateien eintragen, in /SIngular/ config.status ausführen

table.h in /Singular/


//spielwiese
autogen.sh

/libpoly/misc/Makefile.am
*/
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

// Beginnt bei [1,1]
void bigintmat::set(int i, int j, number n)
{
  nlDelete(&(v[(i-1)*col+j-1]), NULL);
  v[(i-1)*col+j-1] = nlCopy(n);
}

// Beginnt bei [0]
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
  	number prod = nlMult(v[i], iop);
  	nlDelete(&(v[i]), NULL);
  	nlDelete(&iop, NULL);
  	v[i] = prod;
  }
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
  mn = si_min(a->rows(),b->rows());
  ma = si_max(a->rows(),b->rows());
  if (a->cols() == 1)
  {
    bim = new bigintmat(ma, 1);
    for (i=0; i<mn; i++)
    {
      number n = nlAdd((*a)[i], (*b)[i]);
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
    number n = nlAdd((*a)[i], (*b)[i]);
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
      number n = nlSub((*a)[i], (*b)[i]);
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
    number n = nlSub((*a)[i], (*b)[i]);
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
        number prod = nlMult((*a)[i*ca+k], (*b)[k*cb+j]);
        number sum2 = nlAdd(sum, prod);
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

void bigintmat::print()
{
  StringSetS("");
  for (int i=0; i<col*row; i++)
  {
    //int n = nlInt(v[i], NULL);
    //char tmp[10];
    //sprintf(tmp, "%d", n);
    nlWrite(v[i], NULL);
    //PrintS(tmp); //
    if (i != col*row-1)
    {
      StringAppend(",");
      if ((i+1)%col == 0)
        StringAppend("\n");
    }    
  }
  PrintS(StringAppend(""));
}

void bigintmat::prettyprint(int swid)
{
 // if (col*row == 0) return "";
  int colwid = floor((swid-2*(col-1))/col); // Berechnung der Spaltenbreite (im Zweifel abrunden)
  char * ps;
  ps = (char*) omAlloc(sizeof(char)*(colwid*col+2*(col-1)+row-1));  //Reale Länge des Strings
  int pos = 0;
  for (int i=0; i<col*row; i++)
  {
    SPrintStart();
    nlWrite(v[i], NULL);
    char * ts = SPrintEnd();
    int nl = strlen(ts); 
    if (nl > colwid)  // Zu lange Zahl wird durch ### dargestellt
    {
      for (int j=0; j<colwid; j++)
        ps[pos+j] = '#';
    }
    else  // Mit Leerzeichen auffüllen und zahl reinschreiben
    {
      for (int j=0; j<colwid-nl; j++)
        ps[pos+j] = ' ';
      for (int j=0; j<nl; j++)
        ps[pos+colwid-nl+j] = ts[j];
    }
    
    // ", " oder "\n" einfügen
    if (((i+1)%col == 0) && (i != col*row))
    {
      ps[colwid] = '\n';
      pos += colwid+1;
    }
    else
    {
      ps[colwid] = ',';
      ps[colwid+1] = ' ';
      pos += colwid+2;
    }
  // Hier ts zerstören
  }
}
