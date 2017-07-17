/* Sound_and_Spectrogram_enums.h
 *
 * Copyright (C) 1992-2007,2013,2015 Paul Boersma
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

enums_begin (kSound_to_Spectrogram_method, 1)
	enums_add (kSound_to_Spectrogram_method, 1, FOURIER, U"Fourier")
enums_end (kSound_to_Spectrogram_method, 1, FOURIER)

enums_begin (kSound_to_Spectrogram_windowShape, 0)
	enums_add (kSound_to_Spectrogram_windowShape, 0, SQUARE, U"square (rectangular)")
	enums_add (kSound_to_Spectrogram_windowShape, 1, HAMMING, U"Hamming (raised sine-squared)")
	enums_add (kSound_to_Spectrogram_windowShape, 2, BARTLETT, U"Bartlett (triangular)")
	enums_add (kSound_to_Spectrogram_windowShape, 3, WELCH, U"Welch (parabolic)")
	enums_add (kSound_to_Spectrogram_windowShape, 4, HANNING, U"Hanning (sine-squared)")
	enums_add (kSound_to_Spectrogram_windowShape, 5, GAUSSIAN, U"Gaussian")
enums_end (kSound_to_Spectrogram_windowShape, 5, GAUSSIAN)

/* End of Sound_and_Spectrogram_enums.h */
