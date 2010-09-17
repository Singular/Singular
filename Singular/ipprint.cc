/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: interpreter: printing
*/

#include <kernel/mod2.h>
#include <Singular/tok.h>
#include <Singular/ipid.h>
#include <omalloc/omalloc.h>
#include <kernel/febase.h>
#include <kernel/polys.h>
#include <kernel/matpol.h>
#include <Singular/subexpr.h>
#include <kernel/intvec.h>
#include <Singular/ipshell.h>
#include <Singular/ipprint.h>
#include <kernel/ideals.h>
#include <Singular/attrib.h>

/*2
* print for: int, string, poly, vector, ideal
*/
/*2
* print for: intvec
*/
static BOOLEAN ipPrint_INTVEC(leftv u)
{
  intvec *v=(intvec *)u->Data();
  v->show();
  PrintLn();
  return FALSE;
}

/*2
* print for: intmat
*/
static BOOLEAN ipPrint_INTMAT(leftv u)
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
static void ipPrint_MA0(matrix m, const char *name)
{
  if (MATCOLS(m)>0)
  {
    char **s=(char **)omAlloc(MATCOLS(m)*MATROWS(m)*sizeof(char*));
    char *ss;
    int *l=(int *)omAlloc0(MATCOLS(m)*sizeof(int));
    int i,j,k;
    int vl=si_max(colmax/MATCOLS(m),8);

    /* make enough space for the "largest" name*/
    ss=(char *)omAlloc(14+strlen(name));
    sprintf(ss,"%s[%d,%d]",name,MATCOLS(m),MATROWS(m));
    vl=si_max(vl,(int)strlen(ss));
    omFree(ss);

    /* convert all polys to string */
    i=MATCOLS(m)*MATROWS(m)-1;
    ss=pString(m->m[i]);
    if ((int)strlen(ss)>colmax) s[i]=NULL;
    else                        s[i]=omStrDup(ss);
    for(i--;i>=0;i--)
    {
      pString(m->m[i]);
      ss=StringAppendS(",");
      if ((int)strlen(ss)>colmax) s[i]=NULL;
      else                        s[i]=omStrDup(ss);
    }
    /* look up the width of all columns, put it in l[col_nr] */
    /* insert names for very long entries */
    for(i=MATROWS(m)-1;i>=0;i--)
    {
      for(j=MATCOLS(m)-1;j>=0;j--)
      {
        if (s[i*MATCOLS(m)+j]==NULL)
        {
          ss=(char *)omAlloc(14+strlen(name));
          s[i*MATCOLS(m)+j]=ss;
          ss[0]='\0';
          sprintf(ss,"%s[%d,%d]",name,i+1,j+1);
          if ((i!=MATROWS(m)-1) || (j!=MATCOLS(m)-1))
          {
            strcat(ss,",");
            vl=si_max(vl,(int)strlen(ss));
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
            omFree((ADDRESS)s[i*MATCOLS(m)+j]);
            ss=(char *)omAlloc(14+strlen(name));
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
//#ifdef TEST
//              PrintS("pagewidth too small in print(matrix)\n");
//#endif
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
      omFree(s[i*MATCOLS(m)]);
      for(j=1;j<MATCOLS(m);j++)
      {
        if (k+l[j]>colmax)
        {
          PrintS("\n  ");
          k=2;
        }
        k+=l[j];
        Print("%-*.*s",l[j],l[j],s[i*MATCOLS(m)+j]);
        omFree(s[i*MATCOLS(m)+j]);
      }
      PrintLn();
    }
    /* clean up */
    omFreeSize((ADDRESS)s,MATCOLS(m)*MATROWS(m)*sizeof(char*));
    omFreeSize((ADDRESS)l,MATCOLS(m)*sizeof(int));
  }
}

/*2
* print for: matrix
*/
static BOOLEAN ipPrint_MA(leftv u)
{
  matrix m=(matrix)u->Data();
  ipPrint_MA0(m,u->Name());
  return FALSE;
}

/*2
* print for: vector
*/
static BOOLEAN ipPrint_V(leftv u)
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
  omFreeSize((ADDRESS)m,l*sizeof(poly));
  return FALSE;
}

BOOLEAN jjPRINT(leftv res, leftv u)
{
  switch(u->Typ())
  {
      case INTVEC_CMD:
        return ipPrint_INTVEC(u);

      case INTMAT_CMD:
        return ipPrint_INTMAT(u);

      case MATRIX_CMD:
        return ipPrint_MA(u);

      case IDEAL_CMD:
      {
        char* s = u->String(NULL, FALSE, 2);
        PrintS(s);
        PrintLn();
        omFree(s);
        return FALSE;
      }

      case MODUL_CMD:
      {
        matrix m = idModule2Matrix(idCopy((ideal) u->Data()));
        ipPrint_MA0(m, u->Name());
        idDelete((ideal *) &m);
        return FALSE;
      }

      case VECTOR_CMD:
        return ipPrint_V(u);

      default:
        u->Print();
        return FALSE;
  }
}


/*2
* dbprint
*/
BOOLEAN jjDBPRINT(leftv res, leftv u)
{
  BOOLEAN print=(printlevel>myynest);
  if ((u->next!=NULL)&&(u->Typ()==INT_CMD))
  {
    print=  (((int)((long)(u->Data()))) > 0);
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
      if (jjPRINT(res, h)) return TRUE;
      h->next=hh;
      h=hh;
    }
  }
  return FALSE;
}

static void ipPrintBetti(leftv u)
{
  int i,j;
  int row_shift=(int)((long)(atGet(u,"rowShift",INT_CMD)));
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
    Print("%5d:",i+row_shift);
    for(j=1;j<=betti->cols();j++)
    {
      int m=IMATELEM(*betti,i+1,j);
      if (m==0)
        PrintS("     -");
      else
        Print(" %5d",m);
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
}


/*2
* print(...,"format")
*/
BOOLEAN jjPRINT_FORMAT(leftv res, leftv u, leftv v)
{
/* ==================== betti ======================================== */
  if ((u->Typ()==INTMAT_CMD)&&(strcmp((char *)v->Data(),"betti")==0))
  {
    ipPrintBetti(u);
    res->data = NULL;
    res->rtyp = NONE;
    return FALSE;
  }
/* ======================== end betti ================================= */

  char* ns = omStrDup((char*) v->Data());
  int dim = 1;
  if (strlen(ns) == 3 && ns[1] == '2')
  {
    dim = 2;
    ns[1] = ns[2];
    ns[2] = '\0';
  }
  if (strcmp(ns,"%l") == 0)
  {
    res->data = (char*) u->String(NULL, TRUE, dim);
    if (dim == 2)
    {
      char* ns = (char*) omAlloc(strlen((char*) res->data) + 2);
      strcpy(ns, (char*) res->data);
      omFree(res->data);
      strcat(ns, "\n");
      res->data = ns;
    }
  }
  else if (strcmp(ns,"%t") == 0)
  {
    SPrintStart();
    if (u->rtyp==IDHDL)
      type_cmd((idhdl) (u->data));
    else
      type_cmd((idhdl) u);
    res->data = SPrintEnd();
    if (dim != 2)
      ((char*)res->data)[strlen((char*)res->data) -1] = '\0';
  }
  else if (strcmp(ns,"%;") == 0)
  {
    SPrintStart();
    u->Print();
    if (dim == 2) PrintLn();
    res->data = SPrintEnd();
  }
  else if  (strcmp(ns,"%p") == 0)
  {
    SPrintStart();
    iiExprArith1(res, u, PRINT_CMD);
    if (dim == 2) PrintLn();
    res->data = SPrintEnd();
  }
  else if (strcmp(ns,"%b") == 0 && (u->Typ()==INTMAT_CMD))
  {
    SPrintStart();
    ipPrintBetti(u);
    if (dim == 2) PrintLn();
    res->data = SPrintEnd();
  }
  else
  {
    res->data = u->String(NULL, FALSE, dim);
    if (dim == 2)
    {
      char* ns = (char*) omAlloc(strlen((char*) res->data) + 2);
      strcpy(ns, (char*) res->data);
      omFree(res->data);
      strcat(ns, "\n");
      res->data = ns;
    }
  }

  omFree(ns);
  res->rtyp = STRING_CMD;
  return FALSE;
}
