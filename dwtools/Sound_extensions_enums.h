/* Sound_extensions_enums.h
 *
 * Copyright (C) 2018-2022 David Weenink
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

enums_begin (kSoundDrawingDirection, 1)
	enums_add (kSoundDrawingDirection, 1, LEFT_TO_RIGHT, U"left-to-right")
	enums_add (kSoundDrawingDirection, 2, RIGHT_TO_LEFT, U"right-to-left")
	enums_add (kSoundDrawingDirection, 3, BOTTOM_TO_TOP, U"bottom-to-top")
	enums_add (kSoundDrawingDirection, 4, TOP_TO_BOTTOM, U"top-to-bottom")
enums_end (kSoundDrawingDirection, 4, LEFT_TO_RIGHT)

enums_begin (kSoundNoiseReductionMethod, 1)
	enums_add (kSoundNoiseReductionMethod, 1, SPECTRAL_SUBTRACTION, U"spectral-subtraction")
enums_end (kSoundNoiseReductionMethod, 1, SPECTRAL_SUBTRACTION)

enums_begin (kSoundToMP3Encoding, 1)
	enums_add (kSoundToMP3Encoding, 1, ABR, U"Average bit rate")
	enums_add (kSoundToMP3Encoding, 2, CBR, U"Constant bit rate")
	enums_add (kSoundToMP3Encoding, 3, VBR, U"Variable bit rate")
enums_end (kSoundToMP3Encoding, 3, 1)

/* End of file Sound_extensions_enums.h */
