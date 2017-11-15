/* Collection.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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

#include "Collection.h"
#include <string>

/********** class Collection **********/

void _CollectionOfDaata_v_copy (_CollectionOfDaata* me, _CollectionOfDaata* thee) {
	thy at._elements = nullptr;   // set to null in case the inherited v_copy crashes
	my structDaata :: v_copy (thee);
	thy _ownershipInitialized = my _ownershipInitialized;
	thy _ownItems = my _ownItems;
	thy _capacity = my _capacity;
	thy size = my size;
	if (my _capacity > 0) {
		thy at._elements = Melder_calloc (Daata, my _capacity);   // filled with null pointers
		thy at._elements --;   // immediately turn from base-0 into base-1  // BUG use NUMvector
	}
	for (integer i = 1; i <= my size; i ++) {
		Daata item = my at [i];
		if (my _ownItems) {
			if (! Thing_isa (item, classDaata))
				Melder_throw (U"Cannot copy item of class ", Thing_className (item), U".");
			thy at [i] = Data_copy (item).releaseToAmbiguousOwner();
		} else {
			thy at [i] = item;   // reference copy: if me doesn't own the items, then thee shouldn't either   // NOTE: the items don't have to be Daata
		}
	}
}

bool _CollectionOfDaata_v_equal (_CollectionOfDaata* me, _CollectionOfDaata* thee) {
	if (! my structDaata :: v_equal (thee)) return false;
	if (my size != thy size) return false;
	for (integer i = 1; i <= my size; i ++) {
		if (! Thing_isa (my at [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (my at [i]), U".");
		if (! Thing_isa (thy at [i], classDaata))
			Melder_throw (U"Collection::equal: "
				U"cannot compare items of class ", Thing_className (thy at [i]), U".");
		bool equal = Data_equal (my at [i], thy at [i]);
		//Melder_casual (U"classCollection_equal: ", equal,
		//	U", item ", i,
		//  U", types ", Thing_className (my item [i]), U" and ", Thing_className (thy item [i]));
		if (! equal) return false;
	}
	return true;
}

bool _CollectionOfDaata_v_canWriteAsEncoding (_CollectionOfDaata* me, int encoding) {
	for (integer i = 1; i <= my size; i ++) {
		Daata data = my at [i];
		if (data -> name && ! Melder_isEncodable (data -> name, encoding)) return false;
		if (! Data_canWriteAsEncoding (data, encoding)) return false;
	}
	return true;
}

void _CollectionOfDaata_v_writeText (_CollectionOfDaata* me, MelderFile file) {
	texputi32 (file, my size, U"size", 0,0,0,0,0);
	texputintro (file, U"item []: ", my size ? nullptr : U"(empty)", 0,0,0,0);
	for (integer i = 1; i <= my size; i ++) {
		Daata thing = my at [i];
		ClassInfo classInfo = thing -> classInfo;
		texputintro (file, U"item [", Melder_integer (i), U"]:", 0,0,0);
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteText (thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		texputw16 (file,
			classInfo -> version > 0 ?
				Melder_cat (classInfo -> className, U" ", classInfo -> version) :
				classInfo -> className,
			U"class", 0,0,0,0,0);
		texputw16 (file, thing -> name, U"name", 0,0,0,0,0);
		Data_writeText (thing, file);
		texexdent (file);
	}
	texexdent (file);
}

void _CollectionOfDaata_v_readText (_CollectionOfDaata* me, MelderReadText text, int formatVersion) {
	if (formatVersion < 0) {
		long_not_integer l_size;
		autostring8 line = Melder_32to8 (MelderReadText_readLine (text));
		if (! line.peek() || ! sscanf (line.peek(), "%ld", & l_size) || l_size < 0)
			Melder_throw (U"Collection::readText: cannot read size.");
		my _grow (l_size);
		for (integer i = 1; i <= l_size; i ++) {
			long_not_integer itemNumberRead;
			int n = 0, length, stringsRead;
			char klas [200], nameTag [2000];
			do {
				line.reset (Melder_32to8 (MelderReadText_readLine (text)));
				if (! line.peek())
					Melder_throw (U"Missing object line.");
			} while (strncmp (line.peek(), "Object ", 7));
			stringsRead = sscanf (line.peek(), "Object %ld: class %199s %1999s%n", & itemNumberRead, klas, nameTag, & n);
			if (stringsRead < 2)
				Melder_throw (U"Collection::readText: cannot read header of object ", i, U".");
			if (itemNumberRead != i)
				Melder_throw (U"Collection::readText: read item number ", itemNumberRead,
					U" while expecting ", i, U".");
			if (stringsRead == 3 && ! strequ (nameTag, "name"))
				Melder_throw (U"Collection::readText: wrong header at object ", i, U".");
			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).releaseToAmbiguousOwner();
			my size ++;
			if (! Thing_isa (my at [i], classDaata) || ! Data_canReadText (my at [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className (my at [i]), U" in collection.");
			Data_readText (my at [i], text, -1);
			if (stringsRead == 3) {
				if (line [n] == U' ') n ++;   // skip space character
				length = strlen (line.peek()+n);
				if (length > 0 && (line.peek()+n) [length - 1] == '\n')
					(line.peek()+n) [length - 1] = '\0';
				Thing_setName (my at [i], Melder_peek8to32 (line.peek()+n));
			}
		}
	} else {
		int32_t l_size = texgeti32 (text);
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring32 className = texgetw16 (text);
			int elementFormatVersion;
			my at [i] = (Daata) Thing_newFromClassName (className.peek(), & elementFormatVersion).releaseToAmbiguousOwner();
			my size ++;
			if (! Thing_isa (my at [i], classDaata) || ! Data_canReadText (my at [i]))
				Melder_throw (U"Cannot read item of class ", Thing_className (my at [i]), U" in collection.");
			autostring32 objectName = texgetw16 (text);
			Thing_setName (my at [i], objectName.peek());
			Data_readText (my at [i], text, elementFormatVersion);
		}
	}
}

void _CollectionOfDaata_v_writeBinary (_CollectionOfDaata* me, FILE *f) {
	binputi32 (my size, f);
	for (integer i = 1; i <= my size; i ++) {
		Daata thing = my at [i];
		ClassInfo classInfo = thing -> classInfo;
		if (! Thing_isa (thing, classDaata) || ! Data_canWriteBinary (thing))
			Melder_throw (U"Objects of class ", classInfo -> className, U" cannot be written.");
		binputw8 (classInfo -> version > 0 ?
			Melder_cat (classInfo -> className, U" ", classInfo -> version) : classInfo -> className, f);
		binputw16 (thing -> name, f);
		Data_writeBinary ((Daata) thing, f);
	}
}

void _CollectionOfDaata_v_readBinary (_CollectionOfDaata* me, FILE *f, int formatVersion) {
	if (formatVersion < 0) {
		int32 l_size = bingeti32 (f);
		if (l_size < 0)
			Melder_throw (U"Empty collection.");
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			char klas [200], name [2000];
			if (fscanf (f, "%199s%1999s", klas, name) < 2)
				Melder_throw (U"Cannot read class and name.");
			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas), nullptr).releaseToAmbiguousOwner();
			my size ++;
			if (! Thing_isa (my at [i], classDaata))
				Melder_throw (U"Cannot read item of class ", Thing_className (my at [i]), U".");
			if (fgetc (f) != ' ')
				Melder_throw (U"Cannot read space.");
			Data_readBinary (my at [i], f, -1);
			if (strcmp (name, "?"))
				Thing_setName (my at [i], Melder_peek8to32 (name));
		}
	} else {
		int32_t l_size = bingeti32 (f);
		if (Melder_debug == 44)
			Melder_casual (U"structCollection :: v_readBinary: Reading ", l_size, U" objects");
		my _grow (l_size);
		for (int32_t i = 1; i <= l_size; i ++) {
			autostring8 klas = bingets8 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object of type ", Melder_peek8to32 (klas.peek()));
			int elementFormatVersion;
			my at [i] = (Daata) Thing_newFromClassName (Melder_peek8to32 (klas.peek()), & elementFormatVersion).releaseToAmbiguousOwner();
			my size ++;
			if (! Thing_isa (my at [i], classDaata) || ! Data_canReadBinary (my at [i]))
				Melder_throw (U"Objects of class ", Thing_className (my at [i]), U" cannot be read.");
			autostring32 name = bingetw16 (f);
			if (Melder_debug == 44)
				Melder_casual (U"structCollection :: v_readBinary: Reading object with name ", name.peek());
			Thing_setName (my at [i], name.peek());
			Data_readBinary (my at [i], f, elementFormatVersion);
		}
	}
}

struct structData_Description theCollectionOfDaata_v_description [] = {
	{ U"size", integerwa, Melder_offsetof (CollectionOf<structThing>*, size), sizeof (integer), nullptr, nullptr, 0, nullptr, nullptr, nullptr, nullptr },
	{ U"items", objectwa, Melder_offsetof (CollectionOf<structThing>*, at), sizeof (Daata), U"Daata", & theClassInfo_Daata, 1, nullptr, U"size", nullptr, nullptr },
	{ }
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

Thing_implement (StringList, Ordered, 0);
Thing_implement (StringSet, SortedSet, 0);

/* End of file Collection.cpp */
