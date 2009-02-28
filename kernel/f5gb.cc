/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5gb.cc,v 1.36 2009-02-28 21:14:06 ederc Exp $ */
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
    //Print("%p\n",gPrev->getFirst());
    //pWrite(gPrev->getFirst()->getPoly());
    poly tempNF =   kNF(gbPrev,currQuotient,f_i);
    f_i         =   tempNF;
    gPrev->insert(ONE,i,f_i);
    // tag the first element in gPrev of the current index for findReductor()
    lTag->setFirstCurrentIdx(gPrev->getLast());
    //Print("1st gPrev: ");
    //pWrite(gPrev->getFirst()->getPoly());
    //Print("2nd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getPoly());
    //pWrite(gPrev->getFirst()->getNext()->getPoly());    
    CList* critPairs        =   new CList();
    CNode* critPairsMinDeg  =   new CNode();   
    // computation of critical pairs with checking of criterion 1 and criterion 2 and saving them
    // in the list critPairs
    criticalPair(gPrev, critPairs, lTag, rTag, rules);
    static LList* sPolyList        =   new LList();
    // labeled polynomials which have passed reduction() and have to be added to list gPrev
    static LList* completed        =   new LList();
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
        //while(NULL != temp && NULL != temp->getLPoly()) {
            //Print("Spolylist element: ");
            //pWrite(temp->getPoly());
            //temp    =   temp->getNext();
        //}
        // reduction process of new S-polynomials and also adds new critical pairs to critPairs
        reduction(sPolyList, critPairs, gPrev, rules, lTag, rTag, gbPrev);
        
        // DEBUG STUFF FOR GPREV
        //temp    =   gPrev->getFirst();
        //int number  =   1;
        //Print("\n\n");
        //while(NULL != temp) {
        //    Print("%d.  ",number);
        //    pWrite(temp->getPoly());
        //    temp    =   temp->getNext();
        //    number++;
        //    Print("\n");
        //}
        //sleep(5);
    
    }
    //Print("REDUCTION DONE\n");
    //Print("%p\n",rules->getFirst());
    //Print("%p\n",rTag->getFirst());
    if(rules->getFirst() != rTag->getFirst()) {
        //Print("+++++++++++++++++++++++++++++++++++++NEW RULES+++++++++++++++++++++++++++++++++++++\n");
        rTag->insert(rules->getFirst());
    }
    else {
        //Print("+++++++++++++++++++++++++++++++++++NO NEW RULES++++++++++++++++++++++++++++++++++++\n");
    }
    lTag->insert(lTag->getFirstCurrentIdx());
    //Print("LTAG LIST: \n");
    LNode* tempTag = lTag->getFirst();
    //Print("INDEX: %d\n",tempTag->getIndex());
    //pWrite(tempTag->getPoly());
    //Print("COMPLETED FIRST IN F5INC: \n");
    //Print("1st gPrev: ");
    //pWrite(gPrev->getFirst()->getPoly());
    //Print("2nd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getPoly());
    //Print("3rd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getNext()->getPoly());
 
 
    return gPrev;
}



/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
void criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules) {
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* newElement   =   gPrev->getLast();
    LNode* temp         =   gPrev->getFirst();
    poly u1             =   pOne();
    poly u2             =   pOne();
    poly lcm            =   pOne();
    poly t              =   pHead(newElement->getPoly());
    Rule* testedRule    =   rules->getFirst()->getRule();
    // computation of critical pairs
    while( gPrev->getLast() != temp) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
       // pWrite( *(l->getPoly()) );
        pLcm(newElement->getPoly(), temp->getPoly(), lcm);
        pSetCoeff(lcm,nOne);
        // computing factors u2 for new labels
        u1 = pDivide(lcm,t);
        pSetCoeff(u1,nOne);
        u2 = pDivide(lcm, pHead(temp->getPoly()));
        pSetCoeff(u2,nOne);
        //if(gPrev->getLast()->getIndex()==5) {
            //Print("IN CRITPAIRS\n");
        //    pWrite(u1);
        //    Print("1st ELEMENT: ");
        //    pWrite(newElement->getPoly());
        //    Print("2nd ELEMENT: ");
        //    pWrite(temp->getPoly());
        //}
        // testing both new labels by the F5 Criterion
        if(!criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag)) {
        //&& !criterion2(u1, newElement, rules, rTag) && !criterion2(u2, temp, rules, rTag)) {
            // if they pass the test, add them to CList critPairs, having the LPoly with greater
            // label as first element in the CPair
            if(newElement->getIndex() == temp->getIndex() && 
            -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                //Print("zweites groesser\n");
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2, 
                                temp->getLPoly(), u1, newElement->getLPoly(), testedRule);                   
                critPairs->insert(cp);
            }
            else {
                //Print("erstes groesser\n");
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1, 
                                newElement->getLPoly(), u2, temp->getLPoly(), testedRule);                   
                critPairs->insert(cp);
            }
        }
        else {
        }
        
        //Print("\n\n");
        temp    =   temp->getNext();
    }
    // for debugging
    //if(NULL != critPairs) {
        //critPairs->print();
        //sleep(5);
    //}
}




