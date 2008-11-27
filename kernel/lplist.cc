/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lplist.cc,v 1.1 2008-11-27 17:15:55 ederc Exp $ */
/*
* ABSTRACT: list interface
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
#include "lpolynomial.h"
#include "lplist.h"


/*
=========================================
=========================================
implementation of the functions of list.h
=========================================
=========================================
*/




/*
===========================
insert general node in list
===========================
*/
Node* GenNode::insert(LPoly* d) {
    int ret = data->compare(*d);
    switch(ret) {
        case 0: case -1: {
            next = next->insert(d);
            return this;
        }
        case 1: {   
            GenNode* newNode = new GenNode(d,this);
            return newNode;
        }
    }            
    return this;
}

/*
========================
get general node in list
========================
*/
void GenNode::get() {
    data->get();
    next->get();
}

/*
=========================
insert first node in list
=========================
*/
Node* FirstNode::insert(LPoly* d) {
    next = next->insert(d);
    return this;
}

/*
===========================================================================================
get first node in list (no element in this place, so go on to the next element in the list)
===========================================================================================
*/
void FirstNode::get() {
    next->get();
}

/*
=======================
insert end node in list
=======================
*/
Node* EndNode::insert(LPoly* d) {
    GenNode* data = new GenNode(d,this);
    return data;
}

/*
===============================================================================
get end node in list (nothing to do, as there are no more elements in the list)
===============================================================================
*/
void EndNode::get() {
}

/*
=========================
insert an element in list
=========================
*/
void LpList::insert(LPoly* d) {
   start->insert(d);
   // there is no return value also we get the address of the new element in the list
   // returning this value in the other insert functions is due to their virtual 
   // declaration in the base class Node
}

/*
==============================
get all elements from the list 
==============================
*/
void LpList::get() {
    start->get();
}

#endif
