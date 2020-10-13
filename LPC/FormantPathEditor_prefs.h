/* FormantPathEditor_prefs.h
 *
 * Copyright (C) 2020 David Weenink
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

prefs_begin (FormantPathEditor)

	prefs_override_int         (FormantPathEditor, shellWidth,                        1, U"800")
	prefs_override_int         (FormantPathEditor, shellHeight,                       1, U"600")	
	prefs_override_bool        (FormantPathEditor, picture_garnish,                   1, true)
	prefs_override_bool        (FormantPathEditor, showSelectionViewer,               1, true)
	prefs_override_bool        (FormantPathEditor, spectrogram_show,                  1, true)
	prefs_override_bool        (FormantPathEditor, formant_show,                      1, true)
	
// from TextGridEditor
	prefs_add_bool_with_data   (FormantPathEditor, useTextStyles,          1, false)
	prefs_add_double_with_data (FormantPathEditor, fontSize,               1, U"18")
	prefs_add_enum_with_data   (FormantPathEditor, alignment,              1, kGraphics_horizontalAlignment, DEFAULT)
	prefs_add_bool_with_data   (FormantPathEditor, shiftDragMultiple,      1, false)
	prefs_add_enum_with_data   (FormantPathEditor, showNumberOf,           1, kTextGridEditor_showNumberOf, DEFAULT)
	prefs_add_enum_with_data   (FormantPathEditor, greenMethod,            1, kMelder_string, DEFAULT)
	prefs_add_string_with_data (FormantPathEditor, greenString,            1, U"some text here for green paint")
	prefs_add_bool_with_data   (FormantPathEditor, picture_showBoundaries, 1, true)
	prefs_add_bool_with_data   (FormantPathEditor, picture_pitch_speckle,  1, false)
	prefs_add_string_with_data (FormantPathEditor, align_language,         1, U"English")
	prefs_add_bool_with_data   (FormantPathEditor, align_includeWords,     1, true)
	prefs_add_bool_with_data   (FormantPathEditor, align_includePhonemes,  1, false)
	prefs_add_bool_with_data   (FormantPathEditor, align_allowSilences,    1, false)	
//end from TextGridEditor	
	
	prefs_add_string_with_data (FormantPathEditor, formant_path_evenColour,            1, U"pink")
	prefs_add_string_with_data (FormantPathEditor, formant_path_oddColour,             1, U"red")
	prefs_add_string_with_data (FormantPathEditor, formant_default_colour,             1, U"blue")
	prefs_add_string_with_data (FormantPathEditor, formant_selected_colour,            1, U"pink")	
	prefs_add_bool_with_data   (FormantPathEditor, formant_draw_showBandWidths,        1, false)
	
	prefs_add_integer_with_data(FormantPathEditor, modeler_numberOfTracks,             1, U"3")
	prefs_add_string_with_data (FormantPathEditor, modeler_numberOfParametersPerTrack, 1, U"7 7 7")
	prefs_add_double_with_data (FormantPathEditor, modeler_varianceExponent,           1, U"1.25")
	prefs_add_bool_with_data   (FormantPathEditor, modeler_draw_showAllModels,         1, true)
	prefs_add_double_with_data (FormantPathEditor, modeler_draw_maximumFrequency,      1, U"5000.0")
	prefs_add_bool_with_data   (FormantPathEditor, modeler_draw_estimatedModels,       1, false)
	prefs_add_bool_with_data   (FormantPathEditor, modeler_draw_showErrorBars,         1, true)
	prefs_add_double_with_data (FormantPathEditor, modeler_draw_yGridLineEvery_Hz,     1, U"1000.0") // Hz
	
prefs_end (FormantPathEditor)
	
/* End of file FormantPathEditor_prefs.h */
