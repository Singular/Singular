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
    Werror("leaving %s (%d)",VoiceName(), VoiceLine());
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
     BIGINTVEC_CMD = 270,
     INTMAT_CMD = 271,
     PROC_CMD = 272,
     RING_CMD = 273,
     BEGIN_RING = 274,
     BUCKET_CMD = 275,
     IDEAL_CMD = 276,
     MAP_CMD = 277,
     MATRIX_CMD = 278,
     MODUL_CMD = 279,
     NUMBER_CMD = 280,
     POLY_CMD = 281,
     RESOLUTION_CMD = 282,
     SMATRIX_CMD = 283,
     VECTOR_CMD = 284,
     BETTI_CMD = 285,
     E_CMD = 286,
     FETCH_CMD = 287,
     FREEMODULE_CMD = 288,
     KEEPRING_CMD = 289,
     IMAP_CMD = 290,
     KOSZUL_CMD = 291,
     MAXID_CMD = 292,
     MONOM_CMD = 293,
     PAR_CMD = 294,
     PREIMAGE_CMD = 295,
     VAR_CMD = 296,
     VALTVARS = 297,
     VMAXDEG = 298,
     VMAXMULT = 299,
     VNOETHER = 300,
     VMINPOLY = 301,
     END_RING = 302,
     CMD_1 = 303,
     CMD_2 = 304,
     CMD_3 = 305,
     CMD_12 = 306,
     CMD_13 = 307,
     CMD_23 = 308,
     CMD_123 = 309,
     CMD_M = 310,
     ROOT_DECL = 311,
     ROOT_DECL_LIST = 312,
     RING_DECL = 313,
     RING_DECL_LIST = 314,
     EXAMPLE_CMD = 315,
     EXPORT_CMD = 316,
     HELP_CMD = 317,
     KILL_CMD = 318,
     LIB_CMD = 319,
     LISTVAR_CMD = 320,
     SETRING_CMD = 321,
     TYPE_CMD = 322,
     STRINGTOK = 323,
     BLOCKTOK = 324,
     INT_CONST = 325,
     UNKNOWN_IDENT = 326,
     MONOM = 327,
     PROC_DEF = 328,
     APPLY = 329,
     ASSUME_CMD = 330,
     BREAK_CMD = 331,
     CONTINUE_CMD = 332,
     ELSE_CMD = 333,
     EVAL = 334,
     QUOTE = 335,
     FOR_CMD = 336,
     IF_CMD = 337,
     SYS_BREAK = 338,
     WHILE_CMD = 339,
     RETURN = 340,
     PARAMETER = 341,
     SYSVAR = 342,
     UMINUS = 343
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 370 "grammar.cc"

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
#define YYLAST   2786

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  105
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  176
/* YYNRULES -- Number of states.  */
#define YYNSTATES  401

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   343

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    98,     2,
     101,   102,     2,    90,    96,    91,   103,    92,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    99,    97,
      89,    88,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    93,     2,    94,    95,     2,   104,     2,     2,     2,
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
      85,    86,    87,   100
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
     216,   221,   230,   235,   244,   249,   253,   257,   261,   265,
     267,   269,   271,   278,   283,   290,   297,   304,   311,   318,
     325,   329,   335,   341,   342,   348,   351,   354,   356,   359,
     362,   366,   370,   374,   378,   382,   386,   390,   394,   398,
     402,   405,   408,   411,   414,   416,   420,   423,   426,   429,
     432,   441,   444,   448,   451,   453,   455,   461,   463,   465,
     470,   472,   476,   478,   482,   484,   486,   488,   490,   492,
     495,   499,   502,   506,   509,   512,   516,   521,   526,   531,
     536,   541,   546,   551,   556,   563,   570,   577,   584,   591,
     598,   605,   609,   611,   620,   623,   628,   636,   639,   641,
     643,   646,   649,   651,   657,   660,   666,   668,   670,   674,
     680,   684,   688,   693,   696,   699,   704
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     106,     0,    -1,    -1,   106,   107,    -1,   108,    -1,   110,
      97,    -1,   122,    97,    -1,   148,    -1,    83,    -1,    97,
      -1,     1,    97,    -1,   143,    -1,   144,    -1,   109,    -1,
     145,    -1,   146,    -1,   131,    -1,   132,    -1,   133,    -1,
      60,    69,    -1,   111,    -1,   134,    -1,   135,    -1,   136,
      -1,   147,    -1,   138,    -1,   139,    -1,   141,    -1,   142,
      -1,   120,   113,    -1,    72,    -1,   121,    -1,   112,    11,
     112,    -1,   114,   103,   112,    -1,   112,   101,   102,    -1,
     112,   101,   113,   102,    -1,    93,   113,    94,    -1,    70,
      -1,    87,    -1,   123,    -1,    17,   101,   114,   102,    -1,
      56,   101,   114,   102,    -1,    57,   101,   113,   102,    -1,
      57,   101,   102,    -1,    58,   101,   114,   102,    -1,    59,
     101,   113,   102,    -1,    59,   101,   102,    -1,    48,   101,
     114,   102,    -1,    51,   101,   114,   102,    -1,    52,   101,
     114,   102,    -1,    54,   101,   114,   102,    -1,    49,   101,
     114,    96,   114,   102,    -1,    51,   101,   114,    96,   114,
     102,    -1,    53,   101,   114,    96,   114,   102,    -1,    54,
     101,   114,    96,   114,   102,    -1,    50,   101,   114,    96,
     114,    96,   114,   102,    -1,    52,   101,   114,    96,   114,
      96,   114,   102,    -1,    53,   101,   114,    96,   114,    96,
     114,   102,    -1,    54,   101,   114,    96,   114,    96,   114,
     102,    -1,    55,   101,   102,    -1,    55,   101,   113,   102,
      -1,   130,   101,   114,    96,   114,    96,   114,   102,    -1,
     130,   101,   114,   102,    -1,    18,   101,   124,    96,   124,
      96,   128,   102,    -1,    18,   101,   114,   102,    -1,   121,
      12,    69,    -1,    68,    12,    69,    -1,   101,   113,   102,
      -1,   113,    96,   114,    -1,   114,    -1,   119,    -1,   112,
      -1,   114,    93,   114,    96,   114,    94,    -1,   114,    93,
     114,    94,    -1,    74,   101,   114,    96,    48,   102,    -1,
      74,   101,   114,    96,    51,   102,    -1,    74,   101,   114,
      96,    52,   102,    -1,    74,   101,   114,    96,    54,   102,
      -1,    74,   101,   114,    96,    55,   102,    -1,    74,   101,
     114,    96,   114,   102,    -1,   116,   114,   118,    -1,   116,
     114,    88,   114,   118,    -1,   117,   114,    96,   114,   118,
      -1,    -1,    79,   101,   115,   114,   102,    -1,    80,   101,
      -1,    75,   101,    -1,   102,    -1,   114,    10,    -1,   114,
       7,    -1,   114,    90,   114,    -1,   114,    91,   114,    -1,
     114,    92,   114,    -1,   114,    95,   114,    -1,   114,    89,
     114,    -1,   114,    98,   114,    -1,   114,     9,   114,    -1,
     114,     4,   114,    -1,   114,     3,   114,    -1,   114,    99,
     114,    -1,     8,   114,    -1,    91,   114,    -1,   122,   129,
      -1,   113,   129,    -1,    71,    -1,   104,   114,   104,    -1,
      56,   112,    -1,    57,   112,    -1,    58,   112,    -1,    59,
     112,    -1,   130,   112,    93,   114,    94,    93,   114,    94,
      -1,   130,   112,    -1,   122,    96,   112,    -1,    17,   112,
      -1,    68,    -1,   114,    -1,   101,   114,    96,   113,   102,
      -1,    71,    -1,   125,    -1,   125,   101,   113,   102,    -1,
     126,    -1,   126,    96,   127,    -1,   126,    -1,   101,   127,
     102,    -1,    88,    -1,    23,    -1,    28,    -1,    16,    -1,
      14,    -1,    89,   123,    -1,    62,    68,    97,    -1,    62,
      97,    -1,    60,    68,    97,    -1,    61,   113,    -1,    63,
     112,    -1,   135,    96,   112,    -1,    65,   101,    56,   102,
      -1,    65,   101,    57,   102,    -1,    65,   101,    58,   102,
      -1,    65,   101,    59,   102,    -1,    65,   101,    18,   102,
      -1,    65,   101,   130,   102,    -1,    65,   101,    17,   102,
      -1,    65,   101,   112,   102,    -1,    65,   101,   112,    96,
      56,   102,    -1,    65,   101,   112,    96,    57,   102,    -1,
      65,   101,   112,    96,    58,   102,    -1,    65,   101,   112,
      96,    59,   102,    -1,    65,   101,   112,    96,    18,   102,
      -1,    65,   101,   112,    96,   130,   102,    -1,    65,   101,
     112,    96,    17,   102,    -1,    65,   101,   102,    -1,    18,
      -1,   137,   112,   129,   124,    96,   124,    96,   128,    -1,
     137,   112,    -1,   137,   112,   129,   112,    -1,   137,   112,
     129,   112,    93,   113,    94,    -1,    87,   123,    -1,    66,
      -1,    34,    -1,   140,   114,    -1,    67,   114,    -1,   113,
      -1,    82,   101,   114,   102,    69,    -1,    78,    69,    -1,
      82,   101,   114,   102,    76,    -1,    76,    -1,    77,    -1,
      84,    68,    69,    -1,    81,    68,    68,    68,    69,    -1,
      17,   121,    69,    -1,    73,    68,    69,    -1,    73,    68,
      68,    69,    -1,    86,   122,    -1,    86,   114,    -1,    85,
     101,   113,   102,    -1,    85,   101,   102,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   319,   319,   321,   355,   356,   358,   360,   364,   369,
     371,   422,   423,   424,   425,   426,   427,   428,   429,   433,
     436,   437,   438,   439,   440,   441,   442,   443,   444,   447,
     454,   459,   463,   467,   471,   475,   489,   517,   541,   547,
     553,   557,   561,   565,   569,   573,   577,   581,   585,   589,
     593,   597,   601,   605,   609,   613,   617,   621,   625,   629,
     633,   639,   643,   647,   651,   655,   660,   666,   670,   681,
     684,   685,   686,   690,   694,   698,   702,   706,   710,   714,
     718,   722,   739,   746,   745,   763,   771,   779,   788,   792,
     796,   800,   804,   808,   812,   816,   820,   824,   828,   832,
     836,   848,   855,   856,   875,   876,   888,   893,   898,   902,
     906,   943,   967,   988,   996,  1000,  1001,  1015,  1023,  1032,
    1077,  1078,  1087,  1088,  1094,  1101,  1102,  1103,  1104,  1112,
    1117,  1122,  1129,  1137,  1149,  1166,  1186,  1190,  1194,  1199,
    1203,  1207,  1211,  1215,  1220,  1226,  1232,  1238,  1244,  1250,
    1256,  1268,  1275,  1279,  1316,  1323,  1328,  1341,  1348,  1348,
    1351,  1419,  1423,  1452,  1465,  1482,  1491,  1496,  1504,  1517,
    1538,  1548,  1567,  1590,  1596,  1608,  1614
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "ARROW",
  "GRING_CMD", "BIGINTMAT_CMD", "BIGINTVEC_CMD", "INTMAT_CMD", "PROC_CMD",
  "RING_CMD", "BEGIN_RING", "BUCKET_CMD", "IDEAL_CMD", "MAP_CMD",
  "MATRIX_CMD", "MODUL_CMD", "NUMBER_CMD", "POLY_CMD", "RESOLUTION_CMD",
  "SMATRIX_CMD", "VECTOR_CMD", "BETTI_CMD", "E_CMD", "FETCH_CMD",
  "FREEMODULE_CMD", "KEEPRING_CMD", "IMAP_CMD", "KOSZUL_CMD", "MAXID_CMD",
  "MONOM_CMD", "PAR_CMD", "PREIMAGE_CMD", "VAR_CMD", "VALTVARS", "VMAXDEG",
  "VMAXMULT", "VNOETHER", "VMINPOLY", "END_RING", "CMD_1", "CMD_2",
  "CMD_3", "CMD_12", "CMD_13", "CMD_23", "CMD_123", "CMD_M", "ROOT_DECL",
  "ROOT_DECL_LIST", "RING_DECL", "RING_DECL_LIST", "EXAMPLE_CMD",
  "EXPORT_CMD", "HELP_CMD", "KILL_CMD", "LIB_CMD", "LISTVAR_CMD",
  "SETRING_CMD", "TYPE_CMD", "STRINGTOK", "BLOCKTOK", "INT_CONST",
  "UNKNOWN_IDENT", "MONOM", "PROC_DEF", "APPLY", "ASSUME_CMD", "BREAK_CMD",
  "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD", "IF_CMD",
  "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR", "'='", "'<'",
  "'+'", "'-'", "'/'", "'['", "']'", "'^'", "','", "';'", "'&'", "':'",
  "UMINUS", "'('", "')'", "'.'", "'`'", "$accept", "lines", "pprompt",
  "flowctrl", "example_dummy", "command", "assign", "elemexpr", "exprlist",
  "expr", "$@1", "quote_start", "assume_start", "quote_end",
  "expr_arithmetic", "left_value", "extendedid", "declare_ip_variable",
  "stringexpr", "rlist", "ordername", "orderelem", "OrderingList",
  "ordering", "cmdeq", "mat_cmd", "filecmd", "helpcmd", "examplecmd",
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
     335,   336,   337,   338,   339,   340,   341,   342,    61,    60,
      43,    45,    47,    91,    93,    94,    44,    59,    38,    58,
     343,    40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   105,   106,   106,   107,   107,   107,   107,   107,   107,
     107,   108,   108,   108,   108,   108,   108,   108,   108,   109,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   111,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   113,   113,
     114,   114,   114,   114,   114,   114,   114,   114,   114,   114,
     114,   114,   114,   115,   114,   116,   117,   118,   119,   119,
     119,   119,   119,   119,   119,   119,   119,   119,   119,   119,
     119,   119,   120,   120,   121,   121,   122,   122,   122,   122,
     122,   122,   122,   122,   123,   124,   124,   125,   126,   126,
     127,   127,   128,   128,   129,   130,   130,   130,   130,   131,
     132,   132,   133,   134,   135,   135,   136,   136,   136,   136,
     136,   136,   136,   136,   136,   136,   136,   136,   136,   136,
     136,   136,   137,   138,   138,   138,   138,   139,   140,   140,
     141,   142,   142,   143,   143,   143,   143,   143,   144,   145,
     146,   146,   146,   147,   147,   148,   148
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
       4,     8,     4,     8,     4,     3,     3,     3,     3,     1,
       1,     1,     6,     4,     6,     6,     6,     6,     6,     6,
       3,     5,     5,     0,     5,     2,     2,     1,     2,     2,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       2,     2,     2,     2,     1,     3,     2,     2,     2,     2,
       8,     2,     3,     2,     1,     1,     5,     1,     1,     4,
       1,     3,     1,     3,     1,     1,     1,     1,     1,     2,
       3,     2,     3,     2,     2,     3,     4,     4,     4,     4,
       4,     4,     4,     4,     6,     6,     6,     6,     6,     6,
       6,     3,     1,     8,     2,     4,     7,     2,     1,     1,
       2,     2,     1,     5,     2,     5,     1,     1,     3,     5,
       3,     3,     4,     2,     2,     4,     3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,   128,   127,     0,   152,   125,
     126,   159,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   158,
       0,   114,    37,   104,    30,     0,     0,     0,   166,   167,
       0,     0,     0,     0,     0,     8,     0,     0,     0,    38,
       0,     0,     0,     9,     0,     0,     3,     4,    13,     0,
      20,    71,   162,    69,     0,     0,    70,     0,    31,     0,
      39,     0,    16,    17,    18,    21,    22,    23,     0,    25,
      26,     0,    27,    28,    11,    12,    14,    15,    24,     7,
      10,     0,     0,     0,     0,     0,     0,    38,   100,     0,
       0,    71,     0,    31,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    71,     0,    71,     0,    71,     0,
      71,     0,    19,   133,     0,   131,    71,     0,   161,     0,
       0,     0,    86,   164,    83,    85,     0,     0,     0,     0,
       0,   174,   173,   114,   157,   129,   101,     0,     0,     0,
       5,     0,     0,   124,     0,   103,     0,     0,    89,     0,
      88,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    29,     0,     0,     6,   102,     0,    71,     0,
      71,   160,     0,     0,     0,     0,     0,     0,    69,   170,
       0,   115,     0,     0,     0,     0,     0,     0,     0,     0,
      59,     0,    69,    43,     0,    69,    46,     0,   132,   130,
       0,     0,     0,     0,     0,     0,   151,    71,     0,    66,
       0,   171,     0,     0,     0,     0,   168,   176,     0,    36,
      67,   105,    32,    34,     0,    68,    98,    97,    96,    94,
      90,    91,    92,     0,    93,    95,    99,    33,     0,    87,
      80,     0,    65,    71,     0,     0,    71,     0,     0,     0,
       0,     0,     0,     0,    40,    69,    64,     0,    47,     0,
       0,     0,    48,     0,    49,     0,     0,    50,    60,    41,
      42,    44,    45,   142,   140,   136,   137,   138,   139,     0,
     143,   141,   172,     0,     0,     0,     0,   175,    35,    73,
       0,     0,     0,     0,    62,     0,    71,   115,     0,    42,
      45,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    84,   169,   163,   165,     0,    81,    82,     0,
       0,     0,     0,     0,     0,    51,     0,    52,     0,     0,
      53,     0,    54,   150,   148,   144,   145,   146,   147,   149,
      74,    75,    76,    77,    78,    79,    72,     0,     0,     0,
       0,   116,   117,     0,   118,   122,     0,     0,     0,     0,
       0,     0,     0,   156,     0,   120,     0,     0,    63,    55,
      56,    57,    58,    61,   110,   153,     0,   123,     0,   121,
     119
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    56,    57,    58,    59,    60,    61,   148,    63,
     223,    64,    65,   250,    66,    67,    68,    69,    70,   192,
     374,   375,   386,   376,   155,    99,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,    87,    88,    89
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -358
static const yytype_int16 yypact[] =
{
    -358,   383,  -358,   -61,  1962,  -358,  -358,  2042,   -69,  -358,
    -358,  -358,   -59,   -29,   -15,    -4,    16,    21,    23,    36,
    2122,  2202,  2282,  2362,   -48,  1962,   -60,  1962,    56,  -358,
    1962,    42,  -358,  -358,  -358,   -11,    74,    76,  -358,  -358,
      31,    78,    93,     8,    96,  -358,    40,    98,  2442,    86,
      86,  1962,  1962,  -358,  1962,  1962,  -358,  -358,  -358,    65,
    -358,    -2,   -65,  1360,  1962,  1962,  -358,  1962,   161,   -71,
    -358,  2522,  -358,  -358,  -358,  -358,   110,  -358,  1962,  -358,
    -358,  1962,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,   109,   -69,   115,   118,   120,   132,  -358,    60,   136,
    1962,    99,  1360,    -8,  2602,  1962,  1962,  1962,  1962,  1962,
    1962,  1962,  1482,  1962,   153,  1562,   171,  1962,   223,  1642,
     225,   142,  -358,   146,   148,  -358,    32,  1722,  1360,    35,
      72,  1962,  -358,  -358,  -358,  -358,   179,  1962,   189,  1802,
    2042,  1360,   159,  -358,  -358,  -358,    60,   -32,   -56,     3,
    -358,  1962,  1882,  -358,  1962,  -358,  1962,  1962,  -358,  1962,
    -358,  1962,  1962,  1962,  1962,  1962,  1962,  1962,  1962,  1962,
     204,   553,   146,   196,  1962,  -358,  -358,  1962,   173,  1962,
      92,  1360,  1962,  1962,  1562,  1962,  1642,  1962,   576,  -358,
    1962,   650,   175,   673,   688,   703,   253,   276,   718,   411,
    -358,   -55,   747,  -358,   -52,   815,  -358,   -47,  -358,  -358,
      48,    58,    89,   101,   130,   151,  -358,    34,   174,  -358,
     221,  -358,   844,  1962,   230,   859,  -358,  -358,   -44,  -358,
    -358,  -358,  -358,  -358,   -43,  1360,    53,  1390,  1390,  1416,
      28,    28,    60,   479,    12,  1375,    28,  -358,  1962,  -358,
    -358,  1962,  -358,   262,   508,  1962,   108,  2602,   576,   747,
     -27,   815,   -25,   508,  -358,   874,  -358,  2602,  -358,  1962,
    1962,  1962,  -358,  1962,  -358,  1962,  1962,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,   536,
    -358,  -358,  -358,  2682,   889,   239,   -58,  -358,  -358,  -358,
    1962,   912,   912,  1962,  -358,   986,   280,  1360,   227,  -358,
    -358,  1962,   229,  1009,  1024,  1039,  1054,   523,   538,   215,
     231,   233,   245,   251,   255,   258,   176,   180,   186,   199,
     203,  1083,  -358,  -358,  -358,  -358,  1151,  -358,  -358,  1180,
     234,  1962,  2602,   -23,   -68,  -358,  1962,  -358,  1962,  1962,
    -358,  1962,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  1962,  1962,    -7,
     266,  -358,  -358,   270,   228,  -358,   268,  1195,  1210,  1225,
    1248,  1322,  1345,  -358,   -68,   284,   274,  1962,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,   270,  -358,   -22,  -358,
    -358
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -358,  -358,  -358,  -358,  -358,  -358,  -358,     7,    -1,    61,
    -358,  -358,  -358,    13,  -358,  -358,   375,   337,   289,  -252,
    -358,  -357,    -9,     4,   -67,     0,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,  -358,
    -358,  -358,  -358,  -358
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -156
static const yytype_int16 yytable[] =
{
      62,    71,   176,   372,   173,   308,   156,   157,   124,   151,
     158,   334,   159,   160,   101,   312,   385,   153,   335,   158,
     121,   122,   160,   153,   123,   174,   175,   114,   116,   118,
     120,   154,   104,   373,   126,   158,    90,   125,   160,   385,
     154,   154,   105,   151,   154,   151,   230,   278,    71,   154,
     280,   147,   154,   154,   129,   282,  -156,   130,   297,   298,
     158,   189,   229,   160,   154,    98,   172,   158,   102,   154,
     160,   154,   106,   154,   154,   309,   136,   310,   178,   371,
     400,   102,   102,   102,   102,   180,   107,   383,   102,   154,
     370,   128,   161,   162,   163,   164,   165,   108,   166,   152,
     133,   167,   168,   151,   219,   165,   169,   231,   138,   141,
     151,   201,   146,   257,   204,   169,   149,   109,   207,   151,
     164,   165,   110,   166,   111,   170,   171,   218,  -134,  -134,
     289,   169,   102,   152,   217,   152,   290,   112,   228,   102,
     220,   221,   181,   162,   163,   164,   165,   101,   166,   182,
     283,   234,   168,   165,   143,   166,   169,   127,   232,   104,
     284,   188,   150,   169,   151,   191,   193,   194,   195,   196,
     197,   198,   199,   173,   202,   131,   247,   132,   205,   134,
     153,   253,   151,   260,   151,   262,   256,  -113,   102,  -154,
     183,   285,   222,   152,   135,  -113,  -113,   137,   225,   139,
     152,   102,   184,   286,  -135,  -135,   179,   156,   157,   152,
     182,   158,   102,   159,   160,   235,   183,   236,   237,   184,
     238,   185,   239,   240,   241,   242,   243,   244,   245,   246,
     102,   185,   287,   186,   151,   102,   151,   187,   254,   208,
     102,  -106,   154,   258,   259,   209,   261,   224,   263,  -106,
    -106,   265,   186,   288,   152,   174,   156,   157,   226,  -107,
     158,  -111,   159,   160,   306,   252,   255,  -107,  -107,  -111,
    -111,   267,   152,   151,   152,   187,   291,   105,   360,   156,
     157,   108,   361,   158,   294,   159,   160,   109,   362,   325,
     292,   151,   248,   161,   162,   163,   164,   165,   295,   166,
     111,   363,   167,   168,   112,   364,   249,   169,   333,   301,
     343,  -108,   302,  -109,   337,   338,   305,   353,   307,  -108,
    -108,  -109,  -109,   342,   152,   344,   152,   368,   307,   387,
     313,   314,   315,   354,   316,   355,   317,   318,   144,   145,
     369,   372,   161,   162,   163,   164,   165,   356,   166,   271,
    -112,   167,   168,   357,   331,   272,   169,   358,  -112,  -112,
     359,   336,   384,   152,   339,   161,   162,   163,   164,   165,
     388,   166,   273,   341,   167,   168,   397,  -155,   274,   169,
     396,   152,   103,     2,     3,   142,   398,   399,   395,     0,
       0,     4,     0,     0,     0,     0,     0,     5,     0,     6,
       7,     8,     0,   307,     0,     0,     9,   377,     0,   378,
     379,    10,   380,     0,   156,   157,     0,    11,   158,     0,
     159,   160,     0,     0,     0,     0,     0,     0,   381,   382,
       0,    12,    13,    14,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,    26,    27,     0,    28,    29,
      30,    31,     0,    32,    33,    34,    35,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    45,    46,    47,    48,
      49,     0,    50,     0,    51,     0,    52,     0,     0,     0,
      53,     0,   156,   157,    54,     0,   158,    55,   159,   160,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     161,   162,   163,   164,   165,     0,   166,   276,     0,   167,
     168,   156,   157,   277,   169,   158,     0,   159,   160,     0,
       0,     0,     0,     0,     0,     0,   156,   157,     0,     0,
     158,     0,   159,   160,     0,     0,     0,     0,     0,     0,
       0,   156,   157,     0,     0,   158,     0,   159,   160,     0,
       5,     0,     6,   319,   320,     0,   156,   157,     0,     9,
     158,     0,   159,   160,    10,     0,     0,     0,   161,   162,
     163,   164,   165,   299,   166,   300,     0,   167,   168,   156,
     157,     0,   169,   158,     0,   159,   160,     0,     0,     0,
       0,     0,   321,   322,   323,   324,     0,   161,   162,   163,
     164,   165,     0,   166,   303,     0,   167,   168,     0,     0,
     304,   169,   161,   162,   163,   164,   165,     0,   166,   349,
       0,   167,   168,     0,     0,   350,   169,   161,   162,   163,
     164,   165,     0,   166,   351,     0,   167,   168,     0,     0,
     352,   169,   161,   162,   163,   164,   165,     0,   166,   251,
       0,   167,   168,   156,   157,     0,   169,   158,     0,   159,
     160,     0,     0,     0,     0,   161,   162,   163,   164,   165,
       0,   166,     0,     0,   167,   168,   156,   157,   264,   169,
     158,     0,   159,   160,     0,     0,     0,     0,     0,     0,
       0,   156,   157,     0,     0,   158,     0,   159,   160,     0,
       0,     0,     0,     0,     0,     0,   156,   157,     0,     0,
     158,     0,   159,   160,     0,     0,     0,     0,     0,     0,
       0,   156,   157,     0,     0,   158,     0,   159,   160,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   161,
     162,   163,   164,   165,     0,   166,     0,     0,   167,   168,
     156,   157,   266,   169,   158,     0,   159,   160,     0,     0,
       0,     0,   161,   162,   163,   164,   165,     0,   166,     0,
       0,   167,   168,     0,     0,   268,   169,   161,   162,   163,
     164,   165,     0,   166,   269,     0,   167,   168,     0,     0,
       0,   169,   161,   162,   163,   164,   165,     0,   166,   270,
       0,   167,   168,     0,     0,     0,   169,   161,   162,   163,
     164,   165,     0,   166,   275,     0,   167,   168,   156,   157,
       0,   169,   158,     0,   159,   160,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   161,   162,   163,   164,
     165,     0,   166,     0,     0,   167,   168,   156,   157,   279,
     169,   158,     0,   159,   160,     0,     0,     0,     0,     0,
       0,     0,   156,   157,     0,     0,   158,     0,   159,   160,
       0,     0,     0,     0,     0,     0,     0,   156,   157,     0,
       0,   158,     0,   159,   160,     0,     0,     0,     0,     0,
       0,     0,   156,   157,     0,     0,   158,     0,   159,   160,
       0,     0,     0,     0,   161,   162,   163,   164,   165,     0,
     166,     0,     0,   167,   168,   156,   157,   281,   169,   158,
       0,   159,   160,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   161,   162,   163,   164,   165,     0,   166,
     293,     0,   167,   168,     0,     0,     0,   169,   161,   162,
     163,   164,   165,     0,   166,     0,     0,   167,   168,     0,
       0,   296,   169,   161,   162,   163,   164,   165,     0,   166,
     311,     0,   167,   168,     0,     0,     0,   169,   161,   162,
     163,   164,   165,     0,   166,     0,     0,   167,   168,   156,
     157,   332,   169,   158,     0,   159,   160,     0,     0,     0,
       0,   161,   162,   163,   164,   165,     0,   166,     0,     0,
     167,   168,   156,   157,   249,   169,   158,     0,   159,   160,
       0,     0,     0,     0,     0,     0,     0,   156,   157,     0,
       0,   158,     0,   159,   160,     0,     0,     0,     0,     0,
       0,     0,   156,   157,     0,     0,   158,     0,   159,   160,
       0,     0,     0,     0,     0,     0,     0,   156,   157,     0,
       0,   158,     0,   159,   160,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   161,   162,   163,   164,   165,
     340,   166,     0,     0,   167,   168,   156,   157,     0,   169,
     158,     0,   159,   160,     0,     0,     0,     0,   161,   162,
     163,   164,   165,     0,   166,     0,     0,   167,   168,     0,
       0,   345,   169,   161,   162,   163,   164,   165,     0,   166,
     346,     0,   167,   168,     0,     0,     0,   169,   161,   162,
     163,   164,   165,     0,   166,     0,     0,   167,   168,     0,
       0,   347,   169,   161,   162,   163,   164,   165,     0,   166,
     348,     0,   167,   168,   156,   157,     0,   169,   158,     0,
     159,   160,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   161,   162,   163,   164,   165,     0,   166,     0,
       0,   167,   168,   156,   157,   365,   169,   158,     0,   159,
     160,     0,     0,     0,     0,     0,     0,     0,   156,   157,
       0,     0,   158,     0,   159,   160,     0,     0,     0,     0,
       0,     0,     0,   156,   157,     0,     0,   158,     0,   159,
     160,     0,     0,     0,     0,     0,     0,     0,   156,   157,
       0,     0,   158,     0,   159,   160,     0,     0,     0,     0,
     161,   162,   163,   164,   165,   366,   166,     0,     0,   167,
     168,   156,   157,     0,   169,   158,     0,   159,   160,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   161,
     162,   163,   164,   165,     0,   166,   367,     0,   167,   168,
       0,     0,     0,   169,   161,   162,   163,   164,   165,     0,
     166,     0,     0,   167,   168,     0,     0,   389,   169,   161,
     162,   163,   164,   165,     0,   166,     0,     0,   167,   168,
       0,     0,   390,   169,   161,   162,   163,   164,   165,     0,
     166,     0,     0,   167,   168,   156,   157,   391,   169,   158,
       0,   159,   160,     0,     0,     0,     0,   161,   162,   163,
     164,   165,     0,   166,     0,     0,   167,   168,   156,   157,
     392,   169,   158,     0,   159,   160,     0,     0,     0,     0,
       0,     0,     0,   156,   157,     0,     0,   158,     0,   159,
     160,     0,     0,     0,     0,     0,     0,     0,   156,   157,
       0,     0,   158,     0,   159,   160,     0,     0,     0,     0,
       0,     0,     0,   156,     0,     0,     0,   158,     0,     0,
     160,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   161,   162,   163,   164,   165,     0,   166,     0,   156,
     167,   168,     0,   158,   393,   169,   160,     0,     0,     0,
       0,     0,     0,     0,   161,   162,   163,   164,   165,   394,
     166,     0,     0,   167,   168,     0,     0,     0,   169,   161,
     162,   163,   164,   165,     0,   166,     0,     0,   167,   168,
       0,     0,     0,   169,   161,   162,   163,   164,   165,     0,
     166,     0,     0,     0,   168,     0,     0,     0,   169,   161,
     162,   163,   164,   165,     0,   166,     0,     0,     0,   168,
       4,     0,     0,   169,     0,     0,     5,     0,     6,    91,
      92,     0,     0,     0,     0,     9,   162,   163,   164,   165,
      10,   166,     0,     0,     0,   168,     0,     0,     0,   169,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,   200,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,   203,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,   210,
     211,     0,     0,    54,   206,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,   212,   213,
     214,   215,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,   216,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,   227,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,   233,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   100,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   113,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   115,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   117,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,   140,
      92,     0,     0,   119,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,    54,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   177,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      12,    13,    14,    15,    16,    17,    18,    19,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       4,     0,     0,    51,     0,    52,     5,     0,     6,    91,
      92,     0,     0,   190,     0,     9,    55,     0,     0,     0,
      10,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     326,    13,    14,   327,   328,    17,   329,   330,    93,    94,
      95,    96,     0,     0,     0,     0,     0,     0,     0,     0,
      31,     0,    32,    33,    34,     0,    36,    37,     0,     0,
       0,    41,    42,     0,     0,     0,     0,     0,     0,    97,
       0,     0,     0,    51,     0,    52,     0,     0,     0,     0,
       0,     0,     0,    54,     0,     0,    55
};

static const yytype_int16 yycheck[] =
{
       1,     1,    69,    71,    12,   257,     3,     4,    68,    11,
       7,    69,     9,    10,     7,   267,   373,    88,    76,     7,
      68,    69,    10,    88,    25,    96,    97,    20,    21,    22,
      23,    96,   101,   101,    27,     7,    97,    97,    10,   396,
      96,    96,   101,    11,    96,    11,   102,   102,    48,    96,
     102,    52,    96,    96,    12,   102,     3,    68,   102,   102,
       7,    69,    94,    10,    96,     4,    67,     7,     7,    96,
      10,    96,   101,    96,    96,   102,    68,   102,    71,   102,
     102,    20,    21,    22,    23,    78,   101,    94,    27,    96,
     342,    30,    89,    90,    91,    92,    93,   101,    95,   101,
      69,    98,    99,    11,    69,    93,   103,   104,    68,    48,
      11,   112,    51,   180,   115,   103,    55,   101,   119,    11,
      92,    93,   101,    95,   101,    64,    65,   127,    96,    97,
      96,   103,    71,   101,   127,   101,   102,   101,   139,    78,
      68,    69,    81,    90,    91,    92,    93,   140,    95,   101,
     102,   152,    99,    93,    68,    95,   103,   101,   151,   101,
     102,   100,    97,   103,    11,   104,   105,   106,   107,   108,
     109,   110,   111,    12,   113,   101,   169,   101,   117,   101,
      88,   174,    11,   184,    11,   186,   179,    88,   127,    97,
     101,   102,   131,   101,   101,    96,    97,   101,   137,   101,
     101,   140,   101,   102,    96,    97,    96,     3,     4,   101,
     101,     7,   151,     9,    10,   154,   101,   156,   157,   101,
     159,   101,   161,   162,   163,   164,   165,   166,   167,   168,
     169,   101,   102,   101,    11,   174,    11,   101,   177,    97,
     179,    88,    96,   182,   183,    97,   185,    68,   187,    96,
      97,   190,   101,   102,   101,    96,     3,     4,    69,    88,
       7,    88,     9,    10,   257,    69,    93,    96,    97,    96,
      97,    96,   101,    11,   101,   101,   102,   101,   102,     3,
       4,   101,   102,     7,   223,     9,    10,   101,   102,   289,
      69,    11,    88,    89,    90,    91,    92,    93,    68,    95,
     101,   102,    98,    99,   101,   102,   102,   103,    69,   248,
     311,    88,   251,    88,   301,   302,   255,   102,   257,    96,
      97,    96,    97,    96,   101,    96,   101,    93,   267,   101,
     269,   270,   271,   102,   273,   102,   275,   276,    49,    50,
     341,    71,    89,    90,    91,    92,    93,   102,    95,    96,
      88,    98,    99,   102,   293,   102,   103,   102,    96,    97,
     102,   300,    96,   101,   303,    89,    90,    91,    92,    93,
     102,    95,    96,    93,    98,    99,   102,    97,   102,   103,
      96,   101,     7,     0,     1,    48,   387,   396,   384,    -1,
      -1,     8,    -1,    -1,    -1,    -1,    -1,    14,    -1,    16,
      17,    18,    -1,   342,    -1,    -1,    23,   346,    -1,   348,
     349,    28,   351,    -1,     3,     4,    -1,    34,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,   367,   368,
      -1,    48,    49,    50,    51,    52,    53,    54,    55,    56,
      57,    58,    59,    60,    61,    62,    63,    -1,    65,    66,
      67,    68,    -1,    70,    71,    72,    73,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    84,    85,    86,
      87,    -1,    89,    -1,    91,    -1,    93,    -1,    -1,    -1,
      97,    -1,     3,     4,   101,    -1,     7,   104,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    93,    -1,    95,    96,    -1,    98,
      99,     3,     4,   102,   103,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      14,    -1,    16,    17,    18,    -1,     3,     4,    -1,    23,
       7,    -1,     9,    10,    28,    -1,    -1,    -1,    89,    90,
      91,    92,    93,    94,    95,    96,    -1,    98,    99,     3,
       4,    -1,   103,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    56,    57,    58,    59,    -1,    89,    90,    91,
      92,    93,    -1,    95,    96,    -1,    98,    99,    -1,    -1,
     102,   103,    89,    90,    91,    92,    93,    -1,    95,    96,
      -1,    98,    99,    -1,    -1,   102,   103,    89,    90,    91,
      92,    93,    -1,    95,    96,    -1,    98,    99,    -1,    -1,
     102,   103,    89,    90,    91,    92,    93,    -1,    95,    96,
      -1,    98,    99,     3,     4,    -1,   103,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      -1,    95,    -1,    -1,    98,    99,     3,     4,   102,   103,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    98,    99,
       3,     4,   102,   103,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    93,    -1,    95,    -1,
      -1,    98,    99,    -1,    -1,   102,   103,    89,    90,    91,
      92,    93,    -1,    95,    96,    -1,    98,    99,    -1,    -1,
      -1,   103,    89,    90,    91,    92,    93,    -1,    95,    96,
      -1,    98,    99,    -1,    -1,    -1,   103,    89,    90,    91,
      92,    93,    -1,    95,    96,    -1,    98,    99,     3,     4,
      -1,   103,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    90,    91,    92,
      93,    -1,    95,    -1,    -1,    98,    99,     3,     4,   102,
     103,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    98,    99,     3,     4,   102,   103,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    90,    91,    92,    93,    -1,    95,
      96,    -1,    98,    99,    -1,    -1,    -1,   103,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    98,    99,    -1,
      -1,   102,   103,    89,    90,    91,    92,    93,    -1,    95,
      96,    -1,    98,    99,    -1,    -1,    -1,   103,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    98,    99,     3,
       4,   102,   103,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    89,    90,    91,    92,    93,    -1,    95,    -1,    -1,
      98,    99,     3,     4,   102,   103,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,
      94,    95,    -1,    -1,    98,    99,     3,     4,    -1,   103,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    98,    99,    -1,
      -1,   102,   103,    89,    90,    91,    92,    93,    -1,    95,
      96,    -1,    98,    99,    -1,    -1,    -1,   103,    89,    90,
      91,    92,    93,    -1,    95,    -1,    -1,    98,    99,    -1,
      -1,   102,   103,    89,    90,    91,    92,    93,    -1,    95,
      96,    -1,    98,    99,     3,     4,    -1,   103,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    90,    91,    92,    93,    -1,    95,    -1,
      -1,    98,    99,     3,     4,   102,   103,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      89,    90,    91,    92,    93,    94,    95,    -1,    -1,    98,
      99,     3,     4,    -1,   103,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,
      90,    91,    92,    93,    -1,    95,    96,    -1,    98,    99,
      -1,    -1,    -1,   103,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    98,    99,    -1,    -1,   102,   103,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    98,    99,
      -1,    -1,   102,   103,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    98,    99,     3,     4,   102,   103,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    89,    90,    91,
      92,    93,    -1,    95,    -1,    -1,    98,    99,     3,     4,
     102,   103,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,    -1,    -1,    -1,     7,    -1,    -1,
      10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    90,    91,    92,    93,    -1,    95,    -1,     3,
      98,    99,    -1,     7,   102,   103,    10,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    90,    91,    92,    93,    94,
      95,    -1,    -1,    98,    99,    -1,    -1,    -1,   103,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    98,    99,
      -1,    -1,    -1,   103,    89,    90,    91,    92,    93,    -1,
      95,    -1,    -1,    -1,    99,    -1,    -1,    -1,   103,    89,
      90,    91,    92,    93,    -1,    95,    -1,    -1,    -1,    99,
       8,    -1,    -1,   103,    -1,    -1,    14,    -1,    16,    17,
      18,    -1,    -1,    -1,    -1,    23,    90,    91,    92,    93,
      28,    95,    -1,    -1,    -1,    99,    -1,    -1,    -1,   103,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,   102,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
       8,    -1,    -1,    91,    -1,    93,    14,    -1,    16,    17,
      18,    -1,    -1,   101,    -1,    23,   104,    -1,    -1,    -1,
      28,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      48,    49,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      68,    -1,    70,    71,    72,    -1,    74,    75,    -1,    -1,
      -1,    79,    80,    -1,    -1,    -1,    -1,    -1,    -1,    87,
      -1,    -1,    -1,    91,    -1,    93,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    -1,   104
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   106,     0,     1,     8,    14,    16,    17,    18,    23,
      28,    34,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    61,    62,    63,    65,    66,
      67,    68,    70,    71,    72,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      89,    91,    93,    97,   101,   104,   107,   108,   109,   110,
     111,   112,   113,   114,   116,   117,   119,   120,   121,   122,
     123,   130,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,   148,
      97,    17,    18,    56,    57,    58,    59,    87,   114,   130,
     101,   112,   114,   121,   101,   101,   101,   101,   101,   101,
     101,   101,   101,   101,   112,   101,   112,   101,   112,   101,
     112,    68,    69,   113,    68,    97,   112,   101,   114,    12,
      68,   101,   101,    69,   101,   101,    68,   101,    68,   101,
      17,   114,   122,    68,   123,   123,   114,   113,   113,   114,
      97,    11,   101,    88,    96,   129,     3,     4,     7,     9,
      10,    89,    90,    91,    92,    93,    95,    98,    99,   103,
     114,   114,   113,    12,    96,    97,   129,   101,   112,    96,
     112,   114,   101,   101,   101,   101,   101,   101,   114,    69,
     101,   114,   124,   114,   114,   114,   114,   114,   114,   114,
     102,   113,   114,   102,   113,   114,   102,   113,    97,    97,
      17,    18,    56,    57,    58,    59,   102,   112,   130,    69,
      68,    69,   114,   115,    68,   114,    69,   102,   113,    94,
     102,   104,   112,   102,   113,   114,   114,   114,   114,   114,
     114,   114,   114,   114,   114,   114,   114,   112,    88,   102,
     118,    96,    69,   112,   114,    93,   112,   129,   114,   114,
     113,   114,   113,   114,   102,   114,   102,    96,   102,    96,
      96,    96,   102,    96,   102,    96,    96,   102,   102,   102,
     102,   102,   102,   102,   102,   102,   102,   102,   102,    96,
     102,   102,    69,    96,   114,    68,   102,   102,   102,    94,
      96,   114,   114,    96,   102,   114,   112,   114,   124,   102,
     102,    96,   124,   114,   114,   114,   114,   114,   114,    17,
      18,    56,    57,    58,    59,   130,    48,    51,    52,    54,
      55,   114,   102,    69,    69,    76,   114,   118,   118,   114,
      94,    93,    96,   113,    96,   102,    96,   102,    96,    96,
     102,    96,   102,   102,   102,   102,   102,   102,   102,   102,
     102,   102,   102,   102,   102,   102,    94,    96,    93,   113,
     124,   102,    71,   101,   125,   126,   128,   114,   114,   114,
     114,   114,   114,    94,    96,   126,   127,   101,   102,   102,
     102,   102,   102,   102,    94,   128,    96,   102,   113,   127,
     102
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
#line 322 "grammar.y"
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
#line 357 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 359 "grammar.y"
    { (yyvsp[(1) - (2)].lv).CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 361 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 365 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 370 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 372 "grammar.y"
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
#line 430 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 433 "grammar.y"
    { omFree((ADDRESS)(yyvsp[(2) - (2)].name)); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 448 "grammar.y"
    {
            if(iiAssign(&(yyvsp[(1) - (2)].lv),&(yyvsp[(2) - (2)].lv))) YYERROR;
          ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 455 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active (7)");
            syMakeMonom(&(yyval.lv),(yyvsp[(1) - (1)].name));
          ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 460 "grammar.y"
    {
            syMake(&(yyval.lv),(yyvsp[(1) - (1)].name));
          ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 464 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), COLONCOLON, &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 468 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), '.', &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 472 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'(')) YYERROR;
          ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 476 "grammar.y"
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
#line 490 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active (8)");
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
#line 518 "grammar.y"
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
              snprintf(tmp,MAX_INT_LEN+5,"%d",i);
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
#line 542 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[(1) - (1)].i);
            (yyval.lv).data = (yyval.lv).Data();
          ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 548 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[(1) - (1)].name);
          ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 554 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 558 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 42:

/* Line 1464 of yacc.c  */
#line 562 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 566 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 570 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 574 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 578 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 582 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 586 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 590 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 594 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 598 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 602 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 606 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 610 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 614 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 618 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 622 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 626 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 630 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 634 "grammar.y"
    {
            int b=iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i)); // handle branchTo
            if (b==TRUE) YYERROR;
            if (b==2) YYACCEPT;
          ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 640 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 644 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 648 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 652 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),RING_CMD)) YYERROR;
          ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 656 "grammar.y"
    {
            if (iiARROW(&(yyval.lv),(yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].name))) YYERROR;
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 661 "grammar.y"
    {
            if (iiARROWparams(&(yyval.lv),(yyvsp[(1) - (3)].name),(yyvsp[(3) - (3)].name))) YYERROR;
            omFree((ADDRESS)(yyvsp[(1) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 666 "grammar.y"
    { (yyval.lv) = (yyvsp[(2) - (3)].lv); ;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 671 "grammar.y"
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

  case 72:

/* Line 1464 of yacc.c  */
#line 687 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[(1) - (6)].lv),&(yyvsp[(3) - (6)].lv),&(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 691 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'[',&(yyvsp[(3) - (4)].lv))) YYERROR;
          ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 695 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 699 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 703 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 707 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 711 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 715 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), 0, &(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 80:

/* Line 1464 of yacc.c  */
#line 719 "grammar.y"
    {
            (yyval.lv)=(yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 723 "grammar.y"
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

  case 82:

/* Line 1464 of yacc.c  */
#line 740 "grammar.y"
    {
            iiTestAssume(&(yyvsp[(2) - (5)].lv),&(yyvsp[(4) - (5)].lv));
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp=NONE;
          ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 746 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 752 "grammar.y"
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

  case 85:

/* Line 1464 of yacc.c  */
#line 764 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 772 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 780 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 789 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),PLUSPLUS)) YYERROR;
          ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 793 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),MINUSMINUS)) YYERROR;
          ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 797 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'+',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 801 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'-',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 805 "grammar.y"
    { /* also for *,% */
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 809 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'^',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 813 "grammar.y"
    { /* also for > */
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 817 "grammar.y"
    { /* also for |*/
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 821 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),NOTEQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 825 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),EQUAL_EQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 829 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),DOTDOT,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 833 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),':',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 100:

/* Line 1464 of yacc.c  */
#line 837 "grammar.y"
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

  case 101:

/* Line 1464 of yacc.c  */
#line 849 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),'-')) YYERROR;
          ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 857 "grammar.y"
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

  case 105:

/* Line 1464 of yacc.c  */
#line 877 "grammar.y"
    {
            if ((yyvsp[(2) - (3)].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[(2) - (3)].lv).CopyD(STRING_CMD);
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 889 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 894 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 899 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 109:

/* Line 1464 of yacc.c  */
#line 903 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 110:

/* Line 1464 of yacc.c  */
#line 907 "grammar.y"
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

  case 111:

/* Line 1464 of yacc.c  */
#line 944 "grammar.y"
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

  case 112:

/* Line 1464 of yacc.c  */
#line 968 "grammar.y"
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

  case 113:

/* Line 1464 of yacc.c  */
#line 989 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 116:

/* Line 1464 of yacc.c  */
#line 1002 "grammar.y"
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

  case 117:

/* Line 1464 of yacc.c  */
#line 1016 "grammar.y"
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[(1) - (1)].name));
        ;}
    break;

  case 118:

/* Line 1464 of yacc.c  */
#line 1024 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = (yyvsp[(1) - (1)].i);
            (yyval.lv).rtyp = INTVEC_CMD;
            (yyval.lv).data = (void *)iv;
          ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 1033 "grammar.y"
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

  case 121:

/* Line 1464 of yacc.c  */
#line 1079 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[(3) - (3)].lv),sizeof(sleftv));
          ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1089 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1095 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 1113 "grammar.y"
    { newFile((yyvsp[(2) - (2)].name)); omFree((yyvsp[(2) - (2)].name)); ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1118 "grammar.y"
    {
            feHelp((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 131:

/* Line 1464 of yacc.c  */
#line 1123 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 132:

/* Line 1464 of yacc.c  */
#line 1130 "grammar.y"
    {
            singular_example((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1138 "grammar.y"
    {
          if (basePack!=(yyvsp[(2) - (2)].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[(2) - (2)].lv),0,currPack)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[(2) - (2)].lv),0)) YYERROR;
        ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 1150 "grammar.y"
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

  case 135:

/* Line 1464 of yacc.c  */
#line 1167 "grammar.y"
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

  case 136:

/* Line 1464 of yacc.c  */
#line 1187 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 1191 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 1195 "grammar.y"
    {
            if ((yyvsp[(3) - (4)].i)==QRING_CMD) (yyvsp[(3) - (4)].i)=RING_CMD;
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 1200 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 1204 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 1208 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
           ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 1212 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 1216 "grammar.y"
    {
            list_cmd(0,(yyvsp[(3) - (4)].lv).Fullname(),"// ",TRUE);
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1221 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 1227 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1233 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 1239 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1245 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1251 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 150:

/* Line 1464 of yacc.c  */
#line 1257 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1269 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 152:

/* Line 1464 of yacc.c  */
#line 1275 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 153:

/* Line 1464 of yacc.c  */
#line 1284 "grammar.y"
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

  case 154:

/* Line 1464 of yacc.c  */
#line 1317 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (2)].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[(2) - (2)].lv).CleanUp();
          ;}
    break;

  case 155:

/* Line 1464 of yacc.c  */
#line 1324 "grammar.y"
    {
            yyInRingConstruction = FALSE;
            if (iiAssignCR(&(yyvsp[(2) - (4)].lv),&(yyvsp[(4) - (4)].lv))) YYERROR;
          ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1329 "grammar.y"
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

  case 157:

/* Line 1464 of yacc.c  */
#line 1342 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)!=LIB_CMD)||(jjLOAD((yyvsp[(2) - (2)].name),TRUE))) YYERROR;
            omFree((yyvsp[(2) - (2)].name));
          ;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 1352 "grammar.y"
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

  case 161:

/* Line 1464 of yacc.c  */
#line 1420 "grammar.y"
    {
            type_cmd(&((yyvsp[(2) - (2)].lv)));
          ;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 1424 "grammar.y"
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

  case 163:

/* Line 1464 of yacc.c  */
#line 1453 "grammar.y"
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

  case 164:

/* Line 1464 of yacc.c  */
#line 1466 "grammar.y"
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

  case 165:

/* Line 1464 of yacc.c  */
#line 1483 "grammar.y"
    {
            int i; TESTSETINT((yyvsp[(3) - (5)].lv),i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 166:

/* Line 1464 of yacc.c  */
#line 1492 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 167:

/* Line 1464 of yacc.c  */
#line 1497 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 1505 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
	    size_t len= strlen((yyvsp[(2) - (3)].name)) + strlen((yyvsp[(3) - (3)].name)) + 36;
            char * s = (char *)omAlloc( len);
            snprintf(s,len,"whileif (!(%s)) break;\n%scontinue;\n " ,(yyvsp[(2) - (3)].name),(yyvsp[(3) - (3)].name));
            newBuffer(s,BT_break);
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 1518 "grammar.y"
    {
            /* $2 */
            /* if (!$3) break; $5; $4; continue; */
            size_t len= strlen((yyvsp[(3) - (5)].name))+strlen((yyvsp[(4) - (5)].name))+strlen((yyvsp[(5) - (5)].name))+36;
	    char *s=(char*)omAlloc(len);
            snprintf(s,len,"forif (!(%s)) break;\n%s%s;\ncontinue;\n "
                   ,(yyvsp[(3) - (5)].name),(yyvsp[(5) - (5)].name),(yyvsp[(4) - (5)].name));
            omFree((ADDRESS)(yyvsp[(3) - (5)].name));
            omFree((ADDRESS)(yyvsp[(4) - (5)].name));
            omFree((ADDRESS)(yyvsp[(5) - (5)].name));
            newBuffer(s,BT_break);
	    len=strlen((yyvsp[(2) - (5)].name)) + 3;
            s = (char *)omAlloc( len);
            snprintf(s,len,"%s;\n",(yyvsp[(2) - (5)].name));
            omFree((ADDRESS)(yyvsp[(2) - (5)].name));
            newBuffer(s,BT_if);
          ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1539 "grammar.y"
    {
            idhdl h = enterid((yyvsp[(2) - (3)].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL) {omFree((ADDRESS)(yyvsp[(2) - (3)].name));omFree((ADDRESS)(yyvsp[(3) - (3)].name)); YYERROR;}
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[(2) - (3)].name), 0, 0);
            int l=strlen((yyvsp[(3) - (3)].name))+31;IDPROC(h)->data.s.body = (char *)omAlloc(l);
            snprintf(IDPROC(h)->data.s.body,l,"parameter list #;\n%s;return();\n\n",(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 171:

/* Line 1464 of yacc.c  */
#line 1549 "grammar.y"
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
            int l=strlen((yyvsp[(3) - (3)].name))+strlen(args)+14;IDPROC(h)->data.s.body = (char *)omAlloc(l);
            snprintf(IDPROC(h)->data.s.body,l,"%s\n%s;return();\n\n",args,(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
            omFree((ADDRESS)(yyvsp[(1) - (3)].name));
          ;}
    break;

  case 172:

/* Line 1464 of yacc.c  */
#line 1568 "grammar.y"
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
            int l=strlen((yyvsp[(4) - (4)].name))+strlen(args)+14;IDPROC(h)->data.s.body = (char *)omAlloc(l);
            snprintf(IDPROC(h)->data.s.body,l,"%s\n%s;return();\n\n",args,(yyvsp[(4) - (4)].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[(4) - (4)].name));
          ;}
    break;

  case 173:

/* Line 1464 of yacc.c  */
#line 1591 "grammar.y"
    {
            // decl. of type proc p(int i)
            if ((yyvsp[(1) - (2)].i)==PARAMETER)  { if (iiParameter(&(yyvsp[(2) - (2)].lv))) YYERROR; }
            else                { if (iiAlias(&(yyvsp[(2) - (2)].lv))) YYERROR; }
          ;}
    break;

  case 174:

/* Line 1464 of yacc.c  */
#line 1597 "grammar.y"
    {
            // decl. of type proc p(i)
            sleftv tmp_expr;
            if ((yyvsp[(1) - (2)].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[(2) - (2)].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 175:

/* Line 1464 of yacc.c  */
#line 1609 "grammar.y"
    {
            iiSetReturn(&(yyvsp[(3) - (4)].lv));
            (yyvsp[(3) - (4)].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 176:

/* Line 1464 of yacc.c  */
#line 1615 "grammar.y"
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
#line 4274 "grammar.cc"
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



