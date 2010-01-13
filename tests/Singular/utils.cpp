
#include "utils.hpp"

double Time ()
{
  struct timeval tv;
  gettimeofday (&tv, NULL);
  
  return tv.tv_sec + (tv.tv_usec / 1.0e6);
}

