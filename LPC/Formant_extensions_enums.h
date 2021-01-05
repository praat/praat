/* Formant_extensions_enums.h
 *
 * Copyright (C) 2021 David Weenink
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

enums_begin (kFormantSlopeUnit, 0)
	enums_add (kFormantSlopeUnit, 0, HERTZ_PER_SECOND, U"hertz/s")
	enums_add (kFormantSlopeUnit, 1, BARK_PER_SECOND, U"bark/s")
enums_end (kFormantSlopeUnit, 1, HERTZ_PER_SECOND)

enums_begin (kFormantSlopeMethod, 0)
	enums_add (kFormantSlopeMethod, 0, LINE_FIT, U"Straight line fit")
enums_end (kFormantSlopeMethod, 0, LINE_FIT)

/* End of file Formant_extensions_enums.h */
