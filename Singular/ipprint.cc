/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: ipprint.cc,v 1.7 1998-10-21 10:25:35 krueger Exp $ */
/*
* ABSTRACT: interpreter: printing
*/

#include "mod2.h"
#include "tok.h"
#include "ipid.h"
#include "mmemory.h"
#include "febase.h"
#include "polys.h"
#include "matpol.h"
#include "subexpr.h"
#include "intvec.h"
#include "ipshell.h"
#include "ipprint.h"

/*2
* print for: int, string, poly, vector, ideal
*/
BOOLEAN jjPRINT_GEN(leftv res, leftv u)
{
  char *s=u->String();
  PrintS(s);
  PrintLn();
  FreeL((ADDRESS)s);
  return FALSE;
}

/*2
* print for: list
*/
BOOLEAN jjPRINT_LIST(leftv res, leftv u)
{
  u->Print();
  return FALSE;
}

/*2
* print for: intvec
*/
BOOLEAN jjPRINT_INTVEC(leftv res, leftv u)
{
  intvec *v=(intvec *)u->Data();
  v->show();
  PrintLn();
  return FALSE;
}

/*2
* print for: intmat
*/
BOOLEAN jjPRINT_INTMAT(leftv res, leftv u)
{
  intvec *v=(intvec *)u->Data();
  int i,j;
  for(i=0;i<v->rows();i++)
  {
    for(j=0;j<v->cols();j++)
    {
      Print(" %5d",IMATELEM(*v,i+1,j+1));
    }
    PrintLn();
  }
  return FALSE;
}

/*2
* internal print for: matrix
*/
void jjPRINT_MA0(matrix m, const char *name)
{
  if (MATCOLS(m)>0)
  {
    char **s=(char **)Alloc(MATCOLS(m)*MATROWS(m)*sizeof(char*));
    char *ss;
    int *l=(int *)Alloc0(MATCOLS(m)*sizeof(int));
    int i,j,k;
    int vl=max(colmax/MATCOLS(m),8);

    /* make enough space for the "largest" name*/
    ss=(char *)AllocL(14+strlen(name));
    sprintf(ss,"%s[%d,%d]",name,MATCOLS(m),MATROWS(m));
    vl=max(vl,strlen(ss));
    FreeL(ss);

    /* convert all polys to string */
    i=MATCOLS(m)*MATROWS(m)-1;
    ss=pString(m->m[i]);
    if ((int)strlen(ss)>colmax) s[i]=NULL;
    else                        s[i]=mstrdup(ss);
    for(i--;i>=0;i--)
    {
      pString(m->m[i]);
      ss=StringAppend(",");
      if ((int)strlen(ss)>colmax) s[i]=NULL;
      else                        s[i]=mstrdup(ss);
    }
    /* look up the width of all columns, put it in l[col_nr] */
    /* insert names for very long entries */
    for(i=MATROWS(m)-1;i>=0;i--)
    {
      for(j=MATCOLS(m)-1;j>=0;j--)
      {
        if (s[i*MATCOLS(m)+j]==NULL)
        {
          ss=(char *)AllocL(14+strlen(name));
          s[i*MATCOLS(m)+j]=ss;
          ss[0]='\0';
          sprintf(ss,"%s[%d,%d]",name,i+1,j+1);
          if ((i!=MATROWS(m)-1) || (j!=MATCOLS(m)-1))
          {
            strcat(ss,",");
            vl=max(vl,strlen(ss));
          }
        }
        k=strlen(s[i*MATCOLS(m)+j]);
        if (k>l[j]) l[j]=k;
      }
    }
    /* does it fit on a line ? */
    int maxlen=0;
    for(j=MATCOLS(m)-1;j>=0;j--)
    {
      maxlen+=l[j];
    }
    if (maxlen>colmax)
    {
      /* NO, it does not fit, so retry: */
      /* look up the width of all columns, clear very long entriess */
      /* put length in l[col_nr] */
      /* insert names for cleared entries */
      for(j=MATCOLS(m)-1;j>=0;j--)
      {
        for(i=MATROWS(m)-1;i>=0;i--)
        {
          k=strlen(s[i*MATCOLS(m)+j]);
          if (/*strlen(s[i*MATCOLS(m)+j])*/ k > vl)
          {
            FreeL((ADDRESS)s[i*MATCOLS(m)+j]);
            ss=(char *)AllocL(14+strlen(name));
            s[i*MATCOLS(m)+j]=ss;
            ss[0]='\0';
            sprintf(ss,"%s[%d,%d]",name,i+1,j+1);
            if ((i!=MATROWS(m)-1) || (j!=MATCOLS(m)-1))
            {
              strcat(ss,",");
            }
            l[j]=strlen(s[i*MATCOLS(m)+j]);
            if (l[j]>vl)
            {
#ifdef TEST
              PrintS("pagewidth too small in print(matrix)\n");
#endif
              vl=l[j]; /* make large names fit*/
            }
            i=MATROWS(m);
          }
          else
          {
            if (k>l[j]) l[j]=k;
          }
        }
      }
    }
    /*output of the matrix*/
    for(i=0;i<MATROWS(m);i++)
    {
      k=l[0];
      Print("%-*.*s",l[0],l[0],s[i*MATCOLS(m)]);
      FreeL(s[i*MATCOLS(m)]);
      for(j=1;j<MATCOLS(m);j++)
      {
        if (k+l[j]>colmax)
        {
          PrintS("\n  ");
          k=2;
        }
        k+=l[j];
        Print("%-*.*s",l[j],l[j],s[i*MATCOLS(m)+j]);
        FreeL(s[i*MATCOLS(m)+j]);
      }
      PrintLn();
    }
    /* clean up */
    Free((ADDRESS)s,MATCOLS(m)*MATROWS(m)*sizeof(char*));
    Free((ADDRESS)l,MATCOLS(m)*sizeof(int));
  }
}

