/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */

/*
* ABSTRACT: operations with sparse matrices (bareiss, ...)
*/

#include <kernel/mod2.h>
#include <kernel/options.h>
#include <kernel/febase.h>
#include <kernel/intvec.h>
#include <kernel/ring.h>
#include <kernel/polys.h>
#include <kernel/ideals.h>
#include <kernel/numbers.h>
#include <kernel/sparsmat.h>
#include <kernel/prCopy.h>
#include <kernel/p_Procs.h>
#include <kernel/kbuckets.h>
#include <kernel/p_Mult_q.h>

// define SM_NO_BUCKETS, if sparsemat stuff should not use buckets
// #define SM_NO_BUCKETS

// this is also influenced by TEST_OPT_NOTBUCKETS
#ifndef SM_NO_BUCKETS
// buckets do carry a small additional overhead: only use them if
// min-length of polys is >= SM_MIN_LENGTH_BUCKET
#define SM_MIN_LENGTH_BUCKET MIN_LENGTH_BUCKET - 5
#else
#define SM_MIN_LENGTH_BUCKET    INT_MAX
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
static void smExactPolyDiv(poly, poly);
static BOOLEAN smIsNegQuot(poly, const poly, const poly);
static void smExpMultDiv(poly, const poly, const poly);
static void smPolyDivN(poly, const number);
static BOOLEAN smSmaller(poly, poly);
static void smCombineChain(poly *, poly);
static void smFindRef(poly *, poly *, poly);

static void smElemDelete(smpoly *);
static smpoly smElemCopy(smpoly);
static float smPolyWeight(smpoly);
static smpoly smPoly2Smpoly(poly);
static poly smSmpoly2Poly(smpoly);
static BOOLEAN smHaveDenom(poly);
static number smCleardenom(ideal);

struct omBin_s* smprec_bin = omGetSpecBin(sizeof(smprec));

static poly pp_Mult_Coeff_mm_DivSelect_MultDiv(poly p, int &lp, poly m,
                                               poly a, poly b)
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
    smExpMultDiv(p, a, b);
  }
  return p;
}

