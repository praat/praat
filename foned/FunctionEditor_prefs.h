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

Prefs_begin (FunctionEditor)

	ClassPrefs_overrideInt   (FunctionEditor, shellWidth,                 1, U"700")
	ClassPrefs_overrideInt   (FunctionEditor, shellHeight,                1, U"440")
	ClassPrefs_addBool       (FunctionEditor, synchronizedZoomAndScroll,  1, true)
	InstancePrefs_addBool    (FunctionEditor, showSelectionViewer, 1, false)
	InstancePrefs_addDouble  (FunctionEditor, arrowScrollStep,     1, U"0.05")

Prefs_end (FunctionEditor)

/* End of file FunctionEditor_prefs.h */
