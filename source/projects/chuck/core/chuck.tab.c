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
     AT_DOC = 376,
     AT_EXAMPLE = 377
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
#define AT_EXAMPLE 377




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
    a_Example example; // 1.5.5.8 (ge,nick,alex) added
}
/* Line 193 of yacc.c.  */
#line 426 "chuck.tab.c"
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
#line 451 "chuck.tab.c"

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
#define YYFINAL  141
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   2393

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  123
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  68
/* YYNRULES -- Number of rules.  */
#define YYNRULES  281
/* YYNRULES -- Number of states.  */
#define YYNSTATES  476

/* YYTRANSLATE(YYLEX) -- Bison symbol number corresponding to YYLEX.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   377

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
     115,   116,   117,   118,   119,   120,   121,   122
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
     276,   278,   281,   285,   288,   291,   297,   305,   311,   319,
     326,   334,   342,   348,   356,   362,   365,   369,   372,   376,
     380,   385,   390,   392,   396,   398,   401,   406,   411,   413,
     417,   419,   422,   427,   432,   434,   438,   440,   442,   445,
     447,   451,   453,   457,   459,   463,   467,   472,   477,   483,
     486,   490,   492,   495,   499,   503,   507,   510,   514,   516,
     520,   522,   525,   528,   532,   537,   542,   548,   552,   556,
     560,   562,   564,   566,   568,   570,   572,   574,   576,   578,
     580,   582,   584,   586,   588,   590,   592,   594,   596,   598,
     600,   602,   604,   610,   612,   616,   618,   622,   624,   628,
     630,   634,   636,   640,   642,   646,   650,   652,   656,   660,
     664,   668,   670,   674,   678,   680,   684,   688,   690,   694,
     698,   702,   704,   708,   710,   714,   716,   719,   722,   725,
     728,   731,   734,   738,   743,   749,   755,   762,   764,   766,
     768,   770,   772,   775,   777,   779,   781,   783,   785,   787,
     789,   791,   793,   795,   797,   799,   801,   803,   805,   807,
     809,   811,   813,   815,   817,   819,   821,   823,   825,   827,
     829,   831,   833,   835,   837,   839,   841,   843,   845,   847,
     849,   851,   853,   855,   857,   859,   861,   863,   865,   867,
     869,   871,   873,   877,   879,   882,   886,   891,   895,   898,
     901,   903,   905,   907,   909,   911,   913,   915,   917,   919,
     923,   927
};

/* YYRHS -- A `-1'-separated list of the rules' RHS.  */
static const yytype_int16 yyrhs[] =
{
     124,     0,    -1,   125,    -1,   124,   125,    -1,   143,    -1,
     134,    -1,   126,    -1,   135,    98,   132,    16,   128,    17,
      -1,   135,    98,   132,   127,    16,   128,    17,    -1,   135,
      99,   132,    16,   128,    17,    -1,   135,    99,   132,   131,
      16,   128,    17,    -1,   101,   132,    -1,   101,   132,   100,
     133,    -1,   100,   133,    -1,   100,   133,   101,   132,    -1,
     129,    -1,    -1,   130,    -1,   130,   129,    -1,   143,    -1,
     134,    -1,   126,    -1,   100,   132,    -1,     3,    -1,     3,
       9,   132,    -1,     3,    -1,     3,    18,   133,    -1,   136,
     137,   141,     3,    12,   142,    13,   148,    -1,   136,   137,
     141,     3,    12,    13,   148,    -1,   136,   137,     3,    12,
     142,    13,   148,    -1,   136,   137,     3,    12,    13,   148,
      -1,   136,   118,    12,   142,    13,   148,    -1,   136,   118,
      12,    13,   148,    -1,   136,   119,    12,    13,   148,    -1,
     136,   119,    12,   142,    13,   148,    -1,   136,   137,   141,
       3,    12,   142,    13,    11,    -1,   136,   137,   141,     3,
      12,    13,    11,    -1,   136,   137,   141,   117,   187,    12,
     142,    13,   148,    -1,   136,   137,   141,   117,    12,   142,
      13,   187,   148,    -1,   136,   137,   141,   117,    12,   187,
      13,    12,   142,    13,   148,    -1,   136,   137,   141,   117,
      12,   142,    13,    12,   187,    13,   148,    -1,   102,    -1,
     104,    -1,    -1,    43,    -1,   102,    -1,   103,    -1,   104,
      -1,   105,    -1,   106,    -1,    -1,     3,    -1,     3,    71,
      -1,    26,   133,    28,    -1,    26,   133,    28,    71,    -1,
     138,    -1,   139,    -1,   140,    -1,   140,   163,    -1,   140,
     166,    -1,   140,   166,     9,   142,    -1,   144,    -1,   143,
     144,    -1,   158,    -1,   147,    -1,   146,    -1,   145,    -1,
     148,    -1,   149,    -1,   152,    -1,   155,    -1,    44,    11,
      -1,    44,   159,    11,    -1,    40,    11,    -1,    41,    11,
      -1,    33,    12,   159,    13,   144,    -1,    33,    12,   159,
      13,   144,    35,   144,    -1,    36,    12,   159,    13,   144,
      -1,    38,   144,    36,    12,   159,    13,    11,    -1,    37,
      12,   158,   158,    13,   144,    -1,    37,    12,   158,   158,
     159,    13,   144,    -1,    37,    12,   159,    10,   159,    13,
     144,    -1,    64,    12,   159,    13,   144,    -1,    38,   144,
      64,    12,   159,    13,    11,    -1,    39,    12,   159,    13,
     144,    -1,    16,    17,    -1,    16,   143,    17,    -1,   120,
     151,    -1,   120,    14,    15,    -1,   120,    16,    17,    -1,
     120,    14,   150,    14,    -1,   120,    16,   150,    17,    -1,
     151,    -1,   151,     9,   150,    -1,     4,    -1,   121,   154,
      -1,   121,    14,   153,    15,    -1,   121,    16,   153,    17,
      -1,   154,    -1,   154,     9,   153,    -1,     4,    -1,   122,
     157,    -1,   122,    14,   156,    15,    -1,   122,    16,   156,
      17,    -1,   157,    -1,   157,     9,   156,    -1,     4,    -1,
      11,    -1,   159,    11,    -1,   160,    -1,   159,     9,   160,
      -1,   161,    -1,   160,   170,   161,    -1,   164,    -1,   161,
     171,   164,    -1,    14,   159,    15,    -1,    14,   159,     9,
      15,    -1,    14,   159,    15,   162,    -1,    14,   159,     9,
      15,   162,    -1,    14,    15,    -1,   163,    14,    15,    -1,
     172,    -1,   140,   165,    -1,    65,   140,   165,    -1,    66,
     140,   165,    -1,   105,   140,   165,    -1,    76,   165,    -1,
     105,    76,   165,    -1,   166,    -1,   166,     9,   165,    -1,
       3,    -1,     3,   162,    -1,     3,   163,    -1,     3,    12,
      13,    -1,     3,    12,   159,    13,    -1,     3,    12,    13,
     162,    -1,     3,    12,   159,    13,   162,    -1,    53,   159,
      13,    -1,    54,   159,    13,    -1,    55,   159,    13,    -1,
      90,    -1,    93,    -1,    77,    -1,    78,    -1,    79,    -1,
      80,    -1,    84,    -1,    85,    -1,    86,    -1,    95,    -1,
      96,    -1,    97,    -1,    81,    -1,    82,    -1,    83,    -1,
     110,    -1,   109,    -1,   114,    -1,   113,    -1,   116,    -1,
     115,    -1,   173,    -1,   173,    45,   159,    10,   172,    -1,
     174,    -1,   173,    31,   174,    -1,   175,    -1,   174,    30,
     175,    -1,   176,    -1,   175,    47,   176,    -1,   177,    -1,
     176,    49,   177,    -1,   178,    -1,   177,    48,   178,    -1,
     179,    -1,   178,    24,   179,    -1,   178,    25,   179,    -1,
     180,    -1,   179,    26,   180,    -1,   179,    28,   180,    -1,
     179,    27,   180,    -1,   179,    29,   180,    -1,   181,    -1,
     180,    88,   181,    -1,   180,    87,   181,    -1,   182,    -1,
     181,    19,   182,    -1,   181,    20,   182,    -1,   183,    -1,
     182,    21,   183,    -1,   182,    22,   183,    -1,   182,    23,
     183,    -1,   184,    -1,   183,    89,   184,    -1,   185,    -1,
     184,    52,   140,    -1,   188,    -1,    50,   185,    -1,    51,
     185,    -1,   186,   185,    -1,    75,   185,    -1,    74,   185,
      -1,    73,   140,    -1,    73,   140,   162,    -1,    73,   140,
      12,    13,    -1,    73,   140,    12,   159,    13,    -1,    73,
     140,    12,    13,   162,    -1,    73,   140,    12,   159,    13,
     162,    -1,    19,    -1,    20,    -1,    89,    -1,    46,    -1,
      21,    -1,   108,    89,    -1,    97,    -1,    90,    -1,    19,
      -1,    20,    -1,    21,    -1,    22,    -1,    23,    -1,    24,
      -1,    25,    -1,    26,    -1,    27,    -1,    28,    -1,    29,
      -1,    30,    -1,    31,    -1,    32,    -1,    46,    -1,    47,
      -1,    48,    -1,    49,    -1,    50,    -1,    51,    -1,    52,
      -1,    72,    -1,    77,    -1,    78,    -1,    79,    -1,    80,
      -1,    81,    -1,    82,    -1,    83,    -1,    84,    -1,    85,
      -1,    86,    -1,    87,    -1,    88,    -1,    89,    -1,    91,
      -1,    93,    -1,    95,    -1,    96,    -1,    97,    -1,   109,
      -1,   110,    -1,   113,    -1,   114,    -1,   115,    -1,   116,
      -1,   189,    -1,   188,    91,   189,    -1,   190,    -1,   189,
     162,    -1,   189,    12,    13,    -1,   189,    12,   159,    13,
      -1,   189,    18,     3,    -1,   189,    50,    -1,   189,    51,
      -1,     3,    -1,     6,    -1,     7,    -1,     4,    -1,     5,
      -1,   162,    -1,   167,    -1,   168,    -1,   169,    -1,   111,
     159,   112,    -1,    12,   159,    13,    -1,    12,    13,    -1
};

