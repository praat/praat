#ifndef _melder_tensor_h_
#define _melder_tensor_h_
/* melder_tensor.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

/********** Arrays with one index **********/

byte * NUMvector_generic (integer elementSize, integer lo, integer hi, bool zero);
/*
	Function:
		create a vector [lo...hi]; if `zero`, then all values are initialized to 0.
	Preconditions:
		hi >= lo;
*/

void NUMvector_free_generic (integer elementSize, byte *v, integer lo) noexcept;
/*
	Function:
		destroy a vector v that was created with NUMvector.
	Preconditions:
		lo must have the same values as with the creation of the vector.
*/

byte * NUMvector_copy_generic (integer elementSize, const byte *v, integer lo, integer hi);
/*
	Function:
		copy (part of) a vector v, which need not have been created with NUMvector, to a new one.
	Preconditions:
		if v != nullptr, the values v [lo..hi] must exist.
*/

void NUMvector_copyElements_generic (integer elementSize, const byte *v, byte *to, integer lo, integer hi);
/*
	copy the vector elements v [lo..hi] to those of a vector 'to'.
	These vectors need not have been created by NUMvector.
*/

bool NUMvector_equal_generic (integer elementSize, const byte *v1, const byte *v2, integer lo, integer hi);
/*
	return true if the vector elements v1 [lo..hi] are equal
	to the corresponding elements of the vector v2; otherwise, return false.
	The vectors need not have been created by NUMvector.
*/

void NUMvector_append_generic (integer elementSize, byte **v, integer lo, integer *hi);
void NUMvector_insert_generic (integer elementSize, byte **v, integer lo, integer *hi, integer position);
/*
	add one element to the vector *v.
	The new element is initialized to zero.
	On success, *v points to the new vector, and *hi is incremented by 1.
	On failure, *v and *hi are not changed.
*/

/********** Arrays with two indices **********/

void * NUMmatrix_generic (integer elementSize, integer row1, integer row2, integer col1, integer col2, bool zero);
/*
	Function:
		create a matrix [row1...row2] [col1...col2]; if `zero`, then all values are initialized to 0.
	Preconditions:
		row2 >= row1;
		col2 >= col1;
*/

void NUMmatrix_free_generic (integer elementSize, byte **m, integer row1, integer col1) noexcept;
/*
	Function:
		destroy a matrix m created with NUM...matrix.
	Preconditions:
		if m != nullptr: row1 and col1
		must have the same value as with the creation of the matrix.
*/

void * NUMmatrix_copy_generic (integer elementSize, void *m, integer row1, integer row2, integer col1, integer col2);
/*
	Function:
		copy (part of) a matrix m, wich does not have to be created with NUMmatrix, to a new one.
	Preconditions:
		if m != nullptr: the values m [rowmin..rowmax] [colmin..colmax] must exist.
*/

void NUMmatrix_copyElements_generic (integer elementSize, char **mfrom, char **mto, integer row1, integer row2, integer col1, integer col2);
/*
	copy the matrix elements m [r1..r2] [c1..c2] to those of a matrix 'to'.
	These matrices need not have been created by NUMmatrix.
*/

bool NUMmatrix_equal_generic (integer elementSize, void *m1, void *m2, integer row1, integer row2, integer col1, integer col2);
/*
	return 1 if the matrix elements m1 [r1..r2] [c1..c2] are equal
	to the corresponding elements of the matrix m2; otherwise, return 0.
	The matrices need not have been created by NUM...matrix.
*/

byte *** NUMtensor3_generic (integer elementSize, integer pla1, integer pla2, integer row1, integer row2, integer col1, integer col2, bool initializeToZero);
void NUMtensor3_free_generic (integer elementSize, byte ***t, integer pla1, integer row1, integer col1) noexcept;

integer NUM_getTotalNumberOfArrays ();   // for debugging

template <class T>
T* NUMvector (integer from, integer to) {
	T* result = reinterpret_cast <T*> (NUMvector_generic (sizeof (T), from, to, true));
	return result;
}

template <class T>
T* NUMvector (integer from, integer to, bool initializeToZero) {
	T* result = reinterpret_cast <T*> (NUMvector_generic (sizeof (T), from, to, initializeToZero));
	return result;
}

template <class T>
void NUMvector_free (T* ptr, integer from) noexcept {
	NUMvector_free_generic (sizeof (T), reinterpret_cast <byte *> (ptr), from);
}

template <class T>
T* NUMvector_copy (const T* ptr, integer lo, integer hi) {
	T* result = reinterpret_cast <T*> (NUMvector_copy_generic (sizeof (T), reinterpret_cast <const byte *> (ptr), lo, hi));
	return result;
}

