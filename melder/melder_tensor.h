#ifndef _melder_tensor_h_
#define _melder_tensor_h_
/* melder_tensor.h
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

enum class kTensorInitializationType { RAW = 0, ZERO = 1 };

byte * MelderTensor_generic (integer cellSize, integer numberOfCells, kTensorInitializationType initializationType);

template <class T>
T* MelderTensor (integer numberOfCells, kTensorInitializationType initializationType) {
	T* result = reinterpret_cast <T*> (MelderTensor_generic (sizeof (T), numberOfCells, initializationType));
	return result;
}

void MelderTensor_free_generic (byte *cells) noexcept;

template <class T>
void MelderTensor_free (T* cells) noexcept {
	MelderTensor_free_generic (reinterpret_cast <byte *> (cells));
}

integer NUM_getTotalNumberOfArrays ();   // for debugging

#pragma mark - TENSOR
/*
	Base-1 tensors, for parallellism with the scripting language.

	Initialization (tested in praat.cpp):
		VEC x;               // initializes x.at to nullptr and x.size to 0
		double a [] = { undefined, 3.14, 2.718, ... };
		VEC x3 { a, 100 };   // initializes x to 100 values from a base-1 array

		autoVEC y;                     // initializes y.at to nullptr and y.size to 0
		autoVEC y2 = newVECzero (100); // initializes y to 100 zeroes, having ownership
		autoVEC y1 = newVECraw (100);  // initializes y to 100 uninitialized values (caution!), having ownership
		y.adoptFromAmbiguousOwner (x); // initializes y to the content of x, taking ownership (explicit, so not "y = x")
		VEC z = y.releaseToAmbiguousOwner();   // releases ownership, y.at becoming nullptr
		"}"                            // end of scope destroys y.at if not nullptr
		autoVEC z1 = y2.move()         // moves the content of y2 to z1, emptying y2

	To return an autoVEC from a function, transfer ownership like this:
		autoVEC foo () {
			autoVEC x = newVECero (100);
			... // fill in the 100 values
			return x;
		}
*/

template <typename T>
class autovector;   // forward declaration, needed in the declaration of vector<>

template <typename T>
class vector {
public:
	T *cells = nullptr;
	integer size = 0;
public:
	vector () = default;
	explicit vector (T *givenCells, integer givenSize, bool dummy): cells (givenCells), size (givenSize) { }
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
		return our cells [i - 1];
	}
	/*
		part (first, last) should crash under the exact
		same conditions as a rising for-loop over the elements
		from first to last (or a falling for-loop over
		the elements from last to first) should crash.
	*/
	vector<T> part (integer first, integer last) const {
		const integer newSize = last - (first - 1);
		/*
			for-loops don't crash if the number of elements is zero.
		*/
		if (newSize <= 0) return vector<T> ();
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 1 && last <= our size);
		return vector<T> (& our cells [first - 1], newSize, false);
	}
	T *begin () const { return our cells; }
	T *end () const { return our cells + our size; }
	T *asArgumentToFunctionThatExpectsZeroBasedArray () const { return our cells; }
	T *asArgumentToFunctionThatExpectsOneBasedArray () const { return our cells - 1; }
};

template <typename T>
class vectorview {
public:
	T * firstCell = nullptr;
	integer size = 0;
	integer stride = 1;
	vectorview () = default;
	vectorview (const vector<T>& other) :
			firstCell (other.cells), size (other.size), stride (1) { }
	vectorview (const autovector<T>& other) = delete;
	explicit vectorview (T * const firstCell_, integer const size_, integer const stride_) :
			firstCell (firstCell_), size (size_), stride (stride_) { }
	T& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	vectorview<T> part (integer first, integer last) const {
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return vectorview<T> ();
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 1 && last <= our size);
		return vectorview<T> (& our operator[] (first), newSize, our stride);
	}
	T *begin () const { return & our operator[] (1); }
	T *end () const { return & our operator[] (our size + 1); }
	T *asArgumentToFunctionThatExpectsZeroBasedArray () const { return & our operator[] [1]; }
	T *asArgumentToFunctionThatExpectsOneBasedArray () const { return & our operator[] [0]; }
};

