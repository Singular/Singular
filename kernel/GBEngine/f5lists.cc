


#include "kernel/mod2.h"

#ifdef HAVE_F5
#include "kernel/GBEngine/kutil.h"
#include "kernel/structs.h"
#ifdef HAVE_OMALLOC
#include "omalloc/omalloc.h"
#else
#include "xalloc/omalloc.h"
#endif
#include "kernel/polys.h"
#include "polys/monomials/p_polys.h"
#include "kernel/ideals.h"
#include "kernel/GBEngine/kstd1.h"
#include "kernel/GBEngine/khstd.h"
#include "polys/kbuckets.h"
#include "polys/weight.h"
#include "misc/intvec.h"
#include "kernel/polys.h"
#include "kernel/GBEngine/f5gb.h"
#include "kernel/GBEngine/f5data.h"
#include "kernel/GBEngine/f5lists.h"

/**
 * functions working on the class PNode
 */
PNode::PNode(poly p, PNode* n) {
  data  = p;
  next  = n;
}

poly PNode::getPoly() {
  return this->data;
}

PNode* PNode::getNext() {
  return this->next;
}
PNode* PNode::insert(poly p) {
  poly q = pOne();
  q = pCopy(p);
  PNode* temp = this;
  if(NULL == temp) {
    PNode* pn = new PNode(q,temp);
    return pn;
  }
  if(1 == pLmCmp(q,temp->getPoly())) {
    PNode* pn = new PNode(q,temp);
    return pn;
  }
  if(0 == pLmCmp(q,temp->getPoly())) {
    return this;
  }
  if(-1 == pLmCmp(q,temp->getPoly())) {
    while(NULL != temp->getNext() && -1 == pLmCmp(q,temp->getNext()->getPoly())) {
      temp = temp->getNext();
    }
    if(NULL == temp->getNext() || 1 == pLmCmp(q,temp->getNext()->getPoly())) {
      PNode* pn = new PNode(q,temp->getNext());
      temp->next = pn;
      return this;
    }
    if(0 == pLmCmp(q,temp->getNext()->getPoly())) {
      return this;
    }
  }
}
/*
PNode* PNode::insert(poly p) {
  PNode* pn = new PNode(p,this);
  return pn;
}
*/
/**
 * functions working on the class PList
 */
PList::PList() {
  first = NULL;
}


void PList::insert(poly p) {
  first = first->insert(p);
}


/*
PNode* PList::insert(poly p) {
  PNode* temp = first;
  PNode* pn   = new PNode(p,temp);
  first       = pn;
  return first;
}
*/
bool PList::check(poly p) {
  PNode* temp = first;
  //Print("\nCHECK: \n");
  //pWrite(p);
  //Print("-----\n");
  while(NULL != temp) {
    //pWrite(temp->getPoly());
    //pWrite(p);
    //Print("COMAPRE: %d\n",pLmEqual(p,temp->getPoly()));
    if(1 == pLmEqual(p,temp->getPoly())) {
      //Print("YES!\n");
      //pWrite(pHead(temp->getPoly()));
      //Print("YES!\n");
      return 1;
    }
    temp = temp->getNext();
  }
  //Print("NOTHING!\n");
  return 0;
}

void PList::print() {
  Print("-----LIST-----\n");
  PNode* temp = first;
  while(NULL != temp) {
    pWrite(temp->getPoly());
    temp = temp->getNext();
  }
}
/*
====================================
functions working on the class LNode
====================================
*/

// generating new list elements (labeled / classical polynomial / LNode view)
LNode::LNode() {
    data                =   NULL;
    next                =   NULL;
}
LNode::LNode(LPolyOld* lp) {
    data                =   lp;
    next                =   NULL;
}

LNode::LNode(LPolyOld* lp, LNode* l) {
//Print("HIER LNODE\n");
    data                =   lp;
    next                =   l;
}

