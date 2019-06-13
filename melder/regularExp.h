#ifndef _regularExp_h_
#define _regularExp_h_
/*******************************************************************************
*                                                                              *
* regularExp.h -- Nirvana Editor Regular Expression Package Header File        *
*                                                                              *
* Copyright 2002 The NEdit Developers                                          *
*                                                                              *
* This is free software; you can redistribute it and/or modify it under the    *
* terms of the GNU General Public License as published by the Free Software    *
* Foundation; either version 2 of the License, or (at your option) any later   *
* version. In addition, you may distribute version of this program linked to   *
* Motif or Open Motif. See README for details.                                 *
*                                                                              *
* This software is distributed in the hope that it will be useful, but WITHOUT *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU General Public License along with *
* software; if not, write to the Free Software Foundation, Inc., 59 Temple     *
* Place, Suite 330, Boston, MA  02111-1307 USA                                 *
*                                                                              *
* Nirvana Text Editor                                                          *
* July 31, 2001                                                                *
*                                                                              *
*                                                                              *
*******************************************************************************/
/*
	djmw 20080110 Extra parameter for SubstituteRE to allow error differentiation
 */

/* Number of text capturing parentheses allowed. */

#include "melder.h"

#define NSUBEXP 50

/* Structure to contain the compiled form of a regular expression plus
   pointers to matched text.  `program' is the actual compiled regex code. */

typedef struct regexp {
   char32 *startp [NSUBEXP];  /* Captured text starting locations. */
   char32 *endp   [NSUBEXP];  /* Captured text ending locations. */
   char32 *extentpBW;         /* Points to the maximum extent of text scanned by
                               ExecRE in front of the string to achieve a match
                               (needed because of positive look-behind.) */
   char32 *extentpFW;         /* Points to the maximum extent of text scanned by
                               ExecRE to achieve a match (needed because of
                               positive look-ahead.) */
   int   top_branch;        /* Zero-based index of the top branch that matches.
                               Used by syntax highlighting only. */
   char32  match_start;       /* Internal use only. */
   char32  anchor;            /* Internal use only. */
   char32  program [1];       /* Unwarranted chumminess with compiler. */
} regexp;

/* Flags for CompileRE default settings (Markus Schwarzenberg) */

typedef enum {
  REDFLT_STANDARD         = 0,
  REDFLT_CASE_INSENSITIVE = 1
  /* REDFLT_MATCH_NEWLINE = 2    Currently not used. */
} RE_DEFAULT_FLAG;

/* Compiles a regular expression into the internal format used by `ExecRE'. */

regexp * CompileRE (
   conststring32 exp,         /* String containing the regex specification. */
   conststring32 *errorText,   /* Text of any error message produced. */
   int  defaultFlags); /* Flags for default RE-operation */

regexp *CompileRE_throwable (conststring32 exp, int defaultFlags);

/* Match a `regexp' structure against a string. */

int ExecRE (
   regexp *prog,                /* Compiled regex. */
   regexp *cross_regex_backref, /* Pointer to a `regexp' that was used in a
                                   previous execution of ExecRE.  Used to
                                   implement back references across regular
                                   expressions for use in syntax
                                   highlighting.*/
   conststring32 string,              /* Text to search within. */
   const char32 *end,                 /* Pointer to the end of `string'.  If NULL will
                                   scan from `string' until '\0' is found. */
   int     reverse,             /* Backward search. */
   char32    prev_char,           /* Character immediately prior to `string'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   char32    succ_char,           /* Character immediately after `end'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   conststring32 look_behind_to,/* Boundary for look-behind; defaults to
                                   "string" if NULL */
   conststring32 match_till);   /* Boundary to where match can extend.
                                   \0 is assumed to be the boundary if not
                                   set. Lookahead can cross the boundary. */


/* Perform substitutions after a `regexp' match. */

int SubstituteRE (
   const regexp *prog,
   conststring32 source,
   mutablestring32 dest,
   int     max,
   int *errorType); // djmw 20080110 0: ok; 1: is not enough memory

/* Enable (or disable) brace counting quantifiers, e.g. `(foo){0,3}'. */

void EnableCountingQuantifier (int is_enabled);

#endif /* _regularExp_h_ */
