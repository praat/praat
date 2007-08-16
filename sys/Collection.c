/* Collection.c
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
 * pb 2004/10/16 C++ compatible structs
 * pb 2006/08/08 reduced compiler warnings
 * pb 2006/12/17 better info
 * pb 2007/06/24 wchar_t
 * pb 2007/08/08 canWriteAsEncoding
 */

#include "Collection.h"

/********** class Collection **********/

static void classCollection_destroy (I) {
	iam (Collection);
	if (my item) { long i; for (i = 1; i <= my size; i ++) forget (my item [i]); }
	if (my item) { my item ++; Melder_free (my item); }
	inherited (Collection) destroy (me);
}

static void classCollection_info (I) {
	iam (Collection);
	MelderInfo_writeLine2 (Melder_integer (my size), L" items");
}

static int classCollection_copy (I, thou) {
	iam (Collection); thouart (Collection);
	long i;
	thy item = NULL;   /* Kill shallow copy of item. */
	if (! inherited (Collection) copy (me, thee)) return 0;
	thy itemClass = my itemClass;
	thy _capacity = my _capacity;
	thy size = my size;
	if (! (thy item = Melder_calloc (void *, my _capacity))) return 0;   /* Filled with NULL. */
	thy item --;   /* Base 1. */
	for (i = 1; i <= my size; i ++) {   /* Try to copy the items themselves. */
		if (! Thing_member (my item [i], classData))
			return Melder_error ("Collection::copy: "
				"cannot copy item of class %s.", Thing_className (my item [i]));
		if (! (thy item [i] = Data_copy (my item [i]))) return 0;
		/* Copy the names of the items (but Data_copy does that). */
		/* if (! Thing_getName (thy item [i]))
			Thing_setName (thy item [i], Thing_getName (my item [i])); */
	}
	return 1;
}

static bool classCollection_equal (I, thou) {
	iam (Collection); thouart (Collection);
	long i;
	if (! inherited (Collection) equal (me, thee)) return 0;
	if (my size != thy size) return 0;
	for (i = 1; i <= my size; i ++) {
		if (! Thing_member (my item [i], classData))
			return Melder_error ("Collection::equal: "
				"cannot compare items of class %s.", Thing_className (my item [i]));
		if (! Thing_member (thy item [i], classData))
			return Melder_error ("Collection::equal: "
				"cannot compare items of class %s.", Thing_className (thy item [i]));
		if (! Data_equal (my item [i], thy item [i])) return 0;
	}
	return 1;
}

static bool classCollection_canWriteAsEncoding (I, int encoding) {
	iam (Collection);
	for (long i = 1; i <= my size; i ++) {
		if (! Data_canWriteAsEncoding (my item [i], encoding)) return false;
	}
	return true;
}

static int classCollection_writeText (I, MelderFile file) {
	iam (Collection);
	long i;
	texputi4 (file, my size, L"size", 0,0,0,0,0);
	texputintro (file, L"item []: ", my size ? NULL : L"(empty)", 0,0,0,0);
	for (i = 1; i <= my size; i ++) {
		Thing thing = my item [i];
		Thing_Table table = thing -> methods;
		texputintro (file, L"item [", Melder_integer (i), L"]:", 0,0,0);
		if (! Thing_member (thing, classData) || ! Data_canWriteText (thing))
			return Melder_error ("(Collection::writeText:) "
				"Objects of class %s cannot be written.", table -> _className);
		wchar_t className [100];
		if (table -> version)
			swprintf (className, 100, L"%ls %ld", table -> _classNameW, table -> version);
		else
			wcscpy (className, table -> _classNameW);
		texputw2 (file, className, L"class", 0,0,0,0,0);
		texputw2 (file, thing -> nameW, L"name", 0,0,0,0,0);
		if (! Data_writeText (thing, file)) return 0;
		texexdent (file);
	}
	texexdent (file);
	return 1;
}

static int classCollection_readText (I, MelderReadString *text) {
	iam (Collection);
	if (Thing_version < 0) {
		long i, size;
		wchar_t *line = MelderReadString_readLine (text);
		if (line == NULL || ! swscanf (line, L"%ld", & size) || size < 0)
			return Melder_error ("Collection::readText: cannot read size.");
		if (! Collection_init (me, NULL, size)) return 0;
		for (i = 1; i <= size; i ++) {
			long itemNumberRead;
			int n = 0, length, stringsRead;
			char klas [200], nameTag [2000];
			do { line = MelderReadString_readLine (text); if (line == NULL) return 0; }
			while (wcsncmp (line, L"Object ", 7));
			stringsRead = swscanf (line, L"Object %ld: class %s %s%n", & itemNumberRead, klas, nameTag, & n);
			if (stringsRead < 2)
				return Melder_error ("Collection::readText: cannot read header of object %ld.", i);
			if (itemNumberRead != i)
				return Melder_error ("Collection::readText: read item number %ld while expecting %ld.", itemNumberRead, i);
			if (stringsRead == 3 && ! strequ (nameTag, "name"))
				return Melder_error ("Collection::readText: wrong header at object %ld.", i);
			if (! (my item [i] = Thing_newFromClassName (klas))) return 0;
			Thing_version = -1;   /* Override. */
			my size ++;
			if (! Thing_member (my item [i], classData) || ! Data_canReadText (my item [i]))
				return Melder_error ("Collection::readText: "
					"cannot read item of class %s.", Thing_className (my item [i]));
			if (! Data_readText (my item [i], text)) return 0;
			if (stringsRead == 3) {
				if (line [n] == ' ') n ++;   /* Skip space character. */
				length = wcslen (line+n);
				if (length > 0 && (line+n) [length - 1] == '\n') (line+n) [length - 1] = '\0';
				Thing_setNameW (my item [i], line+n);
			}
		}
	} else {
		long i, size;
		size = texgeti4 (text);
		if (! Collection_init (me, NULL, size)) return 0;
		for (i = 1; i <= size; i ++) {
			long saveVersion = Thing_version;   /* The version of the Collection... */
			char *klas = texgets2 (text);
			if (klas == NULL) return 0;
			if (! (my item [i] = Thing_newFromClassName (klas))) return 0;
			Melder_free (klas);
			my size ++;
			if (! Thing_member (my item [i], classData) || ! Data_canReadText (my item [i]))
				return Melder_error ("(Collection::readText:) "
					"Cannot read item of class %s.", Thing_className (my item [i]));
			wchar_t *name = texgetw2 (text);
			Thing_setNameW (my item [i], name);
			Melder_free (name);
			if (! Data_readText (my item [i], text)) return 0;
			Thing_version = saveVersion;
		}
	}
	return 1;
}

static int classCollection_writeBinary (I, FILE *f) {
	iam (Collection);
	long i;
	binputi4 (my size, f);
	for (i = 1; i <= my size; i ++) {
		Thing thing = my item [i];
		Thing_Table table = thing -> methods;
		char className [100];
		if (table -> version)
			sprintf (className, "%s %ld", table -> _className, table -> version);
		else
			strcpy (className, table -> _className);
		if (! Thing_member (thing, classData) || ! Data_canWriteBinary (thing))
			return Melder_error ("(Collection::writeBinary:) "
				"Objects of class %s cannot be written.", table -> _className);
		binputs1 (className, f);
		binputw2 (thing -> nameW, f);
		if (! Data_writeBinary (thing, f)) return 0;
	}
	return 1;
}

static int classCollection_readBinary (I, FILE *f) {
	iam (Collection);
	if (Thing_version < 0) {
		long size = bingeti4 (f), i;
		if (size < 0 || ! Collection_init (me, NULL, size)) return 0;
		for (i = 1; i <= size; i ++) {
			char klas [200], name [2000];
			if (fscanf (f, "%s%s", klas, name) < 2 ||
				! (my item [i] = Thing_newFromClassName (klas))) return 0;
			Thing_version = -1;   /* Override. */
			my size ++;
			if (! Thing_member (my item [i], classData))
				return Melder_error ("Collection::readBinary: "
					"cannot read item of class %s.", Thing_className (my item [i]));
			if (fgetc (f) != ' ' || ! Data_readBinary (my item [i], f)) return 0;
			if (strcmp (name, "?")) Thing_setName (my item [i], name);
		}
	} else {
		long i, size;
		size = bingeti4 (f);
		if (! Collection_init (me, NULL, size)) return 0;
		for (i = 1; i <= size; i ++) {
			long saveVersion = Thing_version;   /* The version of the Collection... */
			char *klas = bingets1 (f);
			if (! (my item [i] = Thing_newFromClassName (klas))) return 0;
			Melder_free (klas);
			my size ++;
			if (! Thing_member (my item [i], classData) || ! Data_canReadBinary (my item [i]))
				return Melder_error ("(Collection::readBinary:) "
					"Cannot read item of class %s.", Thing_className (my item [i]));
			char *name = bingets2 (f);
			Thing_setName (my item [i], name);
			Melder_free (name);
			if (! Data_readBinary (my item [i], f)) return 0;
			Thing_version = saveVersion;
		}
	}
	return 1;
}

static struct structData_Description classCollection_description [] = {
	{ L"size", 4, (int) & ((Collection) 0) -> size, sizeof (long) },
	{ L"item", 23, (int) & ((Collection) 0) -> item, sizeof (Data), L"Data", & theStructData, 1, 0, L"my size" },
	{ 0 } };

static long classCollection_position (I, Any data) {
	iam (Collection);
	(void) data;
	return my size + 1;
}

class_methods (Collection, Data)
	class_method_local (Collection, destroy)
	class_method_local (Collection, info)
	class_method_local (Collection, copy)
	class_method_local (Collection, equal)
	class_method_local (Collection, canWriteAsEncoding)
	class_method_local (Collection, writeText)
	class_method_local (Collection, writeBinary)
	class_method_local (Collection, readText)
	class_method_local (Collection, readBinary)
	class_method_local (Collection, description)
	class_method_local (Collection, position)
class_methods_end

int Collection_init (I, void *itemClass, long initialCapacity) {
	iam (Collection);
	my itemClass = itemClass;
	my _capacity = initialCapacity >= 1 ? initialCapacity : 1;
	my size = 0;
	if (! (my item = Melder_calloc (void *, my _capacity))) return 0;
	my item --;   /* Base 1. */
	return 1;
}

Any Collection_create (void *itemClass, long initialCapacity) {
	Collection me = new (Collection);
	if (! me || ! Collection_init (me, itemClass, initialCapacity)) forget (me);
	return me;
}

int _Collection_insertItem (I, Any data, long pos) {
	iam (Collection);
	long i;
	if (my size >= my _capacity) {
		Any *dum = (Any *) Melder_realloc (my item + 1, 2 * my _capacity * sizeof (Any));
		if (! dum) return Melder_error ("Collection_insert: out of memory.");
		my item = dum - 1;
		my _capacity *= 2;
	}
	my size ++;
	for (i = my size; i > pos; i --) my item [i] = my item [i - 1];
	my item [pos] = data;
	return 1;
}

int Collection_addItem (I, Any data) {
	iam (Collection);
	long index;
	Melder_assert (data);
	index = our position (me, data);
	if (index) {
		return _Collection_insertItem (me, data, index);
	} else {
		forget (data);   /* Could not insert; I am owner, so I must dispose of the data!!! */
		return 1;   /* Refusal; all right. */
	}
}