template <class T>
bool NUMvector_equal (const T* v1, const T* v2, integer lo, integer hi) {
	return NUMvector_equal_generic (sizeof (T), reinterpret_cast <const byte *> (v1), reinterpret_cast <const byte *> (v2), lo, hi);
}

template <class T>
void NUMvector_copyElements (const T* vfrom, T* vto, integer lo, integer hi) {
	NUMvector_copyElements_generic (sizeof (T), reinterpret_cast <const byte *> (vfrom), reinterpret_cast <byte *> (vto), lo, hi);
}

template <class T>
void NUMvector_append (T** v, integer lo, integer *hi) {
	NUMvector_append_generic (sizeof (T), reinterpret_cast <byte **> (v), lo, hi);
}

template <class T>
void NUMvector_insert (T** v, integer lo, integer *hi, integer position) {
	NUMvector_insert_generic (sizeof (T), reinterpret_cast <byte **> (v), lo, hi, position);
}

template <class T>
class autoNUMvector {
	T* d_ptr;
	integer d_from;
public:
	autoNUMvector<T> (integer from, integer to) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, true);
	}
	autoNUMvector<T> (integer from, integer to, bool zero) : d_from (from) {
		d_ptr = NUMvector<T> (from, to, zero);
	}
	autoNUMvector (T *ptr, integer from) : d_ptr (ptr), d_from (from) {
	}
	autoNUMvector () : d_ptr (nullptr), d_from (1) {
	}
	~autoNUMvector<T> () {
		if (d_ptr) NUMvector_free (d_ptr, d_from);
	}
	T& operator[] (integer i) {
		return d_ptr [i];
	}
	T* peek () const {
		return d_ptr;
	}
	T* transfer () {
		T* temp = d_ptr;
		d_ptr = nullptr;   // make the pointer non-automatic again
		return temp;
	}
	void reset (integer from, integer to) {
		if (d_ptr) {
			NUMvector_free (d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, true);
	}
	void reset (integer from, integer to, bool zero) {
		if (d_ptr) {
			NUMvector_free (d_ptr, d_from);
			d_ptr = nullptr;
		}
		d_from = from;
		d_ptr = NUMvector<T> (from, to, zero);
	}
};

template <class T>
T** NUMmatrix (integer row1, integer row2, integer col1, integer col2) {
	T** result = static_cast <T**> (NUMmatrix_generic (sizeof (T), row1, row2, col1, col2, true));
	return result;
}

template <class T>
T** NUMmatrix (integer row1, integer row2, integer col1, integer col2, bool zero) {
	T** result = static_cast <T**> (NUMmatrix_generic (sizeof (T), row1, row2, col1, col2, zero));
	return result;
}

template <class T>
void NUMmatrix_free (T** ptr, integer row1, integer col1) noexcept {
	NUMmatrix_free_generic (sizeof (T), reinterpret_cast <byte **> (ptr), row1, col1);
}

template <class T>
T** NUMmatrix_copy (T** ptr, integer row1, integer row2, integer col1, integer col2) {
	#if 1
	T** result = static_cast <T**> (NUMmatrix_copy_generic (sizeof (T), ptr, row1, row2, col1, col2));
	#else
	T** result = static_cast <T**> (NUMmatrix_generic (sizeof (T), row1, row2, col1, col2));
	for (integer irow = row1; irow <= row2; irow ++)
		for (integer icol = col1; icol <= col2; icol ++)
			result [irow] [icol] = ptr [irow] [icol];
	#endif
	return result;
}

template <class T>
bool NUMmatrix_equal (T** m1, T** m2, integer row1, integer row2, integer col1, integer col2) {
	return NUMmatrix_equal_generic (sizeof (T), m1, m2, row1, row2, col1, col2);
}

template <class T>
void NUMmatrix_copyElements (T** mfrom, T** mto, integer row1, integer row2, integer col1, integer col2) {
	NUMmatrix_copyElements_generic (sizeof (T), reinterpret_cast <char **> (mfrom), reinterpret_cast <char **> (mto), row1, row2, col1, col2);
}

