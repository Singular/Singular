/* emacs edit mode for this file is -*- C++ -*- */

#include "factory/templates/ftmpl_list.h"

template <class T>
ListItem<T>::ListItem( const ListItem<T>& i )
{
    next = i.next;
    prev = i.prev;
    item = i.item;
}


template <class T>
ListItem<T>::ListItem( const T& t, ListItem<T>* n, ListItem<T>* p )
{
    next = n;
    prev = p;
    item = new T( t );
}


template <class T>
ListItem<T>::ListItem( T* t, ListItem<T>* n, ListItem<T>* p )
{
    next = n;
    prev = p;
    item = t;
}


template <class T>
ListItem<T>::~ListItem()
{
    delete item;
}


template <class T>
ListItem<T>& ListItem<T>::operator=( const ListItem<T>& i )
{
    if ( this != &i )
    {
        next = i.next;
        prev = i.prev;
        item = i.item;
    }
    return *this;
}


template <class T>
ListItem<T>* ListItem<T>::getNext()
{
    return next;
}


template <class T>
ListItem<T>* ListItem<T>::getPrev()
{
    return prev;
}


template <class T>
T& ListItem<T>::getItem()
{
    return *item;
}

#ifndef NOSTREAMIO
template <class T>
void ListItem<T>::print( OSTREAM & os )
{
    if ( item )
        os << *item;
    else
        os << "(no item)";
}
#endif /* NOSTREAMIO */



template <class T>
List<T>::List()
{
    first = last = 0;
    _length = 0;
}


template <class T>
List<T>::List( const List<T>& l )
{
    ListItem<T>* cur = l.last;
    if ( cur )
    {
        first = new ListItem<T>( *(cur->item), 0, 0 );
        last = first;
        cur = cur->prev;
        while ( cur )
        {
            first = new ListItem<T>( *(cur->item), first, 0 );
            first->next->prev = first;
            cur = cur->prev;
        }
        _length = l._length;
    }
    else
    {
        first = last = 0;
        _length = 0;
    }
}


template <class T>
List<T>::List( const T& t )
{
    first = last = new ListItem<T>( t, 0, 0 );
    _length = 1;
}


template <class T>
List<T>::~List()
{
    ListItem<T> *dummy;
    while ( first )
    {
        dummy = first;
        first = first->next;
        delete dummy;
    }
}


template <class T>
List<T>& List<T>::operator=( const List<T>& l )
{
    if ( this != &l )
    {
        ListItem<T> *dummy;
        while ( first )
        {
            dummy = first;
            first = first->next;
            delete dummy;
        }
        ListItem<T>* cur = l.last;
        if ( cur )
        {
            first = new ListItem<T>( *(cur->item), 0, 0 );
            last = first;
            cur = cur->prev;
            while ( cur )
            {
                first = new ListItem<T>( *(cur->item), first, 0 );
                first->next->prev = first;
                cur = cur->prev;
            }
            _length = l._length;
        }
        else
        {
            first = last = 0;
            _length = 0;
        }
        _length = l._length;
    }
    return *this;
}

template <class T>
int operator== ( const List<T>& l1, const List<T>& l2 )
{
    if (l1.length() != l2.length())
      return 0;
    ListIterator<T> iter2= l2;
    for (ListIterator<T> iter1= l1; iter1.hasItem(); iter1++)
    {
      if (!(iter1.getItem() == iter2.getItem()))
        return 0;
      iter2++;
    }

    return 1;
}


template <class T>
void List<T>::insert ( const T& t )
{
    first = new ListItem<T>( t, first, 0 );
    if ( last )
        first->next->prev = first;
    last = ( last ) ? last : first;
    _length++;
}


template <class T>
void List<T>::insert ( const T& t, int (*cmpf)( const T&, const T& ) )
{
    if ( ! first || cmpf( *first->item, t ) > 0 )
        insert( t );
    else if ( cmpf( *last->item, t ) < 0 )
        append( t );
    else
    {
        ListItem<T> * cursor = first;
        int c;
        while ( (c = cmpf( *cursor->item, t )) < 0 )
            cursor = cursor->next;
        if ( c == 0 )
            *cursor->item = t;
        else
        {
            cursor = cursor->prev;
            cursor->next = new ListItem<T>( t, cursor->next, cursor );
            cursor->next->next->prev = cursor->next;
            _length++;
        }
    }
}


