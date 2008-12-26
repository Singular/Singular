/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lists.h,v 1.1 2008-12-26 13:51:10 ederc Exp $ */
/*
* ABSTRACT: list interface
*/
#include "lpolynomial.h"
#ifndef LISTS_HEADER
#define LISTS_HEADER

#ifdef HAVE_F5


/*
=======================
=======================
linked lists for LPolys
=======================
=======================
*/


/*
===========================
classes for lists of LPolys 
===========================
*/
class LNode;
class LList;


/*
=======================================
LNode class (nodes for lists of LPolys)
=======================================
*/
class LNode {
    private:
        LPoly* data;
        LNode* next;
    public:
        LNode(LPoly* lp, LNode* n) {
            data = lp;
            next = n;
        }
        LNode(poly* t, long* i, poly* p, LNode* n) {
            data->set(t,i,p);
            next = n;
        }
        ~LNode() {
            delete next;
            delete data;   
        }
        LNode* append(LPoly* lp) {
            LNode* new_element = new LNode(lp,NULL);
            next = new_element;
            return new_element;
        }
        LNode* append(poly* t, long* i, poly* p) {
            LNode* new_element = new LNode(t,i,p,NULL);
            next = new_element;
            return new_element;
        }
        LPoly* getLPoly() const {
            return data;
        }
};


/*
============================
class LList(lists of LPolys)
============================
*/
class LList {
    private:
        LNode*  first;
        LNode*  last;
        long    length;
    public:
        LList(LPoly* lp) {
            first = new LNode(lp,NULL);
            last = first;
            length = 1;
        }
        ~LList() {
            delete first;
        }
        void append(LPoly* lp) {
            last = last->append(lp);
            length++;
        }
        void append(poly* t,long* i, poly* p) {
            last = last->append(t,i,p);
            length++;
        }
        long getLength() const {
            return length;
        }
        LNode* getFirst() const {
            return first;
        }
        LNode* getLast() const {
            return last;
        }
};


/*
=======================
=======================
linked lists for CPairs 
=======================
=======================
*/


/*
===========================
classes for lists of CPairs 
===========================
*/
class CNode;
class CList;


/*
=======================================
CNode class (nodes for lists of CPairs)
=======================================
*/
class CNode {
    private:
        LPoly* data;
        CNode* next;
    public:
        CNode(LPoly* lp, CNode* n) {
            data = lp;
            next = n;
        }
        ~CNode() {
            delete next;
            delete data;   
        }
        CNode* append(LPoly* lp) {
            CNode* new_element = new CNode(lp,NULL);
            next = new_element;
            return new_element;
        }
        LPoly* getLPoly() const {
            return data;
        }
};


/*
=============================
class CPList(lists of CPairs)
=============================
*/
class CList {
    private:
        CNode*  first;
        CNode*  last;
        long    length;
    public:
        CList(LPoly* lp) {
            first = new CNode(lp,NULL);
            last = first;
            length = 1;
        }
        ~CList() {
            delete first;
        }
        void append(LPoly* lp) {
            last = last->append(lp);
            length++;
        }
        long getLength() const {
            return length;
        }
        CNode* getFirst() const {
            return first;
        }
        CNode* getLast() const {
            return last;
        }
};




#endif
#endif
 
