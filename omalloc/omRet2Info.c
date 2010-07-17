/*******************************************************************
 *  File:    omRetur2Info.c
 *  Purpose: translation of return addr to RetInfo
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#include <stdio.h>
#include <string.h>

#ifdef HAVE_CONFIG_H
#include <omalloc/omConfig.h>
#endif

#ifndef OM_NDEBUG

#include <omalloc/omDerivedConfig.h>
#include <omalloc/omStructs.h>
#include <omalloc/omRet2Info.h>
#include <omalloc/omFindExec.h>
#include <omalloc/omGetBackTrace.h>

#ifndef MAXPATHLEN
#define MAXPATHLEN 1024
#endif

/* define to also print return addresses in (default)
   output of omPrintInfo */
/* #define OM_PRINT_RETURN_ADDRESS */
static char om_this_prog[MAXPATHLEN] = "";

#ifndef OM_MAX_BACKTRACE_DEPTH
#define OM_MAX_BACKTRACE_DEPTH 16
#endif

void omInitRet_2_Info(const char* argv0)
{
  char buf[MAXPATHLEN];

  if (argv0 != NULL && omFindExec(argv0, buf))
  {
    strcpy(om_this_prog, buf);
  }
}


int omBackTrace_2_RetInfo(void** bt, omRetInfo info, int max)
{
  int i=0, j=0, filled = 0;
  if (max <= 0 || bt == NULL || info == NULL) return 0;
  if (max > OM_MAX_BACKTRACE_DEPTH) max = OM_MAX_BACKTRACE_DEPTH;
  memset(info, 0, max*sizeof(omRetInfo_t));
  while (i<max)
  {
    if (bt[i])
    {
      info[j].addr = bt[i];
      j++;
    }
    i++;
  }
  if (j == 0) return 0;

#if defined(HAVE_POPEN) && defined(OM_PROG_ADDR2LINE)
  if (*om_this_prog != '\0')
  {
    char command[2*MAXPATHLEN + 15 + OM_MAX_BACKTRACE_DEPTH*(2*SIZEOF_VOIDP + 4)];
    FILE *pipe;
    int l;
    l = sprintf(command, "%s -s -C -f -e %s",
                OM_PROG_ADDR2LINE, om_this_prog);
    i=0;
    while (i<j)
    {
      l+=sprintf(&command[l], " %p", info[i].addr);
      i++;
    }
    fflush(NULL);
    pipe = popen(command, "r");
    if (pipe != NULL)
    {
        /* An output entry of addr2line looks as follows:
FunctionName
File:Line
        */
      while ((filled < j) &&
             (fscanf(pipe, "%200[^\n]\n%200[^:]:%d\n", info[filled].func, info[filled].file, &(info[filled].line)) == 3))
      {
        if (*info[filled].func != '?' && *info[filled].file != '?' && info[filled].line > 0)
          filled++;
      }
      pclose(pipe);
    }
  }
#endif
  return filled;
}

int omPrintRetInfo(omRetInfo info, int max, FILE* fd, const char* fmt)
{
  int i = 0;
  if (max <= 0 || info == NULL || fmt == NULL || fd == NULL) return 0;
  while (i < max && info[i].addr != NULL)
  {
    int l = 0;
    while (fmt[l] != 0)
    {
      if (fmt[l] == '%')
      {
        l++;
        if (fmt[l] == 'p') fprintf(fd, "%p", info[i].addr);
        else if (fmt[l] == 'f') fprintf(fd, "%-20s", (*info[i].file != '\0' ? info[i].file : "??"));
        else if (fmt[l] == 'F') fprintf(fd, "%-20s", (*info[i].func != '\0' ? info[i].func : "??"));
        else if (fmt[l] == 'l') fprintf(fd, "%d", info[i].line);
        else if (fmt[l] == 'N')
        {
          if (*info[i].func != '\0')
          {
            char* found = (char*) strchr(info[i].func, '(');
            if (found) *found = '\0';
            fprintf(fd, "%-20s", info[i].func);
            if (found) *found = '(';
          }
          else
            fprintf(fd, "%-20s", "??");
        }
        else if (fmt[l] == 'L')
        {
          int n = fprintf(fd, "%s:%d", (*info[i].func != '\0' ? info[i].file : "??"), info[i].line);
          if (n < 20) fprintf(fd, "%*s", 20-n, " ");
        }
        else if (fmt[l] == 'i') fprintf(fd, "%d", i);
        else
        {
          fputc('%', fd);
          l--;
        }
      }
      else
      {
        fputc(fmt[l], fd);
      }
      l++;
    }
    i++;
  }
  return i;
}

