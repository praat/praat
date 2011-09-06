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

#include <wchar.h>
#if defined (macintosh) || defined (linux)
	#define regularExp_CHAR wchar_t
#else
	#define regularExp_CHAR unsigned short
#endif

#define NSUBEXP 50

/* Structure to contain the compiled form of a regular expression plus
   pointers to matched text.  `program' is the actual compiled regex code. */

#ifdef __cplusplus
	extern "C" {
#endif

typedef struct regexp {
   regularExp_CHAR *startp [NSUBEXP];  /* Captured text starting locations. */
   regularExp_CHAR *endp   [NSUBEXP];  /* Captured text ending locations. */
   regularExp_CHAR *extentpBW;         /* Points to the maximum extent of text scanned by
                               ExecRE in front of the string to achieve a match
                               (needed because of positive look-behind.) */
   regularExp_CHAR *extentpFW;         /* Points to the maximum extent of text scanned by
                               ExecRE to achieve a match (needed because of
                               positive look-ahead.) */
   int   top_branch;        /* Zero-based index of the top branch that matches.
                               Used by syntax highlighting only. */
   regularExp_CHAR  match_start;       /* Internal use only. */
   regularExp_CHAR  anchor;            /* Internal use only. */
   regularExp_CHAR  program [1];       /* Unwarranted chumminess with compiler. */
} regexp;

/* Flags for CompileRE default settings (Markus Schwarzenberg) */

typedef enum {
  REDFLT_STANDARD         = 0,
  REDFLT_CASE_INSENSITIVE = 1
  /* REDFLT_MATCH_NEWLINE = 2    Currently not used. */
} RE_DEFAULT_FLAG;

/* Compiles a regular expression into the internal format used by `ExecRE'. */

regexp * CompileRE (
   const regularExp_CHAR  *exp,         /* String containing the regex specification. */
   const wchar_t **errorText,   /* Text of any error message produced. */
   int  defaultFlags); /* Flags for default RE-operation */

/* Match a `regexp' structure against a string. */

int ExecRE (
   regexp *prog,                /* Compiled regex. */
   regexp *cross_regex_backref, /* Pointer to a `regexp' that was used in a
                                   previous execution of ExecRE.  Used to
                                   implement back references across regular
                                   expressions for use in syntax
                                   highlighting.*/
   const regularExp_CHAR   *string,              /* Text to search within. */
   const regularExp_CHAR   *end,                 /* Pointer to the end of `string'.  If NULL will
                                   scan from `string' until '\0' is found. */
   int     reverse,             /* Backward search. */
   regularExp_CHAR    prev_char,           /* Character immediately prior to `string'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   regularExp_CHAR    succ_char,           /* Character immediately after `end'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   const regularExp_CHAR   *delimiters,    /* Word delimiters to use (NULL for default) */
   const regularExp_CHAR   *look_behind_to,/* Boundary for look-behind; defaults to
                                   "string" if NULL */
   const regularExp_CHAR   *match_till);   /* Boundary to where match can extend.
                                   \0 is assumed to be the boundary if not
                                   set. Lookahead can cross the boundary. */


/* Perform substitutions after a `regexp' match. */

int SubstituteRE (
   const regexp *prog,
   const regularExp_CHAR   *source,
   regularExp_CHAR   *dest,
   int     max,
   int *errorType); // djmw 20080110 0: ok; 1: is not enough memory

/* Builds a default delimiter table that persists across `ExecRE' calls that
   is identical to `delimiters'.  Pass NULL for "default default" set of
   delimiters. */

void SetREDefaultWordDelimiters (
   regularExp_CHAR *delimiters);

/* Enable (or disable) brace counting quantifiers, e.g. `(foo){0,3}'. */

void EnableCountingQuantifier (int is_enabled);

#ifdef __cplusplus
	}
#endif

#endif /* _regularExp_h_ */
