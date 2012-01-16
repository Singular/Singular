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
     INTMAT_CMD = 268,
     PROC_CMD = 269,
     RING_CMD = 270,
     BEGIN_RING = 271,
     IDEAL_CMD = 272,
     MAP_CMD = 273,
     MATRIX_CMD = 274,
     MODUL_CMD = 275,
     NUMBER_CMD = 276,
     POLY_CMD = 277,
     RESOLUTION_CMD = 278,
     VECTOR_CMD = 279,
     BETTI_CMD = 280,
     COEFFS_CMD = 281,
     COEF_CMD = 282,
     CONTRACT_CMD = 283,
     DEGREE_CMD = 284,
     DEG_CMD = 285,
     DIFF_CMD = 286,
     DIM_CMD = 287,
     DIVISION_CMD = 288,
     ELIMINATION_CMD = 289,
     E_CMD = 290,
     FAREY_CMD = 291,
     FETCH_CMD = 292,
     FREEMODULE_CMD = 293,
     KEEPRING_CMD = 294,
     HILBERT_CMD = 295,
     HOMOG_CMD = 296,
     IMAP_CMD = 297,
     INDEPSET_CMD = 298,
     INTERRED_CMD = 299,
     INTERSECT_CMD = 300,
     JACOB_CMD = 301,
     JET_CMD = 302,
     KBASE_CMD = 303,
     KOSZUL_CMD = 304,
     LEADCOEF_CMD = 305,
     LEADEXP_CMD = 306,
     LEAD_CMD = 307,
     LEADMONOM_CMD = 308,
     LIFTSTD_CMD = 309,
     LIFT_CMD = 310,
     MAXID_CMD = 311,
     MINBASE_CMD = 312,
     MINOR_CMD = 313,
     MINRES_CMD = 314,
     MODULO_CMD = 315,
     MONOM_CMD = 316,
     MRES_CMD = 317,
     MULTIPLICITY_CMD = 318,
     ORD_CMD = 319,
     PAR_CMD = 320,
     PARDEG_CMD = 321,
     PREIMAGE_CMD = 322,
     QUOTIENT_CMD = 323,
     QHWEIGHT_CMD = 324,
     REDUCE_CMD = 325,
     REGULARITY_CMD = 326,
     RES_CMD = 327,
     SIMPLIFY_CMD = 328,
     SORTVEC_CMD = 329,
     SRES_CMD = 330,
     STD_CMD = 331,
     SUBST_CMD = 332,
     SYZYGY_CMD = 333,
     VAR_CMD = 334,
     VDIM_CMD = 335,
     WEDGE_CMD = 336,
     WEIGHT_CMD = 337,
     VALTVARS = 338,
     VMAXDEG = 339,
     VMAXMULT = 340,
     VNOETHER = 341,
     VMINPOLY = 342,
     END_RING = 343,
     CMD_1 = 344,
     CMD_2 = 345,
     CMD_3 = 346,
     CMD_12 = 347,
     CMD_13 = 348,
     CMD_23 = 349,
     CMD_123 = 350,
     CMD_M = 351,
     ROOT_DECL = 352,
     ROOT_DECL_LIST = 353,
     RING_DECL = 354,
     RING_DECL_LIST = 355,
     EXAMPLE_CMD = 356,
     EXPORT_CMD = 357,
     HELP_CMD = 358,
     KILL_CMD = 359,
     LIB_CMD = 360,
     LISTVAR_CMD = 361,
     SETRING_CMD = 362,
     TYPE_CMD = 363,
     STRINGTOK = 364,
     BLOCKTOK = 365,
     INT_CONST = 366,
     UNKNOWN_IDENT = 367,
     RINGVAR = 368,
     PROC_DEF = 369,
     BREAK_CMD = 370,
     CONTINUE_CMD = 371,
     ELSE_CMD = 372,
     EVAL = 373,
     QUOTE = 374,
     FOR_CMD = 375,
     IF_CMD = 376,
     SYS_BREAK = 377,
     WHILE_CMD = 378,
     RETURN = 379,
     PARAMETER = 380,
     SYSVAR = 381,
     UMINUS = 382
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
#define INTMAT_CMD 268
#define PROC_CMD 269
#define RING_CMD 270
#define BEGIN_RING 271
#define IDEAL_CMD 272
#define MAP_CMD 273
#define MATRIX_CMD 274
#define MODUL_CMD 275
#define NUMBER_CMD 276
#define POLY_CMD 277
#define RESOLUTION_CMD 278
#define VECTOR_CMD 279
#define BETTI_CMD 280
#define COEFFS_CMD 281
#define COEF_CMD 282
#define CONTRACT_CMD 283
#define DEGREE_CMD 284
#define DEG_CMD 285
#define DIFF_CMD 286
#define DIM_CMD 287
#define DIVISION_CMD 288
#define ELIMINATION_CMD 289
#define E_CMD 290
#define FAREY_CMD 291
#define FETCH_CMD 292
#define FREEMODULE_CMD 293
#define KEEPRING_CMD 294
#define HILBERT_CMD 295
#define HOMOG_CMD 296
#define IMAP_CMD 297
#define INDEPSET_CMD 298
#define INTERRED_CMD 299
#define INTERSECT_CMD 300
#define JACOB_CMD 301
#define JET_CMD 302
#define KBASE_CMD 303
#define KOSZUL_CMD 304
#define LEADCOEF_CMD 305
#define LEADEXP_CMD 306
#define LEAD_CMD 307
#define LEADMONOM_CMD 308
#define LIFTSTD_CMD 309
#define LIFT_CMD 310
#define MAXID_CMD 311
#define MINBASE_CMD 312
#define MINOR_CMD 313
#define MINRES_CMD 314
#define MODULO_CMD 315
#define MONOM_CMD 316
#define MRES_CMD 317
#define MULTIPLICITY_CMD 318
#define ORD_CMD 319
#define PAR_CMD 320
#define PARDEG_CMD 321
#define PREIMAGE_CMD 322
#define QUOTIENT_CMD 323
#define QHWEIGHT_CMD 324
#define REDUCE_CMD 325
#define REGULARITY_CMD 326
#define RES_CMD 327
#define SIMPLIFY_CMD 328
#define SORTVEC_CMD 329
#define SRES_CMD 330
#define STD_CMD 331
#define SUBST_CMD 332
#define SYZYGY_CMD 333
#define VAR_CMD 334
#define VDIM_CMD 335
#define WEDGE_CMD 336
#define WEIGHT_CMD 337
#define VALTVARS 338
#define VMAXDEG 339
#define VMAXMULT 340
#define VNOETHER 341
#define VMINPOLY 342
#define END_RING 343
#define CMD_1 344
#define CMD_2 345
#define CMD_3 346
#define CMD_12 347
#define CMD_13 348
#define CMD_23 349
#define CMD_123 350
#define CMD_M 351
#define ROOT_DECL 352
#define ROOT_DECL_LIST 353
#define RING_DECL 354
#define RING_DECL_LIST 355
#define EXAMPLE_CMD 356
#define EXPORT_CMD 357
#define HELP_CMD 358
#define KILL_CMD 359
#define LIB_CMD 360
#define LISTVAR_CMD 361
#define SETRING_CMD 362
#define TYPE_CMD 363
#define STRINGTOK 364
#define BLOCKTOK 365
#define INT_CONST 366
#define UNKNOWN_IDENT 367
#define RINGVAR 368
#define PROC_DEF 369
#define BREAK_CMD 370
#define CONTINUE_CMD 371
#define ELSE_CMD 372
#define EVAL 373
#define QUOTE 374
#define FOR_CMD 375
#define IF_CMD 376
#define SYS_BREAK 377
#define WHILE_CMD 378
#define RETURN 379
#define PARAMETER 380
#define SYSVAR 381
#define UMINUS 382




#if ! defined (YYSTYPE) && ! defined (YYSTYPE_IS_DECLARED)
typedef int YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif





