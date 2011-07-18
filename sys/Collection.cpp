/* Collection.cpp
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
 * pb 2004/10/16 C++ compatible structs
 * pb 2006/08/08 reduced compiler warnings
 * pb 2006/12/17 better info
 * pb 2007/06/24 wchar_t
 * pb 2007/08/08 canWriteAsEncoding
 * pb 2007/10/01 make sure that names are encodable when writing
 * pb 2008/03/19 removed SortedSetOfFloat
 * pb 2008/07/20 wchar_t
 * pb 2010/07/28 tiny corrections (like a memory leak if out of memory...)
 * pb 2011/03/23 Collection_dontOwnItems
 * pb 2011/04/06 C++
 * pb 2011/07/14 C++
 */

#include "Collection.h"

/********** class Collection **********/

static void classCollection_destroy (I) {
	iam (Collection);
	if (my item != NULL) {
		if (! my _dontOwnItems) {
			for (long i = 1; i <= my size; i ++) {
				forget (my item [i]);
			}
		}
		my item ++;   // base 1
		Melder_free (my item);
	}
	inherited (Collection) destroy (me);
}

void structCollection :: v_info ()
{
	MelderInfo_writeLine2 (Melder_integer (size), L" items");
}

static void classCollection_copy (I, thou) {
	iam (Collection); thouart (Collection);
	thy item = NULL;   // kill shallow copy of item
	inherited (Collection) copy (me, thee); therror
	thy itemClass = my itemClass;
	thy _capacity = my _capacity;
	thy size = my size;
	thy item = Melder_calloc (void *, my _capacity);   // filled with NULL
	thy item --;   // immediately turn from base-0 into base-1
	for (long i = 1; i <= my size; i ++) {
		Thing item = (Thing) my item [i];
		if (my _dontOwnItems) {
			thy item [i] = item;   // reference copy: if me doesn't own the items, then thee shouldn't either   // NOTE: the items don't have to be Data
		} else {
			if (! Thing_member (item, classData))
				Melder_throw ("Cannot copy item of class ", Thing_className (item), ".");
			thy item [i] = Data_copy (item); therror
		}
	}
}

static bool classCollection_equal (I, thou) {
	iam (Collection); thouart (Collection);
	if (! inherited (Collection) equal (me, thee)) return false;
	if (my size != thy size) return false;
	for (long i = 1; i <= my size; i ++) {
		if (! Thing_member ((Thing) my item [i], classData))
			Melder_throw ("Collection::equal: "
				"cannot compare items of class ", Thing_className ((Thing) my item [i]), ".");
		if (! Thing_member ((Thing) thy item [i], classData))
			Melder_throw ("Collection::equal: "
				"cannot compare items of class ", Thing_className ((Thing) thy item [i]), ".");
		bool equal = Data_equal (my item [i], thy item [i]);
		//Melder_casual ("classCollection_equal: %d, items %ld, types %ls and %ls",
		//	equal, i, Thing_className (my item [i]), Thing_className (thy item [i]));
		if (! equal) return false;
	}
	return true;
}

static bool classCollection_canWriteAsEncoding (I, int encoding) {
	iam (Collection);
	for (long i = 1; i <= my size; i ++) {
		Thing thing = (Thing) my item [i];
		if (thing -> name != NULL && ! Melder_isEncodable (thing -> name, encoding)) return false;
		if (! Data_canWriteAsEncoding (thing, encoding)) return false;
	}
	return true;
}

static void classCollection_writeText (I, MelderFile file) {
	iam (Collection);
	texputi4 (file, my size, L"size", 0,0,0,0,0);
	texputintro (file, L"item []: ", my size ? NULL : L"(empty)", 0,0,0,0);
	for (long i = 1; i <= my size; i ++) {
		Thing thing = (Thing) my item [i];
		Thing_Table table = thing -> methods;
		texputintro (file, L"item [", Melder_integer (i), L"]:", 0,0,0);
		if (! Thing_member (thing, classData) || ! Data_canWriteText (thing))
			Melder_throw ("Objects of class ", table -> _className, " cannot be written.");
		texputw2 (file, table -> version > 0 ? Melder_wcscat3 (table -> _className, L" ", Melder_integer (table -> version)) : table -> _className, L"class", 0,0,0,0,0);
		texputw2 (file, thing -> name, L"name", 0,0,0,0,0);
		Data_writeText (thing, file);
		texexdent (file);
	}
	texexdent (file);
}