LNode::LNode(poly t, int i, poly p, RuleOld* r) {
LPolyOld* lp           =   new LPolyOld(t,i,p,r);
data                =   lp;
next                =   NULL;
}

LNode::LNode(poly t, int i, poly p, RuleOld* r, LNode* l) {
    LPolyOld* lp           =   new LPolyOld(t,i,p,r);
    data                =   lp;
    next                =   l;
}

LNode::LNode(LNode* ln) {
    data                =   ln->getLPolyOld();
    next                =   ln->getNext();
}

LNode::~LNode() {
    //delete next;
    //Print("DELETE LNODE\n");
    delete data;
}

void LNode::deleteAll() {
    while(NULL != next) {
        //Print("%p\n",next);
        //pWrite(next->data->getPoly());
        next->deleteAll();
    }
    delete data;
}

// insert new elements to the list always at the end (labeled / classical polynomial view)
// needed for list gPrev
inline LNode* LNode::insert(LPolyOld* lp) {
    //Print("LAST GPREV: ");
    //pWrite(this->getPoly());
    if(NULL == this) {
        LNode* newElement   =   new LNode(lp,this);
        return newElement;
    }
    else {
        LNode* newElement   =   new LNode(lp, NULL);
        this->next          =   newElement;
        return newElement;
    }
}

inline LNode* LNode::insert(poly t, int i, poly p, RuleOld* r) {
    if(NULL == this) {
        LNode* newElement   =   new LNode(t,i,p,r,this);
        return newElement;
    }
    else {
        LNode* newElement   =   new LNode(t, i, p, r, NULL);
        this->next          =   newElement;
        return newElement;
    }
}

// insert new elements to the list always in front (labeled / classical polynomial view)
// needed for sPolyList
inline LNode* LNode::insertSP(LPolyOld* lp) {
    LNode* newElement   =   new LNode(lp, this);
    //Print("INSERTED IN SPOLYLIST: ");
    //pWrite(lp->getTerm());
    return newElement;
}

inline LNode* LNode::insertSP(poly t, int i, poly p, RuleOld* r) {
    LNode* newElement   =   new LNode(t, i, p, r, this);
     //Print("INSERTED IN SPOLYLIST: ");
  //pWrite(t);
return newElement;
}
// insert new elemets to the list w.r.t. increasing labels
// only used for the S-polys to be reduced (TopReduction building new S-polys with higher label)
inline LNode* LNode::insertByLabel(poly t, int i, poly p, RuleOld* r) {
    //Print("ADDING SOLYS TO THE LIST\n");
    //Print("new element: ");
    //pWrite(t);
       if(NULL == this) { // || NULL == data) {
        LNode* newElement   =   new LNode(t, i, p, r, this);
        return newElement;
    }
    else {
         //Print("tested element1: ");
    //pWrite(this->getTerm());
        if(-1 == pLmCmp(t,this->getTerm())) {
            //Print("HIERDRIN\n");
            LNode* newElement   =   new LNode(t, i, p, r, this);
            //Print("%p\n",this);
            //Print("%p\n",newElement->next);
            return newElement;
        }
        else {
            LNode* temp = this;
            while(NULL != temp->next && NULL != temp->next->data) {
                //Print("tested element: ");
                //pWrite(temp->getTerm());
 if(-1 == pLmCmp(t,temp->next->getTerm())) {
                    LNode* newElement   =   new LNode(t, i, p, r, temp->next);
                    temp->next          =   newElement;
                    return this;
                }
                else {
                    temp = temp->next;
                    //Print("%p\n",temp);
                    //Print("%p\n",temp->data);

                    //Print("%p\n",temp->next);
                }
            }
        //Print("HIER\n");
            LNode* newElement   =   new LNode(t, i, p, r, temp->next);
            temp->next          =   newElement;
            return this;
        }
    }
}

inline LNode* LNode::insertFirst(LNode* l) {
    l->next =   this;
    return l;
}

