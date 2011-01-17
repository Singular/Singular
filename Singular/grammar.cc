/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     DOTDOT = 258,
     EQUAL_EQUAL = 259,
     GE = 260,
     LE = 261,
     MINUSMINUS = 262,
     NOT = 263,
     NOTEQUAL = 264,
     PLUSPLUS = 265,
     COLONCOLON = 266,
     GRING_CMD = 267,
     INTMAT_CMD = 268,
     PROC_CMD = 269,
     RING_CMD = 270,
     BEGIN_RING = 271,
     IDEAL_CMD = 272,
     MAP_CMD = 273,
     MATRIX_CMD = 274,
     MODUL_CMD = 275,
     NUMBER_CMD = 276,
     POLY_CMD = 277,
     RESOLUTION_CMD = 278,
     VECTOR_CMD = 279,
     BETTI_CMD = 280,
     CHINREM_CMD = 281,
     COEFFS_CMD = 282,
     COEF_CMD = 283,
     CONTRACT_CMD = 284,
     DEGREE_CMD = 285,
     DEG_CMD = 286,
     DIFF_CMD = 287,
     DIM_CMD = 288,
     DIVISION_CMD = 289,
     ELIMINATION_CMD = 290,
     E_CMD = 291,
     FAREY_CMD = 292,
     FETCH_CMD = 293,
     FREEMODULE_CMD = 294,
     KEEPRING_CMD = 295,
     HILBERT_CMD = 296,
     HOMOG_CMD = 297,
     IMAP_CMD = 298,
     INDEPSET_CMD = 299,
     INTERRED_CMD = 300,
     INTERSECT_CMD = 301,
     JACOB_CMD = 302,
     JET_CMD = 303,
     KBASE_CMD = 304,
     KOSZUL_CMD = 305,
     LEADCOEF_CMD = 306,
     LEADEXP_CMD = 307,
     LEAD_CMD = 308,
     LEADMONOM_CMD = 309,
     LIFTSTD_CMD = 310,
     LIFT_CMD = 311,
     MAXID_CMD = 312,
     MINBASE_CMD = 313,
     MINOR_CMD = 314,
     MINRES_CMD = 315,
     MODULO_CMD = 316,
     MONOM_CMD = 317,
     MRES_CMD = 318,
     MULTIPLICITY_CMD = 319,
     ORD_CMD = 320,
     PAR_CMD = 321,
     PARDEG_CMD = 322,
     PREIMAGE_CMD = 323,
     QUOTIENT_CMD = 324,
     QHWEIGHT_CMD = 325,
     REDUCE_CMD = 326,
     REGULARITY_CMD = 327,
     RES_CMD = 328,
     SIMPLIFY_CMD = 329,
     SORTVEC_CMD = 330,
     SRES_CMD = 331,
     STD_CMD = 332,
     SUBST_CMD = 333,
     SYZYGY_CMD = 334,
     VAR_CMD = 335,
     VDIM_CMD = 336,
     WEDGE_CMD = 337,
     WEIGHT_CMD = 338,
     VALTVARS = 339,
     VMAXDEG = 340,
     VMAXMULT = 341,
     VNOETHER = 342,
     VMINPOLY = 343,
     END_RING = 344,
     CMD_1 = 345,
     CMD_2 = 346,
     CMD_3 = 347,
     CMD_12 = 348,
     CMD_13 = 349,
     CMD_23 = 350,
     CMD_123 = 351,
     CMD_M = 352,
     ROOT_DECL = 353,
     ROOT_DECL_LIST = 354,
     RING_DECL = 355,
     EXAMPLE_CMD = 356,
     EXPORT_CMD = 357,
     HELP_CMD = 358,
     KILL_CMD = 359,
     LIB_CMD = 360,
     LISTVAR_CMD = 361,
     SETRING_CMD = 362,
     TYPE_CMD = 363,
     STRINGTOK = 364,
     BLOCKTOK = 365,
     INT_CONST = 366,
     UNKNOWN_IDENT = 367,
     RINGVAR = 368,
     PROC_DEF = 369,
     BREAK_CMD = 370,
     CONTINUE_CMD = 371,
     ELSE_CMD = 372,
     EVAL = 373,
     QUOTE = 374,
     FOR_CMD = 375,
     IF_CMD = 376,
     SYS_BREAK = 377,
     WHILE_CMD = 378,
     RETURN = 379,
     PARAMETER = 380,
     SYSVAR = 381,
     UMINUS = 382
   };
#endif
/* Tokens.  */
#define DOTDOT 258
#define EQUAL_EQUAL 259
#define GE 260
#define LE 261
#define MINUSMINUS 262
#define NOT 263
#define NOTEQUAL 264
#define PLUSPLUS 265
#define COLONCOLON 266
#define GRING_CMD 267
#define INTMAT_CMD 268
#define PROC_CMD 269
#define RING_CMD 270
#define BEGIN_RING 271
#define IDEAL_CMD 272
#define MAP_CMD 273
#define MATRIX_CMD 274
#define MODUL_CMD 275
#define NUMBER_CMD 276
#define POLY_CMD 277
#define RESOLUTION_CMD 278
#define VECTOR_CMD 279
#define BETTI_CMD 280
#define CHINREM_CMD 281
#define COEFFS_CMD 282
#define COEF_CMD 283
#define CONTRACT_CMD 284
#define DEGREE_CMD 285
#define DEG_CMD 286
#define DIFF_CMD 287
#define DIM_CMD 288
#define DIVISION_CMD 289
#define ELIMINATION_CMD 290
#define E_CMD 291
#define FAREY_CMD 292
#define FETCH_CMD 293
#define FREEMODULE_CMD 294
#define KEEPRING_CMD 295
#define HILBERT_CMD 296
#define HOMOG_CMD 297
#define IMAP_CMD 298
#define INDEPSET_CMD 299
#define INTERRED_CMD 300
#define INTERSECT_CMD 301
#define JACOB_CMD 302
#define JET_CMD 303
#define KBASE_CMD 304
#define KOSZUL_CMD 305
#define LEADCOEF_CMD 306
#define LEADEXP_CMD 307
#define LEAD_CMD 308
#define LEADMONOM_CMD 309
#define LIFTSTD_CMD 310
#define LIFT_CMD 311
#define MAXID_CMD 312
#define MINBASE_CMD 313
#define MINOR_CMD 314
#define MINRES_CMD 315
#define MODULO_CMD 316
#define MONOM_CMD 317
#define MRES_CMD 318
#define MULTIPLICITY_CMD 319
#define ORD_CMD 320
#define PAR_CMD 321
#define PARDEG_CMD 322
#define PREIMAGE_CMD 323
#define QUOTIENT_CMD 324
#define QHWEIGHT_CMD 325
#define REDUCE_CMD 326
#define REGULARITY_CMD 327
#define RES_CMD 328
#define SIMPLIFY_CMD 329
#define SORTVEC_CMD 330
#define SRES_CMD 331
#define STD_CMD 332
#define SUBST_CMD 333
#define SYZYGY_CMD 334
#define VAR_CMD 335
#define VDIM_CMD 336
#define WEDGE_CMD 337
#define WEIGHT_CMD 338
#define VALTVARS 339
#define VMAXDEG 340
#define VMAXMULT 341
#define VNOETHER 342
#define VMINPOLY 343
#define END_RING 344
#define CMD_1 345
#define CMD_2 346
#define CMD_3 347
#define CMD_12 348
#define CMD_13 349
#define CMD_23 350
#define CMD_123 351
#define CMD_M 352
#define ROOT_DECL 353
#define ROOT_DECL_LIST 354
#define RING_DECL 355
#define EXAMPLE_CMD 356
#define EXPORT_CMD 357
#define HELP_CMD 358
#define KILL_CMD 359
#define LIB_CMD 360
#define LISTVAR_CMD 361
#define SETRING_CMD 362
#define TYPE_CMD 363
#define STRINGTOK 364
#define BLOCKTOK 365
#define INT_CONST 366
#define UNKNOWN_IDENT 367
#define RINGVAR 368
#define PROC_DEF 369
#define BREAK_CMD 370
#define CONTINUE_CMD 371
#define ELSE_CMD 372
#define EVAL 373
#define QUOTE 374
#define FOR_CMD 375
#define IF_CMD 376
#define SYS_BREAK 377
#define WHILE_CMD 378
#define RETURN 379
#define PARAMETER 380
#define SYSVAR 381
#define UMINUS 382




/* Copy the first part of user declarations.  */
#line 8 "grammar.y"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <kernel/mod2.h>
#include <omalloc/mylimits.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <kernel/options.h>
#include <Singular/stype.h>
#include <Singular/ipid.h>
#include <kernel/intvec.h>
#include <kernel/febase.h>
#include <kernel/matpol.h>
#include <kernel/ring.h>
#include <kernel/kstd1.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>
#include <Singular/sdb.h>
#include <kernel/ideals.h>
#include <kernel/numbers.h>
#include <kernel/polys.h>
#include <kernel/stairc.h>
#include <kernel/timer.h>
#include <Singular/cntrlc.h>
#include <kernel/maps.h>
#include <kernel/syz.h>
#include <Singular/lists.h>
#include <kernel/longrat.h>
#include <Singular/libparse.h>

#if 0
void debug_list(leftv v)
{
  idhdl r=basePackHdl;
  idhdl h;
  BOOLEAN found=FALSE;
  const char *nn=v->name;
  h=IDROOT->get(nn,myynest);
  if (h!=NULL)
  {
     Print("Curr::%s, (%s)\n",nn,Tok2Cmdname((int)IDTYP(h)));
     found=TRUE;
  }
  else         Print("`%s` not found in IDROOT\n",nn);
  while (r!=NULL)
  {
    if ((IDTYP(r)==PACKAGE_CMD)
    || (IDTYP(r)==RING_CMD)
    || (IDTYP(r)==QRING_CMD))
    {
      h=IDPACKAGE(r)->idroot->get(nn,myynest);
      if (h!=NULL)
      {
        Print("%s::%s, (%s)\n",r->id,nn,Tok2Cmdname((int)IDTYP(h)));
        found=TRUE;
      }
      else         Print("%s::%s not found\n",r->id,nn);
    }
    if (r==basePackHdl) r=IDPACKAGE(r)->idroot;
    r=r->next;
   if (r==basePackHdl) break;
  }
  if (!found)
  {
    listall(TRUE);
  }
}
#endif

/* From the bison docu:

     By defining the macro `YYMAXDEPTH', you can control how deep the
parser stack can become before a stack overflow occurs.  Define the
macro with a value that is an integer.  This value is the maximum number
of tokens that can be shifted (and not reduced) before overflow.  It
must be a constant expression whose value is known at compile time.

   The stack space allowed is not necessarily allocated.  If you
specify a large value for `YYMAXDEPTH', the parser actually allocates a
small stack at first, and then makes it bigger by stages as needed.
This increasing allocation happens automatically and silently.
Therefore, you do not need to make `YYMAXDEPTH' painfully small merely
to save space for ordinary inputs that do not need much stack.

   The default value of `YYMAXDEPTH', if you do not define it, is 10000.
*/
#define YYMAXDEPTH INT_MAX

extern int   yylineno;
extern FILE* yyin;

char       my_yylinebuf[80];
const  char *  currid;
BOOLEAN    yyInRingConstruction=FALSE;
BOOLEAN    expected_parms;
int        cmdtok;
int        inerror = 0;

#define TESTSETINT(a,i)                                \
   if ((a).Typ() != INT_CMD)                           \
   {                                                   \
     WerrorS("no int expression");                     \
     YYERROR;                                          \
   }                                                   \
   (i) = (int)((long)(a).Data());

#define MYYERROR(a) { WerrorS(a); YYERROR; }

void yyerror(const char * fmt)
{

  BOOLEAN old_errorreported=errorreported;
  errorreported = TRUE;
  if (currid!=NULL)
  {
    killid(currid,&IDROOT);
    currid = NULL;
  }
  if(inerror==0)
  {
    {
      if ((strlen(fmt)>1)
      && (strncmp(fmt,"parse",5)!=0)
      && (strncmp(fmt,"syntax",6)!=0))
        WerrorS(fmt);
      Werror( "error occurred in or before %s line %d: `%s`"
             ,VoiceName(), yylineno, my_yylinebuf);
    }
    if (cmdtok!=0)
    {
      const char *s=Tok2Cmdname(cmdtok);
      if (expected_parms)
      {
        Werror("expected %s-expression. type \'help %s;\'",s,s);
      }
      else
      {
        Werror("wrong type declaration. type \'help %s;\'",s);
      }
    }
    if (!old_errorreported && (lastreserved!=NULL))
    {
      Werror("last reserved name was `%s`",lastreserved);
    }
    inerror=1;
  }
  if ((currentVoice!=NULL)
  && (currentVoice->prev!=NULL)
  && (myynest>0)
#ifdef HAVE_SDB
  && ((sdb_flags &1)==0)
#endif
  )
  {
    Werror("leaving %s",VoiceName());
  }
  #ifdef HAVE_FACTORY
  // libfac:
  extern int libfac_interruptflag;
  libfac_interruptflag=0;
  #endif
}



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* Enabling the token table.  */
#ifndef YYTOKEN_TABLE
# define YYTOKEN_TABLE 0
#endif

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif



/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 531 "grammar.cc"

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#elif (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
typedef signed char yytype_int8;
#else
typedef short int yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(e) ((void) (e))
#else
# define YYUSE(e) /* empty */
#endif

/* Identity function, used to suppress warnings about constant conditions.  */
#ifndef lint
# define YYID(n) (n)
#else
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static int
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
}
#endif

#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#     ifndef _STDLIB_H
#      define _STDLIB_H 1
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's `empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (YYID (0))
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined _STDLIB_H \
       && ! ((defined YYMALLOC || defined malloc) \
	     && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef _STDLIB_H
#    define _STDLIB_H 1
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined _STDLIB_H && (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
	 || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
  };

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

/* Copy COUNT objects from FROM to TO.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(To, From, Count) \
      __builtin_memcpy (To, From, (Count) * sizeof (*(From)))
#  else
#   define YYCOPY(To, From, Count)		\
      do					\
	{					\
	  YYSIZE_T yyi;				\
	  for (yyi = 0; yyi < (Count); yyi++)	\
	    (To)[yyi] = (From)[yyi];		\
	}					\
      while (YYID (0))
#  endif
# endif

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1969

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  145
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  168
/* YYNRULES -- Number of states.  */
#define YYNSTATES  373

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   382

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   138,     2,
     142,   143,   140,   130,   136,   131,     2,   132,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   139,   137,
     128,   127,   129,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   133,     2,   134,   135,     2,   144,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   104,
     105,   106,   107,   108,   109,   110,   111,   112,   113,   114,
     115,   116,   117,   118,   119,   120,   121,   122,   123,   124,
     125,   126,   141
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    15,    17,    19,
      21,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      43,    45,    47,    49,    51,    53,    55,    57,    59,    61,
      64,    66,    68,    72,    76,    81,    85,    87,    89,    91,
      96,   100,   102,   104,   106,   110,   117,   122,   127,   132,
     136,   141,   146,   150,   155,   162,   171,   178,   187,   192,
     197,   204,   209,   216,   225,   234,   238,   243,   252,   257,
     266,   271,   280,   285,   289,   295,   296,   302,   305,   307,
     310,   313,   317,   321,   325,   329,   333,   337,   341,   345,
     349,   353,   356,   359,   362,   365,   367,   371,   373,   375,
     378,   381,   384,   387,   396,   399,   408,   411,   415,   418,
     420,   422,   428,   430,   432,   437,   439,   443,   445,   449,
     451,   452,   457,   461,   464,   468,   471,   474,   478,   483,
     488,   493,   498,   503,   508,   513,   518,   523,   530,   537,
     544,   551,   558,   565,   572,   579,   583,   585,   594,   597,
     600,   602,   604,   607,   610,   612,   618,   621,   627,   629,
     631,   635,   641,   645,   649,   654,   657,   660,   665
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     146,     0,    -1,    -1,   146,   147,    -1,   148,    -1,   150,
     137,    -1,   162,   137,    -1,   188,    -1,   122,    -1,   137,
      -1,     1,   137,    -1,   183,    -1,   184,    -1,   149,    -1,
     185,    -1,   186,    -1,   170,    -1,   172,    -1,   173,    -1,
     101,   110,    -1,   151,    -1,   174,    -1,   175,    -1,   176,
      -1,   187,    -1,   178,    -1,   179,    -1,   181,    -1,   182,
      -1,   159,   153,    -1,   113,    -1,   160,    -1,   152,    11,
     152,    -1,   152,   142,   143,    -1,   152,   142,   153,   143,
      -1,   133,   153,   134,    -1,   111,    -1,   126,    -1,   163,
      -1,    14,   142,   154,   143,    -1,   153,   136,   154,    -1,
     154,    -1,   158,    -1,   152,    -1,   142,   153,   143,    -1,
     154,   133,   154,   136,   154,   134,    -1,   154,   133,   154,
     134,    -1,    98,   142,   154,   143,    -1,    99,   142,   153,
     143,    -1,    99,   142,   143,    -1,   100,   142,   154,   143,
      -1,   161,   142,   153,   143,    -1,   161,   142,   143,    -1,
      90,   142,   154,   143,    -1,    91,   142,   154,   136,   154,
     143,    -1,    92,   142,   154,   136,   154,   136,   154,   143,
      -1,    95,   142,   154,   136,   154,   143,    -1,    95,   142,
     154,   136,   154,   136,   154,   143,    -1,    93,   142,   154,
     143,    -1,    94,   142,   154,   143,    -1,    93,   142,   154,
     136,   154,   143,    -1,    96,   142,   154,   143,    -1,    96,
     142,   154,   136,   154,   143,    -1,    94,   142,   154,   136,
     154,   136,   154,   143,    -1,    96,   142,   154,   136,   154,
     136,   154,   143,    -1,    97,   142,   143,    -1,    97,   142,
     153,   143,    -1,    19,   142,   154,   136,   154,   136,   154,
     143,    -1,    19,   142,   154,   143,    -1,    13,   142,   154,
     136,   154,   136,   154,   143,    -1,    13,   142,   154,   143,
      -1,    15,   142,   164,   136,   164,   136,   168,   143,    -1,
      15,   142,   154,   143,    -1,   156,   154,   157,    -1,   156,
     154,   127,   154,   157,    -1,    -1,   118,   142,   155,   154,
     143,    -1,   119,   142,    -1,   143,    -1,   154,    10,    -1,
     154,     7,    -1,   154,   130,   154,    -1,   154,   131,   154,
      -1,   154,   132,   154,    -1,   154,   135,   154,    -1,   154,
     128,   154,    -1,   154,   138,   154,    -1,   154,     9,   154,
      -1,   154,     4,   154,    -1,   154,     3,   154,    -1,   154,
     139,   154,    -1,     8,   154,    -1,   131,   154,    -1,   162,
     169,    -1,   153,   127,    -1,   112,    -1,   144,   154,   144,
      -1,    17,    -1,    20,    -1,    98,   152,    -1,    99,   152,
      -1,   100,   152,    -1,   161,   152,    -1,    19,   152,   133,
     154,   134,   133,   154,   134,    -1,    19,   152,    -1,    13,
     152,   133,   154,   134,   133,   154,   134,    -1,    13,   152,
      -1,   162,   136,   152,    -1,    14,   152,    -1,   109,    -1,
     154,    -1,   142,   154,   136,   153,   143,    -1,   112,    -1,
     165,    -1,   165,   142,   153,   143,    -1,   166,    -1,   166,
     136,   167,    -1,   166,    -1,   142,   167,   143,    -1,   127,
      -1,    -1,   128,   163,   171,   137,    -1,   103,   109,   137,
      -1,   103,   137,    -1,   101,   109,   137,    -1,   102,   153,
      -1,   104,   152,    -1,   175,   136,   152,    -1,   106,   142,
      98,   143,    -1,   106,   142,    99,   143,    -1,   106,   142,
     100,   143,    -1,   106,   142,   161,   143,    -1,   106,   142,
      15,   143,    -1,   106,   142,    19,   143,    -1,   106,   142,
      13,   143,    -1,   106,   142,    14,   143,    -1,   106,   142,
     152,   143,    -1,   106,   142,   152,   136,    98,   143,    -1,
     106,   142,   152,   136,    99,   143,    -1,   106,   142,   152,
     136,   100,   143,    -1,   106,   142,   152,   136,   161,   143,
      -1,   106,   142,   152,   136,    15,   143,    -1,   106,   142,
     152,   136,    19,   143,    -1,   106,   142,   152,   136,    13,
     143,    -1,   106,   142,   152,   136,    14,   143,    -1,   106,
     142,   143,    -1,    15,    -1,   177,   152,   169,   164,   136,
     164,   136,   168,    -1,   177,   152,    -1,   126,   163,    -1,
     107,    -1,    40,    -1,   180,   154,    -1,   108,   154,    -1,
     153,    -1,   121,   142,   154,   143,   110,    -1,   117,   110,
      -1,   121,   142,   154,   143,   115,    -1,   115,    -1,   116,
      -1,   123,   109,   110,    -1,   120,   109,   109,   109,   110,
      -1,    14,   160,   110,    -1,   114,   109,   110,    -1,   114,
     109,   109,   110,    -1,   125,   162,    -1,   125,   154,    -1,
     124,   142,   153,   143,    -1,   124,   142,   143,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   357,   357,   359,   393,   394,   396,   398,   402,   407,
     409,   460,   461,   462,   463,   464,   465,   466,   467,   471,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   485,
     492,   497,   501,   505,   509,   522,   550,   574,   580,   586,
     593,   604,   610,   615,   616,   617,   621,   625,   629,   633,
     637,   641,   645,   649,   653,   657,   661,   665,   669,   673,
     677,   681,   685,   689,   693,   697,   701,   705,   709,   713,
     717,   721,   725,   729,   733,   751,   750,   768,   776,   785,
     789,   793,   797,   801,   805,   809,   813,   817,   821,   825,
     829,   833,   840,   847,   848,   867,   868,   880,   880,   885,
     890,   895,   899,   903,   919,   923,   939,   953,   974,   982,
     986,   987,  1001,  1009,  1018,  1063,  1064,  1073,  1074,  1080,
    1093,  1092,  1100,  1105,  1112,  1120,  1132,  1148,  1167,  1171,
    1175,  1180,  1184,  1188,  1192,  1196,  1200,  1205,  1211,  1217,
    1223,  1229,  1235,  1241,  1247,  1259,  1266,  1270,  1308,  1318,
    1331,  1331,  1334,  1406,  1412,  1441,  1454,  1471,  1480,  1485,
    1493,  1505,  1524,  1535,  1555,  1579,  1585,  1597,  1604
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "GRING_CMD",
  "INTMAT_CMD", "PROC_CMD", "RING_CMD", "BEGIN_RING", "IDEAL_CMD",
  "MAP_CMD", "MATRIX_CMD", "MODUL_CMD", "NUMBER_CMD", "POLY_CMD",
  "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD", "CHINREM_CMD", "COEFFS_CMD",
  "COEF_CMD", "CONTRACT_CMD", "DEGREE_CMD", "DEG_CMD", "DIFF_CMD",
  "DIM_CMD", "DIVISION_CMD", "ELIMINATION_CMD", "E_CMD", "FAREY_CMD",
  "FETCH_CMD", "FREEMODULE_CMD", "KEEPRING_CMD", "HILBERT_CMD",
  "HOMOG_CMD", "IMAP_CMD", "INDEPSET_CMD", "INTERRED_CMD", "INTERSECT_CMD",
  "JACOB_CMD", "JET_CMD", "KBASE_CMD", "KOSZUL_CMD", "LEADCOEF_CMD",
  "LEADEXP_CMD", "LEAD_CMD", "LEADMONOM_CMD", "LIFTSTD_CMD", "LIFT_CMD",
  "MAXID_CMD", "MINBASE_CMD", "MINOR_CMD", "MINRES_CMD", "MODULO_CMD",
  "MONOM_CMD", "MRES_CMD", "MULTIPLICITY_CMD", "ORD_CMD", "PAR_CMD",
  "PARDEG_CMD", "PREIMAGE_CMD", "QUOTIENT_CMD", "QHWEIGHT_CMD",
  "REDUCE_CMD", "REGULARITY_CMD", "RES_CMD", "SIMPLIFY_CMD", "SORTVEC_CMD",
  "SRES_CMD", "STD_CMD", "SUBST_CMD", "SYZYGY_CMD", "VAR_CMD", "VDIM_CMD",
  "WEDGE_CMD", "WEIGHT_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT", "VNOETHER",
  "VMINPOLY", "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12", "CMD_13",
  "CMD_23", "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST", "RING_DECL",
  "EXAMPLE_CMD", "EXPORT_CMD", "HELP_CMD", "KILL_CMD", "LIB_CMD",
  "LISTVAR_CMD", "SETRING_CMD", "TYPE_CMD", "STRINGTOK", "BLOCKTOK",
  "INT_CONST", "UNKNOWN_IDENT", "RINGVAR", "PROC_DEF", "BREAK_CMD",
  "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD", "IF_CMD",
  "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR", "'='", "'<'",
  "'>'", "'+'", "'-'", "'/'", "'['", "']'", "'^'", "','", "';'", "'&'",
  "':'", "'*'", "UMINUS", "'('", "')'", "'`'", "$accept", "lines",
  "pprompt", "flowctrl", "example_dummy", "command", "assign", "elemexpr",
  "exprlist", "expr", "@1", "quote_start", "quote_end", "expr_arithmetic",
  "left_value", "extendedid", "currring_lists", "declare_ip_variable",
  "stringexpr", "rlist", "ordername", "orderelem", "OrderingList",
  "ordering", "cmdeq", "filecmd", "@2", "helpcmd", "examplecmd",
  "exportcmd", "killcmd", "listcmd", "ringcmd1", "ringcmd", "scriptcmd",
  "setrings", "setringcmd", "typecmd", "ifcmd", "whilecmd", "forcmd",
  "proccmd", "parametercmd", "returncmd", 0
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[YYLEX-NUM] -- Internal token number corresponding to
   token YYLEX-NUM.  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,   327,   328,   329,   330,   331,   332,   333,   334,
     335,   336,   337,   338,   339,   340,   341,   342,   343,   344,
     345,   346,   347,   348,   349,   350,   351,   352,   353,   354,
     355,   356,   357,   358,   359,   360,   361,   362,   363,   364,
     365,   366,   367,   368,   369,   370,   371,   372,   373,   374,
     375,   376,   377,   378,   379,   380,   381,    61,    60,    62,
      43,    45,    47,    91,    93,    94,    44,    59,    38,    58,
      42,   382,    40,    41,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   145,   146,   146,   147,   147,   147,   147,   147,   147,
     147,   148,   148,   148,   148,   148,   148,   148,   148,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   151,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     153,   153,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   155,   154,   156,   157,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   159,   159,   160,   160,   161,   161,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   163,
     164,   164,   165,   166,   166,   167,   167,   168,   168,   169,
     171,   170,   172,   172,   173,   174,   175,   175,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   177,   178,   178,   179,
     180,   180,   181,   182,   182,   183,   183,   183,   183,   183,
     184,   185,   186,   186,   186,   187,   187,   188,   188
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     4,     3,     1,     1,     1,     4,
       3,     1,     1,     1,     3,     6,     4,     4,     4,     3,
       4,     4,     3,     4,     6,     8,     6,     8,     4,     4,
       6,     4,     6,     8,     8,     3,     4,     8,     4,     8,
       4,     8,     4,     3,     5,     0,     5,     2,     1,     2,
       2,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     2,     1,     3,     1,     1,     2,
       2,     2,     2,     8,     2,     8,     2,     3,     2,     1,
       1,     5,     1,     1,     4,     1,     3,     1,     3,     1,
       0,     4,     3,     2,     3,     2,     2,     3,     4,     4,
       4,     4,     4,     4,     4,     4,     4,     6,     6,     6,
       6,     6,     6,     6,     6,     3,     1,     8,     2,     2,
       1,     1,     2,     2,     1,     5,     2,     5,     1,     1,
       3,     5,     3,     3,     4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,   146,    97,     0,
      98,   151,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   150,     0,
     109,    36,    95,    30,     0,   158,   159,     0,     0,     0,
       0,     0,     8,     0,     0,     0,    37,     0,     0,     0,
       9,     0,     0,     3,     4,    13,     0,    20,    43,   154,
      41,     0,    42,     0,    31,     0,     0,    38,    16,    17,
      18,    21,    22,    23,     0,    25,    26,     0,    27,    28,
      11,    12,    14,    15,    24,     7,    10,     0,     0,     0,
       0,     0,     0,     0,    37,    91,     0,     0,   106,     0,
     108,    31,     0,     0,   104,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    99,     0,   100,     0,   101,     0,
      19,   125,     0,   123,   126,     0,   153,     0,   156,    75,
      77,     0,     0,     0,     0,     0,   166,   165,   149,   120,
      92,     0,     0,     0,     5,     0,     0,    94,     0,     0,
       0,    80,     0,    79,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,     0,   102,   119,     0,     6,    93,
       0,   148,   152,     0,     0,     0,   162,     0,   110,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    65,
       0,     0,    49,     0,     0,   124,   122,     0,     0,     0,
       0,     0,     0,     0,   145,     0,     0,     0,   163,     0,
       0,     0,   160,   168,     0,     0,    35,    44,    96,    32,
      33,     0,    40,    89,    88,    87,    85,    81,    82,    83,
       0,    84,    86,    90,     0,    78,    73,    52,     0,   107,
     127,     0,     0,    70,     0,    39,    41,    72,     0,     0,
      68,     0,    53,     0,     0,     0,    58,     0,    59,     0,
       0,    61,    66,    47,    48,    50,   134,   135,   132,   133,
     128,   129,   130,     0,   136,   131,   164,     0,     0,     0,
     167,   121,    34,    46,     0,     0,    51,   110,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    76,
     161,   155,   157,     0,    74,     0,     0,     0,     0,     0,
       0,     0,    54,     0,    60,     0,     0,    56,     0,    62,
     143,   144,   141,   142,   137,   138,   139,   140,    45,     0,
       0,     0,   111,   112,     0,   113,   117,     0,     0,     0,
       0,     0,     0,     0,     0,    69,   105,   115,     0,     0,
      71,    67,   103,    55,    63,    57,    64,   147,     0,   118,
       0,   116,   114
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    53,    54,    55,    56,    57,    58,   142,    60,
     209,    61,   236,    62,    63,    64,    96,    66,    67,   179,
     345,   346,   358,   347,   169,    68,   215,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -335
static const yytype_int16 yypact[] =
{
    -335,   281,  -335,  -132,  1199,   822,   848,  -129,  -335,   899,
    -335,  -335,  -123,  -115,  -104,   -73,   -68,   -64,   -55,   -53,
    1751,  1779,  1798,   -24,  1199,  -108,  1825,   -50,  -335,  1199,
    -335,  -335,  -335,  -335,   -28,  -335,  -335,   -51,   -38,   -37,
       0,   -34,  -335,    13,   -26,  1255,    39,    39,  1199,  1199,
    -335,  1199,  1199,  -335,  -335,  -335,    12,  -335,    -5,  -101,
    1678,  1199,  -335,  1199,  -335,  1817,   -83,  -335,  -335,  -335,
    -335,  -335,    24,  -335,  1825,  -335,  -335,  1199,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,    40,    45,  -129,
      51,    52,    54,    55,  -335,    36,    60,  1199,     9,  1199,
      -5,    88,  1314,  1199,    20,  1199,  1199,  1199,  1199,  1199,
    1199,  1199,   904,  1199,    -5,   969,    -5,  1199,    -5,    70,
    -335,    74,    72,  -335,    -5,  1756,  1678,   -11,  -335,  -335,
    -335,    46,  1199,   104,  1025,   848,  1678,    77,  -335,  -335,
      36,   -61,  -127,     8,  -335,  1825,  1084,  -335,  1199,  1199,
    1199,  -335,  1199,  -335,  1199,  1199,  1199,  1199,  1199,  1199,
    1199,  1199,    73,    74,  1140,    -5,  -335,  1825,  -335,  -335,
    1825,    -8,  1678,    87,  1199,   426,  -335,  1199,   456,    85,
     103,  1199,   495,  1368,  1380,   181,   319,  1412,   344,  -335,
    -106,   509,  -335,   -94,   523,  -335,  -335,    81,   -42,    86,
      89,    94,    97,   101,  -335,   107,   102,   118,  -335,  1199,
     138,   563,  -335,  -335,   -86,   111,  -335,  -335,  -335,  -335,
    -335,   -76,  1678,  1690,   446,   446,    26,    48,    48,    36,
    1353,    -3,  1702,    48,  1199,  -335,  -335,  -335,   -75,    -5,
      -5,  1314,  1199,  -335,  1427,  -335,  1439,  -335,  1314,  1199,
    -335,  1456,  -335,  1199,  1199,  1199,  -335,  1199,  -335,  1199,
    1199,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,   424,  -335,  -335,  -335,   593,   141,   -59,
    -335,  -335,  -335,  -335,  1199,   607,  -335,  1678,   117,  1468,
     121,  1199,   119,  1517,   123,   660,  1529,   674,  1605,   358,
     412,   114,   115,   120,   122,   125,   126,   127,   131,  -335,
    -335,  -335,  -335,  1617,  -335,  1314,  1199,  1199,   -72,  -110,
    1199,  1199,  -335,  1199,  -335,  1199,  1199,  -335,  1199,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,   128,
     700,  1629,  -335,  -335,   149,   134,  -335,   136,   744,  1666,
     758,   772,   811,   837,  -110,  -335,  -335,   147,   142,  1199,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,   149,  -335,
     -71,  -335,  -335
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -335,  -335,  -335,  -335,  -335,  -335,  -335,    19,    -1,    18,
    -335,  -335,   -23,  -335,  -335,   278,     7,   241,    56,  -227,
    -335,  -334,   -81,   -66,   132,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,  -335,
    -335,  -335,  -335,  -335
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
      59,   122,   343,   145,   151,    86,   145,   153,    65,   148,
     357,   149,   150,   102,   288,   151,   217,   152,   153,   105,
     145,   292,    95,   121,    98,   100,   147,   106,   104,   123,
     148,   145,   344,   151,   357,   148,   153,   262,   107,   114,
     116,   118,   148,   151,   166,   124,   153,   126,   141,   264,
     148,   311,    65,   167,   168,   151,   312,   280,   153,   128,
     148,   148,   163,   136,   148,   148,   140,   282,   286,   108,
     143,   342,   372,   216,   109,   148,   149,   150,   110,   162,
     151,   127,   152,   153,   165,   119,   120,   111,   339,   112,
     149,   150,   125,   171,   151,   172,   152,   153,   207,   208,
      99,   267,   138,   139,   129,   130,   149,   150,   132,   131,
     151,   190,   152,   153,   193,   173,   134,   175,   145,   166,
     178,   180,   133,   182,   183,   184,   185,   186,   187,   188,
     158,   191,   206,   214,   146,   194,   154,   146,   155,   156,
     157,   158,   174,   159,   205,   221,   160,   161,    30,   144,
     211,   146,   218,   181,   100,   210,   155,   156,   157,   158,
     170,   159,   146,   238,   219,   161,   222,   223,   224,   158,
     225,   159,   226,   227,   228,   229,   230,   231,   232,   233,
     157,   158,    97,   159,   149,   150,   239,    99,   151,   240,
     152,   153,   244,   103,   113,   246,   115,   117,   176,   251,
     234,   154,   164,   155,   156,   157,   158,   195,   159,   196,
     148,   160,   161,   167,   212,   154,   235,   155,   156,   157,
     158,   248,   159,   242,   266,   160,   161,   277,   276,   268,
     243,   154,   269,   155,   156,   157,   158,   270,   159,   249,
     271,   160,   161,   273,   272,   275,   250,   278,   281,   146,
     274,   310,   285,   315,   317,   319,   321,   330,   331,   287,
     289,   343,   314,   332,   354,   333,   287,   293,   334,   335,
     336,   295,   296,   297,   337,   298,   359,   299,   300,   360,
     308,     2,     3,   368,   101,   369,   137,   371,   367,     4,
     318,     0,     0,     0,     5,     6,     7,     0,     8,     0,
       9,    10,   313,   241,     0,     0,     0,     0,     0,   154,
       0,   155,   156,   157,   158,     0,   159,   255,     0,   160,
     161,    11,   149,   150,   256,     0,   151,     0,   152,   153,
       0,     0,     0,   287,   340,   341,     0,     0,   348,   349,
       0,   350,     0,   351,   352,     0,   353,   149,   150,     0,
       0,   151,     0,   152,   153,     0,     0,     0,   370,     0,
       0,   149,   150,     0,     0,   151,     0,   152,   153,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,     0,    27,    28,    29,
      30,     0,    31,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,     0,    47,
       0,     0,    48,     0,    49,   149,   150,     0,    50,   151,
       0,   152,   153,    51,     0,    52,     0,     0,     0,   149,
     150,     0,     0,   151,     0,   152,   153,   301,   302,   303,
       0,     8,     0,   304,    10,     0,     0,   154,     0,   155,
     156,   157,   158,   151,   159,   257,   153,   160,   161,   149,
     150,     0,   258,   151,     0,   152,   153,     0,     0,     0,
       0,     0,   154,     0,   155,   156,   157,   158,     0,   159,
     260,     0,   160,   161,     0,     0,   154,   261,   155,   156,
     157,   158,     0,   159,   326,     0,   160,   161,   149,   150,
       0,   327,   151,     0,   152,   153,     0,     0,     0,     0,
       0,     0,   149,   150,     0,     0,   151,     0,   152,   153,
       0,     0,   305,   306,   307,     0,   149,   150,     0,     0,
     151,     0,   152,   153,     0,     0,     0,     0,     0,     0,
     154,     0,   155,   156,   157,   158,     0,   159,   328,     0,
     160,   161,     0,     0,   154,   329,   155,   156,   157,   158,
       0,   159,     0,     0,   160,   161,   149,   150,     0,   245,
     151,     0,   152,   153,   154,     0,   155,   156,   157,   158,
       0,   159,     0,     0,   154,   161,   155,   156,   157,   158,
       0,   159,     0,     0,   160,   161,   149,   150,     0,   247,
     151,     0,   152,   153,     0,     0,     0,     0,     0,     0,
     149,   150,     0,     0,   151,     0,   152,   153,     0,     0,
       0,     0,     0,   154,     0,   155,   156,   157,   158,     0,
     159,     0,     0,   160,   161,     0,     0,   154,   252,   155,
     156,   157,   158,     0,   159,     0,     0,   160,   161,     0,
       0,   154,   263,   155,   156,   157,   158,     0,   159,     0,
       0,   160,   161,   149,   150,     0,   265,   151,     0,   152,
     153,     0,     0,     0,     0,     0,     0,   149,   150,     0,
       0,   151,     0,   152,   153,     0,     0,     0,     0,     0,
       0,   154,     0,   155,   156,   157,   158,     0,   159,     0,
       0,   160,   161,   149,   150,     0,   279,   151,     0,   152,
     153,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   154,     0,   155,   156,   157,   158,     0,   159,     0,
       0,   160,   161,     0,     0,   154,   309,   155,   156,   157,
     158,     0,   159,     0,     0,   160,   161,   149,   150,     0,
     235,   151,     0,   152,   153,     0,     0,     0,     0,     0,
       0,   149,   150,     0,     0,   151,     0,   152,   153,     0,
       0,     0,     0,     0,     0,   149,   150,     0,     0,   151,
       0,   152,   153,     0,     0,     0,     0,     0,   154,     0,
     155,   156,   157,   158,     0,   159,     0,     0,   160,   161,
       0,     0,   154,   322,   155,   156,   157,   158,     0,   159,
       0,     0,   160,   161,   149,   150,     0,   324,   151,     0,
     152,   153,     0,     0,     0,     0,     0,     0,   154,     0,
     155,   156,   157,   158,     0,   159,    88,     0,   160,   161,
     149,   150,     0,   355,   151,     0,   152,   153,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    88,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   154,     0,   155,   156,   157,   158,     0,   159,
       0,     0,   160,   161,     0,     0,   154,   361,   155,   156,
     157,   158,     0,   159,     0,     0,   160,   161,     0,     0,
     154,   363,   155,   156,   157,   158,     0,   159,     0,     0,
     160,   161,     4,    88,     0,   364,     0,    87,    88,    89,
       0,     8,     0,    90,    10,     0,     0,     0,     0,     0,
       0,    30,     0,    31,    32,    33,     0,     0,     0,   154,
       0,   155,   156,   157,   158,     0,   159,     0,    94,   160,
     161,     0,     0,     0,   365,    49,     0,    30,     0,    31,
      32,    33,     0,     0,    97,   154,    52,   155,   156,   157,
     158,     0,   159,     0,    94,   160,   161,     4,     0,     0,
     366,    49,    87,    88,    89,     0,     8,     0,    90,    10,
      99,     0,    52,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    91,    92,    93,     0,     0,     0,    30,     0,
      31,    32,    33,    30,     0,    31,    32,    33,     0,     0,
       0,     0,    38,    39,     0,    94,     0,     0,     0,     0,
      94,     0,    49,     4,     0,    48,     0,    49,    87,    88,
      89,   103,     8,    52,    90,    10,    51,   189,    52,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    12,
      13,    14,    15,    16,    17,    18,    19,    91,    92,    93,
       0,     0,     0,     0,     0,     0,     0,     0,    30,     0,
      31,    32,    33,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     4,     0,     0,    94,     0,    87,    88,    89,
      48,     8,    49,    90,    10,     0,     0,     0,     0,     0,
       0,    51,   192,    52,     0,    12,    13,    14,    15,    16,
      17,    18,    19,    91,    92,    93,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,    31,    32,    33,     0,
       0,     0,     0,    38,    39,     0,     0,     0,     4,     0,
       0,    94,     0,    87,    88,    89,    48,     8,    49,    90,
      10,     0,     0,     0,     0,     0,     0,    51,   213,    52,
       0,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    91,    92,    93,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,    31,    32,    33,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     4,     0,     0,
      94,     0,    87,    88,    89,    48,     8,    49,    90,    10,
       0,     0,     0,     0,     0,     0,    51,   220,    52,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    91,    92,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,    31,    32,    33,     0,     0,     0,     0,    38,    39,
       0,     0,     0,     4,     0,     0,    94,     0,     5,   135,
      89,    48,     8,    49,     9,    10,     0,     0,     0,     0,
       0,     0,    51,   237,    52,     0,     0,     0,     0,    12,
      13,    14,    15,    16,    17,    18,    19,    91,    92,    93,
       0,     0,     0,     0,     0,     0,     0,     0,    30,     0,
      31,    32,    33,     0,     0,     0,     0,    38,    39,     0,
       0,     0,     4,     0,     0,    94,     0,    87,    88,    89,
      48,     8,    49,    90,    10,     0,     0,     0,     0,     0,
       0,    51,     0,    52,     0,    12,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,   149,   150,     0,     0,
     151,     0,   152,   153,    30,     0,    31,    32,    33,     0,
       0,   149,   150,    38,    39,   151,     0,   152,   153,     0,
       0,    94,     0,   149,   150,     0,    48,   151,    49,   152,
     153,     0,     0,     0,     0,     0,     0,    51,     0,    52,
       0,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    91,    92,    93,   149,   150,     0,     0,   151,
       0,   152,   153,    30,     0,    31,    32,    33,     0,     0,
     149,   150,    38,    39,   151,     0,   152,   153,     0,     0,
      94,     0,   149,   150,     0,    48,   151,    49,   152,   153,
       0,     0,     0,     0,     0,     0,   177,     0,    52,   149,
     150,     0,     0,   151,     0,   152,   153,     0,     0,     0,
       0,   149,   150,     0,     0,   151,     0,   152,   153,     0,
       0,   154,     0,   155,   156,   157,   158,   283,   159,   284,
       0,   160,   161,     0,     0,     0,   154,     0,   155,   156,
     157,   158,     0,   159,   253,     0,   160,   161,   154,     0,
     155,   156,   157,   158,     0,   159,   254,     0,   160,   161,
     149,   150,     0,     0,   151,     0,   152,   153,     0,     0,
       0,     0,   149,   150,     0,     0,   151,     0,   152,   153,
     154,     0,   155,   156,   157,   158,     0,   159,   259,     0,
     160,   161,     0,     0,     0,   154,     0,   155,   156,   157,
     158,   290,   159,     0,     0,   160,   161,   154,     0,   155,
     156,   157,   158,     0,   159,   291,     0,   160,   161,     0,
       0,     0,     0,     0,   154,     0,   155,   156,   157,   158,
     294,   159,     0,     0,   160,   161,   154,     0,   155,   156,
     157,   158,     0,   159,   316,     0,   160,   161,   149,   150,
       0,     0,   151,     0,   152,   153,     0,     0,     0,     0,
     149,   150,     0,     0,   151,     0,   152,   153,     0,     0,
       0,     0,   149,   150,     0,     0,   151,     0,   152,   153,
       0,     0,     0,     0,     0,   154,     0,   155,   156,   157,
     158,     0,   159,   320,     0,   160,   161,   154,     0,   155,
     156,   157,   158,     0,   159,   323,     0,   160,   161,   149,
     150,     0,     0,   151,     0,   152,   153,     0,     0,     0,
       0,   149,   150,     0,     0,   151,     0,   152,   153,     0,
       0,     0,     0,    -1,   150,     0,     0,   151,     0,   152,
     153,     0,     0,     0,     0,     0,   150,     0,     0,   151,
       0,   152,   153,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   154,     0,   155,   156,   157,   158,     0,
     159,   325,     0,   160,   161,   154,     0,   155,   156,   157,
     158,   338,   159,     0,     0,   160,   161,   154,     0,   155,
     156,   157,   158,   356,   159,    88,     0,   160,   161,   197,
     198,   199,     0,     8,     0,   200,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    88,   154,     0,   155,   156,   157,   158,
     362,   159,     0,     0,   160,   161,   154,     0,   155,   156,
     157,   158,    88,   159,     0,     0,   160,   161,   154,     0,
     155,   156,   157,   158,     0,   159,     0,     0,   160,   161,
     154,    88,   155,   156,   157,   158,     0,   159,     0,    88,
       0,   161,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   201,   202,   203,     0,     0,     0,
      30,     0,    31,    32,    33,    30,     0,    31,    32,    33,
       0,     0,     0,     0,     0,     0,     0,    94,     0,     0,
       0,     0,    94,     0,    49,     0,     0,     0,    30,    49,
      31,    32,    33,   113,     0,    52,     0,     0,     0,   204,
      52,     0,     0,     0,     0,    94,     0,    30,     0,    31,
      32,    33,    49,     0,     0,     0,     0,     0,     0,     0,
       0,   115,     0,    52,    94,     0,    30,     0,    31,    32,
      33,    49,     0,     0,    30,     0,    31,    32,    33,     0,
     117,     0,    52,    94,     0,     0,     0,     0,     0,     0,
      49,    94,     0,     0,     0,     0,     0,     0,    49,   164,
       0,    52,     0,     0,     0,     0,     0,     0,     0,    52
};

static const yytype_int16 yycheck[] =
{
       1,   109,   112,    11,     7,   137,    11,    10,     1,   136,
     344,     3,     4,   142,   241,     7,   143,     9,    10,   142,
      11,   248,     4,    24,     5,     6,   127,   142,     9,   137,
     136,    11,   142,     7,   368,   136,    10,   143,   142,    20,
      21,    22,   136,     7,   127,    26,    10,    29,    49,   143,
     136,   110,    45,   136,   137,     7,   115,   143,    10,   110,
     136,   136,    63,    45,   136,   136,    48,   143,   143,   142,
      52,   143,   143,   134,   142,   136,     3,     4,   142,    61,
       7,   109,     9,    10,    65,   109,   110,   142,   315,   142,
       3,     4,   142,    74,     7,    77,     9,    10,   109,   110,
     142,   143,    46,    47,   142,   142,     3,     4,   142,   109,
       7,   112,     9,    10,   115,    97,   142,    99,    11,   127,
     102,   103,   109,   105,   106,   107,   108,   109,   110,   111,
     133,   113,   125,   134,   142,   117,   128,   142,   130,   131,
     132,   133,   133,   135,   125,   146,   138,   139,   109,   137,
     132,   142,   144,   133,   135,   109,   130,   131,   132,   133,
     136,   135,   142,   164,   145,   139,   148,   149,   150,   133,
     152,   135,   154,   155,   156,   157,   158,   159,   160,   161,
     132,   133,   142,   135,     3,     4,   167,   142,     7,   170,
       9,    10,   174,   142,   142,   177,   142,   142,   110,   181,
     127,   128,   142,   130,   131,   132,   133,   137,   135,   137,
     136,   138,   139,   136,   110,   128,   143,   130,   131,   132,
     133,   136,   135,   136,   143,   138,   139,   209,   110,   143,
     143,   128,   143,   130,   131,   132,   133,   143,   135,   136,
     143,   138,   139,   136,   143,   143,   143,   109,   137,   142,
     143,   110,   234,   136,   133,   136,   133,   143,   143,   241,
     242,   112,   285,   143,   136,   143,   248,   249,   143,   143,
     143,   253,   254,   255,   143,   257,   142,   259,   260,   143,
     273,     0,     1,   136,     6,   143,    45,   368,   354,     8,
     291,    -1,    -1,    -1,    13,    14,    15,    -1,    17,    -1,
      19,    20,   284,   171,    -1,    -1,    -1,    -1,    -1,   128,
      -1,   130,   131,   132,   133,    -1,   135,   136,    -1,   138,
     139,    40,     3,     4,   143,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,   315,   316,   317,    -1,    -1,   320,   321,
      -1,   323,    -1,   325,   326,    -1,   328,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,   359,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,   104,    -1,   106,   107,   108,
     109,    -1,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   126,    -1,   128,
      -1,    -1,   131,    -1,   133,     3,     4,    -1,   137,     7,
      -1,     9,    10,   142,    -1,   144,    -1,    -1,    -1,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    13,    14,    15,
      -1,    17,    -1,    19,    20,    -1,    -1,   128,    -1,   130,
     131,   132,   133,     7,   135,   136,    10,   138,   139,     3,
       4,    -1,   143,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,   135,
     136,    -1,   138,   139,    -1,    -1,   128,   143,   130,   131,
     132,   133,    -1,   135,   136,    -1,   138,   139,     3,     4,
      -1,   143,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    98,    99,   100,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
     128,    -1,   130,   131,   132,   133,    -1,   135,   136,    -1,
     138,   139,    -1,    -1,   128,   143,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   138,   139,     3,     4,    -1,   143,
       7,    -1,     9,    10,   128,    -1,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   128,   139,   130,   131,   132,   133,
      -1,   135,    -1,    -1,   138,   139,     3,     4,    -1,   143,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,
     135,    -1,    -1,   138,   139,    -1,    -1,   128,   143,   130,
     131,   132,   133,    -1,   135,    -1,    -1,   138,   139,    -1,
      -1,   128,   143,   130,   131,   132,   133,    -1,   135,    -1,
      -1,   138,   139,     3,     4,    -1,   143,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,   130,   131,   132,   133,    -1,   135,    -1,
      -1,   138,   139,     3,     4,    -1,   143,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   128,    -1,   130,   131,   132,   133,    -1,   135,    -1,
      -1,   138,   139,    -1,    -1,   128,   143,   130,   131,   132,
     133,    -1,   135,    -1,    -1,   138,   139,     3,     4,    -1,
     143,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,    -1,   135,    -1,    -1,   138,   139,
      -1,    -1,   128,   143,   130,   131,   132,   133,    -1,   135,
      -1,    -1,   138,   139,     3,     4,    -1,   143,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,   128,    -1,
     130,   131,   132,   133,    -1,   135,    14,    -1,   138,   139,
       3,     4,    -1,   143,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    14,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,   135,
      -1,    -1,   138,   139,    -1,    -1,   128,   143,   130,   131,
     132,   133,    -1,   135,    -1,    -1,   138,   139,    -1,    -1,
     128,   143,   130,   131,   132,   133,    -1,   135,    -1,    -1,
     138,   139,     8,    14,    -1,   143,    -1,    13,    14,    15,
      -1,    17,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,   109,    -1,   111,   112,   113,    -1,    -1,    -1,   128,
      -1,   130,   131,   132,   133,    -1,   135,    -1,   126,   138,
     139,    -1,    -1,    -1,   143,   133,    -1,   109,    -1,   111,
     112,   113,    -1,    -1,   142,   128,   144,   130,   131,   132,
     133,    -1,   135,    -1,   126,   138,   139,     8,    -1,    -1,
     143,   133,    13,    14,    15,    -1,    17,    -1,    19,    20,
     142,    -1,   144,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,   109,    -1,
     111,   112,   113,   109,    -1,   111,   112,   113,    -1,    -1,
      -1,    -1,   118,   119,    -1,   126,    -1,    -1,    -1,    -1,
     126,    -1,   133,     8,    -1,   131,    -1,   133,    13,    14,
      15,   142,    17,   144,    19,    20,   142,   143,   144,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,    -1,
     111,   112,   113,    -1,    -1,    -1,    -1,   118,   119,    -1,
      -1,    -1,     8,    -1,    -1,   126,    -1,    13,    14,    15,
     131,    17,   133,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,   142,   143,   144,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,    -1,   111,   112,   113,    -1,
      -1,    -1,    -1,   118,   119,    -1,    -1,    -1,     8,    -1,
      -1,   126,    -1,    13,    14,    15,   131,    17,   133,    19,
      20,    -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,
      -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   109,    -1,   111,   112,   113,    -1,    -1,
      -1,    -1,   118,   119,    -1,    -1,    -1,     8,    -1,    -1,
     126,    -1,    13,    14,    15,   131,    17,   133,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,   142,   143,   144,    -1,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
      -1,   111,   112,   113,    -1,    -1,    -1,    -1,   118,   119,
      -1,    -1,    -1,     8,    -1,    -1,   126,    -1,    13,    14,
      15,   131,    17,   133,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,   142,   143,   144,    -1,    -1,    -1,    -1,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,    -1,
     111,   112,   113,    -1,    -1,    -1,    -1,   118,   119,    -1,
      -1,    -1,     8,    -1,    -1,   126,    -1,    13,    14,    15,
     131,    17,   133,    19,    20,    -1,    -1,    -1,    -1,    -1,
      -1,   142,    -1,   144,    -1,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,     3,     4,    -1,    -1,
       7,    -1,     9,    10,   109,    -1,   111,   112,   113,    -1,
      -1,     3,     4,   118,   119,     7,    -1,     9,    10,    -1,
      -1,   126,    -1,     3,     4,    -1,   131,     7,   133,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,   144,
      -1,    -1,    -1,    -1,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,     3,     4,    -1,    -1,     7,
      -1,     9,    10,   109,    -1,   111,   112,   113,    -1,    -1,
       3,     4,   118,   119,     7,    -1,     9,    10,    -1,    -1,
     126,    -1,     3,     4,    -1,   131,     7,   133,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,   142,    -1,   144,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,   128,    -1,   130,   131,   132,   133,   134,   135,   136,
      -1,   138,   139,    -1,    -1,    -1,   128,    -1,   130,   131,
     132,   133,    -1,   135,   136,    -1,   138,   139,   128,    -1,
     130,   131,   132,   133,    -1,   135,   136,    -1,   138,   139,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
     128,    -1,   130,   131,   132,   133,    -1,   135,   136,    -1,
     138,   139,    -1,    -1,    -1,   128,    -1,   130,   131,   132,
     133,   134,   135,    -1,    -1,   138,   139,   128,    -1,   130,
     131,   132,   133,    -1,   135,   136,    -1,   138,   139,    -1,
      -1,    -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,
     134,   135,    -1,    -1,   138,   139,   128,    -1,   130,   131,
     132,   133,    -1,   135,   136,    -1,   138,   139,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,   128,    -1,   130,   131,   132,
     133,    -1,   135,   136,    -1,   138,   139,   128,    -1,   130,
     131,   132,   133,    -1,   135,   136,    -1,   138,   139,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   128,    -1,   130,   131,   132,   133,    -1,
     135,   136,    -1,   138,   139,   128,    -1,   130,   131,   132,
     133,   134,   135,    -1,    -1,   138,   139,   128,    -1,   130,
     131,   132,   133,   134,   135,    14,    -1,   138,   139,    13,
      14,    15,    -1,    17,    -1,    19,    20,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    14,   128,    -1,   130,   131,   132,   133,
     134,   135,    -1,    -1,   138,   139,   128,    -1,   130,   131,
     132,   133,    14,   135,    -1,    -1,   138,   139,   128,    -1,
     130,   131,   132,   133,    -1,   135,    -1,    -1,   138,   139,
     128,    14,   130,   131,   132,   133,    -1,   135,    -1,    14,
      -1,   139,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    98,    99,   100,    -1,    -1,    -1,
     109,    -1,   111,   112,   113,   109,    -1,   111,   112,   113,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   126,    -1,    -1,
      -1,    -1,   126,    -1,   133,    -1,    -1,    -1,   109,   133,
     111,   112,   113,   142,    -1,   144,    -1,    -1,    -1,   143,
     144,    -1,    -1,    -1,    -1,   126,    -1,   109,    -1,   111,
     112,   113,   133,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   142,    -1,   144,   126,    -1,   109,    -1,   111,   112,
     113,   133,    -1,    -1,   109,    -1,   111,   112,   113,    -1,
     142,    -1,   144,   126,    -1,    -1,    -1,    -1,    -1,    -1,
     133,   126,    -1,    -1,    -1,    -1,    -1,    -1,   133,   142,
      -1,   144,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   144
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   146,     0,     1,     8,    13,    14,    15,    17,    19,
      20,    40,    90,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,   103,   104,   106,   107,   108,
     109,   111,   112,   113,   114,   115,   116,   117,   118,   119,
     120,   121,   122,   123,   124,   125,   126,   128,   131,   133,
     137,   142,   144,   147,   148,   149,   150,   151,   152,   153,
     154,   156,   158,   159,   160,   161,   162,   163,   170,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   137,    13,    14,    15,
      19,    98,    99,   100,   126,   154,   161,   142,   152,   142,
     152,   160,   142,   142,   152,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   152,   142,   152,   142,   152,   109,
     110,   153,   109,   137,   152,   142,   154,   109,   110,   142,
     142,   109,   142,   109,   142,    14,   154,   162,   163,   163,
     154,   153,   153,   154,   137,    11,   142,   127,   136,     3,
       4,     7,     9,    10,   128,   130,   131,   132,   133,   135,
     138,   139,   154,   153,   142,   152,   127,   136,   137,   169,
     136,   152,   154,   154,   133,   154,   110,   142,   154,   164,
     154,   133,   154,   154,   154,   154,   154,   154,   154,   143,
     153,   154,   143,   153,   154,   137,   137,    13,    14,    15,
      19,    98,    99,   100,   143,   152,   161,   109,   110,   155,
     109,   154,   110,   143,   153,   171,   134,   143,   144,   152,
     143,   153,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   127,   143,   157,   143,   153,   152,
     152,   169,   136,   143,   154,   143,   154,   143,   136,   136,
     143,   154,   143,   136,   136,   136,   143,   136,   143,   136,
     136,   143,   143,   143,   143,   143,   143,   143,   143,   143,
     143,   143,   143,   136,   143,   143,   110,   154,   109,   143,
     143,   137,   143,   134,   136,   154,   143,   154,   164,   154,
     134,   136,   164,   154,   134,   154,   154,   154,   154,   154,
     154,    13,    14,    15,    19,    98,    99,   100,   161,   143,
     110,   110,   115,   154,   157,   136,   136,   133,   153,   136,
     136,   133,   143,   136,   143,   136,   136,   143,   136,   143,
     143,   143,   143,   143,   143,   143,   143,   143,   134,   164,
     154,   154,   143,   112,   142,   165,   166,   168,   154,   154,
     154,   154,   154,   154,   136,   143,   134,   166,   167,   142,
     143,   143,   134,   143,   143,   143,   143,   168,   136,   143,
     153,   167,   143
};

#define yyerrok		(yyerrstatus = 0)
#define yyclearin	(yychar = YYEMPTY)
#define YYEMPTY		(-2)
#define YYEOF		0

#define YYACCEPT	goto yyacceptlab
#define YYABORT		goto yyabortlab
#define YYERROR		goto yyerrorlab


/* Like YYERROR except do call yyerror.  This remains here temporarily
   to ease the transition to the new meaning of YYERROR, for GCC.
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)					\
do								\
  if (yychar == YYEMPTY && yylen == 1)				\
    {								\
      yychar = (Token);						\
      yylval = (Value);						\
      yytoken = YYTRANSLATE (yychar);				\
      YYPOPSTACK (1);						\
      goto yybackup;						\
    }								\
  else								\
    {								\
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;							\
    }								\
while (YYID (0))


#define YYTERROR	1
#define YYERRCODE	256


/* YYLLOC_DEFAULT -- Set CURRENT to span from RHS[1] to RHS[N].
   If N is 0, then set CURRENT to the empty location which ends
   the previous symbol: RHS[0] (always defined).  */

#define YYRHSLOC(Rhs, K) ((Rhs)[K])
#ifndef YYLLOC_DEFAULT
# define YYLLOC_DEFAULT(Current, Rhs, N)				\
    do									\
      if (YYID (N))                                                    \
	{								\
	  (Current).first_line   = YYRHSLOC (Rhs, 1).first_line;	\
	  (Current).first_column = YYRHSLOC (Rhs, 1).first_column;	\
	  (Current).last_line    = YYRHSLOC (Rhs, N).last_line;		\
	  (Current).last_column  = YYRHSLOC (Rhs, N).last_column;	\
	}								\
      else								\
	{								\
	  (Current).first_line   = (Current).last_line   =		\
	    YYRHSLOC (Rhs, 0).last_line;				\
	  (Current).first_column = (Current).last_column =		\
	    YYRHSLOC (Rhs, 0).last_column;				\
	}								\
    while (YYID (0))
#endif


/* YY_LOCATION_PRINT -- Print the location on the stream.
   This macro was not mandated originally: define only if we know
   we won't break user code: when these are the locations we know.  */

#ifndef YY_LOCATION_PRINT
# if YYLTYPE_IS_TRIVIAL
#  define YY_LOCATION_PRINT(File, Loc)			\
     fprintf (File, "%d.%d-%d.%d",			\
	      (Loc).first_line, (Loc).first_column,	\
	      (Loc).last_line,  (Loc).last_column)
# else
#  define YY_LOCATION_PRINT(File, Loc) ((void) 0)
# endif
#endif


/* YYLEX -- calling `yylex' with the right arguments.  */

#ifdef YYLEX_PARAM
# define YYLEX yylex (&yylval, YYLEX_PARAM)
#else
# define YYLEX yylex (&yylval)
#endif

/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)			\
do {						\
  if (yydebug)					\
    YYFPRINTF Args;				\
} while (YYID (0))