void Collection_removeItem (I, long pos) {
	iam (Collection);
	long i;
	Melder_assert (pos >= 1 && pos <= my size);
	forget (my item [pos]);
	for (i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
}

void Collection_undangleItem (I, Any item) {
	iam (Collection);
	long i, j;
	for (i = my size; i > 0; i --) if (my item [i] == item) {
		for (j = i; j < my size; j ++) my item [j] = my item [j + 1];
		my size --;
	}
}

Any Collection_subtractItem (I, long pos) {
	iam (Collection);
	Any result;
	long i;
	Melder_assert (pos >= 1 && pos <= my size);
	result = my item [pos];
	for (i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
	return result;
}

void Collection_removeAllItems (I) {
	iam (Collection);
	long i;
	for (i = 1; i <= my size; i ++) forget (my item [i]);
	my size = 0;
}

void Collection_shrinkToFit (I) {
	iam (Collection);
	my _capacity = my size ? my size : 1;
	my item = (Any *) Melder_realloc (my item + 1, my _capacity * sizeof (Any)) - 1;
}

Any Collections_merge (I, thou) {
	iam (Collection); thouart (Collection);
	Collection him;
	long i;
	if (my methods != thy methods) return Melder_errorp ("(Collection_join:) "
		"Objects are of different class (%s and %s).",
		Thing_className (me), Thing_className (thee));
	if (! (him = Data_copy (me))) goto error;
	for (i = 1; i <= thy size; i ++) {
		Data tmp = Data_copy (thy item [i]);
		if (! tmp || ! Collection_addItem (him, tmp)) { forget (tmp); goto error; }
	}
	return him;
error:
	forget (him);
	return Melder_errorp ("(Collection_join:) Not performed." );
}

/********** class Ordered **********/

class_methods (Ordered, Collection)
class_methods_end

int Ordered_init (I, void *itemClass, long initialMaximumLength) {
	iam (Ordered);
	if (! Collection_init (me, itemClass, initialMaximumLength)) return 0;
	return 1;
}

Any Ordered_create (void) {
	Ordered me = new (Ordered);
	if (! me || ! Ordered_init (me, NULL, 10)) return NULL;
	return me;
}

int Ordered_addItemPos (I, Any data, long position) {
	iam (Ordered);
	Melder_assert (data);
	if (position < 1 || position > my size) position = my size + 1;
	return _Collection_insertItem (me, data, position);
}

/********** class Sorted **********/

static long classSorted_position (I, Any data) {
	iam (Sorted);
	long left, right;
	if (my size == 0 || our compare (data, my item [my size]) >= 0) return my size + 1;
	if (our compare (data, my item [1]) < 0) return 1;
	/* Binary search. */
	left = 1, right = my size;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (our compare (data, my item [mid]) >= 0) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return right;
}

static int classSorted_compare (Any data1, Any data2) {
	(void) data1;
	(void) data2;
	return 0;   /* All are equal. */
}

class_methods (Sorted, Collection)
	class_method_local (Sorted, position)
	class_method_local (Sorted, compare)
class_methods_end

int Sorted_init (I, void *itemClass, long initialCapacity) {
	iam (Sorted);
	if (! Collection_init (me, itemClass, initialCapacity)) return 0;
	return 1;
}

int Sorted_addItem_unsorted (I, Any data) {
	iam (Sorted);
	return _Collection_insertItem (me, data, my size + 1);
}

void Sorted_sort (I) {
	iam (Sorted);
	NUMsort_p (my size, my item, (int (*) (const void *, const void *)) our compare);
}

/********** class SortedSet **********/

static long classSortedSet_position (I, Any data) {
	iam (SortedSet);
	int where;
	long left = 1, right = my size;

	if (my size == 0) return 1;   /* Empty set? 'data' is going to be the first item. */
	where = our compare (data, my item [my size]);   /* Compare with last item. */
	if (where > 0) return my size + 1;   /* Insert at end. */
	if (where == 0) return 0;
	if (our compare (data, my item [1]) < 0) return 1;   /* Compare with first item. */
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (our compare (data, my item [mid]) >= 0)
			left = mid;
		else
			right = mid;
	}
	Melder_assert (right == left + 1);
	if (! our compare (data, my item [left]) || ! our compare (data, my item [right]))
		return 0;
	return right;
}

class_methods (SortedSet, Sorted)
	class_method_local (SortedSet, position)
class_methods_end

int SortedSet_init (Any me, void *itemClass, long initialCapacity) {
	return Sorted_init (me, itemClass, initialCapacity);
}

int SortedSet_hasItem (I, Any item) {
	iam (SortedSet);
	return our position (me, item) == 0;
}

/********** class SortedSetOfInt **********/

static int classSortedSetOfInt_compare (I, thou) {
	iam (SimpleInt); thouart (SimpleInt);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfInt, SortedSet)
	class_method_local (SortedSetOfInt, compare)
class_methods_end

int SortedSetOfInt_init (I) { iam (SortedSetOfInt); return SortedSet_init (me, classSimpleInt, 10); }

SortedSetOfInt SortedSetOfInt_create (void) {
	SortedSetOfInt me = new (SortedSetOfInt);
	if (! me || ! SortedSetOfInt_init (me)) { forget (me); return NULL; }
	return me;
}

/********** class SortedSetOfShort **********/

static int classSortedSetOfShort_compare (I, thou) {
	iam (SimpleShort); thouart (SimpleShort);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfShort, SortedSet)
	class_method_local (SortedSetOfShort, compare)
class_methods_end

int SortedSetOfShort_init (I) { iam (SortedSetOfShort); return SortedSet_init (me, classSimpleShort, 10); }

SortedSetOfShort SortedSetOfShort_create (void) {
	SortedSetOfShort me = new (SortedSetOfShort);
	if (! me || ! SortedSetOfShort_init (me)) { forget (me); return NULL; }
	return me;
}

/********** class SortedSetOfLong **********/

static int classSortedSetOfLong_compare (I, thou) {
	iam (SimpleLong); thouart (SimpleLong);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfLong, SortedSet)
	class_method_local (SortedSetOfLong, compare)
class_methods_end

int SortedSetOfLong_init (I) { iam (SortedSetOfLong); return SortedSet_init (me, classSimpleLong, 10); }

SortedSetOfLong SortedSetOfLong_create (void) {
	SortedSetOfLong me = new (SortedSetOfLong);
	if (! me || ! SortedSetOfLong_init (me)) { forget (me); return NULL; }
	return me;
}

/********** class SortedSetOfFloat **********/

static int classSortedSetOfFloat_compare (I, thou) {
	iam (SimpleFloat); thouart (SimpleFloat);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfFloat, SortedSet)
	class_method_local (SortedSetOfFloat, compare)
class_methods_end

int SortedSetOfFloat_init (I) { iam (SortedSetOfFloat); return SortedSet_init (me, classSimpleFloat, 10); }

SortedSetOfFloat SortedSetOfFloat_create (void) {
	SortedSetOfFloat me = new (SortedSetOfFloat);
	if (! me || ! SortedSetOfFloat_init (me)) { forget (me); return NULL; }
	return me;
}

/********** class SortedSetOfDouble **********/

static int classSortedSetOfDouble_compare (I, thou) {
	iam (SimpleDouble); thouart (SimpleDouble);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfDouble, SortedSet)
	class_method_local (SortedSetOfDouble, compare)
class_methods_end

int SortedSetOfDouble_init (I) { iam (SortedSetOfDouble); return SortedSet_init (me, classSimpleDouble, 10); }

SortedSetOfDouble SortedSetOfDouble_create (void) {
	SortedSetOfDouble me = new (SortedSetOfDouble);
	if (! me || ! SortedSetOfDouble_init (me)) { forget (me); return NULL; }
	return me;
}

/********** class SortedSetOfString **********/

static int classSortedSetOfString_compare (I, thou) {
	iam (SimpleString); thouart (SimpleString);
	return wcscmp (my string, thy string);
}

class_methods (SortedSetOfString, SortedSet)
	class_method_local (SortedSetOfString, compare)
class_methods_end

int SortedSetOfString_init (I) { iam (SortedSetOfString); return SortedSet_init (me, classSimpleString, 10); }

SortedSetOfString SortedSetOfString_create (void) {
	SortedSetOfString me = new (SortedSetOfString);
	if (! me || ! SortedSetOfString_init (me)) { forget (me); return NULL; }
	return me;
}

long SortedSetOfString_lookUp (SortedSetOfString me, const wchar_t *string) {
	SimpleString *items = (SimpleString *) my item;
	long numberOfItems = my size;
	long left = 1, right = numberOfItems;
	int atStart, atEnd;
	if (numberOfItems == 0) return 0;

	atEnd = wcscmp (string, items [numberOfItems] -> string);
	if (atEnd > 0) return 0;
	if (atEnd == 0) return numberOfItems;

	atStart = wcscmp (string, items [1] -> string);
	if (atStart < 0) return 0;
	if (atStart == 0) return 1;

	while (left < right - 1) {
		long mid = (left + right) / 2;
		int here = wcscmp (string, items [mid] -> string);
		if (here == 0) return mid;
		if (here > 0) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return 0;
}

int SortedSetOfString_add (SortedSetOfString me, const wchar_t *string) {
	static SimpleString simp;
	long index;
	SimpleString newSimp;
	if (! simp) { simp = SimpleString_create (L""); Melder_free (simp -> string); }
	simp -> string = (wchar_t *) string;
	if ((index = our position (me, simp)) == 0) return 1;   /* OK: already there: do not add. */
	newSimp = SimpleString_create (string);
	if (! newSimp || ! _Collection_insertItem (me, newSimp, index)) return 0;   /* Must be out of memory. */
	return 1;   /* OK: added new string. */
}

/********** class Cyclic **********/

static int classCyclic_compare (I, thou) {
	(void) void_me;
	(void) void_thee;
	Melder_fatal ("Cyclic::compare: subclass responsibility.");
	return 0;
}

class_methods (Cyclic, Collection)
	class_method_local (Cyclic, compare)
class_methods_end

int Cyclic_init (Any me, void *itemClass, long initialCapacity) {
	return Collection_init (me, itemClass, initialCapacity);
}

static void cycleLeft (I) {
	iam (Cyclic);
	Data help;
	long i;
	if (my size == 0) return;
	help = my item [1];
	for (i = 1; i < my size; i ++) my item [i] = my item [i + 1];
	my item [my size] = help;
}

void Cyclic_unicize (I) {
	iam (Cyclic);
	long lowest = 1, i;
	if (my size <= 1) return;
	for (i = 1; i <= my size; i ++)
		if (our compare (my item [i], my item [lowest]) < 0) lowest = i;
	for (i = 1; i < lowest; i ++)
		cycleLeft (me);
}

/* End of file Collection.c */
