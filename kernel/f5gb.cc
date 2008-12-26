/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.14 2008-12-26 13:49:29 ederc Exp $ */
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
=======================
static/global variables
=======================
*/
static poly ONE = one_poly();


/*
================================================
computation of ONE polynomial as global variable
================================================
*/
poly one_poly() {
    poly one = pInit();
    pSetCoeff(one,nInit(1));
    return one;
}

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
  
LList* F5inc(long i, poly* f_i, LList* g_prev) {
    LList* g_curr       =   g_prev;
    LNode* prev_last    =   g_prev->getLast(); //tags the end of g_prev->only 1 list for g_prev & g_curr
    g_curr->append(&ONE,&i,f_i);
   
    return g_curr;




}

/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our f5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    long i,j;
    LPoly* lp = new LPoly;
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
    //lp->setIndex(&i);
    //lp->setTerm(&ONE);
    //lp->setPoly(&id->m[0]);
    //lp->set(ONE,1,ONE);    
    /* if(generate_input_list(lp,iTmp,ONE)) {
            Print("One Polynomial in Input => Computations stopped");
            iTmp = idInit(1,0);
            iTmp->m[0] = ONE;
            return(iTmp);
    }
    */
    // only for debugging
    long k = 1;
    LList* g_prev = new LList(lp);
    //LList* g_curr = new LList;
    //LNode* current = new LNode;
    g_prev->append(&ONE,&k,&id->m[2]);
    i = g_prev->getLength();
    Print("%ld\n\n",i); 
    LPoly* current = g_prev->getFirst()->getLPoly();
    if(pComparePolys(current->getPoly(), id->m[0])) {
        Print("Yes!\n");
    }
    //current = g_prev->getFirst();
    //current++;
    //Print("Term: %ld\n\n",current->get()->getTerm);
    /*for(i=2; i<IDELEMS(id); i++) {
        g_curr = F5inc(i,g_prev);
        if(g_curr->poly_test(ONE)) {
            Print("One Polynomial in Input => Computations stopped\n");
            ideal id_new = idInit(1,1);
            id_new->m[0] = ONE;
            return(id_new);               
        }
    }
    */        
    return(id);


}


#endif
