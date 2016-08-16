/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 1

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 7 "grammar.y" /* yacc.c:339  */


#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <kernel/mod2.h>

#include <misc/mylimits.h>
#include <omalloc/omalloc.h>
#include <Singular/tok.h>
#include <misc/options.h>
#include <Singular/stype.h>
#include <Singular/fehelp.h>
#include <Singular/ipid.h>
#include <misc/intvec.h>
#include <kernel/oswrapper/feread.h>
#include <Singular/fevoices.h>
#include <polys/matpol.h>
#include <polys/monomials/ring.h>
#include <kernel/GBEngine/kstd1.h>
#include <Singular/subexpr.h>
#include <Singular/ipshell.h>
#include <Singular/ipconv.h>
#include <Singular/sdb.h>
#include <kernel/ideals.h>
#include <coeffs/numbers.h>
#include <kernel/polys.h>
#include <kernel/combinatorics/stairc.h>
#include <kernel/oswrapper/timer.h>
#include <Singular/cntrlc.h>
#include <polys/monomials/maps.h>
#include <kernel/GBEngine/syz.h>
#include <Singular/lists.h>
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
}


#line 234 "grammar.cc" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "grammar.hh".  */
#ifndef YY_YY_GRAMMAR_HH_INCLUDED
# define YY_YY_GRAMMAR_HH_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
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
    IDEAL_CMD = 274,
    MAP_CMD = 275,
    MATRIX_CMD = 276,
    MODUL_CMD = 277,
    NUMBER_CMD = 278,
    POLY_CMD = 279,
    RESOLUTION_CMD = 280,
    VECTOR_CMD = 281,
    BETTI_CMD = 282,
    E_CMD = 283,
    FETCH_CMD = 284,
    FREEMODULE_CMD = 285,
    KEEPRING_CMD = 286,
    IMAP_CMD = 287,
    KOSZUL_CMD = 288,
    MAXID_CMD = 289,
    MONOM_CMD = 290,
    PAR_CMD = 291,
    PREIMAGE_CMD = 292,
    VAR_CMD = 293,
    VALTVARS = 294,
    VMAXDEG = 295,
    VMAXMULT = 296,
    VNOETHER = 297,
    VMINPOLY = 298,
    END_RING = 299,
    CMD_1 = 300,
    CMD_2 = 301,
    CMD_3 = 302,
    CMD_12 = 303,
    CMD_13 = 304,
    CMD_23 = 305,
    CMD_123 = 306,
    CMD_M = 307,
    ROOT_DECL = 308,
    ROOT_DECL_LIST = 309,
    RING_DECL = 310,
    RING_DECL_LIST = 311,
    EXAMPLE_CMD = 312,
    EXPORT_CMD = 313,
    HELP_CMD = 314,
    KILL_CMD = 315,
    LIB_CMD = 316,
    LISTVAR_CMD = 317,
    SETRING_CMD = 318,
    TYPE_CMD = 319,
    STRINGTOK = 320,
    BLOCKTOK = 321,
    INT_CONST = 322,
    UNKNOWN_IDENT = 323,
    RINGVAR = 324,
    PROC_DEF = 325,
    APPLY = 326,
    ASSUME_CMD = 327,
    BREAK_CMD = 328,
    CONTINUE_CMD = 329,
    ELSE_CMD = 330,
    EVAL = 331,
    QUOTE = 332,
    FOR_CMD = 333,
    IF_CMD = 334,
    SYS_BREAK = 335,
    WHILE_CMD = 336,
    RETURN = 337,
    PARAMETER = 338,
    SYSVAR = 339,
    UMINUS = 340
  };
#endif

/* Value type.  */



int yyparse (void);

#endif /* !YY_YY_GRAMMAR_HH_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 365 "grammar.cc" /* yacc.c:358  */

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
#else
typedef signed char yytype_int8;
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
# elif ! defined YYSIZE_T
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
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
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
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
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
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
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

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  2
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2527

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  103
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  45
/* YYNRULES -- Number of rules.  */
#define YYNRULES  175
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  399

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   340

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    96,     2,
      99,   100,     2,    88,    94,    89,   101,    90,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    97,    95,
      86,    85,    87,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,    91,     2,    92,    93,     2,   102,     2,     2,     2,
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
      98
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   316,   316,   318,   352,   353,   355,   357,   361,   366,
     368,   419,   420,   421,   422,   423,   424,   425,   426,   430,
     433,   434,   435,   436,   437,   438,   439,   440,   441,   444,
     451,   456,   460,   464,   468,   472,   485,   513,   537,   543,
     549,   553,   557,   561,   565,   569,   573,   577,   581,   585,
     589,   593,   597,   601,   605,   609,   613,   617,   621,   625,
     629,   635,   639,   643,   647,   651,   658,   669,   675,   680,
     681,   682,   686,   690,   694,   698,   702,   706,   710,   714,
     718,   735,   742,   741,   759,   767,   775,   784,   788,   792,
     796,   800,   804,   808,   812,   816,   820,   824,   828,   832,
     844,   851,   852,   871,   872,   884,   889,   894,   898,   902,
     938,   964,   985,   993,   997,   998,  1012,  1020,  1029,  1074,
    1075,  1084,  1085,  1091,  1098,  1100,  1102,  1112,  1111,  1119,
    1124,  1131,  1139,  1151,  1167,  1186,  1190,  1194,  1199,  1203,
    1207,  1211,  1215,  1220,  1226,  1232,  1238,  1244,  1250,  1256,
    1268,  1275,  1279,  1316,  1323,  1328,  1345,  1351,  1351,  1354,
    1422,  1426,  1455,  1468,  1485,  1494,  1499,  1507,  1519,  1538,
    1548,  1567,  1590,  1596,  1608,  1614
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "DOTDOT", "EQUAL_EQUAL", "GE", "LE",
  "MINUSMINUS", "NOT", "NOTEQUAL", "PLUSPLUS", "COLONCOLON", "ARROW",
  "GRING_CMD", "BIGINTMAT_CMD", "INTMAT_CMD", "PROC_CMD", "RING_CMD",
  "BEGIN_RING", "IDEAL_CMD", "MAP_CMD", "MATRIX_CMD", "MODUL_CMD",
  "NUMBER_CMD", "POLY_CMD", "RESOLUTION_CMD", "VECTOR_CMD", "BETTI_CMD",
  "E_CMD", "FETCH_CMD", "FREEMODULE_CMD", "KEEPRING_CMD", "IMAP_CMD",
  "KOSZUL_CMD", "MAXID_CMD", "MONOM_CMD", "PAR_CMD", "PREIMAGE_CMD",
  "VAR_CMD", "VALTVARS", "VMAXDEG", "VMAXMULT", "VNOETHER", "VMINPOLY",
  "END_RING", "CMD_1", "CMD_2", "CMD_3", "CMD_12", "CMD_13", "CMD_23",
  "CMD_123", "CMD_M", "ROOT_DECL", "ROOT_DECL_LIST", "RING_DECL",
  "RING_DECL_LIST", "EXAMPLE_CMD", "EXPORT_CMD", "HELP_CMD", "KILL_CMD",
  "LIB_CMD", "LISTVAR_CMD", "SETRING_CMD", "TYPE_CMD", "STRINGTOK",
  "BLOCKTOK", "INT_CONST", "UNKNOWN_IDENT", "RINGVAR", "PROC_DEF", "APPLY",
  "ASSUME_CMD", "BREAK_CMD", "CONTINUE_CMD", "ELSE_CMD", "EVAL", "QUOTE",
  "FOR_CMD", "IF_CMD", "SYS_BREAK", "WHILE_CMD", "RETURN", "PARAMETER",
  "SYSVAR", "'='", "'<'", "'>'", "'+'", "'-'", "'/'", "'['", "']'", "'^'",
  "','", "';'", "'&'", "':'", "UMINUS", "'('", "')'", "'.'", "'`'",
  "$accept", "lines", "pprompt", "flowctrl", "example_dummy", "command",
  "assign", "elemexpr", "exprlist", "expr", "$@1", "quote_start",
  "assume_start", "quote_end", "expr_arithmetic", "left_value",
  "extendedid", "declare_ip_variable", "stringexpr", "rlist", "ordername",
  "orderelem", "OrderingList", "ordering", "cmdeq", "mat_cmd", "filecmd",
  "$@2", "helpcmd", "examplecmd", "exportcmd", "killcmd", "listcmd",
  "ringcmd1", "ringcmd", "scriptcmd", "setrings", "setringcmd", "typecmd",
  "ifcmd", "whilecmd", "forcmd", "proccmd", "parametercmd", "returncmd", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
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
     335,   336,   337,   338,   339,    61,    60,    62,    43,    45,
      47,    91,    93,    94,    44,    59,    38,    58,   340,    40,
      41,    46,    96
};
# endif

#define YYPACT_NINF -365

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-365)))

#define YYTABLE_NINF -155

