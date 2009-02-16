/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.29 2009-02-16 14:23:42 ederc Exp $ */
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

int reductionsToZero   =  0;

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
LList* F5inc(int i, poly f_i, LList* gPrev, ideal gbPrev, poly ONE, LTagList* lTag, RList* rules, RTagList* rTag) {
    int j;
    Print("%p\n",gPrev->getFirst());
    pWrite(gPrev->getFirst()->getPoly());
    gPrev->insert(ONE,i,f_i);
    Print("1st gPrev: ");
    pWrite(gPrev->getFirst()->getPoly());
    Print("2nd gPrev: ");
    pWrite(gPrev->getFirst()->getNext()->getPoly());
    //pWrite(gPrev->getFirst()->getNext()->getPoly());    
    CList* critPairs        =   new CList();
    CNode* critPairsMinDeg  =   new CNode();   
    // computation of critical pairs with checking of criterion 1 and criterion 2
    critPairs               =   criticalPair(gPrev, critPairs, lTag, rTag, rules);
    static LList* sPolyList        =   new LList();
    // labeled polynomials which have passed reduction() and have to be added to list gPrev
    static LList* completed        =   new LList();
    Print("_____________________________________________________________________________%p\n",completed->getFirst()->getLPoly());
    // the reduced labeled polynomials which are returned from subalgorithm reduction()
    static LList* reducedLPolys     =   new LList();
    // while there are critical pairs to be further checked and deleted/computed
    while(NULL != critPairs->getFirst()->getData()) { 
        // critPairs->getMinDeg() deletes the first elements of minimal degree from
        // critPairs, thus the while loop is not infinite.
        critPairs->print();
        critPairsMinDeg =   critPairs->getMinDeg();
        // adds all to be reduced S-polynomials in the list sPolyList and adds 
        // the corresponding rules to the list rules
        // NOTE: inside there is a second check of criterion 2 if new rules are 
        // added
        computeSPols(critPairsMinDeg,rTag,rules,sPolyList);
        
        // DEBUG STUFF FOR SPOLYLIST
        LNode* temp     =   sPolyList->getFirst();
        while(NULL != temp && NULL != temp->getLPoly()) {
            Print("Spolylist element: ");
            pWrite(temp->getPoly());
            temp    =   temp->getNext();
        } 
        //while(NULL != sPolyList->getFirst()->getLPoly()) {
            reduction(sPolyList, completed, gPrev, rules, lTag, rTag, gbPrev);
        //}
    }
    Print("HIER123\n");
    Print("%p\n",rules->getFirst());
    Print("%p\n",rTag->getFirst());
    if(rules->getFirst() != rTag->getFirst()) {
        Print("+++++++++++++++++++++++++++++++++++++NEW RULES+++++++++++++++++++++++++++++++++++++\n");
        rTag->insert(rules->getFirst());
    }
    else {
        Print("+++++++++++++++++++++++++++++++++++NO NEW RULES++++++++++++++++++++++++++++++++++++\n");
    }
    lTag->insert(gPrev->getFirst());
    Print("COMPLETED FIRST IN F5INC: \n");
    return gPrev;
}



