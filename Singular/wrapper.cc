#include "mod2.h"
#include "janet.h"
#include "kstd1.h"

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

	for (int i=0; i < ((ideal)(v->Data()))->idelems(); i++)        
	{
		if (pIsConstant(((ideal)(v->Data()))->m[i]))
		{
			Werror("Constant in input");
			goto zero;
		}

		Poly *beg=NewPoly(pCopy(((ideal)(v->Data()))->m[i]));

		InitHistory(beg);
		InitProl(beg);
		InitLead(beg);

		InsertInCount(Q,beg);
	}

	ideal result;
	int dpO;

	if (ComputeBasis(T,Q))
	{
		dpO=(strstr(rOrdStr(currRing),"dp")!=NULL);
		int ideal_length= dpO ? GB_length() : CountList(T);
		result=idInit(ideal_length,1);
        	int ideal_index=0;

		LCI iT=T->root;
  
        	while(iT) 
        	{
			pTest(iT->info->root);
	    		if (dpO)
			{
				if (pow_(iT->info->lead) == pow_(iT->info->history))
				{
					result->m[ideal_length-ideal_index-1]=pCopy(iT->info->root);
					if (!nGreaterZero(pGetCoeff(iT->info->root)))
						result->m[ideal_length-ideal_index-1]=pNeg(result->m[ideal_length-ideal_index-1]);					

					ideal_index++;
				}
			}
			else
			{
				result->m[ideal_length-ideal_index-1]=pCopy(iT->info->root);
				if (!nGreaterZero(pGetCoeff(iT->info->root)))
					result->m[ideal_length-ideal_index-1]=pNeg(result->m[ideal_length-ideal_index-1]);					

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
	
	if (!dpO)
	{
	  Print ("interred\n");
	  result=kInterRed(result);
	  idSkipZeroes(result);
	}
	res->data = (char *)result;
	res->rtyp = IDEAL_CMD;

	DestroyList(Q);
	DestroyList(T);

	return FALSE;
}
