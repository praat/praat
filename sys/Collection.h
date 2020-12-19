#ifndef _Collection_h_
#define _Collection_h_
/* Collection.h
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2012,2014-2019 Paul Boersma
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

/*
 * Collection objects contain a number of items whose class is a subclass of Thing or Daata.
 */

#include "Simple.h"


#pragma mark - class Collection

/**
	An object of type Collection is a collection of objects of any class T*
	that is a subclass of Thing or Daata.
	These elements ("items") are either owned by the Collection, or they are references.
	You can access the items as `at [1]` through `at [size]`.
	There can be no null items.

	@param  size			the current number of items
	@param  at[1..size]		the items
*/
template <typename T> struct CollectionOf;

typedef CollectionOf<structDaata> _CollectionOfDaata;
extern void _CollectionOfDaata_v_copy (_CollectionOfDaata* me, _CollectionOfDaata* thee);
extern bool _CollectionOfDaata_v_equal (_CollectionOfDaata* me, _CollectionOfDaata* thee);
extern bool _CollectionOfDaata_v_canWriteAsEncoding (_CollectionOfDaata* me, int outputEncoding);
extern void _CollectionOfDaata_v_writeText (_CollectionOfDaata* me, MelderFile openFile);
extern void _CollectionOfDaata_v_readText (_CollectionOfDaata* me, MelderReadText text, int formatVersion);
extern void _CollectionOfDaata_v_writeBinary (_CollectionOfDaata* me, FILE *f);
extern void _CollectionOfDaata_v_readBinary (_CollectionOfDaata* me, FILE *f, int formatVersion);
extern struct structData_Description theCollectionOfDaata_v_description [3];

template <typename T   /*Melder_ENABLE_IF_ISA (T, structThing)*/>
struct ArrayOf {
	T** _elements { nullptr };
	T*& operator[] (integer i) const {
		return our _elements [i];
	}
};

template <typename T   /*Melder_ENABLE_IF_ISA (T, structThing)*/>
struct CollectionOf : structDaata {
	ArrayOf <T> at;
	integer size { 0 };
	integer _capacity { 0 };
	bool _ownItems { true };
	bool _ownershipInitialized { false };

