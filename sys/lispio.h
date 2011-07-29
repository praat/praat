#ifndef _lispio_h_
#define _lispio_h_
/* lispio.h
 *
 * Copyright (C) 1994-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "melder.h"

/* A LISP object is a list or an atom. */

typedef struct Lispio { long length; const char *string; } Lispio;

/* A LISP sequence is a sequence of LISP objects. */

typedef struct LispioSeq { long length; const char *string; } LispioSeq;

/* All of the following routines expect and return structures without leading spaces. */
/* Moreover, a Lispio has no trailing spaces. */

int Lispio_openList (const Lispio *me, LispioSeq *thee);
/*
	Function:
		strip the leading '(' and spaces; strip the trailing ')'.
	Return value:
		my length != 0 && my string [0] == '(';   // Am I a list?
	Postconditions:
		result == 0 || thy length <= my length - 2;
*/

int Lispio_openListFromFile (LispioSeq *me, FILE *f, char *buffer, long maxLength);
/*
	Arguments:
		f: an open text stream;
		buffer [0..maxLength-1]: a buffer preallocated on the heap;
	Return value:
		0: failure:
			Not a list: expected leading '(' but found character '%c'.
			Early end-of-file detected: no matching ')': depth %ld instead of 0.
			Buffer too small for list: list longer than <maxLength> bytes.
		EOF: normal end of file.
		1: OK.
	Warning:
		'me' and all the structures derived from 'me' are valid until the next Lispio_openListFromFile ().
	Example:
		reading all lists in a file:
			LispioSeq entry;
			autofile f = Melder_fopen (fileName, "r");
			autostring8 buffer = Melder_malloc (200000);
			for (;;)
			{
				int status = Lispio_openListFromFile (& entry, f, buffer, 200000);
				if (! status) goto error;
				if (status == EOF) break;   // Leave loop.
				if (! myProcessList (entry)) goto error;
			}
			f.close (file);
*/

char * Lispio_string (const Lispio *me);
/*
	Return value:
		a new null-terminated C string, or NULL if out of memory.
	Postcondition:
		result == NULL || strlen (result) == my length && result [0..my length-1] == my string [0..my length-1];
*/

int Lispio_equal (const Lispio *me, const Lispio *thee);
/*
	Returns thy length == my length && thy string [0..my length-1] == my string [0..my length-1];
*/

int Lispio_strequ (const Lispio *me, const char *string);
/*
	Returns strlen (string) == my length && string [0..my length-1] == my string [0..my length-1];
*/

int Lispio_read (LispioSeq *me, Lispio *first);
/*
	Return value:
		1 if something was found; 0 otherwise.
	Example:
		cycle through all the LISP objects in a sequence:
			while (Lispio_read (& sequence, & object))
				processObject (& object);
*/

long Lispio_count (const LispioSeq *me);

int Lispio_isInteger (const Lispio *me);

int Lispio_integer (const Lispio *me, long *value);

/* End of file lispio.h */
#endif
