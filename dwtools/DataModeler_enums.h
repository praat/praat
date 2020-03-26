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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

enums_begin (kDataModelerData, 1)
	enums_add (kDataModelerData, 1, Valid, U"Valid")
	enums_add (kDataModelerData, 2, Invalid, U"Invalid")
enums_end (kDataModelerData, 2, Invalid)

enums_begin (kDataModelerFunction, 2)
	enums_add (kDataModelerFunction, 1, Polynome, U"Polynome")
	enums_add (kDataModelerFunction, 2, Legendre, U"Legendre")
enums_end (kDataModelerFunction, 2, Legendre)

enums_begin (kDataModelerParameter, 1)
	enums_add (kDataModelerParameter, 1, Free, U"Free")
	enums_add (kDataModelerParameter, 2, Fixed, U"Fixed")
	enums_add (kDataModelerParameter, 3, Undefined, U"Undefined")
enums_end (kDataModelerParameter, 3, Undefined)

enums_begin (kDataModelerWeights, 1)
	enums_add (kDataModelerWeights, 1, EqualWeights, U"Equal weights")
	enums_add (kDataModelerWeights, 2, OneOverSigma, U"One over sigma")
	enums_add (kDataModelerWeights, 3, OneOverSqrtSigma, U"One over sqrt(sigma)")
	enums_add (kDataModelerWeights, 4, Relative, U"Relative")
enums_end (kDataModelerWeights, 4, Relative)

/* End of file DataModeler_enums.h */