static poly smSelectCopy_ExpMultDiv(poly p, poly m, poly a, poly b)
{
  int lp = 0;
  return pp_Mult_Coeff_mm_DivSelect_MultDiv(p, lp, m, a, b);
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
  void smElim();
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
  sparse_mat(ideal);
  ~sparse_mat();
  int smGetSign() { return sign; }
  smpoly * smGetAct() { return m_act; }
  int smGetRed() { return tored; }
  ideal smRes2Mod();
  poly smDet();
  void smBareiss(int, int);
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
long smExpBound( ideal m, int di, int ra, int t)
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
      k = pGetComp(p)-1;
      kr = r[k];
      for (j=pVariables;j>0;j--)
      {
        if(pGetExp(p,j)>kc)
          kc=pGetExp(p,j);
        if(pGetExp(p,j)>kr)
          kr=pGetExp(p,j);
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
void smRingChange(ring *origR, sip_sring &tmpR, long bound)
{
  *origR =currRing;
  tmpR=*currRing;
  int *ord=(int*)omAlloc0(3*sizeof(int));
  int *block0=(int*)omAlloc(3*sizeof(int));
  int *block1=(int*)omAlloc(3*sizeof(int));
  ord[0]=ringorder_c;
  ord[1]=ringorder_dp;
  tmpR.order=ord;
  tmpR.OrdSgn=1;
  block0[1]=1;
  tmpR.block0=block0;
  block1[1]=tmpR.N;
  tmpR.block1=block1;
  tmpR.bitmask = 2*bound;

// ???
//  if (tmpR.bitmask > currRing->bitmask) tmpR.bitmask = currRing->bitmask;

  rComplete(&tmpR,1);
  rChangeCurrRing(&tmpR);
  if (TEST_OPT_PROT)
    Print("[%ld:%d]", (long) tmpR.bitmask, tmpR.ExpL_Size);
}

void smRingClean(ring origR, ip_sring &tmpR)
{
  rUnComplete(&tmpR);
  omFreeSize((ADDRESS)tmpR.order,3*sizeof(int));
  omFreeSize((ADDRESS)tmpR.block0,3*sizeof(int));
  omFreeSize((ADDRESS)tmpR.block1,3*sizeof(int));
}

/*2
* Bareiss or Chinese remainder ?
* I is dXd
* sw = TRUE  -> I Matrix
*      FALSE -> I Module
* return True  -> change Type
*        FALSE -> same Type
*/
BOOLEAN smCheckDet(ideal I, int d, BOOLEAN sw)
{
  int s,t,i;
  poly p;

  if ((d>100) || (currRing->parameter!=NULL))
    return sw;
  if (rField_is_Q(currRing)==FALSE)
    return sw;
  s = t = 0;
  if (sw)
  {
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
      p=I->m[i];
      if (p!=NULL)
      {
        if(!pIsConstant(p))
          return sw;
        s++;
        t+=nSize(pGetCoeff(p));
      }
    }
  }
  else
  {
    for(i=IDELEMS(I)-1;i>=0;i--)
    {
      p=I->m[i];
      if (!pIsConstantPoly(p))
        return sw;
      while (p!=NULL)
      {
        s++;
        t+=nSize(pGetCoeff(p));
        pIter(p);
      }
    }
  }
  s*=15;
  if (t>s)
    return !sw;
  else
    return sw;
}

/* ----------------- basics (used from 'C') ------------------ */
/*2
*returns the determinant of the module I;
*uses Bareiss algorithm
*/
poly smCallDet(ideal I)
{
  if (I->ncols != I->rank)
  {
    Werror("det of %ld x %d module (matrix)",I->rank,I->ncols);
    return NULL;
  }
  int r=idRankFreeModule(I);
  if (I->ncols != r) // some 0-lines at the end
  {
    return NULL;
  }
  long bound=smExpBound(I,r,r,r);
  number diag,h=nInit(1);
  poly res;
  ring origR;
  sip_sring tmpR;
  sparse_mat *det;
  ideal II;

  smRingChange(&origR,tmpR,bound);
  II = idrCopyR(I, origR);
  diag = smCleardenom(II);
  det = new sparse_mat(II);
  idDelete(&II);
  if (det->smGetAct() == NULL)
  {
    delete det;
    rChangeCurrRing(origR);
    smRingClean(origR,tmpR);
    return NULL;
  }
  res=det->smDet();
  if(det->smGetSign()<0) res=pNeg(res);
  delete det;
  rChangeCurrRing(origR);
  res = prMoveR(res, &tmpR);
  smRingClean(origR,tmpR);
  if (nEqual(diag,h) == FALSE)
  {
    pMult_nn(res,diag);
    pNormalize(res);
  }
  nDelete(&diag);
  nDelete(&h);
  return res;
}

void smCallBareiss(ideal I, int x, int y, ideal &M, intvec ** iv)
{
  int r=idRankFreeModule(I),t=r;
  int c=IDELEMS(I),s=c;
  long bound;
  ring origR;
  sip_sring tmpR;
  sparse_mat *bareiss;

  if ((x>0) && (x<t))
    t-=x;
  if ((y>1) && (y<s))
    s-=y;
  if (t>s) t=s;
  bound=2*smExpBound(I,c,r,t);
  smRingChange(&origR,tmpR,bound);
  ideal II = idrCopyR(I, origR);
  bareiss = new sparse_mat(II);
  if (bareiss->smGetAct() == NULL)
  {
    delete bareiss;
    *iv=new intvec(1,pVariables);
    rChangeCurrRing(origR);
  }
  else
  {
    idDelete(&II);
    bareiss->smBareiss(x, y);
    II = bareiss->smRes2Mod();
    *iv = new intvec(bareiss->smGetRed());
    bareiss->smToIntvec(*iv);
    delete bareiss;
    rChangeCurrRing(origR);
    II = idrMoveR(II,&tmpR);
  }
  smRingClean(origR,tmpR);
  M=II;
}

void smCallNewBareiss(ideal I, int x, int y, ideal & M, intvec **iv)
{
  int r=idRankFreeModule(I),t=r;
  int c=IDELEMS(I),s=c;
  long bound;
  ring origR;
  sip_sring tmpR;
  sparse_mat *bareiss;

  if ((x>0) && (x<t))
    t-=x;
  if ((y>1) && (y<s))
    s-=y;
  if (t>s) t=s;
  bound=smExpBound(I,c,r,t);
  smRingChange(&origR,tmpR,bound);
  ideal II = idrCopyR(I, origR);
  bareiss = new sparse_mat(II);
  if (bareiss->smGetAct() == NULL)
  {
    delete bareiss;
    *iv=new intvec(1,pVariables);
    rChangeCurrRing(origR);
  }
  else
  {
    idDelete(&II);
    bareiss->smNewBareiss(x, y);
    II = bareiss->smRes2Mod();
    *iv = new intvec(bareiss->smGetRed());
    bareiss->smToIntvec(*iv);
    delete bareiss;
    rChangeCurrRing(origR);
    II = idrMoveR(II,&tmpR);
  }
  smRingClean(origR,tmpR);
  M=II;
}

/*
* constructor
*/
sparse_mat::sparse_mat(ideal smat)
{
  int i;
  polyset pmat;

  ncols = smat->ncols;
  nrows = idRankFreeModule(smat);
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
    m_act[i] = smPoly2Smpoly(pmat[i-1]);
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

  for (i=crd; i; i--) res->m[i-1] = smSmpoly2Poly(m_res[i]);
  res->rank = idRankFreeModule(res);
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
  int y;
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
* the Bareiss elimination:
*   - with x unreduced last rows, pivots from here are not allowed
*   - the method will finish for number of unreduced columns < y
*/
void sparse_mat::smBareiss(int x, int y)
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
  this->smElim();
  crd++;
  this->smColToRow();
  act--;
  this->smRowToCol();
  this->smZeroElim();
  if (tored != nrows)
    this->smToredElim();
  if (act < y)
  {
    this->smCopToRes();
    return;
  }
  loop
  {
    if (normalize) this->smNormalize();
    this->smPivot();
    oldpiv = piv;
    this->smSelectPR();
    this->smElim();
    crd++;
    this->smColToRow();
    act--;
    this->smRowToCol();
    this->smZeroElim();
    if (tored != nrows)
      this->smToredElim();
    if (act < y)
    {
      if (TEST_OPT_PROT) PrintS(".\n");
      this->smCopToRes();
      return;
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
      w = a->f = smPolyWeight(a);
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

/* steps of elimination */
void sparse_mat::smElim()
{
  poly p = piv->m;        // pivotelement
  smpoly c = m_act[act];  // pivotcolumn
  smpoly r = red;         // row to reduce
  poly q;
  smpoly res, a, b;
  poly w, ha, hb;
  int i;

  if (oldpiv != NULL) q = oldpiv->m; // previous pivot
  else q = NULL;
  if ((c == NULL) || (r == NULL))
  {
    while (r) smElemDelete(&r);
    for (i=1; i<act; i++)
    {
      a = m_act[i];
      while (a != NULL)
      {
        ha = SM_MULT(a->m, p, q);
        pDelete(&a->m);
        if (q) SM_DIV(ha, q);
        a->m = ha;
        a = a->n;
      }
    }
    return;
  }
  for (i=1; i<act; i++)
  {
    a = m_act[i];
    if ((r == NULL) || (i != r->pos))  // cols without elimination
    {
      while (a != NULL)
      {
        ha = SM_MULT(a->m, p, q);
        pDelete(&a->m);
        if (q) SM_DIV(ha, q);
        a->m = ha;
        a = a->n;
      }
    }
    else                    // cols with elimination
    {
      res = dumm;
      res->n = NULL;
      b = c;
      w = r->m;
      loop                  // combine the chains a and b: p*a + w*b
      {
        if (a == NULL)
        {
          if (b != NULL)
          {
            do
            {
              res = res->n = smElemCopy(b);
              hb = SM_MULT(b->m, w, q);
              if (q) SM_DIV(hb, q);
              res->m = hb;
              b = b->n;
            } while (b != NULL);
          }
          else
            res->n = NULL;
          break;
        }
        if (b == NULL)
        {
          do
          {
            ha = SM_MULT(a->m, p, q);
            pDelete(&a->m);
            if (q) SM_DIV(ha, q);
            a->m = ha;
            res = res->n = a;
            a = a->n;
          } while (a != NULL);
          break;
        }
        if (a->pos < b->pos)
        {
          ha = SM_MULT(a->m, p, q);
          pDelete(&a->m);
          if (q) SM_DIV(ha, q);
          a->m = ha;
          res = res->n = a;
          a = a->n;
        }
        else if (a->pos > b->pos)
        {
          res = res->n = smElemCopy(b);
          hb = SM_MULT(b->m, w, q);
          b = b->n;
          if (q) SM_DIV(hb, q);
          res->m = hb;
        }
        else
        {
          ha = SM_MULT(a->m, p, q);
          pDelete(&a->m);
          hb = SM_MULT(b->m, w, q);
          ha = pAdd(ha, hb);
          if (ha != NULL)
          {
            if (q) SM_DIV(ha, q);
            a->m = ha;
            res = res->n = a;
            a = a->n;
          }
          else
          {
            smElemDelete(&a);
          }
          b = b->n;
        }
      }
      m_act[i] = dumm->n;
      if (r) smElemDelete(&r);
    }
  }
}

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
    while (r!=NULL) smElemDelete(&r);
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
          res->m = ppMult_qq(b->m, w);
          res->e = 1;
          res->f = smPolyWeight(res);
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
        res->m = ppMult_qq(b->m, w);
        res->e = 1;
        res->f = smPolyWeight(res);
        b = b->n;
      }
      else
      {
        ha = ppMult_qq(a->m, p);
        pDelete(&a->m);
        hb = ppMult_qq(b->m, w);
        ha = pAdd(ha, hb);
        if (ha != NULL)
        {
          a->m = ha;
          a->e = 1;
          a->f = smPolyWeight(a);
          res = res->n = a;
          a = a->n;
        }
        else
        {
          smElemDelete(&a);
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
    smElemDelete(&r);
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
  int e, ip, ir, ia, lev;

  if ((c == NULL) || (r == NULL))
  {
    while(r!=NULL) smElemDelete(&r);
    pDelete(&hp);
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
          x = SM_MULT(b->m, hr, m_res[ir]->m);
          b = b->n;
          if(ir) SM_DIV(x, m_res[ir]->m);
          res->m = x;
          res->e = e;
          res->f = smPolyWeight(res);
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
        x = SM_MULT(b->m, hr, m_res[ir]->m);
        b = b->n;
        if(ir) SM_DIV(x, m_res[ir]->m);
        res->m = x;
        res->e = e;
        res->f = smPolyWeight(res);
      }
      else
      {
        ha = a->m;
        ia = a->e;
        if (ir >= ia)
        {
          if (ir > ia)
          {
            x = SM_MULT(ha, m_res[ir]->m, m_res[ia]->m);
            pDelete(&ha);
            ha = x;
            if (ia) SM_DIV(ha, m_res[ia]->m);
            ia = ir;
          }
          x = SM_MULT(ha, gp, m_res[ia]->m);
          pDelete(&ha);
          y = SM_MULT(b->m, hr, m_res[ia]->m);
        }
        else if (ir >= ip)
        {
          if (ia < crd)
          {
            x = SM_MULT(ha, m_res[crd]->m, m_res[ia]->m);
            pDelete(&ha);
            ha = x;
            SM_DIV(ha, m_res[ia]->m);
          }
          y = hp;
          if(ir > ip)
          {
            y = SM_MULT(y, m_res[ir]->m, m_res[ip]->m);
            if (ip) SM_DIV(y, m_res[ip]->m);
          }
          ia = ir;
          x = SM_MULT(ha, y, m_res[ia]->m);
          if (y != hp) pDelete(&y);
          pDelete(&ha);
          y = SM_MULT(b->m, hr, m_res[ia]->m);
        }
        else
        {
          x = SM_MULT(hr, m_res[ia]->m, m_res[ir]->m);
          if (ir) SM_DIV(x, m_res[ir]->m);
          y = SM_MULT(b->m, x, m_res[ia]->m);
          pDelete(&x);
          x = SM_MULT(ha, gp, m_res[ia]->m);
          pDelete(&ha);
        }
        ha = pAdd(x, y);
        if (ha != NULL)
        {
          if (ia) SM_DIV(ha, m_res[ia]->m);
          a->m = ha;
          a->e = e;
          a->f = smPolyWeight(a);
          res = res->n = a;
          a = a->n;
        }
        else
        {
          a->m = NULL;
          smElemDelete(&a);
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
    smElemDelete(&r);
  } while (r != NULL);
  pDelete(&hp);
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
          a->m = pNeg(a->m);
          b = b->n = a;
          b->pos = i;
          break;
        }
      }
    }
    else if (a->pos == rpiv)
    {
      m_act[i] = a->n;
      a->m = pNeg(a->m);
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
      ha = SM_MULT(a->m, m_res[e]->m, m_res[f]->m);
      pDelete(&a->m);
      if (f) SM_DIV(ha, m_res[f]->m);
      a->m = ha;
      if (normalize) pNormalize(a->m);
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
        ha = SM_MULT(a->m, m_res[e]->m, m_res[f]->m);
        pDelete(&a->m);
        if (f) SM_DIV(ha, m_res[f]->m);
        a->m = ha;
      }
      if (normalize) pNormalize(a->m);
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
      if(smHaveDenom(a->m)) return 1;
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
      if (e == a->e) pNormalize(a->m);
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
    h = SM_MULT(h, m_res[crd]->m, m_res[f]->m);
    if (f) SM_DIV(h, m_res[f]->m);
    a->m = h;
    if (normalize) pNormalize(a->m);
    a->f = smPolyWeight(a);
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
      smElemDelete(&a);
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
    smElemDelete(&a);
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
    smElemDelete(&m_res[i]);
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
/*2
* exact division a/b
* a destroyed, b NOT destroyed
*/
void smPolyDiv(poly a, poly b)
{
  const number x = pGetCoeff(b);
  number y, yn;
  poly t, h, dummy;
  int i;

  if (pNext(b) == NULL)
  {
    do
    {
      if (!pLmIsConstantComp(b))
      {
        for (i=pVariables; i; i--)
          pSubExp(a,i,pGetExp(b,i));
        pSetm(a);
      }
      y = nDiv(pGetCoeff(a),x);
      nNormalize(y);
      pSetCoeff(a,y);
      pIter(a);
    } while (a != NULL);
    return;
  }
  dummy = pInit();
  do
  {
    for (i=pVariables; i; i--)
      pSubExp(a,i,pGetExp(b,i));
    pSetm(a);
    y = nDiv(pGetCoeff(a),x);
    nNormalize(y);
    pSetCoeff(a,y);
    yn = nNeg(nCopy(y));
    t = pNext(b);
    h = dummy;
    do
    {
      h = pNext(h) = pInit();
      //pSetComp(h,0);
      for (i=pVariables; i; i--)
        pSetExp(h,i,pGetExp(a,i)+pGetExp(t,i));
      pSetm(h);
      pSetCoeff0(h,nMult(yn, pGetCoeff(t)));
      pIter(t);
    } while (t != NULL);
    nDelete(&yn);
    pNext(h) = NULL;
    a = pNext(a) = pAdd(pNext(a), pNext(dummy));
  } while (a!=NULL);
  pLmFree(dummy);
}


//disable that, as it fails with coef buckets
//#define X_MAS
#ifdef X_MAS

/*
*  returns the part of (a*b)/exp(lead(c)) with nonegative exponents
*/
poly smMultDiv(poly a, poly b, const poly c)
{
  poly pa, e, res, r;
  BOOLEAN lead;\
  int la, lb, lr;

  if ((c == NULL) || pLmIsConstantComp(c))
  {
    return ppMult_qq(a, b);
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
  e = pInit();
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
      r = ppMult_mm(a, e);
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
      res = pAdd(res, r);
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
        r = ppMult_mm(a, e);
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
          pa = pAdd(pa,r);
      }
      else
      {
        r = ppMult_mm(a, e);
        smCombineChain(&pa, r);
      }
      pIter(b);
    } while (b != NULL);
  }
  pLmFree(e);
  return res;
}

#else

/*
*  returns the part of (a*b)/exp(lead(c)) with nonegative exponents
*/
poly smMultDiv(poly a, poly b, const poly c)
{
  poly pa, e, res, r;
  BOOLEAN lead;

  if ((c == NULL) || pLmIsConstantComp(c))
  {
    return ppMult_qq(a, b);
  }
  if (smSmaller(a, b))
  {
    r = a;
    a = b;
    b = r;
  }
  res = NULL;
  e = pInit();
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
      r = ppMult_mm(a, e);
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
      res = pAdd(res, r);
    pIter(b);
    if (b == NULL)
    {
      pLmFree(e);
      return res;
    }
  }
  do
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (smIsNegQuot(e, b, c))
    {
      r = smSelectCopy_ExpMultDiv(a, e, b, c);
      if (pLmDivisibleByNoComp(e, a))
        smCombineChain(&pa, r);
      else
        pa = pAdd(pa,r);
    }
    else
    {
      r = ppMult_mm(a, e);
      smCombineChain(&pa, r);
    }
    pIter(b);
  } while (b != NULL);
  pLmFree(e);
  return res;
}
#endif
/*n
* exact division a/b
* a is a result of smMultDiv
* a destroyed, b NOT destroyed
*/
void smSpecialPolyDiv(poly a, poly b)
{
  if (pNext(b) == NULL)
  {
    smPolyDivN(a, pGetCoeff(b));
    return;
  }
  smExactPolyDiv(a, b);
}


/* ------------ internals arithmetic ------------- */
static void smExactPolyDiv(poly a, poly b)
{
  const number x = pGetCoeff(b);
  poly tail = pNext(b), e = pInit();
  poly h;
  number y, yn;
  int lt = pLength(tail);

  if (lt + 1 >= SM_MIN_LENGTH_BUCKET &&  !TEST_OPT_NOT_BUCKETS)
  {
    kBucket_pt bucket = kBucketCreate(currRing);
    kBucketInit(bucket, pNext(a), 0);
    int lh = 0;
    do
    {
      y = nDiv(pGetCoeff(a), x);
      nNormalize(y);
      pSetCoeff(a,y);
      yn = nNeg(nCopy(y));
      pSetCoeff0(e,yn);
      lh = lt;
      if (smIsNegQuot(e, a, b))
      {
        h = pp_Mult_Coeff_mm_DivSelect_MultDiv(tail, lh, e, a, b);
      }
      else
        h = ppMult_mm(tail, e);
      nDelete(&yn);
      kBucket_Add_q(bucket, h, &lh);

      a = pNext(a) = kBucketExtractLm(bucket);
    } while (a!=NULL);
    kBucketDestroy(&bucket);
  }
  else
  {
    do
    {
      y = nDiv(pGetCoeff(a), x);
      nNormalize(y);
      pSetCoeff(a,y);
      yn = nNeg(nCopy(y));
      pSetCoeff0(e,yn);
      if (smIsNegQuot(e, a, b))
        h = smSelectCopy_ExpMultDiv(tail, e, a, b);
      else
        h = ppMult_mm(tail, e);
      nDelete(&yn);
      a = pNext(a) = pAdd(pNext(a), h);
    } while (a!=NULL);
  }
  pLmFree(e);
}