static void classCollection_readText (I, MelderReadText text) {
	iam (Collection);
	if (Thing_version < 0) {
		long size;
		wchar_t *line = MelderReadText_readLine (text);
		if (line == NULL || ! swscanf (line, L"%ld", & size) || size < 0)
			Melder_throw ("Collection::readText: cannot read size.");
		Collection_init (me, NULL, size);
		for (long i = 1; i <= size; i ++) {
			long itemNumberRead;
			int n = 0, length, stringsRead;
			char klas [200], nameTag [2000];
			do { line = MelderReadText_readLine (text); if (line == NULL) Melder_throw ("Missing object line."); }
			while (wcsncmp (line, L"Object ", 7));
			stringsRead = swscanf (line, L"Object %ld: class %s %s%n", & itemNumberRead, klas, nameTag, & n);
			if (stringsRead < 2)
				Melder_throw ("Collection::readText: cannot read header of object ", i, ".");
			if (itemNumberRead != i)
				Melder_throw ("Collection::readText: read item number ", itemNumberRead,
					" while expecting ", i, ".");
			if (stringsRead == 3 && ! strequ (nameTag, "name"))
				Melder_throw ("Collection::readText: wrong header at object ", i, ".");
			my item [i] = Thing_newFromClassNameA (klas); therror
			Thing_version = -1;   /* Override. */
			my size ++;
			if (! Thing_member ((Thing) my item [i], classData) || ! Data_canReadText (my item [i]))
				Melder_throw ("Cannot read item of class ", Thing_className ((Thing) my item [i]), " in collection.");
			Data_readText (my item [i], text); therror
			if (stringsRead == 3) {
				if (line [n] == ' ') n ++;   // skip space character
				length = wcslen (line+n);
				if (length > 0 && (line+n) [length - 1] == '\n') (line+n) [length - 1] = '\0';
				Thing_setName ((Thing) my item [i], line+n);
			}
		}
		return;
	}
	long size = texgeti4 (text);
	Collection_init (me, NULL, size);
	for (long i = 1; i <= size; i ++) {
		long saveVersion = Thing_version;   /* The version of the Collection... */
		autostring8 className = texgets2 (text);
		my item [i] = Thing_newFromClassNameA (className.peek()); therror
		my size ++;
		if (! Thing_member ((Thing) my item [i], classData) || ! Data_canReadText (my item [i]))
			Melder_throw ("Cannot read item of class ", Thing_className ((Thing) my item [i]), " in collection.");
		autostring objectName = texgetw2 (text);
		Thing_setName ((Thing) my item [i], objectName.peek()); therror
		Data_readText (my item [i], text); therror
		Thing_version = saveVersion;
	}
}

static void classCollection_writeBinary (I, FILE *f) {
	iam (Collection);
	binputi4 (my size, f);
	for (long i = 1; i <= my size; i ++) {
		Thing thing = (Thing) my item [i];
		Thing_Table table = thing -> methods;
		if (! Thing_member (thing, classData) || ! Data_canWriteBinary (thing))
			Melder_throw ("Objects of class ", table -> _className, L" cannot be written.");
		binputw1 (table -> version > 0 ?
			Melder_wcscat3 (table -> _className, L" ", Melder_integer (table -> version)) : table -> _className, f);
		binputw2 (thing -> name, f);
		Data_writeBinary (thing, f);
	}
}

