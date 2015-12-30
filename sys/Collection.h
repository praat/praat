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

template <typename T> struct CollectionOf;
typedef CollectionOf<structDaata> _CollectionOfDaata;

void _CollectionOfDaata_v_copy (_CollectionOfDaata* me, _CollectionOfDaata* thee);
bool _CollectionOfDaata_v_equal (_CollectionOfDaata* me, _CollectionOfDaata* thee);
bool _CollectionOfDaata_v_canWriteAsEncoding (_CollectionOfDaata* me, int outputEncoding);
void _CollectionOfDaata_v_writeText (_CollectionOfDaata* me, MelderFile openFile);
void _CollectionOfDaata_v_readText (_CollectionOfDaata* me, MelderReadText text, int formatVersion);
void _CollectionOfDaata_v_writeBinary (_CollectionOfDaata* me, FILE *f);
void _CollectionOfDaata_v_readBinary (_CollectionOfDaata* me, FILE *f, int formatVersion);
extern struct structData_Description theCollectionOfDaata_v_description [];

template <typename T>
struct CollectionOf : structDaata {
	T** _item { nullptr };   // [1..size]
	long _size { 0 };
	long _capacity { 0 };
	bool _ownItems { true };
	bool _ownershipInitialized { false };

	CollectionOf () {
		extern ClassInfo classCollection;
		our classInfo = classCollection;
		our name = nullptr;
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
	//explicit operator bool () const {
	//	return !! our _item;
	//}
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
	void _grow (long newCapacity) {
		if (newCapacity <= our _capacity) return;
		T** oldItem_base0 = ( our _item ? our _item + 1 : nullptr );   // convert from base-1 to base-0
		T** newItem_base0 = (T**) Melder_realloc (oldItem_base0, newCapacity * (int64) sizeof (T*));
		our _item = newItem_base0 - 1;   // convert from base-0 to base-1
		our _capacity = newCapacity;
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

	/**
		Add `thing` to the collection.
		@pre
			!! item;
		@post
			my size >= my old size + 1;
			if (my size > my old _capacity) my _capacity == 2 * my old _capacity;

		You don't transfer ownership of 'thing' to the Collection.

		You cannot call both addItem_move() and addItem_ref() on the same Collection.
	*/
	void addItem_ref (T* thing) {
		Melder_assert (thing);
		long index = our _v_position (thing);
		if (index != 0) {
			our _insertItem_ref (thing, index);
		} else {
			our _initializeOwnership (false);
		}
	}

	/**
		Add 'thing' to the collection.
		@pre
			!! item;
		@post
			my size >= my old size + 1;
			if (my size > my old _capacity) my _capacity == 2 * my old _capacity + 30;

		You transfer ownership of 'thing' to the Collection.

		You cannot call both
		Collection_addItem_move() and Collection_addItem_ref() on the same Collection.
		For a SortedSet, this may mean that the Collection immediately disposes of 'item',
		if that item already occurred in the Collection.
	*/
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

	/*
		Remove the item from the collection, without destroying it.
		@post
			item not found || my size == my old size - 1;
		Usage:
			this is the way in which an item can detach itself from a list;
			often used just before the item is destroyed, hence the name of this procedure.
	*/
	void undangleItem (Thing thing) {
		for (long i = our _size; i > 0; i --) {
			if (our _item [i] == thing) {
				for (long j = i; j < our _size; j ++) {
					our _item [j] = our _item [j + 1];
				}
			}
			our _size --;
		}
	}

	/**
		For subtractItem_move(): FIXME
		Remove the item at 'position' from the collection and transfer ownership to the caller.
		Return value:
			the subtracted item; the caller is responsible for eventually removing it.
		Preconditions:
			1 <= position <= my size;
		Postconditions:
			my size == my old size - 1;
			my _capacity not changed;
	*/
	_Thing_auto<T> subtractItem_move (long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		Melder_assert (our _ownItems);
		_Thing_auto<T> result (our _item [pos]);
		for (long i = pos; i < our _size; i ++) our _item [i] = our _item [i + 1];
		our _size --;
		return result;
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

	/**
		Remove the item at 'position' from the collection and from memory.
		@pre
			1 <= position <= my size;
		@post
			my size == my old size - 1;
			my _capacity not changed;
	*/
	void removeItem (long pos) {
		Melder_assert (pos >= 1 && pos <= our _size);
		if (our _ownItems) _Thing_forget (our _item [pos]);
		for (long i = pos; i < our _size; i ++) our _item [i] = our _item [i + 1];
		our _size --;
	}

	/**
		Remove all items from the collection and from memory.
		@post
			my size == 0;
			my _capacity not changed;
	*/
	void removeAllItems () {
		if (our _ownItems) {
			for (long i = 1; i <= our _size; i ++) {
				_Thing_forget (our _item [i]);
			}
		}
		our _size = 0;
	}

	/**
		Release as much memory as possible without affecting the items.
		@post
			my _capacity == max (my size, 1);
	*/
	void shrinkToFit () {
		our _capacity = ( our _size > 0 ? our _size : 1 );
		our _item ++;
		our _item = (T**) Melder_realloc (our _item, our _capacity * (int64) sizeof (Thing));
		our _item --;
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
	/*
		Function:
			merge two Collections into a new one. The class is the same as the type of `me`.
		Postconditions:
			result -> size >= my size;
			result -> size >= thy size;
	*/
	void merge (CollectionOf<T>* thee) {
		try {
			if (our classInfo != thy classInfo)
				Melder_throw (U"The two collections are of different classes.");
			if (our _ownershipInitialized && thy _ownershipInitialized && our _ownItems != thy _ownItems)
				Melder_throw (U"Cannot mix data and references.");
			if (! our _ownershipInitialized && ! thy _ownershipInitialized) {
				Melder_assert (our size() == 0 && thy size() == 0);
				return;
			}
			our _ownItems = ( our _ownershipInitialized ? our _ownItems : thy _ownItems );
			for (long i = 1; i <= thy size(); i ++) {
				T* item = thy _item [i];
				if (our _ownItems) {
					if (! Thing_isa (item, classDaata))
						Melder_throw (U"Cannot copy item of class ", Thing_className (item), U".");
					our addItem_move (Data_copy (item));
				} else {
					our addItem_ref (item);
				}
			}
		} catch (MelderError) {
			Melder_throw (this, U" and ", thee, U" not merged." );
		}
	}

	/*
		CollectionOf overrides all virtual methods of Daata.
		They are defined in Collection.cpp (if not in this header file).
		These methods are needed only when a Collection object is used
		as an independent pointer-to-object created with XXX_create ().
	*/

	void v_info () override {
		MelderInfo_writeLine (our _size, U" items");
	}

	void v_destroy () override {
		/*
			The items are destroyed automatically be the destructor,
			which is called by delete, which is called by forget().
			So we only have to destroy the members of Daata,
			which are not automatic.
		*/
		structDaata::v_destroy ();
	}

	void v_copy (Daata data_to) override {   // copies all the items
		_CollectionOfDaata_v_copy (reinterpret_cast<_CollectionOfDaata*> (this), reinterpret_cast<_CollectionOfDaata*> (data_to));
	}
	bool v_equal (Daata data2) override {   // compares 'my item [i]' with 'thy item [i]', i = 1..size
		return _CollectionOfDaata_v_equal (reinterpret_cast<_CollectionOfDaata*> (this), reinterpret_cast<_CollectionOfDaata*> (data2));
	}
	bool v_canWriteAsEncoding (int outputEncoding) override {
		return _CollectionOfDaata_v_canWriteAsEncoding (reinterpret_cast<_CollectionOfDaata*> (this), outputEncoding);
	}
	void v_writeText (MelderFile openFile) override {
		_CollectionOfDaata_v_writeText (reinterpret_cast<_CollectionOfDaata*> (this), openFile);
	}
	void v_readText (MelderReadText text, int formatVersion) override {
		_CollectionOfDaata_v_readText (reinterpret_cast<_CollectionOfDaata*> (this), text, formatVersion);
	}
	void v_writeBinary (FILE *f) override {
		_CollectionOfDaata_v_writeBinary (reinterpret_cast<_CollectionOfDaata*> (this), f);
	}
	void v_readBinary (FILE *f, int formatVersion) override {
		_CollectionOfDaata_v_readBinary (reinterpret_cast<_CollectionOfDaata*> (this), f, formatVersion);
	}
	Data_Description v_description () override {
		return & theCollectionOfDaata_v_description [0];
	}

	/*
		CollectionOf<> introduces one virtual methods of its own (not counting the destructor).
	*/

	virtual long _v_position (T* /* data */) {
		return our _size + 1;   // at end
	};
};


#define _Collection_declare(klas,genericClass,itemClass) \
	typedef genericClass<struct##itemClass> struct##klas; \
	typedef genericClass<struct##itemClass> *klas; \
	typedef _Thing_auto <genericClass<struct##itemClass>> auto##klas; \
	extern struct structClassInfo theClassInfo_##klas; \
	extern ClassInfo class##klas; \
	static inline auto##klas klas##_create () { \
		auto##klas me (new genericClass<struct##itemClass>); \
		theTotalNumberOfThings += 1; \
		return me; \
	}

_Collection_declare (Collection, CollectionOf, Thing);

/********** class Ordered **********/

template <typename T>
struct OrderedOf : CollectionOf <T> {

	OrderedOf () {
		extern ClassInfo classOrdered;
		our classInfo = classOrdered;
	}

	/**
		Function:
			insert an item at 'position'.
			If 'position' is less than 1 or greater than the current 'size',
			insert the item at the end.
	*/
	void addItemAtPosition_move (_Thing_auto <T> data, long position) {
		Melder_assert (data);
		if (position < 1 || position > our _size)
			position = our _size + 1;
		our _insertItem_move (data.move(), position);
	}

	OrderedOf<T>&& move () noexcept { return static_cast <OrderedOf<T>&&> (*this); }
};

_Collection_declare (Ordered, OrderedOf, Daata);

/********** class Sorted **********/
/*
	A Sorted is a sorted Collection.
	Behaviour:
		Sorted::addItem inserts an item at such a position that the collection stays sorted.
		Sorted::merge yields a Sorted.
*/

template <typename T>
struct SortedOf : CollectionOf <T> {
	SortedOf () {
		extern ClassInfo classSorted;
		our classInfo = classSorted;
	}
	SortedOf<T>&& move () noexcept { return static_cast <SortedOf<T>&&> (*this); }

	/***** Two routines for optimization. ******/
	/* If you want to add a large group of items,
		it is best to call addItem_unsorted () repeatedly,
		and finish with sort (); this uses the fast 'heapsort' algorithm.
		Calling addItem_move () repeatedly would be slower,
		because on the average half the collection is moved in memory
		with every insertion.
	*/
	/*
		Function:
			add an item to the collection, quickly at the end.
		Warning:
			this leaves the collection unsorted; follow by Sorted_sort ().
	*/
	void addItem_unsorted_move (_Thing_auto <T> data) {
		our _insertItem_move (data.move(), our _size + 1);
	}
	/* Call this after a number of calls to Sorted_addItem_unsorted (). */
	/* The procedure used is 'heapsort'. */
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

_Collection_declare (Sorted, SortedOf, Daata);

/********** class SortedSet **********/

/*
	In a SortedSet, every item must be unique (by key)
*/
template <typename T>
struct SortedSetOf : SortedOf <T> {

	SortedSetOf () {
		extern ClassInfo classSortedSet;
		our classInfo = classSortedSet;
	}

	SortedSetOf<T>&& move () noexcept { return static_cast <SortedSetOf<T>&&> (*this); }

	/**
		@return
			0 (refusal) if the key of 'data' already occurs
	*/
	long _v_position (T* data) override {
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

	bool hasItem (T* item) {
		return our _v_position (item) == 0;
	}
};

/* Behaviour:
	Collection_addItem (SortedSet) refuses to insert an item if this item already occurs.
		Equality is there when the compare routine returns 0.
	Collections_merge (SortedSet) yields a SortedSet that is the union of the two sources.
*/

_Collection_declare (SortedSet, SortedOf, Daata);

/********** class SortedSetOfInt **********/

template <typename T>
struct SortedSetOfIntOf : SortedSetOf <T> {
	SortedSetOfIntOf () {
	}
	SortedSetOfIntOf<T>&& move () noexcept { return static_cast <SortedSetOfIntOf<T>&&> (*this); }
	static int s_compareHook (SimpleInt me, SimpleInt thee) noexcept {
		if (my number < thy number) return -1;
		if (my number > thy number) return +1;
		return 0;
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }
};

/********** class SortedSetOfLong **********/

template <typename T>
struct SortedSetOfLongOf : SortedSetOf <T> {
	SortedSetOfLongOf () {
	}
	SortedSetOfLongOf<T>&& move () noexcept { return static_cast <SortedSetOfLongOf<T>&&> (*this); }
	static int s_compareHook (SimpleLong me, SimpleLong thee) noexcept {
		if (my number < thy number) return -1;
		if (my number > thy number) return +1;
		return 0;
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }
};

/********** class SortedSetOfDouble **********/

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

/********** class SortedSetOfString **********/

template <typename T>
struct SortedSetOfStringOf : SortedSetOf <T> {
	SortedSetOfStringOf () {
	}
	SortedSetOfStringOf<T>&& move () noexcept { return static_cast <SortedSetOfStringOf<T>&&> (*this); }
	static int s_compareHook (SimpleString me, SimpleString thee) noexcept {
		return str32cmp (my string, thy string);
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }

	long lookUp (const char32 *string) {
		long numberOfItems = our _size;
		long left = 1, right = numberOfItems;
		int atStart, atEnd;
		if (numberOfItems == 0) return 0;

		atEnd = str32cmp (string, our _item [numberOfItems] -> string);
		if (atEnd > 0) return 0;
		if (atEnd == 0) return numberOfItems;

		atStart = str32cmp (string, our _item [1] -> string);
		if (atStart < 0) return 0;
		if (atStart == 0) return 1;

		while (left < right - 1) {
			long mid = (left + right) / 2;
			int here = str32cmp (string, our _item [mid] -> string);
			if (here == 0) return mid;
			if (here > 0) left = mid; else right = mid;
		}
		Melder_assert (right == left + 1);
		return 0;
	}

	void addString_copy (const char32 *string) {
		static autoSimpleString simp;
		if (! simp) {
			simp = SimpleString_create (U"");
			Melder_free (simp -> string);
		}
		simp -> string = (char32 *) string;   // reference copy
		long index = our _v_position (simp.get());
		simp -> string = nullptr;   // otherwise Praat will crash at shutdown
		if (index == 0) return;   // OK: already there: do not add
		autoSimpleString newSimp = SimpleString_create (string);
		our _insertItem_move (newSimp.move(), index);
	}
};

_Collection_declare (SortedSetOfString, SortedSetOfStringOf, SimpleString);

#define Collection_define(klas,genericClass,itemClass) \
	Thing_declare (klas); \
	static inline auto##klas klas##_create () { return Thing_new (klas); } \
	struct struct##klas : genericClass<struct##itemClass>

/* End of file Collection.h */
#endif
