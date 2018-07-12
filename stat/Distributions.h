#ifndef _Distributions_h_
#define _Distributions_h_
/* Distributions.h
 *
 * Copyright (C) 1997-2011,2014,2015,2017 Paul Boersma
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

/* Distributions inherits from TableOfReal */
#include "TableOfReal.h"

Thing_define (Distributions, TableOfReal) {
	void v_info ()
		override;
};

autoDistributions Distributions_create (integer numberOfRows, integer numberOfColumns);

void Distributions_peek (Distributions me, integer column, conststring32 *out_string, integer *out_row);

double Distributions_getProbability (Distributions me, conststring32 string, integer column);
double Distributionses_getMeanAbsoluteDifference (Distributions me, Distributions thee, integer column);

autoDistributions Distributions_addTwo (Distributions me, Distributions thee);
autoDistributions Distributions_addMany (OrderedOf<structDistributions>* me);

void Distributions_checkSpecifiedColumnNumberWithinRange (Distributions me, integer columnNumber);

/* End of file Distributions.h */
#endif
