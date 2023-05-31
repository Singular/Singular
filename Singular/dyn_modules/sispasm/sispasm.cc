/*
 * provides (after defintion of a ring with coeffs in Z/p)
 * - type spasm
 * - assignment smatrix,module ->spasm, matrix ->spasm
 * - printing/string(spasm)
 * - transpose(spasm) -> spasm
 * - nrows(spasm) -> int
 * - ncols(spasm) -> int
 * - to_matrix(spams) -> matrix
 * - to_smatrix(spasm) -> smatrix
 * - spasm_kernel(spasm)->spasm
 * - spasm_rref(spasm) -> spasm
 * - <spasm>[<int>,<int>] -> number: reading an entry (get_spasm_entry)
*/
#include "singularconfig.h"
#include "libpolys/polys/monomials/monomials.h"
#include "kernel/ideals.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/blackbox.h"
#include "Singular/mod_lib.h"
#ifdef HAVE_SPASM_H
extern "C"
{
#include "spasm.h"
}

static spasm* conv_matrix2spasm(matrix M, const ring R)
{
  int i=MATROWS(M);
  int j=MATCOLS(M);
  spasm_triplet *T = spasm_triplet_alloc(i, j, 1, R->cf->ch, 1);
  for (int ii=1;ii<=i;ii++)
  {
    for(int jj=1;jj<=j;jj++)
    {
      poly p;
      if ((p=MATELEM(M,ii,jj))!=NULL)
      {
        spasm_add_entry(T,ii-1,jj-1,(spasm_GFp)(long)pGetCoeff(p));
      }
    }
  }
  spasm* A=spasm_compress(T);
  spasm_triplet_free(T);
  return A;
}

static spasm* conv_smatrix2spasm(ideal M, const ring R)
{
  int i=MATROWS((matrix)M);
  int j=MATCOLS((matrix)M);
  spasm_triplet *T = spasm_triplet_alloc(i, j, 1, R->cf->ch, 1);
  for(int jj=0;jj<j;jj++)
  {
    poly p=M->m[jj];
    while (p!=NULL)
    {
      int ii=p_GetComp(p,R);
      spasm_add_entry(T,ii-1,jj,(spasm_GFp)(long)pGetCoeff(p));
      pIter(p);
    }
  }
  spasm* A=spasm_compress(T);
  spasm_triplet_free(T);
  return A;
}

static matrix conv_spasm2matrix(spasm *A, const ring R)
{
  matrix M=mpNew(A->n,A->m);
  int n=A->n;
  int *Aj = A->j;
  int *Ap = A->p;
  spasm_GFp *Ax = A->x;
  for (int i = 0; i < n; i++)
  {
    for (int px = Ap[i]; px < Ap[i + 1]; px++)
    {
      spasm_GFp x = (Ax != NULL) ? Ax[px] : 1;
      MATELEM(M,i+1,Aj[px] + 1)=p_ISet(x,R);
    }
  }
  return M;
}

static ideal conv_spasm2smatrix(spasm *A, const ring R)
{
  ideal M=idInit(A->m,A->n);
  int n=A->n;
  int *Aj = A->j;
  int *Ap = A->p;
  spasm_GFp *Ax = A->x;
  for (int i = 0; i < n; i++)
  {
    for (int px = Ap[i]; px < Ap[i + 1]; px++)
    {
      spasm_GFp x = (Ax != NULL) ? Ax[px] : 1;
      poly p=p_ISet(x,R);
      p_SetComp(p,i+1,R);p_SetmComp(p,R);
      M->m[Aj[px]]=p_Add_q(M->m[Aj[px]],p,R);
    }
  }
  return M;
}

static number get_spasm_entry(spasm *A, int i, int j, const ring R)
{
  matrix M=mpNew(A->n,A->m);
  int n=A->n;
  int *Aj = A->j;
  int *Ap = A->p;
  i--;j--;
  spasm_GFp *Ax = A->x;
  if (i<n)
  {
    for (int px = Ap[i]; px < Ap[i + 1]; px++)
    {
      spasm_GFp x = (Ax != NULL) ? Ax[px] : 1;
      if (j==Aj[px])
        return n_Init(x,R->cf);
    }
  }
  return  n_Init(0,R->cf);;
}

