#ifndef _regularExp_h_
#define _regularExp_h_
/* regularExp.h
 *
 * Copyright (C) 1994-2002 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/* $Id: regularExp.h,v 1.5 2001/04/06 09:49:56 amai Exp $ */
/*----------------------------------------------------------------------*
 *  This is regularExp.h: NEdit Regular Expression Package Header File
 *----------------------------------------------------------------------*/

/*
 djmw 20020812 GPL header
*/

/* Number of text capturing parentheses allowed. */

#define NSUBEXP 50

/* Structure to contain the compiled form of a regular expression plus
   pointers to matched text.  `program' is the actual compiled regex code. */

typedef struct regexp {
   char *startp [NSUBEXP];  /* Captured text starting locations. */
   char *endp   [NSUBEXP];  /* Captured text ending locations. */
   char *extentp;           /* Points to the maximum extent of text scanned by
                               ExecRE to achieve a match (needed because of
                               positive look-ahead.) */
   char  match_start;       /* Internal use only. */
   char  anchor;            /* Internal use only. */
   char  program [1];       /* Unwarranted chumminess with compiler. */
} regexp;

/* Flags for CompileRE default settings (Markus Schwarzenberg) */

typedef enum {
  REDFLT_STANDARD         = 0,
  REDFLT_CASE_INSENSITIVE = 1
  /* REDFLT_MATCH_NEWLINE = 2    Currently not used. */ 
} RE_DEFAULT_FLAG;

/* Compiles a regular expression into the internal format used by `ExecRE'. */

regexp * CompileRE (
   const char  *exp,         /* String containing the regex specification. */
   char **errorText,   /* Text of any error message produced. */
   int  defaultFlags); /* Flags for default RE-operation */

/* Match a `regexp' structure against a string. */

int ExecRE (
   regexp *prog,                /* Compiled regex. */
   regexp *cross_regex_backref, /* Pointer to a `regexp' that was used in a
                                   previous execution of ExecRE.  Used to
                                   implement back references across regular
                                   expressions for use in syntax
                                   highlighting.*/
   const char   *string,              /* Text to search within. */
   const char   *end,                 /* Pointer to the end of `string'.  If NULL will
                                   scan from `string' until '\0' is found. */
   int     reverse,             /* Backward search. */
   char    prev_char,           /* Character immediately prior to `string'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   char    succ_char,           /* Character immediately after `end'.  Set
                                   to '\n' or '\0' if true beginning of text. */
   const char   *delimiters);         /* Word delimiters to use (NULL for default) */

/* Perform substitutions after a `regexp' match. */

void SubstituteRE (
   regexp *prog,
   const char   *source,
   char   *dest,
   int     max);

/* Builds a default delimiter table that persists across `ExecRE' calls that
   is identical to `delimiters'.  Pass NULL for "default default" set of
   delimiters. */

void SetREDefaultWordDelimiters (
   char *delimiters);

/* Enable (or disable) brace counting quantifiers, e.g. `(foo){0,3}'. */

void EnableCountingQuantifier (int is_enabled);

#endif /* _regularExp_h_ */
