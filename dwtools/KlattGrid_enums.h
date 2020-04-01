/* KlattGrid_enums.h
 *
 * Copyright (C) 2019-2020 David Weenink
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
	enums_add (kKlattGridFormantType, 1, ORAL, U"Oral formant")
	enums_add (kKlattGridFormantType, 2, NASAL, U"Nasal formant")
	enums_add (kKlattGridFormantType, 3, FRICATION, U"Frication Formant")
	enums_add (kKlattGridFormantType, 4, TRACHEAL, U"Tracheal formant")
	enums_add (kKlattGridFormantType, 5, NASAL_ANTI, U"Nasal Antiformant")
	enums_add (kKlattGridFormantType, 6, TRACHEALANTI, U"Tracheal antiformant")
	enums_add (kKlattGridFormantType, 7, DELTA, U"Delta formant")
enums_end (kKlattGridFormantType, 7, ORAL)

enums_begin (kKlattGridFilterModel, 1)
	enums_add (kKlattGridFilterModel, 1, CASCADE, U"Cascade")
	enums_add (kKlattGridFilterModel, 2, PARALLEL, U"Parallel")
enums_end (kKlattGridFilterModel, 2, CASCADE)

/* End of file KlattGrid_enums.h */
