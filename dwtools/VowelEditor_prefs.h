/* VowelEditor_prefs.h
 *
 * Copyright (C) 2020 David Weenink
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

prefs_begin (VowelEditor)

	prefs_add_int_with_data    (VowelEditor, shell_width,                 1, U"900")
	prefs_add_int_with_data    (VowelEditor, shell_height,                1, U"700")
	prefs_add_bool_with_data   (VowelEditor, soundFollowsMouse,           1, true)
	prefs_add_enum_with_data   (VowelEditor, window_frequencyScale,       1, kVowelEditor_frequencyScale, LOGARITHMIC)
	prefs_add_double_with_data (VowelEditor, window_f1min,                1, U"200.0")   // Hz
	prefs_add_double_with_data (VowelEditor, window_f1max,                1, U"1200.0")   // Hz
	prefs_add_double_with_data (VowelEditor, window_f2min,                1, U"500.0")   // Hz
	prefs_add_double_with_data (VowelEditor, window_f2max,                1, U"3500.0")   // Hz
	prefs_add_double_with_data (VowelEditor, synthesis_samplingFrequency, 1, U"44100.0")   // Hz
	prefs_add_integer_with_data(VowelEditor, synthesis_numberOfFormants,  1, U"4")   // to synthesize the sound
	prefs_add_double_with_data (VowelEditor, synthesis_q1,                1, U"10.0")   // F/B
	prefs_add_double_with_data (VowelEditor, synthesis_q2,                1, U"10.0")   // F/B
	prefs_add_string_with_data (VowelEditor, synthesis_extraFBPairs,      1, U"2500 250 3500 350 4500 450")   // Hz
	prefs_add_double_with_data (VowelEditor, trajectory_markEvery,        1, U"0.05")   // s
	prefs_add_double_with_data (VowelEditor, trajectory_markLength,       1, U"0.02")   // s
	prefs_add_string_with_data (VowelEditor, trajectory_colour,           1, U"Black")   // Colour
	prefs_add_double_with_data (VowelEditor, trajectory_minimumDuration,  1, U"0.05")  // 50 ms is audible
	prefs_add_double_with_data (VowelEditor, trajectory_duration,         1, U"0.1")   // s
	prefs_add_double_with_data (VowelEditor, trajectory_extendDuration,   1, U"0.1")   // s
	prefs_add_double_with_data (VowelEditor, trajectory_newDuration,      1, U"0.2")   //
	prefs_add_double_with_data (VowelEditor, trajectory_lineWidth,        1, U"2.5")   //
	prefs_add_enum_with_data   (VowelEditor, marks_speakerType,           1, kVowelEditor_speakerType, WOMAN)
	prefs_add_enum_with_data   (VowelEditor, marks_dataSet,               1, kVowelEditor_marksDataSet, AMERICAN_ENGLISH)
	prefs_add_string_with_data (VowelEditor, marks_fileName,              1, U"")
	prefs_add_string_with_data (VowelEditor, marks_colour,                1, U"grey")
	prefs_add_double_with_data (VowelEditor, marks_fontSize,              1, U"14.0")
	prefs_add_double_with_data (VowelEditor, f0_start,                    1, U"140.0")   // Hz
	prefs_add_double_with_data (VowelEditor, f0_slope,                    1, U"0.0")   // oct/s
	prefs_add_double_with_data (VowelEditor, f0_minimum,                  1, U"20.0")   // Hz
	prefs_add_double_with_data (VowelEditor, f0_maximum,                  1, U"2000.0")   // Hz
	prefs_add_double_with_data (VowelEditor, grid_df1,                    1, U"200.0")   // Hz
	prefs_add_double_with_data (VowelEditor, grid_df2,                    1, U"500.0")   // Hz

prefs_end (VowelEditor)

/* End of file VowelEditor_prefs.h */