/* YYRLINE[YYN] -- source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   221,   221,   222,   226,   227,   228,   232,   234,   236,
     238,   243,   244,   245,   246,   250,   251,   255,   256,   261,
     262,   263,   267,   271,   272,   276,   277,   281,   283,   285,
     287,   289,   291,   293,   295,   297,   299,   301,   303,   305,
     307,   312,   313,   314,   318,   319,   320,   321,   325,   326,
     327,   331,   332,   336,   337,   346,   347,   352,   353,   357,
     358,   362,   363,   367,   368,   369,   370,   372,   373,   374,
     375,   379,   380,   381,   382,   386,   388,   393,   395,   397,
     399,   401,   403,   405,   407,   412,   413,   417,   418,   419,
     420,   421,   425,   426,   430,   435,   436,   437,   441,   442,
     446,   450,   451,   452,   456,   457,   461,   465,   466,   470,
     471,   475,   476,   481,   482,   487,   488,   489,   491,   496,
     497,   501,   502,   503,   504,   505,   506,   507,   511,   512,
     516,   517,   518,   519,   520,   521,   522,   526,   531,   536,
     541,   542,   543,   544,   545,   546,   547,   548,   549,   550,
     551,   552,   553,   554,   555,   559,   560,   561,   562,   563,
     564,   568,   569,   574,   575,   580,   581,   586,   587,   592,
     593,   598,   599,   604,   605,   607,   612,   613,   615,   617,
     619,   624,   625,   627,   632,   633,   635,   640,   641,   643,
     645,   650,   651,   656,   657,   662,   663,   665,   667,   669,
     671,   673,   675,   677,   679,   681,   683,   690,   691,   692,
     693,   694,   695,   696,   702,   703,   704,   705,   706,   707,
     708,   709,   710,   711,   712,   713,   714,   715,   716,   717,
     718,   719,   720,   721,   722,   723,   724,   725,   726,   727,
     728,   729,   730,   731,   732,   733,   734,   735,   736,   737,
     738,   739,   740,   741,   742,   743,   744,   745,   746,   747,
     748,   752,   753,   758,   759,   761,   763,   765,   767,   769,
     775,   776,   777,   778,   779,   780,   781,   782,   783,   784,
     785,   786
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
  "AT_DTOR", "AT_IMPORT", "AT_DOC", "AT_EXAMPLE", "$accept", "program",
  "program_section", "class_definition", "class_ext", "class_body",
  "class_body2", "class_section", "iface_ext", "id_list", "id_dot",
  "function_definition", "class_decl", "function_decl", "static_decl",
  "type_decl_a", "type_decl_b", "type_decl", "type_decl2", "arg_list",
  "statement_list", "statement", "jump_statement", "selection_statement",
  "loop_statement", "code_segment", "import_statement", "import_list",
  "import_target", "doc_statement", "doc_list", "doc_target",
  "example_statement", "example_list", "example_target",
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
     375,   376,   377
};
# endif

/* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   123,   124,   124,   125,   125,   125,   126,   126,   126,
     126,   127,   127,   127,   127,   128,   128,   129,   129,   130,
     130,   130,   131,   132,   132,   133,   133,   134,   134,   134,
     134,   134,   134,   134,   134,   134,   134,   134,   134,   134,
     134,   135,   135,   135,   136,   136,   136,   136,   137,   137,
     137,   138,   138,   139,   139,   140,   140,   141,   141,   142,
     142,   143,   143,   144,   144,   144,   144,   144,   144,   144,
     144,   145,   145,   145,   145,   146,   146,   147,   147,   147,
     147,   147,   147,   147,   147,   148,   148,   149,   149,   149,
     149,   149,   150,   150,   151,   152,   152,   152,   153,   153,
     154,   155,   155,   155,   156,   156,   157,   158,   158,   159,
     159,   160,   160,   161,   161,   162,   162,   162,   162,   163,
     163,   164,   164,   164,   164,   164,   164,   164,   165,   165,
     166,   166,   166,   166,   166,   166,   166,   167,   168,   169,
     170,   170,   170,   170,   170,   170,   170,   170,   170,   170,
     170,   170,   170,   170,   170,   171,   171,   171,   171,   171,
     171,   172,   172,   173,   173,   174,   174,   175,   175,   176,
     176,   177,   177,   178,   178,   178,   179,   179,   179,   179,
     179,   180,   180,   180,   181,   181,   181,   182,   182,   182,
     182,   183,   183,   184,   184,   185,   185,   185,   185,   185,
     185,   185,   185,   185,   185,   185,   185,   186,   186,   186,
     186,   186,   186,   186,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   187,   187,   187,   187,   187,   187,   187,   187,   187,
     187,   188,   188,   189,   189,   189,   189,   189,   189,   189,
     190,   190,   190,   190,   190,   190,   190,   190,   190,   190,
     190,   190
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
       1,     2,     3,     2,     2,     5,     7,     5,     7,     6,
       7,     7,     5,     7,     5,     2,     3,     2,     3,     3,
       4,     4,     1,     3,     1,     2,     4,     4,     1,     3,
       1,     2,     4,     4,     1,     3,     1,     1,     2,     1,
       3,     1,     3,     1,     3,     3,     4,     4,     5,     2,
       3,     1,     2,     3,     3,     3,     2,     3,     1,     3,
       1,     2,     2,     3,     4,     4,     5,     3,     3,     3,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     5,     1,     3,     1,     3,     1,     3,     1,
       3,     1,     3,     1,     3,     3,     1,     3,     3,     3,
       3,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     1,     3,     1,     3,     1,     2,     2,     2,     2,
       2,     2,     3,     4,     5,     5,     6,     1,     1,     1,
       1,     1,     2,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     3,     1,     2,     3,     4,     3,     2,     2,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     3,
       3,     2
};

/* YYDEFACT[STATE-NAME] -- Default rule to reduce with in state
   STATE-NUM when YYTABLE doesn't specify something else to do.  Zero
   means the default is an error.  */
static const yytype_uint16 yydefact[] =
{
      43,   270,   273,   274,   271,   272,   107,     0,     0,     0,
     207,   208,   211,     0,     0,     0,     0,     0,     0,     0,
       0,    44,     0,   210,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   209,   213,    45,    46,
      47,     0,     0,     0,     0,     0,     0,    43,     2,     6,
       5,     0,    50,    55,    56,     0,     4,    61,    66,    65,
      64,    67,    68,    69,    70,    63,     0,   109,   111,   275,
     113,   276,   277,   278,   121,   161,   163,   165,   167,   169,
     171,   173,   176,   181,   184,   187,   191,   193,     0,   195,
     261,   263,    52,   281,     0,     0,    85,     0,    25,     0,
       0,     0,     0,     0,     0,    73,    74,    71,     0,   270,
     196,   197,     0,     0,     0,     0,    51,     0,     0,   201,
     200,   199,   130,   126,   128,     0,     0,   212,     0,    94,
       0,     0,    87,   100,     0,     0,    95,   106,     0,     0,
     101,     1,     3,     0,     0,    48,    49,     0,     0,     0,
     122,    62,     0,   108,   142,   143,   144,   145,   152,   153,
     154,   146,   147,   148,   140,   141,   149,   150,   151,     0,
     156,   155,   158,   157,   160,   159,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   198,     0,
       0,     0,   268,   269,   264,   280,     0,   115,    86,     0,
      53,     0,     0,     0,     0,     0,     0,     0,    72,   137,
     138,   139,     0,   123,   124,     0,   202,     0,     0,   131,
     132,     0,   127,   125,   279,    88,     0,    92,    89,     0,
       0,    98,     0,     0,   104,     0,    23,     0,     0,     0,
       0,    51,    57,     0,   110,   112,   114,   164,     0,   166,
     168,   170,   172,   174,   175,   177,   179,   178,   180,   183,
     182,   185,   186,   188,   189,   190,   192,   194,   262,   265,
       0,   267,   116,   117,    26,    54,     0,     0,     0,     0,
       0,     0,     0,     0,   203,     0,   133,     0,   119,     0,
     129,    90,     0,    91,    96,     0,    97,   102,     0,   103,
       0,    43,     0,     0,     0,    43,     0,     0,     0,     0,
       0,     0,     0,     0,     0,    58,     0,     0,     0,   266,
     118,    75,    77,     0,     0,     0,     0,     0,    84,    82,
     205,   204,   135,   134,   120,    93,    99,   105,    24,    21,
       0,    15,    43,    20,    19,    13,    11,    43,     0,    22,
      43,    32,    59,     0,    33,     0,     0,     0,     0,     0,
     215,   216,   217,   218,   219,   220,   221,   222,   223,   224,
     225,   226,   227,   228,   229,   230,   231,   232,   233,   234,
     235,   236,   237,   238,   239,   240,   241,   242,   243,   244,
     245,   246,   247,   248,   249,   214,   250,   251,   252,   253,
     254,   255,   256,   257,   258,   259,   260,     0,   162,     0,
      79,     0,     0,     0,     0,   206,   136,     7,    18,     0,
       0,     0,     9,     0,     0,    31,    34,    30,     0,     0,
       0,   222,     0,     0,     0,    76,    80,    81,    78,    83,
      14,    12,     8,    10,    60,    29,    36,    28,     0,     0,
       0,     0,    35,    27,     0,     0,     0,     0,     0,    38,
       0,    37,     0,     0,    40,    39
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,    47,    48,   349,   314,   350,   351,   352,   317,   247,
      99,   353,    51,    52,   149,    53,    54,    55,   253,   320,
     354,    57,    58,    59,    60,    61,    62,   236,   237,    63,
     240,   241,    64,   243,   244,    65,    66,    67,    68,    69,
     230,    70,   123,   124,    71,    72,    73,   169,   176,    74,
      75,    76,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,   417,    89,    90,    91
};

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
#define YYPACT_NINF -348
static const yytype_int16 yypact[] =
{
     681,    20,  -348,  -348,  -348,  -348,  -348,  1235,  1913,   793,
    -348,  -348,  -348,    28,    77,   103,   196,  1017,   201,   207,
     213,  -348,  1309,  -348,  1987,  1987,  1913,  1913,  1913,   240,
      55,    55,    55,  1987,  1987,   202,  -348,  -348,   -58,  -348,
     -28,    22,   167,  1913,   129,   140,   144,   345,  -348,  -348,
    -348,    51,    34,  -348,  -348,   202,  1017,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,   166,   158,   112,  -348,
    -348,  -348,  -348,  -348,  -348,     8,   228,   235,   236,   238,
     138,   188,   162,   251,   224,   198,   239,  -348,  1987,   199,
     118,  -348,  -348,  -348,   100,    81,  -348,   905,   270,   264,
    1913,  1913,  1383,    -7,  1913,  -348,  -348,  -348,   193,  -348,
    -348,  -348,   107,   122,   133,  1913,   227,   202,   202,   197,
    -348,  -348,   218,  -348,   290,   202,   202,  -348,    10,  -348,
      57,    84,  -348,  -348,   296,   296,  -348,  -348,   297,   297,
    -348,  -348,  -348,   299,   299,  -348,  -348,   291,   292,    66,
    -348,  -348,  1913,  -348,  -348,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  1913,
    -348,  -348,  -348,  -348,  -348,  -348,  1913,  1987,  1913,  1987,
    1987,  1987,  1987,  1987,  1987,  1987,  1987,  1987,  1987,  1987,
    1987,  1987,  1987,  1987,  1987,  1987,  1987,    55,  -348,    40,
    1457,   302,  -348,  -348,  -348,  -348,  1537,   293,  -348,    28,
     237,   146,   152,  1383,   255,   294,   298,   165,  -348,  -348,
    -348,  -348,   174,  -348,  -348,  1611,  -348,  1685,  1765,  -348,
     295,   202,  -348,  -348,  -348,  -348,   300,   303,  -348,   301,
     305,   304,   306,   307,   308,   309,   312,     2,     5,    53,
      60,    14,   310,    12,   158,   112,  -348,   228,   265,   235,
     236,   238,   138,   188,   188,   162,   162,   162,   162,   251,
     251,   224,   224,   198,   198,   198,   239,  -348,   118,  -348,
     176,  -348,   293,  -348,  -348,  -348,  1017,  1017,  1839,  1913,
    1913,  1913,  1017,  1017,   293,   177,   293,   179,  -348,   313,
    -348,  -348,   311,  -348,  -348,   296,  -348,  -348,   297,  -348,
     299,   457,    28,   299,   314,   457,   299,   315,   316,   202,
     320,   316,   321,   115,   322,   295,   317,  2080,  1987,  -348,
    -348,   276,  -348,  1017,   184,   186,   187,   194,  -348,  -348,
    -348,   293,  -348,   293,  -348,  -348,  -348,  -348,  -348,  -348,
     318,  -348,   569,  -348,  1017,   215,   225,   457,   319,  -348,
     457,  -348,   332,   316,  -348,   316,   316,   329,   121,  1121,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,   331,  -348,  1017,
    -348,  1017,  1017,   333,   335,  -348,  -348,  -348,  -348,   299,
      28,   330,  -348,   336,    55,  -348,  -348,  -348,   316,    71,
     341,    28,   342,   347,    55,  -348,  -348,  -348,  -348,  -348,
    -348,  -348,  -348,  -348,  -348,  -348,  -348,  -348,    88,  2179,
     346,   349,  -348,  -348,  2277,   316,    55,   316,   350,  -348,
     354,  -348,   316,   316,  -348,  -348
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -348,  -348,   280,    30,  -348,  -260,    16,  -348,  -348,  -140,
    -201,    37,  -348,  -348,  -348,  -348,  -348,   -30,  -348,  -243,
      27,   -14,  -348,  -348,  -348,  -301,  -348,  -122,   325,  -348,
    -123,   327,  -348,  -129,   324,   -86,     6,  -128,   204,   -84,
     123,   200,   -50,    58,  -348,  -348,  -348,  -348,  -348,    46,
    -348,   203,   208,   210,   211,   212,    93,    75,    91,    92,
      74,   183,    26,  -348,  -347,  -348,   214,  -348
};

/* YYTABLE[YYPACT[STATE-NUM]].  What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule which
   number is the opposite.  If zero, do what YYDEFACT says.
   If YYTABLE_NINF, syntax error.  */
