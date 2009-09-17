#ifndef REFERENCE_COUNTER_H
#define REFERENCE_COUNTER_H

#include <iostream>

class ReferenceCounter {
public:
  /// Set type for storing reference counter
  typedef unsigned long refcount_type;

  /// Default constructor
  ReferenceCounter(): m_counter(0) { std::cout << "\n      ReferenceCounter default constructor"; }

  /// Deep copy
  ReferenceCounter(const ReferenceCounter& rc) { assume(false); /* copy constructor should never be called */ }

  /// Destructor
  ~ReferenceCounter() { std::cout << "\n      ReferenceCounter destructor"; }

  /// Get reference counter
  refcount_type getCounter() const { return m_counter; }

  /// Check, whether reference is used multiple times
  bool isShared() const { return (m_counter > 1); }

  /// Increase reference counter
  refcount_type increment();

  /// Decrease reference counter
  refcount_type decrement();

private:
  /// Store reference counter
  refcount_type m_counter;
};

#endif
/* REFERENCE_COUNTER_H */
