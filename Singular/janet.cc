#include "mod2.h"
#include "febase.h"
#include "janet.h"
#include "polys.h"
#include "numbers.h"
#include "ring.h"
#include "ideals.h"
#include "subexpr.h"
#include "longrat.h"
#include "kutil.h"

//#define DebugPrint

#define pow_(x) pTotaldegree((x))

static int ProdCrit, ChainCrit;

void Debug()
{
	LCI it=T->root;

	Print("T==================================\n");
	while (it)
	{
		pWrite(it->info->root);
		it=it->next;
	}

	it=Q->root;

	Print("Q==================================\n");
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
	Print("===================================\n");
}

int ReducePolyLead(Poly *x,Poly *y)
{
	if (!x->root || !y->root)
		return 0;

/*	poly b1=pDivide(x->root,y->root);

	number gcd=nGcd(pGetCoeff(x->root),pGetCoeff(y->root),currRing);

	number a1=nDiv(pGetCoeff(y->root),gcd);
	pGetCoeff(b1)=nDiv(pGetCoeff(x->root),gcd);

	x->root=pMult_nn(x->root,a1);
        nDelete(&a1);

	x->root=pMinus_mm_Mult_qq(x->root,b1,y->root);

	pDelete(&b1);*/

	x->root=ksOldSpolyRed(y->root,x->root,NULL);	

//	if (x->root) pContent(x->root);

	return 1;
}

int ReducePoly(Poly *x,poly from,Poly *y)
{
	if (!x->root || !y->root)
		return 0;

/*	poly b1=pDivide(from,y->root);

	number gcd=nGcd(pGetCoeff(from),pGetCoeff(y->root),currRing);

	number a1=nDiv(pGetCoeff(y->root),gcd);
	pGetCoeff(b1)=nDiv(pGetCoeff(from),gcd);

	x->root=pMult_nn(x->root,a1);
	nDelete(&a1);*/

//	x->root=pMinus_mm_Mult_qq(x->root,b1,y->root);
//	pDelete(&b1);

	ksOldSpolyTail(y->root,x->root,from,NULL,currRing);

	return 1;
}

void PNF(Poly *p, TreeM *F)
{
	Poly *f;

	if (!p->root) return;

	poly temp=p->root;

	while(temp->next)
 	{
  		f=is_div_(F,temp->next);
  		if (f)
		{
		 	if (ReducePoly(p,temp,f)) //temp->next
			{
				;
			}
		}	 
		else
			temp=temp->next;
 	};

//	pCleardenom(p->root);
	pContent(p->root);
	pTest(p->root);
}

void NFL(Poly *p, TreeM *F)
{
	Poly *f;
	int g1,f1,gg;


	if ((f=is_div_(F,p->lead))==NULL) return;

	int pX=pow_(p->lead);
	int phX=pow_(p->history);
		
 	if (pX!=phX)
 	{
 		int phF=pow_(f->history);
		if (!rIsPluralRing(currRing))
		{
		   /* Product Criterion */
		  if (pX >= (phX+phF))
		  {
		    //		  Print("ProdCrit Works!");
		    ProdCrit++;
		    pDelete(&p->root);
		    p->root=NULL;
		    return;
		  }
		}

		int gg=0; int i=0;
		for(i=0; i<currRing->N;i++)
		{
		  g1=pGetExp(p->history,i+1);
		  f1=pGetExp(f->history,i+1);
		  if (g1 > f1)  { gg=gg+g1; }
		  else { gg=gg+f1; }
		}
		if (pX > gg)
  		{		  
		  //		  Print("ChainCrit Works!");
		  ChainCrit++;
		  pDelete(&p->root);
		  //x->root=NULL;
		  return;
		}
		int pF=pow_(f->lead);

		if ((pX == pF) && (pF == phF))
		{
			pLmDelete(&f->history);
	    		f->history=pCopy(p->history);
		}
	}

	while(f && p->root)
	{
//		Print("R");
		if (ReducePolyLead(p,f) == 0) break;
		if (p->root) f=is_div_(F,p->root);
	}

 	if (!p->root) 
 		return;

 	InitHistory(p);
 	InitProl(p);
	InitLead(p);
 	p->changed=1;

//	pCleardenom(p->root);
 	pContent(p->root);
	pTest(p->root);
}

