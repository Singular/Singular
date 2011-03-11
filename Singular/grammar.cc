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
     COEFFS_CMD = 281,
     COEF_CMD = 282,
     CONTRACT_CMD = 283,
     DEGREE_CMD = 284,
     DEG_CMD = 285,
     DIFF_CMD = 286,
     DIM_CMD = 287,
     DIVISION_CMD = 288,
     ELIMINATION_CMD = 289,
     E_CMD = 290,
     FAREY_CMD = 291,
     FETCH_CMD = 292,
     FREEMODULE_CMD = 293,
     KEEPRING_CMD = 294,
     HILBERT_CMD = 295,
     HOMOG_CMD = 296,
     IMAP_CMD = 297,
     INDEPSET_CMD = 298,
     INTERRED_CMD = 299,
     INTERSECT_CMD = 300,
     JACOB_CMD = 301,
     JET_CMD = 302,
     KBASE_CMD = 303,
     KOSZUL_CMD = 304,
     LEADCOEF_CMD = 305,
     LEADEXP_CMD = 306,
     LEAD_CMD = 307,
     LEADMONOM_CMD = 308,
     LIFTSTD_CMD = 309,
     LIFT_CMD = 310,
     MAXID_CMD = 311,
     MINBASE_CMD = 312,
     MINOR_CMD = 313,
     MINRES_CMD = 314,
     MODULO_CMD = 315,
     MONOM_CMD = 316,
     MRES_CMD = 317,
     MULTIPLICITY_CMD = 318,
     ORD_CMD = 319,
     PAR_CMD = 320,
     PARDEG_CMD = 321,
     PREIMAGE_CMD = 322,
     QUOTIENT_CMD = 323,
     QHWEIGHT_CMD = 324,
     REDUCE_CMD = 325,
     REGULARITY_CMD = 326,
     RES_CMD = 327,
     SIMPLIFY_CMD = 328,
     SORTVEC_CMD = 329,
     SRES_CMD = 330,
     STD_CMD = 331,
     SUBST_CMD = 332,
     SYZYGY_CMD = 333,
     VAR_CMD = 334,
     VDIM_CMD = 335,
     WEDGE_CMD = 336,
     WEIGHT_CMD = 337,
     VALTVARS = 338,
     VMAXDEG = 339,
     VMAXMULT = 340,
     VNOETHER = 341,
     VMINPOLY = 342,
     END_RING = 343,
     CMD_1 = 344,
     CMD_2 = 345,
     CMD_3 = 346,
     CMD_12 = 347,
     CMD_13 = 348,
     CMD_23 = 349,
     CMD_123 = 350,
     CMD_M = 351,
     ROOT_DECL = 352,
     ROOT_DECL_LIST = 353,
     RING_DECL = 354,
     EXAMPLE_CMD = 355,
     EXPORT_CMD = 356,
     HELP_CMD = 357,
     KILL_CMD = 358,
     LIB_CMD = 359,
     LISTVAR_CMD = 360,
     SETRING_CMD = 361,
     TYPE_CMD = 362,
     STRINGTOK = 363,
     BLOCKTOK = 364,
     INT_CONST = 365,
     UNKNOWN_IDENT = 366,
     RINGVAR = 367,
     PROC_DEF = 368,
     BREAK_CMD = 369,
     CONTINUE_CMD = 370,
     ELSE_CMD = 371,
     EVAL = 372,
     QUOTE = 373,
     FOR_CMD = 374,
     IF_CMD = 375,
     SYS_BREAK = 376,
     WHILE_CMD = 377,
     RETURN = 378,
     PARAMETER = 379,
     SYSVAR = 380,
     UMINUS = 381
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
#define COEFFS_CMD 281
#define COEF_CMD 282
#define CONTRACT_CMD 283
#define DEGREE_CMD 284
#define DEG_CMD 285
#define DIFF_CMD 286
#define DIM_CMD 287
#define DIVISION_CMD 288
#define ELIMINATION_CMD 289
#define E_CMD 290
#define FAREY_CMD 291
#define FETCH_CMD 292
#define FREEMODULE_CMD 293
#define KEEPRING_CMD 294
#define HILBERT_CMD 295
#define HOMOG_CMD 296
#define IMAP_CMD 297
#define INDEPSET_CMD 298
#define INTERRED_CMD 299
#define INTERSECT_CMD 300
#define JACOB_CMD 301
#define JET_CMD 302
#define KBASE_CMD 303
#define KOSZUL_CMD 304
#define LEADCOEF_CMD 305
#define LEADEXP_CMD 306
#define LEAD_CMD 307
#define LEADMONOM_CMD 308
#define LIFTSTD_CMD 309
#define LIFT_CMD 310
#define MAXID_CMD 311
#define MINBASE_CMD 312
#define MINOR_CMD 313
#define MINRES_CMD 314
#define MODULO_CMD 315
#define MONOM_CMD 316
#define MRES_CMD 317
#define MULTIPLICITY_CMD 318
#define ORD_CMD 319
#define PAR_CMD 320
#define PARDEG_CMD 321
#define PREIMAGE_CMD 322
#define QUOTIENT_CMD 323
#define QHWEIGHT_CMD 324
#define REDUCE_CMD 325
#define REGULARITY_CMD 326
#define RES_CMD 327
#define SIMPLIFY_CMD 328
#define SORTVEC_CMD 329
#define SRES_CMD 330
#define STD_CMD 331
#define SUBST_CMD 332
#define SYZYGY_CMD 333
#define VAR_CMD 334
#define VDIM_CMD 335
#define WEDGE_CMD 336
#define WEIGHT_CMD 337
#define VALTVARS 338
#define VMAXDEG 339
#define VMAXMULT 340
#define VNOETHER 341
#define VMINPOLY 342
#define END_RING 343
#define CMD_1 344
#define CMD_2 345
#define CMD_3 346
#define CMD_12 347
#define CMD_13 348
#define CMD_23 349
#define CMD_123 350
#define CMD_M 351
#define ROOT_DECL 352
#define ROOT_DECL_LIST 353
#define RING_DECL 354
#define EXAMPLE_CMD 355
#define EXPORT_CMD 356
#define HELP_CMD 357
#define KILL_CMD 358
#define LIB_CMD 359
#define LISTVAR_CMD 360
#define SETRING_CMD 361
#define TYPE_CMD 362
#define STRINGTOK 363
#define BLOCKTOK 364
#define INT_CONST 365
#define UNKNOWN_IDENT 366
#define RINGVAR 367
#define PROC_DEF 368
#define BREAK_CMD 369
#define CONTINUE_CMD 370
#define ELSE_CMD 371
#define EVAL 372
#define QUOTE 373
#define FOR_CMD 374
#define IF_CMD 375
#define SYS_BREAK 376
#define WHILE_CMD 377
#define RETURN 378
#define PARAMETER 379
#define SYSVAR 380
#define UMINUS 381




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
#line 529 "grammar.cc"

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
#define YYLAST   1957

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  145
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  169
/* YYNRULES -- Number of states.  */
#define YYNSTATES  375

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   381

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   137,     2,
     141,   142,   139,   129,   135,   130,   143,   131,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   138,   136,
     127,   126,   128,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   132,     2,   133,   134,     2,   144,     2,     2,     2,
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
     125,   140
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     4,     7,     9,    12,    15,    17,    19,
      21,    24,    26,    28,    30,    32,    34,    36,    38,    40,
      43,    45,    47,    49,    51,    53,    55,    57,    59,    61,
      64,    66,    68,    72,    76,    80,    85,    89,    91,    93,
      95,   100,   104,   106,   108,   110,   114,   121,   126,   131,
     136,   140,   145,   150,   154,   159,   166,   175,   182,   191,
     196,   201,   208,   213,   220,   229,   238,   242,   247,   256,
     261,   270,   275,   284,   289,   293,   299,   300,   306,   309,
     311,   314,   317,   321,   325,   329,   333,   337,   341,   345,
     349,   353,   357,   360,   363,   366,   369,   371,   375,   377,
     379,   382,   385,   388,   391,   400,   403,   412,   415,   419,
     422,   424,   426,   432,   434,   436,   441,   443,   447,   449,
     453,   455,   456,   461,   465,   468,   472,   475,   478,   482,
     487,   492,   497,   502,   507,   512,   517,   522,   527,   534,
     541,   548,   555,   562,   569,   576,   583,   587,   589,   598,
     601,   604,   606,   608,   611,   614,   616,   622,   625,   631,
     633,   635,   639,   645,   649,   653,   658,   661,   664,   669
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     146,     0,    -1,    -1,   146,   147,    -1,   148,    -1,   150,
     136,    -1,   162,   136,    -1,   188,    -1,   121,    -1,   136,
      -1,     1,   136,    -1,   183,    -1,   184,    -1,   149,    -1,
     185,    -1,   186,    -1,   170,    -1,   172,    -1,   173,    -1,
     100,   109,    -1,   151,    -1,   174,    -1,   175,    -1,   176,
      -1,   187,    -1,   178,    -1,   179,    -1,   181,    -1,   182,
      -1,   159,   153,    -1,   112,    -1,   160,    -1,   152,    11,
     152,    -1,   152,   143,   152,    -1,   152,   141,   142,    -1,
     152,   141,   153,   142,    -1,   132,   153,   133,    -1,   110,
      -1,   125,    -1,   163,    -1,    14,   141,   154,   142,    -1,
     153,   135,   154,    -1,   154,    -1,   158,    -1,   152,    -1,
     141,   153,   142,    -1,   154,   132,   154,   135,   154,   133,
      -1,   154,   132,   154,   133,    -1,    97,   141,   154,   142,
      -1,    98,   141,   153,   142,    -1,    98,   141,   142,    -1,
      99,   141,   154,   142,    -1,   161,   141,   153,   142,    -1,
     161,   141,   142,    -1,    89,   141,   154,   142,    -1,    90,
     141,   154,   135,   154,   142,    -1,    91,   141,   154,   135,
     154,   135,   154,   142,    -1,    94,   141,   154,   135,   154,
     142,    -1,    94,   141,   154,   135,   154,   135,   154,   142,
      -1,    92,   141,   154,   142,    -1,    93,   141,   154,   142,
      -1,    92,   141,   154,   135,   154,   142,    -1,    95,   141,
     154,   142,    -1,    95,   141,   154,   135,   154,   142,    -1,
      93,   141,   154,   135,   154,   135,   154,   142,    -1,    95,
     141,   154,   135,   154,   135,   154,   142,    -1,    96,   141,
     142,    -1,    96,   141,   153,   142,    -1,    19,   141,   154,
     135,   154,   135,   154,   142,    -1,    19,   141,   154,   142,
      -1,    13,   141,   154,   135,   154,   135,   154,   142,    -1,
      13,   141,   154,   142,    -1,    15,   141,   164,   135,   164,
     135,   168,   142,    -1,    15,   141,   154,   142,    -1,   156,
     154,   157,    -1,   156,   154,   126,   154,   157,    -1,    -1,
     117,   141,   155,   154,   142,    -1,   118,   141,    -1,   142,
      -1,   154,    10,    -1,   154,     7,    -1,   154,   129,   154,
      -1,   154,   130,   154,    -1,   154,   131,   154,    -1,   154,
     134,   154,    -1,   154,   127,   154,    -1,   154,   137,   154,
      -1,   154,     9,   154,    -1,   154,     4,   154,    -1,   154,
       3,   154,    -1,   154,   138,   154,    -1,     8,   154,    -1,
     130,   154,    -1,   162,   169,    -1,   153,   126,    -1,   111,
      -1,   144,   154,   144,    -1,    17,    -1,    20,    -1,    97,
     152,    -1,    98,   152,    -1,    99,   152,    -1,   161,   152,
      -1,    19,   152,   132,   154,   133,   132,   154,   133,    -1,
      19,   152,    -1,    13,   152,   132,   154,   133,   132,   154,
     133,    -1,    13,   152,    -1,   162,   135,   152,    -1,    14,
     152,    -1,   108,    -1,   154,    -1,   141,   154,   135,   153,
     142,    -1,   111,    -1,   165,    -1,   165,   141,   153,   142,
      -1,   166,    -1,   166,   135,   167,    -1,   166,    -1,   141,
     167,   142,    -1,   126,    -1,    -1,   127,   163,   171,   136,
      -1,   102,   108,   136,    -1,   102,   136,    -1,   100,   108,
     136,    -1,   101,   153,    -1,   103,   152,    -1,   175,   135,
     152,    -1,   105,   141,    97,   142,    -1,   105,   141,    98,
     142,    -1,   105,   141,    99,   142,    -1,   105,   141,   161,
     142,    -1,   105,   141,    15,   142,    -1,   105,   141,    19,
     142,    -1,   105,   141,    13,   142,    -1,   105,   141,    14,
     142,    -1,   105,   141,   152,   142,    -1,   105,   141,   152,
     135,    97,   142,    -1,   105,   141,   152,   135,    98,   142,
      -1,   105,   141,   152,   135,    99,   142,    -1,   105,   141,
     152,   135,   161,   142,    -1,   105,   141,   152,   135,    15,
     142,    -1,   105,   141,   152,   135,    19,   142,    -1,   105,
     141,   152,   135,    13,   142,    -1,   105,   141,   152,   135,
      14,   142,    -1,   105,   141,   142,    -1,    15,    -1,   177,
     152,   169,   164,   135,   164,   135,   168,    -1,   177,   152,
      -1,   125,   163,    -1,   106,    -1,    39,    -1,   180,   154,
      -1,   107,   154,    -1,   153,    -1,   120,   141,   154,   142,
     109,    -1,   116,   109,    -1,   120,   141,   154,   142,   114,
      -1,   114,    -1,   115,    -1,   122,   108,   109,    -1,   119,
     108,   108,   108,   109,    -1,    14,   160,   109,    -1,   113,
     108,   109,    -1,   113,   108,   108,   109,    -1,   124,   162,
      -1,   124,   154,    -1,   123,   141,   153,   142,    -1,   123,
     141,   142,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   357,   357,   359,   393,   394,   396,   398,   402,   407,
     409,   460,   461,   462,   463,   464,   465,   466,   467,   471,
     474,   475,   476,   477,   478,   479,   480,   481,   482,   485,
     492,   497,   501,   505,   509,   513,   526,   554,   578,   584,
     590,   597,   608,   614,   619,   620,   621,   625,   629,   633,
     637,   641,   645,   649,   653,   657,   661,   665,   669,   673,
     677,   681,   685,   689,   693,   697,   701,   705,   709,   713,
     717,   721,   725,   729,   733,   737,   755,   754,   772,   780,
     789,   793,   797,   801,   805,   809,   813,   817,   821,   825,
     829,   833,   837,   844,   851,   852,   871,   872,   884,   884,
     889,   894,   899,   903,   907,   923,   927,   943,   957,   978,
     986,   990,   991,  1005,  1013,  1022,  1067,  1068,  1077,  1078,
    1084,  1097,  1096,  1104,  1109,  1116,  1124,  1136,  1152,  1171,
    1175,  1179,  1184,  1188,  1192,  1196,  1200,  1204,  1209,  1215,
    1221,  1227,  1233,  1239,  1245,  1251,  1263,  1270,  1274,  1312,
    1322,  1335,  1335,  1338,  1410,  1416,  1445,  1458,  1475,  1484,
    1489,  1497,  1509,  1528,  1539,  1559,  1583,  1589,  1601,  1608
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
  "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD", "COEFFS_CMD", "COEF_CMD",
  "CONTRACT_CMD", "DEGREE_CMD", "DEG_CMD", "DIFF_CMD", "DIM_CMD",
  "DIVISION_CMD", "ELIMINATION_CMD", "E_CMD", "FAREY_CMD", "FETCH_CMD",
  "FREEMODULE_CMD", "KEEPRING_CMD", "HILBERT_CMD", "HOMOG_CMD", "IMAP_CMD",
  "INDEPSET_CMD", "INTERRED_CMD", "INTERSECT_CMD", "JACOB_CMD", "JET_CMD",
  "KBASE_CMD", "KOSZUL_CMD", "LEADCOEF_CMD", "LEADEXP_CMD", "LEAD_CMD",
  "LEADMONOM_CMD", "LIFTSTD_CMD", "LIFT_CMD", "MAXID_CMD", "MINBASE_CMD",
  "MINOR_CMD", "MINRES_CMD", "MODULO_CMD", "MONOM_CMD", "MRES_CMD",
  "MULTIPLICITY_CMD", "ORD_CMD", "PAR_CMD", "PARDEG_CMD", "PREIMAGE_CMD",
  "QUOTIENT_CMD", "QHWEIGHT_CMD", "REDUCE_CMD", "REGULARITY_CMD",
  "RES_CMD", "SIMPLIFY_CMD", "SORTVEC_CMD", "SRES_CMD", "STD_CMD",
  "SUBST_CMD", "SYZYGY_CMD", "VAR_CMD", "VDIM_CMD", "WEDGE_CMD",
  "WEIGHT_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT", "VNOETHER", "VMINPOLY",
  "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12", "CMD_13", "CMD_23",
  "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST", "RING_DECL",
  "EXAMPLE_CMD", "EXPORT_CMD", "HELP_CMD", "KILL_CMD", "LIB_CMD",
  "LISTVAR_CMD", "SETRING_CMD", "TYPE_CMD", "STRINGTOK", "BLOCKTOK",
  "INT_CONST", "UNKNOWN_IDENT", "RINGVAR", "PROC_DEF", "BREAK_CMD",
  "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD", "IF_CMD",
  "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR", "'='", "'<'",
  "'>'", "'+'", "'-'", "'/'", "'['", "']'", "'^'", "','", "';'", "'&'",
  "':'", "'*'", "UMINUS", "'('", "')'", "'.'", "'`'", "$accept", "lines",
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
     375,   376,   377,   378,   379,   380,    61,    60,    62,    43,
      45,    47,    91,    93,    94,    44,    59,    38,    58,    42,
     381,    40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   145,   146,   146,   147,   147,   147,   147,   147,   147,
     147,   148,   148,   148,   148,   148,   148,   148,   148,   149,
     150,   150,   150,   150,   150,   150,   150,   150,   150,   151,
     152,   152,   152,   152,   152,   152,   152,   152,   152,   152,
     152,   153,   153,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   155,   154,   156,   157,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   159,   159,   160,   160,   161,   161,
     162,   162,   162,   162,   162,   162,   162,   162,   162,   162,
     163,   164,   164,   165,   166,   166,   167,   167,   168,   168,
     169,   171,   170,   172,   172,   173,   174,   175,   175,   176,
     176,   176,   176,   176,   176,   176,   176,   176,   176,   176,
     176,   176,   176,   176,   176,   176,   176,   177,   178,   178,
     179,   180,   180,   181,   182,   182,   183,   183,   183,   183,
     183,   184,   185,   186,   186,   186,   187,   187,   188,   188
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       4,     3,     1,     1,     1,     3,     6,     4,     4,     4,
       3,     4,     4,     3,     4,     6,     8,     6,     8,     4,
       4,     6,     4,     6,     8,     8,     3,     4,     8,     4,
       8,     4,     8,     4,     3,     5,     0,     5,     2,     1,
       2,     2,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     2,     2,     2,     2,     1,     3,     1,     1,
       2,     2,     2,     2,     8,     2,     8,     2,     3,     2,
       1,     1,     5,     1,     1,     4,     1,     3,     1,     3,
       1,     0,     4,     3,     2,     3,     2,     2,     3,     4,
       4,     4,     4,     4,     4,     4,     4,     4,     6,     6,
       6,     6,     6,     6,     6,     6,     3,     1,     8,     2,
       2,     1,     1,     2,     2,     1,     5,     2,     5,     1,
       1,     3,     5,     3,     3,     4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,     0,     0,   147,    98,     0,
      99,   152,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   151,     0,
     110,    37,    96,    30,     0,   159,   160,     0,     0,     0,
       0,     0,     8,     0,     0,     0,    38,     0,     0,     0,
       9,     0,     0,     3,     4,    13,     0,    20,    44,   155,
      42,     0,    43,     0,    31,     0,     0,    39,    16,    17,
      18,    21,    22,    23,     0,    25,    26,     0,    27,    28,
      11,    12,    14,    15,    24,     7,    10,     0,     0,     0,
       0,     0,     0,     0,    38,    92,     0,     0,   107,     0,
     109,    31,     0,     0,   105,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   100,     0,   101,     0,   102,     0,
      19,   126,     0,   124,   127,     0,   154,     0,   157,    76,
      78,     0,     0,     0,     0,     0,   167,   166,   150,   121,
      93,     0,     0,     0,     5,     0,     0,     0,    95,     0,
       0,     0,    81,     0,    80,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    29,     0,   103,   120,     0,     6,
      94,     0,   149,   153,     0,     0,     0,   163,     0,   111,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      66,     0,     0,    50,     0,     0,   125,   123,     0,     0,
       0,     0,     0,     0,     0,   146,     0,     0,     0,   164,
       0,     0,     0,   161,   169,     0,     0,    36,    45,    97,
      32,    34,     0,    33,    41,    90,    89,    88,    86,    82,
      83,    84,     0,    85,    87,    91,     0,    79,    74,    53,
       0,   108,   128,     0,     0,    71,     0,    40,    42,    73,
       0,     0,    69,     0,    54,     0,     0,     0,    59,     0,
      60,     0,     0,    62,    67,    48,    49,    51,   135,   136,
     133,   134,   129,   130,   131,     0,   137,   132,   165,     0,
       0,     0,   168,   122,    35,    47,     0,     0,    52,   111,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    77,   162,   156,   158,     0,    75,     0,     0,     0,
       0,     0,     0,     0,    55,     0,    61,     0,     0,    57,
       0,    63,   144,   145,   142,   143,   138,   139,   140,   141,
      46,     0,     0,     0,   112,   113,     0,   114,   118,     0,
       0,     0,     0,     0,     0,     0,     0,    70,   106,   116,
       0,     0,    72,    68,   104,    56,    64,    58,    65,   148,
       0,   119,     0,   117,   115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    53,    54,    55,    56,    57,    58,   142,    60,
     210,    61,   238,    62,    63,    64,    96,    66,    67,   180,
     347,   348,   360,   349,   170,    68,   216,    69,    70,    71,
      72,    73,    74,    75,    76,    77,    78,    79,    80,    81,
      82,    83,    84,    85
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -327
static const yytype_int16 yypact[] =
{
    -327,   283,  -327,   -79,  1263,   957,  1010,  -126,  -327,  1747,
    -327,  -327,   -76,   -72,   -55,   -51,   -45,   -44,   -42,   -40,
    1765,  1770,  1775,  -107,  1263,   -97,   222,   -39,  -327,  1263,
    -327,  -327,  -327,  -327,    -8,  -327,  -327,    -4,   -35,   -29,
       0,   -28,  -327,     8,   -23,  1319,    11,    11,  1263,  1263,
    -327,  1263,  1263,  -327,  -327,  -327,   -14,  -327,     1,   -98,
    1704,  1263,  -327,  1263,  -327,  1813,   -81,  -327,  -327,  -327,
    -327,  -327,    -5,  -327,   222,  -327,  -327,  1263,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  -327,    -7,    17,  -126,
      43,    53,    57,    58,  -327,    23,    61,  1263,    29,  1263,
       1,    39,  1377,  1263,    60,  1263,  1263,  1263,  1263,  1263,
    1263,  1263,   903,  1263,     1,  1035,     1,  1263,     1,    68,
    -327,    70,    73,  -327,     1,  1638,  1704,   -33,  -327,  -327,
    -327,    98,  1263,    99,  1091,  1010,  1704,    76,  -327,  -327,
      23,   -61,  -121,    22,  -327,   222,  1149,   222,  -327,  1263,
    1263,  1263,  -327,  1263,  -327,  1263,  1263,  1263,  1263,  1263,
    1263,  1263,  1263,   100,    70,  1205,     1,  -327,   222,  -327,
    -327,   222,    69,  1704,   178,  1263,   496,  -327,  1263,   510,
      78,   314,  1263,   524,  1396,  1416,   346,   360,  1431,   413,
    -327,  -108,   563,  -327,  -100,   586,  -327,  -327,    74,   -59,
      79,    80,    81,    82,    83,  -327,    48,    91,   105,  -327,
    1263,   107,   600,  -327,  -327,   -92,    84,  -327,  -327,  -327,
      75,  -327,   -89,  -327,  1704,  1716,   486,   486,   332,    31,
      31,    23,   946,    54,     9,    31,  1263,  -327,  -327,  -327,
     -86,     1,     1,  1377,  1263,  -327,  1443,  -327,  1455,  -327,
    1377,  1263,  -327,  1474,  -327,  1263,  1263,  1263,  -327,  1263,
    -327,  1263,  1263,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,   -10,  -327,  -327,  -327,   660,
     126,   -41,  -327,  -327,  -327,  -327,  1263,   674,  -327,  1704,
     104,  1489,   109,  1263,   110,  1501,   112,   699,  1532,   736,
    1637,   427,   450,   106,   108,   111,   113,   114,   115,   116,
     117,  -327,  -327,  -327,  -327,  1668,  -327,  1377,  1263,  1263,
     -84,  -105,  1263,  1263,  -327,  1263,  -327,  1263,  1263,  -327,
    1263,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,   125,   750,  1680,  -327,  -327,   135,   122,  -327,   123,
     764,  1692,   810,   835,   849,   900,  -105,  -327,  -327,   129,
     124,  1263,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
     135,  -327,   -75,  -327,  -327
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -327,  -327,  -327,  -327,  -327,  -327,  -327,    72,    -1,    18,
    -327,  -327,   -38,  -327,  -327,   241,     7,   206,    38,  -226,
    -327,  -326,  -118,   -85,    95,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,  -327,
    -327,  -327,  -327,  -327
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
      59,   119,   120,   303,   304,   305,   345,     8,    65,   306,
      10,   122,   145,   151,   149,   102,   152,   290,   153,   154,
     359,   218,    95,   121,   294,   150,   151,   149,   148,   152,
     152,   153,   154,   154,   264,   149,   346,   149,   152,   123,
     145,   154,   266,   149,   359,   167,   149,   126,   141,   149,
     282,   149,    65,   284,   168,   169,   288,    86,   344,   145,
     149,   152,   164,   136,   154,   105,   140,   374,   313,   106,
     143,   145,   217,   314,   149,   208,   209,    98,   100,   163,
     145,   104,    99,   269,   138,   139,   107,   307,   308,   309,
     108,   341,   114,   116,   118,   173,   109,   110,   124,   111,
     127,   112,   125,   150,   151,   128,   129,   152,   131,   153,
     154,   191,   130,   132,   194,   174,   133,   176,   134,    30,
     179,   181,   144,   183,   184,   185,   186,   187,   188,   189,
     171,   192,   207,   215,    97,   195,   155,   166,   156,   157,
     158,   159,   146,   160,   147,   222,   172,   162,   177,   155,
     212,   156,   157,   158,   159,   159,   160,   160,    99,   161,
     162,   175,   158,   159,   240,   160,   219,   224,   225,   226,
     146,   227,   147,   228,   229,   230,   231,   232,   233,   234,
     235,   150,   151,   275,   103,   152,   159,   153,   154,   146,
     276,   147,   182,   246,   113,   167,   248,   206,   115,   117,
     253,   146,   165,   147,   196,   149,   211,   100,   213,   197,
     146,   168,   147,   250,   278,   280,   268,   220,   147,   223,
     283,   270,   271,   272,   273,   274,   236,   155,   279,   156,
     157,   158,   159,   277,   160,   312,    88,   161,   162,   317,
     241,   319,   237,   242,   323,   321,   345,   101,   332,   316,
     333,   137,   373,   334,   287,   335,   336,   337,   338,   339,
     356,   289,   291,   361,   370,   362,   371,   243,   289,   295,
       0,   369,     0,   297,   298,   299,     0,   300,     0,   301,
     302,     0,   310,     2,     3,     0,     0,     0,     0,     0,
       0,     4,   320,     0,     0,     0,     5,     6,     7,     0,
       8,     0,     9,    10,   315,   155,     0,   156,   157,   158,
     159,     0,   160,   244,     0,   161,   162,   150,   151,     0,
     245,   152,    11,   153,   154,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,   289,   342,   343,     0,   152,
     350,   351,   154,   352,     0,   353,   354,    94,   355,   150,
     151,     0,     0,   152,    49,   153,   154,     0,     0,     0,
     372,     0,     0,   150,   151,     0,    52,   152,     0,   153,
     154,     0,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,     0,    27,    28,
      29,    30,     0,    31,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,     0,
      47,     0,     0,    48,     0,    49,   150,   151,     0,    50,
     152,     0,   153,   154,    51,     0,     0,    52,     0,     0,
     150,   151,     0,     0,   152,     0,   153,   154,     0,     0,
       0,   155,     0,   156,   157,   158,   159,     0,   160,   251,
       0,   161,   162,   150,   151,     0,   252,   152,     0,   153,
     154,   156,   157,   158,   159,     0,   160,     0,     0,     0,
     162,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,   257,     0,   161,   162,     0,     0,   155,   258,   156,
     157,   158,   159,   152,   160,   259,   154,   161,   162,   150,
     151,     0,   260,   152,     0,   153,   154,     0,     0,     0,
       0,     0,     0,   150,   151,     0,     0,   152,     0,   153,
     154,     0,     0,     0,     0,     0,     0,   150,   151,     0,
       0,   152,     0,   153,   154,     0,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,     0,   160,   262,     0,
     161,   162,     0,     0,   155,   263,   156,   157,   158,   159,
       0,   160,   328,     0,   161,   162,   150,   151,     0,   329,
     152,     0,   153,   154,     0,     0,     0,   155,     0,   156,
     157,   158,   159,     0,   160,   330,     0,   161,   162,   150,
     151,     0,   331,   152,     0,   153,   154,     0,     0,     0,
       0,     0,     0,   150,   151,     0,     0,   152,     0,   153,
     154,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   155,   162,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,     0,     0,   155,   247,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,     0,
       0,   155,   249,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,   150,   151,     0,   254,   152,     0,   153,
     154,     0,     0,     0,     0,     0,     0,   150,   151,     0,
       0,   152,     0,   153,   154,     0,     0,     0,     0,     0,
     155,     0,   156,   157,   158,   159,     0,   160,     0,     0,
     161,   162,   150,   151,     0,   265,   152,     0,   153,   154,
       0,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,     0,     0,   155,   267,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,   150,
     151,     0,   281,   152,     0,   153,   154,     0,     0,     0,
       0,     0,     0,   150,   151,     0,     0,   152,     0,   153,
     154,     0,     0,     0,     0,     0,     0,   150,   151,     0,
       0,   152,     0,   153,   154,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   155,     0,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,     0,
       0,   155,   311,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,   150,   151,     0,   237,   152,     0,   153,
     154,     0,     0,     0,     0,     0,   155,     0,   156,   157,
     158,   159,     0,   160,     0,     0,   161,   162,   150,   151,
       0,   324,   152,     0,   153,   154,     0,     0,     0,     0,
       0,     0,   150,   151,     0,     0,   152,     0,   153,   154,
       0,     0,     0,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,     0,     0,   155,   326,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,     0,
       0,   155,   357,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,   150,   151,     0,   363,   152,     0,   153,
     154,     4,     0,     0,     0,     0,    87,    88,    89,     0,
       8,     0,    90,    10,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   155,     0,   156,
     157,   158,   159,     0,   160,     0,     0,   161,   162,   150,
     151,     0,   365,   152,     0,   153,   154,     0,     0,     0,
       0,     0,   155,     0,   156,   157,   158,   159,     0,   160,
       0,    88,   161,   162,     0,     0,   155,   366,   156,   157,
     158,   159,     0,   160,     0,     0,   161,   162,     0,     0,
       0,   367,    12,    13,    14,    15,    16,    17,    18,    19,
      91,    92,    93,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,    31,    32,    33,     0,     0,     0,     0,
      38,    39,     0,     0,    88,     0,     0,   155,    94,   156,
     157,   158,   159,    48,   160,    49,     0,   161,   162,     0,
       0,     0,   368,     4,    51,   190,     0,    52,    87,    88,
      89,     0,     8,     0,    90,    10,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,     0,     0,   155,     0,   156,   157,   158,   159,   285,
     160,   286,    94,   161,   162,     0,     0,     0,     0,    49,
       0,     0,     0,     0,     0,     0,     0,     0,    97,     4,
       0,    52,     0,     0,    87,    88,    89,     0,     8,     0,
      90,    10,     0,     0,     0,     0,     0,     0,    30,     0,
      31,    32,    33,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    91,    92,    93,    94,     0,     0,     0,     0,
       0,     0,    49,    30,     0,    31,    32,    33,     0,     0,
       0,    99,    38,    39,    52,     0,     0,     4,     0,     0,
      94,     0,    87,    88,    89,    48,     8,    49,    90,    10,
       0,     0,     0,     0,     0,     0,    51,   193,     0,    52,
      12,    13,    14,    15,    16,    17,    18,    19,    91,    92,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,    31,    32,    33,     0,     0,     0,     0,    38,    39,
       0,     0,     0,     4,     0,     0,    94,     0,    87,    88,
      89,    48,     8,    49,    90,    10,     0,     0,     0,     0,
       0,     0,    51,   214,     0,    52,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,    91,    92,    93,     0,
       0,     0,     0,     0,     0,     0,     0,    30,     0,    31,
      32,    33,     0,     0,     0,     0,    38,    39,     0,     0,
       0,     4,     0,     0,    94,     0,    87,    88,    89,    48,
       8,    49,    90,    10,     0,     0,     0,     0,     0,     0,
      51,   221,     0,    52,    12,    13,    14,    15,    16,    17,
      18,    19,    91,    92,    93,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,    31,    32,    33,     0,     0,
       0,     0,    38,    39,     0,     0,     0,     4,     0,     0,
      94,     0,     5,   135,    89,    48,     8,    49,     9,    10,
       0,     0,     0,     0,     0,     0,    51,   239,     0,    52,
       0,     0,    12,    13,    14,    15,    16,    17,    18,    19,
      91,    92,    93,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,    31,    32,    33,     0,     0,     0,     0,
      38,    39,     0,     0,     0,     4,     0,     0,    94,     0,
      87,    88,    89,    48,     8,    49,    90,    10,     0,   150,
     151,     0,     0,   152,    51,   153,   154,    52,    12,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,   150,
     151,     0,     0,   152,     0,   153,   154,    30,     0,    31,
      32,    33,     0,     0,   150,   151,    38,    39,   152,     0,
     153,   154,     0,     0,    94,     0,   150,   151,     0,    48,
     152,    49,   153,   154,     0,     0,     0,     0,   150,   151,
      51,     0,   152,    52,   153,   154,    12,    13,    14,    15,
      16,    17,    18,    19,    91,    92,    93,   150,   151,     0,
       0,   152,     0,   153,   154,    30,     0,    31,    32,    33,
       0,     0,   150,   151,    38,    39,   152,     0,   153,   154,
       0,     0,    94,     0,   150,   151,     0,    48,   152,    49,
     153,   154,     0,     0,     0,     0,     0,     0,   178,     0,
       0,    52,     0,   155,     0,   156,   157,   158,   159,     0,
     160,   255,     0,   161,   162,   150,   151,     0,     0,   152,
       0,   153,   154,   155,     0,   156,   157,   158,   159,     0,
     160,   256,     0,   161,   162,     0,     0,     0,   155,     0,
     156,   157,   158,   159,     0,   160,   261,     0,   161,   162,
     155,     0,   156,   157,   158,   159,   292,   160,     0,     0,
     161,   162,   155,     0,   156,   157,   158,   159,     0,   160,
     293,     0,   161,   162,     0,     0,     0,     0,     0,     0,
       0,   155,     0,   156,   157,   158,   159,   296,   160,     0,
       0,   161,   162,     0,     0,     0,   155,     0,   156,   157,
     158,   159,     0,   160,   318,     0,   161,   162,   155,     0,
     156,   157,   158,   159,     0,   160,   322,     0,   161,   162,
     150,   151,     0,     0,   152,     0,   153,   154,     0,     0,
       0,   198,   199,   200,     0,     8,     0,   201,    10,   155,
       0,   156,   157,   158,   159,     0,   160,   325,     0,   161,
     162,   150,   151,     0,     0,   152,     0,   153,   154,     0,
       0,     0,     0,   150,   151,     0,     0,   152,     0,   153,
     154,     0,     0,     0,     0,   150,   151,     0,     0,   152,
       0,   153,   154,     0,     0,     0,     0,   150,   151,     0,
       0,   152,     0,   153,   154,     0,     0,     0,     0,    -1,
     151,     0,     0,   152,     0,   153,   154,     0,     0,     0,
       0,     0,     0,     0,     0,   202,   203,   204,     0,     0,
       0,     0,     0,     0,     0,     0,    30,     0,    31,    32,
      33,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    88,     0,    94,   155,     0,   156,   157,   158,   159,
      49,   160,   327,     0,   161,   162,     0,     0,     0,    88,
     205,     0,    52,     0,    88,     0,     0,     0,     0,    88,
       0,     0,     0,     0,     0,   155,     0,   156,   157,   158,
     159,   340,   160,     0,     0,   161,   162,   155,     0,   156,
     157,   158,   159,   358,   160,     0,     0,   161,   162,   155,
       0,   156,   157,   158,   159,   364,   160,    88,     0,   161,
     162,   155,     0,   156,   157,   158,   159,     0,   160,     0,
       0,   161,   162,   155,     0,   156,   157,   158,   159,     0,
     160,     0,     0,   161,   162,    30,     0,    31,    32,    33,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    94,    30,     0,    31,    32,    33,    30,    49,
      31,    32,    33,    30,     0,    31,    32,    33,   103,     0,
      94,    52,     0,     0,     0,    94,     0,    49,     0,     0,
      94,     0,    49,     0,     0,     0,   113,    49,     0,    52,
       0,   115,     0,     0,    52,     0,   117,     0,     0,    52,
       0,    30,     0,    31,    32,    33,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    94,     0,
       0,     0,     0,     0,     0,    49,     0,     0,     0,     0,
       0,     0,     0,     0,   165,     0,     0,    52
};

