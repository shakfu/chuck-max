/*----------------------------------------------------------------------------
  ChucK Strongly-timed Audio Programming Language
    Compiler, Virtual Machine, and Synthesis Engine

  Copyright (c) 2003 Ge Wang and Perry R. Cook. All rights reserved.
    http://chuck.stanford.edu/
    http://chuck.cs.princeton.edu/

  This program is free software; you can redistribute it and/or modify
  it under the dual-license terms of EITHER the MIT License OR the GNU
  General Public License (the latter as published by the Free Software
  Foundation; either version 2 of the License or, at your option, any
  later version).

  This program is distributed in the hope that it will be useful and/or
  interesting, but WITHOUT ANY WARRANTY; without even the implied warranty
  of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  MIT Licence and/or the GNU General Public License for details.

  You should have received a copy of the MIT License and the GNU General
  Public License (GPL) along with this program; a copy of the GPL can also
  be obtained by writing to the Free Software Foundation, Inc., 59 Temple
  Place, Suite 330, Boston, MA 02111-1307 U.S.A.
-----------------------------------------------------------------------------*/

//-----------------------------------------------------------------------------
// file: chuck_parse.h
// desc: chuck parser interface (using flex and bison)
//
// author: Ge Wang (ge@ccrma.stanford.edu | gewang@cs.princeton.edu)
// date: Autumn 2002 - original in chuck_main.cpp
//       Autumn 2005 - this file
//-----------------------------------------------------------------------------
#ifndef __CHUCK_PARSE_H__
#define __CHUCK_PARSE_H__

#include "chuck_def.h"
#include "chuck_absyn.h"

#include <stdio.h>
#include <string>


// 'C' specification necessary for windows to link properly
#ifdef __PLATFORM_WINDOWS__
  extern "C" a_Program g_program;
#else
  extern a_Program g_program;
#endif

// link with the parser
extern "C" int yyparse( void );
extern "C" void yyrestart( FILE * );
extern "C" void yyinitial( void );
extern "C" void yyflush( void );

struct yy_buffer_state;
typedef yy_buffer_state * YY_BUFFER_STATE;
extern "C" YY_BUFFER_STATE yy_scan_string( const char * );
extern "C" void yy_delete_buffer( YY_BUFFER_STATE );

// forward reference
struct Chuck_CompileTarget;
// parse ck file into abstract syntax tree
t_CKBOOL chuck_parse( Chuck_CompileTarget * target );
// reset the parser
void reset_parse();

// convert abstract syntax stmt to string
std::string absyn2str( a_Stmt stmt );
// convert abstract syntax expression to string
std::string absyn2str( a_Exp exp, t_CKBOOL appendSemicolon = TRUE );

// syntax highlighting tools
#include <vector>


// syntax types (for highlighting)
enum SyntaxType
{
    COMMA = 0,
    SEMICOLON,
    DBLCOLON,
    PAREN,
    DOT,
    CHUCK_OP,
    OPERATOR,
    KEYWORD,
    DEBUG_PRINT,
    SPORK,
    INTEGER,
    FLOATING,
    STRING,
    COMMENT,
    OTHER,

    NUM_SYNTAX_TYPES
};


// token info
struct SyntaxToken
{
    // the token
    std::string token;
    // type
    t_CKUINT type;

    // from the beginning of line
    std::string::size_type begin;
    // from the beginning of line
    std::string::size_type end;

    // constructor
    SyntaxToken() : type(0), begin(0), end(0) { }
    // copy constructor
    SyntaxToken( const SyntaxToken & rhs )
    {
        token = rhs.token;
        type = rhs.type;
        begin = rhs.begin;
        end = rhs.end;
    }
};


// token list
struct SyntaxTokenList
{
    std::vector<SyntaxToken> list;
    std::vector<SyntaxToken>::size_type howmany;
    // copy constructor
    SyntaxTokenList( const SyntaxTokenList & rhs )
    {
        // copy
        howmany = rhs.howmany;
        // allocate
        list.resize( howmany );
        // copy
        for( std::vector<SyntaxToken>::size_type i = 0; i < howmany; i++ )
            list[i] = rhs.list[i];
    }
    SyntaxTokenList() {
        howmany = 0;
    }
};


// token query
struct SyntaxQuery
{
public:
    t_CKBOOL parseLine( const std::string & line,
                        SyntaxTokenList & tokens );
};


#endif
