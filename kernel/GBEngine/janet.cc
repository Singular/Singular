

#include "kernel/mod2.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif

#include "coeffs/numbers.h"

#include "polys/monomials/ring.h"
#include "polys/monomials/p_polys.h"
#include "polys/kbuckets.h"

#include "kernel/ideals.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/GBEngine/janet.h"


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

#if (defined(__CYGWIN__))
#include <ctype.h>
#endif


//------GLOBALS-------
static int /*m_s,v_s,vectorized,VarN1,*/offset;
static jList *T,*Q;
static TreeM *G;
// static Poly *phD;
static NodeM *FreeNodes;
static int degree_compatible;
static int (*ListGreatMove)(jList *,jList *,poly);
static int Mask[8]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};

//#define DebugPrint

//#define pow_(x) pTotaldegree((x))
//#define pow_(x) p_Deg((x,currRing))
pFDegProc jDeg;
#define pow_(x) jDeg((x),currRing)

#if 0
void Debug()
{
  LCI it=T->root;

  PrintS("T==================================\n");
  while (it)
  {
    pWrite(it->info->root);
    it=it->next;
  }

  it=Q->root;

  PrintS("Q==================================\n");
  while (it)
  {
    if (it->info->root) pWrite(it->info->root);
    else
    {
      Print("%d.........",it->info->prolonged);
      pWrite(it->info->history);
    }
    it=it->next;
  }
  PrintS("===================================\n");
}
#endif

int ReducePolyLead(Poly *x,Poly *y)
{
  if (!x->root || !y->root)
    return 0;

/*  poly b1=pMDivide(x->root,y->root);

  number gcd=n_Gcd(pGetCoeff(x->root),pGetCoeff(y->root),currRing->cf);

  number a1=nDiv(pGetCoeff(y->root),gcd);
  pGetCoeff(b1)=nDiv(pGetCoeff(x->root),gcd);

  x->root=pMult_nn(x->root,a1);
  nDelete(&a1);

  x->root=pMinus_mm_Mult_qq(x->root,b1,y->root);

  pDelete(&b1);
*/
#if 1
  if (x->root_b==NULL)
  {
    if (x->root_l<=0) x->root_l=pLength(x->root);
    x->root_b=kBucketCreate(currRing);
    kBucketInit(x->root_b,x->root,x->root_l);
  }
  number coef;
  if (y->root_l<=0) y->root_l=pLength(y->root);
  coef=kBucketPolyRed(x->root_b,y->root,y->root_l,NULL);
  nDelete(&coef);
  x->root=kBucketGetLm(x->root_b);
  if (x->root==NULL)
  {
    kBucketDestroy(&x->root_b);
    x->root_b=NULL;
    x->root_l=0;
  }
#else
  x->root=ksOldSpolyRed(y->root,x->root,NULL);
#endif
//  if (x->root) p_SimpleContent(x->root,5,currRing);

  return 1;
}

int ReducePoly(Poly *x,poly from,Poly *y)
{
  if (!x->root || !y->root)
    return 0;

/*  poly b1=pMDivide(from,y->root);

  number gcd=n_Gcd(pGetCoeff(from),pGetCoeff(y->root),currRing->cf);

  number a1=nDiv(pGetCoeff(y->root),gcd);
  pGetCoeff(b1)=nDiv(pGetCoeff(from),gcd);

  x->root=pMult_nn(x->root,a1);
  nDelete(&a1);*/

//  x->root=pMinus_mm_Mult_qq(x->root,b1,y->root);
//  pDelete(&b1);

  ksOldSpolyTail(y->root,x->root,from,NULL,currRing);
  y->root_l=0;

  return 1;
}

