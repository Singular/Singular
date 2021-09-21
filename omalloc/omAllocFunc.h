/*******************************************************************
 *  File:    omAllocFunc.h
 *  Purpose: declaration of ANSI-C conforming malloc functions
 *           which are sure to be functions, which start with the om prefix,
 *           and end with the Func suffix.
 *  Author:  obachman@mathematik.uni-kl.de (Olaf Bachmann)
 *  Created: 11/99
 *******************************************************************/

void* omCallocFunc(size_t nmemb, size_t size);
void* omMallocFunc(size_t size);
void omFreeFunc(void* addr);
void* omVallocFunc(size_t size);
void* omReallocFunc(void* old_addr, size_t new_size);
char* omStrdupFunc(const char* addr);
void* omReallocSizeFunc(void* old_addr, size_t old_size, size_t new_size);
void omFreeSizeFunc(void* addr, size_t size);
