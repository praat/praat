#ifndef _SlopeSelector_h_
#define _SlopeSelector_h_
/* SlopeSelector.h
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
	The algorithm to find the kth (or median) slope in O (n log(n)) time is described in
		Matousek (1991): Randomized optimal algorithm for slope selection.
		Information processing letters 39: 183-187.
	Given points p[i] = (ax[i], ay[i]), i = 1..N, with ax[i] < ax[i+1] for i = 1..N-1.
	Find the line with the k-th largest slope of all the N(N-1)/2 lines connecting
	p[i] = (ax[i],ay[i]) with p[j] = (ax[j],ay[j]) where i = 1..N,and j = i+1..N.
	We consider this problem in the dual space where we have N lines line[l] where
		line[l] is y = ax[N-l+1] * x - ay[N-l+1].
	The ordering of the lines is such that when x = -inf, line [1] has the lowest y-value and
	line [N] the highest. Therefore, the correspondence between line number 'l' and point number 'i' is
        l = N-i+1, for i = 1..N.
	The x coordinate of the intersection of line[i] and line[j] is at x = (ay[N-i+1]-ay[N-j+1])/(ax[N-i+1]-ax[N-j+1]),
	which equals the slope of the line connecting p[i] and p[j].
	In the dual space (x,y) we then search for the intersection with the k-th largest x-coordinate.
	For x = -inf the y-values of line[1]..line[N] are y[1] > y[2] > .. y[N], and
	for x = +inf the y-values of line[1]..line[N] are in reverse order y[N] > y[N-1] > .. y[1].
	If we sort the N crossings of the lines at x = a according to line number this will be a permutation
	of the numbers 1..N. The number of inversions in this permutation has to be equal to the number of intersections
	to the left of x=a (an inversion occurs when i < j && perm[i] > perm[j]).
	The number of inversions in a permutation of size N can be counted in O(N log(N)).
 */

#include "PermutationInversionCounter.h"
#include "melder.h"
#include "NUM2.h"
#include "SlopeSelector_enums.h"
#include "ExtendedReal.h"

#include "SlopeSelector_def.h"

/*
    Preconditions:
        x.size == y.size && x.size > 0
        i < j && x[i] < x[j]
*/

autoSlopeSelector SlopeSelector_create (constVEC const& x, constVEC const& y);

autoSlopeSelector SlopeSelector_create (integer numberOfPoints);

void SlopeSelector_getSlopeAndIntercept (SlopeSelector me, double &slope, double &intercept, kSlopeSelector_method method);

/****************/

void timeSlopeSelection ();

#endif /* _SlopeSelector_h_ */
