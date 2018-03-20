/* Sound_extensions_enums.h
 *
 * Copyright (C) 2018 David Weenink
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
	enums_add (kSoundDrawingDirection, 1, LeftToRight, U"left-to-right")
	enums_add (kSoundDrawingDirection, 2, RightToLeft, U"right-to-left")
	enums_add (kSoundDrawingDirection, 3, BottomToTop, U"bottom-to-top")
	enums_add (kSoundDrawingDirection, 4, TopToBottom, U"top-to-bottom")
enums_end (kSoundDrawingDirection, 4, LeftToRight)

enums_begin (kSoundNoiseReductionMethod, 1)
	enums_add (kSoundNoiseReductionMethod, 1, SpectralSubtraction, U"spectral-subtraction")
enums_end (kSoundNoiseReductionMethod, 1, SpectralSubtraction)

/* End of file Sound_extensions_enums.h */
