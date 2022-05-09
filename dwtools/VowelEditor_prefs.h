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

Prefs_begin (VowelEditor)

	ClassPrefs_overrideInt    (VowelEditor, shellWidth,                  1, U"900")
	ClassPrefs_overrideInt    (VowelEditor, shellHeight,                 1, U"700")
	InstancePrefs_addBool     (VowelEditor, soundFollowsMouse,           1, true)
	InstancePrefs_addEnum     (VowelEditor, window_frequencyScale,       1, kVowelEditor_frequencyScale, LOGARITHMIC)
	InstancePrefs_addDouble   (VowelEditor, window_f1min,                1, U"200.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, window_f1max,                1, U"1200.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, window_f2min,                1, U"500.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, window_f2max,                1, U"3500.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, synthesis_samplingFrequency, 1, U"44100.0")   // Hz
	InstancePrefs_addInteger  (VowelEditor, synthesis_numberOfFormants,  1, U"4")   // to synthesize the sound
	InstancePrefs_addDouble   (VowelEditor, synthesis_q1,                1, U"10.0")   // F/B
	InstancePrefs_addDouble   (VowelEditor, synthesis_q2,                1, U"10.0")   // F/B
	InstancePrefs_addString   (VowelEditor, synthesis_extraFBPairs,      1, U"2500 250 3500 350 4500 450")   // Hz
	InstancePrefs_addDouble   (VowelEditor, trajectory_markEvery,        1, U"0.05")   // s
	InstancePrefs_addDouble   (VowelEditor, trajectory_markLength,       1, U"0.02")   // s
	InstancePrefs_addString   (VowelEditor, trajectory_colour,           1, U"Black")   // Colour
	InstancePrefs_addDouble   (VowelEditor, trajectory_minimumDuration,  1, U"0.05")  // 50 ms is audible
	InstancePrefs_addDouble   (VowelEditor, trajectory_duration,         1, U"0.1")   // s
	InstancePrefs_addDouble   (VowelEditor, trajectory_extendDuration,   1, U"0.1")   // s
	InstancePrefs_addDouble   (VowelEditor, trajectory_newDuration,      1, U"0.2")   //
	InstancePrefs_addDouble   (VowelEditor, trajectory_lineWidth,        1, U"2.5")   //
	InstancePrefs_addEnum     (VowelEditor, marks_speakerType,           1, kVowelEditor_speakerType, WOMAN)
	InstancePrefs_addEnum     (VowelEditor, marks_dataSet,               1, kVowelEditor_marksDataSet, AMERICAN_ENGLISH)
	InstancePrefs_addString   (VowelEditor, marks_fileName,              1, U"")
	InstancePrefs_addString   (VowelEditor, marks_colour,                1, U"grey")
	InstancePrefs_addDouble   (VowelEditor, marks_fontSize,              1, U"14.0")
	InstancePrefs_addDouble   (VowelEditor, f0_start,                    1, U"140.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, f0_slope,                    1, U"0.0")   // oct/s
	InstancePrefs_addDouble   (VowelEditor, f0_minimum,                  1, U"20.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, f0_maximum,                  1, U"2000.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, grid_df1,                    1, U"200.0")   // Hz
	InstancePrefs_addDouble   (VowelEditor, grid_df2,                    1, U"500.0")   // Hz

Prefs_end (VowelEditor)

/* End of file VowelEditor_prefs.h */
