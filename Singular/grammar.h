/* A Bison parser, made by GNU Bison 2.4.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C
   
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
     IDEAL_CMD = 274,
     MAP_CMD = 275,
     MATRIX_CMD = 276,
     MODUL_CMD = 277,
     NUMBER_CMD = 278,
     POLY_CMD = 279,
     RESOLUTION_CMD = 280,
     VECTOR_CMD = 281,
     BETTI_CMD = 282,
     COEFFS_CMD = 283,
     COEF_CMD = 284,
     CONTRACT_CMD = 285,
     DEGREE_CMD = 286,
     DEG_CMD = 287,
     DIFF_CMD = 288,
     DIM_CMD = 289,
     DIVISION_CMD = 290,
     ELIMINATION_CMD = 291,
     E_CMD = 292,
     FAREY_CMD = 293,
     FETCH_CMD = 294,
     FREEMODULE_CMD = 295,
     KEEPRING_CMD = 296,
     HILBERT_CMD = 297,
     HOMOG_CMD = 298,
     IMAP_CMD = 299,
     INDEPSET_CMD = 300,
     INTERRED_CMD = 301,
     INTERSECT_CMD = 302,
     JACOB_CMD = 303,
     JET_CMD = 304,
     KBASE_CMD = 305,
     KOSZUL_CMD = 306,
     LEADCOEF_CMD = 307,
     LEADEXP_CMD = 308,
     LEAD_CMD = 309,
     LEADMONOM_CMD = 310,
     LIFTSTD_CMD = 311,
     LIFT_CMD = 312,
     MAXID_CMD = 313,
     MINBASE_CMD = 314,
     MINOR_CMD = 315,
     MINRES_CMD = 316,
     MODULO_CMD = 317,
     MONOM_CMD = 318,
     MRES_CMD = 319,
     MULTIPLICITY_CMD = 320,
     ORD_CMD = 321,
     PAR_CMD = 322,
     PARDEG_CMD = 323,
     PREIMAGE_CMD = 324,
     QUOTIENT_CMD = 325,
     QHWEIGHT_CMD = 326,
     REDUCE_CMD = 327,
     REGULARITY_CMD = 328,
     RES_CMD = 329,
     SBA_CMD = 330,
     SIMPLIFY_CMD = 331,
     SORTVEC_CMD = 332,
     SRES_CMD = 333,
     STD_CMD = 334,
     SUBST_CMD = 335,
     SYZYGY_CMD = 336,
     VAR_CMD = 337,
     VDIM_CMD = 338,
     WEDGE_CMD = 339,
     WEIGHT_CMD = 340,
     VALTVARS = 341,
     VMAXDEG = 342,
     VMAXMULT = 343,
     VNOETHER = 344,
     VMINPOLY = 345,
     END_RING = 346,
     CMD_1 = 347,
     CMD_2 = 348,
     CMD_3 = 349,
     CMD_12 = 350,
     CMD_13 = 351,
     CMD_23 = 352,
     CMD_123 = 353,
     CMD_M = 354,
     ROOT_DECL = 355,
     ROOT_DECL_LIST = 356,
     RING_DECL = 357,
     RING_DECL_LIST = 358,
     EXAMPLE_CMD = 359,
     EXPORT_CMD = 360,
     HELP_CMD = 361,
     KILL_CMD = 362,
     LIB_CMD = 363,
     LISTVAR_CMD = 364,
     SETRING_CMD = 365,
     TYPE_CMD = 366,
     STRINGTOK = 367,
     BLOCKTOK = 368,
     INT_CONST = 369,
     UNKNOWN_IDENT = 370,
     RINGVAR = 371,
     PROC_DEF = 372,
     APPLY = 373,
     ASSUME_CMD = 374,
     BREAK_CMD = 375,
     CONTINUE_CMD = 376,
     ELSE_CMD = 377,
     EVAL = 378,
     QUOTE = 379,
     FOR_CMD = 380,
     IF_CMD = 381,
     SYS_BREAK = 382,
     WHILE_CMD = 383,
     RETURN = 384,
     PARAMETER = 385,
     SYSVAR = 386,
     UMINUS = 387
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




