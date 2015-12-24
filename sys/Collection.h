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
	Thing *item;   // [1..size]
	long size;
	long _capacity;
	bool _ownItems;
	bool _ownershipInitialized;

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
	The items are either owned by the Collection, or they are references.
	You can access the items in the collection as item [1] through item [size].
	There can be no null items.

	Attributes:
		_capacity >= size		// private; grows as you add items.
		size			// the current number of items.
		item [1..size]		// the items.
*/

template <typename T>
struct CollectionOf {
	T** _item { nullptr };   // [1..size]
	long _size { 0 };
	long _capacity { 0 };
	bool _ownItems { true };
	bool _ownershipInitialized { false };

	CollectionOf () {
	}
	virtual ~ CollectionOf () {
		/*
			We cannot simply do
				//our v_destroy ();
			because C++ will implicitly call the destructor for structDaata,
			whereas structCollection::v_destroy explicitly calls structDaata::v_destroy;
			calling v_destroy here would therefore free structThing::name twice,
			which may not crash Praat (assuming that `name` is nulled the first time)
			but which is not how destruction should be organized.
		*/
		if (our _item) {
			if (our _ownItems) {
				for (long i = 1; i <= our _size; i ++) {
					_Thing_forget (our _item [i]);
				}
			}
			our _item ++;   // convert from base-1 to base-0
			Melder_free (our _item);
		}
	}
	long size () const {
		return _size;
	}
	T*& operator[] (long i) const {
		return _item [i];
	}
	explicit operator bool () const {
		return !! our _item;
	}
	CollectionOf<T>& operator= (const CollectionOf<T>&) = delete;   // disable copy assignment from an l-value of class T*
	template <class Y> CollectionOf<T>& operator= (const CollectionOf<Y>&) = delete;   // disable copy assignment from an l-value of a descendant class of T*
	CollectionOf<T> (CollectionOf<T>&& other) noexcept :
		_item (other. _item),
		_size (other. _size),
		_capacity (other. _capacity),
		_ownItems (other. _ownItems),
		_ownershipInitialized (other. _ownershipInitialized)
	{
		other. _item = nullptr;
		other. _size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
	}
	template <class Y> CollectionOf<T> (CollectionOf<Y>&& other) noexcept :
		_item (other. _item),
		_size (other. _size),
		_capacity (other. _capacity),
		_ownItems (other. _ownItems),
		_ownershipInitialized (other. _ownershipInitialized)
	{
		other. _item = nullptr;
		other. _size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
	}
	CollectionOf<T>& operator= (CollectionOf<T>&& other) noexcept {
		if (other. _item != our _item) {
			if (our _item) {
				if (our _ownItems) {
					for (long i = 1; i <= our _size; i ++) {
						_Thing_forget (our _item [i]);
					}
				}
				our _item ++;   // convert from base-1 to base-0
				Melder_free (our _item);
			}
		}
		our _item = other. _item;
		our _size = other. _size;
		our _capacity = other. _capacity;
		our _ownItems = other. _ownItems;
		our _ownershipInitialized = other. _ownershipInitialized;
		other. _item = nullptr;
		other. _size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
		return *this;
	}
	template <class Y> CollectionOf<T>& operator= (CollectionOf<Y>&& other) noexcept {
		if (other. _item != our _item) {
			if (our _item) {
				if (our _ownItems) {
					for (long i = 1; i <= our _size; i ++) {
						_Thing_forget (our _item [i]);
					}
				}
				our _item ++;   // convert from base-1 to base-0
				Melder_free (our _item);
			}
		}
		our _item = other. _item;
		our _size = other. _size;
		our _capacity = other. _capacity;
		our _ownItems = other. _ownItems;
		our _ownershipInitialized = other. _ownershipInitialized;
		other. _item = nullptr;
		other. _size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
		return *this;
	}
	CollectionOf<T>&& move () noexcept { return static_cast <CollectionOf<T>&&> (*this); }