static spasm* sp_kernel(spasm* A, const ring R)
{
  int n = A->n;
  int m = A->m;
  int*  p = (int*)spasm_malloc(n * sizeof(int));
  int * qinv = (int*)spasm_malloc(m * sizeof(int));
  spasm_find_pivots(A, p, qinv);  /* this does some useless stuff, but
                                   * pushes zero rows to the bottom */
#if 0
  /*from kernel.c*/
  spasm* A_clean = spasm_permute(A, p, SPASM_IDENTITY_PERMUTATION, SPASM_WITH_NUMERICAL_VALUES);
  A = A_clean;
  for (int i = 0; i < n; i++)
  {
    if (spasm_row_weight(A, i) == 0)
    {
      //fprintf(stderr, "[kernel] ignoring %d empty rows\n", n - i);
      A->n = i;
      n = i;
      break;
    }
  }

  spasm* A_t = spasm_transpose(A, SPASM_WITH_NUMERICAL_VALUES);
  spasm_find_pivots(A_t, qinv, p);

  spasm* K = spasm_kernel(A_t, qinv);
  spasm_csr_free(A_t);
#else
  spasm* K = spasm_kernel(A, p);
#endif
  free(p);
  free(qinv);
  return K;
}

static spasm* sp_rref(spasm* A)
{ /* from rref_gplu.c: compute an echelonized form, WITHOUT COLUMN PERMUTATION */
  spasm_lu *LU = spasm_LU(A, SPASM_IDENTITY_PERMUTATION, 1);
  spasm *U = spasm_transpose(LU->L, 1);
  spasm_make_pivots_unitary(U, SPASM_IDENTITY_PERMUTATION, U->n);
  spasm_free_LU(LU);
  return U;
}

static spasm* sp_Mult_v(spasm* A, int *v)
{
  int *y=(int*)omAlloc0(A->n*sizeof(int));
  spasm *AA=spasm_submatrix(A,0,A->n,0,A->m,1); /*copy A*/
  spasm_gaxpy(AA,v,y);
  return AA;
}
/*----------------------------------------------------------------*/
VAR int SPASM_CMD;

static void* sp_Init(blackbox* /*b*/)
{
  if ((currRing!=NULL)&&(rField_is_Zp(currRing)))
  {
    spasm_triplet *T = spasm_triplet_alloc(0, 0, 1, currRing->cf->ch, 1);
    spasm* A=spasm_compress(T);
    spasm_triplet_free(T);
    return (void*)A;
  }
  else
  {
    WerrorS("ring with Z/p coeffs required");
    return NULL;
  }
}
static void sp_destroy(blackbox* /*b*/, void *d)
{
  if (d!=NULL) spasm_csr_free((spasm*)d);
  d=NULL;
}
static char* sp_String(blackbox* /*b*/, void *d)
{
  char buf[30];
  spasm* A=(spasm*)d;
  snprintf(buf,30, "spasm matrix %dx%d",A->n,A->m);
  return omStrDup(buf);
}
static void* sp_Copy(blackbox* /*b*/, void *d)
{
 if (d!=NULL)
 {
   spasm* A=(spasm*)d;
   spasm* B=spasm_submatrix(A,0,A->n,0,A->m,1);
   return (void*)B;
 }
 return NULL;
}
static BOOLEAN sp_Assign(leftv l, leftv r)
{
  spasm* A;
  void*d=l->Data();
  if (d!=NULL) spasm_csr_free((spasm*)d);
  if (l->rtyp==IDHDL)
  {
    IDDATA((idhdl)l->data) = (char*)NULL;
  }
  else
  {
    l->data = (void*)NULL;
  }
  int rt=r->Typ();

  if (rt==l->Typ())
  {
    A=(spasm*)sp_Copy(NULL,r->Data());
  }
  else if ((rt==SMATRIX_CMD)||(rt==MODUL_CMD))
  {
    A=conv_smatrix2spasm((ideal)r->Data(),currRing);
  }
  else if (rt==MATRIX_CMD)
  {
    A=conv_matrix2spasm((matrix)r->Data(),currRing);
  }
  else
    return TRUE;

  if (l->rtyp==IDHDL)
  {
    IDDATA((idhdl)l->data) = (char*)A;
  }
  else
  {
    l->data = (void*)A;
  }
  return FALSE;
}

