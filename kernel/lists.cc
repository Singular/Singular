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

// generating new list elements in the labeled / classical polynomial / LNode view
LNode::LNode(LPoly* lp) {
    data = lp;
    next = NULL;
}
       
LNode::LNode(poly* t, long* i, poly* p) {
    LPoly* lp = new LPoly(t,i,p);
    data = lp;
    Print("Index angelegt?  %ld\n",*(data->getIndex()));
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
       
// append new elements to the list from the labeled / classical polynomial view
LNode* LNode::append(LPoly* lp) {
    LNode* new_element = new LNode(lp);
    next = new_element;
    return new_element;
}
        
LNode* LNode::append(poly* t, long* i, poly* p) {
    LNode* new_element = new LNode(t,i,p);
    next = new_element;
    return new_element;
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
bool LNode:polyTest(poly* p) {
    LNode* temp = new LNode(this);
    while(NULL != temp) {
        if(pComparePolys(*(temp->getPoly()),*p)) {
            return 1;
        }
        temp = temp->next;
    }
    return 0;
}

LNode* LNode::operator++() {
    LNode* temp= new LNode(this);
    return temp->getNext();
}



/*
====================================
functions working on the class LList
====================================
*/

LList::LList(LPoly* lp) {
    first   = new LNode(lp);
    last    = first;
    length  = 1;
}

LList::LList(poly* t,long* i,poly* p) {
    first   = new LNode(t,i,p);
    last    = first;
    length  = 1;
} 

LList::~LList() {
    delete first;
}

void LList::append(LPoly* lp) {
    last = last->append(lp);
    length++;
}

void LList::append(poly* t,long* i, poly* p) {
    last = last->append(t,i,p);
    length++;
}

bool LList::polyTest(poly* p) {
    return first->polyTest(p);
}

long LList::getLength() const {
    return length;
}

LNode* LList::getFirst() {
    return first;
}

LNode* LList::getLast() {
    return last;
}
#endif
