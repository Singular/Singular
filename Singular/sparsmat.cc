#include "mod2.h"
#include "tok.h"
#include "febase.h"
#include "structs.h"
#include "intvec.h"
#include "lists.h"
#include "polys.h"
#include "ipid.h"
#include "ideals.h"
#include "numbers.h"
#include "ring.h"
#include "sparsmat.h"

/* ----------------- macros ------------------ */
#define OLD_DIV
#ifdef OLD_DIV
#define SM_MULT(A,B,C) smMult(A,B)
#define SM_DIV smPolyDiv
#else
#define SM_MULT smMultDiv
#define SM_DIV smSpecialPolyDiv
#endif
/* ----------------- general definitions ------------------ */

/* elements of a 'sparse' matrix in SINGULAR */
typedef struct smprec sm_prec;
typedef sm_prec * smpoly;
struct smprec{
  smpoly n;            // the next element
  int pos;             // position
  int e;               // level
  poly m;              // the element
  float f;             // complexity of the element
};

/* declare internal 'C' stuff */
static void smExactPolyDiv(poly, poly);
static BOOLEAN smIsScalar(const poly);
static BOOLEAN smIsNegQuot(poly, const poly, const poly);
static poly smEMult(poly, const poly);
static poly smDMult(poly, const poly);
static void smPolyDivN(poly a, const number);
static BOOLEAN smSmaller(poly, poly);
static void smCombineChain(poly *, poly);

static void smElemDelete(smpoly *);
static smpoly smElemCopy(smpoly);
static float smPolyWeight(smpoly);
static smpoly smPoly2Smpoly(poly p);
static poly smSmpoly2Poly(smpoly a);

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
  int rpiv, cpiv;      // position of the pivot
  unsigned short *perm;// permutation of rows
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
  void smFinalToCol();
  void smFinalClear();
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
public:
  sparse_mat(ideal);
  ~sparse_mat();
  smpoly * smGetAct() { return m_act; }
  ideal smRes2Mod();
  void smBareiss(int, int);
  void smNewBareiss(int, int);
  void smToIntvec(intvec *);
};

/* ----------------- basics (used from 'C') ------------------ */

lists smCallBareiss(ideal I, int x, int y)
{
  lists res=(lists)Alloc(sizeof(slists));
  sparse_mat *bareiss = new sparse_mat(I);
  intvec *v;
  ideal m;

  if (bareiss->smGetAct() == NULL)
  {
    Free((ADDRESS)res, sizeof(slists));
    return NULL;
  }
  bareiss->smBareiss(x, y);
  m = bareiss->smRes2Mod();
  v = new intvec(m->rank);
  bareiss->smToIntvec(v);
  delete bareiss;
  res->Init(2);
  res->m[0].rtyp=MODUL_CMD;
  res->m[0].data=(void *)m;
  res->m[1].rtyp=INTVEC_CMD;
  res->m[1].data=(void *)v;
  return res;
}

