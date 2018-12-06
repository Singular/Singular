/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/

/*
* ABSTRACT: operations with sparse matrices (bareiss, ...)
*/

#include "misc/auxiliary.h"

#include "misc/mylimits.h"

#include "misc/options.h"

#include "reporter/reporter.h"
#include "misc/intvec.h"

#include "coeffs/numbers.h"

#include "monomials/ring.h"
#include "monomials/p_polys.h"

#include "simpleideals.h"

#include "sparsmat.h"
#include "prCopy.h"

#include "templates/p_Procs.h"

#include "kbuckets.h"
#include "operations/p_Mult_q.h"

// define SM_NO_BUCKETS, if sparsemat stuff should not use buckets
// #define SM_NO_BUCKETS

// this is also influenced by TEST_OPT_NOTBUCKETS
#ifndef SM_NO_BUCKETS
// buckets do carry a small additional overhead: only use them if
// min-length of polys is >= SM_MIN_LENGTH_BUCKET
#define SM_MIN_LENGTH_BUCKET MIN_LENGTH_BUCKET - 5
#else
#define SM_MIN_LENGTH_BUCKET    MAX_INT_VAL
#endif

typedef struct smprec sm_prec;
typedef sm_prec * smpoly;
struct smprec
{
  smpoly n;            // the next element
  int pos;             // position
  int e;               // level
  poly m;              // the element
  float f;             // complexity of the element
};


/* declare internal 'C' stuff */
static void sm_ExactPolyDiv(poly, poly, const ring);
static BOOLEAN sm_IsNegQuot(poly, const poly, const poly, const ring);
static void sm_ExpMultDiv(poly, const poly, const poly, const ring);
static void sm_PolyDivN(poly, const number, const ring);
static BOOLEAN smSmaller(poly, poly);
static void sm_CombineChain(poly *, poly, const ring);
static void sm_FindRef(poly *, poly *, poly, const ring);

static void sm_ElemDelete(smpoly *, const ring);
static smpoly smElemCopy(smpoly);
static float sm_PolyWeight(smpoly, const ring);
static smpoly sm_Poly2Smpoly(poly, const ring);
static poly sm_Smpoly2Poly(smpoly, const ring);
static BOOLEAN sm_HaveDenom(poly, const ring);
static number sm_Cleardenom(ideal, const ring);

omBin smprec_bin = omGetSpecBin(sizeof(smprec));

static poly pp_Mult_Coeff_mm_DivSelect_MultDiv(poly p, int &lp, poly m,
                                               poly a, poly b, const ring currRing)
{
  if (rOrd_is_Comp_dp(currRing) && currRing->ExpL_Size > 2)
  {
    // pp_Mult_Coeff_mm_DivSelectMult only works for (c/C,dp) and
    // ExpL_Size > 2
    // should be generalized, at least to dp with ExpL_Size == 2
    // (is the case for 1 variable)
    int shorter;
    p = currRing->p_Procs->pp_Mult_Coeff_mm_DivSelectMult(p, m, a, b,
                                                          shorter, currRing);
    lp -= shorter;
  }
  else
  {
    p = pp_Mult_Coeff_mm_DivSelect(p, lp, m, currRing);
    sm_ExpMultDiv(p, a, b, currRing);
  }
  return p;
}

static poly sm_SelectCopy_ExpMultDiv(poly p, poly m, poly a, poly b, const ring currRing)
{
  int lp = 0;
  return pp_Mult_Coeff_mm_DivSelect_MultDiv(p, lp, m, a, b, currRing);
}


/* class for sparse matrix:
*      3 parts of matrix during the algorithm
*      m_act[cols][pos(rows)] => m_row[rows][pos(cols)] => m_res[cols][pos(rows)]
*      input                     pivotcols as rows         result
*      pivot                     like a stack              from pivot and pivotcols
*      elimination                                         rows reordered according
*                                                          to pivot choise
*                                                          stored in perm
*      a step is as follows
*      - search of pivot (smPivot)
*      - swap pivot column and last column (smSwapC)
*        select pivotrow to piv and red (smSelectPR)
*        consider sign
*      - elimination (smInitElim, sm0Elim, sm1Elim)
*        clear zero column as result of elimination (smZeroElim)
*      - tranfer from
*        piv and m_row to m_res (smRowToCol)
*        m_act to m_row (smColToRow)
*/
class sparse_mat{
private:
  int nrows, ncols;    // dimension of the problem
  int sign;            // for determinant (start: 1)
  int act;             // number of unreduced columns (start: ncols)
  int crd;             // number of reduced columns (start: 0)
  int tored;           // border for rows to reduce
  int inred;           // unreducable part
  int rpiv, cpiv;      // position of the pivot
  int normalize;       // Normalization flag
  int *perm;           // permutation of rows
  float wpoints;       // weight of all points
  float *wrw, *wcl;    // weights of rows and columns
  smpoly * m_act;      // unreduced columns
  smpoly * m_res;      // reduced columns (result)
  smpoly * m_row;      // reduced part of rows
  smpoly red;          // row to reduce
  smpoly piv, oldpiv;  // pivot and previous pivot
  smpoly dumm;         // allocated dummy
  ring _R;

  void smColToRow();
  void smRowToCol();
  void smFinalMult();
  void smSparseHomog();
  void smWeights();
  void smPivot();
  void smNewWeights();
  void smNewPivot();
  void smZeroElim();
  void smToredElim();
  void smCopToRes();
  void smSelectPR();
  void sm1Elim();
  void smHElim();
  void smMultCol();
  poly smMultPoly(smpoly);
  void smActDel();
  void smColDel();
  void smPivDel();
  void smSign();
  void smInitPerm();
  int smCheckNormalize();
  void smNormalize();
public:
  sparse_mat(ideal, const ring);
  ~sparse_mat();
  int smGetSign() { return sign; }
  smpoly * smGetAct() { return m_act; }
  int smGetRed() { return tored; }
  ideal smRes2Mod();
  poly smDet();
  void smNewBareiss(int, int);
  void smToIntvec(intvec *);
};

/*
* estimate maximal exponent for det or minors,
* the module m has di vectors and maximal rank ra,
* estimate yields for the tXt minors,
* we have di,ra >= t
*/
static void smMinSelect(long *, int, int);

long sm_ExpBound( ideal m, int di, int ra, int t, const ring currRing)
{
  poly p;
  long kr, kc;
  long *r, *c;
  int al, bl, i, j, k;

  if (ra==0) ra=1;
  al = di*sizeof(long);
  c = (long *)omAlloc(al);
  bl = ra*sizeof(long);
  r = (long *)omAlloc0(bl);
  for (i=di-1;i>=0;i--)
  {
    kc = 0;
    p = m->m[i];
    while(p!=NULL)
    {
      k = p_GetComp(p, currRing)-1;
      kr = r[k];
      for (j=currRing->N;j>0;j--)
      {
        long t=p_GetExp(p,j, currRing);
        if(t /*p_GetExp(p,j, currRing)*/ >kc)
          kc=t; /*p_GetExp(p,j, currRing);*/
        if(t /*p_GetExp(p,j, currRing)s*/ >kr)
          kr=t; /*p_GetExp(p,j, currRing);*/
      }
      r[k] = kr;
      pIter(p);
    }
    c[i] = kc;
  }
  if (t<di) smMinSelect(c, t, di);
  if (t<ra) smMinSelect(r, t, ra);
  kr = kc = 0;
  for (j=t-1;j>=0;j--)
  {
    kr += r[j];
    kc += c[j];
  }
  omFreeSize((ADDRESS)c, al);
  omFreeSize((ADDRESS)r, bl);
  if (kr<kc) kc = kr;
  if (kr<1) kr = 1;
  return kr;
}

static void smMinSelect(long *c, int t, int d)
{
  long m;
  int pos, i;
  do
  {
    d--;
    pos = d;
    m = c[pos];
    for (i=d-1;i>=0;i--)
    {
      if(c[i]<m)
      {
        pos = i;
        m = c[i];
      }
    }
    for (i=pos;i<d;i++) c[i] = c[i+1];
  } while (d>t);
}

/* ----------------- ops with rings ------------------ */
ring sm_RingChange(const ring origR, long bound)
{
//  *origR =currRing;
  ring tmpR=rCopy0(origR,FALSE,FALSE);
  rRingOrder_t *ord=(rRingOrder_t*)omAlloc0(3*sizeof(rRingOrder_t));
  int *block0=(int*)omAlloc0(3*sizeof(int));
  int *block1=(int*)omAlloc0(3*sizeof(int));
  ord[0]=ringorder_c;
  ord[1]=ringorder_dp;
  tmpR->order=ord;
  tmpR->OrdSgn=1;
  block0[1]=1;
  tmpR->block0=block0;
  block1[1]=tmpR->N;
  tmpR->block1=block1;
  tmpR->bitmask = 2*bound;
  tmpR->wvhdl = (int **)omAlloc0((3) * sizeof(int*));

// ???
//  if (tmpR->bitmask > currRing->bitmask) tmpR->bitmask = currRing->bitmask;

  rComplete(tmpR,1);
  if (origR->qideal!=NULL)
  {
    tmpR->qideal= idrCopyR_NoSort(origR->qideal, origR, tmpR);
  }
  if (TEST_OPT_PROT)
    Print("[%ld:%d]", (long) tmpR->bitmask, tmpR->ExpL_Size);
  return tmpR;
}

