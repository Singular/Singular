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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif /* HAVE_CONFIG_H */
#include <misc/auxiliary.h>

#include <kernel/mod2.h>
#include <misc/mylimits.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <misc/options.h>
#include <Singular/stype.h>
#include <Singular/fehelp.h>
#include <Singular/ipid.h>
#include <misc/intvec.h>
#include <kernel/febase.h>
#include <polys/matpol.h>
#include <polys/monomials/ring.h>
#include <kernel/kstd1.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>
#include <Singular/sdb.h>
#include <kernel/ideals.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/stairc.h>
#include <kernel/timer.h>
#include <Singular/cntrlc.h>
#include <polys/monomials/maps.h>
#include <kernel/syz.h>
#include <Singular/lists.h>
#include <libpolys/coeffs/longrat.h>
#include <Singular/libparse.h>
#include <coeffs/bigintmat.h>

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
#ifdef HAVE_FACTORY
  // libfac:
#ifdef HAVE_LIBFAC
  extern int libfac_interruptflag;
  libfac_interruptflag=0;
#endif // #ifdef HAVE_LIBFAC
#endif
}



/* Line 189 of yacc.c  */
#line 252 "grammar.cc"

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
     GRING_CMD = 267,
     BIGINTMAT_CMD = 268,
     INTMAT_CMD = 269,
     PROC_CMD = 270,
     RING_CMD = 271,
     BEGIN_RING = 272,
     IDEAL_CMD = 273,
     MAP_CMD = 274,
     MATRIX_CMD = 275,
     MODUL_CMD = 276,
     NUMBER_CMD = 277,
     POLY_CMD = 278,
     RESOLUTION_CMD = 279,
     VECTOR_CMD = 280,
     BETTI_CMD = 281,
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
     SBA_CMD = 329,
     SIMPLIFY_CMD = 330,
     SORTVEC_CMD = 331,
     SRES_CMD = 332,
     STD_CMD = 333,
     SUBST_CMD = 334,
     SYZYGY_CMD = 335,
     VAR_CMD = 336,
     VDIM_CMD = 337,
     WEDGE_CMD = 338,
     WEIGHT_CMD = 339,
     VALTVARS = 340,
     VMAXDEG = 341,
     VMAXMULT = 342,
     VNOETHER = 343,
     VMINPOLY = 344,
     END_RING = 345,
     CMD_1 = 346,
     CMD_2 = 347,
     CMD_3 = 348,
     CMD_12 = 349,
     CMD_13 = 350,
     CMD_23 = 351,
     CMD_123 = 352,
     CMD_M = 353,
     ROOT_DECL = 354,
     ROOT_DECL_LIST = 355,
     RING_DECL = 356,
     RING_DECL_LIST = 357,
     EXAMPLE_CMD = 358,
     EXPORT_CMD = 359,
     HELP_CMD = 360,
     KILL_CMD = 361,
     LIB_CMD = 362,
     LISTVAR_CMD = 363,
     SETRING_CMD = 364,
     TYPE_CMD = 365,
     STRINGTOK = 366,
     BLOCKTOK = 367,
     INT_CONST = 368,
     UNKNOWN_IDENT = 369,
     RINGVAR = 370,
     PROC_DEF = 371,
     APPLY = 372,
     BREAK_CMD = 373,
     CONTINUE_CMD = 374,
     ELSE_CMD = 375,
     EVAL = 376,
     QUOTE = 377,
     FOR_CMD = 378,
     IF_CMD = 379,
     SYS_BREAK = 380,
     WHILE_CMD = 381,
     RETURN = 382,
     PARAMETER = 383,
     SYSVAR = 384,
     UMINUS = 385
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif


/* Copy the second part of user declarations.  */