template <class T>
class autoNUMmatrix {
	T** d_ptr;
	integer d_row1, d_col1;
public:
	autoNUMmatrix (integer row1, integer row2, integer col1, integer col2) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	autoNUMmatrix (integer row1, integer row2, integer col1, integer col2, bool zero) : d_row1 (row1), d_col1 (col1) {
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
	autoNUMmatrix (T **ptr, integer row1, integer col1) : d_ptr (ptr), d_row1 (row1), d_col1 (col1) {
	}
	autoNUMmatrix () : d_ptr (nullptr), d_row1 (0), d_col1 (0) {
	}
	~autoNUMmatrix () {
		if (d_ptr)
			NUMmatrix_free_generic (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
	}
	T*& operator[] (integer row) {
		return d_ptr [row];
	}
	T** peek () const {
		return d_ptr;
	}
	T** transfer () {
		T** temp = d_ptr;
		d_ptr = nullptr;
		return temp;
	}
	void reset (integer row1, integer row2, integer col1, integer col2) {
		if (d_ptr) {
			NUMmatrix_free_generic (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, true);
	}
	void reset (integer row1, integer row2, integer col1, integer col2, bool zero) {
		if (d_ptr) {
			NUMmatrix_free_generic (sizeof (T), reinterpret_cast <byte **> (d_ptr), d_row1, d_col1);
			d_ptr = nullptr;
		}
		d_row1 = row1;
		d_col1 = col1;
		d_ptr = NUMmatrix<T> (row1, row2, col1, col2, zero);
	}
};

#pragma mark - TENSOR
/*
	Base-1 tensors, for parallellism with the scripting language.

	Initialization (tested in praat.cpp):
		VEC x;               // initializes x.at to nullptr and x.size to 0
		VEC x1 (100);        // initializes x to 100 uninitialized values
		VEC x2 (100, 0.0);   // initializes x to 100 zeroes
		NUMvector<double> a (1, 100);
		VEC x3 { a, 100 };   // initializes x to 100 values from a base-1 array

		autoVEC y;                     // initializes y.at to nullptr and y.size to 0
		autoVEC y1 (100);              // initializes y to 100 uninitialized values, having ownership
		autoVEC y2 (100, 0.0);         // initializes y to 100 zeroes, having ownership
		y.adoptFromAmbiguousOwner (x); // initializes y to the content of x, taking ownership (explicit, so not "y = x")
		VEC z = y.releaseToAmbiguousOwner();   // releases ownership, y.at becoming nullptr
		"}"                            // end of scope destroys y.at if not nullptr
		autoVEC z1 = y2.move()         // moves the content of y2 to z1, emptying y2

	To return an autoVEC from a function, transfer ownership like this:
		autoVEC foo () {
			autoVEC x (100);
			... // fill in the 100 values
			return x;
		}
*/

enum class kTensorInitializationType { RAW = 0, ZERO = 1 };

template <typename T>
class autovector;   // forward declaration, needed in the declaration of vector<>

template <typename T>
class vector {
public:
	T *at = nullptr;
	integer size = 0;
public:
	vector () = default;
	vector (T *givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	vector (const vector& other) = default;
	/*
		Letting an autovector convert to a vector would lead to errors such as in
			VEC vec = VECzero (10);
		where VECzero produces a temporary that is deleted immediately
		after the initialization of vec.
		So we rule out this initialization.
	*/
	vector (const autovector<T>& other) = delete;
	/*
		Likewise, an assignment like
			VEC vec1, vec2;
			vec1 = vec2;
		should be allowed...
	*/
	vector& operator= (const vector&) = default;
	/*
		but an assignment like
			autoVEC x = VECzero (10);
			VEC y;
			y = x;
		should be ruled out. Instead, one should do
			y = x.get();
		explicitly.
	*/
	vector& operator= (const autovector<T>&) = delete;
	T& operator[] (integer i) const {
		return our at [i];
	}
	vector<T> subview (integer first, integer last) const {
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 0 && last <= our size);
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return vector<T> ();
		return vector<T> (& our at [first - 1], newSize);
	}
	T *begin () const { return & our at [1]; }
	T *end () const { return & our at [our size + 1]; }
};

template <typename T>
class vectorview {
public:
	T * firstCell = nullptr;
	integer size = 0;
	integer stride = 1;
	vectorview (const vector<T>& other) :
			firstCell (& other.at [1]), size (other.size), stride (1) { }
	vectorview (T * const firstCell, integer const size, integer const stride) :
			firstCell (firstCell), size (size), stride (stride) { }
	T& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	vectorview<T> subview (integer first, integer last) const {
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 0 && last <= our size);
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return vectorview<T> ();
		return vectorview<T> (& our operator[] (first), newSize, our stride);
	}
	T *begin () const { return & our operator[] (1); }
	T *end () const { return & our operator[] (our size + 1); }
};

template <typename T>
class constvector {
public:
	const T *at = nullptr;
	integer size = 0;
	constvector () = default;
	constvector (const T *givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	constvector (vector<T> vec): at (vec.at), size (vec.size) { }
	//constvector (const constvector& other): at (other.at), size (other.size) { }
	//constvector& operator= (const constvector& other) {
	//	our at = other.at;
	//	our size = other.size;
	//}
	const T& operator[] (integer i) const {   // it's still a reference, because we need to be able to take its address
		return our at [i];
	}
	constvector<T> subview (integer first, integer last) const {
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 0 && last <= our size);
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return constvector<T> (nullptr, 0);
		return constvector<T> (& our at [first - 1], newSize);
	}
	const T *begin () const { return & our at [1]; }
	const T *end () const { return & our at [our size + 1]; }
};

template <typename T>
class constvectorview {
public:
	const T * firstCell = nullptr;
	integer size = 0;
	integer stride = 1;
	constvectorview (const constvector<T>& other) :
			firstCell (& other.at [1]), size (other.size), stride (1) { }
	constvectorview (const vector<T>& other) :
			firstCell (& other.at [1]), size (other.size), stride (1) { }
	constvectorview (const T * const firstCell, integer const size, integer const stride) :
			firstCell (firstCell), size (size), stride (stride) { }
	constvectorview (vectorview<T> vec): firstCell (vec.firstCell), size (vec.size), stride (vec.stride) { }
	T const& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	constvectorview<T> subview (integer first, integer last) const {
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 0 && last <= our size);
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return constvectorview<T> ();
		return constvectorview<T> (& our operator[] (first), newSize, our stride);
	}
	const T *begin () const { return & our operator[] (1); }
	const T *end () const { return & our operator[] (our size + 1); }
};

/*
	An autovector is the sole owner of its payload, which is a vector.
	When the autovector ends its life (goes out of scope),
	it should destroy its payload (if it has not sold it),
	because keeping a payload alive when the owner is dead
	would continue to use some of the computer's resources (namely, memory).
*/
template <typename T>
class autovector : public vector<T> {
	integer capacity = 0;
public:
	autovector (): vector<T> (nullptr, 0) { }   // come into existence without a payload
	autovector (integer givenSize, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		Melder_assert (givenSize >= 0);
		our at = ( givenSize == 0 ? nullptr
				: NUMvector<T> (1, givenSize, initializationType == kTensorInitializationType::ZERO) );
		our size = givenSize;
		our capacity = givenSize;
	}
	~autovector () {   // destroy the payload (if any)
		our reset ();
		our capacity = 0;
	}
	vector<T> get () const { return { our at, our size }; }   // let the public use the payload (they may change the values of the elements but not the at-pointer or the size)
	vectorview<T> all () const { return vectorview<T> (& our at [1], our size, 1); }
	void adoptFromAmbiguousOwner (vector<T> given) {   // buy the payload from a non-autovector
		our reset();
		our at = given.at;
		our size = given.size;
		our capacity = given.size;
	}
	vector<T> releaseToAmbiguousOwner () {   // sell the payload to a non-autovector
		T *oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		integer oldSize = our size;
		our capacity = 0;
		return { oldAt, oldSize };
	}
	/*
		Disable copying via construction or assignment (which would violate unique ownership of the payload).
	*/
	autovector (const autovector&) = delete;   // disable copy constructor
	autovector& operator= (const autovector&) = delete;   // disable copy assignment
	/*
		Enable moving of r-values (temporaries, implicitly) or l-values (for variables, via an explicit move()).
		This implements buying a payload from another autovector (which involves destroying our current payload).
	*/
	autovector (autovector&& other) noexcept : vector<T> { other.get() } {   // enable move constructor
		other.at = nullptr;   // disown source
		other.size = 0;   // to keep the source in a valid state
		other.capacity = 0;
	}
	autovector& operator= (autovector&& other) noexcept {   // enable move assignment
		if (other.at != our at) {
			our reset ();
			our at = other.at;
			our size = other.size;
			our capacity = other.capacity;
			other.at = nullptr;   // disown source
			other.size = 0;   // to keep the source in a valid state
			other.capacity = 0;
		}
		return *this;
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autovector<>)
		if (our at) {
			NUMvector_free (our at, 1);
			our at = nullptr;
		}
		our size = 0;
	}
	autovector&& move () noexcept { return static_cast <autovector&&> (*this); }   // enable constriction and assignment for l-values (variables) via explicit move()
	/*
		Unlike std::vector, our vector is not really designed for dynamic resizing,
		i.e. it has no `capacity` member. This is because our vectors should mainly feel happy
		in an environment with matrixes, tensor3s and tensor4s, for which dynamic resizing
		makes little sense.
		The following functions, however, do support dynamic resizing,
		but the capacity should be kept in an external integer.

		Some of these functions are capable of keeping a valid `at` pointer
		while `size` can at the same time be zero.
	*/
	void initWithCapacity (integer *inout_capacity, kTensorInitializationType initializationType = kTensorInitializationType::ZERO) {
		if (*inout_capacity > 0)
			our at = NUMvector<T> (1, *inout_capacity, initializationType == kTensorInitializationType::ZERO);
		our size = 0;
		our capacity = *inout_capacity;
	}
	/*
		If the new size N is less than the current size S,
		then the first N elements of the vector are kept,
		so if you want to keep a different range than the
		first N elements of your original vector,
		you should shift the elements before resizing.

		If the new size N is greater than the current size S,
		then all S elements of the vector are kept,
		and they are the first S elements of the new vector
		the remaining S - N elements may be initialized to zero.
		If you want the original S elements to show up
		elsewhere than at the head of the vector,
		you should shift the elements after resizing.
	*/
	void resize (integer newSize, integer *inout_capacity = nullptr,
		kTensorInitializationType initializationType = kTensorInitializationType::ZERO)
	{
		const integer currentCapacity = ( inout_capacity ? *inout_capacity : our size );
		if (newSize > currentCapacity) {
			/*
				The new capacity is at least twice the old capacity.
				When starting at a capacity of 0, and continually upsizing by one,
				the capacity sequence will be: 0, 11, 33, 77, 165, 341, 693, 1397,
				2805, 5621, 11253, 22517, 45045, 90101, 180213, 360437, 720885...
			*/
			integer newCapacity = ( inout_capacity ? newSize + our size + 10 : newSize );
			/*
				Create without change.
			*/
			T *newAt = NUMvector<T> (1, newCapacity, initializationType == kTensorInitializationType::ZERO);
			/*
				Change without error.
			*/
			for (integer i = 1; i <= our size; i ++)
				newAt [i] = our at [i];
			if (our at) NUMvector_free (our at, 1);
			our at = newAt;
			if (inout_capacity)
				*inout_capacity = newCapacity;
			our capacity = newCapacity;
		}
		our size = newSize;
	}
	void insert (integer position, const T& value, integer *inout_capacity = nullptr) {
		resize (our size + 1, inout_capacity, kTensorInitializationType::RAW);
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = our size; i > position; i --)
			our at [i] = our at [i - 1];
		our at [position] = value;
	}
	void remove (integer position) {
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = position; i < our size; i ++)
			our at [i] = our at [i + 1];
		resize (our size - 1);
	}
};