static void classCollection_readBinary (I, FILE *f) {
	iam (Collection);
	if (Thing_version < 0) {
		long size = bingeti4 (f); therror
		if (size < 0)
			Melder_throw ("Empty collection.");
		Collection_init (me, NULL, size); therror
		for (long i = 1; i <= size; i ++) {
			char klas [200], name [2000];
			if (fscanf (f, "%s%s", klas, name) < 2)
				Melder_throw ("Cannot read class and name.");
			my item [i] = Thing_newFromClassNameA (klas); therror
			Thing_version = -1;   /* Override. */
			my size ++;
			if (! Thing_member ((Thing) my item [i], classData))
				Melder_throw ("Cannot read item of class ", Thing_className ((Thing) my item [i]), ".");
			if (fgetc (f) != ' ')
				Melder_throw ("Cannot read space.");
			Data_readBinary (my item [i], f); therror
			if (strcmp (name, "?")) Thing_setName ((Thing) my item [i], Melder_peekUtf8ToWcs (name));
		}
	} else {
		long size = bingeti4 (f); therror
		Collection_init (me, NULL, size); therror
		for (long i = 1; i <= size; i ++) {
			long saveVersion = Thing_version;   // the version of the Collection...
			autostring8 klas = bingets1 (f);
			my item [i] = Thing_newFromClassNameA (klas.peek()); therror
			my size ++;
			if (! Thing_member ((Thing) my item [i], classData) || ! Data_canReadBinary (my item [i]))
				Melder_throw ("Objects of class ", Thing_className ((Thing) my item [i]), " cannot be read.");
			autostring name = bingetw2 (f);
			Thing_setName ((Thing) my item [i], name.peek()); therror
			Data_readBinary (my item [i], f); therror
			Thing_version = saveVersion;
		}
	}
}

static struct structData_Description classCollection_description [] = {
	{ L"size", longwa, (char *) & ((Collection) & Melder_debug) -> size - (char *) & Melder_debug, sizeof (long) },
	{ L"item", objectwa, (char *) & ((Collection) & Melder_debug) -> item - (char *) & Melder_debug, sizeof (Data), L"Data", & theStructData, 1, 0, L"my size" },
	{ 0 } };

static long classCollection_position (I, Any data) {
	iam (Collection);
	(void) data;
	return my size + 1;
}

class_methods (Collection, Data) {
	class_method_local (Collection, destroy)
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
}

void Collection_init (I, void *itemClass, long initialCapacity) {
	iam (Collection);
	my itemClass = itemClass;
	my _capacity = initialCapacity >= 1 ? initialCapacity : 1;
	my size = 0;
	my item = Melder_calloc (void *, my _capacity);
	my item --;   // base 1
}

Collection Collection_create (void *itemClass, long initialCapacity) {
	autoCollection me = Thing_new (Collection);
	Collection_init (me.peek(), itemClass, initialCapacity);
	return me.transfer();
}

void Collection_dontOwnItems (I) {
	iam (Collection);
	Melder_assert (my size == 0);
	my _dontOwnItems = true;
}

void _Collection_insertItem (I, Any data, long pos) {
	iam (Collection);
	if (my size >= my _capacity) {
		/*
		 * Check without change.
		 */
		Any *dum = (Any *) Melder_realloc (my item + 1, 2 * my _capacity * sizeof (Any));
		/*
		 * From here: change without error.
		 */
		my item = dum - 1;
		my _capacity *= 2;
	}
	my size ++;
	for (long i = my size; i > pos; i --) my item [i] = my item [i - 1];
	my item [pos] = data;
}

#undef our
#define our ((Sorted_Table) my methods) ->  // BUG

void Collection_addItem (I, Any data) {
	iam (Collection);
	try {
		Melder_assert (data != NULL);
		long index = our position (me, data);
		if (index != 0) {
			_Collection_insertItem (me, data, index);
		} else {
			if (! my _dontOwnItems)
				forget (data);   // could not insert; I am the owner, so I must dispose of the data
		}
	} catch (MelderError) {
		Melder_throw (me, ": item not added.");
	}
}

