/* emacs edit mode for this file is -*- C++ -*- */
/* $Id: ftmpl_list.h,v 1.8 2005-01-13 15:10:23 Singular Exp $ */

#ifndef INCL_LIST_H
#define INCL_LIST_H

#include <factoryconf.h>

#ifndef NOSTREAMIO
#include <iostream.h>
#endif /* NOSTREAMIO */

template <class T>
class ListIterator;

template <class T>
class List;

#ifndef NOSTREAMIO
template <class T>
std::ostream& operator<< ( std::ostream &, const List<T> &);
#endif

template <class T>
class ListItem
{
private:
    ListItem * next;
    ListItem * prev;
    T * item;
public:
    ListItem( const ListItem<T>& );
    ListItem( const T&, ListItem<T>*, ListItem<T>* );
    ListItem( T* , ListItem<T>* , ListItem<T>* );
    ~ListItem();
    ListItem<T>& operator= ( const ListItem<T>& );
    ListItem<T>* getNext();
    ListItem<T>* getPrev();
    T& getItem();
#ifndef NOSTREAMIO
    void print ( ostream& );
#endif /* NOSTREAMIO */
    friend class ListIterator<T>;
    friend class List<T>;
};

template <class T>
class List
{
private:
    ListItem<T> *first;
    ListItem<T> *last;
    int _length;
public:
    List();
    List( const List<T>& );
    List( const T& );
    ~List();
    List<T>& operator= ( const List<T>& );
    void insert ( const T& );
    void insert ( const T&, int (*cmpf)( const T&, const T& ) );
    void insert ( const T&, int (*cmpf)( const T&, const T& ), void (*insf)( T&, const T& ) );
    void append ( const T& );
    int isEmpty() const;
    int length() const;
    T getFirst() const;
    void removeFirst();
    T getLast() const;
    void removeLast();
    void sort ( int (*) ( const T&, const T& ) );
#ifndef NOSTREAMIO
    void print ( ostream & ) const;
    friend ostream& operator<< <T>( ostream & os, const List<T> & l );
#endif /* NOSTREAMIO */
    friend class ListIterator<T>;
};

#ifndef NOSTREAMIO
template <class T>
ostream& operator<< ( ostream & os, const List<T> & l );
#endif /* NOSTREAMIO */

template <class T>
class ListIterator {
private:
    List<T> *theList;
    ListItem<T> *current;
public:
    ListIterator();
    ListIterator( const ListIterator<T>& );
    ListIterator( const List<T>& );
    ~ListIterator();
    ListIterator<T>& operator = ( const ListIterator<T>& );
    ListIterator<T>& operator = ( const List<T>& );
    T& getItem() const;
    int hasItem();
    void operator++();
    void operator--();
    void operator++( int );
    void operator--( int );
    void firstItem();
    void lastItem();
    void insert( const T& );
    void append( const T& );
    void remove( int moveright );
};

template <class T>
List<T> Union ( const List<T>&, const List<T>& );

template <class T>
List<T> Difference ( const List<T>&, const List<T>& );

template <class T>
List<T> Union ( const List<T>&, const List<T>&, int (*cmpf)( const T&, const T& ), void (*insf)( T&, const T& ) );

template <class T>
T prod ( const List<T>& );

#endif /* ! INCL_LIST_H */
