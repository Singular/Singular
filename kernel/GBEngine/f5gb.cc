/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: f5gb interface
*/

#include "kernel/mod2.h"
#ifdef HAVE_F5
#include <unistd.h>
#include <omp.h>
#include "kernel/structs.h"
#include "kernel/GBEngine/kutil.h"
#include "kernel/polys.h"
#include "polys/monomials/p_polys.h"
#include "polys/templates/p_Procs.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/f5gb.h"
#include "kernel/GBEngine/f5data.h"
#include "kernel/GBEngine/f5lists.h"
#include "kernel/oswrapper/timer.h"

#define pDeg(A) p_Deg(A,currRing)

THREAD_VAR int notInG              =   0;
THREAD_VAR int numberOfRules       =   0;
THREAD_VAR int reductionsToZero    =   0;
THREAD_VAR int reductionTime       =   0;
THREAD_VAR int spolsTime           =   0;
THREAD_VAR int highestDegree       =   0;
THREAD_VAR int degreeBound         =   0;
THREAD_VAR int numberUsefulPairs   =   0;
THREAD_VAR int numberUselessPairs  =   0;
THREAD_VAR int numberUsefulPairsMinDeg = 0;
THREAD_VAR int highestDegreeGBCriticalPair = 0;
THREAD_VAR int numberRejectedF5CriticalPairs = 0;
THREAD_VAR int numberOfReductions  = 0;
THREAD_VAR int numberOfSpolys  = 0;
/*
====================================================================
sorting ideals by decreasing total degree "left" and "right" are the
pointer of the first and last polynomial in the considered ideal
====================================================================
*/
void qsortDegree(poly* left, poly* right)
{
    poly* ptr1 = left;
    poly* ptr2 = right;
    poly p1,p2;
    p2 = *(left + (right - left >> 1));
    do
    {
            while(p_Totaldegree(*ptr1, currRing) < p_Totaldegree(p2, currRing))
            {
                    ptr1++;
            }
            while(p_Totaldegree(*ptr2, currRing) > p_Totaldegree(p2,currRing))
            {
                    ptr2--;
            }
            if(ptr1 > ptr2)
            {
                    break;
            }
            p1    = *ptr1;
            *ptr1 = *ptr2;
            *ptr2 = p1;
    } while(++ptr1 <= --ptr2);

    if(left < ptr2)
    {
            qsortDegree(left,ptr2);
    }
    if(ptr1 < right)
    {
            qsortDegree(ptr1,right);
    }
}

/*!
 * ======================================================================
 * builds the sum of the entries of the exponent vectors, i.e. the degree
 * of the corresponding monomial
 * ======================================================================
*/
long sumVector(int* v, int k) {
    int i;
    long sum =  0;
    for(i=1; i<=k; i++) {
        Print("%d\n",v[i]);
        Print("%ld\n",sum);
        sum =   sum + v[i];
    }
    return sum;
}

/*!
==========================================================================
compares monomials, i.e. divisibility tests for criterion 1 and criterion 2
==========================================================================
*/
bool compareMonomials(int* m1, int** m2, int numberOfRules, int k) {
    int i,j;
    long sumM1  =   sumVector(m1,k);
    //int k   =   sizeof(m1) / sizeof(int);
    for(i=0; i<numberOfRules; i++) {
        if(sumVector(m2[i],k) <= sumM1) {
            for(j=1; j<=k; j++) {
                if(m1[j]>m2[i][j]) {
                    return true;
                }
            }
        }
    }
    return false;
}

