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
	explicit vector (T *givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	vector (const vector& other) = default;
	/*
		Letting an autovector convert to a vector would lead to errors such as in
			VEC vec = newVECzero (10);
		where newVECzero produces a temporary that is deleted immediately
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
	//vector& operator= (const vector&) = delete;
	vector& operator= (const vector&) = default;
	/*
		but an assignment like
			autoVEC x = newVECzero (10);
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
	vector<T> part (integer first, integer last) const {
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
	explicit vectorview (T * const firstCell_, integer const size_, integer const stride_) :
			firstCell (firstCell_), size (size_), stride (stride_) { }
	T& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	vectorview<T> part (integer first, integer last) const {
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
	explicit constvector (const T *givenAt, integer givenSize): at (givenAt), size (givenSize) { }
	constvector (vector<T> vec): at (vec.at), size (vec.size) { }
	//constvector (const constvector& other): at (other.at), size (other.size) { }
	//constvector& operator= (const constvector& other) {
	//	our at = other.at;
	//	our size = other.size;
	//}
	const T& operator[] (integer i) const {   // it's still a reference, because we need to be able to take its address
		return our at [i];
	}
	constvector<T> part (integer first, integer last) const {
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
	explicit constvectorview (const T * const firstCell_, integer const size_, integer const stride_) :
			firstCell (firstCell_), size (size_), stride (stride_) { }
	constvectorview (vectorview<T> vec): firstCell (vec.firstCell), size (vec.size), stride (vec.stride) { }
	T const& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	constvectorview<T> part (integer first, integer last) const {
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
	explicit autovector (integer givenSize, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
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
	vector<T> get () const { return vector<T> (our at, our size); }   // let the public use the payload (they may change the values of the elements but not the at-pointer or the size)
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
		return vector<T> (oldAt, oldSize);
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
autovector<T> newvectorraw (integer size) {
	return autovector<T> (size, kTensorInitializationType::RAW);
}
template <typename T>
autovector<T> newvectorzero (integer size) {
	return autovector<T> (size, kTensorInitializationType::ZERO);
}
template <typename T>
autovector<T> newvectorcopy (constvector<T> source) {
	autovector<T> result = newvectorraw<T> (source.size);
	for (integer i = 1; i <= source.size; i ++)
		result [i] = source [i];
	return result;
}
template <typename T>
autovector<T> newvectorcopy (vector<T> source) {
	return newvectorcopy (constvector<T> (source));
}

template <typename T>
class automatrix;   // forward declaration, needed in the declaration of matrix
template <typename T>
class matrixview;
template <typename T>
class constmatrixview;

#define PACKED_TENSORS  0

template <typename T>
class matrix {
public:
	T **at_deprecated = nullptr;   // deprecated; WATCH OUT: when removed, change MatrixEditor
	integer nrow = 0, ncol = 0;
	T *cells = nullptr;   // the future
public:
	matrix () = default;
	//matrix (T *givenCells, integer givenNrow, integer givenNcol) :
	//		cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	explicit matrix (T **givenAt, integer givenNrow, integer givenNcol) :
			cells (givenAt ? & givenAt [1] [1] : nullptr), at_deprecated (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	matrix (const matrix& other) = default;
	matrix (const automatrix<T>& other) = delete;
	matrix& operator= (const matrix&) = default;
	matrix& operator= (const automatrix<T>&) = delete;
	vector<T> operator[] (integer rowNumber) const {
		return vector<T> (our cells + (rowNumber - 1) * our ncol - 1, our ncol);
	}
	vector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		Melder_assert (our at_deprecated);
		Melder_assert (our cells);
		return vector<T> (our cells + (rowNumber - 1) * our ncol - 1, our ncol);
	}
	vectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return vectorview<T> (our cells + (columnNumber - 1), our nrow, our ncol);
	}
	vectorview<T> diagonal () const {
		return vectorview<T> (our cells, std::min (our nrow, our ncol), our ncol + 1);
	}
	matrix<T> horizontalBand (integer firstRow, integer lastRow) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		const integer newNrow = lastRow - (firstRow - 1);
		if (newNrow <= 0) return matrix<T> ();
		return matrix<T> (& our at_deprecated [firstRow - 1], newNrow, our ncol);
	}
	matrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return matrixview<T> ();
		return matrixview<T> (our cells + (firstColumn - 1), our nrow, newNcol, our ncol, 1);
	}
	matrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return matrixview<T> ();
		return matrixview<T> (
			our cells + (firstRow - 1) * our ncol + (firstColumn - 1),
			newNrow, newNcol, our ncol, 1
		);
	}
	matrixview<T> transpose () {
		return matrixview<T> (our cells, our ncol, our nrow, 1, our ncol);
	}
};

template <typename T>
class matrixview {
public:
	T * firstCell = nullptr;
	integer nrow = 0, ncol = 0;
	/*mutable*/ integer rowStride = 0, colStride = 1;   // mutable perhaps once an automatrix has strides
	matrixview () = default;
	matrixview (const matrix<T>& other) :
			firstCell (other.cells), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	matrixview (const automatrix<T>& other) = delete;
	explicit matrixview (T * const firstCell_, integer const nrow_, integer const ncol_, integer const rowStride_, integer const colStride_) :
			firstCell (firstCell_), nrow (nrow_), ncol (ncol_), rowStride (rowStride_), colStride (colStride_) { }
	vectorview<T> operator[] (integer rowNumber) const {
		return vectorview<T> (our firstCell + (rowNumber - 1) * our rowStride, our ncol, our colStride);
	}
	vectorview<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		return vectorview<T> (our firstCell + (rowNumber - 1) * our rowStride, our ncol, our colStride);
	}
	vectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return vectorview<T> (our firstCell + (columnNumber - 1) * our colStride, our nrow, our rowStride);
	}
	vectorview<T> diagonal () const {
		return vectorview<T> (our firstCell, std::min (our nrow, our ncol), our rowStride + our colStride);
	}
	matrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return matrixview<T> ();
		return matrixview<T> (our firstCell + (firstColumn - 1) * our colStride,
				our nrow, newNcol, our rowStride, our colStride);
	}
	matrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return matrixview<T> ();
		return matrixview<T> (
			our firstCell + (firstRow - 1) * our rowStride + (firstColumn - 1) * our colStride,
			newNrow, newNcol, our rowStride, our colStride
		);
	}
	matrixview<T> transpose () {
		return matrixview<T> (our firstCell, our ncol, our nrow, our colStride, our rowStride);
	}
};

