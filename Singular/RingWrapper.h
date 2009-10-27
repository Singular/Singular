#ifndef RING_WRAPPER_H
#define RING_WRAPPER_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include "Wrappers.h"

/*! \class RingWrapper
 *  \brief Class RingWrapper provides a uniform interface to different
 *         representations of rings.
 *
 *  Instances of RingWrapper uniformly represent different kinds of
 *  rings that play a role in the computer algebra system SINGULAR
 *  and related suites such as PolyBoRi. Especially SINGULAR-internal
 *  rings can be wrapped by instances of RingWrapper.<br>
 *  This is just a first implementation, so far for wrapping SINGULAR-internal
 *  rings only.<br>
 *  An important feature of RingWrapper is the check whether two given rings
 *  (wrapped as instances of RingWrapper) are compatible with respect to
 *  basic arithmetics; such as PolyWrapper::operator+ (const PolyWrapper& p) const.
 *  \author Michael Brickenstein, Alexander Dreyer, Oleksandr Motsak, Hans Sch&ouml;nemann, Frank Seelisch, http://www.mathematik.uni-kl.de
 */
class RingWrapper
{
private:
  /*! a member for storing a SINGULAR-internal ring;
      note that this is not just a reference to but an instance
      of a SINGULAR-internal ring */
  SingularRing m_singularRing;
public:
  /*!
   *  A constructor for RingWrapper.<br>
   *  This constructor builds a SINGULAR-internal ring with a given
   *  name, characteristic and list of variables. The monomial order
   *  must also be provided and must be one of {dp, lp, Dp, ds, ls, Ds}.
   *  @param ringName the name of the wrapped SINGULAR-internal ring
   *  @param characteristic the characteristic of the wrapped SINGULAR-internal ring
   *  @param varNumber the number of variables in the wrapped SINGULAR-internal ring
   *  @param varNames the array of names of variables in the wrapped SINGULAR-internal ring
   *  @param ringOrder one of {dp, lp, Dp, ds, ls, Ds}
   *  @see RingWrapper::RingWrapper ()
   */
  RingWrapper (const char* ringName, const int characteristic,
               const int varNumber, const char** varNames, const char* ringOrder);
               
  /*!
   *  A default constructor for RingWrapper.<br>
   *  This constructor wraps the current SINGULAR-internal ring which is
   *  assumed to use one of the monomial orderings of {dp, lp, Dp, ds, ls, Ds}.
   *  @see RingWrapper::RingWrapper (const char* ringName, const int characteristic,
                                     const int varNumber, const char** varNames, const char* ringOrder)
   */
  RingWrapper ();
  
  /*!
   *  A destructor for RingWrapper.<br>
   *  Since the private member is an instance of a SINGULAR-internal ring and not
   *  a reference, the destructor for this SINGULAR-internal ring will be called.
   *  @see RingWrapper::RingWrapper ()
   *  @see RingWrapper::RingWrapper (const char* ringName, const int characteristic,
                                     const int varNumber, const char** varNames, const char* ringOrder)
   */
  ~RingWrapper ();

  /*!
   *  A method for printing a string representation of the given instance.<br>
   *  This method does not use std::cout but only SINGULAR-internal print routines.
   *  @see RingWrapper::printLn () const
   *  @see RingWrapper::toString () const
   */
  void print () const;
  
  /*!
   *  A method for printing a string representation of the given instance
   *  followed by a linefeed.<br>
   *  This method does not use std::cout but only SINGULAR-internal print routines.
   *  @see RingWrapper::print () const
   *  @see RingWrapper::toString () const
   */
  void printLn () const;
  
  /*!
   *  A method for obtaining a string representation of the given instance.
   *  @see RingWrapper::print () const
   *  @see RingWrapper::printLn () const
   */
  char* toString () const;

  /*!
   *  A method for checking whether two wrapped rings are compatible with
   *  respect to basic arithmetics such as PolyWrapper::operator+ (const PolyWrapper& p)
   *  @param r another RingWrapper
   *  @return true if the tow wrpaped rings are compatible
   */
  bool isCompatible (const RingWrapper& r) const;
  
  /*!
   *  A method for retrieving the wrapped SINGULAR-internal ring.
   *  @return the wrapped SINGULAR-internal ring
   */
  const SingularRing& getSingularRing () const;
};

#endif
/* HAVE_WRAPPERS */

#endif
/* RING_WRAPPER_H */