void Collection_removeItem (I, long pos) {
	iam (Collection);
	Melder_assert (pos >= 1 && pos <= my size);
	if (! my _dontOwnItems) forget (my item [pos]);
	for (long i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
}

void Collection_undangleItem (I, Any item) {
	iam (Collection);
	for (long i = my size; i > 0; i --) if (my item [i] == item) {
		for (long j = i; j < my size; j ++) my item [j] = my item [j + 1];
		my size --;
	}
}

Any Collection_subtractItem (I, long pos) {
	iam (Collection);
	Melder_assert (pos >= 1 && pos <= my size);
	Any result = my item [pos];
	for (long i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
	return result;
}

void Collection_removeAllItems (I) {
	iam (Collection);
	if (! my _dontOwnItems)
		for (long i = 1; i <= my size; i ++)
			forget (my item [i]);
	my size = 0;
}

void Collection_shrinkToFit (I) {
	iam (Collection);
	my _capacity = my size ? my size : 1;
	my item = (Any *) Melder_realloc (my item + 1, my _capacity * sizeof (Any)) - 1;
}

Any Collections_merge (I, thou) {
	iam (Collection); thouart (Collection);
	try {
		if (my methods != thy methods)
			Melder_throw ("Objects are of different class.");
		if (my _dontOwnItems != thy _dontOwnItems)
			Melder_throw ("Cannot mix data and references.");
		autoCollection him = (Collection) Data_copy (me);
		for (long i = 1; i <= thy size; i ++) {
			Thing item = (Thing) thy item [i];
			if (my _dontOwnItems) {
				Collection_addItem (him.peek(), item);
			} else {
				if (! Thing_member (item, classData))
					Melder_throw ("Cannot copy item of class ", Thing_className (item), ".");
				Collection_addItem (him.peek(), Data_copy (item));
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not merged." );
	}
}

/********** class Ordered **********/

class_methods (Ordered, Collection) {
	class_methods_end
}

void Ordered_init (I, void *itemClass, long initialMaximumLength) {
	iam (Ordered);
	Collection_init (me, itemClass, initialMaximumLength);
}

Ordered Ordered_create (void) {
	autoOrdered me = Thing_new (Ordered);
	Ordered_init (me.peek(), NULL, 10);
	return me.transfer();
}

void Ordered_addItemPos (I, Any data, long position) {
	iam (Ordered);
	Melder_assert (data);
	if (position < 1 || position > my size)
		position = my size + 1;
	_Collection_insertItem (me, data, position);
}

/********** class Sorted **********/

static long classSorted_position (I, Any data) {
	iam (Sorted);
	if (my size == 0 || our compare (data, my item [my size]) >= 0) return my size + 1;
	if (our compare (data, my item [1]) < 0) return 1;
	/* Binary search. */
	long left = 1, right = my size;
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

class_methods (Sorted, Collection) {
	class_method_local (Sorted, position)
	class_method_local (Sorted, compare)
	class_methods_end
}

void Sorted_init (I, void *itemClass, long initialCapacity) {
	iam (Sorted);
	Collection_init (me, itemClass, initialCapacity);
}

void Sorted_addItem_unsorted (I, Any data) {
	iam (Sorted);
	_Collection_insertItem (me, data, my size + 1);
}

void Sorted_sort (I) {
	iam (Sorted);
	NUMsort_p (my size, my item, (int (*) (const void *, const void *)) our compare);
}

/********** class SortedSet **********/

static long classSortedSet_position (I, Any data) {
	iam (SortedSet);
	if (my size == 0) return 1;   /* Empty set? 'data' is going to be the first item. */
	int where = our compare (data, my item [my size]);   /* Compare with last item. */
	if (where > 0) return my size + 1;   /* Insert at end. */
	if (where == 0) return 0;
	if (our compare (data, my item [1]) < 0) return 1;   /* Compare with first item. */
	long left = 1, right = my size;
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

class_methods (SortedSet, Sorted) {
	class_method_local (SortedSet, position)
	class_methods_end
}

void SortedSet_init (Any me, void *itemClass, long initialCapacity) {
	Sorted_init (me, itemClass, initialCapacity);
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

class_methods (SortedSetOfInt, SortedSet) {
	class_method_local (SortedSetOfInt, compare)
	class_methods_end
}

void SortedSetOfInt_init (I) {
	iam (SortedSetOfInt);
	SortedSet_init (me, classSimpleInt, 10);
}

SortedSetOfInt SortedSetOfInt_create (void) {
	autoSortedSetOfInt me = Thing_new (SortedSetOfInt);
	SortedSetOfInt_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfShort **********/

static int classSortedSetOfShort_compare (I, thou) {
	iam (SimpleShort); thouart (SimpleShort);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfShort, SortedSet) {
	class_method_local (SortedSetOfShort, compare)
	class_methods_end
}

void SortedSetOfShort_init (I) {
	iam (SortedSetOfShort);
	SortedSet_init (me, classSimpleShort, 10);
}

SortedSetOfShort SortedSetOfShort_create (void) {
	autoSortedSetOfShort me = Thing_new (SortedSetOfShort);
	SortedSetOfShort_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfLong **********/

static int classSortedSetOfLong_compare (I, thou) {
	iam (SimpleLong); thouart (SimpleLong);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfLong, SortedSet) {
	class_method_local (SortedSetOfLong, compare)
	class_methods_end
}

void SortedSetOfLong_init (I) {
	iam (SortedSetOfLong);
	SortedSet_init (me, classSimpleLong, 10);
}

SortedSetOfLong SortedSetOfLong_create (void) {
	autoSortedSetOfLong me = Thing_new (SortedSetOfLong);
	SortedSetOfLong_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfDouble **********/

static int classSortedSetOfDouble_compare (I, thou) {
	iam (SimpleDouble); thouart (SimpleDouble);
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

class_methods (SortedSetOfDouble, SortedSet) {
	class_method_local (SortedSetOfDouble, compare)
	class_methods_end
}

void SortedSetOfDouble_init (I) {
	iam (SortedSetOfDouble);
	SortedSet_init (me, classSimpleDouble, 10);
}

SortedSetOfDouble SortedSetOfDouble_create (void) {
	autoSortedSetOfDouble me = Thing_new (SortedSetOfDouble);
	SortedSetOfDouble_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfString **********/

static int classSortedSetOfString_compare (I, thou) {
	iam (SimpleString); thouart (SimpleString);
	return wcscmp (my string, thy string);
}

class_methods (SortedSetOfString, SortedSet) {
	class_method_local (SortedSetOfString, compare)
	class_methods_end
}

void SortedSetOfString_init (I) {
	iam (SortedSetOfString);
	SortedSet_init (me, classSimpleString, 10);
}

SortedSetOfString SortedSetOfString_create (void) {
	autoSortedSetOfString me = Thing_new (SortedSetOfString);
	SortedSetOfString_init (me.peek());
	return me.transfer();
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

void SortedSetOfString_add (SortedSetOfString me, const wchar *string) {
	static SimpleString simp;
	if (simp == NULL) {
		simp = SimpleString_create (L"");
		Melder_free (simp -> string);
	}
	simp -> string = (wchar *) string;
	long index = our position (me, simp);
	if (index == 0) return;   // OK: already there: do not add
	autoSimpleString newSimp = SimpleString_create (string);
	_Collection_insertItem (me, newSimp.transfer(), index);
}

/********** class Cyclic **********/

static int classCyclic_compare (I, thou) {
	(void) void_me;
	(void) void_thee;
	Melder_fatal ("Cyclic::compare: subclass responsibility.");
	return 0;
}

class_methods (Cyclic, Collection) {
	class_method_local (Cyclic, compare)
	class_methods_end
}

void Cyclic_init (Any me, void *itemClass, long initialCapacity) {
	Collection_init (me, itemClass, initialCapacity);
}

static void cycleLeft (I) {
	iam (Cyclic);
	if (my size == 0) return;
	Data help = (Data) my item [1];
	for (long i = 1; i < my size; i ++) my item [i] = my item [i + 1];
	my item [my size] = help;
}

void Cyclic_unicize (I) {
	iam (Cyclic);
	if (my size <= 1) return;
	long lowest = 1;
	for (long i = 1; i <= my size; i ++)
		if (our compare (my item [i], my item [lowest]) < 0) lowest = i;
	for (long i = 1; i < lowest; i ++)
		cycleLeft (me);
}

/* End of file Collection.cpp */
