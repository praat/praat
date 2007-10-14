/* Thing.c
 *
 * Copyright (C) 1992-2007 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2004/05/08 less geekspeak in info ("class")
 * pb 2004/05/08 added date to info
 * pb 2004/10/16 structThing -> theStructThing etc.
 * pb 2006/12/10 info method can contain only MelderInfo_writeXXX
 * pb 2007/06/11 wchar_t
 */

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

static long theTotalNumberOfThings;

static void destroy (I) { iam (Thing); Melder_free (my name); }

static void info (I) {
	iam (Thing);
	MelderInfo_writeLine2 (L"Object type: ", Thing_classNameW (me));
	MelderInfo_writeLine2 (L"Object name: ", my name ? my name : L"<no name>");
	time_t today = time (NULL);
	MelderInfo_writeLine2 (L"Date: ", Melder_peekUtf8ToWcs (ctime (& today)));   /* Includes a newline. */
}

static void nameChanged (Any thing) {
	(void) thing;
}

/* Because Thing has no parent, we cannot use the macro `class_methods': */
static void _Thing_initialize (void *table);
struct structThing_Table theStructThing =
	{ _Thing_initialize, "Thing", L"Thing", NULL, sizeof (struct structThing) };
Thing_Table classThing = & theStructThing;
static void _Thing_initialize (void *table) {
	Thing_Table us = table;
	us -> destroy = destroy;
	us -> info = info;
	us -> nameChanged = nameChanged;
}

char * Thing_className (I) { iam (Thing); return our _className; }
wchar_t * Thing_classNameW (I) { iam (Thing); return our _classNameW; }

Any Thing_new (void *table) {
	Thing_Table us = table;
	Thing me = (Thing) _Melder_calloc (1, us -> _size);
	if (! me) return Melder_errorp ("(Thing_new:) Out of memory.");
	theTotalNumberOfThings += 1;
	my methods = us;
	my name = NULL;
	if (! us -> destroy)   /* Table not initialized? */
		us -> _initialize (us);
	return me;
}

static int numberOfReadableClasses = 0;
static void *readableClasses [1 + 1000];
void Thing_recognizeClassesByName (void *readableClass, ...) {
	va_list arg;
	void *klas;
	if (! readableClass) return;
	va_start (arg, readableClass);
	readableClasses [++ numberOfReadableClasses] = readableClass;
	while ((klas = va_arg (arg, void*)) != NULL) {
		if (++ numberOfReadableClasses > 1000)
			Melder_fatal ("(Thing_recognizeClassesByName:) Too many (1001) readable classes.");
		readableClasses [numberOfReadableClasses] = klas;
	}
	va_end (arg);
}

static int numberOfAliases = 0;
static struct { void *readableClass; const char *otherName; } aliases [1 + 100];
void Thing_recognizeClassByOtherName (void *readableClass, const char *otherName) {
	aliases [++ numberOfAliases]. readableClass = readableClass;
	aliases [numberOfAliases]. otherName = otherName;
}

long Thing_version;   /* Global variable! */
void *Thing_classFromClassName (const char *klas) {
	int i;
	char *space;
	static char buffer [1+100];
	strncpy (buffer, klas ? klas : "", 100);
	space = strchr (buffer, ' ');
	if (space) {
		*space = '\0';   /* Strip version number. */
		Thing_version = atol (space + 1);
	} else {
		Thing_version = 0;
	}

	/*
	 * First try the class names that were registered with Thing_recognizeClassesByName.
	 */
	for (i = 1; i <= numberOfReadableClasses; i ++) {
		Thing_Table table = readableClasses [i];
		if (strequ (buffer, table -> _className)) {
			if (! table -> destroy)   /* Table not initialized? */
				table -> _initialize (table);
			return table;
		}
	}

	/*
	 * Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	 */
	for (i = 1; i <= numberOfAliases; i ++) {
		if (strequ (buffer, aliases [i]. otherName)) {
			Thing_Table table = aliases [i]. readableClass;
			if (! table -> destroy)   /* Table not initialized? */
				table -> _initialize (table);
			return table;
		}
	}

	return Melder_errorp ("(Thing_classFromClassName:) Class \"%s\" not recognized.", buffer);
}
void *Thing_classFromClassNameW (const wchar_t *klas) {
	return Thing_classFromClassName (Melder_peekWcsToUtf8 (klas));
}

Any Thing_newFromClassName (const char *className) {
	void *table = Thing_classFromClassName (className);
	if (! table) return Melder_errorp ("(Thing_newFromClassName:) Thing not created.");
	return Thing_new (table);
}
Any Thing_newFromClassNameW (const wchar_t *className) {
	void *table = Thing_classFromClassNameW (className);
	if (! table) return Melder_errorp ("(Thing_newFromClassName:) Thing not created.");
	return Thing_new (table);
}

void _Thing_forget (Thing *pme) {
	Thing me = *pme;
	if (! me) return;
	our destroy (me);
	Melder_free (me);
	theTotalNumberOfThings -= 1;
	*pme = NULL;
}

int Thing_subclass (void *klas, void *ancestor) {
	Thing_Table me = klas;
	while (me != ancestor && me != NULL) me = my _parent;
	return me != NULL;
}

int Thing_member (I, void *klas) {
	Thing me = void_me;
	if (! me) Melder_fatal ("(Thing_member:) Found NULL object.");
	return Thing_subclass (my methods, klas);
}

void * _Thing_check (I, void *klas, const char *fileName, int line) {
	Thing me = void_me;   /* NOT the macro `iam (Thing);' because that would be recursive. */
	Thing_Table table = my methods;
	if (! me) Melder_fatal ("(_Thing_check:) NULL object passed to a function\n"
		"in file %.100s at line %d.", fileName, line);
	while (table != klas && table != NULL) table = table -> _parent;
	if (! table)
		Melder_fatal ("(_Thing_check:) Object of wrong class (%.50s) passed to a function\n"
				"in file %.100s at line %d.", our _className, fileName, line);
	return me;
}

void Thing_info (I) {
	iam (Thing);
	Melder_clearInfo ();
	MelderInfo_open ();
	our info (me);   /* This calls a set of MelderInfo_writeXXX. */
	MelderInfo_close ();
}

wchar_t * Thing_getName (I) { iam (Thing); return my name; }

wchar_t * Thing_messageName (I) {
	iam (Thing);
	static MelderString buffers [11];
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	MelderString_append4 (& buffers [ibuffer], our _classNameW, L" \"", my name ? my name : L"(nameless)", L"\"");
	return buffers [ibuffer]. string;
}

void Thing_setName (I, const wchar_t *name) {
	iam (Thing);
	if (name != my name) {   /* Pointer comparison! So that Thing_setName (me, my name) does not fail. */
		Melder_free (my name);
		my name = Melder_wcsdup (name);
	}
	our nameChanged (me);
}

long Thing_getTotalNumberOfThings (void) { return theTotalNumberOfThings; }

void Thing_overrideClass (I, void *klas) {
	iam (Thing);
	my methods = klas;
	if (! ((Thing_Table) klas) -> destroy)
		((Thing_Table) klas) -> _initialize (klas);
}

void Thing_swap (I, thou) {
	iam (Thing);
	thouart (Thing);
	int i, n;
	char *p, *q;
	Melder_assert (my methods == thy methods);
	n = our _size;
	for (p = (char *) me, q = (char *) thee, i = n; i > 0; i --, p ++, q ++) {
		char tmp = *p;
		*p = *q;
		*q = tmp;
	}
}

/* End of file Thing.c */
