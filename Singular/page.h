#ifndef PAGE_H
#define PAGE_H
#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <sys/types.h>
#include <sys/times.h>

void * Page_Create(size_t size);
void Page_AllowAccess(void * address, size_t size);
void Page_DenyAccess(void * address, size_t size);
size_t Page_Size(void);
extern unsigned long page_tab[2048];
extern char          use_tab [2048];
extern int           page_tab_ind;
#ifdef __cplusplus
}
#endif
#endif