void PNF(Poly *p, TreeM *F)
{
  if (p->root==NULL) return;

  Poly *f;
  BOOLEAN done=FALSE;
  poly temp=p->root;

//  if (TEST_OPT_PROT) { PrintS("r"); mflush(); }
  int count=0;
  poly pp=p->root;
  int old_size=nSize(pGetCoeff(pp));
  p->root_l=0;
  while(temp->next)
  {
    f=is_div_(F,temp->next);
    if (f)
    {
      if (ReducePoly(p,temp,f)) //temp->next
      {
        count++;
        //if (TEST_OPT_PROT) { PrintS("-"); mflush(); }
        if ((f!=NULL)
        && (count>20)
        && (nSize(pGetCoeff(pp))>old_size)
        )
        {
           p_SimpleContent(pp,1,currRing);
           //p_Content(pp,currRing);
           count=0;
         //  old_size=nSize(pGetCoeff(pp));
        }
      }
      done=TRUE;
    }
    else
      temp=temp->next;
   }

  if (done) p_ContentForGB(p->root,currRing);
  //if (done) p_SimpleContent(p->root,-1,currRing);
  pTest(p->root);
}

void NFL(Poly *p, TreeM *F)
{
  Poly *f;
  // int g1,f1,gg;

  if ((f=is_div_(F,p->lead))==NULL) return;

  int pX=pow_(p->lead);
  int phX=pow_(p->history);

  if (pX!=phX)
  {
    int phF=pow_(f->history);
    if (pX >= (phX+phF))
    {
      pDelete(&p->root);
      //p->root=NULL;
      return;
    }

/*    poly p2=pInit();
    pLcm(p->history,f->history,p2);
    pSetm(p2);

    if (pLmCmp(p->root,p2) > 0)
    {
      pLmDelete(&p2);
      pDelete(&p->root);
      //p->root=NULL;
      return;
    }

    pLmDelete(&p2);
*/
/*    for(int i=0, gg=0 ; i<currRing->N;i++)
      if ((g1=pGetExp(p->history,i+1)) > (f1=pGetExp(f->history,i+1)))
        gg+=g1;
      else gg+=f1;

    if (pX > gg)
      {
        pDelete(&p->root);
        //x->root=NULL;
        return;
    }
*/
    int pF=pow_(f->lead);

    if ((pX == pF) && (pF == phF))
    {
      pLmFree(&f->history);
      if (p->history!=NULL)
        f->history=p_Copy_noCheck(p->history,currRing); /* cf of p->history is NULL */
    }
  }

  //if (TEST_OPT_PROT) { PrintS("R"); mflush(); }
  int /*old_size, */count;
  count=0;
  while(f && p->root)
  {
//    PrintS("R");
//    if (TEST_OPT_PROT) { PrintS("R"); mflush(); }
#if 0
    old_size=nSize(pGetCoeff(p->root));
#endif
    if (ReducePolyLead(p,f) == 0) break;
    if (p->root!=NULL)
    {
      count++;
#if 0
      if ((count>4) && (3<nSize(pGetCoeff(p->root)))
      && (nSize(pGetCoeff(p->root))>old_size))
      {
        p_SimpleContent(p->root,old_size,currRing);
        count=0;
      }
#else
      if (count>50)
      {
        kBucketClear(p->root_b,&p->root,&p->root_l);
        p_SimpleContent(p->root,2,currRing);
        kBucketInit(p->root_b,p->root,p->root_l);
        count=0;
        //PrintS(".");
      }
#endif
      f=is_div_(F,p->root);
    }
  }
#if 1
  if (p->root_b!=NULL)
  {
    kBucketClear(p->root_b,&p->root,&p->root_l);
    kBucketDestroy(&p->root_b);
    p->root_b=NULL;
  }
#endif

  if (!p->root)
    return;

  InitHistory(p);
  InitProl(p);
  InitLead(p);
  p->changed=1;

  p_ContentForGB(p->root,currRing);
  //p_SimpleContent(p->root,-1i,currRing);
  pTest(p->root);
}

