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
 * - spasm_rref(spasm) -> spasm (legacy SpaSM)
 * - spasm_rref_permuted(spasm) -> spasm (SpaSM 1.3; RREF of A*Q)
 * - spasm_kernel_basis(module) -> module
 * - <spasm>[<int>,<int>] -> number: reading an entry (get_spasm_entry)
*/
#include "singularconfig.h"
#include "libpolys/polys/monomials/monomials.h"
#include "kernel/ideals.h"
#include "Singular/ipid.h"
#include "Singular/ipshell.h"
#include "Singular/blackbox.h"
#include "Singular/mod_lib.h"
#include <stdio.h>
#ifdef _WIN32
#include <io.h>
#define SPASM_DUP _dup
#define SPASM_DUP2 _dup2
#define SPASM_CLOSE _close
#define SPASM_FILENO _fileno
#define SPASM_NULL_DEVICE "NUL"
#else
#include <unistd.h>
#define SPASM_DUP dup
#define SPASM_DUP2 dup2
#define SPASM_CLOSE close
#define SPASM_FILENO fileno
#define SPASM_NULL_DEVICE "/dev/null"
#endif
#ifdef HAVE_SPASM
// SpaSM <=1.2 includes its bundled cycleclock.h from the public header.  That
// header defines an unsigned global int64 typedef which conflicts with
// Singular's signed int64.  The wrapper uses none of the timing helpers, so
// suppress that private header while retaining the legacy matrix API.
#if !defined(HAVE_STRUCT_SPASM_CSR) && !defined(GOOGLE_BASE_CYCLECLOCK_H_)
#define GOOGLE_BASE_CYCLECLOCK_H_
#define SINGULAR_SUPPRESSED_SPASM_CYCLECLOCK
#endif
extern "C"
{
#include "spasm.h"
}
#ifdef SINGULAR_SUPPRESSED_SPASM_CYCLECLOCK
#undef SINGULAR_SUPPRESSED_SPASM_CYCLECLOCK
#undef GOOGLE_BASE_CYCLECLOCK_H_
#endif

// SpaSM 1.3 made the CSR and field element names explicit.  Keep the rest of
// this wrapper source-compatible with the legacy API which Singular used
// originally.
#if defined(HAVE_STRUCT_SPASM_CSR) || defined(SPASM_VERSION)
#define SINGULAR_SPASM_CURRENT_API 1
#endif

#ifdef SINGULAR_SPASM_CURRENT_API
typedef struct spasm_csr spasm;
typedef spasm_ZZp spasm_GFp;
#ifndef SPASM_WITH_NUMERICAL_VALUES
#define SPASM_WITH_NUMERICAL_VALUES 1
#endif

// SpaSM's default rank computation may stop after a randomized completion
// test.  Requesting L disables that early exit and the randomized low-rank
// path, so every remaining row is eliminated and the resulting rank/kernel
// is exact.  (The complete flag is unnecessary because only U and qinv are
// consumed below.)
static spasm_lu* spasm_echelonize_exact(spasm* A)
{
  struct echelonize_opts opts;
  spasm_echelonize_init_opts(&opts);
  opts.L=1;
  return spasm_echelonize(A,&opts);
}
#endif

static BOOLEAN spasm_ring_supported(const ring R)
{
  if ((R==NULL) || (!rField_is_Zp(R)) || (R->cf->ch==2)) return FALSE;
#ifndef SINGULAR_SPASM_CURRENT_API
  // Legacy SpaSM silently substitutes 46337 for larger characteristics.
  if (R->cf->ch>46337) return FALSE;
#endif
  return TRUE;
}