/* Line 264 of yacc.c  */
#line 423 "grammar.cc"

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
#define YYLAST   1934

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  149
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  44
/* YYNRULES -- Number of rules.  */
#define YYNRULES  170
/* YYNRULES -- Number of states.  */
#define YYNSTATES  371

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   385

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   141,     2,
     145,   146,   143,   133,   139,   134,   147,   135,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,   142,   140,
     131,   130,   132,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   136,     2,   137,   138,     2,   148,     2,     2,     2,
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
     125,   126,   127,   128,   129,   144
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
     136,   140,   145,   150,   154,   159,   164,   169,   174,   181,
     188,   195,   202,   211,   220,   229,   238,   242,   247,   256,
     261,   270,   275,   282,   289,   296,   303,   310,   317,   321,
     327,   328,   334,   337,   339,   342,   345,   349,   353,   357,
     361,   365,   369,   373,   377,   381,   385,   388,   391,   394,
     397,   399,   403,   406,   409,   412,   415,   424,   427,   431,
     434,   436,   438,   444,   446,   448,   453,   455,   459,   461,
     465,   467,   469,   471,   473,   474,   479,   483,   486,   490,
     493,   496,   500,   505,   510,   515,   520,   525,   530,   535,
     540,   547,   554,   561,   568,   575,   582,   589,   593,   595,
     604,   607,   610,   612,   614,   617,   620,   622,   628,   631,
     637,   639,   641,   645,   651,   655,   659,   664,   667,   670,
     675
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     150,     0,    -1,    -1,   150,   151,    -1,   152,    -1,   154,
     140,    -1,   165,   140,    -1,   192,    -1,   125,    -1,   140,
      -1,     1,   140,    -1,   187,    -1,   188,    -1,   153,    -1,
     189,    -1,   190,    -1,   174,    -1,   176,    -1,   177,    -1,
     103,   112,    -1,   155,    -1,   178,    -1,   179,    -1,   180,
      -1,   191,    -1,   182,    -1,   183,    -1,   185,    -1,   186,
      -1,   163,   157,    -1,   115,    -1,   164,    -1,   156,    11,
     156,    -1,   156,   147,   156,    -1,   156,   145,   146,    -1,
     156,   145,   157,   146,    -1,   136,   157,   137,    -1,   113,
      -1,   129,    -1,   166,    -1,    15,   145,   158,   146,    -1,
     157,   139,   158,    -1,   158,    -1,   162,    -1,   156,    -1,
     145,   157,   146,    -1,   158,   136,   158,   139,   158,   137,
      -1,   158,   136,   158,   137,    -1,    99,   145,   158,   146,
      -1,   100,   145,   157,   146,    -1,   100,   145,   146,    -1,
     101,   145,   158,   146,    -1,   102,   145,   157,   146,    -1,
     102,   145,   146,    -1,    91,   145,   158,   146,    -1,    94,
     145,   158,   146,    -1,    95,   145,   158,   146,    -1,    97,
     145,   158,   146,    -1,    92,   145,   158,   139,   158,   146,
      -1,    94,   145,   158,   139,   158,   146,    -1,    96,   145,
     158,   139,   158,   146,    -1,    97,   145,   158,   139,   158,
     146,    -1,    93,   145,   158,   139,   158,   139,   158,   146,
      -1,    95,   145,   158,   139,   158,   139,   158,   146,    -1,
      96,   145,   158,   139,   158,   139,   158,   146,    -1,    97,
     145,   158,   139,   158,   139,   158,   146,    -1,    98,   145,
     146,    -1,    98,   145,   157,   146,    -1,   173,   145,   158,
     139,   158,   139,   158,   146,    -1,   173,   145,   158,   146,
      -1,    16,   145,   167,   139,   167,   139,   171,   146,    -1,
      16,   145,   158,   146,    -1,   117,   145,   158,   139,    91,
     146,    -1,   117,   145,   158,   139,    94,   146,    -1,   117,
     145,   158,   139,    95,   146,    -1,   117,   145,   158,   139,
      97,   146,    -1,   117,   145,   158,   139,    98,   146,    -1,
     117,   145,   158,   139,   158,   146,    -1,   160,   158,   161,
      -1,   160,   158,   130,   158,   161,    -1,    -1,   121,   145,
     159,   158,   146,    -1,   122,   145,    -1,   146,    -1,   158,
      10,    -1,   158,     7,    -1,   158,   133,   158,    -1,   158,
     134,   158,    -1,   158,   135,   158,    -1,   158,   138,   158,
      -1,   158,   131,   158,    -1,   158,   141,   158,    -1,   158,
       9,   158,    -1,   158,     4,   158,    -1,   158,     3,   158,
      -1,   158,   142,   158,    -1,     8,   158,    -1,   134,   158,
      -1,   165,   172,    -1,   157,   130,    -1,   114,    -1,   148,
     158,   148,    -1,    99,   156,    -1,   100,   156,    -1,   101,
     156,    -1,   102,   156,    -1,   173,   156,   136,   158,   137,
     136,   158,   137,    -1,   173,   156,    -1,   165,   139,   156,
      -1,    15,   156,    -1,   111,    -1,   158,    -1,   145,   158,
     139,   157,   146,    -1,   114,    -1,   168,    -1,   168,   145,
     157,   146,    -1,   169,    -1,   169,   139,   170,    -1,   169,
      -1,   145,   170,   146,    -1,   130,    -1,    20,    -1,    14,
      -1,    13,    -1,    -1,   131,   166,   175,   140,    -1,   105,
     111,   140,    -1,   105,   140,    -1,   103,   111,   140,    -1,
     104,   157,    -1,   106,   156,    -1,   179,   139,   156,    -1,
     108,   145,    99,   146,    -1,   108,   145,   100,   146,    -1,
     108,   145,   101,   146,    -1,   108,   145,   102,   146,    -1,
     108,   145,    16,   146,    -1,   108,   145,   173,   146,    -1,
     108,   145,    15,   146,    -1,   108,   145,   156,   146,    -1,
     108,   145,   156,   139,    99,   146,    -1,   108,   145,   156,
     139,   100,   146,    -1,   108,   145,   156,   139,   101,   146,
      -1,   108,   145,   156,   139,   102,   146,    -1,   108,   145,
     156,   139,    16,   146,    -1,   108,   145,   156,   139,   173,
     146,    -1,   108,   145,   156,   139,    15,   146,    -1,   108,
     145,   146,    -1,    16,    -1,   181,   156,   172,   167,   139,
     167,   139,   171,    -1,   181,   156,    -1,   129,   166,    -1,
     109,    -1,    40,    -1,   184,   158,    -1,   110,   158,    -1,
     157,    -1,   124,   145,   158,   146,   112,    -1,   120,   112,
      -1,   124,   145,   158,   146,   118,    -1,   118,    -1,   119,
      -1,   126,   111,   112,    -1,   123,   111,   111,   111,   112,
      -1,    15,   164,   112,    -1,   116,   111,   112,    -1,   116,
     111,   111,   112,    -1,   128,   165,    -1,   128,   158,    -1,
     127,   145,   157,   146,    -1,   127,   145,   146,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   370,   370,   372,   406,   407,   409,   411,   415,   420,
     422,   473,   474,   475,   476,   477,   478,   479,   480,   484,
     487,   488,   489,   490,   491,   492,   493,   494,   495,   498,
     505,   510,   514,   518,   522,   526,   539,   567,   591,   597,
     603,   610,   621,   627,   632,   633,   634,   638,   642,   646,
     650,   654,   658,   662,   666,   670,   674,   678,   682,   686,
     690,   694,   698,   702,   706,   710,   714,   718,   722,   726,
     730,   734,   738,   742,   746,   750,   754,   758,   762,   766,
     784,   783,   801,   809,   818,   822,   826,   830,   834,   838,
     842,   846,   850,   854,   858,   862,   866,   873,   880,   881,
     900,   901,   913,   918,   923,   927,   931,   971,   997,  1018,
    1026,  1030,  1031,  1045,  1053,  1062,  1107,  1108,  1117,  1118,
    1124,  1131,  1133,  1135,  1145,  1144,  1152,  1157,  1164,  1172,
    1184,  1200,  1219,  1223,  1227,  1232,  1236,  1240,  1244,  1248,
    1253,  1259,  1265,  1271,  1277,  1283,  1289,  1301,  1308,  1312,
    1349,  1359,  1365,  1365,  1368,  1440,  1444,  1473,  1486,  1503,
    1512,  1517,  1525,  1537,  1556,  1566,  1585,  1608,  1614,  1626,
    1632
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "GRING_CMD",
  "BIGINTMAT_CMD", "INTMAT_CMD", "PROC_CMD", "RING_CMD", "BEGIN_RING",
  "IDEAL_CMD", "MAP_CMD", "MATRIX_CMD", "MODUL_CMD", "NUMBER_CMD",
  "POLY_CMD", "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD", "COEFFS_CMD",
  "COEF_CMD", "CONTRACT_CMD", "DEGREE_CMD", "DEG_CMD", "DIFF_CMD",
  "DIM_CMD", "DIVISION_CMD", "ELIMINATION_CMD", "E_CMD", "FAREY_CMD",
  "FETCH_CMD", "FREEMODULE_CMD", "KEEPRING_CMD", "HILBERT_CMD",
  "HOMOG_CMD", "IMAP_CMD", "INDEPSET_CMD", "INTERRED_CMD", "INTERSECT_CMD",
  "JACOB_CMD", "JET_CMD", "KBASE_CMD", "KOSZUL_CMD", "LEADCOEF_CMD",
  "LEADEXP_CMD", "LEAD_CMD", "LEADMONOM_CMD", "LIFTSTD_CMD", "LIFT_CMD",
  "MAXID_CMD", "MINBASE_CMD", "MINOR_CMD", "MINRES_CMD", "MODULO_CMD",
  "MONOM_CMD", "MRES_CMD", "MULTIPLICITY_CMD", "ORD_CMD", "PAR_CMD",
  "PARDEG_CMD", "PREIMAGE_CMD", "QUOTIENT_CMD", "QHWEIGHT_CMD",
  "REDUCE_CMD", "REGULARITY_CMD", "RES_CMD", "SBA_CMD", "SIMPLIFY_CMD",
  "SORTVEC_CMD", "SRES_CMD", "STD_CMD", "SUBST_CMD", "SYZYGY_CMD",
  "VAR_CMD", "VDIM_CMD", "WEDGE_CMD", "WEIGHT_CMD", "VALTVARS", "VMAXDEG",
  "VMAXMULT", "VNOETHER", "VMINPOLY", "END_RING", "CMD_1", "CMD_2",
  "CMD_3", "CMD_12", "CMD_13", "CMD_23", "CMD_123", "CMD_M", "ROOT_DECL",
  "ROOT_DECL_LIST", "RING_DECL", "RING_DECL_LIST", "EXAMPLE_CMD",
  "EXPORT_CMD", "HELP_CMD", "KILL_CMD", "LIB_CMD", "LISTVAR_CMD",
  "SETRING_CMD", "TYPE_CMD", "STRINGTOK", "BLOCKTOK", "INT_CONST",
  "UNKNOWN_IDENT", "RINGVAR", "PROC_DEF", "APPLY", "BREAK_CMD",
  "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE", "FOR_CMD", "IF_CMD",
  "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER", "SYSVAR", "'='", "'<'",
  "'>'", "'+'", "'-'", "'/'", "'['", "']'", "'^'", "','", "';'", "'&'",
  "':'", "'*'", "UMINUS", "'('", "')'", "'.'", "'`'", "$accept", "lines",
  "pprompt", "flowctrl", "example_dummy", "command", "assign", "elemexpr",
  "exprlist", "expr", "$@1", "quote_start", "quote_end", "expr_arithmetic",
  "left_value", "extendedid", "declare_ip_variable", "stringexpr", "rlist",
  "ordername", "orderelem", "OrderingList", "ordering", "cmdeq", "mat_cmd",
  "filecmd", "$@2", "helpcmd", "examplecmd", "exportcmd", "killcmd",
  "listcmd", "ringcmd1", "ringcmd", "scriptcmd", "setrings", "setringcmd",
  "typecmd", "ifcmd", "whilecmd", "forcmd", "proccmd", "parametercmd",
  "returncmd", 0
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
     375,   376,   377,   378,   379,   380,   381,   382,   383,   384,
      61,    60,    62,    43,    45,    47,    91,    93,    94,    44,
      59,    38,    58,    42,   385,    40,    41,    46,    96
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   149,   150,   150,   151,   151,   151,   151,   151,   151,
     151,   152,   152,   152,   152,   152,   152,   152,   152,   153,
     154,   154,   154,   154,   154,   154,   154,   154,   154,   155,
     156,   156,   156,   156,   156,   156,   156,   156,   156,   156,
     156,   157,   157,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     159,   158,   160,   161,   162,   162,   162,   162,   162,   162,
     162,   162,   162,   162,   162,   162,   162,   162,   163,   163,
     164,   164,   165,   165,   165,   165,   165,   165,   165,   165,
     166,   167,   167,   168,   169,   169,   170,   170,   171,   171,
     172,   173,   173,   173,   175,   174,   176,   176,   177,   178,
     179,   179,   180,   180,   180,   180,   180,   180,   180,   180,
     180,   180,   180,   180,   180,   180,   180,   180,   181,   182,
     182,   183,   184,   184,   185,   186,   186,   187,   187,   187,
     187,   187,   188,   189,   190,   190,   190,   191,   191,   192,
     192
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       4,     3,     1,     1,     1,     3,     6,     4,     4,     4,
       3,     4,     4,     3,     4,     4,     4,     4,     6,     6,
       6,     6,     8,     8,     8,     8,     3,     4,     8,     4,
       8,     4,     6,     6,     6,     6,     6,     6,     3,     5,
       0,     5,     2,     1,     2,     2,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     2,     2,     2,     2,
       1,     3,     2,     2,     2,     2,     8,     2,     3,     2,
       1,     1,     5,     1,     1,     4,     1,     3,     1,     3,
       1,     1,     1,     1,     0,     4,     3,     2,     3,     2,
       2,     3,     4,     4,     4,     4,     4,     4,     4,     4,
       6,     6,     6,     6,     6,     6,     6,     3,     1,     8,
       2,     2,     1,     1,     2,     2,     1,     5,     2,     5,
       1,     1,     3,     5,     3,     3,     4,     2,     2,     4,
       3
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,   123,   122,     0,   148,   121,
     153,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   152,     0,
     110,    37,   100,    30,     0,     0,   160,   161,     0,     0,
       0,     0,     0,     8,     0,     0,     0,    38,     0,     0,
       0,     9,     0,     0,     3,     4,    13,     0,    20,    44,
     156,    42,     0,    43,     0,    31,     0,    39,     0,    16,
      17,    18,    21,    22,    23,     0,    25,    26,     0,    27,
      28,    11,    12,    14,    15,    24,     7,    10,     0,     0,
       0,     0,     0,     0,    38,    96,     0,     0,   109,    31,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     102,     0,   103,     0,   104,     0,   105,     0,    19,   129,
       0,   127,   130,     0,   155,     0,     0,   158,    80,    82,
       0,     0,     0,     0,     0,   168,   167,   151,   124,    97,
       0,     0,     0,     5,     0,     0,     0,    99,     0,     0,
       0,    85,     0,    84,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    29,   120,     0,     6,    98,     0,   107,
       0,   150,   154,     0,   164,     0,   111,     0,     0,     0,
       0,     0,     0,     0,     0,    66,     0,     0,    50,     0,
       0,    53,     0,   128,   126,     0,     0,     0,     0,     0,
       0,   147,     0,     0,     0,   165,     0,     0,     0,     0,
     162,   170,     0,     0,    36,    45,   101,    32,    34,     0,
      33,    41,    94,    93,    92,    90,    86,    87,    88,     0,
      89,    91,    95,     0,    83,    78,   108,     0,     0,   131,
       0,    40,    42,    71,     0,    54,     0,     0,     0,    55,
       0,    56,     0,     0,    57,    67,    48,    49,    51,    52,
     138,   136,   132,   133,   134,   135,     0,   139,   137,   166,
       0,     0,     0,     0,   169,   125,    35,    47,     0,     0,
       0,    69,     0,   111,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    81,   163,   157,   159,
       0,    79,     0,     0,     0,     0,     0,    58,     0,    59,
       0,     0,    60,     0,    61,   146,   144,   140,   141,   142,
     143,   145,    72,    73,    74,    75,    76,    77,    46,     0,
       0,     0,   112,   113,     0,   114,   118,     0,     0,     0,
       0,     0,     0,     0,     0,   116,     0,     0,    70,    62,
      63,    64,    65,    68,   106,   149,     0,   119,     0,   117,
     115
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    54,    55,    56,    57,    58,    59,   141,    61,
     207,    62,   235,    63,    64,    65,    66,    67,   177,   345,
     346,   356,   347,   167,    96,    69,   213,    70,    71,    72,
      73,    74,    75,    76,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -329
