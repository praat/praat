#ifndef _LPC_enums_h_
#define _LPC_enums_h_

/* LPC_enums.h
 *
 * Copyright (C) 2020 David Weenink
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

enums_begin (kLPC_Analysis, 1)
	enums_add (kLPC_Analysis, 1, AUTOCORRELATION, U"Autocorrelation")
	enums_add (kLPC_Analysis, 2, COVARIANCE, U"Covariance")
	enums_add (kLPC_Analysis, 3, BURG, U"Burg")
	enums_add (kLPC_Analysis, 4, MARPLE, U"Marple")
	enums_add (kLPC_Analysis, 5, ROBUST, U"Robust")
enums_end (kLPC_Analysis, 5, BURG)

#endif /* _LPC_enums_h_ */
