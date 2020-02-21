#ifndef _melder_require_h_
#define _melder_require_h_
/* melder_require.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

#define Melder_require(condition, ...)  do { if (! (condition)) Melder_throw (__VA_ARGS__); } while (false)

inline void checkElementNumber (integer elementNumber, integer maximumElementNumber) {
	Melder_require (elementNumber >= 1,
		U"The element number should be at least 1, not ", elementNumber, U".");
	Melder_require (elementNumber <= maximumElementNumber,
		U"The element number should be at most ", maximumElementNumber, U", not", elementNumber, U".");
}
template <typename T>
void checkElementNumber (integer elementNumber, const constvector<T>& vec) {
	Melder_require (elementNumber >= 1,
		U"The element number should be at least 1, not ", elementNumber, U".");
	Melder_require (elementNumber <= vec.size,
		U"The element number should be at most the number of elements (", vec.size, U"), not", elementNumber, U".");
}
template <typename T>
void checkElementNumber (integer elementNumber, const vector<T>& x) {
	checkElementNumber (elementNumber, constvector<T> (x));
}

template <typename T>
void checkElementRange (integer firstElement, integer lastElement, const constvector<T>& x, integer minimumNewNumberOfElements) {
	Melder_require (firstElement >= 1,
		U"The first element should be at least 1, not ", firstElement, U".");
	integer minimumLastRow = firstElement + (minimumNewNumberOfElements - 1);
	Melder_require (lastElement >= minimumLastRow,
		U"The last element should be at least ", minimumLastRow, U", not ", lastElement,
		U", because the new vector should contain at least ", minimumNewNumberOfElements,
		U" elements (the first element is ", firstElement, U")."
	);
	Melder_require (lastElement <= x.size,
		U"The last element should be at most the number of elements (", x.size, U"), not", lastElement, U".");
}
template <typename T>
void checkElementRange (integer firstElement, integer lastElement, const vector<T>& x, integer minimumNewNumberOfElements) {
	checkElementRange (firstElement, lastElement, constvector<T> (x), minimumNewNumberOfElements);
}

inline void checkRowNumber (integer rowNumber, integer maximumRowNumber) {
	Melder_require (rowNumber >= 1,
		U"The row number should be at least 1, not ", rowNumber, U".");
	Melder_require (rowNumber <= maximumRowNumber,
		U"The row number should be at most ", maximumRowNumber, U", not", rowNumber, U".");
}
template <typename T>
void checkRowNumber (integer rowNumber, const constmatrix<T>& mat) {
	Melder_require (rowNumber >= 1,
		U"The row number should be at least 1, not ", rowNumber, U".");
	Melder_require (rowNumber <= mat.nrow,
		U"The row number should be at most the number of rows (", mat.nrow, U"), not", rowNumber, U".");
}
template <typename T>
void checkRowNumber (integer rowNumber, const matrix<T>& x) {
	checkRowNumber (rowNumber, constmatrix<T> (x));
}

template <typename T>
void checkRowRange (integer firstRow, integer lastRow, const constmatrix<T>& x, integer minimumNewNumberOfRows) {
	Melder_require (firstRow >= 1,
		U"The first row should be at least 1, not ", firstRow, U".");
	integer minimumLastRow = firstRow + (minimumNewNumberOfRows - 1);
	Melder_require (lastRow >= minimumLastRow,
		U"The last row should be at least ", minimumLastRow, U", not ", lastRow,
		U", because the new matrix should contain at least ", minimumNewNumberOfRows,
		U" rows (the first row is ", firstRow, U")."
	);
	Melder_require (lastRow <= x.nrow,
		U"The last row should be at most the number of rows (", x.nrow, U"), not", lastRow, U".");
}
template <typename T>
void checkRowRange (integer firstRow, integer lastRow, const matrix<T>& x, integer minimumNewNumberOfRows) {
	checkRowRange (firstRow, lastRow, constmatrix<T> (x), minimumNewNumberOfRows);
}

inline void checkColumnNumber (integer columnNumber, integer maximumColumnNumber) {
	Melder_require (columnNumber >= 1,
		U"The column number should be at least 1, not ", columnNumber, U".");
	Melder_require (columnNumber <= maximumColumnNumber,
		U"The column number should be at most ", maximumColumnNumber, U", not", columnNumber, U".");
}
template <typename T>
void checkColumnNumber (integer columnNumber, const constmatrix<T>& mat) {
	Melder_require (columnNumber >= 1,
		U"The column number should be at least 1, not ", columnNumber, U".");
	Melder_require (columnNumber <= mat.ncol,
		U"The column number should be at most the number of columns (", mat.ncol, U"), not", columnNumber, U".");
}
template <typename T>
void checkColumnNumber (integer columnNumber, const matrix<T>& x) {
	checkColumnNumber (columnNumber, constmatrix<T> (x));
}

template <typename T>
void checkColumnRange (integer firstColumn, integer lastColumn, const constmatrix<T>& x, integer minimumNewNumberOfColumns) {
	Melder_require (firstColumn >= 1,
		U"The first column should be at least 1, not ", firstColumn, U".");
	integer minimumLastColumn = firstColumn + (minimumNewNumberOfColumns - 1);
	Melder_require (lastColumn >= minimumLastColumn,
		U"The last column should be at least ", minimumLastColumn, U", not ", lastColumn,
		U", because the new matrix should contain at least ", minimumNewNumberOfColumns,
		U" columns (the first column is ", firstColumn, U")."
	);
	Melder_require (lastColumn <= x.ncol,
		U"The last column should be at most the number of columns (", x.ncol, U"), not", lastColumn, U".");
}
template <typename T>
void checkColumnRange (integer firstColumn, integer lastColumn, const matrix<T>& x, integer minimumNewNumberOfColumns) {
	checkColumnRange (firstColumn, lastColumn, constmatrix<T> (x), minimumNewNumberOfColumns);
}

inline void fixUnspecifiedRange (integer *inout_from, integer *inout_to, integer maximumAllowedValue) {
	if (*inout_from == 0) *inout_from = 1;
	if (*inout_to == 0) *inout_to = maximumAllowedValue;
}

template <typename T>
void fixUnspecifiedElementRange (integer *inout_firstElement, integer *inout_lastElement, const constvector<T> x) {
	fixUnspecifiedRange (inout_firstElement, inout_lastElement, x.size);
}
template <typename T>
void fixUnspecifiedElementRange (integer *inout_firstElement, integer *inout_lastElement, const vector<T> x) {
	fixUnspecifiedElementRange (inout_firstElement, inout_lastElement, constvector<T> (x));
}
template <typename T>
void fixAndCheckElementRange (integer *inout_firstElement, integer *inout_lastElement, const constvector<T> x, integer minimumNewNumberOfElements) {
	fixUnspecifiedElementRange (inout_firstElement, inout_lastElement, x);
	checkElementRange (*inout_firstElement, *inout_lastElement, x, minimumNewNumberOfElements);
}
template <typename T>
void fixAndCheckElementRange (integer *inout_firstElement, integer *inout_lastElement, const vector<T> x, integer minimumNewNumberOfElements) {
	fixAndCheckElementRange (inout_firstElement, inout_lastElement, constvector<T> (x), minimumNewNumberOfElements);
}
template <typename T>
void fixUnspecifiedRowRange (integer *inout_firstRow, integer *inout_lastRow, const constmatrix<T> x) {
	fixUnspecifiedRange (inout_firstRow, inout_lastRow, x.nrow);
}
template <typename T>
void fixUnspecifiedRowRange (integer *inout_firstRow, integer *inout_lastRow, const matrix<T> x) {
	fixUnspecifiedRange (inout_firstRow, inout_lastRow, x.nrow);
}
template <typename T>
void fixAndCheckRowRange (integer *inout_firstRow, integer *inout_lastRow, const constmatrix<T> x, integer minimumNewNumberOfRows) {
	fixUnspecifiedRowRange (inout_firstRow, inout_lastRow, x);
	checkRowRange (*inout_firstRow, *inout_lastRow, x, minimumNewNumberOfRows);
}
template <typename T>
void fixAndCheckRowRange (integer *inout_firstRow, integer *inout_lastRow, const matrix<T> x, integer minimumNewNumberOfRows) {
	fixAndCheckRowRange (inout_firstRow, inout_lastRow, constmatrix<T> (x), minimumNewNumberOfRows);
}
template <typename T>
void fixUnspecifiedColumnRange (integer *inout_firstColumn, integer *inout_lastColumn, const constmatrix<T> x) {
	fixUnspecifiedRange (inout_firstColumn, inout_lastColumn, x.ncol);
}
template <typename T>
void fixUnspecifiedColumnRange (integer *inout_firstColumn, integer *inout_lastColumn, const matrix<T> x) {
	fixUnspecifiedRange (inout_firstColumn, inout_lastColumn, x.ncol);
}
template <typename T>
void fixAndCheckColumnRange (integer *inout_firstColumn, integer *inout_lastColumn, const constmatrix<T> x, integer minimumNewNumberOfColumns) {
	fixUnspecifiedColumnRange (inout_firstColumn, inout_lastColumn, x);
	checkColumnRange (*inout_firstColumn, *inout_lastColumn, x, minimumNewNumberOfColumns);
}
template <typename T>
void fixAndCheckColumnRange (integer *inout_firstColumn, integer *inout_lastColumn, const matrix<T> x, integer minimumNewNumberOfColumns) {
	fixAndCheckColumnRange (inout_firstColumn, inout_lastColumn, constmatrix<T> (x), minimumNewNumberOfColumns);
}

/* End of file melder_require.h */
#endif
