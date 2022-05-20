/* FormantPathEditor_prefs.h
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

Prefs_begin (FormantPathEditor)

	ClassPrefs_overrideInt        (FormantPathEditor, shellWidth,                        1, U"800")
	ClassPrefs_overrideInt        (FormantPathEditor, shellHeight,                       1, U"600")
	ClassPrefs_overrideBool       (FormantPathEditor, function_picture_garnish,          1, true)
	InstancePrefs_overrideBool    (FormantPathEditor, showSelectionViewer,               1, true)
	InstancePrefs_overrideBool    (FormantPathEditor, spectrogram_show,                  1, true)
	InstancePrefs_overrideBool    (FormantPathEditor, formant_show,                      1, true)
	
// from TextGridEditor
	InstancePrefs_addBool    (FormantPathEditor, useTextStyles,          1, false)
	InstancePrefs_addDouble  (FormantPathEditor, fontSize,               1, U"18")
	InstancePrefs_addEnum    (FormantPathEditor, alignment,              1, kGraphics_horizontalAlignment, DEFAULT)
	InstancePrefs_addBool    (FormantPathEditor, shiftDragMultiple,      1, false)
	InstancePrefs_addEnum    (FormantPathEditor, showNumberOf,           1, kTextGridEditor_showNumberOf, DEFAULT)
	InstancePrefs_addEnum    (FormantPathEditor, greenMethod,            1, kMelder_string, DEFAULT)
	InstancePrefs_addString  (FormantPathEditor, greenString,            1, U"some text here for green paint")
	InstancePrefs_addBool    (FormantPathEditor, picture_showBoundaries, 1, true)
	InstancePrefs_addBool    (FormantPathEditor, picture_pitch_speckle,  1, false)
	InstancePrefs_addString  (FormantPathEditor, align_language,         1, U"English")
	InstancePrefs_addBool    (FormantPathEditor, align_includeWords,     1, true)
	InstancePrefs_addBool    (FormantPathEditor, align_includePhonemes,  1, false)
	InstancePrefs_addBool    (FormantPathEditor, align_allowSilences,    1, false)
//end from TextGridEditor	
	
	InstancePrefs_addString  (FormantPathEditor, formant_path_evenColour,            1, U"pink")
	InstancePrefs_addString  (FormantPathEditor, formant_path_oddColour,             1, U"red")
	InstancePrefs_addString  (FormantPathEditor, formant_default_colour,             1, U"blue")
	InstancePrefs_addString  (FormantPathEditor, formant_selected_colour,            1, U"pink")
	InstancePrefs_addBool    (FormantPathEditor, formant_draw_showBandWidths,        1, false)
	
	InstancePrefs_addInteger (FormantPathEditor, modeler_numberOfTracks,             1, U"3")
	InstancePrefs_addString  (FormantPathEditor, modeler_numberOfParametersPerTrack, 1, U"3 3 3")
	InstancePrefs_addDouble  (FormantPathEditor, modeler_varianceExponent,           1, U"1.25")
	InstancePrefs_addBool    (FormantPathEditor, modeler_draw_showAllModels,         1, true)
	InstancePrefs_addDouble  (FormantPathEditor, modeler_draw_maximumFrequency,      1, U"6200.0")
	InstancePrefs_addBool    (FormantPathEditor, modeler_draw_estimatedModels,       1, false)
	InstancePrefs_addBool    (FormantPathEditor, modeler_draw_showBandwidths,        1, true)
	InstancePrefs_addDouble  (FormantPathEditor, modeler_draw_yGridLineEvery_Hz,     1, U"1000.0") // Hz
	
Prefs_end (FormantPathEditor)
	
/* End of file FormantPathEditor_prefs.h */
