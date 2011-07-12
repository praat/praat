#ifndef _Collection_h_
#define _Collection_h_
/* Collection.h
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
 * pb 2011/07/11
 */

/* Collections contain a number of items whose class is a subclass of Data.

	class Collection = Data {
		void *itemClass;   // The class of which all items must be members (see Thing_member).
		long size;
		Any item [1..size];
	introduce:
		long position (I, Any data);
	override:
		void destroy (I);   // Destroys all the items.
		int copy (I, thou);   // Copies all the items.
		int equal (I, thou);   // Compares 'my item [i]' with 'thy item [i]', i = 1..size.
	};
	class Ordered = Collection {
	};
	class Sorted = Collection {
	introduce:
		int compare (I, thou);   // Compares the keys of two items;
				// returns negative if me < thee, 0 if me == thee, and positive if me > thee.
	};
	class SortedSet = Sorted {   // Every item must be unique (by key).
		override long position (I, Any data);   // Returns 0 (refusal) if the key of 'data' already occurs.
	};
	class Cyclic = Collection;   // The cyclic list (a, b, c, d) equals (b, c, d, a) but not (d, c, a, b).
*/

#include "Simple.h"

#ifdef __cplusplus
	extern "C" {
#endif

Thing_declare1cpp (Collection);
Thing_declare1cpp (Ordered);
Thing_declare1cpp (Sorted);
Thing_declare1cpp (SortedSet);
Thing_declare1cpp (SortedSetOfInt);
Thing_declare1cpp (SortedSetOfShort);
Thing_declare1cpp (SortedSetOfLong);
Thing_declare1cpp (SortedSetOfDouble);
Thing_declare1cpp (SortedSetOfString);
Thing_declare1cpp (Cyclic);

/*
	An object of type Collection is a collection of items of any class.
	It is the owner of its items.
	You can access the items in the collection as item [1] through item [size].
	There can be no NULL items.

	Attributes:
		_capacity >= size		// private; grows as you add items.
		size			// the current number of items.
		item [1..size]		// the items.
*/

void Collection_init (I, void *itemClass, long initialCapacity);
Collection Collection_create (void *itemClass, long initialCapacity);
/*
	Function:
		return a new empty Collection, or NULL if out of memory.
	Preconditions:
		initialCapacity >= 1;
	Postconditions:
		my _capacity == initialCapacity;
*/

void Collection_dontOwnItems (I);

/*
	Data_copy, Data_equal, Data_writeXXX, Data_readXXX
	try to copy, compare, write, or read all the items.
	However, if any of the items is not of class Data,
	these routines fail with a message and return 0.
*/

void Collection_addItem (I, Any item);
/*
	Function:
		add the 'item' to the collection. Return 0 if out of memory, else 1.
	Preconditions:
 		item != NULL;
	Postconditions if result == 1:
		my size >= my old size + 1;
		if (my size > my old _capacity) my _capacity == 2 * my old _capacity;
	When calling this function, you transfer ownership of 'item' to the Collection, unless dontOwnItems is on.
	For a SortedSet, this may mean that the Collection immediately disposes of 'item',
	if that item already occurred in the Collection.
*/

void Collection_removeItem (I, long position);
/*
	Function:
		remove the item at 'position' from the collection and from memory.
	Preconditions:
		1 <= position <= my size;
	Postconditions:
		my size == my old size - 1;
		my _capacity not changed;
*/

void Collection_undangleItem (I, Any item);
/*
	Function:
		remove the item from the collection, without destroying it.
	Postconditions:
		item not found || my size == my old size - 1;
	Usage:
		this is the way in which an item can detach itself from a list;
		often used just before the item is destroyed, hence the name of this procedure.
*/

Any Collection_subtractItem (I, long position);
/*
	Function:
		remove the item at 'position' from the collection and transfer ownership to the caller.
	Return value:
		the subtracted item; the caller is responsible for eventually removing it.
	Preconditions:
		1 <= position <= my size;
	Postconditions:
		my size == my old size - 1;
		my _capacity not changed;
*/

void Collection_removeAllItems (I);
/*
	Function:
		remove all items from the collection and from memory.
	Postconditions:
		my size == 0;
		my _capacity not changed;
*/

void Collection_shrinkToFit (I);
/*
	Function:
		release as much memory as possible without affecting the items.
	Postconditions:
		my _capacity == max (my size, 1);
*/

Any Collections_merge (I, thou);
/*
	Function:
		merge two Collections into a new one.
	Postconditions:
		result -> size >= my size;
		result -> size >= thy size;
*/

/* For the inheritors. */

void _Collection_insertItem (I, Any item, long position);

/* Methods:

	static long position (I, Any data, long hint);
		Question asked by Collection_addItem: return a position for the data.
	Collection::position always returns my size + 1 (add item at the end).

*/

/********** class Ordered **********/

Ordered Ordered_create (void);
void Ordered_init (I, void *itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (Ordered) inserts an item at the end.
*/

void Ordered_addItemPos (I, Any data, long position);
/*
	Function:
		insert an item at 'position'.
		If 'position' is less than 1 or greater than the current 'size',
		insert the item at the end.
*/

/********** class Sorted **********/
/* A Sorted is a sorted Collection. */

void Sorted_init (I, void *itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (Sorted) inserts an item at such a position that the collection stays sorted.
	Collections_merge (Sorted) yields a Sorted.
*/

/***** Two routines for optimization. ******/
/* If you want to add a large group of items,
	it is best to call Sorted_addItem_unsorted () repeatedly,
	and finish with Sorted_sort (); this uses the fast 'heapsort' algorithm.
	Calling Collection_addItem () repeatedly would be slower,
	because on the average half the collection is moved in memory
	with every insertion.
*/

void Sorted_addItem_unsorted (I, Any data);
/*
	Function:
		add an item to the collection, quickly at the end.
	Warning:
		this leaves the collection unsorted; follow by Sorted_sort ().
*/
void Sorted_sort (I);
/* Call this after a number of calls to Sorted_addItem_unsorted (). */
/* The procedure used is 'heapsort'. */

/********** class SortedSet **********/

void SortedSet_init (I, void *itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (SortedSet) refuses to insert an item if this item already occurs.
		Equality is there when the compare routine returns 0.
	Collections_merge (SortedSet) yields a SortedSet that is the union of the two sources.
*/

int SortedSet_hasItem (I, Any item);

/********** class SortedSetOfInt **********/

void SortedSetOfInt_init (I);
SortedSetOfInt SortedSetOfInt_create (void);

/********** class SortedSetOfShort **********/

void SortedSetOfShort_init (I);
SortedSetOfShort SortedSetOfShort_create (void);

/********** class SortedSetOfLong **********/

void SortedSetOfLong_init (I);
SortedSetOfLong SortedSetOfLong_create (void);

/********** class SortedSetOfDouble **********/

void SortedSetOfDouble_init (I);
SortedSetOfDouble SortedSetOfDouble_create (void);

/********** class SortedSetOfString **********/

void SortedSetOfString_init (I);
SortedSetOfString SortedSetOfString_create (void);
long SortedSetOfString_lookUp (SortedSetOfString me, const wchar_t *string);
void SortedSetOfString_add (SortedSetOfString me, const wchar_t *string);

/********** class Cyclic **********/

void Cyclic_init (I, void *itemClass, long initialCapacity);

void Cyclic_unicize (I);

#ifdef __cplusplus
	}

	struct structCollection : public structData {
		void *itemClass;
		long _capacity, size;
		bool _dontOwnItems;
		Any *item;
	};
	#define Collection__methods(klas) Data__methods(klas) \
		long (*position) (I, Any data);
	Thing_declare2cpp (Collection, Data);

	struct structOrdered : public structCollection {
	};
	#define Ordered__methods(klas) Collection__methods(klas)
	Thing_declare2cpp (Ordered, Collection);

	struct structSorted : public structCollection {
	};
	#define Sorted__methods(klas) Collection__methods(klas) \
		int (*compare) (I, thou);
	Thing_declare2cpp (Sorted, Collection);

	struct structSortedSet : public structSorted {
	};
	#define SortedSet__methods(klas) Sorted__methods(klas)
	Thing_declare2cpp (SortedSet, Sorted);

	struct structSortedSetOfInt : public structSortedSet {
	};
	#define SortedSetOfInt__methods(klas) SortedSet__methods(klas)
	Thing_declare2cpp (SortedSetOfInt, SortedSet);

	struct structSortedSetOfShort : public structSortedSet {
	};
	#define SortedSetOfShort__methods(klas) SortedSet__methods(klas)
	Thing_declare2cpp (SortedSetOfShort, SortedSet);

	struct structSortedSetOfLong : public structSortedSet {
	};
	#define SortedSetOfLong__methods(klas) SortedSet__methods(klas)
	Thing_declare2cpp (SortedSetOfLong, SortedSet);

	struct structSortedSetOfDouble : public structSortedSet {
	};
	#define SortedSetOfDouble__methods(klas) SortedSet__methods(klas)
	Thing_declare2cpp (SortedSetOfDouble, SortedSet);

	struct structSortedSetOfString : public structSortedSet {
	};
	#define SortedSetOfString__methods(klas) SortedSet__methods(klas)
	Thing_declare2cpp (SortedSetOfString, SortedSet);

	struct structCyclic : public structCollection {
	};
	#define Cyclic__methods(klas) Collection__methods(klas) \
		int (*compare) (I, thou);   /* virtual */
	Thing_declare2cpp (Cyclic, Collection);

#endif

/* End of file Collection.h */
#endif
