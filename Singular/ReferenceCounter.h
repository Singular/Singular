#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#ifdef HAVE_WRAPPERS

#include "Wrappers.h"

/*! \class ReferenceCounter
 *  \brief Class ReferenceCounter may be used to derive classes from it
 *         which deploy reference counting.
 *
 *  Any instance of a class, say C, derived from ReferenceCounter is
 *  equipped with a counter that stores how many instances of C exist.
 *  Thereby, it becomes possible to better manage instances of C and
 *  make shallow copies of instances of C by simply incrementing the
 *  reference counter (which is a member of the superclass of C, i.e.,
 *  of ReferenceCounter).
 *  \author Michael Brickenstein, Alexander Dreyer, Oleksandr Motsak, Hans Sch&ouml;nemann, Frank Seelisch, http://www.mathematik.uni-kl.de
 */
class ReferenceCounter {
private:
  /*! private member for storing the actual reference counter */
  ReferenceCounterType m_counter;
protected:
  /*!
   *  A method for retrieving the actual reference counter.
   *  @return the actual reference counter
   */
  ReferenceCounterType getCounter () const;

  /*!
   *  A method for retrieving whether the reference counter is
   *  greater than 1. In this case, at least two references to the given
   *  instance of ReferenceCounter exist.
   *  @return true if the reference counter is greater than 1
   */
  bool isShared () const;

  /*!
   *  A method for incrementing the reference counter.
   *  @return the value of the reference counter after incrementing
   *  @see ReferenceCounterType::decrement ();
   */
  ReferenceCounterType increment ();

  /*!
   *  A method for decrementing the reference counter.
   *  @return the value of the reference counter after decrementing
   *  @see ReferenceCounterType::increment ();
   */
  ReferenceCounterType decrement ();
public:
  /*!
   *  A constructor for ReferenceCounter.<br>
   *  This constructor will create a new instance with actual
   *  reference counter set to zero.
   */
  ReferenceCounter ();

  /*!
   *  A copy constructor for ReferenceCounter.<br>
   *  As this constructor should neither be called by the user nor
   *  implicitely by some method, it will halt the program.
   */
  ReferenceCounter (const ReferenceCounter& rc);

  /*!
   *  A destructor for ReferenceCounter.
   */
  ~ReferenceCounter ();
/*! We enable PolyWrapper to "see" all methods of ReferenceCounter. */
friend class PolyWrapper;
};

#endif
/* HAVE_WRAPPERS */

#endif
/* REFERENCE_COUNTER_H */