template <typename T>
class constmatrix {
public:
	const T *cells = nullptr;
	const T * const * at_deprecated = nullptr;
	integer nrow = 0, ncol = 0;
	constmatrix () = default;
	//constmatrix (const T *givenCells, integer givenNrow, integer givenNcol): cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	explicit constmatrix (const T * const *givenAt, integer givenNrow, integer givenNcol) :
			cells (givenAt ? & givenAt [1] [1] : nullptr), at_deprecated (givenAt), nrow (givenNrow), ncol (givenNcol) { }
	constmatrix (matrix<T> mat) :
			cells (mat.cells), at_deprecated (mat.at_deprecated), nrow (mat.nrow), ncol (mat.ncol) { }

	constvector<T> operator[] (integer rowNumber) const {
		return constvector<T> (our cells + (rowNumber - 1) * our ncol - 1, our ncol);
	}
	constvector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		Melder_assert (our at_deprecated);
		Melder_assert (our cells);
		return constvector<T> (our cells + (rowNumber - 1) * our ncol - 1, our ncol);
	}
	constvectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return constvectorview<T> (our cells + (columnNumber - 1), our nrow, our ncol);
	}
	constvectorview<T> diagonal () const {
		return constvectorview<T> (our cells, std::min (our nrow, our ncol), our ncol + 1);
	}
	constmatrix<T> horizontalBand (integer firstRow, integer lastRow) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		const integer newNrow = lastRow - (firstRow - 1);
		if (newNrow <= 0) return constmatrix<T> ();
		return constmatrix<T> (our cells + (firstRow - 1) * our ncol, newNrow, our ncol);
	}
	constmatrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return constmatrixview<T> ();
		return constmatrixview<T> (our cells + (firstColumn - 1), our nrow, newNcol, our ncol, 1);
	}
	constmatrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return constmatrixview<T> ();
		return constmatrixview<T> (
			our cells + (firstRow - 1) * our ncol + (firstColumn - 1),
			newNrow, newNcol, our ncol, 1
		);
	}
	constmatrixview<T> transpose () {
		return constmatrixview<T> (our cells, our ncol, our nrow, 1, our ncol);
	}
};

