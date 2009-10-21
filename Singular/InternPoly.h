#ifndef INTERN_POLY_H
#define INTERN_POLY_H

#ifdef HAVE_WRAPPERS

#include "Wrappers.h"
#include "ReferenceCounter.h"
#include "RingWrapper.h"

/*! \class InternPoly
 *  \brief Class InternPoly is the superclass of all classes which
 *         wrap specific polynomial representations.
 *
 *  InternPoly is derived from ReferenceCounter, thus any instance of
 *  InternPoly and of its derived classes deploys reference counting and
 *  facilities for fast shallow copying.<br>
 *  Classes derived from InternPoly are intended to wrap specific polynomial
 *  representations such as SINGULAR-internal polynomials, as done by
 *  CanonicalPoly. Each such instance needs to <em>know</em> in which ring it
 *  lives. Therefore, InterPoly has a private member for storing a reference
 *  to some instance of RingWrapper.<br>
 *  Any instance of PolyWrapper holds a pointer to an instance of InternPoly
 *  wherein the polynomial is actually wrapped. This way, technicalities
 *  can be implemented inside InternPoly (and in the classes derived from it)
 *  and only the user interface for polynomail actions can by made available
 *  through PolyWrapper.<br>
 *  \author Frank Seelisch, http://www.mathematik.uni-kl.de/~seelisch
 */
class InternPoly : public ReferenceCounter
{
protected:
  /*! private member for storing a reference to a RingWrapper
      which wraps the ring in which the polynomial lives */
  const RingWrapper& m_ring;
  
  /*! an enumeration variable for making the code more readable;
      for any class C derived from InternPoly, the call <c>C::getPolyType ()</c>
      is to return one of these enums so that, based on the return value, the
      programmer may perform a <em>static</em> cast to cast a given instance
      of InternPoly to the correct derived class */
  static const int UNSPECIFIED_POLY_TYPE = 0;
  
  /*! an enumeration variable for making the code more readable;
      for any class C derived from InternPoly, the call <c>C::getPolyType ()</c>
      is to return one of these enums so that, based on the return value, the
      programmer may perform a <em>static</em> cast to cast a given instance
      of InternPoly to the correct derived class */
  static const int CANONICAL_POLY_TYPE   = 1;
  
  /*!
   *  A method for retrieving the type of the represented poly.
   *  For any class C derived from InternPoly, the call <c>C::getPolyType ()</c>
   *  is to return one of the defined enums so that, based on the return value, the
   *  programmer may perform a <em>static</em> cast to cast any given instance
   *  of InternPoly to the correct derived class.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   *  @return the polynomial type, one of the defined enums
   */
  virtual int getPolyType () const;
  
  /*!
   *  A method for adding two instances of InterPoly which are known to
   *  live in the same, or at least in compatible rings.<br>
   *  The result of the addition will be stored in the InternPoly given by <c>*this</c>;
   *  hence the given object will be modified by the operation.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   *  @param ip another InternPoly to be added to <c>this</c> InternPoly
   *  @see InternPoly::add (const InternPoly* ip)
   *  @see RingWrapper::isCompatible (const RingWrapper& r) const
   */
  virtual void addCompatible (const InternPoly* ip);
  
  /*!
   *  A method for deeply copying the given instance of InternPoly.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   *  @return a pointer to the deep copy of <c>this</c> InternPoly
   */
  virtual InternPoly* deepCopy () const;
  
  /*!
   *  A method for retrieving the RingWrapper which wraps the ring in
   *  which the represented polynomial lives.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   *  @return a RingWrapper wrapping the ring in which the polynomial lives
   */
  const RingWrapper& getRing () const;
  
  /*!
   *  A method for adding two instances of InterPoly which are not known to
   *  live in compatible rings.<br>
   *  In case of compatibility, the result of the addition will be stored
   *  in the InternPoly given by <c>*this</c>, hence the given object will be
   *  modified by the operation.<br>
   *  @param ip another InternPoly to be added to <c>this</c> InternPoly
   *  @see InternPoly::addCompatible (const InternPoly* ip)
   *  @see RingWrapper::isCompatible (const RingWrapper& r) const
   */
  void add (const InternPoly* ip);
  
  /*!
   *  A method for obtaining a string representation of the given instance.<br>
   *  This method needs to be reimplemented in each class derived from InternPoly.
   */
  virtual char* toString () const;
public:
  /*!
   *  A constructor for InternPoly which just sets the RingWrapper.<br>
   *  This constructor is assumed to be called implicitely when a constructor
   *  of one of the classes derived from InternPoly will be called.
   *  @param r a RingWrapper wrapping the ring in which the polynomial lives
   */
  InternPoly (const RingWrapper& r);
  
  /*!
   *  A copy constructor for InternPoly.<br>
   *  As this constructor should neither be called by the user nor
   *  implicitely by some method, it will halt the program.
   *  @param p a reference to another InternPoly
   */
  InternPoly (const InternPoly& p);
  
  /*!
   *  A destructor for InternPoly.
   */
  virtual ~InternPoly ();

/*! We enable PolyWrapper to "see" all methods of InternPoly. */
friend class PolyWrapper;

/*! We enable CanonicalPoly to "see" all methods of InternPoly. */
friend class CanonicalPoly;
};

#endif
/* HAVE_WRAPPERS */

#endif
/* INTERN_POLY_H */