/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton implementation for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

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
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

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
#define YYBISON_VERSION "2.3"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Using locations.  */
#define YYLSP_NEEDED 1



/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     ID = 258,
     STRING_LIT = 259,
     CHAR_LIT = 260,
     INT_VAL = 261,
     FLOAT_VAL = 262,
     POUND = 263,
     COMMA = 264,
     COLON = 265,
     SEMICOLON = 266,
     LPAREN = 267,
     RPAREN = 268,
     LBRACK = 269,
     RBRACK = 270,
     LBRACE = 271,
     RBRACE = 272,
     DOT = 273,
     PLUS = 274,
     MINUS = 275,
     TIMES = 276,
     DIVIDE = 277,
     PERCENT = 278,
     EQ = 279,
     NEQ = 280,
     LT = 281,
     LE = 282,
     GT = 283,
     GE = 284,
     AND = 285,
     OR = 286,
     ASSIGN = 287,
     IF = 288,
     THEN = 289,
     ELSE = 290,
     WHILE = 291,
     FOR = 292,
     DO = 293,
     LOOP = 294,
     BREAK = 295,
     CONTINUE = 296,
     NULL_TOK = 297,
     FUNCTION = 298,
     RETURN = 299,
     QUESTION = 300,
     EXCLAMATION = 301,
     S_OR = 302,
     S_AND = 303,
     S_XOR = 304,
     PLUSPLUS = 305,
     MINUSMINUS = 306,
     DOLLAR = 307,
     POUNDPAREN = 308,
     PERCENTPAREN = 309,
     ATPAREN = 310,
     SIMULT = 311,
     PATTERN = 312,
     CODE = 313,
     TRANSPORT = 314,
     HOST = 315,
     TIME = 316,
     WHENEVER = 317,
     NEXT = 318,
     UNTIL = 319,
     EXTERNAL = 320,
     GLOBAL = 321,
     EVERY = 322,
     BEFORE = 323,
     AFTER = 324,
     AT = 325,
     AT_SYM = 326,
     ATAT_SYM = 327,
     NEW = 328,
     SIZEOF = 329,
     TYPEOF = 330,
     SAME = 331,
     PLUS_CHUCK = 332,
     MINUS_CHUCK = 333,
     TIMES_CHUCK = 334,
     DIVIDE_CHUCK = 335,
     S_AND_CHUCK = 336,
     S_OR_CHUCK = 337,
     S_XOR_CHUCK = 338,
     SHIFT_RIGHT_CHUCK = 339,
     SHIFT_LEFT_CHUCK = 340,
     PERCENT_CHUCK = 341,
     SHIFT_RIGHT = 342,
     SHIFT_LEFT = 343,
     TILDA = 344,
     CHUCK = 345,
     COLONCOLON = 346,
     S_CHUCK = 347,
     AT_CHUCK = 348,
     LEFT_S_CHUCK = 349,
     UNCHUCK = 350,
     UPCHUCK = 351,
     DOWNCHUCK = 352,
     CLASS = 353,
     INTERFACE = 354,
     EXTENDS = 355,
     IMPLEMENTS = 356,
     PUBLIC = 357,
     PROTECTED = 358,
     PRIVATE = 359,
     STATIC = 360,
     ABSTRACT = 361,
     CONST = 362,
     SPORK = 363,
     ARROW_RIGHT = 364,
     ARROW_LEFT = 365,
     L_HACK = 366,
     R_HACK = 367,
     GRUCK_RIGHT = 368,
     GRUCK_LEFT = 369,
     UNGRUCK_RIGHT = 370,
     UNGRUCK_LEFT = 371,
     AT_OP = 372,
     AT_CTOR = 373,
     AT_DTOR = 374,
     AT_IMPORT = 375,
     AT_DOC = 376
   };
#endif
/* Tokens.  */
#define ID 258
#define STRING_LIT 259
#define CHAR_LIT 260
#define INT_VAL 261
#define FLOAT_VAL 262
#define POUND 263
#define COMMA 264
#define COLON 265
#define SEMICOLON 266
#define LPAREN 267
#define RPAREN 268
#define LBRACK 269
#define RBRACK 270
#define LBRACE 271
#define RBRACE 272
#define DOT 273
#define PLUS 274
#define MINUS 275
#define TIMES 276
#define DIVIDE 277
#define PERCENT 278
#define EQ 279
#define NEQ 280
#define LT 281
#define LE 282
#define GT 283
#define GE 284
#define AND 285
#define OR 286
#define ASSIGN 287
#define IF 288
#define THEN 289
#define ELSE 290
#define WHILE 291
#define FOR 292
#define DO 293
#define LOOP 294
#define BREAK 295
#define CONTINUE 296
#define NULL_TOK 297
#define FUNCTION 298
#define RETURN 299
#define QUESTION 300
#define EXCLAMATION 301
#define S_OR 302
#define S_AND 303
#define S_XOR 304
#define PLUSPLUS 305
#define MINUSMINUS 306
#define DOLLAR 307
#define POUNDPAREN 308
#define PERCENTPAREN 309
#define ATPAREN 310
#define SIMULT 311
#define PATTERN 312
#define CODE 313
#define TRANSPORT 314
#define HOST 315
#define TIME 316
#define WHENEVER 317
#define NEXT 318
#define UNTIL 319
#define EXTERNAL 320
#define GLOBAL 321
#define EVERY 322
#define BEFORE 323
#define AFTER 324
#define AT 325
#define AT_SYM 326
#define ATAT_SYM 327
#define NEW 328
#define SIZEOF 329
#define TYPEOF 330
#define SAME 331
#define PLUS_CHUCK 332
#define MINUS_CHUCK 333
#define TIMES_CHUCK 334
#define DIVIDE_CHUCK 335
#define S_AND_CHUCK 336
#define S_OR_CHUCK 337
#define S_XOR_CHUCK 338
#define SHIFT_RIGHT_CHUCK 339
#define SHIFT_LEFT_CHUCK 340
#define PERCENT_CHUCK 341
#define SHIFT_RIGHT 342
#define SHIFT_LEFT 343
#define TILDA 344
#define CHUCK 345
#define COLONCOLON 346
#define S_CHUCK 347
#define AT_CHUCK 348
#define LEFT_S_CHUCK 349
#define UNCHUCK 350
#define UPCHUCK 351
#define DOWNCHUCK 352
#define CLASS 353
#define INTERFACE 354
#define EXTENDS 355
#define IMPLEMENTS 356
#define PUBLIC 357
#define PROTECTED 358
#define PRIVATE 359
#define STATIC 360
#define ABSTRACT 361
#define CONST 362
#define SPORK 363
#define ARROW_RIGHT 364
#define ARROW_LEFT 365
#define L_HACK 366
#define R_HACK 367
#define GRUCK_RIGHT 368
#define GRUCK_LEFT 369
#define UNGRUCK_RIGHT 370
#define UNGRUCK_LEFT 371
#define AT_OP 372
#define AT_CTOR 373
#define AT_DTOR 374
#define AT_IMPORT 375
#define AT_DOC 376




/* Copy the first part of user declarations.  */
#line 1 "chuck.y"


/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler and Virtual Machine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.cs.princeton.edu/
    http://soundlab.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307
  U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck.tab.c
// desc: chuck parser
//
// author: Ge Wang (gewang@cs.princeton.edu) - generated by yacc
//         Perry R. Cook (prc@cs.princeton.edu)
//
// initial version created by Ge Wang;
// based on ansi C grammar by Jeff Lee, maintained by Jutta Degener
//
// date: Summer 2002
//-----------------------------------------------------------------------------
#include "chuck_absyn.h"
#include "chuck_errmsg.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// function
int yylex( void );

void yyerror( char * s )
{
    EM_error( EM_tokPos, "%s", s );
}

a_Program g_program = NULL;



/* Enabling traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
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

#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 77 "chuck.y"
{
    int pos;
    int ival;
    double fval;
    c_str sval;

    a_Program program;
    a_Section program_section;
    a_Stmt_List stmt_list;
    a_Class_Def class_def;
    a_Class_Ext class_ext;
    a_Class_Body class_body;
    a_Stmt stmt;
    a_Exp exp;
    a_Func_Def func_def;
    a_Var_Decl_List var_decl_list;
    a_Var_Decl var_decl;
    a_Type_Decl type_decl;
    a_Arg_List arg_list;
    a_Id_List id_list;
    a_Array_Sub array_sub;
    a_Complex complex_exp;
    a_Polar polar_exp;
    a_Vec vec_exp; // ge: added 1.3.5.3
    a_Import import; // 1.5.4.0 (ge) added
    a_Doc doc; // 1.5.4.4 (ge) added
}
/* Line 193 of yacc.c.  */
#line 423 "chuck.tab.c"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
# define yyltype YYLTYPE /* obsolescent; will be withdrawn */
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


/* Copy the second part of user declarations.  */


/* Line 216 of yacc.c.  */
#line 448 "chuck.tab.c"

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
YYID (int i)
#else
static int
YYID (i)
    int i;
#endif
{
  return i;
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
	 || (defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL \
	     && defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss;
  YYSTYPE yyvs;
    YYLTYPE yyls;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE) + sizeof (YYLTYPE)) \
      + 2 * YYSTACK_GAP_MAXIMUM)

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
# define YYSTACK_RELOCATE(Stack)					\
    do									\
      {									\
	YYSIZE_T yynewbytes;						\
	YYCOPY (&yyptr->Stack, Stack, yysize);				\
	Stack = &yyptr->Stack;						\
	yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
	yyptr += yynewbytes / sizeof (*yyptr);				\
      }									\
    while (YYID (0))

#endif

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  135
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2373

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  122
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  65
/* YYNRULES -- Number of rules.  */
#define YYNRULES  274
/* YYNRULES -- Number of states.  */
#define YYNSTATES  463

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   376

#define YYTRANSLATE(YYX)						\
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[YYLEX] -- Bison symbol number corresponding to YYLEX.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
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
     115,   116,   117,   118,   119,   120,   121
};

#if YYDEBUG
/* YYPRHS[YYN] -- Index of the first RHS symbol of rule number YYN in
   YYRHS.  */
