#ifndef PAGE_H
#define PAGE_H
#if defined(PAGE_TEST)
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/times.h>

void * mmPage_Create(size_t size);
void mmPage_Delete(void * address);

void mmPage_AllowAccess(void * address);
void mmPage_DenyAccess(void * address);
void mmWriteStat();
void mmEndStat();
#define MAX_PAGE_TAB 4096
#define MM_STAT_FILE "mem_stat.txt"

extern unsigned long mmPage_tab[MAX_PAGE_TAB];
extern char          mmUse_tab [MAX_PAGE_TAB];
extern int           mmPage_tab_ind;
extern int           mmPage_tab_acc;
#ifdef __cplusplus
}
#endif
#endif
#endif
