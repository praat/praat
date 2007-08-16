#ifndef _Collection_h_
#define _Collection_h_
/* Collection.h
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
 * pb 2007/08/12
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

#ifndef _Simple_h_
	#include "Simple.h"
#endif

#define Collection_members Data_members \
	void *itemClass; \
	long _capacity, size; \
	Any *item;
#define Collection_methods Data_methods \
	long (*position) (I, Any data);
class_create (Collection, Data);

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

int Collection_init (I, void *itemClass, long initialCapacity);
Any Collection_create (void *itemClass, long initialCapacity);
/*
	Function:
		return a new empty Collection, or NULL if out of memory.
	Preconditions:
		initialCapacity >= 1;
	Postconditions:
		my _capacity == initialCapacity;
*/

/*
	Data_copy, Data_equal, Data_writeXXX, Data_readXXX
	try to copy, compare, write, or read all the items.
	However, if any of the items is not of class Data,
	these routines fail with a message and return 0.
*/

int Collection_addItem (I, Any item);
/*
	Function:
		add the 'item' to the collection. Return 0 if out of memory, else 1.
	Preconditions:
 		item != NULL;
	Postconditions if result == 1:
		my size >= my old size + 1;
		if (my size > my old _capacity) my _capacity == 2 * my old _capacity;
	When calling this function, you transfer ownership of 'item' to the Collection.
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

int _Collection_insertItem (I, Any item, long position);

/* Methods:

	static long position (I, Any data, long hint);
		Question asked by Collection_addItem: return a position for the data.
	Collection::position always returns my size + 1 (add item at the end).

*/

/********** class Ordered **********/

#define Ordered_members Collection_members
#define Ordered_methods Collection_methods
class_create (Ordered, Collection);

Any Ordered_create (void);
int Ordered_init (I, void *itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (Ordered) inserts an item at the end.
*/

int Ordered_addItemPos (I, Any data, long position);
/*
	Function:
		insert an item at 'position'.
		If 'position' is less than 1 or greater than the current 'size',
		insert the item at the end.
*/

/********** class Sorted **********/
/* A Sorted is a sorted Collection. */

#define Sorted_members Collection_members
#define Sorted_methods Collection_methods \
	int (*compare) (I, thou);
class_create (Sorted, Collection);

int Sorted_init (I, void *itemClass, long initialCapacity);

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

int Sorted_addItem_unsorted (I, Any data);
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

#define SortedSet_members Sorted_members
#define SortedSet_methods Sorted_methods
class_create (SortedSet, Sorted);

int SortedSet_init (I, void *itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (SortedSet) refuses to insert an item if this item already occurs.
		Equality is there when the compare routine returns 0.
	Collections_merge (SortedSet) yields a SortedSet that is the union of the two sources.
*/

int SortedSet_hasItem (I, Any item);

/********** class SortedSetOfInt **********/

#define SortedSetOfInt_members SortedSet_members
#define SortedSetOfInt_methods SortedSet_methods
class_create (SortedSetOfInt, SortedSet);

int SortedSetOfInt_init (I);
SortedSetOfInt SortedSetOfInt_create (void);

/********** class SortedSetOfShort **********/

#define SortedSetOfShort_members SortedSet_members
#define SortedSetOfShort_methods SortedSet_methods
class_create (SortedSetOfShort, SortedSet);

int SortedSetOfShort_init (I);
SortedSetOfShort SortedSetOfShort_create (void);

/********** class SortedSetOfLong **********/

#define SortedSetOfLong_members SortedSet_members
#define SortedSetOfLong_methods SortedSet_methods
class_create (SortedSetOfLong, SortedSet);

int SortedSetOfLong_init (I);
SortedSetOfLong SortedSetOfLong_create (void);

/********** class SortedSetOfFloat **********/

#define SortedSetOfFloat_members SortedSet_members
#define SortedSetOfFloat_methods SortedSet_methods
class_create (SortedSetOfFloat, SortedSet);

int SortedSetOfFloat_init (I);
SortedSetOfFloat SortedSetOfFloat_create (void);

/********** class SortedSetOfDouble **********/

#define SortedSetOfDouble_members SortedSet_members
#define SortedSetOfDouble_methods SortedSet_methods
class_create (SortedSetOfDouble, SortedSet);

int SortedSetOfDouble_init (I);
SortedSetOfDouble SortedSetOfDouble_create (void);

/********** class SortedSetOfString **********/

#define SortedSetOfString_members SortedSet_members
#define SortedSetOfString_methods SortedSet_methods
class_create (SortedSetOfString, SortedSet);

int SortedSetOfString_init (I);
SortedSetOfString SortedSetOfString_create (void);
long SortedSetOfString_lookUp (SortedSetOfString me, const wchar_t *string);
int SortedSetOfString_add (SortedSetOfString me, const wchar_t *string);

/********** class Cyclic **********/

#define Cyclic_members Collection_members
#define Cyclic_methods Collection_methods \
	int (*compare) (I, thou);   /* virtual */
class_create (Cyclic, Collection);

int Cyclic_init (I, void *itemClass, long initialCapacity);

void Cyclic_unicize (I);

/* End of file Collection.h */
#endif
