#include <NTL/config.h>


#if (defined(__cplusplus) && !defined(NTL_CXX_ONLY))
extern "C" double _ntl_GetTime();
#endif


double _ntl_GetTime(void)
{
   return 0;
}
