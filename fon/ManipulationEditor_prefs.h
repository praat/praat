/* ManipulationEditor_prefs.h
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

prefs_begin (ManipulationEditor)
	// new:
		prefs_add_double_with_data (ManipulationEditor, pitch_minimum,                                            1,  L"50.0")   // Hz
		prefs_add_double_with_data (ManipulationEditor, pitch_maximum,                                            1, L"300.0")   // Hz
		prefs_add_enum_with_data   (ManipulationEditor, pitch_units,            1, kManipulationEditor_pitchUnits,       DEFAULT)
		prefs_add_enum_with_data   (ManipulationEditor, pitch_draggingStrategy, 1, kManipulationEditor_draggingStrategy, DEFAULT)
		prefs_add_double_with_data (ManipulationEditor, pitch_stylize_frequencyResolution,                        1, L"2.0")
		prefs_add_bool_with_data   (ManipulationEditor, pitch_stylize_useSemitones,                               1, true)
		prefs_add_long_with_data   (ManipulationEditor, pitch_interpolateQuadratically_numberOfPointsPerParabola, 1, L"4")
		prefs_add_double_with_data (ManipulationEditor, duration_minimum,                                         1, L"0.25")
		prefs_add_double_with_data (ManipulationEditor, duration_maximum,                                         1, L"3.0")
prefs_end (ManipulationEditor)

/* End of file ManipulationEditor_prefs.h */