/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
this is a special version for reduction() in which the first 
generator of the critical pair is not tested by criterion2()
as there are no rules added until then to test for
================================================================
*/
void criticalPairRed(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules) {
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* newElement   =   gPrev->getLast();
    LNode* temp         =   gPrev->getFirst();
    poly u1             =   pOne();
    poly u2             =   pOne();
    poly lcm            =   pOne();
    poly t              =   pHead(newElement->getPoly());
    Rule* testedRule    =   rules->getFirst()->getRule();
    // computation of critical pairs
    while( gPrev->getLast() != temp) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
       // pWrite( *(l->getPoly()) );
        pLcm(newElement->getPoly(), temp->getPoly(), lcm);
        pSetCoeff(lcm,nOne);
        // computing factors u2 for new labels
        u1 = pDivide(lcm,t);
        pSetCoeff(u1,nOne);
        u2 = pDivide(lcm, pHead(temp->getPoly()));
        pSetCoeff(u2,nOne);
        //if(gPrev->getLast()->getIndex()==5) {
            //Print("IN CRITPAIRS\n");
        //    pWrite(u1);
        //    Print("1st ELEMENT: ");
        //    pWrite(newElement->getPoly());
        //    Print("2nd ELEMENT: ");
        //    pWrite(temp->getPoly());
        //}
        // testing both new labels by the F5 Criterion
        if(!criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag) && 
           !criterion2(u2, temp, rules, rTag)) {
            // if they pass the test, add them to CList critPairs, having the LPoly with greater
            // label as first element in the CPair
            if(newElement->getIndex() == temp->getIndex() && 
            -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                //Print("zweites groesser\n");
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2, 
                                temp->getLPoly(), u1, newElement->getLPoly(), testedRule);                   
                critPairs->insert(cp);
            }
            else {
                //Print("erstes groesser\n");
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1, 
                                newElement->getLPoly(), u2, temp->getLPoly(), testedRule);                   
                critPairs->insert(cp);
            }
        }
        else {
        }
        
        //Print("\n\n");
        temp    =   temp->getNext();
    }
    // for debugging
    //if(NULL != critPairs) {
        //critPairs->print();
        //sleep(5);
    //}
}




/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
bool criterion1(LList* gPrev, poly t, LNode* l, LTagList* lTag) {
    // starts at the first element in gPrev with index = (index of l)-1, these tags are saved in lTag
	int idx =   l->getIndex();
    if(idx == 1) {
        return false;
    }
    else {
        LNode* testNode =   gPrev->getFirst();
        // save the monom t1*label_term(l) as it is tested various times in the following
        poly u1 = ppMult_qq(t,l->getTerm());
        //Print("------------------------------IN CRITERION 1-----------------------------------------\n");
        //Print("TESTED ELEMENT: ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(ppMult_qq(t,l->getTerm()));
        //Print("%d\n\n",l->getIndex());
        while(testNode->getIndex() < idx && NULL != testNode->getLPoly()) {
            //pWrite(testNode->getPoly());
            //Print("%d\n",testNode->getIndex());
            if(pLmDivisibleByNoComp(testNode->getPoly(),u1)) {
                //Print("Criterion 1 NOT passed!\n");
                return true;
            }
            //pWrite(testNode->getNext()->getPoly());
            testNode    =   testNode->getNext();
        }
        return false;
    }
}



