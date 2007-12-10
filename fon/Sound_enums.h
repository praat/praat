/* Sound_enums.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
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

/*
 * pb 2007/12/07
 */

enums_begin (kSound_windowShape, 0)
	enums_add (kSound_windowShape, 0, RECTANGULAR, L"rectangular")
	enums_add (kSound_windowShape, 1, TRIANGULAR, L"triangular")
	enums_add (kSound_windowShape, 2, PARABOLIC, L"parabolic")
	enums_add (kSound_windowShape, 3, HANNING, L"Hanning")
	enums_add (kSound_windowShape, 4, HAMMING, L"Hamming")
	enums_add (kSound_windowShape, 5, GAUSSIAN_1, L"Gaussian1")
	enums_add (kSound_windowShape, 6, GAUSSIAN_2, L"Gaussian2")
	enums_add (kSound_windowShape, 7, GAUSSIAN_3, L"Gaussian3")
	enums_add (kSound_windowShape, 8, GAUSSIAN_4, L"Gaussian4")
	enums_add (kSound_windowShape, 9, GAUSSIAN_5, L"Gaussian5")
	enums_add (kSound_windowShape, 10, KAISER_1, L"Kaiser1")
	enums_add (kSound_windowShape, 11, KAISER_2, L"Kaiser2")
enums_end (kSound_windowShape, 11, RECTANGULAR)

/* End of file Sound_enums.h */
