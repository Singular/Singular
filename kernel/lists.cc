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
====================================
functions working on the class LNode
====================================
*/

// generating new list elements (labeled / classical polynomial / LNode view)
LNode::LNode(LPoly* lp) {
    data = lp;
    next = NULL;
}
       
LNode::LNode(poly* t, int* i, poly* p) {
    LPoly* lp = new LPoly(t,i,p);
    data = lp;
    Print("Index angelegt?  %d\n",*(data->getIndex()));
    next = NULL;
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
    LNode* newElement = new LNode(lp);
    newElement->next = this;
    return newElement;
}
        
LNode* LNode::insert(poly* t, int* i, poly* p) {
    LNode* newElement = new LNode(t,i,p);
    newElement->next = this;
    return newElement;
}
        
// get next from current LNode
LNode* LNode::getNext() {
    return next;
}
        
// get the LPoly* out of LNode*
LPoly* LNode::getLPoly() {
    return data;
}

// get the address of the polynomial part of LPoly* of LNode*
poly* LNode::getPoly() {
    return data->getPoly();
}

// test if for any list element the polynomial part of the data is equal to *p
bool LNode::polyTest(poly* p) {
    LNode* temp = new LNode(this);
    while(NULL != temp) {
        if(pComparePolys(*(temp->getPoly()),*p)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}


/*
====================================
functions working on the class LList
====================================
*/

LList::LList(LPoly* lp) {
    first   = new LNode(lp);
    length  = 1;
}

LList::LList(poly* t,int* i,poly* p) {
    first   = new LNode(t,i,p);
    length  = 1;
} 

LList::~LList() {
    delete first;
}

// insertion in front of the list
void LList::insert(LPoly* lp) {
    first = first->insert(lp);
    length++;
}

void LList::insert(poly* t,int* i, poly* p) {
    first = first->insert(t,i,p);
    length++;
}

bool LList::polyTest(poly* p) {
    return first->polyTest(p);
}

int LList::getLength() const {
    return length;
}

LNode* LList::getFirst() {
    return first;
}


/*
=======================================
functions working on the class PrevNode
=======================================
*/

PrevNode::PrevNode(LNode* l) {
    data = l;
    next = NULL;
}
       
PrevNode::~PrevNode() {
    delete next;
    delete data;   
}
       
PrevNode* PrevNode::append(LNode* l) {
    PrevNode* new_element = new PrevNode(l);
    next = new_element;
    return new_element;
}

LNode* PrevNode::getLNode() {
    return this->data;
}

LNode* PrevNode::getPrevLast(int i) {
    int j;
    PrevNode* temp = this;
    for(j=1;j<=i-1;j++) {
        temp = temp->next;
    }
    return temp->data;
}

/*
=======================================
functions working on the class PrevList
=======================================
*/

PrevList::PrevList(LNode* l) {
    PrevNode* first =   new PrevNode(l);
    last            =   first;    
}

void PrevList::append(LNode* l) {
    last = last->append(l);
}

LNode* PrevList::getPrevLast(int i) {
    switch(i) {
        case 0:     return NULL;
        case 1:     return first->getLNode();
        default:    first->getPrevLast(i);
    }
}

/*
====================================
functions working on the class CNode
====================================
*/

CNode::CNode(CPair* c) {
    data    =   c;   
    next    =   NULL;    
}

CNode::~CNode() {
    delete next;
    delete data;
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
// working only with linked, but not doubly linked lists due to memory usage we have to check the 
// insertion around the first element separately from the insertion around all other elements in the list
CNode* CNode::insert(CPair* c) {
    if( c->getDeg() < this->data->getDeg() ) { // lower degree than the first list element
        CNode* newElement = new CNode(c);
        newElement->next = this;
        return newElement;
    }
    if( c->getDeg() == this->data->getDeg() ) { // same degree than the first list element
        if( c->getT1() <= this->data->getT1() ) {
            CNode* newElement   =   new CNode(c);
            newElement->next    =   this;
            return newElement;
        }
        else {
            CNode* temp = this;
            while(  temp->next != NULL ) {
                if( temp->next->data->getDeg() == c->getDeg() ) { 
                    if( c->getT1() <= temp->next->data->getT1() ) {
                        temp = temp->next;
                    }
                    else {
                        CNode* newElement   =   new CNode(c);
                        newElement->next    =   temp->next;
                        temp->next          =   newElement;
                        return this;
                    } 
                }
                else {
                    CNode* newElement   =   new CNode(c);
                    newElement->next    =   temp->next;
                    temp->next          =   newElement;
                    return this;
                }
            }
            CNode* newElement   =   new CNode(c);
            newElement->next    =   NULL;
            temp->next          =   newElement;
            return this;
        }
    } // outer if-clause
    if( c->getDeg() > this->data->getDeg() ) { // greater degree than the first list element
        CNode* temp =   this;
        while( temp->next != NULL) {   
            if( c->getDeg() < temp->next->data->getDeg() ) {
                CNode* newElement   =   new CNode(c);
                newElement->next    =   temp->next;
                temp->next          =   newElement;
                return this;
            }
            if( c->getDeg() == temp->next->data->getDeg() ) {
                if( c->getT1() <= temp->next->data->getT1() ) {
                    CNode* newElement   =   new CNode(c);
                    newElement->next    =   temp->next;
                    temp->next          =   newElement;
                    return this;
                }
                else {
                    temp = temp->next;
                    while(  temp->next != NULL ) {
                        if( temp->next->data->getDeg() == c->getDeg() ) { 
                            if( c->getT1() <= temp->next->data->getT1() ) {
                                temp = temp->next;
                            }
                            else {
                                CNode* newElement   =   new CNode(c);
                                newElement->next    =   temp->next;
                                temp->next          =   newElement;
                                return this;
                            } 
                        }
                        else {
                            CNode* newElement   =   new CNode(c);
                            newElement->next    =   temp->next;
                            temp->next          =   newElement;
                            return this;
                        }
                    }
                    CNode* newElement   =   new CNode(c);
                    newElement->next    =   NULL;
                    temp->next          =   newElement;
                    return this;
                }
            }
            if( c->getDeg() > temp->next->data->getDeg() ) {
                temp    =   temp->next;
            }
        }
        CNode* newElement   =   new CNode(c);
        newElement->next    =   NULL;
        temp->next          =   newElement;
        return this;
    }
}

/*
====================================
functions working on the class CList
====================================
*/

CList::CList(CPair* c) {
    first   = new CNode(c);
}

CList::~CList() {
    delete first;
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
void CList::insert(CPair* c) {
    first = first->insert(c);
}
#endif
