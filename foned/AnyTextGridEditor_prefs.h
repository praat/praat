/* AnyTextGridEditor_prefs.h
 *
 * Copyright (C) 2013,2015,2016,2019,2022 Paul Boersma
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

Prefs_begin (AnyTextGridEditor)

	ClassPrefs_overrideInt        (AnyTextGridEditor, shellWidth,               1, U"800")
	ClassPrefs_overrideInt        (AnyTextGridEditor, shellHeight,              1, U"600")
	ClassPrefs_overrideBool       (AnyTextGridEditor, function_picture_garnish, 1, true)
	InstancePrefs_overrideBool    (AnyTextGridEditor, showSelectionViewer,      1, true)

	InstancePrefs_addBool         (AnyTextGridEditor, useTextStyles,            1, false)
	InstancePrefs_addDouble       (AnyTextGridEditor, fontSize,                 1, U"18")
	InstancePrefs_addEnum         (AnyTextGridEditor, alignment,                1, kGraphics_horizontalAlignment, DEFAULT)
	InstancePrefs_addBool         (AnyTextGridEditor, shiftDragMultiple,        1, true)
	InstancePrefs_addEnum         (AnyTextGridEditor, showNumberOf,             1, kTextGridArea_showNumberOf, DEFAULT)
	InstancePrefs_addEnum         (AnyTextGridEditor, greenMethod,              1, kMelder_string, DEFAULT)
	InstancePrefs_addString       (AnyTextGridEditor, greenString,              1, U"some text here for green paint")
	ClassPrefs_addBool            (AnyTextGridEditor, picture_showBoundaries,   1, true)
	ClassPrefs_addBool            (AnyTextGridEditor, picture_pitch_speckle,    1, false)
	InstancePrefs_addString       (AnyTextGridEditor, align_language,           1, U"English")
	InstancePrefs_addBool         (AnyTextGridEditor, align_includeWords,       1, true)
	InstancePrefs_addBool         (AnyTextGridEditor, align_includePhonemes,    1, false)
	InstancePrefs_addBool         (AnyTextGridEditor, align_allowSilences,      1, false)

Prefs_end (AnyTextGridEditor)

/* End of file AnyTextGridEditor_prefs.h */
