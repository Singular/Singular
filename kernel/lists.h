/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: lists.h,v 1.3 2009-01-15 17:44:24 ederc Exp $ */
/*
* ABSTRACT: list interface
*/
#include "lpolynomial.h"
#ifndef LISTS_HEADER
#define LISTS_HEADER

#ifdef HAVE_F5
/*
============================
============================
classes for lists used in F5 
============================
============================
*/
class LNode;
class LList;
class PrevNode;
class PrevList;
class CNode;
class CList;


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
                LNode(poly* t, int* i, poly* p);
                LNode(LNode* ln);
                ~LNode();
        // append new elements to the list from the labeled / classical polynomial view
        LNode*  insert(LPoly* lp);
        LNode*  insert(poly* t, int* i, poly* p);
        // get next from current LNode
        LNode*  getNext();
        
        // get the LPoly* out of LNode*
        LPoly*  getLPoly();
        // get the address of the polynomial part of LPoly* of LNode*
        poly*   getPoly();
        // test if for any list element the polynomial part of the data is equal to *p
        bool    polyTest(poly* p);
};


/*
============================
class LList(lists of LPolys)
============================
*/
class LList {
    private:
        LNode*  first;
        int     length;
    public:
                LList(LPoly* lp);
                LList(poly* t,int* i,poly* p);
                ~LList();
        void    insert(LPoly* lp);
        // insertion in front of the list
        void    insert(poly* t,int* i, poly* p);
        bool    polyTest(poly* p);
        int     getLength() const;
        LNode*  getFirst();
};


/*
=============================================
PrevNode class (nodes for lists of gPrevLast)
=============================================
*/
class PrevNode {
    private:
        LNode*      data;
        PrevNode*   next;
    public:
        PrevNode(LNode* l);
        ~PrevNode();
        PrevNode*   append(LNode* l);
        LNode*      getLNode();
        LNode*      getPrevLast(int i);
};


/*
====================================================
class PrevList(lists of last node elements in gPrev)
====================================================
*/
class PrevList {
    private:
        PrevNode*   first;
        PrevNode*   last;
    public:
                PrevList(LNode* l);
                ~PrevList();
        void    append(LNode* l);
        LNode*  getPrevLast(int i);
};


/*
=======================================
CNode class (nodes for lists of CPairs)
=======================================
*/
class CNode {
    private:
        CPair* data;
        CNode* next;
    public:
                CNode(CPair* c);
                ~CNode(); 
        CNode*  insert(CPair* c); 
        CNode*  getLPoly() const; 
};


/*
=============================
class CPList(lists of CPairs)
=============================
*/
class CList {
    private:
        CNode*  first;
    public:
                CList(CPair* c); 
                ~CList(); 
        void    insert(CPair* c); 
};
#endif
#endif