int omPrintBackTrace(void** bt, int max, FILE* fd)
{
  int i;

  omRetInfo_t info[OM_MAX_BACKTRACE_DEPTH];
  if (max > OM_MAX_BACKTRACE_DEPTH) max = OM_MAX_BACKTRACE_DEPTH;

  i = omBackTrace_2_RetInfo(bt, info, max);
#ifdef OM_PRINT_RETURN_ADDRESS
  return omPrintRetInfo(info, i, fd, "  #%i at %L in %N ra=%p\n");
#else
  return omPrintRetInfo(info, i, fd, "  #%i at %L in %N\n");
#endif
}

int omPrintCurrentBackTraceMax(FILE* fd, int max)
{
  int i;
  void* bt[OM_MAX_BACKTRACE_DEPTH];
  if (max > OM_MAX_BACKTRACE_DEPTH)
    max = OM_MAX_BACKTRACE_DEPTH;
  if (max <= 0) return 0;
  i = omGetBackTrace(bt, 1, max);
  return omPrintBackTrace(bt, i, fd);
}

/*************************************************************
 *
 * Various Filters
 *
 *************************************************************/
int omFilterRetInfo_i(omRetInfo info, int max, int i)
{
  int j=0, k=i;

  while (k < max)
  {
    info[j] = info[k];
    j++;
    k++;
  }
  return j;
}

/*************************************************************
 *
 * Low level routines
 *
 *************************************************************/

int _omPrintBackTrace(void** bt, int max, FILE* fd , OM_FLR_DECL)
{
  int i = 0;

  omRetInfo_t info[OM_MAX_BACKTRACE_DEPTH];
  if (max > OM_MAX_BACKTRACE_DEPTH) max = OM_MAX_BACKTRACE_DEPTH;
  if (bt != NULL)
  {
  for (; i<max; i++)
  {
    if (bt[i] == NULL)
    {
      max = i+1;
      break;
    }
  }
  i = omBackTrace_2_RetInfo(bt, info, max);
  }
#ifdef OM_TRACK_RETURN
  if (i == 0)
    i = omBackTrace_2_RetInfo(((void*)&r),info, 1);
#endif
#ifndef OM_INTERNAL_DEBUG
  if (i > 1)
  {
#ifdef OM_TRACK_RETURN
    if (r != NULL)
      omFilterRetInfo(info, i, addr_i == r);
#endif
#ifdef OM_TRACK_FILE_LINE
    if (f != NULL && l > 0)
      omFilterRetInfo(info, i, strcmp(f, file_i) == 0 && l + 2 >= line_i && l - 2 <= line_i);
#endif
    /* if we have both, use overwrite what we got from return addressse --
       they sometimes are wrong */
#if defined(OM_TRACK_RETURN) && defined(OM_TRACK_FILE_LINE)
    if (r != NULL && info[0].addr == r && l > 0 && f != 0)
    {
      strcpy(info[0].file, f);
      info[0].line = l;
    }
#endif
  }
  if (i == 0)
  {
#endif /* ! OM_INTERNAL_DEBUG */

#ifdef OM_TRACK_FILE_LINE
    fprintf(fd, " %s:%d", f, l);
#endif
#ifdef OM_TRACK_RETURN
    fprintf(fd," ra=%p", r);
#endif

#ifndef OM_INTERNAL_DEBUG
    return 1;
  }
  else
#endif /* ! OM_INTERNAL_DEBUG */
#ifdef OM_PRINT_RETURN_ADDRESS
    return omPrintRetInfo(info, i, fd, "\n  #%i at %L in %N ra=%p");
#else
    return omPrintRetInfo(info, i, fd, "\n  #%i at %L in %N");
#endif
}

int _omPrintCurrentBackTrace(FILE* fd , OM_FLR_DECL)
{
  int i;
  void* bt[OM_MAX_BACKTRACE_DEPTH];

  i = omGetBackTrace(bt, 1, OM_MAX_BACKTRACE_DEPTH);
  return _omPrintBackTrace(bt, i, fd , OM_FLR_VAL);
}
#endif /* ! OM_NDEBUG */