template <typename T>
class constmatrixview {
public:
	const T * firstCell = nullptr;
	integer nrow = 0, ncol = 0;
	integer rowStride = 0, colStride = 1;
	constmatrixview () = default;
	constmatrixview (const constmatrix<T>& other) :
			firstCell (other. cells), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	constmatrixview (const matrix<T>& other) :
			firstCell (other. cells), nrow (other.nrow), ncol (other.ncol), rowStride (other.ncol), colStride (1) { }
	constmatrixview (const automatrix<T>& other) = delete;
	explicit constmatrixview (const T * const firstCell_, integer const nrow_, integer const ncol_, integer const rowStride_, integer const colStride_) :
			firstCell (firstCell_), nrow (nrow_), ncol (ncol_), rowStride (rowStride_), colStride (colStride_) { }
	constmatrixview (matrixview<T> mat) :
			firstCell (mat.firstCell), nrow (mat.nrow), ncol (mat.ncol), rowStride (mat.rowStride), colStride (mat.colStride) { }
	constvectorview<T> operator[] (integer i) const {
		return constvectorview<T> (our firstCell + (i - 1) * our rowStride, our ncol, our colStride);
	}
	constvectorview<T> column (integer columnNumber) const {
		return constvectorview<T> (our firstCell + (columnNumber - 1) * our colStride, our nrow, our rowStride);
	}
	constvectorview<T> diagonal () const {
		return constvectorview<T> (our firstCell, std::min (our nrow, our ncol), our rowStride + our colStride);
	}
	constmatrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return constmatrixview<T> ();
		return constmatrixview<T> (our firstCell + (firstColumn - 1) * our colStride,
				our nrow, newNcol, our rowStride, our colStride);
	}
	constmatrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 0 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 0 && lastColumn <= our ncol);
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return constmatrixview<T> ();
		return constmatrixview<T> (
			our firstCell
			+ (firstRow - 1) * our rowStride
			+ (firstColumn - 1) * our colStride,
			newNrow, newNcol,
			our rowStride, our colStride
		);
	}
	constmatrixview<T> transpose () {
		return constmatrixview<T> (our firstCell, our ncol, our nrow, our colStride, our rowStride);
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
	explicit automatrix (integer givenNrow, integer givenNcol, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		Melder_assert (givenNrow >= 0);
		Melder_assert (givenNcol >= 0);
		#if PACKED_TENSORS
		our cells = ( givenNrow == 0 || givenNcol == 0 ? nullptr
				: NUMvector<T> (0, givenNrow * givenNcol - 1, initializationType == kTensorInitializationType::ZERO));
		#else
		our at_deprecated = ( givenNrow == 0 || givenNcol == 0 ? nullptr
				: NUMmatrix<T> (1, givenNrow, 1, givenNcol, initializationType == kTensorInitializationType::ZERO));
		our cells = our at_deprecated ? & our at_deprecated [1] [1] : nullptr;
		#endif
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	~automatrix () {   // destroy the payload (if any)
		#if PACKED_TENSORS
		if (our cells) NUMvector_free (our cells, 1);
		#else
		if (our at_deprecated) NUMmatrix_free (our at_deprecated, 1, 1);
		#endif
	}
	//matrix<T> get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	const matrix<T>& get () { return *this; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	matrixview<T> all () const {
		#if PACKED_TENSORS
		return matrixview<T> (our cells, our nrow, our ncol, our ncol, 1);
		#else
		return matrixview<T> (& our at_deprecated [1] [1], our nrow, our ncol, our ncol, 1);
		#endif
	}
	void adoptFromAmbiguousOwner (matrix<T> given) {   // buy the payload from a non-automatrix
		our reset();
		our cells = given.cells;
		our at_deprecated = given.at_deprecated;
		our nrow = given.nrow;
		our ncol = given.ncol;
	}
	matrix<T> releaseToAmbiguousOwner () {   // sell the payload to a non-automatrix
		T **oldAt = our at_deprecated;
		our at_deprecated = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		our cells = nullptr;
		return matrix<T> (oldAt, our nrow, our ncol);
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
		other.cells = nullptr;   // disown source
		other.at_deprecated = nullptr;   // disown source
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
		if (other.at_deprecated != our at_deprecated) {
			if (our at_deprecated) NUMmatrix_free (our at_deprecated, 1, 1);
			our at_deprecated = other.at_deprecated;
			our cells = other.cells;
			our nrow = other.nrow;
			our ncol = other.ncol;
			other.at_deprecated = nullptr;   // disown source
			other.cells = nullptr;   // disown source
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
		if (our at_deprecated) {
			NUMmatrix_free (our at_deprecated, 1, 1);
			our at_deprecated = nullptr;
			our cells = nullptr;
		}
		#endif
		our nrow = 0;
		our ncol = 0;
	}
	automatrix&& move () noexcept { return static_cast <automatrix&&> (*this); }
};

template <typename T>
automatrix<T> newmatrixraw (integer nrow, integer ncol) {
	return automatrix<T> (nrow, ncol, kTensorInitializationType::RAW);
}
template <typename T>
automatrix<T> newmatrixzero (integer nrow, integer ncol) {
	return automatrix<T> (nrow, ncol, kTensorInitializationType::ZERO);
}
template <typename T>
vector<T> asvector (matrix<T> const& x) {
	#if PACKED_TENSORS
	return vector<T> (x.cells, x.nrow * x.ncol);
	#else
	return vector<T> (& x [1] [0], x.nrow * x.ncol);
	#endif
}
template <typename T>
constvector<T> asvector (constmatrix<T> const& x) {
	return constvector<T> (& x [1] [0], x.nrow * x.ncol);
}
template <typename T>
void matrixcopy_preallocated (matrixview<T> const& target, constmatrixview<T> const& source) {
	Melder_assert (source.nrow == target.nrow && source.ncol == target.ncol);
	for (integer irow = 1; irow <= source.nrow; irow ++)
		for (integer icol = 1; icol <= source.ncol; icol ++)
			target [irow] [icol] = source [irow] [icol];
}
template <typename T>
void matrixcopy_preallocated (matrixview<T> const& target, matrixview<T> const& source) {
	matrixcopy_preallocated (target, constmatrixview<T> (source));
}
template <typename T>
automatrix<T> newmatrixcopy (constmatrixview<T> const& source) {
	automatrix<T> result = newmatrixraw<T> (source.nrow, source.ncol);
	matrixcopy_preallocated (result.all(), source);
	return result;
}
template <typename T>
automatrix<T> newmatrixcopy (matrixview<T> const& source) {
	return newmatrixcopy<T> (constmatrixview<T> (source));
}

template <typename T>
void assertCell (constvector<T> const& x, integer elementNumber) {
	Melder_assert (elementNumber >= 1 && elementNumber <= x.size);
}
template <typename T>
void assertRow (constmatrix<T> const& x, integer rowNumber) {
	Melder_assert (rowNumber >= 1 && rowNumber <= x.nrow);
}
template <typename T>
void assertColumn (constmatrix<T> const& x, integer columnNumber) {
	Melder_assert (columnNumber >= 1 && columnNumber <= x.ncol);
}
template <typename T>
void assertCell (constmatrix<T> const& x, integer rowNumber, integer columnNumber) {
	assertRow (x, rowNumber);
	assertColumn (x, columnNumber);
}
template <typename T>
automatrix<T> newmatrixpart (constmatrix<T> const& x, integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) {
	assertCell (x, firstRow, firstColumn);
	assertCell (x, lastRow, lastColumn);
	integer numberOfRows = lastRow - (firstRow - 1);
	Melder_assert (numberOfRows >= 0);
	integer numberOfColumns = lastColumn - (firstColumn - 1);
	Melder_assert (numberOfColumns >= 0);
	automatrix<T> result = newmatrixraw<T> (numberOfRows, numberOfColumns);
	for (integer irow = 1; irow <= numberOfRows; irow ++)
		for (integer icol = 1; icol <= numberOfColumns; icol ++)
			result [irow] [icol] = x [firstRow - 1 + irow] [firstColumn - 1 + icol];
	return result;
}
template <typename T>
automatrix<T> newmatrixpart (matrix<T> const& x, integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) {
	newmatrixpart (constmatrix<T> (x), firstRow, lastRow, firstColumn, lastColumn);
}

template <typename T>
class autotensor3;

template <typename T>
class tensor3 {
public:
	T * cells = nullptr;
	integer ndim1 = 0, ndim2 = 0, ndim3 = 0;
	integer stride1 = 0, stride2 = 0, stride3 = 1;
	tensor3 () = default;
	tensor3 (const autotensor3<T>& other) = delete;
	explicit tensor3 (T * cells_,
		integer ndim1_, integer ndim2_, integer ndim3_,
		integer stride1_, integer stride2_, integer stride3_
	) :
		cells (cells_),
		ndim1 (ndim1_), ndim2 (ndim2_), ndim3 (ndim3_),
		stride1 (stride1_), stride2 (stride2_), stride3 (stride3_)
	{ }
	matrixview<T> operator[] (integer dim1) const {
		return matrixview<T> (our cells + (dim1 - 1) * our stride1, our ndim2, our ndim3, our stride2, our stride3);
	}
	vectorview<T> line1 (integer dim2, integer dim3) const {
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return vectorview<T> (
			our cells
			+ (dim2 - 1) * our stride2
			+ (dim3 - 1) * our stride3,
			our ndim1,
			our stride1
		);
	}
	vectorview<T> line2 (integer dim1, integer dim3) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return vectorview<T> (
			our cells
			+ (dim1 - 1) * our stride1
			+ (dim3 - 1) * our stride3,
			our ndim2,
			our stride2
		);
	}
	vectorview<T> line3 (integer dim1, integer dim2) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		return vectorview<T> (
			our cells
			+ (dim1 - 1) * our stride1
			+ (dim2 - 1) * our stride2,
			our ndim3,
			our stride3
		);
	}
	matrixview<T> plane12 (integer dim3) const {
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return matrixview<T> (
			our cells
			+ (dim3 - 1) * our stride3,
			our ndim1, our ndim2,
			our stride1, our stride2
		);
	}
	matrixview<T> plane13 (integer dim2) const {
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		return matrixview<T> (
			our cells
			+ (dim2 - 1) * our stride2,
			our ndim1, our ndim3,
			our stride1, our stride3
		);
	}
	matrixview<T> plane23 (integer dim1) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		return matrixview<T> (
			our cells
			+ (dim1 - 1) * our stride1,
			our ndim2, our ndim3,
			our stride2, our stride3
		);
	}
	tensor3<T> part (
		integer firstDim1, integer lastDim1,
		integer firstDim2, integer lastDim2,
		integer firstDim3, integer lastDim3
	) const {
		Melder_assert (firstDim1 >= 1 && firstDim1 <= our ndim1);
		Melder_assert (lastDim1 >= 1 && lastDim1 <= our ndim1);
		Melder_assert (firstDim2 >= 1 && firstDim2 <= our ndim2);
		Melder_assert (lastDim2 >= 1 && lastDim2 <= our ndim2);
		Melder_assert (firstDim3 >= 1 && firstDim3 <= our ndim3);
		Melder_assert (lastDim3 >= 1 && lastDim3 <= our ndim3);
		const integer newNdim1 = lastDim1 - (firstDim1 - 1);
		const integer newNdim2 = lastDim2 - (firstDim2 - 1);
		const integer newNdim3 = lastDim3 - (firstDim3 - 1);
		if (newNdim1 <= 0 || newNdim2 <= 0 || newNdim3 <= 0)
			return tensor3<T> ();
		return tensor3<T> (
			our cells
			+ (firstDim1 - 1) * our stride1
			+ (firstDim2 - 1) * our stride2
			+ (firstDim3 - 1) * our stride3,
			newNdim1, newNdim2, newNdim3,
			our stride1, our stride2, our stride3
		);
	}
};

template <typename T>
class consttensor3 {
public:
	const T * cells = nullptr;
	integer ndim1 = 0, ndim2 = 0, ndim3 = 0;
	integer stride1 = 0, stride2 = 0, stride3 = 1;
	consttensor3 () = default;
	consttensor3 (const autotensor3<T>& other) = delete;
	explicit consttensor3 (const T * cells_,
		integer ndim1_, integer ndim2_, integer ndim3_,
		integer stride1_, integer stride2_, integer stride3_
	) :
		cells (cells_),
		ndim1 (ndim1_), ndim2 (ndim2_), ndim3 (ndim3_),
		stride1 (stride1_), stride2 (stride2_), stride3 (stride3_)
	{ }
	consttensor3 (tensor3<T> ten) :
		cells (ten.cells),
		ndim1 (ten.ndim1), ndim2 (ten.ndim2), ndim3 (ten.ndim3),
		stride1 (ten.stride1), stride2 (ten.stride2), stride3 (ten.stride3)
	{ }
	constmatrixview<T> operator[] (integer dim1) const {
		return constmatrixview<T> (our cells + (dim1 - 1) * our stride1, our ndim2, our ndim3, our stride2, our stride3);
	}
	constvectorview<T> line1 (integer dim2, integer dim3) const {
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return constvectorview<T> (
			our cells
			+ (dim2 - 1) * our stride2
			+ (dim3 - 1) * our stride3,
			our nidm1,
			our stride1
		);
	}
	constvectorview<T> line2 (integer dim1, integer dim3) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return constvectorview<T> (
			our cells
			+ (dim1 - 1) * our stride1
			+ (dim3 - 1) * our stride3,
			our ndim2,
			our stride2
		);
	}
	constvectorview<T> line3 (integer dim1, integer dim2) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		return constvectorview<T> (
			our cells
			+ (dim1 - 1) * our stride1
			+ (dim2 - 1) * our stride2,
			our ndim3,
			our stride3
		);
	}
	constmatrixview<T> plane12 (integer dim3) const {
		Melder_assert (dim3 >= 1 && dim3 <= our ndim3);
		return constmatrixview<T> (
			our cells
			+ (dim3 - 1) * our stride3,
			our ndim1, our ndim2,
			our stride1, our stride2
		);
	}
	constmatrixview<T> plane13 (integer dim2) const {
		Melder_assert (dim2 >= 1 && dim2 <= our ndim2);
		return constmatrixview<T> (
			our cells
			+ (dim2 - 1) * our stride2,
			our ndim1, our ndim3,
			our stride1, our stride3
		);
	}
	constmatrixview<T> plane23 (integer dim1) const {
		Melder_assert (dim1 >= 1 && dim1 <= our ndim1);
		return constmatrixview<T> (
			our cells
			+ (dim1 - 1) * our stride1,
			our ndim2, our ndim3,
			our stride2, our stride3
		);
	}
	consttensor3<T> part (
		integer firstDim1, integer lastDim1,
		integer firstDim2, integer lastDim2,
		integer firstDim3, integer lastDim3
	) const {
		Melder_assert (firstDim1 >= 1 && firstDim1 <= our ndim1);
		Melder_assert (lastDim1 >= 1 && lastDim1 <= our ndim1);
		Melder_assert (firstDim2 >= 1 && firstDim2 <= our ndim2);
		Melder_assert (lastDim2 >= 1 && lastDim2 <= our ndim2);
		Melder_assert (firstDim3 >= 1 && firstDim3 <= our ndim3);
		Melder_assert (lastDim3 >= 1 && lastDim3 <= our ndim3);
		const integer newNdim1 = lastDim1 - (firstDim1 - 1);
		const integer newNdim2 = lastDim2 - (firstDim2 - 1);
		const integer newNdim3 = lastDim3 - (firstDim3 - 1);
		if (newNdim1 <= 0 || newNdim2 <= 0 || newNdim3 <= 0)
			return consttensor3<T> ();
		return consttensor3<T> (
			our cells
			+ (firstDim1 - 1) * our stride1
			+ (firstDim2 - 1) * our stride2
			+ (firstDim3 - 1) * our stride3,
			newNdim1, newNdim2, newNdim3,
			our stride1, our stride2, our stride3
		);
	}
};