int ValidatePoly(Poly *x, TreeM */*F*/)
{
  Poly /*f,*/*g;
  // int g1,f1;

  if (x->root) return 1;

  g=is_present(T,x->history); //it's a prolongation - do we have a parent ?

  if (!g)  return 0; //if not - kill him !

  poly lmX=pMDivide(x->lead,g->root);
  pSetCoeff0(lmX,nInit(1));

/*  if ((f=is_div_(F,lmX)) != NULL)
  {
    int pX=pow_(lmX);
    int phX=pow_(x->history);

    if (pX!=phX)
    {
      int phF=pow_(f->history);
      if (pX >= (phX+phF))
      {
        pLmDelete(&lmX);
        //x->root=NULL;
        return 0;
      }

      for(int i=0, gg=0 ; i<currRing->N;i++)
        if ((g1=pGetExp(x->history,i+1)) > (f1=pGetExp(f->history,i+1)))
          gg+=g1;
        else
          gg+=f1;

      if (pX > gg)
      {
        pLmDelete(&lmX);
        return 0;
      }
      int pF=pow_(f->root);

      if ((pX == pF) && (pF == phF))
        f->history=x->history;
    }
  }

  pLmDelete(&lmX);

*/
  x->root=pCopy(g->root);
  x->root_l=g->root_l;

  x->root=pMult(x->root,lmX);

  pTest(x->root);

  x->prolonged=-1;

  return 1;
}

Poly *NewPoly(poly p)
{
  Poly *beg=(Poly *)GCM(sizeof(Poly));

  beg->root=p;//(p == NULL ? pInit() : p);
  beg->root_b=NULL;
  beg->root_l=0;
  beg->history=NULL;//pInit();
  beg->lead=NULL;
  beg->mult=(char *)GCMA(sizeof(char)*2*offset);

  for (int i=0; i < currRing->N; i++)
  {
    ClearMult(beg,i);
    ClearProl(beg,i);
  };

  beg->prolonged=-1;

  return beg;
}

void DestroyPoly(Poly *x)
{
  pDelete(&x->root);
  pLmFree(&x->history);
  if (x->lead!=NULL) pLmFree(&x->lead);
  GCF(x->mult);
  GCF(x);
}

void ControlProlong(Poly *x)
{
  for (int i = 0; i< offset; i++)
  {
    (x->mult+offset)[i]&=~((x->mult)[i]);
//    if (!GetMult(x,i) && !GetProl(x,i))
//      ProlVar(x,i);
  }
}

void InitHistory(Poly *p)
{
  if (p->history) pLmFree(&p->history);
  p->history=pLmInit(p->root);
  p->changed=0;
}

void InitLead(Poly *p)
{
  if (p->lead!=NULL) pLmFree(&p->lead);
  p->lead=pLmInit(p->root);
  p->prolonged=-1;
}

void InitProl(Poly *p)
{
  memset(p->mult+offset,0,sizeof(char)*offset);
}

int GetMult(Poly *x,int i)
{
  return x->mult[i/8] & Mask[i%8];
}

void SetMult(Poly *x,int i)
{
  x->mult[i/8] |= Mask[i%8];
}

void ClearMult(Poly *x,int i)
{
  x->mult[i/8] &= ~Mask[i%8];
}

int GetProl(Poly *x, int i)
{
  return (x->mult+offset)[i/8] & Mask[i%8];
}

void SetProl(Poly *x, int i)
{
  (x->mult+offset)[i/8] |= Mask[i%8];
}

void ClearProl(Poly *x, int i)
{
  (x->mult+offset)[i/8] &= ~Mask[i%8];
}

int LengthCompare(poly p1,poly p2)
{
  do
  {
    if (p1 == NULL) return 1;
    if (p2 == NULL) return 0;
    pIter(p1);
    pIter(p2);
  }while(p1 && p2);
  return 1;
}

int ProlCompare(Poly *item1, Poly *item2)
{
  switch(pLmCmp(item1->lead,item2->lead))
  {
    case -1:
      return 1;

    case 1:
      return 0;

    default:
      if ((item1->root_l<=0)||(item2->root_l<=0))
        return LengthCompare(item1->root,item2->root);
      return item1->root_l<=item2->root_l;
  }
}

