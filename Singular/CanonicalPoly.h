#ifndef CANONICAL_POLY_H
#define CANONICAL_POLY_H

#ifdef HAVE_WRAPPERS

#include "ring.h"
#include <string>
#include "Wrappers.h"
#include "InternPoly.h"

/*! \class CanonicalPoly
 *  \brief Class CanonicalPoly is the class for representing SINGULAR-internal,
 *         so-called canonical polynomials.
 *
 *  CanonicalPoly is derived from InternPoly which is derived from ReferenceCounter,
 *  thus any instance of CanonicalPoly deploys reference counting and
 *  facilities for fast shallow copying.<br>
 *  Any instance of CanonicalPoly wraps an actual SINGULAR polynomial.
 *  \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
 */
class CanonicalPoly : public InternPoly
{
private:
  /*! placeholder for a SINGULAR-internal polynomial */
  SingularPoly m_poly;
  
  /*!
   *  A method for retrieving the stored SINGULAR-internal polynomial.
   *  @return the stored SINGULAR-internal polynomial
   */
  const SingularPoly& getSingularPoly () const;
protected:
  /*!
   *  A method for deeply copying the given instance of CanonicalPoly.<br>
   *  @return a pointer to the deep copy of <c>this</c> CanonicalPoly
   */
  CanonicalPoly* deepCopy () const;
  
  /*!
   *  A method for adding an instance of CanonicalPoly and an instance of
   *  InternPoly.<br>
   *  Both polynomials are known to live in compatible rings. For the time being,
   *  the implementation of this method will only succeed when the argument InternPoly
   *  can also be casted to CanonicalPoly. This cast will be performed static in case
   *  that <c>ip.getPolyType()</c> yields the enum CANONICAL_POLY_TYPE. Otherwise the
   *  program will halt.<br>
   *  In case of success, the result of the addition will be stored in the
   *  CanonicalPoly given by <c>*this</c>, hence the given object will then be modified
   *  by the operation.<br>
   *  @param ip another InternPoly to be added to <c>this</c> CanonicalPoly
   *  @see InternPoly::addCompatible (const InternPoly* ip)
   *  @see InternPoly::add (const InternPoly* ip)
   *  @see RingWrapper::isCompatible (const RingWrapper& r) const
   */
  void addCompatible (const InternPoly* ip);
  
  /*!
   *  A method for retrieving the type of the represented poly.
   *  For any class C derived from InternPoly, the call <c>C::getPolyType ()</c>
   *  is to return one of the defined enums so that, based on the return value, the
   *  programmer may perform a <em>static</em> cast to cast any given instance
   *  of InternPoly to the correct derived class.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   *  @return the enum CANONICAL_POLY_TYPE
   */
  int getPolyType () const;
  
  /*!
   *  A method for obtaining a string representation of the given instance.
   */
  char* toString () const;
public:
  ~CanonicalPoly ();
  
  /*!
   *  A constructor for CanonicalPoly.<br>
   *  This constructor will create a representation of the given integer
   *  as a wrapped SINGULAR-internal polynomial.
   *  @param r a RingWrapper wrapping the ring in which the polynomial lives
   *  @see CanonicalPoly::CanonicalPoly (const SingularPoly&, const RingWrapper& r)
   */
  CanonicalPoly (const int i, const RingWrapper& r);
  
  /*!
   *  A constructor for CanonicalPoly.<br>
   *  This constructor will wrap the given SINGULAR-internal polynomial.
   *  @param r a RingWrapper wrapping the ring in which the polynomial lives
   *  @see CanonicalPoly::CanonicalPoly (const int i, const RingWrapper& r)
   */
  CanonicalPoly (const SingularPoly&, const RingWrapper& r);
  
/*! We enable PolyWrapper to "see" all methods of CanonicalPoly. */
friend class PolyWrapper;
};

#endif
/* HAVE_WRAPPERS */

#endif
/* CANONICAL_POLY_H */