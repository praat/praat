/* FunctionEditor_prefs.h
 *
 * Copyright (C) 2013,2015,2016,2022 Paul Boersma
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

EditorPrefs_begin (FunctionEditor)

	EditorClassPrefs_overrideInt  (FunctionEditor, shellWidth,                 1, U"700")
	EditorClassPrefs_overrideInt  (FunctionEditor, shellHeight,                1, U"440")
	prefs_add_bool (FunctionEditor, synchronizedZoomAndScroll,  1, true)
	prefs_add_bool_with_data   (FunctionEditor, showSelectionViewer, 1, false)
	EditorInstancePrefs_addDouble (FunctionEditor, arrowScrollStep,     1, U"0.05")
	prefs_add_bool (FunctionEditor, picture_drawSelectionTimes, 1, true)
	prefs_add_bool (FunctionEditor, picture_drawSelectionHairs, 1, true)

EditorPrefs_end (FunctionEditor)

/* End of file FunctionEditor_prefs.h */
