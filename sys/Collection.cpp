/* Collection.cpp
 *
 * Copyright (C) 1992-2012,2015 Paul Boersma
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

#include "Collection.h"
#include <string>

/********** class Collection **********/

void structCollection :: v_destroy () {
	if (our item) {
		if (our _ownItems) {
			for (long i = 1; i <= our size; i ++) {
				forget (our item [i]);
			}
		}
		our item ++;   // base 1
		Melder_free (our item);
	}
	Collection_Parent :: v_destroy ();
}

void structCollection :: v_info () {
	MelderInfo_writeLine (size, U" items");
}

void structCollection :: v_copy (Daata thee_Daata) {
	Collection thee = static_cast <Collection> (thee_Daata);
	thy item = nullptr;   // set to null in case the inherited v_copy crashes
	Collection_Parent :: v_copy (thee);
	thy itemClass = our itemClass;
	thy _ownershipInitialized = our _ownershipInitialized;
	thy _ownItems = our _ownItems;
	thy _capacity = our _capacity;
	thy size = our size;
	thy item = Melder_calloc (Thing, our _capacity);   // filled with null pointers
	thy item --;   // immediately turn from base-0 into base-1  // BUG use NUMvector
	for (long i = 1; i <= our size; i ++) {
		Thing itempie = our item [i];
		if (our _ownItems) {
			if (! Thing_isa (itempie, classDaata))
				Melder_throw (U"Cannot copy item of class ", Thing_className (itempie), U".");
			thy item [i] = Data_copy (static_cast <Daata> (itempie));
		} else {
			thy item [i] = itempie;   // reference copy: if me doesn't own the items, then thee shouldn't either   // NOTE: the items don't have to be Daata
		}
	}
}