#define yytable_value_is_error(Yytable_value) \
  (!!((Yytable_value) == (-155)))

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -365,   374,  -365,   -71,  1840,  -365,  -365,  1905,   -63,  -365,
    -365,   -44,   -24,   -19,   -15,   -10,     8,    15,    25,  1970,
    2035,  2100,  2165,   -38,  1840,   -60,  1840,    29,  -365,  1840,
    -365,  -365,  -365,  -365,    65,    35,    48,  -365,  -365,     1,
      71,    74,   111,    80,  -365,   124,    87,  2230,   128,   128,
    1840,  1840,  -365,  1840,  1840,  -365,  -365,  -365,    88,  -365,
       2,   -76,  1348,  1840,  1840,  -365,  1840,   187,   -73,  -365,
    2295,  -365,  -365,  -365,  -365,   108,  -365,  1840,  -365,  -365,
    1840,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,
     107,   -63,   109,   119,   123,   126,  -365,    42,   134,  1840,
     208,  1348,    -6,  2360,  1840,  1840,  1840,  1840,  1840,  1840,
    1840,  1450,  1840,   216,  1515,   249,  1840,   284,  1580,   581,
     140,  -365,   149,   150,  -365,    -7,  1645,  1348,   -12,  1840,
    -365,  -365,  -365,  -365,   189,  1840,   195,  1710,  1905,  1348,
     163,  -365,  -365,    42,   -51,   -74,     7,  -365,  1840,  1775,
    -365,  1840,  1840,  1840,  -365,  1840,  -365,  1840,  1840,  1840,
    1840,  1840,  1840,  1840,  1840,  1840,   162,   563,   149,   201,
    -365,  1840,  -365,  -365,  1840,    -9,  1840,    68,  1348,  1840,
    1840,  1515,  1840,  1580,  1840,   635,  -365,  1840,   658,   171,
     674,   690,   704,   266,   399,   730,   468,  -365,   -69,   799,
    -365,   -62,   825,  -365,   -55,  -365,  -365,    12,    46,    92,
      97,   121,   138,  -365,   130,   141,   212,  -365,   841,  1840,
     209,   855,  -365,  -365,   -52,   190,  -365,  -365,  -365,  -365,
    -365,   -43,  1348,    30,   191,   191,   239,    84,    84,    42,
     494,    51,  1362,    84,  -365,  1840,  -365,  -365,  1840,  -365,
     611,   508,  1840,    55,  2360,   635,   799,   -32,   825,   -31,
     508,  -365,   871,  -365,  2360,  -365,  1840,  1840,  1840,  -365,
    1840,  -365,  1840,  1840,  -365,  -365,  -365,  -365,  -365,  -365,
    -365,  -365,  -365,  -365,  -365,  -365,   521,  -365,  -365,  -365,
    2425,   894,   221,   -58,  -365,  -365,  -365,  -365,  1840,   966,
     966,  1840,  -365,   989,   173,  1348,   202,  -365,  -365,  1840,
     211,  1003,  1019,  1033,  1061,   524,   540,   214,   217,   223,
     226,   231,   233,   235,   199,   213,   246,   265,   271,  1128,
    -365,  -365,  -365,  -365,  1156,  -365,  -365,  1170,   218,  1840,
    2360,   -30,   -65,  -365,  1840,  -365,  1840,  1840,  -365,  1840,
    -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,
    -365,  -365,  -365,  -365,  -365,  1840,  1840,   -48,   227,  -365,
    -365,   269,   240,  -365,   241,  1184,  1200,  1223,  1295,  1318,
    1334,  -365,   -65,   253,   251,  1840,  -365,  -365,  -365,  -365,
    -365,  -365,  -365,  -365,   269,  -365,   -17,  -365,  -365
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       2,     0,     1,     0,     0,   126,   125,     0,   151,   124,
     158,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   157,     0,
     113,    37,   103,    30,     0,     0,     0,   165,   166,     0,
       0,     0,     0,     0,     8,     0,     0,     0,    38,     0,
       0,     0,     9,     0,     0,     3,     4,    13,     0,    20,
      69,   161,    67,     0,     0,    68,     0,    31,     0,    39,
       0,    16,    17,    18,    21,    22,    23,     0,    25,    26,
       0,    27,    28,    11,    12,    14,    15,    24,     7,    10,
       0,     0,     0,     0,     0,     0,    38,    99,     0,     0,
      69,     0,    31,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    69,     0,    69,     0,    69,     0,    69,
       0,    19,   132,     0,   130,    69,     0,   160,     0,     0,
      85,   163,    82,    84,     0,     0,     0,     0,     0,   173,
     172,   156,   127,   100,     0,     0,     0,     5,     0,     0,
     102,     0,     0,     0,    88,     0,    87,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    29,     0,
     123,     0,     6,   101,     0,    69,     0,    69,   159,     0,
       0,     0,     0,     0,     0,    67,   169,     0,   114,     0,
       0,     0,     0,     0,     0,     0,     0,    59,     0,    67,
      43,     0,    67,    46,     0,   131,   129,     0,     0,     0,
       0,     0,     0,   150,    69,     0,     0,   170,     0,     0,
       0,     0,   167,   175,     0,     0,    36,    70,   104,    32,
      34,     0,    66,    97,    96,    95,    93,    89,    90,    91,
       0,    92,    94,    98,    33,     0,    86,    79,     0,    65,
      69,     0,     0,    69,     0,     0,     0,     0,     0,     0,
       0,    40,    67,    64,     0,    47,     0,     0,     0,    48,
       0,    49,     0,     0,    50,    60,    41,    42,    44,    45,
     141,   139,   135,   136,   137,   138,     0,   142,   140,   171,
       0,     0,     0,     0,   174,   128,    35,    72,     0,     0,
       0,     0,    62,     0,    69,   114,     0,    42,    45,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      83,   168,   162,   164,     0,    80,    81,     0,     0,     0,
       0,     0,     0,    51,     0,    52,     0,     0,    53,     0,
      54,   149,   147,   143,   144,   145,   146,   148,    73,    74,
      75,    76,    77,    78,    71,     0,     0,     0,     0,   115,
     116,     0,   117,   121,     0,     0,     0,     0,     0,     0,
       0,   155,     0,   119,     0,     0,    63,    55,    56,    57,
      58,    61,   109,   152,     0,   122,     0,   120,   118
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -365,  -365,  -365,  -365,  -365,  -365,  -365,   118,    -1,    52,
    -365,  -365,  -365,    77,  -365,  -365,   342,   311,   242,  -235,
    -365,  -364,   -33,   -14,   196,     0,  -365,  -365,  -365,  -365,
    -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,  -365,
    -365,  -365,  -365,  -365,  -365
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,    55,    56,    57,    58,    59,    60,   145,    62,
     219,    63,    64,   247,    65,    66,    67,    68,    69,   189,
     372,   373,   384,   374,   173,    98,    71,   225,    72,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      84,    85,    86,    87,    88
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      61,    70,   148,   370,   148,   123,   169,   383,   332,   150,
     152,   153,   170,   148,   154,   333,   155,   156,   151,   306,
     151,   171,   172,   122,    89,   151,   227,   120,   121,   310,
     383,   275,   151,  -155,   371,   124,   103,   154,   277,   151,
     156,   226,   151,   151,   381,   279,   151,    70,   294,   154,
     144,   151,   156,   216,   217,   104,    97,   296,   154,   101,
     186,   156,   151,   151,   151,   168,   148,   131,   307,   308,
     369,   101,   101,   101,   101,   105,  -110,   151,   101,   148,
     106,   127,   252,   398,   107,  -110,  -110,  -133,  -133,   108,
     149,   154,   149,   157,   156,   158,   159,   160,   161,   139,
     162,   149,   143,   163,   164,   368,   146,   109,   165,   228,
     198,   179,   280,   201,   110,   166,   167,   204,   158,   159,
     160,   161,   101,   162,   111,   100,   215,   164,   126,   101,
     128,   165,   178,   161,   129,   162,   224,   113,   115,   117,
     119,   148,   161,   165,   125,   103,   281,   130,   231,  -134,
    -134,   185,   165,   170,   149,   188,   190,   191,   192,   193,
     194,   195,   196,  -153,   199,   152,   153,   149,   202,   154,
     132,   155,   156,   133,   160,   161,   134,   162,   101,   135,
     257,   218,   259,   147,   148,   165,   137,   221,   175,   136,
     101,   180,   282,    30,   152,   177,   181,   283,   154,   169,
     101,   156,   176,   232,   233,   234,   179,   235,   180,   236,
     237,   238,   239,   240,   241,   242,   243,   101,   181,   148,
     182,   284,   182,   101,   286,   183,   251,   148,   101,   149,
     287,   255,   256,   184,   258,   205,   260,   183,   285,   262,
     184,   288,   152,   151,   214,   206,   154,   245,   157,   156,
     158,   159,   160,   161,   220,   162,   100,   171,   163,   164,
     148,   222,   246,   165,   339,   264,   229,   249,  -154,   152,
     153,   291,   149,   154,   292,   155,   156,   157,   289,   158,
     159,   160,   161,   244,   162,   295,   323,   331,   164,   250,
     141,   142,   165,  -112,   253,   148,   340,   299,   104,   358,
     300,  -105,  -112,  -112,   303,   342,   305,   149,   341,   366,
    -105,  -105,   107,   359,   351,   149,   305,   352,   311,   312,
     313,   382,   314,   353,   315,   316,   354,   158,   159,   160,
     161,   355,   162,   356,  -106,   357,   164,   370,   367,   385,
     165,   386,   329,  -106,  -106,   108,   360,   394,   149,   102,
     334,   395,   157,   337,   158,   159,   160,   161,   140,   162,
     268,   397,   163,   164,   110,   361,   269,   165,   393,  -107,
     111,   362,   304,   254,     2,     3,   335,   336,  -107,  -107,
       0,     0,     4,   149,   396,     0,     0,     0,     5,     6,
       7,     8,   305,     0,     0,     9,   375,     0,   376,   377,
       0,   378,   152,   153,     0,    10,   154,     0,   155,   156,
       0,     0,     0,     0,     0,     0,     0,   379,   380,    11,
      12,    13,    14,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,    26,     0,    27,    28,    29,    30,
       0,    31,    32,    33,    34,    35,    36,    37,    38,    39,
      40,    41,    42,    43,    44,    45,    46,    47,    48,     0,
      49,     0,     0,    50,     0,    51,     0,     0,     0,    52,
       0,   152,   153,    53,     0,   154,    54,   155,   156,     0,
       0,     0,     0,     0,     0,   157,     0,   158,   159,   160,
     161,     0,   162,   270,     0,   163,   164,   152,   153,   271,
     165,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,   152,   153,     0,     0,   154,     0,   155,   156,     0,
       0,     0,     0,     0,     0,     0,     0,   152,   153,     0,
       0,   154,     0,   155,   156,     5,     6,   317,   318,     0,
       0,     0,     9,   152,   153,     0,     0,   154,     0,   155,
     156,     0,     0,     0,   157,     0,   158,   159,   160,   161,
       0,   162,   273,     0,   163,   164,   152,   153,   274,   165,
     154,     0,   155,   156,   319,   320,   321,   322,     0,     0,
     157,     0,   158,   159,   160,   161,   297,   162,   298,     0,
     163,   164,   148,     0,   157,   165,   158,   159,   160,   161,
       0,   162,   301,     0,   163,   164,     0,     0,   302,   165,
     157,     0,   158,   159,   160,   161,     0,   162,   347,     0,
     163,   164,   148,     0,   348,   165,   157,     0,   158,   159,
     160,   161,     0,   162,   349,     0,   163,   164,   152,   153,
     350,   165,   154,     0,   155,   156,     0,     0,     0,   157,
       0,   158,   159,   160,   161,     0,   162,   248,     0,   163,
     164,   152,   153,     0,   165,   154,  -108,   155,   156,     0,
       0,     0,     0,     0,     0,  -108,  -108,   152,   153,     0,
     149,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   153,     0,  -111,   154,     0,   155,
     156,     0,     0,     0,     0,  -111,  -111,   152,   153,     0,
     149,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,   157,     0,   158,   159,   160,   161,     0,   162,     0,
       0,   163,   164,   152,   153,   261,   165,   154,     0,   155,
     156,     0,     0,     0,   157,     0,   158,   159,   160,   161,
       0,   162,     0,     0,   163,   164,     0,     0,   263,   165,
     157,     0,   158,   159,   160,   161,     0,   162,     0,     0,
     163,   164,     0,     0,   265,   165,   157,     0,   158,   159,
     160,   161,     0,   162,   266,     0,   163,   164,     0,     0,
     157,   165,   158,   159,   160,   161,     0,   162,   267,     0,
     163,   164,   152,   153,     0,   165,   154,     0,   155,   156,
       0,     0,     0,     0,     0,     0,   157,     0,   158,   159,
     160,   161,     0,   162,   272,     0,   163,   164,   152,   153,
       0,   165,   154,     0,   155,   156,     0,     0,     0,     0,
       0,     0,     0,     0,   152,   153,     0,     0,   154,     0,
     155,   156,     0,     0,     0,     0,     0,     0,   152,   153,
       0,     0,   154,     0,   155,   156,     0,     0,     0,     0,
       0,     0,     0,     0,   152,   153,     0,     0,   154,     0,
     155,   156,     0,     0,     0,   157,     0,   158,   159,   160,
     161,     0,   162,     0,     0,   163,   164,   152,   153,   276,
     165,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,   157,     0,   158,   159,   160,   161,     0,   162,     0,
       0,   163,   164,     0,     0,   278,   165,   157,     0,   158,
     159,   160,   161,     0,   162,   290,     0,   163,   164,     0,
       0,   157,   165,   158,   159,   160,   161,     0,   162,     0,
       0,   163,   164,     0,     0,   293,   165,   157,     0,   158,
     159,   160,   161,     0,   162,   309,     0,   163,   164,   152,
     153,     0,   165,   154,     0,   155,   156,     0,     0,     0,
     157,     0,   158,   159,   160,   161,     0,   162,     0,     0,
     163,   164,   152,   153,   330,   165,   154,     0,   155,   156,
       0,     0,     0,     0,     0,     0,   152,   153,     0,     0,
     154,     0,   155,   156,     0,     0,     0,     0,     0,     0,
       0,     0,   152,   153,     0,     0,   154,     0,   155,   156,
       0,     0,     0,     0,     0,     0,   152,   153,     0,     0,
     154,     0,   155,   156,     0,     0,     0,     0,     0,     0,
       0,     0,   157,     0,   158,   159,   160,   161,     0,   162,
       0,     0,   163,   164,   152,   153,   246,   165,   154,     0,
     155,   156,     0,     0,     0,   157,     0,   158,   159,   160,
     161,   338,   162,     0,     0,   163,   164,     0,     0,   157,
     165,   158,   159,   160,   161,     0,   162,     0,     0,   163,
     164,     0,     0,   343,   165,   157,     0,   158,   159,   160,
     161,     0,   162,   344,     0,   163,   164,     0,     0,   157,
     165,   158,   159,   160,   161,     0,   162,     0,     0,   163,
     164,   152,   153,   345,   165,   154,     0,   155,   156,     0,
       0,     0,     0,     0,     0,     0,     0,   157,     0,   158,
     159,   160,   161,     0,   162,   346,     0,   163,   164,   152,
     153,     0,   165,   154,     0,   155,   156,     0,     0,     0,
       0,     0,     0,   152,   153,     0,     0,   154,     0,   155,
     156,     0,     0,     0,     0,     0,     0,   152,   153,     0,
       0,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,     0,     0,   152,   153,     0,     0,   154,     0,   155,
     156,     0,     0,     0,   157,     0,   158,   159,   160,   161,
       0,   162,     0,     0,   163,   164,   152,   153,   363,   165,
     154,     0,   155,   156,     0,     0,     0,     0,     0,     0,
       0,     0,   157,     0,   158,   159,   160,   161,   364,   162,
       0,     0,   163,   164,     0,     0,   157,   165,   158,   159,
     160,   161,     0,   162,   365,     0,   163,   164,     0,     0,
     157,   165,   158,   159,   160,   161,     0,   162,     0,     0,
     163,   164,     0,     0,   387,   165,   157,     0,   158,   159,
     160,   161,     0,   162,     0,     0,   163,   164,   152,   153,
     388,   165,   154,     0,   155,   156,     0,     0,     0,   157,
       0,   158,   159,   160,   161,     0,   162,     0,     0,   163,
     164,   152,   153,   389,   165,   154,     0,   155,   156,     0,
       0,     0,     0,     0,     0,     0,     0,   152,   153,     0,
       0,   154,     0,   155,   156,     0,     0,     0,     0,     0,
       0,   152,   153,     0,     0,   154,     0,   155,   156,     0,
       0,     0,     0,     0,     0,   152,   153,     0,     0,   154,
       0,   155,   156,     0,     0,     0,     0,     0,     0,     0,
       0,   157,     0,   158,   159,   160,   161,     0,   162,     0,
       0,   163,   164,     0,     0,   390,   165,     0,     0,     0,
       0,     0,     0,     0,   157,     0,   158,   159,   160,   161,
       0,   162,     0,     0,   163,   164,     0,     0,   391,   165,
     157,     0,   158,   159,   160,   161,   392,   162,     0,     0,
     163,   164,     0,     0,   157,   165,   158,   159,   160,   161,
       0,   162,     0,     0,   163,   164,     0,     0,   157,   165,
     158,   159,   160,   161,     0,   162,     0,     0,     4,   164,
       0,     0,     0,   165,     5,     6,    90,    91,     0,     0,
       0,     9,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,    53,
     197,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,    53,   200,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,   207,   208,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,    53,
     203,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,   209,   210,
     211,   212,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,    53,   213,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,    53,
     223,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,    53,   230,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,    99,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,   112,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,   114,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,   116,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,   138,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,   118,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    19,    20,    21,    22,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,    53,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
      11,    12,    13,    14,    15,    16,    17,    18,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     4,     0,
       0,    40,    41,     0,     5,     6,    90,    91,     0,    96,
       0,     9,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,   174,     0,     0,    54,     0,     0,
       0,     0,     0,     0,     0,    11,    12,    13,    14,    15,
      16,    17,    18,    92,    93,    94,    95,     0,     0,     0,
       0,     0,     0,     0,     0,    30,     0,    31,    32,    33,
       0,    35,    36,     4,     0,     0,    40,    41,     0,     5,
       6,    90,    91,     0,    96,     0,     9,     0,     0,    50,
       0,    51,     0,     0,     0,     0,     0,     0,     0,   187,
       0,     0,    54,     0,     0,     0,     0,     0,     0,     0,
     324,    12,    13,   325,   326,    16,   327,   328,    92,    93,
      94,    95,     0,     0,     0,     0,     0,     0,     0,     0,
      30,     0,    31,    32,    33,     0,    35,    36,     0,     0,
       0,    40,    41,     0,     0,     0,     0,     0,     0,    96,
       0,     0,     0,     0,    50,     0,    51,     0,     0,     0,
       0,     0,     0,     0,    53,     0,     0,    54
};