static const yytype_uint16 yyprhs[] =
{
       0,     0,     3,     5,     8,    10,    12,    14,    21,    29,
      36,    44,    47,    52,    55,    60,    62,    63,    65,    68,
      70,    72,    74,    77,    79,    83,    85,    89,    98,   106,
     114,   121,   128,   134,   140,   147,   156,   164,   174,   184,
     196,   208,   210,   212,   213,   215,   217,   219,   221,   223,
     225,   226,   228,   231,   235,   240,   242,   244,   246,   249,
     252,   257,   259,   262,   264,   266,   268,   270,   272,   274,
     276,   279,   283,   286,   289,   295,   303,   309,   317,   324,
     332,   340,   346,   354,   360,   363,   367,   370,   374,   378,
     383,   388,   390,   394,   396,   399,   404,   409,   411,   415,
     417,   419,   422,   424,   428,   430,   434,   436,   440,   444,
     449,   454,   460,   463,   467,   469,   472,   476,   480,   484,
     487,   491,   493,   497,   499,   502,   505,   509,   514,   519,
     525,   529,   533,   537,   539,   541,   543,   545,   547,   549,
     551,   553,   555,   557,   559,   561,   563,   565,   567,   569,
     571,   573,   575,   577,   579,   581,   587,   589,   593,   595,
     599,   601,   605,   607,   611,   613,   617,   619,   623,   627,
     629,   633,   637,   641,   645,   647,   651,   655,   657,   661,
     665,   667,   671,   675,   679,   681,   685,   687,   691,   693,
     696,   699,   702,   705,   708,   711,   715,   720,   726,   732,
     739,   741,   743,   745,   747,   749,   752,   754,   756,   758,
     760,   762,   764,   766,   768,   770,   772,   774,   776,   778,
     780,   782,   784,   786,   788,   790,   792,   794,   796,   798,
     800,   802,   804,   806,   808,   810,   812,   814,   816,   818,
     820,   822,   824,   826,   828,   830,   832,   834,   836,   838,
     840,   842,   844,   846,   848,   850,   854,   856,   859,   863,
     868,   872,   875,   878,   880,   882,   884,   886,   888,   890,
     892,   894,   896,   900,   904
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     123,     0,    -1,   124,    -1,   123,   124,    -1,   142,    -1,
     133,    -1,   125,    -1,   134,    98,   131,    16,   127,    17,
      -1,   134,    98,   131,   126,    16,   127,    17,    -1,   134,
      99,   131,    16,   127,    17,    -1,   134,    99,   131,   130,
      16,   127,    17,    -1,   101,   131,    -1,   101,   131,   100,
     132,    -1,   100,   132,    -1,   100,   132,   101,   131,    -1,
     128,    -1,    -1,   129,    -1,   129,   128,    -1,   142,    -1,
     133,    -1,   125,    -1,   100,   131,    -1,     3,    -1,     3,
       9,   131,    -1,     3,    -1,     3,    18,   132,    -1,   135,
     136,   140,     3,    12,   141,    13,   147,    -1,   135,   136,
     140,     3,    12,    13,   147,    -1,   135,   136,     3,    12,
     141,    13,   147,    -1,   135,   136,     3,    12,    13,   147,
      -1,   135,   118,    12,   141,    13,   147,    -1,   135,   118,
      12,    13,   147,    -1,   135,   119,    12,    13,   147,    -1,
     135,   119,    12,   141,    13,   147,    -1,   135,   136,   140,
       3,    12,   141,    13,    11,    -1,   135,   136,   140,     3,
      12,    13,    11,    -1,   135,   136,   140,   117,   183,    12,
     141,    13,   147,    -1,   135,   136,   140,   117,    12,   141,
      13,   183,   147,    -1,   135,   136,   140,   117,    12,   183,
      13,    12,   141,    13,   147,    -1,   135,   136,   140,   117,
      12,   141,    13,    12,   183,    13,   147,    -1,   102,    -1,
     104,    -1,    -1,    43,    -1,   102,    -1,   103,    -1,   104,
      -1,   105,    -1,   106,    -1,    -1,     3,    -1,     3,    71,
      -1,    26,   132,    28,    -1,    26,   132,    28,    71,    -1,
     137,    -1,   138,    -1,   139,    -1,   139,   159,    -1,   139,
     162,    -1,   139,   162,     9,   141,    -1,   143,    -1,   142,
     143,    -1,   154,    -1,   146,    -1,   145,    -1,   144,    -1,
     147,    -1,   148,    -1,   151,    -1,    44,    11,    -1,    44,
     155,    11,    -1,    40,    11,    -1,    41,    11,    -1,    33,
      12,   155,    13,   143,    -1,    33,    12,   155,    13,   143,
      35,   143,    -1,    36,    12,   155,    13,   143,    -1,    38,
     143,    36,    12,   155,    13,    11,    -1,    37,    12,   154,
     154,    13,   143,    -1,    37,    12,   154,   154,   155,    13,
     143,    -1,    37,    12,   155,    10,   155,    13,   143,    -1,
      64,    12,   155,    13,   143,    -1,    38,   143,    64,    12,
     155,    13,    11,    -1,    39,    12,   155,    13,   143,    -1,
      16,    17,    -1,    16,   142,    17,    -1,   120,   150,    -1,
     120,    14,    15,    -1,   120,    16,    17,    -1,   120,    14,
     149,    14,    -1,   120,    16,   149,    17,    -1,   150,    -1,
     150,     9,   149,    -1,     4,    -1,   121,   153,    -1,   121,
      14,   152,    15,    -1,   121,    16,   152,    17,    -1,   153,
      -1,   153,     9,   152,    -1,     4,    -1,    11,    -1,   155,
      11,    -1,   156,    -1,   155,     9,   156,    -1,   157,    -1,
     156,   166,   157,    -1,   160,    -1,   157,   167,   160,    -1,
      14,   155,    15,    -1,    14,   155,     9,    15,    -1,    14,
     155,    15,   158,    -1,    14,   155,     9,    15,   158,    -1,
      14,    15,    -1,   159,    14,    15,    -1,   168,    -1,   139,
     161,    -1,    65,   139,   161,    -1,    66,   139,   161,    -1,
     105,   139,   161,    -1,    76,   161,    -1,   105,    76,   161,
      -1,   162,    -1,   162,     9,   161,    -1,     3,    -1,     3,
     158,    -1,     3,   159,    -1,     3,    12,    13,    -1,     3,
      12,   155,    13,    -1,     3,    12,    13,   158,    -1,     3,
      12,   155,    13,   158,    -1,    53,   155,    13,    -1,    54,
     155,    13,    -1,    55,   155,    13,    -1,    90,    -1,    93,
      -1,    77,    -1,    78,    -1,    79,    -1,    80,    -1,    84,
      -1,    85,    -1,    86,    -1,    95,    -1,    96,    -1,    97,
      -1,    81,    -1,    82,    -1,    83,    -1,   110,    -1,   109,
      -1,   114,    -1,   113,    -1,   116,    -1,   115,    -1,   169,
      -1,   169,    45,   155,    10,   168,    -1,   170,    -1,   169,
      31,   170,    -1,   171,    -1,   170,    30,   171,    -1,   172,
      -1,   171,    47,   172,    -1,   173,    -1,   172,    49,   173,
      -1,   174,    -1,   173,    48,   174,    -1,   175,    -1,   174,
      24,   175,    -1,   174,    25,   175,    -1,   176,    -1,   175,
      26,   176,    -1,   175,    28,   176,    -1,   175,    27,   176,
      -1,   175,    29,   176,    -1,   177,    -1,   176,    88,   177,
      -1,   176,    87,   177,    -1,   178,    -1,   177,    19,   178,
      -1,   177,    20,   178,    -1,   179,    -1,   178,    21,   179,
      -1,   178,    22,   179,    -1,   178,    23,   179,    -1,   180,
      -1,   179,    89,   180,    -1,   181,    -1,   180,    52,   139,
      -1,   184,    -1,    50,   181,    -1,    51,   181,    -1,   182,
     181,    -1,    75,   181,    -1,    74,   181,    -1,    73,   139,
      -1,    73,   139,   158,    -1,    73,   139,    12,    13,    -1,
      73,   139,    12,   155,    13,    -1,    73,   139,    12,    13,
     158,    -1,    73,   139,    12,   155,    13,   158,    -1,    19,
      -1,    20,    -1,    89,    -1,    46,    -1,    21,    -1,   108,
      89,    -1,    97,    -1,    90,    -1,    19,    -1,    20,    -1,
      21,    -1,    22,    -1,    23,    -1,    24,    -1,    25,    -1,
      26,    -1,    27,    -1,    28,    -1,    29,    -1,    30,    -1,
      31,    -1,    32,    -1,    46,    -1,    47,    -1,    48,    -1,
      49,    -1,    50,    -1,    51,    -1,    52,    -1,    72,    -1,
      77,    -1,    78,    -1,    79,    -1,    80,    -1,    81,    -1,
      82,    -1,    83,    -1,    84,    -1,    85,    -1,    86,    -1,
      87,    -1,    88,    -1,    89,    -1,    91,    -1,    93,    -1,
      95,    -1,    96,    -1,    97,    -1,   109,    -1,   110,    -1,
     113,    -1,   114,    -1,   115,    -1,   116,    -1,   185,    -1,
     184,    91,   185,    -1,   186,    -1,   185,   158,    -1,   185,
      12,    13,    -1,   185,    12,   155,    13,    -1,   185,    18,
       3,    -1,   185,    50,    -1,   185,    51,    -1,     3,    -1,
       6,    -1,     7,    -1,     4,    -1,     5,    -1,   158,    -1,
     163,    -1,   164,    -1,   165,    -1,   111,   155,   112,    -1,
      12,   155,    13,    -1,    12,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   216,   216,   217,   221,   222,   223,   227,   229,   231,
     233,   238,   239,   240,   241,   245,   246,   250,   251,   256,
     257,   258,   262,   266,   267,   271,   272,   276,   278,   280,
     282,   284,   286,   288,   290,   292,   294,   296,   298,   300,
     302,   307,   308,   309,   313,   314,   315,   316,   320,   321,
     322,   326,   327,   331,   332,   341,   342,   347,   348,   352,
     353,   357,   358,   362,   363,   364,   365,   367,   368,   369,
     373,   374,   375,   376,   380,   382,   387,   389,   391,   393,
     395,   397,   399,   401,   406,   407,   411,   412,   413,   414,
     415,   419,   420,   424,   429,   430,   431,   435,   436,   440,
     444,   445,   449,   450,   454,   455,   460,   461,   466,   467,
     468,   470,   475,   476,   480,   481,   482,   483,   484,   485,
     486,   490,   491,   495,   496,   497,   498,   499,   500,   501,
     505,   510,   515,   520,   521,   522,   523,   524,   525,   526,
     527,   528,   529,   530,   531,   532,   533,   534,   538,   539,
     540,   541,   542,   543,   547,   548,   553,   554,   559,   560,
     565,   566,   571,   572,   577,   578,   583,   584,   586,   591,
     592,   594,   596,   598,   603,   604,   606,   611,   612,   614,
     619,   620,   622,   624,   629,   630,   635,   636,   641,   642,
     644,   646,   648,   650,   652,   654,   656,   658,   660,   662,
     669,   670,   671,   672,   673,   674,   675,   681,   682,   683,
     684,   685,   686,   687,   688,   689,   690,   691,   692,   693,
     694,   695,   696,   697,   698,   699,   700,   701,   702,   703,
     704,   705,   706,   707,   708,   709,   710,   711,   712,   713,
     714,   715,   716,   717,   718,   719,   720,   721,   722,   723,
     724,   725,   726,   727,   731,   732,   737,   738,   740,   742,
     744,   746,   748,   754,   755,   756,   757,   758,   759,   760,
     761,   762,   763,   764,   765
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || YYTOKEN_TABLE
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "ID", "STRING_LIT", "CHAR_LIT",
  "INT_VAL", "FLOAT_VAL", "POUND", "COMMA", "COLON", "SEMICOLON", "LPAREN",
  "RPAREN", "LBRACK", "RBRACK", "LBRACE", "RBRACE", "DOT", "PLUS", "MINUS",
  "TIMES", "DIVIDE", "PERCENT", "EQ", "NEQ", "LT", "LE", "GT", "GE", "AND",
  "OR", "ASSIGN", "IF", "THEN", "ELSE", "WHILE", "FOR", "DO", "LOOP",
  "BREAK", "CONTINUE", "NULL_TOK", "FUNCTION", "RETURN", "QUESTION",
  "EXCLAMATION", "S_OR", "S_AND", "S_XOR", "PLUSPLUS", "MINUSMINUS",
  "DOLLAR", "POUNDPAREN", "PERCENTPAREN", "ATPAREN", "SIMULT", "PATTERN",
  "CODE", "TRANSPORT", "HOST", "TIME", "WHENEVER", "NEXT", "UNTIL",
  "EXTERNAL", "GLOBAL", "EVERY", "BEFORE", "AFTER", "AT", "AT_SYM",
  "ATAT_SYM", "NEW", "SIZEOF", "TYPEOF", "SAME", "PLUS_CHUCK",
  "MINUS_CHUCK", "TIMES_CHUCK", "DIVIDE_CHUCK", "S_AND_CHUCK",
  "S_OR_CHUCK", "S_XOR_CHUCK", "SHIFT_RIGHT_CHUCK", "SHIFT_LEFT_CHUCK",
  "PERCENT_CHUCK", "SHIFT_RIGHT", "SHIFT_LEFT", "TILDA", "CHUCK",
  "COLONCOLON", "S_CHUCK", "AT_CHUCK", "LEFT_S_CHUCK", "UNCHUCK",
  "UPCHUCK", "DOWNCHUCK", "CLASS", "INTERFACE", "EXTENDS", "IMPLEMENTS",
  "PUBLIC", "PROTECTED", "PRIVATE", "STATIC", "ABSTRACT", "CONST", "SPORK",
  "ARROW_RIGHT", "ARROW_LEFT", "L_HACK", "R_HACK", "GRUCK_RIGHT",
  "GRUCK_LEFT", "UNGRUCK_RIGHT", "UNGRUCK_LEFT", "AT_OP", "AT_CTOR",
  "AT_DTOR", "AT_IMPORT", "AT_DOC", "$accept", "program",
  "program_section", "class_definition", "class_ext", "class_body",
  "class_body2", "class_section", "iface_ext", "id_list", "id_dot",
  "function_definition", "class_decl", "function_decl", "static_decl",
  "type_decl_a", "type_decl_b", "type_decl", "type_decl2", "arg_list",
  "statement_list", "statement", "jump_statement", "selection_statement",
  "loop_statement", "code_segment", "import_statement", "import_list",
  "import_target", "doc_statement", "doc_list", "doc_target",
  "expression_statement", "expression", "chuck_expression",
  "arrow_expression", "array_exp", "array_empty", "decl_expression",
  "var_decl_list", "var_decl", "complex_exp", "polar_exp", "vec_exp",
  "chuck_operator", "arrow_operator", "conditional_expression",
  "logical_or_expression", "logical_and_expression",
  "inclusive_or_expression", "exclusive_or_expression", "and_expression",
  "equality_expression", "relational_expression", "shift_expression",
  "additive_expression", "multiplicative_expression", "tilda_expression",
  "cast_expression", "unary_expression", "unary_operator",
  "overloadable_operator", "dur_expression", "postfix_expression",
  "primary_expression", 0
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
     375,   376
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   122,   123,   123,   124,   124,   124,   125,   125,   125,
     125,   126,   126,   126,   126,   127,   127,   128,   128,   129,
     129,   129,   130,   131,   131,   132,   132,   133,   133,   133,
     133,   133,   133,   133,   133,   133,   133,   133,   133,   133,
     133,   134,   134,   134,   135,   135,   135,   135,   136,   136,
     136,   137,   137,   138,   138,   139,   139,   140,   140,   141,
     141,   142,   142,   143,   143,   143,   143,   143,   143,   143,
     144,   144,   144,   144,   145,   145,   146,   146,   146,   146,
     146,   146,   146,   146,   147,   147,   148,   148,   148,   148,
     148,   149,   149,   150,   151,   151,   151,   152,   152,   153,
     154,   154,   155,   155,   156,   156,   157,   157,   158,   158,
     158,   158,   159,   159,   160,   160,   160,   160,   160,   160,
     160,   161,   161,   162,   162,   162,   162,   162,   162,   162,
     163,   164,   165,   166,   166,   166,   166,   166,   166,   166,
     166,   166,   166,   166,   166,   166,   166,   166,   167,   167,
     167,   167,   167,   167,   168,   168,   169,   169,   170,   170,
     171,   171,   172,   172,   173,   173,   174,   174,   174,   175,
     175,   175,   175,   175,   176,   176,   176,   177,   177,   177,
     178,   178,   178,   178,   179,   179,   180,   180,   181,   181,
     181,   181,   181,   181,   181,   181,   181,   181,   181,   181,
     182,   182,   182,   182,   182,   182,   182,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   183,   183,   183,   183,   183,   183,
     183,   183,   183,   183,   184,   184,   185,   185,   185,   185,
     185,   185,   185,   186,   186,   186,   186,   186,   186,   186,
     186,   186,   186,   186,   186
};