// SpaSM 1.x writes progress reports directly to stderr and has no verbosity
// switch.  The specialized Singular-kernel entry point is an implementation
// detail, so silence only that synchronous call; the public SpaSM blackbox
// operations retain upstream's diagnostics.
class spasm_stderr_silencer
{
private:
  int saved;
  FILE *sink;

public:
  spasm_stderr_silencer(): saved(-1), sink(NULL)
  {
    fflush(stderr);
    saved=SPASM_DUP(SPASM_FILENO(stderr));
    if (saved<0) return;
    sink=fopen(SPASM_NULL_DEVICE,"w");
    if ((sink==NULL) ||
        (SPASM_DUP2(SPASM_FILENO(sink),SPASM_FILENO(stderr))<0))
    {
      if (sink!=NULL) fclose(sink);
      sink=NULL;
      SPASM_CLOSE(saved);
      saved=-1;
    }
  }

  ~spasm_stderr_silencer()
  {
    if (saved>=0)
    {
      fflush(stderr);
      SPASM_DUP2(saved,SPASM_FILENO(stderr));
      SPASM_CLOSE(saved);
    }
    if (sink!=NULL) fclose(sink);
  }
};

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
        if (!p_IsConstant(p,R))
        {
          spasm_triplet_free(T);
          WerrorS("SpaSM matrices must have constant entries");
          return NULL;
        }
        spasm_add_entry(T,ii-1,jj-1,(spasm_GFp)n_Int(pGetCoeff(p),R->cf));
      }
    }
  }
  spasm* A=spasm_compress(T);
  spasm_triplet_free(T);
  return A;
}