/*
==================================================
computes incrementally gbs of subsets of the input
gb{f_m} -> gb{f_m,f_(m-1)} -> gb{f_m,...,f_1}
==================================================
*/
LList* F5inc(int i, poly f_i, LList* gPrev, LList* reducers, ideal gbPrev, poly ONE, LTagList* lTag, RList* rules, RTagList* rTag, int plus, int termination) {
    //Print("in f5inc\n");
    //pWrite(rules->getFirst()->getRuleTerm());
    int iterationstep = i;
    int j;
    //Print("%p\n",gPrev->getFirst());
    //pWrite(gPrev->getFirst()->getPoly());
    poly tempNF =   kNF(gbPrev,currRing->qideal,f_i);
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
    CListOld* critPairs        =   new CListOld();
    CNode* critPairsMinDeg  =   new CNode();
    PList* rejectedGBList = new PList();
    // computation of critical pairs with checking of criterion 1 and criterion 2 and saving them
    // in the list critPairs
    criticalPair(gPrev, critPairs, lTag, rTag, rules, rejectedGBList, plus);
    THREAD_VAR static LList* sPolyList        =   new LList();
    //sPolyList->print();
    // labeled polynomials which have passed reduction() and have to be added to list gPrev
    THREAD_VAR static LList* completed        =   new LList();
    // the reduced labeled polynomials which are returned from subalgorithm reduction()
    THREAD_VAR static LList* reducedLPolys     =   new LList();
    // while there are critical pairs to be further checked and deleted/computed
    while(NULL != critPairs->getFirst()) {
        // critPairs->getMinDeg() deletes the first elements of minimal degree from
        // critPairs, thus the while loop is not infinite.
        // adds all to be reduced S-polynomials in the list sPolyList and adds
        // the corresponding rules to the list rules
        // NOTE: inside there is a second check of criterion 2 if new rules are
        // added
        //int timer4  =   initTimer();
        //startTimer();
        //critPairs->print();

        //definition of a local numberUsefulPairs variable for the next degree
        //step:
        //if in this degree all pairs are useless, skip this degree step
        //completely!
        //long arrideg = critPairs->getFirst()->getData()->getDeg();
        //if(plus && highestDegreeGBCriticalPair < critPairs->getFirst()->getData()->getDeg()) {
        //  return gPrev;
        //}
        //else {
        critPairsMinDeg =   critPairs->getMinDeg();
        //numberUsefulPairsMinDeg = computeUsefulMinDeg(critPairsMinDeg);
        //if(numberUsefulPairs > 0) {
          //if(numberUsefulPairsMinDeg > 0) {
            //Print("number of useful pairs:  %d\n",numberUsefulPairs);
            //Print("number of useless pairs: %d\n\n",numberUselessPairs);
          //Print("Degree of following reduction: %d\n",critPairsMinDeg->getData()->getDeg());
        long degreecheck = critPairsMinDeg->getData()->getDeg();

          computeSPols(critPairsMinDeg,rTag,rules,sPolyList, rejectedGBList);
        //}
          //}
        //}
        //else {
        //  return gPrev;
        //}
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
        //int timer3  =   initTimer();
        //startTimer();
        //sPolyList->print();
        //reduction(sPolyList, critPairs, gPrev, rules, lTag, rTag, gbPrev);
        //Print("BEFORE REDUCTION: \n");
        //Print("number of useful pairs:  %d\n",numberUsefulPairs);
        //Print("number of useless pairs: %d\n\n",numberUselessPairs);
        newReduction(sPolyList, critPairs, gPrev, reducers, rules, lTag, rTag, gbPrev, termination, rejectedGBList, plus);
        //Print("ITERATION:  %d",iterationstep);
        //Print("NAECHSTER GRAD:  %ld",degreecheck);
        //sleep(5);
        //if(i == 5 && pDeg(gPrev->getLast()->getPoly()) == 8) {
        //  Print("RAUS BEI DEG 8 \n");
        //  return gPrev;
        //}
        //Print("ARRIS DEG: %ld\n",arrideg);
        // Arris idea stated by John in an email
        //if(arrisCheck(critPairs->getFirst(),gPrev->getFirst(),arrideg)) {
        //  Print("ARRIS DEGREE: %ld\n", arrideg);
        //  return gPrev;
        //}


        //bool aha = checkDGB(gPrev);


        //timer3      =  getTimer();
        //reductionTime = reductionTime + timer3;
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
    PrintS("\n\n\nRULES: \n");
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
    //Print("COMPLETE REDUCTION TIME UNTIL NOW: %d\n",reductionTime);
    //Print("COMPLETE SPOLS TIME UNTIL NOW:     %d\n",spolsTime);
    degreeBound = 0;
    return gPrev;
}



bool checkDGB(LList* gPrev) {
  Print("D-GB CHECK at degree %ld\n",pDeg(gPrev->getLast()->getPoly()));
  LNode* temp = gPrev->getFirst();
  LNode* temp2;
  bool isDGb = 0;
  // construct the d-GB gb
  ideal gb =   idInit(gPrev->getLength(),1);
  for(int j=0;j<=gPrev->getLength()-1;j++) {
          gb->m[j] =   temp->getPoly();
          pWrite(gb->m[j]);
          temp        =   temp->getNext();
  }
  /*
  Kstd1_deg = pDeg(gPrev->getLast()->getPoly());
  BITSET save = test;
  test |= Sy_bit(OPT_DEGBOUND);
  kStd();
  test = save;
  */
  temp = gPrev->getFirst();
  while(NULL != temp) {
    temp2 = temp->getNext();
    number nOne = nInit(1);
    poly lcm = pOne();
    poly sp = pOne();
    while(NULL != temp2) {
      pLcm(temp->getPoly(),temp2->getPoly(),lcm);
      pSetCoeff(lcm,nOne);
      pSetm(lcm);
      PrintS("LCM:   ");
      pWrite(lcm);
      if(pDeg(lcm) <= pDeg(gPrev->getLast()->getPoly())) {
        poly u1 = pOne();
        poly u2 = pOne();
        u1  = pMDivide(lcm,temp->getPoly());
        pSetCoeff(u1,nOne);
        u2  = pMDivide(lcm,temp2->getPoly());
        pSetCoeff(u2,nOne);
        sp      =   ksOldSpolyRedNew(ppMult_qq(u1,temp->getPoly()),
            ppMult_qq(u2,temp2->getPoly()));
        pNorm(sp);

        poly reducer  = pOne();
        //reducer       = gb->m[0];
        int i         = 0;
        pWrite(pHead(sp));

        while(i<IDELEMS(gb)) {
          reducer = gb->m[i];
          if(pDivisibleBy(reducer,pHead(sp))) {
            poly uReducer = pOne();
            uReducer = pMDivide(lcm,reducer);
            pSetCoeff(uReducer,nOne);
            sp = ksOldSpolyRedNew(sp,ppMult_qq(uReducer,reducer));
            //pNorm(tempSP);
            //sp = tempSP;
            pNorm(sp);
            pWrite(sp);
            i = 0;
          }
          else {
            i++;
          }
        }

        pWrite(pHead(sp));
      }
      temp2 = temp2->getNext();
    }
    temp  = temp->getNext();
  }
  PrintS("------------------\n");
  return isDGb;
}

/*
 * Arris Check if we are finished after the current degree step:
 * Checks all remaining critical pairs, i.e. those of higher degree,
 * by the two Buchberger criteria.
 * return value: 0, if all remaining critical pairs are deleted by
 *                  Buchberger's criteria
 *               1, otherwise
 */
bool arrisCheck(CNode* first, LNode* firstGCurr, long arrideg) {
  CNode* temp = first;

  //product criterion check
  while(NULL != temp) {
    if(!pLmEqual(pHead(temp->getLp2Poly()),temp->getT1())) {
        return 0;
    }
    temp = temp->getNext();
  }

  // chain criterion
  LNode* tempPoly = firstGCurr;
  while(NULL != tempPoly) {
    LNode* tempPoly2 = tempPoly->getNext();
    while(NULL != tempPoly2) {
      number nOne = nInit(1);
      poly lcm = pOne();
      pLcm(tempPoly->getPoly(),tempPoly2->getPoly(),lcm);
      pSetCoeff(lcm,nOne);
      pSetm(lcm);
      if(pDeg(lcm) > arrideg) {
        LNode* tempPoly3 = firstGCurr;
        while(NULL != tempPoly3) {
          if(tempPoly3 != tempPoly2 && tempPoly3 != tempPoly && pDivisibleBy(tempPoly3->getPoly(),lcm)) {
        poly lcm1 = pOne();
        poly lcm2 = pOne();
        pLcm(tempPoly3->getPoly(),tempPoly->getPoly(),lcm1);
        pSetCoeff(lcm1,nOne);
        pSetm(lcm1);
        pLcm(tempPoly3->getPoly(),tempPoly2->getPoly(),lcm2);
        pSetCoeff(lcm2,nOne);
        pSetm(lcm2);
        if(pDeg(lcm1) < pDeg(lcm) && pDeg(lcm2) < pDeg(lcm)) {
          return 0;
        }
      }
        tempPoly3 = tempPoly3->getNext();
        }
      }
      tempPoly2 = tempPoly2->getNext();
    }
    tempPoly = tempPoly->getNext();
  }
  return 1;
}



/*
================================================================
computes a list of critical pairs for the next reduction process
the first element is always "useful" thus the critical pair
computed is either "useful" or "useless" depending on the second
element which generates the critical pair.
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
inline void criticalPair(LList* gPrev, CListOld* critPairs, LTagList* lTag, RTagList* rTag, RList* rules, PList* rejectedGBList, int plus) {
    //Print("IN CRITPAIRS\n");
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* newElement   =   gPrev->getLast();
    LNode* temp         =   gPrev->getFirst();
    poly u1             =   pOne();
    poly u2             =   pOne();
    poly lcm            =   pOne();
    poly t              =   pHead(newElement->getPoly());
    RuleOld* testedRuleOld    =   NULL;
    //Print("NEW:   ");
    //pWrite(newElement->getPoly());
    //Print("ADDRESS:  %p",newElement);
    if(NULL != rules->getFirst()) {
        testedRuleOld  =   rules->getFirst()->getRuleOld();
    }
    // computation of critical pairs
    while( gPrev->getLast() != temp) {
        //Print("TEMP:  ");
        //pWrite(pHead(temp->getPoly()));
        //Print("ADDRESS:  %p",temp);
        pLcm(newElement->getPoly(), temp->getPoly(), lcm);
        pSetCoeff(lcm,nOne);
        // computing factors u2 for new labels
        u1 = pMDivide(lcm,t);
        if(NULL == u1) {
            break;
        }
        pSetCoeff(u1,nOne);
        u2 = pMDivide(lcm,temp->getPoly());
        pSetCoeff(u2,nOne);
        int degree = pDeg(ppMult_qq(u2,pHead(temp->getPoly())));
        // testing both new labels by the F5 Criterion
        if(!temp->getDel()) {
          if(plus && highestDegreeGBCriticalPair < degree) {
            highestDegreeGBCriticalPair = degree;
          }
          if(!criterion2(gPrev->getFirst()->getIndex(), u2, temp, rules, rTag)
            && !criterion2(gPrev->getFirst()->getIndex(), u1, newElement, rules, rTag)
            && !criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag)) {
              // if they pass the test, add them to CListOld critPairs, having the LPolyOld with greater
              // label as first element in the CPairOld
              if(newElement->getIndex() == temp->getIndex() &&
              -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                  CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2,
                                  temp->getLPolyOld(), u1, newElement->getLPolyOld(), 0 , testedRuleOld);
                  critPairs->insert(cp);
                  // counting the number of useful pairs
                  numberUsefulPairs++;
              }
              else {
                  CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1,
                                  newElement->getLPolyOld(), u2, temp->getLPolyOld(), 0, testedRuleOld);
                  critPairs->insert(cp);
                  // counting the number of useful pairs
                  numberUsefulPairs++;
              }
          }
          else {
            // TODO: generate a list of lcms of rejected GB critical pairs and
            //       check F5 critical pairs with it at their creation
            //Print("--------------------------------------------------------------------\n");
            //Print("--------------------------------------------------------------------\n");
            pSetm(lcm);
            rejectedGBList->insert(lcm);
            //Print("-----------------------REJECTED IN CRIT PAIR------------------------\n");
            //pWrite(lcm);
            //Print("--------------------------------------------------------------------\n");
            //rejectedGBList->print();
          }
        }
        else {
        //Print("LABEL:  ");
        //pWrite(ppMult_qq(u1,newElement->getTerm()));
          //if(rejectedGBList->check(lcm)) { // if there is equality of lcms then we need the F5 critical pair
            if(!criterion2(gPrev->getFirst()->getIndex(), u2, temp, rules, rTag)
              && !criterion2(gPrev->getFirst()->getIndex(), u1, newElement, rules, rTag)
              && !criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag)) {
                // if they pass the test, add them to CListOld critPairs, having the LPolyOld with greater
                // label as first element in the CPairOld
                if(newElement->getIndex() == temp->getIndex() &&
                -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                    CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2,
                                    temp->getLPolyOld(), u1, newElement->getLPolyOld(), 1 , testedRuleOld);
                    critPairs->insert(cp);
                    numberUselessPairs++;
                }
                else {
                    CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1,
                                    newElement->getLPolyOld(), u2, temp->getLPolyOld(), 1, testedRuleOld);
                    critPairs->insert(cp);
                    numberUselessPairs++;
                }
            }
          //}
        }
        temp    =   temp->getNext();
    }
}



/*
================================================================
computes a list of critical pairs for the next reduction process
the first element is always "useless" thus the critical pair
computed is "useless".
first element in gPrev is always the newest element which must
build critical pairs with all other elements in gPrev
================================================================
*/
inline void criticalPair2(LList* gPrev, CListOld* critPairs, LTagList* lTag, RTagList* rTag, RList* rules, PList* rejectedGBList) {
    //Print("IN CRITPAIRS\n");
    // initialization for usage in pLcm()
    number nOne         =   nInit(1);
    LNode* newElement   =   gPrev->getLast();
    LNode* temp         =   gPrev->getFirst();
    poly u1             =   pOne();
    poly u2             =   pOne();
    poly lcm            =   pOne();
    poly t              =   pHead(newElement->getPoly());
    RuleOld* testedRuleOld    =   NULL;
    if(NULL != rules->getFirst()) {
        testedRuleOld  =   rules->getFirst()->getRuleOld();
    }
    // computation of critical pairs
    while( gPrev->getLast() != temp) {
        pLcm(newElement->getPoly(), temp->getPoly(), lcm);
        pSetCoeff(lcm,nOne);
        // computing factors u2 for new labels
        u1 = pMDivide(lcm,t);
        if(NULL == u1) {
            break;
        }
        pSetCoeff(u1,nOne);
        u2 = pMDivide(lcm,temp->getPoly());
        pSetCoeff(u2,nOne);
       // testing both new labels by the F5 Criterion
        //Print("LABEL:  ");
        //pWrite(ppMult_qq(u1,newElement->getTerm()));
        //if(rejectedGBList->check(lcm)) { // if there is equality of lcms then we need the F5 critical pair
          if(!criterion2(gPrev->getFirst()->getIndex(), u2, temp, rules, rTag)
            && !criterion2(gPrev->getFirst()->getIndex(), u1, newElement, rules, rTag)
            && !criterion1(gPrev,u1,newElement,lTag) && !criterion1(gPrev,u2,temp,lTag)) {
              // if they pass the test, add them to CListOld critPairs, having the LPolyOld with greater
              // label as first element in the CPairOld
              if(newElement->getIndex() == temp->getIndex() &&
              -1 == pLmCmp(ppMult_qq(u1, newElement->getTerm()),ppMult_qq(u2, temp->getTerm()))) {
                  CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u2,
                                  temp->getLPolyOld(), u1, newElement->getLPolyOld(), 1 , testedRuleOld);
                  critPairs->insert(cp);
                  numberUselessPairs++;
              }
              else {
                  CPairOld* cp   =   new CPairOld(pDeg(ppMult_qq(u2,pHead(temp->getPoly()))), u1,
                                  newElement->getLPolyOld(), u2, temp->getLPolyOld(), 1, testedRuleOld);
                  critPairs->insert(cp);
                  numberUselessPairs++;
              }
          }
        //}
        temp    =   temp->getNext();
    }
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
                if(idx != gPrev->getLast()->getIndex()) {
                    //Print("DOCH!\n");
                }
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
        poly temp   =   kNF(testId,currRing->qideal,u1);
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
    PrintS("RULES: \n");
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
    testNode    =   rules->getFirst();
    /*
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
            *-------------------------------------
             * TODO: WHEN INTERREDUCING THE GB THE
             *       INDEX OF THE PREVIOUS ELEMENTS
             *       GETS HIGHER!
             *-----------------------------------*
            //testNode    =   rules->getFirst();
            testNode    =   rTag->get(l->getIndex());
            if(NULL == testNode) {
                testNode    =   rules->getFirst();
            }
            //Print("TESTNODE ADDRESS: %p\n",testNode);
        }
    }
    */
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
        if(testNode->getRuleOld() == l->getRuleOld()) {
            //Print("%p\n%p\n",testNode->getRule(),l->getRule());
            //Print("EQUAL\n");
        }
        else {
            //Print("NOT EQUAL\n");
        }
    }
    while(NULL != testNode && testNode->getRuleOld() != l->getRuleOld()
          && l->getIndex() == testNode->getRuleOldIndex()) {
        //Print("%p\n",testNode);
        //pWrite(testNode->getRuleTerm());
        //pWrite(t);
        //pWrite(l->getTerm());
        //pWrite(u1);
        //Print("%d\n",testNode->getRuleIndex());
        if(pLmDivisibleByNoComp(testNode->getRuleOldTerm(),u1)) {
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
inline bool criterion2(poly t, LPolyOld* l, RList* rules, RuleOld* testedRuleOld) {
    //Print("------------------------------IN CRITERION 2/2-----------------------------------------\n");
    //Print("LAST RuleOld TESTED: %p",testedRuleOld);
    /*
    PrintS("RULES: \n");
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
        while(NULL != testNode && testNode->getRuleOld() != testedRuleOld) {
        //pWrite(testNode->getRuleTerm());
        if(pLmDivisibleByNoComp(testNode->getRuleOldTerm(),u1)) {
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
 * check for useful pairs in the given subset of critical pairs
 */
int computeUsefulMinDeg(CNode* first) {
  CNode* temp = first;
  int numberUsefulPairsMinDeg = 0;
  while(NULL != temp) {
    if(!temp->getDel()) {
      numberUsefulPairsMinDeg++;
    }
    temp = temp->getNext();
  }
  return numberUsefulPairsMinDeg;
}
/*
==================================
Computation of S-Polynomials in F5
==================================
*/
void computeSPols(CNode* first, RTagList* rTag, RList* rules, LList* sPolyList, PList* rejectedGBList) {
    CNode* temp         =   first;
    //Print("\nDegree:  %d\n",temp->getData()->getDeg());
    // only GB-critical pairs are computed
    //while(NULL != temp && temp->getDel()) {
    //  temp = temp->getNext();
    //}
    //if(temp->getData()->getDeg() == 11) {
    //  Print("PAIRS OF DEG 11:\n");
    //}
    RList* f5rules = new RList();
    CListOld* f5pairs = new CListOld();
    poly sp     =   pInit();
    number sign =   nInit(-1);
    //Print("###############################IN SPOLS##############################\n");
    //first->print();
/*
 * first of all: do everything for GB critical pairs
 */
    while(NULL != temp) {
    //  if(temp->getData()->getDeg() == 11) {
        //Print("--------------------------\n");
        //Print("redundant? %d\n",temp->getDel());
        //pWrite(pHead(temp->getLp1Poly()));
        //Print("redundant: %d\n",temp->getAdLp1()->getDel());
        //pWrite(pHead(temp->getLp2Poly()));
        //Print("redundant: %d\n",temp->getAdLp2()->getDel());
        //pWrite(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
        //  sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
        //      ppMult_qq(temp->getT2(),temp->getLp2Poly()));
          //Print("BEGIN SPOLY2\n====================\n");
        //  pNorm(sp);
        //  pWrite(pHead(sp));
        //Print("--------------------------\n");
      //}
      if(!criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRuleOld())) {
      //if(temp->getDel() == 0 && !criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRuleOld())) {
        if(temp->getLp2Index() == temp->getLp1Index()) {
          if(!criterion2(temp->getT2(),temp->getAdLp2(),rules,temp->getTestedRuleOld())) {
            sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                ppMult_qq(temp->getT2(),temp->getLp2Poly()));
            pNorm(sp);
            if(NULL == sp) {
              reductionsToZero++;
              rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
              numberOfRules++;
              pDelete(&sp);
            }
            else {
              rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
              numberOfRules++;
              sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRuleOld());
            //Print("INSERTED\n");
              numberOfSpolys++;
            }
          }
          else {
            if(highestDegreeGBCriticalPair < pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()))) {
              highestDegreeGBCriticalPair = pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()));
            }
            rejectedGBList->insert(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
            //Print("rejected!\n");

            //Print("CRITERION 2 in SPOLS 2nd generator\n");
          }
        }
        else {
          sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
              ppMult_qq(temp->getT2(),temp->getLp2Poly()));
          //Print("BEGIN SPOLY2\n====================\n");
          pNorm(sp);
          //pWrite(sp);
          //Print("END SPOLY2\n====================\n");
          if(NULL == sp) {
            reductionsToZero++;
            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
            numberOfRules++;
            pDelete(&sp);
          }
          else {
            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
            numberOfRules++;
            sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRuleOld());
            //Print("INSERTED\n");
            numberOfSpolys++;
          }
        }
      }
      /*
      if(temp->getDel() == 0 && criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRuleOld())) {
        //Print("rejected!\n");
        rejectedGBList->insert(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
      }


      if(temp->getDel() == 1 && !criterion2(temp->getT1(),temp->getAdLp1(),rules,temp->getTestedRuleOld())) {
        if(highestDegree < pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()))) {
          highestDegree   = pDeg(ppMult_qq(temp->getT1(),temp->getLp1Poly()));
        }
        if(temp->getLp2Index() == temp->getLp1Index()) {
          if(!criterion2(temp->getT2(),temp->getAdLp2(),rules,temp->getTestedRuleOld())) {
            sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                ppMult_qq(temp->getT2(),temp->getLp2Poly()));
            pNorm(sp);
            if(NULL == sp) {
              reductionsToZero++;
              rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
              numberOfRules++;
              pDelete(&sp);
            }
            else {
              rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
              numberOfRules++;


              // save the address of the rule again in a different
              // list

              f5rules->insert(rules->getFirst()->getRuleOld());
              f5pairs->insertWithoutSort(temp->getData());
              ///////////////////////////////////////

              //sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRuleOld());
            }
          }
        }
        else {
          sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
              ppMult_qq(temp->getT2(),temp->getLp2Poly()));
          //Print("BEGIN SPOLY2\n====================\n");
          pNorm(sp);
          //pWrite(sp);
          //Print("END SPOLY2\n====================\n");
          if(NULL == sp) {
            reductionsToZero++;
            //rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
            numberOfRules++;
            pDelete(&sp);
          }
          else {
            rules->insert(temp->getLp1Index(),ppMult_qq(temp->getT1(),temp->getLp1Term()));
            numberOfRules++;
            // save the address of the rule again in a different
            // list

            f5rules->insert(rules->getFirst()->getRuleOld());
            f5pairs->insertWithoutSort(temp->getData());
            ///////////////////////////////////////
            //sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,rules->getFirst()->getRuleOld());
            //  numberOfSpolys++;
          }
        }
      }
        // the following else is not needed at all as we are checking
        // F5-critical pairs in this step
        /*
           else {
           if(!temp->getDel()) {
           rejectedGBList->insert(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
           }

           }
           */

        temp    =   temp->getNext();

      }

      /*
      temp = f5pairs->getFirst();
      RNode* tempRule = f5rules->getFirst();
      int howmany = 1;
      while(NULL != temp) {
        //Print("RULE:  ");
        //pWrite(ppMult_qq(temp->getT1(),temp->getLp1Term()));
        sp      =   ksOldSpolyRedNew(ppMult_qq(temp->getT1(),temp->getLp1Poly()),
                                     ppMult_qq(temp->getT2(),temp->getLp2Poly()));
        pNorm(sp);
        if(rejectedGBList->check(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())))) { //|| (temp->getData()->getDeg() == 10 && howmany == 2)) {
          if((temp->getData()->getDeg() == 10) && (howmany == 2)) {
            //Print("HIER DRIN IM SAFTLADEN\n");
          }
          sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,tempRule->getRuleOld());
              numberOfSpolys++;
              howmany++;
        }
        else {
          numberRejectedF5CriticalPairs++;
              howmany++;
          if(numberRejectedF5CriticalPairs < -1) { // ||
          }
          else {
             //numberRejectedF5CriticalPairs == 7) {
            /*
            PrintS("--------------------------------- rejected F5-critical pair-------------------------------------\n");
            pWrite(pHead(ppMult_qq(temp->getT1(),temp->getLp1Poly())));
            PrintS("Label: ");
            pWrite(ppMult_qq(temp->getT1(),temp->getLp1Term()));
            Print("%d\n",temp->getDel());
            PrintS("Comes from:\n ");
            pWrite(pHead(temp->getLp1Poly()));
            PrintS("Label: ");
            pWrite(temp->getLp1Term());
            Print("%d\n",temp->getLp1Index());
            pWrite(pHead(temp->getLp2Poly()));
            PrintS("Label: ");
            pWrite(temp->getLp2Term());
            Print("%d\n",temp->getLp2Index());
            //Print("--------------------------------------LIST OF GB PAIRS REJECTED-----------------------------------------\n");
            //rejectedGBList->print();
            PrintS("--------------------------------------------------------------------------------------------------------\n");
            //if(temp->getLp1Index() < 7) {
              sPolyList->insertByLabel(ppMult_qq(temp->getT1(),temp->getLp1Term()),temp->getLp1Index(),sp,tempRule->getRuleOld());

            //}
          //numberOfSpolys++;
          }
        //pSetExp(tempRule->getRuleOld()->getTerm(),1,1000);
        }
        temp      = temp->getNext();
        tempRule  = tempRule->getNext();

      }
      */
      // these critical pairs can be deleted now as they are either useless for further computations or
      // already saved as an S-polynomial to be reduced in the following
      delete first;
//Print("NUMBER SPOLYS: %d\n", numberOfSpolys);
//Print("SPOLY LIST: \n");
//LNode* tempSpoly = sPolyList->getFirst();
//if(NULL != tempSpoly) {
//  pWrite(pHead(tempSpoly->getPoly()));
//  tempSpoly = tempSpoly->getNext();
//}
//Print("-----SP------\n");
//else {
//  Print("EMPTY SPOLYLIST!\n");
//}
}

/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
void reduction(LList* sPolyList, CListOld* critPairs, LList* gPrev, RList* rules, LTagList* lTag, RTagList* rTag,
                 ideal gbPrev, PList* rejectedGBList, int plus) {
    //Print("##########################################In REDUCTION!########################################\n");
    // check if sPolyList has any elements
    // NOTE: due to initialization sPolyList always has a default NULL element
    LNode* temp = sPolyList->getFirst();
    while(NULL != temp) {
        // temp is the first element in the sPolyList which should be reduced
        // due to earlier sorting this is the element of minimal degree AND
        // minimal label
        // delete the above first element from sPolyList, temp will be either reduced to
        // zero or added to gPrev, but never come back to sPolyList
        //pWrite(sPolyList->getFirst()->getPoly());
        //Print("LIST OF SPOLYNOMIALS!\n");
        //sPolyList->print();
        sPolyList->setFirst(temp->getNext());
        poly tempNF = kNF(gbPrev,currRing->qideal,temp->getPoly());
        if(NULL != tempNF) {
            pNorm(tempNF);
            temp->setPoly(tempNF);
            // try further reductions of temp with polynomials in gPrev
            // with label index = current label index: this is done such that there
            // is no label corruption during the reduction process
            //Print("lower label reduction:  ");
            //pWrite(tempNF);
            topReduction(temp,sPolyList,gPrev,critPairs,rules,lTag,rTag,gbPrev, rejectedGBList,plus);

        }
        else {
            reductionsToZero++;
            delete temp;
        }
        //if(NULL != temp->getPoly()) {
        //    criticalPair(gPrev,critPairs,lTag,rTag,rules);
        //}
        temp =   sPolyList->getFirst();
    }
    //sPolyList->print();
    //delete sPolyList;
}

