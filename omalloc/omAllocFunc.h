/*******************************************************************
 *  File:    omAllocFunc.h
 *  Purpose: declaration of ANSI-C conforming malloc functions
 *           which are sure to be functions, which start with the om prefix,
 *           and end with the Func suffix.
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omAllocFunc.h,v 1.2 2001-04-05 10:30:39 Singular Exp $
 *******************************************************************/

void* omCallocFunc(size_t nmemb, size_t size);
void* omMallocFunc(size_t size);
void omFreeFunc(void* addr);
void* omVallocFunc(size_t size);
#if defined(sgi)
void* memalign(size_t size_1, size_t size_2);
#else
void* memalign(void* alignment, size_t size);
#endif
void* omReallocFunc(void* old_addr, size_t new_size);
char* omStrdupFunc(const char* addr);
void* omReallocSizeFunc(void* old_addr, size_t old_size, size_t new_size);
void omFreeSizeFunc(void* addr, size_t size);