int ValidatePoly(Poly *x, TreeM *F)
{
	Poly *f,*g;
	int g1,f1;

	if (x->root) return 1;
 
	g=is_present(T,x->history); //it's a prolongation - do we have a parent ?

 	if (!g)  return 0; //if not - kill him !

	poly lmX=pDivide(x->lead,g->root);
	pSetCoeff(lmX,nInit(1));

/*	if ((f=is_div_(F,lmX)) != NULL)
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
				else gg+=f1;  

			if (pX > gg)
  			{
  				pLmDelete(&lmX);
				//x->root=NULL;
  				return 0;
  			}
			int pF=pow_(f->root);

			if ((pX == pF) && (pF == phF))
		    		f->history=x->history;
		}
	}

	pLmDelete(&lmX);

*/	x->root=pCopy(g->root);

	x->root=pMult(lmX,x->root);

	pTest(x->root);

	x->prolonged=-1;

	return 1;
}

Poly *NewPoly(poly p=NULL)
{
	Poly *beg=(Poly *)GCM(sizeof(Poly));

	beg->root=p;//(p == NULL ? pInit() : p);
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
	pDelete(&x->history);
	if (x->lead) pDelete(&x->lead);
	GCF(x->mult);
	GCF(x);
}

void ControlProlong(Poly *x)
{
	for (int i = 0; i< offset; i++)
	{
		(x->mult+offset)[i]&=~((x->mult)[i]);
//		if (!GetMult(x,i) && !GetProl(x,i))
//			ProlVar(x,i);
	}
}

void InitHistory(Poly *p)
{
	if (p->history) pDelete(&p->history);
	p->history=pLmInit(p->root);
	p->changed=0;
}

void InitLead(Poly *p)
{
	if (p->lead) pDelete(&p->lead);
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
			return LengthCompare(item1->root,item2->root);
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
//		pTest(Pr->root);
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
//			Print("*");
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
//				Print("!");
				NFL((*l)->info,G);
//                                Print("$");
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
//	Print("\n");
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
   			};
     
			curr=curr->left;
			power_tmp--;
		};

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
  ProdCrit=0;
  ChainCrit=0;
	offset=(currRing->N % 8 == 0) ? (currRing->N/8)*8 : (currRing->N/8+1)*8;
	if (strstr(Ord,"dp\0") || strstr(Ord,"Dp\0"))
	{
		degree_compatible=1;
		ListGreatMove=ListGreatMoveDegree;
	}
	else
	{
		degree_compatible=0;
		ListGreatMove=ListGreatMoveOrder;
	}

	Define(&G);
};

static Poly *h,*f;

void insert_in_G(Poly *x)
{
 insert_(&G,x);
}

void T2G();

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

int ComputeBasis(jList *_T,jList *_Q)
{
  int gb_l,i,ret_value=1;

  T=_T; Q=_Q;

  //  Debug();

  while((h=FindMinList(Q))!=NULL)
  {
 
    //        Print("New element\n");
    //	Debug();

        if (!degree_compatible)
        {
                if (!ValidatePoly(h,G))
                {
                        DestroyPoly(h);
                        continue;
                };

                h->changed=0;

                NFL(h,G);

                if (!h->root)
                {
                        DestroyPoly(h);
                        continue;
                };
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
//			Print("<-\n");
                        DestroyTree(G->root);
                        G->root=create();
                        T2G();
                }
        }

	//	Print("PNF\n");
        PNF(h,G);
	if (TEST_OPT_PROT)
	{
	  Print("s%d",pow_(h->root));
	}
        insert_(&G,h);
        InsertInList(T,h);

	//	Print("For each PNF\n");
        if (degree_compatible)
            ForEachPNF(T,pow_(h->root));

	//	Print("Control of prolongations\n");
        if (h->changed)
            ForEachControlProlong(T);
        else
            ControlProlong(h);

	//	Debug();

	//	Print("NFListQ\n");
        if (degree_compatible)
            NFListQ();
	//Debug();
    }

//    gb_l=GB_length();

  if (TEST_OPT_PROT)
  {
    Print("\nLength of Janet basis: %d",CountList(T));
    Print("\nproduct criterion:%d chain criterion:%d\n",ProdCrit,ChainCrit);
//    Print("Length of Groebner basis:    %d\n",gb_l);
  }

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
