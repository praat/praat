#ifndef _melder_sort_h_
#define _melder_sort_h_
/* melder_sort.h
 *
 * Copyright (C) 1992-2019 Paul Boersma
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

/********** Sorting **********/

double NUMquantile (integer n, double a [], double factor);
double NUMquantile (const constVECVU& a, double factor) noexcept;
/*
	An estimate of the quantile 'factor' (between 0 and 1) of the distribution
	from which the set 'a [1..n]' is a sorted array of random samples.
	For instance, if 'factor' is 0.5, this function returns an estimate of
	the median of the distribution underlying the sorted set a [].
	If your array has not been sorted, first sort it with VECsort ().
*/

/* End of file melder_sort.h */
#endif
