/* DataModeler_enums.h
 *
 * Copyright (C) 2021-2022 David Weenink
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

enums_begin (kDataModelerFunction, 1)
	enums_add (kDataModelerFunction, 1, LINEAR, U"Linear terms")
	enums_add (kDataModelerFunction, 2, POLYNOME, U"Standard polynomials")
	enums_add (kDataModelerFunction, 3, LEGENDRE, U"Legendre polynomials")
	enums_add (kDataModelerFunction, 4, SIGMOID, U"Sigmoid")
	enums_add (kDataModelerFunction, 5, SIGMOID_PLUS_CONSTANT, U"Sigmoid plus constant")
	enums_add (kDataModelerFunction, 6, EXPONENTIAL, U"Exponential")
	enums_add (kDataModelerFunction, 7, EXPONENTIAL_PLUS_CONSTANT, U"Exponential plus constant")
enums_end (kDataModelerFunction, 7, LEGENDRE)

enums_begin (kDataModelerParameterStatus, 1)
	enums_add (kDataModelerParameterStatus, 1, FREE, U"Free")
	enums_add (kDataModelerParameterStatus, 2, FIXED_, U"Fixed")
	enums_add (kDataModelerParameterStatus, 3, FIXED_EXTRA, U"Fixed extra")
	enums_add (kDataModelerParameterStatus, 4, NOT_DEFINED, U"Undefined")
enums_end (kDataModelerParameterStatus, 4, NOT_DEFINED)

enums_begin (kDataModelerWeights, 1)
	enums_add (kDataModelerWeights, 1, EQUAL_WEIGHTS, U"Equal")
	enums_add (kDataModelerWeights, 2, ONE_OVER_SIGMA, U"One over sigma")
	enums_add (kDataModelerWeights, 3, ONE_OVER_SQRTSIGMA, U"One over sqrt(sigma)")
	enums_add (kDataModelerWeights, 4, RELATIVE_, U"Relative")
enums_end (kDataModelerWeights, 4, RELATIVE_)

enums_begin (kDataModelerShrinkage, 0)
	enums_add (kDataModelerShrinkage, 0, NONE, U"None")
	enums_add (kDataModelerShrinkage, 1, RIDGE_REGRESSION, U"Ridge regression")
	//enums_add (kDataModelerShrinkage, 2, LASSO, U"Lasso")
enums_end (kDataModelerShrinkage, 1, RIDGE_REGRESSION)

/* End of file DataModeler_enums.h */
