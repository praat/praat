/* ManipulationEditor_prefs.h
 *
 * Copyright (C) 2013,2015-2017,2020 Paul Boersma
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

prefs_begin (ManipulationEditor)

	prefs_add_enum_with_data    (ManipulationEditor, pitch_draggingStrategy, 1, kManipulationEditor_draggingStrategy, DEFAULT)
	prefs_add_double_with_data  (ManipulationEditor, pitch_stylize_frequencyResolution,                        1, U"2.0")
	prefs_add_bool_with_data    (ManipulationEditor, pitch_stylize_useSemitones,                               1, true)
	prefs_add_integer_with_data (ManipulationEditor, pitch_interpolateQuadratically_numberOfPointsPerParabola, 1, U"4")

prefs_end (ManipulationEditor)

/* End of file ManipulationEditor_prefs.h */