/*
========================================================================
reduction including subalgorithm topReduction() using Faugere's criteria
========================================================================
*/
void newReduction(LList* sPolyList, CListOld* critPairs, LList* gPrev, LList* reducers, RList* rules, LTagList* lTag, RTagList* rTag,
                 ideal gbPrev, int termination, PList* rejectedGBList, int plus) {
    //Print("##########################################In REDUCTION!########################################\n");
    // check if sPolyList has any elements
    // NOTE: due to initialization sPolyList always has a default NULL element
    //Print("--1--\n");
    LNode* temp = sPolyList->getFirst();
    //Print("START OF REDUCTION:  ");
    while(NULL != temp) {
        numberOfReductions++;
        // temp is the first element in the sPolyList which should be reduced
        // due to earlier sorting this is the element of minimal degree AND
        // minimal label
        // delete the above first element from sPolyList, temp will be either reduced to
        // zero or added to gPrev, but never come back to sPolyList
        //Print("LIST OF SPOLYNOMIALS!\n");
        //sPolyList->print();
        //pWrite(sPolyList->getFirst()->getPoly());
        sPolyList->setFirst(temp->getNext());
        //if(pDeg(temp->getPoly()) > 11) {
        //  Print("YES!!!!!!!!!!!!!!!!\n");
        //}
        //pWrite(temp->getPoly());
        //poly tempNF = kNF(gbPrev,currRing->qideal,temp->getPoly());
        //Print("!!!\n");
        //if(NULL != tempNF) {
            //pNorm(tempNF);
            //temp->setPoly(tempNF);
            //Print("lower label reduction:  ");
            //pWrite(tempNF);
            // try further reductions of temp with polynomials in gPrev
            // with label index = current label index: this is done such that there
            // is no label corruption during the reduction process
            findReducers(temp,sPolyList,gbPrev,gPrev,reducers,critPairs,rules,lTag,rTag, termination, rejectedGBList,plus);
        //}
        //else {
        //    reductionsToZero++;
        //    delete temp;
        //}
        //if(NULL != temp->getPoly()) {
        //    criticalPair(gPrev,critPairs,lTag,rTag,rules);
        //}
        //Print("HIER AUCH ?\n");
        //Print("--2--\n");
        //sPolyList->print();
        //critPairs->print();
        temp =   sPolyList->getFirst();
        //Print("%p\n",temp);
    }
    //sPolyList->print();
    //delete sPolyList;
    //Print("REDUCTION FERTIG\n");
}


