#include "mod2.h"
#include "janet.h"
#include "kstd1.h"
#include "ipid.h"

#define pow_(x) pTotaldegree((x))

//extern int (*ListGreatMove)(jList *,jList *,poly);
extern int ComputeBasis(jList *,jList *);
extern void Initialization(char *);

BOOLEAN jjJanetBasis(leftv res, leftv v)
{
	Initialization(rOrdStr(currRing));

  	jList *Q=(jList *)GCM(sizeof(jList));
	Q->root=NULL;
		
	jList *T=(jList *)GCM(sizeof(jList));
	T->root=NULL;

	ideal input = (ideal)(v->Data());
	/* the second arg is an integer, defining what to return:
	 0 (default case) = Groebner basis,
         1                = Janet basis.
	*/
	int doJanet;
	if ((v->next!=NULL) && (v->next->Typ() == INT_CMD))
	{
	  doJanet = (int)v->next->CopyD();
	  //	  Print("doJanet:%d\n",doJanet);
	}	       
	else 
	{
	  doJanet = 0;
	}
	ideal result;
	int dpO = !doJanet; /* compatibility */

	for (int i=0; i < input->idelems(); i++)        
	{
	  /* nonzero constant check */
	  if (pIsConstant(input->m[i])) { goto zero; }
	  Poly *beg=NewPoly(pCopy(input->m[i]));
	  InitHistory(beg);
	  InitProl(beg);
	  InitLead(beg);
	  InsertInCount(Q,beg);
	}

	if (ComputeBasis(T,Q))
	{
	  //		dpO=(strstr(rOrdStr(currRing),"dp")!=NULL);
	  int ideal_length= dpO ? GB_length() : CountList(T);
	  result=idInit(ideal_length,1);
	  int ideal_index=0;
	  
	  LCI iT=T->root;
	  
	  while (iT) 
	  {
#ifdef PDEBUG		 
	    pTest(iT->info->root);
#endif
	    if (!(dpO && (pow_(iT->info->lead) != pow_(iT->info->history)))) 
	    {
	      result->m[ideal_length-ideal_index-1]=pCopy(iT->info->root);
	      if (!nGreaterZero(pGetCoeff(iT->info->root)))
	      {
		result->m[ideal_length-ideal_index-1]=pNeg(result->m[ideal_length-ideal_index-1]); 
	      }
	      ideal_index++;
	    }    
	    iT=iT->next; 
	  }
	}
	else
        {
zero:
	  result=idInit(1,1);
	  result->m[0]=pOne();	
	}
	
	/* now we make the basis shorter... getting Groebner */
	if (!doJanet) /* if (!dpO) */ 
	{
	  if (TEST_OPT_PROT)
	  {
	    Print ("interred\n");
	  }
	  result=kInterRed(result);
	  idSkipZeroes(result);
	}
	res->data = (char *)result;
	res->rtyp = v->Typ();
	if (!doJanet) setFlag(res,FLAG_STD);

	DestroyList(Q);
	DestroyList(T);
	
	return FALSE;
}
