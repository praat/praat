#ifndef _Collection_h_
#define _Collection_h_
/* Collection.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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
 * Collection objects contain a number of items whose class is a subclass of Daata.
 */

#include "Simple.h"

Thing_define (Collection, Daata) {
	ClassInfo itemClass;   // the class of which all items must be members (see Thing_isa)
	bool _ownershipInitialized, _ownItems;
	long _capacity, size;
	Thing *item;   // [1..size]

	void v_info ()
		override;
	void v_destroy ()
		override;   // destroys all the items
	void v_copy (Daata data_to)
		override;   // copies all the items
	bool v_equal (Daata data2)
		override;   // compares 'my item [i]' with 'thy item [i]', i = 1..size
	bool v_canWriteAsEncoding (int outputEncoding)
		override;
	void v_writeText (MelderFile openFile)
		override;
	void v_readText (MelderReadText text, int formatVersion)
		override;
	void v_writeBinary (FILE *f)
		override;
	void v_readBinary (FILE *f, int formatVersion)
		override;
	static Data_Description s_description;
	Data_Description v_description ()
		override { return s_description; }

	virtual long v_position (Thing /* data */) { return size + 1; /* at end */ };
};
/*
	An object of type Collection is a collection of items of any class.
	It is the owner of its items.
	You can access the items in the collection as item [1] through item [size].
	There can be no null items.

	Attributes:
		_capacity >= size		// private; grows as you add items.
		size			// the current number of items.
		item [1..size]		// the items.
*/

void Collection_init (Collection me, ClassInfo itemClass, long initialCapacity);
autoCollection Collection_create (ClassInfo itemClass, long initialCapacity);
/*
	Function:
		return a new empty Collection.
	Preconditions:
		initialCapacity >= 1;
	Postconditions:
		my _capacity == initialCapacity;
	Failures:
		Out of memory.
*/

/*
	Data_copy, Data_equal, Data_writeXXX, Data_readXXX
	try to copy, compare, write, or read all the items.
	However, if any of the items is not of class Daata,
	these routines fail with a message.
*/

void Collection_addItem_move (Collection me, autoThing item);
void Collection_addItem_ref (Collection me, Thing item);
/*
	Function:
		add the 'item' to the collection.
	Preconditions:
 		!! item;
	Postconditions:
		my size >= my old size + 1;
		if (my size > my old _capacity) my _capacity == 2 * my old _capacity;
	When calling Collection_addItem_move(), you transfer ownership of 'item' to the Collection;
	when calling Collection_addItem_ref(), you don't. You cannot call both
	Collection_addItem_move() and Collection_addItem_ref() on the same Collection.
	For a SortedSet, this may mean that the Collection immediately disposes of 'item',
	if that item already occurred in the Collection.
*/

void Collection_removeItem (Collection me, long position);
/*
	Function:
		remove the item at 'position' from the collection and from memory.
	Preconditions:
		1 <= position <= my size;
	Postconditions:
		my size == my old size - 1;
		my _capacity not changed;
*/

void Collection_undangleItem (Collection me, Thing item);
/*
	Function:
		remove the item from the collection, without destroying it.
	Postconditions:
		item not found || my size == my old size - 1;
	Usage:
		this is the way in which an item can detach itself from a list;
		often used just before the item is destroyed, hence the name of this procedure.
*/

Thing Collection_subtractItem (Collection me, long position);
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

void Collection_removeAllItems (Collection me);
/*
	Function:
		remove all items from the collection and from memory.
	Postconditions:
		my size == 0;
		my _capacity not changed;
*/

void Collection_shrinkToFit (Collection me);
/*
	Function:
		release as much memory as possible without affecting the items.
	Postconditions:
		my _capacity == max (my size, 1);
*/

autoCollection Collections_merge (Collection me, Collection thee);
/*
	Function:
		merge two Collections into a new one. The class is the same as the type of `me`.
	Postconditions:
		result -> size >= my size;
		result -> size >= thy size;
*/

typedef MelderCompareHook<structDaata> Collection_ItemCompareHook;

void Collection_sort (Collection me, Collection_ItemCompareHook compareHook);

/* For the inheritors. */

void _Collection_insertItem (Collection me, Thing item, long position);
void _Collection_insertItem_move (Collection me, autoThing item, long position);
void _Collection_insertItem_ref (Collection me, Thing item, long position);

/********** class Ordered **********/

