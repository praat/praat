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
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfTracks,            1, U"3")
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfParametersF1Track, 1, U"3")
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfParametersF2Track, 1, U"3")
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfParametersF3Track, 1, U"2")
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfParametersF4Track, 1, U"0")
	prefs_add_int_with_data    (FormantEditor, modeler_numberOfParametersF5Track, 1, U"0")
	prefs_add_bool_with_data   (FormantEditor, modeler_draw_errorBars,            1, true)
	prefs_add_double_with_data (FormantEditor, modeler_draw_errorBarWidth_mm,     1, U"1.0")
	prefs_add_double_with_data (FormantEditor, modeler_draw_trackOffsetX_s,       1, U"0.001") // s
	prefs_add_enum_with_data   (FormantEditor, analysisHistory_lpcType,           1, kLPC_Analysis, BURG)
	prefs_add_double_with_data (FormantEditor, analysisHistory_timeStep,          1, U"0.005") // s
	prefs_add_double_with_data (FormantEditor, analysisHistory_maximumNumberOfFormants, 1, U"5.0")
	prefs_add_double_with_data (FormantEditor, analysisHistory_windowLength	,     1, U"0.025") // s
	prefs_add_double_with_data (FormantEditor, analysisHistory_preemphasisFrequency, 1, U"50.0") // Hz
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol1,              1, U"1e-6")
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol2,              1, U"1e-6")
	prefs_add_double_with_data (FormantEditor, analysisHistory_numberOfStdDev,    1, U"1.5")
	prefs_add_double_with_data (FormantEditor, analysisHistory_tol,               1, U"1e-6")
	prefs_add_integer_with_data (FormantEditor, analysisHistory_maximumNumberOfIterations, 1, U"5")
	prefs_add_integer_with_data (FormantEditor, analysisHistory_numberOfCeilings, 1, U"9")
	prefs_add_double_with_data (FormantEditor, analysisHistory_minimumCeiling,    1, U"4000.0")
	prefs_add_double_with_data (FormantEditor, analysisHistory_maximumCeiling,    1, U"6000.0")

prefs_end (FormantEditor)
	
/* End of file FormantEditor_prefs.h */
