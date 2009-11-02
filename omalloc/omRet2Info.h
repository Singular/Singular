/*******************************************************************
 *  File:    omAddr2Info.h
 *  Purpose: translation of return addr to RetInfo
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_RET_2_INFO_H
#define OM_RET_2_INFO_H


struct omRetInfo_s
{
  void* addr;
  char  func[200]; /* if you change these numbers, alos change */
  char  file[200]; /* the fscanf statement in BackTrace_2_RetInfo */
  int   line;
};

/* print to fd array of max RetInfo's and use fmt as format string for one RetInfo.
   The following conversions are recognized:
    %i -- number of return addr
    %p -- address (in hex)
    %F -- function name with arguments (for C++, only)
    %N -- funtion name without arguments
    %f -- file
    %l -- line
    %L -- file:line
   return how many lines were printed */
#ifndef OM_NDEBUG
int omPrintRetInfo(omRetInfo info, int max, FILE *fd, const char* fmt);
/* translate array of max BackTrace addresses into array of max RetInfos
   return how many were translated */
int omBackTrace_2_RetInfo(void** bt, omRetInfo info, int max);

/* call with argv[0] to enable RetInfo translations */
void omInitRet_2_Info(const char* argv0);

int omPrintBackTrace(void** bt, int max, FILE* fd);
#define omPrintCurrentBackTrace(fd) omPrintCurrentBackTraceMax(fd, 1000)
int omPrintCurrentBackTraceMax(FILE* fd, int max);

int omFilterRetInfo_i(omRetInfo info, int max, int i);

#define omFilterRetInfo(info, max, cond)        \
do                                              \
{                                               \
  int _max = max;                               \
  int _i = max -1;                              \
  while (_i > 0)                                \
  {                                             \
    void* addr_i = info[_i].addr;               \
    char* file_i = info[_i].file;               \
    char* func_i = info[_i].func;               \
    int   line_i = info[_i].line;               \
                                                \
    if (cond)                                   \
      max = omFilterRetInfo_i(info, max, _i);   \
    _i--;                                       \
  }                                             \
}                                               \
while (0)


/*BEGINPRIVATE*/
int _omPrintBackTrace(void** bt, int max, FILE* fd , OM_FLR_DECL);
int _omPrintCurrentBackTrace(FILE* fd , OM_FLR_DECL);
/*ENDPRIVATE*/

#else /* OM_NDEBUG */
#define omPrintRetInfo(i, max, fd, fmt) ((void)0)
#define omBackTrace_2_RetInfo(bt, i, m) ((void)0)
#define omInitRet_2_Info(a)             ((void)0)
#define omPrintBackTrace(bt,max,fd)     ((void)0)
#define omPrintCurrentBackTrace(fd)     ((void)0)
#define omPrintCurrentBackTraceMax(fd,max) ((void)0)
#define omFilterRetInfo_i(info,max,i)   ((void)0)
#define omFilterRetInfo(info, max, cond)((void)0)
#endif /* ! OM_NDEBUG */

#endif /* OM_RET_2_INFO_H */