# define YY_SYMBOL_PRINT(Title, Type, Value, Location)			  \
do {									  \
  if (yydebug)								  \
    {									  \
      YYFPRINTF (stderr, "%s ", Title);					  \
      yy_symbol_print (stderr,						  \
		  Type, Value); \
      YYFPRINTF (stderr, "\n");						  \
    }									  \
} while (YYID (0))


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# else
  YYUSE (yyoutput);
# endif
  switch (yytype)
    {
      default:
	break;
    }
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)				\
do {								\
  if (yydebug)							\
    yy_stack_print ((Bottom), (Top));				\
} while (YYID (0))


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_reduce_print (YYSTYPE *yyvsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yyrule)
    YYSTYPE *yyvsp;
    int yyrule;
#endif
{
  int yynrhs = yyr2[yyrule];
  int yyi;
  unsigned long int yylno = yyrline[yyrule];
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
	     yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, Rule); \
} while (YYID (0))

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef	YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif



#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static YYSIZE_T
yystrlen (const char *yystr)
#else
static YYSIZE_T
yystrlen (yystr)
    const char *yystr;
#endif
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static char *
yystpcpy (char *yydest, const char *yysrc)
#else
static char *
yystpcpy (yydest, yysrc)
    char *yydest;
    const char *yysrc;