/*2
* print for: matrix
*/
BOOLEAN jjPRINT_MA(leftv res, leftv u)
{
  matrix m=(matrix)u->Data();
  jjPRINT_MA0(m,u->Fullname());
  return FALSE;
}

/*2
* print for: vector
*/
BOOLEAN jjPRINT_V(leftv res, leftv u)
{
  polyset m=NULL;
  int l,j;
  /*convert into an array of the components*/
  pVec2Polys((poly)u->Data(), &m, &l);
  /*output*/
  PrintS("[");
  j=0;
  loop
  {
    PrintS(pString(m[j]));
    j++;
    if (j<l) PrintS(",");
    else
    {
      PrintS("]\n");
      break;
    }
  }
  /* clean up */
  for(j=l-1;j>=0;j--) pDelete(&m[j]);
  Free((ADDRESS)m,l*sizeof(poly));
  return FALSE;
}

/*2
* dbprint
*/
BOOLEAN jjDBPRINT(leftv res, leftv u)
{
  BOOLEAN print=(printlevel>myynest);
  if ((u->next!=NULL)&&(u->Typ()==INT_CMD))
  {
    print=((int)u->Data()>0);
    u=u->next;
  }
  if (print)
  {
    BOOLEAN r=FALSE;
    leftv h=u;
    leftv hh;
    while (h!=NULL)
    {
      hh=h->next;
      h->next=NULL;
      if (iiExprArith1(res,h,PRINT_CMD)) return TRUE;
      h->next=hh;
      h=hh;
    }
  }
  return FALSE;
}

/*2
* print(...,"format")
*/
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v)
{
/* ==================== betti ======================================== */
  if ((u->Typ()==INTMAT_CMD)&&(strcmp((char *)v->Data(),"betti")==0))
  {
    int i,j;
    intvec * betti=(intvec *)u->Data();
    // head line --------------------------------------------------------
    PrintS("      "); // 6 spaces for no. and :
    for(j=0;j<betti->cols();j++) Print(" %5d",j); // 6 spaces pro column
    PrintS("\n------"); // 6 spaces for no. and :
    for(j=0;j<betti->cols();j++) PrintS("------"); // 6 spaces pro column
    PrintLn();
    // the table --------------------------------------------------------
    for(i=0;i<betti->rows();i++)
    {
      Print("%5d:",i);
      for(j=1;j<=betti->cols();j++)
      {
        Print(" %5d",IMATELEM(*betti,i+1,j));
      }
      PrintLn();
    }
    // sum --------------------------------------------------------------
    PrintS("------"); // 6 spaces for no. and :
    for(j=0;j<betti->cols();j++) PrintS("------"); // 6 spaces pro column
    PrintS("\ntotal:");
    for(j=0;j<betti->cols();j++)
    {
      int s=0;
      for(i=0;i<betti->rows();i++)
      {
        s+=IMATELEM(*betti,i+1,j+1);
      }
      Print(" %5d",s); // 6 spaces pro column
    }
    PrintLn();
    return FALSE;
  }
/* ======================== end betti ================================= */

  return jjPRINT_GEN(res,u);
}