template <typename T>
autovector<T> vectorraw (integer size) {
	return autovector<T> (size, kTensorInitializationType::RAW);
}
template <typename T>
autovector<T> vectorzero (integer size) {
	return autovector<T> (size, kTensorInitializationType::ZERO);
}
template <typename T>
void vectorcopy_preallocated (vector<T> target, constvector<T> source) {
	Melder_assert (source.size == target.size);
	for (integer i = 1; i <= source.size; i ++)
		target [i] = source [i];
}
template <typename T>
void vectorcopy_preallocated (vector<T> target, vector<T> source) {
	vectorcopy_preallocated (target, constvector<T> (source));
}
template <typename T>
autovector<T> vectorcopy (constvector<T> source) {
	autovector<T> result = vectorraw<T> (source.size);
	vectorcopy_preallocated (result.get(), source);
	return result;
}
template <typename T>
autovector<T> vectorcopy (vector<T> source) {
	return vectorcopy (constvector<T> (source));
}

template <typename T>
class automatrix;   // forward declaration, needed in the declaration of matrix

#define PACKED_TENSORS  0

template <typename T>
class matrix {
public:
	#if PACKED_TENSORS
	T *cells = nullptr;
	#else
	T **at = nullptr;
	#endif
	integer nrow = 0, ncol = 0;
public:
	matrix () = default;
	#if PACKED_TENSORS
	matrix (T *givenCells, integer givenNrow, integer givenNcol): cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	#else
	matrix (T **givenAt, integer givenNrow, integer givenNcol): at (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	#endif
	matrix (const matrix& other) = default;
	matrix (const automatrix<T>& other) = delete;
	matrix& operator= (const matrix&) = default;
	matrix& operator= (const automatrix<T>&) = delete;
	#if PACKED_TENSORS
	vector<T> operator[] (integer i) const {
		return vector (our cells + (i - 1) * our ncol, our ncol);
	}
	#else
	T * const & operator[] (integer i) const {
		return our at [i];
	}
	#endif
	vector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		#if PACKED_TENSORS
		return vector (our cells + (rowNumber - 1) * our ncol, our ncol);
		#else
		return vector<T> (our at [rowNumber], our ncol);
		#endif
	}
	vectorview<T> column (const integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return vectorview<T> (& our at [1] [columnNumber], our nrow, our ncol);
	}
	matrix<T> horizontalBand (integer firstRow, integer lastRow) const {
		const integer offsetRow = firstRow - 1;
		Melder_assert (offsetRow >= 0 && offsetRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		const integer newNrow = lastRow - offsetRow;
		if (newNrow <= 0) return matrix<T> ();
		return matrix<T> (& our at [offsetRow], newNrow, our ncol);
	}
};

template <typename T>
class matrixview {
public:
	T * firstCell = nullptr;
	integer nrow = 0, ncol = 0;
	/*mutable*/ integer rowStride = 0, colStride = 1;   // mutable perhaps once an automatrix has strides
	matrixview (const matrix<T>& other) :
			firstCell (& other.at [1] [1]), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	matrixview (T * const firstCell, integer const nrow, integer const ncol, integer const rowStride, integer const colStride) :
			firstCell (firstCell), nrow (nrow), ncol (ncol), rowStride (rowStride), colStride (colStride) { }
	vectorview<T> operator[] (integer i) const {
		return vectorview<T> (our firstCell + (i - 1) * our rowStride, our ncol, our colStride);
	}
	vectorview<T> column (integer columnNumber) const {
		return vectorview<T> (our firstCell + (columnNumber - 1) * our colStride, our nrow, our rowStride);
	}
};

template <typename T>
class constmatrix {
public:
	#if PACKED_TENSORS
	const T *cells = nullptr;
	#else
	const T * const * at = nullptr;
	#endif
	integer nrow = 0, ncol = 0;
	constmatrix () = default;
	#if PACKED_TENSORS
	constmatrix (const T *givenCells, integer givenNrow, integer givenNcol): cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	#else
	constmatrix (const T * const *givenAt, integer givenNrow, integer givenNcol): at (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	#endif
	#if PACKED_TENSORS
	constmatrix (matrix<T> mat): cells (mat.cells), nrow (mat.nrow), ncol (mat.ncol) { }
	#else
	constmatrix (matrix<T> mat): at (mat.at), nrow (mat.nrow), ncol (mat.ncol) { }
	#endif
	#if PACKED_TENSORS
	const T * operator[] (integer i) const {
		return our cells + (i - 1) * our ncol;
	}
	#else
	const T * const & operator[] (integer i) const {
		return our at [i];
	}
	#endif
	constvector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		return constvector<T> (our at [rowNumber], our ncol);
	}
	constmatrix<T> horizontalBand (integer firstRow, integer lastRow) const {
		const integer offsetRow = firstRow - 1;
		Melder_assert (offsetRow >= 0 && offsetRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		const integer newNrow = lastRow - offsetRow;
		if (newNrow <= 0) return matrix<T> (nullptr, 0, 0);
		return constmatrix<T> (& our at [offsetRow], newNrow, our ncol);
	}
};

template <typename T>
class constmatrixview {
public:
	const T * firstCell = nullptr;
	integer nrow = 0, ncol = 0;
	integer rowStride = 0, colStride = 1;
	constmatrixview (const constmatrix<T>& other) :
			firstCell (& other.at [1] [1]), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	constmatrixview (const matrix<T>& other) :
			firstCell (& other.at [1] [1]), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	constmatrixview (const T * const firstCell, integer const nrow, integer const ncol, integer const rowStride, integer const colStride) :
			firstCell (firstCell), nrow (nrow), ncol (ncol), rowStride (rowStride), colStride (colStride) { }
	constmatrixview (matrixview<T> mat) :
			firstCell (mat.firstCell), nrow (mat.nrow), ncol (mat.ncol), rowStride (mat.rowStride), colStride (mat.colStride) { }
	constvectorview<T> operator[] (integer i) const {
		return constvectorview<T> (our firstCell + (i - 1) * our rowStride, our ncol, our colStride);
	}
	constvectorview<T> column (integer columnNumber) const {
		return constvectorview<T> (our firstCell + (columnNumber - 1) * our colStride, our nrow, our rowStride);
	}
};

/*
	An automatrix is the sole owner of its payload, which is a matrix.
	When the automatrix ends its life (goes out of scope),
	it should destroy its payload (if it has not sold it),
	because keeping a payload alive when the owner is dead
	would continue to use some of the computer's resources (namely, memory).
*/
template <typename T>
class automatrix : public matrix<T> {
public:
	automatrix (): matrix<T> { nullptr, 0, 0 } { }   // come into existence without a payload
	automatrix (integer givenNrow, integer givenNcol, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		Melder_assert (givenNrow >= 0);
		Melder_assert (givenNcol >= 0);
		#if PACKED_TENSORS
		our cells = ( givenNrow == 0 || givenNcol == 0 ? nullptr
				: NUMvector<T> (1, givenNrow * givenNcol, initializationType == kTensorInitializationType::ZERO));
		#else
		our at = ( givenNrow == 0 || givenNcol == 0 ? nullptr
				: NUMmatrix<T> (1, givenNrow, 1, givenNcol, initializationType == kTensorInitializationType::ZERO));
		#endif
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	~automatrix () {   // destroy the payload (if any)
		#if PACKED_TENSORS
		if (our cells) NUMvector_free (our cells, 1);
		#else
		if (our at) NUMmatrix_free (our at, 1, 1);
		#endif
	}
	//matrix<T> get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	const matrix<T>& get () { return *this; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	matrixview<T> all () const { return matrixview<T> (& our at [1] [1], our nrow, our ncol, our ncol, 1); }
	void adoptFromAmbiguousOwner (matrix<T> given) {   // buy the payload from a non-automatrix
		our reset();
		#if PACKED_TENSORS
		our cells = given.cells;
		#else
		our at = given.at;
		#endif
		our nrow = given.nrow;
		our ncol = given.ncol;
	}
	matrix<T> releaseToAmbiguousOwner () {   // sell the payload to a non-automatrix
		T **oldAt = our at;
		our at = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return { oldAt, our nrow, our ncol };
	}
	/*
		Disable copying via construction or assignment (which would violate unique ownership of the payload).
	*/
	automatrix (const automatrix&) = delete;   // disable copy constructor
	automatrix& operator= (const automatrix&) = delete;   // disable copy assignment
	/*
		Enable moving of r-values (temporaries, implicitly) or l-values (for variables, via an explicit move()).
		This implements buying a payload from another automatrix (which involves destroying our current payload).
	*/
	automatrix (automatrix&& other) noexcept : matrix<T> { other.get() } {   // enable move constructor
		#if PACKED_TENSORS
		other.cells = nullptr;   // disown source
		#else
		other.at = nullptr;   // disown source
		#endif
		other.nrow = 0;   // to keep the source in a valid state
		other.ncol = 0;   // to keep the source in a valid state
	}
	automatrix& operator= (automatrix&& other) noexcept {   // enable move assignment
		#if PACKED_TENSORS
		if (other.cells != our cells) {
			if (our cells) NUMvector_free (our cells, 1);
			our cells = other.cells;
			our nrow = other.nrow;
			our ncol = other.ncol;
			other.cells = nullptr;   // disown source
			other.nrow = 0;   // to keep the source in a valid state
			other.ncol = 0;   // to keep the source in a valid state
		}
		#else
		if (other.at != our at) {
			if (our at) NUMmatrix_free (our at, 1, 1);
			our at = other.at;
			our nrow = other.nrow;
			our ncol = other.ncol;
			other.at = nullptr;   // disown source
			other.nrow = 0;   // to keep the source in a valid state
			other.ncol = 0;   // to keep the source in a valid state
		}
		#endif
		return *this;
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autoMAT)
		#if PACKED_TENSORS
		if (our cells) {
			NUMvector_free (our cells, 1);
			our cells = nullptr;
		}
		#else
		if (our at) {
			NUMmatrix_free (our at, 1, 1);
			our at = nullptr;
		}
		#endif
		our nrow = 0;
		our ncol = 0;
	}
	automatrix&& move () noexcept { return static_cast <automatrix&&> (*this); }
};

template <typename T>
automatrix<T> matrixraw (integer nrow, integer ncol) {
	return automatrix<T> (nrow, ncol, kTensorInitializationType::RAW);
}
template <typename T>
automatrix<T> matrixzero (integer nrow, integer ncol) {
	return automatrix<T> (nrow, ncol, kTensorInitializationType::ZERO);
}
template <typename T>
vector<T> asvector (matrix<T> x) {
	#if PACKED_TENSORS
	return vector<T> (x.cells, x.nrow * x.ncol);
	#else
	return vector<T> (x [1], x.nrow * x.ncol);
	#endif
}
template <typename T>
constvector<T> asvector (constmatrix<T> x) {
	return constvector<T> (x [1], x.nrow * x.ncol);
}
template <typename T>
void matrixcopy_preallocated (matrix<T> target, constmatrix<T> source) {
	Melder_assert (source.nrow == target.nrow && source.ncol == target.ncol);
	for (integer irow = 1; irow <= source.nrow; irow ++)
		for (integer icol = 1; icol <= source.ncol; icol ++)
			target [irow] [icol] = source [irow] [icol];
}
template <typename T>
void matrixcopy_preallocated (matrix<T> target, matrix<T> source) {
	matrixcopy_preallocated (target, constmatrix<T> (source));
}
template <typename T>
automatrix<T> matrixcopy (constmatrix<T> source) {
	automatrix<T> result = matrixraw<T> (source.nrow, source.ncol);
	matrixcopy_preallocated (result.get(), source);
	return result;
}
template <typename T>
automatrix<T> matrixcopy (matrix<T> source) {
	return matrixcopy (constmatrix<T> (source));
}

template <typename T>
void assertCell (const constvector<T>& x, integer elementNumber) {
	Melder_assert (elementNumber >= 1 && elementNumber <= x.size);
}
template <typename T>
void assertRow (const constmatrix<T>& x, integer rowNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= x.nrow);
}
template <typename T>
void assertColumn (const constmatrix<T>& x, integer columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= x.nrow);
}
template <typename T>
void assertCell (const constmatrix<T>& x, integer rowNumber, integer columnNumber) {
	assertRow (x, rowNumber);
	assertColumn (x, columnNumber);
}
template <typename T>
automatrix<T> matrixpart (const constmatrix<T>& x, integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) {
	assertCell (x, firstRow, firstColumn);
	assertCell (x, lastRow, lastColumn);
	integer numberOfRows = lastRow - firstRow + 1;
	Melder_assert (numberOfRows >= 0);
	integer numberOfColumns = lastColumn - firstColumn + 1;
	Melder_assert (numberOfColumns >= 0);
	automatrix<T> result = matrixraw<T> (numberOfRows, numberOfColumns);
	for (integer irow = 1; irow <= numberOfRows; irow ++)
		for (integer icol = 1; icol <= numberOfColumns; icol ++)
			result [irow] [icol] = x [firstRow - 1 + irow] [firstColumn - 1 + icol];
	return result;
}
template <typename T>
automatrix<T> matrixpart (const matrix<T>& x, integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) {
	matrixpart (constmatrix<T> (x), firstRow, lastRow, firstColumn, lastColumn);
}