void sm_KillModifiedRing(ring r)
{
  if (r->qideal!=NULL) id_Delete(&(r->qideal),r);
  for(int i=r->N-1;i>=0;i--) omFree(r->names[i]);
  omFreeSize(r->names,r->N*sizeof(char*));
  rKillModifiedRing(r);
}

/*2
* Bareiss or Chinese remainder ?
* I is dXd
* sw = TRUE  -> I Matrix
*      FALSE -> I Module
* return True  -> change Type
*        FALSE -> same Type
*/
BOOLEAN sm_CheckDet(ideal I, int d, BOOLEAN sw, const ring r)
{
  int s,t,i;
  poly p;

  if (d>100)
    return TRUE;
  if (!rField_is_Q(r))
    return TRUE;
  s = t = 0;
  // now: field is Q, n<=100
  for(i=IDELEMS(I)-1;i>=0;i--)
  {
    p=I->m[i];
    if (p!=NULL)
    {
      if(!p_IsConstant(p,r))
        return TRUE;
      s++;
      t+=n_Size(pGetCoeff(p),r->cf);
    }
  }
  s*=15;
  if (t>s)
    return FALSE;// few large constanst entries
  else
    return TRUE; //many small entries
}

/* ----------------- basics (used from 'C') ------------------ */
/*2
*returns the determinant of the module I;
*uses Bareiss algorithm
*/
poly sm_CallDet(ideal I,const ring R)
{
  if (I->ncols != I->rank)
  {
    Werror("det of %ld x %d module (matrix)",I->rank,I->ncols);
    return NULL;
  }
  int r=id_RankFreeModule(I,R);
  if (I->ncols != r) // some 0-lines at the end
  {
    return NULL;
  }
  long bound=sm_ExpBound(I,r,r,r,R);
  number diag,h=n_Init(1,R->cf);
  poly res;
  ring tmpR;
  sparse_mat *det;
  ideal II;

  tmpR=sm_RingChange(R,bound);
  II = idrCopyR(I, R, tmpR);
  diag = sm_Cleardenom(II,tmpR);
  det = new sparse_mat(II,tmpR);
  id_Delete(&II,tmpR);
  if (det->smGetAct() == NULL)
  {
    delete det;
    sm_KillModifiedRing(tmpR);
    return NULL;
  }
  res=det->smDet();
  if(det->smGetSign()<0) res=p_Neg(res,tmpR);
  delete det;
  res = prMoveR(res, tmpR, R);
  sm_KillModifiedRing(tmpR);
  if (!n_Equal(diag,h,R->cf))
  {
    p_Mult_nn(res,diag,R);
    p_Normalize(res,R);
  }
  n_Delete(&diag,R->cf);
  n_Delete(&h,R->cf);
  return res;
}

void sm_CallBareiss(ideal I, int x, int y, ideal & M, intvec **iv, const ring R)
{
  int r=id_RankFreeModule(I,R),t=r;
  int c=IDELEMS(I),s=c;
  long bound;
  ring tmpR;
  sparse_mat *bareiss;

  if ((x>0) && (x<t))
    t-=x;
  if ((y>1) && (y<s))
    s-=y;
  if (t>s) t=s;
  bound=sm_ExpBound(I,c,r,t,R);
  tmpR=sm_RingChange(R,bound);
  ideal II = idrCopyR(I, R, tmpR);
  bareiss = new sparse_mat(II,tmpR);
  if (bareiss->smGetAct() == NULL)
  {
    delete bareiss;
    *iv=new intvec(1,rVar(tmpR));
  }
  else
  {
    id_Delete(&II,tmpR);
    bareiss->smNewBareiss(x, y);
    II = bareiss->smRes2Mod();
    *iv = new intvec(bareiss->smGetRed());
    bareiss->smToIntvec(*iv);
    delete bareiss;
    II = idrMoveR(II,tmpR,R);
  }
  sm_KillModifiedRing(tmpR);
  M=II;
}

/*
* constructor
*/
sparse_mat::sparse_mat(ideal smat, const ring RR)
{
  int i;
  poly* pmat;
  _R=RR;

  ncols = smat->ncols;
  nrows = id_RankFreeModule(smat,RR);
  if (nrows <= 0)
  {
    m_act = NULL;
    return;
  }
  sign = 1;
  inred = act = ncols;
  crd = 0;
  tored = nrows; // without border
  i = tored+1;
  perm = (int *)omAlloc(sizeof(int)*(i+1));
  perm[i] = 0;
  m_row = (smpoly *)omAlloc0(sizeof(smpoly)*i);
  wrw = (float *)omAlloc(sizeof(float)*i);
  i = ncols+1;
  wcl = (float *)omAlloc(sizeof(float)*i);
  m_act = (smpoly *)omAlloc(sizeof(smpoly)*i);
  m_res = (smpoly *)omAlloc0(sizeof(smpoly)*i);
  dumm = (smpoly)omAllocBin(smprec_bin);
  m_res[0] = (smpoly)omAllocBin(smprec_bin);
  m_res[0]->m = NULL;
  pmat = smat->m;
  for(i=ncols; i; i--)
  {
    m_act[i] = sm_Poly2Smpoly(pmat[i-1], RR);
    pmat[i-1] = NULL;
  }
  this->smZeroElim();
  oldpiv = NULL;
}

/*
* destructor
*/
sparse_mat::~sparse_mat()
{
  int i;
  if (m_act == NULL) return;
  omFreeBin((ADDRESS)m_res[0],  smprec_bin);
  omFreeBin((ADDRESS)dumm,  smprec_bin);
  i = ncols+1;
  omFreeSize((ADDRESS)m_res, sizeof(smpoly)*i);
  omFreeSize((ADDRESS)m_act, sizeof(smpoly)*i);
  omFreeSize((ADDRESS)wcl, sizeof(float)*i);
  i = nrows+1;
  omFreeSize((ADDRESS)wrw, sizeof(float)*i);
  omFreeSize((ADDRESS)m_row, sizeof(smpoly)*i);
  omFreeSize((ADDRESS)perm, sizeof(int)*(i+1));
}

/*
* transform the result to a module
*/

ideal sparse_mat::smRes2Mod()
{
  ideal res = idInit(crd, crd);
  int i;

  for (i=crd; i; i--)
  {
    res->m[i-1] = sm_Smpoly2Poly(m_res[i],_R);
    res->rank=si_max(res->rank, p_MaxComp(res->m[i-1],_R));
  }
  return res;
}

/*
* permutation of rows
*/
void sparse_mat::smToIntvec(intvec *v)
{
  int i;

  for (i=v->rows()-1; i>=0; i--)
    (*v)[i] = perm[i+1];
}
/* ---------------- the algorithm's ------------------ */
/*
* the determinant (up to sign), uses new Bareiss elimination
*/
poly sparse_mat::smDet()
{
  poly res = NULL;

  if (sign == 0)
  {
    this->smActDel();
    return NULL;
  }
  if (act < 2)
  {
    if (act != 0) res = m_act[1]->m;
    omFreeBin((void *)m_act[1],  smprec_bin);
    return res;
  }
  normalize = 0;
  this->smInitPerm();
  this->smPivot();
  this->smSign();
  this->smSelectPR();
  this->sm1Elim();
  crd++;
  m_res[crd] = piv;
  this->smColDel();
  act--;
  this->smZeroElim();
  if (sign == 0)
  {
    this->smActDel();
    return NULL;
  }
  if (act < 2)
  {
    this->smFinalMult();
    this->smPivDel();
    if (act != 0) res = m_act[1]->m;
    omFreeBin((void *)m_act[1],  smprec_bin);
    return res;
  }
  loop
  {
    this->smNewPivot();
    this->smSign();
    this->smSelectPR();
    this->smMultCol();
    this->smHElim();
    crd++;
    m_res[crd] = piv;
    this->smColDel();
    act--;
    this->smZeroElim();
    if (sign == 0)
    {
      this->smPivDel();
      this->smActDel();
      return NULL;
    }
    if (act < 2)
    {
      if (TEST_OPT_PROT) PrintS(".\n");
      this->smFinalMult();
      this->smPivDel();
      if (act != 0) res = m_act[1]->m;
      omFreeBin((void *)m_act[1],  smprec_bin);
      return res;
    }
  }
}

/*
* the new Bareiss elimination:
*   - with x unreduced last rows, pivots from here are not allowed
*   - the method will finish for number of unreduced columns < y
*/
void sparse_mat::smNewBareiss(int x, int y)
{
  if ((x > 0) && (x < nrows))
  {
    tored -= x;
    this->smToredElim();
  }
  if (y < 1) y = 1;
  if (act <= y)
  {
    this->smCopToRes();
    return;
  }
  normalize = this->smCheckNormalize();
  if (normalize) this->smNormalize();
  this->smPivot();
  this->smSelectPR();
  this->sm1Elim();
  crd++;
  this->smColToRow();
  act--;
  this->smRowToCol();
  this->smZeroElim();
  if (tored != nrows)
    this->smToredElim();
  if (act <= y)
  {
    this->smFinalMult();
    this->smCopToRes();
    return;
  }
  loop
  {
    if (normalize) this->smNormalize();
    this->smNewPivot();
    this->smSelectPR();
    this->smMultCol();
    this->smHElim();
    crd++;
    this->smColToRow();
    act--;
    this->smRowToCol();
    this->smZeroElim();
    if (tored != nrows)
      this->smToredElim();
    if (act <= y)
    {
      if (TEST_OPT_PROT) PrintS(".\n");
      this->smFinalMult();
      this->smCopToRes();
      return;
    }
  }
}

/* ----------------- pivot method ------------------ */

/*
* prepare smPivot, compute weights for rows and columns
* and the weight for all points
*/
void sparse_mat::smWeights()
{
  float wc, wp, w;
  smpoly a;
  int i;

  wp = 0.0;
  for (i=tored; i; i--) wrw[i] = 0.0; // ???
  for (i=act; i; i--)
  {
    wc = 0.0;
    a = m_act[i];
    loop
    {
      if (a->pos > tored)
        break;
      w = a->f = sm_PolyWeight(a,_R);
      wc += w;
      wrw[a->pos] += w;
      a = a->n;
      if (a == NULL)
        break;
    }
    wp += wc;
    wcl[i] = wc;
  }
  wpoints = wp;
}

/*
* compute pivot
*/
void sparse_mat::smPivot()
{
  float wopt = 1.0e30;
  float wc, wr, wp, w;
  smpoly a;
  int i, copt, ropt;

  this->smWeights();
  for (i=act; i; i--)
  {
    a = m_act[i];
    loop
    {
      if (a->pos > tored)
        break;
      w = a->f;
      wc = wcl[i]-w;
      wr = wrw[a->pos]-w;
      if ((wr<0.25) || (wc<0.25)) // row or column with only one point
      {
        if (w<wopt)
        {
          wopt = w;
          copt = i;
          ropt = a->pos;
        }
      }
      else // elimination
      {
        wp = w*(wpoints-wcl[i]-wr);
        wp += wr*wc;
        if (wp < wopt)
        {
          wopt = wp;
          copt = i;
          ropt = a->pos;
        }
      }
      a = a->n;
      if (a == NULL)
        break;
    }
  }
  rpiv = ropt;
  cpiv = copt;
  if (cpiv != act)
  {
    a = m_act[act];
    m_act[act] = m_act[cpiv];
    m_act[cpiv] = a;
  }
}

/*
* prepare smPivot, compute weights for rows and columns
* and the weight for all points
*/
void sparse_mat::smNewWeights()
{
  float wc, wp, w, hp = piv->f;
  smpoly a;
  int i, f, e = crd;

  wp = 0.0;
  for (i=tored; i; i--) wrw[i] = 0.0; // ???
  for (i=act; i; i--)
  {
    wc = 0.0;
    a = m_act[i];
    loop
    {
      if (a->pos > tored)
        break;
      w = a->f;
      f = a->e;
      if (f < e)
      {
        w *= hp;
        if (f) w /= m_res[f]->f;
      }
      wc += w;
      wrw[a->pos] += w;
      a = a->n;
      if (a == NULL)
        break;
    }
    wp += wc;
    wcl[i] = wc;
  }
  wpoints = wp;
}

/*
* compute pivot
*/
void sparse_mat::smNewPivot()
{
  float wopt = 1.0e30, hp = piv->f;
  float wc, wr, wp, w;
  smpoly a;
  int i, copt, ropt, f, e = crd;

  this->smNewWeights();
  for (i=act; i; i--)
  {
    a = m_act[i];
    loop
    {
      if (a->pos > tored)
        break;
      w = a->f;
      f = a->e;
      if (f < e)
      {
        w *= hp;
        if (f) w /= m_res[f]->f;
      }
      wc = wcl[i]-w;
      wr = wrw[a->pos]-w;
      if ((wr<0.25) || (wc<0.25)) // row or column with only one point
      {
        if (w<wopt)
        {
          wopt = w;
          copt = i;
          ropt = a->pos;
        }
      }
      else // elimination
      {
        wp = w*(wpoints-wcl[i]-wr);
        wp += wr*wc;
        if (wp < wopt)
        {
          wopt = wp;
          copt = i;
          ropt = a->pos;
        }
      }
      a = a->n;
      if (a == NULL)
        break;
    }
  }
  rpiv = ropt;
  cpiv = copt;
  if (cpiv != act)
  {
    a = m_act[act];
    m_act[act] = m_act[cpiv];
    m_act[cpiv] = a;
  }
}

/* ----------------- elimination ------------------ */

/* first step of elimination */
void sparse_mat::sm1Elim()
{
  poly p = piv->m;        // pivotelement
  smpoly c = m_act[act];  // pivotcolumn
  smpoly r = red;         // row to reduce
  smpoly res, a, b;
  poly w, ha, hb;

  if ((c == NULL) || (r == NULL))
  {
    while (r!=NULL) sm_ElemDelete(&r,_R);
    return;
  }
  do
  {
    a = m_act[r->pos];
    res = dumm;
    res->n = NULL;
    b = c;
    w = r->m;
    loop                  // combine the chains a and b: p*a + w*b
    {
      if (a == NULL)
      {
        do
        {
          res = res->n = smElemCopy(b);
          res->m = pp_Mult_qq(b->m, w,_R);
          res->e = 1;
          res->f = sm_PolyWeight(res,_R);
          b = b->n;
        } while (b != NULL);
        break;
      }
      if (a->pos < b->pos)
      {
        res = res->n = a;
        a = a->n;
      }
      else if (a->pos > b->pos)
      {
        res = res->n = smElemCopy(b);
        res->m = pp_Mult_qq(b->m, w,_R);
        res->e = 1;
        res->f = sm_PolyWeight(res,_R);
        b = b->n;
      }
      else
      {
        ha = pp_Mult_qq(a->m, p,_R);
        p_Delete(&a->m,_R);
        hb = pp_Mult_qq(b->m, w,_R);
        ha = p_Add_q(ha, hb,_R);
        if (ha != NULL)
        {
          a->m = ha;
          a->e = 1;
          a->f = sm_PolyWeight(a,_R);
          res = res->n = a;
          a = a->n;
        }
        else
        {
          sm_ElemDelete(&a,_R);
        }
        b = b->n;
      }
      if (b == NULL)
      {
        res->n = a;
        break;
      }
    }
    m_act[r->pos] = dumm->n;
    sm_ElemDelete(&r,_R);
  } while (r != NULL);
}

/* higher steps of elimination */
void sparse_mat::smHElim()
{
  poly hp = this->smMultPoly(piv);
  poly gp = piv->m;       // pivotelement
  smpoly c = m_act[act];  // pivotcolumn
  smpoly r = red;         // row to reduce
  smpoly res, a, b;
  poly ha, hr, x, y;
  int e, ip, ir, ia;

  if ((c == NULL) || (r == NULL))
  {
    while(r!=NULL) sm_ElemDelete(&r,_R);
    p_Delete(&hp,_R);
    return;
  }
  e = crd+1;
  ip = piv->e;
  do
  {
    a = m_act[r->pos];
    res = dumm;
    res->n = NULL;
    b = c;
    hr = r->m;
    ir = r->e;
    loop                  // combine the chains a and b: (hp,gp)*a(l) + hr*b(h)
    {
      if (a == NULL)
      {
        do
        {
          res = res->n = smElemCopy(b);
          x = SM_MULT(b->m, hr, m_res[ir]->m,_R);
          b = b->n;
          if(ir) SM_DIV(x, m_res[ir]->m,_R);
          res->m = x;
          res->e = e;
          res->f = sm_PolyWeight(res,_R);
        } while (b != NULL);
        break;
      }
      if (a->pos < b->pos)
      {
        res = res->n = a;
        a = a->n;
      }
      else if (a->pos > b->pos)
      {
        res = res->n = smElemCopy(b);
        x = SM_MULT(b->m, hr, m_res[ir]->m,_R);
        b = b->n;
        if(ir) SM_DIV(x, m_res[ir]->m,_R);
        res->m = x;
        res->e = e;
        res->f = sm_PolyWeight(res,_R);
      }
      else
      {
        ha = a->m;
        ia = a->e;
        if (ir >= ia)
        {
          if (ir > ia)
          {
            x = SM_MULT(ha, m_res[ir]->m, m_res[ia]->m,_R);
            p_Delete(&ha,_R);
            ha = x;
            if (ia) SM_DIV(ha, m_res[ia]->m,_R);
            ia = ir;
          }
          x = SM_MULT(ha, gp, m_res[ia]->m,_R);
          p_Delete(&ha,_R);
          y = SM_MULT(b->m, hr, m_res[ia]->m,_R);
        }
        else if (ir >= ip)
        {
          if (ia < crd)
          {
            x = SM_MULT(ha, m_res[crd]->m, m_res[ia]->m,_R);
            p_Delete(&ha,_R);
            ha = x;
            SM_DIV(ha, m_res[ia]->m,_R);
          }
          y = hp;
          if(ir > ip)
          {
            y = SM_MULT(y, m_res[ir]->m, m_res[ip]->m,_R);
            if (ip) SM_DIV(y, m_res[ip]->m,_R);
          }
          ia = ir;
          x = SM_MULT(ha, y, m_res[ia]->m,_R);
          if (y != hp) p_Delete(&y,_R);
          p_Delete(&ha,_R);
          y = SM_MULT(b->m, hr, m_res[ia]->m,_R);
        }
        else
        {
          x = SM_MULT(hr, m_res[ia]->m, m_res[ir]->m,_R);
          if (ir) SM_DIV(x, m_res[ir]->m,_R);
          y = SM_MULT(b->m, x, m_res[ia]->m,_R);
          p_Delete(&x,_R);
          x = SM_MULT(ha, gp, m_res[ia]->m,_R);
          p_Delete(&ha,_R);
        }
        ha = p_Add_q(x, y,_R);
        if (ha != NULL)
        {
          if (ia) SM_DIV(ha, m_res[ia]->m,_R);
          a->m = ha;
          a->e = e;
          a->f = sm_PolyWeight(a,_R);
          res = res->n = a;
          a = a->n;
        }
        else
        {
          a->m = NULL;
          sm_ElemDelete(&a,_R);
        }
        b = b->n;
      }
      if (b == NULL)
      {
        res->n = a;
        break;
      }
    }
    m_act[r->pos] = dumm->n;
    sm_ElemDelete(&r,_R);
  } while (r != NULL);
  p_Delete(&hp,_R);
}