#define YYTABLE_NINF -52
static const yytype_int16 yytable[] =
{
     117,   118,   119,   103,   248,   150,   204,   322,   284,   239,
     245,   126,   242,    94,    95,   326,   213,   361,   311,   152,
     364,   315,   443,   -51,   254,   116,   323,    56,   108,   215,
      49,    98,   112,   113,   114,   226,    97,    50,   229,   177,
     -41,   -41,   151,   109,     2,     3,     4,     5,    13,   128,
     110,   111,     7,   178,     8,   358,   116,   216,   116,   120,
     121,   129,   435,   116,   436,   437,   318,   223,   224,   251,
     -42,   -42,   235,   321,    56,   232,   233,    49,   254,    13,
     367,    13,   456,   151,    50,    92,    13,     9,   129,   100,
     206,    92,    13,    26,    27,    28,   207,   431,   125,   462,
     433,   238,   312,   313,     9,   316,   211,   212,   214,   152,
     217,   355,   465,   205,   198,   101,   152,   468,   116,   252,
     219,   222,   234,   283,   116,   440,   442,   288,   366,   327,
     200,   152,     8,   129,   439,   220,   201,   455,   457,   145,
     146,    13,   152,   130,   133,   131,   221,    13,   137,   143,
     144,    43,   147,   148,   134,   152,   135,   463,   138,   286,
     139,   152,   183,   184,   469,   287,   471,   277,   202,   203,
     348,   474,   475,   356,   152,   152,   359,   153,   292,   347,
     345,   300,   346,   152,   258,   152,   152,   293,   152,   329,
     341,   454,   343,   152,   204,   152,   152,   421,   330,   422,
     423,   461,   152,   152,   218,   122,   280,   424,   102,   225,
     340,     8,   342,   104,   185,   186,   187,   188,   105,   319,
     319,   170,   171,   470,   106,   172,   173,   174,   175,   451,
     227,   295,   228,   297,    95,   154,   155,   156,   157,   158,
     159,   160,   161,   162,   163,   193,   194,   195,   164,   189,
     190,   165,   115,   166,   167,   168,   127,   425,   179,   426,
     265,   266,   267,   268,   152,   289,   153,   273,   274,   275,
     191,   192,   331,   332,   152,   328,   263,   264,   338,   339,
     269,   270,   180,   271,   272,   181,   182,   196,   209,   450,
     199,   197,   210,   319,   334,   335,   336,   337,    92,   231,
     133,   137,   246,   249,   250,   281,   290,     8,   285,   299,
     291,   419,   302,   305,   301,   129,   429,   308,   303,   420,
     304,   310,   307,   306,   324,   430,   309,   142,   344,   368,
     357,   360,     9,   363,   365,   427,   432,   298,   319,   319,
     151,   434,   438,   444,   448,   141,   449,   452,     1,     2,
       3,     4,     5,   453,   458,   459,     6,     7,   466,     8,
     460,     9,   467,   472,    10,    11,    12,   473,   428,   132,
     140,    13,   136,   255,   418,   325,   256,   362,    14,   276,
     257,    15,    16,    17,    18,    19,    20,   259,    21,    22,
     260,    23,   261,     0,   262,    24,    25,     0,    26,    27,
      28,     0,     0,     0,   319,   445,     0,   446,   447,    29,
      30,    31,     0,   278,   319,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,   319,     0,     0,     0,
       0,     0,    37,     0,     0,     0,     0,    38,    39,    40,
      41,     0,     0,    42,     0,     0,    43,     0,     0,     0,
       1,     2,     3,     4,     5,    44,    45,    46,     6,     7,
       0,     8,     0,     9,   -16,     0,    10,    11,    12,     0,
       0,     0,     0,    13,     0,     0,     0,     0,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,     0,
      21,    22,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,    38,
      39,    40,    41,     0,     0,    42,     0,     0,    43,     0,
       0,     0,     1,     2,     3,     4,     5,    44,    45,    46,
       6,     7,     0,     8,     0,     9,   -17,     0,    10,    11,
      12,     0,     0,     0,     0,    13,     0,     0,     0,     0,
       0,     0,    14,     0,     0,    15,    16,    17,    18,    19,
      20,     0,    21,    22,     0,    23,     0,     0,     0,    24,
      25,     0,    26,    27,    28,     0,     0,     0,     0,     0,
       0,     0,     0,    29,    30,    31,     0,     0,     0,     0,
       0,     0,    32,    33,    34,    35,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,    36,     0,
       0,     0,     0,     0,     0,     0,    37,     0,     0,     0,
       0,    38,    39,    40,    41,     0,     0,    42,     0,     0,
      43,     0,     0,     0,     1,     2,     3,     4,     5,    44,
      45,    46,     6,     7,     0,     8,     0,     9,     0,     0,
      10,    11,    12,     0,     0,     0,     0,    13,     0,     0,
       0,     0,     0,     0,    14,     0,     0,    15,    16,    17,
      18,    19,    20,     0,    21,    22,     0,    23,     0,     0,
       0,    24,    25,     0,    26,    27,    28,     0,     0,     0,
       0,     0,     0,     0,     0,    29,    30,    31,     0,     0,
       0,     0,     0,     0,    32,    33,    34,    35,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      36,     0,     0,     0,     0,     0,     0,     0,    37,     0,
       0,     0,     0,    38,    39,    40,    41,     0,     0,    42,
       0,     0,    43,     0,     0,     0,     1,     2,     3,     4,
       5,    44,    45,    46,     6,     7,     0,     8,     0,     9,
      96,     0,    10,    11,    12,     0,     0,     0,     0,    13,
       0,     0,     0,     0,     0,     0,    14,     0,     0,    15,
      16,    17,    18,    19,    20,     0,     0,    22,     0,    23,
       0,     0,     0,    24,    25,     0,    26,    27,    28,     0,
       0,     0,     0,     0,     0,     0,     0,    29,    30,    31,
       0,     0,     0,     0,     0,     0,    32,    33,    34,    35,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    36,     0,     0,     0,     0,     0,     0,     0,
      37,     0,     0,     0,     0,     0,     0,     0,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     0,     1,     2,
       3,     4,     5,    44,    45,    46,     6,     7,     0,     8,
       0,     9,   208,     0,    10,    11,    12,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,    14,     0,
       0,    15,    16,    17,    18,    19,    20,     0,     0,    22,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,    29,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    36,     0,     0,     0,     0,     0,
       0,     0,    37,     0,     0,     0,     0,     0,     0,     0,
      41,     0,     0,    42,     0,     0,    43,     0,     0,     0,
       1,     2,     3,     4,     5,    44,    45,    46,     6,     7,
       0,     8,     0,     9,     0,     0,    10,    11,    12,     0,
       0,     0,     0,    13,     0,     0,     0,     0,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,     0,
       0,    22,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,    29,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,    41,     0,   116,    42,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,    44,    45,    46,
     370,   371,   372,   373,   374,   375,   376,   441,   378,   379,
     380,   381,   382,   383,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,   384,   385,   386,
     387,   388,   389,   390,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   391,     0,     0,     0,     0,   392,   393,
     394,   395,   396,   397,   398,   399,   400,   401,   402,   403,
     404,   405,   406,     0,   407,     0,   408,   409,   410,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     411,   412,     0,     0,   413,   414,   415,   416,     1,     2,
       3,     4,     5,     0,     0,     0,     0,     7,    93,     8,
       0,     0,     0,     0,    10,    11,    12,     0,     0,     0,
       0,    13,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     1,     2,     3,     4,     5,     0,     0,     0,
     107,     7,     0,     8,    36,     0,     0,     0,    10,    11,
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
     279,     8,    36,     0,     0,     0,    10,    11,    12,     0,
      37,     0,     0,    13,     0,     0,     0,     0,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     0,     0,     0,     0,     0,     0,
       1,     2,     3,     4,     5,     0,    36,     0,     0,     7,
       0,     8,   282,     0,    37,     0,    10,    11,    12,     0,
       0,     0,    41,    13,     0,    42,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    30,    31,     0,     0,     0,     0,     0,     0,
      32,    33,    34,    35,     1,     2,     3,     4,     5,     0,
       0,     0,     0,     7,   294,     8,    36,     0,     0,     0,
      10,    11,    12,     0,    37,     0,     0,    13,     0,     0,
       0,     0,    41,     0,     0,    42,     0,     0,    43,     0,
       0,     0,     0,     0,     0,     0,     0,    23,     0,     0,
       0,    24,    25,     0,    26,    27,    28,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    30,    31,     0,     0,
       0,     0,     0,     0,    32,    33,    34,    35,     1,     2,
       3,     4,     5,     0,     0,     0,     0,     7,   296,     8,
      36,     0,     0,     0,    10,    11,    12,     0,    37,     0,
       0,    13,     0,     0,     0,     0,    41,     0,     0,    42,
       0,     0,    43,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     0,     0,     0,     0,     0,     0,     1,     2,
       3,     4,     5,     0,    36,     0,     0,     7,     0,     8,
     298,     0,    37,     0,    10,    11,    12,     0,     0,     0,
      41,    13,     0,    42,     0,     0,    43,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,    23,     0,     0,     0,    24,    25,     0,    26,    27,
      28,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      30,    31,     0,     0,     0,     0,     0,     0,    32,    33,
      34,    35,     1,     2,     3,     4,     5,     0,     0,     0,
       0,     7,   333,     8,    36,     0,     0,     0,    10,    11,
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
     109,     2,     3,     4,     5,     0,     0,     0,     0,     7,
       0,     8,    36,     0,     0,     0,    10,    11,    12,     0,
      37,     0,     0,     0,     0,     0,     0,     0,    41,     0,
       0,    42,     0,     0,    43,     0,     0,     0,     0,     0,
       0,     0,     0,    23,     0,     0,     0,    24,    25,     0,
      26,    27,    28,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
      32,    33,    34,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,    36,     0,     0,     0,
       0,     0,     0,     0,    37,     0,     0,     0,     0,     0,
       0,     0,   369,     0,     0,    42,     0,     0,    43,   370,
     371,   372,   373,   374,   375,   376,   377,   378,   379,   380,
     381,   382,   383,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   384,   385,   386,   387,
     388,   389,   390,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   391,     0,     0,     0,     0,   392,   393,   394,
     395,   396,   397,   398,   399,   400,   401,   402,   403,   404,
     405,   406,     0,   407,     0,   408,   409,   410,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   411,
     412,   464,     0,   413,   414,   415,   416,     0,   370,   371,
     372,   373,   374,   375,   376,   377,   378,   379,   380,   381,
     382,   383,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   384,   385,   386,   387,   388,
     389,   390,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,   391,     0,     0,     0,     0,   392,   393,   394,   395,
     396,   397,   398,   399,   400,   401,   402,   403,   404,   405,
     406,     0,   407,     0,   408,   409,   410,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   411,   412,
       0,     0,   413,   414,   415,   416,   370,   371,   372,   373,
     374,   375,   376,   377,   378,   379,   380,   381,   382,   383,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   384,   385,   386,   387,   388,   389,   390,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,   391,
       0,     0,     0,     0,   392,   393,   394,   395,   396,   397,
     398,   399,   400,   401,   402,   403,   404,   405,   406,     0,
     407,     0,   408,   409,   410,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   411,   412,     0,     0,
     413,   414,   415,   416
};

