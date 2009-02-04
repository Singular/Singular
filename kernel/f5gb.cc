/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.22 2009-02-04 19:27:11 ederc Exp $ */
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
#include "f5data.h"
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
    // tag the first element of index i-1 for criterion 1 
    LTagList* lTag  =   new LTagList(gPrev->getFirst());
    // first element in rTag is NULL, this must be done due to possible later improvements
    RTagList* rTag  =   new RTagList();
    gPrev->insert(ONE,i,f_i);
    CList* critPairs    =   new CList();
    critPairs           =   criticalPair(gPrev, critPairs, lTag, rTag);
    
    return gPrev;
}


/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
CList* criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag) {
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* first        =   gPrev->getFirst();
    LNode* l            =   gPrev->getFirst()->getNext();
    poly* u1            =   new poly(pInit());
    poly* u2            =   new poly(pInit());
    poly* lcm           =   new poly(pInit());
    // computation of critical pairs
    while( NULL != l) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
        //pWrite( *(l->getPoly()) );
        pLcm(first->getPoly(), l->getPoly(), *lcm);
        pSetCoeff(*lcm,nOne);
        // computing factors u1 & u2 for new labels
        *u1 = pDivide(*lcm,pHead(first->getPoly()));
        pSetCoeff(*u1,nOne);
        *u2 = pDivide(*lcm, pHead(l->getPoly()));
        pSetCoeff(*u2,nOne);
        pWrite(*u2);
        // testing both new labels by the F5 Criterion
        if(!criterion1(u1, first, lTag) && !criterion1(u2, l, lTag) && 
           !criterion2(u1, first, rTag) && !criterion2(u2, l, rTag)) {
            Print("Criteria passed\n");
            // if they pass the test, add them to CList critPairs, having the LPoly with greater
            // label as first element in the CPair
            if(first->getIndex() == l->getIndex() && 
            pMult(*u1, first->getTerm()) < pMult(*u2, l->getTerm())) {
                CPair* cp   =   new CPair(pDeg(ppMult_qq(*u2,pHead(l->getPoly()))), *u2, 
                                l->getLPoly(), *u1, first->getLPoly());                   
                    critPairs->insert(cp);
            }
            else {
                CPair* cp   =   new CPair(pDeg(ppMult_qq(*u2,pHead(l->getPoly()))), *u1, 
                                first->getLPoly(), *u2, l->getLPoly());                   
                    critPairs->insert(cp);
            }
        }
        else {
            Print("Criteria not passed\n");
        }
        
        // for debugging
        if(NULL != critPairs) {
            critPairs->print(); 
        }
        l   =   l->getNext();
    }
    Print("ENDE CRITPAIRS\n");
    return critPairs;
}

/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
bool criterion1(poly* t, LNode* l, LTagList* lTag) {
    // starts at the first element in gPrev with index = (index of l)-1, these tags are saved in lTag
    LNode* testNode =   lTag->get(l->getIndex());
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
    while(NULL != testNode) {
        if(pLmDivisibleByNoComp(pHead(testNode->getPoly()),u1)) {
            return true;
        }
        testNode    =   testNode->getNext();
    }
    return false;
}

/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
bool criterion2(poly* t, LNode* l, RTagList* rTag) {
    // start at the previously added element to gPrev, as all other elements will have the same index for sure
    RNode* testNode =   rTag->get(l->getIndex());
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
    // first element added to rTag was NULL, check for this
    Print("Hier1\n");
    while(NULL != testNode && testNode->getRule()->getOrigin() != l->getLPoly()) {
        Print("Hier2\n");
        if(pLmDivisibleByNoComp(ppMult_qq(*t,l->getTerm()),testNode->getRuleTerm())) {
            return true;
        }
        testNode    =   testNode->getNext();
    }
    return false;
}

/*
==========================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in sPols(), with added lastRuleTested parameter
==========================================================================================================
*/
bool criterion2(poly* t, LNode* l, RTagList* rTag, Rule* lastRuleTested) {
    // start at the previously added element to gPrev, as all other elements will have the same index for sure
    RNode* testNode =   rTag->getFirst();
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
    // first element added to rTag was NULL, check for this
    Print("Hier1\n");
    while(NULL != testNode && testNode->getRule()->getOrigin() != l->getLPoly()) {
        Print("Hier2\n");
        if(pLmDivisibleByNoComp(ppMult_qq(*t,l->getTerm()),testNode->getRuleTerm())) {
            return true;
        }
        testNode    =   testNode->getNext();
    }
    return false;
}

/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int i,j;
    // 1 polynomial for defining initial labels & further tests
    poly ONE = pOne();
    // list of rules
    RList* rules    =   new RList();
    i = 1;
    LList* gPrev    =   new LList( &ONE, &i, &id->m[0]);
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
        gPrev   =   F5inc(&i, &id->m[i-1], gPrev, &ONE);
        Print("JA\n");
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
