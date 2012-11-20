/* Thing.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

static long theTotalNumberOfThings;

void structThing :: v_info ()
{
	MelderInfo_writeLine (L"Object type: ", Thing_className (this));
	MelderInfo_writeLine (L"Object name: ", this -> name ? this -> name : L"<no name>");
	time_t today = time (NULL);
	MelderInfo_writeLine (L"Date: ", Melder_peekUtf8ToWcs (ctime (& today)));   /* Includes a newline. */
}

/*
 * Instead of the Thing_implement macro.
 */
struct structClassInfo theClassInfo_Thing = {
	L"Thing",
	NULL,      // no parent class
	sizeof (class structThing),
	NULL,      // no _new function (not needed; plus, it would have to be called "_Thing_new", but that name has been given to something else)
	0,         // version
	0,         // sequentialUniqueIdOfReadableClass
	NULL       // dummyObject
};
ClassInfo classThing = & theClassInfo_Thing;

const wchar_t * Thing_className (Thing me) { return my classInfo -> className; }

Any _Thing_new (ClassInfo classInfo_) {
	Thing me = (Thing) classInfo_ -> _new ();
	trace ("created %ls", classInfo_ -> className);
	theTotalNumberOfThings += 1;
	my classInfo = classInfo_;
	Melder_assert (my name == NULL);   // check that _new called calloc
	if (Melder_debug == 40) Melder_casual ("created %ls (%p, %p)", classInfo_ -> className, classInfo_, me);
	return me;
}

static int theNumberOfReadableClasses = 0;
static ClassInfo theReadableClasses [1 + 1000];
static void _Thing_addOneReadableClass (ClassInfo readableClass) {
	if (++ theNumberOfReadableClasses > 1000)
		Melder_fatal ("(Thing_recognizeClassesByName:) Too many (1001) readable classes.");
	theReadableClasses [theNumberOfReadableClasses] = readableClass;
	readableClass -> sequentialUniqueIdOfReadableClass = theNumberOfReadableClasses;
}
void Thing_recognizeClassesByName (ClassInfo readableClass, ...) {
	va_list arg;
	if (readableClass == NULL) return;
	va_start (arg, readableClass);
	_Thing_addOneReadableClass (readableClass);
	ClassInfo klas;
	while ((klas = va_arg (arg, ClassInfo)) != NULL) {
		_Thing_addOneReadableClass (klas);
	}
	va_end (arg);
}

long Thing_listReadableClasses (void) {
	Melder_clearInfo ();
	MelderInfo_open ();
	for (long iclass = 1; iclass <= theNumberOfReadableClasses; iclass ++) {
		ClassInfo klas = theReadableClasses [iclass];
		MelderInfo_writeLine (Melder_integer (klas -> sequentialUniqueIdOfReadableClass), L"\t", klas -> className);
	}
	MelderInfo_close ();
	return theNumberOfReadableClasses;
}

static int theNumberOfAliases = 0;
static struct {
	ClassInfo readableClass;
	const wchar_t *otherName;
} theAliases [1 + 100];

void Thing_recognizeClassByOtherName (ClassInfo readableClass, const wchar_t *otherName) {
	theAliases [++ theNumberOfAliases]. readableClass = readableClass;
	theAliases [theNumberOfAliases]. otherName = otherName;
}

long Thing_version;   // global variable!
ClassInfo Thing_classFromClassName (const wchar_t *klas) {
	static wchar_t buffer [1+100];
	wcsncpy (buffer, klas ? klas : L"", 100);
	wchar_t *space = wcschr (buffer, ' ');
	if (space) {
		*space = '\0';   // strip version number
		Thing_version = wcstol (space + 1, NULL, 10);
	} else {
		Thing_version = 0;
	}

	/*
	 * First try the class names that were registered with Thing_recognizeClassesByName.
	 */
	for (int i = 1; i <= theNumberOfReadableClasses; i ++) {
		ClassInfo classInfo = theReadableClasses [i];
		if (wcsequ (buffer, classInfo -> className)) {
			return classInfo;
		}
	}

	/*
	 * Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	 */
	for (int i = 1; i <= theNumberOfAliases; i ++) {
		if (wcsequ (buffer, theAliases [i]. otherName)) {
			ClassInfo classInfo = theAliases [i]. readableClass;
			return classInfo;
		}
	}

	Melder_throw ("Class \"", buffer, "\" not recognized.");
}