/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
CList* criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules) {
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* first        =   gPrev->getFirst();
    LNode* l            =   first->getNext();
    poly* u1            =   new poly(pInit());
    poly* u2            =   new poly(pInit());
    poly* lcm           =   new poly(pInit());
    poly t              =   pHead(first->getPoly());
    // computation of critical pairs
    while( NULL != l) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
        //pWrite( *(l->getPoly()) );
        pLcm(first->getPoly(), l->getPoly(), *lcm);
        pSetCoeff(*lcm,nOne);
        // computing factors u2 for new labels
        pWrite(t);
        *u1 = pDivide(*lcm,t);
        pWrite(*u1);
        pSetCoeff(*u1,nOne);
        pWrite(pHead(l->getPoly()));
        *u2 = pDivide(*lcm, pHead(l->getPoly()));
        pSetCoeff(*u2,nOne);
        Print("IN CRITPAIRS\n");
        pWrite(*u2);
        // testing both new labels by the F5 Criterion
        if(!criterion1(u1, first, lTag) && !criterion1(u2, l, lTag) && 
           !criterion2(u1, first, rules, rTag) && !criterion2(u2, l, rules, rTag)) {
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
    while(NULL != testNode && NULL != testNode->getLPoly()) {
        if(pLmDivisibleByNoComp(testNode->getPoly(),u1)) {
            Print("Criterion 1 NOT passed!\n");
            return true;
        }
        //pWrite(testNode->getNext()->getPoly());
		testNode    =   testNode->getNext();
        Print("ADDRESS OF TEST NODE: %p\n",testNode);
    Print("Hier\n");
    }
    Print("HIER DRIN CRITERION 1\n");
	
    return false;
}



/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
bool criterion2(poly* t, LNode* l, RList* rules, RTagList* rTag) {
	Print("HIER DRIN CRITERION 2:=========================\n");
    // start at the previously added element to gPrev, as all other elements will have the same index for sure
	Print("%d\n",l->getIndex());
    RNode* testNode =   new RNode();
    if(NULL == rTag->getFirst()->getRule()) {
        testNode    =   rules->getFirst();
    }
    else {
        Print("%d\n",l->getIndex());
        Print("%d\n",rTag->getFirst()->getRuleIndex());
pWrite(rTag->getFirst()->getRuleTerm());
        if(l->getIndex() > rTag->getFirst()->getRuleIndex()) {
            testNode    =   rules->getFirst();
            Print("TEST NODE: %p\n",testNode);
        }
        else {
            testNode    =   rTag->get(l->getIndex());
        }
    }
	// save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
	pWrite(u1);
    // first element added to rTag was NULL, check for this
    pWrite(l->getPoly());
    //Print("%p\n",testNode->getRule());
    Print("HIER !!!!\n");
    // NOTE: testNode is possibly NULL as rTag->get() returns NULL for elements of index <=1!
    while(NULL != testNode && NULL != testNode->getRule() && testNode->getRule() != l->getRule() 
          && l->getIndex() == testNode->getRuleIndex()) {
		pWrite(ppMult_qq(*t,l->getTerm()));
		pWrite(testNode->getRuleTerm());
		if(pLmDivisibleBy(ppMult_qq(*t,l->getTerm()),testNode->getRuleTerm())) {
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
bool criterion2(poly* t, LPoly* l, RList* rules, Rule* lastRuleTested) {
    // start at the previously added element to gPrev, as all other elements will have the same index for sure
	RNode* testNode =   rules->getFirst();
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(*t,l->getTerm());
	// first element added to rTag was NULL, check for this
	while(NULL != testNode->getRule() && l->getRule() != lastRuleTested) {
        if(pLmDivisibleBy(testNode->getRuleTerm(),ppMult_qq(*t,l->getTerm()))) {
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
void computeSPols(CNode* first, RTagList* rTag, RList* rules, LList* sPolyList) { 
    CNode* temp         =   first;
    poly sp      =   pInit();
    Print("###############################IN SPOLS##############################\n");
    while(NULL != temp->getData()) {
        // only if a new rule was added since the last test in subalgorithm criticalPair()
        //if(rules->getFirst() != rTag->getFirst()) {
            Print("IN SPOLS => NEW RULES AVAILABLE\n");
            if(!criterion2(temp->getAdT1(),temp->getAdLp1(),rules,temp->getLastRuleTested())) {
                // the second component is tested only when it has the actual index, otherwise there is
                // no new rule to test since the last test in subalgorithm criticalPair()
                if(temp->getLp2Index() == temp->getLp1Index()) {
                    if(!criterion2(temp->getAdT2(),temp->getAdLp2(),rules,temp->getLastRuleTested())) {
                        // computation of S-polynomial
                        sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                         ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                        Print("BEGIN SPOLY1\n====================\n");
                        pWrite(sp);
                        Print("END SPOLY1\n====================\n");
                        if(NULL == sp) {
                            // as rules consist only of pointers we need to save the labeled
                            // S-polynomial also of a zero S-polynomial
                            //zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                            // origin of rule can be set NULL as the labeled polynomial
                            // will never be used again as it is zero => no problems with 
                            // further criterion2() tests and termination conditions
                            Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ZERO REDUCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
							reductionsToZero++;
                            rules->insert(temp->getLp1Index(),temp->getT1());
                            // as sp = NULL, delete it
                            delete(&sp);
                        }
                        else {
                            rules->insert(temp->getLp1Index(),temp->getT1());
                            sPolyList->insert(temp->getT1(),temp->getLp1Index(),sp,rules->getFirst()->getRule());
                        }
                        // data is saved in sPolyList or zero => delete sp
                    }
                }
                else { // temp->getLp2Index() < temp->getLp1Index()
                    // computation of S-polynomial
                    sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                     ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                    Print("BEGIN SPOLY2\n====================\n");
                    pWrite(sp);
                    Print("END SPOLY2\n====================\n");
                    if(NULL == sp) {
                        // zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                        // origin of rule can be set NULL as the labeled polynomial
                        // will never be used again as it is zero => no problems with 
                        // further criterion2() tests and termination conditions
                            Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ZERO REDUCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                        reductionsToZero++;
                        rules->insert(temp->getLp1Index(),temp->getT1());
                        // as sp = NULL, delete it
                        delete(&sp);
                    }
                    else {
                        rules->insert(temp->getLp1Index(),temp->getT1());
                        sPolyList->insert(temp->getT1(),temp->getLp1Index(),sp,rules->getFirst()->getRule());
                    }
                    // data is saved in sPolyList or zero => delete sp
                }
            }
        //}
        temp    =   temp->getNext();
    }
    // these critical pairs can be deleted now as they are either useless for further computations or 
    // already saved as an S-polynomial to be reduced in the following
    //pDelete(&sp);
    delete first;    
}



/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
void reduction(LList* sPolyList, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag, 
                 ideal gbPrev) { 
    Print("##########################################In REDUCTION!########################################\n");
     // LNode* saving the last element in gPrev before the completed elements from the reduction process are added
    // this is needed to get the new critical pairs computed
    LNode* gPrevTag = gPrev->getLast();
    //for debugging
    pWrite(gPrevTag->getPoly());
    // check if sPolyList has any elements
    // NOTE: due to initialization sPolyList always has a default NULL element
    while(sPolyList->getLength() > 1) {
        // temp is the first element in the sPolyList which should be reduced
        // due to earlier sorting this is the element of minimal degree AND 
        // minimal label
        LNode* temp =   sPolyList->getFirst();
        // delete the above first element from sPolyList, temp will be either reduced to
        // zero or added to gPrev, but never come back to sPolyList
        sPolyList->setFirst(temp->getNext());
        pWrite(temp->getPoly());
        poly tempNF = kNF(gbPrev,currQuotient,temp->getPoly());
        Print("LENGTH: %d\n",sPolyList->getLength());
        pWrite(tempNF);
        pWrite(temp->getPoly());
        if(NULL != tempNF) {
            // write the reduced polynomial in temp
            temp->setPoly(tempNF);
            // try further reductions of temp with polynomials in gPrev
            // with label index = current label index: this is done such that there 
            // is no label corruption during the reduction process
            topReduction(temp,completed,gPrev,rules,lTag,rTag);
        }
    }
}    



/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
void topReduction(LNode* l, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag) {
    Print("##########################################In topREDUCTION!########################################\n");

}


/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
LNode* findReductor(LNode* l,LList* completed,LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag,
                    LNode* gPrevRedCheck) {
   Print("HIER DU NULL!\n");
    return NULL;
}



/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int i,j;
    int gbLength;
    // 1 polynomial for defining initial labels & further tests
    poly ONE = pOne();
    // tag the first element of index i-1 for criterion 1 
    LTagList* lTag  =   new LTagList();
    Print("LTAG BEGINNING: %p\n",lTag->getFirst());
    
    // first element in rTag is first element of rules which is NULL RNode, 
    // this must be done due to possible later improvements
    RList* rules    =   new RList();
    RTagList* rTag  =   new RTagList(rules->getFirst());
    i = 1;
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
    LList* gPrev    =   new LList(ONE, i, id->m[0]);
    Print("%p\n",id->m[0]);
    pWrite(id->m[0]);
    Print("%p\n",gPrev->getFirst()->getPoly());
    pWrite(gPrev->getFirst()->getPoly());

    lTag->insert(gPrev->getFirst());
    // computing the groebner basis of the elements of index < actual index
    gbLength    =   gPrev->getLength();
    Print("Laenge der bisherigen Groebner Basis: %d\n",gPrev->getLength());
    ideal gbPrev    =   idInit(gbLength,1);
    // initializing the groebner basis of elements of index < actual index
    gbPrev->m[0]    =   gPrev->getFirst()->getPoly();
    idShow(gbPrev);
    idShow(currQuotient);

    for(i=2; i<=IDELEMS(id); i++) {
        gPrev   =   F5inc(i, id->m[i-1], gPrev, gbPrev, ONE, lTag, rules, rTag);
        // comuting new groebner basis gbPrev
        ideal gbAdd =   idInit(gPrev->getLength()-gbLength,1);
        LNode*  temp    =   gPrev->getFirst();
        for(j=0;j<=gPrev->getLength()-gbLength-1;j++) {
            gbAdd->m[j] =   temp->getPoly();
            temp        =   temp->getNext();
        }
        gbPrev  =   idAdd(gbPrev,gbAdd);
        Print("GROEBNER BASIS:\n====================================================\n");
        idShow(gbPrev);
        Print("===================================================\n");

        Print("JA\n");
    } 
    Print("\n\nNumber of zero-reductions:  %d\n",reductionsToZero);
    return(gbPrev);


}

#endif