#endif
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
	switch (*++yyp)
	  {
	  case '\'':
	  case ',':
	    goto do_not_strip_quotes;

	  case '\\':
	    if (*++yyp != '\\')
	      goto do_not_strip_quotes;
	    /* Fall through.  */
	  default:
	    if (yyres)
	      yyres[yyn] = *yyp;
	    yyn++;
	    break;

	  case '"':
	    if (yyres)
	      yyres[yyn] = '\0';
	    return yyn;
	  }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into YYRESULT an error message about the unexpected token
   YYCHAR while in state YYSTATE.  Return the number of bytes copied,
   including the terminating null byte.  If YYRESULT is null, do not
   copy anything; just return the number of bytes that would be
   copied.  As a special case, return 0 if an ordinary "syntax error"
   message will do.  Return YYSIZE_MAXIMUM if overflow occurs during
   size calculation.  */
static YYSIZE_T
yysyntax_error (char *yyresult, int yystate, int yychar)
{
  int yyn = yypact[yystate];

  if (! (YYPACT_NINF < yyn && yyn <= YYLAST))
    return 0;
  else
    {
      int yytype = YYTRANSLATE (yychar);
      YYSIZE_T yysize0 = yytnamerr (0, yytname[yytype]);
      YYSIZE_T yysize = yysize0;
      YYSIZE_T yysize1;
      int yysize_overflow = 0;
      enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
      char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
      int yyx;

# if 0
      /* This is so xgettext sees the translatable formats that are
	 constructed on the fly.  */
      YY_("syntax error, unexpected %s");
      YY_("syntax error, unexpected %s, expecting %s");
      YY_("syntax error, unexpected %s, expecting %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s");
      YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s");
# endif
      char *yyfmt;
      char const *yyf;
      static char const yyunexpected[] = "syntax error, unexpected %s";
      static char const yyexpecting[] = ", expecting %s";
      static char const yyor[] = " or %s";
      char yyformat[sizeof yyunexpected
		    + sizeof yyexpecting - 1
		    + ((YYERROR_VERBOSE_ARGS_MAXIMUM - 2)
		       * (sizeof yyor - 1))];
      char const *yyprefix = yyexpecting;

      /* Start YYX at -YYN if negative to avoid negative indexes in
	 YYCHECK.  */
      int yyxbegin = yyn < 0 ? -yyn : 0;

      /* Stay within bounds of both yycheck and yytname.  */
      int yychecklim = YYLAST - yyn + 1;
      int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
      int yycount = 1;

      yyarg[0] = yytname[yytype];
      yyfmt = yystpcpy (yyformat, yyunexpected);

      for (yyx = yyxbegin; yyx < yyxend; ++yyx)
	if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR)
	  {
	    if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
	      {
		yycount = 1;
		yysize = yysize0;
		yyformat[sizeof yyunexpected - 1] = '\0';
		break;
	      }
	    yyarg[yycount++] = yytname[yyx];
	    yysize1 = yysize + yytnamerr (0, yytname[yyx]);
	    yysize_overflow |= (yysize1 < yysize);
	    yysize = yysize1;
	    yyfmt = yystpcpy (yyfmt, yyprefix);
	    yyprefix = yyor;
	  }

      yyf = YY_(yyformat);
      yysize1 = yysize + yystrlen (yyf);
      yysize_overflow |= (yysize1 < yysize);
      yysize = yysize1;

      if (yysize_overflow)
	return YYSIZE_MAXIMUM;

      if (yyresult)
	{
	  /* Avoid sprintf, as that infringes on the user's name space.
	     Don't have undefined behavior even if the translation
	     produced a string with the wrong number of "%s"s.  */
	  char *yyp = yyresult;
	  int yyi = 0;
	  while ((*yyp = *yyf) != '\0')
	    {
	      if (*yyp == '%' && yyf[1] == 's' && yyi < yycount)
		{
		  yyp += yytnamerr (yyp, yyarg[yyi++]);
		  yyf += 2;
		}
	      else
		{
		  yyp++;
		  yyf++;
		}
	    }
	}
      return yysize;
    }
}
#endif /* YYERROR_VERBOSE */


