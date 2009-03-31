/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: f5gb interface
*/

#include "mod2.h"
#ifdef HAVE_F5
#include <unistd.h>
#include "structs.h"
#include "kutil.h"
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
#include "timer.h"
int reductionsToZero    =   0;
int reductionTime       =   0;
int spolsTime           =   0;
int highestDegree       =   0;
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
    Print("in f5inc\n");            
    //pWrite(rules->getFirst()->getRuleTerm());
    int j;
    //Print("%p\n",gPrev->getFirst());
    //pWrite(gPrev->getFirst()->getPoly());
    poly tempNF =   kNF(gbPrev,currQuotient,f_i);
    f_i         =   tempNF;
    //gPrev->insert(ONE,i,f_i);
    gPrev->insert(ONE,gPrev->getLength()+1,f_i);
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
    //sPolyList->print();
    // labeled polynomials which have passed reduction() and have to be added to list gPrev
    static LList* completed        =   new LList();
    // the reduced labeled polynomials which are returned from subalgorithm reduction()
    static LList* reducedLPolys     =   new LList();
    // while there are critical pairs to be further checked and deleted/computed
    while(NULL != critPairs->getFirst()) { 
        // critPairs->getMinDeg() deletes the first elements of minimal degree from
        // critPairs, thus the while loop is not infinite.
        critPairsMinDeg =   critPairs->getMinDeg();
        // adds all to be reduced S-polynomials in the list sPolyList and adds 
        // the corresponding rules to the list rules
        // NOTE: inside there is a second check of criterion 2 if new rules are 
        // added
        //int timer4  =   initTimer();
        //startTimer();
        computeSPols(critPairsMinDeg,rTag,rules,sPolyList);
        //timer4  =   getTimer();
        //Print("SPOLS TIMER: %d\n",timer4);
        //spolsTime  =   spolsTime  +   timer4;
        // DEBUG STUFF FOR SPOLYLIST
        LNode* temp     =   sPolyList->getFirst();
        //while(NULL != temp && NULL != temp->getLPoly()) {
            //Print("Spolylist element: ");
            //pWrite(temp->getPoly());
            //temp    =   temp->getNext();
        //}
        // reduction process of new S-polynomials and also adds new critical pairs to critPairs
        int timer3  =   initTimer();
        startTimer();
        reduction(sPolyList, critPairs, gPrev, rules, lTag, rTag, gbPrev);
        timer3      =  getTimer();
        reductionTime = reductionTime + timer3;
        //Print("REDUCTION TIMER: %d\n",timer3); 
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
    //if(rules->getFirst() != rTag->getFirst()) {
        //Print("+++++++++++++++++++++++++++++++++++++NEW RULES+++++++++++++++++++++++++++++++++++++\n");
        //rTag->insert(rules->getFirst());
    //}
    //else {
        //Print("+++++++++++++++++++++++++++++++++++NO NEW RULES++++++++++++++++++++++++++++++++++++\n");
    //}
    lTag->insert(lTag->getFirstCurrentIdx());
    //Print("INDEX: %d\n",tempTag->getIndex());
    //pWrite(tempTag->getPoly());
    //Print("COMPLETED FIRST IN F5INC: \n");
    //Print("1st gPrev: ");
    //pWrite(gPrev->getFirst()->getPoly());
    //Print("2nd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getPoly());
    //Print("3rd gPrev: ");
    //pWrite(gPrev->getFirst()->getNext()->getNext()->getPoly());
    //delete sPolyList;
    //critPairs->print();
    delete critPairs;
    //Print("IN F5INC\n");
    /*
    Print("\n\n\nRULES: \n");
        RNode* tempR    =   rules->getFirst();
        Print("%p\n",tempR);
        int t   = 1;
        while(NULL != tempR) {
            Print("ADDRESS OF %d RNODE: %p\n",t,tempR);
            Print("ADDRESS OF RULE: %p\n",tempR->getRule());
            pWrite(tempR->getRuleTerm());
            Print("ADDRESS OF TERM: %p\n",tempR->getRuleTerm());
            Print("%d\n\n",tempR->getRuleIndex());
            tempR   =   tempR->getNext();
            t++;
        }
    */
    //gPrev->print();
    Print("COMPLETE REDUCTION TIME UNTIL NOW: %d\n",reductionTime);
    Print("COMPLETE SPOLS TIME UNTIL NOW:     %d\n",spolsTime);
    return gPrev;
}