void ProlVar(Poly *temp,int i)
{
  Poly *Pr;

  if (!GetProl(temp,i) && !GetMult(temp,i))
  {
    Pr=NewPoly();
    SetProl(temp,i);

    Pr->prolonged=i;
    Pr->history=pLmInit(temp->history);
    Pr->lead=pLmInit(temp->lead);
    pIncrExp(Pr->lead,i+1);
    pSetm(Pr->lead);
    InitProl(temp);

     Pr->changed=0;
//    pTest(Pr->root);
      InsertInCount(Q,Pr);
   }
}

void DestroyListNode(ListNode *x)
{
  DestroyPoly(x->info);
  GCF(x);
}

ListNode* CreateListNode(Poly *x)
{
  ListNode* ret=(ListNode *)GCM(sizeof(ListNode));
  ret->info=x;
  ret->next=NULL;
  return ret;
}


Poly *FindMinList(jList *L)
{
  LI min=&(L->root);
  LI l;
  LCI xl;
  Poly *x;

  if (degree_compatible)
  {
    while ((*min) && ((*min)->info->root == NULL))
      min=&((*min)->next);
  }

  if (!(*min)) return NULL;

  l=&((*min)->next);

  while (*l)
  {
    if ((*l)->info->root != NULL)
    {
      if (ProlCompare((*l)->info,(*min)->info))
        min=l;
    }

    l=&((*l)->next);
  }
  x=(*min)->info;
  xl=*min;
  *min=(*min)->next;
  GCF(xl);

  return x;
}

void InsertInList(jList *x,Poly *y)
{
  ListNode *ins;
  LI ix=&(x->root);

  while (*ix)
  {
    if (pLmCmp(y->lead,(*ix)->info->lead) == -1)
      ix=(ListNode **)&((*ix)->next);
    else
      break;
  }

  ins=CreateListNode(y);
  ins->next=(ListNode *)(*ix);
  *ix=ins;
  return;
}

void InsertInCount(jList *x,Poly *y)
{
  ListNode *ins;
  LI ix=&(x->root);

  ins=CreateListNode(y);
  ins->next=(ListNode *)(*ix);
  *ix=ins;
  return;
}

int ListGreatMoveOrder(jList *A,jList *B,poly x)
{
  LCI y=A->root;

  if (!y || pLmCmp(y->info->lead,x) < 0) return 0;

  while(y && pLmCmp(y->info->lead,x) >= 0)
  {
    InsertInCount(B,y->info);
    A->root=y->next;
    GCF(y);
    y=A->root;
  }

  return 1;
}

int ListGreatMoveDegree(jList *A,jList *B,poly x)
{
  LCI y=A->root;
  int pow_x=pow_(x);

  if (!y || pow_(y->info->lead) <= pow_x) return 0;

  while(y && pow_(y->info->lead) > pow_x)
  {
    InsertInCount(B,y->info);
    A->root=y->next;
    GCF(y);
    y=A->root;
  }

  return 1;
}

int CountList(jList *Q)
{
  int i=0;
  LCI y=Q->root;

  while(y)
  {
    i++;
    y=y->next;
  }

  return i;
}

void NFListQ()
{
  LCI ll;
  int p,p1;
  LI l;

  do
  {
    if (!Q->root) break;

    ll=Q->root;

    p=pow_(Q->root->info->lead);

    while (ll)
    {
      int ploc=pow_(ll->info->lead);
      if (ploc < p) p=ploc;
      ll=ll->next;
    }

    p1=1;

    l=&(Q->root);

    while (*l)
    {
//      PrintS("*");
      int ploc=pow_((*l)->info->lead);

      if (ploc == p)
      {
        if (!ValidatePoly((*l)->info,G))
        {
          ll=(*l);
          *l=(*l)->next;
          DestroyListNode(ll);
          continue;
        };

        (*l)->info->changed=0;
//        PrintS("!");
        NFL((*l)->info,G);
//                                PrintS("$");
        if (!(*l)->info->root)
        {
          ll=(*l);
          *l=(*l)->next;
          DestroyListNode(ll);
          continue;
        };
        p1=0;
      }

      l=&((*l)->next);
    }
  }while(p1);
//  PrintLn();
}