/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
bool criterion2(poly t, LNode* l, RList* rules, RTagList* rTag) {
    //Print("------------------------------IN CRITERION 2-----------------------------------------\n");
        //Print("RULES: \n");
        RNode* tempR    =   rules->getFirst();
        while(NULL != tempR->getRule()) {
            //Print("ADDRESS OF RULE: %p\n",tempR->getRule());
            //pWrite(tempR->getRuleTerm());
            //Print("ADDRESS OF TERM: %p\n",tempR->getRuleTerm());
            //Print("%d\n\n",tempR->getRuleIndex());
            tempR   =   tempR->getNext();
        }
        //Print("TESTED ELEMENT: ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(ppMult_qq(t,l->getTerm()));
        //Print("%d\n\n",l->getIndex());
// start at the previously added element to gPrev, as all other elements will have the same index for sure
    RNode* testNode =   new RNode();
    if(NULL == rTag->getFirst()->getRule()) {
        testNode    =   rules->getFirst();
    }
    else {
        if(l->getIndex() > rTag->getFirst()->getRuleIndex()) {
            testNode    =   rules->getFirst();
        }
        else {
        //Print("DEBUG\n");
        //Print("L INDEX: %d\n",l->getIndex());
            testNode    =   rTag->get(l->getIndex());
            //Print("TESTNODE ADDRESS: %p\n",testNode);
        }
    }
	// save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(t,l->getTerm());
    // first element added to rTag was NULL, check for this
    //Print("%p\n",testNode->getRule());
    // NOTE: testNode is possibly NULL as rTag->get() returns NULL for elements of index <=1!
    if(NULL != testNode && NULL != testNode->getRule()) {   
        //pWrite(testNode->getRuleTerm());
    }
    if(NULL != testNode) {
        if(testNode->getRule() == l->getRule()) {
            //Print("%p\n%p\n",testNode->getRule(),l->getRule());
            //Print("EQUAL\n");
        }
        else {
            //Print("NOT EQUAL\n");
        }
    }
    while(NULL != testNode && NULL != testNode->getRule() && testNode->getRule() != l->getRule() 
          && l->getIndex() == testNode->getRuleIndex()) {
        //pWrite(testNode->getRuleTerm());
		//pWrite(testNode->getRuleTerm());
        //pWrite(t);
        //pWrite(l->getTerm());
        //pWrite(u1);
        //Print("%d\n",testNode->getRuleIndex());
        if(pLmDivisibleBy(testNode->getRuleTerm(),u1)) {
            //Print("Criterion 2 NOT passed!\n");
            pDelete(&u1);
            return true;
        }
		testNode    =   testNode->getNext();
    }
    pDelete(&u1);
    return false;
}



