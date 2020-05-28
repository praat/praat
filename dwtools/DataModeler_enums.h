/* DataModeler_enums.h
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

enums_begin (kDataModelerData, 1)
	enums_add (kDataModelerData, 1, VALID, U"Valid")
	enums_add (kDataModelerData, 2, INVALID, U"Invalid")
enums_end (kDataModelerData, 2, INVALID)

enums_begin (kDataModelerFunction, 2)
	enums_add (kDataModelerFunction, 1, POLYNOME, U"Standard polynomials")
	enums_add (kDataModelerFunction, 2, LEGENDRE, U"Legendre polynomials")
enums_end (kDataModelerFunction, 2, LEGENDRE)

enums_begin (kDataModelerParameter, 1)
	enums_add (kDataModelerParameter, 1, FREE, U"Free")
	enums_add (kDataModelerParameter, 2, FIXED_, U"Fixed")
	enums_add (kDataModelerParameter, 3, NOT_DEFINED, U"Undefined")
enums_end (kDataModelerParameter, 3, NOT_DEFINED)

enums_begin (kDataModelerWeights, 1)
	enums_add (kDataModelerWeights, 1, EQUAL_WEIGHTS, U"Equal")
	enums_add (kDataModelerWeights, 2, ONE_OVER_SIGMA, U"One over sigma")
	enums_add (kDataModelerWeights, 3, ONE_OVER_SQRTSIGMA, U"One over sqrt(sigma)")
	enums_add (kDataModelerWeights, 4, RELATIVE_, U"Relative")
enums_end (kDataModelerWeights, 4, RELATIVE_)

/* End of file DataModeler_enums.h */
