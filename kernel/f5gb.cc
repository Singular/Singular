/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.27 2009-02-12 12:43:31 ederc Exp $ */
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
    Print("%p",gPrev->getFirst()->getNext());
    //pWrite(gPrev->getFirst()->getNext()->getPoly());    
    CList* critPairs        =   new CList();
    CNode* critPairsMinDeg  =   new CNode();   
    LNode* sPolys           =   new LNode();
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
        critPairsMinDeg =   critPairs->getMinDeg();
        // adds all to be reduced S-polynomials in the list sPolyList and adds 
        // the corresponding rules to the list rules
        // NOTE: inside there is a second check of criterion 2 if new rules are 
        // added
        computeSPols(critPairsMinDeg,rTag,rules,sPolyList);
        
        // DEBUG STUFF FOR SPOLYLIST
        LNode* temp     =   sPolyList->getFirst();
        while(NULL != temp->getLPoly()) {
            Print("Spolylist element: ");
            pWrite(temp->getPoly());
            temp    =   temp->getNext();
        } 
        reducedLPolys   =   reduction(sPolyList, completed, gPrev, rules, lTag, rTag, gbPrev);
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
    static poly sp      =   pInit();
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
LList* reduction(LList* sPolyList, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag, 
                 ideal gbPrev) { 
    Print("##########################################In REDUCTION!########################################\n");
    // temporary normal form and zero polynomial for testing
    static poly tempNF =   pInit();
    TopRed* ret =   new TopRed();
    // compute only if there are any S-polynomials to be reduced
    Print("LENGTH OF SPOLYLIST: %d\n",sPolyList->getLength());
    if(NULL != sPolyList->getFirst()->getLPoly()) {
        LNode* temp =   sPolyList->getFirst();
        Print("HIER REDUCTION\n");
        while(NULL != temp->getLPoly()) {
            if(NULL != completed->getFirst()->getLPoly()) {
				Print("BIS HIERHIN UND NICHT WEITER\n");	
				Print("%p\n",completed->getFirst());
				pWrite(completed->getFirst()->getPoly());
			}
			tempNF = kNF(gbPrev,currQuotient,temp->getPoly());  
            pWrite(tempNF);
            pWrite(temp->getPoly());
            temp->setPoly(tempNF);
            pWrite(temp->getPoly());
            // test if normal form is zero
            if(NULL == tempNF) {
                reductionsToZero++;
                // TODO: sPolyList -> delete first element of list as it is zero and done
                // TODO: problem is that when deleting the first element, the origin of the 
                // corresponding rule is deleted!
                temp    =   temp->getNext();
                //sPolyList->setFirst(temp);
                Print("///////////////////////////////////////////////////////////////////////\n");
                Print("%p\n",temp);
                Print("%p\n",temp->getLPoly());
                Print("///////////////////////////////////////////////////////////////////////\n");
            }
            else {
				Print("HIERLALA\n");
				if(NULL != completed->getFirst()->getLPoly()) {
					Print("%p\n",completed->getFirst());
					pWrite(completed->getFirst()->getPoly());
				}
				//
				//
				//
				//
				// NOTE: until this point the element completed->getFirst()->getPoly() exists!!!
				//
				//
				//
				// 
                ret =   topReduction(temp, completed, gPrev, rules, lTag, rTag);
                // in topReduction() the investigated first element of sPolyList will be deleted after its
                // reduction has finished => the next to be investigated element is the newly first element
                // in sPolyList => the while loop is finite
                // first possible after implementation of topReduction(): temp = sPolyList->getFirst();
                
				completed   =   ret->getCompleted();
				
				Print("%p\n",completed->getFirst());
				Print("%p\n",ret->getCompleted()->getFirst());
				pWrite(completed->getFirst()->getPoly());
            Print("~~~HIER~~~\n");
			Print("%d\n",completed->getFirst()->getIndex());
                if(NULL != ret->getToDo()) {
                    sPolyList->insertByLabel(ret->getToDo()->getFirst());
                }
            }
			temp	=	sPolyList->getFirst();
			Print("END WHILE LOOP: ");
			Print("%p\n",completed->getFirst());
			pWrite(completed->getFirst()->getPoly());
						
        }
    }
    return ret->getCompleted();
}    