inline LNode* LNode::insertByLabel(LNode* l) {
    //Print("ADDING SOLYS TO THE LIST\n");
    //Print("new element: ");
    //pWrite(t);
       if(NULL == this) { // || NULL == data) {
        l->next =   this;
        return l;
    }
    else {
         //Print("tested element1: ");
    //pWrite(this->getTerm());
        if(-1 == pLmCmp(l->getTerm(),this->getTerm())) {
            //Print("HIERDRIN\n");
            l->next =   this;
            //Print("%p\n",this);
            //Print("%p\n",newElement->next);
            return l;
        }
        else {
            LNode* temp = this;
            while(NULL != temp->next && NULL != temp->next->data) {
                //Print("tested element: ");
                //pWrite(temp->getTerm());
                if(-1 == pLmCmp(l->getTerm(),temp->next->getTerm())) {
                    l->next     =   temp->next;
                    temp->next  =   l;
                    return this;
                }
                else {
                    temp = temp->next;
                    //Print("%p\n",temp);
                    //Print("%p\n",temp->data);

                    //Print("%p\n",temp->next);
                }
            }
        //Print("HIER\n");
            l->next     =   temp->next;
            temp->next  =   l;
            return this;
        }
    }
}

// deletes the first elements of the list with the same degree
// only used for the S-polys, which are already sorted by increasing degree by CListOld
LNode*  LNode::deleteByDeg() {
    return this;
}

// get next from current LNode
LNode* LNode::getNext() {
    return next;
}

// get the LPolyOld* out of LNode*
LPolyOld* LNode::getLPolyOld() {
    return data;
}

// get the data from the LPolyOld saved in LNode
poly LNode::getPoly() {
    return data->getPoly();
}

poly LNode::getTerm() {
    return data->getTerm();
}

int LNode::getIndex() {
    return data->getIndex();
}

RuleOld* LNode::getRuleOld() {
    return data->getRuleOld();
}

void LNode::setRuleOld(RuleOld* r) {
    return data->setRuleOld(r);
}

bool LNode::getDel() {
    return data->getDel();
}

// set the data from the LPolyOld saved in LNode
void LNode::setPoly(poly p) {
    data->setPoly(p);
}

void LNode::setTerm(poly t) {
    data->setTerm(t);
}

void LNode::setIndex(int i) {
    data->setIndex(i);
}

void LNode::setNext(LNode* l) {
    next    =   l;
}