static const yytype_int16 yycheck[] =
{
       1,     1,    11,    68,    11,    65,    12,   371,    66,    85,
       3,     4,    85,    11,     7,    73,     9,    10,    94,   254,
      94,    94,    95,    24,    95,    94,   100,    65,    66,   264,
     394,   100,    94,     3,    99,    95,    99,     7,   100,    94,
      10,    92,    94,    94,    92,   100,    94,    47,   100,     7,
      51,    94,    10,    65,    66,    99,     4,   100,     7,     7,
      66,    10,    94,    94,    94,    66,    11,    66,   100,   100,
     100,    19,    20,    21,    22,    99,    85,    94,    26,    11,
      99,    29,    91,   100,    99,    94,    95,    94,    95,    99,
      99,     7,    99,    86,    10,    88,    89,    90,    91,    47,
      93,    99,    50,    96,    97,   340,    54,    99,   101,   102,
     111,    99,   100,   114,    99,    63,    64,   118,    88,    89,
      90,    91,    70,    93,    99,     7,   126,    97,    99,    77,
      65,   101,    80,    91,    99,    93,   137,    19,    20,    21,
      22,    11,    91,   101,    26,    99,   100,    99,   149,    94,
      95,    99,   101,    85,    99,   103,   104,   105,   106,   107,
     108,   109,   110,    95,   112,     3,     4,    99,   116,     7,
      99,     9,    10,    99,    90,    91,    65,    93,   126,    99,
     181,   129,   183,    95,    11,   101,    99,   135,    70,    65,
     138,    99,   100,    65,     3,    77,    99,   100,     7,    12,
     148,    10,    94,   151,   152,   153,    99,   155,    99,   157,
     158,   159,   160,   161,   162,   163,   164,   165,    99,    11,
      99,   100,    99,   171,    94,    99,   174,    11,   176,    99,
     100,   179,   180,    99,   182,    95,   184,    99,   100,   187,
      99,   100,     3,    94,   126,    95,     7,    85,    86,    10,
      88,    89,    90,    91,    65,    93,   138,    94,    96,    97,
      11,    66,   100,   101,    91,    94,   148,    66,    95,     3,
       4,   219,    99,     7,    65,     9,    10,    86,    66,    88,
      89,    90,    91,   165,    93,    95,   286,    66,    97,   171,
      48,    49,   101,    85,   176,    11,    94,   245,    99,   100,
     248,    85,    94,    95,   252,    94,   254,    99,   309,    91,
      94,    95,    99,   100,   100,    99,   264,   100,   266,   267,
     268,    94,   270,   100,   272,   273,   100,    88,    89,    90,
      91,   100,    93,   100,    85,   100,    97,    68,   339,    99,
     101,   100,   290,    94,    95,    99,   100,    94,    99,     7,
     298,   100,    86,   301,    88,    89,    90,    91,    47,    93,
      94,   394,    96,    97,    99,   100,   100,   101,   382,    85,
      99,   100,   254,   177,     0,     1,   299,   300,    94,    95,
      -1,    -1,     8,    99,   385,    -1,    -1,    -1,    14,    15,
      16,    17,   340,    -1,    -1,    21,   344,    -1,   346,   347,
      -1,   349,     3,     4,    -1,    31,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   365,   366,    45,
      46,    47,    48,    49,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    60,    -1,    62,    63,    64,    65,
      -1,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    -1,
      86,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,    95,
      -1,     3,     4,    99,    -1,     7,   102,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    86,    -1,    88,    89,    90,
      91,    -1,    93,    94,    -1,    96,    97,     3,     4,   100,
     101,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    14,    15,    16,    17,    -1,
      -1,    -1,    21,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    86,    -1,    88,    89,    90,    91,
      -1,    93,    94,    -1,    96,    97,     3,     4,   100,   101,
       7,    -1,     9,    10,    53,    54,    55,    56,    -1,    -1,
      86,    -1,    88,    89,    90,    91,    92,    93,    94,    -1,
      96,    97,    11,    -1,    86,   101,    88,    89,    90,    91,
      -1,    93,    94,    -1,    96,    97,    -1,    -1,   100,   101,
      86,    -1,    88,    89,    90,    91,    -1,    93,    94,    -1,
      96,    97,    11,    -1,   100,   101,    86,    -1,    88,    89,
      90,    91,    -1,    93,    94,    -1,    96,    97,     3,     4,
     100,   101,     7,    -1,     9,    10,    -1,    -1,    -1,    86,
      -1,    88,    89,    90,    91,    -1,    93,    94,    -1,    96,
      97,     3,     4,    -1,   101,     7,    85,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    94,    95,     3,     4,    -1,
      99,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    85,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    94,    95,     3,     4,    -1,
      99,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    88,    89,    90,    91,    -1,    93,    -1,
      -1,    96,    97,     3,     4,   100,   101,     7,    -1,     9,
      10,    -1,    -1,    -1,    86,    -1,    88,    89,    90,    91,
      -1,    93,    -1,    -1,    96,    97,    -1,    -1,   100,   101,
      86,    -1,    88,    89,    90,    91,    -1,    93,    -1,    -1,
      96,    97,    -1,    -1,   100,   101,    86,    -1,    88,    89,
      90,    91,    -1,    93,    94,    -1,    96,    97,    -1,    -1,
      86,   101,    88,    89,    90,    91,    -1,    93,    94,    -1,
      96,    97,     3,     4,    -1,   101,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    88,    89,
      90,    91,    -1,    93,    94,    -1,    96,    97,     3,     4,
      -1,   101,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
      -1,    -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,
       9,    10,    -1,    -1,    -1,    86,    -1,    88,    89,    90,
      91,    -1,    93,    -1,    -1,    96,    97,     3,     4,   100,
     101,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    88,    89,    90,    91,    -1,    93,    -1,
      -1,    96,    97,    -1,    -1,   100,   101,    86,    -1,    88,
      89,    90,    91,    -1,    93,    94,    -1,    96,    97,    -1,
      -1,    86,   101,    88,    89,    90,    91,    -1,    93,    -1,
      -1,    96,    97,    -1,    -1,   100,   101,    86,    -1,    88,
      89,    90,    91,    -1,    93,    94,    -1,    96,    97,     3,
       4,    -1,   101,     7,    -1,     9,    10,    -1,    -1,    -1,
      86,    -1,    88,    89,    90,    91,    -1,    93,    -1,    -1,
      96,    97,     3,     4,   100,   101,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,
      -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    88,    89,    90,    91,    -1,    93,
      -1,    -1,    96,    97,     3,     4,   100,   101,     7,    -1,
       9,    10,    -1,    -1,    -1,    86,    -1,    88,    89,    90,
      91,    92,    93,    -1,    -1,    96,    97,    -1,    -1,    86,
     101,    88,    89,    90,    91,    -1,    93,    -1,    -1,    96,
      97,    -1,    -1,   100,   101,    86,    -1,    88,    89,    90,
      91,    -1,    93,    94,    -1,    96,    97,    -1,    -1,    86,
     101,    88,    89,    90,    91,    -1,    93,    -1,    -1,    96,
      97,     3,     4,   100,   101,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    86,    -1,    88,
      89,    90,    91,    -1,    93,    94,    -1,    96,    97,     3,
       4,    -1,   101,     7,    -1,     9,    10,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,     3,     4,    -1,    -1,     7,    -1,     9,
      10,    -1,    -1,    -1,    86,    -1,    88,    89,    90,    91,
      -1,    93,    -1,    -1,    96,    97,     3,     4,   100,   101,
       7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    86,    -1,    88,    89,    90,    91,    92,    93,
      -1,    -1,    96,    97,    -1,    -1,    86,   101,    88,    89,
      90,    91,    -1,    93,    94,    -1,    96,    97,    -1,    -1,
      86,   101,    88,    89,    90,    91,    -1,    93,    -1,    -1,
      96,    97,    -1,    -1,   100,   101,    86,    -1,    88,    89,
      90,    91,    -1,    93,    -1,    -1,    96,    97,     3,     4,
     100,   101,     7,    -1,     9,    10,    -1,    -1,    -1,    86,
      -1,    88,    89,    90,    91,    -1,    93,    -1,    -1,    96,
      97,     3,     4,   100,   101,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,    -1,
      -1,     7,    -1,     9,    10,    -1,    -1,    -1,    -1,    -1,
      -1,     3,     4,    -1,    -1,     7,    -1,     9,    10,    -1,
      -1,    -1,    -1,    -1,    -1,     3,     4,    -1,    -1,     7,
      -1,     9,    10,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    86,    -1,    88,    89,    90,    91,    -1,    93,    -1,
      -1,    96,    97,    -1,    -1,   100,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    86,    -1,    88,    89,    90,    91,
      -1,    93,    -1,    -1,    96,    97,    -1,    -1,   100,   101,
      86,    -1,    88,    89,    90,    91,    92,    93,    -1,    -1,
      96,    97,    -1,    -1,    86,   101,    88,    89,    90,    91,
      -1,    93,    -1,    -1,    96,    97,    -1,    -1,    86,   101,
      88,    89,    90,    91,    -1,    93,    -1,    -1,     8,    97,
      -1,    -1,    -1,   101,    14,    15,    16,    17,    -1,    -1,
      -1,    21,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
     100,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,   100,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,     8,    -1,
      -1,    76,    77,    -1,    14,    15,    16,    17,    -1,    84,
      -1,    21,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,   102,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    45,    46,    47,    48,    49,
      50,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    67,    68,    69,
      -1,    71,    72,     8,    -1,    -1,    76,    77,    -1,    14,
      15,    16,    17,    -1,    84,    -1,    21,    -1,    -1,    89,
      -1,    91,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,
      -1,    -1,   102,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    -1,    67,    68,    69,    -1,    71,    72,    -1,    -1,
      -1,    76,    77,    -1,    -1,    -1,    -1,    -1,    -1,    84,
      -1,    -1,    -1,    -1,    89,    -1,    91,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    99,    -1,    -1,   102
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   104,     0,     1,     8,    14,    15,    16,    17,    21,
      31,    45,    46,    47,    48,    49,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    60,    62,    63,    64,
      65,    67,    68,    69,    70,    71,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    86,
      89,    91,    95,    99,   102,   105,   106,   107,   108,   109,
     110,   111,   112,   114,   115,   117,   118,   119,   120,   121,
     128,   129,   131,   132,   133,   134,   135,   136,   137,   138,
     139,   140,   141,   142,   143,   144,   145,   146,   147,    95,
      16,    17,    53,    54,    55,    56,    84,   112,   128,    99,
     110,   112,   119,    99,    99,    99,    99,    99,    99,    99,
      99,    99,    99,   110,    99,   110,    99,   110,    99,   110,
      65,    66,   111,    65,    95,   110,    99,   112,    65,    99,
      99,    66,    99,    99,    65,    99,    65,    99,    16,   112,
     120,   121,   121,   112,   111,   111,   112,    95,    11,    99,
      85,    94,     3,     4,     7,     9,    10,    86,    88,    89,
      90,    91,    93,    96,    97,   101,   112,   112,   111,    12,
      85,    94,    95,   127,    99,   110,    94,   110,   112,    99,
      99,    99,    99,    99,    99,   112,    66,    99,   112,   122,
     112,   112,   112,   112,   112,   112,   112,   100,   111,   112,
     100,   111,   112,   100,   111,    95,    95,    16,    17,    53,
      54,    55,    56,   100,   110,   128,    65,    66,   112,   113,
      65,   112,    66,   100,   111,   130,    92,   100,   102,   110,
     100,   111,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   112,   112,   110,    85,   100,   116,    94,    66,
     110,   112,    91,   110,   127,   112,   112,   111,   112,   111,
     112,   100,   112,   100,    94,   100,    94,    94,    94,   100,
      94,   100,    94,    94,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,   100,   100,    94,   100,   100,    66,
      94,   112,    65,   100,   100,    95,   100,    92,    94,   112,
     112,    94,   100,   112,   110,   112,   122,   100,   100,    94,
     122,   112,   112,   112,   112,   112,   112,    16,    17,    53,
      54,    55,    56,   128,    45,    48,    49,    51,    52,   112,
     100,    66,    66,    73,   112,   116,   116,   112,    92,    91,
      94,   111,    94,   100,    94,   100,    94,    94,   100,    94,
     100,   100,   100,   100,   100,   100,   100,   100,   100,   100,
     100,   100,   100,   100,    92,    94,    91,   111,   122,   100,
      68,    99,   123,   124,   126,   112,   112,   112,   112,   112,
     112,    92,    94,   124,   125,    99,   100,   100,   100,   100,
     100,   100,    92,   126,    94,   100,   111,   125,   100
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   103,   104,   104,   105,   105,   105,   105,   105,   105,
     105,   106,   106,   106,   106,   106,   106,   106,   106,   107,
     108,   108,   108,   108,   108,   108,   108,   108,   108,   109,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   110,   110,   110,   110,
     110,   110,   110,   110,   110,   110,   111,   111,   112,   112,
     112,   112,   112,   112,   112,   112,   112,   112,   112,   112,
     112,   112,   113,   112,   114,   115,   116,   117,   117,   117,
     117,   117,   117,   117,   117,   117,   117,   117,   117,   117,
     117,   118,   118,   119,   119,   120,   120,   120,   120,   120,
     120,   120,   120,   121,   122,   122,   123,   124,   124,   125,
     125,   126,   126,   127,   128,   128,   128,   130,   129,   131,
     131,   132,   133,   134,   134,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   136,   137,   137,   137,   137,   138,   139,   139,   140,
     141,   141,   142,   142,   142,   142,   142,   143,   144,   145,
     145,   145,   146,   146,   147,   147
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     0,     2,     1,     2,     2,     1,     1,     1,
       2,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     2,
       1,     1,     3,     3,     3,     4,     3,     1,     1,     1,
       4,     4,     4,     3,     4,     4,     3,     4,     4,     4,
       4,     6,     6,     6,     6,     8,     8,     8,     8,     3,
       4,     8,     4,     8,     4,     3,     3,     1,     1,     1,
       3,     6,     4,     6,     6,     6,     6,     6,     6,     3,
       5,     5,     0,     5,     2,     2,     1,     2,     2,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     2,
       2,     2,     2,     1,     3,     2,     2,     2,     2,     8,
       2,     3,     2,     1,     1,     5,     1,     1,     4,     1,
       3,     1,     3,     1,     1,     1,     1,     0,     4,     3,
       2,     3,     2,     2,     3,     4,     4,     4,     4,     4,
       4,     4,     4,     6,     6,     6,     6,     6,     6,     6,
       3,     1,     8,     2,     4,     7,     2,     1,     1,     2,
       2,     1,     5,     2,     5,     1,     1,     3,     5,     3,
       3,     4,     2,     2,     4,     3
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              );
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

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
#ifndef YYINITDEPTH
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
static YYSIZE_T
yystrlen (const char *yystr)
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
static char *
yystpcpy (char *yydest, const char *yysrc)
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

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
/* The lookahead symbol.  */
int yychar;


