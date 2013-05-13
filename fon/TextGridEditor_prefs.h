/* TextGridEditor_prefs.h
 *
 * Copyright (C) 2013 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

prefs_begin (TextGridEditor)
	// overridden:
		prefs_add_int    (TextGridEditor, shellWidth,                 1, L"800")
		prefs_add_int    (TextGridEditor, shellHeight,                1, L"600")
		prefs_add_bool   (TextGridEditor, picture_garnish,            1, true)
	// new:
		prefs_add_bool_with_data   (TextGridEditor, useTextStyles,      1, false)
		prefs_add_int_with_data    (TextGridEditor, fontSize,           1, L"18")
		prefs_add_enum_with_data   (TextGridEditor, alignment,          1, kGraphics_horizontalAlignment, DEFAULT)
		prefs_add_bool_with_data   (TextGridEditor, shiftDragMultiple,  1, true)
		prefs_add_enum_with_data   (TextGridEditor, showNumberOf,       1, kTextGridEditor_showNumberOf, DEFAULT)
		prefs_add_enum_with_data   (TextGridEditor, greenMethod,        1, kMelder_string, DEFAULT)
		prefs_add_string_with_data (TextGridEditor, greenString,        1, L"some text here for green paint")
		prefs_add_bool   (TextGridEditor, picture_showBoundaries,     1, true)
		prefs_add_bool   (TextGridEditor, picture_pitch_speckle,      1, false)
		prefs_add_string_with_data (TextGridEditor, align_language,        1, L"English")
		prefs_add_bool_with_data   (TextGridEditor, align_includeWords,    1, true)
		prefs_add_bool_with_data   (TextGridEditor, align_includePhonemes, 1, false)
		prefs_add_bool_with_data   (TextGridEditor, align_allowSilences,   1, false)
prefs_end (TextGridEditor)

/* End of file TextGridEditor_prefs.h */
