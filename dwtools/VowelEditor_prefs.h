/* VowelEditor_prefs.h
 *
 * Copyright (C) 2020 David Weenink, 2022 Paul Boersma
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

EditorPrefs_begin (VowelEditor)

	prefs_add_int_with_data    (VowelEditor, shell_width,                 1, U"900")
	prefs_add_int_with_data    (VowelEditor, shell_height,                1, U"700")
	prefs_add_bool_with_data   (VowelEditor, soundFollowsMouse,           1, true)
	prefs_add_enum_with_data   (VowelEditor, window_frequencyScale,       1, kVowelEditor_frequencyScale, LOGARITHMIC)
	EditorInstancePrefs_addDouble  (VowelEditor, window_f1min,                1, U"200.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, window_f1max,                1, U"1200.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, window_f2min,                1, U"500.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, window_f2max,                1, U"3500.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, synthesis_samplingFrequency, 1, U"44100.0")   // Hz
	prefs_add_integer_with_data(VowelEditor, synthesis_numberOfFormants,  1, U"4")   // to synthesize the sound
	EditorInstancePrefs_addDouble  (VowelEditor, synthesis_q1,                1, U"10.0")   // F/B
	EditorInstancePrefs_addDouble  (VowelEditor, synthesis_q2,                1, U"10.0")   // F/B
	prefs_add_string_with_data (VowelEditor, synthesis_extraFBPairs,      1, U"2500 250 3500 350 4500 450")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_markEvery,        1, U"0.05")   // s
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_markLength,       1, U"0.02")   // s
	prefs_add_string_with_data (VowelEditor, trajectory_colour,           1, U"Black")   // Colour
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_minimumDuration,  1, U"0.05")  // 50 ms is audible
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_duration,         1, U"0.1")   // s
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_extendDuration,   1, U"0.1")   // s
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_newDuration,      1, U"0.2")   //
	EditorInstancePrefs_addDouble  (VowelEditor, trajectory_lineWidth,        1, U"2.5")   //
	prefs_add_enum_with_data   (VowelEditor, marks_speakerType,           1, kVowelEditor_speakerType, WOMAN)
	prefs_add_enum_with_data   (VowelEditor, marks_dataSet,               1, kVowelEditor_marksDataSet, AMERICAN_ENGLISH)
	prefs_add_string_with_data (VowelEditor, marks_fileName,              1, U"")
	prefs_add_string_with_data (VowelEditor, marks_colour,                1, U"grey")
	EditorInstancePrefs_addDouble  (VowelEditor, marks_fontSize,              1, U"14.0")
	EditorInstancePrefs_addDouble  (VowelEditor, f0_start,                    1, U"140.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, f0_slope,                    1, U"0.0")   // oct/s
	EditorInstancePrefs_addDouble  (VowelEditor, f0_minimum,                  1, U"20.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, f0_maximum,                  1, U"2000.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, grid_df1,                    1, U"200.0")   // Hz
	EditorInstancePrefs_addDouble  (VowelEditor, grid_df2,                    1, U"500.0")   // Hz

EditorPrefs_end (VowelEditor)

/* End of file VowelEditor_prefs.h */