// obachman --> Wilfried: check the following
static BOOLEAN smIsNegQuot(poly a, const poly b, const poly c)
{
  if (pLmDivisibleByNoComp(c, b))
  {
    pExpVectorDiff(a, b, c);
    // Hmm: here used to be a pSetm(a): but it is unnecessary,
    // if b and c are correct
    return FALSE;
  }
  else
  {
    int i;
    for (i=pVariables; i>0; i--)
    {
      if(pGetExp(c,i) > pGetExp(b,i))
        pSetExp(a,i,pGetExp(c,i)-pGetExp(b,i));
      else
        pSetExp(a,i,0);
    }
    // here we actually might need a pSetm, if a is to be used in
    // comparisons
    return TRUE;
  }
}

static void smExpMultDiv(poly t, const poly b, const poly c)
{
  int i;
  pTest(t);
  pLmTest(b);
  pLmTest(c);
  poly bc = p_New(currRing);

  p_ExpVectorDiff(bc, b, c, currRing);

  while(t!=NULL)
  {
    pExpVectorAdd(t, bc);
    pIter(t);
  }
  p_LmFree(bc, currRing);
}


static void smPolyDivN(poly a, const number x)
{
  number y;

  do
  {
    y = nDiv(pGetCoeff(a),x);
    nNormalize(y);
    pSetCoeff(a,y);
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

static void smCombineChain(poly *px, poly r)
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
    i = pLmCmp(pb, r);
    if (i > 0)
      pa = pb;
    else
    {
      if (i == 0)
      {
        x = nAdd(pGetCoeff(pb), pGetCoeff(r));
        pLmDelete(&r);
        if (nIsZero(x))
        {
          pLmDelete(&pb);
          pNext(pa) = pAdd(pb,r);
        }
        else
        {
          pa = pb;
          pSetCoeff(pa,x);
          pNext(pa) = pAdd(pNext(pa), r);
        }
      }
      else
      {
        pa = pNext(pa) = r;
        pNext(pa) = pAdd(pb, pNext(pa));
      }
      break;
    }
  }
  *px = pa;
}