template <typename T>
class autotensor3 : public tensor3<T> {
public:
	autotensor3 () = default;   // come into existence without a payload
	explicit autotensor3 (integer givenNdim1, integer givenNdim2, integer givenNdim3, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		Melder_assert (givenNdim1 >= 0);
		Melder_assert (givenNdim2 >= 0);
		Melder_assert (givenNdim3 >= 0);
		our cells = ( givenNdim1 == 0 || givenNdim2 == 0 || givenNdim3 == 0 ? nullptr
				: NUMvector<T> (0, givenNdim3 * givenNdim2 * givenNdim1 - 1, initializationType == kTensorInitializationType::ZERO));
		our ndim1 = givenNdim1;
		our ndim2 = givenNdim2;
		our ndim3 = givenNdim3;
		our stride3 = 1;
		our stride2 = givenNdim3;
		our stride1 = givenNdim3 * givenNdim2;
	}
	~autotensor3 () {   // destroy the payload (if any)
		if (our cells) NUMvector_free (our cells, 0);
	}
	//tensor3<T> get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the structure)
	const tensor3<T>& get () { return *this; }   // let the public use the payload (they may change the values in the cells but not the structure)
	void adoptFromAmbiguousOwner (tensor3<T> const& given) {   // buy the payload from a non-automatrix
		our reset();
		our cells = given.cells;
		our ndim1 = given.ndim1;
		our ndim2 = given.ndim2;
		our ndim3 = given.ndim3;
		our stride1 = given.stride1;
		our stride2 = given.stride2;
		our stride3 = given.stride3;
	}
	tensor3<T> releaseToAmbiguousOwner () {   // sell the payload to a non-automatrix
		T * oldCells = our cells;
		our cells = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return tensor3<T> (oldCells, our ndim1, our ndim2, our ndim3, our stride1, our stride2, our stride3);
	}
	/*
		Disable copying via construction or assignment (which would violate unique ownership of the payload).
	*/
	autotensor3 (const autotensor3&) = delete;   // disable copy constructor
	autotensor3& operator= (const autotensor3&) = delete;   // disable copy assignment
	/*
		Enable moving of r-values (temporaries, implicitly) or l-values (for variables, via an explicit move()).
		This implements buying a payload from another automatrix (which involves destroying our current payload).
	*/
	autotensor3 (autotensor3&& other) noexcept : tensor3<T> (other.get()) {   // enable move constructor
		other.cells = nullptr;   // disown source
		other.ndim1 = 0;   // to keep the source in a valid state
		other.ndim2 = 0;   // to keep the source in a valid state
		other.ndim3 = 0;   // to keep the source in a valid state
	}
	autotensor3& operator= (autotensor3&& other) noexcept {   // enable move assignment
		if (other.cells != our cells) {
			if (our cells) NUMvector_free (our cells, 0);
			our cells = other.cells;
			our ndim1 = other.ndim1;
			our ndim2 = other.ndim2;
			our ndim3 = other.ndim3;
			other.cells = nullptr;   // disown source
			other.ndim1 = 0;   // to keep the source in a valid state
			other.ndim2 = 0;   // to keep the source in a valid state
			other.ndim3 = 0;   // to keep the source in a valid state
		}
		return *this;
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autoMAT)
		if (our cells) {
			NUMvector_free (our cells, 0);
			our cells = nullptr;
		}
		our ndim1 = 0;
		our ndim2 = 0;
		our ndim3 = 0;
	}
	autotensor3&& move () noexcept { return static_cast <autotensor3&&> (*this); }
};
template <typename T>
autotensor3<T> newtensor3raw (integer ndim1, integer ndim2, integer ndim3) {
	return autotensor3<T> (ndim1, ndim2, ndim3, kTensorInitializationType::RAW);
}
template <typename T>
autotensor3<T> newtensor3zero (integer ndim1, integer ndim2, integer ndim3) {
	return autotensor3<T> (ndim1, ndim2, ndim3, kTensorInitializationType::ZERO);
}
template <typename T>
void tensor3copy_preallocated (tensor3<T> const& target, consttensor3<T> const& source) {
	Melder_assert (source.ndim1 == target.ndim1 && source.ndim2 == target.ndim2 && source.ndim3 == target.ndim3);
	for (integer idim1 = 1; idim1 <= source.ndim1; idim1 ++)
		for (integer idim2 = 1; idim2 <= source.ndim2; idim2 ++)
			for (integer idim3 = 1; idim3 <= source.ndim3; idim3 ++)
				target [idim1] [idim2] [idim3] = source [idim1] [idim2] [idim3];
}
template <typename T>
void tensor3copy_preallocated (tensor3<T> const& target, tensor3<T> const& source) {
	tensor3copy_preallocated (target, consttensor3<T> (source));
}
template <typename T>
autotensor3<T> newtensor3copy (consttensor3<T> const& source) {
	autotensor3<T> result = newtensor3raw<T> (source.ndim1, source.ndim2, source.ndim3);
	tensor3copy_preallocated (result.get(), source);
	return result;
}
template <typename T>
autotensor3<T> newtensor3copy (tensor3<T> const& source) {
	return newtensor3copy<T> (consttensor3<T> (source));
}
template <typename T>
void assertDim1 (consttensor3<T> const& x, integer dim1) {
	Melder_assert (dim1 >= 1 && dim1 <= x.ndim1);
}
template <typename T>
void assertDim2 (consttensor3<T> const& x, integer dim2) {
	Melder_assert (dim2 >= 1 && dim2 <= x.ndim2);
}
template <typename T>
void assertDim3 (consttensor3<T> const& x, integer dim3) {
	Melder_assert (dim3 >= 1 && dim3 <= x.ndim3);
}
template <typename T>
void assertCell (consttensor3<T> const& x, integer dim1, integer dim2, integer dim3) {
	assertDim1 (x, dim1);
	assertDim2 (x, dim2);
	assertDim3 (x, dim3);
}
template <typename T>
autotensor3<T> newtensor3part (consttensor3<T> const& x,
	integer firstDim1, integer lastDim1,
	integer firstDim2, integer lastDim2,
	integer firstDim3, integer lastDim3
) {
	assertCell (x, firstDim1, firstDim2, firstDim3);
	assertCell (x, lastDim1, lastDim2, lastDim3);
	integer ndim1 = lastDim1 - (firstDim1 - 1);
	Melder_assert (ndim1 >= 0);
	integer ndim2 = lastDim2 - (firstDim2 - 1);
	Melder_assert (ndim2 >= 0);
	integer ndim3 = lastDim3 - (firstDim3 - 1);
	Melder_assert (ndim3 >= 0);
	autotensor3<T> result = newtensor3raw<T> (ndim1, ndim2, ndim3);
	for (integer idim1 = 1; idim1 <= ndim1; idim1 ++)
		for (integer idim2 = 1; idim2 <= ndim2; idim2 ++)
			for (integer idim3 = 1; idim3 <= ndim3; idim3 ++)
				result [idim1] [idim2] [idim3] = x [firstDim1 - 1 + idim1] [firstDim2 - 1 + idim2] [firstDim3 - 1 + idim3];
	return result;
}
template <typename T>
autotensor3<T> newtensor3part (tensor3<T> const& x,
	integer firstDim1, integer lastDim1,
	integer firstDim2, integer lastDim2,
	integer firstDim3, integer lastDim3
) {
	return newtensor3part (consttensor3<T> (x), firstDim1, lastDim1, firstDim2, lastDim2, firstDim3, lastDim3);
}