lists smCallNewBareiss(ideal I, int x, int y)
{
  lists res=(lists)Alloc(sizeof(slists));
  ring origR =NULL;
  sip_sring tmpR;
  ideal II;
  if (currRing->order[0]!=ringorder_c)
  {
    origR =currRing;
    tmpR=*origR;
    int *ord=(int*)Alloc0(3*sizeof(int));
    int *block0=(int*)Alloc(3*sizeof(int));
    int *block1=(int*)Alloc(3*sizeof(int));
    ord[0]=ringorder_c;
    ord[1]=ringorder_dp;
    tmpR.order=ord;
    block0[1]=1;
    tmpR.block0=block0;
    block1[1]=tmpR.N;
    tmpR.block1=block1;
    rComplete(&tmpR,1);
    rChangeCurrRing(&tmpR,TRUE);
    // fetch data from the old ring
    II=idInit(IDELEMS(I),I->rank);
    int k;
    for (k=0;k<IDELEMS(I);k++) II->m[k] = pFetchCopy(origR, I->m[k]);
  }
  else
  {
    II=idCopy(I);
  }
  sparse_mat *bareiss = new sparse_mat(II);
  intvec *v;
  ideal m;

  if (bareiss->smGetAct() == NULL)
  {
    Free((ADDRESS)res, sizeof(slists));
    if (origR!=NULL)
    {
      rChangeCurrRing(origR,TRUE);
      rUnComplete(&tmpR);
      Free((ADDRESS)tmpR.order,3*sizeof(int));
      Free((ADDRESS)tmpR.block0,3*sizeof(int));
      Free((ADDRESS)tmpR.block1,3*sizeof(int));
    }
    return NULL;
  }
  bareiss->smNewBareiss(x, y);
  m = bareiss->smRes2Mod();
  v = new intvec(m->rank);
  bareiss->smToIntvec(v);
  delete bareiss;
  ideal mm;
  if (origR!=NULL)
  {
    rChangeCurrRing(origR,TRUE);
    mm=idInit(IDELEMS(m),m->rank);
    int k;
    for (k=0;k<IDELEMS(m);k++) mm->m[k] = pFetchCopy(origR, m->m[k]);
    rChangeCurrRing(&tmpR,FALSE);
    idDelete(&m);
    rChangeCurrRing(origR,TRUE);
    rUnComplete(&tmpR);
    Free((ADDRESS)tmpR.order,3*sizeof(int));
    Free((ADDRESS)tmpR.block0,3*sizeof(int));
    Free((ADDRESS)tmpR.block1,3*sizeof(int));
  }
  else
  {
    mm=m;
  }
  res->Init(2);
  res->m[0].rtyp=MODUL_CMD;
  res->m[0].data=(void *)mm;
  res->m[1].rtyp=INTVEC_CMD;
  res->m[1].data=(void *)v;
  return res;
}

