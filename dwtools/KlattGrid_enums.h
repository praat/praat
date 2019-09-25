/* KlattGrid_enums.h
 *
 * Copyright (C) 2019 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kKlattGridFormantType, 1)
	enums_add (kKlattGridFormantType, 1, Oral, U"Oral formant")
	enums_add (kKlattGridFormantType, 2, Nasal, U"Nasal formant")
	enums_add (kKlattGridFormantType, 3, Frication, U"Frication Formant")
	enums_add (kKlattGridFormantType, 4, Tracheal, U"Tracheal formant")
	enums_add (kKlattGridFormantType, 5, NasalAnti, U"Nasal Antiformant")
	enums_add (kKlattGridFormantType, 6, TrachealAnti, U"Tracheal antiformant")
	enums_add (kKlattGridFormantType, 7, Delta, U"Delta formant")
enums_end (kKlattGridFormantType, 7, Oral)

enums_begin (kKlattGridFilterModel, 1)
	enums_add (kKlattGridFilterModel, 1, Cascade, U"Cascade")
	enums_add (kKlattGridFilterModel, 2, Parallel, U"Parallel")
enums_end (kKlattGridFilterModel, 2, Cascade)

/* End of file KlattGrid_enums.h */
