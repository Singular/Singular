/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C
   
      Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006,
   2009, 2010 Free Software Foundation, Inc.
   
   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.
   
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.
   
   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

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
#define YYBISON_VERSION "2.4.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1

/* Using locations.  */
#define YYLSP_NEEDED 0



/* Copy the first part of user declarations.  */

/* Line 189 of yacc.c  */
#line 7 "grammar.y"


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "kernel/mod2.h"
#include "Singular/grammar.h"

#include "misc/mylimits.h"
#include "omalloc/omalloc.h"
#include "Singular/tok.h"
#include "misc/options.h"
#include "Singular/stype.h"
#include "Singular/fehelp.h"
#include "Singular/ipid.h"
#include "misc/intvec.h"
#include "kernel/oswrapper/feread.h"
#include "Singular/fevoices.h"
#include "polys/matpol.h"
#include "polys/monomials/ring.h"
#include "kernel/GBEngine/kstd1.h"
#include "Singular/subexpr.h"
#include "Singular/ipshell.h"
#include "Singular/ipconv.h"
#include "Singular/sdb.h"
#include "kernel/ideals.h"
#include "coeffs/numbers.h"
#include "kernel/polys.h"
#include "kernel/combinatorics/stairc.h"
#include "kernel/oswrapper/timer.h"
#include "Singular/cntrlc.h"
#include "polys/monomials/maps.h"
#include "kernel/GBEngine/syz.h"
#include "Singular/lists.h"
#include "Singular/libparse.h"
#include "coeffs/bigintmat.h"

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
    || (IDTYP(r)==RING_CMD))
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
#define YYMAXDEPTH MAX_INT_VAL

extern int   yylineno;
extern FILE* yyin;

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
   (i) = (int)((long)(a).Data());(a).CleanUp()

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
}



/* Line 189 of yacc.c  */
#line 241 "grammar.cc"

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
     ARROW = 267,
     GRING_CMD = 268,
     BIGINTMAT_CMD = 269,
     INTMAT_CMD = 270,
     PROC_CMD = 271,
     RING_CMD = 272,
     BEGIN_RING = 273,
     BUCKET_CMD = 274,
     IDEAL_CMD = 275,
     MAP_CMD = 276,
     MATRIX_CMD = 277,
     MODUL_CMD = 278,
     NUMBER_CMD = 279,
     POLY_CMD = 280,
     RESOLUTION_CMD = 281,
     SMATRIX_CMD = 282,
     VECTOR_CMD = 283,
     BETTI_CMD = 284,
     E_CMD = 285,
     FETCH_CMD = 286,
     FREEMODULE_CMD = 287,
     KEEPRING_CMD = 288,
     IMAP_CMD = 289,
     KOSZUL_CMD = 290,
     MAXID_CMD = 291,
     MONOM_CMD = 292,
     PAR_CMD = 293,
     PREIMAGE_CMD = 294,
     VAR_CMD = 295,
     VALTVARS = 296,
     VMAXDEG = 297,
     VMAXMULT = 298,
     VNOETHER = 299,
     VMINPOLY = 300,
     END_RING = 301,
     CMD_1 = 302,
     CMD_2 = 303,
     CMD_3 = 304,
     CMD_12 = 305,
     CMD_13 = 306,
     CMD_23 = 307,
     CMD_123 = 308,
     CMD_M = 309,
     ROOT_DECL = 310,
     ROOT_DECL_LIST = 311,
     RING_DECL = 312,
     RING_DECL_LIST = 313,
     EXAMPLE_CMD = 314,
     EXPORT_CMD = 315,
     HELP_CMD = 316,
     KILL_CMD = 317,
     LIB_CMD = 318,
     LISTVAR_CMD = 319,
     SETRING_CMD = 320,
     TYPE_CMD = 321,
     STRINGTOK = 322,
     BLOCKTOK = 323,
     INT_CONST = 324,
     UNKNOWN_IDENT = 325,
     RINGVAR = 326,
     PROC_DEF = 327,
     APPLY = 328,
     ASSUME_CMD = 329,
     BREAK_CMD = 330,
     CONTINUE_CMD = 331,
     ELSE_CMD = 332,
     EVAL = 333,
     QUOTE = 334,
     FOR_CMD = 335,
     IF_CMD = 336,
     SYS_BREAK = 337,
     WHILE_CMD = 338,
     RETURN = 339,
     PARAMETER = 340,
     SYSVAR = 341,
     UMINUS = 342
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 369 "grammar.cc"

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
# if defined YYENABLE_NLS && YYENABLE_NLS
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
YYID (int yyi)
#else
static int
YYID (yyi)
    int yyi;