/*
=================================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in computeSPols(), with added lastRuleTested parameter
=================================================================================================================
*/
bool criterion2(poly t, LPoly* l, RList* rules, Rule* testedRule) {
    //Print("------------------------------IN CRITERION 2-----------------------------------------\n");
    //Print("LAST RULE TESTED: %p",testedRule);
    //Print("RULES: \n");
        RNode* tempR    =   rules->getFirst();
        //while(NULL != tempR->getRule()) {
            //pWrite(tempR->getRuleTerm());
            //Print("%d\n\n",tempR->getRuleIndex());
            //tempR   =   tempR->getNext();
        //}
        //Print("TESTED ELEMENT: ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(ppMult_qq(t,l->getTerm()));
        //Print("%d\n\n",l->getIndex());
// start at the previously added element to gPrev, as all other elements will have the same index for sure
	RNode* testNode =   rules->getFirst();
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(t,l->getTerm());
	// first element added to rTag was NULL, check for this
	while(NULL != testNode->getRule() && testNode->getRule() != testedRule) {
        //pWrite(testNode->getRuleTerm());
        if(pLmDivisibleBy(testNode->getRuleTerm(),u1)) {
            //Print("Criterion 2 NOT passed!\n");
            pDelete(&u1);
            return true;
        }
		testNode    =   testNode->getNext();
    }
    pDelete(&u1);
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
    //Print("###############################IN SPOLS##############################\n");
    while(NULL != temp->getData()) {
        // only if a new rule was added since the last test in subalgorithm criticalPair()
        //if(rules->getFirst() != rTag->getFirst()) {
            if(!criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRule())) {
                // the second component is tested only when it has the actual index, otherwise there is
                // no new rule to test since the last test in subalgorithm criticalPair()
                if(temp->getLp2Index() == temp->getLp1Index()) {
                    if(!criterion2(temp->getT2(),temp->getAdLp2(),rules,temp->getTestedRule())) {
                        // computation of S-polynomial
                        sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                         ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                        //Print("BEGIN SPOLY1\n====================\n");
                        pNorm(sp);
                        //pWrite(sp);
                        //Print("END SPOLY1\n====================\n");
                        if(NULL == sp) {
                            // as rules consist only of pointers we need to save the labeled
                            // S-polynomial also of a zero S-polynomial
                            //zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                            // origin of rule can be set NULL as the labeled polynomial
                            // will never be used again as it is zero => no problems with 
                            // further criterion2() tests and termination conditions
                            //Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ZERO REDUCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
							reductionsToZero++;
                            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                            //Print("RULE ADDED: \n");
                            //pWrite(rules->getFirst()->getRuleTerm());
                            // as sp = NULL, delete it
                            pDelete(&sp);
                            //Print("HIER\n");
                        }
                        else {
                            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                            //Print("RULE ADDED: \n");
                            //pWrite(rules->getFirst()->getRuleTerm());  
                            sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRule());
                        }
                        // data is saved in sPolyList or zero => delete sp
                    }
                }
                else { // temp->getLp2Index() < temp->getLp1Index()
                    // computation of S-polynomial
                    sp      =   pSub(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                     ppMult_qq(temp->getT2(),temp->getLp2Poly()));
                    //Print("BEGIN SPOLY2\n====================\n");
                    pNorm(sp);
                    //pWrite(sp);
                    //Print("END SPOLY2\n====================\n");
                    if(NULL == sp) {
                        // zeroList->insert(temp->getAdT1(),temp->getLp1Index(),&sp);
                        // origin of rule can be set NULL as the labeled polynomial
                        // will never be used again as it is zero => no problems with 
                        // further criterion2() tests and termination conditions
                            //Print("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ZERO REDUCTION~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
                        reductionsToZero++;
                        rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                        //Print("RULE ADDED: \n");
                        //pWrite(rules->getFirst()->getRuleTerm()); 
                        // as sp = NULL, delete it
                        pDelete(&sp);
                    }
                    else {
                        rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                        //Print("RULE ADDED: \n");
                        //pWrite(rules->getFirst()->getRuleTerm()); 
                        //Print("%d\n",rules->getFirst()->getRuleIndex());
                        //Print("%p\n",sPolyList->getFirst());
                        sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRule());
                    }
                    // data is saved in sPolyList or zero => delete sp
                }
            }
        //}
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
void reduction(LList* sPolyList, CList* critPairs, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag, 
                 ideal gbPrev) { 
    //Print("##########################################In REDUCTION!########################################\n");
    // check if sPolyList has any elements
    // NOTE: due to initialization sPolyList always has a default NULL element
    while(sPolyList->getLength() > 0) {
        //Print("SPOLYLIST LENGTH: %d\n",sPolyList->getLength());
        if(sPolyList->getLength() > 1) {
        //Print("%p\n",sPolyList->getFirst());
        //Print("%p\n",sPolyList->getFirst()->getLPoly());
        //Print("%p\n",sPolyList->getFirst()->getNext());
        //Print("%p\n",sPolyList->getFirst()->getNext()->getLPoly());
        //Print("%p\n",sPolyList->getFirst());
        }
        //if(gPrev->getLast()->getIndex() == 5) {
            //sPolyList->print();
            //sleep(5);
        //}
        
        // temp is the first element in the sPolyList which should be reduced
        // due to earlier sorting this is the element of minimal degree AND 
        // minimal label
        LNode* temp =   sPolyList->getFirst();
        //pWrite(temp->getPoly());
        // delete the above first element from sPolyList, temp will be either reduced to
        // zero or added to gPrev, but never come back to sPolyList
        if(NULL != temp && NULL != temp->getLPoly()) {
            sPolyList->setFirst(temp->getNext());
        //Print("HIER\n");
        }
        else {
            break;
        }
        //Print("HALLO %p\n",temp->getPoly());
        //Print("%p\n",temp->getPoly());
        //pWrite(temp->getPoly());
        //idShow(gbPrev);
        poly tempNF = kNF(gbPrev,currQuotient,temp->getPoly());
        //Print("LENGTH: %d\n",sPolyList->getLength());
        //pWrite(tempNF);
        //pWrite(temp->getPoly());
        if(NULL != tempNF) {
            pNorm(tempNF);
            // write the reduced polynomial in temp
            temp->setPoly(tempNF);
            // try further reductions of temp with polynomials in gPrev
            // with label index = current label index: this is done such that there 
            // is no label corruption during the reduction process
            topReduction(temp,sPolyList,gPrev,rules,lTag,rTag,gbPrev);
        
        }
        if(NULL != temp->getPoly()) {
            //CList* newCritPairs = new CList;
            //Print("##################IN CRITPAIRS IN REDUCTION#####################\n");
            criticalPairRed(gPrev,critPairs,lTag,rTag,rules);
            //Print("+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++H I E R++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++\n");
        }
        else {
            //delete temp;
            LNode* tempLoop = gPrev->getFirst();
            //Print("AUSGABE IN REDUCTION:\n");        
            //while(NULL != tempLoop) {
                //pWrite(tempLoop->getPoly());
                //tempLoop = tempLoop->getNext();
            //}
            //sleep(10);
        }
    }
}    



