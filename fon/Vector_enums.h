/* Vector_enums.h
 *
 * Copyright (C) 2020 Paul Boersma
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

enums_begin (kVector_valueInterpolation, 0)
	enums_add (kVector_valueInterpolation, 0, NEAREST, U"nearest")
	enums_add (kVector_valueInterpolation, 1, LINEAR, U"linear")
	enums_add (kVector_valueInterpolation, 2, CUBIC, U"cubic")
	enums_add (kVector_valueInterpolation, 3, SINC70, U"sinc70")
	enums_add (kVector_valueInterpolation, 4, SINC700, U"sinc700")
enums_end (kVector_valueInterpolation, 4, LINEAR)

enums_begin (kVector_peakInterpolation, 0)
	enums_add (kVector_peakInterpolation, 0, NONE, U"none")
	enums_add (kVector_peakInterpolation, 1, PARABOLIC, U"parabolic")
	enums_add (kVector_peakInterpolation, 2, CUBIC, U"cubic")
	enums_add (kVector_peakInterpolation, 3, SINC70, U"sinc70")
	enums_add (kVector_peakInterpolation, 4, SINC700, U"sinc700")
enums_end (kVector_peakInterpolation, 4, PARABOLIC)

/* End of file Vector_enums.h */
