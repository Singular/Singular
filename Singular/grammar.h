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
     ASSUME_CMD = 373,
     BREAK_CMD = 374,
     CONTINUE_CMD = 375,
     ELSE_CMD = 376,
     EVAL = 377,
     QUOTE = 378,
     FOR_CMD = 379,
     IF_CMD = 380,
     SYS_BREAK = 381,
     WHILE_CMD = 382,
     RETURN = 383,
     PARAMETER = 384,
     SYSVAR = 385,
     UMINUS = 386
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




