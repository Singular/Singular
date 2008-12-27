/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lists.h,v 1.2 2008-12-27 13:50:05 ederc Exp $ */
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
        // generating new list elements from the labeled / classical polynomial view
                LNode(LPoly* lp);
                LNode(poly* t, long* i, poly* p);
                LNode(LNode* ln);
                ~LNode();
        // append new elements to the list from the labeled / classical polynomial view
        LNode*  append(LPoly* lp);
        LNode*  append(poly* t, long* i, poly* p);
        // get next from current LNode
        LNode*  getNext();
        
        // get the LPoly* out of LNode*
        LPoly*  getLPoly();
        // get the address of the polynomial part of LPoly* of LNode*
        poly*   getPoly();
        // test if for any list element the polynomial part of the data is equal to *p
        bool    polyTest(poly* p);
        LNode*  operator++();
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
                LList(LPoly* lp);
                LList(poly* t,long* i,poly* p);
                ~LList();
        void    append(LPoly* lp);
        void    append(poly* t,long* i, poly* p);
        bool    polyTest(poly* p);
        long    getLength() const;
        LNode*  getFirst();
        LNode*  getLast();
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
