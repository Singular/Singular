#include <sys/time.h>
double _ntl_GetTime() { 
    struct timeval tv; 
    gettimeofday(&tv, 0); 
    return (tv.tv_sec + (tv.tv_usec / 1000000.0)); 
}