/* FormantPathArea_prefs.h
 *
 * Copyright (C) 2020 David Weenink, 2022 Paul Boersma
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

Prefs_begin (FormantPathArea)

	ClassPrefs_overrideBool       (FormantPathArea, picture_garnish,          1, true)
	InstancePrefs_overrideBool    (FormantPathArea, spectrogram_show,         1, true)
	InstancePrefs_overrideBool    (FormantPathArea, formant_show,             1, true)
	InstancePrefs_overrideBool    (FormantPathArea, pitch_show,               1, false)
	InstancePrefs_overrideBool    (FormantPathArea, intensity_show,           1, false)
	InstancePrefs_overrideBool    (FormantPathArea, pulses_show,              1, false)

	InstancePrefs_addString  (FormantPathArea, formant_path_evenColour,            1, U"pink")
	InstancePrefs_addString  (FormantPathArea, formant_path_oddColour,             1, U"red")
	InstancePrefs_addString  (FormantPathArea, formant_default_colour,             1, U"blue")
	InstancePrefs_addString  (FormantPathArea, formant_selected_colour,            1, U"pink")
	InstancePrefs_addBool    (FormantPathArea, formant_draw_showBandWidths,        1, false)
	
Prefs_end (FormantPathArea)
	
/* End of file FormantPathArea_prefs.h */
