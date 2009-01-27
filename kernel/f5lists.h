/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5lists.h,v 1.1 2009-01-27 10:38:08 Singular Exp $ */
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
class RList;
class RNode;

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
        // insert new elements to the list in first place from the labeled / classical polynomial view
        LNode*  insert(LPoly* lp);
        LNode*  insert(poly* t, int* i, poly* p);
        // insert new elements to the list with resp. to increasing labels
        // only used for the S-polys to be reduced (TopReduction building new S-polys with higher label)
        LNode*  insertByLabel(LPoly* lp);
        // deletes the first elements of the list with the same degree
        // get next from current LNode
        LNode*  getNext();
        // only used for the S-polys, which are already sorted by increasing degree by CList
        LNode*  deleteByDeg();
        // get the LPoly* out of LNode*
        LPoly*  getLPoly();
        // get the data from the LPoly saved in LNode
        poly*   getPoly();
        poly*   getTerm();
        int*    getIndex();
        // test if for any list element the polynomial part of the data is equal to *p
        bool    polyTest(poly* p);
        LNode*  getNext(LNode* l);
};


/*
============================
class LList(lists of LPolys)
============================
*/
class LList {
    private:
        LNode*  first;
    public:
                LList(LPoly* lp);
                LList(poly* t,int* i,poly* p);
                ~LList();
        void    insert(LPoly* lp);
        // insertion in front of the list
        void    insert(poly* t,int* i, poly* p);
        void    insertByLabel(LPoly* lp);
        void    deleteByDeg();
        bool    polyTest(poly* p);
        LNode*  getFirst(); 
        LNode*  getNext(LNode* l);
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
class CNode (nodes for lists of CPairs)
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
        CNode*  getMinDeg();
        //CNode*  getLPoly() const; 
};


/*
============================
class CList(lists of CPairs)
============================
*/
class CList {
    private:
        CNode*  first;
    public:
                CList(CPair* c); 
                ~CList(); 
        void    insert(CPair* c);
        CNode*  getMinDeg();

};


/*
======================================
class RNode (nodes for lists of Rules)
======================================
*/
class RNode {
    private:
        Rule*   data;
        RNode*  next;
    public:
                RNode(Rule* r);
                ~RNode();
        RNode*  insert(Rule* r);
};

/*
============================
class RList (lists of Rules)
============================
*/
class RList {
    private:
        RNode*  first;
    public:
                RList(Rule* r);
                ~RList();
        void    insert(Rule* r);
};
#endif
#endif
