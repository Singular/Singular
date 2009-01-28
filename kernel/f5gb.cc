/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.18 2009-01-28 17:20:49 Singular Exp $ */
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
#include "f5lists.h"


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
LList* F5inc(int* i, poly* f_i, LList* gPrev, poly* ONE) {
    gPrev->insert(ONE,i,f_i);
    CList* critPairs    =   criticalPair(gPrev);
     
    return gPrev;
}


/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
CList* criticalPair(LList* gPrev) {
    poly lcm   = pInit();
    number n    = nInit(2);
    nWrite(n);
    pWrite(lcm);
    // initialization for usage in pLcm()
    LNode* first        =   gPrev->getFirst();
    LNode* l            =   gPrev->getFirst()->getNext();
    poly t1, t2         =   pInit();
    t1                  =   pHead(*first->getPoly());
    poly u1             =   pOne();
    poly u2             =   pOne();
    CList*  critpairs   =   NULL;
    // computation of critical pairs
    while( NULL != l) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
        //pWrite( *(l->getPoly()) );
        pLcm(*first->getPoly(), *l->getPoly(), t1);
        pWrite(t1);
        pWrite(u1);
        pWrite(u2);
        pSetCoeff0(lcm,n);
        poly p = lcm; 
        pWrite(p);
        Print("%ld\n",pDeg(t1));
        // computing factors u1 & u2 for new labels
        u1  = pDivide(lcm, t1);
        pWrite(u1);
        pWrite(t1);
        Print("%ld\n",pDeg(t1));
        //pSetCoeff(u1,pGetCoeff(pOne()));
        pWrite(u1);
        t2  = pHead(*l->getPoly());
        pWrite(t2);
        // testing stuff
        u1 = ppMult_qq(t1,t2);
        pWrite(u1);
        pWrite(t2);
        Print("%ld\n",pDeg(u1));
        Print("%ld\n",pDeg(t2));
        u2  = pDivide(lcm, t2);
        pSetCoeff(u2, pGetCoeff(pOne()));
        pWrite(u2);
        Print("%ld\n",pDeg(u2)); 
        // testing both new labels by the F5 Criterion
        if(!criterion1(first, gPrev) &&
           !criterion1(l, gPrev)) {
            if(*first->getIndex() == *l->getIndex()) {
                if(pMult(u1, *first->getTerm()) < pMult(u2, *l->getTerm())) {
                    //if(!critPairs) {
                        CPair* cp   =   new CPair(pDeg(lcm), u1, first->getLPoly(), u2, 
                                        l->getLPoly());                   
                    //}
                }
            }
        }

        Print("\n");
        l   =   l->getNext();
    }
    return NULL;
}

/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
bool criterion1(LNode* l, LList* gPrev) {
    LNode* testNode =   l->getNext();
    while(NULL != testNode) {
        while(*testNode->getIndex() == *l->getIndex()) {
            testNode = testNode->getNext();
        }
    }
        

    return true;
}

/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our f5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int i,j;
    // 1 polynomial for defining initial labels & further tests
    static poly ONE = pOne(); 
    i = 1;
    LList* gPrev = new LList( &ONE, &i, &id->m[0]);
    poly* lcm = new poly;
    // initialization for usage in pLcm()
    *lcm = pOne();
    pWrite(*lcm); 
    // definition of one-polynomial as global constant ONE
    //poly one = pInit();
    //pSetCoeff(one, nInit(1));
    //static poly ONE = one;
    
    for(j=0; j<IDELEMS(id); j++) {
        if(NULL != id->m[j]) { 
            if(pComparePolys(id->m[j],ONE)) {
                Print("One Polynomial in Input => Computations stopped\n");
                ideal idNew = idInit(1,1);
                idNew->m[0] = ONE;
                return(idNew);
            }   
        }
    } 
    pLcm( id->m[0], id->m[1], *lcm);
    Print("LCM NEU\n");
    //*lcm = pHead(*lcm);
    //pWrite(*lcm);
    Print("\n\n"); 
    id = kInterRed(id,0);  
    qsortDegree(&id->m[0],&id->m[IDELEMS(id)-1]);
    idShow(id);
    poly p = pHead(id->m[0]);
    pWrite(p);
    poly q = pHead(id->m[2]);
    pWrite(q);
    for(i=2; i<=IDELEMS(id); i++) {
        gPrev   =   F5inc( &i, &id->m[i-1], gPrev, &ONE );
    } 
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
