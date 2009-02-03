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
====================================
functions working on the class LNode
====================================
*/

// generating new list elements (labeled / classical polynomial / LNode view)
LNode::LNode(LPoly* lp) {
    data = lp;
    next = NULL;
}
       
LNode::LNode(LPoly* lp, LNode* l) {
    data = lp;
    next = l;
}
LNode::LNode(poly* t, int* i, poly* p) {
    LPoly* lp = new LPoly(t,i,p);
    data = lp;
    next = NULL;
}
       
LNode::LNode(poly* t, int* i, poly* p, LNode* l) {
    LPoly* lp = new LPoly(t,i,p);
    data = lp;
    next = l;
}

 LNode::LNode(LNode* ln) {
    data = ln->getLPoly();
    next = ln->getNext();
}
        
LNode::~LNode() {
    delete next;
    delete data;   
}
       
// insert new elements to the list always in front (labeled / classical polynomial view)
LNode* LNode::insert(LPoly* lp) {
    LNode* newElement = new LNode(lp, this);
    return newElement;
}
        
LNode* LNode::insert(poly* t, int* i, poly* p) {
    LNode* newElement = new LNode(t, i, p, this);
    return newElement;
}
        
// insert new elemets to the list w.r.t. increasing labels
// only used for the S-polys to be reduced (TopReduction building new S-polys with higher label)
LNode* LNode::insertByLabel(LPoly* lp) {
    if( lp->getTerm() <= this->data->getTerm() ) {
        LNode* newElement   =   new LNode(lp, this);
        return newElement;
    }
    else {
        LNode* temp = this;
        while( NULL != temp->next ) {
            if( lp->getTerm() <= temp->next->data->getTerm() ) {
                LNode* newElement   =   new LNode(lp, temp->next);
                temp->next          =   newElement;
                return this;
            }
            else {
                temp = temp->next;
            }
        }
        LNode* newElement   =   new LNode(lp, NULL);
        temp->next          =   newElement;
        return this;
    }
}

// deletes the first elements of the list with the same degree
// only used for the S-polys, which are already sorted by increasing degree by CList
LNode*  LNode::deleteByDeg() {
    return this;
}

// get next from current LNode
LNode* LNode::getNext() {
    return next;
}
        
// get the LPoly* out of LNode*
LPoly* LNode::getLPoly() {
    return data;
}

// get the data from the LPoly saved in LNode
poly LNode::getPoly() {
    return data->getPoly();
}

poly LNode::getTerm() {
    return data->getTerm();
}

int LNode::getIndex() {
    return data->getIndex();
}

