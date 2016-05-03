/* FunctionEditor_prefs.h
 *
 * Copyright (C) 2013,2015 Paul Boersma
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

prefs_begin (FunctionEditor)

	prefs_add_int  (FunctionEditor, shellWidth,                 1, U"700")
	prefs_add_int  (FunctionEditor, shellHeight,                1, U"440")
	prefs_add_bool (FunctionEditor, synchronizedZoomAndScroll,  1, true)
	prefs_add_bool_with_data   (FunctionEditor, showSelectionViewer, 1, false)
	prefs_add_double_with_data (FunctionEditor, arrowScrollStep,     1, U"0.05")
	prefs_add_bool (FunctionEditor, picture_drawSelectionTimes, 1, true)
	prefs_add_bool (FunctionEditor, picture_drawSelectionHairs, 1, true)

prefs_end (FunctionEditor)

/* End of file FunctionEditor_prefs.h */