Any Thing_newFromClassNameA (const char *className) {
	try {
		ClassInfo classInfo = Thing_classFromClassName (Melder_peekUtf8ToWcs (className));
		return _Thing_new (classInfo);
	} catch (MelderError) {
		Melder_throw (className, " not created.");
	}
}

Any Thing_newFromClassName (const wchar_t *className) {
	try {
		ClassInfo classInfo = Thing_classFromClassName (className);
		return _Thing_new (classInfo);
	} catch (MelderError) {
		Melder_throw (className, " not created.");
	}
}

Thing _Thing_dummyObject (ClassInfo classInfo) {
	if (classInfo -> dummyObject == NULL) {
		classInfo -> dummyObject = (Thing) classInfo -> _new ();
	}
	Melder_assert (classInfo -> dummyObject != NULL);
	return classInfo -> dummyObject;
}

void _Thing_forget_nozero (Thing me) {
	if (! me) return;
	if (Melder_debug == 40) Melder_casual ("destroying %ls", my classInfo -> className);
	my v_destroy ();
	theTotalNumberOfThings -= 1;
}

void _Thing_forget (Thing me) {
	if (! me) return;
	if (Melder_debug == 40) Melder_casual ("destroying %ls", my classInfo -> className);
	my v_destroy ();
	trace ("destroying %ls", my classInfo -> className);
	//Melder_free (me);
	delete me;
	theTotalNumberOfThings -= 1;
}

bool Thing_subclass (ClassInfo klas, ClassInfo ancestor) {
	while (klas != ancestor && klas != NULL) klas = klas -> parent;
	return klas != NULL;
}

bool Thing_member (Thing me, ClassInfo klas) {
	if (! me) Melder_fatal ("(Thing_member:) Found NULL object.");
	return Thing_subclass (my classInfo, klas);
}

void * _Thing_check (Thing me, ClassInfo klas, const char *fileName, int line) {
	if (! me) Melder_fatal ("(_Thing_check:) NULL object passed to a function\n"
		"in file %.100s at line %d.", fileName, line);
	ClassInfo l_classInfo = my classInfo;
	while (l_classInfo != klas && l_classInfo != NULL) l_classInfo = l_classInfo -> parent;
	if (! l_classInfo)
		Melder_fatal ("(_Thing_check:) Object of wrong class (%.50s) passed to a function\n"
				"in file %.100s at line %d.", Melder_peekWcsToUtf8 (my classInfo -> className), fileName, line);
	return me;
}

void Thing_infoWithId (Thing me, unsigned long id) {
	Melder_clearInfo ();
	MelderInfo_open ();
	if (id != 0) MelderInfo_writeLine (L"Object id: ", Melder_integer (id));
	my v_info ();
	MelderInfo_close ();
}

void Thing_info (Thing me) {
	Thing_infoWithId (me, 0);
}

wchar_t * Thing_getName (Thing me) { return my name; }

wchar_t * Thing_messageName (Thing me) {
	static MelderString buffers [11];
	static int ibuffer = 0;
	if (++ ibuffer == 11) ibuffer = 0;
	MelderString_empty (& buffers [ibuffer]);
	if (my name) {
		MelderString_append (& buffers [ibuffer], my classInfo -> className, L" \"", my name, L"\"");
	} else {
		MelderString_append (& buffers [ibuffer], my classInfo -> className);
	}
	return buffers [ibuffer]. string;
}

void Thing_setName (Thing me, const wchar_t *name) {
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
	Melder_assert (my classInfo == thy classInfo);
	int n = my classInfo -> size;
	char *p, *q;
	int i;
	for (p = (char *) me, q = (char *) thee, i = n; i > 0; i --, p ++, q ++) {
		char tmp = *p;
		*p = *q;
		*q = tmp;
	}
}

/* End of file Thing.cpp */
