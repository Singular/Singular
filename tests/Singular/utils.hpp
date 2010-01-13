#ifndef _utils_hpp_
#define _utils_hpp_

#include <time.h>

#ifdef _WIN32
#include <windows.h>
   #if !defined(_WINSOCK2API_) && !defined(_WINSOCKAPI_)
         struct timeval
         {
            long tv_sec;
            long tv_usec;
         };
   #endif

   int gettimeofday(struct timeval* tp, void* tzp)
   {
      DWORD t;
      t = timeGetTime();
      tp->tv_sec = t / 1000;
      tp->tv_usec = t % 1000;
      return 0;
   }
#else
   #include <sys/time.h>
#endif

extern double Time ();


#endif /* _utils_hpp_ */
