/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.23 2009-02-06 20:12:35 ederc Exp $ */
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
LList* F5inc(int i, poly f_i, LList* gPrev, ideal gbPrev, poly ONE, int* reductionsToZero) {
    int j;
    // tag the first element of index i-1 for criterion 1 
    LTagList* lTag  =   new LTagList(gPrev->getFirst());
    // first element in rTag is NULL, this must be done due to possible later improvements
    RTagList* rTag  =   new RTagList();
    gPrev->insert(ONE,i,f_i);
    Print("1st gPrev: ");
    pWrite(gPrev->getFirst()->getPoly());
    Print("2nd gPrev: ");
    pWrite(gPrev->getFirst()->getNext()->getPoly());    
    CList* critPairs        =   new CList();
    RList* rules            =   new RList();
    CNode* critPairsMinDeg  =   new CNode();   
    LNode* sPolys           =   new LNode();
    // computation of critical pairs with checking of criterion 1 and criterion 2
    critPairs               =   criticalPair(gPrev, critPairs, lTag, rTag);
    LList* sPolyList        =   new LList();
    // labeled polynomials which have passed reduction() and have to be added to list gPrev
    LList* completed        =   new LList();
    // the reduced labeled polynomials which are returned from subalgorithm reduction()
    LNode* reducedLPolys     =   new LNode();
    // while there are critical pairs to be further checked and deleted/computed
    while(NULL != critPairs->getFirst()->getData()) { 
        // critPairs->getMinDeg() deletes the first elements of minimal degree from
        // critPairs, thus the while loop is not infinite.
        critPairsMinDeg =   critPairs->getMinDeg();
        // adds all to be reduced S-polynomials in the list sPolyList and adds 
        // the corresponding rules to the list rules
        // NOTE: inside there is a second check of criterion 2 if new rules are 
        // added
        computeSPols(critPairsMinDeg,rTag,rules,sPolyList,reductionsToZero);
         
        reducedLPolys   =   reduction(sPolyList, completed, gbPrev, reductionsToZero);
    }
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
    LNode* l            =   first->getNext();
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
        }
        
        // for debugging
        if(NULL != critPairs) {
            critPairs->print(); 
        }
        l   =   l->getNext();
    }
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
            Print("Criterion 1 NOT passed!\n");
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
    while(NULL != testNode && testNode->getRule()->getOrigin() != l->getLPoly()) {
        if(pLmDivisibleByNoComp(ppMult_qq(*t,l->getTerm()),testNode->getRuleTerm())) {
            Print("Criterion 2 NOT passed!\n");
            return true;
        }
        testNode    =   testNode->getNext();
    }
    return false;
}



/*
=================================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in computeSPols(), with added lastRuleTested parameter
=================================================================================================================
*/
bool criterion2(poly* t, LPoly* l, RTagList* rTag, Rule* lastRuleTested) {
    // start at the previously added element to gPrev, as all other elements will have the same index for sure
    RNode* testNode =   rTag->getFirst();
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
    // first element added to rTag was NULL, check for this
    while(NULL != testNode && testNode->getRule() != lastRuleTested) {
        if(pLmDivisibleByNoComp(ppMult_qq(*t,l->getTerm()),testNode->getRuleTerm())) {
            Print("Criterion 2 NOT passed!\n");
            return true;
        }
        testNode    =   testNode->getNext();
    }
    return false;
}