static spasm* conv_smatrix2spasm(ideal M, const ring R)
{
  // Modules and sparse matrices store their row count in rank; nrows is 1
  // for this representation.  Keeping the explicit rank also preserves
  // trailing zero rows during a round trip through SpaSM.
  int i=(int)M->rank;
  int j=IDELEMS(M);
  spasm_triplet *T = spasm_triplet_alloc(i, j, 1, R->cf->ch, 1);
  for(int jj=0;jj<j;jj++)
  {
    poly p=M->m[jj];
    while (p!=NULL)
    {
      if (!p_LmIsConstantComp(p,R))
      {
        spasm_triplet_free(T);
        WerrorS("SpaSM matrices must have constant entries");
        return NULL;
      }
      int ii=p_GetComp(p,R);
      spasm_add_entry(T,ii-1,jj,(spasm_GFp)n_Int(pGetCoeff(p),R->cf));
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
  const int *Aj = A->j;
  const auto *Ap = A->p;
  spasm_GFp *Ax = A->x;
  for (int i = 0; i < n; i++)
  {
    for (auto px = Ap[i]; px < Ap[i + 1]; px++)
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
  const int *Aj = A->j;
  const auto *Ap = A->p;
  spasm_GFp *Ax = A->x;
  for (int i = 0; i < n; i++)
  {
    for (auto px = Ap[i]; px < Ap[i + 1]; px++)
    {
      spasm_GFp x = (Ax != NULL) ? Ax[px] : 1;
      poly p=p_ISet(x,R);
      p_SetComp(p,i+1,R);p_SetmComp(p,R);
      M->m[Aj[px]]=p_Add_q(M->m[Aj[px]],p,R);
    }
  }
  return M;
}

// A SpaSM kernel matrix stores one right-kernel vector in each row.  Singular
// modules store generators in columns, so transpose this representation while
// converting it and preserve the full ambient rank when the kernel is zero.
static ideal conv_spasm_kernel2module(spasm *K, const ring R)
{
  ideal M=idInit(K->n,K->m);
  const int *Kj=K->j;
  const auto *Kp=K->p;
  spasm_GFp *Kx=K->x;
  for (int i=0;i<K->n;i++)
  {
    poly v=NULL;
    for (auto px=Kp[i];px<Kp[i+1];px++)
    {
      const spasm_GFp x=(Kx!=NULL) ? Kx[px] : 1;
      poly term=p_ISet(x,R);
      p_SetComp(term,Kj[px]+1,R);
      p_SetmComp(term,R);
      v=p_Add_q(v,term,R);
    }
    M->m[i]=v;
  }
  return M;
}

static number get_spasm_entry(spasm *A, int i, int j, const ring R)
{
  int n=A->n;
  const int *Aj = A->j;
  const auto *Ap = A->p;
  i--;j--;
  spasm_GFp *Ax = A->x;
  if (i<n)
  {
    for (auto px = Ap[i]; px < Ap[i + 1]; px++)
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
#ifdef SINGULAR_SPASM_CURRENT_API
  spasm_lu *LU=spasm_echelonize_exact(A);
  spasm *K=spasm_kernel(LU);
  spasm_lu_free(LU);
  return K;
#else
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
#endif
}

static spasm* sp_rref(spasm* A)
{
#ifdef SINGULAR_SPASM_CURRENT_API
  spasm_lu *LU=spasm_echelonize_exact(A);
  int *qinv=(int*)omAlloc(A->m*sizeof(int));
  spasm *U=spasm_rref(LU,qinv);
  omFreeSize((ADDRESS)qinv,A->m*sizeof(int));
  spasm_lu_free(LU);
  return U;
#else
  /* from rref_gplu.c: compute an echelonized form, WITHOUT COLUMN PERMUTATION */
  spasm_lu *LU = spasm_LU(A, SPASM_IDENTITY_PERMUTATION, 1);
  spasm *U = spasm_transpose(LU->L, 1);
  spasm_make_pivots_unitary(U, SPASM_IDENTITY_PERMUTATION, U->n);
  spasm_free_LU(LU);
  return U;
#endif
}

// Return at most one right-kernel vector as a Singular module generator.
// In the current SpaSM API the rank is available before a kernel basis is
// constructed, so the overwhelmingly common full-column-rank case allocates
// no kernel matrix at all.
static ideal sp_first_kernel_vector(spasm* A, const ring R)
{
  const int columnCount=A->m;
  spasm *K=NULL;
#ifdef SINGULAR_SPASM_CURRENT_API
  spasm_lu *LU=spasm_echelonize_exact(A);
  if (LU->r<columnCount) K=spasm_kernel(LU);
  spasm_lu_free(LU);
#else
  K=sp_kernel(A,R);
#endif

  ideal result=idInit((K!=NULL && K->n>0) ? 1 : 0,columnCount);
  if (K!=NULL && K->n>0)
  {
    poly v=NULL;
    const auto *Kp=K->p;
    const int *Kj=K->j;
    const spasm_GFp *Kx=K->x;
    for (auto px=Kp[0]; px<Kp[1]; px++)
    {
      const spasm_GFp x=(Kx!=NULL) ? Kx[px] : 1;
      poly term=p_ISet(x,R);
      p_SetComp(term,Kj[px]+1,R);
      p_SetmComp(term,R);
      v=p_Add_q(v,term,R);
    }
    result->m[0]=v;
  }
  if (K!=NULL) spasm_csr_free(K);
  return result;
}
/*----------------------------------------------------------------*/
VAR int SPASM_CMD;

static void* sp_Init(blackbox* /*b*/)
{
  if (spasm_ring_supported(currRing))
  {
    spasm_triplet *T = spasm_triplet_alloc(0, 0, 1, currRing->cf->ch, 1);
    spasm* A=spasm_compress(T);
    spasm_triplet_free(T);
    return (void*)A;
  }
  else
  {
    WerrorS("SpaSM requires an odd supported prime field");
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
  snprintf(buf,30,"spasm matrix %dx%d",A->n,A->m);
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
  if (!spasm_ring_supported(currRing))
  {
    WerrorS("SpaSM requires an odd supported prime field");
    return TRUE;
  }
  spasm* A=NULL;
  int rt=r->Typ();

  if (rt==l->Typ())
  {
    if (l->Data()==r->Data()) return FALSE;
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

  if (A==NULL) return TRUE;

  // Conversion can fail (for example on a nonconstant matrix), so replace
  // the destination only after the new SpaSM object has been constructed.
  void*d=l->Data();
  if (d!=NULL) spasm_csr_free((spasm*)d);
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

static BOOLEAN supports_current_ring(leftv res, leftv args)
{
  if (args!=NULL) return TRUE;
  res->rtyp=INT_CMD;
  res->data=(void*)(long)spasm_ring_supported(currRing);
  return FALSE;
}

static BOOLEAN first_kernel_vector(leftv res, leftv args)
{
  if ((args==NULL) || (args->next!=NULL) ||
      ((args->Typ()!=SMATRIX_CMD) && (args->Typ()!=MODUL_CMD)))
    return TRUE;
  if (!spasm_ring_supported(currRing))
  {
    WerrorS("SpaSM requires an odd supported prime field");
    return TRUE;
  }

  spasm_stderr_silencer silence;
  spasm *A=conv_smatrix2spasm((ideal)args->Data(),currRing);
  if (A==NULL) return TRUE;
  ideal K=sp_first_kernel_vector(A,currRing);
  spasm_csr_free(A);
  res->rtyp=MODUL_CMD;
  res->data=(void*)K;
  return FALSE;
}

// Return a complete basis of the right kernel as Singular module generators.
// This is the sparse, exact backend used by sheafSectionModuleSpaSM.  Unlike
// the public blackbox operations, suppress SpaSM's unconditional progress
// diagnostics because this entry point is an implementation detail.
static BOOLEAN kernel_basis(leftv res, leftv args)
{
  if ((args==NULL) || (args->next!=NULL) ||
      ((args->Typ()!=SMATRIX_CMD) && (args->Typ()!=MODUL_CMD)))
    return TRUE;
  if (!spasm_ring_supported(currRing))
  {
    WerrorS("SpaSM requires an odd supported prime field");
    return TRUE;
  }

  spasm_stderr_silencer silence;
  spasm *A=conv_smatrix2spasm((ideal)args->Data(),currRing);
  if (A==NULL) return TRUE;
  const int columnCount=A->m;
  spasm *K=sp_kernel(A,currRing);
  spasm_csr_free(A);
  ideal result;
  if (K==NULL)
  {
    // Legacy SpaSM may represent a full-column-rank kernel by NULL.
    result=idInit(0,columnCount);
  }
  else
  {
    result=conv_spasm_kernel2module(K,currRing);
    spasm_csr_free(K);
  }
  res->rtyp=MODUL_CMD;
  res->data=(void*)result;
  return FALSE;
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
    spasm *A=(spasm*)a1->Data();
    const int i=(int)(long)a2->Data();
    const int j=(int)(long)a3->Data();
    if ((i<1) || (i>A->n) || (j<1) || (j>A->m))
    {
      WerrorS("SpaSM matrix index out of range");
      return TRUE;
    }
    res->rtyp=NUMBER_CMD;
    res->data=(char*)get_spasm_entry(A,i,j,currRing);
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
  p->iiAddCproc("sispasm.so","spasm_kernel",FALSE,kernel);
#ifdef SINGULAR_SPASM_CURRENT_API
  p->iiAddCproc("sispasm.so","spasm_rref_permuted",FALSE,rref);
#else
  p->iiAddCproc("sispasm.so","spasm_rref",FALSE,rref);
#endif
  p->iiAddCproc("sispasm.so","to_smatrix",FALSE,to_smatrix);
  p->iiAddCproc("sispasm.so","to_matrix",FALSE,to_matrix);
  p->iiAddCproc("sispasm.so","spasm_supports_current_ring",FALSE,
                supports_current_ring);
  p->iiAddCproc("sispasm.so","spasm_first_kernel_vector",FALSE,
                first_kernel_vector);
  p->iiAddCproc("sispasm.so","spasm_kernel_basis",FALSE,kernel_basis);
  return (MAX_TOK);
}
#else
extern "C" int SI_MOD_INIT(sispasm)(SModulFunctions* psModulFunctions)
{
  return MAX_TOK;
}
#endif