/* YYR2[YYN] -- Number of symbols composing right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     2,     1,     1,     1,     6,     7,     6,
       7,     2,     4,     2,     4,     1,     0,     1,     2,     1,
       1,     1,     2,     1,     3,     1,     3,     8,     7,     7,
       6,     6,     5,     5,     6,     8,     7,     9,     9,    11,
      11,     1,     1,     0,     1,     1,     1,     1,     1,     1,
       0,     1,     2,     3,     4,     1,     1,     1,     2,     2,
       4,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       2,     3,     2,     2,     5,     7,     5,     7,     6,     7,
       7,     5,     7,     5,     2,     3,     2,     3,     3,     4,
       4,     1,     3,     1,     2,     4,     4,     1,     3,     1,
       1,     2,     1,     3,     1,     3,     1,     3,     3,     4,
       4,     5,     2,     3,     1,     2,     3,     3,     3,     2,
       3,     1,     3,     1,     2,     2,     3,     4,     4,     5,
       3,     3,     3,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     5,     1,     3,     1,     3,
       1,     3,     1,     3,     1,     3,     1,     3,     3,     1,
       3,     3,     3,     3,     1,     3,     3,     1,     3,     3,
       1,     3,     3,     3,     1,     3,     1,     3,     1,     2,
       2,     2,     2,     2,     2,     3,     4,     5,     5,     6,
       1,     1,     1,     1,     1,     2,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     3,     1,     2,     3,     4,
       3,     2,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      43,   263,   266,   267,   264,   265,   100,     0,     0,     0,
     200,   201,   204,     0,     0,     0,     0,     0,     0,     0,
       0,    44,     0,   203,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   202,   206,    45,    46,
      47,     0,     0,     0,     0,     0,    43,     2,     6,     5,
       0,    50,    55,    56,     0,     4,    61,    66,    65,    64,
      67,    68,    69,    63,     0,   102,   104,   268,   106,   269,
     270,   271,   114,   154,   156,   158,   160,   162,   164,   166,
     169,   174,   177,   180,   184,   186,     0,   188,   254,   256,
      52,   274,     0,     0,    84,     0,    25,     0,     0,     0,
       0,     0,     0,    72,    73,    70,     0,   263,   189,   190,
       0,     0,     0,     0,    51,     0,     0,   194,   193,   192,
     123,   119,   121,     0,     0,   205,     0,    93,     0,     0,
      86,    99,     0,     0,    94,     1,     3,     0,     0,    48,
      49,     0,     0,     0,   115,    62,     0,   101,   135,   136,
     137,   138,   145,   146,   147,   139,   140,   141,   133,   134,
     142,   143,   144,     0,   149,   148,   151,   150,   153,   152,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   191,     0,     0,     0,   261,   262,   257,   273,
       0,   108,    85,     0,    53,     0,     0,     0,     0,     0,
       0,     0,    71,   130,   131,   132,     0,   116,   117,     0,
     195,     0,     0,   124,   125,     0,   120,   118,   272,    87,
       0,    91,    88,     0,     0,    97,     0,    23,     0,     0,
       0,     0,    51,    57,     0,   103,   105,   107,   157,     0,
     159,   161,   163,   165,   167,   168,   170,   172,   171,   173,
     176,   175,   178,   179,   181,   182,   183,   185,   187,   255,
     258,     0,   260,   109,   110,    26,    54,     0,     0,     0,
       0,     0,     0,     0,     0,   196,     0,   126,     0,   112,
       0,   122,    89,     0,    90,    95,     0,    96,     0,    43,
       0,     0,     0,    43,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    58,     0,     0,     0,   259,   111,    74,
      76,     0,     0,     0,     0,     0,    83,    81,   198,   197,
     128,   127,   113,    92,    98,    24,    21,     0,    15,    43,
      20,    19,    13,    11,    43,     0,    22,    43,    32,    59,
       0,    33,     0,     0,     0,     0,     0,   208,   209,   210,
     211,   212,   213,   214,   215,   216,   217,   218,   219,   220,
     221,   222,   223,   224,   225,   226,   227,   228,   229,   230,
     231,   232,   233,   234,   235,   236,   237,   238,   239,   240,
     241,   242,   207,   243,   244,   245,   246,   247,   248,   249,
     250,   251,   252,   253,     0,   155,     0,    78,     0,     0,
       0,     0,   199,   129,     7,    18,     0,     0,     0,     9,
       0,     0,    31,    34,    30,     0,     0,     0,   215,     0,
       0,     0,    75,    79,    80,    77,    82,    14,    12,     8,
      10,    60,    29,    36,    28,     0,     0,     0,     0,    35,
      27,     0,     0,     0,     0,     0,    38,     0,    37,     0,
       0,    40,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    46,    47,   336,   302,   337,   338,   339,   305,   238,
      97,   340,    50,    51,   143,    52,    53,    54,   244,   308,
     341,    56,    57,    58,    59,    60,    61,   230,   231,    62,
     234,   235,    63,    64,    65,    66,    67,   224,    68,   121,
     122,    69,    70,    71,   163,   170,    72,    73,    74,    75,
      76,    77,    78,    79,    80,    81,    82,    83,    84,    85,
      86,   404,    87,    88,    89
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -338
static const yytype_int16 yypact[] =
{
     665,    15,  -338,  -338,  -338,  -338,  -338,  1215,  1893,   776,
    -338,  -338,  -338,   148,    42,   145,   151,   998,   156,   149,
     175,  -338,  1289,  -338,  1967,  1967,  1893,  1893,  1893,   171,
      37,    37,    37,  1967,  1967,   198,  -338,  -338,    44,  -338,
      96,    21,   118,  1893,   155,   158,   332,  -338,  -338,  -338,
     168,    47,  -338,  -338,   198,   998,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,    46,    51,    99,  -338,  -338,  -338,
    -338,  -338,  -338,    -8,   211,   169,   174,   200,   247,   217,
     186,   256,   233,   150,   213,  -338,  1967,   177,    16,  -338,
    -338,  -338,   167,    55,  -338,   887,   265,   262,  1893,  1893,
    1363,    24,  1893,  -338,  -338,  -338,    62,  -338,  -338,  -338,
     172,   183,   184,  1893,   223,   198,   198,    75,  -338,  -338,
      98,  -338,   284,   198,   198,  -338,     8,  -338,    86,   141,
    -338,  -338,   291,   291,  -338,  -338,  -338,   293,   293,  -338,
    -338,   285,   286,    53,  -338,  -338,  1893,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  1893,  -338,  -338,  -338,  -338,  -338,  -338,
    1893,  1967,  1893,  1967,  1967,  1967,  1967,  1967,  1967,  1967,
    1967,  1967,  1967,  1967,  1967,  1967,  1967,  1967,  1967,  1967,
    1967,    37,  -338,    39,  1437,   296,  -338,  -338,  -338,  -338,
    1517,   287,  -338,   148,   231,   191,   193,  1363,   248,   288,
     292,   208,  -338,  -338,  -338,  -338,   209,  -338,  -338,  1591,
    -338,  1665,  1745,  -338,   289,   198,  -338,  -338,  -338,  -338,
     294,   297,  -338,   295,   290,   300,   298,   301,    -1,     6,
      82,   112,    13,   299,     7,    51,    99,  -338,   211,   268,
     169,   174,   200,   247,   217,   217,   186,   186,   186,   186,
     256,   256,   233,   233,   150,   150,   150,   213,  -338,    16,
    -338,   216,  -338,   287,  -338,  -338,  -338,   998,   998,  1819,
    1893,  1893,  1893,   998,   998,   287,   219,   287,   221,  -338,
     302,  -338,  -338,   307,  -338,  -338,   291,  -338,   293,   443,
     148,   293,   303,   443,   293,   304,   305,   198,   309,   305,
     310,   113,   313,   289,   306,  2060,  1967,  -338,  -338,   279,
    -338,   998,   222,   224,   227,   229,  -338,  -338,  -338,   287,
    -338,   287,  -338,  -338,  -338,  -338,  -338,   312,  -338,   554,
    -338,   998,   215,   230,   443,   314,  -338,   443,  -338,   315,
     305,  -338,   305,   305,   320,   114,  1101,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,   322,  -338,   998,  -338,   998,   998,
     329,   330,  -338,  -338,  -338,  -338,   293,   148,   325,  -338,
     328,    37,  -338,  -338,  -338,   305,    80,   334,   148,   336,
     337,    37,  -338,  -338,  -338,  -338,  -338,  -338,  -338,  -338,
    -338,  -338,  -338,  -338,  -338,   138,  2159,   342,   343,  -338,
    -338,  2257,   305,    37,   305,   344,  -338,   346,  -338,   305,
     305,  -338,  -338
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -338,  -338,   316,    26,  -338,  -282,    22,  -338,  -338,  -134,
    -198,    29,  -338,  -338,  -338,  -338,  -338,   -30,  -338,  -233,
      52,   -14,  -338,  -338,  -338,  -270,  -338,  -120,   311,  -338,
    -117,   318,   -86,     5,  -126,   197,   -82,   121,   196,   -47,
      60,  -338,  -338,  -338,  -338,  -338,    58,  -338,   206,   207,
     205,   214,   212,   102,    71,   105,   106,    73,   194,    25,
    -338,  -337,  -338,   188,  -338
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -52
static const yytype_int16 yytable[] =
{
     115,   116,   117,   101,   239,   275,   198,   144,   310,   233,
     314,   124,    92,    93,   207,   299,   236,   146,   -51,   430,
     245,   345,   303,   171,   114,   311,    48,   106,   194,    49,
       8,   110,   111,   112,   195,   220,   348,   172,   223,   351,
     114,   145,   107,     2,     3,     4,     5,    13,   126,   108,
     109,     7,    55,     8,    98,   146,   242,   147,   118,   119,
     209,    95,   418,    13,   200,   420,   196,   197,   217,   218,
     201,   146,    48,   212,   245,    49,   226,   227,   354,    13,
     422,   145,   423,   424,    90,   114,    90,   219,   210,     8,
     127,   443,    26,    27,    28,   306,     9,   123,    55,   300,
     301,   229,   342,   205,   206,   208,   304,   211,    13,   452,
     221,   192,   222,   243,   455,   114,   114,   114,   216,   274,
     228,   279,   427,   429,   315,   309,   353,   426,   148,   149,
     150,   151,   152,   153,   154,   155,   156,   157,    13,    13,
      13,   158,   -41,   -41,   159,   127,   160,   161,   162,   449,
      43,    96,   139,   140,     9,   442,   444,    99,   232,   127,
     103,   268,   131,   100,   335,   141,   142,   343,   102,   128,
     346,   129,   132,   333,   133,   450,   146,   249,   291,   334,
     199,   146,   456,   113,   458,   213,   104,   198,   441,   461,
     462,   318,   146,   146,   -42,   -42,   214,   215,   448,   271,
     146,   120,   146,   328,   277,   330,   278,   125,   164,   165,
     307,   307,   166,   167,   168,   169,   174,   146,   146,   438,
     457,   283,   284,   175,   286,   146,   288,    93,   146,   317,
     146,   146,   329,   146,   331,   408,   146,   409,   146,   190,
     410,   173,   411,   179,   180,   181,   182,   412,   176,   413,
     256,   257,   258,   259,   187,   188,   189,   146,   280,   147,
     264,   265,   266,   319,   320,   191,   137,   138,   193,   326,
     327,   177,   178,   183,   184,   185,   186,   146,   316,   254,
     255,   307,   437,   203,   322,   323,   324,   325,   260,   261,
     204,   262,   263,   225,    90,   131,   237,   240,   241,   272,
     281,     8,   276,   290,   282,   295,   293,   407,   292,   296,
     298,   127,   294,   312,   406,   297,   416,   332,   355,   344,
     347,     9,   350,   352,   421,   307,   307,   145,   289,   414,
     417,   419,   135,   425,   431,     1,     2,     3,     4,     5,
     435,   436,   439,     6,     7,   440,     8,   445,     9,   446,
     447,    10,    11,    12,   453,   130,   454,   459,    13,   460,
     246,   415,   136,   134,   313,    14,   247,   349,    15,    16,
      17,    18,    19,    20,   405,    21,    22,   248,    23,   251,
     250,   269,    24,    25,   267,    26,    27,    28,   253,   252,
       0,   307,   432,     0,   433,   434,    29,    30,    31,     0,
       0,   307,     0,     0,     0,    32,    33,    34,    35,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    36,     0,   307,     0,     0,     0,     0,     0,    37,
       0,     0,     0,     0,    38,    39,    40,    41,     0,     0,
      42,     0,     0,    43,     0,     0,     1,     2,     3,     4,
       5,     0,    44,    45,     6,     7,     0,     8,     0,     9,
     -16,     0,    10,    11,    12,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,    14,     0,     0,    15,
      16,    17,    18,    19,    20,     0,    21,    22,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,     0,     0,     0,     0,     0,     0,     0,
      37,     0,     0,     0,     0,    38,    39,    40,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     1,     2,     3,
       4,     5,     0,    44,    45,     6,     7,     0,     8,     0,
       9,   -17,     0,    10,    11,    12,     0,     0,     0,     0,
      13,     0,     0,     0,     0,     0,     0,    14,     0,     0,
      15,    16,    17,    18,    19,    20,     0,    21,    22,     0,
      23,     0,     0,     0,    24,    25,     0,    26,    27,    28,
       0,     0,     0,     0,     0,     0,     0,     0,    29,    30,
      31,     0,     0,     0,     0,     0,     0,    32,    33,    34,
      35,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    36,     0,     0,     0,     0,     0,     0,
       0,    37,     0,     0,     0,     0,    38,    39,    40,    41,
       0,     0,    42,     0,     0,    43,     0,     0,     1,     2,
       3,     4,     5,     0,    44,    45,     6,     7,     0,     8,
       0,     9,     0,     0,    10,    11,    12,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,    14,     0,
       0,    15,    16,    17,    18,    19,    20,     0,    21,    22,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,    29,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,     0,     0,     0,
       0,     0,    37,     0,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    42,     0,     0,    43,     0,     0,     1,
       2,     3,     4,     5,     0,    44,    45,     6,     7,     0,
       8,     0,     9,    94,     0,    10,    11,    12,     0,     0,
       0,     0,    13,     0,     0,     0,     0,     0,     0,    14,
       0,     0,    15,    16,    17,    18,    19,    20,     0,     0,
      22,     0,    23,     0,     0,     0,    24,    25,     0,    26,
      27,    28,     0,     0,     0,     0,     0,     0,     0,     0,
      29,    30,    31,     0,     0,     0,     0,     0,     0,    32,
      33,    34,    35,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    36,     0,     0,     0,     0,
       0,     0,     0,    37,     0,     0,     0,     0,     0,     0,
       0,    41,     0,     0,    42,     0,     0,    43,     0,     0,
       1,     2,     3,     4,     5,     0,    44,    45,     6,     7,
       0,     8,     0,     9,   202,     0,    10,    11,    12,     0,
       0,     0,     0,    13,     0,     0,     0,     0,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,     0,
       0,    22,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,    41,     0,     0,    42,     0,     0,    43,     0,
       0,     1,     2,     3,     4,     5,     0,    44,    45,     6,
       7,     0,     8,     0,     9,     0,     0,    10,    11,    12,
       0,     0,     0,     0,    13,     0,     0,     0,     0,     0,
       0,    14,     0,     0,    15,    16,    17,    18,    19,    20,
       0,     0,    22,     0,    23,     0,     0,     0,    24,    25,
       0,    26,    27,    28,     0,     0,     0,     0,     0,     0,
       0,     0,    29,    30,    31,     0,     0,     0,     0,     0,
       0,    32,    33,    34,    35,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,    36,     0,     0,
       0,     0,     0,     0,     0,    37,     0,     0,     0,     0,
       0,     0,     0,    41,   114,     0,    42,     0,     0,    43,
       0,     0,     0,     0,     0,     0,     0,     0,    44,    45,
     357,   358,   359,   360,   361,   362,   363,   428,   365,   366,
     367,   368,   369,   370,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   371,   372,   373,
     374,   375,   376,   377,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   378,     0,     0,     0,     0,   379,   380,
     381,   382,   383,   384,   385,   386,   387,   388,   389,   390,
     391,   392,   393,     0,   394,     0,   395,   396,   397,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     398,   399,     0,     0,   400,   401,   402,   403,     1,     2,
       3,     4,     5,     0,     0,     0,     0,     7,    91,     8,
       0,     0,     0,     0,    10,    11,    12,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     1,     2,     3,     4,     5,     0,     0,     0,
     105,     7,     0,     8,    36,     0,     0,     0,    10,    11,
      12,     0,    37,     0,     0,    13,     0,     0,     0,     0,
      41,     0,     0,    42,     0,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,    31,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,     1,     2,     3,     4,
       5,     0,     0,     0,     6,     7,     0,     8,    36,     0,
       0,     0,    10,    11,    12,     0,    37,     0,     0,    13,
       0,     0,     0,     0,    41,     0,     0,    42,     0,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       1,     2,     3,     4,     5,     0,     0,     0,     0,     7,
     270,     8,    36,     0,     0,     0,    10,    11,    12,     0,
      37,     0,     0,    13,     0,     0,     0,     0,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     0,    36,     0,     0,     7,
       0,     8,   273,     0,    37,     0,    10,    11,    12,     0,
       0,     0,    41,    13,     0,    42,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     1,     2,     3,     4,     5,     0,
       0,     0,     0,     7,   285,     8,    36,     0,     0,     0,
      10,    11,    12,     0,    37,     0,     0,    13,     0,     0,
       0,     0,    41,     0,     0,    42,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
       0,    24,    25,     0,    26,    27,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    30,    31,     0,     0,
       0,     0,     0,     0,    32,    33,    34,    35,     1,     2,
       3,     4,     5,     0,     0,     0,     0,     7,   287,     8,
      36,     0,     0,     0,    10,    11,    12,     0,    37,     0,
       0,    13,     0,     0,     0,     0,    41,     0,     0,    42,
       0,     0,    43,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     0,    36,     0,     0,     7,     0,     8,
     289,     0,    37,     0,    10,    11,    12,     0,     0,     0,
      41,    13,     0,    42,     0,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     1,     2,     3,     4,     5,     0,     0,     0,
       0,     7,   321,     8,    36,     0,     0,     0,    10,    11,
      12,     0,    37,     0,     0,    13,     0,     0,     0,     0,
      41,     0,     0,    42,     0,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    30,    31,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,     1,     2,     3,     4,
       5,     0,     0,     0,     0,     7,     0,     8,    36,     0,
       0,     0,    10,    11,    12,     0,    37,     0,     0,    13,
       0,     0,     0,     0,    41,     0,     0,    42,     0,     0,
      43,     0,     0,     0,     0,     0,     0,     0,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
     107,     2,     3,     4,     5,     0,     0,     0,     0,     7,
       0,     8,    36,     0,     0,     0,    10,    11,    12,     0,
      37,     0,     0,     0,     0,     0,     0,     0,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,   356,     0,     0,    42,     0,     0,    43,   357,
     358,   359,   360,   361,   362,   363,   364,   365,   366,   367,
     368,   369,   370,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   371,   372,   373,   374,
     375,   376,   377,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   378,     0,     0,     0,     0,   379,   380,   381,
     382,   383,   384,   385,   386,   387,   388,   389,   390,   391,
     392,   393,     0,   394,     0,   395,   396,   397,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   398,
     399,   451,     0,   400,   401,   402,   403,     0,   357,   358,
     359,   360,   361,   362,   363,   364,   365,   366,   367,   368,
     369,   370,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   371,   372,   373,   374,   375,
     376,   377,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   378,     0,     0,     0,     0,   379,   380,   381,   382,
     383,   384,   385,   386,   387,   388,   389,   390,   391,   392,
     393,     0,   394,     0,   395,   396,   397,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   398,   399,
       0,     0,   400,   401,   402,   403,   357,   358,   359,   360,
     361,   362,   363,   364,   365,   366,   367,   368,   369,   370,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   371,   372,   373,   374,   375,   376,   377,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   378,
       0,     0,     0,     0,   379,   380,   381,   382,   383,   384,
     385,   386,   387,   388,   389,   390,   391,   392,   393,     0,
     394,     0,   395,   396,   397,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   398,   399,     0,     0,
     400,   401,   402,   403
};

static const yytype_int16 yycheck[] =
{
      30,    31,    32,    17,   138,   203,    88,    54,   241,   129,
       3,    41,     7,     8,   100,    16,   133,     9,     3,   356,
     146,   303,    16,    31,     3,    12,     0,    22,    12,     0,
      14,    26,    27,    28,    18,   117,   306,    45,   120,   309,
       3,    55,     3,     4,     5,     6,     7,    26,    43,    24,
      25,    12,     0,    14,    12,     9,     3,    11,    33,    34,
      36,     9,   344,    26,     9,   347,    50,    51,   115,   116,
      15,     9,    46,    11,   200,    46,   123,   124,   311,    26,
     350,    95,   352,   353,    71,     3,    71,    12,    64,    14,
       4,    11,    53,    54,    55,    13,    16,    76,    46,   100,
     101,    15,   300,    98,    99,   100,   100,   102,    26,   446,
      12,    86,    14,   143,   451,     3,     3,     3,   113,   201,
     112,   207,   355,   356,   117,    13,    13,    13,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    26,    26,
      26,    90,    98,    99,    93,     4,    95,    96,    97,    11,
     111,     3,   105,   106,    16,   425,   426,    12,    17,     4,
      11,   191,     4,    12,   298,   118,   119,   301,    12,    14,
     304,    16,    14,   293,    16,   445,     9,   172,   225,   296,
      13,     9,   452,    12,   454,    13,    11,   269,   421,   459,
     460,   273,     9,     9,    98,    99,    13,    13,   431,   194,
       9,     3,     9,   285,    13,   287,    13,    89,   109,   110,
     240,   241,   113,   114,   115,   116,    47,     9,     9,   417,
     453,    13,    13,    49,   219,     9,   221,   222,     9,    13,
       9,     9,    13,     9,    13,    13,     9,    13,     9,    89,
      13,    30,    13,    26,    27,    28,    29,   329,    48,   331,
     179,   180,   181,   182,    21,    22,    23,     9,    10,    11,
     187,   188,   189,   277,   278,    52,    98,    99,    91,   283,
     284,    24,    25,    87,    88,    19,    20,     9,    10,   177,
     178,   311,   416,    18,   279,   280,   281,   282,   183,   184,
      28,   185,   186,     9,    71,     4,     3,    12,    12,     3,
      12,    14,    71,    14,    12,    15,     9,   321,    14,     9,
       9,     4,    17,    14,    35,    17,   101,    15,    12,    16,
      16,    16,    13,    13,     9,   355,   356,   341,    15,    17,
     100,    17,     0,    13,    12,     3,     4,     5,     6,     7,
      11,    11,    17,    11,    12,    17,    14,    13,    16,    13,
      13,    19,    20,    21,    12,    44,    13,    13,    26,    13,
     163,   339,    46,    45,   243,    33,   170,   307,    36,    37,
      38,    39,    40,    41,   316,    43,    44,   171,    46,   174,
     173,   193,    50,    51,   190,    53,    54,    55,   176,   175,
      -1,   421,   406,    -1,   408,   409,    64,    65,    66,    -1,
      -1,   431,    -1,    -1,    -1,    73,    74,    75,    76,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    89,    -1,   453,    -1,    -1,    -1,    -1,    -1,    97,
      -1,    -1,    -1,    -1,   102,   103,   104,   105,    -1,    -1,
     108,    -1,    -1,   111,    -1,    -1,     3,     4,     5,     6,
       7,    -1,   120,   121,    11,    12,    -1,    14,    -1,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    -1,    36,
      37,    38,    39,    40,    41,    -1,    43,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      97,    -1,    -1,    -1,    -1,   102,   103,   104,   105,    -1,
      -1,   108,    -1,    -1,   111,    -1,    -1,     3,     4,     5,
       6,     7,    -1,   120,   121,    11,    12,    -1,    14,    -1,
      16,    17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,
      26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    -1,
      36,    37,    38,    39,    40,    41,    -1,    43,    44,    -1,
      46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,
      66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,
      76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    97,    -1,    -1,    -1,    -1,   102,   103,   104,   105,
      -1,    -1,   108,    -1,    -1,   111,    -1,    -1,     3,     4,
       5,     6,     7,    -1,   120,   121,    11,    12,    -1,    14,
      -1,    16,    -1,    -1,    19,    20,    21,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      -1,    36,    37,    38,    39,    40,    41,    -1,    43,    44,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    -1,    -1,    -1,   102,   103,   104,
     105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,     3,
       4,     5,     6,     7,    -1,   120,   121,    11,    12,    -1,
      14,    -1,    16,    17,    -1,    19,    20,    21,    -1,    -1,
      -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,
      -1,    -1,    36,    37,    38,    39,    40,    41,    -1,    -1,
      44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,
      54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,
      74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,   105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,
       3,     4,     5,     6,     7,    -1,   120,   121,    11,    12,
      -1,    14,    -1,    16,    17,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   105,    -1,    -1,   108,    -1,    -1,   111,    -1,
      -1,     3,     4,     5,     6,     7,    -1,   120,   121,    11,
      12,    -1,    14,    -1,    16,    -1,    -1,    19,    20,    21,
      -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,
      -1,    33,    -1,    -1,    36,    37,    38,    39,    40,    41,
      -1,    -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,
      -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,
      -1,    73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   105,     3,    -1,   108,    -1,    -1,   111,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,
      49,    50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    87,    88,
      89,    90,    91,    -1,    93,    -1,    95,    96,    97,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     109,   110,    -1,    -1,   113,   114,   115,   116,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    -1,    12,    13,    14,
      -1,    -1,    -1,    -1,    19,    20,    21,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      11,    12,    -1,    14,    89,    -1,    -1,    -1,    19,    20,
      21,    -1,    97,    -1,    -1,    26,    -1,    -1,    -1,    -1,
     105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    11,    12,    -1,    14,    89,    -1,
      -1,    -1,    19,    20,    21,    -1,    97,    -1,    -1,    26,
      -1,    -1,    -1,    -1,   105,    -1,    -1,   108,    -1,    -1,
     111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    -1,    12,
      13,    14,    89,    -1,    -1,    -1,    19,    20,    21,    -1,
      97,    -1,    -1,    26,    -1,    -1,    -1,    -1,   105,    -1,
      -1,   108,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
       3,     4,     5,     6,     7,    -1,    89,    -1,    -1,    12,
      -1,    14,    15,    -1,    97,    -1,    19,    20,    21,    -1,
      -1,    -1,   105,    26,    -1,   108,    -1,    -1,   111,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,     3,     4,     5,     6,     7,    -1,
      -1,    -1,    -1,    12,    13,    14,    89,    -1,    -1,    -1,
      19,    20,    21,    -1,    97,    -1,    -1,    26,    -1,    -1,
      -1,    -1,   105,    -1,    -1,   108,    -1,    -1,   111,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    65,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,     3,     4,
       5,     6,     7,    -1,    -1,    -1,    -1,    12,    13,    14,
      89,    -1,    -1,    -1,    19,    20,    21,    -1,    97,    -1,
      -1,    26,    -1,    -1,    -1,    -1,   105,    -1,    -1,   108,
      -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    -1,    -1,    -1,    -1,     3,     4,
       5,     6,     7,    -1,    89,    -1,    -1,    12,    -1,    14,
      15,    -1,    97,    -1,    19,    20,    21,    -1,    -1,    -1,
     105,    26,    -1,   108,    -1,    -1,   111,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,     3,     4,     5,     6,     7,    -1,    -1,    -1,
      -1,    12,    13,    14,    89,    -1,    -1,    -1,    19,    20,
      21,    -1,    97,    -1,    -1,    26,    -1,    -1,    -1,    -1,
     105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,     3,     4,     5,     6,
       7,    -1,    -1,    -1,    -1,    12,    -1,    14,    89,    -1,
      -1,    -1,    19,    20,    21,    -1,    97,    -1,    -1,    26,
      -1,    -1,    -1,    -1,   105,    -1,    -1,   108,    -1,    -1,
     111,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
       3,     4,     5,     6,     7,    -1,    -1,    -1,    -1,    12,
      -1,    14,    89,    -1,    -1,    -1,    19,    20,    21,    -1,
      97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,
      -1,   108,    -1,    -1,   111,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    12,    -1,    -1,   108,    -1,    -1,   111,    19,
      20,    21,    22,    23,    24,    25,    26,    27,    28,    29,
      30,    31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,
      50,    51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    72,    -1,    -1,    -1,    -1,    77,    78,    79,
      80,    81,    82,    83,    84,    85,    86,    87,    88,    89,
      90,    91,    -1,    93,    -1,    95,    96,    97,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,
     110,    12,    -1,   113,   114,   115,   116,    -1,    19,    20,
      21,    22,    23,    24,    25,    26,    27,    28,    29,    30,
      31,    32,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    46,    47,    48,    49,    50,
      51,    52,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    72,    -1,    -1,    -1,    -1,    77,    78,    79,    80,
      81,    82,    83,    84,    85,    86,    87,    88,    89,    90,
      91,    -1,    93,    -1,    95,    96,    97,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   109,   110,
      -1,    -1,   113,   114,   115,   116,    19,    20,    21,    22,
      23,    24,    25,    26,    27,    28,    29,    30,    31,    32,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    46,    47,    48,    49,    50,    51,    52,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    72,
      -1,    -1,    -1,    -1,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    87,    88,    89,    90,    91,    -1,
      93,    -1,    95,    96,    97,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   109,   110,    -1,    -1,
     113,   114,   115,   116
};

/* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
   symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     3,     4,     5,     6,     7,    11,    12,    14,    16,
      19,    20,    21,    26,    33,    36,    37,    38,    39,    40,
      41,    43,    44,    46,    50,    51,    53,    54,    55,    64,
      65,    66,    73,    74,    75,    76,    89,    97,   102,   103,
     104,   105,   108,   111,   120,   121,   123,   124,   125,   133,
     134,   135,   137,   138,   139,   142,   143,   144,   145,   146,
     147,   148,   151,   154,   155,   156,   157,   158,   160,   163,
     164,   165,   168,   169,   170,   171,   172,   173,   174,   175,
     176,   177,   178,   179,   180,   181,   182,   184,   185,   186,
      71,    13,   155,   155,    17,   142,     3,   132,    12,    12,
      12,   143,    12,    11,    11,    11,   155,     3,   181,   181,
     155,   155,   155,    12,     3,   139,   139,   139,   181,   181,
       3,   161,   162,    76,   139,    89,   155,     4,    14,    16,
     150,     4,    14,    16,   153,     0,   124,    98,    99,   105,
     106,   118,   119,   136,   161,   143,     9,    11,    77,    78,
      79,    80,    81,    82,    83,    84,    85,    86,    90,    93,
      95,    96,    97,   166,   109,   110,   113,   114,   115,   116,
     167,    31,    45,    30,    47,    49,    48,    24,    25,    26,
      27,    28,    29,    87,    88,    19,    20,    21,    22,    23,
      89,    52,   181,    91,    12,    18,    50,    51,   158,    13,
       9,    15,    17,    18,    28,   155,   155,   154,   155,    36,
      64,   155,    11,    13,    13,    13,   155,   161,   161,    12,
     158,    12,    14,   158,   159,     9,   161,   161,   112,    15,
     149,   150,    17,   149,   152,   153,   152,     3,   131,   131,
      12,    12,     3,   139,   140,   156,   157,   160,   170,   155,
     171,   172,   173,   174,   175,   175,   176,   176,   176,   176,
     177,   177,   178,   178,   179,   179,   179,   180,   139,   185,
      13,   155,     3,    15,   158,   132,    71,    13,    13,   154,
      10,    12,    12,    13,    13,    13,   155,    13,   155,    15,
      14,   161,    14,     9,    17,    15,     9,    17,     9,    16,
     100,   101,   126,    16,   100,   130,    13,   139,   141,    13,
     141,    12,    14,   159,     3,   117,    10,    13,   158,   143,
     143,    13,   155,   155,   155,   155,   143,   143,   158,    13,
     158,    13,    15,   149,   152,   131,   125,   127,   128,   129,
     133,   142,   132,   131,    16,   127,   131,    16,   147,   162,
      13,   147,    13,    13,   141,    12,    12,    19,    20,    21,
      22,    23,    24,    25,    26,    27,    28,    29,    30,    31,
      32,    46,    47,    48,    49,    50,    51,    52,    72,    77,
      78,    79,    80,    81,    82,    83,    84,    85,    86,    87,
      88,    89,    90,    91,    93,    95,    96,    97,   109,   110,
     113,   114,   115,   116,   183,   168,    35,   143,    13,    13,
      13,    13,   158,   158,    17,   128,   101,   100,   127,    17,
     127,     9,   147,   147,   147,    13,    13,   141,    26,   141,
     183,    12,   143,   143,   143,    11,    11,   131,   132,    17,
      17,   141,   147,    11,   147,    13,    13,    13,   141,    11,
     147,    12,   183,    12,    13,   183,   147,   141,   147,    13,
      13,   147,   147
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
   Once GCC version 2 has supplanted version 1, this can go.  */

