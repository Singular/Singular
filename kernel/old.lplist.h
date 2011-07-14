/****************************************
*  Computer Algebra System SINGULAR     *
****************************************/
/* $Id$ */
/*
* ABSTRACT: list interface
*/
#include <kernel/lpolynomial.h>
#ifndef LPLIST_HEADER
#define LPLIST_HEADER

#ifdef HAVE_F5





/*
=======================
=======================
linked lists for LPolys
=======================
=======================
*/







/*
=================
classes in a list
=================
*/
class Node;
class FirstNode;
class LastNode;
class GenNode;
class LpList;

/*
===================
abstract Node class
===================
*/
class Node {
    public:
        Node() {};
        virtual         ~Node() {};
        // the following two functions have to be redefined in FirstNode, LastNode, and GenNode
        virtual Node*   insert(LPoly* lp) = 0;
        virtual void    get() = 0;
};

/*
========================
subclass GenNode of Node
========================
*/
class GenNode : public Node {
    private:
        LPoly*  data;
        Node*   next;
    public:
        GenNode(LPoly* d, Node* n) : data(d), next(n){};
        ~GenNode() {
            delete next;
            delete data;
        }
        Node* insert(LPoly* d);
        void get();
};


/*
========================
subclass EndNode of Node
========================
*/
class EndNode : public Node {
    public:
        EndNode() {};
        ~EndNode() {};
        Node* insert(LPoly* d);
        void get();
};


/*
==========================
subclass FirstNode of Node
==========================
*/
class FirstNode : public Node {
    private:
        Node* next;
    public:
        FirstNode() {
            next = new EndNode;
        }
        ~FirstNode() {};
        Node* insert(LPoly* d);
        void get();
};


/*
============
class LpList
============
*/
class LpList {
    private:
        FirstNode *start;
    public:
        LpList() {
            start = new FirstNode;
        }
        ~LpList() {
            delete start;
        }
        void insert(LPoly* d);
        void get();
};



#endif
#endif
 