	void _initializeOwnership (bool ownItems) {
		if (our _ownershipInitialized) {
			Melder_assert (our _ownItems == ownItems);
		} else {
			our _ownItems = ownItems;
			our _ownershipInitialized = true;
		}
	}
	void _makeRoomForOneMoreItem (long pos) {
		if (our _size >= our _capacity) {
			long newCapacity = 2 * our _capacity + 30;   // enough room to guarantee space for one more item, if _capacity >= 0
			T** oldItem_base0 = ( our _item ? our _item + 1 : nullptr );   // convert from base-1 to base-0
			T** newItem_base0 = (T**) Melder_realloc (oldItem_base0, newCapacity * (int64) sizeof (T*));
			our _item = newItem_base0 - 1;   // convert from base-0 to base-1
			our _capacity = newCapacity;
		}
		our _size ++;
		for (long i = our _size; i > pos; i --) our _item [i] = our _item [i - 1];
	}
	void _insertItem_move (_Thing_auto <T> data, long pos) {
		our _initializeOwnership (true);
		our _makeRoomForOneMoreItem (pos);
		our _item [pos] = data.releaseToAmbiguousOwner();
	}
	void _insertItem_ref (T* data, long pos) {
		our _initializeOwnership (false);
		our _makeRoomForOneMoreItem (pos);
		our _item [pos] = data;
	}
	void addItem_ref (T* thing) {
		//Melder_casual (U"addItem_ref ", _capacity, U" ", _ownItems);
		Melder_assert (thing);
		long index = our _v_position (thing);
		if (index != 0) {
			our _insertItem_ref (thing, index);
		} else {
			our _initializeOwnership (false);
		}
	}
	void addItem_move (_Thing_auto<T> thing) {
		T* thingRef = thing.get();
		long index = our _v_position (thingRef);
		if (index != 0) {
			our _insertItem_move (thing.move(), index);
		} else {
			our _initializeOwnership (true);
			thing.reset();   // could not insert; I am the owner, so I must dispose of the data
		}
	}
	T* subtractItem_ref (long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		Melder_assert (! our _ownItems);
		T* result = our _item [pos];
		for (long i = pos; i < our _size; i ++) our _item [i] = our _item [i + 1];
		our _size --;
		return result;
	}
	void replaceItem_ref (T* data, long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		Melder_assert (! our _ownItems);
		our _item [pos] = data;
	}
	void replaceItem_move (_Thing_auto <T> data, long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		Melder_assert (our _ownItems);
		_Thing_forget (our _item [pos]);
		our _item [pos] = data.releaseToAmbiguousOwner();
	}
	void removeItem (long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		if (our _ownItems) _Thing_forget (our _item [pos]);
		for (long i = pos; i < our _size; i ++) our _item [i] = our _item [i + 1];
		our _size --;
	}
	void sort (int (*compare) (T*, T*)) {
		long l, r, j, i;
		T* k;
		T** a = our _item;
		long n = our _size;
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

	virtual long _v_position (T* data) {
		return our _size + 1; /* at end */
	};
};

void Collection_init (Collection me, long initialCapacity);
autoCollection Collection_create (long initialCapacity);
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

void _Collection_insertItem_move (Collection me, autoThing item, long position);
void _Collection_insertItem_ref (Collection me, Thing item, long position);

/********** class Ordered **********/

Thing_define (Ordered, Collection) {
};

template <typename T>
struct OrderedOf : CollectionOf <T> {
	OrderedOf () {
	}
	void addItemAtPosition_move (_Thing_auto <T> data, long position) {
		Melder_assert (data);
		if (position < 1 || position > our _size)
			position = our _size + 1;
		our _insertItem_move (data.move(), position);
	}
	OrderedOf<T>&& move () noexcept { return static_cast <OrderedOf<T>&&> (*this); }
};

autoOrdered Ordered_create ();
void Ordered_init (Ordered me, long initialCapacity);

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

template <typename T>
struct SortedOf : CollectionOf <T> {
	SortedOf () {
	}
	SortedOf<T>&& move () noexcept { return static_cast <SortedOf<T>&&> (*this); }
	void addItem_unsorted_move (_Thing_auto <T> data) {
		our _insertItem_move (data.move(), our _size + 1);
	}
	void sort () {
		our CollectionOf<T>::sort (our v_getCompareHook ());
	}

	long _v_position (T* data) override {
		typename SortedOf<T>::CompareHook compare = our v_getCompareHook ();
		if (our _size == 0 || compare (data, our _item [our _size]) >= 0) return our _size + 1;
		if (compare (data, our _item [1]) < 0) return 1;
		/* Binary search. */
		long left = 1, right = our _size;
		while (left < right - 1) {
			long mid = (left + right) / 2;
			if (compare (data, our _item [mid]) >= 0) left = mid; else right = mid;
		}
		Melder_assert (right == left + 1);
		return right;
	}

	static int s_compareHook (T* /* data1 */, T* /* data2 */) noexcept { return 0; }
	typedef int (*CompareHook) (T*, T*);
	virtual CompareHook v_getCompareHook () { return s_compareHook; }
		// should compare the keys of two items; returns negative if me < thee, 0 if me == thee, and positive if me > thee
};

void Sorted_init (Sorted me, long initialCapacity);

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

template <typename T>
struct SortedSetOf : SortedOf <T> {
	SortedSetOf () {
	}
	SortedSetOf<T>&& move () noexcept { return static_cast <SortedSetOf<T>&&> (*this); }
	long _v_position (T* data) override {   // returns 0 (refusal) if the key of 'data' already occurs
		typename SortedOf<T>::CompareHook compare = our v_getCompareHook ();
		if (our _size == 0) return 1;   // empty set? then 'data' is going to be the first item
		int where = compare (data, our _item [our _size]);   // compare with last item
		if (where > 0) return our _size + 1;   // insert at end
		if (where == 0) return 0;
		if (compare (data, our _item [1]) < 0) return 1;   // compare with first item
		long left = 1, right = our _size;
		while (left < right - 1) {
			long mid = (left + right) / 2;
			if (compare (data, our _item [mid]) >= 0)
				left = mid;
			else
				right = mid;
		}
		Melder_assert (right == left + 1);
		if (! compare (data, our _item [left]) || ! compare (data, our _item [right]))
			return 0;
		return right;
	}
};

void SortedSet_init (SortedSet me, long initialCapacity);

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

template <typename T>
struct SortedSetOfDoubleOf : SortedSetOf <T> {
	SortedSetOfDoubleOf () {
	}
	SortedSetOfDoubleOf<T>&& move () noexcept { return static_cast <SortedSetOfDoubleOf<T>&&> (*this); }
	static int s_compareHook (SimpleDouble me, SimpleDouble thee) noexcept {
		if (my number < thy number) return -1;
		if (my number > thy number) return +1;
		return 0;
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }
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

void Cyclic_init (Cyclic me, long initialCapacity);

void Cyclic_cycleLeft (Cyclic me);
void Cyclic_unicize (Cyclic me);


/* End of file Collection.h */
#endif