static const yytype_int16 yycheck[] =
{
      30,    31,    32,    17,   144,    55,    90,   250,   209,   131,
     139,    41,   135,     7,     8,     3,   102,   318,    16,     9,
     321,    16,   369,     3,   152,     3,    12,     0,    22,    36,
       0,     3,    26,    27,    28,   119,     9,     0,   122,    31,
      98,    99,    56,     3,     4,     5,     6,     7,    26,    43,
      24,    25,    12,    45,    14,   315,     3,    64,     3,    33,
      34,     4,   363,     3,   365,   366,    13,   117,   118,     3,
      98,    99,    15,    13,    47,   125,   126,    47,   206,    26,
     323,    26,    11,    97,    47,    71,    26,    16,     4,    12,
       9,    71,    26,    53,    54,    55,    15,   357,    76,    11,
     360,    17,   100,   101,    16,   100,   100,   101,   102,     9,
     104,   312,   459,    13,    88,    12,     9,   464,     3,   149,
      13,   115,   112,   207,     3,   368,   369,   213,    13,   117,
      12,     9,    14,     4,    13,    13,    18,   438,   439,   105,
     106,    26,     9,    14,     4,    16,    13,    26,     4,    98,
      99,   111,   118,   119,    14,     9,    16,   458,    14,    13,
      16,     9,    24,    25,   465,    13,   467,   197,    50,    51,
     310,   472,   473,   313,     9,     9,   316,    11,    13,   308,
     302,   231,   305,     9,   178,     9,     9,    13,     9,    13,
      13,   434,    13,     9,   278,     9,     9,    13,   282,    13,
      13,   444,     9,     9,    11,     3,   200,    13,    12,    12,
     294,    14,   296,    12,    26,    27,    28,    29,    11,   249,
     250,   109,   110,   466,    11,   113,   114,   115,   116,   430,
      12,   225,    14,   227,   228,    77,    78,    79,    80,    81,
      82,    83,    84,    85,    86,    21,    22,    23,    90,    87,
      88,    93,    12,    95,    96,    97,    89,   341,    30,   343,
     185,   186,   187,   188,     9,    10,    11,   193,   194,   195,
      19,    20,   286,   287,     9,    10,   183,   184,   292,   293,
     189,   190,    47,   191,   192,    49,    48,    89,    18,   429,
      91,    52,    28,   323,   288,   289,   290,   291,    71,     9,
       4,     4,     3,    12,    12,     3,    12,    14,    71,    14,
      12,    35,     9,     9,    14,     4,   101,     9,    17,   333,
      15,     9,    15,    17,    14,   100,    17,    47,    15,    12,
      16,    16,    16,    13,    13,    17,    17,    15,   368,   369,
     354,     9,    13,    12,    11,     0,    11,    17,     3,     4,
       5,     6,     7,    17,    13,    13,    11,    12,    12,    14,
      13,    16,    13,    13,    19,    20,    21,    13,   352,    44,
      46,    26,    45,   169,   328,   252,   176,   319,    33,   196,
     177,    36,    37,    38,    39,    40,    41,   179,    43,    44,
     180,    46,   181,    -1,   182,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,   434,   419,    -1,   421,   422,    64,
      65,    66,    -1,   199,   444,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,   466,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    -1,    -1,    -1,   102,   103,   104,
     105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,    -1,
       3,     4,     5,     6,     7,   120,   121,   122,    11,    12,
      -1,    14,    -1,    16,    17,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    36,    37,    38,    39,    40,    41,    -1,
      43,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,   102,
     103,   104,   105,    -1,    -1,   108,    -1,    -1,   111,    -1,
      -1,    -1,     3,     4,     5,     6,     7,   120,   121,   122,
      11,    12,    -1,    14,    -1,    16,    17,    -1,    19,    20,
      21,    -1,    -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,
      -1,    -1,    33,    -1,    -1,    36,    37,    38,    39,    40,
      41,    -1,    43,    44,    -1,    46,    -1,    -1,    -1,    50,
      51,    -1,    53,    54,    55,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    64,    65,    66,    -1,    -1,    -1,    -1,
      -1,    -1,    73,    74,    75,    76,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,
      -1,   102,   103,   104,   105,    -1,    -1,   108,    -1,    -1,
     111,    -1,    -1,    -1,     3,     4,     5,     6,     7,   120,
     121,   122,    11,    12,    -1,    14,    -1,    16,    -1,    -1,
      19,    20,    21,    -1,    -1,    -1,    -1,    26,    -1,    -1,
      -1,    -1,    -1,    -1,    33,    -1,    -1,    36,    37,    38,
      39,    40,    41,    -1,    43,    44,    -1,    46,    -1,    -1,
      -1,    50,    51,    -1,    53,    54,    55,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    64,    65,    66,    -1,    -1,
      -1,    -1,    -1,    -1,    73,    74,    75,    76,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    97,    -1,
      -1,    -1,    -1,   102,   103,   104,   105,    -1,    -1,   108,
      -1,    -1,   111,    -1,    -1,    -1,     3,     4,     5,     6,
       7,   120,   121,   122,    11,    12,    -1,    14,    -1,    16,
      17,    -1,    19,    20,    21,    -1,    -1,    -1,    -1,    26,
      -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,    -1,    36,
      37,    38,    39,    40,    41,    -1,    -1,    44,    -1,    46,
      -1,    -1,    -1,    50,    51,    -1,    53,    54,    55,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,    65,    66,
      -1,    -1,    -1,    -1,    -1,    -1,    73,    74,    75,    76,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   105,    -1,
      -1,   108,    -1,    -1,   111,    -1,    -1,    -1,     3,     4,
       5,     6,     7,   120,   121,   122,    11,    12,    -1,    14,
      -1,    16,    17,    -1,    19,    20,    21,    -1,    -1,    -1,
      -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,    33,    -1,
      -1,    36,    37,    38,    39,    40,    41,    -1,    -1,    44,
      -1,    46,    -1,    -1,    -1,    50,    51,    -1,    53,    54,
      55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    64,
      65,    66,    -1,    -1,    -1,    -1,    -1,    -1,    73,    74,
      75,    76,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
     105,    -1,    -1,   108,    -1,    -1,   111,    -1,    -1,    -1,
       3,     4,     5,     6,     7,   120,   121,   122,    11,    12,
      -1,    14,    -1,    16,    -1,    -1,    19,    20,    21,    -1,
      -1,    -1,    -1,    26,    -1,    -1,    -1,    -1,    -1,    -1,
      33,    -1,    -1,    36,    37,    38,    39,    40,    41,    -1,
      -1,    44,    -1,    46,    -1,    -1,    -1,    50,    51,    -1,
      53,    54,    55,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    64,    65,    66,    -1,    -1,    -1,    -1,    -1,    -1,
      73,    74,    75,    76,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    89,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    97,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   105,    -1,     3,   108,    -1,    -1,   111,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,   120,   121,   122,
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
     104,   105,   108,   111,   120,   121,   122,   124,   125,   126,
     134,   135,   136,   138,   139,   140,   143,   144,   145,   146,
     147,   148,   149,   152,   155,   158,   159,   160,   161,   162,
     164,   167,   168,   169,   172,   173,   174,   175,   176,   177,
     178,   179,   180,   181,   182,   183,   184,   185,   186,   188,
     189,   190,    71,    13,   159,   159,    17,   143,     3,   133,
      12,    12,    12,   144,    12,    11,    11,    11,   159,     3,
     185,   185,   159,   159,   159,    12,     3,   140,   140,   140,
     185,   185,     3,   165,   166,    76,   140,    89,   159,     4,
      14,    16,   151,     4,    14,    16,   154,     4,    14,    16,
     157,     0,   125,    98,    99,   105,   106,   118,   119,   137,
     165,   144,     9,    11,    77,    78,    79,    80,    81,    82,
      83,    84,    85,    86,    90,    93,    95,    96,    97,   170,
     109,   110,   113,   114,   115,   116,   171,    31,    45,    30,
      47,    49,    48,    24,    25,    26,    27,    28,    29,    87,
      88,    19,    20,    21,    22,    23,    89,    52,   185,    91,
      12,    18,    50,    51,   162,    13,     9,    15,    17,    18,
      28,   159,   159,   158,   159,    36,    64,   159,    11,    13,
      13,    13,   159,   165,   165,    12,   162,    12,    14,   162,
     163,     9,   165,   165,   112,    15,   150,   151,    17,   150,
     153,   154,   153,   156,   157,   156,     3,   132,   132,    12,
      12,     3,   140,   141,   160,   161,   164,   174,   159,   175,
     176,   177,   178,   179,   179,   180,   180,   180,   180,   181,
     181,   182,   182,   183,   183,   183,   184,   140,   189,    13,
     159,     3,    15,   162,   133,    71,    13,    13,   158,    10,
      12,    12,    13,    13,    13,   159,    13,   159,    15,    14,
     165,    14,     9,    17,    15,     9,    17,    15,     9,    17,
       9,    16,   100,   101,   127,    16,   100,   131,    13,   140,
     142,    13,   142,    12,    14,   163,     3,   117,    10,    13,
     162,   144,   144,    13,   159,   159,   159,   159,   144,   144,
     162,    13,   162,    13,    15,   150,   153,   156,   132,   126,
     128,   129,   130,   134,   143,   133,   132,    16,   128,   132,
      16,   148,   166,    13,   148,    13,    13,   142,    12,    12,
      19,    20,    21,    22,    23,    24,    25,    26,    27,    28,
      29,    30,    31,    32,    46,    47,    48,    49,    50,    51,
      52,    72,    77,    78,    79,    80,    81,    82,    83,    84,
      85,    86,    87,    88,    89,    90,    91,    93,    95,    96,
      97,   109,   110,   113,   114,   115,   116,   187,   172,    35,
     144,    13,    13,    13,    13,   162,   162,    17,   129,   101,
     100,   128,    17,   128,     9,   148,   148,   148,    13,    13,
     142,    26,   142,   187,    12,   144,   144,   144,    11,    11,
     132,   133,    17,    17,   142,   148,    11,   148,    13,    13,
      13,   142,    11,   148,    12,   187,    12,    13,   187,   148,
     142,   148,    13,    13,   148,   148
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
#line 221 "chuck.y"
    { (yyval.program) = g_program = new_program( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 3:
#line 222 "chuck.y"
    { (yyval.program) = g_program = append_program( (yyvsp[(1) - (2)].program), (yyvsp[(2) - (2)].program_section), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 4:
#line 226 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 5:
#line 227 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 6:
#line 228 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 7:
#line 233 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 8:
#line 235 "chuck.y"
    { (yyval.class_def) = new_class_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 9:
#line 237 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (6)].ival), (yyvsp[(3) - (6)].id_list), NULL, (yyvsp[(5) - (6)].class_body), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 10:
#line 239 "chuck.y"
    { (yyval.class_def) = new_iface_def( (yyvsp[(1) - (7)].ival), (yyvsp[(3) - (7)].id_list), (yyvsp[(4) - (7)].class_ext), (yyvsp[(6) - (7)].class_body), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 11:
#line 243 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 12:
#line 244 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(4) - (4)].id_list), (yyvsp[(2) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 13:
#line 245 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (2)].id_list), NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 14:
#line 246 "chuck.y"
    { (yyval.class_ext) = new_class_ext( (yyvsp[(2) - (4)].id_list), (yyvsp[(4) - (4)].id_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 15:
#line 250 "chuck.y"
    { (yyval.class_body) = (yyvsp[(1) - (1)].class_body); ;}
    break;

  case 16:
#line 251 "chuck.y"
    { (yyval.class_body) = NULL; ;}
    break;

  case 17:
#line 255 "chuck.y"
    { (yyval.class_body) = new_class_body( (yyvsp[(1) - (1)].program_section), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 18:
#line 256 "chuck.y"
    { (yyval.class_body) = prepend_class_body( (yyvsp[(1) - (2)].program_section), (yyvsp[(2) - (2)].class_body), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 19:
#line 261 "chuck.y"
    { (yyval.program_section) = new_section_stmt( (yyvsp[(1) - (1)].stmt_list), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 20:
#line 262 "chuck.y"
    { (yyval.program_section) = new_section_func_def( (yyvsp[(1) - (1)].func_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 21:
#line 263 "chuck.y"
    { (yyval.program_section) = new_section_class_def( (yyvsp[(1) - (1)].class_def), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 22:
#line 267 "chuck.y"
    { (yyval.class_ext) = new_class_ext( NULL, (yyvsp[(2) - (2)].id_list), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 23:
#line 271 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1 */ ); ;}
    break;

  case 24:
#line 272 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1 */ ); ;}
    break;

  case 25:
#line 276 "chuck.y"
    { (yyval.id_list) = new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column  /*, &@1*/ ); ;}
    break;

  case 26:
#line 277 "chuck.y"
    { (yyval.id_list) = prepend_id_list( (yyvsp[(1) - (3)].sval), (yyvsp[(3) - (3)].id_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column  /*, &@1*/ ); ;}
    break;

  case 27:
#line 282 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), (yyvsp[(8) - (8)].stmt), TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 28:
#line 284 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 29:
#line 286 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), NULL, (yyvsp[(3) - (7)].sval), (yyvsp[(5) - (7)].arg_list), (yyvsp[(7) - (7)].stmt), TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 30:
#line 288 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), (yyvsp[(2) - (6)].ival), NULL, (yyvsp[(3) - (6)].sval), NULL, (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 31:
#line 290 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@construct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 32:
#line 292 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@construct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 33:
#line 294 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (5)].ival), ae_key_instance, NULL, "@destruct", NULL, (yyvsp[(5) - (5)].stmt), TRUE, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 34:
#line 296 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (6)].ival), ae_key_instance, NULL, "@destruct", (yyvsp[(4) - (6)].arg_list), (yyvsp[(6) - (6)].stmt), TRUE, (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 35:
#line 298 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (8)].ival), (yyvsp[(2) - (8)].ival), (yyvsp[(3) - (8)].type_decl), (yyvsp[(4) - (8)].sval), (yyvsp[(6) - (8)].arg_list), NULL, TRUE, (yylsp[(1) - (8)]).first_line, (yylsp[(1) - (8)]).first_column ); ;}
    break;

  case 36:
#line 300 "chuck.y"
    { (yyval.func_def) = new_func_def( (yyvsp[(1) - (7)].ival), (yyvsp[(2) - (7)].ival), (yyvsp[(3) - (7)].type_decl), (yyvsp[(4) - (7)].sval), NULL, NULL, TRUE, (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 37:
#line 302 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(5) - (9)].ival), (yyvsp[(7) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, FALSE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(5) - (9)]).first_column ); ;}
    break;

  case 38:
#line 304 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (9)].ival), (yyvsp[(2) - (9)].ival), (yyvsp[(3) - (9)].type_decl), (yyvsp[(8) - (9)].ival), (yyvsp[(6) - (9)].arg_list), (yyvsp[(9) - (9)].stmt), TRUE, TRUE, (yylsp[(1) - (9)]).first_line, (yylsp[(1) - (9)]).first_column, (yylsp[(8) - (9)]).first_column ); ;}
    break;

  case 39:
#line 306 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(6) - (11)].ival), (yyvsp[(9) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, FALSE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(5) - (11)]).first_column ); ;}
    break;

  case 40:
#line 308 "chuck.y"
    { (yyval.func_def) = new_op_overload( (yyvsp[(1) - (11)].ival), (yyvsp[(2) - (11)].ival), (yyvsp[(3) - (11)].type_decl), (yyvsp[(9) - (11)].ival), (yyvsp[(6) - (11)].arg_list), (yyvsp[(11) - (11)].stmt), TRUE, TRUE, (yylsp[(1) - (11)]).first_line, (yylsp[(1) - (11)]).first_column, (yylsp[(8) - (11)]).first_column ); ;}
    break;

  case 41:
#line 312 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 42:
#line 313 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 43:
#line 314 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 44:
#line 318 "chuck.y"
    { (yyval.ival) = ae_key_func; ;}
    break;

  case 45:
#line 319 "chuck.y"
    { (yyval.ival) = ae_key_public; ;}
    break;

  case 46:
#line 320 "chuck.y"
    { (yyval.ival) = ae_key_protected; ;}
    break;

  case 47:
#line 321 "chuck.y"
    { (yyval.ival) = ae_key_private; ;}
    break;

  case 48:
#line 325 "chuck.y"
    { (yyval.ival) = ae_key_static; ;}
    break;

  case 49:
#line 326 "chuck.y"
    { (yyval.ival) = ae_key_abstract; ;}
    break;

  case 50:
#line 327 "chuck.y"
    { (yyval.ival) = ae_key_instance; ;}
    break;

  case 51:
#line 331 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column /*, &@1*/ ), 0, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 52:
#line 332 "chuck.y"
    { (yyval.type_decl) = new_type_decl( new_id_list( (yyvsp[(1) - (2)].sval), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column /*, &@1*/ ), 1, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 53:
#line 336 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (3)].id_list), 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 54:
#line 337 "chuck.y"
    { (yyval.type_decl) = new_type_decl( (yyvsp[(2) - (4)].id_list), 1, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 55:
#line 346 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 56:
#line 347 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 57:
#line 352 "chuck.y"
    { (yyval.type_decl) = (yyvsp[(1) - (1)].type_decl); ;}
    break;

  case 58:
#line 353 "chuck.y"
    { (yyval.type_decl) = add_type_decl_array( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 59:
#line 357 "chuck.y"
    { (yyval.arg_list) = new_arg_list( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 60:
#line 358 "chuck.y"
    { (yyval.arg_list) = prepend_arg_list( (yyvsp[(1) - (4)].type_decl), (yyvsp[(2) - (4)].var_decl), (yyvsp[(4) - (4)].arg_list), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 61:
#line 362 "chuck.y"
    { (yyval.stmt_list) = new_stmt_list( (yyvsp[(1) - (1)].stmt), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 62:
#line 363 "chuck.y"
    { (yyval.stmt_list) = append_stmt_list( (yyvsp[(1) - (2)].stmt_list), (yyvsp[(2) - (2)].stmt), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 63:
#line 367 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 64:
#line 368 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 65:
#line 369 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 66:
#line 370 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 67:
#line 372 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 68:
#line 373 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 69:
#line 374 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 70:
#line 375 "chuck.y"
    { (yyval.stmt) = (yyvsp[(1) - (1)].stmt); ;}
    break;

  case 71:
#line 379 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 72:
#line 380 "chuck.y"
    { (yyval.stmt) = new_stmt_from_return( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 73:
#line 381 "chuck.y"
    { (yyval.stmt) = new_stmt_from_break( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 74:
#line 382 "chuck.y"
    { (yyval.stmt) = new_stmt_from_continue( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 75:
#line 387 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 76:
#line 389 "chuck.y"
    { (yyval.stmt) = new_stmt_from_if( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].stmt), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 77:
#line 394 "chuck.y"
    { (yyval.stmt) = new_stmt_from_while( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 78:
#line 396 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_while( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 79:
#line 398 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (6)].stmt), (yyvsp[(4) - (6)].stmt), NULL, (yyvsp[(6) - (6)].stmt), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 80:
#line 400 "chuck.y"
    { (yyval.stmt) = new_stmt_from_for( (yyvsp[(3) - (7)].stmt), (yyvsp[(4) - (7)].stmt), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 81:
#line 402 "chuck.y"
    { (yyval.stmt) = new_stmt_from_foreach( (yyvsp[(3) - (7)].exp), (yyvsp[(5) - (7)].exp), (yyvsp[(7) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 82:
#line 404 "chuck.y"
    { (yyval.stmt) = new_stmt_from_until( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 83:
#line 406 "chuck.y"
    { (yyval.stmt) = new_stmt_from_do_until( (yyvsp[(5) - (7)].exp), (yyvsp[(2) - (7)].stmt), (yylsp[(1) - (7)]).first_line, (yylsp[(1) - (7)]).first_column ); ;}
    break;

  case 84:
#line 408 "chuck.y"
    { (yyval.stmt) = new_stmt_from_loop( (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].stmt), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 85:
#line 412 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 86:
#line 413 "chuck.y"
    { (yyval.stmt) = new_stmt_from_code( (yyvsp[(2) - (3)].stmt_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 87:
#line 417 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(2) - (2)].import), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 88:
#line 418 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 89:
#line 419 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 90:
#line 420 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(3) - (4)].import), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 91:
#line 421 "chuck.y"
    { (yyval.stmt) = new_stmt_from_import( (yyvsp[(3) - (4)].import), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 92:
#line 425 "chuck.y"
    { (yyval.import) = (yyvsp[(1) - (1)].import); ;}
    break;

  case 93:
#line 426 "chuck.y"
    { (yyval.import) = prepend_import( (yyvsp[(1) - (3)].import), (yyvsp[(3) - (3)].import), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 94:
#line 430 "chuck.y"
    { (yyval.import) = new_import( (yyvsp[(1) - (1)].sval), NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 95:
#line 435 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(2) - (2)].doc), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 96:
#line 436 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(3) - (4)].doc), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 97:
#line 437 "chuck.y"
    { (yyval.stmt) = new_stmt_from_doc( (yyvsp[(3) - (4)].doc), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 98:
#line 441 "chuck.y"
    { (yyval.doc) = (yyvsp[(1) - (1)].doc); ;}
    break;

  case 99:
#line 442 "chuck.y"
    { (yyval.doc) = prepend_doc( (yyvsp[(1) - (3)].doc), (yyvsp[(3) - (3)].doc), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 100:
#line 446 "chuck.y"
    { (yyval.doc) = new_doc( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 101:
#line 450 "chuck.y"
    { (yyval.stmt) = new_stmt_from_example( (yyvsp[(2) - (2)].example), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 102:
#line 451 "chuck.y"
    { (yyval.stmt) = new_stmt_from_example( (yyvsp[(3) - (4)].example), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 103:
#line 452 "chuck.y"
    { (yyval.stmt) = new_stmt_from_example( (yyvsp[(3) - (4)].example), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 104:
#line 456 "chuck.y"
    { (yyval.example) = (yyvsp[(1) - (1)].example); ;}
    break;

  case 105:
#line 457 "chuck.y"
    { (yyval.example) = prepend_example( (yyvsp[(1) - (3)].example), (yyvsp[(3) - (3)].example), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 106:
#line 461 "chuck.y"
    { (yyval.example) = new_example( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 107:
#line 465 "chuck.y"
    { (yyval.stmt) = NULL; ;}
    break;

  case 108:
#line 466 "chuck.y"
    { (yyval.stmt) = new_stmt_from_expression( (yyvsp[(1) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 109:
#line 470 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 110:
#line 471 "chuck.y"
    { (yyval.exp) = append_expression( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 111:
#line 475 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 112:
#line 477 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 113:
#line 481 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 114:
#line 483 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), (yyvsp[(2) - (3)].ival), (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 115:
#line 487 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 116:
#line 488 "chuck.y"
    { (yyval.array_sub) = new_array_sub( (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 117:
#line 490 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(4) - (4)].array_sub), (yyvsp[(2) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 118:
#line 492 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(5) - (5)].array_sub), (yyvsp[(2) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 119:
#line 496 "chuck.y"
    { (yyval.array_sub) = new_array_sub( NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 120:
#line 497 "chuck.y"
    { (yyval.array_sub) = prepend_array_sub( (yyvsp[(1) - (3)].array_sub), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 121:
#line 501 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 122:
#line 502 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(1) - (2)].type_decl), (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 123:
#line 503 "chuck.y"
    { (yyval.exp) = new_exp_decl_external( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 124:
#line 504 "chuck.y"
    { (yyval.exp) = new_exp_decl_global( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 0, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 125:
#line 505 "chuck.y"
    { (yyval.exp) = new_exp_decl( (yyvsp[(2) - (3)].type_decl), (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 126:
#line 506 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(2) - (2)].var_decl_list), 0, 0, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 127:
#line 507 "chuck.y"
    { (yyval.exp) = new_exp_decl( NULL, (yyvsp[(3) - (3)].var_decl_list), 1, 0, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 128:
#line 511 "chuck.y"
    { (yyval.var_decl_list) = new_var_decl_list( (yyvsp[(1) - (1)].var_decl), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 129:
#line 512 "chuck.y"
    { (yyval.var_decl_list) = prepend_var_decl_list( (yyvsp[(1) - (3)].var_decl), (yyvsp[(3) - (3)].var_decl_list), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 130:
#line 516 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (1)].sval), FALSE, NULL, NULL, (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 131:
#line 517 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 132:
#line 518 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (2)].sval), FALSE, NULL, (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 133:
#line 519 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (3)].sval), TRUE, NULL, NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 134:
#line 520 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, (yyvsp[(3) - (4)].exp), NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 135:
#line 521 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (4)].sval), TRUE, NULL, (yyvsp[(4) - (4)].array_sub), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 136:
#line 522 "chuck.y"
    { (yyval.var_decl) = new_var_decl( (yyvsp[(1) - (5)].sval), TRUE, (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 137:
#line 527 "chuck.y"
    { (yyval.complex_exp) = new_complex( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 138:
#line 532 "chuck.y"
    { (yyval.polar_exp) = new_polar( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 139:
#line 537 "chuck.y"
    { (yyval.vec_exp) = new_vec( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 140:
#line 541 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 141:
#line 542 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 142:
#line 543 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 143:
#line 544 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 144:
#line 545 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 145:
#line 546 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 146:
#line 547 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 147:
#line 548 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 148:
#line 549 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 149:
#line 550 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 150:
#line 551 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 151:
#line 552 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 152:
#line 553 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 153:
#line 554 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 154:
#line 555 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 155:
#line 559 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 156:
#line 560 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 157:
#line 561 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 158:
#line 562 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 159:
#line 563 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 160:
#line 564 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 161:
#line 568 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 162:
#line 570 "chuck.y"
    { (yyval.exp) = new_exp_from_if( (yyvsp[(1) - (5)].exp), (yyvsp[(3) - (5)].exp), (yyvsp[(5) - (5)].exp), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 163:
#line 574 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 164:
#line 576 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 165:
#line 580 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 166:
#line 582 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 167:
#line 586 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 168:
#line 588 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_or, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 169:
#line 592 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 170:
#line 594 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_xor, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 171:
#line 598 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 172:
#line 600 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_s_and, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 173:
#line 604 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 174:
#line 606 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_eq, (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 175:
#line 608 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_neq, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 176:
#line 612 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 177:
#line 614 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_lt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 178:
#line 616 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_gt, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 179:
#line 618 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_le, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 180:
#line 620 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_ge, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 181:
#line 624 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 182:
#line 626 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_left, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 183:
#line 628 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_shift_right, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 184:
#line 632 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 185:
#line 634 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_plus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 186:
#line 636 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_minus, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 187:
#line 640 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 188:
#line 642 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_times, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 189:
#line 644 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_divide, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 190:
#line 646 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_percent, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 191:
#line 650 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 192:
#line 652 "chuck.y"
    { (yyval.exp) = new_exp_from_binary( (yyvsp[(1) - (3)].exp), ae_op_tilda, (yyvsp[(3) - (3)].exp), (yylsp[(2) - (3)]).first_line, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 193:
#line 656 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 194:
#line 658 "chuck.y"
    { (yyval.exp) = new_exp_from_cast( (yyvsp[(3) - (3)].type_decl), (yyvsp[(1) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(2) - (3)]).first_column ); ;}
    break;

  case 195:
#line 662 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 196:
#line 664 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_plusplus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 197:
#line 666 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_minusminus, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 198:
#line 668 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( (yyvsp[(1) - (2)].ival), (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 199:
#line 670 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_typeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 200:
#line 672 "chuck.y"
    { (yyval.exp) = new_exp_from_unary( ae_op_sizeof, (yyvsp[(2) - (2)].exp), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 201:
#line 674 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (2)].type_decl), FALSE, NULL, NULL, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 202:
#line 676 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (3)].type_decl), FALSE, NULL, (yyvsp[(3) - (3)].array_sub), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 203:
#line 678 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (4)].type_decl), TRUE, NULL, NULL, (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 204:
#line 680 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, (yyvsp[(4) - (5)].exp), NULL, (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 205:
#line 682 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (5)].type_decl), TRUE, NULL, (yyvsp[(5) - (5)].array_sub), (yylsp[(1) - (5)]).first_line, (yylsp[(1) - (5)]).first_column ); ;}
    break;

  case 206:
#line 684 "chuck.y"
    { (yyval.exp) = new_exp_from_unary2( ae_op_new, (yyvsp[(2) - (6)].type_decl), TRUE, (yyvsp[(4) - (6)].exp), (yyvsp[(6) - (6)].array_sub), (yylsp[(1) - (6)]).first_line, (yylsp[(1) - (6)]).first_column ); ;}
    break;

  case 207:
#line 690 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 208:
#line 691 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 209:
#line 692 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 210:
#line 693 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 211:
#line 694 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 212:
#line 695 "chuck.y"
    { (yyval.ival) = ae_op_spork; ;}
    break;

  case 213:
#line 696 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 214:
#line 702 "chuck.y"
    { (yyval.ival) = ae_op_chuck; ;}
    break;

  case 215:
#line 703 "chuck.y"
    { (yyval.ival) = ae_op_plus; ;}
    break;

  case 216:
#line 704 "chuck.y"
    { (yyval.ival) = ae_op_minus; ;}
    break;

  case 217:
#line 705 "chuck.y"
    { (yyval.ival) = ae_op_times; ;}
    break;

  case 218:
#line 706 "chuck.y"
    { (yyval.ival) = ae_op_divide; ;}
    break;

  case 219:
#line 707 "chuck.y"
    { (yyval.ival) = ae_op_percent; ;}
    break;

  case 220:
#line 708 "chuck.y"
    { (yyval.ival) = ae_op_eq; ;}
    break;

  case 221:
#line 709 "chuck.y"
    { (yyval.ival) = ae_op_neq; ;}
    break;

  case 222:
#line 710 "chuck.y"
    { (yyval.ival) = ae_op_lt; ;}
    break;

  case 223:
#line 711 "chuck.y"
    { (yyval.ival) = ae_op_le; ;}
    break;

  case 224:
#line 712 "chuck.y"
    { (yyval.ival) = ae_op_gt; ;}
    break;

  case 225:
#line 713 "chuck.y"
    { (yyval.ival) = ae_op_ge; ;}
    break;

  case 226:
#line 714 "chuck.y"
    { (yyval.ival) = ae_op_and; ;}
    break;

  case 227:
#line 715 "chuck.y"
    { (yyval.ival) = ae_op_or; ;}
    break;

  case 228:
#line 716 "chuck.y"
    { (yyval.ival) = ae_op_assign; ;}
    break;

  case 229:
#line 717 "chuck.y"
    { (yyval.ival) = ae_op_exclamation; ;}
    break;

  case 230:
#line 718 "chuck.y"
    { (yyval.ival) = ae_op_s_or; ;}
    break;

  case 231:
#line 719 "chuck.y"
    { (yyval.ival) = ae_op_s_and; ;}
    break;

  case 232:
#line 720 "chuck.y"
    { (yyval.ival) = ae_op_s_xor; ;}
    break;

  case 233:
#line 721 "chuck.y"
    { (yyval.ival) = ae_op_plusplus; ;}
    break;

  case 234:
#line 722 "chuck.y"
    { (yyval.ival) = ae_op_minusminus; ;}
    break;

  case 235:
#line 723 "chuck.y"
    { (yyval.ival) = ae_op_dollar; ;}
    break;

  case 236:
#line 724 "chuck.y"
    { (yyval.ival) = ae_op_at_at; ;}
    break;

  case 237:
#line 725 "chuck.y"
    { (yyval.ival) = ae_op_plus_chuck; ;}
    break;

  case 238:
#line 726 "chuck.y"
    { (yyval.ival) = ae_op_minus_chuck; ;}
    break;

  case 239:
#line 727 "chuck.y"
    { (yyval.ival) = ae_op_times_chuck; ;}
    break;

  case 240:
#line 728 "chuck.y"
    { (yyval.ival) = ae_op_divide_chuck; ;}
    break;

  case 241:
#line 729 "chuck.y"
    { (yyval.ival) = ae_op_s_and_chuck; ;}
    break;

  case 242:
#line 730 "chuck.y"
    { (yyval.ival) = ae_op_s_or_chuck; ;}
    break;

  case 243:
#line 731 "chuck.y"
    { (yyval.ival) = ae_op_s_xor_chuck; ;}
    break;

  case 244:
#line 732 "chuck.y"
    { (yyval.ival) = ae_op_shift_right_chuck; ;}
    break;

  case 245:
#line 733 "chuck.y"
    { (yyval.ival) = ae_op_shift_left_chuck; ;}
    break;

  case 246:
#line 734 "chuck.y"
    { (yyval.ival) = ae_op_percent_chuck; ;}
    break;

  case 247:
#line 735 "chuck.y"
    { (yyval.ival) = ae_op_shift_right; ;}
    break;

  case 248:
#line 736 "chuck.y"
    { (yyval.ival) = ae_op_shift_left; ;}
    break;

  case 249:
#line 737 "chuck.y"
    { (yyval.ival) = ae_op_tilda; ;}
    break;

  case 250:
#line 738 "chuck.y"
    { (yyval.ival) = ae_op_coloncolon; ;}
    break;

  case 251:
#line 739 "chuck.y"
    { (yyval.ival) = ae_op_at_chuck; ;}
    break;

  case 252:
#line 740 "chuck.y"
    { (yyval.ival) = ae_op_unchuck; ;}
    break;

  case 253:
#line 741 "chuck.y"
    { (yyval.ival) = ae_op_upchuck; ;}
    break;

  case 254:
#line 742 "chuck.y"
    { (yyval.ival) = ae_op_downchuck; ;}
    break;

  case 255:
#line 743 "chuck.y"
    { (yyval.ival) = ae_op_arrow_right; ;}
    break;

  case 256:
#line 744 "chuck.y"
    { (yyval.ival) = ae_op_arrow_left; ;}
    break;

  case 257:
#line 745 "chuck.y"
    { (yyval.ival) = ae_op_gruck_right; ;}
    break;

  case 258:
#line 746 "chuck.y"
    { (yyval.ival) = ae_op_gruck_left; ;}
    break;

  case 259:
#line 747 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_right; ;}
    break;

  case 260:
#line 748 "chuck.y"
    { (yyval.ival) = ae_op_ungruck_left; ;}
    break;

  case 262:
#line 754 "chuck.y"
    { (yyval.exp) = new_exp_from_dur( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 263:
#line 758 "chuck.y"
    { (yyval.exp) = (yyvsp[(1) - (1)].exp); ;}
    break;

  case 264:
#line 760 "chuck.y"
    { (yyval.exp) = new_exp_from_array( (yyvsp[(1) - (2)].exp), (yyvsp[(2) - (2)].array_sub), (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 265:
#line 762 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (3)].exp), NULL, (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 266:
#line 764 "chuck.y"
    { (yyval.exp) = new_exp_from_func_call( (yyvsp[(1) - (4)].exp), (yyvsp[(3) - (4)].exp), (yylsp[(1) - (4)]).first_line, (yylsp[(1) - (4)]).first_column ); ;}
    break;

  case 267:
#line 766 "chuck.y"
    { (yyval.exp) = new_exp_from_member_dot( (yyvsp[(1) - (3)].exp), (yyvsp[(3) - (3)].sval), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column, (yylsp[(3) - (3)]).first_column ); ;}
    break;

  case 268:
#line 768 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_plusplus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 269:
#line 770 "chuck.y"
    { (yyval.exp) = new_exp_from_postfix( (yyvsp[(1) - (2)].exp), ae_op_minusminus, (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;

  case 270:
#line 775 "chuck.y"
    { (yyval.exp) = new_exp_from_id( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 271:
#line 776 "chuck.y"
    { (yyval.exp) = new_exp_from_int( (yyvsp[(1) - (1)].ival), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 272:
#line 777 "chuck.y"
    { (yyval.exp) = new_exp_from_float( (yyvsp[(1) - (1)].fval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 273:
#line 778 "chuck.y"
    { (yyval.exp) = new_exp_from_str( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 274:
#line 779 "chuck.y"
    { (yyval.exp) = new_exp_from_char( (yyvsp[(1) - (1)].sval), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 275:
#line 780 "chuck.y"
    { (yyval.exp) = new_exp_from_array_lit( (yyvsp[(1) - (1)].array_sub), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 276:
#line 781 "chuck.y"
    { (yyval.exp) = new_exp_from_complex( (yyvsp[(1) - (1)].complex_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 277:
#line 782 "chuck.y"
    { (yyval.exp) = new_exp_from_polar( (yyvsp[(1) - (1)].polar_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 278:
#line 783 "chuck.y"
    { (yyval.exp) = new_exp_from_vec( (yyvsp[(1) - (1)].vec_exp), (yylsp[(1) - (1)]).first_line, (yylsp[(1) - (1)]).first_column ); ;}
    break;

  case 279:
#line 784 "chuck.y"
    { (yyval.exp) = new_exp_from_hack( (yyvsp[(2) - (3)].exp), (yylsp[(1) - (3)]).first_line, (yylsp[(1) - (3)]).first_column ); ;}
    break;

  case 280:
#line 785 "chuck.y"
    { (yyval.exp) = (yyvsp[(2) - (3)].exp); ;}
    break;

  case 281:
#line 786 "chuck.y"
    { (yyval.exp) = new_exp_from_nil( (yylsp[(1) - (2)]).first_line, (yylsp[(1) - (2)]).first_column ); ;}
    break;


/* Line 1267 of yacc.c.  */
#line 3945 "chuck.tab.c"
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