/* ----------------- transfer ------------------ */

/*
* select the pivotrow and store it to red and piv
*/
void sparse_mat::smSelectPR()
{
  smpoly b = dumm;
  smpoly a, ap;
  int i;

  if (TEST_OPT_PROT)
  {
    if ((crd+1)%10)
      PrintS(".");
    else
      PrintS(".\n");
  }
  a = m_act[act];
  if (a->pos < rpiv)
  {
    do
    {
      ap = a;
      a = a->n;
    } while (a->pos < rpiv);
    ap->n = a->n;
  }
  else
    m_act[act] = a->n;
  piv = a;
  a->n = NULL;
  for (i=1; i<act; i++)
  {
    a = m_act[i];
    if (a->pos < rpiv)
    {
      loop
      {
        ap = a;
        a = a->n;
        if ((a == NULL) || (a->pos > rpiv))
          break;
        if (a->pos == rpiv)
        {
          ap->n = a->n;
          a->m = p_Neg(a->m,_R);
          b = b->n = a;
          b->pos = i;
          break;
        }
      }
    }
    else if (a->pos == rpiv)
    {
      m_act[i] = a->n;
      a->m = p_Neg(a->m,_R);
      b = b->n = a;
      b->pos = i;
    }
  }
  b->n = NULL;
  red = dumm->n;
}

/*
* store the pivotcol in m_row
*   m_act[cols][pos(rows)] => m_row[rows][pos(cols)]
*/
void sparse_mat::smColToRow()
{
  smpoly c = m_act[act];
  smpoly h;

  while (c != NULL)
  {
    h = c;
    c = c->n;
    h->n = m_row[h->pos];
    m_row[h->pos] = h;
    h->pos = crd;
  }
}

/*
* store the pivot and the assosiated row in m_row
* to m_res (result):
*   piv + m_row[rows][pos(cols)] => m_res[cols][pos(rows)]
*/
void sparse_mat::smRowToCol()
{
  smpoly r = m_row[rpiv];
  smpoly a, ap, h;

  m_row[rpiv] = NULL;
  perm[crd] = rpiv;
  piv->pos = crd;
  m_res[crd] = piv;
  while (r != NULL)
  {
    ap = m_res[r->pos];
    loop
    {
      a = ap->n;
      if (a == NULL)
      {
        ap->n = h = r;
        r = r->n;
        h->n = a;
        h->pos = crd;
        break;
      }
      ap = a;
    }
  }
}

/* ----------------- C++ stuff ------------------ */

/*
*  clean m_act from zeros (after elim)
*/
void sparse_mat::smZeroElim()
{
  int i = 0;
  int j;

  loop
  {
    i++;
    if (i > act) return;
    if (m_act[i] == NULL) break;
  }
  j = i;
  loop
  {
    j++;
    if (j > act) break;
    if (m_act[j] != NULL)
    {
      m_act[i] = m_act[j];
      i++;
    }
  }
  act -= (j-i);
  sign = 0;
}

/*
*  clean m_act from cols not to reduced (after elim)
*  put them to m_res
*/
void sparse_mat::smToredElim()
{
  int i = 0;
  int j;

  loop
  {
    i++;
    if (i > act) return;
    if (m_act[i]->pos > tored)
    {
      m_res[inred] = m_act[i];
      inred--;
      break;
    }
  }
  j = i;
  loop
  {
    j++;
    if (j > act) break;
    if (m_act[j]->pos > tored)
    {
      m_res[inred] = m_act[j];
      inred--;
    }
    else
    {
      m_act[i] = m_act[j];
      i++;
    }
  }
  act -= (j-i);
  sign = 0;
}

/*
*  copy m_act to m_res
*/
void sparse_mat::smCopToRes()
{
  smpoly a,ap,r,h;
  int i,j,k,l;

  i = 0;
  if (act)
  {
    a = m_act[act]; // init perm
    do
    {
      i++;
      perm[crd+i] = a->pos;
      a = a->n;
    } while ((a != NULL) && (a->pos <= tored));
    for (j=act-1;j;j--) // load all positions of perm
    {
      a = m_act[j];
      k = 1;
      loop
      {
        if (perm[crd+k] >= a->pos)
        {
          if (perm[crd+k] > a->pos)
          {
            for (l=i;l>=k;l--) perm[crd+l+1] = perm[crd+l];
            perm[crd+k] = a->pos;
            i++;
          }
          a = a->n;
          if ((a == NULL) || (a->pos > tored)) break;
        }
        k++;
        if ((k > i) && (a->pos <= tored))
        {
          do
          {
            i++;
            perm[crd+i] = a->pos;
            a = a->n;
          } while ((a != NULL) && (a->pos <= tored));
          break;
        }
      }
    }
  }
  for (j=act;j;j--) // renumber m_act
  {
    k = 1;
    a = m_act[j];
    while ((a != NULL) && (a->pos <= tored))
    {
      if (perm[crd+k] == a->pos)
      {
        a->pos = crd+k;
        a = a->n;
      }
      k++;
    }
  }
  tored = crd+i;
  for(k=1;k<=i;k++) // clean this from m_row
  {
    j = perm[crd+k];
    if (m_row[j] != NULL)
    {
      r = m_row[j];
      m_row[j] = NULL;
      do
      {
        ap = m_res[r->pos];
        loop
        {
          a = ap->n;
          if (a == NULL)
          {
            h = ap->n = r;
            r = r->n;
            h->n = NULL;
            h->pos = crd+k;
            break;
          }
          ap = a;
        }
      } while (r!=NULL);
    }
  }
  while(act) // clean m_act
  {
    crd++;
    m_res[crd] = m_act[act];
    act--;
  }
  for (i=1;i<=tored;i++) // take the rest of m_row
  {
    if(m_row[i] != NULL)
    {
      tored++;
      r = m_row[i];
      m_row[i] = NULL;
      perm[tored] = i;
      do
      {
        ap = m_res[r->pos];
        loop
        {
          a = ap->n;
          if (a == NULL)
          {
            h = ap->n = r;
            r = r->n;
            h->n = NULL;
            h->pos = tored;
            break;
          }
          ap = a;
        }
      } while (r!=NULL);
    }
  }
  for (i=tored+1;i<=nrows;i++) // take the rest of m_row
  {
    if(m_row[i] != NULL)
    {
      r = m_row[i];
      m_row[i] = NULL;
      do
      {
        ap = m_res[r->pos];
        loop
        {
          a = ap->n;
          if (a == NULL)
          {
            h = ap->n = r;
            r = r->n;
            h->n = NULL;
            h->pos = i;
            break;
          }
          ap = a;
        }
      } while (r!=NULL);
    }
  }
  while (inred < ncols) // take unreducable
  {
    crd++;
    inred++;
    m_res[crd] = m_res[inred];
  }
}

/*
* multiply and divide the column, that goes to result
*/
void sparse_mat::smMultCol()
{
  smpoly a = m_act[act];
  int e = crd;
  poly ha;
  int f;

  while (a != NULL)
  {
    f = a->e;
    if (f < e)
    {
      ha = SM_MULT(a->m, m_res[e]->m, m_res[f]->m,_R);
      p_Delete(&a->m,_R);
      if (f) SM_DIV(ha, m_res[f]->m,_R);
      a->m = ha;
      if (normalize) p_Normalize(a->m,_R);
    }
    a = a->n;
  }
}

/*
* multiply and divide the m_act finaly
*/
void sparse_mat::smFinalMult()
{
  smpoly a;
  poly ha;
  int i, f;
  int e = crd;

  for (i=act; i; i--)
  {
    a = m_act[i];
    do
    {
      f = a->e;
      if (f < e)
      {
        ha = SM_MULT(a->m, m_res[e]->m, m_res[f]->m, _R);
        p_Delete(&a->m,_R);
        if (f) SM_DIV(ha, m_res[f]->m, _R);
        a->m = ha;
      }
      if (normalize) p_Normalize(a->m, _R);
      a = a->n;
    } while (a != NULL);
  }
}

