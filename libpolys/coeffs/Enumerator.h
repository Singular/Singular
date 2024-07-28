// -*- Mode: C++; tab-width: 2; indent-tabs-mode: nil; c-basic-offset: 2 -*-
/*****************************************************************************\
 * Computer Algebra System SINGULAR
\*****************************************************************************/
/** @file Enumerator.h
 *
 * Abstract API for enumerators.
 *
 * ABSTRACT: Abstract interface for forward iterable containers (enumerators)
 * of standalone objects (e.g. polynomials as sets of numbers), without any
 * knowledge of their internals.
 *
 * @author Oleksandr Motsak
 *
 *
 **/
/*****************************************************************************/

#ifndef ENUMERATOR_H
#define ENUMERATOR_H

/** @class IBaseEnumerator
 *
 * Base enumerator interface for simple iteration over a generic collection.
 *
 * Abstract API of enumerators for enumerable collections of standalone objects.
 * Just like IEnumerator from C#. Usage pattern can be as follows:
 *
 * @code
 *   IBaseEnumerator& itr = ...;
 *   itr.Reset(); // goes to the "-1" element
 *   // NOTE: itr is not usable here!
 *   while( itr.MoveNext() )
 *   {
 *      do something custom with itr...
 *   }
 * @endcode
 *
 * Note that the Reset()
 *
 * @sa IEnumerator
 */
class IBaseEnumerator // IDisposable
{
  public:
    /// Advances the enumerator to the next element of the collection.
    /// returns true if the enumerator was successfully advanced to the
    /// next element;
    /// false if the enumerator has passed the end of the collection.
    virtual bool MoveNext() = 0;

    /// Sets the enumerator to its initial position: -1,
    /// which is before the first element in the collection.
    virtual void Reset() = 0;

    /// Current position is inside the collection (not -1 or past the end)
    virtual bool IsValid() const = 0;

  private:
    /// disable copy constructor and assignment operator
    IBaseEnumerator(const IBaseEnumerator&);
    void operator=(const IBaseEnumerator&);

  protected:
    IBaseEnumerator(){}
    ~IBaseEnumerator() {} // TODO: needed?


};


/** @class IAccessor
 *
 * Templated accessor interface for accessing individual data (for instance, of an enumerator).
 *
 * T is the type of objects to access, available via the Current() method.
 *
 * @sa IBaseEnumerator
 */
template <typename T>
class IAccessor // IDisposable
{
  public:
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;

    /// Gets the current element in the collection (read and write).
    virtual reference Current() = 0;

    /// Gets the current element in the collection (read only).
    virtual const_reference Current() const = 0;

 protected:
    IAccessor(){}
    ~IAccessor() {} // TODO: needed?

};

/** @class IEnumerator
 *
 * Templated enumerator interface for simple iteration over a generic collection of T's.
 *
 * Abstract API of enumerators for generic enumerable collections of standalone
 * objects of type T. Inspired by IEnumerator from C#. Usage parrten can be as
 * follows:
 *
 * @code
 *   IEnumerator<T>& itr = ...;
 *
 *   itr.Reset(); // goes before the first element, thus no itr.Current() is available here!
 *
 *   while( itr.MoveNext() )
 *   {
 *      use/change itr.Current()...
 *   }
 * @endcode
 *
 * T is the type of objects to enumerate, available via Current() method
 *
 * @sa IBaseEnumerator
 */
template <typename T>
class IEnumerator: public virtual IBaseEnumerator, public virtual IAccessor<T>
{
  public:
};

#if 0
// the following is not used for now //

// include basic definitions
//??// #include <iterator>

/** @class IBaseIterator
 *
 * A base abstract iterator API with virtualized standard iterator operators
 *
 * Abstract API for iterators that should work with STL and BOOST.
 *
 * @sa STL iterators
 */
template <class A>
class IBaseIterator //??// : public std::iterator<std::forward_iterator_tag, A>
{
  public:
    typedef IBaseIterator<A> self;
    typedef self& self_reference;
    typedef const self_reference const_self_reference;

    virtual bool operator==(const_self_reference rhs) = 0;

    /// ++itr, goes to the next state, returns the new state
    virtual self_reference operator++() = 0;

    /// itr++, goes to the next state, returns the previous state
    virtual self_reference operator++(int) = 0;

    virtual A& operator*() = 0;
    virtual A* operator->() = 0;

    inline bool operator!=(const_self_reference rhs){ return !((*this) == rhs); }
};

/** @class AIterator
 *
 * An abstract iterator with virtualized assignment operator and
 * constructors.
 *
 * Abstract API for iterators that should work with STL and BOOST.
 *
 * @sa STL iterators
 */
template <class A>
class IIterator: public IBaseIterator<A>
{
  public:
    typedef IIterator<A> self;
    typedef self& self_reference;
    typedef const self_reference const_self_reference;

    IIterator(){ void_constructor(); }

    IIterator(const_self_reference itr) { copy_constructor(itr); }

    virtual self_reference operator=(const_self_reference other) = 0;

  private:
    virtual void void_constructor() = 0;
    virtual void copy_constructor(const_self_reference itr) = 0;
};

/** @class IContainer
 *
 * Container of standalone objects
 *
 * Abstract API for containers of objects and their iterators
 *
 * @sa STL containers and iterators
 */
template <class T>
class IContainer
{
  public:
    typedef T value_type;
    typedef value_type& reference;
    typedef const value_type& const_reference;
//??//    typedef std::size_t size_type;
//??//    virtual size_type size() const = 0;
    virtual bool empty() const = 0;

    typedef IIterator<reference> iterator;
    virtual iterator begin() = 0;
    virtual iterator end() = 0;

    typedef IIterator<const_reference> const_iterator;
    virtual const_iterator begin() const = 0;
    virtual const_iterator end() const = 0;
};
#endif


#endif
/* #ifndef ENUMERATOR_H */

// Vi-modeline: vim: filetype=c:syntax:shiftwidth=2:tabstop=8:textwidth=0:expandtab
