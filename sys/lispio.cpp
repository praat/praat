/* lispio.cpp
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

/*
 * pb 1995/07/28
 * pb 2002/03/07 GPL
 * pb 2011/05/15 C++
 */

#include "melder.h"
#include "lispio.h"
#include <ctype.h>

#define my  me ->
#define thy  thee ->

int Lispio_openList (const Lispio *me, LispioSeq *thee) {
	int ch;
	if (my length == 0) return 0;   /* Empty object. */
	if (my string [0] != '(') return 0;   /* Not a list. */
	Melder_assert (my length >= 2);
	Melder_assert (my string [my length - 1] == ')');
	/* Peel leading '(' and trailing ')'. */
	thy length = my length - 2;
	thy string = my string + 1;
	/* Forget leading spaces. */
	while (thy length != 0 && ((ch = *thy string) == ' ' || ch == '\n' || ch == '\t')) {
		thy length --;
		thy string ++;
	}
	return 1;
}

int Lispio_openListFromFile (LispioSeq *seq, FILE *f, char *buffer, long maxLength) {
	long depth = 0, length = 0;
	int ch;
	Lispio list;

	/* Skip spaces to left parenthesis, checking for normal end-of-file. */

	while ((ch = getc (f)) != '(') {
		if (ch == EOF) return EOF;
		if (ch != ' ' && ch != '\n' && ch != '\t')
			Melder_throw ("Lispio_openListFromFile: not a list: expected leading `(' but found character `%c'.", ch);
	}
	buffer [0] = ch;
	depth = 1;
	length = 1;
	while (depth != 0) {
		ch = getc (f);
		if (ch == EOF)
			Melder_throw ("Lispio_openListFromFile: early end-of-file detected: no matching `)': depth %ld instead of 0.", depth);
		if (length >= maxLength)
			Melder_throw ("Lispio_openListFromFile: buffer too small for list: list longer than %ld bytes.", maxLength);
		buffer [length ++] = ch;
		if (ch == '(') depth += 1; else if (ch == ')') depth -= 1;
	}
	list.length = length;
	list.string = buffer;
	return Lispio_openList (& list, seq);   // always 1
}

char * Lispio_string (const Lispio *me) {
	autostring8 result = Melder_malloc (char, my length + 1);
	memcpy (result.peek(), my string, my length);
	result [my length] = '\0';
	return result.transfer();
}

int Lispio_equal (const Lispio *me, const Lispio *thee) {
	long length = my length;
	return thy length == length && (length == 0 || ! memcmp (my string, thy string, length));
}

int Lispio_strequ (const Lispio *me, const char *string) {
	long length = my length, thyLength = strlen (string);
	return thyLength == length && (length == 0 || ! memcmp (my string, string, length));
}

int Lispio_read (LispioSeq *me, Lispio *first) {
	int ch;
	first -> string = my string;
	if (my length == 0) { first -> length = 0; return 0; }
	if ((ch = my string [0]) == '(') {   /* First is a list? */
		long depth = 1;
		first -> length = 1;
		while (depth != 0) {
			ch = first -> string [first -> length ++];
			if (ch == '(') depth += 1; else if (ch == ')') depth -= 1;
		}
	} else {   /* First is an atom? */
		first -> length = 1;
		while ((ch = first -> string [first -> length ++]) != ' ' && ch != '\n' && ch != '\t' && ch != '(' && ch != ')')
			{ }
		first -> length -= 1;   /* Put back delimiter. */
	}
	my length -= first -> length;
	Melder_assert (my length >= 0);
	my string += first -> length;
	while (my length != 0 && ((ch = *my string) == ' ' || ch == '\n' || ch == '\t'))
		{ my length --; my string ++; }   /* Remove leading spaces. */
	return 1;
}

long Lispio_count (const LispioSeq *me) {
	long result = 0;
	LispioSeq seq = * (LispioSeq *) me;
	Lispio obj;
	while (Lispio_read (& seq, & obj)) result ++;
	return result;
}

int Lispio_integer (const Lispio *me, long *value) {
	char buffer [12];
	int ichar = 0, numeric = 1;
	*value = 0;
	if (my length < 1 || my length > 11) return 0;
	if (my string [0] == '+' || my string [0] == '-') {
		if (my length < 2) return 0;   /* Only a sign. */
		ichar = 1;
	}
	for (; ichar < my length; ichar ++) if (! isdigit (my string [ichar])) numeric = 0;
	strncpy (buffer, my string, my length);
	buffer [my length] = '\0';
	*value = atol (buffer);
	return numeric;
}

/* End of file lispio.cpp */