/*
	instead of vector<double> we say VEC, because we want to have a one-to-one
	relation between VEC functions and the scripting language.
	For instance, we have newVECraw and newVECzero because Praat scripting has raw# and zero#.
*/
using VEC = vector <double>;
using VECVU = vectorview <double>;
using constVEC = constvector <double>;
using constVECVU = constvectorview <double>;
using autoVEC = autovector <double>;
inline autoVEC newVECraw (integer size) {
	return newvectorraw <double> (size);
}
inline autoVEC newVECzero (integer size) {
	return newvectorzero <double> (size);
}
inline autoVEC newVECcopy (constVEC source) {
	return newvectorcopy (source);
}

/*
	And simply because we use vector<integer> so much as well,
	we have an abbreviation for that as well, namely INTVEC.
	But the scripting language has nothing that corresponds to INTVEC,
	so any numeric vector to be used by the scripting language
	should be a VEC, even if it contains integers.
	This is fine, as a double can contain an integer up to 54 bits.
*/
using INTVEC = vector <integer>;
using INTVECVU = vectorview <integer>;
using constINTVEC = constvector <integer>;
using constINTVECVU = constvectorview <integer>;
using autoINTVEC = autovector <integer>;
inline autoINTVEC newINTVECraw (integer size) {
	return newvectorraw <integer> (size);
}
inline autoINTVEC newINTVECzero (integer size) {
	return newvectorzero <integer> (size);
}
inline autoINTVEC newINTVECcopy (constINTVEC source) {
	return newvectorcopy (source);
}

