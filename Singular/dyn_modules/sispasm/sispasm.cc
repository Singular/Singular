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

spasm* conv_matrix2spasm(matrix M, const ring R)
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

spasm* conv_smatrix2spasm(ideal M, const ring R)
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

matrix conv_spasm2matrix(spasm *A, const ring R)
{
  matrix M=mpNew(A->n,A->m);
  int n=A->m;
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

ideal conv_spasm2smatrix(spasm *A, const ring R)
{
  ideal M=idInit(A->m,A->n);
  int n=A->m;
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

spasm* sp_kernel(spasm* A, const ring R)
{ /*from kernel.c*/
  int n = A->n;
  int m = A->m;
  int*  p = (int*)spasm_malloc(n * sizeof(int));
  int * qinv = (int*)spasm_malloc(m * sizeof(int));
  spasm_find_pivots(A, p, qinv);  /* this does some useless stuff, but
                                   * pushes zero rows to the bottom */
  spasm* A_clean = spasm_permute(A, p, SPASM_IDENTITY_PERMUTATION, SPASM_WITH_NUMERICAL_VALUES);
  spasm_csr_free(A);
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
  free(p);
  free(qinv);
  spasm_csr_free(A_t);
  return K;
}

spasm* sp_rref(spasm* A)
{ /* from rref_gplu.c: compute an echelonized form, WITHOUT COLUMN PERMUTATION */
  spasm_lu *LU = spasm_LU(A, SPASM_IDENTITY_PERMUTATION, 1);
  spasm *U = spasm_transpose(LU->L, 1);
  spasm_make_pivots_unitary(U, SPASM_IDENTITY_PERMUTATION, U->n);
  spasm_free_LU(LU);
  return U;
}

/*----------------------------------------------------------------*/
VAR int SPASM_CMD;

static void* sp_Init(blackbox* /*b*/)
{
  spasm_triplet *T = spasm_triplet_alloc(0, 0, 1, currRing->cf->ch, 1);
  spasm* A=spasm_compress(T);
  spasm_triplet_free(T);
  return (void*)A;
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
  sprintf(buf,"spasm matrix %dx%d",A->n,A->m);
  return omStrDup(buf);
}
static void* sp_Copy(blackbox* /*b*/, void *d)
{
 if (d!=NULL)
 {
   spasm* A=(spasm*)d;
   spasm* B=spasm_submatrix(A,0,A->m,0,A->m,1);
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

  if (r->Typ()==l->Typ())
  {
    A=(spasm*)sp_Copy(NULL,r->Data());
  }
  else if (r->Typ()==SMATRIX_CMD)
  {
    A=conv_smatrix2spasm((ideal)r->Data(),currRing);
  }
  else if (r->Typ()==MATRIX_CMD)
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
    res->data=(void*)sp_rref((spasm*)u->Data(),currRing);
    return FALSE;
  }
  return TRUE;
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
