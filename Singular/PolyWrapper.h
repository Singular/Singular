#ifndef POLY_WRAPPER_H
#define POLY_WRAPPER_H

#ifdef HAVE_WRAPPERS

#include "Wrappers.h"
#include "InternPoly.h"
#include "RingWrapper.h"

/*! \class PolyWrapper
 *  \brief Class PolyWrapper provides a uniform interface to different
 *         representations of polynomials.
 *
 *  Instances of PolyWrapper uniformly represent different kinds of
 *  polynomials that play a role in the computer algebra system SINGULAR
 *  and related suites such as PolyBoRi. Especially SINGULAR-internal
 *  polynomials can be wrapped by instances of PolyWrapper.<br>
 *  This implementation comes with an automatic reference counting so that copying
 *  PolyWrappers is cheap as it will simply be done by incrementing the
 *  respective reference counter. Constructors and destructors automatically
 *  manage the reference counting. Methods that create new instances of
 *  PolyWrapper, e.g. PolyWrapper::operator+ (const PolyWrapper& p) const,
 *  will internally enforce the instantiation of a new ReferenceCounter.<br>
 *  Summarizing this means that all methods of PolyWrapper <em>know</em> when
 *  to make a shallow copy (by incrementing the ReferenceCounter) and when to
 *  make a deep copy.<br>
 *  Polynomials wrapped by PolyWrapper <em>know</em> in which ring they live.
 *  A wrapper object of that ring can be obtained by calling
 *  PolyWrapper::getRing () const.
 *  \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
 */
class PolyWrapper
{
private:
  /*! a pointer to an internal polynomial representation,
      e.g. to a SINGULAR-internal polynomial */
  InternPoly* m_pInternPoly;
  
  /*!
   *  A constructor for PolyWrapper.<br>
   *  This constructor just wraps the provided instance of InternPoly,
   *  which may e.g. be a SINGULAR-internal polynomial.
   *  @param ip the internal polynomial to be wrapped
   *  @see PolyWrapper::PolyWrapper ()
   *  @see PolyWrapper::PolyWrapper (const PolyWrapper& p)
   *  @see PolyWrapper::PolyWrapper (const int i, const RingWrapper& r)
   *  @see PolyWrapper::PolyWrapper (const SingularPoly& sp, const RingWrapper& r)
   */
  PolyWrapper (InternPoly* ip);
  
  /*!
   *  A method for retrieving the wrapped InternPoly.
   *  @return the wrapped InternPoly
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   */
  InternPoly* getInternPoly () const;
public:
  /*!
   *  A default constructor for PolyWrapper.<br>
   *  This constructor will make the program stop as it should
   *  never be called by the user or as a side-effect of any action.
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   *  @see PolyWrapper::PolyWrapper (const PolyWrapper& p)
   *  @see PolyWrapper::PolyWrapper (const int i, const RingWrapper& r)
   *  @see PolyWrapper::PolyWrapper (const SingularPoly& sp, const RingWrapper& r)
   */
  PolyWrapper ();

  /*!
   *  A copy constructor for PolyWrapper.<br>
   *  This constructor creates a shallow copy of the argument. This is
   *  simply done by incrementing the ReferenceCounter.
   *  @param p the PolyWrapper to be shallow-copied
   *  @see PolyWrapper::PolyWrapper ()
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   *  @see PolyWrapper::PolyWrapper (const int i, const RingWrapper& r)
   *  @see PolyWrapper::PolyWrapper (const SingularPoly& sp, const RingWrapper& r)
   */
  PolyWrapper (const PolyWrapper& p);

  /*!
   *  A constructor for PolyWrapper.<br>
   *  This constructor creates a SINGULAR-internal polynomial representing
   *  the given argument integer, and then a wrapped version of it.<br>
   *  The user may use RingWrapper::RingWrapper () to create a RingWrapper wrapping
   *  the current SINGULAR-internal ring (<c>currRing</c>).
   *  @param i the integer to be represented as a polynomial
   *  @param r a RingWrapper wrapping the internal ring in which the polynomial lives
   *  @see PolyWrapper::PolyWrapper ()
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   *  @see PolyWrapper::PolyWrapper (const PolyWrapper& p)
   *  @see PolyWrapper::PolyWrapper (const SingularPoly& sp, const RingWrapper& r)
   */
  PolyWrapper (const int i, const RingWrapper& r);
  
