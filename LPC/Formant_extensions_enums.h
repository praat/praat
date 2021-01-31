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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kSlopeCurve, 1)
	enums_add (kSlopeCurve, 1, EXPONENTIAL_PLUS_CONSTANT, U"Exponential plus constant")
	enums_add (kSlopeCurve, 2, PARABOLIC, U"Parabolic")
	enums_add (kSlopeCurve, 3, SIGMOID_PLUS_CONSTANT, U"Sigmoid plus constant")
enums_end (kSlopeCurve, 3, PARABOLIC)


/* End of file Formant_extensions_enums.h */