/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

/*ARGSUSED*/
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
#else
static void
yydestruct (yymsg, yytype, yyvaluep)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
#endif
{
  YYUSE (yyvaluep);

  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  switch (yytype)
    {

      default:
	break;
    }
}


/* Prevent warnings from -Wmissing-prototypes.  */

#ifdef YYPARSE_PARAM
#if defined __STDC__ || defined __cplusplus
int yyparse (void *YYPARSE_PARAM);
#else
int yyparse ();
#endif
#else /* ! YYPARSE_PARAM */
#if defined __STDC__ || defined __cplusplus
int yyparse (void);
#else
int yyparse ();
#endif
#endif /* ! YYPARSE_PARAM */






/*----------.
| yyparse.  |
`----------*/

#ifdef YYPARSE_PARAM
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void *YYPARSE_PARAM)
#else
int
yyparse (YYPARSE_PARAM)
    void *YYPARSE_PARAM;
#endif
#else /* ! YYPARSE_PARAM */
#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
int
yyparse (void)
#else
int
yyparse ()

#endif
#endif
{
  /* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;

  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;


  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;

  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
	/* Give user a chance to reallocate the stack.  Use copies of
	   these so that the &'s don't force the real ones into
	   memory.  */
	YYSTYPE *yyvs1 = yyvs;
	yytype_int16 *yyss1 = yyss;


	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),

		    &yystacksize);

	yyss = yyss1;
	yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
	goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
	yystacksize = YYMAXDEPTH;

      {
	yytype_int16 *yyss1 = yyss;
	union yyalloc *yyptr =
	  (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
	if (! yyptr)
	  goto yyexhaustedlab;
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);

#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;


      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = YYLEX;
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yyn == 0 || yyn == YYTABLE_NINF)
	goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     `$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 3:
#line 360 "grammar.y"
    {
            if (timerv)
            {
              writeTime("used time:");
              startTimer();
            }
            if (rtimerv)
            {
              writeRTime("used real time:");
              startRTimer();
            }
            prompt_char = '>';
#ifdef HAVE_SDB
            if (sdb_flags & 2) { sdb_flags=1; YYERROR; }
#endif
            if(siCntrlc)
            {
              WerrorS("abort...");
	      while((currentVoice!=NULL) && (currentVoice->prev!=NULL)) exitVoice();
	      if (currentVoice!=NULL) currentVoice->ifsw=0;
            }
            if (errorreported) /* also catches abort... */
            {
              yyerror("");
            }
            if (inerror==2) PrintLn();
            errorreported = inerror = cmdtok = 0;
            lastreserved = currid = NULL;
            expected_parms = siCntrlc = FALSE;
          ;}
    break;

  case 5:
#line 395 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:
#line 397 "grammar.y"
    { (yyvsp[(1) - (2)].lv).CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:
#line 399 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:
#line 403 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:
#line 408 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:
#line 410 "grammar.y"
    {
            #ifdef SIQ
            siq=0;
            #endif
            yyInRingConstruction = FALSE;
            currentVoice->ifsw=0;
            if (inerror)
            {
/*  bison failed here*/
              if ((inerror!=3) && ((yyvsp[(1) - (2)]).i<UMINUS) && ((yyvsp[(1) - (2)]).i>' '))
              {
                // 1: yyerror called
                // 2: scanner put actual string
                // 3: error rule put token+\n
                inerror=3;
                Print(" error at token `%s`\n",iiTwoOps((yyvsp[(1) - (2)]).i));
              }
/**/

            }
            if (!errorreported) WerrorS("...parse error");
            yyerror("");
            yyerrok;
#ifdef HAVE_SDB
            if ((sdb_flags & 1) && currentVoice->pi!=NULL)
            {
              currentVoice->pi->trace_flag |=1;
            }
            else
#endif
            if (myynest>0)
            {
              feBufferTypes t=currentVoice->Typ();
              //PrintS("leaving yyparse\n");
              exitBuffer(BT_proc);
              if (t==BT_example)
                YYACCEPT;
              else
                YYABORT;
            }
            else if (currentVoice->prev!=NULL)
            {
              exitVoice();
            }
#ifdef HAVE_SDB
            if (sdb_flags &2) sdb_flags=1;
#endif
          ;}
    break;

  case 18:
#line 468 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:
#line 471 "grammar.y"
    { omFree((ADDRESS)(yyvsp[(2) - (2)].name)); ;}
    break;

  case 29:
#line 486 "grammar.y"
    {
            if(iiAssign(&(yyvsp[(1) - (2)].lv),&(yyvsp[(2) - (2)].lv))) YYERROR;
          ;}
    break;

  case 30:
#line 493 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&(yyval.lv),omStrDup((yyvsp[(1) - (1)].name)));
          ;}
    break;

  case 31:
#line 498 "grammar.y"
    {
            syMake(&(yyval.lv),(yyvsp[(1) - (1)].name));
          ;}
    break;

  case 32:
#line 502 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), COLONCOLON, &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 33:
#line 506 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'(')) YYERROR;
          ;}
    break;

  case 34:
#line 510 "grammar.y"
    {
	    if ((yyvsp[(1) - (4)].lv).rtyp==UNKNOWN)
	    { // for x(i)(j)
	      if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'(',&(yyvsp[(3) - (4)].lv))) YYERROR;
	    }
	    else
	    {
              (yyvsp[(1) - (4)].lv).next=(leftv)omAllocBin(sleftv_bin);
              memcpy((yyvsp[(1) - (4)].lv).next,&(yyvsp[(3) - (4)].lv),sizeof(sleftv));
              if(iiExprArithM(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'(')) YYERROR;
	    }
          ;}
    break;

  case 35:
#line 523 "grammar.y"
    {
            if (currRingHdl==NULL) MYYERROR("no ring active");
            int j = 0;
            memset(&(yyval.lv),0,sizeof(sleftv));
            (yyval.lv).rtyp=VECTOR_CMD;
            leftv v = &(yyvsp[(2) - (3)].lv);
            while (v!=NULL)
            {
              int i,t;
              sleftv tmp;
              memset(&tmp,0,sizeof(tmp));
              i=iiTestConvert((t=v->Typ()),POLY_CMD);
              if((i==0) || (iiConvert(t /*v->Typ()*/,POLY_CMD,i,v,&tmp)))
              {
                pDelete((poly *)&(yyval.lv).data);
                (yyvsp[(2) - (3)].lv).CleanUp();
                MYYERROR("expected '[poly,...'");
              }
              poly p = (poly)tmp.CopyD(POLY_CMD);
              pSetCompP(p,++j);
              (yyval.lv).data = (void *)pAdd((poly)(yyval.lv).data,p);
              v->next=tmp.next;tmp.next=NULL;
              tmp.CleanUp();
              v=v->next;
            }
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 36:
#line 551 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            int i = atoi((yyvsp[(1) - (1)].name));
            /*remember not to omFree($1)
            *because it is a part of the scanner buffer*/
            (yyval.lv).rtyp  = INT_CMD;
            (yyval.lv).data = (void *)(long)i;

            /* check: out of range input */
            int l = strlen((yyvsp[(1) - (1)].name))+2;
            number n;
            if (l >= MAX_INT_LEN)
            {
              char tmp[MAX_INT_LEN+5];
              sprintf(tmp,"%d",i);
              if (strcmp(tmp,(yyvsp[(1) - (1)].name))!=0)
              {
                nlRead((yyvsp[(1) - (1)].name),&n);
                (yyval.lv).rtyp=BIGINT_CMD;
                (yyval.lv).data = n;
              }
            }
          ;}
    break;

  case 37:
#line 575 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[(1) - (1)].i);
            (yyval.lv).data = (yyval.lv).Data();
          ;}
    break;

  case 38:
#line 581 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[(1) - (1)].name);
          ;}
    break;

  case 39:
#line 587 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 40:
#line 594 "grammar.y"
    {
            leftv v = &(yyvsp[(1) - (3)].lv);
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&((yyvsp[(3) - (3)].lv)),sizeof(sleftv));
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
          ;}
    break;

  case 41:
#line 605 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 42:
#line 611 "grammar.y"
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 43:
#line 615 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (1)].lv); ;}
    break;

  case 44:
#line 616 "grammar.y"
    { (yyval.lv) = (yyvsp[(2) - (3)].lv); ;}
    break;

  case 45:
#line 618 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[(1) - (6)].lv),&(yyvsp[(3) - (6)].lv),&(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 46:
#line 622 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'[',&(yyvsp[(3) - (4)].lv))) YYERROR;
          ;}
    break;

  case 47:
#line 626 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 48:
#line 630 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 49:
#line 634 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 50:
#line 638 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 51:
#line 642 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 52:
#line 646 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 53:
#line 650 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 54:
#line 654 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 55:
#line 658 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 56:
#line 662 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 57:
#line 666 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 58:
#line 670 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 59:
#line 674 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 60:
#line 678 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 61:
#line 682 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 62:
#line 686 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 63:
#line 690 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 64:
#line 694 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 65:
#line 698 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 66:
#line 702 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 67:
#line 706 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),MATRIX_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 68:
#line 710 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),MATRIX_CMD)) YYERROR;
          ;}
    break;

  case 69:
#line 714 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),INTMAT_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 70:
#line 718 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),INTMAT_CMD)) YYERROR;
          ;}
    break;

  case 71:
#line 722 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 72:
#line 726 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),RING_CMD)) YYERROR;
          ;}
    break;

  case 73:
#line 730 "grammar.y"
    {
            (yyval.lv)=(yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 74:
#line 734 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            if (siq>0)
            { if (iiExprArith2(&(yyval.lv),&(yyvsp[(2) - (5)].lv),'=',&(yyvsp[(4) - (5)].lv))) YYERROR; }
            else
            #endif
            {
              memset(&(yyval.lv),0,sizeof((yyval.lv)));
              (yyval.lv).rtyp=NONE;
              if (iiAssign(&(yyvsp[(2) - (5)].lv),&(yyvsp[(4) - (5)].lv))) YYERROR;
            }
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 75:
#line 751 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 76:
#line 757 "grammar.y"
    {
            #ifdef SIQ
            if (siq<=0) (yyvsp[(4) - (5)].lv).Eval();
            #endif
            (yyval.lv)=(yyvsp[(4) - (5)].lv);
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 77:
#line 769 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 78:
#line 777 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 79:
#line 786 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),PLUSPLUS)) YYERROR;
          ;}
    break;

  case 80:
#line 790 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),MINUSMINUS)) YYERROR;
          ;}
    break;

  case 81:
#line 794 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'+',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 82:
#line 798 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'-',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 83:
#line 802 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 84:
#line 806 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'^',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 85:
#line 810 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 86:
#line 814 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 87:
#line 818 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),NOTEQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 88:
#line 822 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),EQUAL_EQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 89:
#line 826 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),DOTDOT,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 90:
#line 830 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),':',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 91:
#line 834 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            int i; TESTSETINT((yyvsp[(2) - (2)].lv),i);
            (yyval.lv).rtyp  = INT_CMD;
            (yyval.lv).data = (void *)(long)(i == 0 ? 1 : 0);
          ;}
    break;

  case 92:
#line 841 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),'-')) YYERROR;
          ;}
    break;

  case 93:
#line 847 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (2)].lv); ;}
    break;

  case 94:
#line 849 "grammar.y"
    {
            if ((yyvsp[(1) - (2)].lv).rtyp==0)
            {
              Werror("`%s` is undefined",(yyvsp[(1) - (2)].lv).Fullname());
              YYERROR;
            }
            else if (((yyvsp[(1) - (2)].lv).rtyp==MODUL_CMD)
            // matrix m; m[2]=...
            && ((yyvsp[(1) - (2)].lv).e!=NULL) && ((yyvsp[(1) - (2)].lv).e->next==NULL))
            {
              MYYERROR("matrix must have 2 indices");
            }
            (yyval.lv) = (yyvsp[(1) - (2)].lv);
          ;}
    break;

  case 96:
#line 869 "grammar.y"
    {
            if ((yyvsp[(2) - (3)].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[(2) - (3)].lv).CopyD(STRING_CMD);
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 99:
#line 886 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 100:
#line 891 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 101:
#line 896 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 102:
#line 900 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 103:
#line 904 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&(currRing->idroot), TRUE)) YYERROR;
            int r; TESTSETINT((yyvsp[(4) - (8)].lv),r);
            int c; TESTSETINT((yyvsp[(7) - (8)].lv),c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            leftv v=&(yyval.lv);
            //while (v->next!=NULL) { v=v->next; }
            idhdl h=(idhdl)v->data;
            idDelete(&IDIDEAL(h));
            IDMATRIX(h) = mpNew(r,c);
            if (IDMATRIX(h)==NULL) YYERROR;
          ;}
    break;

  case 104:
#line 920 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 105:
#line 924 "grammar.y"
    {
            int r; TESTSETINT((yyvsp[(4) - (8)].lv),r);
            int c; TESTSETINT((yyvsp[(7) - (8)].lv),c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&((yyvsp[(2) - (8)].lv).req_packhdl->idroot)))
              YYERROR;
            leftv v=&(yyval.lv);
            idhdl h=(idhdl)v->data;
            delete IDINTVEC(h);
            IDINTVEC(h) = new intvec(r,c,0);
            if (IDINTVEC(h)==NULL) YYERROR;
          ;}
    break;

  case 106:
#line 940 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
            leftv v=&(yyval.lv);
            idhdl h;
            do
            {
               h=(idhdl)v->data;
               delete IDINTVEC(h);
               IDINTVEC(h) = new intvec(1,1,0);
               v=v->next;
            } while (v!=NULL);
          ;}
    break;

  case 107:
#line 954 "grammar.y"
    {
            int t=(yyvsp[(1) - (3)].lv).Typ();
            sleftv r;
            memset(&r,0,sizeof(sleftv));
            if ((BEGIN_RING<t) && (t<END_RING))
            {
              if (iiDeclCommand(&r,&(yyvsp[(3) - (3)].lv),myynest,t,&(currRing->idroot), TRUE))
                YYERROR;
            }
            else
            {
              if (iiDeclCommand(&r,&(yyvsp[(3) - (3)].lv),myynest,t,&((yyvsp[(3) - (3)].lv).req_packhdl->idroot)))
                YYERROR;
            }
            leftv v=&(yyvsp[(1) - (3)].lv);
            while (v->next!=NULL) v=v->next;
            v->next=(leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&r,sizeof(sleftv));
            (yyval.lv)=(yyvsp[(1) - (3)].lv);
          ;}
    break;

  case 108:
#line 975 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 111:
#line 988 "grammar.y"
    {
            leftv v = &(yyvsp[(2) - (5)].lv);
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&((yyvsp[(4) - (5)].lv)),sizeof(sleftv));
            (yyval.lv) = (yyvsp[(2) - (5)].lv);
          ;}
    break;

  case 112:
#line 1002 "grammar.y"
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[(1) - (1)].name));
        ;}
    break;

  case 113:
#line 1010 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = (yyvsp[(1) - (1)].i);
            (yyval.lv).rtyp = INTVEC_CMD;
            (yyval.lv).data = (void *)iv;
          ;}
    break;

  case 114:
#line 1019 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            leftv sl = &(yyvsp[(3) - (4)].lv);
            int slLength;
            {
              slLength =  exprlist_length(sl);
              int l = 2 +  slLength;
              intvec *iv = new intvec(l);
              (*iv)[0] = slLength;
              (*iv)[1] = (yyvsp[(1) - (4)].i);

              int i = 2;
              while ((i<l) && (sl!=NULL))
              {
                if (sl->Typ() == INT_CMD)
                {
                  (*iv)[i++] = (int)((long)(sl->Data()));
                }
                else if ((sl->Typ() == INTVEC_CMD)
                ||(sl->Typ() == INTMAT_CMD))
                {
                  intvec *ivv = (intvec *)(sl->Data());
                  int ll = 0,l = ivv->length();
                  for (; l>0; l--)
                  {
                    (*iv)[i++] = (*ivv)[ll++];
                  }
                }
                else
                {
                  delete iv;
                  (yyvsp[(3) - (4)].lv).CleanUp();
                  MYYERROR("wrong type in ordering");
                }
                sl = sl->next;
              }
              (yyval.lv).rtyp = INTVEC_CMD;
              (yyval.lv).data = (void *)iv;
            }
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 116:
#line 1065 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[(3) - (3)].lv),sizeof(sleftv));
          ;}
    break;

  case 118:
#line 1075 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 119:
#line 1081 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 120:
#line 1093 "grammar.y"
    { if ((yyvsp[(1) - (2)].i) != '<') YYERROR;
            if((feFilePending=feFopen((yyvsp[(2) - (2)].name),"r",NULL,TRUE))==NULL) YYERROR; ;}
    break;

  case 121:
#line 1096 "grammar.y"
    { newFile((yyvsp[(2) - (4)].name),feFilePending); ;}
    break;

  case 122:
#line 1101 "grammar.y"
    {
            feHelp((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 123:
#line 1106 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 124:
#line 1113 "grammar.y"
    {
            singular_example((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 125:
#line 1121 "grammar.y"
    {
          if (basePack!=(yyvsp[(2) - (2)].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[(2) - (2)].lv),0,currPackHdl)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[(2) - (2)].lv),0)) YYERROR;
        ;}
    break;

  case 126:
#line 1133 "grammar.y"
    {
          leftv v=&(yyvsp[(2) - (2)].lv);
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 127:
#line 1149 "grammar.y"
    {
          leftv v=&(yyvsp[(3) - (3)].lv);
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 128:
#line 1168 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 129:
#line 1172 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 130:
#line 1176 "grammar.y"
    {
            if ((yyvsp[(3) - (4)].i)==QRING_CMD) (yyvsp[(3) - (4)].i)=RING_CMD;
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 131:
#line 1181 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 132:
#line 1185 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 133:
#line 1189 "grammar.y"
    {
            list_cmd(MATRIX_CMD,NULL,"// ",TRUE);
           ;}
    break;

  case 134:
#line 1193 "grammar.y"
    {
            list_cmd(INTMAT_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 135:
#line 1197 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 136:
#line 1201 "grammar.y"
    {
            list_cmd(0,(yyvsp[(3) - (4)].lv).Fullname(),"// ",TRUE);
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 137:
#line 1206 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 138:
#line 1212 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 139:
#line 1218 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 140:
#line 1224 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 141:
#line 1230 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 142:
#line 1236 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 143:
#line 1242 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 144:
#line 1248 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 145:
#line 1260 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 146:
#line 1266 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 147:
#line 1275 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (8)].lv).name;
            ring b=
            rInit(&(yyvsp[(4) - (8)].lv),            /* characteristik and list of parameters*/
                  &(yyvsp[(6) - (8)].lv),            /* names of ringvariables */
                  &(yyvsp[(8) - (8)].lv));            /* ordering */
            idhdl newRingHdl=NULL;

            if (b!=NULL)
            {
                newRingHdl=enterid(ring_name, myynest, RING_CMD,
                                   &((yyvsp[(2) - (8)].lv).req_packhdl->idroot));
              (yyvsp[(2) - (8)].lv).CleanUp();
              if (newRingHdl!=NULL)
              {
                omFreeSize(IDRING(newRingHdl),sizeof(ip_sring));
                IDRING(newRingHdl)=b;
              }
              else
              {
                rKill(b);
              }
            }
            yyInRingConstruction = FALSE;
            if (newRingHdl==NULL)
            {
              MYYERROR("cannot make ring");
            }
            else
            {
              rSetHdl(newRingHdl);
            }
          ;}
    break;

  case 148:
#line 1309 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (2)].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[(2) - (2)].lv).CleanUp();
          ;}
    break;

  case 149:
#line 1319 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)!=LIB_CMD)||(iiLibCmd((yyvsp[(2) - (2)].name),TRUE,TRUE,TRUE)))
            //if ($1==LIB_CMD)
            //{
            //  sleftv tmp;
            //  if(iiExprArith1(&tmp,&$2,LIB_CMD)) YYERROR;
            //}
            //else
                YYERROR;
          ;}
    break;

  case 152:
#line 1335 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)==KEEPRING_CMD) && (myynest==0))
               MYYERROR("only inside a proc allowed");
            const char * n=(yyvsp[(2) - (2)].lv).Name();
            if ((((yyvsp[(2) - (2)].lv).Typ()==RING_CMD)||((yyvsp[(2) - (2)].lv).Typ()==QRING_CMD))
            && ((yyvsp[(2) - (2)].lv).rtyp==IDHDL))
            {
              idhdl h=(idhdl)(yyvsp[(2) - (2)].lv).data;
              if ((yyvsp[(2) - (2)].lv).e!=NULL) h=rFindHdl((ring)(yyvsp[(2) - (2)].lv).Data(),NULL, NULL);
              //Print("setring %s lev %d (ptr:%x)\n",IDID(h),IDLEV(h),IDRING(h));
              if ((yyvsp[(1) - (2)].i)==KEEPRING_CMD)
              {
                if (h!=NULL)
                {
                  if (IDLEV(h)!=0)
                  {
                    if (iiExport(&(yyvsp[(2) - (2)].lv),myynest-1)) YYERROR;
#if 1
                    idhdl p=IDRING(h)->idroot;
                    idhdl root=p;
                    int prevlev=myynest-1;
                    while (p!=NULL)
                    {
                      if (IDLEV(p)==myynest)
                      {
                        idhdl old=root->get(IDID(p),prevlev);
                        if (old!=NULL)
                        {
                          if (BVERBOSE(V_REDEFINE))
                            Warn("redefining %s",IDID(p));
                          killhdl2(old,&root,IDRING(h));
                          IDRING(h)->idroot=root;
                        }
                        IDLEV(p)=prevlev;
                      }
                      p=IDNEXT(p);
                    }
#endif
                  }
#ifdef USE_IILOCALRING
                  iiLocalRing[myynest-1]=IDRING(h);
#endif
                  procstack->cRing=IDRING(h);
                  procstack->cRingHdl=h;
                }
                else
                {
                  Werror("%s is no identifier",n);
                  (yyvsp[(2) - (2)].lv).CleanUp();
                  YYERROR;
                }
              }
              if (h!=NULL) rSetHdl(h);
              else
              {
                Werror("cannot find the name of the basering %s",n);
                (yyvsp[(2) - (2)].lv).CleanUp();
                YYERROR;
              }
              (yyvsp[(2) - (2)].lv).CleanUp();
            }
            else
            {
              Werror("%s is no name of a ring/qring",n);
              (yyvsp[(2) - (2)].lv).CleanUp();
              YYERROR;
            }
          ;}
    break;

  case 153:
#line 1407 "grammar.y"
    {
            if ((yyvsp[(2) - (2)].lv).rtyp!=IDHDL) MYYERROR("identifier expected");
            idhdl h = (idhdl)(yyvsp[(2) - (2)].lv).data;
            type_cmd(h);
          ;}
    break;

  case 154:
#line 1413 "grammar.y"
    {
            //Print("typ is %d, rtyp:%d\n",$1.Typ(),$1.rtyp);
            #ifdef SIQ
            if ((yyvsp[(1) - (1)].lv).rtyp!=COMMAND)
            {
            #endif
              if ((yyvsp[(1) - (1)].lv).Typ()==UNKNOWN)
              {
                if ((yyvsp[(1) - (1)].lv).name!=NULL)
                {
                  Werror("`%s` is undefined",(yyvsp[(1) - (1)].lv).name);
                  omFree((ADDRESS)(yyvsp[(1) - (1)].lv).name);
                }
                YYERROR;
              }
            #ifdef SIQ
            }
            #endif
            (yyvsp[(1) - (1)].lv).Print(&sLastPrinted);
            (yyvsp[(1) - (1)].lv).CleanUp(currRing);
            if (errorreported) YYERROR;
          ;}
    break;

  case 155:
#line 1442 "grammar.y"
    {
            int i; TESTSETINT((yyvsp[(3) - (5)].lv),i);
            if (i!=0)
            {
              newBuffer( (yyvsp[(5) - (5)].name), BT_if);
            }
            else
            {
              omFree((ADDRESS)(yyvsp[(5) - (5)].name));
              currentVoice->ifsw=1;
            }
          ;}
    break;

  case 156:
#line 1455 "grammar.y"
    {
            if (currentVoice->ifsw==1)
            {
              currentVoice->ifsw=0;
              newBuffer( (yyvsp[(2) - (2)].name), BT_else);
            }
            else
            {
              if (currentVoice->ifsw!=2)
              {
                Warn("`else` without `if` in level %d",myynest);
              }
              omFree((ADDRESS)(yyvsp[(2) - (2)].name));
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 157:
#line 1472 "grammar.y"
    {
            int i; TESTSETINT((yyvsp[(3) - (5)].lv),i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 158:
#line 1481 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 159:
#line 1486 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 160:
#line 1494 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen((yyvsp[(2) - (3)].name)) + strlen((yyvsp[(3) - (3)].name)) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,(yyvsp[(2) - (3)].name),(yyvsp[(3) - (3)].name));
            newBuffer(s,BT_break);
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 161:
#line 1506 "grammar.y"
    {
            /* $2 */
            /* if (!$3) break; $5; $4; continue; */
            char * s = (char *)omAlloc( strlen((yyvsp[(3) - (5)].name))+strlen((yyvsp[(4) - (5)].name))+strlen((yyvsp[(5) - (5)].name))+36);
            sprintf(s,"forif (!(%s)) break;\n%s%s;\ncontinue;\n "
                   ,(yyvsp[(3) - (5)].name),(yyvsp[(5) - (5)].name),(yyvsp[(4) - (5)].name));
            omFree((ADDRESS)(yyvsp[(3) - (5)].name));
            omFree((ADDRESS)(yyvsp[(4) - (5)].name));
            omFree((ADDRESS)(yyvsp[(5) - (5)].name));
            newBuffer(s,BT_break);
            s = (char *)omAlloc( strlen((yyvsp[(2) - (5)].name)) + 3);
            sprintf(s,"%s;\n",(yyvsp[(2) - (5)].name));
            omFree((ADDRESS)(yyvsp[(2) - (5)].name));
            newBuffer(s,BT_if);
          ;}
    break;

  case 162:
#line 1525 "grammar.y"
    {
            procinfov pi;
            idhdl h = enterid((yyvsp[(2) - (3)].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL) {omFree((ADDRESS)(yyvsp[(2) - (3)].name));omFree((ADDRESS)(yyvsp[(3) - (3)].name)); YYERROR;}
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(2) - (3)].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(3) - (3)].name))+31);;
            sprintf(IDPROC(h)->data.s.body,"parameter list #;\n%s;return();\n\n",(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 163:
#line 1536 "grammar.y"
    {
            idhdl h = enterid((yyvsp[(1) - (3)].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)(yyvsp[(1) - (3)].name));
              omFree((ADDRESS)(yyvsp[(2) - (3)].name));
              omFree((ADDRESS)(yyvsp[(3) - (3)].name));
              YYERROR;
            }
            char *args=iiProcArgs((yyvsp[(2) - (3)].name),FALSE);
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
            procinfov pi;
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(1) - (3)].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(3) - (3)].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(1) - (3)].name));
          ;}
    break;

  case 164:
#line 1556 "grammar.y"
    {
            omFree((ADDRESS)(yyvsp[(3) - (4)].name));
            idhdl h = enterid((yyvsp[(1) - (4)].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)(yyvsp[(1) - (4)].name));
              omFree((ADDRESS)(yyvsp[(2) - (4)].name));
              omFree((ADDRESS)(yyvsp[(4) - (4)].name));
              YYERROR;
            }
            char *args=iiProcArgs((yyvsp[(2) - (4)].name),FALSE);
            omFree((ADDRESS)(yyvsp[(2) - (4)].name));
            procinfov pi;
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(1) - (4)].name), 0, 0);
            omFree((ADDRESS)(yyvsp[(1) - (4)].name));
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(4) - (4)].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[(4) - (4)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(4) - (4)].name));
          ;}
    break;

  case 165:
#line 1580 "grammar.y"
    {
	    // decl. of type proc p(int i)
            if ((yyvsp[(1) - (2)].i)==PARAMETER)  { if (iiParameter(&(yyvsp[(2) - (2)].lv))) YYERROR; }
	    else                { if (iiAlias(&(yyvsp[(2) - (2)].lv))) YYERROR; } 
          ;}
    break;

  case 166:
#line 1586 "grammar.y"
    {
	    // decl. of type proc p(i)
            sleftv tmp_expr;
	    if ((yyvsp[(1) - (2)].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[(2) - (2)].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 167:
#line 1598 "grammar.y"
    {
            if(iiRETURNEXPR==NULL) YYERROR;
            iiRETURNEXPR[myynest].Copy(&(yyvsp[(3) - (4)].lv));
            (yyvsp[(3) - (4)].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 168:
#line 1605 "grammar.y"
    {
            if ((yyvsp[(1) - (3)].i)==RETURN)
            {
              if(iiRETURNEXPR==NULL) YYERROR;
              iiRETURNEXPR[myynest].Init();
              iiRETURNEXPR[myynest].rtyp=NONE;
              if (exitBuffer(BT_proc)) YYERROR;
            }
          ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3936 "grammar.cc"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;


  /* Now `shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*------------------------------------.