/*!
 * ================================================================================
 * searches for reducers of temp similar to the symbolic preprocessing of F4  and
 * divides them into a "good" and "bad" part:
 *
 * the "good" ones are the reducers which do not corrupt the label of temp, with
 * these the normal form of temp is computed
 *
 * the "bad" ones are the reducers which corrupt the label of temp, they are tested
 * later on for possible new rules and S-polynomials to be added to the algorithm
 * ================================================================================
*/
void findReducers(LNode* l, LList* sPolyList, ideal gbPrev, LList* gPrev, LList* reducers, CListOld* critPairs, RList* rules, LTagList* lTag, RTagList* rTag, int termination, PList* rejectedGBList, int plus) {
    int canonicalize    =   0;
    //int timerRed        =   0;
    bool addToG         =   1;
    number sign         =   nInit(-1);
    LList* good         =   new LList();
    LList* bad          =   new LList();
    LList* monomials    =   new LList(l->getLPolyOld());
    poly u              =   pOne();
    number nOne         =   nInit(1);
    LNode* tempRed      =   lTag->getFirstCurrentIdx();
    LNode* tempMon      =   monomials->getFirst();
    poly tempPoly       =   pInit();
    poly redPoly        =   NULL;
    int idx             =   l->getIndex();
    //Print("IN FIND REDUCERS:  ");
    //pWrite(l->getPoly());
    tempPoly    =   pCopy(l->getPoly());
    //tempMon->setPoly(tempPoly);
    //while(NULL != tempMon) {
        // iteration over all monomials in tempMon
        kBucket* bucket  =   kBucketCreate(currRing);
        kBucketInit(bucket,tempPoly,0);
        tempPoly    =   kBucketGetLm(bucket);
        //Print("\n\n\nTO BE REDUCED:  ");
        //pWrite(l->getPoly());
        //pWrite(l->getTerm());
        //pWrite(tempPoly);
        while(NULL != tempPoly) {
            // iteration of all elements in gPrev of the current index
            tempRed =   gPrev->getFirst();
            while(NULL != tempRed) {
                //Print("TEMPREDPOLY:  ");
                //pWrite(tempRed->getPoly());
                if(pLmDivisibleByNoComp(tempRed->getPoly(),tempPoly)) {
                    u   =   pMDivideM(tempPoly,tempRed->getPoly());
                    //Print("U:  ");
                    //pWrite(u);
                    if(pLmCmp(u,pOne()) != 0) { // else u = 1 and no test need to be done!
                    if(tempRed->getIndex() != idx) {
                            // passing criterion1 ?
                            if(!criterion1(gPrev,u,tempRed,lTag)) {
                                    poly tempRedPoly    =   tempRed->getPoly();
                                    //Print("REDUCER: ");
                                    //pWrite(tempRedPoly);
                                    pIter(tempRedPoly);
                                    int lTempRedPoly    =   pLength(tempRedPoly);
                                    kBucketExtractLm(bucket);
                                    kBucket_Minus_m_Mult_p(bucket,u,tempRedPoly,&lTempRedPoly);
                                    canonicalize++;
                                    //Print("Reduction\n");
                                    if(!(canonicalize % 50)) {
                                        kBucketCanonicalize(bucket);
                                    }
                                    tempPoly    =   kBucketGetLm(bucket);
                                    //Print("TEMPPOLY:  ");
                                    //pWrite(tempPoly);
                                    if(NULL != tempPoly) {
                                        tempRed     =   gPrev->getFirst();
                                        continue;
                                    }
                                    else {
                                        break;
                                    }
                             }

                    }
                    else {
                        if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 0) {
                         //Print("NOT ALLOWED REDUCER, EQUAL LABELS:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(pHead(tempRed->getPoly()));
                                      addToG  = 0;
                        }
                        if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) != 0) {
                            // passing criterion2 ?
                            if(!criterion2(gPrev->getFirst()->getIndex(), u,tempRed,rules,rTag)) {
                                // passing criterion1 ?
                                if(!criterion1(gPrev,u,tempRed,lTag)) {
                                    if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1) {
                                        if(NULL == redPoly) {
                                            bad->insert(tempRed->getLPolyOld());
                                            addToG  = 1;
                                            //poly tempRedPoly    =   tempRed->getPoly();
                                            //break;
                                        }
                                    }
                                    else {
                                        poly tempRedPoly    =   tempRed->getPoly();
                                        //Print("REDUCER: ");
                                        //pWrite(tempRedPoly);
                                        pIter(tempRedPoly);
                                        int lTempRedPoly    =   pLength(tempRedPoly);
                                        //Print("HEAD MONOMIAL KBUCKET: ");
                                        //pWrite(kBucketGetLm(bucket));
                                        kBucketExtractLm(bucket);
                                        kBucket_Minus_m_Mult_p(bucket,u,tempRedPoly,&lTempRedPoly);
                                        canonicalize++;
                                        //Print("REDUCTION\n");
                                        addToG  = 1;
                                        if(!(canonicalize % 50)) {
                                            kBucketCanonicalize(bucket);
                                        }
                                        //Print("HEAD MONOMIAL KBUCKET: ");
                                        //pWrite(kBucketGetLm(bucket));
                                        tempPoly    =   kBucketGetLm(bucket);
                                        //Print("TEMPPOLY:  ");
                                        //pWrite(tempPoly);
                                        if(NULL != tempPoly) {
                                            tempRed     =   gPrev->getFirst();
                                            continue;
                                        }
                                        else {
                                            break;
                                        }
                                    }
                                }
                            }
                            else {
                              //Print("CRIT 1  ");

                                      if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1 ) {
                                      //Print("NOT ALLOWED REDUCER, CRIT 1:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(tempRed->getPoly());
                                      if(pLmDivisibleByNoComp(tempRed->getTerm(),l->getTerm())) {
                                        //Print("REDUCER LABEL:  ");
                                        //pWrite(tempRed->getTerm());
addToG  = 0;
                                      }
                                    }
                            }
                        }
                        else {
                          //Print("CRIT 2  ");
                                    if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1) {
                                      //Print("NOT ALLOWED REDUCER, CRIT 2:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(tempRed->getPoly());
                                      if(pLmDivisibleByNoComp(tempRed->getTerm(),l->getTerm())) {
                                        //Print("REDUCER LABEL:  ");
                                        //pWrite(tempRed->getTerm());
addToG  = 0;
                                      }
                                    }
                        }
                    }
                    }
                    else { // u = 1 => reduce without checking criteria
                        poly tempRedPoly    =   tempRed->getPoly();
                        //Print("REDUCER: ");
                        //pWrite(tempRedPoly);
                        pIter(tempRedPoly);
                        int lTempRedPoly    =   pLength(tempRedPoly);
                        kBucketExtractLm(bucket);
                        kBucket_Minus_m_Mult_p(bucket,u,tempRedPoly,&lTempRedPoly);
                        canonicalize++;
                        //Print("Reduction\n");
                        if(!(canonicalize % 50)) {
                            kBucketCanonicalize(bucket);
                        }
                        tempPoly    =   kBucketGetLm(bucket);
                        //Print("TEMPPOLY:  ");
                        //pWrite(tempPoly);
                        if(NULL != tempPoly) {
                            tempRed     =   gPrev->getFirst();
                            continue;
                        }
                        else {
                            break;
                        }

                    }
                }
                tempRed =   tempRed->getNext();
            }
            // reduction process with elements in LList* reducers
          if(NULL != tempPoly) {
            //Print("HERE\n");
            tempRed =   reducers->getFirst();
            while(NULL != tempRed) {
                //Print("TEMPREDPOLY:  ");
                //pWrite(tempRed->getPoly());
              //pWrite(tempPoly);
              if(pLmDivisibleByNoComp(tempRed->getPoly(),tempPoly)) {
                    //Print("A\n");
                    u   =   pMDivideM(tempPoly,tempRed->getPoly());
                    //Print("U:  ");
                    //pWrite(u);
                    if(tempRed->getIndex() != idx) {
                            // passing criterion1 ?
                            if(!criterion1(gPrev,u,tempRed,lTag)) {
                                    poly tempRedPoly    =   tempRed->getPoly();
                                    //Print("REDUCER: ");
                                    //pWrite(ppMult_qq(u,tempRedPoly));
                                    pIter(tempRedPoly);
                                    int lTempRedPoly    =   pLength(tempRedPoly);
                                    kBucketExtractLm(bucket);
                                    kBucket_Minus_m_Mult_p(bucket,u,tempRedPoly,&lTempRedPoly);
                                    canonicalize++;
                                    //Print("Reduction\n");
                                    if(!(canonicalize % 50)) {
                                        kBucketCanonicalize(bucket);
                                    }
                                    tempPoly    =   kBucketGetLm(bucket);
                                    //Print("TEMPPOLY:  ");
                                    //pWrite(tempPoly);
                                    if(NULL != tempPoly) {
                                        tempRed     =   gPrev->getFirst();
                                        continue;
                                    }
                                    else {
                                        break;
                                    }
                             }

                    }
                    else {
                        if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 0) {
                         //Print("NOT ALLOWED REDUCER, EQUAL LABELS:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(pHead(tempRed->getPoly()));
                                      //addToG  = 0;
                        }
                        if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) != 0) {
                            // passing criterion2 ?
                            if(!criterion2(gPrev->getFirst()->getIndex(), u,tempRed,rules,rTag)) {
                                // passing criterion1 ?
                                if(!criterion1(gPrev,u,tempRed,lTag)) {
                                    if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1) {
                                        if(NULL == redPoly) {
                                            bad->insert(tempRed->getLPolyOld());
                                            addToG  = 1;
                                            //poly tempRedPoly    =   tempRed->getPoly();
                                            //break;
                                        }
                                    }
                                    else {
                                        poly tempRedPoly    =   tempRed->getPoly();
                                        //Print("REDUCER: ");
                                        //pWrite(ppMult_qq(u,tempRedPoly));
                                        pIter(tempRedPoly);
                                        int lTempRedPoly    =   pLength(tempRedPoly);
                                        //Print("HEAD MONOMIAL KBUCKET: ");
                                        //pWrite(kBucketGetLm(bucket));
                                        kBucketExtractLm(bucket);
                                        kBucket_Minus_m_Mult_p(bucket,u,tempRedPoly,&lTempRedPoly);
                                        canonicalize++;
                                        //Print("REDUCTION\n");
                                        addToG  = 1;
                                        if(!(canonicalize % 50)) {
                                            kBucketCanonicalize(bucket);
                                        }
                                        //Print("HEAD MONOMIAL KBUCKET: ");
                                        //pWrite(kBucketGetLm(bucket));
                                        tempPoly    =   kBucketGetLm(bucket);
                                        //Print("TEMPPOLY:  ");
                                        //pWrite(tempPoly);
                                        if(NULL != tempPoly) {
                                            tempRed     =   gPrev->getFirst();
                                            continue;
                                        }
                                        else {
                                            break;
                                        }
                                    }
                                }
                            }
                            else {
                              //Print("CRIT 1  ");

                                      if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1 ) {
                                      //Print("NOT ALLOWED REDUCER, CRIT 1:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(tempRed->getPoly());
                                      if(pLmDivisibleByNoComp(tempRed->getTerm(),l->getTerm())) {
                                        //Print("REDUCER LABEL:  ");
                                        //pWrite(tempRed->getTerm());
                                        addToG  = 0;
                                      }
                                    }
                            }
                        }
                        else {
                          //Print("CRIT 2  ");
                                    if(pLmCmp(ppMult_qq(u,tempRed->getTerm()),l->getTerm()) == 1) {
                                      //Print("NOT ALLOWED REDUCER, CRIT 2:\n");
                                      //pWrite(u);
                                      //pWrite(tempRed->getTerm());
                                      //pWrite(tempRed->getPoly());
                                      if(pLmDivisibleByNoComp(tempRed->getTerm(),l->getTerm())) {
                                        //Print("REDUCER LABEL:  ");
                                        //pWrite(tempRed->getTerm());
addToG  = 0;
                                      }
                                    }
                        }
                    }

                }
                tempRed =   tempRed->getNext();
            }
          }
            if(NULL != tempPoly) {
                if(NULL == redPoly) {
                    redPoly =   kBucketExtractLm(bucket);
                }
                else {
                    redPoly     =   p_Merge_q(redPoly,kBucketExtractLm(bucket),currRing);
                }
               // for top-reduction only
               redPoly  = p_Merge_q(redPoly,kBucketClear(bucket),currRing);
               break;
               // end for top-reduction only
               tempPoly    =   kBucketGetLm(bucket);

            }
        }
        if(NULL == redPoly) {
            reductionsToZero++;
            //pDelete(&redPoly);
        }
        else
        {
            PrintS("\nELEMENT ADDED TO GPREV: ");
            pNorm(redPoly);
            if(highestDegree < pDeg(redPoly))
            {
              highestDegree   = pDeg(redPoly);
            }
            pWrite(pHead(redPoly));
            //pWrite(l->getTerm());
            //Print("%d\n",canonicalize);
            l->setPoly(redPoly);
            // give "l" the label if it is "useful" (addToG = 0) or "useless"
            // (addToG = 1)
            l->setDel(!addToG);
            Print("redundant? %d\n\n",l->getDel());
            if(addToG == 0 && termination == 1) {
              reducers->insert(l->getLPolyOld());
            }
            else {
              gPrev->insert(l->getLPolyOld());
            }
            //Print("%d\n\n",termination);
            if(termination == 1) {
            if(addToG) {
              //Print("----------------HERE?-----------------\n");
              criticalPair(gPrev,critPairs,lTag,rTag,rules, rejectedGBList,plus);
            }
            else {
              notInG++;
              //Print("\nNONONO");
              //pWrite(pHead(l->getPoly()));
              //pWrite(l->getTerm());
            }
            }
            // case in which all new elements are added to gPrev!
            // using boolean "addToG" to know if the element is "useful" (0) or
            // "useless" (1)
            else {
              if(!l->getDel()) {
                criticalPair(gPrev,critPairs,lTag,rTag,rules,rejectedGBList,plus);
              }
              else {
                criticalPair2(gPrev,critPairs,lTag,rTag,rules, rejectedGBList);
              }
            }
        }

    // if there are "bad" reducers than try to compute new S-polynomials and rules

    if(NULL != bad->getFirst()) {
        //Print("BAD STUFF LIST:\n");
        //bad->print();
        LNode* tempBad  =   bad->getFirst();
        //pWrite(l->getPoly());
        while(NULL != tempBad) {
            if(pDivisibleBy(tempBad->getPoly(),l->getPoly())) {
                //Print("BAD STUFF\n");
                //pWrite(l->getPoly());
                //pWrite(tempBad->getPoly());
                u   =   pMDivide(l->getPoly(),tempBad->getPoly());
                //Print("MULTIPLIER:  ");
                //pWrite(u);
                pSetCoeff(u,nOne);
                if(pLmCmp(ppMult_qq(u,tempBad->getTerm()),l->getTerm()) != 0) {
                    // passing criterion2 ?
                    if(!criterion2(gPrev->getFirst()->getIndex(), u,tempBad,rules,rTag)) {
                        // passing criterion1 ?
                        if(!criterion1(gPrev,u,tempBad,lTag)) {
                            //Print("HIERHIERHIERHIERHIERHIER\n");
                            rules->insert(tempBad->getIndex(),ppMult_qq(u,tempBad->getTerm()));
                            numberOfRules++;
                            //gPrev->print();
                            //pWrite(l->getPoly());
                            poly temp   =   ksOldSpolyRedNew(ppMult_qq(u,tempBad->getPoly()),l->getPoly());
                            //pWrite(l->getPoly());
                            //Print("%p\n",temp);
                            //gPrev->print();
                            if(NULL != temp) {
                                pNorm(temp);
                                LNode* tempBadNew   =   new LNode(ppMult_qq(u,tempBad->getTerm()),tempBad->getIndex(),temp,rules->getFirst()->getRuleOld());
                                //pWrite(temp);
                                //pWrite(tempBadNew->getPoly());
                                //pWrite(tempBadNew->getTerm());
                                //pWrite(pHead(tempBadNew->getPoly()));
                                //Print("%p\n",tempBadNew->getPoly());
                                //tempRed->getLPoly()->setRule(rules->getFirst()->getRule());
                                tempBadNew->setDel(1);

                                sPolyList->insertByLabel(tempBadNew);
                                //Print("BAD SPOLYLIST: \n");
                                //sPolyList->print();
                            }
                        }
                    }
                }
            }
        //Print("HIER\n");
            tempBad =   tempBad->getNext();
            //Print("%p\n",tempBad);
        }
       // Print("-------------------BAD STUFF LIST-----------------------------\n");
    }
        //Print("HIER AUCH\n");
        //Print("SPOLYLIST IN BAD: \n");
        //sPolyList->print();
    //Print("END FIND REDUCERS\n");
}