template <class T>
void List<T>::insert ( const T& t, int (*cmpf)( const T&, const T& ), void (*insf)( T&, const T& ) )
{
    if ( ! first || cmpf( *first->item, t ) > 0 )
        insert( t );
    else if ( cmpf( *last->item, t ) < 0 )
        append( t );
    else
    {
        ListItem<T> * cursor = first;
        int c;
        while ( (c = cmpf( *cursor->item, t )) < 0 )
            cursor = cursor->next;
        if ( c == 0 )
            insf( *cursor->item, t );
        else
        {
            cursor = cursor->prev;
            cursor->next = new ListItem<T>( t, cursor->next, cursor );
            cursor->next->next->prev = cursor->next;
            _length++;
        }
    }
}


template <class T>
void List<T>::append ( const T& t )
{
    last = new ListItem<T>( t, 0, last );
    if ( first )
        last->prev->next = last;
    first = ( first ) ? first : last;
    _length++;
}


template <class T>
int List<T>::isEmpty() const
{
    return ( first == 0 );
}

template <class T>
int List<T>::length() const
{
    return _length;
}

template <class T>
T List<T>::getFirst() const
{
    ASSERT( first, "List: no item available" );
    return first->getItem();
}


template <class T>
void List<T>::removeFirst()
{
    if ( first )
    {
        _length--;
        if ( first == last )
        {
            delete first;
            first = last = 0;
        }
        else
        {
            ListItem<T> *dummy = first;
            first->next->prev = 0;
            first = first->next;
            delete dummy;
        }
    }
}


template <class T>
T List<T>::getLast() const
{
    ASSERT( first, "List: no item available" );
    return last->getItem();
}


template <class T>
void List<T>::removeLast()
{
    if ( last )
    {
        _length--;
        if ( first == last )
        {
            delete last;
            first = last = 0;
        }
        else
        {
            ListItem<T> *dummy = last;
            last->prev->next = 0;
            last = last->prev;
            delete dummy;
        }
    }
}


template <class T>
void List<T>::sort( int (*swapit) ( const T&, const T& ) )
{
    if ( first != last )
    {
        int swap;
        do
        {
            swap = 0;
            ListItem<T> *cur = first;
            while ( cur->next )
            {
                if ( swapit( *(cur->item), *(cur->next->item) ) )
                {
                    T* dummy = cur->item;
                    cur->item = cur->next->item;
                    cur->next->item = dummy;
                    swap = 1;
                }
                cur = cur->next;
            }
        } while (swap);
    }
}


#ifndef NOSTREAMIO
template <class T>
void List<T>::print ( OSTREAM & os ) const
{
    ListItem<T> *cur = first;
    os << "( ";
    while ( cur )
    {
        cur->print( os );
        if ( (cur = cur->getNext()) )
            os << ", ";
    }
    os << " )";
}
#endif /* NOSTREAMIO */


template <class T>
ListIterator<T>::ListIterator()
{
    theList = 0;
    current = 0;
}


template <class T>
ListIterator<T>::ListIterator ( const ListIterator<T> & i )
{
    theList = i.theList;
    current = i.current;
}


template <class T>
ListIterator<T>::ListIterator ( const List<T> & l )
{
    theList = (List<T>*)&l;
    current = l.first;
}


template <class T>
ListIterator<T>::~ListIterator() { }


template <class T>
ListIterator<T>& ListIterator<T>::operator= ( const ListIterator<T> & I )
{
    if ( this != &I )
    {
        theList = I.theList;
        current = I.current;
    }
    return *this;
}


template <class T>
ListIterator<T>& ListIterator<T>::operator= ( const List<T> & l )
{
    theList = (List<T>*)&l;
    current = l.first;
    return *this;
}


template <class T>
T& ListIterator<T>::getItem () const
{
    ASSERT( current, "ListIterator: no item available" );
    return current->getItem();
}


template <class T>
int ListIterator<T>::hasItem ()
{
    return current != 0;
}


template <class T>
void ListIterator<T>::operator++ ()
{
    if ( current )
        current = current->next;
}


template <class T>
void ListIterator<T>::operator-- ()
{
    if ( current )
        current = current->prev;
}


template <class T>
void ListIterator<T>::operator++ ( int )
{
    if ( current )
        current = current->next;
}


template <class T>
void ListIterator<T>::operator-- ( int )
{
    if ( current )
        current = current->prev;
}


template <class T>
void ListIterator<T>::firstItem ()
{
    current = theList->first;
}


template <class T>
void ListIterator<T>::lastItem ()
{
    current = theList->last;
}


template <class T>
void ListIterator<T>::insert ( const T & t )
{
    if ( current )
    {
        if ( ! current->prev )
            theList->insert( t );
        else
        {
            current->prev = new ListItem<T>( t, current, current->prev );
            current->prev->prev->next = current->prev;
            theList->_length++;
        }
    }
}


template <class T>
void ListIterator<T>::append ( const T & t )
{
    if ( current )
    {
        if ( ! current->next )
            theList->append( t );
        else
        {
            current->next = new ListItem<T>( t, current->next, current );
            current->next->next->prev = current->next;
            theList->_length++;
        }
    }
}


