/* Sound_enums.h
 *
 * Copyright (C) 1992-2010,2015 Paul Boersma
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

enums_begin (kSound_windowShape, 0)
	enums_add (kSound_windowShape, 0, RECTANGULAR, U"rectangular")
	enums_add (kSound_windowShape, 1, TRIANGULAR, U"triangular")
	enums_add (kSound_windowShape, 2, PARABOLIC, U"parabolic")
	enums_add (kSound_windowShape, 3, HANNING, U"Hanning")
	enums_add (kSound_windowShape, 4, HAMMING, U"Hamming")
	enums_add (kSound_windowShape, 5, GAUSSIAN_1, U"Gaussian1")
	enums_add (kSound_windowShape, 6, GAUSSIAN_2, U"Gaussian2")
	enums_add (kSound_windowShape, 7, GAUSSIAN_3, U"Gaussian3")
	enums_add (kSound_windowShape, 8, GAUSSIAN_4, U"Gaussian4")
	enums_add (kSound_windowShape, 9, GAUSSIAN_5, U"Gaussian5")
	enums_add (kSound_windowShape, 10, KAISER_1, U"Kaiser1")
	enums_add (kSound_windowShape, 11, KAISER_2, U"Kaiser2")
enums_end (kSound_windowShape, 11, RECTANGULAR)

enums_begin (kSounds_convolve_scaling, 1)
	enums_add (kSounds_convolve_scaling, 1, INTEGRAL, U"integral")
	enums_add (kSounds_convolve_scaling, 2, SUM, U"sum")
	enums_add (kSounds_convolve_scaling, 3, NORMALIZE, U"normalize")
	enums_add (kSounds_convolve_scaling, 4, PEAK_099, U"peak 0.99")
enums_end (kSounds_convolve_scaling, 4, PEAK_099)

enums_begin (kSounds_convolve_signalOutsideTimeDomain, 1)
	enums_add (kSounds_convolve_signalOutsideTimeDomain, 1, ZERO, U"zero")
	enums_add (kSounds_convolve_signalOutsideTimeDomain, 2, SIMILAR, U"similar")
	//enums_add (kSounds_convolve_signalOutsideTimeDomain, 3, PERIODIC, U"periodic")
enums_end (kSounds_convolve_signalOutsideTimeDomain, 2, ZERO)

/* End of file Sound_enums.h */