/*
=======================================================================================
merging 2 polynomials p & q without requiring that all monomials of p & q are different
if there are equal monomials in p & q only one of these monomials (always that of p!)
is taken into account
=======================================================================================

poly p_MergeEq_q(poly p, poly q, const ring r) {
  assume(p != NULL && q != NULL);
  p_Test(p, r);
  p_Test(q, r);
#if PDEBUG > 0
  int l = pLength(p) + pLength(q);
#endif

  spolyrec rp;
  poly a = &rp;
  DECLARE_LENGTH(const unsigned long length = r->CmpL_Size);
  DECLARE_ORDSGN(const long* ordsgn = r->ordsgn);

  Top:     // compare p and q w.r.t. monomial ordering
  p_MemCmp(p->exp, q->exp, length, ordsgn, goto Equal, goto Greater , goto Smaller);

  Equal:
  a =   pNext(a)    =   p;
  pIter(p);
  pIter(q);
  if(NULL == p) {
      if(NULL == q) {
          goto Finish;
      }
      else {
          pNext(a)  =   q;
          goto Finish;
      }
  }
  goto Top;

  Greater:
  a = pNext(a) = p;
  pIter(p);
  if (p==NULL) { pNext(a) = q; goto Finish;}
  goto Top;

  Smaller:
  a = pNext(a) = q;
  pIter(q);
  if (q==NULL) { pNext(a) = p; goto Finish;}
  goto Top;

  Finish:

  p_Test(pNext(&rp), r);
#if PDEBUG > 0
  pAssume1(l - pLength(pNext(&rp)) == 0);
#endif
  return pNext(&rp);
}
*/