/*
* check for denominators
*/
int sparse_mat::smCheckNormalize()
{
  int i;
  smpoly a;

  for (i=act; i; i--)
  {
    a = m_act[i];
    do
    {
      if(sm_HaveDenom(a->m,_R)) return 1;
      a = a->n;
    } while (a != NULL);
  }
  return 0;
}

/*
* normalize
*/
void sparse_mat::smNormalize()
{
  smpoly a;
  int i;
  int e = crd;

  for (i=act; i; i--)
  {
    a = m_act[i];
    do
    {
      if (e == a->e) p_Normalize(a->m,_R);
      a = a->n;
    } while (a != NULL);
  }
}

/*
* multiply and divide the element, save poly
*/
poly sparse_mat::smMultPoly(smpoly a)
{
  int f = a->e;
  poly r, h;

  if (f < crd)
  {
    h = r = a->m;
    h = SM_MULT(h, m_res[crd]->m, m_res[f]->m, _R);
    if (f) SM_DIV(h, m_res[f]->m, _R);
    a->m = h;
    if (normalize) p_Normalize(a->m,_R);
    a->f = sm_PolyWeight(a,_R);
    return r;
  }
  else
    return NULL;
}

/*
* delete the m_act finaly
*/
void sparse_mat::smActDel()
{
  smpoly a;
  int i;

  for (i=act; i; i--)
  {
    a = m_act[i];
    do
    {
      sm_ElemDelete(&a,_R);
    } while (a != NULL);
  }
}

/*
* delete the pivotcol
*/
void sparse_mat::smColDel()
{
  smpoly a = m_act[act];

  while (a != NULL)
  {
    sm_ElemDelete(&a,_R);
  }
}

/*
* delete pivot elements
*/
void sparse_mat::smPivDel()
{
  int i=crd;

  while (i != 0)
  {
    sm_ElemDelete(&m_res[i],_R);
    i--;
  }
}

/*
* the sign of the determinant
*/
void sparse_mat::smSign()
{
  int j,i;
  if (act > 2)
  {
    if (cpiv!=act) sign=-sign;
    if ((act%2)==0) sign=-sign;
    i=1;
    j=perm[1];
    while(j<rpiv)
    {
      sign=-sign;
      i++;
      j=perm[i];
    }
    while(perm[i]!=0)
    {
      perm[i]=perm[i+1];
      i++;
    }
  }
  else
  {
    if (cpiv!=1) sign=-sign;
    if (rpiv!=perm[1]) sign=-sign;
  }
}

void sparse_mat::smInitPerm()
{
  int i;
  for (i=act;i;i--) perm[i]=i;
}

/* ----------------- arithmetic ------------------ */
#ifdef OLD_DIV
/*2
* exact division a/b
* a destroyed, b NOT destroyed
*/
void sm_PolyDiv(poly a, poly b, const ring R)
{
  const number x = pGetCoeff(b);
  number y, yn;
  poly t, h, dummy;
  int i;

  if (pNext(b) == NULL)
  {
    do
    {
      if (!p_LmIsConstantComp(b,R))
      {
        for (i=rVar(R); i; i--)
          p_SubExp(a,i,p_GetExp(b,i,R),R);
        p_Setm(a,R);
      }
      y = n_Div(pGetCoeff(a),x,R->cf);
      n_Normalize(y,R->cf);
      p_SetCoeff(a,y,R);
      pIter(a);
    } while (a != NULL);
    return;
  }
  dummy = p_Init(R);
  do
  {
    for (i=rVar(R); i; i--)
      p_SubExp(a,i,p_GetExp(b,i,R),R);
    p_Setm(a,R);
    y = n_Div(pGetCoeff(a),x,R->cf);
    n_Normalize(y,R->cf);
    p_SetCoeff(a,y,R);
    yn = n_InpNeg(n_Copy(y,R->cf),R->cf);
    t = pNext(b);
    h = dummy;
    do
    {
      h = pNext(h) = p_Init(R);
      //pSetComp(h,0);
      for (i=rVar(R); i; i--)
        p_SetExp(h,i,p_GetExp(a,i,R)+p_GetExp(t,i,R),R);
      p_Setm(h,R);
      pSetCoeff0(h,n_Mult(yn, pGetCoeff(t),R->cf));
      pIter(t);
    } while (t != NULL);
    n_Delete(&yn,R->cf);
    pNext(h) = NULL;
    a = pNext(a) = p_Add_q(pNext(a), pNext(dummy),R);
  } while (a!=NULL);
  p_LmFree(dummy, R);
}
#endif

//disable that, as it fails with coef buckets
//#define X_MAS
#ifdef X_MAS
// Note: the following in not addapted to SW :(
/*
///  returns the part of (a*b)/exp(lead(c)) with nonegative exponents
poly smMultDiv(poly a, poly b, const poly c)
{
  poly pa, e, res, r;
  BOOLEAN lead;
  int la, lb, lr;

  if ((c == NULL) || pLmIsConstantComp(c))
  {
    return pp_Mult_qq(a, b);
  }

  pqLength(a, b, la, lb, SM_MIN_LENGTH_BUCKET);

  // we iter over b, so, make sure b is the shortest
  // such that we minimize our iterations
  if (lb > la)
  {
    r = a;
    a = b;
    b = r;
    lr = la;
    la = lb;
    lb = lr;
  }
  res = NULL;
  e = p_Init();
  lead = FALSE;
  while (!lead)
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (smIsNegQuot(e, b, c))
    {
      lead = pLmDivisibleByNoComp(e, a);
      r = smSelectCopy_ExpMultDiv(a, e, b, c);
    }
    else
    {
      lead = TRUE;
      r = pp_Mult__mm(a, e);
    }
    if (lead)
    {
      if (res != NULL)
      {
        smFindRef(&pa, &res, r);
        if (pa == NULL)
          lead = FALSE;
      }
      else
      {
        pa = res = r;
      }
    }
    else
      res = p_Add_q(res, r);
    pIter(b);
    if (b == NULL)
    {
      pLmFree(e);
      return res;
    }
  }

  if (!TEST_OPT_NOT_BUCKETS && lb >= SM_MIN_LENGTH_BUCKET)
  {
    // use buckets if minimum length is smaller than threshold
    spolyrec rp;
    poly append;
    // find the last monomial before pa
    if (res == pa)
    {
      append = &rp;
      pNext(append) = res;
    }
    else
    {
      append = res;
      while (pNext(append) != pa)
      {
        assume(pNext(append) != NULL);
        pIter(append);
      }
    }
    kBucket_pt bucket = kBucketCreate(currRing);
    kBucketInit(bucket, pNext(append), 0);
    do
    {
      pSetCoeff0(e,pGetCoeff(b));
      if (smIsNegQuot(e, b, c))
      {
        lr = la;
        r = pp_Mult_Coeff_mm_DivSelect_MultDiv(a, lr, e, b, c);
        if (pLmDivisibleByNoComp(e, a))
          append = kBucket_ExtractLarger_Add_q(bucket, append, r, &lr);
        else
          kBucket_Add_q(bucket, r, &lr);
      }
      else
      {
        r = pp_Mult__mm(a, e);
        append = kBucket_ExtractLarger_Add_q(bucket, append, r, &la);
      }
      pIter(b);
    } while (b != NULL);
    pNext(append) = kBucketClear(bucket);
    kBucketDestroy(&bucket);
    pTest(append);
  }
  else
  {
    // use old sm stuff
    do
    {
      pSetCoeff0(e,pGetCoeff(b));
      if (smIsNegQuot(e, b, c))
      {
        r = smSelectCopy_ExpMultDiv(a, e, b, c);
        if (pLmDivisibleByNoComp(e, a))
          smCombineChain(&pa, r);
        else
          pa = p_Add_q(pa,r);
      }
      else
      {
        r = pp_Mult__mm(a, e);
        smCombineChain(&pa, r);
      }
      pIter(b);
    } while (b != NULL);
  }
  pLmFree(e);
  return res;
}
*/
#else

/*
*  returns the part of (a*b)/exp(lead(c)) with nonegative exponents
*/
poly sm_MultDiv(poly a, poly b, const poly c, const ring R)
{
  poly pa, e, res, r;
  BOOLEAN lead;

  if ((c == NULL) || p_LmIsConstantComp(c,R))
  {
    return pp_Mult_qq(a, b, R);
  }
  if (smSmaller(a, b))
  {
    r = a;
    a = b;
    b = r;
  }
  res = NULL;
  e = p_Init(R);
  lead = FALSE;
  while (!lead)
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (sm_IsNegQuot(e, b, c, R))
    {
      lead = p_LmDivisibleByNoComp(e, a, R);
      r = sm_SelectCopy_ExpMultDiv(a, e, b, c, R);
    }
    else
    {
      lead = TRUE;
      r = pp_Mult_mm(a, e,R);
    }
    if (lead)
    {
      if (res != NULL)
      {
        sm_FindRef(&pa, &res, r, R);
        if (pa == NULL)
          lead = FALSE;
      }
      else
      {
        pa = res = r;
      }
    }
    else
      res = p_Add_q(res, r, R);
    pIter(b);
    if (b == NULL)
    {
      p_LmFree(e, R);
      return res;
    }
  }
  do
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (sm_IsNegQuot(e, b, c, R))
    {
      r = sm_SelectCopy_ExpMultDiv(a, e, b, c, R);
      if (p_LmDivisibleByNoComp(e, a,R))
        sm_CombineChain(&pa, r, R);
      else
        pa = p_Add_q(pa,r,R);
    }
    else
    {
      r = pp_Mult_mm(a, e, R);
      sm_CombineChain(&pa, r, R);
    }
    pIter(b);
  } while (b != NULL);
  p_LmFree(e, R);
  return res;
}
#endif /*else X_MAS*/