#define YYFAIL		goto yyerrlab

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
# define YYLEX yylex (YYLEX_PARAM)
#else
# define YYLEX yylex ()
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
		  Type, Value, Location); \
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
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (!yyvaluep)
    return;
  YYUSE (yylocationp);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, YYLTYPE const * const yylocationp)
#else
static void
yy_symbol_print (yyoutput, yytype, yyvaluep, yylocationp)
    FILE *yyoutput;
    int yytype;
    YYSTYPE const * const yyvaluep;
    YYLTYPE const * const yylocationp;
#endif
{
  if (yytype < YYNTOKENS)
    YYFPRINTF (yyoutput, "token %s (", yytname[yytype]);
  else
    YYFPRINTF (yyoutput, "nterm %s (", yytname[yytype]);

  YY_LOCATION_PRINT (yyoutput, *yylocationp);
  YYFPRINTF (yyoutput, ": ");
  yy_symbol_value_print (yyoutput, yytype, yyvaluep, yylocationp);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

#if (defined __STDC__ || defined __C99__FUNC__ \
     || defined __cplusplus || defined _MSC_VER)
static void
yy_stack_print (yytype_int16 *bottom, yytype_int16 *top)
#else
static void
yy_stack_print (bottom, top)
    yytype_int16 *bottom;
    yytype_int16 *top;