template <class T>
void ListIterator<T>::remove ( int moveright )
{
    if ( current )
    {
        ListItem <T>*dummynext = current->next, *dummyprev = current->prev;
        if ( current->prev )
        {
            current->prev->next = current->next;
            if ( current->next )
                current->next->prev = current->prev;
            else
                theList->last = current->prev;
            delete current;
            current = ( moveright ) ? dummynext : dummyprev;
        }
        else
        {
            if ( current->next )
                current->next->prev = 0;
            theList->first = current->next;
            delete current;
            current = ( moveright ) ? dummynext : dummyprev;
        }
        theList->_length--;
    }
}

#ifndef NOSTREAMIO
template <class T>
OSTREAM& operator<<( OSTREAM & os, const List<T> & l )
{
    l.print( os );
    return os;
}
#endif /* NOSTREAMIO */

template <class T>
List<T> Union ( const List<T> & F, const List<T> & G )
{
    List<T> L = G;
    ListIterator<T> i, j;
    T f;
    bool iselt;

    for ( i = F; i.hasItem(); i++ )
    {
        f = i.getItem();
        iselt = false;
        j = G;
        while ( ( ! iselt ) && j.hasItem() )
        {
            iselt =  f == j.getItem();
            j++;
        }
        if ( ! iselt )
            L.append( f );
    }
    return L;
}

template <class T>
List<T> Union ( const List<T> & F, const List<T> & G, int (*cmpf)( const T&, const T& ), void (*insf)( T&, const T& ) )
{
    List<T> L = G;
    ListIterator<T> i;

    for ( i = F; i.hasItem(); ++i )
        L.insert( i.getItem(), cmpf, insf );
    return L;
}

template <class T>
List<T> Union ( const List<T> & F, const List<T> & G, int (*ecmpf)( const T&, const T& ))
{
    List<T> L = G;
    ListIterator<T> i, j;
    T f;
    bool iselt;

    for ( i = F; i.hasItem(); i++ )
    {
        f = i.getItem();
        iselt = false;
        j = G;
        while ( ( ! iselt ) && j.hasItem() )
        {
            iselt =  ecmpf (f, j.getItem());
            j++;
        }
        if ( ! iselt )
            L.append( f );
    }
    return L;
}

template <class T>
List<T> Difference ( const List<T> & F, const List<T> & G )
{
    List<T> L;
    ListIterator<T> i, j;
    T f;
    int found;
    for ( i = F; i.hasItem(); ++i )
    {
        f = i.getItem();
        found = 0;
        for ( j = G; j.hasItem() && (!found); ++j )
            found = f == j.getItem();
        if ( ! found )
            L.append( f );
    }
    return L;
}

template <class T>
List<T> Difference ( const List<T> & F, const List<T> & G, int (*ecmpf)( const T&, const T& ))
{
    List<T> L;
    ListIterator<T> i, j;
    T f;
    int found;
    for ( i = F; i.hasItem(); ++i )
    {
        f = i.getItem();
        found = 0;
        for ( j = G; j.hasItem() && (!found); ++j )
            found = ecmpf (f, j.getItem());
        if ( ! found )
            L.append( f );
    }
    return L;
}

template <class T>
List<T> Difference ( const List<T> & F, const T & G, int (*ecmpf)( const T&, const T& ))
{
    List<T> L;
    ListIterator<T> i;
    int found;
    for ( i = F; i.hasItem(); ++i )
    {
        found = ecmpf (G, i.getItem());
        if ( ! found )
            L.append( i.getItem() );
    }
    return L;
}

template <class T>
List<T> Difference ( const List<T> & F, const T & G)
{
    List<T> L;
    ListIterator<T> i;
    int found;
    for ( i = F; i.hasItem(); ++i )
    {
        found = G == i.getItem();
        if ( ! found )
            L.append( i.getItem() );
    }
    return L;
}

template <class T>
T prod ( const List<T> & F )
{
    ListIterator<T> i;
    T p = 1;
    for ( i = F; i.hasItem(); i++ )
        p *= i.getItem();
    return p;
}

template <class T>
bool find (const List<T> & F, const T& t)
{
  if (F.length() == 0) return false;
  ListIterator<T> J= F;
  while (J.hasItem())
  {
    if (J.getItem() == t)
      return true;
    J++;
  }
  return false;
}

template <class T>
bool find (const List<T> & F, const T& t, int (*ecmpf)( const T&, const T& ))
{
  if (F.length() == 0) return false;
  ListIterator<T> J= F;
  while (J.hasItem())
  {
    if (ecmpf (J.getItem(), t))
      return true;
    J++;
  }
  return false;
}