template <typename T>
class constvector {
public:
	const T *cells = nullptr;
	integer size = 0;
	constvector () = default;
	explicit constvector (const T *givenCells, integer givenSize, bool dummy): cells (givenCells), size (givenSize) { }
	constvector (vector<T> vec): cells (vec.cells), size (vec.size) { }
	//constvector (const constvector& other): cells (other.cells), size (other.size) { }
	//constvector& operator= (const constvector& other) {
	//	our cells = other.cells;
	//	our size = other.size;
	//}
	const T& operator[] (integer i) const {   // it's still a reference, because we need to be able to take its address
		return our cells [i - 1];
	}
	constvector<T> part (integer first, integer last) const {
		const integer newSize = last - (first - 1);
		if (newSize <= 0)
			return constvector<T> (nullptr, 0, false);
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 1 && last <= our size);
		return constvector<T> (& our cells [first - 1], newSize, false);
	}
	const T *begin () const { return our cells; }
	const T *end () const { return our cells + our size; }
	const T *asArgumentToFunctionThatExpectsZeroBasedArray () const { return our cells; }
	const T *asArgumentToFunctionThatExpectsOneBasedArray () const { return our cells - 1; }
};

template <typename T>
class constvectorview {
public:
	const T * firstCell = nullptr;
	integer size = 0;
	integer stride = 1;
	constvectorview () = default;
	constvectorview (const constvector<T>& other) :
			firstCell (other.cells), size (other.size), stride (1) { }
	constvectorview (const vector<T>& other) :
			firstCell (other.cells), size (other.size), stride (1) { }
	constvectorview (const autovector<T>& other) = delete;
	explicit constvectorview (const T * const firstCell_, integer const size_, integer const stride_) :
			firstCell (firstCell_), size (size_), stride (stride_) { }
	constvectorview (vectorview<T> vec): firstCell (vec.firstCell), size (vec.size), stride (vec.stride) { }
	T const& operator[] (integer i) const {
		return our firstCell [(i - 1) * our stride];
	}
	constvectorview<T> part (integer first, integer last) const {
		const integer newSize = last - (first - 1);
		if (newSize <= 0) return constvectorview<T> ();
		Melder_assert (first >= 1 && first <= our size);
		Melder_assert (last >= 1 && last <= our size);
		return constvectorview<T> (& our operator[] (first), newSize, our stride);
	}
	const T *begin () const { return & our operator[] (1); }
	const T *end () const { return & our operator[] (our size + 1); }
	const T *asArgumentToFunctionThatExpectsZeroBasedArray () const { return & our operator[] [1]; }
	const T *asArgumentToFunctionThatExpectsOneBasedArray () const { return & our operator[] [0]; }
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
	integer _capacity = 0;
public:
	autovector (): vector<T> (nullptr, 0, false) { }   // come into existence without a payload
	explicit autovector (integer givenSize, kTensorInitializationType initializationType) {   // come into existence and manufacture a payload
		Melder_assert (givenSize >= 0);
		our cells = ( givenSize == 0 ? nullptr : MelderTensor <T> (givenSize, initializationType) );
		our size = givenSize;
		our _capacity = givenSize;
	}
	~autovector () {   // destroy the payload (if any)
		our reset ();
		our _capacity = 0;
	}
	vector<T> get () const { return vector<T> (our cells, our size, false); }   // let the public use the payload (they may change the values of the elements but not the at-pointer or the size)
	vectorview<T> all () const { return vectorview<T> (our cells, our size, 1); }
	void adoptFromAmbiguousOwner (vector<T> given) {   // buy the payload from a non-autovector
		our reset();
		our cells = given.cells;
		our size = given.size;
		our _capacity = given.size;
	}
	vector<T> releaseToAmbiguousOwner () {   // sell the payload to a non-autovector
		T *oldCells = our cells;
		our cells = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		integer oldSize = our size;
		our _capacity = 0;
		return vector<T> (oldCells, oldSize, false);
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
		other.cells = nullptr;   // disown source
		other.size = 0;   // to keep the source in a valid state
		other._capacity = 0;
	}
	autovector& operator= (autovector&& other) noexcept {   // enable move assignment
		if (other.cells != our cells) {
			our reset ();
			our cells = other.cells;
			our size = other.size;
			our _capacity = other._capacity;
			other.cells = nullptr;   // disown source
			other.size = 0;   // to keep the source in a valid state
			other._capacity = 0;
		}
		return *this;
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autovector<>)
		if (our cells) {
			MelderTensor_free (our cells);
			our cells = nullptr;
		}
		our size = 0;
	}
	autovector&& move () noexcept { return static_cast <autovector&&> (*this); }   // enable constriction and assignment for l-values (variables) via explicit move()
	/*
		Some of the following functions are capable of keeping a valid `cells` pointer
		while `size` can at the same time be zero.
	*/
	void initWithCapacity (integer capacity, kTensorInitializationType initializationType = kTensorInitializationType::ZERO) {
		if (capacity > 0)
			our cells = MelderTensor <T> (capacity, initializationType);
		our size = 0;
		our _capacity = capacity;
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
	void resize (integer newSize, kTensorInitializationType initializationType = kTensorInitializationType::ZERO) {
		if (newSize > our _capacity) {
			/*
				The new capacity is at least twice the old capacity.
				When starting at a capacity of 0, and continually upsizing by one,
				the capacity sequence will be: 0, 11, 33, 77, 165, 341, 693, 1397,
				2805, 5621, 11253, 22517, 45045, 90101, 180213, 360437, 720885...
			*/
			integer newCapacity = newSize + our size + 10;
			/*
				Create without change.
			*/
			T *newCells = MelderTensor<T> (newCapacity, initializationType);
			/*
				Change without error.
			*/
			for (integer i = 1; i <= our size; i ++)
				newCells [i - 1] = std::move (our cells [i - 1]);
			if (our cells)
				MelderTensor_free (our cells);
			our cells = newCells;
			our _capacity = newCapacity;
		}
		our size = newSize;
	}
	void insert (integer position, const T& value) {
		resize (our size + 1, kTensorInitializationType::RAW);
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = our size; i > position; i --)
			our cells [i - 1] = std::move (our cells [i - 2]);
		our cells [position - 1] = value;
	}
	T* append () {
		resize (our size + 1, kTensorInitializationType::ZERO);
		return & our cells [our size - 1];
	}
	void remove (integer position) {
		Melder_assert (position >= 1 && position <= our size);
		for (integer i = position; i < our size; i ++)
			our cells [i - 1] = std::move (our cells [i]);
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
autovector<T> newvectorcopy (constvectorview<T> source) {
	autovector<T> result = newvectorraw<T> (source.size);
	for (integer i = 1; i <= source.size; i ++)
		result [i] = source [i];
	return result;
}
template <typename T>
autovector<T> newvectorcopy (vectorview<T> source) {
	return newvectorcopy (constvectorview<T> (source));
}

template <typename T>
class automatrix;   // forward declaration, needed in the declaration of matrix
template <typename T>
class matrixview;
template <typename T>
class constmatrixview;

template <typename T>
class matrix {
public:
	T *cells = nullptr;
	integer nrow = 0, ncol = 0;
public:
	matrix () = default;
	explicit matrix (T *givenCells, integer givenNrow, integer givenNcol) :
			cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	matrix (const matrix& other) = default;
	matrix (const automatrix<T>& other) = delete;
	matrix& operator= (const matrix&) = default;
	matrix& operator= (const automatrix<T>&) = delete;
	vector<T> operator[] (integer rowNumber) const {
		return vector<T> (our cells + (rowNumber - 1) * our ncol, our ncol, false);
	}
	vector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		Melder_assert (our cells);
		return vector<T> (our cells + (rowNumber - 1) * our ncol, our ncol, false);
	}
	vectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return vectorview<T> (our cells + (columnNumber - 1), our nrow, our ncol);
	}
	vectorview<T> diagonal () const {
		return vectorview<T> (our cells, std::min (our nrow, our ncol), our ncol + 1);
	}
	matrixview<T> horizontalBand (integer firstRow, integer lastRow) const {
		const integer newNrow = lastRow - (firstRow - 1);
		if (newNrow <= 0) return matrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		return matrixview<T> (our cells + (firstRow - 1) * our ncol, newNrow, our ncol, our ncol, 1);
	}
	matrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return matrixview<T> ();
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return matrixview<T> (our cells + (firstColumn - 1), our nrow, newNcol, our ncol, 1);
	}
	matrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return matrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return matrixview<T> (
			our cells + (firstRow - 1) * our ncol + (firstColumn - 1),
			newNrow, newNcol, our ncol, 1
		);
	}
	matrixview<T> transpose () const {
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
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return matrixview<T> ();
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return matrixview<T> (our firstCell + (firstColumn - 1) * our colStride,
				our nrow, newNcol, our rowStride, our colStride);
	}
	matrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return matrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return matrixview<T> (
			our firstCell + (firstRow - 1) * our rowStride + (firstColumn - 1) * our colStride,
			newNrow, newNcol, our rowStride, our colStride
		);
	}
	matrixview<T> transpose () const {
		return matrixview<T> (our firstCell, our ncol, our nrow, our colStride, our rowStride);
	}
};