void ForEachPNF(jList *x,int i)
{
  LCI y=x->root;

  while(y)
  {
    if (pow_(y->info->root) == i) PNF(y->info,G);
    y=y->next;
  }
}

void ForEachControlProlong(jList *x)
{
  LCI y=x->root;

  while(y)
  {
    ControlProlong(y->info);
    y=y->next;
  }
}

void DestroyList(jList *x)
{
  LCI y=x->root,z;

  while(y)
  {
    z=y->next;
    DestroyPoly(y->info);
    GCF(y);
    y=z;
  }

  GCF(x);
}

Poly* is_present(jList *F,poly x)
{
  LCI iF=F->root;
  while(iF)
    if (pLmCmp(iF->info->root,x) == 0)
      return iF->info;
    else iF=iF->next;

  return NULL;
}

int GB_length()
{
  LCI iT=T->root;
  int l=0;

  while(iT)
  {
    if (pow_(iT->info->lead) == pow_(iT->info->history))
      ++l;
    iT=iT->next;
  }

  return l;
}

static Poly *temp_l;

NodeM* create()
{
  NodeM *y;

  if (FreeNodes == NULL)
  {
    y=(NodeM *)GCM(sizeof(NodeM));
  }
  else
  {
    y=FreeNodes;
    FreeNodes=FreeNodes->left;
  }

  y->left=y->right=NULL;
  y->ended=NULL;
  return y;
}

void DestroyFreeNodes()
{
  NodeM *y;

  while((y=FreeNodes)!=NULL)
  {
    FreeNodes=FreeNodes->left;
    GCF(y);
  }
}

#if 0
static void go_right(NodeM *current,poly_function disp)
{
  if (current)
  {
    go_right(current->left,disp);
    if (current->ended) disp(current->ended);
    go_right(current->right,disp);
  }
}

void ForEach(TreeM *t,poly_function disp)
{
  go_right(t->root,disp);
}
#endif

void DestroyTree(NodeM *G)
{
  if (G)
  {
    DestroyTree(G->left);
    DestroyTree(G->right);
    G->left=FreeNodes;
    FreeNodes=G;
  }
}

void Define(TreeM **G)
{
  *G=(TreeM *)GCM(sizeof(TreeM));
  (*G)->root=create();
}

int sp_div(poly m1,poly m2,int from)
{

  if (pow_(m2) == 0 && pow_(m1)) return 0;

  for(int k=from; k < currRing->N; k++)
    if (pGetExp(m1,k+1) < pGetExp(m2,k+1)) return 0;

  return 1;
}

void div_l(poly item, NodeM *x,int from)
{
  if (x && !temp_l)
  {
    div_l(item,x->left,from);
    if ((x->ended) && sp_div(item,x->ended->root,from))
    {
      temp_l=x->ended;
      return;
    };
    div_l(item,x->right,from);
  }
}

Poly* is_div_upper(poly item, NodeM *x,int from)
{
  temp_l=NULL;
  div_l(item,x,from);
  return temp_l;
}

Poly* is_div_(TreeM *tree, poly item)
{
  int power_tmp,i,i_con=currRing->N-1;
  NodeM *curr=tree->root;

  if (!curr) return NULL;
  if (pow_(item) == 0) return NULL;

  for ( ; i_con>=0 && !pGetExp(item,i_con+1) ; i_con--)
    ;

  for (i=0; i <= i_con ; i++)
  {
    power_tmp=pGetExp(item,i+1);

    while (power_tmp)
    {
      if (curr->ended) return curr->ended;

      if (!curr->left)
      {
        if (curr->right)
          return is_div_upper(item,curr->right,i); //??????
        return NULL;
      }

      curr=curr->left;
      power_tmp--;
    }

    if (curr->ended) return curr->ended;

    if (!curr->right) return NULL;

    curr=curr->right;
  }

  if (curr->ended) return curr->ended;
  else return NULL;
}