Thing_define (Ordered, Collection) {
};

autoOrdered Ordered_create ();
void Ordered_init (Ordered me, ClassInfo itemClass, long initialCapacity);

/* Behaviour:
	Collection_addItem (Ordered) inserts an item at the end.
*/

void Ordered_addItemAtPosition_move (Ordered me, autoThing data, long position);
/*
	Function:
		insert an item at 'position'.
		If 'position' is less than 1 or greater than the current 'size',
		insert the item at the end.
*/

/********** class Sorted **********/
/* A Sorted is a sorted Collection. */

Thing_define (Sorted, Collection) {
	long v_position (Thing data)
		override;

	static int s_compareHook (Daata data1, Daata data2) noexcept;
	virtual Data_CompareHook v_getCompareHook () { return s_compareHook; }
		// should compare the keys of two items; returns negative if me < thee, 0 if me == thee, and positive if me > thee
};

void Sorted_init (Sorted me, ClassInfo itemClass, long initialCapacity);

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

void Sorted_addItem_unsorted_move (Sorted me, autoThing data);
/*
	Function:
		add an item to the collection, quickly at the end.
	Warning:
		this leaves the collection unsorted; follow by Sorted_sort ().
*/
void Sorted_sort (Sorted me);
/* Call this after a number of calls to Sorted_addItem_unsorted (). */
/* The procedure used is 'heapsort'. */

/********** class SortedSet **********/

Thing_define (SortedSet, Sorted) {   // every item must be unique (by key)
	long v_position (Thing data) override;   // returns 0 (refusal) if the key of 'data' already occurs
};

void SortedSet_init (SortedSet me, ClassInfo itemClass, long initialCapacity);

inline static bool SortedSet_hasItem (SortedSet me, Thing a_item) {
	return my v_position (a_item) == 0;
}

/* Behaviour:
	Collection_addItem (SortedSet) refuses to insert an item if this item already occurs.
		Equality is there when the compare routine returns 0.
	Collections_merge (SortedSet) yields a SortedSet that is the union of the two sources.
*/

/********** class SortedSetOfInt **********/

Thing_define (SortedSetOfInt, SortedSet) {
	static int s_compareHook (SimpleInt data1, SimpleInt data2) noexcept;
	Data_CompareHook v_getCompareHook () override { return s_compareHook; }
};

void SortedSetOfInt_init (SortedSetOfInt me);
autoSortedSetOfInt SortedSetOfInt_create ();

/********** class SortedSetOfLong **********/

Thing_define (SortedSetOfLong, SortedSet) {
	static int s_compareHook (SimpleLong data1, SimpleLong data2) noexcept;
	Data_CompareHook v_getCompareHook () override { return s_compareHook; }
};

void SortedSetOfLong_init (SortedSetOfLong me);
autoSortedSetOfLong SortedSetOfLong_create ();

/********** class SortedSetOfDouble **********/

Thing_define (SortedSetOfDouble, SortedSet) {
	static int s_compareHook (SimpleDouble data1, SimpleDouble data2) noexcept;
	Data_CompareHook v_getCompareHook () override { return s_compareHook; }
};

void SortedSetOfDouble_init (SortedSetOfDouble me);
autoSortedSetOfDouble SortedSetOfDouble_create ();

/********** class SortedSetOfString **********/

Thing_define (SortedSetOfString, SortedSet) {
	static int s_compareHook (SimpleString me, SimpleString thee) noexcept { return str32cmp (my string, thy string); }
	Data_CompareHook v_getCompareHook () override { return s_compareHook; }
};

void SortedSetOfString_init (SortedSetOfString me);
autoSortedSetOfString SortedSetOfString_create ();
void SortedSetOfString_addString_copy (SortedSetOfString me, const char32 *string);
long SortedSetOfString_lookUp (SortedSetOfString me, const char32 *string);

/********** class Cyclic **********/

Thing_define (Cyclic, Collection) {   // the cyclic list (a, b, c, d) equals (b, c, d, a) but not (d, c, a, b)
	static int s_compareHook (Daata data1, Daata data2) noexcept;
	virtual Data_CompareHook v_getCompareHook () { return s_compareHook; }
};

void Cyclic_init (Cyclic me, ClassInfo itemClass, long initialCapacity);

void Cyclic_cycleLeft (Cyclic me);
void Cyclic_unicize (Cyclic me);


/* End of file Collection.h */
#endif