#endif
{
  return yyi;
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
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
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
# define YYSTACK_RELOCATE(Stack_alloc, Stack)				\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack_alloc, Stack, yysize);			\
	Stack = &yyptr->Stack_alloc;					\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2567

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  104
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  175
/* YYNRULES -- Number of states.  */
#define YYNSTATES  398

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   342

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    97,     2,
     100,   101,     2,    89,    95,    90,   102,    91,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    98,    96,
      88,    87,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    92,     2,    93,    94,     2,   103,     2,     2,     2,
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
      85,    86,    99
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
      95,   100,   105,   110,   114,   119,   124,   128,   133,   138,
     143,   148,   155,   162,   169,   176,   185,   194,   203,   212,
     216,   221,   230,   235,   244,   249,   253,   257,   261,   263,
     265,   267,   274,   279,   286,   293,   300,   307,   314,   321,
     325,   331,   337,   338,   344,   347,   350,   352,   355,   358,
     362,   366,   370,   374,   378,   382,   386,   390,   394,   398,
     401,   404,   407,   410,   412,   416,   419,   422,   425,   428,
     437,   440,   444,   447,   449,   451,   457,   459,   461,   466,
     468,   472,   474,   478,   480,   482,   484,   486,   488,   491,
     495,   498,   502,   505,   508,   512,   517,   522,   527,   532,
     537,   542,   547,   552,   559,   566,   573,   580,   587,   594,
     601,   605,   607,   616,   619,   624,   632,   635,   637,   639,
     642,   645,   647,   653,   656,   662,   664,   666,   670,   676,
     680,   684,   689,   692,   695,   700
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     105,     0,    -1,    -1,   105,   106,    -1,   107,    -1,   109,
      96,    -1,   121,    96,    -1,   147,    -1,    82,    -1,    96,
      -1,     1,    96,    -1,   142,    -1,   143,    -1,   108,    -1,
     144,    -1,   145,    -1,   130,    -1,   131,    -1,   132,    -1,
      59,    68,    -1,   110,    -1,   133,    -1,   134,    -1,   135,
      -1,   146,    -1,   137,    -1,   138,    -1,   140,    -1,   141,
      -1,   119,   112,    -1,    71,    -1,   120,    -1,   111,    11,
     111,    -1,   113,   102,   111,    -1,   111,   100,   101,    -1,
     111,   100,   112,   101,    -1,    92,   112,    93,    -1,    69,
      -1,    86,    -1,   122,    -1,    16,   100,   113,   101,    -1,
      55,   100,   113,   101,    -1,    56,   100,   112,   101,    -1,
      56,   100,   101,    -1,    57,   100,   113,   101,    -1,    58,
     100,   112,   101,    -1,    58,   100,   101,    -1,    47,   100,
     113,   101,    -1,    50,   100,   113,   101,    -1,    51,   100,
     113,   101,    -1,    53,   100,   113,   101,    -1,    48,   100,
     113,    95,   113,   101,    -1,    50,   100,   113,    95,   113,
     101,    -1,    52,   100,   113,    95,   113,   101,    -1,    53,
     100,   113,    95,   113,   101,    -1,    49,   100,   113,    95,
     113,    95,   113,   101,    -1,    51,   100,   113,    95,   113,
      95,   113,   101,    -1,    52,   100,   113,    95,   113,    95,
     113,   101,    -1,    53,   100,   113,    95,   113,    95,   113,
     101,    -1,    54,   100,   101,    -1,    54,   100,   112,   101,
      -1,   129,   100,   113,    95,   113,    95,   113,   101,    -1,
     129,   100,   113,   101,    -1,    17,   100,   123,    95,   123,
      95,   127,   101,    -1,    17,   100,   113,   101,    -1,   120,
      12,    68,    -1,   100,   112,   101,    -1,   112,    95,   113,
      -1,   113,    -1,   118,    -1,   111,    -1,   113,    92,   113,
      95,   113,    93,    -1,   113,    92,   113,    93,    -1,    73,
     100,   113,    95,    47,   101,    -1,    73,   100,   113,    95,
      50,   101,    -1,    73,   100,   113,    95,    51,   101,    -1,
      73,   100,   113,    95,    53,   101,    -1,    73,   100,   113,
      95,    54,   101,    -1,    73,   100,   113,    95,   113,   101,
      -1,   115,   113,   117,    -1,   115,   113,    87,   113,   117,
      -1,   116,   113,    95,   113,   117,    -1,    -1,    78,   100,
     114,   113,   101,    -1,    79,   100,    -1,    74,   100,    -1,
     101,    -1,   113,    10,    -1,   113,     7,    -1,   113,    89,
     113,    -1,   113,    90,   113,    -1,   113,    91,   113,    -1,
     113,    94,   113,    -1,   113,    88,   113,    -1,   113,    97,
     113,    -1,   113,     9,   113,    -1,   113,     4,   113,    -1,
     113,     3,   113,    -1,   113,    98,   113,    -1,     8,   113,
      -1,    90,   113,    -1,   121,   128,    -1,   112,   128,    -1,
      70,    -1,   103,   113,   103,    -1,    55,   111,    -1,    56,
     111,    -1,    57,   111,    -1,    58,   111,    -1,   129,   111,
      92,   113,    93,    92,   113,    93,    -1,   129,   111,    -1,
     121,    95,   111,    -1,    16,   111,    -1,    67,    -1,   113,
      -1,   100,   113,    95,   112,   101,    -1,    70,    -1,   124,
      -1,   124,   100,   112,   101,    -1,   125,    -1,   125,    95,
     126,    -1,   125,    -1,   100,   126,   101,    -1,    87,    -1,
      22,    -1,    27,    -1,    15,    -1,    14,    -1,    88,   122,
      -1,    61,    67,    96,    -1,    61,    96,    -1,    59,    67,
      96,    -1,    60,   112,    -1,    62,   111,    -1,   134,    95,
     111,    -1,    64,   100,    55,   101,    -1,    64,   100,    56,
     101,    -1,    64,   100,    57,   101,    -1,    64,   100,    58,
     101,    -1,    64,   100,    17,   101,    -1,    64,   100,   129,
     101,    -1,    64,   100,    16,   101,    -1,    64,   100,   111,
     101,    -1,    64,   100,   111,    95,    55,   101,    -1,    64,
     100,   111,    95,    56,   101,    -1,    64,   100,   111,    95,
      57,   101,    -1,    64,   100,   111,    95,    58,   101,    -1,
      64,   100,   111,    95,    17,   101,    -1,    64,   100,   111,
      95,   129,   101,    -1,    64,   100,   111,    95,    16,   101,
      -1,    64,   100,   101,    -1,    17,    -1,   136,   111,   128,
     123,    95,   123,    95,   127,    -1,   136,   111,    -1,   136,
     111,   128,   111,    -1,   136,   111,   128,   111,    92,   112,
      93,    -1,    86,   122,    -1,    65,    -1,    33,    -1,   139,
     113,    -1,    66,   113,    -1,   112,    -1,    81,   100,   113,
     101,    68,    -1,    77,    68,    -1,    81,   100,   113,   101,
      75,    -1,    75,    -1,    76,    -1,    83,    67,    68,    -1,
      80,    67,    67,    67,    68,    -1,    16,   120,    68,    -1,
      72,    67,    68,    -1,    72,    67,    67,    68,    -1,    85,
     121,    -1,    85,   113,    -1,    84,   100,   112,   101,    -1,
      84,   100,   101,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   318,   318,   320,   354,   355,   357,   359,   363,   368,
     370,   421,   422,   423,   424,   425,   426,   427,   428,   432,
     435,   436,   437,   438,   439,   440,   441,   442,   443,   446,
     453,   458,   462,   466,   470,   474,   488,   516,   540,   546,
     552,   556,   560,   564,   568,   572,   576,   580,   584,   588,
     592,   596,   600,   604,   608,   612,   616,   620,   624,   628,
     632,   638,   642,   646,   650,   654,   659,   663,   674,   677,
     678,   679,   683,   687,   691,   695,   699,   703,   707,   711,
     715,   732,   739,   738,   756,   764,   772,   781,   785,   789,
     793,   797,   801,   805,   809,   813,   817,   821,   825,   829,
     841,   848,   849,   868,   869,   881,   886,   891,   895,   899,
     936,   960,   981,   989,   993,   994,  1008,  1016,  1025,  1070,
    1071,  1080,  1081,  1087,  1094,  1095,  1096,  1097,  1105,  1110,
    1115,  1122,  1130,  1142,  1159,  1179,  1183,  1187,  1192,  1196,
    1200,  1204,  1208,  1213,  1219,  1225,  1231,  1237,  1243,  1249,
    1261,  1268,  1272,  1309,  1316,  1321,  1334,  1341,  1341,  1344,
    1412,  1416,  1445,  1458,  1475,  1484,  1489,  1497,  1509,  1528,
    1538,  1557,  1580,  1586,  1598,  1604
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "ARROW",
  "GRING_CMD", "BIGINTMAT_CMD", "INTMAT_CMD", "PROC_CMD", "RING_CMD",
  "BEGIN_RING", "BUCKET_CMD", "IDEAL_CMD", "MAP_CMD", "MATRIX_CMD",
  "MODUL_CMD", "NUMBER_CMD", "POLY_CMD", "RESOLUTION_CMD", "SMATRIX_CMD",
  "VECTOR_CMD", "BETTI_CMD", "E_CMD", "FETCH_CMD", "FREEMODULE_CMD",
  "KEEPRING_CMD", "IMAP_CMD", "KOSZUL_CMD", "MAXID_CMD", "MONOM_CMD",
  "PAR_CMD", "PREIMAGE_CMD", "VAR_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT",
  "VNOETHER", "VMINPOLY", "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12",
  "CMD_13", "CMD_23", "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST",
  "RING_DECL", "RING_DECL_LIST", "EXAMPLE_CMD", "EXPORT_CMD", "HELP_CMD",
  "KILL_CMD", "LIB_CMD", "LISTVAR_CMD", "SETRING_CMD", "TYPE_CMD",
  "STRINGTOK", "BLOCKTOK", "INT_CONST", "UNKNOWN_IDENT", "RINGVAR",
  "PROC_DEF", "APPLY", "ASSUME_CMD", "BREAK_CMD", "CONTINUE_CMD",
  "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD", "IF_CMD", "SYS_BREAK",
  "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR", "'='", "'<'", "'+'", "'-'",
  "'/'", "'['", "']'", "'^'", "','", "';'", "'&'", "':'", "UMINUS", "'('",
  "')'", "'.'", "'`'", "$accept", "lines", "pprompt", "flowctrl",
  "example_dummy", "command", "assign", "elemexpr", "exprlist", "expr",
  "$@1", "quote_start", "assume_start", "quote_end", "expr_arithmetic",
  "left_value", "extendedid", "declare_ip_variable", "stringexpr", "rlist",
  "ordername", "orderelem", "OrderingList", "ordering", "cmdeq", "mat_cmd",
  "filecmd", "helpcmd", "examplecmd", "exportcmd", "killcmd", "listcmd",
  "ringcmd1", "ringcmd", "scriptcmd", "setrings", "setringcmd", "typecmd",
  "ifcmd", "whilecmd", "forcmd", "proccmd", "parametercmd", "returncmd", 0
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
     335,   336,   337,   338,   339,   340,   341,    61,    60,    43,
      45,    47,    91,    93,    94,    44,    59,    38,    58,   342,
      40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   104,   105,   105,   106,   106,   106,   106,   106,   106,
     106,   107,   107,   107,   107,   107,   107,   107,   107,   108,
     109,   109,   109,   109,   109,   109,   109,   109,   109,   110,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   111,   111,   111,
     111,   111,   111,   111,   111,   111,   111,   112,   112,   113,
     113,   113,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   114,   113,   115,   116,   117,   118,   118,   118,
     118,   118,   118,   118,   118,   118,   118,   118,   118,   118,
     118,   119,   119,   120,   120,   121,   121,   121,   121,   121,
     121,   121,   121,   122,   123,   123,   124,   125,   125,   126,
     126,   127,   127,   128,   129,   129,   129,   129,   130,   131,
     131,   132,   133,   134,   134,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   136,   137,   137,   137,   137,   138,   139,   139,   140,
     141,   141,   142,   142,   142,   142,   142,   143,   144,   145,
     145,   145,   146,   146,   147,   147
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       4,     4,     4,     3,     4,     4,     3,     4,     4,     4,
       4,     6,     6,     6,     6,     8,     8,     8,     8,     3,
       4,     8,     4,     8,     4,     3,     3,     3,     1,     1,
       1,     6,     4,     6,     6,     6,     6,     6,     6,     3,
       5,     5,     0,     5,     2,     2,     1,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     1,     3,     2,     2,     2,     2,     8,
       2,     3,     2,     1,     1,     5,     1,     1,     4,     1,
       3,     1,     3,     1,     1,     1,     1,     1,     2,     3,
       2,     3,     2,     2,     3,     4,     4,     4,     4,     4,
       4,     4,     4,     6,     6,     6,     6,     6,     6,     6,
       3,     1,     8,     2,     4,     7,     2,     1,     1,     2,
       2,     1,     5,     2,     5,     1,     1,     3,     5,     3,
       3,     4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,   127,   126,     0,   151,   124,
     125,   158,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   157,
       0,   113,    37,   103,    30,     0,     0,     0,   165,   166,
       0,     0,     0,     0,     0,     8,     0,     0,     0,    38,
       0,     0,     0,     9,     0,     0,     3,     4,    13,     0,
      20,    70,   161,    68,     0,     0,    69,     0,    31,     0,
      39,     0,    16,    17,    18,    21,    22,    23,     0,    25,
      26,     0,    27,    28,    11,    12,    14,    15,    24,     7,
      10,     0,     0,     0,     0,     0,     0,    38,    99,     0,
       0,    70,     0,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    70,     0,    70,     0,    70,     0,
      70,     0,    19,   132,     0,   130,    70,     0,   160,     0,
       0,    85,   163,    82,    84,     0,     0,     0,     0,     0,
     173,   172,   156,   128,   100,     0,     0,     0,     5,     0,
       0,   123,     0,   102,     0,     0,    88,     0,    87,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      29,     0,     0,     6,   101,     0,    70,     0,    70,   159,
       0,     0,     0,     0,     0,     0,    68,   169,     0,   114,
       0,     0,     0,     0,     0,     0,     0,     0,    59,     0,
      68,    43,     0,    68,    46,     0,   131,   129,     0,     0,
       0,     0,     0,     0,   150,    70,     0,     0,   170,     0,
       0,     0,     0,   167,   175,     0,    36,    66,   104,    32,
      34,     0,    67,    97,    96,    95,    93,    89,    90,    91,
       0,    92,    94,    98,    33,     0,    86,    79,     0,    65,
      70,     0,     0,    70,     0,     0,     0,     0,     0,     0,
       0,    40,    68,    64,     0,    47,     0,     0,     0,    48,
       0,    49,     0,     0,    50,    60,    41,    42,    44,    45,
     141,   139,   135,   136,   137,   138,     0,   142,   140,   171,
       0,     0,     0,     0,   174,    35,    72,     0,     0,     0,
       0,    62,     0,    70,   114,     0,    42,    45,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    83,
     168,   162,   164,     0,    80,    81,     0,     0,     0,     0,
       0,     0,    51,     0,    52,     0,     0,    53,     0,    54,
     149,   147,   143,   144,   145,   146,   148,    73,    74,    75,
      76,    77,    78,    71,     0,     0,     0,     0,   115,   116,
       0,   117,   121,     0,     0,     0,     0,     0,     0,     0,
     155,     0,   119,     0,     0,    63,    55,    56,    57,    58,
      61,   109,   152,     0,   122,     0,   120,   118
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    56,    57,    58,    59,    60,    61,   146,    63,
     220,    64,    65,   247,    66,    67,    68,    69,    70,   190,
     371,   372,   383,   373,   153,    99,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -364
static const yytype_int16 yypact[] =
{
    -364,   345,  -364,   -88,  1861,  -364,  -364,  1928,   -74,  -364,
    -364,  -364,   -50,   -47,   -35,   -26,   -19,   -14,     5,     9,
    1995,  2062,  2129,  2196,   -28,  1861,   -63,  1861,    13,  -364,
    1861,  -364,  -364,  -364,  -364,   -39,    24,    26,  -364,  -364,
     -27,    44,    46,    34,    48,  -364,    86,    73,  2263,   101,
     101,  1861,  1861,  -364,  1861,  1861,  -364,  -364,  -364,    82,
    -364,    16,   -77,  1328,  1861,  1861,  -364,  1861,   186,   -81,
    -364,  2330,  -364,  -364,  -364,  -364,   106,  -364,  1861,  -364,
    -364,  1861,  -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,
    -364,   107,   -74,   109,   112,   114,   122,  -364,    10,   127,
    1861,   156,  1328,    -9,  2397,  1861,  1861,  1861,  1861,  1861,
    1861,  1861,  1459,  1861,   185,  1526,   553,  1861,   568,  1593,
     583,   134,  -364,   142,   154,  -364,    27,  1660,  1328,     2,
    1861,  -364,  -364,  -364,  -364,   188,  1861,   190,  1727,  1928,
    1328,   174,  -364,  -364,    10,   -84,   -59,    68,  -364,  1861,
    1794,  -364,  1861,  -364,  1861,  1861,  -364,  1861,  -364,  1861,
    1861,  1861,  1861,  1861,  1861,  1861,  1861,  1861,   222,   609,
     142,   203,  1861,  -364,  -364,  1861,   128,  1861,    76,  1328,
    1861,  1861,  1526,  1861,  1593,  1861,   250,  -364,  1861,   632,
     191,   647,   662,   677,   372,   440,   705,   468,  -364,   -58,
     773,  -364,   -41,   801,  -364,   -38,  -364,  -364,   -17,    94,
     103,   116,   118,   140,  -364,    20,   147,   220,  -364,   816,
    1861,   223,   831,  -364,  -364,   -37,  -364,  -364,  -364,  -364,
    -364,   -34,  1328,   144,  1368,  1368,  1433,     6,     6,    10,
     483,    15,  1343,     6,  -364,  1861,  -364,  -364,  1861,  -364,
     595,   498,  1861,    45,  2397,   250,   773,   -33,   801,   -13,
     498,  -364,   846,  -364,  2397,  -364,  1861,  1861,  1861,  -364,
    1861,  -364,  1861,  1861,  -364,  -364,  -364,  -364,  -364,  -364,
    -364,  -364,  -364,  -364,  -364,  -364,   497,  -364,  -364,  -364,
    2464,   869,   226,   -56,  -364,  -364,  -364,  1861,   942,   942,
    1861,  -364,   965,   182,  1328,   201,  -364,  -364,  1861,   204,
     980,   995,  1010,  1038,   513,   536,   202,   205,   207,   216,
     217,   227,   228,   161,   163,   165,   167,   175,  1106,  -364,
    -364,  -364,  -364,  1134,  -364,  -364,  1149,   210,  1861,  2397,
     -10,   -65,  -364,  1861,  -364,  1861,  1861,  -364,  1861,  -364,
    -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,
    -364,  -364,  -364,  -364,  1861,  1861,   -72,   209,  -364,  -364,
     235,   221,  -364,   229,  1164,  1179,  1202,  1275,  1298,  1313,
    -364,   -65,   232,   230,  1861,  -364,  -364,  -364,  -364,  -364,
    -364,  -364,  -364,   235,  -364,    74,  -364,  -364
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -364,  -364,  -364,  -364,  -364,  -364,  -364,    72,    -1,    25,
    -364,  -364,  -364,   -15,  -364,  -364,   325,   285,   251,  -220,
    -364,  -363,   -57,   -46,   -68,     1,  -364,  -364,  -364,  -364,
    -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,  -364,
    -364,  -364,  -364,  -364
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -155
static const yytype_int16 yytable[] =
{
      62,   174,    71,   171,   124,   369,   151,   382,    90,   226,
     151,   152,   331,   156,   172,   173,   158,   156,   152,   332,
     158,   380,   156,   152,   123,   158,   104,   149,   129,    98,
     382,   149,   102,   125,   305,   370,   152,   152,   149,   121,
     122,   132,   227,   275,   309,   102,   102,   102,   102,    71,
     105,   145,   102,   106,   152,   128,   149,   152,   152,   187,
     277,   152,   152,   279,   294,   107,   170,   295,   306,   217,
     218,   154,   155,   140,   108,   156,   144,   157,   158,   101,
     147,   109,   152,   180,   280,   152,   110,   149,   307,   168,
     169,   368,   114,   116,   118,   120,   102,   162,   163,   126,
     164,   135,   163,   102,   164,   111,   179,   163,   167,   112,
     254,   199,   167,   127,   202,   286,   150,   167,   205,   367,
     150,   287,  -133,  -133,   130,   186,   131,   150,   216,   189,
     191,   192,   193,   194,   195,   196,   197,   225,   200,   149,
    -134,  -134,   203,   176,   133,   150,   134,  -155,   136,   231,
     178,   156,   102,   137,   158,   219,   159,   160,   161,   162,
     163,   222,   164,   151,   102,   165,   166,   149,    31,   152,
     167,   228,  -153,   138,   102,   397,   150,   232,   148,   233,
     234,   257,   235,   259,   236,   237,   238,   239,   240,   241,
     242,   243,   102,   149,   104,   281,   149,   102,   171,   215,
     251,   177,   102,   181,   282,   255,   256,   180,   258,   181,
     260,   101,   182,   262,   183,  -110,   182,   283,   183,   284,
     252,   229,   184,  -110,  -110,   154,   155,   185,   150,   156,
     206,   157,   158,   160,   161,   162,   163,   152,   164,   244,
     184,   285,   166,  -112,   250,   291,   167,   185,   288,   253,
     207,  -112,  -112,   154,   155,   221,   150,   156,   223,   157,
     158,   105,   357,   108,   358,   109,   359,   111,   360,   172,
     298,   249,  -105,   299,   338,   112,   361,   302,  -154,   304,
    -105,  -105,   150,   334,   335,   150,   264,   322,   289,   304,
     292,   310,   311,   312,   330,   313,   339,   314,   315,   341,
     142,   143,   365,   350,   381,   369,   351,   340,   352,   245,
     159,   160,   161,   162,   163,   328,   164,   353,   354,   165,
     166,   384,   333,   246,   167,   336,   303,   393,   355,   356,
     385,   394,   103,   141,     0,   392,   396,   366,   159,   160,
     161,   162,   163,     0,   164,     2,     3,   165,   166,     0,
       0,   261,   167,     4,     0,     0,     0,     0,     0,     5,
       6,     7,     8,     0,   304,     0,     0,     9,   374,     0,
     375,   376,    10,   377,     0,   154,   155,     0,    11,   156,
       0,   157,   158,   395,     0,     0,     0,     0,     0,   378,
     379,     0,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,    26,    27,     0,    28,
      29,    30,    31,     0,    32,    33,    34,    35,    36,    37,
      38,    39,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    49,     0,    50,     0,    51,     0,    52,     0,     0,
       0,    53,     0,   154,   155,    54,     0,   156,    55,   157,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,     0,   164,   268,     0,   165,
     166,   154,   155,   269,   167,   156,     0,   157,   158,     0,
       0,     0,     0,     0,     0,     0,   154,   155,     0,     0,
     156,     0,   157,   158,     0,     0,     0,     0,     0,     0,
       0,   154,   155,     0,     0,   156,     0,   157,   158,     0,
       0,     5,     6,   316,   317,     0,   154,   155,     0,     9,
     156,     0,   157,   158,    10,     0,     0,     0,   159,   160,
     161,   162,   163,     0,   164,   270,     0,   165,   166,   154,
     155,   271,   167,   156,     0,   157,   158,     0,     0,     0,
       0,     0,   318,   319,   320,   321,   159,   160,   161,   162,
     163,     0,   164,   273,   149,   165,   166,     0,     0,   274,
     167,   159,   160,   161,   162,   163,   296,   164,   297,   149,
     165,   166,     0,     0,     0,   167,   159,   160,   161,   162,
     163,     0,   164,   300,   149,   165,   166,     0,     0,   301,
     167,   159,   160,   161,   162,   163,   149,   164,   346,     0,
     165,   166,   154,   155,   347,   167,   156,     0,   157,   158,
       0,     0,     0,     0,   159,   160,   161,   162,   163,     0,
     164,   348,     0,   165,   166,   154,   155,   349,   167,   156,
    -106,   157,   158,     0,     0,     0,     0,     0,  -106,  -106,
     154,   155,     0,   150,   156,  -107,   157,   158,     0,     0,
       0,     0,     0,  -107,  -107,   154,   155,     0,   150,   156,
    -108,   157,   158,     0,     0,     0,     0,     0,  -108,  -108,
     154,   155,  -111,   150,   156,     0,   157,   158,     0,     0,
    -111,  -111,     0,     0,     0,   150,     0,   159,   160,   161,
     162,   163,     0,   164,   248,     0,   165,   166,   154,   155,
       0,   167,   156,     0,   157,   158,     0,     0,     0,     0,
     159,   160,   161,   162,   163,     0,   164,     0,     0,   165,
     166,     0,     0,   263,   167,   159,   160,   161,   162,   163,
       0,   164,     0,     0,   165,   166,     0,     0,   265,   167,
     159,   160,   161,   162,   163,     0,   164,   266,     0,   165,
     166,     0,     0,     0,   167,   159,   160,   161,   162,   163,
       0,   164,   267,     0,   165,   166,   154,   155,     0,   167,
     156,     0,   157,   158,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   159,   160,   161,   162,   163,     0,   164,
     272,     0,   165,   166,   154,   155,     0,   167,   156,     0,
     157,   158,     0,     0,     0,     0,     0,     0,     0,   154,
     155,     0,     0,   156,     0,   157,   158,     0,     0,     0,
       0,     0,     0,     0,   154,   155,     0,     0,   156,     0,
     157,   158,     0,     0,     0,     0,     0,     0,     0,   154,
     155,     0,     0,   156,     0,   157,   158,     0,     0,     0,
       0,   159,   160,   161,   162,   163,     0,   164,     0,     0,
     165,   166,   154,   155,   276,   167,   156,     0,   157,   158,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   159,
     160,   161,   162,   163,     0,   164,     0,     0,   165,   166,
       0,     0,   278,   167,   159,   160,   161,   162,   163,     0,
     164,   290,     0,   165,   166,     0,     0,     0,   167,   159,
     160,   161,   162,   163,     0,   164,     0,     0,   165,   166,
       0,     0,   293,   167,   159,   160,   161,   162,   163,     0,
     164,   308,     0,   165,   166,   154,   155,     0,   167,   156,
       0,   157,   158,     0,     0,     0,     0,   159,   160,   161,
     162,   163,     0,   164,     0,     0,   165,   166,   154,   155,
     329,   167,   156,     0,   157,   158,     0,     0,     0,     0,
       0,     0,     0,   154,   155,     0,     0,   156,     0,   157,
     158,     0,     0,     0,     0,     0,     0,     0,   154,   155,
       0,     0,   156,     0,   157,   158,     0,     0,     0,     0,
       0,     0,     0,   154,   155,     0,     0,   156,     0,   157,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     159,   160,   161,   162,   163,     0,   164,     0,     0,   165,
     166,   154,   155,   246,   167,   156,     0,   157,   158,     0,
       0,     0,     0,   159,   160,   161,   162,   163,   337,   164,
       0,     0,   165,   166,     0,     0,     0,   167,   159,   160,
     161,   162,   163,     0,   164,     0,     0,   165,   166,     0,
       0,   342,   167,   159,   160,   161,   162,   163,     0,   164,
     343,     0,   165,   166,     0,     0,     0,   167,   159,   160,
     161,   162,   163,     0,   164,     0,     0,   165,   166,   154,
     155,   344,   167,   156,     0,   157,   158,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   159,   160,   161,   162,
     163,     0,   164,   345,     0,   165,   166,   154,   155,     0,
     167,   156,     0,   157,   158,     0,     0,     0,     0,     0,
       0,     0,   154,   155,     0,     0,   156,     0,   157,   158,
       0,     0,     0,     0,     0,     0,     0,   154,   155,     0,
       0,   156,     0,   157,   158,     0,     0,     0,     0,     0,
       0,     0,   154,   155,     0,     0,   156,     0,   157,   158,
       0,     0,     0,     0,   159,   160,   161,   162,   163,     0,
     164,     0,     0,   165,   166,   154,   155,   362,   167,   156,
       0,   157,   158,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   159,   160,   161,   162,   163,   363,   164,     0,
       0,   165,   166,     0,     0,     0,   167,   159,   160,   161,
     162,   163,     0,   164,   364,     0,   165,   166,     0,     0,
       0,   167,   159,   160,   161,   162,   163,     0,   164,     0,
       0,   165,   166,     0,     0,   386,   167,   159,   160,   161,
     162,   163,     0,   164,     0,     0,   165,   166,   154,   155,
     387,   167,   156,     0,   157,   158,     0,     0,     0,     0,
     159,   160,   161,   162,   163,     0,   164,     0,     0,   165,
     166,   154,   155,   388,   167,   156,     0,   157,   158,     0,
       0,     0,     0,     0,     0,     0,   154,   155,     0,     0,
     156,     0,   157,   158,     0,     0,     0,     0,     0,     0,
       0,   154,   155,     0,     0,   156,     0,   157,   158,     0,
       0,     0,     0,     0,     0,     0,   154,   155,     0,     0,
     156,     0,   157,   158,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   159,   160,   161,   162,   163,     0,   164,
       0,   154,   165,   166,     0,   156,   389,   167,   158,     0,
       0,     0,     0,     0,     0,     0,   159,   160,   161,   162,
     163,     0,   164,     0,     0,   165,   166,     0,     0,   390,
     167,   159,   160,   161,   162,   163,   391,   164,     0,     0,
     165,   166,     0,     0,     0,   167,   159,   160,   161,   162,
     163,     0,   164,     0,     0,   165,   166,     0,     0,     0,
     167,   159,   160,   161,   162,   163,   154,   164,     0,     0,
     156,   166,     0,   158,     0,   167,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   159,   160,   161,   162,
     163,     0,   164,     0,     0,     0,   166,     4,     0,     0,
     167,     0,     0,     5,     6,    91,    92,     0,     0,     0,
       0,     9,     0,     0,     0,     0,    10,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    13,    14,    15,
      16,    17,    18,    19,    93,    94,    95,    96,     0,     0,
       0,     0,   160,   161,   162,   163,    31,   164,    32,    33,
      34,   166,    36,    37,     4,   167,     0,    41,    42,     0,
       5,     6,    91,    92,     0,    97,     0,     0,     9,    51,
       0,    52,     0,    10,     0,     0,     0,     0,     0,    54,
     198,     0,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,    13,    14,    15,    16,    17,    18,
      19,    93,    94,    95,    96,     0,     0,     0,     0,     0,
       0,     0,     0,    31,     0,    32,    33,    34,     0,    36,
      37,     4,     0,     0,    41,    42,     0,     5,     6,    91,
      92,     0,    97,     0,     0,     9,    51,     0,    52,     0,
      10,     0,     0,     0,     0,     0,    54,   201,     0,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     4,     0,
       0,    41,    42,     0,     5,     6,   208,   209,     0,    97,
       0,     0,     9,    51,     0,    52,     0,    10,     0,     0,
       0,     0,     0,    54,   204,     0,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    12,    13,    14,
      15,    16,    17,    18,    19,   210,   211,   212,   213,     0,
       0,     0,     0,     0,     0,     0,     0,    31,     0,    32,
      33,    34,     0,    36,    37,     4,     0,     0,    41,    42,
       0,     5,     6,    91,    92,     0,    97,     0,     0,     9,
      51,     0,    52,     0,    10,     0,     0,     0,     0,     0,
      54,   214,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    93,    94,    95,    96,     0,     0,     0,     0,
       0,     0,     0,     0,    31,     0,    32,    33,    34,     0,
      36,    37,     4,     0,     0,    41,    42,     0,     5,     6,
      91,    92,     0,    97,     0,     0,     9,    51,     0,    52,
       0,    10,     0,     0,     0,     0,     0,    54,   224,     0,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    93,
      94,    95,    96,     0,     0,     0,     0,     0,     0,     0,
       0,    31,     0,    32,    33,    34,     0,    36,    37,     4,
       0,     0,    41,    42,     0,     5,     6,    91,    92,     0,
      97,     0,     0,     9,    51,     0,    52,     0,    10,     0,
       0,     0,     0,     0,    54,   230,     0,    55,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    12,    13,
      14,    15,    16,    17,    18,    19,    93,    94,    95,    96,
       0,     0,     0,     0,     0,     0,     0,     0,    31,     0,
      32,    33,    34,     0,    36,    37,     4,     0,     0,    41,
      42,     0,     5,     6,    91,    92,     0,    97,     0,     0,
       9,    51,     0,    52,     0,    10,     0,     0,     0,     0,
       0,    54,     0,     0,    55,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    12,    13,    14,    15,    16,
      17,    18,    19,    93,    94,    95,    96,     0,     0,     0,
       0,     0,     0,     0,     0,    31,     0,    32,    33,    34,
       0,    36,    37,     4,     0,     0,    41,    42,     0,     5,
       6,    91,    92,     0,    97,     0,     0,     9,    51,     0,
      52,     0,    10,     0,     0,     0,     0,     0,   100,     0,
       0,    55,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    12,    13,    14,    15,    16,    17,    18,    19,
      93,    94,    95,    96,     0,     0,     0,     0,     0,     0,
       0,     0,    31,     0,    32,    33,    34,     0,    36,    37,
       4,     0,     0,    41,    42,     0,     5,     6,    91,    92,
       0,    97,     0,     0,     9,    51,     0,    52,     0,    10,
       0,     0,     0,     0,     0,   113,     0,     0,    55,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,    12,
      13,    14,    15,    16,    17,    18,    19,    93,    94,    95,
      96,     0,     0,     0,     0,     0,     0,     0,     0,    31,
       0,    32,    33,    34,     0,    36,    37,     4,     0,     0,
      41,    42,     0,     5,     6,    91,    92,     0,    97,     0,
       0,     9,    51,     0,    52,     0,    10,     0,     0,     0,
       0,     0,   115,     0,     0,    55,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    12,    13,    14,    15,
      16,    17,    18,    19,    93,    94,    95,    96,     0,     0,
       0,     0,     0,     0,     0,     0,    31,     0,    32,    33,
      34,     0,    36,    37,     4,     0,     0,    41,    42,     0,
       5,     6,    91,    92,     0,    97,     0,     0,     9,    51,
       0,    52,     0,    10,     0,     0,     0,     0,     0,   117,
       0,     0,    55,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    12,    13,    14,    15,    16,    17,    18,
      19,    93,    94,    95,    96,     0,     0,     0,     0,     0,
       0,     0,     0,    31,     0,    32,    33,    34,     0,    36,
      37,     4,     0,     0,    41,    42,     0,     5,     6,   139,
      92,     0,    97,     0,     0,     9,    51,     0,    52,     0,
      10,     0,     0,     0,     0,     0,   119,     0,     0,    55,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     4,     0,
       0,    41,    42,     0,     5,     6,    91,    92,     0,    97,
       0,     0,     9,    51,     0,    52,     0,    10,     0,     0,
       0,     0,     0,    54,     0,     0,    55,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    12,    13,    14,
      15,    16,    17,    18,    19,    93,    94,    95,    96,     0,
       0,     0,     0,     0,     0,     0,     0,    31,     0,    32,
      33,    34,     0,    36,    37,     4,     0,     0,    41,    42,
       0,     5,     6,    91,    92,     0,    97,     0,     0,     9,
      51,     0,    52,     0,    10,     0,     0,     0,     0,     0,
     175,     0,     0,    55,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    12,    13,    14,    15,    16,    17,
      18,    19,    93,    94,    95,    96,     0,     0,     0,     0,
       0,     0,     0,     0,    31,     0,    32,    33,    34,     0,
      36,    37,     4,     0,     0,    41,    42,     0,     5,     6,
      91,    92,     0,    97,     0,     0,     9,    51,     0,    52,
       0,    10,     0,     0,     0,     0,     0,   188,     0,     0,
      55,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   323,    13,    14,   324,   325,    17,   326,   327,    93,
      94,    95,    96,     0,     0,     0,     0,     0,     0,     0,
       0,    31,     0,    32,    33,    34,     0,    36,    37,     0,
       0,     0,    41,    42,     0,     0,     0,     0,     0,     0,
      97,     0,     0,     0,    51,     0,    52,     0,     0,     0,
       0,     0,     0,     0,    54,     0,     0,    55
};

static const yytype_int16 yycheck[] =
{
       1,    69,     1,    12,    67,    70,    87,   370,    96,    93,
      87,    95,    68,     7,    95,    96,    10,     7,    95,    75,
      10,    93,     7,    95,    25,    10,   100,    11,    67,     4,
     393,    11,     7,    96,   254,   100,    95,    95,    11,    67,
      68,    68,   101,   101,   264,    20,    21,    22,    23,    48,
     100,    52,    27,   100,    95,    30,    11,    95,    95,    68,
     101,    95,    95,   101,   101,   100,    67,   101,   101,    67,
      68,     3,     4,    48,   100,     7,    51,     9,    10,     7,
      55,   100,    95,   100,   101,    95,   100,    11,   101,    64,
      65,   101,    20,    21,    22,    23,    71,    91,    92,    27,
      94,    67,    92,    78,    94,   100,    81,    92,   102,   100,
     178,   112,   102,   100,   115,    95,   100,   102,   119,   339,
     100,   101,    95,    96,   100,   100,   100,   100,   127,   104,
     105,   106,   107,   108,   109,   110,   111,   138,   113,    11,
      95,    96,   117,    71,   100,   100,   100,     3,   100,   150,
      78,     7,   127,    67,    10,   130,    88,    89,    90,    91,
      92,   136,    94,    87,   139,    97,    98,    11,    67,    95,
     102,   103,    96,   100,   149,   101,   100,   152,    96,   154,
     155,   182,   157,   184,   159,   160,   161,   162,   163,   164,
     165,   166,   167,    11,   100,   101,    11,   172,    12,   127,
     175,    95,   177,   100,   101,   180,   181,   100,   183,   100,
     185,   139,   100,   188,   100,    87,   100,   101,   100,   101,
      92,   149,   100,    95,    96,     3,     4,   100,   100,     7,
      96,     9,    10,    89,    90,    91,    92,    95,    94,   167,
     100,   101,    98,    87,   172,   220,   102,   100,   101,   177,
      96,    95,    96,     3,     4,    67,   100,     7,    68,     9,
      10,   100,   101,   100,   101,   100,   101,   100,   101,    95,
     245,    68,    87,   248,    92,   100,   101,   252,    96,   254,
      95,    96,   100,   298,   299,   100,    95,   286,    68,   264,
      67,   266,   267,   268,    68,   270,    95,   272,   273,    95,
      49,    50,    92,   101,    95,    70,   101,   308,   101,    87,
      88,    89,    90,    91,    92,   290,    94,   101,   101,    97,
      98,   100,   297,   101,   102,   300,   254,    95,   101,   101,
     101,   101,     7,    48,    -1,   381,   393,   338,    88,    89,
      90,    91,    92,    -1,    94,     0,     1,    97,    98,    -1,
      -1,   101,   102,     8,    -1,    -1,    -1,    -1,    -1,    14,
      15,    16,    17,    -1,   339,    -1,    -1,    22,   343,    -1,
     345,   346,    27,   348,    -1,     3,     4,    -1,    33,     7,
      -1,     9,    10,   384,    -1,    -1,    -1,    -1,    -1,   364,
     365,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    -1,    64,
      65,    66,    67,    -1,    69,    70,    71,    72,    73,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    -1,    88,    -1,    90,    -1,    92,    -1,    -1,
      -1,    96,    -1,     3,     4,   100,    -1,     7,   103,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    95,    -1,    97,
      98,     3,     4,   101,   102,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    14,    15,    16,    17,    -1,     3,     4,    -1,    22,
       7,    -1,     9,    10,    27,    -1,    -1,    -1,    88,    89,
      90,    91,    92,    -1,    94,    95,    -1,    97,    98,     3,
       4,   101,   102,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    55,    56,    57,    58,    88,    89,    90,    91,
      92,    -1,    94,    95,    11,    97,    98,    -1,    -1,   101,
     102,    88,    89,    90,    91,    92,    93,    94,    95,    11,
      97,    98,    -1,    -1,    -1,   102,    88,    89,    90,    91,
      92,    -1,    94,    95,    11,    97,    98,    -1,    -1,   101,
     102,    88,    89,    90,    91,    92,    11,    94,    95,    -1,
      97,    98,     3,     4,   101,   102,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,    -1,
      94,    95,    -1,    97,    98,     3,     4,   101,   102,     7,
      87,     9,    10,    -1,    -1,    -1,    -1,    -1,    95,    96,
       3,     4,    -1,   100,     7,    87,     9,    10,    -1,    -1,
      -1,    -1,    -1,    95,    96,     3,     4,    -1,   100,     7,
      87,     9,    10,    -1,    -1,    -1,    -1,    -1,    95,    96,
       3,     4,    87,   100,     7,    -1,     9,    10,    -1,    -1,
      95,    96,    -1,    -1,    -1,   100,    -1,    88,    89,    90,
      91,    92,    -1,    94,    95,    -1,    97,    98,     3,     4,
      -1,   102,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    -1,    -1,    97,
      98,    -1,    -1,   101,   102,    88,    89,    90,    91,    92,
      -1,    94,    -1,    -1,    97,    98,    -1,    -1,   101,   102,
      88,    89,    90,    91,    92,    -1,    94,    95,    -1,    97,
      98,    -1,    -1,    -1,   102,    88,    89,    90,    91,    92,
      -1,    94,    95,    -1,    97,    98,     3,     4,    -1,   102,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    89,    90,    91,    92,    -1,    94,
      95,    -1,    97,    98,     3,     4,    -1,   102,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    88,    89,    90,    91,    92,    -1,    94,    -1,    -1,
      97,    98,     3,     4,   101,   102,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    88,
      89,    90,    91,    92,    -1,    94,    -1,    -1,    97,    98,
      -1,    -1,   101,   102,    88,    89,    90,    91,    92,    -1,
      94,    95,    -1,    97,    98,    -1,    -1,    -1,   102,    88,
      89,    90,    91,    92,    -1,    94,    -1,    -1,    97,    98,
      -1,    -1,   101,   102,    88,    89,    90,    91,    92,    -1,
      94,    95,    -1,    97,    98,     3,     4,    -1,   102,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    88,    89,    90,
      91,    92,    -1,    94,    -1,    -1,    97,    98,     3,     4,
     101,   102,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    -1,    -1,    97,
      98,     3,     4,   101,   102,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    88,    89,    90,    91,    92,    93,    94,
      -1,    -1,    97,    98,    -1,    -1,    -1,   102,    88,    89,
      90,    91,    92,    -1,    94,    -1,    -1,    97,    98,    -1,
      -1,   101,   102,    88,    89,    90,    91,    92,    -1,    94,
      95,    -1,    97,    98,    -1,    -1,    -1,   102,    88,    89,
      90,    91,    92,    -1,    94,    -1,    -1,    97,    98,     3,
       4,   101,   102,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    -1,    94,    95,    -1,    97,    98,     3,     4,    -1,
     102,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    88,    89,    90,    91,    92,    -1,
      94,    -1,    -1,    97,    98,     3,     4,   101,   102,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    88,    89,    90,    91,    92,    93,    94,    -1,
      -1,    97,    98,    -1,    -1,    -1,   102,    88,    89,    90,
      91,    92,    -1,    94,    95,    -1,    97,    98,    -1,    -1,
      -1,   102,    88,    89,    90,    91,    92,    -1,    94,    -1,
      -1,    97,    98,    -1,    -1,   101,   102,    88,    89,    90,
      91,    92,    -1,    94,    -1,    -1,    97,    98,     3,     4,
     101,   102,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      88,    89,    90,    91,    92,    -1,    94,    -1,    -1,    97,
      98,     3,     4,   101,   102,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    88,    89,    90,    91,    92,    -1,    94,
      -1,     3,    97,    98,    -1,     7,   101,   102,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    -1,    94,    -1,    -1,    97,    98,    -1,    -1,   101,
     102,    88,    89,    90,    91,    92,    93,    94,    -1,    -1,
      97,    98,    -1,    -1,    -1,   102,    88,    89,    90,    91,
      92,    -1,    94,    -1,    -1,    97,    98,    -1,    -1,    -1,
     102,    88,    89,    90,    91,    92,     3,    94,    -1,    -1,
       7,    98,    -1,    10,    -1,   102,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    88,    89,    90,    91,
      92,    -1,    94,    -1,    -1,    -1,    98,     8,    -1,    -1,
     102,    -1,    -1,    14,    15,    16,    17,    -1,    -1,    -1,
      -1,    22,    -1,    -1,    -1,    -1,    27,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    67,    94,    69,    70,
      71,    98,    73,    74,     8,   102,    -1,    78,    79,    -1,
      14,    15,    16,    17,    -1,    86,    -1,    -1,    22,    90,
      -1,    92,    -1,    27,    -1,    -1,    -1,    -1,    -1,   100,
     101,    -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    69,    70,    71,    -1,    73,
      74,     8,    -1,    -1,    78,    79,    -1,    14,    15,    16,
      17,    -1,    86,    -1,    -1,    22,    90,    -1,    92,    -1,
      27,    -1,    -1,    -1,    -1,    -1,   100,   101,    -1,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,    69,    70,    71,    -1,    73,    74,     8,    -1,
      -1,    78,    79,    -1,    14,    15,    16,    17,    -1,    86,
      -1,    -1,    22,    90,    -1,    92,    -1,    27,    -1,    -1,
      -1,    -1,    -1,   100,   101,    -1,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    69,
      70,    71,    -1,    73,    74,     8,    -1,    -1,    78,    79,
      -1,    14,    15,    16,    17,    -1,    86,    -1,    -1,    22,
      90,    -1,    92,    -1,    27,    -1,    -1,    -1,    -1,    -1,
     100,   101,    -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    -1,    69,    70,    71,    -1,
      73,    74,     8,    -1,    -1,    78,    79,    -1,    14,    15,
      16,    17,    -1,    86,    -1,    -1,    22,    90,    -1,    92,
      -1,    27,    -1,    -1,    -1,    -1,    -1,   100,   101,    -1,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    -1,    69,    70,    71,    -1,    73,    74,     8,
      -1,    -1,    78,    79,    -1,    14,    15,    16,    17,    -1,
      86,    -1,    -1,    22,    90,    -1,    92,    -1,    27,    -1,
      -1,    -1,    -1,    -1,   100,   101,    -1,   103,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,
      49,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,
      69,    70,    71,    -1,    73,    74,     8,    -1,    -1,    78,
      79,    -1,    14,    15,    16,    17,    -1,    86,    -1,    -1,
      22,    90,    -1,    92,    -1,    27,    -1,    -1,    -1,    -1,
      -1,   100,    -1,    -1,   103,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    67,    -1,    69,    70,    71,
      -1,    73,    74,     8,    -1,    -1,    78,    79,    -1,    14,
      15,    16,    17,    -1,    86,    -1,    -1,    22,    90,    -1,
      92,    -1,    27,    -1,    -1,    -1,    -1,    -1,   100,    -1,
      -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    67,    -1,    69,    70,    71,    -1,    73,    74,
       8,    -1,    -1,    78,    79,    -1,    14,    15,    16,    17,
      -1,    86,    -1,    -1,    22,    90,    -1,    92,    -1,    27,
      -1,    -1,    -1,    -1,    -1,   100,    -1,    -1,   103,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,
      -1,    69,    70,    71,    -1,    73,    74,     8,    -1,    -1,
      78,    79,    -1,    14,    15,    16,    17,    -1,    86,    -1,
      -1,    22,    90,    -1,    92,    -1,    27,    -1,    -1,    -1,
      -1,    -1,   100,    -1,    -1,   103,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,    50,
      51,    52,    53,    54,    55,    56,    57,    58,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    69,    70,
      71,    -1,    73,    74,     8,    -1,    -1,    78,    79,    -1,
      14,    15,    16,    17,    -1,    86,    -1,    -1,    22,    90,
      -1,    92,    -1,    27,    -1,    -1,    -1,    -1,    -1,   100,
      -1,    -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    67,    -1,    69,    70,    71,    -1,    73,
      74,     8,    -1,    -1,    78,    79,    -1,    14,    15,    16,
      17,    -1,    86,    -1,    -1,    22,    90,    -1,    92,    -1,
      27,    -1,    -1,    -1,    -1,    -1,   100,    -1,    -1,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      47,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      67,    -1,    69,    70,    71,    -1,    73,    74,     8,    -1,
      -1,    78,    79,    -1,    14,    15,    16,    17,    -1,    86,
      -1,    -1,    22,    90,    -1,    92,    -1,    27,    -1,    -1,
      -1,    -1,    -1,   100,    -1,    -1,   103,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    67,    -1,    69,
      70,    71,    -1,    73,    74,     8,    -1,    -1,    78,    79,
      -1,    14,    15,    16,    17,    -1,    86,    -1,    -1,    22,
      90,    -1,    92,    -1,    27,    -1,    -1,    -1,    -1,    -1,
     100,    -1,    -1,   103,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    47,    48,    49,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    67,    -1,    69,    70,    71,    -1,
      73,    74,     8,    -1,    -1,    78,    79,    -1,    14,    15,
      16,    17,    -1,    86,    -1,    -1,    22,    90,    -1,    92,
      -1,    27,    -1,    -1,    -1,    -1,    -1,   100,    -1,    -1,
     103,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    67,    -1,    69,    70,    71,    -1,    73,    74,    -1,
      -1,    -1,    78,    79,    -1,    -1,    -1,    -1,    -1,    -1,
      86,    -1,    -1,    -1,    90,    -1,    92,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   100,    -1,    -1,   103
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   105,     0,     1,     8,    14,    15,    16,    17,    22,
      27,    33,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    64,    65,
      66,    67,    69,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      88,    90,    92,    96,   100,   103,   106,   107,   108,   109,
     110,   111,   112,   113,   115,   116,   118,   119,   120,   121,
     122,   129,   130,   131,   132,   133,   134,   135,   136,   137,
     138,   139,   140,   141,   142,   143,   144,   145,   146,   147,
      96,    16,    17,    55,    56,    57,    58,    86,   113,   129,
     100,   111,   113,   120,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   111,   100,   111,   100,   111,   100,
     111,    67,    68,   112,    67,    96,   111,   100,   113,    67,
     100,   100,    68,   100,   100,    67,   100,    67,   100,    16,
     113,   121,   122,   122,   113,   112,   112,   113,    96,    11,
     100,    87,    95,   128,     3,     4,     7,     9,    10,    88,
      89,    90,    91,    92,    94,    97,    98,   102,   113,   113,
     112,    12,    95,    96,   128,   100,   111,    95,   111,   113,
     100,   100,   100,   100,   100,   100,   113,    68,   100,   113,
     123,   113,   113,   113,   113,   113,   113,   113,   101,   112,
     113,   101,   112,   113,   101,   112,    96,    96,    16,    17,
      55,    56,    57,    58,   101,   111,   129,    67,    68,   113,
     114,    67,   113,    68,   101,   112,    93,   101,   103,   111,
     101,   112,   113,   113,   113,   113,   113,   113,   113,   113,
     113,   113,   113,   113,   111,    87,   101,   117,    95,    68,
     111,   113,    92,   111,   128,   113,   113,   112,   113,   112,
     113,   101,   113,   101,    95,   101,    95,    95,    95,   101,
      95,   101,    95,    95,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   101,   101,    95,   101,   101,    68,
      95,   113,    67,   101,   101,   101,    93,    95,   113,   113,
      95,   101,   113,   111,   113,   123,   101,   101,    95,   123,
     113,   113,   113,   113,   113,   113,    16,    17,    55,    56,
      57,    58,   129,    47,    50,    51,    53,    54,   113,   101,
      68,    68,    75,   113,   117,   117,   113,    93,    92,    95,
     112,    95,   101,    95,   101,    95,    95,   101,    95,   101,
     101,   101,   101,   101,   101,   101,   101,   101,   101,   101,
     101,   101,   101,    93,    95,    92,   112,   123,   101,    70,
     100,   124,   125,   127,   113,   113,   113,   113,   113,   113,
      93,    95,   125,   126,   100,   101,   101,   101,   101,   101,
     101,    93,   127,    95,   101,   112,   126,   101
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
   Once GCC version 2 has supplanted version 1, this can go.  However,
   YYFAIL appears to be in use.  Nevertheless, it is formally deprecated
   in Bison 2.4.2's NEWS entry, where a plan to phase it out is
   discussed.  */

#define YYFAIL		goto yyerrlab
#if defined YYFAIL
  /* This is here to suppress warnings from the GCC cpp's
     -Wunused-macros.  Normally we don't worry about that warning, but
     some users do, and we want to make it easy for users to remove
     YYFAIL uses, which will produce warnings from Bison 2.5.  */
#endif

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
# if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
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
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
#else
static void
yy_stack_print (yybottom, yytop)
    yytype_int16 *yybottom;
    yytype_int16 *yytop;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
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
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       		       );
      YYFPRINTF (stderr, "\n");
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





/*-------------------------.
| yyparse or yypush_parse.  |
`-------------------------*/

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
/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       `yyss': related to states.
       `yyvs': related to semantic values.

       Refer to the stacks thru separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yytoken = 0;
  yyss = yyssa;
  yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */

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
	YYSTACK_RELOCATE (yyss_alloc, yyss);
	YYSTACK_RELOCATE (yyvs_alloc, yyvs);
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

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
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

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
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

/* Line 1464 of yacc.c  */
#line 321 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 356 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 358 "grammar.y"
    { (yyvsp[(1) - (2)].lv).CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 360 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 364 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 369 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 371 "grammar.y"
    {
            #ifdef SIQ
            siq=0;
            #endif
            yyInRingConstruction = FALSE;
            currentVoice->ifsw=0;
            if (inerror)
            {
/*  bison failed here*/
              if ((inerror!=3) && ((yyvsp[(1) - (2)].i)<UMINUS) && ((yyvsp[(1) - (2)].i)>' '))
              {
                // 1: yyerror called
                // 2: scanner put actual string
                // 3: error rule put token+\n
                inerror=3;
                Print(" error at token `%s`\n",iiTwoOps((yyvsp[(1) - (2)].i)));
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

/* Line 1464 of yacc.c  */
#line 429 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 432 "grammar.y"
    { omFree((ADDRESS)(yyvsp[(2) - (2)].name)); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 447 "grammar.y"
    {
            if(iiAssign(&(yyvsp[(1) - (2)].lv),&(yyvsp[(2) - (2)].lv))) YYERROR;
          ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 454 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&(yyval.lv),omStrDup((yyvsp[(1) - (1)].name)));
          ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 459 "grammar.y"
    {
            syMake(&(yyval.lv),(yyvsp[(1) - (1)].name));
          ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 463 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), COLONCOLON, &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 467 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), '.', &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 471 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'(')) YYERROR;
          ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 475 "grammar.y"
    {
            if ((yyvsp[(1) - (4)].lv).next==NULL)
            {
              (yyvsp[(1) - (4)].lv).next=(leftv)omAllocBin(sleftv_bin);
              memcpy((yyvsp[(1) - (4)].lv).next,&(yyvsp[(3) - (4)].lv),sizeof(sleftv));
              if(iiExprArithM(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'(')) YYERROR;
            }
            else if ((yyvsp[(1) - (4)].lv).rtyp==UNKNOWN)
            { // for x(i)(j)
              if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'(',&(yyvsp[(3) - (4)].lv))) YYERROR;
            }
            else YYERROR;
          ;}
    break;

  case 36:

/* Line 1464 of yacc.c  */
#line 489 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 517 "grammar.y"
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
                n_Read((yyvsp[(1) - (1)].name),&n,coeffs_BIGINT);
                (yyval.lv).rtyp=BIGINT_CMD;
                (yyval.lv).data = n;
              }
            }
          ;}
    break;

  case 38:

/* Line 1464 of yacc.c  */
#line 541 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[(1) - (1)].i);
            (yyval.lv).data = (yyval.lv).Data();
          ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 547 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[(1) - (1)].name);
          ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 553 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 557 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 561 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 565 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 569 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 573 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 577 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 581 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 585 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 589 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 593 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 597 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 601 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 605 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 609 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 613 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 617 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 621 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 625 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 629 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 633 "grammar.y"
    {
            int b=iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i)); // handle branchTo
            if (b==TRUE) YYERROR;
            if (b==2) YYACCEPT;
          ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 639 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 643 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 647 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 651 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),RING_CMD)) YYERROR;
          ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 655 "grammar.y"
    {
            if (iiARROW(&(yyval.lv),(yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].name))) YYERROR;
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 659 "grammar.y"
    { (yyval.lv) = (yyvsp[(2) - (3)].lv); ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 664 "grammar.y"
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

  case 71:

/* Line 1464 of yacc.c  */
#line 680 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[(1) - (6)].lv),&(yyvsp[(3) - (6)].lv),&(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 684 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'[',&(yyvsp[(3) - (4)].lv))) YYERROR;
          ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 688 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 692 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 696 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 700 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 704 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 708 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), 0, &(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 712 "grammar.y"
    {
            (yyval.lv)=(yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 716 "grammar.y"
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

  case 81:

/* Line 1464 of yacc.c  */
#line 733 "grammar.y"
    {
            iiTestAssume(&(yyvsp[(2) - (5)].lv),&(yyvsp[(4) - (5)].lv));
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp=NONE;
          ;}
    break;

  case 82:

/* Line 1464 of yacc.c  */
#line 739 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 745 "grammar.y"
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

  case 84:

/* Line 1464 of yacc.c  */
#line 757 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 765 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 773 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 782 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),PLUSPLUS)) YYERROR;
          ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 786 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),MINUSMINUS)) YYERROR;
          ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 790 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'+',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 794 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'-',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 798 "grammar.y"
    { /* also for *,% */
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 802 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'^',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 806 "grammar.y"
    { /* also for > */
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 810 "grammar.y"
    { /* also for |*/
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 814 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),NOTEQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 818 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),EQUAL_EQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 822 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),DOTDOT,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 826 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),':',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 830 "grammar.y"
    {
            if (siq>0)
            { if (iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),NOT)) YYERROR; }
            else
            {
              memset(&(yyval.lv),0,sizeof((yyval.lv)));
              int i; TESTSETINT((yyvsp[(2) - (2)].lv),i);
              (yyval.lv).rtyp  = INT_CMD;
              (yyval.lv).data = (void *)(long)(i == 0 ? 1 : 0);
            }
          ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 842 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),'-')) YYERROR;
          ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 850 "grammar.y"
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

  case 104:

/* Line 1464 of yacc.c  */
#line 870 "grammar.y"
    {
            if ((yyvsp[(2) - (3)].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[(2) - (3)].lv).CopyD(STRING_CMD);
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 882 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 887 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 892 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 896 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 900 "grammar.y"
    {
            int r; TESTSETINT((yyvsp[(4) - (8)].lv),r);
            int c; TESTSETINT((yyvsp[(7) - (8)].lv),c);
            leftv v;
            idhdl h;
            if (((yyvsp[(1) - (8)].i) == MATRIX_CMD) || ((yyvsp[(1) - (8)].i) == SMATRIX_CMD ))
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&(currRing->idroot), TRUE)) YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              idDelete(&IDIDEAL(h));
              if ((yyvsp[(1) - (8)].i) == MATRIX_CMD)
                IDMATRIX(h) = mpNew(r,c);
              else
                IDIDEAL(h) = idInit(c,r);
              if (IDMATRIX(h)==NULL) YYERROR;
            }
            else if (((yyvsp[(1) - (8)].i) == INTMAT_CMD)||((yyvsp[(1) - (8)].i) == BIGINTMAT_CMD))
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&((yyvsp[(2) - (8)].lv).req_packhdl->idroot)))
                YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              if ((yyvsp[(1) - (8)].i)==INTMAT_CMD)
              {
                delete IDINTVEC(h);
                IDINTVEC(h) = new intvec(r,c,0);
              }
              else
              {
                delete IDBIMAT(h);
                IDBIMAT(h) = new bigintmat(r, c, coeffs_BIGINT);
              }
              if (IDINTVEC(h)==NULL) YYERROR;
            }
          ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 937 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i) == MATRIX_CMD)||((yyvsp[(1) - (2)].i) == SMATRIX_CMD))
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
            }
            else if (((yyvsp[(1) - (2)].i) == INTMAT_CMD)||((yyvsp[(1) - (2)].i) == BIGINTMAT_CMD))
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
                YYERROR;
              if ((yyvsp[(1) - (2)].i) == INTMAT_CMD)
              {
                leftv v=&(yyval.lv);
                idhdl h;
                do
                {
                  h=(idhdl)v->data;
                  delete IDINTVEC(h);
                  IDINTVEC(h) = new intvec(1,1,0);
                  v=v->next;
                } while (v!=NULL);
              }
            }
          ;}
    break;

  case 111:

/* Line 1464 of yacc.c  */
#line 961 "grammar.y"
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

  case 112:

/* Line 1464 of yacc.c  */
#line 982 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 115:

/* Line 1464 of yacc.c  */
#line 995 "grammar.y"
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

  case 116:

/* Line 1464 of yacc.c  */
#line 1009 "grammar.y"
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[(1) - (1)].name));
        ;}
    break;

  case 117:

/* Line 1464 of yacc.c  */
#line 1017 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = (yyvsp[(1) - (1)].i);
            (yyval.lv).rtyp = INTVEC_CMD;
            (yyval.lv).data = (void *)iv;
          ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1026 "grammar.y"
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

  case 120:

/* Line 1464 of yacc.c  */
#line 1072 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[(3) - (3)].lv),sizeof(sleftv));
          ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1082 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1088 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1106 "grammar.y"
    { newFile((yyvsp[(2) - (2)].name)); omFree((yyvsp[(2) - (2)].name)); ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 1111 "grammar.y"
    {
            feHelp((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1116 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 1123 "grammar.y"
    {
            singular_example((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 1131 "grammar.y"
    {
          if (basePack!=(yyvsp[(2) - (2)].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[(2) - (2)].lv),0,currPack)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[(2) - (2)].lv),0)) YYERROR;
        ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1143 "grammar.y"
    {
          leftv v=&(yyvsp[(2) - (2)].lv);
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
               omFree((ADDRESS)v->name); v->name=NULL;
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 1160 "grammar.y"
    {
          leftv v=&(yyvsp[(3) - (3)].lv);
          if (v->rtyp!=IDHDL)
          {
            if (v->name!=NULL)
            {
               Werror("`%s` is undefined in kill",v->name);
               omFree((ADDRESS)v->name); v->name=NULL;
            }
            else               WerrorS("kill what ?");
          }
          else
          {
            killhdl((idhdl)v->data,v->req_packhdl);
          }
        ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 1180 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 1184 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 1188 "grammar.y"
    {
            if ((yyvsp[(3) - (4)].i)==QRING_CMD) (yyvsp[(3) - (4)].i)=RING_CMD;
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 1193 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 1197 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 1201 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
           ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 1205 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 1209 "grammar.y"
    {
            list_cmd(0,(yyvsp[(3) - (4)].lv).Fullname(),"// ",TRUE);
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 1214 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1220 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 1226 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1232 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 1238 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1244 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1250 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 1262 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1268 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 1277 "grammar.y"
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
                                   &((yyvsp[(2) - (8)].lv).req_packhdl->idroot),FALSE);
              (yyvsp[(2) - (8)].lv).CleanUp();
              if (newRingHdl!=NULL)
              {
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

  case 153:

/* Line 1464 of yacc.c  */
#line 1310 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (2)].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[(2) - (2)].lv).CleanUp();
          ;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 1317 "grammar.y"
    {
            yyInRingConstruction = FALSE;
            if (iiAssignCR(&(yyvsp[(2) - (4)].lv),&(yyvsp[(4) - (4)].lv))) YYERROR;
          ;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 1322 "grammar.y"
    {
          yyInRingConstruction = FALSE;
          sleftv tmp;
          (yyvsp[(4) - (7)].lv).next=(leftv)omAlloc(sizeof(sleftv));
          memcpy((yyvsp[(4) - (7)].lv).next,&(yyvsp[(6) - (7)].lv),sizeof(sleftv));
          memset(&(yyvsp[(6) - (7)].lv),0,sizeof(sleftv));
          if (iiExprArithM(&tmp,&(yyvsp[(4) - (7)].lv),'[')) YYERROR;
          if (iiAssignCR(&(yyvsp[(2) - (7)].lv),&tmp)) YYERROR;
        ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1335 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)!=LIB_CMD)||(jjLOAD((yyvsp[(2) - (2)].name),TRUE))) YYERROR;
            omFree((yyvsp[(2) - (2)].name));
          ;}
    break;

  case 159:

/* Line 1464 of yacc.c  */
#line 1345 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)==KEEPRING_CMD) && (myynest==0))
               MYYERROR("only inside a proc allowed");
            const char * n=(yyvsp[(2) - (2)].lv).Name();
            if (((yyvsp[(2) - (2)].lv).Typ()==RING_CMD)
            && ((yyvsp[(2) - (2)].lv).rtyp==IDHDL))
            {
              idhdl h=(idhdl)(yyvsp[(2) - (2)].lv).data;
              if ((yyvsp[(2) - (2)].lv).e!=NULL) h=rFindHdl((ring)(yyvsp[(2) - (2)].lv).Data(),NULL);
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
                  iiLocalRing[myynest-1]=IDRING(h);
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

  case 160:

/* Line 1464 of yacc.c  */
#line 1413 "grammar.y"
    {
            type_cmd(&((yyvsp[(2) - (2)].lv)));
          ;}
    break;

  case 161:

/* Line 1464 of yacc.c  */
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

  case 162:

/* Line 1464 of yacc.c  */
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

  case 163:

/* Line 1464 of yacc.c  */
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

  case 164:

/* Line 1464 of yacc.c  */
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

  case 165:

/* Line 1464 of yacc.c  */
#line 1485 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 1490 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
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

  case 168:

/* Line 1464 of yacc.c  */
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

  case 169:

/* Line 1464 of yacc.c  */
#line 1529 "grammar.y"
    {
            idhdl h = enterid((yyvsp[(2) - (3)].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL) {omFree((ADDRESS)(yyvsp[(2) - (3)].name));omFree((ADDRESS)(yyvsp[(3) - (3)].name)); YYERROR;}
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(2) - (3)].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(3) - (3)].name))+31);;
            sprintf(IDPROC(h)->data.s.body,"parameter list #;\n%s;return();\n\n",(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1539 "grammar.y"
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
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(1) - (3)].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(3) - (3)].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(1) - (3)].name));
          ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 1558 "grammar.y"
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
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(1) - (4)].name), 0, 0);
            omFree((ADDRESS)(yyvsp[(1) - (4)].name));
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[(4) - (4)].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[(4) - (4)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(4) - (4)].name));
          ;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 1581 "grammar.y"
    {
            // decl. of type proc p(int i)
            if ((yyvsp[(1) - (2)].i)==PARAMETER)  { if (iiParameter(&(yyvsp[(2) - (2)].lv))) YYERROR; }
            else                { if (iiAlias(&(yyvsp[(2) - (2)].lv))) YYERROR; }
          ;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 1587 "grammar.y"
    {
            // decl. of type proc p(i)
            sleftv tmp_expr;
            if ((yyvsp[(1) - (2)].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[(2) - (2)].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 1599 "grammar.y"
    {
            iiRETURNEXPR.Copy(&(yyvsp[(3) - (4)].lv));
            (yyvsp[(3) - (4)].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 1605 "grammar.y"
    {
            if ((yyvsp[(1) - (3)].i)==RETURN)
            {
              iiRETURNEXPR.Init();
              iiRETURNEXPR.rtyp=NONE;
              if (exitBuffer(BT_proc)) YYERROR;
            }
          ;}
    break;



/* Line 1464 of yacc.c  */
#line 4211 "grammar.cc"
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
      /* If just tried and failed to reuse lookahead token after an
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

  /* Else will try to reuse lookahead token after shifting the error
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

#if !defined(yyoverflow) || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
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