/*
	instead of vector<double> we say VEC, because we want to have a one-to-one
	relation between VEC functions and the scripting language.
	For instance, we make VECraw and VECzero because Praat scripting has raw# and zero#.
*/
using VEC = vector <double>;
using VECVU = vectorview <double>;
using constVEC = constvector <double>;
using constVECVU = constvectorview <double>;
using autoVEC = autovector <double>;
inline autoVEC VECraw  (integer size) { return vectorraw  <double> (size); }
inline autoVEC VECzero (integer size) { return vectorzero <double> (size); }
inline void VECcopy_preallocated (VEC target, constVEC source) { vectorcopy_preallocated (target, source); }
inline autoVEC VECcopy (constVEC source) { return vectorcopy (source); }

/*
	And simply because we use vector<integer> so much as well,
	we have an abbreviation for that as well, namely INTVEC.
	But the scripting language has nothing that corresponds to INTVEC,
	so any numeric vector to be used by the scripting language
	should be a VEC, even if it contains integers.
	This is fine, as a double can contain an integer up to 54 bits.
*/
using INTVEC = vector <integer>;
using constINTVEC = constvector <integer>;
using autoINTVEC = autovector <integer>;
inline autoINTVEC INTVECraw  (integer size) { return vectorraw  <integer> (size); }
inline autoINTVEC INTVECzero (integer size) { return vectorzero <integer> (size); }
inline void INTVECcopy_preallocated (INTVEC target, constINTVEC source) { vectorcopy_preallocated (target, source); }
inline autoINTVEC INTVECcopy (constINTVEC source) { return vectorcopy (source); }