/*
* constructor
*/
sparse_mat::sparse_mat(ideal smat)
{
  int i;

  ncols = smat->ncols;
  nrows = idRankFreeModule(smat);
  if (nrows <= 0)
  {
    m_act = NULL;
    WerrorS("module expected");
    return;
  }
  sign = 1;
  act = ncols;
  crd = 0;
  tored = nrows; // without border
  i = tored+1;
  perm = (unsigned short *)Alloc(sizeof(unsigned short)*i);
  wrw = (float *)Alloc(sizeof(float)*i);
  i = ncols+1;
  wcl = (float *)Alloc(sizeof(float)*i);
  m_act = (smpoly *)Alloc(sizeof(smpoly)*i);
  m_row = (smpoly *)Alloc0(sizeof(smpoly)*i);
  m_res = (smpoly *)Alloc0(sizeof(smpoly)*i);
  dumm = (smpoly)Alloc(sizeof(smprec));
  m_res[0] = (smpoly)Alloc(sizeof(smprec));
  m_res[0]->m = NULL;
  for(i=ncols; i; i--) m_act[i] = smPoly2Smpoly(smat->m[i-1]);
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
  Free((ADDRESS)m_res[0], sizeof(smprec));
  Free((ADDRESS)dumm, sizeof(smprec));
  i = ncols+1;
  Free((ADDRESS)m_res, sizeof(smpoly)*i);
  Free((ADDRESS)m_row, sizeof(smpoly)*i);
  Free((ADDRESS)m_act, sizeof(smpoly)*i);
  Free((ADDRESS)wcl, sizeof(float)*i);
  i = nrows+1;
  Free((ADDRESS)wrw, sizeof(float)*i);
  Free((ADDRESS)perm, sizeof(unsigned short)*i);
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
  if (y < 2) y = 2;
  if (act < y)
  {
    if (act != 0)
    {
      oldpiv = NULL;
      this->smCopToRes();
    }
    return;
  }
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
      if (act != 0)
      {
        this->smCopToRes();
      }
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
  if (y < 2) y = 2;
  if (act < y)
  {
    if (act != 0)
    {
      oldpiv = NULL;
      this->smCopToRes();
    }
    return;
  }
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
  if (act < y)
  {
    this->smFinalMult();
    this->smCopToRes();
    return;
  }
  loop
  {
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
    if (act < y)
    {
      if (act != 0)
      {
        this->smFinalMult();
        this->smCopToRes();
      }
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
      do
      {
        ha = SM_MULT(a->m, p, q);
        pDelete(&a->m);
        if (q) SM_DIV(ha, q);
        a->m = ha;
        a = a->n;
      } while (a != NULL);
    }
    return;
  }
  for (i=1; i<act; i++)
  {
    a = m_act[i];
    if ((r == NULL) || (i != r->pos))  // cols without elimination
    {
      do
      {
        ha = SM_MULT(a->m, p, q);
        pDelete(&a->m);
        if (q) SM_DIV(ha, q);
        a->m = ha;
        a = a->n;
      } while (a != NULL);
    }
    else                    // cols with elimination
    {
      res = dumm;
      res->n = NULL;
      b = c;
      w = r->m;
      loop                  // combine the chains a and b: p*a + w*b
      {
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
    return; 
  do
  {
    a = m_act[r->pos];
    res = dumm;
    res->n = NULL;
    b = c;
    w = r->m;
    loop                  // combine the chains a and b: p*a + w*b
    {
      if (a->pos < b->pos)
      {
        res = res->n = a;
        a = a->n;
      }
      else if (a->pos > b->pos)
      {
        res = res->n = smElemCopy(b);
        res->m = smMult(b->m, w);
        res->e = 1;
        res->f = smPolyWeight(res);
        b = b->n;
      }      
      else
      {
        ha = smMult(a->m, p);
        pDelete(&a->m);
        hb = smMult(b->m, w);
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
      if (a == NULL)
      {
        do
        {
          res = res->n = smElemCopy(b);
          res->m = smMult(b->m, w);
          res->e = 1;
          res->f = smPolyWeight(res);
          b = b->n;
        } while (b != NULL);
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
  poly ha, hr, gc, x, y;
  int e, ip, ir, ia, lev;

  if ((c == NULL) || (r == NULL))
  {
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
        y = NULL;
        x = hr;
        ha = a->m;
        ia = a->e;
        if (ir > ia)
        {
          gc = SM_MULT(ha, m_res[ir]->m, m_res[ia]->m);
          pDelete(&ha);
          ha = gc;
          if (ia) SM_DIV(ha, m_res[ia]->m);
          ia = ir;
        }
        else if (ir < ia)
        {
          if (ip < ia)
          {
            gc = SM_MULT(ha, m_res[crd]->m, m_res[ia]->m);
            pDelete(&ha);
            ha = gc;
            if (ia) SM_DIV(ha, m_res[ia]->m);
            y = hp;
            ia = ip;
            if (ir > ia)
            {
              y = SM_MULT(y, m_res[ir]->m, m_res[ia]->m);
              if (ia) SM_DIV(y, m_res[ia]->m);
              ia = ir;
            }
            else if (ir < ia)
            {
              x = SM_MULT(x, m_res[ia]->m, m_res[ir]->m);
              if (ir) SM_DIV(x, m_res[ir]->m);
            }
          }
          else
          {
            x = SM_MULT(x, m_res[ia]->m, m_res[ir]->m);
            if (ir) SM_DIV(x, m_res[ir]->m);
          }
        }
        if (y == NULL) y = gp;
        gc = SM_MULT(ha, y, m_res[ia]->m);
        pDelete(&ha);
        x = SM_MULT(x, b->m, m_res[ia]->m);
        x = pAdd(x, gc);
        if (x != NULL)
        {
          if (ia) SM_DIV(x, m_res[ia]->m);
          a->m = x;
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

/*
* store red, the pivot and the assosiated row in m_row
* to m_res (result):
*   piv + m_row[rows][pos(cols)] => m_res[cols][pos(rows)]
*/
void sparse_mat::smFinalToCol()
{
  smpoly r, a, ap, h;

  perm[crd] = rpiv;
  piv->pos = crd;
  ap = m_res[crd];
  if (ap == NULL)
    m_res[crd] = piv;
  else
  {
    loop
    {
      a = ap->n;
      if (a == NULL)
      {
        ap->n = piv;
        break;
      }
      ap = a;
    }
  }
  r = m_row[rpiv];
  m_row[rpiv] = NULL;
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
  r = red;
  while (r != NULL)
  {
    ap = m_res[r->pos];
    if (ap == NULL)
    {
      m_res[r->pos] = h = r;
      r = r->n;
      h->n = NULL;
      h->pos = crd;
    }
    else
    {
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
}

/*
* final clear of m_row
*/
void sparse_mat::smFinalClear()
{
  int i;
  smpoly r, a, ap, h;

  for (i=nrows; i; i--)
  {
    if (m_row[i] != NULL)
    {
      crd++;
      perm[crd] = i;
      r = m_row[i];
      while (r != NULL)
      {
        ap = m_res[r->pos];
        if (ap == NULL)
        {
          m_res[r->pos] = h = r;
          r = r->n;
          h->n = NULL;
          h->pos = crd;
        }
        else
        {
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
      m_res[crd] = m_act[i];
      crd++;
      break;
    }
  }
  j = i;
  loop
  {
    j++;
    if (j > act) break;
    if (m_act[i]->pos > tored)
    {
      m_res[crd] = m_act[i];
      crd++;
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
  while (act != 0)
  {
    rpiv = m_act[act]->pos;
    this->smSelectPR();
    crd++;
    this->smColToRow();
    act--;
    this->smFinalToCol();
    if (act != 0) this->smZeroElim();
  }
//  this->smFinalClear();
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
    }
    a = a->n;
  }
}

/*
* multiply and divide the m_act finaly
*/
void sparse_mat::smFinalMult()
{
  smpoly a = m_act[act];
  int e = crd;
  poly ha;
  int i, f;

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
    a->f = smPolyWeight(a);
    return r;
  }
  else
    return NULL;
}

/* ----------------- arithmetic ------------------ */

/*
*  returns a*b
*  a,b NOT destroyed
*/
poly smMult(poly a, poly b)
{
  poly pa, res, r;

  if (smSmaller(a, b))
  {
    r = a;
    a = b;
    b = r;
  }
  if (pNext(b) == NULL)
  {
    if (smIsScalar(b))
      return pMultCopyN(a, pGetCoeff(b));
    else
      return smEMult(a, b);
  }
  pa = res = smEMult(a, b);
  pIter(b);
  do
  {
    r = smEMult(a, b);
    smCombineChain(&pa, r);
    pIter(b);
  } while (b != NULL);
  return res;
}

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
      if (!smIsScalar(b))
      {
        for (i=pVariables; i; i--)
          pSubExp(a,i,pGetExp(b,i));
        pSetm(a);
      }
      y = nIntDiv(pGetCoeff(a),x);
      pSetCoeff(a,y);
      pIter(a);
    } while (a != NULL);
    return;
  }
  dummy = pNew();
  do
  {
    for (i=pVariables; i; i--)
      pSubExp(a,i,pGetExp(b,i));
    pSetm(a);
    y = nIntDiv(pGetCoeff(a),x);
    pSetCoeff(a,y);
    yn = nNeg(nCopy(y));
    t = pNext(b);
    h = dummy;
    do
    {
      h = pNext(h) = pNew();
      pSetComp(h,0);
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
  pFree1(dummy);
}

/*
*  returns the part of (a*b)/exp(lead(c)) with nonegative exponents
*/
poly smMultDiv(poly a, poly b, const poly c)
{
  poly pa, e, res, r;

  if (smSmaller(a, b))
  {
    r = a;
    a = b;
    b = r;
  }
  if ((c == NULL) || smIsScalar(c))
  {
    if (pNext(b) == NULL)
    {
      if (smIsScalar(b))
        return pMultCopyN(a, pGetCoeff(b));
      else
        return smEMult(a, b);
    }
    pa = res = smEMult(a, b);
    pIter(b);
    do
    {
      r = smEMult(a, b);
      smCombineChain(&pa, r);
      pIter(b);
    } while (b != NULL);
    return res;
  }
  e = pNew();
  loop
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (smIsNegQuot(e, b, c))
      res = smDMult(a, e);
    else
      res = smEMult(a, e);
    pIter(b);
    if (b == NULL)
    {
      pFree1(e);
      return res;
    }
    if (res != NULL) break;
  }
  do
  {
    pSetCoeff0(e,pGetCoeff(b));
    if (smIsNegQuot(e, b, c))
    {
      r = smDMult(a, e);
      if (r != NULL)
        res = pAdd(res,r); 
    }
    else
    {
      r = smEMult(a, e);
      res = pAdd(res,r);
    }
    pIter(b);
  } while (b != NULL);
  pFree1(e);
  return res;
}

/*2
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
  poly tail = pNext(b), e = pNew();
  poly h;
  number y, yn;

  do
  {
    y = nIntDiv(pGetCoeff(a), x);
    pSetCoeff(a,y);
    yn = nNeg(nCopy(y));
    pSetCoeff0(e,yn);
    if (smIsNegQuot(e, a, b)) 
      h = smDMult(tail, e);
    else
      h = smEMult(tail, e);
    nDelete(&yn);
    a = pNext(a) = pAdd(pNext(a), h);
  } while (a!=NULL);
  pFree1(e);
}

static BOOLEAN smIsScalar(const poly Term)
{
  int i;

  for (i=pVariables; i; i--)
  {
    if (pGetExp(Term,i)) return FALSE;
  }
  return TRUE;
}

static BOOLEAN smIsNegQuot(poly a, const poly b, const poly c)
{
  int i;

  for (i=pVariables; i; i--)
  {
    pSetExp(a,i,pGetExp(b,i)-pGetExp(c,i));
    if (pGetExp(a,i) < 0)
    {
      while(--i) pSetExp(a,i,pGetExp(b,i)-pGetExp(c,i));
      return TRUE;
    }
  }
  return FALSE;
}

static poly smEMult(poly t, const poly e)
{
  const number y = pGetCoeff(e);
  poly res, h;
  int i;

  h = res = pNew();
  loop
  {
    pSetComp(h,0);
    for (i=pVariables; i; i--)
      pSetExp(h,i,pGetExp(e,i)+pGetExp(t,i));
    pSetm(h);
    pSetCoeff0(h,nMult(y,pGetCoeff(t)));
    pIter(t);
    if (t == NULL)
    {
      pNext(h) = NULL;
      return res;
    }
    h = pNext(h) = pNew();
  }
}

static poly smDMult(poly t, const poly e)
{
  const number y = pGetCoeff(e);
  number x;
  poly res, h, r;
  int i;
  EXPONENT_TYPE w;

  r = h = res = pNew();
  loop
  {
    x = pGetCoeff(t);
    for (i=pVariables; i; i--)
    {
      w = pGetExp(e,i)+pGetExp(t,i);
      if (w < 0) break;
      pSetExp(h,i,w);
    }
    pIter(t);
    if (i == 0)
    {
      pSetComp(h,0);
      pSetm(h);
      pSetCoeff0(h,nMult(y,x));
      if (t == NULL)
      {
        pNext(h) = NULL;
        return res;
      }
      r = h;
      h = pNext(h) = pNew();
    }
    else if (t == NULL)
    {
      if (r != h)
        pNext(r) = NULL;
      else
        res = NULL;
      pFree1(h);
      return res;
    }
  }
}

static void smPolyDivN(poly a, const number x)
{
  number y;

  do
  {
    y = nIntDiv(pGetCoeff(a),x);
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
    i = pComp0(pb, r);
    if (i > 0)
      pa = pb;
    else
    {
      if (i == 0)
      {
        x = nAdd(pGetCoeff(pb), pGetCoeff(r));
        pDelete1(&r);
        if (nIsZero(x))
        {
          pDelete1(&pb);
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

/* ----------------- internal 'C' stuff ------------------ */

static void smElemDelete(smpoly *r)
{
  smpoly a = *r, b = a->n;

  pDelete(&a->m);
  Free((void *)a, sizeof(smprec));
  *r = b;
}

static smpoly smElemCopy(smpoly a)
{
  smpoly r = (smpoly)Alloc(sizeof(smprec));

  memcpy(r, a, sizeof(smprec));
/*  r->m = pCopy(r->m); */
  return r;
}

/*
* from poly to smpoly
* do not destroy p
*/
static smpoly smPoly2Smpoly(poly p)
{
  poly pp, q;
  smpoly res, a;
  EXPONENT_TYPE x;

  if (p == NULL)
    return NULL;
  q = pCopy(p);
  a = res = (smpoly)Alloc(sizeof(smprec));
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
      a = a->n = (smpoly)Alloc(sizeof(smprec));
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
  EXPONENT_TYPE x;

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
    Free((void *)b, sizeof(smprec));
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