#endif
{
  YYFPRINTF (stderr, "Stack now");
  for (; bottom <= top; ++bottom)
    YYFPRINTF (stderr, " %d", *bottom);
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
yy_reduce_print (YYSTYPE *yyvsp, YYLTYPE *yylsp, int yyrule)
#else
static void
yy_reduce_print (yyvsp, yylsp, yyrule)
    YYSTYPE *yyvsp;
    YYLTYPE *yylsp;
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
      fprintf (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr, yyrhs[yyprhs[yyrule] + yyi],
		       &(yyvsp[(yyi + 1) - (yynrhs)])
		       , &(yylsp[(yyi + 1) - (yynrhs)])		       );
      fprintf (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)		\
do {					\
  if (yydebug)				\
    yy_reduce_print (yyvsp, yylsp, Rule); \
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, YYLTYPE *yylocationp)
#else
static void
yydestruct (yymsg, yytype, yyvaluep, yylocationp)
    const char *yymsg;
    int yytype;
    YYSTYPE *yyvaluep;
    YYLTYPE *yylocationp;
#endif
{
  YYUSE (yyvaluep);
  YYUSE (yylocationp);

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



/* The look-ahead symbol.  */
int yychar;

/* The semantic value of the look-ahead symbol.  */
YYSTYPE yylval;

/* Number of syntax errors so far.  */
int yynerrs;
/* Location data for the look-ahead symbol.  */
YYLTYPE yylloc;



/*----------.
| yyparse.  |
`----------*/

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
  
  int yystate;
  int yyn;
  int yyresult;
  /* Number of tokens to shift before error messages enabled.  */
  int yyerrstatus;
  /* Look-ahead token as an internal (translated) token number.  */
  int yytoken = 0;
#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

  /* Three stacks and their tools:
     `yyss': related to states,
     `yyvs': related to semantic values,
     `yyls': related to locations.

     Refer to the stacks thru separate pointers, to allow yyoverflow
     to reallocate them elsewhere.  */

  /* The state stack.  */
  yytype_int16 yyssa[YYINITDEPTH];
  yytype_int16 *yyss = yyssa;
  yytype_int16 *yyssp;

  /* The semantic value stack.  */
  YYSTYPE yyvsa[YYINITDEPTH];
  YYSTYPE *yyvs = yyvsa;
  YYSTYPE *yyvsp;

  /* The location stack.  */
  YYLTYPE yylsa[YYINITDEPTH];
  YYLTYPE *yyls = yylsa;
  YYLTYPE *yylsp;
  /* The locations where the error started and ended.  */
  YYLTYPE yyerror_range[2];

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N), yylsp -= (N))

  YYSIZE_T yystacksize = YYINITDEPTH;

  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;
  YYLTYPE yyloc;

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY;		/* Cause a token to be read.  */

  /* Initialize stack pointers.
     Waste one element of value and location stack
     so that they stay on the same level as the state stack.
     The wasted elements are never initialized.  */

  yyssp = yyss;
  yyvsp = yyvs;
  yylsp = yyls;
