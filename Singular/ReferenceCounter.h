#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#ifdef HAVE_WRAPPERS

/// Set type for storing reference counter
typedef unsigned long refcount_type;

class ReferenceCounter {
private:
  /// Store reference counter
  refcount_type m_counter;
public:
  ReferenceCounter ();

  /// Deep copy
  ReferenceCounter (const ReferenceCounter& rc);

  /// Destructor
  ~ReferenceCounter ();

  /// Get reference counter
  refcount_type getCounter () const;

  /// Check, whether reference is used multiple times
  bool isShared () const;

  /// Increase reference counter
  refcount_type increment ();

  /// Decrease reference counter
  refcount_type decrement ();
};

#endif // HAVE_WRAPPERS

#endif
/* REFERENCE_COUNTER_H */
