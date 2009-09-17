#include "mod2.h"
#include "ReferenceCounter.h"

using namespace std;

ReferenceCounter::refcount_type ReferenceCounter::decrement() 
{
  assume(m_counter > 0); // Ensure positivity of counter
  cout << "\n      ReferenceCounter decremented to " << m_counter - 1;
  return --m_counter; 
};

ReferenceCounter::refcount_type ReferenceCounter::increment()
{
  std::cout << "\n      ReferenceCounter incremented to " << m_counter + 1; return ++m_counter;
}