/*
================================================================
computes a list of critical pairs for the next reduction process
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
inline void criticalPair(LList* gPrev, CList* critPairs, LTagList* lTag, RTagList* rTag, RList* rules) {
    //Print("IN CRITPAIRS\n");
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* newElement   =   gPrev->getLast();
    LNode* temp         =   gPrev->getFirst();
    poly u1             =   pOne();
    poly u2             =   pOne();
    poly lcm            =   pOne();
    poly t              =   pHead(newElement->getPoly());
    Rule* testedRule    =   NULL;
    if(NULL != rules->getFirst()) {
        testedRule  =   rules->getFirst()->getRule();
    }
    // computation of critical pairs
    //critPairs->print();
    while( gPrev->getLast() != temp) {
        //pWrite( *(gPrev->getFirst()->getPoly()) );
       // pWrite( *(l->getPoly()) );
       //pWrite(newElement->getPoly());
        //pWrite(temp->getPoly());
        pLcm(newElement->getPoly(), temp->getPoly(), lcm);
        pSetCoeff(lcm,nOne);
        // computing factors u2 for new labels
        //pExpVectorDiff(u1,lcm,t);
        //Print("U1: ");
        //pWrite(u1);
        u1 = pDivide(lcm,t);
        //pWrite(u1);
        pSetCoeff(u1,nOne);
        //Print("%p\n",u1);
        //critPairs->print();
        //pExpVectorDiff(u2,lcm,temp->getPoly());
        //Print("U2: ");
        //pWrite(u2);
        u2 = pDivide(lcm, pHead(temp->getPoly()));
        //pWrite(u2);
        //Print("%p\n",u2);
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
        //critPairs->print();
        if(!criterion2(gPrev->getFirst()->getIndex(), u2, temp, rules, rTag)
           && !criterion2(gPrev->getFirst()->getIndex(), u1, newElement, rules, rTag) 
           && !criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag)) {
            // if they pass the test, add them to CList critPairs, having the LPoly with greater
            // label as first element in the CPair
            if(newElement->getIndex() == temp->getIndex() && 
            -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                //Print("zweites groesser\n");
                
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2, 
                                temp->getLPoly(), u1, newElement->getLPoly(), testedRule);                   
                
                critPairs->insert(cp);
                //Print("LALA %p\n",critPairs->getFirst());
                //sleep(5);
            }
            else {
                CPair* cp   =   new CPair(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1, 
                                newElement->getLPoly(), u2, temp->getLPoly(), testedRule);                   
                //Print("erstes groesser\n");
                critPairs->insert(cp);
                //Print("LALA %p\n",critPairs->getFirst());
                //sleep(5);
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
    //Print("END CRITPAIRS\n");
}







/*
========================================
Criterion 1, i.e. Faugere's F5 Criterion
========================================
*/
inline bool criterion1(LList* gPrev, poly t, LNode* l, LTagList* lTag) {
    // starts at the first element in gPrev with index = (index of l)-1, these tags are saved in lTag
	int idx =   l->getIndex();
    int i;
    if(idx == 1) {
        //Print("HIER\n");
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
        while(testNode->getIndex() < idx) { // && NULL != testNode->getLPoly()) {
            //pWrite(testNode->getPoly());
            //Print("%d\n",testNode->getIndex());
            if(pLmDivisibleByNoComp(testNode->getPoly(),u1)) {
                //Print("Criterion 1 NOT passed!\n");
                return true;
            }
            //pWrite(testNode->getNext()->getPoly());
            testNode    =   testNode->getNext();
        }
        /*
        ideal testId    =   idInit(idx-1,1);
        for(i=0;i<idx-1;i++) {
            testId->m[i]  =   pHead(testNode->getPoly());
            testNode        =   testNode->getNext();
        }
        poly temp   =   kNF(testId,currQuotient,u1);
        //pWrite(temp);
        for(i=0;i<IDELEMS(testId);i++) {
            testId->m[i]    =   NULL;
        }
        idDelete(&testId);
        if(NULL == temp) {
            //if(l->getIndex() != gPrev->getFirst()->getIndex()) {
            //    Print("----------------------------Criterion1 not passed----------------------------------\n");
            //}
            return true;
        }
        */
        return false;
    }
}



