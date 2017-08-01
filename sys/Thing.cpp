/* Thing.cpp
 *
 * Copyright (C) 1992-2012,2015,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdarg.h>
#include <time.h>
#include "Thing.h"

long theTotalNumberOfThings;

void structThing :: v_info ()
{
	MelderInfo_writeLine (U"Object type: ", Thing_className (this));
	MelderInfo_writeLine (U"Object name: ", this -> name ? this -> name : U"<no name>");
	time_t today = time (nullptr);
	MelderInfo_writeLine (U"Date: ", Melder_peek8to32 (ctime (& today)));   // includes a newline
}

/*
 * Instead of the Thing_implement macro.
 */
struct structClassInfo theClassInfo_Thing = {
	U"Thing",
	nullptr,   // no parent class
	sizeof (class structThing),
	nullptr,   // no _new function (not needed, because Thing is never instantiated)
	0,         // version
	0,         // sequentialUniqueIdOfReadableClass
	nullptr    // dummyObject
};
ClassInfo classThing = & theClassInfo_Thing;

const char32 * Thing_className (Thing me) { return my classInfo -> className; }

autoThing Thing_newFromClass (ClassInfo classInfo) {
	autoThing me (classInfo -> _new ());
	trace (U"created ", classInfo -> className);
	theTotalNumberOfThings += 1;
	my classInfo = classInfo;
	Melder_assert (my name == nullptr);   // confirm that _new called calloc, so that we see null pointers
	if (Melder_debug == 40) Melder_casual (U"created ", classInfo -> className, U" (", Melder_pointer (classInfo), U", ", me.get(), U")");
	return me;
}

static int theNumberOfReadableClasses = 0;
static ClassInfo theReadableClasses [1 + 1000];
static void _Thing_addOneReadableClass (ClassInfo readableClass) {
	if (++ theNumberOfReadableClasses > 1000)
		Melder_fatal (U"(Thing_recognizeClassesByName:) Too many (1001) readable classes.");
	theReadableClasses [theNumberOfReadableClasses] = readableClass;
	readableClass -> sequentialUniqueIdOfReadableClass = theNumberOfReadableClasses;
}
void Thing_recognizeClassesByName (ClassInfo readableClass, ...) {
	va_list arg;
	if (! readableClass) return;
	va_start (arg, readableClass);
	_Thing_addOneReadableClass (readableClass);
	ClassInfo klas;
	while ((klas = va_arg (arg, ClassInfo)) != nullptr) {
		_Thing_addOneReadableClass (klas);
	}
	va_end (arg);
}

long Thing_listReadableClasses () {
	Melder_clearInfo ();
	MelderInfo_open ();
	for (long iclass = 1; iclass <= theNumberOfReadableClasses; iclass ++) {
		ClassInfo klas = theReadableClasses [iclass];
		MelderInfo_writeLine (klas -> sequentialUniqueIdOfReadableClass, U"\t", klas -> className);
	}
	MelderInfo_close ();
	return theNumberOfReadableClasses;
}

static int theNumberOfAliases = 0;
static struct {
	ClassInfo readableClass;
	const char32 *otherName;
} theAliases [1 + 100];

void Thing_recognizeClassByOtherName (ClassInfo readableClass, const char32 *otherName) {
	theAliases [++ theNumberOfAliases]. readableClass = readableClass;
	theAliases [theNumberOfAliases]. otherName = otherName;
}

ClassInfo Thing_classFromClassName (const char32 *klas, int *p_formatVersion) {
	static char32 buffer [1+100];
	str32ncpy (buffer, klas ? klas : U"", 100);
	char32 *space = str32chr (buffer, U' ');
	if (space) {
		*space = U'\0';   // strip version number
		if (p_formatVersion) *p_formatVersion = Melder_atoi (space + 1);
	} else {
		if (p_formatVersion) *p_formatVersion = 0;
	}

	/*
	 * First try the class names that were registered with Thing_recognizeClassesByName.
	 */
	for (int i = 1; i <= theNumberOfReadableClasses; i ++) {
		ClassInfo classInfo = theReadableClasses [i];
		if (str32equ (buffer, classInfo -> className)) {
			return classInfo;
		}
	}

	/*
	 * Then try the aliases that were registered with Thing_recognizeClassByOtherName.
	 */
	for (int i = 1; i <= theNumberOfAliases; i ++) {
		if (str32equ (buffer, theAliases [i]. otherName)) {
			ClassInfo classInfo = theAliases [i]. readableClass;
			return classInfo;
		}
	}

	Melder_throw (U"Class \"", buffer, U"\" not recognized.");
}