#define emptyVEC  VEC (nullptr, 0)
#define emptyINTVEC  INTVEC (nullptr, 0)

using MAT = matrix <double>;
using MATVU = matrixview <double>;
using constMAT = constmatrix <double>;
using constMATVU = constmatrixview <double>;
using autoMAT = automatrix <double>;
inline autoMAT MATraw  (integer nrow, integer ncol) { return matrixraw  <double> (nrow, ncol); }
inline autoMAT MATzero (integer nrow, integer ncol) { return matrixzero <double> (nrow, ncol); }
inline autoMAT MATcopy (constMAT source) { return matrixcopy (source); }
inline autoMAT MATpart (const constMAT& source, integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) {
	return matrixpart (source, firstRow, lastRow, firstColumn, lastColumn);
}

using INTMAT = matrix <integer>;
using constINTMAT = constmatrix <integer>;
using autoINTMAT = automatrix <integer>;
inline autoINTMAT INTMATraw  (integer nrow, integer ncol) { return matrixraw  <integer> (nrow, ncol); }
inline autoINTMAT INTMATzero (integer nrow, integer ncol) { return matrixzero <integer> (nrow, ncol); }
inline autoINTMAT INTMATcopy (constINTMAT source) { return matrixcopy (source); }

#define emptyMAT  MAT (nullptr, 0, 0)
#define emptyINTMAT  INTMAT (nullptr, 0, 0)

inline constMATVU constMATVUtranspose (const constMATVU& mat) {
	return constMATVU (& mat [1] [1], mat.ncol, mat.nrow, mat.colStride, mat.rowStride);
}

conststring32 Melder_VEC (constVEC value);
conststring32 Melder_MAT (constMAT value);

/* End of file melder_tensor.h */
#endif
