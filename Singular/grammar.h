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