#if defined YYLTYPE_IS_TRIVIAL && YYLTYPE_IS_TRIVIAL
  /* Initialize the default location before parsing starts.  */
  yylloc.first_line   = yylloc.last_line   = 1;
  yylloc.first_column = yylloc.last_column = 0;
#endif

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
	YYLTYPE *yyls1 = yyls;

	/* Each stack pointer address is followed by the size of the
	   data in use in that stack, in bytes.  This used to be a
	   conditional around just the two extra args, but that might
	   be undefined if yyoverflow is a macro.  */
	yyoverflow (YY_("memory exhausted"),
		    &yyss1, yysize * sizeof (*yyssp),
		    &yyvs1, yysize * sizeof (*yyvsp),
		    &yyls1, yysize * sizeof (*yylsp),
		    &yystacksize);
	yyls = yyls1;
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
	YYSTACK_RELOCATE (yyss);
	YYSTACK_RELOCATE (yyvs);
	YYSTACK_RELOCATE (yyls);
#  undef YYSTACK_RELOCATE
	if (yyss1 != yyssa)
	  YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;
      yylsp = yyls + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
		  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
	YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     look-ahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to look-ahead token.  */
  yyn = yypact[yystate];
  if (yyn == YYPACT_NINF)
    goto yydefault;

  /* Not known => get a look-ahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid look-ahead symbol.  */
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

  if (yyn == YYFINAL)
    YYACCEPT;

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the look-ahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token unless it is eof.  */
  if (yychar != YYEOF)
    yychar = YYEMPTY;

  yystate = yyn;
  *++yyvsp = yylval;
  *++yylsp = yylloc;
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

  /* Default location.  */
  YYLLOC_DEFAULT (yyloc, (yylsp - yylen), yylen);
  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 216 "chuck.y"
    { (yyval.program) = g_program = new_program( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 3:
#line 217 "chuck.y"
    { (yyval.program) = g_program = append_program( (yyvsp[(1) - (2)].program), (yyvsp[(2) - (2)].program_section), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 4:
#line 221 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 5:
#line 222 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 6:
#line 223 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 7:
#line 228 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 8:
#line 230 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 9:
#line 232 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 10:
#line 234 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 11:
#line 238 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 12:
#line 239 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(4) - (4)].id_list), (yyvsp[(2) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 13:
#line 240 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (2)].id_list), NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 14:
#line 241 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (4)].id_list), (yyvsp[(4) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 15:
#line 245 "chuck.y"
    { (yyval.class_body) = (yyvsp[(1) - (1)].class_body); ;}
    break;

  case 16:
#line 246 "chuck.y"
    { (yyval.class_body) = NULL; ;}
    break;

  case 17:
#line 250 "chuck.y"
    { (yyval.class_body) = new_class_body( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 18:
#line 251 "chuck.y"
    { (yyval.class_body) = prepend_class_body( (yyvsp[(1) - (2)].program_section), (yyvsp[(2) - (2)].class_body), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 19:
#line 256 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 20:
#line 257 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 21:
#line 258 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 22:
#line 262 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 23:
#line 266 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1 */ ); ;}
    break;

  case 24:
#line 267 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1 */ ); ;}
    break;

  case 25:
#line 271 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1*/ ); ;}
    break;

  case 26:
#line 272 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1*/ ); ;}
    break;

  case 27:
#line 277 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), (yyvsp[(8) - (8)].stmt), TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 28:
#line 279 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 29:
#line 281 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), NULL, (yyvsp[(3) - (7)].sval), (yyvsp[(5) - (7)].arg_list), (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 30:
#line 283 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), (yyvsp[(2) - (6)].ival), NULL, (yyvsp[(3) - (6)].sval), NULL, (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 31:
#line 285 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@construct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 32:
#line 287 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@construct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 33:
#line 289 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@destruct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 34:
#line 291 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@destruct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 35:
#line 293 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), NULL, TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 36:
#line 295 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, NULL, TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 37:
#line 297 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(5) - (9)].ival), (yyvsp[(7) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, FALSE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(5) - (9)]).first_column ); ;}
    break;

  case 38:
#line 299 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(8) - (9)].ival), (yyvsp[(6) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, TRUE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(8) - (9)]).first_column ); ;}
    break;

  case 39:
#line 301 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(6) - (11)].ival), (yyvsp[(9) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, FALSE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(5) - (11)]).first_column ); ;}
    break;

  case 40:
#line 303 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(9) - (11)].ival), (yyvsp[(6) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, TRUE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(8) - (11)]).first_column ); ;}
    break;

  case 41:
#line 307 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 42:
#line 308 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 43:
#line 309 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 44:
#line 313 "chuck.y"
    { (yyval.ival) = ae_key_func; ;}
    break;

  case 45:
#line 314 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 46:
#line 315 "chuck.y"
    { (yyval.ival) = ae_key_protected; ;}
    break;

  case 47:
#line 316 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 48:
#line 320 "chuck.y"
    { (yyval.ival) = ae_key_static; ;}
    break;

  case 49:
#line 321 "chuck.y"
    { (yyval.ival) = ae_key_abstract; ;}
    break;

  case 50:
#line 322 "chuck.y"
    { (yyval.ival) = ae_key_instance; ;}
    break;

  case 51:
#line 326 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column /*, &@1*/ ), 0, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 52:
#line 327 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (2)].sval), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column /*, &@1*/ ), 1, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 53:
#line 331 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (3)].id_list), 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 54:
#line 332 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (4)].id_list), 1, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 55:
#line 341 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 56:
#line 342 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 57:
#line 347 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 58:
#line 348 "chuck.y"
    { (yyval.type_decl) = add_type_decl_array( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 59:
#line 352 "chuck.y"
    { (yyval.arg_list) = new_arg_list( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 60:
#line 353 "chuck.y"
    { (yyval.arg_list) = prepend_arg_list( (yyvsp[(1) - (4)].type_decl), (yyvsp[(2) - (4)].var_decl), (yyvsp[(4) - (4)].arg_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 61:
#line 357 "chuck.y"
    { (yyval.stmt_list) = new_stmt_list( (yyvsp[(1) - (1)].stmt), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 62:
#line 358 "chuck.y"
    { (yyval.stmt_list) = append_stmt_list( (yyvsp[(1) - (2)].stmt_list), (yyvsp[(2) - (2)].stmt), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 63:
#line 362 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 64:
#line 363 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 65:
#line 364 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 66:
#line 365 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 67:
#line 367 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 68:
#line 368 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 69:
#line 369 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 70:
#line 373 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 71:
#line 374 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 72:
#line 375 "chuck.y"
    { (yyval.stmt) = new_stmt_from_break( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 73:
#line 376 "chuck.y"
    { (yyval.stmt) = new_stmt_from_continue( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 74:
#line 381 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 75:
#line 383 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 76:
#line 388 "chuck.y"
    { (yyval.stmt) = new_stmt_from_while( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 77:
#line 390 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_while( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 78:
#line 392 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (6)].stmt), (yyvsp[(4) - (6)].stmt), NULL, (yyvsp[(6) - (6)].stmt), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 79:
#line 394 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (7)].stmt), (yyvsp[(4) - (7)].stmt), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 80:
#line 396 "chuck.y"
    { (yyval.stmt) = new_stmt_from_foreach( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 81:
#line 398 "chuck.y"
    { (yyval.stmt) = new_stmt_from_until( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 82:
#line 400 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_until( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 83:
#line 402 "chuck.y"
    { (yyval.stmt) = new_stmt_from_loop( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 84:
#line 406 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 85:
#line 407 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( (yyvsp[(2) - (3)].stmt_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 86:
#line 411 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(2) - (2)].import), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 87:
#line 412 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 88:
#line 413 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 89:
#line 414 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(3) - (4)].import), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 90:
#line 415 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(3) - (4)].import), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 91:
#line 419 "chuck.y"
    { (yyval.import) = (yyvsp[(1) - (1)].import); ;}
    break;

  case 92:
#line 420 "chuck.y"
    { (yyval.import) = prepend_import( (yyvsp[(1) - (3)].import), (yyvsp[(3) - (3)].import), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 93:
#line 424 "chuck.y"
    { (yyval.import) = new_import( (yyvsp[(1) - (1)].sval), NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 94:
#line 429 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(2) - (2)].doc), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 95:
#line 430 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(3) - (4)].doc), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 96:
#line 431 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(3) - (4)].doc), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 97:
#line 435 "chuck.y"
    { (yyval.doc) = (yyvsp[(1) - (1)].doc); ;}
    break;

  case 98:
#line 436 "chuck.y"
    { (yyval.doc) = prepend_doc( (yyvsp[(1) - (3)].doc), (yyvsp[(3) - (3)].doc), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 99:
#line 440 "chuck.y"
    { (yyval.doc) = new_doc( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 100:
#line 444 "chuck.y"
    { (yyval.stmt) = NULL; ;}
    break;

  case 101:
#line 445 "chuck.y"
    { (yyval.stmt) = new_stmt_from_expression( (yyvsp[(1) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 102:
#line 449 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 103:
#line 450 "chuck.y"
    { (yyval.exp) = append_expression( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 104:
#line 454 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 105:
#line 456 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 106:
#line 460 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 107:
#line 462 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 108:
#line 466 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 109:
#line 467 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 110:
#line 469 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(4) - (4)].array_sub), (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 111:
#line 471 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(5) - (5)].array_sub), (yyvsp[(2) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 112:
#line 475 "chuck.y"
    { (yyval.array_sub) = new_array_sub( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 113:
#line 476 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(1) - (3)].array_sub), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 114:
#line 480 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 115:
#line 481 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 116:
#line 482 "chuck.y"
    { (yyval.exp) = new_exp_decl_external( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 117:
#line 483 "chuck.y"
    { (yyval.exp) = new_exp_decl_global( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 118:
#line 484 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 119:
#line 485 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 120:
#line 486 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 121:
#line 490 "chuck.y"
    { (yyval.var_decl_list) = new_var_decl_list( (yyvsp[(1) - (1)].var_decl), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 122:
#line 491 "chuck.y"
    { (yyval.var_decl_list) = prepend_var_decl_list( (yyvsp[(1) - (3)].var_decl), (yyvsp[(3) - (3)].var_decl_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 123:
#line 495 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (1)].sval), FALSE, NULL, NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 124:
#line 496 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 125:
#line 497 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 126:
#line 498 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (3)].sval), TRUE, NULL, NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 127:
#line 499 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, (yyvsp[(3) - (4)].exp), NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 128:
#line 500 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, NULL, (yyvsp[(4) - (4)].array_sub), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 129:
#line 501 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (5)].sval), TRUE, (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 130:
#line 506 "chuck.y"
    { (yyval.complex_exp) = new_complex( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 131:
#line 511 "chuck.y"
    { (yyval.polar_exp) = new_polar( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 132:
#line 516 "chuck.y"
    { (yyval.vec_exp) = new_vec( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 133:
#line 520 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 134:
#line 521 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 135:
#line 522 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 136:
#line 523 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 137:
#line 524 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 138:
#line 525 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 139:
#line 526 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 140:
#line 527 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 141:
#line 528 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 142:
#line 529 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 143:
#line 530 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 144:
#line 531 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 145:
#line 532 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 146:
#line 533 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 147:
#line 534 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 148:
#line 538 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 149:
#line 539 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 150:
#line 540 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 151:
#line 541 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 152:
#line 542 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 153:
#line 543 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 154:
#line 547 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 155:
#line 549 "chuck.y"
    { (yyval.exp) = new_exp_from_if( (yyvsp[(1) - (5)].exp), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 156:
#line 553 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 157:
#line 555 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 158:
#line 559 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 159:
#line 561 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 160:
#line 565 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 161:
#line 567 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 162:
#line 571 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 163:
#line 573 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_xor, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 164:
#line 577 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 165:
#line 579 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 166:
#line 583 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 167:
#line 585 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_eq, (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 168:
#line 587 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_neq, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 169:
#line 591 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 170:
#line 593 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_lt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 171:
#line 595 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_gt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 172:
#line 597 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_le, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 173:
#line 599 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_ge, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 174:
#line 603 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 175:
#line 605 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_left, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 176:
#line 607 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_right, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 177:
#line 611 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 178:
#line 613 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_plus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 179:
#line 615 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_minus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 180:
#line 619 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 181:
#line 621 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_times, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 182:
#line 623 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_divide, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 183:
#line 625 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_percent, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 184:
#line 629 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 185:
#line 631 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_tilda, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 186:
#line 635 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 187:
#line 637 "chuck.y"
    { (yyval.exp) = new_exp_from_cast( (yyvsp[(3) - (3)].type_decl), (yyvsp[(1) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 188:
#line 641 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 189:
#line 643 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_plusplus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 190:
#line 645 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_minusminus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 191:
#line 647 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( (yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 192:
#line 649 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_typeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 193:
#line 651 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_sizeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 194:
#line 653 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (2)].type_decl), FALSE, NULL, NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 195:
#line 655 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (3)].type_decl), FALSE, NULL, (yyvsp[(3) - (3)].array_sub), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 196:
#line 657 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (4)].type_decl), TRUE, NULL, NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 197:
#line 659 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, (yyvsp[(4) - (5)].exp), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 198:
#line 661 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, NULL, (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 199:
#line 663 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (6)].type_decl), TRUE, (yyvsp[(4) - (6)].exp), (yyvsp[(6) - (6)].array_sub), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 200:
#line 669 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 201:
#line 670 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 202:
#line 671 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 203:
#line 672 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 204:
#line 673 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 205:
#line 674 "chuck.y"
    { (yyval.ival) = ae_op_spork; ;}
    break;

  case 206:
#line 675 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 207:
#line 681 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 208:
#line 682 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 209:
#line 683 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 210:
#line 684 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 211:
#line 685 "chuck.y"
    { (yyval.ival) = ae_op_divide; ;}
    break;

  case 212:
#line 686 "chuck.y"
    { (yyval.ival) = ae_op_percent; ;}
    break;

  case 213:
#line 687 "chuck.y"
    { (yyval.ival) = ae_op_eq; ;}
    break;

  case 214:
#line 688 "chuck.y"
    { (yyval.ival) = ae_op_neq; ;}
    break;

  case 215:
#line 689 "chuck.y"
    { (yyval.ival) = ae_op_lt; ;}
    break;

  case 216:
#line 690 "chuck.y"
    { (yyval.ival) = ae_op_le; ;}
    break;

  case 217:
#line 691 "chuck.y"
    { (yyval.ival) = ae_op_gt; ;}
    break;

  case 218:
#line 692 "chuck.y"
    { (yyval.ival) = ae_op_ge; ;}
    break;

  case 219:
#line 693 "chuck.y"
    { (yyval.ival) = ae_op_and; ;}
    break;

  case 220:
#line 694 "chuck.y"
    { (yyval.ival) = ae_op_or; ;}
    break;

  case 221:
#line 695 "chuck.y"
    { (yyval.ival) = ae_op_assign; ;}
    break;

  case 222:
#line 696 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 223:
#line 697 "chuck.y"
    { (yyval.ival) = ae_op_s_or; ;}
    break;

  case 224:
#line 698 "chuck.y"
    { (yyval.ival) = ae_op_s_and; ;}
    break;

  case 225:
#line 699 "chuck.y"
    { (yyval.ival) = ae_op_s_xor; ;}
    break;

  case 226:
#line 700 "chuck.y"
    { (yyval.ival) = ae_op_plusplus; ;}
    break;

  case 227:
#line 701 "chuck.y"
    { (yyval.ival) = ae_op_minusminus; ;}
    break;

  case 228:
#line 702 "chuck.y"
    { (yyval.ival) = ae_op_dollar; ;}
    break;

  case 229:
#line 703 "chuck.y"
    { (yyval.ival) = ae_op_at_at; ;}
    break;

  case 230:
#line 704 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 231:
#line 705 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 232:
#line 706 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 233:
#line 707 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 234:
#line 708 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 235:
#line 709 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 236:
#line 710 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 237:
#line 711 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 238:
#line 712 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 239:
#line 713 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 240:
#line 714 "chuck.y"
    { (yyval.ival) = ae_op_shift_right; ;}
    break;

  case 241:
#line 715 "chuck.y"
    { (yyval.ival) = ae_op_shift_left; ;}
    break;

  case 242:
#line 716 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 243:
#line 717 "chuck.y"
    { (yyval.ival) = ae_op_coloncolon; ;}
    break;

  case 244:
#line 718 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 245:
#line 719 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 246:
#line 720 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 247:
#line 721 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 248:
#line 722 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 249:
#line 723 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 250:
#line 724 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 251:
#line 725 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 252:
#line 726 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 253:
#line 727 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 255:
#line 733 "chuck.y"
    { (yyval.exp) = new_exp_from_dur( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 256:
#line 737 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 257:
#line 739 "chuck.y"
    { (yyval.exp) = new_exp_from_array( (yyvsp[(1) - (2)].exp), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 258:
#line 741 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (3)].exp), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 259:
#line 743 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (4)].exp), (yyvsp[(3) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 260:
#line 745 "chuck.y"
    { (yyval.exp) = new_exp_from_member_dot( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].sval), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column ); ;}
    break;

  case 261:
#line 747 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_plusplus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 262:
#line 749 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_minusminus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 263:
#line 754 "chuck.y"
    { (yyval.exp) = new_exp_from_id( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 264:
#line 755 "chuck.y"
    { (yyval.exp) = new_exp_from_int( (yyvsp[(1) - (1)].ival), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 265:
#line 756 "chuck.y"
    { (yyval.exp) = new_exp_from_float( (yyvsp[(1) - (1)].fval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 266:
#line 757 "chuck.y"
    { (yyval.exp) = new_exp_from_str( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 267:
#line 758 "chuck.y"
    { (yyval.exp) = new_exp_from_char( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 268:
#line 759 "chuck.y"
    { (yyval.exp) = new_exp_from_array_lit( (yyvsp[(1) - (1)].array_sub), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 269:
#line 760 "chuck.y"
    { (yyval.exp) = new_exp_from_complex( (yyvsp[(1) - (1)].complex_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 270:
#line 761 "chuck.y"
    { (yyval.exp) = new_exp_from_polar( (yyvsp[(1) - (1)].polar_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 271:
#line 762 "chuck.y"
    { (yyval.exp) = new_exp_from_vec( (yyvsp[(1) - (1)].vec_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 272:
#line 763 "chuck.y"
    { (yyval.exp) = new_exp_from_hack( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 273:
#line 764 "chuck.y"
    { (yyval.exp) = (yyvsp[(2) - (3)].exp); ;}
    break;

  case 274:
#line 765 "chuck.y"
    { (yyval.exp) = new_exp_from_nil( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3893 "chuck.tab.c"
      default: break;
    }
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;
  *++yylsp = yyloc;

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

  yyerror_range[0] = yylloc;

  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse look-ahead token after an
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
		      yytoken, &yylval, &yylloc);
	  yychar = YYEMPTY;
	}
    }

  /* Else will try to reuse look-ahead token after shifting the error
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

  yyerror_range[0] = yylsp[1-yylen];
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

      yyerror_range[0] = *yylsp;
      yydestruct ("Error: popping",
		  yystos[yystate], yyvsp, yylsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  if (yyn == YYFINAL)
    YYACCEPT;

  *++yyvsp = yylval;

  yyerror_range[1] = yylloc;
  /* Using YYLLOC is tempting, but would change the location of
     the look-ahead.  YYLOC is available though.  */
  YYLLOC_DEFAULT (yyloc, (yyerror_range - 1), 2);
  *++yylsp = yyloc;

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

#ifndef yyoverflow
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEOF && yychar != YYEMPTY)
     yydestruct ("Cleanup: discarding lookahead",
		 yytoken, &yylval, &yylloc);
  /* Do not reclaim the symbols of the rule which action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
		  yystos[*yyssp], yyvsp, yylsp);
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



