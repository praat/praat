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

void _CollectionOfDaata_v_copy (_CollectionOfDaata* me, _CollectionOfDaata* thee) {
	thy _item = nullptr;   // set to null in case the inherited v_copy crashes
	my structDaata :: v_copy (thee);
	thy _ownershipInitialized = my _ownershipInitialized;
	thy _ownItems = my _ownItems;
	thy _capacity = my _capacity;
	thy _size = my _size;
	if (my _capacity > 0) {
		thy _item = Melder_calloc (Daata, my _capacity);   // filled with null pointers
		thy _item --;   // immediately turn from base-0 into base-1  // BUG use NUMvector
	}
	for (long i = 1; i <= my _size; i ++) {
		Daata itempie = my _item [i];
		if (my _ownItems) {
			if (! Thing_isa (itempie, classDaata))
				Melder_throw (U"Cannot copy item of class ", Thing_className (itempie), U".");
			thy _item [i] = Data_copy (itempie).releaseToAmbiguousOwner();
		} else {
			thy _item [i] = itempie;   // reference copy: if me doesn't own the items, then thee shouldn't either   // NOTE: the items don't have to be Daata
		}
	}
}

bool _CollectionOfDaata_v_equal (_CollectionOfDaata* me, _CollectionOfDaata* thee) {
	if (! my structDaata :: v_equal (thee)) return false;
	if (my _size != thy _size) return false;
	for (long i = 1; i <= my _size; i ++) {
		if (! Thing_isa (my _item [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (my _item [i]), U".");
		if (! Thing_isa (thy _item [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (thy _item [i]), U".");
		bool equal = Data_equal (my _item [i], thy _item [i]);
		//Melder_casual (U"classCollection_equal: ", equal,
		//	U", item ", i,
		//  U", types ", Thing_className (my item [i]), U" and ", Thing_className (thy item [i]));
		if (! equal) return false;
	}
	return true;
}

bool _CollectionOfDaata_v_canWriteAsEncoding (_CollectionOfDaata* me, int encoding) {
	for (long i = 1; i <= my _size; i ++) {
		Daata data = my _item [i];
		if (data -> name && ! Melder_isEncodable (data -> name, encoding)) return false;
		if (! Data_canWriteAsEncoding (data, encoding)) return false;
	}
	return true;
}

void _CollectionOfDaata_v_writeText (_CollectionOfDaata* me, MelderFile file) {
	texputi4 (file, my _size, U"size", 0,0,0,0,0);
	texputintro (file, U"item []: ", my _size ? nullptr : U"(empty)", 0,0,0,0);
	for (long i = 1; i <= my _size; i ++) {
		Daata thing = my _item [i];
		ClassInfo classInfo = thing -> classInfo;
		texputintro (file, U"item [", Melder_integer (i), U"]:", 0,0,0);
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteText (thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		texputw2 (file,
			classInfo -> version > 0 ?
				Melder_cat (classInfo -> className, U" ", classInfo -> version) :
				classInfo -> className,
			U"class", 0,0,0,0,0);
		texputw2 (file, thing -> name, U"name", 0,0,0,0,0);
		Data_writeText (thing, file);
		texexdent (file);
	}
	texexdent (file);
}

void _CollectionOfDaata_v_readText (_CollectionOfDaata* me, MelderReadText text, int formatVersion) {
	if (formatVersion < 0) {
		long l_size;
		autostring8 line = Melder_32to8 (MelderReadText_readLine (text));
		if (! line.peek() || ! sscanf (line.peek(), "%ld", & l_size) || l_size < 0)
			Melder_throw (U"Collection::readText: cannot read size.");
		my _grow (l_size);
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
			my _item [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).releaseToAmbiguousOwner();
			my _size ++;
			if (! Thing_isa (my _item [i], classDaata) || ! Data_canReadText (my _item [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className (my _item [i]), U" in collection.");
			Data_readText (my _item [i], text, -1);
			if (stringsRead == 3) {
				if (line [n] == U' ') n ++;   // skip space character
				length = strlen (line.peek()+n);
				if (length > 0 && (line.peek()+n) [length - 1] == '\n')
					(line.peek()+n) [length - 1] = '\0';
				Thing_setName (my _item [i], Melder_peek8to32 (line.peek()+n));
			}
		}
	} else {
		int32_t l_size = texgeti4 (text);
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring32 className = texgetw2 (text);
			int elementFormatVersion;
			my _item [i] = (Daata) Thing_newFromClassName (className.peek(), & elementFormatVersion).releaseToAmbiguousOwner();
			my _size ++;
			if (! Thing_isa (my _item [i], classDaata) || ! Data_canReadText (my _item [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className (my _item [i]), U" in collection.");
			autostring32 objectName = texgetw2 (text);
			Thing_setName (my _item [i], objectName.peek());
			Data_readText (my _item [i], text, elementFormatVersion);
		}
	}
}

void _CollectionOfDaata_v_writeBinary (_CollectionOfDaata* me, FILE *f) {
	binputi4 (my _size, f);
	for (long i = 1; i <= my _size; i ++) {
		Daata thing = my _item [i];
		ClassInfo classInfo = thing -> classInfo;
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteBinary (thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		binputw1 (classInfo -> version > 0 ?
			Melder_cat (classInfo -> className, U" ", classInfo -> version) : classInfo -> className, f);
		binputw2 (thing -> name, f);
		Data_writeBinary ((Daata) thing, f);
	}
}

void _CollectionOfDaata_v_readBinary (_CollectionOfDaata* me, FILE *f, int formatVersion) {
	if (formatVersion < 0) {
		int32 l_size = bingeti4 (f);
		if (l_size < 0)
			Melder_throw (U"Empty collection.");
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			char klas [200], name [2000];
			if (fscanf (f, "%s%s", klas, name) < 2)   // BUG
				Melder_throw (U"Cannot read class and name.");
			my _item [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).releaseToAmbiguousOwner();
			my _size ++;
			if (! Thing_isa (my _item [i], classDaata))
				Melder_throw (U"Cannot read item of class ", Thing_className (my _item [i]), U".");
			if (fgetc (f) != ' ')
				Melder_throw (U"Cannot read space.");
			Data_readBinary (my _item [i], f, -1);
			if (strcmp (name, "?"))
				Thing_setName (my _item [i], Melder_peek8to32 (name));
		}
	} else {
		int32_t l_size = bingeti4 (f);
		if (Melder_debug == 44)
			Melder_casual (U"structCollection :: v_readBinary: Reading ", l_size, U" objects");
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring8 klas = bingets1 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object of type ", Melder_peek8to32 (klas.peek()));
			int elementFormatVersion;
			my _item [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas.peek()), & elementFormatVersion).releaseToAmbiguousOwner();
			my _size ++;
			if (! Thing_isa (my _item [i], classDaata) || ! Data_canReadBinary (my _item [i]))
				Melder_throw (U"Objects of class ", Thing_className (my _item [i]), U" cannot be read.");
			autostring32 name = bingetw2 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object with name ", name.peek());
			Thing_setName (my _item [i], name.peek());
			Data_readBinary (my _item [i], f, elementFormatVersion);
		}
	}
}

struct structData_Description theCollectionOfDaata_v_description [] = {
	{ U"size", longwa, Melder_offsetof (CollectionOf<structThing>*, _size), sizeof (long) },
	{ U"item", objectwa, Melder_offsetof (CollectionOf<structThing>*, _item), sizeof (Daata), U"Daata", & theClassInfo_Daata, 1, 0, U"size" },
	{ 0 }
};

#define _Collection_implement(klas,genericClass,itemClass,parentClass,version) \
	static Thing _##klas##_new () { return new genericClass<struct##itemClass>; } \
	struct structClassInfo theClassInfo_##klas = { U"" #klas, & theClassInfo_##parentClass, \
		sizeof (genericClass<struct##itemClass>), _##klas##_new, version, 0, nullptr}; \
	ClassInfo class##klas = & theClassInfo_##klas

_Collection_implement (Collection, CollectionOf, Thing, Daata, 0);
_Collection_implement (Ordered, OrderedOf, Daata, Collection, 0);
_Collection_implement (Sorted, SortedOf, Daata, Collection, 0);
_Collection_implement (SortedSet, SortedSetOf, Daata, Sorted, 0);
_Collection_implement (SortedSetOfInt, SortedSetOfIntOf, SimpleInt, SortedSet, 0);
_Collection_implement (SortedSetOfLong, SortedSetOfLongOf, SimpleLong, SortedSet, 0);
_Collection_implement (SortedSetOfDouble, SortedSetOfDoubleOf, SimpleDouble, SortedSet, 0);
_Collection_implement (SortedSetOfString, SortedSetOfStringOf, SimpleString, SortedSet, 0);

/* End of file Collection.cpp */
