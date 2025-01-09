#ifndef _NUMpermutations_h_
#define _NUMpermutations_h_
/* NUMpermutations.h
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

#include "NUM.h"

template <typename T>
integer mergeInversions (vector <T> v, integer p, integer q, integer r, vector<T> buffer) {
    const integer nl = q - p + 1;
    const integer nr = r - q;
    vector<T> vl = buffer.part (1, nl), vr = buffer.part (nl + 1, nl + nr);
    vl  <<=  v.part (p, q);
    vr  <<=  v.part (q + 1, r);
    integer i = 1, j = 1, k = p, numberOfInversions = 0;
    while (i <= nl && j <= nr) {
        if (vl [i] > vr [j]) {
            numberOfInversions += nl - i + 1;
            v [k] = vr [j];
            j ++;
        } else {
            v [k] = vl [i];
            i ++;
        }
        k ++;
    }
    while (i <= nl) {
        v [k] = vl [i];
        k ++; i ++;
    }
    while (j <= nr) {
        v [k] = vr [j];
        k ++; j ++;
    }
    return numberOfInversions;
}

/*
    Count the number of pairs where if i < j then v [j] > v[i]
    vl (1, nl) & vr (1, nr) are buffer vectors of sizes
        nl = (1+v.size)/2, nr = v.size - nl;
*/

template <typename T>
integer countNumberOfInversions (vector<T> v, integer p, integer r, vector<T> buffer) {
    integer numberOfInversions = 0;
    if (p < r) {
        const integer q = (p + r) / 2;
        numberOfInversions += countNumberOfInversions (v, p, q, buffer);
        numberOfInversions += countNumberOfInversions (v, q + 1, r, buffer);
        numberOfInversions += mergeInversions (v, p, q, r, buffer);
    }
    return numberOfInversions;
}

#endif // _NUMpermutations_h_