static void ClearMultiplicative(NodeM *xx,int i)
{
  if (!xx) return;

  while (xx->left)
  {
    ClearMultiplicative(xx->right, i);
    xx = xx->left;
  }
  if ((xx->ended) && (GetMult(xx->ended,i)))
  {
    ClearMult(xx->ended,i);
    ProlVar(xx->ended,i);
  }
  else
    ClearMultiplicative(xx->right,i);
}
//======================================================
void insert_(TreeM **tree, Poly *item)
{
 int power_tmp,i,i_con=currRing->N-1;
 NodeM *curr=(*tree)->root;

 for ( ; (i_con>=0) && !pGetExp(item->root,i_con+1) ; i_con--)
  SetMult(item,i_con);

 for (i = 0; i<= i_con; i++)
 //<=
 {
  power_tmp=pGetExp(item->root,i+1);

  ClearMult(item,i);

  while (power_tmp)
  {
   if (!curr->left)
   {
     SetMult(item,i);
     ClearMultiplicative(curr->right,i);
     curr->left=create();
   };
   curr=curr->left;
   power_tmp--;
  };

  if (i<i_con)
  {
   if (!curr->left) SetMult(item,i);
   if (!curr->right) curr->right=create();
   curr=curr->right;

   ProlVar(item,i);
  }
 }

 curr->ended=item;
}

void Initialization(char *Ord)
{
  offset=(currRing->N % 8 == 0) ? (currRing->N/8)*8 : (currRing->N/8+1)*8;
  if (strstr(Ord,"dp\0") || strstr(Ord,"Dp\0"))
  {
    degree_compatible=1;
    jDeg=p_Deg;
    ListGreatMove=ListGreatMoveDegree;
  }
  else
  {
    degree_compatible=0;
    jDeg=p_Totaldegree;
    ListGreatMove=ListGreatMoveOrder;
  }

  Define(&G);
}

static Poly *h/*,*f*/;

#if 0
void insert_in_G(Poly *x)
{
 insert_(&G,x);
}
#endif

void T2G();

#if 0
void Q2TG()
{
  LCI t;
  Poly *x;

  while (Q->root)
  {
    t=Q->root;
    x=t->info;
    insert_(&G,x);
    InsertInList(T,x);
    Q->root=t->next;
    GCF(t);
  }
}
#endif

int ComputeBasis(jList *_lT,jList *_lQ)
{
  // int gb_l,i,ret_value=1;

  T=_lT; Q=_lQ;

//  Debug();

  while((h=FindMinList(Q))!=NULL)
  {
//        PrintS("New element\n");
//  Debug();

        if (!degree_compatible)
        {
          if (!ValidatePoly(h,G))
          {
            DestroyPoly(h);
            continue;
          }

          h->changed=0;

          NFL(h,G);

          if (!h->root)
          {
            DestroyPoly(h);
            continue;
          }
        }

        if (h->root)
        {
          if (pIsConstant(h->root))
          {
            WarnS("Constant in basis\n");
            return 0;
          }

          if (h->changed && ListGreatMove(T,Q,h->root))
          {
//      PrintS("<-\n");
            DestroyTree(G->root);
            G->root=create();
            T2G();
          }
        }

//  PrintS("PNF\n");
        PNF(h,G);
//        Print("{%d}\n",pow_(h->root));
        insert_(&G,h);
        InsertInList(T,h);

//  PrintS("For each PNF\n");
        if (degree_compatible)
            ForEachPNF(T,pow_(h->root));

//  PrintS("Control of prolongations\n");
        if (h->changed)
            ForEachControlProlong(T);
        else
            ControlProlong(h);

//  Debug();

//  PrintS("NFListQ\n");
        if (degree_compatible)
            NFListQ();
//Debug();
    }

//    gb_l=GB_length();

    Print("Length of Janet basis: %d\n",CountList(T));
//    Print("Length of Groebner basis:    %d\n",gb_l);

    DestroyTree(G->root);
    GCF(G);
    DestroyFreeNodes();

    return 1;
}

void T2G()
{
 LCI i=T->root;
 while (i)
 {
  insert_(&G,i->info);
  i=i->next;
 }
}
