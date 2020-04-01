/* Graphics_extensions_enums.h
 *
 * Copyright (C) 2018-2020 David Weenink
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
	enums_add (kGraphicsMatrixCellDrawingOrder, 1, ROWS, U"rows")
	enums_add (kGraphicsMatrixCellDrawingOrder, 2, COLUMNS, U"columns")
	enums_add (kGraphicsMatrixCellDrawingOrder, 3, INCREASING_VALUES, U"increasing values")
	enums_add (kGraphicsMatrixCellDrawingOrder, 4, DECREASING_VALUES, U"decreasing values")
	enums_add (kGraphicsMatrixCellDrawingOrder, 5, RANDOM, U"random")
enums_end (kGraphicsMatrixCellDrawingOrder, 5, RANDOM)

enums_begin (kGraphicsMatrixOrigin, 1)
	enums_add (kGraphicsMatrixOrigin, 1, TOP_LEFT, U"top-left")
	enums_add (kGraphicsMatrixOrigin, 2, TOP_RIGHT, U"top-right")
	enums_add (kGraphicsMatrixOrigin, 3, BOTTOM_LEFT, U"bottom-left")
	enums_add (kGraphicsMatrixOrigin, 4, BOTTOM_RIGHT, U"bottom-right")
enums_end (kGraphicsMatrixOrigin, 4, BOTTOM_RIGHT)

/* End of file Graphics_extensions_enums.h */