/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
void topReduction(LNode* l, LList* sPolyList, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag, ideal gbPrev) {
    //Print("##########################################In topREDUCTION!########################################\n");
    // try l as long as there are reductors found by findReductor()
    do {
        LNode* tempRed  =   new LNode();
        //Print("TESTED POLYNOMIAL IN THE FOLLOWING: ");
        //pWrite(l->getPoly());
        //Print("HIER\n");
        tempRed  =   findReductor(l,gPrev,rules,lTag,rTag);
        //Print("--------------------------------HIER DEBUG 2----------------------------------\n");
        // if a reductor for l is found and saved in tempRed
        if(NULL != tempRed) {
            // if label of reductor is greater than the label of l we have to built a new element
            // and add it to sPolyList
            //Print("REDUCTOR POLYNOMIAL: ");
            //pWrite(tempRed->getPoly());
            //Print("TERM: ");
            //pWrite(tempRed->getTerm());
            if(pLmCmp(tempRed->getTerm(),l->getTerm()) == 1) {
                // needed sinc pSub destroys the arguments!
                //Print("H----------I------------E--------------R\n");
                poly temp_poly_l    =   pInit();
                temp_poly_l         =   pCopy(l->getPoly());
                //Print("POLYNOMIAL L: ");
                //pWrite(l->getPoly());
                //pWrite(temp_poly_l);
                poly temp           =   pSub(tempRed->getPoly(),temp_poly_l);
                //Print("POLYNOMIAL L: ");
                //pWrite(l->getPoly());
                //pWrite(temp_poly_l);
                //Print("AFTER REDUCTION STEP: ");
                //pWrite(temp);
                //sleep(20);
                //pWrite(temp);
                if(NULL != temp) {
                    pNorm(temp);
                    tempRed->setPoly(temp);
                    //pWrite(tempRed->getPoly());
                    // for debugging
                    //pWrite(tempRed->getPoly());
                    //Print("RULE ADDED\n");
                    rules->insert(tempRed->getIndex(),tempRed->getTerm());
                    tempRed->getLPoly()->setRule(rules->getFirst()->getRule());
                    //Print("%p\n",sPolyList->getFirst());
                    //Print("%p\n",sPolyList->getFirst()->getLPoly());
                    //Print("SPOLYLIST LENGTH: %d\n",sPolyList->getLength());
                    //sPolyList->print();
                    
                    sPolyList->insertByLabel(tempRed);
                    //sPolyList->print();
                    //Print("SPOLYLIST LENGTH: %d\n",sPolyList->getLength());
                    //Print("OH JE\n");
                }
                else {
                    pDelete(&temp);
                    reductionsToZero++;
                    //Print("RULE ADDED\n");
        //Print("wieder hier2\n");
                    rules->insert(tempRed->getIndex(),tempRed->getTerm());
                    delete tempRed;
                }
            }
            // label of reductor is smaller than the label of l, subtract reductor from l and delete the 
            // gPrevRedCheck pointer added to l during findReductor() as the head term of l changes 
            // after subtraction 
            else {
                poly temp_poly_l    =   pInit();
                temp_poly_l         =   pCopy(l->getPoly());
                poly temp   =   pSub(temp_poly_l,tempRed->getPoly());
                //Print("AFTER REDUCTION STEP: ");
                //pWrite(temp);
                if(NULL != temp) {
                    pNorm(temp);
                    poly tempNF =   kNF(gbPrev,currQuotient,temp);  
                    pNorm(tempNF);
                    //pWrite(tempNF);
                    if(NULL == tempNF) {
                        reductionsToZero++;
                        pDelete(&tempNF);
                        l->setPoly(NULL);
                        break;
                    }
                    l->setPoly(tempNF);
                    
                    //pWrite(l->getPoly());
                    l->setGPrevRedCheck(NULL);
                }
                else {
                    //Print("ZERO REDUCTION!\n");
                    reductionsToZero++;
                    pDelete(&temp);
                    l->setPoly(NULL);
                    //pWrite(gPrev->getLast()->getPoly()); 
                    break;
                }
            }    
        }
        else {
            if(NULL != l->getPoly()) {
                pNorm(l->getPoly());
                //Print("----------------------------------ADDED TO GPREV IN TOPREDUCTION:-------------------------------------- ");
                //pWrite(l->getPoly());
                //pWrite(l->getTerm());
                //Print("INDEX: %d\n\n\n", l->getIndex());
                //sleep(20);
                gPrev->insert(l->getLPoly());
                //Print("GPREV: \n");
                LNode* tempLoop = gPrev->getFirst();
                //while(NULL != tempLoop) {
                    //Print("HERE\n");
                    //pWrite(tempLoop->getPoly());
                    //tempLoop = tempLoop->getNext();
                //}
            }
            break;
        }
    } while(1);
}


