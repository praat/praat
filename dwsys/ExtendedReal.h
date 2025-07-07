#ifndef _ExtendedReal_h_
#define _ExtendedReal_h_
/* ExtendedReal.h
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
	We extend the set of real numbers into a non-Archimedian ordered field by adding infinitesimals.
	A positive infinitesimal number eps is smaller than any positive real number.
	The predicate a < b, where a and b are reals, is affected by adding infinitesimals in the following way:
	The test might become a + eps**2**i) < b + eps**2**j (a**b**c is shorthand notation for power (a, power (b,c))). 
	Assume without loss of generality that 0 < i <j.
	If a != b, then if a < b is true, then also a + eps**i < b + eps**j. Adding the infinitesimal didn't
	change the result. However, if a == b than the infinitisimals breaks the tie.
	So adding infinitesimals to a and b we need to change the predicate from (a < b) to (if a!=b ? a < b : i > j).
	
	Our simple implementation is the ExtendedReal with consists of the struct {double real; integer extension;}
	'extension' represent the infinitesimal power (eps, power (2,extension)).
	
	An ExtendedReal can be copied but should not be saved in a file.
*/

#include "Data.h"
#include "melder.h"

#include "ExtendedReal_def.h"

template <typename structT>
autoVEC vector_to_VEC (vector<structT> v);

/*
	Put the kth smallest element at position k
*/
template <typename structT>
void NUMselect (vector<structT> const& v, integer kth_asIfSorted);

structExtendedReal& NUMget_kth_inplace (vector<structExtendedReal> const& v, integer kth_asIfSorted);
structExtendedReal& NUMget_kth (constvectorview<structExtendedReal> const& v, integer kth_asIfSorted);

structExtendedReal& NUMquantile (constvectorview<structExtendedReal> const& v, double factor);
structExtendedReal& NUMquantile_inplace (vector<structExtendedReal> const& v, double factor);

#endif /* _ExtendedReal_h_ */
