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
     BUCKET_CMD = 274,
     IDEAL_CMD = 275,
     MAP_CMD = 276,
     MATRIX_CMD = 277,
     MODUL_CMD = 278,
     NUMBER_CMD = 279,
     POLY_CMD = 280,
     RESOLUTION_CMD = 281,
     VECTOR_CMD = 282,
     BETTI_CMD = 283,
     E_CMD = 284,
     FETCH_CMD = 285,
     FREEMODULE_CMD = 286,
     KEEPRING_CMD = 287,
     IMAP_CMD = 288,
     KOSZUL_CMD = 289,
     MAXID_CMD = 290,
     MONOM_CMD = 291,
     PAR_CMD = 292,
     PREIMAGE_CMD = 293,
     VAR_CMD = 294,
     VALTVARS = 295,
     VMAXDEG = 296,
     VMAXMULT = 297,
     VNOETHER = 298,
     VMINPOLY = 299,
     END_RING = 300,
     CMD_1 = 301,
     CMD_2 = 302,
     CMD_3 = 303,
     CMD_12 = 304,
     CMD_13 = 305,
     CMD_23 = 306,
     CMD_123 = 307,
     CMD_M = 308,
     ROOT_DECL = 309,
     ROOT_DECL_LIST = 310,
     RING_DECL = 311,
     RING_DECL_LIST = 312,
     EXAMPLE_CMD = 313,
     EXPORT_CMD = 314,
     HELP_CMD = 315,
     KILL_CMD = 316,
     LIB_CMD = 317,
     LISTVAR_CMD = 318,
     SETRING_CMD = 319,
     TYPE_CMD = 320,
     STRINGTOK = 321,
     BLOCKTOK = 322,
     INT_CONST = 323,
     UNKNOWN_IDENT = 324,
     RINGVAR = 325,
     PROC_DEF = 326,
     APPLY = 327,
     ASSUME_CMD = 328,
     BREAK_CMD = 329,
     CONTINUE_CMD = 330,
     ELSE_CMD = 331,
     EVAL = 332,
     QUOTE = 333,
     FOR_CMD = 334,
     IF_CMD = 335,
     SYS_BREAK = 336,
     WHILE_CMD = 337,
     RETURN = 338,
     PARAMETER = 339,
     SYSVAR = 340,
     UMINUS = 341
   };
#endif



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




