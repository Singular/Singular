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



#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
#endif