static const yytype_int16 yycheck[] =
{
       1,   108,   109,    13,    14,    15,   111,    17,     1,    19,
      20,   108,    11,     4,   135,   141,     7,   243,     9,    10,
     346,   142,     4,    24,   250,     3,     4,   135,   126,     7,
       7,     9,    10,    10,   142,   135,   141,   135,     7,   136,
      11,    10,   142,   135,   370,   126,   135,    29,    49,   135,
     142,   135,    45,   142,   135,   136,   142,   136,   142,    11,
     135,     7,    63,    45,    10,   141,    48,   142,   109,   141,
      52,    11,   133,   114,   135,   108,   109,     5,     6,    61,
      11,     9,   141,   142,    46,    47,   141,    97,    98,    99,
     141,   317,    20,    21,    22,    77,   141,   141,    26,   141,
     108,   141,   141,     3,     4,   109,   141,     7,   108,     9,
      10,   112,   141,   141,   115,    97,   108,    99,   141,   108,
     102,   103,   136,   105,   106,   107,   108,   109,   110,   111,
     135,   113,   125,   134,   141,   117,   127,    65,   129,   130,
     131,   132,   141,   134,   143,   146,    74,   138,   109,   127,
     132,   129,   130,   131,   132,   132,   134,   134,   141,   137,
     138,   132,   131,   132,   165,   134,   144,   149,   150,   151,
     141,   153,   143,   155,   156,   157,   158,   159,   160,   161,
     162,     3,     4,   135,   141,     7,   132,     9,    10,   141,
     142,   143,   132,   175,   141,   126,   178,   125,   141,   141,
     182,   141,   141,   143,   136,   135,   108,   135,   109,   136,
     141,   135,   143,   135,   109,   108,   142,   145,   143,   147,
     136,   142,   142,   142,   142,   142,   126,   127,   210,   129,
     130,   131,   132,   142,   134,   109,    14,   137,   138,   135,
     168,   132,   142,   171,   132,   135,   111,     6,   142,   287,
     142,    45,   370,   142,   236,   142,   142,   142,   142,   142,
     135,   243,   244,   141,   135,   142,   142,   172,   250,   251,
      -1,   356,    -1,   255,   256,   257,    -1,   259,    -1,   261,
     262,    -1,   275,     0,     1,    -1,    -1,    -1,    -1,    -1,
      -1,     8,   293,    -1,    -1,    -1,    13,    14,    15,    -1,
      17,    -1,    19,    20,   286,   127,    -1,   129,   130,   131,
     132,    -1,   134,   135,    -1,   137,   138,     3,     4,    -1,
     142,     7,    39,     9,    10,    -1,    -1,    -1,    -1,    -1,
     108,    -1,   110,   111,   112,   317,   318,   319,    -1,     7,
     322,   323,    10,   325,    -1,   327,   328,   125,   330,     3,
       4,    -1,    -1,     7,   132,     9,    10,    -1,    -1,    -1,
     361,    -1,    -1,     3,     4,    -1,   144,     7,    -1,     9,
      10,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,    -1,   105,   106,
     107,   108,    -1,   110,   111,   112,   113,   114,   115,   116,
     117,   118,   119,   120,   121,   122,   123,   124,   125,    -1,
     127,    -1,    -1,   130,    -1,   132,     3,     4,    -1,   136,
       7,    -1,     9,    10,   141,    -1,    -1,   144,    -1,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,   127,    -1,   129,   130,   131,   132,    -1,   134,   135,
      -1,   137,   138,     3,     4,    -1,   142,     7,    -1,     9,
      10,   129,   130,   131,   132,    -1,   134,    -1,    -1,    -1,
     138,    -1,    -1,   127,    -1,   129,   130,   131,   132,    -1,
     134,   135,    -1,   137,   138,    -1,    -1,   127,   142,   129,
     130,   131,   132,     7,   134,   135,    10,   137,   138,     3,
       4,    -1,   142,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
     127,    -1,   129,   130,   131,   132,    -1,   134,   135,    -1,
     137,   138,    -1,    -1,   127,   142,   129,   130,   131,   132,
      -1,   134,   135,    -1,   137,   138,     3,     4,    -1,   142,
       7,    -1,     9,    10,    -1,    -1,    -1,   127,    -1,   129,
     130,   131,   132,    -1,   134,   135,    -1,   137,   138,     3,
       4,    -1,   142,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,   127,    -1,   129,   130,   131,   132,    -1,
     134,    -1,    -1,   127,   138,   129,   130,   131,   132,    -1,
     134,    -1,    -1,   137,   138,    -1,    -1,   127,   142,   129,
     130,   131,   132,    -1,   134,    -1,    -1,   137,   138,    -1,
      -1,   127,   142,   129,   130,   131,   132,    -1,   134,    -1,
      -1,   137,   138,     3,     4,    -1,   142,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
     127,    -1,   129,   130,   131,   132,    -1,   134,    -1,    -1,
     137,   138,     3,     4,    -1,   142,     7,    -1,     9,    10,
      -1,    -1,    -1,   127,    -1,   129,   130,   131,   132,    -1,
     134,    -1,    -1,   137,   138,    -1,    -1,   127,   142,   129,
     130,   131,   132,    -1,   134,    -1,    -1,   137,   138,     3,
       4,    -1,   142,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,    -1,   129,
     130,   131,   132,    -1,   134,    -1,    -1,   137,   138,    -1,
      -1,   127,   142,   129,   130,   131,   132,    -1,   134,    -1,
      -1,   137,   138,     3,     4,    -1,   142,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,   127,    -1,   129,   130,
     131,   132,    -1,   134,    -1,    -1,   137,   138,     3,     4,
      -1,   142,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,   127,    -1,   129,   130,   131,   132,    -1,
     134,    -1,    -1,   137,   138,    -1,    -1,   127,   142,   129,
     130,   131,   132,    -1,   134,    -1,    -1,   137,   138,    -1,
      -1,   127,   142,   129,   130,   131,   132,    -1,   134,    -1,
      -1,   137,   138,     3,     4,    -1,   142,     7,    -1,     9,
      10,     8,    -1,    -1,    -1,    -1,    13,    14,    15,    -1,
      17,    -1,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   127,    -1,   129,
     130,   131,   132,    -1,   134,    -1,    -1,   137,   138,     3,
       4,    -1,   142,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,   127,    -1,   129,   130,   131,   132,    -1,   134,
      -1,    14,   137,   138,    -1,    -1,   127,   142,   129,   130,
     131,   132,    -1,   134,    -1,    -1,   137,   138,    -1,    -1,
      -1,   142,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,    -1,   110,   111,   112,    -1,    -1,    -1,    -1,
     117,   118,    -1,    -1,    14,    -1,    -1,   127,   125,   129,
     130,   131,   132,   130,   134,   132,    -1,   137,   138,    -1,
      -1,    -1,   142,     8,   141,   142,    -1,   144,    13,    14,
      15,    -1,    17,    -1,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   108,    -1,   110,   111,   112,
      -1,    -1,    -1,   127,    -1,   129,   130,   131,   132,   133,
     134,   135,   125,   137,   138,    -1,    -1,    -1,    -1,   132,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   141,     8,
      -1,   144,    -1,    -1,    13,    14,    15,    -1,    17,    -1,
      19,    20,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,
     110,   111,   112,    -1,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   125,    -1,    -1,    -1,    -1,
      -1,    -1,   132,   108,    -1,   110,   111,   112,    -1,    -1,
      -1,   141,   117,   118,   144,    -1,    -1,     8,    -1,    -1,
     125,    -1,    13,    14,    15,   130,    17,   132,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,   141,   142,    -1,   144,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,
      -1,   110,   111,   112,    -1,    -1,    -1,    -1,   117,   118,
      -1,    -1,    -1,     8,    -1,    -1,   125,    -1,    13,    14,
      15,   130,    17,   132,    19,    20,    -1,    -1,    -1,    -1,
      -1,    -1,   141,   142,    -1,   144,    -1,    -1,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,   110,
     111,   112,    -1,    -1,    -1,    -1,   117,   118,    -1,    -1,
      -1,     8,    -1,    -1,   125,    -1,    13,    14,    15,   130,
      17,   132,    19,    20,    -1,    -1,    -1,    -1,    -1,    -1,
     141,   142,    -1,   144,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   108,    -1,   110,   111,   112,    -1,    -1,
      -1,    -1,   117,   118,    -1,    -1,    -1,     8,    -1,    -1,
     125,    -1,    13,    14,    15,   130,    17,   132,    19,    20,
      -1,    -1,    -1,    -1,    -1,    -1,   141,   142,    -1,   144,
      -1,    -1,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   108,    -1,   110,   111,   112,    -1,    -1,    -1,    -1,
     117,   118,    -1,    -1,    -1,     8,    -1,    -1,   125,    -1,
      13,    14,    15,   130,    17,   132,    19,    20,    -1,     3,
       4,    -1,    -1,     7,   141,     9,    10,   144,    89,    90,
      91,    92,    93,    94,    95,    96,    97,    98,    99,     3,
       4,    -1,    -1,     7,    -1,     9,    10,   108,    -1,   110,
     111,   112,    -1,    -1,     3,     4,   117,   118,     7,    -1,
       9,    10,    -1,    -1,   125,    -1,     3,     4,    -1,   130,
       7,   132,     9,    10,    -1,    -1,    -1,    -1,     3,     4,
     141,    -1,     7,   144,     9,    10,    89,    90,    91,    92,
      93,    94,    95,    96,    97,    98,    99,     3,     4,    -1,
      -1,     7,    -1,     9,    10,   108,    -1,   110,   111,   112,
      -1,    -1,     3,     4,   117,   118,     7,    -1,     9,    10,
      -1,    -1,   125,    -1,     3,     4,    -1,   130,     7,   132,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,   141,    -1,
      -1,   144,    -1,   127,    -1,   129,   130,   131,   132,    -1,
     134,   135,    -1,   137,   138,     3,     4,    -1,    -1,     7,
      -1,     9,    10,   127,    -1,   129,   130,   131,   132,    -1,
     134,   135,    -1,   137,   138,    -1,    -1,    -1,   127,    -1,
     129,   130,   131,   132,    -1,   134,   135,    -1,   137,   138,
     127,    -1,   129,   130,   131,   132,   133,   134,    -1,    -1,
     137,   138,   127,    -1,   129,   130,   131,   132,    -1,   134,
     135,    -1,   137,   138,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   127,    -1,   129,   130,   131,   132,   133,   134,    -1,
      -1,   137,   138,    -1,    -1,    -1,   127,    -1,   129,   130,
     131,   132,    -1,   134,   135,    -1,   137,   138,   127,    -1,
     129,   130,   131,   132,    -1,   134,   135,    -1,   137,   138,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    13,    14,    15,    -1,    17,    -1,    19,    20,   127,
      -1,   129,   130,   131,   132,    -1,   134,   135,    -1,   137,
     138,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    98,    99,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   108,    -1,   110,   111,
     112,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    14,    -1,   125,   127,    -1,   129,   130,   131,   132,
     132,   134,   135,    -1,   137,   138,    -1,    -1,    -1,    14,
     142,    -1,   144,    -1,    14,    -1,    -1,    -1,    -1,    14,
      -1,    -1,    -1,    -1,    -1,   127,    -1,   129,   130,   131,
     132,   133,   134,    -1,    -1,   137,   138,   127,    -1,   129,
     130,   131,   132,   133,   134,    -1,    -1,   137,   138,   127,
      -1,   129,   130,   131,   132,   133,   134,    14,    -1,   137,
     138,   127,    -1,   129,   130,   131,   132,    -1,   134,    -1,
      -1,   137,   138,   127,    -1,   129,   130,   131,   132,    -1,
     134,    -1,    -1,   137,   138,   108,    -1,   110,   111,   112,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   125,   108,    -1,   110,   111,   112,   108,   132,
     110,   111,   112,   108,    -1,   110,   111,   112,   141,    -1,
     125,   144,    -1,    -1,    -1,   125,    -1,   132,    -1,    -1,
     125,    -1,   132,    -1,    -1,    -1,   141,   132,    -1,   144,
      -1,   141,    -1,    -1,   144,    -1,   141,    -1,    -1,   144,
      -1,   108,    -1,   110,   111,   112,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   125,    -1,
      -1,    -1,    -1,    -1,    -1,   132,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   141,    -1,    -1,   144
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   146,     0,     1,     8,    13,    14,    15,    17,    19,
      20,    39,    89,    90,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   105,   106,   107,
     108,   110,   111,   112,   113,   114,   115,   116,   117,   118,
     119,   120,   121,   122,   123,   124,   125,   127,   130,   132,
     136,   141,   144,   147,   148,   149,   150,   151,   152,   153,
     154,   156,   158,   159,   160,   161,   162,   163,   170,   172,
     173,   174,   175,   176,   177,   178,   179,   180,   181,   182,
     183,   184,   185,   186,   187,   188,   136,    13,    14,    15,
      19,    97,    98,    99,   125,   154,   161,   141,   152,   141,
     152,   160,   141,   141,   152,   141,   141,   141,   141,   141,
     141,   141,   141,   141,   152,   141,   152,   141,   152,   108,
     109,   153,   108,   136,   152,   141,   154,   108,   109,   141,
     141,   108,   141,   108,   141,    14,   154,   162,   163,   163,
     154,   153,   153,   154,   136,    11,   141,   143,   126,   135,
       3,     4,     7,     9,    10,   127,   129,   130,   131,   132,
     134,   137,   138,   154,   153,   141,   152,   126,   135,   136,
     169,   135,   152,   154,   154,   132,   154,   109,   141,   154,
     164,   154,   132,   154,   154,   154,   154,   154,   154,   154,
     142,   153,   154,   142,   153,   154,   136,   136,    13,    14,
      15,    19,    97,    98,    99,   142,   152,   161,   108,   109,
     155,   108,   154,   109,   142,   153,   171,   133,   142,   144,
     152,   142,   153,   152,   154,   154,   154,   154,   154,   154,
     154,   154,   154,   154,   154,   154,   126,   142,   157,   142,
     153,   152,   152,   169,   135,   142,   154,   142,   154,   142,
     135,   135,   142,   154,   142,   135,   135,   135,   142,   135,
     142,   135,   135,   142,   142,   142,   142,   142,   142,   142,
     142,   142,   142,   142,   142,   135,   142,   142,   109,   154,
     108,   142,   142,   136,   142,   133,   135,   154,   142,   154,
     164,   154,   133,   135,   164,   154,   133,   154,   154,   154,
     154,   154,   154,    13,    14,    15,    19,    97,    98,    99,
     161,   142,   109,   109,   114,   154,   157,   135,   135,   132,
     153,   135,   135,   132,   142,   135,   142,   135,   135,   142,
     135,   142,   142,   142,   142,   142,   142,   142,   142,   142,
     133,   164,   154,   154,   142,   111,   141,   165,   166,   168,
     154,   154,   154,   154,   154,   154,   135,   142,   133,   166,
     167,   141,   142,   142,   133,   142,   142,   142,   142,   168,
     135,   142,   153,   167,   142
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
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), '.', &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 34:
#line 510 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'(')) YYERROR;
          ;}
    break;

  case 35:
#line 514 "grammar.y"
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

  case 36:
#line 527 "grammar.y"
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

  case 37:
#line 555 "grammar.y"
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

  case 38:
#line 579 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[(1) - (1)].i);
            (yyval.lv).data = (yyval.lv).Data();
          ;}
    break;

  case 39:
#line 585 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[(1) - (1)].name);
          ;}
    break;

  case 40:
#line 591 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 41:
#line 598 "grammar.y"
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

  case 42:
#line 609 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 43:
#line 615 "grammar.y"
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 44:
#line 619 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (1)].lv); ;}
    break;

  case 45:
#line 620 "grammar.y"
    { (yyval.lv) = (yyvsp[(2) - (3)].lv); ;}
    break;

  case 46:
#line 622 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[(1) - (6)].lv),&(yyvsp[(3) - (6)].lv),&(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 47:
#line 626 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'[',&(yyvsp[(3) - (4)].lv))) YYERROR;
          ;}
    break;

  case 48:
#line 630 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 49:
#line 634 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 50:
#line 638 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 51:
#line 642 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 52:
#line 646 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 53:
#line 650 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 54:
#line 654 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 55:
#line 658 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 56:
#line 662 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 57:
#line 666 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 58:
#line 670 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
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
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 61:
#line 682 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 62:
#line 686 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 63:
#line 690 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
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
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 66:
#line 702 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 67:
#line 706 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 68:
#line 710 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),MATRIX_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 69:
#line 714 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),MATRIX_CMD)) YYERROR;
          ;}
    break;

  case 70:
#line 718 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),INTMAT_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 71:
#line 722 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),INTMAT_CMD)) YYERROR;
          ;}
    break;

  case 72:
#line 726 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 73:
#line 730 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),RING_CMD)) YYERROR;
          ;}
    break;

  case 74:
#line 734 "grammar.y"
    {
            (yyval.lv)=(yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 75:
#line 738 "grammar.y"
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

  case 76:
#line 755 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 77:
#line 761 "grammar.y"
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

  case 78:
#line 773 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 79:
#line 781 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 80:
#line 790 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),PLUSPLUS)) YYERROR;
          ;}
    break;

  case 81:
#line 794 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),MINUSMINUS)) YYERROR;
          ;}
    break;

  case 82:
#line 798 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'+',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 83:
#line 802 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'-',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 84:
#line 806 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 85:
#line 810 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'^',&(yyvsp[(3) - (3)].lv))) YYERROR;
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
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 88:
#line 822 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),NOTEQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 89:
#line 826 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),EQUAL_EQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 90:
#line 830 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),DOTDOT,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 91:
#line 834 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),':',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 92:
#line 838 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            int i; TESTSETINT((yyvsp[(2) - (2)].lv),i);
            (yyval.lv).rtyp  = INT_CMD;
            (yyval.lv).data = (void *)(long)(i == 0 ? 1 : 0);
          ;}
    break;

  case 93:
#line 845 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),'-')) YYERROR;
          ;}
    break;

  case 94:
#line 851 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (2)].lv); ;}
    break;

  case 95:
#line 853 "grammar.y"
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

  case 97:
#line 873 "grammar.y"
    {
            if ((yyvsp[(2) - (3)].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[(2) - (3)].lv).CopyD(STRING_CMD);
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 100:
#line 890 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 101:
#line 895 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
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
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 104:
#line 908 "grammar.y"
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

  case 105:
#line 924 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 106:
#line 928 "grammar.y"
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

  case 107:
#line 944 "grammar.y"
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

  case 108:
#line 958 "grammar.y"
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

  case 109:
#line 979 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 112:
#line 992 "grammar.y"
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

  case 113:
#line 1006 "grammar.y"
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[(1) - (1)].name));
        ;}
    break;

  case 114:
#line 1014 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = (yyvsp[(1) - (1)].i);
            (yyval.lv).rtyp = INTVEC_CMD;
            (yyval.lv).data = (void *)iv;
          ;}
    break;

  case 115:
#line 1023 "grammar.y"
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

  case 117:
#line 1069 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[(3) - (3)].lv),sizeof(sleftv));
          ;}
    break;

  case 119:
#line 1079 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 120:
#line 1085 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 121:
#line 1097 "grammar.y"
    { if ((yyvsp[(1) - (2)].i) != '<') YYERROR;
            if((feFilePending=feFopen((yyvsp[(2) - (2)].name),"r",NULL,TRUE))==NULL) YYERROR; ;}
    break;

  case 122:
#line 1100 "grammar.y"
    { newFile((yyvsp[(2) - (4)].name),feFilePending); ;}
    break;

  case 123:
#line 1105 "grammar.y"
    {
            feHelp((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 124:
#line 1110 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 125:
#line 1117 "grammar.y"
    {
            singular_example((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 126:
#line 1125 "grammar.y"
    {
          if (basePack!=(yyvsp[(2) - (2)].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[(2) - (2)].lv),0,currPackHdl)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[(2) - (2)].lv),0)) YYERROR;
        ;}
    break;

  case 127:
#line 1137 "grammar.y"
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

  case 128:
#line 1153 "grammar.y"
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

  case 129:
#line 1172 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 130:
#line 1176 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 131:
#line 1180 "grammar.y"
    {
            if ((yyvsp[(3) - (4)].i)==QRING_CMD) (yyvsp[(3) - (4)].i)=RING_CMD;
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 132:
#line 1185 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 133:
#line 1189 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 134:
#line 1193 "grammar.y"
    {
            list_cmd(MATRIX_CMD,NULL,"// ",TRUE);
           ;}
    break;

  case 135:
#line 1197 "grammar.y"
    {
            list_cmd(INTMAT_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 136:
#line 1201 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 137:
#line 1205 "grammar.y"
    {
            list_cmd(0,(yyvsp[(3) - (4)].lv).Fullname(),"// ",TRUE);
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 138:
#line 1210 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 139:
#line 1216 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 140:
#line 1222 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 141:
#line 1228 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 142:
#line 1234 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 143:
#line 1240 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 144:
#line 1246 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 145:
#line 1252 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 146:
#line 1264 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 147:
#line 1270 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 148:
#line 1279 "grammar.y"
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

  case 149:
#line 1313 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (2)].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[(2) - (2)].lv).CleanUp();
          ;}
    break;

  case 150:
#line 1323 "grammar.y"
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

  case 153:
#line 1339 "grammar.y"
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

  case 154:
#line 1411 "grammar.y"
    {
            if ((yyvsp[(2) - (2)].lv).rtyp!=IDHDL) MYYERROR("identifier expected");
            idhdl h = (idhdl)(yyvsp[(2) - (2)].lv).data;
            type_cmd(h);
          ;}
    break;

  case 155:
#line 1417 "grammar.y"
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

  case 156:
#line 1446 "grammar.y"
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

  case 157:
#line 1459 "grammar.y"
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

  case 158:
#line 1476 "grammar.y"
    {
            int i; TESTSETINT((yyvsp[(3) - (5)].lv),i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 159:
#line 1485 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 160:
#line 1490 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 161:
#line 1498 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen((yyvsp[(2) - (3)].name)) + strlen((yyvsp[(3) - (3)].name)) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,(yyvsp[(2) - (3)].name),(yyvsp[(3) - (3)].name));
            newBuffer(s,BT_break);
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 162:
#line 1510 "grammar.y"
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

  case 163:
#line 1529 "grammar.y"
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

  case 164:
#line 1540 "grammar.y"
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

  case 165:
#line 1560 "grammar.y"
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

  case 166:
#line 1584 "grammar.y"
    {
	    // decl. of type proc p(int i)
            if ((yyvsp[(1) - (2)].i)==PARAMETER)  { if (iiParameter(&(yyvsp[(2) - (2)].lv))) YYERROR; }
	    else                { if (iiAlias(&(yyvsp[(2) - (2)].lv))) YYERROR; } 
          ;}
    break;

  case 167:
#line 1590 "grammar.y"
    {
	    // decl. of type proc p(i)
            sleftv tmp_expr;
	    if ((yyvsp[(1) - (2)].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[(2) - (2)].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 168:
#line 1602 "grammar.y"
    {
            if(iiRETURNEXPR==NULL) YYERROR;
            iiRETURNEXPR[myynest].Copy(&(yyvsp[(3) - (4)].lv));
            (yyvsp[(3) - (4)].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 169:
#line 1609 "grammar.y"
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
#line 3940 "grammar.cc"
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



