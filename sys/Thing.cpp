/* Thing.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2008/04/04 Thing_infoWithId
 * pb 2008/07/20 wchar_t
 * fb 2010/02/26 fix possible null pointer dereference
 * pb 2011/05/15 C++
 */

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

static long theTotalNumberOfThings;

static void destroy (I) { iam (Thing); Melder_free (my name); }

static void info (I) {
	iam (Thing);
	MelderInfo_writeLine2 (L"Object type: ", Thing_className (me));
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
	{ _Thing_initialize, L"Thing", NULL, sizeof (struct structThing) };
Thing_Table classThing = & theStructThing;
static void _Thing_initialize (void *table) {
	Thing_Table us = (Thing_Table) table;
	us -> destroy = destroy;
	us -> info = info;
	us -> nameChanged = nameChanged;
}

const wchar_t * Thing_className (I) { iam (Thing); return our _className; }

Any _Thing_new (void *table) {
	Thing_Table us = (Thing_Table) table;
	Thing me = (Thing) _Melder_calloc_e (1, us -> _size);
	if (! me) return Melder_errorp ("(Thing_new:) Out of memory.");
	theTotalNumberOfThings += 1;
	my methods = us;
	my name = NULL;
	if (! us -> destroy)   /* Table not initialized? */
		us -> _initialize (us);
	if (Melder_debug == 40) Melder_casual ("created %ls", my methods -> _className);
	return me;
}

static int numberOfReadableClasses = 0;
static void *readableClasses [1 + 1000];
static void _Thing_addOneReadableClass (Thing_Table readableClass) {
	if (++ numberOfReadableClasses > 1000)
		Melder_fatal ("(Thing_recognizeClassesByName:) Too many (1001) readable classes.");
	readableClasses [numberOfReadableClasses] = readableClass;
	readableClass -> sequentialUniqueIdOfReadableClass = numberOfReadableClasses;
}
void Thing_recognizeClassesByName (void *readableClass, ...) {
	va_list arg;
	void *klas;
	if (readableClass == NULL) return;
	va_start (arg, readableClass);
	_Thing_addOneReadableClass ((Thing_Table) readableClass);
	while ((klas = va_arg (arg, void*)) != NULL) {
		_Thing_addOneReadableClass ((Thing_Table) klas);
	}
	va_end (arg);
}

long Thing_listReadableClasses (void) {
	Melder_clearInfo ();
	MelderInfo_open ();
	for (long iclass = 1; iclass <= numberOfReadableClasses; iclass ++) {
		Thing_Table klas = (Thing_Table) readableClasses [iclass];
		MelderInfo_writeLine3 (Melder_integer (klas -> sequentialUniqueIdOfReadableClass), L"\t", klas -> _className);
	}
	MelderInfo_close ();
	return numberOfReadableClasses;
}

static int numberOfAliases = 0;
static struct { void *readableClass; const wchar_t *otherName; } aliases [1 + 100];
void Thing_recognizeClassByOtherName (void *readableClass, const wchar_t *otherName) {
	aliases [++ numberOfAliases]. readableClass = readableClass;
	aliases [numberOfAliases]. otherName = otherName;
}

long Thing_version;   /* Global variable! */
void *Thing_classFromClassName (const wchar_t *klas) {
	int i;
	wchar_t *space;
	static wchar_t buffer [1+100];
	wcsncpy (buffer, klas ? klas : L"", 100);
	space = wcschr (buffer, ' ');
	if (space) {
		*space = '\0';   /* Strip version number. */
		Thing_version = wcstol (space + 1, NULL, 10);
	} else {
		Thing_version = 0;
	}

	/*
	 * First try the class names that were registered with Thing_recognizeClassesByName.
	 */
	for (i = 1; i <= numberOfReadableClasses; i ++) {
		Thing_Table table = (Thing_Table) readableClasses [i];
		if (wcsequ (buffer, table -> _className)) {
			if (! table -> destroy)   /* Table not initialized? */
				table -> _initialize (table);
			return table;
		}
	}

	/*
	 * Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	 */
	for (i = 1; i <= numberOfAliases; i ++) {
		if (wcsequ (buffer, aliases [i]. otherName)) {
			Thing_Table table = (Thing_Table) aliases [i]. readableClass;
			if (! table -> destroy)   /* Table not initialized? */
				table -> _initialize (table);
			return table;
		}
	}

	return Melder_errorp3 (L"(Thing_classFromClassName:) Class \"", buffer, L"\" not recognized.");
}

Any Thing_newFromClassNameA (const char *className) {
	void *table = Thing_classFromClassName (Melder_peekUtf8ToWcs (className));
	if (! table) return Melder_errorp ("(Thing_newFromClassName:) Thing not created.");
	return _Thing_new (table);
}
Any Thing_newFromClassName (const wchar_t *className) {
	void *table = Thing_classFromClassName (className);
	if (! table) return Melder_errorp ("(Thing_newFromClassName:) Thing not created.");
	return _Thing_new (table);
}

void _Thing_forget (Thing *pme) {
	Thing me = *pme;
	if (! me) return;
	if (Melder_debug == 40) Melder_casual ("destroying %ls", my methods -> _className);
	our destroy (me);
	Melder_free (me);
	theTotalNumberOfThings -= 1;
	*pme = NULL;
}

int Thing_subclass (void *klas, void *ancestor) {
	Thing_Table me = (Thing_Table) klas;
	while (me != ancestor && me != NULL) me = my _parent;
	return me != NULL;
}

int Thing_member (I, void *klas) {
	Thing me = (Thing) void_me;
	if (! me) Melder_fatal ("(Thing_member:) Found NULL object.");
	return Thing_subclass (my methods, klas);
}

void * _Thing_check (I, void *klas, const char *fileName, int line) {
	Thing me = (Thing) void_me;   /* NOT the macro `iam (Thing);' because that would be recursive. */
	if (! me) Melder_fatal ("(_Thing_check:) NULL object passed to a function\n"
		"in file %.100s at line %d.", fileName, line);
	Thing_Table table = my methods;
	while (table != klas && table != NULL) table = table -> _parent;
	if (! table)
		Melder_fatal ("(_Thing_check:) Object of wrong class (%.50s) passed to a function\n"
				"in file %.100s at line %d.", Melder_peekWcsToUtf8 (our _className), fileName, line);
	return me;
}

void Thing_infoWithId (I, unsigned long id) {
	iam (Thing);
	Melder_clearInfo ();
	MelderInfo_open ();
	if (id != 0) MelderInfo_writeLine2 (L"Object id: ", Melder_integer (id));
	our info (me);   /* This calls a set of MelderInfo_writeXXX. */
	MelderInfo_close ();
}

void Thing_info (I) {
	iam (Thing);
	Thing_infoWithId (me, 0);
}

wchar_t * Thing_getName (I) { iam (Thing); return my name; }

wchar_t * Thing_messageName (I) {
	iam (Thing);
	static MelderString buffers [11];
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	MelderString_append4 (& buffers [ibuffer], our _className, L" \"", my name ? my name : L"(nameless)", L"\"");
	return buffers [ibuffer]. string;
}

void Thing_setName (I, const wchar_t *name) {
	iam (Thing);
	/*
	 * First check without change.
	 */
	wchar *newName = Melder_wcsdup_f (name);   // BUG: that's no checking
	/*
	 * Then change without error.
	 */
	Melder_free (my name);
	my name = newName;
	our nameChanged (me);   // BUG: what if this fails?
}

long Thing_getTotalNumberOfThings (void) { return theTotalNumberOfThings; }

void Thing_overrideClass (I, void *klas) {
	iam (Thing);
	my methods = (Thing_Table) klas;
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

/* End of file Thing.cpp */
