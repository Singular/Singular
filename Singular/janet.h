#ifndef __JANET_INTERFACE__
#define __JANET_INTERFACE__

#include "febase.h"
#include "polys.h"
#include "numbers.h"
#include "ring.h"
#include "ideals.h"
#include "subexpr.h"
#include "longrat.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <omalloc.h>

#define GCM(sz) omAlloc((sz))
#define GCMA(sz) omAlloc((sz))
#define GCR(x,y) omRealloc((x),(y))
#define GCF(x) omFree((x))

#if (defined(__CYGWIN__))
#include <ctype.h>
#endif
#include <stdarg.h>

#define ListNode struct LISTNODE
#define TreeM struct TREEM
#define NodeM struct NODEM

static int Mask[8]={0x80,0x40,0x20,0x10,0x8,0x4,0x2,0x1};

typedef struct
{
	poly root; //poly for parent, NULL for prol
	poly history; //parent
	poly lead; //leading monomial for prolongation
	char *mult; //[multi].[prol]
	int changed;
	int prolonged; //number of prolonged variable for prolongation, otherwise = -1;
} Poly;

typedef void (*poly_function)(Poly *);

ListNode
{
 Poly *info;
 ListNode *next;
};

typedef struct
{
 ListNode *root;
} jList;

NodeM
{
 NodeM *left,*right;
 Poly *ended;
};

TreeM
{
 NodeM *root;
};

typedef ListNode* LCI;
typedef ListNode** LI;

//------GLOBALS-------
static int m_s,v_s,vectorized,VarN1,offset;
static jList *T,*Q;

static TreeM *G;

static Poly *phD;
static NodeM *FreeNodes;

//-------FUNCS----------
Poly* FindMinList(jList *);
void DestroyTree(NodeM *);
NodeM* create();
void ForEach(TreeM *,poly_function);
void ControlProlong(Poly *);
Poly* is_div_(TreeM *root, poly item);
void insert_(TreeM **tree, Poly *item);
Poly* NewPoly(poly p=NULL);
void DestroyPoly();

static int degree_compatible;

void NFL(Poly *,TreeM *);
void PNF(Poly *,TreeM *);
void ClearProl(Poly *x, int i);
void InitProl(Poly *p);
void InitHistory(Poly *p);
Poly *is_present(jList *,poly);
int GetMult(Poly *,int);
int GB_length();

void InsertInList(jList *,Poly *);
void ForEachPNF(jList *,int);
void ClearMult(Poly *,int);
void ProlVar(Poly *,int);
void SetMult(Poly *,int);
void InitLead(Poly *);
void InsertInCount(jList *,Poly *);
int GetProl(Poly *, int);
void SetProl(Poly *, int);
int ProlCompare(Poly *, Poly *);
int ValidatePoly(Poly *,TreeM *);
static int (*ListGreatMove)(jList *,jList *,poly);
int ListGreatMoveDegree(jList *,jList *,poly);
int ListGreatMoveOrder(jList *,jList *,poly);
void ForEachControlProlong(jList *);
void NFListQ();
int CountList(jList *);
void DestroyList(jList *);

int ReducePoly(Poly *x,Poly *y);
int ReducePolyLead(Poly *x,Poly *y);
void Define(TreeM **G);
ListNode* CreateListNode(Poly *x);
void DestroyListNode(ListNode *x);
void DestroyFreeNodes();
BOOLEAN jjJanetBasis(leftv res, leftv v);
#endif //JANET_INTERFACE