  /*!
   *  A constructor for PolyWrapper.<br>
   *  This constructor creates a wrapped version of the given SINGULAR-internal
   *  polynomial.<br>
   *  The user may use RingWrapper::RingWrapper () to create a RingWrapper wrapping
   *  the current SINGULAR-internal ring (<c>currRing</c>).
   *  @param sp a SINGULAR-internal polynomial
   *  @param r a RingWrapper wrapping the internal ring in which the polynomial lives
   *  @see PolyWrapper::PolyWrapper ()
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   *  @see PolyWrapper::PolyWrapper (const PolyWrapper& p)
   *  @see PolyWrapper::PolyWrapper (const int i, const RingWrapper& r)
   */
  PolyWrapper (const SingularPoly& sp, const RingWrapper& r);

  /*!
   *  A destructor for PolyWrapper.<br>
   *  This constructor creates a wrapped version of the given SINGULAR-internal
   *  polynomial.<br>
   *  This routine will automatically decrement the ReferenceCounter and, in case
   *  the counter reaches zero, enforce the destruction of related objects.
   *  @see PolyWrapper::PolyWrapper ()
   *  @see PolyWrapper::PolyWrapper (InternPoly* ip)
   *  @see PolyWrapper::PolyWrapper (const PolyWrapper& p)
   *  @see PolyWrapper::PolyWrapper (const int i, const RingWrapper& r)
   *  @see PolyWrapper::PolyWrapper (const SingularPoly& sp, const RingWrapper& r)
   */
  ~PolyWrapper ();
  
  /*!
   *  A method for printing a string representation of the given instance.<br>
   *  This method does not use std::cout but only SINGULAR-internal print routines.
   *  @see PolyWrapper::printLn () const
   *  @see PolyWrapper::toString () const
   */
  void print () const;
  
  /*!
   *  A method for printing a string representation of the given instance
   *  followed by a linefeed.<br>
   *  This method does not use std::cout but only SINGULAR-internal print routines.
   *  @see PolyWrapper::print () const
   *  @see PolyWrapper::toString () const
   */
  void printLn () const;

  /*!
   *  A method for obtaining a string representation of the given instance.
   *  @see PolyWrapper::print () const
   *  @see PolyWrapper::printLn () const
   */
  char* toString () const;
  
  /*!
   *  Assignment operator for PolyWrapper.<br>
   *  This method will just exchange the wrapped instance of InternPoly.
   *  Consequently, the ReferenceCounter of the InternPoly wrapped by the
   *  given argument will be decremented (and in case of reaching zero,
   *  the destruction of related objects will be enforced).
   *  @param p an instance of PolyWrapper
   *  @return a reference to the new PolyWrapper
   */
  PolyWrapper& operator= (const PolyWrapper& p);
  
  /*!
   *  Addition operator for PolyWrapper.<br>
   *  This method will compute a PolyWrapper which wraps the result of
   *  adding the polynomials wrapped by <c>this</c> PolyWrapper and by
   *  the argument PolyWrapper.
   *  @param p an instance of PolyWrapper
   *  @return a PolyWrapper representing the sum
   */
  PolyWrapper operator+ (const PolyWrapper& p) const;

  /*!
   *  Modifying addition operator for PolyWrapper.<br>
   *  This method will compute (in the PolyWrapper given by <c>this</c>),
   *  the result of adding the polynomials wrapped by <c>this</c> PolyWrapper and by
   *  the argument PolyWrapper. Note that this method will in general change the
   *  given object.
   *  @param p an instance of PolyWrapper
   *  @return a reference to <c>this</c> PolyWrapper now representing the sum
   */
  PolyWrapper& operator+= (const PolyWrapper& p);
  
  /*!
   *  A method for retrieving a RingWrapper wrapping the internal ring in which
   *  the given polynomial lives.
   *  @return a reference to a RingWrapper
   */
  const RingWrapper& getRing () const;
};

#endif
/* HAVE_WRAPPERS */

#endif
/* POLY_WRAPPER_H */