/*
=====================================================================================
top reduction in F5, i.e. reduction of a given S-polynomial by labeled polynomials of
the same index whereas the labels are taken into account
=====================================================================================
*/
void topReduction(LNode* l, LList* sPolyList, LList* gPrev, CListOld* critPairs,  RList* rules, LTagList* lTag, RTagList* rTag, ideal gbPrev, PList* rejectedGBList, int plus) {
    //Print("##########################################In topREDUCTION!########################################\n");
    // try l as long as there are reductors found by findReductor()
    LNode* gPrevRedCheck    =   lTag->getFirstCurrentIdx();
    LNode* tempRed;
    poly pOne               =   pOne();
    do {
        //int timer5  =   initTimer();
        //startTimer();
        //Print("TOP REDUCTION:  ");
        //pWrite(l->getPoly());
        tempRed  =   findReductor(l,sPolyList,gPrevRedCheck,gPrev,rules,lTag,rTag);
        //timer5  =   getTimer();
        //reductionTime   =   reductionTime   +   timer5;
        // if a reductor for l is found and saved in tempRed
        if(NULL != tempRed) {
            // if label of reductor is greater than the label of l we have to built a new element
            // and add it to sPolyList

            if(pLmCmp(tempRed->getTerm(),l->getTerm()) == 1) {
                // needed sinc pSub destroys the arguments!
                //poly temp_poly_l    =   pInit();
                //temp_poly_l         =   pCopy(l->getPoly());
                //Print("VORHER: ");
                //pWrite(tempRed->getPoly());
                //poly temp           =   pMinus_mm_Mult_qq(tempRed->getPoly(),pOne,l->getPoly());
                poly temp   =   ksOldSpolyRedNew(l->getPoly(),tempRed->getPoly());
                rules->insert(tempRed->getIndex(),tempRed->getTerm());
                //Print("NACHHER: ");
                //pWrite(tempRed->getPoly());
                //Print("TEMP: ");
                //pWrite(temp);
                if(NULL != temp) {
                    pNorm(temp);
                    //tempRed->setPoly(temp);
                    //tempRed->setDel(1);
                    //rules->insert(tempRed->getIndex(),tempRed->getTerm());
                    LNode* tempRedNew   =   new LNode(tempRed->getTerm(),tempRed->getIndex(),temp,rules->getFirst()->getRuleOld());
                    //tempRed->getLPoly()->setRule(rules->getFirst()->getRule());
                    tempRedNew->setDel(1);
                    sPolyList->insertByLabel(tempRedNew);
                }
                else {
                    pDelete(&temp);
                    reductionsToZero++;
                    //delete tempRed;
                }
            }

            // label of reductor is smaller than the label of l, subtract reductor from l and delete the
            // gPrevRedCheck pointer added to l during findReductor() as the head term of l changes
            // after subtraction
            else {

                //poly temp_poly_l    =   pInit();
                //temp_poly_l         =   pCopy(l->getPoly());
                //poly temp   =   pMinus_mm_Mult_qq(tempRed->getPoly(),pOne,l->getPoly());
                //Print("REDUCER: ");
                //pWrite(tempRed->getPoly());
                //pWrite(tempRed->getTerm());
                poly temp   =   ksOldSpolyRedNew(l->getPoly(),tempRed->getPoly());
                //Print("REDUCED ELEMENT:  ");
                if(NULL != temp) {
                    pNorm(temp);
                    //pWrite(temp);
                    poly tempNF =   kNF(gbPrev,currRing->qideal,temp);
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
                //Print("ELEMENT ADDED TO GPREV: ");
                //pWrite(l->getPoly());
                gPrev->insert(l->getLPolyOld());
                //Print("TEMP RED == 0  ");
                //pWrite(l->getPoly());
                //pWrite(l->getTerm());
                //rules->print();
                criticalPair(gPrev,critPairs,lTag,rTag,rules, rejectedGBList,plus);
                //Print("LIST OF CRITICAL PAIRS:    \n");
                //critPairs->print();
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
LNode* findReductor(LNode* l, LList* sPolyList, LNode* gPrevRedCheck, LList* gPrev, RList* rules, LTagList* lTag,RTagList* rTag, PList* rejectedGBList) {
    // allociation of memory for the possible reductor
    //Print("LPOLY:  ");
    //pWrite(l->getPoly());
    poly u      =   pOne();
    poly red;
    number nOne =   nInit(1);
    LNode* temp;
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
            u   =   pMDivide(t,temp->getPoly());
            pSetCoeff(u,nOne);
            red =   ppMult_qq(u,temp->getPoly());
            pNorm(red);
            // check if both have the same label
            if(pLmCmp(ppMult_qq(u,temp->getTerm()),l->getTerm()) != 0) {
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
            if(pLmCmp(ppMult_qq(u,temp->getTerm()),l->getTerm()) == 1) {
                // passing criterion2 ?
                if(!criterion2(gPrev->getFirst()->getIndex(), u,temp,rules,rTag)) {
                    // passing criterion1 ?
                    if(!criterion1(gPrev,u,temp,lTag)) {
                        poly tempSpoly  =   ksOldSpolyRedNew(red,l->getPoly());
                        rules->insert(temp->getIndex(),ppMult_qq(u,temp->getTerm()));
                        gPrevRedCheck   =   temp->getNext();
                        if(NULL != tempSpoly) {
                            pNorm(tempSpoly);
                            sPolyList->insertByLabel(ppMult_qq(u,temp->getTerm()),temp->getIndex(),tempSpoly,rules->getFirst()->getRuleOld());
                    //Print("NEW ONE: ");
                    //pWrite(tempSpoly);
                    //Print("HIER\n");
                            //sPolyList->print();
                            //sleep(5);
                        }
                    }
                }
            }
        }
        //Print("AUCH HIER\n");
        temp = temp->getNext();
    }

//    delete temp;
 return NULL;
}



/*
==========================================================================
MAIN:computes a gb of the ideal i in the ring r with our F5 implementation
OPTIONS: INTEGER "opt" is to be set "0" for F5, "1" for F5R, "2" for F5C
==========================================================================
*/
ideal F5main(ideal id, ring r, int opt, int plus, int termination)
{
  switch(opt)
  {
    case 0:
      PrintS("\nComputations are done by the standard F5 Algorithm");
      break;
    case 1:
      PrintS("\nComputations are done by the variant F5R of the F5 Algorithm");
      break;
    case 2:
      PrintS("\nComputations are done by the variant F5C of the F5 Algorithm");
      break;
    default:
      WerrorS("\nThe option can only be set to \"0\", \"1\" or \"2\":\n\"0\": standard F5 Algorithm\n\"1\": variant F5R\n\"2\": variant F5C\nComputations are aborted!\n");
      return id;
  }

    int timer   =   initTimer();
    startTimer();
    int i,j,k,l;
    int gbLength;
    // 1 polynomial for defining initial labels & further tests
    poly ONE = pOne();
    poly pOne   =   pOne();
    number nOne =   nInit(1);
    // tag the first element of index i-1 for criterion 1
    //Print("LTAG BEGINNING: %p\n",lTag);

    // DEBUGGING STUFF START
    //Print("NUMBER: %d\n",r->N);
    /*
    int* ev = new int[r->N +1];
    for(i=0;i<IDELEMS(id);i++) {
        p_GetExpV(id->m[i],ev,currRing);
        //ev2  =   pGetExp(id->m[i],1);
        pWrite(id->m[i]);
        Print("EXP1: %d\n",ev[1]);
        Print("EXP2: %d\n",ev[2]);
        Print("EXP3: %d\n\n",ev[3]);
        Print("SUM: %ld\n\n\n",sumVector(ev,r->N));
    }
    delete ev;
    */
    /*DEBUGGING STUFF END */

    // first element in rTag is first element of rules which is NULL RNode,
    // this must be done due to possible later improvements
    RList* rules    =   new RList();
    //Print("RULES FIRST: %p\n",rules->getFirst());
    //Print("RULES FIRST DATA: %p\n",rules->getFirst()->getRule());
    //RTagList* rTag  =   new RTagList(rules->getFirst());
    RTagList* rTag  =   NULL;
    i = 1;
    /*for(j=0; j<IDELEMS(id); j++) {
        if(NULL != id->m[j]) {
            if(pComparePolys(id->m[j],ONE)) {
                PrintS("One Polynomial in Input => Computations stopped\n");
                ideal idNew = idInit(1,1);
                idNew->m[0] = ONE;
                return(idNew);
            }
        }
    }*/
    ideal idNew     =   kInterRed(id);
    id              =   idNew;
    //qsortDegree(&id->m[0],&id->m[IDELEMS(id)-1]);
    //idShow(id);
    LList* gPrev    =   new LList(ONE, i, id->m[0]);
    LList* reducers =   new LList();
    //idShow(id);
    //Print("%p\n",id->m[0]);
    //pWrite(id->m[0]);
    //Print("%p\n",gPrev->getFirst()->getPoly());
    //pWrite(gPrev->getFirst()->getPoly());

    LTagList* lTag  =   new LTagList(gPrev->getFirst());
    //lTag->insert(gPrev->getFirst());
    lTag->setFirstCurrentIdx(gPrev->getFirst());
    // computing the groebner basis of the elements of index < actual index
    gbLength    =   gPrev->getLength();
    //Print("Laenge der bisherigen Groebner Basis: %d\n",gPrev->getLength());
    ideal gbPrev    =   idInit(gbLength,1);
    // initializing the groebner basis of elements of index < actual index
    gbPrev->m[0]    =   gPrev->getFirst()->getPoly();
    //idShow(gbPrev);
    //idShow(currRing->qideal);
    for(i=2; i<=IDELEMS(id); i++) {
        LNode* gPrevTag =   gPrev->getLast();
        //Print("Last POlynomial in GPREV: ");
        //Print("%p\n",gPrevTag);
        //pWrite(gPrevTag->getPoly());
        Print("Iteration: %d\n\n",i);
        gPrev   =   F5inc(i, id->m[i-1], gPrev, reducers, gbPrev, ONE, lTag, rules, rTag, plus, termination);
        //Print("%d\n",gPrev->count(gPrevTag->getNext()));
        //Print("%d\n",gPrev->getLength());
        //Print("____________________________________ITERATION STEP DONE________________________________________\n");

        // DEBUGGING STUFF
        LNode* temp    =   gPrev->getFirst();


        /////////////////////////////////////////////////////////////////////////////////
        //                                                                             //
        // one needs to choose one of the following 3 implementations of the algorithm //
        // F5,F5R or F5C                                                               //
        //                                                                             //
        /////////////////////////////////////////////////////////////////////////////////


        //
        // standard "F5"
        //
        if(0 == opt) {
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
                }
                    gbPrev          =   idAdd(gbPrev,gbAdd);
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
            //if(i == IDELEMS(id)) {
            //    ideal tempId        =   kInterRed(gbPrev);
            //    gbPrev              =   tempId;
            //}
        }
        gbLength    =   gPrev->getLength();
        }


        //
        //  "F5R"
        //
        if(1 == opt) {
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
                }
                    gbPrev          =   idAdd(gbPrev,gbAdd);
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
            // comment this out if you want F5 instead of F5R
            if(i<IDELEMS(id)) {
                ideal tempId    =   kInterRed(gbPrev);
                gbPrev          =   tempId;
            }
        }
        gbLength    =   gPrev->getLength();
        }


        //
        // "F5C"
        //
        if(2 == opt) {
        if(gPrev->getLength() > gbLength) {
            if(i < IDELEMS(id)) {
                ideal gbAdd =   idInit(gPrev->getLength()-gbLength,1);
                LNode*  temp =   gPrevTag;
                for(j=0;j<=gPrev->getLength()-gbLength-1;j++) {
                    temp        =   temp->getNext();
                        gbAdd->m[j] =   temp->getPoly();
                }
                    gbPrev          =   idAdd(gbPrev,gbAdd);
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
            if(i<IDELEMS(id)) {
                ideal tempId    =   kInterRed(gbPrev);
                gbPrev          =   tempId;
                delete gPrev;
                delete rules;
                gPrev    =   new LList(pOne,1,gbPrev->m[0]);
                gPrev->insert(pOne,1,gbPrev->m[1]);
                rules    =   new RList();
                //rTag     =   new RTagList(rules->getFirst());
                for(k=2; k<IDELEMS(gbPrev); k++) {
                    gPrev->insert(pOne,k+1,gbPrev->m[k]);
                    /*
                    for(l=0; l<k; l++) {
                    }
                    rTag->insert(rules->getFirst());
                    */
                }
            }
            gbLength    =   gPrev->getLength();
        }
        }


    }
    timer   =   getTimer();
    //Print("\n\nADDING TIME IN REDUCTION: %d\n\n",reductionTime);
    Print("\n\nNumber of zero-reductions:           %d\n",reductionsToZero);
    Print("Number of rules:                     %d\n",numberOfRules);
    Print("Number of rejected F5-critical pairs:%d\n",numberRejectedF5CriticalPairs);
    Print("Number of reductions:                %d\n",numberOfReductions);
    Print("Elements not added to G:             %d\n",notInG);
    Print("Highest Degree during computations:  %d\n",highestDegree);
    Print("Degree d_0 in F5+:                   %d\n",highestDegreeGBCriticalPair);
    Print("Time for computations:               %d/1000 seconds\n",timer);
    Print("Number of elements in gb:            %d\n",IDELEMS(gbPrev));
    //LNode* temp    =   gPrev->getFirst();
    //while(NULL != temp) {
    //    pWrite(temp->getPoly());
    //    temp    =   temp->getNext();
    // }
    //gPrev->print();
    //delete lTag;
    delete rTag;
    delete gPrev;
    notInG                        =   0;
    numberOfRules                 =   0;
    reductionsToZero              =   0;
    highestDegree                 =   0;
    highestDegreeGBCriticalPair   =   0;
    reductionTime                 =   0;
    spolsTime                     =   0;
    numberUselessPairs            =   0;
    numberUsefulPairs             =   0;
    numberRejectedF5CriticalPairs =   0;
    numberOfReductions            =   0;
    numberOfSpolys                =   0;
    return(gbPrev);


}

#endif