template <typename T>
class constmatrix {
public:
	const T *cells = nullptr;
	integer nrow = 0, ncol = 0;
	constmatrix () = default;
	//explicit constmatrix (const T *givenCells, integer givenNrow, integer givenNcol): cells (givenCells), nrow (givenNrow), ncol (givenNcol) { }
	constmatrix (matrix<T> mat) :
			cells (mat.cells), nrow (mat.nrow), ncol (mat.ncol) { }

	constvector<T> operator[] (integer rowNumber) const {
		return constvector<T> (our cells + (rowNumber - 1) * our ncol, our ncol, false);
	}
	constvector<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		Melder_assert (our cells);
		return constvector<T> (our cells + (rowNumber - 1) * our ncol, our ncol, false);
	}
	constvectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return constvectorview<T> (our cells + (columnNumber - 1), our nrow, our ncol);
	}
	constvectorview<T> diagonal () const {
		return constvectorview<T> (our cells, std::min (our nrow, our ncol), our ncol + 1);
	}
	constmatrixview<T> horizontalBand (integer firstRow, integer lastRow) const {
		const integer newNrow = lastRow - (firstRow - 1);
		if (newNrow <= 0) return constmatrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		return constmatrixview<T> (our cells + (firstRow - 1) * our ncol, newNrow, our ncol, our ncol, 1);
	}
	constmatrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return constmatrixview<T> ();
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return constmatrixview<T> (our cells + (firstColumn - 1), our nrow, newNcol, our ncol, 1);
	}
	constmatrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return constmatrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return constmatrixview<T> (
			our cells + (firstRow - 1) * our ncol + (firstColumn - 1),
			newNrow, newNcol, our ncol, 1
		);
	}
	constmatrixview<T> transpose () const {
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
	constvectorview<T> row (integer rowNumber) const {
		Melder_assert (rowNumber >= 1 && rowNumber <= our nrow);
		return constvectorview<T> (our firstCell + (rowNumber - 1) * our rowStride, our ncol, our colStride);
	}
	constvectorview<T> column (integer columnNumber) const {
		Melder_assert (columnNumber >= 1 && columnNumber <= our ncol);
		return constvectorview<T> (our firstCell + (columnNumber - 1) * our colStride, our nrow, our rowStride);
	}
	constvectorview<T> diagonal () const {
		return constvectorview<T> (our firstCell, std::min (our nrow, our ncol), our rowStride + our colStride);
	}
	constmatrixview<T> verticalBand (integer firstColumn, integer lastColumn) const {
		const integer newNcol = lastColumn - (firstColumn - 1);
		if (newNcol <= 0) return constmatrixview<T> ();
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return constmatrixview<T> (our firstCell + (firstColumn - 1) * our colStride,
				our nrow, newNcol, our rowStride, our colStride);
	}
	constmatrixview<T> part (integer firstRow, integer lastRow, integer firstColumn, integer lastColumn) const {
		const integer newNrow = lastRow - (firstRow - 1), newNcol = lastColumn - (firstColumn - 1);
		if (newNrow <= 0 || newNcol <= 0) return constmatrixview<T> ();
		Melder_assert (firstRow >= 1 && firstRow <= our nrow);
		Melder_assert (lastRow >= 1 && lastRow <= our nrow);
		Melder_assert (firstColumn >= 1 && firstColumn <= our ncol);
		Melder_assert (lastColumn >= 1 && lastColumn <= our ncol);
		return constmatrixview<T> (
			our firstCell
			+ (firstRow - 1) * our rowStride
			+ (firstColumn - 1) * our colStride,
			newNrow, newNcol,
			our rowStride, our colStride
		);
	}
	constmatrixview<T> transpose () const {
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
		our cells = ( givenNrow == 0 || givenNcol == 0 ? nullptr
				: MelderTensor <T> (givenNrow * givenNcol, initializationType) );
		our nrow = givenNrow;
		our ncol = givenNcol;
	}
	~automatrix () {   // destroy the payload (if any)
		if (our cells)
			MelderTensor_free (our cells);
	}
	//matrix<T> get () { return { our cells, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	const matrix<T>& get () const { return *this; }   // let the public use the payload (they may change the values in the cells but not the at-pointer, nrow or ncol)
	matrixview<T> all () const {
		return matrixview<T> (our cells, our nrow, our ncol, our ncol, 1);
	}
	void adoptFromAmbiguousOwner (matrix<T> given) {   // buy the payload from a non-automatrix
		our reset();
		our cells = given.cells;
		our nrow = given.nrow;
		our ncol = given.ncol;
	}
	matrix<T> releaseToAmbiguousOwner () {   // sell the payload to a non-automatrix
		T *oldCells = our cells;
		our cells = nullptr;   // disown ourselves, preventing automatic destruction of the payload
		return matrix<T> (oldCells, our nrow, our ncol);
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
		other.nrow = 0;   // to keep the source in a valid state
		other.ncol = 0;   // to keep the source in a valid state
	}
	automatrix& operator= (automatrix&& other) noexcept {   // enable move assignment
		if (other.cells != our cells) {
			if (our cells)
				MelderTensor_free (our cells);
			our cells = other.cells;
			our nrow = other.nrow;
			our ncol = other.ncol;
			other.cells = nullptr;   // disown source
			other.nrow = 0;   // to keep the source in a valid state
			other.ncol = 0;   // to keep the source in a valid state
		}
		return *this;
	}
	void reset () noexcept {   // on behalf of ambiguous owners (otherwise this could be in autoMAT)
		if (our cells) {
			MelderTensor_free (our cells);
			our cells = nullptr;
		}
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
void matrixcopy (matrixview<T> const& target, constmatrixview<T> const& source) {
	Melder_assert (source.nrow == target.nrow && source.ncol == target.ncol);
	for (integer irow = 1; irow <= source.nrow; irow ++)
		for (integer icol = 1; icol <= source.ncol; icol ++)
			target [irow] [icol] = source [irow] [icol];
}
template <typename T>
void matrixcopy (matrixview<T> const& target, matrixview<T> const& source) {
	matrixcopy (target, constmatrixview<T> (source));
}
template <typename T>
automatrix<T> newmatrixcopy (constmatrixview<T> const& source) {
	automatrix<T> result = newmatrixraw<T> (source.nrow, source.ncol);
	matrixcopy (result.all(), source);
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
		const integer newNdim1 = lastDim1 - (firstDim1 - 1);
		const integer newNdim2 = lastDim2 - (firstDim2 - 1);
		const integer newNdim3 = lastDim3 - (firstDim3 - 1);
		if (newNdim1 <= 0 || newNdim2 <= 0 || newNdim3 <= 0)
			return tensor3<T> ();
		Melder_assert (firstDim1 >= 1 && firstDim1 <= our ndim1);
		Melder_assert (lastDim1 >= 1 && lastDim1 <= our ndim1);
		Melder_assert (firstDim2 >= 1 && firstDim2 <= our ndim2);
		Melder_assert (lastDim2 >= 1 && lastDim2 <= our ndim2);
		Melder_assert (firstDim3 >= 1 && firstDim3 <= our ndim3);
		Melder_assert (lastDim3 >= 1 && lastDim3 <= our ndim3);
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
		const integer newNdim1 = lastDim1 - (firstDim1 - 1);
		const integer newNdim2 = lastDim2 - (firstDim2 - 1);
		const integer newNdim3 = lastDim3 - (firstDim3 - 1);
		if (newNdim1 <= 0 || newNdim2 <= 0 || newNdim3 <= 0)
			return consttensor3<T> ();
		Melder_assert (firstDim1 >= 1 && firstDim1 <= our ndim1);
		Melder_assert (lastDim1 >= 1 && lastDim1 <= our ndim1);
		Melder_assert (firstDim2 >= 1 && firstDim2 <= our ndim2);
		Melder_assert (lastDim2 >= 1 && lastDim2 <= our ndim2);
		Melder_assert (firstDim3 >= 1 && firstDim3 <= our ndim3);
		Melder_assert (lastDim3 >= 1 && lastDim3 <= our ndim3);
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
				: MelderTensor <T> (givenNdim3 * givenNdim2 * givenNdim1, initializationType) );
		our ndim1 = givenNdim1;
		our ndim2 = givenNdim2;
		our ndim3 = givenNdim3;
		our stride3 = 1;
		our stride2 = givenNdim3;
		our stride1 = givenNdim3 * givenNdim2;
	}
	~autotensor3 () {   // destroy the payload (if any)
		if (our cells)
			MelderTensor_free (our cells);
	}
	//tensor3<T> get () { return { our at, our nrow, our ncol }; }   // let the public use the payload (they may change the values in the cells but not the structure)
	const tensor3<T>& get () const { return *this; }   // let the public use the payload (they may change the values in the cells but not the structure)
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
			if (our cells)
				MelderTensor_free (our cells);
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
			MelderTensor_free (our cells);
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
void tensor3copy (tensor3<T> const& target, consttensor3<T> const& source) {
	Melder_assert (source.ndim1 == target.ndim1 && source.ndim2 == target.ndim2 && source.ndim3 == target.ndim3);
	for (integer idim1 = 1; idim1 <= source.ndim1; idim1 ++)
		for (integer idim2 = 1; idim2 <= source.ndim2; idim2 ++)
			for (integer idim3 = 1; idim3 <= source.ndim3; idim3 ++)
				target [idim1] [idim2] [idim3] = source [idim1] [idim2] [idim3];
}
template <typename T>
void tensor3copy (tensor3<T> const& target, tensor3<T> const& source) {
	tensor3copy (target, consttensor3<T> (source));
}
template <typename T>
autotensor3<T> newtensor3copy (consttensor3<T> const& source) {
	autotensor3<T> result = newtensor3raw<T> (source.ndim1, source.ndim2, source.ndim3);
	tensor3copy (result.get(), source);
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
inline autoVEC newVECcopy (constVECVU const& source) {
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
inline autoINTVEC newINTVECcopy (constINTVECVU const& source) {
	return newvectorcopy (source);
}

using BOOLVEC = vector <bool>;
using BOOLVECVU = vectorview <bool>;
using constBOOLVEC = constvector <bool>;
using constBOOLVECVU = constvectorview <bool>;
using autoBOOLVEC = autovector <bool>;
inline autoBOOLVEC newBOOLVECraw (integer size) {
	return newvectorraw <bool> (size);
}
inline autoBOOLVEC newBOOLVECzero (integer size) {
	return newvectorzero <bool> (size);
}
inline autoBOOLVEC newBOOLVECcopy (constBOOLVECVU const& source) {
	return newvectorcopy (source);
}

using BYTEVEC = vector <byte>;
using BYTEVECVU = vectorview <byte>;
using constBYTEVEC = constvector <byte>;
using constBYTEVECVU = constvectorview <byte>;
using autoBYTEVEC = autovector <byte>;
inline autoBYTEVEC newBYTEVECraw (integer size) {
	return newvectorraw <byte> (size);
}
inline autoBYTEVEC newBYTEVECzero (integer size) {
	return newvectorzero <byte> (size);
}
inline autoBYTEVEC newBYTEVECcopy (constBYTEVECVU const& source) {
	return newvectorcopy (source);
}

using COMPVEC = vector <dcomplex>;
using COMPVECVU = vectorview <dcomplex>;
using constCOMPVEC = constvector <dcomplex>;
using constCOMPVECVU = constvectorview <dcomplex>;
using autoCOMPVEC = autovector <dcomplex>;
inline autoCOMPVEC newCOMPVECraw (integer size) {
	return newvectorraw <dcomplex> (size);
}
inline autoCOMPVEC newCOMPVECzero (integer size) {
	return newvectorzero <dcomplex> (size);
}
inline autoCOMPVEC newCOMPVECcopy (constCOMPVECVU const& source) {
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

conststring32 Melder_VEC (constVECVU const& value);
conststring32 Melder_MAT (constMATVU const& value);

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
