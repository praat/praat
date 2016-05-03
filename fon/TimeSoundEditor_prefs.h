/* TimeSoundEditor_prefs.h
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

prefs_begin (TimeSoundEditor)

	prefs_add_enum_with_data   (TimeSoundEditor, sound_scalingStrategy,  1, kTimeSoundEditor_scalingStrategy, DEFAULT)
	prefs_add_double_with_data (TimeSoundEditor, sound_scaling_height,   1, U"2.0")
	prefs_add_double_with_data (TimeSoundEditor, sound_scaling_minimum,  1, U"-1.0")
	prefs_add_double_with_data (TimeSoundEditor, sound_scaling_maximum,  1, U"1.0")
	prefs_add_bool             (TimeSoundEditor, picture_preserveTimes,  1, true)
	prefs_add_double           (TimeSoundEditor, picture_bottom,         1, U"0.0")
	prefs_add_double           (TimeSoundEditor, picture_top,            1, U"0.0 (= auto)")
	prefs_add_bool             (TimeSoundEditor, picture_garnish,        1, true)
	prefs_add_enum             (TimeSoundEditor, extract_windowShape,    1, kSound_windowShape, DEFAULT)
	prefs_add_double           (TimeSoundEditor, extract_relativeWidth,  1, U"1.0")
	prefs_add_bool             (TimeSoundEditor, extract_preserveTimes,  1, true)
	prefs_add_double           (TimeSoundEditor, extract_overlap,        1, U"0.01")

prefs_end (TimeSoundEditor)

/* End of file TimeSoundEditor_prefs.h */