	CollectionOf () {
		extern ClassInfo classCollection;
		our classInfo = classCollection;
		our name. reset();
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
		if (our at._elements) {
			if (our _ownItems)
				for (integer i = 1; i <= our size; i ++)
					_Thing_forget (our at [i]);
			our at._elements ++;   // convert from base-1 to base-0
			Melder_free (our at._elements);
		}
	}
	//explicit operator bool () const noexcept {
	//	return !! our item;
	//}
	CollectionOf<T>& operator= (const CollectionOf<T>&) = delete;   // disable copy assignment from an l-value of class T*
	template <class Y> CollectionOf<T>& operator= (const CollectionOf<Y>&) = delete;   // disable copy assignment from an l-value of a descendant class of T*
	CollectionOf<T> (CollectionOf<T>&& other) noexcept :
		at (other. at),
		size (other. size),
		_capacity (other. _capacity),
		_ownItems (other. _ownItems),
		_ownershipInitialized (other. _ownershipInitialized)
	{
		other. at._elements = nullptr;
		other. size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
	}
	template <class Y> CollectionOf<T> (CollectionOf<Y>&& other) noexcept :
		at (other. at),
		size (other. size),
		_capacity (other. _capacity),
		_ownItems (other. _ownItems),
		_ownershipInitialized (other. _ownershipInitialized)
	{
		other. at._elements = nullptr;
		other. size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
	}
	CollectionOf<T>& operator= (CollectionOf<T>&& other) noexcept {
		if (other. at._elements != our at._elements) {
			if (our at._elements) {
				if (our _ownItems)
					for (integer i = 1; i <= our size; i ++)
						_Thing_forget (our at [i]);
				our at._elements ++;   // convert from base-1 to base-0
				Melder_free (our at._elements);
			}
		}
		our at = other. at;
		our size = other. size;
		our _capacity = other. _capacity;
		our _ownItems = other. _ownItems;
		our _ownershipInitialized = other. _ownershipInitialized;
		other. at._elements = nullptr;
		other. size = 0;
		other. _capacity = 0;
		other. _ownItems = false;
		other. _ownershipInitialized = false;
		return *this;
	}
	template <class Y> CollectionOf<T>& operator= (CollectionOf<Y>&& other) noexcept {
		if (other. at._elements != our at._elements) {
			if (our at._elements) {
				if (our _ownItems)
					for (integer i = 1; i <= our size; i ++)
						_Thing_forget (our at [i]);
				our at._elements ++;   // convert from base-1 to base-0
				Melder_free (our at._elements);
			}
		}
		our at = other. at;
		our size = other. size;
		our _capacity = other. _capacity;
		our _ownItems = other. _ownItems;
		our _ownershipInitialized = other. _ownershipInitialized;
		other. at._elements = nullptr;
		other. size = 0;
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
	void _grow (integer newCapacity) {
		if (newCapacity <= our _capacity)
			return;
		T** oldItem_base0 = ( our at._elements ? our at._elements + 1 : nullptr );   // convert from base-1 to base-0
		T** newItem_base0 = (T**) Melder_realloc (oldItem_base0, newCapacity * (int64) sizeof (T*));
		our at._elements = newItem_base0 - 1;   // convert from base-0 to base-1
		our _capacity = newCapacity;
	}
	void _makeRoomForOneMoreItem (integer pos) {
		if (our size >= our _capacity) {
			integer newCapacity = 2 * our _capacity + 30;   // enough room to guarantee space for one more item, if _capacity >= 0
			T** oldItem_base0 = ( our at._elements ? our at._elements + 1 : nullptr );   // convert from base-1 to base-0
			T** newItem_base0 = (T**) Melder_realloc (oldItem_base0, newCapacity * (int64) sizeof (T*));
			our at._elements = newItem_base0 - 1;   // convert from base-0 to base-1
			our _capacity = newCapacity;
		}
		our size ++;
		for (integer i = our size; i > pos; i --) our at [i] = our at [i - 1];
	}
	T* _insertItem_move (autoSomeThing <T> data, integer pos) {
		our _initializeOwnership (true);
		our _makeRoomForOneMoreItem (pos);
		return our at [pos] = data.releaseToAmbiguousOwner();
	}
	void _insertItem_ref (T* data, integer pos) {
		our _initializeOwnership (false);
		our _makeRoomForOneMoreItem (pos);
		our at [pos] = data;
	}

	/**
		Add `thing` to the collection.
		@pre
			!! thing;
		@post
			my size >= my old size

		You don't transfer ownership of `thing` to the Collection.

		You cannot call both addItem_move() and addItem_ref() on the same Collection.
	*/
	void addItem_ref (T* thing) {
		Melder_assert (thing);
		integer index = our _v_position (thing);
		if (index != 0) {
			our _insertItem_ref (thing, index);
		} else {
			our _initializeOwnership (false);
		}
	}

	/**
		Add 'thing' to the collection.
		@pre
			!! thing;
		@post
			my size >= my old size

		You transfer ownership of 'thing' to the Collection.

		You cannot call both addItem_move() and addItem_ref() on the same Collection.
		For a SortedSet, this may mean that the Collection immediately disposes of 'item',
		if that item already occurred in the Collection.
	*/
	T* addItem_move (autoSomeThing<T> thing) {
		T* thingRef = thing.get();
		integer index = our _v_position (thingRef);
		if (index != 0) {
			return our _insertItem_move (thing.move(), index);
		} else {
			our _initializeOwnership (true);
			thing.reset();   // could not insert; I am the owner, so I must dispose of the data
			return nullptr;
		}
	}

	/**
		Remove the item from the collection, without destroying it.
		@post
			item not found || my size == my old size - 1;

		Usage:
			this is the way in which an item can detach itself from a list;
			often used just before the item is destroyed, hence the name of this procedure.
	*/
	void undangleItem (Thing thing) {
		for (integer i = our size; i > 0; i --) {
			if (our at [i] == thing) {
				for (integer j = i; j < our size; j ++) {
					our at [j] = our at [j + 1];
				}
				our size --;
			}
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
	autoSomeThing<T> subtractItem_move (integer pos) {
		Melder_assert (pos >= 1 && pos <= our size);
		Melder_assert (our _ownItems);
		autoSomeThing<T> result;
		result. adoptFromAmbiguousOwner (our at [pos]);
		for (integer i = pos; i < our size; i ++)
			our at [i] = our at [i + 1];
		our size --;
		return result;
	}
	T* subtractItem_ref (integer pos) {
		Melder_assert (pos >= 1 && pos <= our size);
		Melder_assert (! our _ownItems);
		T* result = our at [pos];
		for (integer i = pos; i < our size; i ++)
			our at [i] = our at [i + 1];
		our size --;
		return result;
	}
	void replaceItem_ref (T* data, integer pos) {
		Melder_assert (pos >= 1 && pos <= our size);
		Melder_assert (! our _ownItems);
		our at [pos] = data;
	}
	T* replaceItem_move (autoSomeThing <T> data, integer pos) {
		Melder_assert (pos >= 1 && pos <= our size);
		Melder_assert (our _ownItems);
		_Thing_forget (our at [pos]);
		return our at [pos] = data.releaseToAmbiguousOwner();
	}

	/**
		Remove the item at 'position' from the collection and from memory.
		@pre
			1 <= position <= my size;
		@post
			my size == my old size - 1;
			my _capacity not changed;
	*/
	void removeItem (integer pos) {
		Melder_assert (pos >= 1 && pos <= our size);
		if (our _ownItems) _Thing_forget (our at [pos]);
		for (integer i = pos; i < our size; i ++)
			our at [i] = our at [i + 1];
		our size --;
	}

	/**
		Remove all items from the collection and from memory.
		@post
			my size == 0;
			my _capacity not changed;
	*/
	void removeAllItems () {
		if (our _ownItems) {
			for (integer i = 1; i <= our size; i ++)
				_Thing_forget (our at [i]);
		}
		our size = 0;
	}

	/**
		Release as much memory as possible without affecting the items.
		@post
			my _capacity == max (my size, 1);
	*/
	void shrinkToFit () {
		our _capacity = ( our size > 0 ? our size : 1 );
		our at ++;
		our at = (T**) Melder_realloc (our at, our _capacity * (int64) sizeof (Thing));
		our at --;
	}
	void sort (int (*compare) (T*, T*)) {
		integer l, r, j, i;
		T* k;
		T** a = our at._elements;
		integer n = our size;
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
			result->size >= my size;
			result->size >= thy size;
	*/
	void merge (CollectionOf<T>* thee) {
		try {
			if (our classInfo != thy classInfo)
				Melder_throw (U"The two collections are of different classes.");
			if (our _ownershipInitialized && thy _ownershipInitialized && our _ownItems != thy _ownItems)
				Melder_throw (U"Cannot mix data and references.");
			if (! our _ownershipInitialized && ! thy _ownershipInitialized) {
				Melder_assert (our size == 0 && thy size == 0);
				return;
			}
			our _ownItems = ( our _ownershipInitialized ? our _ownItems : thy _ownItems );
			for (integer i = 1; i <= thy size; i ++) {
				T* item = thy at [i];
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
		MelderInfo_writeLine (our size, U" items");
	}

	void v_destroy () noexcept override {
		/*
			The items are destroyed automatically by the destructor,
			which is called by delete, which is called by forget().
			So we only have to destroy the members of Daata,
			many of which are not destroyed automatically.
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
		CollectionOf<> introduces one virtual method of its own (not counting the destructor).
	*/
	virtual integer _v_position (T* /* data */) {
		return our size + 1;   // at end
	};
};


#define _Collection_declare(klas,genericClass,itemClass) \
	typedef genericClass<struct##itemClass> struct##klas; \
	typedef genericClass<struct##itemClass> *klas; \
	typedef autoSomeThing <genericClass<struct##itemClass>> auto##klas; \
	extern struct structClassInfo theClassInfo_##klas; \
	extern ClassInfo class##klas; \
	static inline auto##klas klas##_create () { \
		auto##klas me; \
		me. adoptFromAmbiguousOwner (new genericClass<struct##itemClass>); \
		theTotalNumberOfThings += 1; \
		return me; \
	}

_Collection_declare (Collection, CollectionOf, Thing);


#pragma mark - class Ordered
/*
	An Ordered is a Collection in which the order of the elements is crucial.
	It could also be called a List.
*/

template <typename T>
struct OrderedOf : CollectionOf <T   /*Melder_ENABLE_IF_ISA (T, structDaata)*/> {

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
	T* addItemAtPosition_move (autoSomeThing <T> data, integer position) {
		Melder_assert (data);
		if (position < 1 || position > our size)
			position = our size + 1;
		return our _insertItem_move (data.move(), position);
	}

	OrderedOf<T>&& move () noexcept { return static_cast <OrderedOf<T>&&> (*this); }
};

_Collection_declare (Ordered, OrderedOf, Daata);

#pragma mark - class Sorted
/*
	A Sorted is a sorted Collection.
	Behaviour:
		Sorted::addItem inserts an item at such a position that the collection stays sorted.
		Sorted::merge yields a Sorted.
*/

template <typename T   /*Melder_ENABLE_IF_ISA (T, structDaata)*/>
struct SortedOf : CollectionOf <T> {
	SortedOf () {
		extern ClassInfo classSorted;
		our classInfo = classSorted;
	}
	SortedOf<T>&& move () noexcept { return static_cast <SortedOf<T>&&> (*this); }

	/***** Two routines for optimization. ******/
	/* If you want to add a large group of items,
		it is best to call addItem_unsorted () repeatedly,
		and finish with Sorted::sort(), which uses the fast 'heapsort' algorithm,
		and, when appropriate, also SortedSet::unicize(), which has linear complexity.
		Calling addItem_move () repeatedly would be slower,
		because on average half the collection is moved in memory
		with every insertion.
	*/
	/*
		Function:
			add an item to the collection, quickly at the end.
		Warning:
			this leaves the collection unsorted; follow by Sorted::sort ().
	*/
	void addItem_unsorted_move (autoSomeThing <T> data) {
		our _insertItem_move (data.move(), our size + 1);
	}
	/* Call this after a number of calls to Sorted_addItem_unsorted (). */
	/* The procedure used is 'heapsort'. */
	void sort () {
		our CollectionOf<T>::sort (our v_getCompareHook ());
	}

	integer _v_position (T* data) override {
		typename SortedOf<T>::CompareHook compare = our v_getCompareHook ();
		if (our size == 0 || compare (data, our at [our size]) >= 0) return our size + 1;
		if (compare (data, our at [1]) < 0) return 1;
		/* Binary search. */
		integer left = 1, right = our size;
		while (left < right - 1) {
			integer mid = (left + right) / 2;
			if (compare (data, our at [mid]) >= 0) left = mid; else right = mid;
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


#pragma mark - class SortedSet
/*
	In a SortedSet, every item must be unique (by key).
*/

template <typename T   /*Melder_ENABLE_IF_ISA (T, structDaata)*/>
struct SortedSetOf : SortedOf <T> {

	SortedSetOf () {
		extern ClassInfo classSortedSet;
		our classInfo = classSortedSet;
	}

	SortedSetOf<T>&& move () noexcept { return static_cast <SortedSetOf<T>&&> (*this); }

	/**
		@return
			0 (refusal) if the key of `data` already occurs
	*/
	integer _v_position (T* data) override {
		typename SortedOf<T>::CompareHook compare = our v_getCompareHook ();
		if (our size == 0) return 1;   // empty set? then 'data' is going to be the first item
		int where = compare (data, our at [our size]);   // compare with last item
		if (where > 0) return our size + 1;   // insert at end
		if (where == 0) return 0;
		if (compare (data, our at [1]) < 0) return 1;   // compare with first item
		integer left = 1, right = our size;
		while (left < right - 1) {
			integer mid = (left + right) / 2;
			if (compare (data, our at [mid]) >= 0)
				left = mid;
			else
				right = mid;
		}
		Melder_assert (right == left + 1);
		if (! compare (data, our at [left]) || ! compare (data, our at [right]))
			return 0;
		return right;
	}

	bool hasItem (T* item) {
		return our _v_position (item) == 0;
	}

	/**
		See the comments at Sorted::sort().
		@pre Must have been sorted beforehand.
		@post All elements are different (by key).
	*/
	void unicize () {
		typename SortedOf<T>::CompareHook compare = our v_getCompareHook ();
		integer n = 0, ifrom = 1;
		for (integer i = 1; i <= our size; i ++) {
			if (i == our size || compare (our at [i], our at [i + 1]))
			{
				/*
				 * Detected a change.
				 */
				n ++;
				integer ito = i;
				/*
				 * Move item 'ifrom' to 'n'.
				 */
				Melder_assert (ifrom >= n);
				if (ifrom != n) {
					our at [n] = our at [ifrom];   // surface copy
					our at [ifrom] = nullptr;   // undangle
				}
				/*
				 * Purge items from 'ifrom'+1 to 'ito'.
				 */
				if (our _ownItems) {
					for (integer j = ifrom + 1; j <= ito; j ++) {
						_Thing_forget (our at [j]);
					}
				}
				ifrom = ito + 1;
			}
		}
		our size = n;
	}

};

/* Behaviour:
	Collection_addItem (SortedSet) refuses to insert an item if this item already occurs.
		Equality is there when the compare routine returns 0.
	Collections_merge (SortedSet) yields a SortedSet that is the union of the two sources.
*/

_Collection_declare (SortedSet, SortedSetOf, Daata);


#pragma mark - class SortedSetOfInteger

template <typename T   Melder_ENABLE_IF_ISA (T, structSimpleInteger)>
struct SortedSetOfIntegerOf : SortedSetOf <T> {
	SortedSetOfIntegerOf () {
	}
	SortedSetOfIntegerOf<T>&& move () noexcept { return static_cast <SortedSetOfIntegerOf<T>&&> (*this); }
	static int s_compareHook (SimpleInteger me, SimpleInteger thee) noexcept {
		if (my number < thy number) return -1;
		if (my number > thy number) return +1;
		return 0;
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }
};


#pragma mark - class SortedSetOfDouble

template <typename T   /*Melder_ENABLE_IF_ISA (T, structSimpleDouble)*/>
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


#pragma mark - class SortedSetOfString

template <typename T   Melder_ENABLE_IF_ISA (T, structSimpleString)>
struct SortedSetOfStringOf : SortedSetOf <T> {
	SortedSetOfStringOf () {
	}
	SortedSetOfStringOf<T>&& move () noexcept { return static_cast <SortedSetOfStringOf<T>&&> (*this); }
	static int s_compareHook (SimpleString me, SimpleString thee) noexcept {
		return str32cmp (my string.get(), thy string.get());
	}
	typename SortedOf<T>::CompareHook v_getCompareHook ()
		override { return (typename SortedOf<T>::CompareHook) our s_compareHook; }

	integer lookUp (conststring32 string) {
		integer numberOfItems = our size;
		integer left = 1, right = numberOfItems;
		int atStart, atEnd;
		if (numberOfItems == 0) return 0;

		atEnd = str32cmp (string, our at [numberOfItems] -> string.get());
		if (atEnd > 0) return 0;
		if (atEnd == 0) return numberOfItems;

		atStart = str32cmp (string, our at [1] -> string.get());
		if (atStart < 0) return 0;
		if (atStart == 0) return 1;

		while (left < right - 1) {
			integer mid = (left + right) / 2;
			int here = str32cmp (string, our at [mid] -> string.get());
			if (here == 0) return mid;
			if (here > 0) left = mid; else right = mid;
		}
		Melder_assert (right == left + 1);
		return 0;
	}

};


#pragma mark - Collections of specific types

#define Collection_define(klas,genericClass,itemClass) \
	Thing_declare (klas); \
	static inline auto##klas klas##_create () { return Thing_new (klas); } \
	struct struct##klas : genericClass<struct##itemClass>

#pragma mark class DaataList

Collection_define (DaataList, OrderedOf, /* generic */ Daata) {
};

#pragma mark class StringList

Collection_define (StringList, OrderedOf, /* final */ SimpleString) {
};

#pragma mark class StringSet

Collection_define (StringSet, SortedSetOfStringOf, /* final */ SimpleString) {
	void addString_copy (conststring32 string) {
		autoSimpleString newSimp = SimpleString_create (string);
		our addItem_move (newSimp.move());
	}
};

/* End of file Collection.h */
#endif
