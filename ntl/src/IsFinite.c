
#include <NTL/IsFinite.h>

double _ntl_IsFinite__local;
double *_ntl_IsFinite__ptr1 = &_ntl_IsFinite__local;
double *_ntl_IsFinite__ptr2 = &_ntl_IsFinite__local;

long _ntl_IsFinite(double *p)
{
   *_ntl_IsFinite__ptr1 = *p;
   *_ntl_IsFinite__ptr2 = (*_ntl_IsFinite__ptr2 - *p);
   if (*_ntl_IsFinite__ptr1 != 0.0) return 0;
   return 1;
}

void _ntl_ForceToMem(double *p)
{ }