using BOOLVEC = vector <bool>;
using constBOOLVEC = constvector <bool>;
using autoBOOLVEC = autovector <bool>;
inline autoBOOLVEC newBOOLVECraw (integer size) {
	return newvectorraw <bool> (size);
}
inline autoBOOLVEC newBOOLVECzero (integer size) {
	return newvectorzero <bool> (size);
}
inline autoBOOLVEC newBOOLVECcopy (constBOOLVEC source) {
	return newvectorcopy (source);
}

using MAT = matrix <double>;
using MATVU = matrixview <double>;
using constMAT = constmatrix <double>;
using constMATVU = constmatrixview <double>;
using autoMAT = automatrix <double>;
inline autoMAT newMATraw (integer nrow, integer ncol) {
	return newmatrixraw <double> (nrow, ncol);
}
inline autoMAT newMATzero (integer nrow, integer ncol) {
	return newmatrixzero <double> (nrow, ncol);
}
inline autoMAT newMATcopy (constMATVU source) {
	return newmatrixcopy (source);
}
inline autoMAT newMATpart (const constMAT& source,
	integer firstRow, integer lastRow,
	integer firstColumn, integer lastColumn
) {
	return newmatrixpart (source, firstRow, lastRow, firstColumn, lastColumn);
}

using TEN3 = tensor3 <double>;
using constTEN3 = consttensor3 <double>;
using autoTEN3 = autotensor3 <double>;
inline autoTEN3 newTEN3raw (integer ndim1, integer ndim2, integer ndim3) {
	return newtensor3raw <double> (ndim1, ndim2, ndim3);
}
inline autoTEN3 newTEN3zero (integer ndim1, integer ndim2, integer ndim3) {
	return newtensor3zero <double> (ndim1, ndim2, ndim3);
}
inline autoTEN3 newTEN3copy (constTEN3 source) {
	return newtensor3copy (source);
}
inline autoTEN3 newTEN3part (const constTEN3& source,
	integer firstDim1, integer lastDim1,
	integer firstDim2, integer lastDim2,
	integer firstDim3, integer lastDim3
) {
	return newtensor3part (source, firstDim1, lastDim1, firstDim2, lastDim2, firstDim3, lastDim3);
}