void LNode::setDel(bool d) {
    data->setDel(d);
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

// for debugging
void LNode::print()
{
    LNode* temp = this;
    PrintS("___________________List of S-polynomials______________________:\n");
    while(NULL != temp && NULL != temp->data) {
        Print("Index: %d\n",temp->getIndex());
        PrintS("Term: ");
        pWrite(temp->getTerm());
        PrintS("Poly: ");
        pWrite(temp->getPoly());
        temp = temp->next;
    }
    PrintS("_______________________________________________________________\n");
}

int LNode::count(LNode* l) {
    int nonDel  =   0;
    LNode* temp =   l;
    while(NULL != temp) {
        if(!temp->getDel()) {
            nonDel++;
            temp    =   temp->next;
        }
        else {
            temp    =   temp->next;
        }
    }
    return nonDel;
}

/*
====================================
functions working on the class LList
====================================
*/

LList::LList() {
    first   =   last    =   NULL;;
    length  =   0;
}

LList::LList(LPolyOld* lp) {
    first   =   new LNode(lp);
    last    =   first;
    length  =   1;
}

LList::LList(poly t,int i,poly p,RuleOld* r) {
    first   =   new LNode(t,i,p,r);
    last    =   first;
    length  =   1;
}

LList::~LList() {
    LNode* temp;
    while(first) {
        temp    =   first;
        first   =   first->getNext();
        delete  temp;
        //Print("%p\n",first);
    }
}

// insertion at the end of the list, needed for gPrev
void LList::insert(LPolyOld* lp) {
    last = last->insert(lp);
    if(NULL == first) {
        first   =   last;
    }
    //Print("NEW LAST GPREV: ");
    //pWrite(last->getPoly());
    //Print("%p\n",first);
    //pWrite(first->getPoly());
    length++;
    //Print("LENGTH %d\n",length);
}

void LList::insert(poly t,int i, poly p, RuleOld* r) {
    last = last->insert(t,i,p,r);
    if(NULL == first) {
        first   =   last;
    }
    length++;
    //Print("LENGTH %d\n",length);
}

// insertion in front of the list, needed for sPolyList
void LList::insertSP(LPolyOld* lp) {
    first = first->insertSP(lp);
    length++;
    //Print("LENGTH %d\n",length);
}

void LList::insertSP(poly t,int i, poly p, RuleOld* r) {
    first = first->insertSP(t,i,p,r);
    length++;
    //Print("LENGTH %d\n",length);
}


void LList::insertByLabel(poly t, int i, poly p, RuleOld* r) {
    first = first->insertByLabel(t,i,p,r);
    length++;
    //Print("LENGTH %d\n",length);
}

void LList::insertFirst(LNode* l) {
    first = first->insertFirst(l);
    length++;
    //Print("LENGTH %d\n",length);
}

void LList::insertByLabel(LNode* l) {
    first = first->insertByLabel(l);
    length++;
    //Print("LENGTH %d\n",length);
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

LNode* LList::getLast() {
    return last;
}

int LList::getLength() {
    return length;
}

void LList::setFirst(LNode* l) {
    LNode* temp =   first;
    temp->setNext(NULL);
    first       =   l;
    length--;
}

void LList::print() {
    first->print();
}

int LList::count(LNode* l) {
    return first->count(l);
}
/*
=======================================
functions working on the class LTagNode
=======================================
*/
LTagNode::LTagNode() {
    data    =   NULL;
    next    =   NULL;
}

LTagNode::LTagNode(LNode* l) {
    data = l;
    next = NULL;
}

LTagNode::LTagNode(LNode* l, LTagNode* n) {
    data = l;
    next = n;
}

 LTagNode::~LTagNode() {
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

LTagNode* LTagNode::getNext() {
    return next;
}

// NOTE: We insert at the beginning of the list and length = i-1, where i is the actual index.
//       Thus given actual index i and idx being the index of the LPolyOld under investigation
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
LTagList::LTagList() {
    LTagNode* first =   new LTagNode();

    length          =   0;
}

LTagList::LTagList(LNode* l) {
    LTagNode* first =   new LTagNode(l);
    length          =   1;
}

LTagList::~LTagList() {
    LTagNode* temp;
    while(first) {
        temp    =   first;
        first   =   first->getNext();
        delete  temp;
        //Print("%p\n",first);
    }
}

// declaration with first as parameter in LTagNode due to sorting of LTagList
void LTagList::insert(LNode* l) {
    first   =   first->insert(l);
    length++;
}

void LTagList::setFirstCurrentIdx(LNode* l) {
    firstCurrentIdx =   l;
}

LNode* LTagList::get(int idx) {
    return first->get(idx, length);
}

LNode* LTagList::getFirst() {
    return first->getLNode();
}

LNode* LTagList::getFirstCurrentIdx() {
    return firstCurrentIdx;
}

/*
=====================================
functions working on the class TopRed
=====================================
*/

TopRed::TopRed() {
    _completed  =   NULL;
    _toDo       =   NULL;
}

TopRed::TopRed(LList* c, LList* t) {
    _completed  =   c;
    _toDo       =   t;
}

LList* TopRed::getCompleted() {
    return _completed;
}

LList* TopRed::getToDo() {
    return _toDo;
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

CNode::CNode(CPairOld* c) {
    data    =   c;
    next    =   NULL;
}

CNode::CNode(CPairOld* c, CNode* n) {
    data    =   c;
    next    =   n;
}

CNode::~CNode() {
    delete data;
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
// working only with linked, but not doubly linked lists due to memory usage we have to check the
// insertion around the first element separately from the insertion around all other elements in the list
CNode* CNode::insert(CPairOld* c) {
    if(NULL == this) {
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
            if(1 != pLmCmp(u1,ppMult_qq(this->data->getT1(), this->data->getLp1Term()))) {
                //pWrite(u1);
                //Print("Multi-Term in CritPairs Sortierung altes Element: ");
                //pWrite(ppMult_qq(this->data->getT1(),this->data->getLp1Term()));
                CNode* newElement   =   new CNode(c, this);
                return newElement;
            }
            else {
                //Print("Insert Deg\n");
                CNode* temp = this;
                while(  NULL != temp->next) {
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
                CNode* newElement   =   new CNode(c, NULL);
                temp->next          =   newElement;
                return this;
            }
        } // outer if-clause
        if( c->getDeg() > this->data->getDeg() ) { // greater degree than the first list element
            CNode* temp =   this;
            while( NULL != temp->next ) {
                if( c->getDeg() < temp->next->data->getDeg() ) {
                    CNode* newElement   =   new CNode(c, temp->next);
                    temp->next          =   newElement;
                    return this;
                }
                if( c->getDeg() == temp->next->data->getDeg() ) {
                    if(1 != pLmCmp(u1,ppMult_qq(temp->next->data->getT1(),temp->next->data->getLp1Term()))) {
                        CNode* newElement   =   new CNode(c, temp->next);
                        temp->next          =   newElement;
                        return this;
                    }
                    else {
                        temp = temp->next;
                        while(  NULL != temp->next ) {
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
                        CNode* newElement   =   new CNode(c, NULL);
                        temp->next          =   newElement;
                        return this;
                    }
                }
                if( c->getDeg() > temp->next->data->getDeg() ) {
                    temp    =   temp->next;
                }
            }
            CNode* newElement   =   new CNode(c, NULL);
            temp->next          =   newElement;
            return this;
        }
    }
}


CNode* CNode::insertWithoutSort(CPairOld* cp) {
    CNode* newElement = new CNode(cp);
    newElement->next  = this;
    return newElement;
}


// get the first elements from CListOld which by the above sorting have minimal degree
CNode* CNode::getMinDeg() {
    CNode* temp = this;
    while(NULL != temp) {
        while(NULL != temp->next && temp->next->data->getDeg() == this->data->getDeg()) {
            temp = temp->next;
        }
        CNode* returnCNode  =   temp->next;
        // every CListOld should end with a (NULL,NULL) element for a similar behaviour
        // using termination conditions throughout the algorithm
        temp->next          =   NULL;
        return returnCNode;
    }
    return NULL;
}

CPairOld* CNode::getData() {
    return data;
}

CNode* CNode::getNext() {
    return next;
}

LPolyOld* CNode::getAdLp1() {
    return this->data->getAdLp1();
}

LPolyOld* CNode::getAdLp2() {
    return this->data->getAdLp2();
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

poly* CNode::getAdT1() {
    return this->data->getAdT1();
}

poly CNode::getT2() {
    return this->data->getT2();
}

poly* CNode::getAdT2() {
    return this->data->getAdT2();
}

bool CNode::getDel() {
  return data->getDel();
}

RuleOld* CNode::getTestedRuleOld() {
    return this->data->getTestedRuleOld();
}

// for debugging
void CNode::print()
{
    CNode* temp = this;
    PrintS("___________________List of critical pairs______________________:\n");
    while(NULL != temp)
    {
        pWrite(ppMult_qq(temp->getT1(),temp->getLp1Term()));
        Print("LP1 Index: %d\n",temp->getLp1Index());
        PrintS("T1: ");
        pWrite(temp->getT1());
        Print("%p\n",temp->getT1());
        PrintS("LP1 Term: ");
        pWrite(temp->getLp1Term());
        PrintS("LP1 Poly: ");
        pWrite(temp->getLp1Poly());
        Print("LP2 Index: %d\n",temp->getLp2Index());
        PrintS("T2: ");
        pWrite(temp->getT2());
        Print("%p\n",temp->getT2());
        PrintS("LP2 Term: ");
        pWrite(temp->getLp2Term());
        PrintS("LP2 Poly: ");
        pWrite(temp->getLp2Poly());
        PrintLn();
        temp = temp->next;
    }
}

/*
====================================
functions working on the class CListOld
====================================
*/
// for initialization of CListOlds, last element alwas has data=NULL and next=NULL
CListOld::CListOld() {
    first   =   NULL;
}

CListOld::CListOld(CPairOld* c) {
    first   =   new CNode(c);
}

CListOld::~CListOld() {
    CNode* temp;
    while(NULL != first) {
        temp    =   first;
        first   =   first->getNext();
        delete  temp;
    }
}

// insert sorts the critical pairs firstly by increasing total degree, secondly by increasing label
// note: as all critical pairs have the same index here, the second sort is done on the terms of the labels
void CListOld::insert(CPairOld* c) {
    first = first->insert(c);
}

void CListOld::insertWithoutSort(CPairOld* c) {
    first = first->insertWithoutSort(c);
}

CNode* CListOld::getFirst() {
    return first;
}

// get the first elements from CListOld which by the above sorting have minimal degree
// returns the pointer on the first element of those
CNode* CListOld::getMinDeg() {
    CNode* temp     =   first;
    first           =   first->getMinDeg();
    return temp;
}

void CListOld::print() {
    first->print();
}

/*
====================================
functions working on the class RNode
====================================
*/
RNode::RNode() {
    //Print("HIER RNODE CONSTRUCTOR\n");
    data    =   NULL;
    next    =   NULL;
}

RNode::RNode(RuleOld* r) {
    data    =   r;
    next    =   NULL;
}

RNode::~RNode() {
    //Print("DELETE RuleOld\n");
    delete  data;
}

RNode* RNode::insert(RuleOld* r) {
    RNode* newElement   =   new RNode(r);
    newElement->next    =   this;
    return newElement;
}

RNode* RNode::insert(int i, poly t) {
    //Print("IN INSERT: ");
    //pWrite(t);
    RuleOld*   r           =   new RuleOld(i,t);
    //Print("ADDRESS OF RuleOld: %p\n",r);
    RNode* newElement   =   new RNode(r);
    //Print("ADDRESS OF RNODE: %p\n",newElement);
    //Print("ADDRESS OF RNODE DATA: %p\n",newElement->getRuleOld());
    newElement->next    =   this;
    return newElement;
}


RNode* RNode::insertOrdered(RuleOld* r) {
    RNode* newElement   =   new RNode(r);
    RNode* temp         =   this;
    if(NULL == temp) {
        newElement->next =   temp;
        return newElement;
    }
    if(1 == pLmCmp(newElement->getRuleOldTerm(),temp->getRuleOldTerm())) {
        newElement->next =   temp;
        return newElement;
    }
    else {
        while(NULL != temp && 1 ==  pLmCmp(temp->getRuleOldTerm(),newElement->getRuleOldTerm())) {
            temp    =   temp->getNext();
        }
        newElement->next =   temp;
        return this;
    }
}


RNode* RNode::getNext() {
    return next;
}

RuleOld* RNode::getRuleOld() {
    return data;
}

int RNode::getRuleOldIndex() {
    return data->getIndex();
}

poly RNode::getRuleOldTerm() {
    return data->getTerm();
}

void RNode::print() {
    RNode* temp  =   this;
    while(NULL != temp) {
        pWrite(temp->getRuleOldTerm());
        Print("%d\n\n",temp->getRuleOldIndex());
        temp    =   temp->getNext();
    }
}

/*
====================================
functions working on the class RList
====================================
*/
RList::RList() {
    first = NULL;
}

RList::RList(RuleOld* r) {
    first = new RNode(r);
}

RList::~RList() {
    RNode* temp;
    //Print("%p\n",first);
    while(first) {
        temp    =   first;
        first   =   first->getNext();
        //Print("1 %p\n",first);
        //if(first) {
            //Print("1' %p\n",first->getRuleOld());
            //Print("2 %p\n",first->getNext());
            //Print("3 %p\n",first->getNext()->getRuleOld());
            //Print("3 %p\n",first->getNext()->getRuleOldTerm());
        //}
        delete  temp;
    }
    //Print("FERTIG\n");
}

void RList::insert(int i, poly t) {
    first = first->insert(i,t);
}

void RList::insert(RuleOld* r) {
    first = first->insert(r);
}

void RList::insertOrdered(RuleOld* r) {
    first   =   first->insertOrdered(r);
}

RNode* RList::getFirst() {
    return first;
}

RuleOld* RList::getRuleOld() {
    return this->getRuleOld();
}

void RList::print() {
    first->print();
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
    delete data;
}

// declaration with first as parameter due to sorting of RTagList
RTagNode* RTagNode::insert(RNode* r) {
    //Print("Hier1\n");
    RTagNode* newElement  = new RTagNode(r, this);
    //Print("Hier2\n");
    return newElement;
}

RNode* RTagNode::getRNode() {
    //Print("%p\n", this);
    //Print("%p\n",this->data);
    return this->data;
}

RTagNode* RTagNode::getNext() {
    return next;
}

// NOTE: We insert at the beginning of the list and length = i-1, where i is the actual index.
//       Thus given actual index i and idx being the index of the LPolyOld under investigation
//       the element on position length-idx+1 is the right one
RNode* RTagNode::get(int idx, int length) {
    if(idx==1 || idx==0) {
        // NOTE: We set this NULL as putting it the last element in the list, i.e. the element having
        //       RNode* = NULL would cost lots of iterations at each step of F5inc, with increasing
        //       length of the list this should be prevented
        return NULL;
    }
    else {
        int j;
        RTagNode* temp = this;
    //Print("\n\nHIER IN GET IDX\n");
    //Print("FOR LOOP: %d\n",length-idx+1);
    for(j=1; j<=length-idx+1; j++) {
            temp = temp->next;
        }
        return temp->data;
    }
}

void RTagNode::set(RNode* r) {
    this->data  =   r;
}


void RTagNode::print() {
    RTagNode* temp  =   this;
    if(NULL != temp && NULL != temp->getRNode()) {
        Print("1. element: %d,  ",getRNode()->getRuleOld()->getIndex());
        pWrite(getRNode()->getRuleOld()->getTerm());
        temp    =   temp->next;
        int i   =   2;
        while(NULL != temp->getRNode() && NULL != temp) {
            Print("%d. element: %d,  ",i,getRNode()->getRuleOld()->getIndex());
            pWrite(getRNode()->getRuleOld()->getTerm());
            temp    =   temp->next;
            i++;
        }
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

RTagList::~RTagList() {
    RTagNode* temp;
    while(first) {
        temp    =   first;
        first   =   first->getNext();
        delete  temp;
    }
}

// declaration with first as parameter in LTagNode due to sorting of LTagList
void RTagList::insert(RNode* r) {
    first = first->insert(r);
    //Print("LENGTH:%d\n",length);
    length = length +1;
    //Print("LENGTH:%d\n",length);
}

RNode* RTagList::getFirst() {
    return first->getRNode();
}

RNode* RTagList::get(int idx) {
    return first->get(idx, length);
}

void RTagList::setFirst(RNode* r) {
    first->set(r);
}

void RTagList::print() {
    first->print();
}

int RTagList::getLength() {
    return length;
}
#endif