/*n
* exact division a/b
* a is a result of smMultDiv
* a destroyed, b NOT destroyed
*/
void sm_SpecialPolyDiv(poly a, poly b, const ring R)
{
  if (pNext(b) == NULL)
  {
    sm_PolyDivN(a, pGetCoeff(b),R);
    return;
  }
  sm_ExactPolyDiv(a, b, R);
}

/* ------------ internals arithmetic ------------- */
static void sm_ExactPolyDiv(poly a, poly b, const ring R)
{
  const number x = pGetCoeff(b);
  poly tail = pNext(b), e = p_Init(R);
  poly h;
  number y, yn;
  int lt = pLength(tail);

  if (lt + 1 >= SM_MIN_LENGTH_BUCKET &&  !TEST_OPT_NOT_BUCKETS)
  {
    kBucket_pt bucket = kBucketCreate(R);
    kBucketInit(bucket, pNext(a), 0);
    int lh = 0;
    do
    {
      y = n_Div(pGetCoeff(a), x, R->cf);
      n_Normalize(y, R->cf);
      p_SetCoeff(a,y, R);
      yn = n_InpNeg(n_Copy(y, R->cf), R->cf);
      pSetCoeff0(e,yn);
      lh = lt;
      if (sm_IsNegQuot(e, a, b, R))
      {
        h = pp_Mult_Coeff_mm_DivSelect_MultDiv(tail, lh, e, a, b, R);
      }
      else
        h = pp_Mult_mm(tail, e, R);
      n_Delete(&yn, R->cf);
      kBucket_Add_q(bucket, h, &lh);

      a = pNext(a) = kBucketExtractLm(bucket);
    } while (a!=NULL);
    kBucketDestroy(&bucket);
  }
  else
  {
    do
    {
      y = n_Div(pGetCoeff(a), x, R->cf);
      n_Normalize(y, R->cf);
      p_SetCoeff(a,y, R);
      yn = n_InpNeg(n_Copy(y, R->cf), R->cf);
      pSetCoeff0(e,yn);
      if (sm_IsNegQuot(e, a, b, R))
        h = sm_SelectCopy_ExpMultDiv(tail, e, a, b, R);
      else
        h = pp_Mult_mm(tail, e, R);
      n_Delete(&yn, R->cf);
      a = pNext(a) = p_Add_q(pNext(a), h, R);
    } while (a!=NULL);
  }
  p_LmFree(e, R);
}

// obachman --> Wilfried: check the following
static BOOLEAN sm_IsNegQuot(poly a, const poly b, const poly c, const ring R)
{
  if (p_LmDivisibleByNoComp(c, b,R))
  {
    p_ExpVectorDiff(a, b, c,R);
    // Hmm: here used to be a p_Setm(a): but it is unnecessary,
    // if b and c are correct
    return FALSE;
  }
  else
  {
    int i;
    for (i=rVar(R); i>0; i--)
    {
      if(p_GetExp(c,i,R) > p_GetExp(b,i,R))
        p_SetExp(a,i,p_GetExp(c,i,R)-p_GetExp(b,i,R),R);
      else
        p_SetExp(a,i,0,R);
    }
    // here we actually might need a p_Setm, if a is to be used in
    // comparisons
    return TRUE;
  }
}

static void sm_ExpMultDiv(poly t, const poly b, const poly c, const ring R)
{
  p_Test(t,R);
  p_LmTest(b,R);
  p_LmTest(c,R);
  poly bc = p_New(R);

  p_ExpVectorDiff(bc, b, c, R);

  while(t!=NULL)
  {
    p_ExpVectorAdd(t, bc, R);
    pIter(t);
  }
  p_LmFree(bc, R);
}


static void sm_PolyDivN(poly a, const number x, const ring R)
{
  number y;

  do
  {
    y = n_Div(pGetCoeff(a),x, R->cf);
    n_Normalize(y, R->cf);
    p_SetCoeff(a,y, R);
    pIter(a);
  } while (a != NULL);
}

static BOOLEAN smSmaller(poly a, poly b)
{
  loop
  {
    pIter(b);
    if (b == NULL) return TRUE;
    pIter(a);
    if (a == NULL) return FALSE;
  }
}

static void sm_CombineChain(poly *px, poly r, const ring R)
{
  poly pa = *px, pb;
  number x;
  int i;

  loop
  {
    pb = pNext(pa);
    if (pb == NULL)
    {
      pa = pNext(pa) = r;
      break;
    }
    i = p_LmCmp(pb, r,R);
    if (i > 0)
      pa = pb;
    else
    {
      if (i == 0)
      {
        x = n_Add(pGetCoeff(pb), pGetCoeff(r),R->cf);
        p_LmDelete(&r,R);
        if (n_IsZero(x,R->cf))
        {
          p_LmDelete(&pb,R);
          pNext(pa) = p_Add_q(pb,r,R);
        }
        else
        {
          pa = pb;
          p_SetCoeff(pa,x,R);
          pNext(pa) = p_Add_q(pNext(pa), r, R);
        }
      }
      else
      {
        pa = pNext(pa) = r;
        pNext(pa) = p_Add_q(pb, pNext(pa),R);
      }
      break;
    }
  }
  *px = pa;
}


static void sm_FindRef(poly *ref, poly *px, poly r, const ring R)
{
  number x;
  int i;
  poly pa = *px, pp = NULL;

  loop
  {
    i = p_LmCmp(pa, r,R);
    if (i > 0)
    {
      pp = pa;
      pIter(pa);
      if (pa==NULL)
      {
        pNext(pp) = r;
        break;
      }
    }
    else
    {
      if (i == 0)
      {
        x = n_Add(pGetCoeff(pa), pGetCoeff(r),R->cf);
        p_LmDelete(&r,R);
        if (n_IsZero(x,R->cf))
        {
          p_LmDelete(&pa,R);
          if (pp!=NULL)
            pNext(pp) = p_Add_q(pa,r,R);
          else
            *px = p_Add_q(pa,r,R);
        }
        else
        {
          pp = pa;
          p_SetCoeff(pp,x,R);
          pNext(pp) = p_Add_q(pNext(pp), r, R);
        }
      }
      else
      {
        if (pp!=NULL)
          pp = pNext(pp) = r;
        else
          *px = pp = r;
        pNext(pp) = p_Add_q(pa, pNext(r),R);
      }
      break;
    }
  }
  *ref = pp;
}

/* ----------------- internal 'C' stuff ------------------ */

static void sm_ElemDelete(smpoly *r, const ring R)
{
  smpoly a = *r, b = a->n;

  p_Delete(&a->m, R);
  omFreeBin((void *)a,  smprec_bin);
  *r = b;
}

static smpoly smElemCopy(smpoly a)
{
  smpoly r = (smpoly)omAllocBin(smprec_bin);
  memcpy(r, a, sizeof(smprec));
/*  r->m = pCopy(r->m); */
  return r;
}

/*
* from poly to smpoly
* do not destroy p
*/
static smpoly sm_Poly2Smpoly(poly q, const ring R)
{
  poly pp;
  smpoly res, a;
  long x;

  if (q == NULL)
    return NULL;
  a = res = (smpoly)omAllocBin(smprec_bin);
  a->pos = x = p_GetComp(q,R);
  a->m = q;
  a->e = 0;
  loop
  {
    p_SetComp(q,0,R);
    pp = q;
    pIter(q);
    if (q == NULL)
    {
      a->n = NULL;
      return res;
    }
    if (p_GetComp(q,R) != x)
    {
      a = a->n = (smpoly)omAllocBin(smprec_bin);
      pNext(pp) = NULL;
      a->pos = x = p_GetComp(q,R);
      a->m = q;
      a->e = 0;
    }
  }
}

/*
* from smpoly to poly
* destroy a
*/
static poly sm_Smpoly2Poly(smpoly a, const ring R)
{
  smpoly b;
  poly res, pp, q;
  long x;

  if (a == NULL)
    return NULL;
  x = a->pos;
  q = res = a->m;
  loop
  {
    p_SetComp(q,x,R);
    pp = q;
    pIter(q);
    if (q == NULL)
      break;
  }
  loop
  {
    b = a;
    a = a->n;
    omFreeBin((void *)b,  smprec_bin);
    if (a == NULL)
      return res;
    x = a->pos;
    q = pNext(pp) = a->m;
    loop
    {
      p_SetComp(q,x,R);
      pp = q;
      pIter(q);
      if (q == NULL)
        break;
    }
  }
}