using INTMAT = matrix <integer>;
using INTMATVU = matrixview <integer>;
using constINTMAT = constmatrix <integer>;
using constINTMATVU = constmatrixview <integer>;
using autoINTMAT = automatrix <integer>;
inline autoINTMAT newINTMATraw (integer nrow, integer ncol) {
	return newmatrixraw <integer> (nrow, ncol);
}
inline autoINTMAT newINTMATzero (integer nrow, integer ncol) {
	return newmatrixzero <integer> (nrow, ncol);
}
inline autoINTMAT newINTMATcopy (constINTMATVU source) {
	return newmatrixcopy (source);
}

using BOOLMAT = matrix <bool>;
using BOOLMATVU = matrixview <bool>;
using constBOOLMAT = constmatrix <bool>;
using constBOOLMATVU = constmatrixview <bool>;
using autoBOOLMAT = automatrix <bool>;
inline autoBOOLMAT newBOOLMATraw (integer nrow, integer ncol) {
	return newmatrixraw <bool> (nrow, ncol);
}
inline autoBOOLMAT newBOOLMATzero (integer nrow, integer ncol) {
	return newmatrixzero <bool> (nrow, ncol);
}
inline autoBOOLMAT newBOOLMATcopy (constBOOLMATVU source) {
	return newmatrixcopy (source);
}

using BYTEMAT = matrix <byte>;
using BYTEMATVU = matrixview <byte>;
using constBYTEMAT = constmatrix <byte>;
using constBYTEMATVU = constmatrixview <byte>;
using autoBYTEMAT = automatrix <byte>;
inline autoBYTEMAT newBYTEMATraw (integer nrow, integer ncol) {
	return newmatrixraw <byte> (nrow, ncol);
}
inline autoBYTEMAT newBYTEMATzero (integer nrow, integer ncol) {
	return newmatrixzero <byte> (nrow, ncol);
}
inline autoBYTEMAT newBYTEMATcopy (constBYTEMATVU source) {
	return newmatrixcopy (source);
}

conststring32 Melder_VEC (constVEC value);
conststring32 Melder_MAT (constMAT value);

inline void operator<<= (INTVECVU const& target, constINTVECVU const& source) {
	Melder_assert (target.size == source.size);
	for (integer i = 1; i <= target.size; i ++)
		target [i] = source [i];
}
inline void operator<<= (TEN3 const& target, constTEN3 const& source) {
	Melder_assert (target.ndim1 == source.ndim1);
	Melder_assert (target.ndim2 == source.ndim2);
	Melder_assert (target.ndim3 == source.ndim3);
	for (integer idim1 = 1; idim1 <= target.ndim1; idim1 ++)
		for (integer idim2 = 1; idim2 <= target.ndim2; idim2 ++)
			for (integer idim3 = 1; idim3 <= target.ndim3; idim3 ++)
				target [idim1] [idim2] [idim3] = source [idim1] [idim2] [idim3];
}

/* End of file melder_tensor.h */
#endif
