/* A Bison parser, made by GNU Bison 1.875d.  */

/* Skeleton parser for Yacc-like parsing with Bison,
   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004 Free Software Foundation, Inc.

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
   Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

/* As a special exception, when this file is copied by Bison into a
   Bison output file, you may use that output file without restriction.
   This special exception was added by the Free Software Foundation
   in version 1.24 of Bison.  */

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
     SIMPLIFY_CMD = 329,
     SORTVEC_CMD = 330,
     SRES_CMD = 331,
     STD_CMD = 332,
     SUBST_CMD = 333,
     SYZYGY_CMD = 334,
     VAR_CMD = 335,
     VDIM_CMD = 336,
     WEDGE_CMD = 337,
     WEIGHT_CMD = 338,
     VALTVARS = 339,
     VMAXDEG = 340,
     VMAXMULT = 341,
     VNOETHER = 342,
     VMINPOLY = 343,
     END_RING = 344,
     CMD_1 = 345,
     CMD_2 = 346,
     CMD_3 = 347,
     CMD_12 = 348,
     CMD_13 = 349,
     CMD_23 = 350,
     CMD_123 = 351,
     CMD_M = 352,
     ROOT_DECL = 353,
     ROOT_DECL_LIST = 354,
     RING_DECL = 355,
     RING_DECL_LIST = 356,
     EXAMPLE_CMD = 357,
     EXPORT_CMD = 358,
     HELP_CMD = 359,
     KILL_CMD = 360,
     LIB_CMD = 361,
     LISTVAR_CMD = 362,
     SETRING_CMD = 363,
     TYPE_CMD = 364,
     STRINGTOK = 365,
     BLOCKTOK = 366,
     INT_CONST = 367,
     UNKNOWN_IDENT = 368,
     RINGVAR = 369,
     PROC_DEF = 370,
     BREAK_CMD = 371,
     CONTINUE_CMD = 372,
     ELSE_CMD = 373,
     EVAL = 374,
     QUOTE = 375,
     FOR_CMD = 376,
     IF_CMD = 377,
     SYS_BREAK = 378,
     WHILE_CMD = 379,
     RETURN = 380,
     PARAMETER = 381,
     SYSVAR = 382,
     UMINUS = 383
   };
#endif
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
#define BIGINTMAT_CMD 268
#define INTMAT_CMD 269
#define PROC_CMD 270
#define RING_CMD 271
#define BEGIN_RING 272
#define IDEAL_CMD 273
#define MAP_CMD 274
#define MATRIX_CMD 275
#define MODUL_CMD 276
#define NUMBER_CMD 277
#define POLY_CMD 278
#define RESOLUTION_CMD 279
#define VECTOR_CMD 280
#define BETTI_CMD 281
#define COEFFS_CMD 282
#define COEF_CMD 283
#define CONTRACT_CMD 284
#define DEGREE_CMD 285
#define DEG_CMD 286
#define DIFF_CMD 287
#define DIM_CMD 288
#define DIVISION_CMD 289
#define ELIMINATION_CMD 290
#define E_CMD 291
#define FAREY_CMD 292
#define FETCH_CMD 293
#define FREEMODULE_CMD 294
#define KEEPRING_CMD 295
#define HILBERT_CMD 296
#define HOMOG_CMD 297
#define IMAP_CMD 298
#define INDEPSET_CMD 299
#define INTERRED_CMD 300
#define INTERSECT_CMD 301
#define JACOB_CMD 302
#define JET_CMD 303
#define KBASE_CMD 304
#define KOSZUL_CMD 305
#define LEADCOEF_CMD 306
#define LEADEXP_CMD 307
#define LEAD_CMD 308
#define LEADMONOM_CMD 309
#define LIFTSTD_CMD 310
#define LIFT_CMD 311
#define MAXID_CMD 312
#define MINBASE_CMD 313
#define MINOR_CMD 314
#define MINRES_CMD 315
#define MODULO_CMD 316
#define MONOM_CMD 317
#define MRES_CMD 318
#define MULTIPLICITY_CMD 319
#define ORD_CMD 320
#define PAR_CMD 321
#define PARDEG_CMD 322
#define PREIMAGE_CMD 323
#define QUOTIENT_CMD 324
#define QHWEIGHT_CMD 325
#define REDUCE_CMD 326
#define REGULARITY_CMD 327
#define RES_CMD 328
#define SIMPLIFY_CMD 329
#define SORTVEC_CMD 330
#define SRES_CMD 331
#define STD_CMD 332
#define SUBST_CMD 333
#define SYZYGY_CMD 334
#define VAR_CMD 335
#define VDIM_CMD 336
#define WEDGE_CMD 337
#define WEIGHT_CMD 338
#define VALTVARS 339
#define VMAXDEG 340
#define VMAXMULT 341
#define VNOETHER 342
#define VMINPOLY 343
#define END_RING 344
#define CMD_1 345
#define CMD_2 346
#define CMD_3 347
#define CMD_12 348
#define CMD_13 349
#define CMD_23 350
#define CMD_123 351
#define CMD_M 352
#define ROOT_DECL 353
#define ROOT_DECL_LIST 354
#define RING_DECL 355
#define RING_DECL_LIST 356
#define EXAMPLE_CMD 357
#define EXPORT_CMD 358
#define HELP_CMD 359
#define KILL_CMD 360
#define LIB_CMD 361
#define LISTVAR_CMD 362
#define SETRING_CMD 363
#define TYPE_CMD 364
#define STRINGTOK 365
#define BLOCKTOK 366
#define INT_CONST 367
#define UNKNOWN_IDENT 368
#define RINGVAR 369
#define PROC_DEF 370
#define BREAK_CMD 371
#define CONTINUE_CMD 372
#define ELSE_CMD 373
#define EVAL 374
#define QUOTE 375
#define FOR_CMD 376
#define IF_CMD 377
#define SYS_BREAK 378
#define WHILE_CMD 379
#define RETURN 380
#define PARAMETER 381
#define SYSVAR 382
#define UMINUS 383




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