/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
TopRed* topReduction(LNode* l, LList* completed, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag) {
    Print("##########################################In topREDUCTION!########################################\n");
    LNode* red   =   new LNode();
    do {
        red  =   findReductor(l, completed, gPrev, rules, lTag, rTag, 
                              red->getGPrevRedCheck(), red->getCompletedRedCheck());
            Print("HIER TOP RED\n");
        // no reductor found
        if(NULL == red) {
            pWrite(l->getPoly());
            pNorm(l->getPoly());
            pWrite(l->getPoly());
            Print("________________________INSERT IN COMPLETED!____________________________\n");
            completed->insert(l->getLPoly());
            Print("%p\n",completed->getFirst()->getLPoly());
			Print("%d\n",completed->getFirst()->getIndex());
            pWrite(completed->getFirst()->getPoly());
            TopRed* ret =   new TopRed(completed,NULL);
            return ret;
        }
        // reductor found
        else {
            Print("REDUCTOR VORHER:  ");
            pWrite(red->getPoly()); 
            red->setPoly(ppMult_nn(red->getPoly(),pGetCoeff(l->getPoly())));
            Print("REDUCTOR NACHHER:  ");
            pWrite(red->getPoly());
        }            
    } while(NULL != red);
}



/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
LNode* findReductor(LNode* l,LList* completed,LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag,
                    LNode* gPrevRedCheck, LNode* completedRedCheck) {
    number nOne     =   nInit(1);
    static poly u          =   pInit();
    static poly redPoly    =   pInit();
    poly t          =   pHead(l->getPoly());
    LNode* temp     =   new LNode();
    // setting starting point for search of reductors in gPrev
    Print("----------------------------------------------------------------------------%p\n",completed->getFirst()->getLPoly());
    if(NULL != gPrevRedCheck) { 
        temp =   gPrevRedCheck;
    }
    else {
        temp =   gPrev->getFirst();
    }
    // search only for reductors with the same index, as reductions with elements of lower
    // index were already done in reduction() beforehand
    while(NULL != temp->getLPoly() && temp->getIndex() == l->getIndex()) {
        // divides head term t?
        Print("HIER FIND\n");
        if(pLmDivisibleByNoComp(temp->getPoly(),t)) {
            u       =   pDivide(t,pHead(temp->getPoly()));
            pSetCoeff(u,nOne);
            // multiply reductor with factor and normalize it, i.e. LC = 1
            redPoly =   ppMult_qq(u,temp->getPoly());
            pNorm(redPoly);
            u       =   ppMult_qq(u,temp->getTerm());
            Print("IN FIND REDUCTOR:  ");
            pWrite(u);
            pWrite(redPoly);
            // same label? NOTE: also used to 
            if(pLmCmp(u,l->getTerm()) != 0) {
                // passing criterion 2?
                if(!criterion2(&u,temp, rules, rTag)) {
                    // passing criterion 1?
                Print("HIER DRIN\n");
                    if(!criterion1(&u,temp,lTag)) {
                        // set tag findRedCheck such that you can start at this point when the 
                        // next time a reductor is searched for in findReductor()
                        LNode* red      =   new LNode(u,temp->getIndex(),redPoly,NULL,temp->getNext(),completedRedCheck);
                        return red;
                    }
                }
            }
        }
        temp    =   temp->getNext();
    }

    // do the same as above now for the elements in completed
    if(NULL != completedRedCheck) {
        temp    =   completedRedCheck;
    }
    else {
        temp    =   completed->getFirst();
        Print("HIER FIND2\n");
		Print("%p\n",temp->getLPoly());
    }
    // search only for reductors with the same index, as reductions with elements of lower
    // index where already done in reduction() beforehand
    while(NULL != temp->getLPoly()) {
        // divides head term t?
        if(pLmDivisibleByNoComp(temp->getPoly(),t)) {
            u       =   pDivide(t,pHead(temp->getPoly()));
            redPoly =   ppMult_qq(u,temp->getPoly());
            u       =   ppMult_qq(u,temp->getTerm());
            Print("IN FIND REDUCTOR1:  ");
            pWrite(u);
            pWrite(redPoly);
            // same label? NOTE: also used to 
            if(pLmCmp(u,l->getTerm()) != 0) {
                // passing criterion 2?
                if(!criterion2(&u,temp, rules, rTag)) {
                    // passing criterion 1?
                    if(!criterion1(&u,temp,lTag)) {
                        // set tag findRedCheck such that you can start at this point when the 
                        // next time a reductor is searched for in findReductor()
                        LNode* red      =   new LNode(u,temp->getIndex(),redPoly,NULL,gPrevRedCheck,temp->getNext());
                        return red;
                    }
                }
            }
        }
        temp    =   temp->getNext();
    }

    // no reductor found
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
