/* ManipulationEditor_enums.h
 *
 * Copyright (C) 1992-2007,2013 Paul Boersma
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

enums_begin (kManipulationEditor_draggingStrategy, 1)
	enums_add (kManipulationEditor_draggingStrategy, 1, ALL, L"all")
	enums_add (kManipulationEditor_draggingStrategy, 2, HORIZONTAL, L"only horizontal")
	enums_add (kManipulationEditor_draggingStrategy, 3, VERTICAL, L"only vertical")
	enums_add (kManipulationEditor_draggingStrategy, 4, HYBRID, L"single all, multiple only vertical")
enums_end (kManipulationEditor_draggingStrategy, 4, ALL)

enums_begin (kManipulationEditor_pitchUnits, 1)
	enums_add (kManipulationEditor_pitchUnits, 1, HERTZ, L"Hertz")
	enums_add (kManipulationEditor_pitchUnits, 2, SEMITONES, L"semitones re 100 Hz")
enums_end (kManipulationEditor_pitchUnits, 2, HERTZ)

/* End of file ManipulationEditor.h */
