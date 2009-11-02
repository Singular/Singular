/*******************************************************************
 *  File:    omDerivedConfig.h.in
 *  Purpose: configuration which are derived from omConfig.h
 *  Author:  obachman (Olaf Bachmann)
 *  Created: 11/99
 *  Version: $Id$
 *******************************************************************/
#ifndef OM_DERIVED_CONFIG_H
#define OM_DERIVED_CONFIG_H

#ifndef SIZEOF_VOIDP
/* configure makes sure that SIZEOF_VOIDP == SIZEOF_LONG */
#define SIZEOF_VOIDP SIZEOF_LONG
#endif

/* SIZEOF_VOIDP == 8 || SIZEOF_VOIDP == 4 checked by configure */
#if SIZEOF_VOIDP == 8
#define LOG_SIZEOF_LONG  3
#define LOG_SIZEOF_VOIDP 3
#define LOG_BIT_SIZEOF_LONG 6
#else
#define LOG_SIZEOF_LONG  2
#define LOG_SIZEOF_VOIDP 2
#define LOG_BIT_SIZEOF_LONG 5
#endif

/* SIZEOF_SYSTEM_PAGE == 4096 || SIZEOF_SYSTEM_PAGE == 8192 checked by configure */
#if SIZEOF_SYSTEM_PAGE == 8192
#define LOG_BIT_SIZEOF_SYSTEM_PAGE 13
#else
#define LOG_BIT_SIZEOF_SYSTEM_PAGE 12
#endif

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif

#ifndef BIT_SIZEOF_LONG
#define BIT_SIZEOF_LONG (CHAR_BIT << LOG_SIZEOF_LONG)
#endif

#ifdef OM_ALIGN_8
#define SIZEOF_OM_ALIGNMENT 8
#define SIZEOF_OM_ALIGNMENT_1 7
#define LOG_SIZEOF_OM_ALIGNMENT 3
#define SIZEOF_STRICT_ALIGNMENT 8
#else
#define SIZEOF_OM_ALIGNMENT 4
#define SIZEOF_OM_ALIGNMENT_1 3
#define LOG_SIZEOF_OM_ALIGNMENT 2
#ifdef OM_ALIGNMENT_NEEDS_WORK
#define SIZEOF_STRICT_ALIGNMENT 8
#else
#define SIZEOF_STRICT_ALIGNMENT 4
#endif
#endif

#define OM_ALIGN_SIZE(size) \
   ((((unsigned long) size) + SIZEOF_OM_ALIGNMENT_1) & (~SIZEOF_OM_ALIGNMENT_1))

#define OM_IS_ALIGNED(what) \
   ((((unsigned long) what) & SIZEOF_OM_ALIGNMENT_1) == 0)

#define OM_STRICT_ALIGN_SIZE(size)                                          \
   ((((size_t) (size)) + SIZEOF_STRICT_ALIGNMENT - 1) & (~ (SIZEOF_STRICT_ALIGNMENT - 1)))

#define OM_IS_STRICT_ALIGNED(what) \
   ((((unsigned long) what) & (SIZEOF_STRICT_ALIGNMENT -1)) == 0)

#if defined(OM_NDEBUG) && defined(OM_HAVE_TRACK)
#undef OM_HAVE_TRACK
#endif

/* define to enable assume */
#ifndef HAVE_OM_ASSUME
#if defined(OM_INTERNAL_DEBUG)
#define HAVE_OM_ASSUME
#endif
#endif

/* set to 0 to disable aso memory mamagent */
#ifndef HAVE_ASO
#define HAVE_ASO 1
#endif
#if defined(HAVE_ASO) && HAVE_ASO == 1
/* define to enable ASO debugging */
#undef ASO_DEBUG
#endif

#ifndef NULL
#define NULL ((void*)0)
#endif

#if !defined(OM_TRACK_FILE_LINE) && ! defined(OM_TRACK_RETURN)
#define OM_TRACK_FILE_LINE
#endif

/* The following macros save a lot of typing */
#ifdef OM_TRACK_FILE_LINE
#define OM_FL_DECL      const char* f, const int l
#define OM_FL_VAL       f, l
#define OM_FL           __FILE__,__LINE__
#define OM_FL_KOMMA     ,
#else
#define OM_FL_DECL
#define OM_FL_VAL
#define OM_FL
#define OM_FL_KOMMA
#endif

#define OM_CFL_DECL     char check OM_FL_KOMMA OM_FL_DECL
#define OM_CFL_VAL      check OM_FL_KOMMA OM_FL_VAL
#define OM_CFL          OM_CHECK OM_FL_KOMMA OM_FL

#define OM_TFL_DECL     char track OM_FL_KOMMA OM_FL_DECL
#define OM_TFL_VAL      track OM_FL_KOMMA OM_FL_VAL
#define OM_TFL          OM_TRACK OM_FL_KOMMA OM_FL

#define OM_CTFL_DECL    char check, char track OM_FL_KOMMA OM_FL_DECL
#define OM_CTFL_VAL     check,track OM_FL_KOMMA OM_FL_VAL
#define OM_CTFL         OM_CHECK,OM_TRACK OM_FL_KOMMA OM_FL

#ifdef OM_TRACK_RETURN
#define OM_FLR_DECL         OM_FL_DECL OM_FL_KOMMA const void* r
#define OM_FLR_VAL          OM_FL_VAL OM_FL_KOMMA r
#define OM_FLR              OM_FL OM_FL_KOMMA 0
#define OM_R_DEF            void* r; GET_RET_ADDR(r)
#ifdef OM_TRACK_FILE_LINE
#define OM_FLR_ARG(f,l,r)   f,l,r
#else
#define OM_FLR_ARG(f,l,r)   r
#endif

#else

#define OM_FLR_DECL OM_FL_DECL
#define OM_FLR_VAL  OM_FL_VAL
#define OM_FLR      OM_FL
#define OM_R_DEF    ((void)0)
#define OM_FLR_ARG(f,l,r)   f,l
#endif

#endif /* OM_DERIVED_CONFIG_H  */