/* The semantic value of the lookahead symbol.  */
/* Default value used for initialization, for pacifying older GCCs
   or non-GCC compilers.  */
YY_INITIAL_VALUE (static YYSTYPE yyval_default;)
YYSTYPE yylval YY_INITIAL_VALUE (= yyval_default);

    /* Number of syntax errors so far.  */
    int yynerrs;

    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
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
  int yytoken = 0;
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

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
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
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex (&yylval);
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
      if (yytable_value_is_error (yyn))
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
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

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
     '$$ = $1'.

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
#line 319 "grammar.y" /* yacc.c:1646  */
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
          }
#line 2185 "grammar.cc" /* yacc.c:1646  */
    break;

  case 5:
#line 354 "grammar.y" /* yacc.c:1646  */
    {currentVoice->ifsw=0;}
#line 2191 "grammar.cc" /* yacc.c:1646  */
    break;

  case 6:
#line 356 "grammar.y" /* yacc.c:1646  */
    { (yyvsp[-1].lv).CleanUp(); currentVoice->ifsw=0;}
#line 2197 "grammar.cc" /* yacc.c:1646  */
    break;

  case 7:
#line 358 "grammar.y" /* yacc.c:1646  */
    {
            YYACCEPT;
          }
#line 2205 "grammar.cc" /* yacc.c:1646  */
    break;

  case 8:
#line 362 "grammar.y" /* yacc.c:1646  */
    {
            currentVoice->ifsw=0;
            iiDebug();
          }
