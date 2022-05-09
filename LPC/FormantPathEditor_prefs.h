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

EditorPrefs_begin (FormantPathEditor)

	EditorClassPrefs_overrideInt         (FormantPathEditor, shellWidth,                        1, U"800")
	EditorClassPrefs_overrideInt         (FormantPathEditor, shellHeight,                       1, U"600")	
	EditorClassPrefs_overrideBool        (FormantPathEditor, picture_garnish,                   1, true)
	EditorInstancePrefs_overrideBool    (FormantPathEditor, showSelectionViewer,               1, true)
	EditorInstancePrefs_overrideBool    (FormantPathEditor, spectrogram_show,                  1, true)
	EditorInstancePrefs_overrideBool    (FormantPathEditor, formant_show,                      1, true)
	
// from TextGridEditor
	EditorInstancePrefs_addBool    (FormantPathEditor, useTextStyles,          1, false)
	EditorInstancePrefs_addDouble  (FormantPathEditor, fontSize,               1, U"18")
	EditorInstancePrefs_addEnum    (FormantPathEditor, alignment,              1, kGraphics_horizontalAlignment, DEFAULT)
	EditorInstancePrefs_addBool    (FormantPathEditor, shiftDragMultiple,      1, false)
	EditorInstancePrefs_addEnum    (FormantPathEditor, showNumberOf,           1, kTextGridEditor_showNumberOf, DEFAULT)
	EditorInstancePrefs_addEnum    (FormantPathEditor, greenMethod,            1, kMelder_string, DEFAULT)
	EditorInstancePrefs_addString  (FormantPathEditor, greenString,            1, U"some text here for green paint")
	EditorInstancePrefs_addBool    (FormantPathEditor, picture_showBoundaries, 1, true)
	EditorInstancePrefs_addBool    (FormantPathEditor, picture_pitch_speckle,  1, false)
	EditorInstancePrefs_addString  (FormantPathEditor, align_language,         1, U"English")
	EditorInstancePrefs_addBool    (FormantPathEditor, align_includeWords,     1, true)
	EditorInstancePrefs_addBool    (FormantPathEditor, align_includePhonemes,  1, false)
	EditorInstancePrefs_addBool    (FormantPathEditor, align_allowSilences,    1, false)
//end from TextGridEditor	
	
	EditorInstancePrefs_addString  (FormantPathEditor, formant_path_evenColour,            1, U"pink")
	EditorInstancePrefs_addString  (FormantPathEditor, formant_path_oddColour,             1, U"red")
	EditorInstancePrefs_addString  (FormantPathEditor, formant_default_colour,             1, U"blue")
	EditorInstancePrefs_addString  (FormantPathEditor, formant_selected_colour,            1, U"pink")
	EditorInstancePrefs_addBool    (FormantPathEditor, formant_draw_showBandWidths,        1, false)
	
	EditorInstancePrefs_addInteger (FormantPathEditor, modeler_numberOfTracks,             1, U"3")
	EditorInstancePrefs_addString  (FormantPathEditor, modeler_numberOfParametersPerTrack, 1, U"3 3 3")
	EditorInstancePrefs_addDouble  (FormantPathEditor, modeler_varianceExponent,           1, U"1.25")
	EditorInstancePrefs_addBool    (FormantPathEditor, modeler_draw_showAllModels,         1, true)
	EditorInstancePrefs_addDouble  (FormantPathEditor, modeler_draw_maximumFrequency,      1, U"6200.0")
	EditorInstancePrefs_addBool    (FormantPathEditor, modeler_draw_estimatedModels,       1, false)
	EditorInstancePrefs_addBool    (FormantPathEditor, modeler_draw_showBandwidths,        1, true)
	EditorInstancePrefs_addDouble  (FormantPathEditor, modeler_draw_yGridLineEvery_Hz,     1, U"1000.0") // Hz
	
EditorPrefs_end (FormantPathEditor)
	
/* End of file FormantPathEditor_prefs.h */
