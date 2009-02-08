/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id: f5lists.h,v 1.7 2009-02-08 19:17:54 ederc Exp $ */
/*
* ABSTRACT: list interface
*/
#include "f5data.h"
#ifndef F5LISTS_HEADER
#define F5LISTS_HEADER

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
class LRed;
class LTagNode;
class LTagList;
class CNode;
class CList;
class RList;
class RNode;
class RTagNode;
class RTagList;


/*
=======================================
class LNode (nodes for lists of LPolys)
=======================================
*/
class LNode {
    private:
        LPoly* data;
        LNode* next;
    public:
        // generating new list elements from the labeled / classical polynomial view
                LNode();
                LNode(LPoly* lp);
                LNode(LPoly* lp, LNode* l);
                LNode(poly t, int i, poly p);
                LNode(poly t, int i, poly p, LNode* l);
                LNode(LNode* ln);
                ~LNode();
        // insert new elements to the list in first place from the labeled / classical polynomial view
        LNode*  insert(LPoly* lp);
        LNode*  insert(poly t, int i, poly p);
        // insert new elements to the list with resp. to increasing labels
        // only used for the S-polys to be reduced (TopReduction building new S-polys with higher label)
        LNode*  insertByLabel(poly t, int i, poly p);
        // deletes the first elements of the list with the same degree
        // get next from current LNode
        LNode*  getNext();
        // only used for the S-polys, which are already sorted by increasing degree by CList
        LNode*  deleteByDeg();
        // get the LPoly* out of LNode*
        LPoly*  getLPoly();
        // get the data from the LPoly saved in LNode
        poly    getPoly();
        poly    getTerm();
        int     getIndex(); 
        // set the data from the LPoly saved in LNode
        void    setPoly(poly p);
        void    setTerm(poly t);
        void    setIndex(int i);
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
        int     length;
    public:
                LList();
                LList(LPoly* lp);
                LList(poly t,int i,poly p);
                ~LList();
        void    insert(LPoly* lp);
        // insertion in front of the list
        void    insert(poly t,int i, poly p);
        void    insertByLabel(poly t, int i, poly p);
        void    deleteByDeg();
        bool    polyTest(poly* p);
        LNode*  getFirst(); 
        LNode*  getNext(LNode* l);
        int     getLength();
        void    setFirst(LNode* l);
};


/*
=========================================
class LRed (nodes for lists of Reductors)
=========================================
*/
class LRed {
    private:
        LPoly* data;
        LNode* gPrevRedCheck;
        LNode* completedRedCheck;
    public:
        // generating new list elements from the labeled / classical polynomial view
                LRed();
                LRed(poly t, int i, poly p, LNode* g, LNode* c);
                ~LRed();
        // get the LPoly* out of LNode*
        LPoly*  getLPoly();
        // get the last reductor tested in subalgorithm findReductor() in f5gb.cc 
        LNode*  getGPrevRedCheck();
        LNode*  getCompletedRedCheck();
        // get data from the labeled polynomial 
        poly    getPoly();
        poly    getTerm();
        int     getIndex(); 
        // set the data from the LPoly saved in LNode
        void    setPoly(poly p);
        void    setTerm(poly t);
        void    setIndex(int i);
};


/*
==============================================
class LtagNode (nodes for lists of LPoly tags)
==============================================
*/
class LTagNode {
    private:
        LNode*      data;
        LTagNode*   next;
    public:
        LTagNode(LNode* l);
        LTagNode(LNode* l, LTagNode* n);
        ~LTagNode();
        // declaration with first as parameter due to sorting of LTagList
        LTagNode*   insert(LNode* l);
        LNode*      getLNode();
        LNode*      get(int i, int length);
};


/*
=========================================================================
class LTagList(lists of LPoly tags, i.e. first elements of a given index)
=========================================================================
*/
class LTagList {
    private:
        LTagNode*   first;
        int         length;
    public:
                LTagList(LNode* l);
                ~LTagList();
        // declaration with first as parameter in LTagNode due to sorting of LTagList
        void    insert(LNode* l);
        LNode*  get(int idx);
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
                CNode();
                CNode(CPair* c);
                CNode(CPair* c, CNode* n);
                ~CNode(); 
        CNode*  insert(CPair* c, CNode* last); 
        CNode*  getMinDeg();
        CPair*  getData();
        CNode*  getNext();
        LPoly*  getAdLp1();
        LPoly*  getAdLp2();
        poly    getLp1Poly();
        poly    getLp2Poly();
        poly    getLp1Term();
        poly    getLp2Term();
        poly    getT1(); 
        poly*   getAdT1();  
        poly    getT2(); 
        poly*   getAdT2();  
        int     getLp1Index();
        int     getLp2Index();
        Rule*   getLastRuleTested();
        void    print();
};


/*
============================
class CList(lists of CPairs)
============================
*/
class CList {
    private:
        CNode*  first;
        // last alway has data=NULL and next=NULL, for initialization purposes used
        CNode*  last;
    public:
                // for initialization of CLists, last element alwas has data=NULL and next=NULL
                CList(); 
                CList(CPair* c); 
                ~CList(); 
        CNode*  getFirst();
        void    insert(CPair* c);
        CNode*  getMinDeg();
        void    print();
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
                RNode();
                RNode(Rule* r);
                ~RNode();
        RNode*  insert(Rule* r);
        RNode*  insert(int i, poly t, LPoly* l);
        RNode*  getNext();
        Rule*   getRule();
        int     getRuleIndex();
        poly    getRuleTerm();
        LPoly*  getRuleOrigin();
};

/*
============================
class RList (lists of Rules)
============================
*/
class RList {
    private:
        RNode*  first;
        // last alway has data=NULL and next=NULL, for initialization purposes used
        RNode*  last;
    public:
                RList();
                RList(Rule* r);
                ~RList();
        void    insert(Rule* r);
        void    insert(int i, poly t, LPoly* l);
        RNode*  getFirst();
        Rule*   getRule();
};



/*
=============================================
class RtagNode (nodes for lists of Rule tags)
=============================================
*/
class RTagNode {
    private:
        RNode*      data;
        RTagNode*   next;
    public:
                    RTagNode();
                    RTagNode(RNode* r);
                    RTagNode(RNode* r, RTagNode* n);
                    ~RTagNode();
        // declaration with first as parameter due to sorting of LTagList
        RTagNode*   insert(RNode* r);
        RNode*      getRNode();
        RNode*      get(int idx, int length);
        void        set(RNode*);
};


/*
========================================================================
class RTagList(lists of Rule tags, i.e. first elements of a given index)
========================================================================
*/
class RTagList {
    private:
        RTagNode*   first;
        int         length;
    public:
                RTagList();
                RTagList(RNode* r);
                ~RTagList();
        // declaration with first as parameter in LTagNode due to sorting of LTagList
        void    insert(RNode* r);
        void    insert(int i, poly* t, LPoly* l);
        RNode*  getFirst();
        RNode*  get(int idx);
        void    setFirst(RNode* r);
};
#endif
#endif