static const yytype_int16 yypact[] =
{
    -329,   274,  -329,  -101,  1312,  -329,  -329,   389,  -103,  -329,
    -329,   -78,   -75,   -57,   -53,   -47,   -46,   -44,   -41,   812,
     858,  1740,  1781,   -87,  1312,  -108,   333,   -40,  -329,  1312,
    -329,  -329,  -329,  -329,   -54,   -39,  -329,  -329,    -4,   -36,
     -34,   -52,   -32,  -329,    18,   -19,  1370,    19,    19,  1312,
    1312,  -329,  1312,  1312,  -329,  -329,  -329,     5,  -329,     7,
    -119,  1693,  1312,  -329,  1312,  -329,  -113,  -329,  1786,  -329,
    -329,  -329,  -329,    -8,  -329,   333,  -329,  -329,  1312,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,    -6,  -103,
       2,     8,    10,    16,  -329,    -3,    17,  1312,     7,    54,
    1429,  1312,  1312,  1312,  1312,  1312,  1312,  1312,   968,  1312,
       7,  1078,     7,  1312,     7,  1136,     7,    28,  -329,    38,
      40,  -329,     7,  1757,  1693,   -38,  1312,  -329,  -329,  -329,
      67,  1312,    69,  1195,   389,  1693,    43,  -329,  -329,    -3,
    -125,  -117,    62,  -329,   333,  1253,   333,  -329,  1312,  1312,
    1312,  -329,  1312,  -329,  1312,  1312,  1312,  1312,  1312,  1312,
    1312,  1312,    93,    38,  -329,   333,  -329,  -329,  1312,    -9,
     333,    -5,  1693,   461,  -329,  1312,   475,    48,   489,   998,
    1024,   182,   211,  1507,   293,  -329,  -109,   503,  -329,   -96,
     517,  -329,   -94,  -329,  -329,   -70,    42,    53,    55,    56,
      59,  -329,     4,    60,    82,  -329,  1529,  1312,    96,   548,
    -329,  -329,   -92,    68,  -329,  -329,  -329,    64,  -329,   -91,
    -329,  1693,  1705,   538,   538,   353,    21,    21,    -3,   986,
      -2,   942,    21,  1312,  -329,  -329,     7,   408,  1312,     7,
    1429,  -329,  1545,  -329,  1429,  -329,  1312,  1312,  1312,  -329,
    1312,  -329,  1312,  1312,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,   460,  -329,  -329,  -329,
    1487,   657,    97,   -56,  -329,  -329,  -329,  -329,  1312,   688,
    1312,  -329,  1557,  1693,    73,  1312,    74,   702,  1587,   716,
    1603,   433,   447,    70,    71,    79,    84,    86,    87,    90,
     -67,   -65,   -61,   -59,   -55,   730,  -329,  -329,  -329,  -329,
    1615,  -329,  1627,    83,  1429,   -88,  -104,  -329,  1312,  -329,
    1312,  1312,  -329,  1312,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  1312,
    1312,    98,  -329,  -329,   124,    95,  -329,    99,   744,   758,
     772,   797,   912,  1647,  -104,   102,   100,  1312,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,   124,  -329,   -86,  -329,
    -329
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -329,  -329,  -329,  -329,  -329,  -329,  -329,    14,    -1,    15,
    -329,  -329,   -37,  -329,  -329,   236,   198,    47,  -231,  -329,
    -328,  -116,  -107,    78,     0,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,  -329,
    -329,  -329,  -329,  -329
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -1
static const yytype_int16 yytable[] =
{
      60,    68,   144,   120,   151,   151,   144,   153,   153,   284,
     343,   147,   214,   286,   148,   144,   355,   164,   144,    95,
     148,    98,   148,   119,   117,   118,   165,   166,   151,   215,
     148,   153,   121,   110,   112,   114,   116,   255,   355,    87,
     122,   344,   100,   148,   124,   148,    68,   148,   148,   140,
     257,   148,   259,   148,   274,   276,   308,   125,   342,   130,
     370,   135,   309,   163,   139,   149,   150,   101,   142,   151,
     102,   152,   153,   204,   205,    97,   260,   162,   101,   332,
     104,   333,   169,   341,   105,   334,   107,   335,   103,   171,
     108,   336,   104,   172,   137,   138,   149,   150,   105,   106,
     151,   107,   152,   153,   108,   123,   126,   186,   127,   128,
     189,   129,   173,   131,   192,   176,   178,   179,   180,   181,
     182,   183,   184,   203,   187,   164,   133,   238,   190,   132,
      30,   170,   212,   158,   158,   159,   145,   202,   146,    97,
     145,   206,   146,   266,   219,   143,   209,   109,    98,   145,
     267,   146,   145,   111,   146,   113,   157,   158,   217,   159,
     220,   115,   168,   221,   222,   223,   174,   224,   193,   225,
     226,   227,   228,   229,   230,   231,   232,   148,   208,   236,
     194,   210,   165,   237,   239,   149,   150,   244,   261,   151,
     242,   152,   153,   154,   269,   155,   156,   157,   158,   262,
     159,   263,   264,   160,   161,   265,   268,   272,   275,   307,
     216,   146,   314,   316,   149,   150,   325,   326,   151,   340,
     152,   153,   271,   233,   154,   327,   155,   156,   157,   158,
     328,   159,   329,   330,   160,   161,   331,   354,   343,   234,
     357,   366,   311,    99,   136,   358,   367,   365,   279,   240,
     369,     0,     0,   282,     0,   283,     0,     0,     0,   283,
       0,   287,   288,   289,     0,   290,   299,   291,   292,     0,
       0,     0,     0,     0,     2,     3,     0,     0,     0,     0,
       0,     0,     4,     0,   315,   305,     0,     5,     6,     7,
       8,     0,     0,   310,     9,   312,   149,   150,     0,     0,
     151,     0,   152,   153,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   154,    10,   155,   156,   157,   158,     0,
     159,   248,     0,   160,   161,     0,     0,     0,   249,   283,
       0,     0,     0,   348,     0,   349,   350,     0,   351,     0,
       0,     0,   154,     0,   155,   156,   157,   158,    88,   159,
     250,     0,   160,   161,   352,   353,   368,   251,     0,     0,
     151,     0,     0,   153,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
      26,     0,    27,    28,    29,    30,     0,    31,    32,    33,
      34,    35,    36,    37,    38,    39,    40,    41,    42,    43,
      44,    45,    46,    47,    88,    48,     0,     0,    49,     0,
      50,   149,   150,     0,    51,   151,     0,   152,   153,    52,
       0,     0,    53,     0,   154,     0,   155,   156,   157,   158,
       0,   159,   253,     0,   160,   161,   149,   150,     0,   254,
     151,     0,   152,   153,    30,     0,    31,    32,    33,     0,
     149,   150,     0,     0,   151,     0,   152,   153,     0,     0,
       0,     0,    94,     0,   149,   150,     0,     0,   151,    50,
     152,   153,     0,     5,     6,   293,   294,     0,   149,   150,
       9,    53,   151,     0,   152,   153,   155,   156,   157,   158,
       0,   159,   149,   150,     0,   161,   151,     0,   152,   153,
      30,     0,    31,    32,    33,     0,   149,   150,     0,     0,
     151,     0,   152,   153,     0,     0,     0,     0,    94,     0,
     149,   150,     0,     0,   151,    50,   152,   153,     0,     0,
       0,     0,     0,     0,    97,     0,     0,    53,     0,   154,
       0,   155,   156,   157,   158,   151,   159,   280,   153,   160,
     161,   149,   150,     0,   281,   151,     0,   152,   153,   295,
     296,   297,   298,     0,   154,     0,   155,   156,   157,   158,
       0,   159,   321,     0,   160,   161,     0,     0,   154,   322,
     155,   156,   157,   158,     0,   159,   323,     0,   160,   161,
       0,     0,   154,   324,   155,   156,   157,   158,     0,   159,
       0,     0,   160,   161,     0,     0,   154,   241,   155,   156,
     157,   158,     0,   159,     0,     0,   160,   161,     0,     0,
     154,   243,   155,   156,   157,   158,     0,   159,     0,     0,
     160,   161,     0,     0,   154,   245,   155,   156,   157,   158,
       0,   159,     0,     0,   160,   161,     0,     0,   154,   256,
     155,   156,   157,   158,     0,   159,     0,     0,   160,   161,
     149,   150,     0,   258,   151,     0,   152,   153,     0,   154,
       0,   155,   156,   157,   158,     0,   159,     0,     0,   154,
     161,   155,   156,   157,   158,     0,   159,     0,     0,   160,
     161,   149,   150,     0,   273,   151,     0,   152,   153,     0,
       0,     0,     0,     0,     0,   149,   150,     0,     0,   151,
       0,   152,   153,     0,     0,     0,     0,     0,     0,   149,
     150,     0,     0,   151,     0,   152,   153,     0,     0,     0,
       0,     0,     0,   149,   150,     0,     0,   151,     0,   152,
     153,     0,     0,     0,     0,     0,     0,   149,   150,     0,
       0,   151,     0,   152,   153,     0,     0,     0,     0,     0,
       0,   149,   150,     0,     0,   151,     0,   152,   153,     0,
       0,     0,     0,     0,     0,   149,   150,     0,     0,   151,
       0,   152,   153,     0,     0,     0,     0,     0,   154,     0,
     155,   156,   157,   158,     0,   159,     0,     0,   160,   161,
     149,   150,     0,   306,   151,     0,   152,   153,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   154,
       0,   155,   156,   157,   158,     0,   159,    88,     0,   160,
     161,     0,     0,   154,   234,   155,   156,   157,   158,     0,
     159,     0,     0,   160,   161,     0,     0,   154,   317,   155,
     156,   157,   158,     0,   159,     0,     0,   160,   161,     0,
       0,   154,   319,   155,   156,   157,   158,     0,   159,     0,
       0,   160,   161,    88,     0,   154,   337,   155,   156,   157,
     158,     0,   159,     0,     0,   160,   161,     0,     0,   154,
     359,   155,   156,   157,   158,     0,   159,     0,     0,   160,
     161,     0,     0,   154,   360,   155,   156,   157,   158,     0,
     159,     0,     0,   160,   161,   149,   150,     0,   361,   151,
       0,   152,   153,    30,     0,    31,    32,    33,   154,     0,
     155,   156,   157,   158,     0,   159,     0,     0,   160,   161,
       0,    94,     0,   362,     0,     0,   150,     0,    50,   151,
       0,   152,   153,     0,     0,     0,     0,   109,     0,     0,
      53,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,    31,    32,    33,     0,     0,     4,     0,     0,     0,
       0,     5,     6,    88,    89,     0,     0,    94,     9,   149,
     150,     0,     0,   151,    50,   152,   153,     0,     0,     0,
       0,   149,   150,   111,     0,   151,    53,   152,   153,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   149,   150,     0,
       0,   151,     0,   152,   153,     0,     0,     0,     0,     0,
       0,     0,     0,   154,     0,   155,   156,   157,   158,     0,
     159,     0,     0,   160,   161,     0,     0,     0,   363,    11,
      12,    13,    14,    15,    16,    17,    18,    90,    91,    92,
      93,     0,     0,   154,     0,   155,   156,   157,   158,    30,
     159,    31,    32,    33,   161,    35,     4,     0,     0,    39,
      40,     5,     6,    88,    89,     0,     0,    94,     9,     0,
       0,     0,    49,     0,    50,     0,     0,     0,     0,     0,
       0,     0,     0,    52,   185,     0,    53,   154,     0,   155,
     156,   157,   158,   277,   159,   278,     0,   160,   161,   154,
       0,   155,   156,   157,   158,     0,   159,   246,     0,   160,
     161,     0,     0,     0,     4,     0,     0,     0,     0,     5,
       6,    88,    89,     0,     0,   154,     9,   155,   156,   157,
     158,     0,   159,   247,     0,   160,   161,     0,     0,    11,
      12,    13,    14,    15,    16,    17,    18,    90,    91,    92,
      93,     0,     0,     0,     0,     0,     0,     0,     0,    30,
       0,    31,    32,    33,     0,    35,     0,     0,     0,    39,
      40,     0,     0,     4,     0,     0,     0,    94,     5,     6,
      88,    89,    49,     0,    50,     9,     0,     0,     0,     0,
       0,     0,     0,    52,   188,     0,    53,    11,    12,    13,
      14,    15,    16,    17,    18,    90,    91,    92,    93,     0,
       0,     0,     0,     0,     0,     0,     0,    30,     0,    31,
      32,    33,     0,    35,     0,     0,     0,    39,    40,     0,
       0,     4,     0,     0,     0,    94,     5,     6,    88,    89,
      49,     0,    50,     9,     0,     0,     0,     0,     0,     0,
       0,    52,   191,     0,    53,     0,    11,    12,    13,    14,
      15,    16,    17,    18,    90,    91,    92,    93,     0,     0,
       0,     0,     0,     0,     0,     0,    30,     0,    31,    32,
      33,     0,    35,     0,     0,     0,    39,    40,     0,     0,
       4,     0,     0,     0,    94,     5,     6,    88,    89,    49,
       0,    50,     9,     0,     0,     0,     0,     0,     0,     0,
      52,   211,     0,    53,    11,    12,    13,    14,    15,    16,
      17,    18,    90,    91,    92,    93,     0,     0,     0,     0,
       0,     0,     0,     0,    30,     0,    31,    32,    33,     0,
      35,     0,     0,     0,    39,    40,     0,     0,     4,     0,
       0,     0,    94,     5,     6,   134,    89,    49,     0,    50,
       9,     0,     0,     0,     0,     0,     0,     0,    52,   218,
       0,    53,     0,    11,    12,    13,    14,    15,    16,    17,
      18,    90,    91,    92,    93,     0,     0,     0,     0,     0,
       0,     0,     0,    30,     0,    31,    32,    33,     0,    35,
       0,     0,     0,    39,    40,     0,     0,     4,     0,     0,
       0,    94,     5,     6,    88,    89,    49,     0,    50,     9,
       0,     0,     0,     0,     0,     0,     0,    52,     0,     0,
      53,    11,    12,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,     0,     0,     0,     0,     0,     0,     0,
       0,    30,     0,    31,    32,    33,     0,    35,     0,     0,
       0,    39,    40,     0,     0,     4,     0,     0,     0,    94,
       5,     6,    88,    89,    49,     0,    50,     9,     0,     0,
     149,   150,     0,     0,   151,    52,   152,   153,    53,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    90,    91,
      92,    93,   149,   150,     0,     0,   151,     0,   152,   153,
      30,     0,    31,    32,    33,     0,    35,     0,   149,   150,
      39,    40,   151,     0,   152,   153,     0,     0,    94,     0,
     149,   150,     0,    49,   151,    50,   152,   153,     0,     0,
       0,     0,     0,     0,   175,     0,     0,    53,   300,    12,
      13,   301,   302,    16,   303,   304,    90,    91,    92,    93,
     149,   150,     0,     0,   151,     0,   152,   153,    30,     0,
      31,    32,    33,     0,    35,     0,   149,   150,    39,    40,
     151,     0,   152,   153,     0,     0,    94,     0,   149,   150,
       0,    49,   151,    50,   152,   153,     0,     0,     0,     0,
     149,   150,    52,     0,   151,    53,   152,   153,   154,     0,
     155,   156,   157,   158,     0,   159,   252,     0,   160,   161,
     149,   150,     0,     0,   151,     0,   152,   153,     0,     0,
     154,     0,   155,   156,   157,   158,     0,   159,   270,     0,
     160,   161,     0,     0,     0,     0,   154,     0,   155,   156,
     157,   158,     0,   159,   285,     0,   160,   161,   154,     0,
     155,   156,   157,   158,   313,   159,   149,   150,   160,   161,
     151,     0,   152,   153,     0,     0,     0,     0,    -1,   150,
       0,     0,   151,     0,   152,   153,     0,     0,   154,     0,
     155,   156,   157,   158,     0,   159,   318,     0,   160,   161,
       0,     0,     0,     0,   154,     0,   155,   156,   157,   158,
       0,   159,   320,     0,   160,   161,   154,     0,   155,   156,
     157,   158,   338,   159,     0,    88,   160,   161,   154,     0,
     155,   156,   157,   158,     0,   159,   339,     0,   160,   161,
       5,     6,   195,   196,     0,     0,     0,     9,   154,     0,
     155,   156,   157,   158,   364,   159,     0,     0,   160,   161,
       0,     0,     0,     0,     0,     0,    88,     0,     0,     0,
       0,    88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,   154,     0,   155,   156,   157,   158,
       0,   159,     0,     0,   160,   161,   154,     0,   155,   156,
     157,   158,     0,   159,     0,     0,   160,   161,     0,     0,
       0,    30,     0,    31,    32,    33,   197,   198,   199,   200,
       0,     0,     0,     0,     0,     0,     0,     0,    30,    94,
      31,    32,    33,     0,     0,     0,    50,     0,     0,     0,
       0,     0,     0,     0,     0,   113,    94,     0,    53,     0,
       0,     0,    30,    50,    31,    32,    33,    30,     0,    31,
      32,    33,     0,   201,     0,    53,     0,     0,     0,     0,
      94,     0,     0,     0,     0,    94,     0,    50,     0,     0,
       0,     0,    50,     0,     0,     0,   115,     0,     0,    53,
       0,   168,     0,     0,    53
};

static const yytype_int16 yycheck[] =
{
       1,     1,    11,   111,     7,     7,    11,    10,    10,   240,
     114,   130,   137,   244,   139,    11,   344,   130,    11,     4,
     139,     7,   139,    24,   111,   112,   139,   140,     7,   146,
     139,    10,   140,    19,    20,    21,    22,   146,   366,   140,
      26,   145,   145,   139,    29,   139,    46,   139,   139,    50,
     146,   139,   146,   139,   146,   146,   112,   111,   146,   111,
     146,    46,   118,    64,    49,     3,     4,   145,    53,     7,
     145,     9,    10,   111,   112,   145,   146,    62,   145,   146,
     145,   146,    68,   314,   145,   146,   145,   146,   145,    75,
     145,   146,   145,    78,    47,    48,     3,     4,   145,   145,
       7,   145,     9,    10,   145,   145,   145,   108,   112,   145,
     111,   145,    97,   145,   115,   100,   101,   102,   103,   104,
     105,   106,   107,   123,   109,   130,   145,   136,   113,   111,
     111,   139,   133,   136,   136,   138,   145,   123,   147,   145,
     145,   126,   147,   139,   145,   140,   131,   145,   134,   145,
     146,   147,   145,   145,   147,   145,   135,   136,   144,   138,
     146,   145,   145,   148,   149,   150,   112,   152,   140,   154,
     155,   156,   157,   158,   159,   160,   161,   139,   111,   165,
     140,   112,   139,   168,   170,     3,     4,   139,   146,     7,
     175,     9,    10,   131,   112,   133,   134,   135,   136,   146,
     138,   146,   146,   141,   142,   146,   146,   111,   140,   112,
     148,   147,   139,   139,     3,     4,   146,   146,     7,   136,
       9,    10,   207,   130,   131,   146,   133,   134,   135,   136,
     146,   138,   146,   146,   141,   142,   146,   139,   114,   146,
     145,   139,   279,     7,    46,   146,   146,   354,   233,   171,
     366,    -1,    -1,   238,    -1,   240,    -1,    -1,    -1,   244,
      -1,   246,   247,   248,    -1,   250,   266,   252,   253,    -1,
      -1,    -1,    -1,    -1,     0,     1,    -1,    -1,    -1,    -1,
      -1,    -1,     8,    -1,   285,   270,    -1,    13,    14,    15,
      16,    -1,    -1,   278,    20,   280,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   131,    40,   133,   134,   135,   136,    -1,
     138,   139,    -1,   141,   142,    -1,    -1,    -1,   146,   314,
      -1,    -1,    -1,   318,    -1,   320,   321,    -1,   323,    -1,
      -1,    -1,   131,    -1,   133,   134,   135,   136,    15,   138,
     139,    -1,   141,   142,   339,   340,   357,   146,    -1,    -1,
       7,    -1,    -1,    10,    -1,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,   104,   105,
     106,    -1,   108,   109,   110,   111,    -1,   113,   114,   115,
     116,   117,   118,   119,   120,   121,   122,   123,   124,   125,
     126,   127,   128,   129,    15,   131,    -1,    -1,   134,    -1,
     136,     3,     4,    -1,   140,     7,    -1,     9,    10,   145,
      -1,    -1,   148,    -1,   131,    -1,   133,   134,   135,   136,
      -1,   138,   139,    -1,   141,   142,     3,     4,    -1,   146,
       7,    -1,     9,    10,   111,    -1,   113,   114,   115,    -1,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,   129,    -1,     3,     4,    -1,    -1,     7,   136,
       9,    10,    -1,    13,    14,    15,    16,    -1,     3,     4,
      20,   148,     7,    -1,     9,    10,   133,   134,   135,   136,
      -1,   138,     3,     4,    -1,   142,     7,    -1,     9,    10,
     111,    -1,   113,   114,   115,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,   129,    -1,
       3,     4,    -1,    -1,     7,   136,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,   145,    -1,    -1,   148,    -1,   131,
      -1,   133,   134,   135,   136,     7,   138,   139,    10,   141,
     142,     3,     4,    -1,   146,     7,    -1,     9,    10,    99,
     100,   101,   102,    -1,   131,    -1,   133,   134,   135,   136,
      -1,   138,   139,    -1,   141,   142,    -1,    -1,   131,   146,
     133,   134,   135,   136,    -1,   138,   139,    -1,   141,   142,
      -1,    -1,   131,   146,   133,   134,   135,   136,    -1,   138,
      -1,    -1,   141,   142,    -1,    -1,   131,   146,   133,   134,
     135,   136,    -1,   138,    -1,    -1,   141,   142,    -1,    -1,
     131,   146,   133,   134,   135,   136,    -1,   138,    -1,    -1,
     141,   142,    -1,    -1,   131,   146,   133,   134,   135,   136,
      -1,   138,    -1,    -1,   141,   142,    -1,    -1,   131,   146,
     133,   134,   135,   136,    -1,   138,    -1,    -1,   141,   142,
       3,     4,    -1,   146,     7,    -1,     9,    10,    -1,   131,
      -1,   133,   134,   135,   136,    -1,   138,    -1,    -1,   131,
     142,   133,   134,   135,   136,    -1,   138,    -1,    -1,   141,
     142,     3,     4,    -1,   146,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,     3,
       4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,   131,    -1,
     133,   134,   135,   136,    -1,   138,    -1,    -1,   141,   142,
       3,     4,    -1,   146,     7,    -1,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   131,
      -1,   133,   134,   135,   136,    -1,   138,    15,    -1,   141,
     142,    -1,    -1,   131,   146,   133,   134,   135,   136,    -1,
     138,    -1,    -1,   141,   142,    -1,    -1,   131,   146,   133,
     134,   135,   136,    -1,   138,    -1,    -1,   141,   142,    -1,
      -1,   131,   146,   133,   134,   135,   136,    -1,   138,    -1,
      -1,   141,   142,    15,    -1,   131,   146,   133,   134,   135,
     136,    -1,   138,    -1,    -1,   141,   142,    -1,    -1,   131,
     146,   133,   134,   135,   136,    -1,   138,    -1,    -1,   141,
     142,    -1,    -1,   131,   146,   133,   134,   135,   136,    -1,
     138,    -1,    -1,   141,   142,     3,     4,    -1,   146,     7,
      -1,     9,    10,   111,    -1,   113,   114,   115,   131,    -1,
     133,   134,   135,   136,    -1,   138,    -1,    -1,   141,   142,
      -1,   129,    -1,   146,    -1,    -1,     4,    -1,   136,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,   145,    -1,    -1,
     148,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
      -1,   113,   114,   115,    -1,    -1,     8,    -1,    -1,    -1,
      -1,    13,    14,    15,    16,    -1,    -1,   129,    20,     3,
       4,    -1,    -1,     7,   136,     9,    10,    -1,    -1,    -1,
      -1,     3,     4,   145,    -1,     7,   148,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   131,    -1,   133,   134,   135,   136,    -1,
     138,    -1,    -1,   141,   142,    -1,    -1,    -1,   146,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,    -1,    -1,   131,    -1,   133,   134,   135,   136,   111,
     138,   113,   114,   115,   142,   117,     8,    -1,    -1,   121,
     122,    13,    14,    15,    16,    -1,    -1,   129,    20,    -1,
      -1,    -1,   134,    -1,   136,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   145,   146,    -1,   148,   131,    -1,   133,
     134,   135,   136,   137,   138,   139,    -1,   141,   142,   131,
      -1,   133,   134,   135,   136,    -1,   138,   139,    -1,   141,
     142,    -1,    -1,    -1,     8,    -1,    -1,    -1,    -1,    13,
      14,    15,    16,    -1,    -1,   131,    20,   133,   134,   135,
     136,    -1,   138,   139,    -1,   141,   142,    -1,    -1,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,
      -1,   113,   114,   115,    -1,   117,    -1,    -1,    -1,   121,
     122,    -1,    -1,     8,    -1,    -1,    -1,   129,    13,    14,
      15,    16,   134,    -1,   136,    20,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   145,   146,    -1,   148,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,   113,
     114,   115,    -1,   117,    -1,    -1,    -1,   121,   122,    -1,
      -1,     8,    -1,    -1,    -1,   129,    13,    14,    15,    16,
     134,    -1,   136,    20,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   145,   146,    -1,   148,    -1,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   111,    -1,   113,   114,
     115,    -1,   117,    -1,    -1,    -1,   121,   122,    -1,    -1,
       8,    -1,    -1,    -1,   129,    13,    14,    15,    16,   134,
      -1,   136,    20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     145,   146,    -1,   148,    91,    92,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   111,    -1,   113,   114,   115,    -1,
     117,    -1,    -1,    -1,   121,   122,    -1,    -1,     8,    -1,
      -1,    -1,   129,    13,    14,    15,    16,   134,    -1,   136,
      20,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   145,   146,
      -1,   148,    -1,    91,    92,    93,    94,    95,    96,    97,
      98,    99,   100,   101,   102,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   111,    -1,   113,   114,   115,    -1,   117,
      -1,    -1,    -1,   121,   122,    -1,    -1,     8,    -1,    -1,
      -1,   129,    13,    14,    15,    16,   134,    -1,   136,    20,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   145,    -1,    -1,
     148,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   111,    -1,   113,   114,   115,    -1,   117,    -1,    -1,
      -1,   121,   122,    -1,    -1,     8,    -1,    -1,    -1,   129,
      13,    14,    15,    16,   134,    -1,   136,    20,    -1,    -1,
       3,     4,    -1,    -1,     7,   145,     9,    10,   148,    -1,
      91,    92,    93,    94,    95,    96,    97,    98,    99,   100,
     101,   102,     3,     4,    -1,    -1,     7,    -1,     9,    10,
     111,    -1,   113,   114,   115,    -1,   117,    -1,     3,     4,
     121,   122,     7,    -1,     9,    10,    -1,    -1,   129,    -1,
       3,     4,    -1,   134,     7,   136,     9,    10,    -1,    -1,
      -1,    -1,    -1,    -1,   145,    -1,    -1,   148,    91,    92,
      93,    94,    95,    96,    97,    98,    99,   100,   101,   102,
       3,     4,    -1,    -1,     7,    -1,     9,    10,   111,    -1,
     113,   114,   115,    -1,   117,    -1,     3,     4,   121,   122,
       7,    -1,     9,    10,    -1,    -1,   129,    -1,     3,     4,
      -1,   134,     7,   136,     9,    10,    -1,    -1,    -1,    -1,
       3,     4,   145,    -1,     7,   148,     9,    10,   131,    -1,
     133,   134,   135,   136,    -1,   138,   139,    -1,   141,   142,
       3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,    -1,
     131,    -1,   133,   134,   135,   136,    -1,   138,   139,    -1,
     141,   142,    -1,    -1,    -1,    -1,   131,    -1,   133,   134,
     135,   136,    -1,   138,   139,    -1,   141,   142,   131,    -1,
     133,   134,   135,   136,   137,   138,     3,     4,   141,   142,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,   131,    -1,
     133,   134,   135,   136,    -1,   138,   139,    -1,   141,   142,
      -1,    -1,    -1,    -1,   131,    -1,   133,   134,   135,   136,
      -1,   138,   139,    -1,   141,   142,   131,    -1,   133,   134,
     135,   136,   137,   138,    -1,    15,   141,   142,   131,    -1,
     133,   134,   135,   136,    -1,   138,   139,    -1,   141,   142,
      13,    14,    15,    16,    -1,    -1,    -1,    20,   131,    -1,
     133,   134,   135,   136,   137,   138,    -1,    -1,   141,   142,
      -1,    -1,    -1,    -1,    -1,    -1,    15,    -1,    -1,    -1,
      -1,    15,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   131,    -1,   133,   134,   135,   136,
      -1,   138,    -1,    -1,   141,   142,   131,    -1,   133,   134,
     135,   136,    -1,   138,    -1,    -1,   141,   142,    -1,    -1,
      -1,   111,    -1,   113,   114,   115,    99,   100,   101,   102,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   111,   129,
     113,   114,   115,    -1,    -1,    -1,   136,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,   145,   129,    -1,   148,    -1,
      -1,    -1,   111,   136,   113,   114,   115,   111,    -1,   113,
     114,   115,    -1,   146,    -1,   148,    -1,    -1,    -1,    -1,
     129,    -1,    -1,    -1,    -1,   129,    -1,   136,    -1,    -1,
      -1,    -1,   136,    -1,    -1,    -1,   145,    -1,    -1,   148,
      -1,   145,    -1,    -1,   148
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   150,     0,     1,     8,    13,    14,    15,    16,    20,
      40,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   108,   109,   110,
     111,   113,   114,   115,   116,   117,   118,   119,   120,   121,
     122,   123,   124,   125,   126,   127,   128,   129,   131,   134,
     136,   140,   145,   148,   151,   152,   153,   154,   155,   156,
     157,   158,   160,   162,   163,   164,   165,   166,   173,   174,
     176,   177,   178,   179,   180,   181,   182,   183,   184,   185,
     186,   187,   188,   189,   190,   191,   192,   140,    15,    16,
      99,   100,   101,   102,   129,   158,   173,   145,   156,   164,
     145,   145,   145,   145,   145,   145,   145,   145,   145,   145,
     156,   145,   156,   145,   156,   145,   156,   111,   112,   157,
     111,   140,   156,   145,   158,   111,   145,   112,   145,   145,
     111,   145,   111,   145,    15,   158,   165,   166,   166,   158,
     157,   157,   158,   140,    11,   145,   147,   130,   139,     3,
       4,     7,     9,    10,   131,   133,   134,   135,   136,   138,
     141,   142,   158,   157,   130,   139,   140,   172,   145,   156,
     139,   156,   158,   158,   112,   145,   158,   167,   158,   158,
     158,   158,   158,   158,   158,   146,   157,   158,   146,   157,
     158,   146,   157,   140,   140,    15,    16,    99,   100,   101,
     102,   146,   156,   173,   111,   112,   158,   159,   111,   158,
     112,   146,   157,   175,   137,   146,   148,   156,   146,   157,
     156,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     158,   158,   158,   130,   146,   161,   156,   158,   136,   156,
     172,   146,   158,   146,   139,   146,   139,   139,   139,   146,
     139,   146,   139,   139,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   139,   146,   146,   112,
     139,   158,   111,   146,   146,   140,   146,   137,   139,   158,
     139,   146,   158,   158,   167,   139,   167,   158,   158,   158,
     158,   158,   158,    15,    16,    99,   100,   101,   102,   173,
      91,    94,    95,    97,    98,   158,   146,   112,   112,   118,
     158,   161,   158,   137,   139,   157,   139,   146,   139,   146,
     139,   139,   146,   139,   146,   146,   146,   146,   146,   146,
     146,   146,   146,   146,   146,   146,   146,   146,   137,   139,
     136,   167,   146,   114,   145,   168,   169,   171,   158,   158,
     158,   158,   158,   158,   139,   169,   170,   145,   146,   146,
     146,   146,   146,   146,   137,   171,   139,   146,   157,   170,
     146
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
#line 373 "grammar.y"
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
#line 408 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 6:

/* Line 1464 of yacc.c  */
#line 410 "grammar.y"
    { (yyvsp[(1) - (2)].lv).CleanUp(); currentVoice->ifsw=0;;}
    break;

  case 7:

/* Line 1464 of yacc.c  */
#line 412 "grammar.y"
    {
            YYACCEPT;
          ;}
    break;

  case 8:

/* Line 1464 of yacc.c  */
#line 416 "grammar.y"
    {
            currentVoice->ifsw=0;
            iiDebug();
          ;}
    break;

  case 9:

/* Line 1464 of yacc.c  */
#line 421 "grammar.y"
    {currentVoice->ifsw=0;;}
    break;

  case 10:

/* Line 1464 of yacc.c  */
#line 423 "grammar.y"
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
#line 481 "grammar.y"
    {if (currentVoice!=NULL) currentVoice->ifsw=0;;}
    break;

  case 19:

/* Line 1464 of yacc.c  */
#line 484 "grammar.y"
    { omFree((ADDRESS)(yyvsp[(2) - (2)].name)); ;}
    break;

  case 29:

/* Line 1464 of yacc.c  */
#line 499 "grammar.y"
    {
            if(iiAssign(&(yyvsp[(1) - (2)].lv),&(yyvsp[(2) - (2)].lv))) YYERROR;
          ;}
    break;

  case 30:

/* Line 1464 of yacc.c  */
#line 506 "grammar.y"
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&(yyval.lv),omStrDup((yyvsp[(1) - (1)].name)));
          ;}
    break;

  case 31:

/* Line 1464 of yacc.c  */
#line 511 "grammar.y"
    {
            syMake(&(yyval.lv),(yyvsp[(1) - (1)].name));
          ;}
    break;

  case 32:

/* Line 1464 of yacc.c  */
#line 515 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), COLONCOLON, &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 33:

/* Line 1464 of yacc.c  */
#line 519 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[(1) - (3)].lv), '.', &(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 34:

/* Line 1464 of yacc.c  */
#line 523 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'(')) YYERROR;
          ;}
    break;

  case 35:

/* Line 1464 of yacc.c  */
#line 527 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 540 "grammar.y"
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
#line 568 "grammar.y"
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
#line 592 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[(1) - (1)].i);
            (yyval.lv).data = (yyval.lv).Data();
          ;}
    break;

  case 39:

/* Line 1464 of yacc.c  */
#line 598 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[(1) - (1)].name);
          ;}
    break;

  case 40:

/* Line 1464 of yacc.c  */
#line 604 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 41:

/* Line 1464 of yacc.c  */
#line 611 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 622 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 43:

/* Line 1464 of yacc.c  */
#line 628 "grammar.y"
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            (yyval.lv) = (yyvsp[(1) - (1)].lv);
          ;}
    break;

  case 44:

/* Line 1464 of yacc.c  */
#line 632 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (1)].lv); ;}
    break;

  case 45:

/* Line 1464 of yacc.c  */
#line 633 "grammar.y"
    { (yyval.lv) = (yyvsp[(2) - (3)].lv); ;}
    break;

  case 46:

/* Line 1464 of yacc.c  */
#line 635 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[(1) - (6)].lv),&(yyvsp[(3) - (6)].lv),&(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 47:

/* Line 1464 of yacc.c  */
#line 639 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (4)].lv),'[',&(yyvsp[(3) - (4)].lv))) YYERROR;
          ;}
    break;

  case 48:

/* Line 1464 of yacc.c  */
#line 643 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 49:

/* Line 1464 of yacc.c  */
#line 647 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 50:

/* Line 1464 of yacc.c  */
#line 651 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 51:

/* Line 1464 of yacc.c  */
#line 655 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 52:

/* Line 1464 of yacc.c  */
#line 659 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 53:

/* Line 1464 of yacc.c  */
#line 663 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 54:

/* Line 1464 of yacc.c  */
#line 667 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 55:

/* Line 1464 of yacc.c  */
#line 671 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 56:

/* Line 1464 of yacc.c  */
#line 675 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 57:

/* Line 1464 of yacc.c  */
#line 679 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 58:

/* Line 1464 of yacc.c  */
#line 683 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 59:

/* Line 1464 of yacc.c  */
#line 687 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 60:

/* Line 1464 of yacc.c  */
#line 691 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 61:

/* Line 1464 of yacc.c  */
#line 695 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(3) - (6)].lv),(yyvsp[(1) - (6)].i),&(yyvsp[(5) - (6)].lv),TRUE)) YYERROR;
          ;}
    break;

  case 62:

/* Line 1464 of yacc.c  */
#line 699 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 63:

/* Line 1464 of yacc.c  */
#line 703 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 64:

/* Line 1464 of yacc.c  */
#line 707 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 65:

/* Line 1464 of yacc.c  */
#line 711 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 66:

/* Line 1464 of yacc.c  */
#line 715 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[(1) - (3)].i))) YYERROR;
          ;}
    break;

  case 67:

/* Line 1464 of yacc.c  */
#line 719 "grammar.y"
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 68:

/* Line 1464 of yacc.c  */
#line 723 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[(1) - (8)].i),&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 69:

/* Line 1464 of yacc.c  */
#line 727 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),(yyvsp[(1) - (4)].i))) YYERROR;
          ;}
    break;

  case 70:

/* Line 1464 of yacc.c  */
#line 731 "grammar.y"
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[(3) - (8)].lv),&(yyvsp[(5) - (8)].lv),&(yyvsp[(7) - (8)].lv))) YYERROR;
          ;}
    break;

  case 71:

/* Line 1464 of yacc.c  */
#line 735 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(3) - (4)].lv),RING_CMD)) YYERROR;
          ;}
    break;

  case 72:

/* Line 1464 of yacc.c  */
#line 739 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 73:

/* Line 1464 of yacc.c  */
#line 743 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 74:

/* Line 1464 of yacc.c  */
#line 747 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 75:

/* Line 1464 of yacc.c  */
#line 751 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 76:

/* Line 1464 of yacc.c  */
#line 755 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), (yyvsp[(5) - (6)].i), NULL)) YYERROR;
          ;}
    break;

  case 77:

/* Line 1464 of yacc.c  */
#line 759 "grammar.y"
    {
            if (iiApply(&(yyval.lv), &(yyvsp[(3) - (6)].lv), 0, &(yyvsp[(5) - (6)].lv))) YYERROR;
          ;}
    break;

  case 78:

/* Line 1464 of yacc.c  */
#line 763 "grammar.y"
    {
            (yyval.lv)=(yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 79:

/* Line 1464 of yacc.c  */
#line 767 "grammar.y"
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

  case 80:

/* Line 1464 of yacc.c  */
#line 784 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 81:

/* Line 1464 of yacc.c  */
#line 790 "grammar.y"
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

  case 82:

/* Line 1464 of yacc.c  */
#line 802 "grammar.y"
    {
            #ifdef SIQ
            siq++;
            #endif
          ;}
    break;

  case 83:

/* Line 1464 of yacc.c  */
#line 810 "grammar.y"
    {
            #ifdef SIQ
            siq--;
            #endif
          ;}
    break;

  case 84:

/* Line 1464 of yacc.c  */
#line 819 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),PLUSPLUS)) YYERROR;
          ;}
    break;

  case 85:

/* Line 1464 of yacc.c  */
#line 823 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(1) - (2)].lv),MINUSMINUS)) YYERROR;
          ;}
    break;

  case 86:

/* Line 1464 of yacc.c  */
#line 827 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'+',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 87:

/* Line 1464 of yacc.c  */
#line 831 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'-',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 88:

/* Line 1464 of yacc.c  */
#line 835 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 89:

/* Line 1464 of yacc.c  */
#line 839 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),'^',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 90:

/* Line 1464 of yacc.c  */
#line 843 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 91:

/* Line 1464 of yacc.c  */
#line 847 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),(yyvsp[(2) - (3)].i),&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 92:

/* Line 1464 of yacc.c  */
#line 851 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),NOTEQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 93:

/* Line 1464 of yacc.c  */
#line 855 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),EQUAL_EQUAL,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 94:

/* Line 1464 of yacc.c  */
#line 859 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),DOTDOT,&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 95:

/* Line 1464 of yacc.c  */
#line 863 "grammar.y"
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[(1) - (3)].lv),':',&(yyvsp[(3) - (3)].lv))) YYERROR;
          ;}
    break;

  case 96:

/* Line 1464 of yacc.c  */
#line 867 "grammar.y"
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            int i; TESTSETINT((yyvsp[(2) - (2)].lv),i);
            (yyval.lv).rtyp  = INT_CMD;
            (yyval.lv).data = (void *)(long)(i == 0 ? 1 : 0);
          ;}
    break;

  case 97:

/* Line 1464 of yacc.c  */
#line 874 "grammar.y"
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[(2) - (2)].lv),'-')) YYERROR;
          ;}
    break;

  case 98:

/* Line 1464 of yacc.c  */
#line 880 "grammar.y"
    { (yyval.lv) = (yyvsp[(1) - (2)].lv); ;}
    break;

  case 99:

/* Line 1464 of yacc.c  */
#line 882 "grammar.y"
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

  case 101:

/* Line 1464 of yacc.c  */
#line 902 "grammar.y"
    {
            if ((yyvsp[(2) - (3)].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[(2) - (3)].lv).CopyD(STRING_CMD);
            (yyvsp[(2) - (3)].lv).CleanUp();
          ;}
    break;

  case 102:

/* Line 1464 of yacc.c  */
#line 914 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 103:

/* Line 1464 of yacc.c  */
#line 919 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 104:

/* Line 1464 of yacc.c  */
#line 924 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 105:

/* Line 1464 of yacc.c  */
#line 928 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
          ;}
    break;

  case 106:

/* Line 1464 of yacc.c  */
#line 932 "grammar.y"
    {
            int r; TESTSETINT((yyvsp[(4) - (8)].lv),r);
            int c; TESTSETINT((yyvsp[(7) - (8)].lv),c);
            if (r < 1)
              MYYERROR("rows must be greater than 0");
            if (c < 0)
              MYYERROR("cols must be greater than -1");
            leftv v;
            idhdl h;
            if ((yyvsp[(1) - (8)].i) == MATRIX_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&(currRing->idroot), TRUE)) YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              idDelete(&IDIDEAL(h));
              IDMATRIX(h) = mpNew(r,c);
              if (IDMATRIX(h)==NULL) YYERROR;
            }
            else if ((yyvsp[(1) - (8)].i) == INTMAT_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&((yyvsp[(2) - (8)].lv).req_packhdl->idroot)))
                YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              delete IDINTVEC(h);
              IDINTVEC(h) = new intvec(r,c,0);
              if (IDINTVEC(h)==NULL) YYERROR;
            }
            else /* BIGINTMAT_CMD */
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (8)].lv),myynest,(yyvsp[(1) - (8)].i),&((yyvsp[(2) - (8)].lv).req_packhdl->idroot)))
                YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              delete IDBIMAT(h);
              IDBIMAT(h) = new bigintmat(r, c, coeffs_BIGINT);
              if (IDBIMAT(h)==NULL) YYERROR;
            }
          ;}
    break;

  case 107:

/* Line 1464 of yacc.c  */
#line 972 "grammar.y"
    {
            if ((yyvsp[(1) - (2)].i) == MATRIX_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&(currRing->idroot), TRUE)) YYERROR;
            }
            else if ((yyvsp[(1) - (2)].i) == INTMAT_CMD)
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
            }
            else /* BIGINTMAT_CMD */
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
                YYERROR;
            }
          ;}
    break;

  case 108:

/* Line 1464 of yacc.c  */
#line 998 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 1019 "grammar.y"
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[(2) - (2)].lv),myynest,(yyvsp[(1) - (2)].i),&((yyvsp[(2) - (2)].lv).req_packhdl->idroot)))
              YYERROR;
          ;}
    break;

  case 112:

/* Line 1464 of yacc.c  */
#line 1032 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 1046 "grammar.y"
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[(1) - (1)].name));
        ;}
    break;

  case 114:

/* Line 1464 of yacc.c  */
#line 1054 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 1063 "grammar.y"
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

/* Line 1464 of yacc.c  */
#line 1109 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(1) - (3)].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[(3) - (3)].lv),sizeof(sleftv));
          ;}
    break;

  case 119:

/* Line 1464 of yacc.c  */
#line 1119 "grammar.y"
    {
            (yyval.lv) = (yyvsp[(2) - (3)].lv);
          ;}
    break;

  case 120:

/* Line 1464 of yacc.c  */
#line 1125 "grammar.y"
    {
            expected_parms = TRUE;
          ;}
    break;

  case 121:

/* Line 1464 of yacc.c  */
#line 1132 "grammar.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); ;}
    break;

  case 122:

/* Line 1464 of yacc.c  */
#line 1134 "grammar.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); ;}
    break;

  case 123:

/* Line 1464 of yacc.c  */
#line 1136 "grammar.y"
    { (yyval.i) = (yyvsp[(1) - (1)].i); ;}
    break;

  case 124:

/* Line 1464 of yacc.c  */
#line 1145 "grammar.y"
    { if ((yyvsp[(1) - (2)].i) != '<') YYERROR;
            if((feFilePending=feFopen((yyvsp[(2) - (2)].name),"r",NULL,TRUE))==NULL) YYERROR; ;}
    break;

  case 125:

/* Line 1464 of yacc.c  */
#line 1148 "grammar.y"
    { newFile((yyvsp[(2) - (4)].name),feFilePending); ;}
    break;

  case 126:

/* Line 1464 of yacc.c  */
#line 1153 "grammar.y"
    {
            feHelp((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 127:

/* Line 1464 of yacc.c  */
#line 1158 "grammar.y"
    {
            feHelp(NULL);
          ;}
    break;

  case 128:

/* Line 1464 of yacc.c  */
#line 1165 "grammar.y"
    {
            singular_example((yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
          ;}
    break;

  case 129:

/* Line 1464 of yacc.c  */
#line 1173 "grammar.y"
    {
          if (basePack!=(yyvsp[(2) - (2)].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[(2) - (2)].lv),0,currPackHdl)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[(2) - (2)].lv),0)) YYERROR;
        ;}
    break;

  case 130:

/* Line 1464 of yacc.c  */
#line 1185 "grammar.y"
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

  case 131:

/* Line 1464 of yacc.c  */
#line 1201 "grammar.y"
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

  case 132:

/* Line 1464 of yacc.c  */
#line 1220 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 133:

/* Line 1464 of yacc.c  */
#line 1224 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 134:

/* Line 1464 of yacc.c  */
#line 1228 "grammar.y"
    {
            if ((yyvsp[(3) - (4)].i)==QRING_CMD) (yyvsp[(3) - (4)].i)=RING_CMD;
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 135:

/* Line 1464 of yacc.c  */
#line 1233 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
          ;}
    break;

  case 136:

/* Line 1464 of yacc.c  */
#line 1237 "grammar.y"
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 137:

/* Line 1464 of yacc.c  */
#line 1241 "grammar.y"
    {
            list_cmd((yyvsp[(3) - (4)].i),NULL,"// ",TRUE);
           ;}
    break;

  case 138:

/* Line 1464 of yacc.c  */
#line 1245 "grammar.y"
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          ;}
    break;

  case 139:

/* Line 1464 of yacc.c  */
#line 1249 "grammar.y"
    {
            list_cmd(0,(yyvsp[(3) - (4)].lv).Fullname(),"// ",TRUE);
            (yyvsp[(3) - (4)].lv).CleanUp();
          ;}
    break;

  case 140:

/* Line 1464 of yacc.c  */
#line 1254 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 141:

/* Line 1464 of yacc.c  */
#line 1260 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 142:

/* Line 1464 of yacc.c  */
#line 1266 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 143:

/* Line 1464 of yacc.c  */
#line 1272 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 144:

/* Line 1464 of yacc.c  */
#line 1278 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 145:

/* Line 1464 of yacc.c  */
#line 1284 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 146:

/* Line 1464 of yacc.c  */
#line 1290 "grammar.y"
    {
            if((yyvsp[(3) - (6)].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[(5) - (6)].i),NULL,"// ",TRUE);
            (yyvsp[(3) - (6)].lv).CleanUp();
          ;}
    break;

  case 147:

/* Line 1464 of yacc.c  */
#line 1302 "grammar.y"
    {
            list_cmd(-1,NULL,"// ",TRUE);
          ;}
    break;

  case 148:

/* Line 1464 of yacc.c  */
#line 1308 "grammar.y"
    { yyInRingConstruction = TRUE; ;}
    break;

  case 149:

/* Line 1464 of yacc.c  */
#line 1317 "grammar.y"
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

  case 150:

/* Line 1464 of yacc.c  */
#line 1350 "grammar.y"
    {
            const char *ring_name = (yyvsp[(2) - (2)].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[(2) - (2)].lv).CleanUp();
          ;}
    break;

  case 151:

/* Line 1464 of yacc.c  */
#line 1360 "grammar.y"
    {
            if (((yyvsp[(1) - (2)].i)!=LIB_CMD)||(jjLOAD((yyvsp[(2) - (2)].name),TRUE))) YYERROR;
          ;}
    break;

  case 154:

/* Line 1464 of yacc.c  */
#line 1369 "grammar.y"
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

  case 155:

/* Line 1464 of yacc.c  */
#line 1441 "grammar.y"
    {
            type_cmd(&((yyvsp[(2) - (2)].lv)));
          ;}
    break;

  case 156:

/* Line 1464 of yacc.c  */
#line 1445 "grammar.y"
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

  case 157:

/* Line 1464 of yacc.c  */
#line 1474 "grammar.y"
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

  case 158:

/* Line 1464 of yacc.c  */
#line 1487 "grammar.y"
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

  case 159:

/* Line 1464 of yacc.c  */
#line 1504 "grammar.y"
    {
            int i; TESTSETINT((yyvsp[(3) - (5)].lv),i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          ;}
    break;

  case 160:

/* Line 1464 of yacc.c  */
#line 1513 "grammar.y"
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 161:

/* Line 1464 of yacc.c  */
#line 1518 "grammar.y"
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          ;}
    break;

  case 162:

/* Line 1464 of yacc.c  */
#line 1526 "grammar.y"
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen((yyvsp[(2) - (3)].name)) + strlen((yyvsp[(3) - (3)].name)) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,(yyvsp[(2) - (3)].name),(yyvsp[(3) - (3)].name));
            newBuffer(s,BT_break);
            omFree((ADDRESS)(yyvsp[(2) - (3)].name));
            omFree((ADDRESS)(yyvsp[(3) - (3)].name));
          ;}
    break;

  case 163:

/* Line 1464 of yacc.c  */
#line 1538 "grammar.y"
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

  case 164:

/* Line 1464 of yacc.c  */
#line 1557 "grammar.y"
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

  case 165:

/* Line 1464 of yacc.c  */
#line 1567 "grammar.y"
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

  case 166:

/* Line 1464 of yacc.c  */
#line 1586 "grammar.y"
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

  case 167:

/* Line 1464 of yacc.c  */
#line 1609 "grammar.y"
    {
            // decl. of type proc p(int i)
            if ((yyvsp[(1) - (2)].i)==PARAMETER)  { if (iiParameter(&(yyvsp[(2) - (2)].lv))) YYERROR; }
            else                { if (iiAlias(&(yyvsp[(2) - (2)].lv))) YYERROR; }
          ;}
    break;

  case 168:

/* Line 1464 of yacc.c  */
#line 1615 "grammar.y"
    {
            // decl. of type proc p(i)
            sleftv tmp_expr;
            if ((yyvsp[(1) - (2)].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[(2) - (2)].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          ;}
    break;

  case 169:

/* Line 1464 of yacc.c  */
#line 1627 "grammar.y"
    {
            iiRETURNEXPR.Copy(&(yyvsp[(3) - (4)].lv));
            (yyvsp[(3) - (4)].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          ;}
    break;

  case 170:

/* Line 1464 of yacc.c  */
#line 1633 "grammar.y"
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
#line 4151 "grammar.cc"
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



