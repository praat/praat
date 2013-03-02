/* FormantGridEditor_prefs.h
 *
 * Copyright (C) 2013 Paul Boersma
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

prefs_begin (FormantGridEditor)
	// new:
		prefs_add_double_with_data (FormantGridEditor, formantFloor,                    1, L"0.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, formantCeiling,                  1, L"11000.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, bandwidthFloor,                  1, L"0.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, bandwidthCeiling,                1, L"1000.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, play_samplingFrequency,          1, L"44100.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, source_pitch_tStart,             1, L"0.0%")   // relative time
		prefs_add_double_with_data (FormantGridEditor, source_pitch_f0Start,            1, L"150.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, source_pitch_tMid,               1, L"25.0%")   // relative time
		prefs_add_double_with_data (FormantGridEditor, source_pitch_f0Mid,              1, L"180.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, source_pitch_tEnd,               1, L"100.0%")   // relative time
		prefs_add_double_with_data (FormantGridEditor, source_pitch_f0End,              1, L"120.0")   // Hz
		prefs_add_double_with_data (FormantGridEditor, source_phonation_adaptFactor,    1, L"1.0")
		prefs_add_double_with_data (FormantGridEditor, source_phonation_maximumPeriod,  1, L"0.05")
		prefs_add_double_with_data (FormantGridEditor, source_phonation_openPhase,      1, L"0.7")
		prefs_add_double_with_data (FormantGridEditor, source_phonation_collisionPhase, 1, L"0.03")
		prefs_add_double_with_data (FormantGridEditor, source_phonation_power1,         1, L"3.0")
		prefs_add_double_with_data (FormantGridEditor, source_phonation_power2,         1, L"4.0")
prefs_end (FormantGridEditor)

/* End of file FormantGridEditor_prefs.h */