/*
=====================================
Criterion 2, i.e. Rewritten Criterion
=====================================
*/
inline bool criterion2(int idx, poly t, LNode* l, RList* rules, RTagList* rTag) {
    //Print("------------------------------IN CRITERION 2/1-----------------------------------------\n");
    /*  
    Print("RULES: \n");
        RNode* tempR    =   rules->getFirst();
        Print("%p\n",tempR);
        int i   = 1;
        while(NULL != tempR) {
            Print("ADDRESS OF %d RNODE: %p\n",i,tempR);
            Print("ADDRESS OF RULE: %p\n",tempR->getRule());
            pWrite(tempR->getRuleTerm());
            Print("ADDRESS OF TERM: %p\n",tempR->getRuleTerm());
            Print("%d\n\n",tempR->getRuleIndex());
            tempR   =   tempR->getNext();
            i++;
        }
        //Print("TESTED ELEMENT: ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(ppMult_qq(t,l->getTerm()));
        //Print("%d\n\n",l->getIndex());
      */
// start at the previously added element to gPrev, as all other elements will have the same index for sure
    if(idx > l->getIndex()) {
        return false;
    }
    
    RNode* testNode; // =   new RNode();
    

    if(NULL == rTag->getFirst()) {
        if(NULL != rules->getFirst()) {
            testNode    =   rules->getFirst();
        }
        else {
            return false;
        }
    }
    else {

        if(l->getIndex() > rTag->getFirst()->getRuleIndex()) {
            testNode    =   rules->getFirst();
        }
        else {
       //Print("HIER\n"); 
            //Print("DEBUG\n");
        //Print("L INDEX: %d\n",l->getIndex());
            /*-------------------------------------
             * TODO: WHEN INTERREDUCING THE GB THE
             *       INDEX OF THE PREVIOUS ELEMENTS
             *       GETS HIGHER!
             *-----------------------------------*/
            //testNode    =   rules->getFirst();
            testNode    =   rTag->get(l->getIndex());
            if(NULL == testNode) {
                testNode    =   rules->getFirst();
            }
            //Print("TESTNODE ADDRESS: %p\n",testNode);
        }
    }
    //testNode    =   rules->getFirst();
	// save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(t,l->getTerm());
    // first element added to rTag was NULL, check for this
    //Print("%p\n",testNode->getRule());
    // NOTE: testNode is possibly NULL as rTag->get() returns NULL for elements of index <=1!
    //Print("TESTNODE: %p\n",testNode);
    //pWrite(testNode->getRuleTerm());
    if(NULL != testNode ) {   
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
    while(NULL != testNode && testNode->getRule() != l->getRule() 
          && l->getIndex() == testNode->getRuleIndex()) {
        //Print("%p\n",testNode);
        //pWrite(testNode->getRuleTerm());
        //pWrite(t);
        //pWrite(l->getTerm());
        //pWrite(u1);
        //Print("%d\n",testNode->getRuleIndex());
        if(pLmDivisibleByNoComp(testNode->getRuleTerm(),u1)) {
            //Print("-----------------Criterion 2 NOT passed!-----------------------------------\n");
            //Print("INDEX: %d\n",l->getIndex());
            pDelete(&u1);
    //Print("------------------------------IN CRITERION 2/1-----------------------------------------\n\n");
            return true;
        }
		testNode    =   testNode->getNext();
    }
    //delete testNode;
    pDelete(&u1);
    //Print("------------------------------IN CRITERION 2/1-----------------------------------------\n\n");
    return false;
}



/*
=================================================================================================================
Criterion 2, i.e. Rewritten Criterion, for its second call in computeSPols(), with added lastRuleTested parameter
=================================================================================================================
*/
inline bool criterion2(poly t, LPoly* l, RList* rules, Rule* testedRule) {
    //Print("------------------------------IN CRITERION 2/2-----------------------------------------\n");
    //Print("LAST RULE TESTED: %p",testedRule);
    /*
    Print("RULES: \n");
        RNode* tempR    =   rules->getFirst();
        while(NULL != tempR) {
            pWrite(tempR->getRuleTerm());
            Print("%d\n\n",tempR->getRuleIndex());
            tempR   =   tempR->getNext();
        }
        //Print("TESTED ELEMENT: ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(ppMult_qq(t,l->getTerm()));
        //Print("%d\n\n",l->getIndex());
    */
// start at the previously added element to gPrev, as all other elements will have the same index for sure
	RNode* testNode =   rules->getFirst();
    // save the monom t1*label_term(l) as it is tested various times in the following
    poly u1 = ppMult_qq(t,l->getTerm());
	// first element added to rTag was NULL, check for this
	while(NULL != testNode && testNode->getRule() != testedRule) {
        //pWrite(testNode->getRuleTerm());
        if(pLmDivisibleByNoComp(testNode->getRuleTerm(),u1)) {
            //Print("--------------------------Criterion 2 NOT passed!------------------------------\n");
            //Print("INDEX: %d\n",l->getIndex());
            pDelete(&u1);
    //Print("------------------------------IN CRITERION 2/2-----------------------------------------\n\n");
            return true;
        }
		testNode    =   testNode->getNext();
    }
    pDelete(&u1);
    //Print("------------------------------IN CRITERION 2/2-----------------------------------------\n\n");
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
    //first->print();

    while(NULL != temp) {
        //Print("JA\n");
        // only if a new rule was added since the last test in subalgorithm criticalPair()
        //if(rules->getFirst() != rTag->getFirst()) {
        if(!criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRule())) {
                // the second component is tested only when it has the actual index, otherwise there is
                // no new rule to test since the last test in subalgorithm criticalPair()
                if(highestDegree < pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()))) { 
                    highestDegree   = pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()));
                    //pWrite(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
                }   
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
                        //Print("IN SPOLS 1\n");
                            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                            //Print("RULE ADDED: \n");
                            //pWrite(rules->getFirst()->getRuleTerm());
                            // as sp = NULL, delete it
                            pDelete(&sp);
                            //Print("HIER\n");
                        }
                        else {
                        //Print("IN SPOLS 2\n");
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
                        //Print("IN SPOLS 3\n");
                        rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                        //Print("RULE ADDED: \n");
                        //pWrite(rules->getFirst()->getRuleTerm()); 
                        // as sp = NULL, delete it
                        pDelete(&sp);
                    }
                    else {
                        //Print("IN SPOLS 4\n");
                        rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
                        //Print("RULE ADDED: \n");
                        //pWrite(rules->getFirst()->getRuleTerm()); 
                        //Print("%d\n",rules->getFirst()->getRuleIndex());
                        //Print("%p\n",sPolyList->getFirst());
                        sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRule());
                    }
                }
            }
        //}
        //Print("%p\n",temp);
        temp    =   temp->getNext();
        //Print("%p\n",temp);
        //Print("%p\n",temp->getData());
        //pWrite(temp->getLp1Poly());
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
    LNode* temp;
    while(sPolyList->getLength() > 0) {
        // temp is the first element in the sPolyList which should be reduced
        // due to earlier sorting this is the element of minimal degree AND 
        // minimal label
        temp =   sPolyList->getFirst();
        // delete the above first element from sPolyList, temp will be either reduced to
        // zero or added to gPrev, but never come back to sPolyList
        if(NULL != temp) { 
            sPolyList->setFirst(temp->getNext());
        }
        else {
            break;
        }
        poly tempNF = kNF(gbPrev,currQuotient,temp->getPoly());
        if(NULL != tempNF) {
            pNorm(tempNF);
            temp->setPoly(tempNF);
            // try further reductions of temp with polynomials in gPrev
            // with label index = current label index: this is done such that there 
            // is no label corruption during the reduction process
            topReduction(temp,sPolyList,gPrev,rules,lTag,rTag,gbPrev);
        
        }
        if(NULL != temp->getPoly()) {
            criticalPair(gPrev,critPairs,lTag,rTag,rules);
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
        LNode* gPrevRedCheck    =   new LNode(lTag->getFirstCurrentIdx());
        LNode* tempRed          =   new LNode();
        tempRed  =   findReductor(l,gPrevRedCheck,gPrev,rules,lTag,rTag);
        // if a reductor for l is found and saved in tempRed
        if(NULL != tempRed) {
            // if label of reductor is greater than the label of l we have to built a new element
            // and add it to sPolyList
            if(pLmCmp(tempRed->getTerm(),l->getTerm()) == 1) {
                // needed sinc pSub destroys the arguments!
                poly temp_poly_l    =   pInit();
                temp_poly_l         =   pCopy(l->getPoly());
                poly temp           =   pSub(tempRed->getPoly(),temp_poly_l);
                if(NULL != temp) {
                    pNorm(temp);
                    tempRed->setPoly(temp);
                    tempRed->setDel(1);
                    rules->insert(tempRed->getIndex(),tempRed->getTerm());
                    tempRed->getLPoly()->setRule(rules->getFirst()->getRule());
                    sPolyList->insertByLabel(tempRed);
                }
                else {
                    pDelete(&temp);
                    reductionsToZero++;
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
                if(NULL != temp) {
                    pNorm(temp);
                    poly tempNF =   kNF(gbPrev,currQuotient,temp);  
                    pNorm(tempNF);
                    if(NULL == tempNF) {
                        reductionsToZero++;
                        pDelete(&tempNF);
                        l->setPoly(NULL);
                        break;
                    }
                    l->setPoly(tempNF);
                    
                    gPrevRedCheck   =   lTag->getFirstCurrentIdx();
                }
                else {
                    reductionsToZero++;
                    pDelete(&temp);
                    l->setPoly(NULL);
                    break;
                }
            }    
        }
        else {
            if(NULL != l->getPoly()) {
                pNorm(l->getPoly());
                gPrev->insert(l->getLPoly());
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
LNode* findReductor(LNode* l, LNode* gPrevRedCheck, LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag) {
    // allociation of memory for the possible reductor
    poly u      =   pOne();
    poly red    =   pOne();
    number nOne =   nInit(1);
    LNode* temp =   new LNode();
    // head term of actual element such that we do not have to call pHead at each new reductor test
    poly t      =   pHead(l->getPoly());
    // if l was already checked use the information in gPrevRedCheck such
    // that we can start searching for new reducers from this point and 
    // not from the first element of gPrev with the current index
    temp    =   gPrevRedCheck;
    // search for reductors until we are at the end of gPrev resp. at the
    // end of the elements of the current index
    while(NULL != temp && temp->getIndex() == l->getIndex()) {
        // does the head of the element of gPrev divides the head of
        // the to be reduced element?
        if(pLmDivisibleByNoComp(pHead(temp->getPoly()),t)) {
            // get all the information needed for the following tests
            // of the criteria
            u   =   pDivide(t,pHead(temp->getPoly()));
            pSetCoeff(u,nOne);
            red =   ppMult_qq(u,temp->getPoly());
            pNorm(red);
            // check if both have the same label
            if(pLmCmp(u,l->getTerm()) != 0) {
                // passing criterion2 ?
                if(!criterion2(gPrev->getFirst()->getIndex(), u,temp,rules,rTag)) {
                    // passing criterion1 ?
                    if(!criterion1(gPrev,u,temp,lTag)) {
                            gPrevRedCheck   =   temp->getNext();
                            LNode* redNode  =   new LNode(ppMult_qq(u,temp->getTerm()),temp->getIndex(),red,NULL,NULL);
                            return redNode;
                    }
                }
            }
        }
        temp = temp->getNext();
    }
    
//    delete temp;
 return NULL;
}



/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
==========================================================================
*/
ideal F5main(ideal id, ring r) {
    int timer   =   initTimer();
    startTimer();
    int i,j,k,l;
    int gbLength;
    // 1 polynomial for defining initial labels & further tests
    poly ONE = pOne();
    poly pOne   =   pOne();
    number nOne =   nInit(1);
    // tag the first element of index i-1 for criterion 1 
    LTagList* lTag  =   new LTagList();
    //Print("LTAG BEGINNING: %p\n",lTag);
    
    // DEBUGGING STUFF START
    //Print("NUMBER: %d\n",r->N);
    
    //int* ev = new int[r->N];
    //int  ev2;
    //for(i=0;i<IDELEMS(id);i++) {
        //pGetExpV(id->m[i],ev);
        //ev2  =   pGetExp(id->m[i],1);
        //pWrite(id->m[i]);
        //Print("%d\n",ev2);
        //Print("EXP1: %d\n",ev[1]);
        //Print("EXP2: %d\n",ev[2]);
        //Print("EXP3: %d\n\n",ev[3]);
    //}
    //delete ev;
    
    /*DEBUGGING STUFF END */
    
    // first element in rTag is first element of rules which is NULL RNode, 
    // this must be done due to possible later improvements
    RList* rules    =   new RList();
    //Print("RULES FIRST: %p\n",rules->getFirst());
    //Print("RULES FIRST DATA: %p\n",rules->getFirst()->getRule());
    RTagList* rTag  =   new RTagList(rules->getFirst());
    i = 1;
    /*for(j=0; j<IDELEMS(id); j++) {
        if(NULL != id->m[j]) { 
            if(pComparePolys(id->m[j],ONE)) {
                Print("One Polynomial in Input => Computations stopped\n");
                ideal idNew = idInit(1,1);
                idNew->m[0] = ONE;
                return(idNew);
            }   
        }
    }*/ 
    ideal idNew     =   kInterRed(id); 
    id              =   idNew;
    //qsortDegree(&id->m[0],&id->m[IDELEMS(id)-1]);
    idShow(id);
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
        Print("%d\n",gPrev->count(gPrevTag->getNext()));
        Print("%d\n",gPrev->getLength());
        //Print("____________________________________ITERATION STEP DONE________________________________________\n");
        
        // DEBUGGING STUFF
        LNode* temp    =   gPrev->getFirst();
    // computing new groebner basis gbPrev
        if(gPrev->getLength() > gbLength) {
            if(i < IDELEMS(id)) {
                ideal gbAdd =   idInit(gPrev->getLength()-gbLength,1);
                LNode*  temp =   gPrevTag;
                int counter =   0;
                for(j=0;j<=gPrev->getLength()-gbLength-1;j++) {
                    temp        =   temp->getNext();
                    if(0 == temp->getDel()) {
                        counter++;
                        gbAdd->m[j] =   temp->getPoly();
                    }
                        //if(1 == temp->getDel()) {
                        //    pWrite(temp->getPoly());
                        //}
                }
                if(counter != gPrev->count(gPrevTag->getNext())) {
                    Print("----------------------------------WRONG COUNTING---------------------------\n");
                }
                    gbPrev          =   idAdd(gbPrev,gbAdd);
                    //idShow(gbPrev);
            }
            else {
                ideal gbAdd =   idInit(gPrev->getLength()-gbLength,1);
                LNode*  temp =   gPrevTag;
                for(j=0;j<=gPrev->getLength()-gbLength-1;j++) {
                    temp        =   temp->getNext();
                    gbAdd->m[j] =   temp->getPoly();
                }
                gbPrev          =   idAdd(gbPrev,gbAdd);
            }
            // interreduction stuff
            if(i<IDELEMS(id)) {
                int timer2  =   initTimer();
                startTimer();
                ideal tempId    =   kInterRed(gbPrev);
                
                //idShow(tempId);
                gbPrev          =   tempId;
                timer2  =   getTimer();
                Print("Timer INTERREDUCTION: %d\n",timer2);
                //idShow(gbPrev);
                //qsortDegree(&gbPrev->m[0],&gbPrev->m[IDELEMS(gbPrev)-1]);
                delete gPrev;
                //sleep(5);
                //Print("RULES FIRST NOW1: %p\n",rules->getFirst());
                //Print("HIER\n");
                delete rules;
                //delete rTag;
                //Print("HIER AUCH\n");
                //Print("%p\n",rules->getFirst());
                gPrev    =   new LList(pOne,1,gbPrev->m[0]);
                gPrev->insert(pOne,1,gbPrev->m[1]);
                //poly tempPoly = pInit();
                //pLcm(pHead(gbPrev->m[0]),pHead(gbPrev->m[1]),tempPoly);
                //tempPoly    =   pDivide(tempPoly,pOne());
                //pSetCoeff(tempPoly,nOne);
                rules    =   new RList();
                rTag     =   new RTagList(rules->getFirst());
                
                //Print("%p\n",rules->getFirst());
                //pWrite(tempPoly);
                //rules->insert(2,tempPoly);
                //rTag->insert(rules->getFirst());
                //Print("%p\n",rules->getFirst());
                //Print("%p\n",rTag->getFirst());
                //Print("%p\n",rules->getFirst());
                //Print("%p\n",rules->getFirst()->getNext()->getNext());
                //Print("HIERLALA\n");
            //pWrite(rules->getFirst()->getRuleTerm());
           // Print("RULES FIRST NOW2: %p\n",rules->getFirst());
                for(k=2; k<IDELEMS(gbPrev); k++) {
                    gPrev->insert(pOne,k+1,gbPrev->m[k]);
                    for(l=0; l<k; l++) {
                        //pWrite(gbPrev->m[k]);
                        //pWrite(gbPrev->m[l]);
                        //poly tempPoly2  =   pOne();
                        //pLcm(pHead(gbPrev->m[k]),pHead(gbPrev->m[l]),tempPoly2);
                        //tempPoly2   =   pDivide(tempPoly2,pOne());
                        //pSetCoeff(tempPoly2,nOne);
                        //pWrite(tempPoly2);
                        //rules->insert(k+1,tempPoly2);
                    }
                    //rTag->insert(rules->getFirst());
                }
            }
            gbLength    =   gPrev->getLength(); 
            //Print("HIER\n");
        }
        //gPrev->print();
        //int anzahl  =   1;
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
        Print("LENGTH OF GPREV: %d\n",gPrev->getLength());
    } 
        //idShow(gbPrev);
    Print("\n\nNumber of zero-reductions:  %d\n",reductionsToZero);
    timer   =   getTimer();
    Print("Highest Degree during computations: %d\n",highestDegree);
    Print("Time for computations: %d/10000 seconds\n",timer);
    //LNode* temp    =   gPrev->getFirst();
    //while(NULL != temp) {
    //    pWrite(temp->getPoly());
    //    temp    =   temp->getNext();
    // }
    //gPrev->print();
    //delete lTag;
    //delete rTag;
    //delete gPrev;
    reductionTime   =   0;
    spolsTime       =   0;
    return(gbPrev);


}

#endif