/*
==================================
Computation of S-Polynomials in F5
==================================
*/
void computeSPols(CNode* first, RTagList* rTag, RList* rules, LList* sPolyList, int* reductionsToZero) { 
    CNode* temp =   first;
    poly zero   =   pInit();
    while(NULL != temp->getData()) {
        // only if a new rule was added since the last test in subalgorithm criticalPair()
        if(rules->getFirst() != rTag->getFirst()) {
            Print("IN SPOLS => NEW RULES AVAILABLE\n");
            if(!criterion2(temp->getAdT1(),temp->getAdLp1(),rTag,temp->getLastRuleTested())) {
                // the second component is tested only when it has the actual index, otherwise there is
                // no new rule to test since the last test in subalgorithm criticalPair()
                if(temp->getLp2Index() == temp->getLp1Index()) {
                    if(!criterion2(temp->getAdT2(),temp->getAdLp2(),rTag,temp->getLastRuleTested())) {
                        // computation of S-polynomial
                        poly sp =   pInit();
                        sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                         ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                        Print("BEGIN SPOLY\n====================\n");
                        pWrite(sp);
                        Print("END SPOLY\n====================\n");
                        if(!pCmp(sp,zero)) {
                            // as rules consist only of pointers we need to save the labeled
                            // S-polynomial also of a zero S-polynomial
                            //zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                            // origin of rule can be set NULL as the labeled polynomial
                            // will never be used again as it is zero => no problems with 
                            // further criterion2() tests and termination conditions
                            *reductionsToZero++;
                            rules->insert(temp->getLp1Index(),temp->getT1(),NULL);
                        }
                        else {
                            sPolyList->insert(temp->getT1(),temp->getLp1Index(),sp);
                            rules->insert(temp->getLp1Index(),temp->getT1(),sPolyList->getFirst()->getLPoly());
                        }
                        // data is saved in sPolyList or zero => delete sp
                        pDelete(&sp);
                    }
                }
                else { // temp->getLp2Index() < temp->getLp1Index()
                    // computation of S-polynomial
                    poly sp =   pInit();
                    sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                     ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                    Print("BEGIN SPOLY\n====================\n");
                    pWrite(sp);
                    Print("END SPOLY\n====================\n");
                    if(!pCmp(sp,zero)) {
                        // zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                        // origin of rule can be set NULL as the labeled polynomial
                        // will never be used again as it is zero => no problems with 
                        // further criterion2() tests and termination conditions
                        *reductionsToZero++;
                        rules->insert(temp->getLp1Index(),temp->getT1(),NULL);
                    }
                    else {
                        sPolyList->insert(temp->getT1(),temp->getLp1Index(),sp);
                        rules->insert(temp->getLp1Index(),temp->getT1(),sPolyList->getFirst()->getLPoly());
                    }
                    // data is saved in sPolyList or zero => delete sp
                    pDelete(&sp);
                }
            }
        }
        temp    =   temp->getNext();
    }
    // these critical pairs can be deleted now as they are either useless for further computations or 
    // already saved as an S-polynomial to be reduced in the following
    delete first;    
}



/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
LNode* reduction(LList* sPolyList, LList* completed, ideal gbPrev, int* reductionsToZero) { 
    poly zero   =   pInit();
    // compute only if there are any S-polynomials to be reduced
    if(NULL != sPolyList->getFirst()->getLPoly()) {
        LNode* temp =   sPolyList->getFirst();
        while(NULL != temp->getLPoly()) {
            temp->setPoly(kNF(gbPrev,currQuotient,temp->getPoly()));
            Print("Nach NORMAL FORM: ");
            pWrite(temp->getPoly());
            // test if temp->getPoly() is zero polynomial
            if(!pCmp(temp->getPoly(),zero)) {
                *reductionsToZero++;
                // TODO: sPolyList -> delete first element of list as it is zero and done
            }
            //completed   =   topReduction();
            // in topReduction() the investigated first element of sPolyList will be deleted after its
            // reduction has finished => the next to be investigated element is the newly first element
            // in sPolyList => the while loop is finite
            // first possible after implementation of topReduction(): temp = sPolyList->getFirst();
            temp    =   temp->getNext();
        }
    }
    return NULL;
}    



/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
LNode* topReduction(LNode* l, LList* gPrev, LList* completed) {
    LPoly* red  =   findReductor(l, gPrev, completed);
    return NULL;
}



/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
LPoly* findReductor(LNode* l, LList* gPrev, LList* completed) {
    return NULL;
}



/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int i,j;
    int* reductionsToZero   =   new int;
    *reductionsToZero       =   0;
    // 1 polynomial for defining initial labels & further tests
    poly ONE = pOne();
    i = 1;
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
    LList* gPrev    =   new LList(ONE, i, id->m[0]);
    pWrite(id->m[0]);
    poly p = pHead(id->m[0]);
    pWrite(p);
    poly q = pHead(id->m[2]);
    pWrite(q);
    
    // computing the groebner basis of the elements of index < actual index
    Print("Laenge der bisherigen Groebner Basis: %d\n",gPrev->getLength());
    ideal gbPrev    =   idInit(gPrev->getLength(),1);
    // initializing the groebner basis of elements of index < actual index
    gbPrev->m[0]    =   gPrev->getFirst()->getPoly();
    idShow(gbPrev);
    idShow(currQuotient);

    for(i=2; i<=IDELEMS(id); i++) {
        gPrev   =   F5inc(i, id->m[i-1], gPrev, gbPrev, ONE, reductionsToZero);
        Print("JA\n");
        //TODO: idAdd for computing gbPrev with the actual index!
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
    Print("\n\nNumber of zero-reductions:  %d\n",*reductionsToZero);
    return(id);


}

#endif
