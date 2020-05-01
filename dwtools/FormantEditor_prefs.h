/* FormantEditor_prefs.h
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

prefs_begin (FormantEditor)

	prefs_override_int         (FormantEditor, shellWidth,                        1, U"800")
	prefs_override_int         (FormantEditor, shellHeight,                       1, U"600")	
	prefs_override_bool        (FormantEditor, picture_garnish,                   1, true)
	prefs_override_bool        (FormantEditor, showSelectionViewer,               1, true)
	
// from TextGridEditor
	prefs_add_bool_with_data   (FormantEditor, useTextStyles,          1, false)
	prefs_add_double_with_data (FormantEditor, fontSize,               1, U"18")
	prefs_add_enum_with_data   (FormantEditor, alignment,              1, kGraphics_horizontalAlignment, DEFAULT)
	prefs_add_bool_with_data   (FormantEditor, shiftDragMultiple,      1, true)
	prefs_add_enum_with_data   (FormantEditor, showNumberOf,           1, kTextGridEditor_showNumberOf, DEFAULT)
	prefs_add_enum_with_data   (FormantEditor, greenMethod,            1, kMelder_string, DEFAULT)
	prefs_add_string_with_data (FormantEditor, greenString,            1, U"some text here for green paint")
	prefs_add_bool             (FormantEditor, picture_showBoundaries, 1, true)
	prefs_add_bool             (FormantEditor, picture_pitch_speckle,  1, false)
	prefs_add_string_with_data (FormantEditor, align_language,         1, U"English")
	prefs_add_bool_with_data   (FormantEditor, align_includeWords,     1, true)
	prefs_add_bool_with_data   (FormantEditor, align_includePhonemes,  1, false)
	prefs_add_bool_with_data   (FormantEditor, align_allowSilences,    1, false)	
//end from TextGridEditor	
	
	prefs_add_bool_with_data   (FormantEditor, formant_draw_showBandWidths,       1, true)
	prefs_add_double_with_data (FormantEditor, formant_draw_trackOffsetX_s,       1, U"0.001")
	prefs_add_integer_with_data(FormantEditor, modeler_numberOfTracks,            1, U"3")
	prefs_add_string_with_data (FormantEditor, modeler_numberOfParametersPerTrack,1, U"3 3 2")
	prefs_add_double_with_data (FormantEditor, modeler_varianceExponent,          1, U"1.25")
	prefs_add_bool_with_data   (FormantEditor, modeler_draw_allModels,            1, true)
	prefs_add_double_with_data (FormantEditor, modeler_draw_xSpace_fraction,      1, U"0.1")
	prefs_add_double_with_data (FormantEditor, modeler_draw_ySpace_fraction,      1, U"0.1")
	prefs_add_double_with_data (FormantEditor, modeler_draw_maximumFrequency,     1, U"5000.0")
	prefs_add_bool_with_data   (FormantEditor, modeler_draw_estimatedTracks,      1, false)
	prefs_add_bool_with_data   (FormantEditor, modeler_draw_errorBars,            1, true)
	prefs_add_double_with_data (FormantEditor, modeler_draw_errorBarWidth_s,      1, U"0.001")
	prefs_add_double_with_data (FormantEditor, modeler_draw_xTrackShift_s,        1, U"-0.001") // s
	prefs_add_string_with_data (FormantEditor, modeler_draw_best3colour,          1, U"green pink blue") //
	prefs_add_double_with_data (FormantEditor, modeler_draw_yGridLineEvery_Hz,    1, U"1000.0") // Hz
	
	prefs_add_enum_with_data   (FormantEditor, analysisHistory_lpcType,           1, kLPC_Analysis, BURG)
	prefs_add_double_with_data (FormantEditor, analysisHistory_timeStep,          1, U"0.005") // s
	prefs_add_double_with_data (FormantEditor, analysisHistory_maximumNumberOfFormants, 1, U"5.0")
	prefs_add_double_with_data (FormantEditor, analysisHistory_windowLength	,     1, U"0.025") // s
	prefs_add_double_with_data (FormantEditor, analysisHistory_preemphasisFrequency, 1, U"50.0") // Hz
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol1,              1, U"1e-6")
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol2,              1, U"1e-6")
	prefs_add_double_with_data (FormantEditor, analysisHistory_numberOfStdDev,    1, U"1.5")
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol,               1, U"1e-6")
	prefs_add_integer_with_data(FormantEditor, analysisHistory_maximumNumberOfIterations, 1, U"5")
	prefs_add_integer_with_data(FormantEditor, analysisHistory_numberOfCeilings,  1, U"9")
	prefs_add_double_with_data (FormantEditor, analysisHistory_minimumCeiling,    1, U"4000.0")
	prefs_add_double_with_data (FormantEditor, analysisHistory_maximumCeiling,    1, U"6000.0")
	prefs_add_double_with_data (FormantEditor, analysisHistory_formantSafetyMargin,1, U"50.0") // Hz

prefs_end (FormantEditor)
	
/* End of file FormantEditor_prefs.h */
