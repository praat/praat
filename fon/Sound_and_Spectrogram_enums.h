/* Sound_and_Spectrogram_enums.h
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

enums_begin (kSound_to_Spectrogram_method, 1)
	enums_add (kSound_to_Spectrogram_method, 1, FOURIER, L"Fourier")
enums_end (kSound_to_Spectrogram_method, 1, FOURIER)

enums_begin (kSound_to_Spectrogram_windowShape, 0)
	enums_add (kSound_to_Spectrogram_windowShape, 0, SQUARE, L"square (rectangular)")
	enums_add (kSound_to_Spectrogram_windowShape, 1, HAMMING, L"Hamming (raised sine-squared)")
	enums_add (kSound_to_Spectrogram_windowShape, 2, BARTLETT, L"Bartlett (triangular)")
	enums_add (kSound_to_Spectrogram_windowShape, 3, WELCH, L"Welch (parabolic)")
	enums_add (kSound_to_Spectrogram_windowShape, 4, HANNING, L"Hanning (sine-squared)")
	enums_add (kSound_to_Spectrogram_windowShape, 5, GAUSSIAN, L"Gaussian")
enums_end (kSound_to_Spectrogram_windowShape, 5, GAUSSIAN)

/* End of Sound_and_Spectrogram_enums.h */