// test if for any list element the polynomial part of the data is equal to *p
bool LNode::polyTest(poly* p) {
    LNode* temp = new LNode(this);
    while(NULL != temp) {
        if(pComparePolys(temp->getPoly(),*p)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

LNode* LNode::getNext(LNode* l) {
    return l->next;
}

/*
====================================
functions working on the class LList
====================================
*/

LList::LList(LPoly* lp) {
    first   = new LNode(lp);
}

LList::LList(poly* t,int* i,poly* p) {
    first   = new LNode(t,i,p);
} 

LList::~LList() {
    delete first;
}

// insertion in front of the list
void LList::insert(LPoly* lp) {
    first = first->insert(lp);
}

void LList::insert(poly* t,int* i, poly* p) {
    first = first->insert(t,i,p);
}

void LList::insertByLabel(LPoly* lp) {
    first = first->insertByLabel(lp);
}

void LList::deleteByDeg() {
    first = first->deleteByDeg();
}

bool LList::polyTest(poly* p) {
    return first->polyTest(p);
}

LNode* LList::getFirst() {
    return first;
}

LNode* LList::getNext(LNode* l) {
    return l->getNext();
}

/*
=======================================
functions working on the class LTagNode
=======================================
*/

LTagNode::LTagNode(LNode* l) {
    data = l;
    next = NULL;
}
       
LTagNode::LTagNode(LNode* l, LTagNode* n) {
    data = l;
    next = n;
}

 LTagNode::~LTagNode() {
    delete next;
    delete data;   
}
       
// declaration with first as parameter due to sorting of LTagList
LTagNode* LTagNode::insert(LNode* l) {
    LTagNode* newElement  = new LTagNode(l, this);
    return newElement;
}

LNode* LTagNode::getLNode() {
    return this->data;
}

// NOTE: We insert at the beginning of the list and length = i-1, where i is the actual index.
//       Thus given actual index i and idx being the index of the LPoly under investigation
//       the element on position length-idx is the right one
LNode* LTagNode::get(int idx, int length) {
    if(idx == 1) {
        return NULL;
    }
    else {
        int j;
        LTagNode* temp = this; // last
        for(j=1;j<=length-idx+1;j++) {
            temp = temp->next;
        }
        return temp->data;
    }
}


/*
=======================================
functions working on the class LTagList
=======================================
*/

LTagList::LTagList(LNode* l) {
    LTagNode* first =   new LTagNode(l);
    length          =   1;
}

// declaration with first as parameter in LTagNode due to sorting of LTagList
void LTagList::insert(LNode* l) {
    first   =   first->insert(l);
    length++;
}

LNode* LTagList::get(int idx) {
    return first->get(idx, length);
}

/*
====================================
functions working on the class CNode
====================================
*/

CNode::CNode() {
    data    =   NULL;   
    next    =   NULL;    
}

CNode::CNode(CPair* c) {
    data    =   c;   
    next    =   NULL;    
}

CNode::CNode(CPair* c, CNode* n) {
    data    =   c;   
    next    =   n;    
}

CNode::~CNode() {
    delete next;
    delete data;
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
// working only with linked, but not doubly linked lists due to memory usage we have to check the 
// insertion around the first element separately from the insertion around all other elements in the list
CNode* CNode::insert(CPair* c, CNode* last) {
    if(NULL == this->data) {
        CNode* newElement   =   new CNode(c, this);
        return newElement;
    }
    else {
        poly u1 = ppMult_qq(c->getT1(),c->getLp1Term());
        if( c->getDeg() < this->data->getDeg() ) { // lower degree than the first list element
            CNode* newElement   =   new CNode(c, this);
            return newElement;
        }
        if( c->getDeg() == this->data->getDeg() ) { // same degree than the first list element
            if(0 == pLmCmp(u1,ppMult_qq(this->data->getT1(), this->data->getLp1Term())) ||
               -1 == pLmCmp(u1,ppMult_qq(this->data->getT1(), this->data->getLp1Term()))) {
                Print("Leck mich am Arsch: ");
                pWrite(u1);
                Print("Multi-Term in CritPairs Sortierung altes Element: ");
                pWrite(ppMult_qq(this->data->getT1(),this->data->getLp1Term()));
                CNode* newElement   =   new CNode(c, this);
                return newElement;
            }
            else {
                Print("Insert Deg\n");
                CNode* temp = this;
                while(  NULL != temp->next->data ) {
                    if(temp->next->data->getDeg() == c->getDeg() ) { 
                        if(1 == pLmCmp(u1,ppMult_qq(temp->next->data->getT1(),temp->next->data->getLp1Term()))) {
                            temp = temp->next;
                        }
                        else {
                            CNode* newElement   =   new CNode(c, temp->next);
                            temp->next          =   newElement;
                            return this;
                        } 
                    }
                    else {
                        CNode* newElement   =   new CNode(c, temp->next);
                        temp->next          =   newElement;
                        return this;
                    }
                }
                CNode* newElement   =   new CNode(c, last);
                temp->next          =   newElement;
                return this;
            }
        } // outer if-clause
        if( c->getDeg() > this->data->getDeg() ) { // greater degree than the first list element
            CNode* temp =   this;
            while( NULL != temp->next->data ) {   
                if( c->getDeg() < temp->next->data->getDeg() ) {
                    CNode* newElement   =   new CNode(c, temp->next);
                    temp->next          =   newElement;
                    return this;
                }
                if( c->getDeg() == temp->next->data->getDeg() ) {
                    if(0 == pLmCmp(u1,ppMult_qq(temp->next->data->getT1(),temp->next->data->getLp1Term())) || 
                       -1 == pLmCmp(u1,ppMult_qq(temp->next->data->getT1(),temp->next->data->getLp1Term()))) { 
                        CNode* newElement   =   new CNode(c, temp->next);
                        temp->next          =   newElement;
                        return this;
                    }
                    else {
                        temp = temp->next;
                        while(  NULL != temp->next->data ) {
                            if( temp->next->data->getDeg() == c->getDeg() ) { 
                                if(1 == pLmCmp(u1,ppMult_qq(temp->next->data->getT1(),
                                               temp->next->data->getLp1Term()))) {
                                    temp = temp->next;
                                }
                                else {
                                    CNode* newElement   =   new CNode(c, temp->next);
                                    temp->next          =   newElement;
                                    return this;
                                } 
                            }
                            else {
                                CNode* newElement   =   new CNode(c, temp->next);
                                temp->next          =   newElement;
                                return this;
                            }
                        }
                        CNode* newElement   =   new CNode(c, last);
                        temp->next          =   newElement;
                        return this;
                    }
                }
                if( c->getDeg() > temp->next->data->getDeg() ) {
                    temp    =   temp->next;
                }
            }
            CNode* newElement   =   new CNode(c, last);
            temp->next          =   newElement;
            return this;
        }
    }
}

// get the first elements from CList which by the above sorting have minimal degree
CNode* CNode::getMinDeg() {
    CNode* temp = this;
    while( NULL != temp->data ) {
        while( temp->next->data->getDeg() == this->data->getDeg() ) {
            temp = temp->next;
        }
        CNode* returnCNode  =   temp->next;    
        temp->next          =   NULL;
        return returnCNode;
    }
    return NULL;
}

poly CNode::getLp1Poly() {
    return this->data->getLp1Poly();
}

poly CNode::getLp2Poly() {
    return this->data->getLp2Poly();
}

poly CNode::getLp1Term() {
    return this->data->getLp1Term();
}

poly CNode::getLp2Term() {
    return this->data->getLp2Term();
}

int CNode::getLp1Index() {
    return this->data->getLp1Index();
}

int CNode::getLp2Index() {
    return this->data->getLp2Index();
}

poly CNode::getT1() {
    return this->data->getT1();
}

poly CNode::getT2() {
    return this->data->getT2();
}

// for debugging
void CNode::print() {
    CNode* temp = this;
    Print("List of critical pairs:\n");
    while(NULL != temp->data) {
        Print("Index: %d\n",temp->getLp1Index());
        Print("T1: ");
        pWrite(temp->getT1());
        Print("Lp1 Term: ");
        pWrite(temp->getLp1Term());
        Print("%d\n",temp->getLp2Index());
        pWrite(temp->getT2());
        pWrite(temp->getLp2Term());
        Print("\n");
        temp = temp->next;
    }
}

/*
====================================
functions working on the class CList
====================================
*/
// for initialization of CLists, last element alwas has data=NULL and next=NULL
CList::CList() {
    first   =   new CNode();
    last    =   first;
}

CList::CList(CPair* c) {
    first   =   new CNode(c);
    last    =   first;
}

CList::~CList() {
    delete first;
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
void CList::insert(CPair* c) {
    first = first->insert(c, last);
}

// get the first elements from CList which by the above sorting have minimal degree
// returns the pointer on the first element of those
CNode* CList::getMinDeg() {
    CNode* temp     =   first;
    first           =   first->getMinDeg();
    return temp;
}

void CList::print() {
    first->print();
}

/*
====================================
functions working on the class RNode
====================================
*/
RNode::RNode() {
    data    =   NULL;
    next    =   NULL;
}

RNode::RNode(Rule* r) {
    data    =   r;
    next    =   NULL;
}

RNode::~RNode() {
    delete  next;
    delete  data;
}

RNode* RNode::insert(Rule* r) {
    RNode* newElement   =   new RNode(r);
    newElement->next    =   this;
    return newElement;
}

RNode* RNode::getNext() {
    return next;
}    

Rule* RNode::getRule() {
    return data;
}

int RNode::getRuleIndex() {
    return data->getIndex();
}

poly RNode::getRuleTerm() {
    return data->getTerm();
}

/*
====================================
functions working on the class RList
====================================
*/
RList::RList() {
    first = new RNode();
}

RList::RList(Rule* r) {
    first = new RNode(r);
}

void RList::insert(Rule* r) {
    first = first->insert(r);
}

RNode* RList::getFirst() {
    return first;
}

Rule* RList::getRule() {
    return this->getRule();
}

/*
=======================================
functions working on the class RTagNode
=======================================
*/

RTagNode::RTagNode() {
    data = NULL;
    next = NULL;
}
 
RTagNode::RTagNode(RNode* r) {
    data = r;
    next = NULL;
}
       
RTagNode::RTagNode(RNode* r, RTagNode* n) {
    data = r;
    next = n;
}

 RTagNode::~RTagNode() {
    delete next;
    delete data;   
}
       
// declaration with first as parameter due to sorting of LTagList
RTagNode* RTagNode::insert(RNode* r) {
    Print("Hier1\n");
    RTagNode* newElement  = new RTagNode(r, this);
    Print("Hier2\n");
    return newElement;
}

RNode* RTagNode::getRNode() {
    return this->data;
}

// NOTE: We insert at the beginning of the list and length = i-1, where i is the actual index.
//       Thus given actual index i and idx being the index of the LPoly under investigation
//       the element on position length-idx+1 is the right one
RNode* RTagNode::get(int idx, int length) {
    if(idx==1 || idx==0) {
        return NULL;
    }
    else {
        int j;
        RTagNode* temp = this; 
        for(j=1; j<=length-idx+1; j++) {
            temp = temp->next;
        }
        return temp->data;
    }
}


/*
=======================================
functions working on the class LTagList
=======================================
*/

RTagList::RTagList() {
    RTagNode* first =   new RTagNode();
    length          =   0;
}

RTagList::RTagList(RNode* r) {
    RTagNode* first =   new RTagNode(r);
    length          =   1;
}

// declaration with first as parameter in LTagNode due to sorting of LTagList
void RTagList::insert(RNode* r) {
    first = first->insert(r);
    length++;
}

RNode* RTagList::get(int idx) {
    return first->get(idx, length);
}
#endif
