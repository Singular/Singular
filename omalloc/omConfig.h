/* omConfig.h.  Generated automatically by configure.  */
/* -*-c++-*- */
/*******************************************************************
 *  File:    omConfig.h.in
 *  Purpose: configuration for omalloc  
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id: omConfig.h,v 1.2 2000-08-14 12:37:00 obachman Exp $
 *******************************************************************/
#ifndef OM_CONFIG_H
#define OM_CONFIG_H

/*******************************************************************
 * Defines which are set by configure
 ******************************************************************/
/* define if you provide an external config file to be included
   by omDefaultConfig.h */
#ifndef OM_HAVE_EXTERNAL_CONFIG_H
/* #undef OM_HAVE_EXTERNAL_CONFIG_H */
#endif
/* define if you can inline */
#ifndef OM_INLINE
#define OM_INLINE static inline
#endif
/* define for inline decl in headers */
#ifndef OM_INLINE_DECL
#define OM_INLINE_DECL static inline
#endif
/* define for inline implementation in Include*/
#ifndef OM_INLINE_IMPL
#define OM_INLINE_IMPL static inline
#endif
/* define for local inline */
#ifndef OM_INLINE_LOCAL
#define OM_INLINE_LOCAL static inline
#endif
/* defint to nothing if you do not understand const */
#ifndef const
/* #undef const */
#endif
/* define to the name of addr2line program, undef otherwise */
#ifndef OM_PROG_ADDR2LINE
#define OM_PROG_ADDR2LINE "addr2line"
#endif
/* define to the name of nm program, undef otherwise */
#ifndef OM_PROG_NM
#define OM_PROG_NM "nm"
#endif
/* define if you have popen */
#ifndef HAVE_POPEN
#define HAVE_POPEN 1
#endif
/* define if you have readlink */
#ifndef HAVE_READLINK
#define HAVE_READLINK 1
#endif
/* define if you have sbrk */
#ifndef HAVE_SBRK
#define HAVE_SBRK 1
#endif
/* define if you have getcwd */
#ifndef HAVE_GETCWD
#define HAVE_GETCWD 1
#endif
/* define if you have getwd */
#ifndef HAVE_GETWD
#define HAVE_GETWD 1
#endif
/* define if you have a working mmap */
#ifndef HAVE_WORKING_MMAP
#define HAVE_WORKING_MMAP 1
#endif
/* define if you have STDC_HEADERS */
#ifndef STDC_HEADERS
#define STDC_HEADERS 1
#endif
/* define if you have unistd.h */
#ifndef HAVE_UNISTD_H
#define HAVE_UNISTD_H 1
#endif
/* define if you have fcntl.h */
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H
#endif
/* define if you have sys/mman.h */
#ifndef HAVE_SYS_MMAN_H
#define HAVE_SYS_MMAN_H
#endif
/* define if GET_RETURN_ADDR works */
#ifndef OM_GET_RETURN_ADDR_WORK
#define OM_GET_RETURN_ADDR_WORKS 1
#endif
/* define if GET_RETURN_ADDR works */
#ifndef OM_GET_BACKTRACE_WORK
#define OM_GET_BACKTRACE_WORKS 1
#endif
/* Define sizeof(long) */
#ifndef SIZEOF_LONG
#define SIZEOF_LONG 4
#endif
/* Define sizeof(double) */
#ifndef SIZEOF_DOUBLE
#define SIZEOF_DOUBLE 8
#endif
/* Define sizeof(size_t) */
#ifndef SIZEOF_SIZE_T
#define SIZEOF_SIZE_T 4
#endif
#ifndef SIZEOF_SYSTEM_PAGE
/* Define pagesize of your system */
#define SIZEOF_SYSTEM_PAGE 4096
#endif
#ifndef SIZEOF_OM_PAGE
/* size of page as we can use it */
#define SIZEOF_OM_PAGE SIZEOF_SYSTEM_PAGE
#endif
/* define if you want to use mmap for valloc */
#ifndef OM_HAVE_VALLOC_MMAP
#define OM_HAVE_VALLOC_MMAP 1
#endif
/* define if you want to use malloc-provided valloc */
#ifndef HAVE_VALLOC_MALLOC
/* #undef OM_HAVE_VALLOC_MALLOC */
#endif
#ifndef OM_MALLOC_PROVIDES_SIZEOF_ADDR
/* define if your malloc provides a working OM_MALLOC_SIZEOF_ADDR */
#define OM_MALLOC_PROVIDES_SIZEOF_ADDR 1
#endif
#ifndef OM_EMULATE_OMALLOC
/* define if you only want to emulate omalloc, but not actually use it */
/* #undef OM_EMULATE_OMALLOC */
#endif
#ifndef OM_ALIGN_8
/* define if you want your memory to be 8-aligned */
/* #undef OM_ALIGN_8 */
#endif
#ifndef OM_ALIGNMENT_NEEDS_WORK
/* define if you insist on AllocAligned returnes 8-aligned */
/* #undef OM_ALIGNMENT_NEEDS_WORK */
#endif
/* define if you want to have a dense distribution of bins */
#ifndef OM_HAVE_DENSE_BIN_DISTRIBUTION
/* #undef OM_HAVE_DENSE_BIN_DISTRIBUTION */
#endif
/* define to disable all debugging stuff */
#ifndef OM_NDEBUG
/* #undef OM_NDEBUG */
#endif
/* define to enable debug tracking */
#ifndef OM_HAVE_TRACK
#define OM_HAVE_TRACK 1
#endif
/* define to enable tracking of files/line numbers */
#ifndef OM_TRACK_FILE_LINE
/* #undef OM_TRACK_FILE_LINE */
#endif
/* define to enable tracking of return addresses */
#ifndef OM_TRACK_RETURN
#define OM_TRACK_RETURN 1
#endif
/* define to enable tracking of stack backtraces */
#ifndef OM_TRACK_BACKTRACE
#define OM_TRACK_BACKTRACE 1
#endif
/* define to enable internal debugging */
#ifndef OM_INTERNAL_DEBUG
/* #undef OM_INTERNAL_DEBUG */
#endif
/* define to -1/0/1/2/3 if you want omalloc to provide 
   "underlying malloc"/no/normal/debug ANSI-C conforming  
   malloc/calloc/realloc/free funcs */
#ifndef OM_PROVIDE_MALLOC
#define OM_PROVIDE_MALLOC 0
#endif
#endif /* OM_CONFIG_H  */