static void smFindRef(poly *ref, poly *px, poly r)
{
  number x;
  int i;
  poly pa = *px, pp = NULL;

  loop
  {
    i = pLmCmp(pa, r);
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
        x = nAdd(pGetCoeff(pa), pGetCoeff(r));
        pLmDelete(&r);
        if (nIsZero(x))
        {
          pLmDelete(&pa);
          if (pp!=NULL)
            pNext(pp) = pAdd(pa,r);
          else
            *px = pAdd(pa,r);
        }
        else
        {
          pp = pa;
          pSetCoeff(pp,x);
          pNext(pp) = pAdd(pNext(pp), r);
        }
      }
      else
      {
        if (pp!=NULL)
          pp = pNext(pp) = r;
        else
          *px = pp = r;
        pNext(pp) = pAdd(pa, pNext(r));
      }
      break;
    }
  }
  *ref = pp;
}

/* ----------------- internal 'C' stuff ------------------ */

static void smElemDelete(smpoly *r)
{
  smpoly a = *r, b = a->n;

  pDelete(&a->m);
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
static smpoly smPoly2Smpoly(poly q)
{
  poly pp;
  smpoly res, a;
  long x;

  if (q == NULL)
    return NULL;
  a = res = (smpoly)omAllocBin(smprec_bin);
  a->pos = x = pGetComp(q);
  a->m = q;
  a->e = 0;
  loop
  {
    pSetComp(q,0);
    pp = q;
    pIter(q);
    if (q == NULL)
    {
      a->n = NULL;
      return res;
    }
    if (pGetComp(q) != x)
    {
      a = a->n = (smpoly)omAllocBin(smprec_bin);
      pNext(pp) = NULL;
      a->pos = x = pGetComp(q);
      a->m = q;
      a->e = 0;
    }
  }
}

/*
* from smpoly to poly
* destroy a
*/
static poly smSmpoly2Poly(smpoly a)
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
    pSetComp(q,x);
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
      pSetComp(q,x);
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
static float smPolyWeight(smpoly a)
{
  poly p = a->m;
  int i;
  float res = (float)nSize(pGetCoeff(p));

  if (pNext(p) == NULL)
  {
    for(i=pVariables; i>0; i--)
    {
      if (pGetExp(p,i) != 0) return res+1.0;
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
      res += (float)nSize(pGetCoeff(p));
      pIter(p);
    }
    while (p);
    return res+(float)i;
  }
}

static BOOLEAN smHaveDenom(poly a)
{
  BOOLEAN sw;
  number x,o=nInit(1);

  while (a != NULL)
  {
    x = nGetDenom(pGetCoeff(a));
    sw = nEqual(x,o);
    nDelete(&x);
    if (!sw)
    {
      nDelete(&o);
      return TRUE;
    }
    pIter(a);
  }
  nDelete(&o);
  return FALSE;
}

static number smCleardenom(ideal id)
{
  poly a;
  number x,y,res=nInit(1);
  BOOLEAN sw=FALSE;

  for (int i=0; i<IDELEMS(id); i++)
  {
    a = id->m[i];
    sw = smHaveDenom(a);
    if (sw) break;
  }
  if (!sw) return res;
  for (int i=0; i<IDELEMS(id); i++)
  {
    a = id->m[i];
    if (a!=NULL)
    {
      x = nCopy(pGetCoeff(a));
      p_Cleardenom(a, currRing);
      y = nDiv(x,pGetCoeff(a));
      nDelete(&x);
      x = nMult(res,y);
      nNormalize(x);
      nDelete(&res);
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
static void smNumberDelete(smnumber *);
static smnumber smNumberCopy(smnumber);
static smnumber smPoly2Smnumber(poly);
static poly smSmnumber2Poly(number);
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
  number one;          // the "1"
  number *sol;         // field for solution
  int *wrw, *wcl;      // weights of rows and columns
  smnumber * m_act;    // unreduced columns
  smnumber * m_res;    // reduced columns (result)
  smnumber * m_row;    // reduced part of rows
  smnumber red;        // row to reduce
  smnumber piv;        // pivot
  smnumber dumm;       // allocated dummy
  void smColToRow();
  void smRowToCol();
  void smSelectPR();
  void smRealPivot();
  void smZeroToredElim();
  void smGElim();
  void smAllDel();
public:
  sparse_number_mat(ideal);
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
ideal smCallSolv(ideal I)
{
  sparse_number_mat *linsolv;
  ring origR;
  sip_sring tmpR;
  ideal rr;

  if (idIsConstant(I)==FALSE)
  {
    WerrorS("symbol in equation");
    return NULL;
  }
  I->rank = idRankFreeModule(I);
  if (smCheckSolv(I)) return NULL;
  smRingChange(&origR,tmpR,1);
  rr=idrCopyR(I,origR);
  linsolv = new sparse_number_mat(rr);
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
  rChangeCurrRing(origR);
  if (rr!=NULL)
    rr = idrMoveR(rr,&tmpR);
  smRingClean(origR,tmpR);
  return rr;
}

/*
* constructor, destroy smat
*/
sparse_number_mat::sparse_number_mat(ideal smat)
{
  int i;
  polyset pmat;

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
    m_act[i] = smPoly2Smnumber(pmat[i-1]);
  }
  omFreeSize((ADDRESS)pmat,smat->ncols*sizeof(poly));
  omFreeBin((ADDRESS)smat, sip_sideal_bin);
  one = nInit(1);
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
  nDelete(&one);
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
    sol[i] = nDiv(x, m_res[i]->m);
    nDelete(&x);
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
        z = nMult(sol[j], s->m);
        if (x != NULL)
        {
          y = x;
          x = nSub(y, z);
          nDelete(&y);
          nDelete(&z);
        }
        else
          x = nNeg(z);
      }
      s = s->n;
    }
    if (sol[i] != NULL)
    {
      if (x != NULL)
      {
        y = nAdd(x, sol[i]);
        nDelete(&x);
        if (nIsZero(y))
        {
          nDelete(&sol[i]);
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
      sol[i] = nDiv(x, d->m);
      nDelete(&x);
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
    res->m[j] = smSmnumber2Poly(sol[i]);
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

  xo=nInit(0);
  for (i=act; i; i--)
  {
    a = m_act[i];
    while ((a!=NULL) && (a->pos<=tored))
    {
      x = a->m;
      if (nGreaterZero(x))
      {
        if (nGreater(x,xo))
        {
          nDelete(&xo);
          xo = nCopy(x);
          copt = i;
          ropt = a->pos;
        }
      }
      else
      {
        xo = nNeg(xo);
        if (nGreater(xo,x))
        {
          nDelete(&xo);
          xo = nCopy(x);
          copt = i;
          ropt = a->pos;
        }
        xo = nNeg(xo);
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
  nDelete(&xo);
}

/* ----------------- elimination ------------------ */

/* one step of Gauss-elimination */
void sparse_number_mat::smGElim()
{
  number p = nDiv(one,piv->m);  // pivotelement
  smnumber c = m_act[act];      // pivotcolumn
  smnumber r = red;             // row to reduce
  smnumber res, a, b;
  number w, ha, hb;

  if ((c == NULL) || (r == NULL))
  {
    while (r!=NULL) smNumberDelete(&r);
    return;
  }
  do
  {
    a = m_act[r->pos];
    res = dumm;
    res->n = NULL;
    b = c;
    w = nMult(r->m, p);
    nDelete(&r->m);
    r->m = w;
    loop   // combine the chains a and b: a + w*b
    {
      if (a == NULL)
      {
        do
        {
          res = res->n = smNumberCopy(b);
          res->m = nMult(b->m, w);
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
        res->m = nMult(b->m, w);
        b = b->n;
      }
      else
      {
        hb = nMult(b->m, w);
        ha = nAdd(a->m, hb);
        nDelete(&hb);
        nDelete(&a->m);
        if (nIsZero(ha))
        {
          smNumberDelete(&a);
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
    smNumberDelete(&r);
  } while (r != NULL);
  nDelete(&p);
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
          a->m = nNeg(a->m);
          b = b->n = a;
          b->pos = i;
          break;
        }
      }
    }
    else if (a->pos == rpiv)
    {
      m_act[i] = a->n;
      a->m = nNeg(a->m);
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
      smNumberDelete(&a);
  }
  for (i=crd; i; i--)
  {
    a = m_res[i];
    while (a != NULL)
      smNumberDelete(&a);
  }
  if (act)
  {
    for (i=nrows; i; i--)
    {
      a = m_row[i];
      while (a != NULL)
        smNumberDelete(&a);
    }
  }
}

/* ----------------- internal 'C' stuff ------------------ */

static void smNumberDelete(smnumber *r)
{
  smnumber a = *r, b = a->n;

  nDelete(&a->m);
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
static smnumber smPoly2Smnumber(poly q)
{
  smnumber a, res;
  poly p = q;

  if (p == NULL)
    return NULL;
  a = res = (smnumber)omAllocBin(smnrec_bin);
  a->pos = pGetComp(p);
  a->m = pGetCoeff(p);
  nNew(&pGetCoeff(p));
  loop
  {
    pIter(p);
    if (p == NULL)
    {
      pDelete(&q);
      a->n = NULL;
      return res;
    }
    a = a->n = (smnumber)omAllocBin(smnrec_bin);
    a->pos = pGetComp(p);
    a->m = pGetCoeff(p);
    nNew(&pGetCoeff(p));
  }
}

/*
* from smnumber to poly
* destroy a
*/
static poly smSmnumber2Poly(number a)
{
  poly res;

  if (a == NULL) return NULL;
  res = pInit();
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