/*
* weigth of a polynomial, for pivot strategy
*/
static float sm_PolyWeight(smpoly a, const ring R)
{
  poly p = a->m;
  int i;
  float res = (float)n_Size(pGetCoeff(p),R->cf);

  if (pNext(p) == NULL)
  {
    for(i=rVar(R); i>0; i--)
    {
      if (p_GetExp(p,i,R) != 0) return res+1.0;
    }
    return res;
  }
  else
  {
    i = 0;
    res = 0.0;
    do
    {
      i++;
      res += (float)n_Size(pGetCoeff(p),R->cf);
      pIter(p);
    }
    while (p);
    return res+(float)i;
  }
}

static BOOLEAN sm_HaveDenom(poly a, const ring R)
{
  BOOLEAN sw;
  number x;

  while (a != NULL)
  {
    x = n_GetDenom(pGetCoeff(a),R->cf);
    sw = n_IsOne(x,R->cf);
    n_Delete(&x,R->cf);
    if (!sw)
    {
      return TRUE;
    }
    pIter(a);
  }
  return FALSE;
}

static number sm_Cleardenom(ideal id, const ring R)
{
  poly a;
  number x,y,res=n_Init(1,R->cf);
  BOOLEAN sw=FALSE;

  for (int i=0; i<IDELEMS(id); i++)
  {
    a = id->m[i];
    sw = sm_HaveDenom(a,R);
    if (sw) break;
  }
  if (!sw) return res;
  for (int i=0; i<IDELEMS(id); i++)
  {
    a = id->m[i];
    if (a!=NULL)
    {
      x = n_Copy(pGetCoeff(a),R->cf);
      p_Cleardenom(a, R);
      y = n_Div(x,pGetCoeff(a),R->cf);
      n_Delete(&x,R->cf);
      x = n_Mult(res,y,R->cf);
      n_Normalize(x,R->cf);
      n_Delete(&res,R->cf);
      res = x;
    }
  }
  return res;
}

/* ----------------- gauss elimination ------------------ */
/* in structs.h */
typedef struct smnrec sm_nrec;
typedef sm_nrec * smnumber;
struct smnrec{
  smnumber n;          // the next element
  int pos;             // position
  number m;            // the element
};

static omBin smnrec_bin = omGetSpecBin(sizeof(smnrec));

/* declare internal 'C' stuff */
static void sm_NumberDelete(smnumber *, const ring R);
static smnumber smNumberCopy(smnumber);
static smnumber sm_Poly2Smnumber(poly, const ring);
static poly sm_Smnumber2Poly(number,const ring);
static BOOLEAN smCheckSolv(ideal);

/* class for sparse number matrix:
*/
class sparse_number_mat{
private:
  int nrows, ncols;    // dimension of the problem
  int act;             // number of unreduced columns (start: ncols)
  int crd;             // number of reduced columns (start: 0)
  int tored;           // border for rows to reduce
  int sing;            // indicator for singular problem
  int rpiv;            // row-position of the pivot
  int *perm;           // permutation of rows
  number *sol;         // field for solution
  int *wrw, *wcl;      // weights of rows and columns
  smnumber * m_act;    // unreduced columns
  smnumber * m_res;    // reduced columns (result)
  smnumber * m_row;    // reduced part of rows
  smnumber red;        // row to reduce
  smnumber piv;        // pivot
  smnumber dumm;       // allocated dummy
  ring _R;
  void smColToRow();
  void smRowToCol();
  void smSelectPR();
  void smRealPivot();
  void smZeroToredElim();
  void smGElim();
  void smAllDel();
public:
  sparse_number_mat(ideal, const ring);
  ~sparse_number_mat();
  int smIsSing() { return sing; }
  void smTriangular();
  void smSolv();
  ideal smRes2Ideal();
};

/* ----------------- basics (used from 'C') ------------------ */
/*2
* returns the solution of a linear equation
* solution of M*x = r (M has dimension nXn) =>
*   I = module(transprose(M)) + r*gen(n+1)
* uses  Gauss-elimination
*/
ideal sm_CallSolv(ideal I, const ring R)
{
  sparse_number_mat *linsolv;
  ring tmpR;
  ideal rr;

  if (id_IsConstant(I,R)==FALSE)
  {
    WerrorS("symbol in equation");
    return NULL;
  }
  I->rank = id_RankFreeModule(I,R);
  if (smCheckSolv(I)) return NULL;
  tmpR=sm_RingChange(R,1);
  rr=idrCopyR(I,R, tmpR);
  linsolv = new sparse_number_mat(rr,tmpR);
  rr=NULL;
  linsolv->smTriangular();
  if (linsolv->smIsSing() == 0)
  {
    linsolv->smSolv();
    rr = linsolv->smRes2Ideal();
  }
  else
    WerrorS("singular problem for linsolv");
  delete linsolv;
  if (rr!=NULL)
    rr = idrMoveR(rr,tmpR,R);
  sm_KillModifiedRing(tmpR);
  return rr;
}

/*
* constructor, destroy smat
*/
sparse_number_mat::sparse_number_mat(ideal smat, const ring R)
{
  int i;
  poly* pmat;
  _R=R;

  crd = sing = 0;
  act = ncols = smat->ncols;
  tored = nrows = smat->rank;
  i = tored+1;
  perm = (int *)omAlloc(sizeof(int)*i);
  m_row = (smnumber *)omAlloc0(sizeof(smnumber)*i);
  wrw = (int *)omAlloc(sizeof(int)*i);
  i = ncols+1;
  wcl = (int *)omAlloc(sizeof(int)*i);
  m_act = (smnumber *)omAlloc(sizeof(smnumber)*i);
  m_res = (smnumber *)omAlloc0(sizeof(smnumber)*i);
  dumm = (smnumber)omAllocBin(smnrec_bin);
  pmat = smat->m;
  for(i=ncols; i; i--)
  {
    m_act[i] = sm_Poly2Smnumber(pmat[i-1],_R);
  }
  omFreeSize((ADDRESS)pmat,smat->ncols*sizeof(poly));
  omFreeBin((ADDRESS)smat, sip_sideal_bin);
}

/*
* destructor
*/
sparse_number_mat::~sparse_number_mat()
{
  int i;
  omFreeBin((ADDRESS)dumm,  smnrec_bin);
  i = ncols+1;
  omFreeSize((ADDRESS)m_res, sizeof(smnumber)*i);
  omFreeSize((ADDRESS)m_act, sizeof(smnumber)*i);
  omFreeSize((ADDRESS)wcl, sizeof(int)*i);
  i = nrows+1;
  omFreeSize((ADDRESS)wrw, sizeof(int)*i);
  omFreeSize((ADDRESS)m_row, sizeof(smnumber)*i);
  omFreeSize((ADDRESS)perm, sizeof(int)*i);
}

/*
* triangularization by Gauss-elimination
*/
void sparse_number_mat::smTriangular()
{
  tored--;
  this->smZeroToredElim();
  if (sing != 0) return;
  while (act > 1)
  {
    this->smRealPivot();
    this->smSelectPR();
    this->smGElim();
    crd++;
    this->smColToRow();
    act--;
    this->smRowToCol();
    this->smZeroToredElim();
    if (sing != 0) return;
  }
  if (TEST_OPT_PROT) PrintS(".\n");
  piv = m_act[1];
  rpiv = piv->pos;
  m_act[1] = piv->n;
  piv->n = NULL;
  crd++;
  this->smColToRow();
  act--;
  this->smRowToCol();
}

/*
* solve the triangular system
*/
void sparse_number_mat::smSolv()
{
  int i, j;
  number x, y, z;
  smnumber s, d, r = m_row[nrows];

  m_row[nrows] = NULL;
  sol = (number *)omAlloc0(sizeof(number)*(crd+1));
  while (r != NULL)  // expand the rigth hand side
  {
    sol[r->pos] = r->m;
    s = r;
    r = r->n;
    omFreeBin((ADDRESS)s,  smnrec_bin);
  }
  i = crd;  // solve triangular system
  if (sol[i] != NULL)
  {
    x = sol[i];
    sol[i] = n_Div(x, m_res[i]->m,_R->cf);
    n_Delete(&x,_R->cf);
  }
  i--;
  while (i > 0)
  {
    x = NULL;
    d = m_res[i];
    s = d->n;
    while (s != NULL)
    {
      j = s->pos;
      if (sol[j] != NULL)
      {
        z = n_Mult(sol[j], s->m,_R->cf);
        if (x != NULL)
        {
          y = x;
          x = n_Sub(y, z,_R->cf);
          n_Delete(&y,_R->cf);
          n_Delete(&z,_R->cf);
        }
        else
          x = n_InpNeg(z,_R->cf);
      }
      s = s->n;
    }
    if (sol[i] != NULL)
    {
      if (x != NULL)
      {
        y = n_Add(x, sol[i],_R->cf);
        n_Delete(&x,_R->cf);
        if (n_IsZero(y,_R->cf))
        {
          n_Delete(&sol[i],_R->cf);
          sol[i] = NULL;
        }
        else
          sol[i] = y;
      }
    }
    else
      sol[i] = x;
    if (sol[i] != NULL)
    {
      x = sol[i];
      sol[i] = n_Div(x, d->m,_R->cf);
      n_Delete(&x,_R->cf);
    }
    i--;
  }
  this->smAllDel();
}