bool structCollection :: v_equal (Daata thee_Daata) {
	Collection thee = static_cast <Collection> (thee_Daata);
	if (! Collection_Parent :: v_equal (thee)) return false;
	if (size != thy size) return false;
	for (long i = 1; i <= size; i ++) {
		if (! Thing_isa (item [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (item [i]), U".");
		if (! Thing_isa (thy item [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (thy item [i]), U".");
		bool equal = Data_equal (static_cast <Daata> (item [i]), static_cast <Daata> (thy item [i]));
		//Melder_casual (U"classCollection_equal: ", equal,
		//	U", item ", i,
		//  U", types ", Thing_className (my item [i]), U" and ", Thing_className (thy item [i]));
		if (! equal) return false;
	}
	return true;
}

bool structCollection :: v_canWriteAsEncoding (int encoding) {
	for (long i = 1; i <= size; i ++) {
		Daata data = (Daata) item [i];
		if (data -> name != nullptr && ! Melder_isEncodable (data -> name, encoding)) return false;
		if (! Data_canWriteAsEncoding (data, encoding)) return false;
	}
	return true;
}

void structCollection :: v_writeText (MelderFile file) {
	texputi4 (file, size, U"size", 0,0,0,0,0);
	texputintro (file, U"item []: ", size ? nullptr : U"(empty)", 0,0,0,0);
	for (long i = 1; i <= size; i ++) {
		Thing thing = item [i];
		ClassInfo classInfo = thing -> classInfo;
		texputintro (file, U"item [", Melder_integer (i), U"]:", 0,0,0);
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteText ((Daata) thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		texputw2 (file,
			classInfo -> version > 0 ?
				Melder_cat (classInfo -> className, U" ", classInfo -> version) :
				classInfo -> className,
			U"class", 0,0,0,0,0);
		texputw2 (file, thing -> name, U"name", 0,0,0,0,0);
		Data_writeText ((Daata) thing, file);
		texexdent (file);
	}
	texexdent (file);
}

void structCollection :: v_readText (MelderReadText text, int formatVersion) {
	if (formatVersion < 0) {
		long l_size;
		autostring8 line = Melder_32to8 (MelderReadText_readLine (text));
		if (! line.peek() || ! sscanf (line.peek(), "%ld", & l_size) || l_size < 0)
			Melder_throw (U"Collection::readText: cannot read size.");
		Collection_init (this, nullptr, l_size);
		for (long i = 1; i <= l_size; i ++) {
			long itemNumberRead;
			int n = 0, length, stringsRead;
			char klas [200], nameTag [2000];
			do {
				line.reset (Melder_32to8 (MelderReadText_readLine (text)));
				if (! line.peek())
					Melder_throw (U"Missing object line.");
			} while (strncmp (line.peek(), "Object ", 7));
			stringsRead = sscanf (line.peek(), "Object %ld: class %s %s%n", & itemNumberRead, klas, nameTag, & n);
			if (stringsRead < 2)
				Melder_throw (U"Collection::readText: cannot read header of object ", i, U".");
			if (itemNumberRead != i)
				Melder_throw (U"Collection::readText: read item number ", itemNumberRead,
					U" while expecting ", i, U".");
			if (stringsRead == 3 && ! strequ (nameTag, "name"))
				Melder_throw (U"Collection::readText: wrong header at object ", i, U".");
			our item [i] = Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).transfer();
			our size ++;
			if (! Thing_isa ((Thing) our item [i], classDaata) || ! Data_canReadText ((Daata) our item [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className ((Thing) our item [i]), U" in collection.");
			Data_readText ((Daata) our item [i], text, -1);
			if (stringsRead == 3) {
				if (line [n] == U' ') n ++;   // skip space character
				length = strlen (line.peek()+n);
				if (length > 0 && (line.peek()+n) [length - 1] == '\n')
					(line.peek()+n) [length - 1] = '\0';
				Thing_setName ((Thing) item [i], Melder_peek8to32 (line.peek()+n));
			}
		}
	} else {
		int32_t l_size = texgeti4 (text);
		Collection_init (this, nullptr, l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring32 className = texgetw2 (text);
			int elementFormatVersion;
			our item [i] = Thing_newFromClassName (className.peek(), & elementFormatVersion).transfer();
			our size ++;
			if (! Thing_isa ((Thing) our item [i], classDaata) || ! Data_canReadText ((Daata) our item [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className ((Thing) our item [i]), U" in collection.");
			autostring32 objectName = texgetw2 (text);
			Thing_setName ((Thing) our item [i], objectName.peek());
			Data_readText ((Daata) our item [i], text, elementFormatVersion);
		}
	}
}

void structCollection :: v_writeBinary (FILE *f) {
	binputi4 (our size, f);
	for (long i = 1; i <= our size; i ++) {
		Thing thing = our item [i];
		ClassInfo classInfo = thing -> classInfo;
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteBinary ((Daata) thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		binputw1 (classInfo -> version > 0 ?
			Melder_cat (classInfo -> className, U" ", classInfo -> version) : classInfo -> className, f);
		binputw2 (thing -> name, f);
		Data_writeBinary ((Daata) thing, f);
	}
}

void structCollection :: v_readBinary (FILE *f, int formatVersion) {
	if (formatVersion < 0) {
		int32 l_size = bingeti4 (f);
		if (l_size < 0)
			Melder_throw (U"Empty collection.");
		Collection_init (this, nullptr, l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			char klas [200], name [2000];
			if (fscanf (f, "%s%s", klas, name) < 2)   // BUG
				Melder_throw (U"Cannot read class and name.");
			our item [i] = Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).transfer();
			our size ++;
			if (! Thing_isa (our item [i], classDaata))
				Melder_throw (U"Cannot read item of class ", Thing_className ((Thing) our item [i]), U".");
			if (fgetc (f) != ' ')
				Melder_throw (U"Cannot read space.");
			Data_readBinary ((Daata) our item [i], f, -1);
			if (strcmp (name, "?"))
				Thing_setName (our item [i], Melder_peek8to32 (name));
		}
	} else {
		int32_t l_size = bingeti4 (f);
		if (Melder_debug == 44)
			Melder_casual (U"structCollection :: v_readBinary: Reading ", l_size, U" objects");
		Collection_init (this, nullptr, l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring8 klas = bingets1 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object of type ", Melder_peek8to32 (klas.peek()));
			int elementFormatVersion;
			our item [i] = Thing_newFromClassName (Melder_peek8to32 (klas.peek()), & elementFormatVersion).transfer();
			our size ++;
			if (! Thing_isa (our item [i], classDaata) || ! Data_canReadBinary ((Daata) our item [i]))
				Melder_throw (U"Objects of class ", Thing_className ((Thing) our item [i]), U" cannot be read.");
			autostring32 name = bingetw2 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object with name ", name.peek());
			Thing_setName (our item [i], name.peek());
			Data_readBinary ((Daata) our item [i], f, elementFormatVersion);
		}
	}
}

static struct structData_Description theCollection_description [] = {
	{ U"size", longwa, Melder_offsetof (Collection, size), sizeof (long) },
	{ U"item", objectwa, Melder_offsetof (Collection, item), sizeof (Daata), U"Daata", & theClassInfo_Daata, 1, 0, U"size" },
	{ 0 }
};
Data_Description structCollection :: s_description = & theCollection_description [0];

Thing_implement (Collection, Daata, 0);

void Collection_init (Collection me, ClassInfo itemClass_, long initialCapacity) {
	my itemClass = itemClass_;
	my _ownershipInitialized = false;
	my _ownItems = true;
	my _capacity = initialCapacity >= 1 ? initialCapacity : 1;
	my size = 0;
	my item = Melder_calloc (Thing, my _capacity);
	my item --;   // base 1
}

Collection Collection_create (ClassInfo itemClass, long initialCapacity) {
	autoCollection me = Thing_new (Collection);
	Collection_init (me.peek(), itemClass, initialCapacity);
	return me.transfer();
}

void Collection_dontOwnItems (Collection me) {
	Melder_assert (my size == 0);
	my _ownItems = false;
	my _ownershipInitialized = true;
}

static inline void _Collection_initializeOwnership (Collection me, bool ownItems) {
	if (my _ownershipInitialized) {
		Melder_assert (my _ownItems == ownItems);
	} else {
		my _ownItems = ownItems;
		my _ownershipInitialized = true;
	}
}

void _Collection_insertItem (Collection me, Thing data, long pos) {
	my _ownershipInitialized = true;
	if (my size >= my _capacity) {
		Thing *dum = (Thing *) Melder_realloc (my item + 1, 2 * my _capacity * (int64) sizeof (Thing));
		my item = dum - 1;
		my _capacity *= 2;
	}
	my size ++;
	for (long i = my size; i > pos; i --) my item [i] = my item [i - 1];
	my item [pos] = data;
}

void _Collection_insertItem_move (Collection me, autoThing data, long pos) {
	_Collection_initializeOwnership (me, true);
	if (my size >= my _capacity) {
		Thing *dum = (Thing *) Melder_realloc (my item + 1, 2 * my _capacity * (int64) sizeof (Thing));
		my item = dum - 1;
		my _capacity *= 2;
	}
	my size ++;
	for (long i = my size; i > pos; i --) my item [i] = my item [i - 1];
	my item [pos] = data.transfer();
}

void _Collection_insertItem_ref (Collection me, Thing data, long pos) {
	_Collection_initializeOwnership (me, false);
	if (my size >= my _capacity) {
		Thing *dum = (Thing *) Melder_realloc (my item + 1, 2 * my _capacity * (int64) sizeof (Thing));
		my item = dum - 1;
		my _capacity *= 2;
	}
	my size ++;
	for (long i = my size; i > pos; i --) my item [i] = my item [i - 1];
	my item [pos] = data;
}

void Collection_addItem (Collection me, Thing data) {
	try {
		Melder_assert (data);
		long index = my v_position (data);
		if (index != 0) {
			_Collection_insertItem (me, data, index);
		} else {
			if (my _ownItems)
				forget (data);   // could not insert; I am the owner, so I must dispose of the data
		}
	} catch (MelderError) {
		Melder_throw (me, U": item not added.");
	}
}

void Collection_addItem_move (Collection me, autoThing data) {
	try {
		Melder_assert (data);
		long index = my v_position (data.get());
		if (index != 0) {
			_Collection_insertItem_move (me, data.move(), index);
		} else {
			_Collection_initializeOwnership (me, true);
			data.reset();   // could not insert; I am the owner, so I must dispose of the data
		}
	} catch (MelderError) {
		Melder_throw (me, U": item not added.");
	}
}

void Collection_addItem_ref (Collection me, Thing data) {
	try {
		Melder_assert (data);
		long index = my v_position (data);
		if (index != 0) {
			_Collection_insertItem_ref (me, data, index);
		} else {
			_Collection_initializeOwnership (me, false);
		}
	} catch (MelderError) {
		Melder_throw (me, U": item not added.");
	}
}

void Collection_removeItem (Collection me, long pos) {
	Melder_assert (pos >= 1 && pos <= my size);
	if (my _ownItems) forget (my item [pos]);
	for (long i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
}

void Collection_undangleItem (Collection me, Thing item) {
	for (long i = my size; i > 0; i --) if (my item [i] == item) {
		for (long j = i; j < my size; j ++) my item [j] = my item [j + 1];
		my size --;
	}
}

Thing Collection_subtractItem (Collection me, long pos) {
	Melder_assert (pos >= 1 && pos <= my size);
	Thing result = my item [pos];
	for (long i = pos; i < my size; i ++) my item [i] = my item [i + 1];
	my size --;
	return result;
}

void Collection_removeAllItems (Collection me) {
	if (my _ownItems)
		for (long i = 1; i <= my size; i ++)
			forget (my item [i]);
	my size = 0;
}

void Collection_shrinkToFit (Collection me) {
	my _capacity = my size ? my size : 1;
	my item = (Thing *) Melder_realloc (my item + 1, my _capacity * (int64) sizeof (Thing)) - 1;
}

Collection Collections_merge (Collection me, Collection thee) {
	try {
		if (my classInfo != thy classInfo)
			Melder_throw (U"Objects are of different class.");
		if (my _ownItems != thy _ownItems)
			Melder_throw (U"Cannot mix data and references.");
		autoCollection him = Data_copy (me);
		for (long i = 1; i <= thy size; i ++) {
			Thing item = thy item [i];
			if (my _ownItems) {
				if (! Thing_isa (item, classDaata))
					Melder_throw (U"Cannot copy item of class ", Thing_className (item), U".");
				Collection_addItem (him.peek(), Data_copy ((Daata) item));
			} else {
				Collection_addItem (him.peek(), item);
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not merged." );
	}
}

void Collection_sort (Collection me, Collection_ItemCompareHook compareHook) {
	int (*compare) (Daata, Daata) = compareHook.get();
	long l, r, j, i;
	Daata k;
	Daata *a = (Daata *) my item;
	long n = my size;
	if (n < 2) return;
	l = (n >> 1) + 1;
	r = n;
	for (;;) {
		if (l > 1) {
			l --;
			k = a [l];
		} else { 
			k = a [r];
			a [r] = a [1];
			r --;
			if (r == 1) { a [1] = k; return; }
		}
		j = l;
		for (;;) {
			i = j;
			j = j << 1;
			if (j > r) break;
			if (j < r && compare (a [j], a [j + 1]) < 0) j ++;
			if (compare (k, a [j]) >= 0) break;
			a [i] = a [j];
		}
		a [i] = k;
	}
}

/********** class Ordered **********/

Thing_implement (Ordered, Collection, 0);

void Ordered_init (Ordered me, ClassInfo itemClass, long initialMaximumLength) {
	Collection_init (me, itemClass, initialMaximumLength);
}

Ordered Ordered_create () {
	autoOrdered me = Thing_new (Ordered);
	Ordered_init (me.peek(), nullptr, 10);
	return me.transfer();
}

void Ordered_addItemPos (Ordered me, Thing data, long position) {
	Melder_assert (data);
	if (position < 1 || position > my size)
		position = my size + 1;
	_Collection_insertItem (me, data, position);
}

/********** class Sorted **********/

Thing_implement (Sorted, Collection, 0);

long structSorted :: v_position (Thing data) {
	Data_CompareHook::FunctionType compare = v_getCompareHook ().get();
	if (size == 0 || compare ((Daata) data, (Daata) item [size]) >= 0) return size + 1;
	if (compare ((Daata) data, (Daata) item [1]) < 0) return 1;
	/* Binary search. */
	long left = 1, right = size;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (compare ((Daata) data, (Daata) item [mid]) >= 0) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return right;
}

int structSorted :: s_compareHook (Daata /* data1 */, Daata /* data2 */) noexcept {
	return 0;   // in the base class, all are equal
}

void Sorted_init (Sorted me, ClassInfo itemClass, long initialCapacity) {
	Collection_init (me, itemClass, initialCapacity);
}

void Sorted_addItem_unsorted (Sorted me, Thing data) {
	_Collection_insertItem (me, data, my size + 1);
}

void Sorted_sort (Sorted me) {
	Collection_sort (me, my v_getCompareHook ());
}

/********** class SortedSet **********/

long structSortedSet :: v_position (Thing data) {
	Data_CompareHook::FunctionType compare = v_getCompareHook ().get();
	if (size == 0) return 1;   // empty set? then 'data' is going to be the first item
	int where = compare ((Daata) data, (Daata) item [size]);   // compare with last item
	if (where > 0) return size + 1;   // insert at end
	if (where == 0) return 0;
	if (compare ((Daata) data, (Daata) item [1]) < 0) return 1;   // compare with first item
	long left = 1, right =size;
	while (left < right - 1) {
		long mid = (left + right) / 2;
		if (compare ((Daata) data, (Daata) item [mid]) >= 0)
			left = mid;
		else
			right = mid;
	}
	Melder_assert (right == left + 1);
	if (! compare ((Daata) data, (Daata) item [left]) || ! compare ((Daata) data, (Daata) item [right]))
		return 0;
	return right;
}

Thing_implement (SortedSet, Sorted, 0);

void SortedSet_init (SortedSet me, ClassInfo itemClass, long initialCapacity) {
	Sorted_init (me, itemClass, initialCapacity);
}

/********** class SortedSetOfInt **********/

Thing_implement (SortedSetOfInt, SortedSet, 0);

int structSortedSetOfInt :: s_compareHook (SimpleInt me, SimpleInt thee) noexcept {
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

void SortedSetOfInt_init (SortedSetOfInt me) {
	SortedSet_init (me, classSimpleInt, 10);
}

SortedSetOfInt SortedSetOfInt_create () {
	autoSortedSetOfInt me = Thing_new (SortedSetOfInt);
	SortedSetOfInt_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfLong **********/

Thing_implement (SortedSetOfLong, SortedSet, 0);

int structSortedSetOfLong :: s_compareHook (SimpleLong me, SimpleLong thee) noexcept {
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

void SortedSetOfLong_init (SortedSetOfLong me) {
	SortedSet_init (me, classSimpleLong, 10);
}

SortedSetOfLong SortedSetOfLong_create () {
	autoSortedSetOfLong me = Thing_new (SortedSetOfLong);
	SortedSetOfLong_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfDouble **********/

int structSortedSetOfDouble :: s_compareHook (SimpleDouble me, SimpleDouble thee) noexcept {
	if (my number < thy number) return -1;
	if (my number > thy number) return +1;
	return 0;
}

Thing_implement (SortedSetOfDouble, SortedSet, 0);

void SortedSetOfDouble_init (SortedSetOfDouble me) {
	SortedSet_init (me, classSimpleDouble, 10);
}

SortedSetOfDouble SortedSetOfDouble_create () {
	autoSortedSetOfDouble me = Thing_new (SortedSetOfDouble);
	SortedSetOfDouble_init (me.peek());
	return me.transfer();
}

/********** class SortedSetOfString **********/

Thing_implement (SortedSetOfString, SortedSet, 0);

void SortedSetOfString_init (SortedSetOfString me) {
	SortedSet_init (me, classSimpleString, 10);
}

SortedSetOfString SortedSetOfString_create () {
	autoSortedSetOfString me = Thing_new (SortedSetOfString);
	SortedSetOfString_init (me.peek());
	return me.transfer();
}

long SortedSetOfString_lookUp (SortedSetOfString me, const char32 *string) {
	SimpleString *items = (SimpleString *) my item;
	long numberOfItems = my size;
	long left = 1, right = numberOfItems;
	int atStart, atEnd;
	if (numberOfItems == 0) return 0;

	atEnd = str32cmp (string, items [numberOfItems] -> string);
	if (atEnd > 0) return 0;
	if (atEnd == 0) return numberOfItems;

	atStart = str32cmp (string, items [1] -> string);
	if (atStart < 0) return 0;
	if (atStart == 0) return 1;

	while (left < right - 1) {
		long mid = (left + right) / 2;
		int here = str32cmp (string, items [mid] -> string);
		if (here == 0) return mid;
		if (here > 0) left = mid; else right = mid;
	}
	Melder_assert (right == left + 1);
	return 0;
}

void SortedSetOfString_addString (SortedSetOfString me, const char32 *string) {
	static autoSimpleString simp;
	if (! simp) {
		simp = SimpleString_create (U"");
		Melder_free (simp -> string);
	}
	simp -> string = (char32 *) string;   // reference copy
	long index = my v_position (simp.get());
	simp -> string = nullptr;   // otherwise Praat will crash at shutdown
	if (index == 0) return;   // OK: already there: do not add
	autoSimpleString newSimp = SimpleString_create (string);
	_Collection_insertItem (me, newSimp.transfer(), index);
}

/********** class Cyclic **********/

Thing_implement (Cyclic, Collection, 0);

int structCyclic :: s_compareHook (Daata /* me */, Daata /* thee */) noexcept {
	Melder_fatal (U"Cyclic::compare: subclass responsibility.");
	return 0;
}

void Cyclic_cycleLeft (Cyclic me) {
	if (my size == 0) return;   // for size == 1 no motion will take place either, but in that case the algorithm determines that automatically
	Daata help = (Daata) my item [1];
	for (long i = 1; i < my size; i ++) my item [i] = my item [i + 1];
	my item [my size] = help;
}

void Cyclic_unicize (Cyclic me) {
	Data_CompareHook compare = my v_getCompareHook ();
	if (my size <= 1) return;
	long lowest = 1;
	for (long i = 1; i <= my size; i ++)
		if (compare ((Daata) my item [i], (Daata) my item [lowest]) < 0) lowest = i;
	for (long i = 1; i < lowest; i ++)
		Cyclic_cycleLeft (me);
}

void Cyclic_init (Cyclic me, ClassInfo itemClass, long initialCapacity) {
	Collection_init (me, itemClass, initialCapacity);
}

/* End of file Collection.cpp */