/*
=====================================================================
subalgorithm to find a possible reductor for the labeled polynomial l
=====================================================================
*/
LNode* findReductor(LNode* l, LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag) {
    // allociation of memory for the possible reductor
    //Print("IN FIND REDUCTOR\n");
    poly u      =   pOne();
    poly red    =   pOne();
    number nOne =   nInit(1);
    LNode* temp =   new LNode();
    // head term of actual element such that we do not have to call pHead at each new reductor test
    poly t      =   pHead(l->getPoly());
    // if l was already checked use the information in gPrevRedCheck such
    // that we can start searching for new reducers from this point and 
    // not from the first element of gPrev with the current index
    if(NULL != l->getGPrevRedCheck()) {
        temp    =   l->getGPrevRedCheck()->getNext();
    } 
    // no reductors were searched for l before, thus start at the first
    // element of gPrev with the current index, tagged by lTag
    else {
        temp    =   lTag->getFirstCurrentIdx();
    }
    // search for reductors until we are at the end of gPrev resp. at the
    // end of the elements of the current index
    while(NULL != temp && temp->getIndex() == l->getIndex()) {
        // does the head of the element of gPrev divides the head of
        // the to be reduced element?
        //Print("-------------FOUND REDUCTORS----------------------\n");
        //Print("\n");
        //pWrite(temp->getPoly());
        //pWrite(temp->getTerm());
        //pWrite(t);
        //pWrite(l->getPoly());
        //Print("HALLO\n");
        if(pLmDivisibleByNoComp(temp->getPoly(),t)) {
        //Print("HALLO\n");
            // get all the information needed for the following tests
            // of the criteria
            u   =   pDivide(t,pHead(temp->getPoly()));
            pSetCoeff(u,nOne);
            //Print("HIER FINDRED\n");
            //pWrite(u);
            //Print("\n");
            red =   ppMult_qq(u,temp->getPoly());
            pNorm(red);
            //u   =   ppMult_qq(u,temp->getTerm());
            //pSetCoeff(u,nOne);
            // check if both have the same label
        //Print("HALLO\n");
            if(pLmCmp(u,l->getTerm()) != 0) {
        //Print("HALLO\n");
                // passing criterion2 ?
                if(!criterion2(u,temp,rules,rTag)) {
                    // passing criterion1 ?
                    if(!criterion1(gPrev,u,temp,lTag)) {
                            //Print("HIER DEBUG\n");
                            l->setGPrevRedCheck(temp);
                            LNode* redNode  =   new LNode(ppMult_qq(u,temp->getTerm()),temp->getIndex(),red,NULL,NULL);
                            return redNode;
                    }
                }
            }
        }
        //Print("%p\n",temp->getNext());
        //pWrite(temp->getPoly());
        //Print("HALLO\n");
        temp = temp->getNext();
    }
    
//    delete temp;
   //Print("1st gPrev: ");
    //pWrite(gPrev->getFirst()->getPoly());
    //Print("2nd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getPoly());
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
    //Print("LTAG BEGINNING: %p\n",lTag->getFirst());
    
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
    ideal idNew     =   kInterRed(id); 
    idDelete(&id);
    id              =   idNew;
    qsortDegree(&id->m[0],&id->m[IDELEMS(id)-1]);
    LList* gPrev    =   new LList(ONE, i, id->m[0]);
    //idShow(id); 
    //Print("%p\n",id->m[0]);
    //pWrite(id->m[0]);
    //Print("%p\n",gPrev->getFirst()->getPoly());
    //pWrite(gPrev->getFirst()->getPoly());

    lTag->insert(gPrev->getFirst());
    lTag->setFirstCurrentIdx(gPrev->getFirst());
    // computing the groebner basis of the elements of index < actual index
    gbLength    =   gPrev->getLength();
    //Print("Laenge der bisherigen Groebner Basis: %d\n",gPrev->getLength());
    ideal gbPrev    =   idInit(gbLength,1);
    // initializing the groebner basis of elements of index < actual index
    gbPrev->m[0]    =   gPrev->getFirst()->getPoly();
    //idShow(gbPrev);
    //idShow(currQuotient);

    for(i=2; i<=IDELEMS(id); i++) {
        LNode* gPrevTag =   gPrev->getLast();
        //Print("Last POlynomial in GPREV: ");
        //Print("%p\n",gPrevTag);    
        //pWrite(gPrevTag->getPoly());
        gPrev   =   F5inc(i, id->m[i-1], gPrev, gbPrev, ONE, lTag, rules, rTag);
        // DEBUGGING STUFF
        LNode* temp    =   gPrev->getFirst();
    // computing new groebner basis gbPrev
        if(gPrev->getLength() > gbLength) {
            ideal gbAdd =   idInit(gPrev->getLength()-gbLength,1);
        LNode*  temp    =   gPrevTag;
        //Print("%p\n",gPrevTag);    
        //Print("%p\n",gPrev->getLast());    
        //pWrite(temp->getPoly());
        //Print("LENGTH OF GPREV LIST: %d\n",gPrev->getLength());
        //Print("%d\n",gbLength);
        for(j=0;j<=gPrev->getLength()-gbLength-1;j++) {
            //Print("YES\n");
            temp        =   temp->getNext();
            gbAdd->m[j] =   temp->getPoly();
            //pWrite(temp->getPoly());
        }
        gbLength    =   gPrev->getLength(); 
        gbPrev  =   idAdd(gbPrev,gbAdd);
        }
        int anzahl  =   1;
        //while(NULL != temp) {
        //    Print("%d. Element: ",anzahl);
        //    pWrite(temp->getPoly());
        //    Print("\n");
        //    temp    =   temp->getNext();
        //    anzahl++;
        //} 
        //sleep(5);
        //Print("GROEBNER BASIS:\n====================================================\n");
        //idShow(gbPrev);
        //Print("===================================================\n");
        //Print("JA\n");
    } 
    Print("\n\nNumber of zero-reductions:  %d\n",reductionsToZero);
    //LNode* temp    =   gPrev->getFirst();
    //while(NULL != temp) {
    //    pWrite(temp->getPoly());
    //    temp    =   temp->getNext();
    // }
    delete(gPrev);
    return(gbPrev);


}

#endif