/*
* transform the result to an ideal
*/
ideal sparse_number_mat::smRes2Ideal()
{
  int i, j;
  ideal res = idInit(crd, 1);

  for (i=crd; i; i--)
  {
    j = perm[i]-1;
    res->m[j] = sm_Smnumber2Poly(sol[i],_R);
  }
  omFreeSize((ADDRESS)sol, sizeof(number)*(crd+1));
  return res;
}

/* ----------------- pivot method ------------------ */

/*
* compute pivot
*/
void sparse_number_mat::smRealPivot()
{
  smnumber a;
  number x, xo;
  int i, copt, ropt;

  xo=n_Init(0,_R->cf);
  for (i=act; i; i--)
  {
    a = m_act[i];
    while ((a!=NULL) && (a->pos<=tored))
    {
      x = a->m;
      if (n_GreaterZero(x,_R->cf))
      {
        if (n_Greater(x,xo,_R->cf))
        {
          n_Delete(&xo,_R->cf);
          xo = n_Copy(x,_R->cf);
          copt = i;
          ropt = a->pos;
        }
      }
      else
      {
        xo = n_InpNeg(xo,_R->cf);
        if (n_Greater(xo,x,_R->cf))
        {
          n_Delete(&xo,_R->cf);
          xo = n_Copy(x,_R->cf);
          copt = i;
          ropt = a->pos;
        }
        xo = n_InpNeg(xo,_R->cf);
      }
      a = a->n;
    }
  }
  rpiv = ropt;
  if (copt != act)
  {
    a = m_act[act];
    m_act[act] = m_act[copt];
    m_act[copt] = a;
  }
  n_Delete(&xo,_R->cf);
}

/* ----------------- elimination ------------------ */

/* one step of Gauss-elimination */
void sparse_number_mat::smGElim()
{
  number p = n_Invers(piv->m,_R->cf);  // pivotelement
  smnumber c = m_act[act];      // pivotcolumn
  smnumber r = red;             // row to reduce
  smnumber res, a, b;
  number w, ha, hb;

  if ((c == NULL) || (r == NULL))
  {
    while (r!=NULL) sm_NumberDelete(&r,_R);
    return;
  }
  do
  {
    a = m_act[r->pos];
    res = dumm;
    res->n = NULL;
    b = c;
    w = n_Mult(r->m, p,_R->cf);
    n_Delete(&r->m,_R->cf);
    r->m = w;
    loop   // combine the chains a and b: a + w*b
    {
      if (a == NULL)
      {
        do
        {
          res = res->n = smNumberCopy(b);
          res->m = n_Mult(b->m, w,_R->cf);
          b = b->n;
        } while (b != NULL);
        break;
      }
      if (a->pos < b->pos)
      {
        res = res->n = a;
        a = a->n;
      }
      else if (a->pos > b->pos)
      {
        res = res->n = smNumberCopy(b);
        res->m = n_Mult(b->m, w,_R->cf);
        b = b->n;
      }
      else
      {
        hb = n_Mult(b->m, w,_R->cf);
        ha = n_Add(a->m, hb,_R->cf);
        n_Delete(&hb,_R->cf);
        n_Delete(&a->m,_R->cf);
        if (n_IsZero(ha,_R->cf))
        {
          sm_NumberDelete(&a,_R);
        }
        else
        {
          a->m = ha;
          res = res->n = a;
          a = a->n;
        }
        b = b->n;
      }
      if (b == NULL)
      {
        res->n = a;
        break;
      }
    }
    m_act[r->pos] = dumm->n;
    sm_NumberDelete(&r,_R);
  } while (r != NULL);
  n_Delete(&p,_R->cf);
}

/* ----------------- transfer ------------------ */

/*
* select the pivotrow and store it to red and piv
*/
void sparse_number_mat::smSelectPR()
{
  smnumber b = dumm;
  smnumber a, ap;
  int i;

  if (TEST_OPT_PROT)
  {
    if ((crd+1)%10)
      PrintS(".");
    else
      PrintS(".\n");
  }
  a = m_act[act];
  if (a->pos < rpiv)
  {
    do
    {
      ap = a;
      a = a->n;
    } while (a->pos < rpiv);
    ap->n = a->n;
  }
  else
    m_act[act] = a->n;
  piv = a;
  a->n = NULL;
  for (i=1; i<act; i++)
  {
    a = m_act[i];
    if (a->pos < rpiv)
    {
      loop
      {
        ap = a;
        a = a->n;
        if ((a == NULL) || (a->pos > rpiv))
          break;
        if (a->pos == rpiv)
        {
          ap->n = a->n;
          a->m = n_InpNeg(a->m,_R->cf);
          b = b->n = a;
          b->pos = i;
          break;
        }
      }
    }
    else if (a->pos == rpiv)
    {
      m_act[i] = a->n;
      a->m = n_InpNeg(a->m,_R->cf);
      b = b->n = a;
      b->pos = i;
    }
  }
  b->n = NULL;
  red = dumm->n;
}

/*
* store the pivotcol in m_row
*   m_act[cols][pos(rows)] => m_row[rows][pos(cols)]
*/
void sparse_number_mat::smColToRow()
{
  smnumber c = m_act[act];
  smnumber h;

  while (c != NULL)
  {
    h = c;
    c = c->n;
    h->n = m_row[h->pos];
    m_row[h->pos] = h;
    h->pos = crd;
  }
}

/*
* store the pivot and the assosiated row in m_row
* to m_res (result):
*   piv + m_row[rows][pos(cols)] => m_res[cols][pos(rows)]
*/
void sparse_number_mat::smRowToCol()
{
  smnumber r = m_row[rpiv];
  smnumber a, ap, h;

  m_row[rpiv] = NULL;
  perm[crd] = rpiv;
  piv->pos = crd;
  m_res[crd] = piv;
  while (r != NULL)
  {
    ap = m_res[r->pos];
    loop
    {
      a = ap->n;
      if (a == NULL)
      {
        ap->n = h = r;
        r = r->n;
        h->n = a;
        h->pos = crd;
        break;
      }
      ap = a;
    }
  }
}

/* ----------------- C++ stuff ------------------ */

/*
*  check singularity
*/
void sparse_number_mat::smZeroToredElim()
{
  smnumber a;
  int i = act;

  loop
  {
    if (i == 0) return;
    a = m_act[i];
    if ((a==NULL) || (a->pos>tored))
    {
      sing = 1;
      this->smAllDel();
      return;
    }
    i--;
  }
}

/*
* delete all smnumber's
*/
void sparse_number_mat::smAllDel()
{
  smnumber a;
  int i;

  for (i=act; i; i--)
  {
    a = m_act[i];
    while (a != NULL)
      sm_NumberDelete(&a,_R);
  }
  for (i=crd; i; i--)
  {
    a = m_res[i];
    while (a != NULL)
      sm_NumberDelete(&a,_R);
  }
  if (act)
  {
    for (i=nrows; i; i--)
    {
      a = m_row[i];
      while (a != NULL)
        sm_NumberDelete(&a,_R);
    }
  }
}

/* ----------------- internal 'C' stuff ------------------ */

static void sm_NumberDelete(smnumber *r, const ring R)
{
  smnumber a = *r, b = a->n;

  n_Delete(&a->m,R->cf);
  omFreeBin((ADDRESS)a,  smnrec_bin);
  *r = b;
}

static smnumber smNumberCopy(smnumber a)
{
  smnumber r = (smnumber)omAllocBin(smnrec_bin);
  memcpy(r, a, sizeof(smnrec));
  return r;
}

/*
* from poly to smnumber
* do not destroy p
*/
static smnumber sm_Poly2Smnumber(poly q, const ring R)
{
  smnumber a, res;
  poly p = q;

  if (p == NULL)
    return NULL;
  a = res = (smnumber)omAllocBin(smnrec_bin);
  a->pos = p_GetComp(p,R);
  a->m = pGetCoeff(p);
  n_New(&pGetCoeff(p),R->cf);
  loop
  {
    pIter(p);
    if (p == NULL)
    {
      p_Delete(&q,R);
      a->n = NULL;
      return res;
    }
    a = a->n = (smnumber)omAllocBin(smnrec_bin);
    a->pos = p_GetComp(p,R);
    a->m = pGetCoeff(p);
    n_New(&pGetCoeff(p),R->cf);
  }
}

/*
* from smnumber to poly
* destroy a
*/
static poly sm_Smnumber2Poly(number a, const ring R)
{
  poly res;

  if (a == NULL) return NULL;
  res = p_Init(R);
  pSetCoeff0(res, a);
  return res;
}

/*2
* check the input
*/
static BOOLEAN smCheckSolv(ideal I)
{ int i = I->ncols;
  if ((i == 0) || (i != I->rank-1))
  {
    WerrorS("wrong dimensions for linsolv");
    return TRUE;
  }
  for(;i;i--)
  {
    if(I->m[i-1] == NULL)
    {
      WerrorS("singular input for linsolv");
      return TRUE;
    }
  }
  return FALSE;
}
