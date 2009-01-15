/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.16 2009-01-15 17:44:23 ederc Exp $ */
/*
* ABSTRACT: f5gb interface
*/
#include "mod2.h"
#ifdef HAVE_F5
#include "kutil.h"
#include "structs.h"
#include "omalloc.h"
#include "polys.h"
#include "p_polys.h"
#include "ideals.h"
#include "febase.h"
#include "kstd1.h"
#include "khstd.h"
#include "kbuckets.h"
#include "weight.h"
#include "intvec.h"
#include "pInline1.h"
#include "f5gb.h"
#include "lpolynomial.h"
#include "lists.h"


/*
====================================================================
sorting ideals by decreasing total degree "left" and "right" are the 
pointer of the first and last polynomial in the considered ideal
====================================================================
*/
void qsortDegree(poly* left, poly* right) {
    poly* ptr1 = left;
    poly* ptr2 = right;
    poly p1,p2;
    p2 = *(left + (right - left >> 1));
    do {
            while(pTotaldegree(*ptr1, currRing) < pTotaldegree(p2, currRing)) {
                    ptr1++;
            } 
            while(pTotaldegree(*ptr2, currRing) > pTotaldegree(p2,currRing)) {
                    ptr2--;
            }
            if(ptr1 > ptr2) {
                    break;
            }
            p1    = *ptr1;
            *ptr1 = *ptr2;
            *ptr2 = p1;
    } while(++ptr1 <= --ptr2);

    if(left < ptr2) {
            qsortDegree(left,ptr2);
    }
    if(ptr1 < right) {
            qsortDegree(ptr1,right);
    }
}


/*
==================================================
computes incrementally gbs of subsets of the input 
gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}
==================================================
*/
LList* F5inc(int* i, poly* f_i, LList* g_prev, poly* ONE) {
    LList* g_curr       =   g_prev;
    g_curr->insert(ONE,i,f_i);
   
    return g_curr;
}

/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our f5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int i,j;
    const int idElems = IDELEMS(id);
    // 1 polynomial for defining initial labels & further tests
    static poly ONE = pOne(); 
    
    
    // definition of one-polynomial as global constant ONE
    //poly one = pInit();
    //pSetCoeff(one, nInit(1));
    //static poly ONE = one;
    
    for(j=0; j<IDELEMS(id); j++) {
        if(NULL != id->m[j]) { 
            if(pComparePolys(id->m[j],ONE)) {
                Print("One Polynomial in Input => Computations stopped\n");
                ideal id_new = idInit(1,1);
                id_new->m[0] = ONE;
                return(id_new);
            }   
        }
    } 
    
    id = kInterRed(id,0);  
    qsortDegree(&id->m[0],&id->m[IDELEMS(id)-1]);
    idShow(id);
    i = 1;
    // only for debugging
    //LNode* current;
    //LList* g_curr = new LList(lp);
    //}
    //for(i=2; i<IDELEMS(id); i++) {
        //g_curr = F5inc(&i,&id->m[i],g_prev);
        //if(g_curr->polyTest(&ONE)) {
        //    Print("One Polynomial in Input => Computations stopped\n");
         //   ideal id_new = idInit(1,1);
        //    id_new->m[0] = ONE;
        //    return(id_new);               
        //}
    //}
    return(id);


}

#endif
