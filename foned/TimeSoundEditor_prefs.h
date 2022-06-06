/* TimeSoundEditor_prefs.h
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

Prefs_begin (TimeSoundEditor)

	ClassPrefs_addBool       (TimeSoundEditor, sound_picture_preserveTimes, 1, true)
	ClassPrefs_addDouble     (TimeSoundEditor, sound_picture_bottom,        1, U"0.0")
	ClassPrefs_addDouble     (TimeSoundEditor, sound_picture_top,           1, U"0.0 (= auto)")
	ClassPrefs_addBool       (TimeSoundEditor, function_picture_garnish,    1, true)
	ClassPrefs_addEnum       (TimeSoundEditor, extract_windowShape,         1, kSound_windowShape, DEFAULT)
	ClassPrefs_addDouble     (TimeSoundEditor, extract_relativeWidth,       1, U"1.0")
	ClassPrefs_addBool       (TimeSoundEditor, extract_preserveTimes,       1, true)
	ClassPrefs_addDouble     (TimeSoundEditor, extract_overlap,             1, U"0.01")

Prefs_end (TimeSoundEditor)

/* End of file TimeSoundEditor_prefs.h */
