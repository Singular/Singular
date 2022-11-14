#ifndef F5LISTS_HEADER
#define F5LISTS_HEADER
/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/*
* ABSTRACT: list interface
*/
#include "kernel/GBEngine/f5data.h"

#ifdef HAVE_F5
/*
============================
============================
classes for lists used in F5
============================
============================
*/
class PNode;
class PList;
class LNode;
class LList;
class LTagNode;
class LTagList;
class CNode;
class CListOld;
class RList;
class RNode;
class RTagNode;
class RTagList;


/**
 * class PNode of nodes of polynomials
 */
class PNode {
  private:
    poly   data;
    PNode*  next;
  public:
    PNode(poly p, PNode* n);
    poly getPoly();
    PNode* getNext();
    PNode* insert(poly p);
};

/**
 * class PList of lists of PNodes
 */
class PList {
  private:
    PNode* first;
  public:
    PList();
    void insert(poly p);
    bool check(poly p);
    void print();
};

/*
=======================================
class LNode (nodes for lists of LPolyOlds)
=======================================
*/
class LNode {
    private:
        LPolyOld*  data;
        LNode*  next;
    public:
        // generating new list elements from the labeled / classical polynomial view
                LNode();
                LNode(LPolyOld* lp);
                LNode(LPolyOld* lp, LNode* l);
                LNode(poly t, int i, poly p, RuleOld* r=NULL);
                LNode(poly t, int i, poly p, RuleOld* r, LNode* l);
                LNode(LNode* ln);
                ~LNode();
        void    deleteAll();
        // insert new elements to the list at the end from the labeled / classical polynomial view
        // needed for gPrev
        LNode*  insert(LPolyOld* lp);
        LNode*  insert(poly t, int i, poly p, RuleOld* r);
        LNode*  insertByDeg(LPolyOld* lp);
        // insert new elements to the list in front from the labeled / classical polynomial view
        // needed for sPolyList
        LNode*  insertSP(LPolyOld* lp);
        LNode*  insertSP(poly t, int i, poly p, RuleOld* r);
        // insert new elements to the list with resp. to increasing labels
        // only used for the S-polys to be reduced (TopReduction building new S-polys with higher label)
        LNode*  insertByLabel(poly t, int i, poly p, RuleOld* r);
        LNode*  insertByLabel(LNode* l);
        LNode*  insertFirst(LNode* l);
        // deletes the first elements of the list with the same degree
        // get next & prev from current LNode
        LNode*  getNext();
        LNode*  getPrev();
        // only used for the S-polys, which are already sorted by increasing degree by CListOld
        LNode*  deleteByDeg();
        // get the LPolyOld* out of LNode*
        LPolyOld*  getLPolyOld();
        // get the data from the LPolyOld saved in LNode
        poly    getPoly();
        poly    getTerm();
        int     getIndex();
        RuleOld*   getRuleOld();
        bool    getDel();
        // set the data from the LPolyOld saved in LNode
        void    setPoly(poly p);
        void    setTerm(poly t);
        void    setIndex(int i);
        void    setNext(LNode* l);
        void    setRuleOld(RuleOld* r);
        void    setDel(bool d);
        // test if for any list element the polynomial part of the data is equal to *p
        bool    polyTest(poly* p);
        LNode*  getNext(LNode* l);
        void    print();
        int     count(LNode* l);
};


/*
============================
class LList(lists of LPolyOlds)
============================
*/
class LList {
    private:
        LNode*  first;
        LNode*  last;
        int     length;
    public:
                LList();
                LList(LPolyOld* lp);
                LList(poly t,int i,poly p, RuleOld* r = NULL);
                ~LList();
        // insertion at the end of the list
        // needed for gPrev
        void    insert(LPolyOld* lp);
        void    insert(poly t,int i, poly p, RuleOld* r = NULL);
        void    insertByDeg(LPolyOld* lp);
        // insertion in front of the list
        // needed for sPolyList
        void    insertSP(LPolyOld* lp);
        void    insertSP(poly t,int i, poly p, RuleOld* r = NULL);
        void    insertByLabel(poly t, int i, poly p, RuleOld* r = NULL);
        void    insertByLabel(LNode* l);
        void    insertFirst(LNode* l);
        void    deleteByDeg();
        bool    polyTest(poly* p);
        LNode*  getFirst();
        LNode*  getLast();
        int     getLength();
        void    setFirst(LNode* l);
        void    print();
        int     count(LNode* l);
};



