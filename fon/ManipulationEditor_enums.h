/* ManipulationEditor_enums.h
 *
 * Copyright (C) 1992-2005,2007,2013,2015,2020 Paul Boersma
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

enums_begin (kManipulationEditor_draggingStrategy, 1)
	enums_add (kManipulationEditor_draggingStrategy, 1, ALL, U"all")
	enums_add (kManipulationEditor_draggingStrategy, 2, HORIZONTAL, U"only horizontal")
	enums_add (kManipulationEditor_draggingStrategy, 3, VERTICAL, U"only vertical")
	enums_add (kManipulationEditor_draggingStrategy, 4, HYBRID, U"single all, multiple only vertical")
enums_end (kManipulationEditor_draggingStrategy, 4, ALL)

enums_begin (kManipulationEditor_pitchUnits, 1)
	enums_add (kManipulationEditor_pitchUnits, 1, HERTZ, U"Hertz")
	enums_add (kManipulationEditor_pitchUnits, 2, SEMITONES, U"semitones re 100 Hz")
enums_end (kManipulationEditor_pitchUnits, 2, HERTZ)

/* End of file ManipulationEditor_enums.h */