static BOOLEAN to_smatrix(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==SPASM_CMD))
  {
    res->rtyp=SMATRIX_CMD;
    res->data=(void*)conv_spasm2smatrix((spasm*)u->Data(),currRing);
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN to_matrix(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==SPASM_CMD))
  {
    res->rtyp=MATRIX_CMD;
    res->data=(void*)conv_spasm2matrix((spasm*)u->Data(),currRing);
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN kernel(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==SPASM_CMD))
  {
    res->rtyp=SPASM_CMD;
    res->data=(void*)sp_kernel((spasm*)u->Data(),currRing);
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN rref(leftv res, leftv args)
{
  leftv u = args;
  if ((u!=NULL) && (u->Typ()==SPASM_CMD))
  {
    res->rtyp=SPASM_CMD;
    res->data=(void*)sp_rref((spasm*)u->Data());
    return FALSE;
  }
  return TRUE;
}
static BOOLEAN sp_Op1(int op,leftv res, leftv arg)
{
  if(op==TRANSPOSE_CMD)
  {
    res->rtyp=arg->Typ();
    res->data=(void*)spasm_transpose((spasm*)arg->Data(),SPASM_WITH_NUMERICAL_VALUES);
    return FALSE;
  }
  else if (op==COLS_CMD)
  {
    spasm* A=(spasm*)arg->Data();
    res->rtyp=INT_CMD;
    res->data=(void*)(long)A->m;
    return FALSE;
  }
  else if (op==ROWS_CMD)
  {
    spasm* A=(spasm*)arg->Data();
    res->rtyp=INT_CMD;
    res->data=(void*)(long)A->n;
    return FALSE;
  }
  return  blackboxDefaultOp1(op,res,arg);
}
static BOOLEAN sp_Op3(int op,leftv res, leftv a1, leftv a2, leftv a3)
{
  if ((op=='[')
  && (a2->Typ()==INT_CMD)
  && (a3->Typ()==INT_CMD))
  {
    res->rtyp=NUMBER_CMD;
    res->data=(char*)get_spasm_entry((spasm*)a1->Data(),(int)(long)a2->Data(),
                              (int)(long)a3->Data(),currRing);
    return FALSE;
  }
  return  blackboxDefaultOp3(op,res,a1,a2,a3);
}
/*----------------------------------------------------------------*/
// initialisation of the module
extern "C" int SI_MOD_INIT(sispasm)(SModulFunctions* p)
{
  blackbox *b=(blackbox*)omAlloc0(sizeof(blackbox));
  b->blackbox_destroy=sp_destroy;
  b->blackbox_String=sp_String;
  b->blackbox_Init=sp_Init;
  b->blackbox_Copy=sp_Copy;
  b->blackbox_Assign=sp_Assign;
  b->blackbox_Op1=sp_Op1;
  b->blackbox_Op3=sp_Op3;
  SPASM_CMD=setBlackboxStuff(b,"spasm");
  p->iiAddCproc("spasm.so","spasm_kernel",FALSE,kernel);
  p->iiAddCproc("spasm.so","spasm_rref",FALSE,rref);
  p->iiAddCproc("spasm.so","to_smatrix",FALSE,to_smatrix);
  p->iiAddCproc("spasm.so","to_matrix",FALSE,to_matrix);
  return (MAX_TOK);
}
#else
extern "C" int SI_MOD_INIT(sispasm)(SModulFunctions* psModulFunctions)
{
  PrintS("no spasm support\n");
  return MAX_TOK;
}
#endif
