/* Pitch_enums.h
 *
 * Copyright (C) 1992-2007,2013,2015,2016,2018 Paul Boersma
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

enums_begin (kPitch_unit, 0)
	enums_add (kPitch_unit, 0, HERTZ, U"Hertz")
	enums_add (kPitch_unit, 1, HERTZ_LOGARITHMIC, U"Hertz (logarithmic)")
	enums_add (kPitch_unit, 2, MEL, U"mel")
	enums_add (kPitch_unit, 3, LOG_HERTZ, U"logHertz")
	enums_add (kPitch_unit, 4, SEMITONES_1, U"semitones re 1 Hz")
	enums_add (kPitch_unit, 5, SEMITONES_100, U"semitones re 100 Hz")
	enums_add (kPitch_unit, 6, SEMITONES_200, U"semitones re 200 Hz")
	enums_add (kPitch_unit, 7, SEMITONES_440, U"semitones re 440 Hz")
	enums_add (kPitch_unit, 8, ERB, U"ERB")
enums_end (kPitch_unit, 8, HERTZ)

/* End of file Pitch_enums.h */
