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
 * pb 2011/07/13 C++
 */

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

static long theTotalNumberOfThings;

static void destroy (I) { iam (Thing); }

void structThing :: v_info ()
{
	MelderInfo_writeLine2 (L"Object type: ", Thing_className (this));
	MelderInfo_writeLine2 (L"Object name: ", this -> name ? this -> name : L"<no name>");
	time_t today = time (NULL);
	MelderInfo_writeLine2 (L"Date: ", Melder_peekUtf8ToWcs (ctime (& today)));   /* Includes a newline. */
}

static void info (I) {
	iam (Thing);
}

static void _Thing_initialize (void *table);
struct structThing_Table theStructThing = {
	_Thing_initialize,
	L"Thing",
	NULL,      // no parent class
	sizeof (struct structThing),
	NULL       // no _new function (not needed; plus, it would have to be called "_Thing_new", but that name has been given to something else)
};
Thing_Table classThing = & theStructThing;
static void _Thing_initialize (void *table) {
	Thing_Table us = (Thing_Table) table;
	us -> destroy = destroy;
	us -> info = info;
}

const wchar * Thing_className (Thing me) { return our _className; }

Any _Thing_new (void *table) {
	Thing_Table us = (Thing_Table) table;
	if (! us -> destroy) {   // table not initialized?
		us -> _initialize (us);
		//Melder_casual ("Initializing class %ls (%ld).", us -> _className, table);
	}
	Thing me = (Thing) us -> _new ();
	theTotalNumberOfThings += 1;
	my methods = us;
	my name = NULL;
	if (Melder_debug == 40) Melder_casual ("created %ls (%ld, %ld, %ld)", my methods -> _className, us, table, my methods);
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
	if (readableClass == NULL) return;
	va_start (arg, readableClass);
	_Thing_addOneReadableClass ((Thing_Table) readableClass);
	void *klas;
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
	static wchar buffer [1+100];
	wcsncpy (buffer, klas ? klas : L"", 100);
	wchar *space = wcschr (buffer, ' ');
	if (space) {
		*space = '\0';   // strip version number
		Thing_version = wcstol (space + 1, NULL, 10);
	} else {
		Thing_version = 0;
	}

	/*
	 * First try the class names that were registered with Thing_recognizeClassesByName.
	 */
	for (int i = 1; i <= numberOfReadableClasses; i ++) {
		Thing_Table table = (Thing_Table) readableClasses [i];
		if (wcsequ (buffer, table -> _className)) {
			if (! table -> destroy)   // table not initialized?
				table -> _initialize (table);
			return table;
		}
	}

	/*
	 * Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	 */
	for (int i = 1; i <= numberOfAliases; i ++) {
		if (wcsequ (buffer, aliases [i]. otherName)) {
			Thing_Table table = (Thing_Table) aliases [i]. readableClass;
			if (! table -> destroy)   // table not initialized?
				table -> _initialize (table);
			return table;
		}
	}

	Melder_throw ("Class \"", buffer, "\" not recognized.");
}

Any Thing_newFromClassNameA (const char *className) {
	try {
		void *table = Thing_classFromClassName (Melder_peekUtf8ToWcs (className));
		return _Thing_new (table);
	} catch (MelderError) {
		Melder_throw (className, " not created.");
	}
}

Any Thing_newFromClassName (const wchar *className) {
	try {
		void *table = Thing_classFromClassName (className);
		return _Thing_new (table);
	} catch (MelderError) {
		Melder_throw (className, " not created.");
	}
}

void _Thing_forget_nozero (Thing me) {
	if (! me) return;
	if (Melder_debug == 40) Melder_casual ("destroying %ls", my methods -> _className);
	our destroy (me);
	my v_destroy ();
	theTotalNumberOfThings -= 1;
}

void _Thing_forget (Thing *pme) {
	Thing me = *pme;
	if (! me) return;
	if (Melder_debug == 40) Melder_casual ("destroying %ls", my methods -> _className);
	our destroy (me);
	my v_destroy ();
	Melder_free (me);
	theTotalNumberOfThings -= 1;
	*pme = NULL;
}

bool Thing_subclass (void *klas, void *ancestor) {
	Thing_Table me = (Thing_Table) klas;
	while (me != ancestor && me != NULL) me = my _parent;
	return me != NULL;
}

bool Thing_member (Thing me, void *klas) {
	if (! me) Melder_fatal ("(Thing_member:) Found NULL object.");
	return Thing_subclass (my methods, klas);
}

void * _Thing_check (Thing me, void *klas, const char *fileName, int line) {
	if (! me) Melder_fatal ("(_Thing_check:) NULL object passed to a function\n"
		"in file %.100s at line %d.", fileName, line);
	Thing_Table table = my methods;
	while (table != klas && table != NULL) table = table -> _parent;
	if (! table)
		Melder_fatal ("(_Thing_check:) Object of wrong class (%.50s) passed to a function\n"
				"in file %.100s at line %d.", Melder_peekWcsToUtf8 (our _className), fileName, line);
	return me;
}

void Thing_infoWithId (Thing me, unsigned long id) {
	Melder_clearInfo ();
	MelderInfo_open ();
	if (id != 0) MelderInfo_writeLine2 (L"Object id: ", Melder_integer (id));
	my v_info ();
	our info (me);   // this calls a set of MelderInfo_writeXXX
	MelderInfo_close ();
}

void Thing_info (Thing me) {
	Thing_infoWithId (me, 0);
}

wchar * Thing_getName (Thing me) { return my name; }

wchar * Thing_messageName (Thing me) {
	static MelderString buffers [11];
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	MelderString_append4 (& buffers [ibuffer], our _className, L" \"", my name ? my name : L"(nameless)", L"\"");
	return buffers [ibuffer]. string;
}

void Thing_setName (Thing me, const wchar *name) {
	/*
	 * First check without change.
	 */
	autostring newName = Melder_wcsdup_f (name);   // BUG: that's no checking
	/*
	 * Then change without error.
	 */
	Melder_free (my name);
	my name = newName.transfer();
	my v_nameChanged ();
}

long Thing_getTotalNumberOfThings (void) { return theTotalNumberOfThings; }

void Thing_swap (Thing me, Thing thee) {
	Melder_assert (my methods == thy methods);
	int n = our _size;
	char *p, *q;
	int i;
	for (p = (char *) me, q = (char *) thee, i = n; i > 0; i --, p ++, q ++) {
		char tmp = *p;
		*p = *q;
		*q = tmp;
	}
}

/* End of file Thing.cpp */