| yyerrlab -- here on detecting error |
`------------------------------------*/
yyerrlab:
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
      {
	YYSIZE_T yysize = yysyntax_error (0, yystate, yychar);
	if (yymsg_alloc < yysize && yymsg_alloc < YYSTACK_ALLOC_MAXIMUM)
	  {
	    YYSIZE_T yyalloc = 2 * yysize;
	    if (! (yysize <= yyalloc && yyalloc <= YYSTACK_ALLOC_MAXIMUM))
	      yyalloc = YYSTACK_ALLOC_MAXIMUM;
	    if (yymsg != yymsgbuf)
	      YYSTACK_FREE (yymsg);
	    yymsg = (char *) YYSTACK_ALLOC (yyalloc);
	    if (yymsg)
	      yymsg_alloc = yyalloc;
	    else
	      {
		yymsg = yymsgbuf;
		yymsg_alloc = sizeof yymsgbuf;
	      }
	  }

	if (0 < yysize && yysize <= yymsg_alloc)
	  {
	    (void) yysyntax_error (yymsg, yystate, yychar);
	    yyerror (yymsg);
	  }
	else
	  {
	    yyerror (YY_("syntax error"));
	    if (yysize != 0)
	      goto yyexhaustedlab;
	  }
      }
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
	 error, discard it.  */

      if (yychar <= YYEOF)
	{
	  /* Return failure if at end of input.  */
	  if (yychar == YYEOF)
	    YYABORT;
	}
      else
	{
	  yydestruct ("Error: discarding",
		      yytoken, &yylval);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule which action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;	/* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (yyn != YYPACT_NINF)
	{
	  yyn += YYTERROR;
	  if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
	    {
	      yyn = yytable[yyn];
	      if (0 < yyn)
		break;
	    }
	}

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
	YYABORT;


      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  /* Make sure YYID is used.  */
  return YYID (yyresult);
}