/*
==============================================
class LtagNode (nodes for lists of LPolyOld tags)
==============================================
*/
class LTagNode {
    private:
        LNode*      data;
        LTagNode*   next;
    public:
        LTagNode();
        LTagNode(LNode* l);
        LTagNode(LNode* l, LTagNode* n);
        ~LTagNode();
        // declaration with first as parameter due to sorting of LTagList
        LTagNode*   insert(LNode* l);
        LNode*      getLNode();
        LTagNode*   getNext();
        LNode*      get(int i, int length);
};


/*
=========================================================================
class LTagList(lists of LPolyOld tags, i.e. first elements of a given index)
=========================================================================
*/
class LTagList {
    private:
        LTagNode*   first;
        LNode*      firstCurrentIdx;
        int         length;
    public:
                LTagList();
                LTagList(LNode* l);
                ~LTagList();
        // declaration with first as parameter in LTagNode due to sorting of LTagList
        void    insert(LNode* l);
        void    setFirstCurrentIdx(LNode* l);
        LNode*  get(int idx);
        LNode*  getFirst();
        LNode*  getFirstCurrentIdx();
};

LNode*  getGPrevRedCheck();
LNode*  getcompletedRedCheck();


/*
======================================================================================
class TopRed(return values of subalgorithm TopRed in f5gb.cc), i.e. the first elements
             of the lists LList* completed & LList* sPolyList
======================================================================================
*/
class TopRed {
    private:
        LList*  _completed;
        LList*  _toDo;
    public:
                TopRed();
                TopRed(LList* c, LList* t);
        LList*  getCompleted();
        LList*  getToDo();
};


/*
=======================================
class CNode (nodes for lists of CPairOlds)
=======================================
*/
class CNode {
    private:
        CPairOld* data;
        CNode* next;
    public:
                CNode();
                CNode(CPairOld* c);
                CNode(CPairOld* c, CNode* n);
                ~CNode();
        CNode*  insert(CPairOld* c);
        CNode*  insertWithoutSort(CPairOld* cp);
        CNode*  getMinDeg();
        CPairOld*  getData();
        CNode*  getNext();
        LPolyOld*  getAdLp1();
        LPolyOld*  getAdLp2();
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
        bool    getDel();
        RuleOld*   getTestedRuleOld();
        void    print();
};


/*
============================
class CListOld(lists of CPairOlds)
============================
*/
class CListOld {
    private:
        CNode*  first;
    public:
                // for initialization of CListOlds, last element always has data=NULL and next=NULL
                CListOld();
                CListOld(CPairOld* c);
                ~CListOld();
        CNode*  getFirst();
        void    insert(CPairOld* c);
        void    insertWithoutSort(CPairOld* c);
        CNode*  getMinDeg();
        void    print();
};


/*
======================================
class RNode (nodes for lists of RuleOlds)
======================================
*/
class RNode {
    private:
        RuleOld*   data;
        RNode*  next;
    public:
                RNode();
                RNode(RuleOld* r);
                ~RNode();
        RNode*  insert(RuleOld* r);
        RNode*  insert(int i, poly t);
        RNode*  insertOrdered(RuleOld* r);
        RNode*  getNext();
        RuleOld*   getRuleOld();
        int     getRuleOldIndex();
        poly    getRuleOldTerm();
        void    print();
};

/*
============================
class RList (lists of RuleOlds)
============================
*/
class RList {
    private:
        RNode*  first;
        // last always has data=NULL and next=NULL, for initialization purposes used
        RNode*  last;
    public:
                RList();
                RList(RuleOld* r);
                ~RList();
        void    insert(RuleOld* r);
        void    insert(int i, poly t);
        void    insertOrdered(RuleOld* r);
        RNode*  getFirst();
        RuleOld*   getRuleOld();
        void    print();
};



/*
=============================================
class RtagNode (nodes for lists of RuleOld tags)
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
        RTagNode*   getNext();
        RNode*      get(int idx, int length);
        void        set(RNode*);
        void        print();
};


/*
========================================================================
class RTagList(lists of RuleOld tags, i.e. first elements of a given index)
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
        RNode*  getFirst();
        RNode*  get(int idx);
        void    setFirst(RNode* r);
        void    print();
        int     getLength();
};
#endif
#endif