autoThing Thing_newFromClassName (const char32 *className, int *p_formatVersion) {
	try {
		ClassInfo classInfo = Thing_classFromClassName (className, p_formatVersion);
		return Thing_newFromClass (classInfo);
	} catch (MelderError) {
		Melder_throw (className, U" not created.");
	}
}

Thing _Thing_dummyObject (ClassInfo classInfo) {
	if (! classInfo -> dummyObject) {
		classInfo -> dummyObject = classInfo -> _new ();
	}
	Melder_assert (classInfo -> dummyObject);
	return classInfo -> dummyObject;
}

void _Thing_forget_nozero (Thing me) {
	if (! me) return;
	if (Melder_debug == 40) Melder_casual (U"destroying ", my classInfo -> className);
	//Melder_casual (U"_Thing_forget_nozero before");
	my v_destroy ();
	//Melder_casual (U"_Thing_forget_nozero after");
	theTotalNumberOfThings -= 1;
}

void _Thing_forget (Thing me) {
	if (! me) return;
	if (Melder_debug == 40) Melder_casual (U"destroying ", my classInfo -> className);
	my v_destroy ();
	trace (U"destroying ", my classInfo -> className);
	//Melder_free (me);
	delete me;
	theTotalNumberOfThings -= 1;
}

bool Thing_isSubclass (ClassInfo klas, ClassInfo ancestor) {
	while (klas != ancestor && klas) klas = klas -> semanticParent;
	return !! klas;
}

bool Thing_isa (Thing me, ClassInfo klas) {
	if (! me) Melder_fatal (U"(Thing_isa:) Found null object.");
	return Thing_isSubclass (my classInfo, klas);
}

void * _Thing_check (Thing me, ClassInfo klas, const char *fileName, int line) {
	if (! me)
		Melder_fatal (U"(_Thing_check:)"
			U" null object passed to a function\n"
			U"in file ", Melder_peek8to32 (fileName),
			U" at line ", line,
			U"."
		);
	ClassInfo classInfo = my classInfo;
	while (classInfo != klas && classInfo) classInfo = classInfo -> semanticParent;
	if (! classInfo)
		Melder_fatal (U"(_Thing_check:)"
			U" Object of wrong class (", my classInfo -> className,
			U") passed to a function\n"
			U"in file ", Melder_peek8to32 (fileName),
			U" at line ", line,
			U"."
		);
	return me;
}

void Thing_infoWithIdAndFile (Thing me, long id, MelderFile file) {
	//Melder_assert (me);
	Melder_clearInfo ();
	MelderInfo_open ();
	if (id != 0) MelderInfo_writeLine (U"Object id: ", id);
	if (! MelderFile_isNull (file)) MelderInfo_writeLine (U"Associated file: ", Melder_fileToPath (file));
	my v_info ();
	MelderInfo_close ();
}

void Thing_info (Thing me) {
	Thing_infoWithIdAndFile (me, 0, nullptr);
}

const char32 * Thing_getName (Thing me) { return my name; }

const char32 * Thing_messageName (Thing me) {
	static MelderString buffers [19] { };
	static int ibuffer = 0;
	if (++ ibuffer == 19) ibuffer = 0;
	if (my name) {
		MelderString_copy (& buffers [ibuffer], my classInfo -> className, U" \"", my name, U"\"");
	} else {
		MelderString_copy (& buffers [ibuffer], my classInfo -> className);
	}
	return buffers [ibuffer]. string;
}

void Thing_setName (Thing me, const char32 *name /* cattable */) {
	/*
	 * First check without change.
	 */
	autostring32 newName = Melder_dup_f (name);   // BUG: that's no checking
	/*
	 * Then change without error.
	 */
	Melder_free (my name);
	my name = newName.transfer();
	my v_nameChanged ();
}

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