#line 2214 "grammar.cc" /* yacc.c:1646  */
    break;

  case 9:
#line 367 "grammar.y" /* yacc.c:1646  */
    {currentVoice->ifsw=0;}
#line 2220 "grammar.cc" /* yacc.c:1646  */
    break;

  case 10:
#line 369 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq=0;
            #endif
            yyInRingConstruction = FALSE;
            currentVoice->ifsw=0;
            if (inerror)
            {
/*  bison failed here*/
              if ((inerror!=3) && ((yyvsp[-1].i)<UMINUS) && ((yyvsp[-1].i)>' '))
              {
                // 1: yyerror called
                // 2: scanner put actual string
                // 3: error rule put token+\n
                inerror=3;
                Print(" error at token `%s`\n",iiTwoOps((yyvsp[-1].i)));
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
          }
#line 2273 "grammar.cc" /* yacc.c:1646  */
    break;

  case 18:
#line 427 "grammar.y" /* yacc.c:1646  */
    {if (currentVoice!=NULL) currentVoice->ifsw=0;}
#line 2279 "grammar.cc" /* yacc.c:1646  */
    break;

  case 19:
#line 430 "grammar.y" /* yacc.c:1646  */
    { omFree((ADDRESS)(yyvsp[0].name)); }
#line 2285 "grammar.cc" /* yacc.c:1646  */
    break;

  case 29:
#line 445 "grammar.y" /* yacc.c:1646  */
    {
            if(iiAssign(&(yyvsp[-1].lv),&(yyvsp[0].lv))) YYERROR;
          }
#line 2293 "grammar.cc" /* yacc.c:1646  */
    break;

  case 30:
#line 452 "grammar.y" /* yacc.c:1646  */
    {
            if (currRing==NULL) MYYERROR("no ring active");
            syMake(&(yyval.lv),omStrDup((yyvsp[0].name)));
          }
#line 2302 "grammar.cc" /* yacc.c:1646  */
    break;

  case 31:
#line 457 "grammar.y" /* yacc.c:1646  */
    {
            syMake(&(yyval.lv),(yyvsp[0].name));
          }
#line 2310 "grammar.cc" /* yacc.c:1646  */
    break;

  case 32:
#line 461 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[-2].lv), COLONCOLON, &(yyvsp[0].lv))) YYERROR;
          }
#line 2318 "grammar.cc" /* yacc.c:1646  */
    break;

  case 33:
#line 465 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv), &(yyvsp[-2].lv), '.', &(yyvsp[0].lv))) YYERROR;
          }
#line 2326 "grammar.cc" /* yacc.c:1646  */
    break;

  case 34:
#line 469 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-2].lv),'(')) YYERROR;
          }
#line 2334 "grammar.cc" /* yacc.c:1646  */
    break;

  case 35:
#line 473 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-3].lv).rtyp==UNKNOWN)
            { // for x(i)(j)
              if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),'(',&(yyvsp[-1].lv))) YYERROR;
            }
            else
            {
              (yyvsp[-3].lv).next=(leftv)omAllocBin(sleftv_bin);
              memcpy((yyvsp[-3].lv).next,&(yyvsp[-1].lv),sizeof(sleftv));
              if(iiExprArithM(&(yyval.lv),&(yyvsp[-3].lv),'(')) YYERROR;
            }
          }
#line 2351 "grammar.cc" /* yacc.c:1646  */
    break;

  case 36:
#line 486 "grammar.y" /* yacc.c:1646  */
    {
            if (currRingHdl==NULL) MYYERROR("no ring active");
            int j = 0;
            memset(&(yyval.lv),0,sizeof(sleftv));
            (yyval.lv).rtyp=VECTOR_CMD;
            leftv v = &(yyvsp[-1].lv);
            while (v!=NULL)
            {
              int i,t;
              sleftv tmp;
              memset(&tmp,0,sizeof(tmp));
              i=iiTestConvert((t=v->Typ()),POLY_CMD);
              if((i==0) || (iiConvert(t /*v->Typ()*/,POLY_CMD,i,v,&tmp)))
              {
                pDelete((poly *)&(yyval.lv).data);
                (yyvsp[-1].lv).CleanUp();
                MYYERROR("expected '[poly,...'");
              }
              poly p = (poly)tmp.CopyD(POLY_CMD);
              pSetCompP(p,++j);
              (yyval.lv).data = (void *)pAdd((poly)(yyval.lv).data,p);
              v->next=tmp.next;tmp.next=NULL;
              tmp.CleanUp();
              v=v->next;
            }
            (yyvsp[-1].lv).CleanUp();
          }
#line 2383 "grammar.cc" /* yacc.c:1646  */
    break;

  case 37:
#line 514 "grammar.y" /* yacc.c:1646  */
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            int i = atoi((yyvsp[0].name));
            /*remember not to omFree($1)
            *because it is a part of the scanner buffer*/
            (yyval.lv).rtyp  = INT_CMD;
            (yyval.lv).data = (void *)(long)i;

            /* check: out of range input */
            int l = strlen((yyvsp[0].name))+2;
            number n;
            if (l >= MAX_INT_LEN)
            {
              char tmp[MAX_INT_LEN+5];
              sprintf(tmp,"%d",i);
              if (strcmp(tmp,(yyvsp[0].name))!=0)
              {
                n_Read((yyvsp[0].name),&n,coeffs_BIGINT);
                (yyval.lv).rtyp=BIGINT_CMD;
                (yyval.lv).data = n;
              }
            }
          }
#line 2411 "grammar.cc" /* yacc.c:1646  */
    break;

  case 38:
#line 538 "grammar.y" /* yacc.c:1646  */
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp = (yyvsp[0].i);
            (yyval.lv).data = (yyval.lv).Data();
          }
#line 2421 "grammar.cc" /* yacc.c:1646  */
    break;

  case 39:
#line 544 "grammar.y" /* yacc.c:1646  */
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp  = STRING_CMD;
            (yyval.lv).data = (yyvsp[0].name);
          }
#line 2431 "grammar.cc" /* yacc.c:1646  */
    break;

  case 40:
#line 550 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2439 "grammar.cc" /* yacc.c:1646  */
    break;

  case 41:
#line 554 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2447 "grammar.cc" /* yacc.c:1646  */
    break;

  case 42:
#line 558 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2455 "grammar.cc" /* yacc.c:1646  */
    break;

  case 43:
#line 562 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[-2].i))) YYERROR;
          }
#line 2463 "grammar.cc" /* yacc.c:1646  */
    break;

  case 44:
#line 566 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2471 "grammar.cc" /* yacc.c:1646  */
    break;

  case 45:
#line 570 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArithM(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2479 "grammar.cc" /* yacc.c:1646  */
    break;

  case 46:
#line 574 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[-2].i))) YYERROR;
          }
#line 2487 "grammar.cc" /* yacc.c:1646  */
    break;

  case 47:
#line 578 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2495 "grammar.cc" /* yacc.c:1646  */
    break;

  case 48:
#line 582 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2503 "grammar.cc" /* yacc.c:1646  */
    break;

  case 49:
#line 586 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2511 "grammar.cc" /* yacc.c:1646  */
    break;

  case 50:
#line 590 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2519 "grammar.cc" /* yacc.c:1646  */
    break;

  case 51:
#line 594 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),(yyvsp[-5].i),&(yyvsp[-1].lv),TRUE)) YYERROR;
          }
#line 2527 "grammar.cc" /* yacc.c:1646  */
    break;

  case 52:
#line 598 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),(yyvsp[-5].i),&(yyvsp[-1].lv),TRUE)) YYERROR;
          }
#line 2535 "grammar.cc" /* yacc.c:1646  */
    break;

  case 53:
#line 602 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),(yyvsp[-5].i),&(yyvsp[-1].lv),TRUE)) YYERROR;
          }
#line 2543 "grammar.cc" /* yacc.c:1646  */
    break;

  case 54:
#line 606 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),(yyvsp[-5].i),&(yyvsp[-1].lv),TRUE)) YYERROR;
          }
#line 2551 "grammar.cc" /* yacc.c:1646  */
    break;

  case 55:
#line 610 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[-7].i),&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2559 "grammar.cc" /* yacc.c:1646  */
    break;

  case 56:
#line 614 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[-7].i),&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2567 "grammar.cc" /* yacc.c:1646  */
    break;

  case 57:
#line 618 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[-7].i),&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2575 "grammar.cc" /* yacc.c:1646  */
    break;

  case 58:
#line 622 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[-7].i),&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2583 "grammar.cc" /* yacc.c:1646  */
    break;

  case 59:
#line 626 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArithM(&(yyval.lv),NULL,(yyvsp[-2].i))) YYERROR;
          }
#line 2591 "grammar.cc" /* yacc.c:1646  */
    break;

  case 60:
#line 630 "grammar.y" /* yacc.c:1646  */
    {
            int b=iiExprArithM(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i)); // handle branchTo
            if (b==TRUE) YYERROR;
            if (b==2) YYACCEPT;
          }
#line 2601 "grammar.cc" /* yacc.c:1646  */
    break;

  case 61:
#line 636 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),(yyvsp[-7].i),&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2609 "grammar.cc" /* yacc.c:1646  */
    break;

  case 62:
#line 640 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),(yyvsp[-3].i))) YYERROR;
          }
#line 2617 "grammar.cc" /* yacc.c:1646  */
    break;

  case 63:
#line 644 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),RING_CMD,&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2625 "grammar.cc" /* yacc.c:1646  */
    break;

  case 64:
#line 648 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),RING_CMD)) YYERROR;
          }
#line 2633 "grammar.cc" /* yacc.c:1646  */
    break;

  case 65:
#line 652 "grammar.y" /* yacc.c:1646  */
    {
            if (iiARROW(&(yyval.lv),(yyvsp[-2].name),(yyvsp[0].name))) YYERROR;
          }
#line 2641 "grammar.cc" /* yacc.c:1646  */
    break;

  case 66:
#line 659 "grammar.y" /* yacc.c:1646  */
    {
            leftv v = &(yyvsp[-2].lv);
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&((yyvsp[0].lv)),sizeof(sleftv));
            (yyval.lv) = (yyvsp[-2].lv);
          }
#line 2656 "grammar.cc" /* yacc.c:1646  */
    break;

  case 67:
#line 670 "grammar.y" /* yacc.c:1646  */
    {
            (yyval.lv) = (yyvsp[0].lv);
          }
#line 2664 "grammar.cc" /* yacc.c:1646  */
    break;

  case 68:
#line 676 "grammar.y" /* yacc.c:1646  */
    {
            /*if ($1.typ == eunknown) YYERROR;*/
            (yyval.lv) = (yyvsp[0].lv);
          }
#line 2673 "grammar.cc" /* yacc.c:1646  */
    break;

  case 69:
#line 680 "grammar.y" /* yacc.c:1646  */
    { (yyval.lv) = (yyvsp[0].lv); }
#line 2679 "grammar.cc" /* yacc.c:1646  */
    break;

  case 70:
#line 681 "grammar.y" /* yacc.c:1646  */
    { (yyval.lv) = (yyvsp[-1].lv); }
#line 2685 "grammar.cc" /* yacc.c:1646  */
    break;

  case 71:
#line 683 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith3(&(yyval.lv),'[',&(yyvsp[-5].lv),&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
          }
#line 2693 "grammar.cc" /* yacc.c:1646  */
    break;

  case 72:
#line 687 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),'[',&(yyvsp[-1].lv))) YYERROR;
          }
#line 2701 "grammar.cc" /* yacc.c:1646  */
    break;

  case 73:
#line 691 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), (yyvsp[-1].i), NULL)) YYERROR;
          }
#line 2709 "grammar.cc" /* yacc.c:1646  */
    break;

  case 74:
#line 695 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), (yyvsp[-1].i), NULL)) YYERROR;
          }
#line 2717 "grammar.cc" /* yacc.c:1646  */
    break;

  case 75:
#line 699 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), (yyvsp[-1].i), NULL)) YYERROR;
          }
#line 2725 "grammar.cc" /* yacc.c:1646  */
    break;

  case 76:
#line 703 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), (yyvsp[-1].i), NULL)) YYERROR;
          }
#line 2733 "grammar.cc" /* yacc.c:1646  */
    break;

  case 77:
#line 707 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), (yyvsp[-1].i), NULL)) YYERROR;
          }
#line 2741 "grammar.cc" /* yacc.c:1646  */
    break;

  case 78:
#line 711 "grammar.y" /* yacc.c:1646  */
    {
            if (iiApply(&(yyval.lv), &(yyvsp[-3].lv), 0, &(yyvsp[-1].lv))) YYERROR;
          }
#line 2749 "grammar.cc" /* yacc.c:1646  */
    break;

  case 79:
#line 715 "grammar.y" /* yacc.c:1646  */
    {
            (yyval.lv)=(yyvsp[-1].lv);
          }
#line 2757 "grammar.cc" /* yacc.c:1646  */
    break;

  case 80:
#line 719 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq++;
            if (siq>0)
            { if (iiExprArith2(&(yyval.lv),&(yyvsp[-3].lv),'=',&(yyvsp[-1].lv))) YYERROR; }
            else
            #endif
            {
              memset(&(yyval.lv),0,sizeof((yyval.lv)));
              (yyval.lv).rtyp=NONE;
              if (iiAssign(&(yyvsp[-3].lv),&(yyvsp[-1].lv))) YYERROR;
            }
            #ifdef SIQ
            siq--;
            #endif
          }
#line 2778 "grammar.cc" /* yacc.c:1646  */
    break;

  case 81:
#line 736 "grammar.y" /* yacc.c:1646  */
    {
            iiTestAssume(&(yyvsp[-3].lv),&(yyvsp[-1].lv));
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            (yyval.lv).rtyp=NONE;
          }
#line 2788 "grammar.cc" /* yacc.c:1646  */
    break;

  case 82:
#line 742 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq--;
            #endif
          }
#line 2798 "grammar.cc" /* yacc.c:1646  */
    break;

  case 83:
#line 748 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            if (siq<=0) (yyvsp[-1].lv).Eval();
            #endif
            (yyval.lv)=(yyvsp[-1].lv);
            #ifdef SIQ
            siq++;
            #endif
          }
#line 2812 "grammar.cc" /* yacc.c:1646  */
    break;

  case 84:
#line 760 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq++;
            #endif
          }
#line 2822 "grammar.cc" /* yacc.c:1646  */
    break;

  case 85:
#line 768 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq++;
            #endif
          }
#line 2832 "grammar.cc" /* yacc.c:1646  */
    break;

  case 86:
#line 776 "grammar.y" /* yacc.c:1646  */
    {
            #ifdef SIQ
            siq--;
            #endif
          }
#line 2842 "grammar.cc" /* yacc.c:1646  */
    break;

  case 87:
#line 785 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),PLUSPLUS)) YYERROR;
          }
#line 2850 "grammar.cc" /* yacc.c:1646  */
    break;

  case 88:
#line 789 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[-1].lv),MINUSMINUS)) YYERROR;
          }
#line 2858 "grammar.cc" /* yacc.c:1646  */
    break;

  case 89:
#line 793 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),'+',&(yyvsp[0].lv))) YYERROR;
          }
#line 2866 "grammar.cc" /* yacc.c:1646  */
    break;

  case 90:
#line 797 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),'-',&(yyvsp[0].lv))) YYERROR;
          }
#line 2874 "grammar.cc" /* yacc.c:1646  */
    break;

  case 91:
#line 801 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),(yyvsp[-1].i),&(yyvsp[0].lv))) YYERROR;
          }
#line 2882 "grammar.cc" /* yacc.c:1646  */
    break;

  case 92:
#line 805 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),'^',&(yyvsp[0].lv))) YYERROR;
          }
#line 2890 "grammar.cc" /* yacc.c:1646  */
    break;

  case 93:
#line 809 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),(yyvsp[-1].i),&(yyvsp[0].lv))) YYERROR;
          }
#line 2898 "grammar.cc" /* yacc.c:1646  */
    break;

  case 94:
#line 813 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),(yyvsp[-1].i),&(yyvsp[0].lv))) YYERROR;
          }
#line 2906 "grammar.cc" /* yacc.c:1646  */
    break;

  case 95:
#line 817 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),NOTEQUAL,&(yyvsp[0].lv))) YYERROR;
          }
#line 2914 "grammar.cc" /* yacc.c:1646  */
    break;

  case 96:
#line 821 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),EQUAL_EQUAL,&(yyvsp[0].lv))) YYERROR;
          }
#line 2922 "grammar.cc" /* yacc.c:1646  */
    break;

  case 97:
#line 825 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),DOTDOT,&(yyvsp[0].lv))) YYERROR;
          }
#line 2930 "grammar.cc" /* yacc.c:1646  */
    break;

  case 98:
#line 829 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith2(&(yyval.lv),&(yyvsp[-2].lv),':',&(yyvsp[0].lv))) YYERROR;
          }
#line 2938 "grammar.cc" /* yacc.c:1646  */
    break;

  case 99:
#line 833 "grammar.y" /* yacc.c:1646  */
    {
            if (siq>0)
            { if (iiExprArith1(&(yyval.lv),&(yyvsp[0].lv),NOT)) YYERROR; }
            else
            {
              memset(&(yyval.lv),0,sizeof((yyval.lv)));
              int i; TESTSETINT((yyvsp[0].lv),i);
              (yyval.lv).rtyp  = INT_CMD;
              (yyval.lv).data = (void *)(long)(i == 0 ? 1 : 0);
            }
          }
#line 2954 "grammar.cc" /* yacc.c:1646  */
    break;

  case 100:
#line 845 "grammar.y" /* yacc.c:1646  */
    {
            if(iiExprArith1(&(yyval.lv),&(yyvsp[0].lv),'-')) YYERROR;
          }
#line 2962 "grammar.cc" /* yacc.c:1646  */
    break;

  case 101:
#line 851 "grammar.y" /* yacc.c:1646  */
    { (yyval.lv) = (yyvsp[-1].lv); }
#line 2968 "grammar.cc" /* yacc.c:1646  */
    break;

  case 102:
#line 853 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-1].lv).rtyp==0)
            {
              Werror("`%s` is undefined",(yyvsp[-1].lv).Fullname());
              YYERROR;
            }
            else if (((yyvsp[-1].lv).rtyp==MODUL_CMD)
            // matrix m; m[2]=...
            && ((yyvsp[-1].lv).e!=NULL) && ((yyvsp[-1].lv).e->next==NULL))
            {
              MYYERROR("matrix must have 2 indices");
            }
            (yyval.lv) = (yyvsp[-1].lv);
          }
#line 2987 "grammar.cc" /* yacc.c:1646  */
    break;

  case 104:
#line 873 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-1].lv).Typ()!=STRING_CMD)
            {
              MYYERROR("string expression expected");
            }
            (yyval.name) = (char *)(yyvsp[-1].lv).CopyD(STRING_CMD);
            (yyvsp[-1].lv).CleanUp();
          }
#line 3000 "grammar.cc" /* yacc.c:1646  */
    break;

  case 105:
#line 885 "grammar.y" /* yacc.c:1646  */
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&((yyvsp[0].lv).req_packhdl->idroot)))
              YYERROR;
          }
#line 3009 "grammar.cc" /* yacc.c:1646  */
    break;

  case 106:
#line 890 "grammar.y" /* yacc.c:1646  */
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&((yyvsp[0].lv).req_packhdl->idroot)))
              YYERROR;
          }
#line 3018 "grammar.cc" /* yacc.c:1646  */
    break;

  case 107:
#line 895 "grammar.y" /* yacc.c:1646  */
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&(currRing->idroot), TRUE)) YYERROR;
          }
#line 3026 "grammar.cc" /* yacc.c:1646  */
    break;

  case 108:
#line 899 "grammar.y" /* yacc.c:1646  */
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&(currRing->idroot), TRUE)) YYERROR;
          }
#line 3034 "grammar.cc" /* yacc.c:1646  */
    break;

  case 109:
#line 903 "grammar.y" /* yacc.c:1646  */
    {
            int r; TESTSETINT((yyvsp[-4].lv),r);
            int c; TESTSETINT((yyvsp[-1].lv),c);
            leftv v;
            idhdl h;
            if ((yyvsp[-7].i) == MATRIX_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[-6].lv),myynest,(yyvsp[-7].i),&(currRing->idroot), TRUE)) YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              idDelete(&IDIDEAL(h));
              IDMATRIX(h) = mpNew(r,c);
              if (IDMATRIX(h)==NULL) YYERROR;
            }
            else if ((yyvsp[-7].i) == INTMAT_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[-6].lv),myynest,(yyvsp[-7].i),&((yyvsp[-6].lv).req_packhdl->idroot)))
                YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              delete IDINTVEC(h);
              IDINTVEC(h) = new intvec(r,c,0);
              if (IDINTVEC(h)==NULL) YYERROR;
            }
            else /* BIGINTMAT_CMD */
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[-6].lv),myynest,(yyvsp[-7].i),&((yyvsp[-6].lv).req_packhdl->idroot)))
                YYERROR;
              v=&(yyval.lv);
              h=(idhdl)v->data;
              delete IDBIMAT(h);
              IDBIMAT(h) = new bigintmat(r, c, coeffs_BIGINT);
              if (IDBIMAT(h)==NULL) YYERROR;
            }
          }
#line 3074 "grammar.cc" /* yacc.c:1646  */
    break;

  case 110:
#line 939 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-1].i) == MATRIX_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&(currRing->idroot), TRUE)) YYERROR;
            }
            else if ((yyvsp[-1].i) == INTMAT_CMD)
            {
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&((yyvsp[0].lv).req_packhdl->idroot)))
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
              if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&((yyvsp[0].lv).req_packhdl->idroot)))
                YYERROR;
            }
          }
#line 3104 "grammar.cc" /* yacc.c:1646  */
    break;

  case 111:
#line 965 "grammar.y" /* yacc.c:1646  */
    {
            int t=(yyvsp[-2].lv).Typ();
            sleftv r;
            memset(&r,0,sizeof(sleftv));
            if ((BEGIN_RING<t) && (t<END_RING))
            {
              if (iiDeclCommand(&r,&(yyvsp[0].lv),myynest,t,&(currRing->idroot), TRUE))
                YYERROR;
            }
            else
            {
              if (iiDeclCommand(&r,&(yyvsp[0].lv),myynest,t,&((yyvsp[0].lv).req_packhdl->idroot)))
                YYERROR;
            }
            leftv v=&(yyvsp[-2].lv);
            while (v->next!=NULL) v=v->next;
            v->next=(leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&r,sizeof(sleftv));
            (yyval.lv)=(yyvsp[-2].lv);
          }
#line 3129 "grammar.cc" /* yacc.c:1646  */
    break;

  case 112:
#line 986 "grammar.y" /* yacc.c:1646  */
    {
            if (iiDeclCommand(&(yyval.lv),&(yyvsp[0].lv),myynest,(yyvsp[-1].i),&((yyvsp[0].lv).req_packhdl->idroot)))
              YYERROR;
          }
#line 3138 "grammar.cc" /* yacc.c:1646  */
    break;

  case 115:
#line 999 "grammar.y" /* yacc.c:1646  */
    {
            leftv v = &(yyvsp[-3].lv);
            while (v->next!=NULL)
            {
              v=v->next;
            }
            v->next = (leftv)omAllocBin(sleftv_bin);
            memcpy(v->next,&((yyvsp[-1].lv)),sizeof(sleftv));
            (yyval.lv) = (yyvsp[-3].lv);
          }
#line 3153 "grammar.cc" /* yacc.c:1646  */
    break;

  case 116:
#line 1013 "grammar.y" /* yacc.c:1646  */
    {
          // let rInit take care of any errors
          (yyval.i)=rOrderName((yyvsp[0].name));
        }
#line 3162 "grammar.cc" /* yacc.c:1646  */
    break;

  case 117:
#line 1021 "grammar.y" /* yacc.c:1646  */
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            intvec *iv = new intvec(2);
            (*iv)[0] = 1;
            (*iv)[1] = (yyvsp[0].i);
            (yyval.lv).rtyp = INTVEC_CMD;
            (yyval.lv).data = (void *)iv;
          }
#line 3175 "grammar.cc" /* yacc.c:1646  */
    break;

  case 118:
#line 1030 "grammar.y" /* yacc.c:1646  */
    {
            memset(&(yyval.lv),0,sizeof((yyval.lv)));
            leftv sl = &(yyvsp[-1].lv);
            int slLength;
            {
              slLength =  exprlist_length(sl);
              int l = 2 +  slLength;
              intvec *iv = new intvec(l);
              (*iv)[0] = slLength;
              (*iv)[1] = (yyvsp[-3].i);

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
                  (yyvsp[-1].lv).CleanUp();
                  MYYERROR("wrong type in ordering");
                }
                sl = sl->next;
              }
              (yyval.lv).rtyp = INTVEC_CMD;
              (yyval.lv).data = (void *)iv;
            }
            (yyvsp[-1].lv).CleanUp();
          }
#line 3221 "grammar.cc" /* yacc.c:1646  */
    break;

  case 120:
#line 1076 "grammar.y" /* yacc.c:1646  */
    {
            (yyval.lv) = (yyvsp[-2].lv);
            (yyval.lv).next = (sleftv *)omAllocBin(sleftv_bin);
            memcpy((yyval.lv).next,&(yyvsp[0].lv),sizeof(sleftv));
          }
#line 3231 "grammar.cc" /* yacc.c:1646  */
    break;

  case 122:
#line 1086 "grammar.y" /* yacc.c:1646  */
    {
            (yyval.lv) = (yyvsp[-1].lv);
          }
#line 3239 "grammar.cc" /* yacc.c:1646  */
    break;

  case 123:
#line 1092 "grammar.y" /* yacc.c:1646  */
    {
            expected_parms = TRUE;
          }
#line 3247 "grammar.cc" /* yacc.c:1646  */
    break;

  case 124:
#line 1099 "grammar.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3253 "grammar.cc" /* yacc.c:1646  */
    break;

  case 125:
#line 1101 "grammar.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3259 "grammar.cc" /* yacc.c:1646  */
    break;

  case 126:
#line 1103 "grammar.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3265 "grammar.cc" /* yacc.c:1646  */
    break;

  case 127:
#line 1112 "grammar.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].i) != '<') YYERROR;
            if((feFilePending=feFopen((yyvsp[0].name),"r",NULL,TRUE))==NULL) YYERROR; }
#line 3272 "grammar.cc" /* yacc.c:1646  */
    break;

  case 128:
#line 1115 "grammar.y" /* yacc.c:1646  */
    { newFile((yyvsp[-2].name),feFilePending); }
#line 3278 "grammar.cc" /* yacc.c:1646  */
    break;

  case 129:
#line 1120 "grammar.y" /* yacc.c:1646  */
    {
            feHelp((yyvsp[-1].name));
            omFree((ADDRESS)(yyvsp[-1].name));
          }
#line 3287 "grammar.cc" /* yacc.c:1646  */
    break;

  case 130:
#line 1125 "grammar.y" /* yacc.c:1646  */
    {
            feHelp(NULL);
          }
#line 3295 "grammar.cc" /* yacc.c:1646  */
    break;

  case 131:
#line 1132 "grammar.y" /* yacc.c:1646  */
    {
            singular_example((yyvsp[-1].name));
            omFree((ADDRESS)(yyvsp[-1].name));
          }
#line 3304 "grammar.cc" /* yacc.c:1646  */
    break;

  case 132:
#line 1140 "grammar.y" /* yacc.c:1646  */
    {
          if (basePack!=(yyvsp[0].lv).req_packhdl)
          {
            if(iiExport(&(yyvsp[0].lv),0,currPack)) YYERROR;
          }
          else
            if (iiExport(&(yyvsp[0].lv),0)) YYERROR;
        }
#line 3317 "grammar.cc" /* yacc.c:1646  */
    break;

  case 133:
#line 1152 "grammar.y" /* yacc.c:1646  */
    {
          leftv v=&(yyvsp[0].lv);
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
        }
#line 3337 "grammar.cc" /* yacc.c:1646  */
    break;

  case 134:
#line 1168 "grammar.y" /* yacc.c:1646  */
    {
          leftv v=&(yyvsp[0].lv);
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
        }
#line 3357 "grammar.cc" /* yacc.c:1646  */
    break;

  case 135:
#line 1187 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
          }
#line 3365 "grammar.cc" /* yacc.c:1646  */
    break;

  case 136:
#line 1191 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
          }
#line 3373 "grammar.cc" /* yacc.c:1646  */
    break;

  case 137:
#line 1195 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-1].i)==QRING_CMD) (yyvsp[-1].i)=RING_CMD;
            list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
          }
#line 3382 "grammar.cc" /* yacc.c:1646  */
    break;

  case 138:
#line 1200 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
          }
#line 3390 "grammar.cc" /* yacc.c:1646  */
    break;

  case 139:
#line 1204 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd(RING_CMD,NULL,"// ",TRUE);
          }
#line 3398 "grammar.cc" /* yacc.c:1646  */
    break;

  case 140:
#line 1208 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
           }
#line 3406 "grammar.cc" /* yacc.c:1646  */
    break;

  case 141:
#line 1212 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd(PROC_CMD,NULL,"// ",TRUE);
          }
#line 3414 "grammar.cc" /* yacc.c:1646  */
    break;

  case 142:
#line 1216 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd(0,(yyvsp[-1].lv).Fullname(),"// ",TRUE);
            (yyvsp[-1].lv).CleanUp();
          }
#line 3423 "grammar.cc" /* yacc.c:1646  */
    break;

  case 143:
#line 1221 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3433 "grammar.cc" /* yacc.c:1646  */
    break;

  case 144:
#line 1227 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3443 "grammar.cc" /* yacc.c:1646  */
    break;

  case 145:
#line 1233 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3453 "grammar.cc" /* yacc.c:1646  */
    break;

  case 146:
#line 1239 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3463 "grammar.cc" /* yacc.c:1646  */
    break;

  case 147:
#line 1245 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3473 "grammar.cc" /* yacc.c:1646  */
    break;

  case 148:
#line 1251 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3483 "grammar.cc" /* yacc.c:1646  */
    break;

  case 149:
#line 1257 "grammar.y" /* yacc.c:1646  */
    {
            if((yyvsp[-3].lv).Typ() == PACKAGE_CMD)
              list_cmd((yyvsp[-1].i),NULL,"// ",TRUE);
            (yyvsp[-3].lv).CleanUp();
          }
#line 3493 "grammar.cc" /* yacc.c:1646  */
    break;

  case 150:
#line 1269 "grammar.y" /* yacc.c:1646  */
    {
            list_cmd(-1,NULL,"// ",TRUE);
          }
#line 3501 "grammar.cc" /* yacc.c:1646  */
    break;

  case 151:
#line 1275 "grammar.y" /* yacc.c:1646  */
    { yyInRingConstruction = TRUE; }
#line 3507 "grammar.cc" /* yacc.c:1646  */
    break;

  case 152:
#line 1284 "grammar.y" /* yacc.c:1646  */
    {
            const char *ring_name = (yyvsp[-6].lv).name;
            ring b=
            rInit(&(yyvsp[-4].lv),            /* characteristik and list of parameters*/
                  &(yyvsp[-2].lv),            /* names of ringvariables */
                  &(yyvsp[0].lv));            /* ordering */
            idhdl newRingHdl=NULL;

            if (b!=NULL)
            {
              newRingHdl=enterid(ring_name, myynest, RING_CMD,
                                   &((yyvsp[-6].lv).req_packhdl->idroot),FALSE);
              (yyvsp[-6].lv).CleanUp();
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
          }
#line 3544 "grammar.cc" /* yacc.c:1646  */
    break;

  case 153:
#line 1317 "grammar.y" /* yacc.c:1646  */
    {
            const char *ring_name = (yyvsp[0].lv).name;
            if (!inerror) rDefault(ring_name);
            yyInRingConstruction = FALSE;
            (yyvsp[0].lv).CleanUp();
          }
#line 3555 "grammar.cc" /* yacc.c:1646  */
    break;

  case 154:
#line 1324 "grammar.y" /* yacc.c:1646  */
    {
            yyInRingConstruction = FALSE;
            if (iiAssignCR(&(yyvsp[-2].lv),&(yyvsp[0].lv))) YYERROR;
          }
#line 3564 "grammar.cc" /* yacc.c:1646  */
    break;

  case 155:
#line 1329 "grammar.y" /* yacc.c:1646  */
    {
	  #ifdef SINGULAR_4_1
	  yyInRingConstruction = FALSE;
	  sleftv tmp;
	  (yyvsp[-3].lv).next=(leftv)omAlloc(sizeof(sleftv));
	  memcpy((yyvsp[-3].lv).next,&(yyvsp[-1].lv),sizeof(sleftv));
	  memset(&(yyvsp[-1].lv),0,sizeof(sleftv));
	  if (iiExprArithM(&tmp,&(yyvsp[-3].lv),'[')) YYERROR;
          if (iiAssignCR(&(yyvsp[-5].lv),&tmp)) YYERROR;
	  #else
	  YYERROR;
	  #endif
	}
#line 3582 "grammar.cc" /* yacc.c:1646  */
    break;

  case 156:
#line 1346 "grammar.y" /* yacc.c:1646  */
    {
            if (((yyvsp[-1].i)!=LIB_CMD)||(jjLOAD((yyvsp[0].name),TRUE))) YYERROR;
          }
#line 3590 "grammar.cc" /* yacc.c:1646  */
    break;

  case 159:
#line 1355 "grammar.y" /* yacc.c:1646  */
    {
            if (((yyvsp[-1].i)==KEEPRING_CMD) && (myynest==0))
               MYYERROR("only inside a proc allowed");
            const char * n=(yyvsp[0].lv).Name();
            if ((((yyvsp[0].lv).Typ()==RING_CMD)||((yyvsp[0].lv).Typ()==QRING_CMD))
            && ((yyvsp[0].lv).rtyp==IDHDL))
            {
              idhdl h=(idhdl)(yyvsp[0].lv).data;
              if ((yyvsp[0].lv).e!=NULL) h=rFindHdl((ring)(yyvsp[0].lv).Data(),NULL);
              //Print("setring %s lev %d (ptr:%x)\n",IDID(h),IDLEV(h),IDRING(h));
              if ((yyvsp[-1].i)==KEEPRING_CMD)
              {
                if (h!=NULL)
                {
                  if (IDLEV(h)!=0)
                  {
                    if (iiExport(&(yyvsp[0].lv),myynest-1)) YYERROR;
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
                  (yyvsp[0].lv).CleanUp();
                  YYERROR;
                }
              }
              if (h!=NULL) rSetHdl(h);
              else
              {
                Werror("cannot find the name of the basering %s",n);
                (yyvsp[0].lv).CleanUp();
                YYERROR;
              }
              (yyvsp[0].lv).CleanUp();
            }
            else
            {
              Werror("%s is no name of a ring/qring",n);
              (yyvsp[0].lv).CleanUp();
              YYERROR;
            }
          }
#line 3659 "grammar.cc" /* yacc.c:1646  */
    break;

  case 160:
#line 1423 "grammar.y" /* yacc.c:1646  */
    {
            type_cmd(&((yyvsp[0].lv)));
          }
#line 3667 "grammar.cc" /* yacc.c:1646  */
    break;

  case 161:
#line 1427 "grammar.y" /* yacc.c:1646  */
    {
            //Print("typ is %d, rtyp:%d\n",$1.Typ(),$1.rtyp);
            #ifdef SIQ
            if ((yyvsp[0].lv).rtyp!=COMMAND)
            {
            #endif
              if ((yyvsp[0].lv).Typ()==UNKNOWN)
              {
                if ((yyvsp[0].lv).name!=NULL)
                {
                  Werror("`%s` is undefined",(yyvsp[0].lv).name);
                  omFree((ADDRESS)(yyvsp[0].lv).name);
                }
                YYERROR;
              }
            #ifdef SIQ
            }
            #endif
            (yyvsp[0].lv).Print(&sLastPrinted);
            (yyvsp[0].lv).CleanUp(currRing);
            if (errorreported) YYERROR;
          }
#line 3694 "grammar.cc" /* yacc.c:1646  */
    break;

  case 162:
#line 1456 "grammar.y" /* yacc.c:1646  */
    {
            int i; TESTSETINT((yyvsp[-2].lv),i);
            if (i!=0)
            {
              newBuffer( (yyvsp[0].name), BT_if);
            }
            else
            {
              omFree((ADDRESS)(yyvsp[0].name));
              currentVoice->ifsw=1;
            }
          }
#line 3711 "grammar.cc" /* yacc.c:1646  */
    break;

  case 163:
#line 1469 "grammar.y" /* yacc.c:1646  */
    {
            if (currentVoice->ifsw==1)
            {
              currentVoice->ifsw=0;
              newBuffer( (yyvsp[0].name), BT_else);
            }
            else
            {
              if (currentVoice->ifsw!=2)
              {
                Warn("`else` without `if` in level %d",myynest);
              }
              omFree((ADDRESS)(yyvsp[0].name));
            }
            currentVoice->ifsw=0;
          }
#line 3732 "grammar.cc" /* yacc.c:1646  */
    break;

  case 164:
#line 1486 "grammar.y" /* yacc.c:1646  */
    {
            int i; TESTSETINT((yyvsp[-2].lv),i);
            if (i)
            {
              if (exitBuffer(BT_break)) YYERROR;
            }
            currentVoice->ifsw=0;
          }
#line 3745 "grammar.cc" /* yacc.c:1646  */
    break;

  case 165:
#line 1495 "grammar.y" /* yacc.c:1646  */
    {
            if (exitBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          }
#line 3754 "grammar.cc" /* yacc.c:1646  */
    break;

  case 166:
#line 1500 "grammar.y" /* yacc.c:1646  */
    {
            if (contBuffer(BT_break)) YYERROR;
            currentVoice->ifsw=0;
          }
#line 3763 "grammar.cc" /* yacc.c:1646  */
    break;

  case 167:
#line 1508 "grammar.y" /* yacc.c:1646  */
    {
            /* -> if(!$2) break; $3; continue;*/
            char * s = (char *)omAlloc( strlen((yyvsp[-1].name)) + strlen((yyvsp[0].name)) + 36);
            sprintf(s,"whileif (!(%s)) break;\n%scontinue;\n " ,(yyvsp[-1].name),(yyvsp[0].name));
            newBuffer(s,BT_break);
            omFree((ADDRESS)(yyvsp[-1].name));
            omFree((ADDRESS)(yyvsp[0].name));
          }
#line 3776 "grammar.cc" /* yacc.c:1646  */
    break;

  case 168:
#line 1520 "grammar.y" /* yacc.c:1646  */
    {
            /* $2 */
            /* if (!$3) break; $5; $4; continue; */
            char * s = (char *)omAlloc( strlen((yyvsp[-2].name))+strlen((yyvsp[-1].name))+strlen((yyvsp[0].name))+36);
            sprintf(s,"forif (!(%s)) break;\n%s%s;\ncontinue;\n "
                   ,(yyvsp[-2].name),(yyvsp[0].name),(yyvsp[-1].name));
            omFree((ADDRESS)(yyvsp[-2].name));
            omFree((ADDRESS)(yyvsp[-1].name));
            omFree((ADDRESS)(yyvsp[0].name));
            newBuffer(s,BT_break);
            s = (char *)omAlloc( strlen((yyvsp[-3].name)) + 3);
            sprintf(s,"%s;\n",(yyvsp[-3].name));
            omFree((ADDRESS)(yyvsp[-3].name));
            newBuffer(s,BT_if);
          }
#line 3796 "grammar.cc" /* yacc.c:1646  */
    break;

  case 169:
#line 1539 "grammar.y" /* yacc.c:1646  */
    {
            idhdl h = enterid((yyvsp[-1].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL) {omFree((ADDRESS)(yyvsp[-1].name));omFree((ADDRESS)(yyvsp[0].name)); YYERROR;}
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[-1].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[0].name))+31);;
            sprintf(IDPROC(h)->data.s.body,"parameter list #;\n%s;return();\n\n",(yyvsp[0].name));
            omFree((ADDRESS)(yyvsp[0].name));
            omFree((ADDRESS)(yyvsp[-1].name));
          }
#line 3810 "grammar.cc" /* yacc.c:1646  */
    break;

  case 170:
#line 1549 "grammar.y" /* yacc.c:1646  */
    {
            idhdl h = enterid((yyvsp[-2].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)(yyvsp[-2].name));
              omFree((ADDRESS)(yyvsp[-1].name));
              omFree((ADDRESS)(yyvsp[0].name));
              YYERROR;
            }
            char *args=iiProcArgs((yyvsp[-1].name),FALSE);
            omFree((ADDRESS)(yyvsp[-1].name));
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[-2].name), 0, 0);
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[0].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[0].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[0].name));
            omFree((ADDRESS)(yyvsp[-2].name));
          }
#line 3833 "grammar.cc" /* yacc.c:1646  */
    break;

  case 171:
#line 1568 "grammar.y" /* yacc.c:1646  */
    {
            omFree((ADDRESS)(yyvsp[-1].name));
            idhdl h = enterid((yyvsp[-3].name),myynest,PROC_CMD,&IDROOT,TRUE);
            if (h==NULL)
            {
              omFree((ADDRESS)(yyvsp[-3].name));
              omFree((ADDRESS)(yyvsp[-2].name));
              omFree((ADDRESS)(yyvsp[0].name));
              YYERROR;
            }
            char *args=iiProcArgs((yyvsp[-2].name),FALSE);
            omFree((ADDRESS)(yyvsp[-2].name));
            iiInitSingularProcinfo(IDPROC(h),"", (yyvsp[-3].name), 0, 0);
            omFree((ADDRESS)(yyvsp[-3].name));
            IDPROC(h)->data.s.body = (char *)omAlloc(strlen((yyvsp[0].name))+strlen(args)+14);;
            sprintf(IDPROC(h)->data.s.body,"%s\n%s;return();\n\n",args,(yyvsp[0].name));
            omFree((ADDRESS)args);
            omFree((ADDRESS)(yyvsp[0].name));
          }
#line 3857 "grammar.cc" /* yacc.c:1646  */
    break;

  case 172:
#line 1591 "grammar.y" /* yacc.c:1646  */
    {
            // decl. of type proc p(int i)
            if ((yyvsp[-1].i)==PARAMETER)  { if (iiParameter(&(yyvsp[0].lv))) YYERROR; }
            else                { if (iiAlias(&(yyvsp[0].lv))) YYERROR; }
          }
#line 3867 "grammar.cc" /* yacc.c:1646  */
    break;

  case 173:
#line 1597 "grammar.y" /* yacc.c:1646  */
    {
            // decl. of type proc p(i)
            sleftv tmp_expr;
            if ((yyvsp[-1].i)==ALIAS_CMD) MYYERROR("alias requires a type");
            if ((iiDeclCommand(&tmp_expr,&(yyvsp[0].lv),myynest,DEF_CMD,&IDROOT))
            || (iiParameter(&tmp_expr)))
              YYERROR;
          }
#line 3880 "grammar.cc" /* yacc.c:1646  */
    break;

  case 174:
#line 1609 "grammar.y" /* yacc.c:1646  */
    {
            iiRETURNEXPR.Copy(&(yyvsp[-1].lv));
            (yyvsp[-1].lv).CleanUp();
            if (exitBuffer(BT_proc)) YYERROR;
          }
#line 3890 "grammar.cc" /* yacc.c:1646  */
    break;

  case 175:
#line 1615 "grammar.y" /* yacc.c:1646  */
    {
            if ((yyvsp[-2].i)==RETURN)
            {
              iiRETURNEXPR.Init();
              iiRETURNEXPR.rtyp=NONE;
              if (exitBuffer(BT_proc)) YYERROR;
            }
          }
#line 3903 "grammar.cc" /* yacc.c:1646  */
    break;


#line 3907 "grammar.cc" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
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

  /* Do not reclaim the symbols of the rule whose action triggered
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
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
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

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


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

#if !defined yyoverflow || YYERROR_VERBOSE
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
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
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
  return yyresult;
}
