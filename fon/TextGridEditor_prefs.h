/* TextGridEditor_prefs.h
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

Prefs_begin (TextGridEditor)

	ClassPrefs_overrideInt        (TextGridEditor, shellWidth,             1, U"800")
	ClassPrefs_overrideInt        (TextGridEditor, shellHeight,            1, U"600")
	ClassPrefs_overrideBool       (TextGridEditor, picture_garnish,        1, true)
	InstancePrefs_overrideBool    (TextGridEditor, showSelectionViewer,    1, true)

	InstancePrefs_addBool         (TextGridEditor, useTextStyles,          1, false)
	InstancePrefs_addDouble       (TextGridEditor, fontSize,               1, U"18")
	InstancePrefs_addEnum         (TextGridEditor, alignment,              1, kGraphics_horizontalAlignment, DEFAULT)
	InstancePrefs_addBool         (TextGridEditor, shiftDragMultiple,      1, true)
	InstancePrefs_addEnum         (TextGridEditor, showNumberOf,           1, kTextGridEditor_showNumberOf, DEFAULT)
	InstancePrefs_addEnum         (TextGridEditor, greenMethod,            1, kMelder_string, DEFAULT)
	InstancePrefs_addString       (TextGridEditor, greenString,            1, U"some text here for green paint")
	ClassPrefs_addBool            (TextGridEditor, picture_showBoundaries, 1, true)
	ClassPrefs_addBool            (TextGridEditor, picture_pitch_speckle,  1, false)
	InstancePrefs_addString       (TextGridEditor, align_language,         1, U"English")
	InstancePrefs_addBool         (TextGridEditor, align_includeWords,     1, true)
	InstancePrefs_addBool         (TextGridEditor, align_includePhonemes,  1, false)
	InstancePrefs_addBool         (TextGridEditor, align_allowSilences,    1, false)

Prefs_end (TextGridEditor)

/* End of file TextGridEditor_prefs.h */
