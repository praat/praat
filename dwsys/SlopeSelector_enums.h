/* SlopeSelector_enums.h
 *
 * Copyright (C) 2025 David Weenink
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

enums_begin (kSlopeSelector_method, 1)
	enums_add (kSlopeSelector_method, 1, THEILSEN, U"Theil-Sen")
	enums_add (kSlopeSelector_method, 2, SIEGEL, U"Siegel")	
	enums_add (kSlopeSelector_method, 3, LEAST_SQUARES, U"Least squares")	
enums_end (kSlopeSelector_method, 1, THEILSEN)

/* End of file SlopeSelector_enums.h */
