/* Graphics_extensions_enums.h
 *
 * Copyright (C) 2018 David Weenink
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

enums_begin (kGraphicsMatrixCellDrawingOrder, 1)
	enums_add (kGraphicsMatrixCellDrawingOrder, 1, Rows, U"rows")
	enums_add (kGraphicsMatrixCellDrawingOrder, 2, Columns, U"columns")
	enums_add (kGraphicsMatrixCellDrawingOrder, 3, IncreasingValues, U"increasing-values")
	enums_add (kGraphicsMatrixCellDrawingOrder, 4, DecreasingValues, U"decreasing-values")
	enums_add (kGraphicsMatrixCellDrawingOrder, 5, Random, U"random")
enums_end (kGraphicsMatrixCellDrawingOrder, 5, Rows)

enums_begin (kGraphicsMatrixOrigin, 1)
	enums_add (kGraphicsMatrixOrigin, 1, TopLeft, U"top-left")
	enums_add (kGraphicsMatrixOrigin, 2, TopRight, U"top-right")
	enums_add (kGraphicsMatrixOrigin, 3, BottomLeft, U"bottom-left")
	enums_add (kGraphicsMatrixOrigin, 4, BottomRight, U"bottom-right")
enums_end (kGraphicsMatrixOrigin, 4, TopLeft)

/* End of file Graphics_extensions_enums.h */
