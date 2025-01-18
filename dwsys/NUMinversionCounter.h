#ifndef _NUMinversionCounter_h_
#define _NUMinversionCounter_h_
/* NUMinversionCounter.h
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

/*
    The algorithm to do the actual counting was found in (CLRS) Corman, Leiserson, Rivest & Stein: Introduction to algorithms,
        third edition, The MIT press, as the solution of problem 2.4.d.
    However, their solution contains three gross errors which I have corrected.
*/

/*
    Given a vector v with n elements, we define an inversion as i < j && v [i] > v [j].
    We use a tric to store the inversion between two elements (i,j) with only **one** number by using the following
    lower triangular (n-1) x (n-1) matrix, with i being the **column** index and j being the **row** index!

             1  2  3  4  5  ... n-1
          +--------------- ...--
    1(2)  |  1
    2(3)  |  2  3
    3(4)  |  4  5  6
    4(5)  |  7  8  9 10
    5(6)  | 11 12 13 14 15
    ...
    n-1(n)|                   n(n-1)/2

    E.g. the inversion between element 1 and element 2 is coded with the number 2,
        the inversion between element  2 and 5 is code with the number 8.
    From inversion (i,j) to the index:
        index = (j-1)*n + i
    From the code to the inversion (i,j)
        j = Melder_iroundUp (+0.5 + sqrt (0.25 + 2 * index));
        i = index - (j-2) * (j - 1) / 2;
 */

template <typename T>
class InversionCounter {

private:
    integer numberOfInversions;
    bool registerInversions;
    autovector<T> buffer;
    autovector<T> dataCopy;
    vector<T> v;

    void init (vector<T> const& data, bool preserveData, bool specifyInversions) {
        if (preserveData) {
            dataCopy = newvectorcopy<T> (constvectorview<T> (data));
            v = dataCopy.get();
        } else
            v = data;
        registerInversions = specifyInversions;
        buffer = newvectorraw<T> (v.size);
        if (registerInversions)
            inversions = raw_INTVEC (v.size * (v.size - 1) / 2);
        setCompareFunction (compare_);
    }

    integer mergeInversions (integer p, integer q, integer r) {
        const integer nl = q - p + 1;
        const integer nr = r - q;
        vector<T> vl = buffer.part (1, nl), vr = buffer.part (nl + 1, nl + nr);
        vl  <<=  v.part (p, q);
        vr  <<=  v.part (q + 1, r);
        integer i = 1, j = 1, k = p, numberOfInversions = 0;
        while (i <= nl && j <= nr) {
            if (compare (vl [i], vr [j]) > 0) { //! vl [i] > vr [j], CLRS : L[i] <= R[j]
                if (registerInversions) {
                    integer col = p + i - 1, row = q + j; // col < row
                    for (integer ii = 1; ii <= nl - i + 1; ii ++, col ++) {
                        inversions [++ numberOfInversions] = (row - 1) * v.size + col;
                    }
                } else
                   numberOfInversions += nl - i + 1;
                v [k] = vr [j ++]; //! CLRS: A[k] = L[i++];
            } else
                v [k] = vl [i ++]; //! CLRS: A[k] = R[j++]
            k ++;
        }
        while (i <= nl)
            v [k ++] = vl [i ++];
        while (j <= nr)
            v [k ++] = vr [j ++];
        return numberOfInversions;
    }

    integer countNumberOfInversions_ (integer p, integer r) {
        integer numberOfInversions = 0;
        if (p < r) {
            const integer q = (p + r) / 2;
            numberOfInversions += countNumberOfInversions_ (p, q);
            numberOfInversions += countNumberOfInversions_ (q + 1, r);
            numberOfInversions += mergeInversions (p, q, r);
        }
        return numberOfInversions;
    }

    static int compare_ (T& a, T& b) {
        return ( a > b ? 1 : (b > a ? -1 : 0) );
    }

public:

    autoINTVEC inversions;
    int (*compare) (T& a, T& b);

    InversionCounter (vector<T> const& data) {
        init (data, true, false);
    }

    InversionCounter (vector<T> const& data, bool preserveData) {
       init (data, preserveData, false);
    }
    InversionCounter (vector<T> const& data, bool preserveData, bool specifyInversions) {
        init (data, preserveData, specifyInversions);
    }

    ~InversionCounter () {
    }

    inline integer getNumberOfInversions () {
        Melder_assert (v.size > 0);
        return countNumberOfInversions_ (1_integer, v.size);
    }

    inline void setCompareFunction (int (*compareFunction) (T& a, T& b)) {
        compare = (compareFunction ? compareFunction : compare_);
    }

    inline void getInversionFromIndex (integer index, integer *i, integer *j) {
        *j = Melder_iroundUp (+0.5 + sqrt (0.25 + 2 * index));
        *i = index - (*j - 2) * (*j - 1) / 2;
    }

    inline integer inversionToIndex (integer i, integer j) {
        Melder_assert (i < j);
        return j * (j - 1) / 2 + i;
    }

};

#endif // _NUMinversionCounter_h_
