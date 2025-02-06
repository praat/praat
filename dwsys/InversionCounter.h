#ifndef _InversionCounter_h_
#define _InversionCounter_h_
/* InversionCounter.h
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

#include "NUM2.h"

/*
    The algorithm to do the counting only was found in (CLRS) Corman, Leiserson, Rivest & Stein: Introduction to algorithms,
        third edition, The MIT press, as the solution of problem 2.4.d.
    However, on 15 January 2025, their solution on the web-site of the book had the counting variable in the wrong position
*/

/*
    Given a vector v with n elements, we define an inversion as i < j && v [i] > v [j].
    We use a tric to store the inversion between two elements (i,j) with only **one** number code by using the following
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

    E.g. the inversion between element 1 and element 2 is coded with the number 1,
        the inversion between element 2 and 5 is code with the number 8.
    From inversion (i<j) to the index:
        index = (j-2)*(j-1)/2 + i
    From the code to the inversion (i,j)
        j = Melder_iroundUp (0.5 + sqrt (0.25 + 2 * index));
        i = index - (j-2) * (j - 1) / 2;
 */

template <typename T>
struct structIndexedNumber {
    T number;
    integer index;
};

template<typename T> using IndexedNumber = structIndexedNumber<T>;
typedef IndexedNumber<integer> IndexedInteger;
typedef IndexedNumber<double> IndexedDouble;

template <typename T, typename VT>
class InversionCounter {

private:

    autovector<VT> buffer;      // working space for the merge-sort
    autovector<VT> dataCopy;
    vector<VT> v;               // link to the data that are processed
    INTVEC inversions;
    INTVEC sortedSelectedInversionIndices;
    INTVEC otherPos;
    integer totalNumberOfInversions = 0;
    integer totalNumberOfInversionsInInterval = 0;
    integer numberOfInversionsRegistered = 0;
    integer numberOfInversionsToRegister = 0;
    integer posInSortedSelectedInversionIndices = 1;
    bool workWithCopyOfData = false;

    bool (*compare) (VT& a, VT& b);

    bool compareIndexedNumbers (IndexedNumber<T>& a, IndexedNumber<T>& b) {
        return a.number <= b.number;
    }

    bool compareIndexedNumbers (T& a, T& b) {
        return a < b;
    }

    bool compareNumbers (T& a, T& b) {
        return a < b;
    }

    void init (vector<VT> const& data) {
        if (workWithCopyOfData) {
            if (dataCopy._capacity > 0) { // no need to copy data that will be overwritten
                dataCopy.resize (data.size);
                for (integer i = 1; i <= data.size; i ++) // operator '<<=' only works for simple types!
                    dataCopy [i] =  data [i];
            } else
                dataCopy = newvectorcopy<VT> (constvectorview<VT> (data));
            v = dataCopy.get();
        } else
            v = data;

        buffer.resize (v.size);
        totalNumberOfInversions = 0;
        totalNumberOfInversionsInInterval = 0;
        numberOfInversionsRegistered = 0;
        numberOfInversionsToRegister = 0;
        posInSortedSelectedInversionIndices = 1;
        otherPos = {};
    }

    integer mergeInversions (integer p, integer q, integer r) {
        const integer nl = q - p + 1;
        const integer nr = r - q;
        vector<VT> vl = buffer.part (1, nl), vr = buffer.part (nl + 1, nl + nr);
        for (integer ii = 1; ii <= nl; ii ++)
            vl [ii] = v [p + ii - 1];
        for (integer ii = 1; ii <= nr; ii ++)
            vr [ii] = v [q + 1 + ii - 1];
        integer i = 1, j = 1, k = p, localNumberOfInversions = 0;
        while (i <= nl && j <= nr) {
            if (compareIndexedNumbers (vl [i], vr [j])) {
                v [k ++] = vl [i ++];
            } else { // vl[i] > vr[j]
                localNumberOfInversions += nl - i + 1;
                if (numberOfInversionsToRegister > 0) {
                    for (integer ii = i; ii <= nl; ii ++) {
                        totalNumberOfInversions ++;
                        if (otherPos.size > 0 && otherPos [vl [ii].number] < otherPos [vr [j].number]) // if also inversion in other: skip
                            continue;
                        /*
                            The inversions that passed are in the 'interval'
                        */
                        totalNumberOfInversionsInInterval ++;
                        const integer index = inversionToIndex (vr [j].number, vl [ii].number);
                        while (totalNumberOfInversionsInInterval == sortedSelectedInversionIndices [posInSortedSelectedInversionIndices]) {
                            inversions [++ numberOfInversionsRegistered] = index; //
                            if (-- numberOfInversionsToRegister == 0)
                                break;
                            posInSortedSelectedInversionIndices ++;
                        }
                    }
                }
                v [k ++] = vr [j ++]; //! CLRS: A[k] = L[i++];
            }
        }
        while (i <= nl)
            v [k ++] = vl [i ++];
        while (j <= nr)
            v [k ++] = vr [j ++];
        return localNumberOfInversions;
    }

    integer countInversionsBySorting_ (integer p, integer r) {
        integer numberOfInversions = 0;
        if (p < r) {
            const integer q = (p + r) / 2;
            numberOfInversions += countInversionsBySorting_ (p, q);
            numberOfInversions += countInversionsBySorting_ (q + 1, r);
            numberOfInversions += mergeInversions (p, q, r);
        }
        return numberOfInversions;
    }

public:

    InversionCounter () {
    }

    InversionCounter (bool workWithCopyOfData) {
        our workWithCopyOfData = workWithCopyOfData;
    }

    void workWithCopy (bool workWithCopyOfData) {
        our workWithCopyOfData = workWithCopyOfData;
    }
    /*
        If we need to know the memory beforehand, we allocate it while creating the object.
    */
    InversionCounter (bool workWithCopyOfData, integer dataSize) {
        our workWithCopyOfData = workWithCopyOfData;
        buffer = newvectorraw<VT> (dataSize);
        if (workWithCopyOfData) {
            dataCopy = newvectorraw<VT> (dataSize);
            v = dataCopy.get();
        }
    }

    inline integer getNumberOfInversionsRegistered () {
        return numberOfInversionsRegistered;
    }

    integer getNumberOfInversionsbySorting (vector<VT> const& data) {
        init (data);
        return countInversionsBySorting_ (1_integer, v.size);
    }

    integer getNumberOfInversionsbySorting (vector<VT> const& data, INTVEC const& inversions) {
        Melder_assert (inversions.size > 0);
        init (data);
        our inversions = inversions;
        return countInversionsBySorting_ (1_integer, v.size);
    }

    integer getSelectedInversionsbySorting (vector<VT> const& data, INTVEC const& sortedSelectedInversionIndices, INTVEC const& out_inversions) {
        Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
        init (data);
        our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
        numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
        our inversions = out_inversions;
        const integer numberOfInversions = countInversionsBySorting_ (1_integer, v.size);
        return numberOfInversions;
    }

    integer getSelectedInversionsNotInOtherbySorting (vector<VT> const& data, INTVEC const& sortedSelectedInversionIndices, INTVEC const& otherPos, INTVEC const& out_inversions) {
        Melder_assert (out_inversions.size == sortedSelectedInversionIndices.size);
        Melder_assert (otherPos.size == data.size);
        init (data);
        our sortedSelectedInversionIndices = sortedSelectedInversionIndices;
        our otherPos = otherPos;
        numberOfInversionsToRegister = sortedSelectedInversionIndices.size;
        our inversions = out_inversions;
        const integer numberOfInversions = countInversionsBySorting_ (1_integer, v.size);
        return numberOfInversions;
    }

    /*
        The following two methods first make a copy of the data
    */
    integer getNumberOfInversions (vector<VT> const& data, INTVEC const& inversions) {
        Melder_assert (inversions.size > 0);
        init (data);
        our inversions = inversions;
        const integer numberOfInversions = countInversionsBySorting_ (1_integer, v.size);
        return numberOfInversions;
    }

    inline void getInversionFromIndex (integer index, integer *ilow, integer *ihigh) {
        *ihigh = Melder_iroundUp (+0.5 + sqrt (0.25 + 2 * index));
        *ilow = index - (*ihigh - 2) * (*ihigh - 1) / 2;
        Melder_assert (*ilow < *ihigh);
    }

    inline integer inversionToIndex (integer ilow, integer ihigh) {
        Melder_assert (ilow < ihigh);
        return (ihigh - 2) * (ihigh - 1) / 2 + ilow;
    }

    autovector<VT> convertToIndexedNumberVector (constvector<T> const& v) {
        autovector<VT> result = newvectorraw<VT> (v.size);
        convertToIndexedNumberVector_inplace (v, result.get());
        return result;
    }

    void convertToIndexedNumberVector_inplace (constvector<T> const& v, vector<VT> const& result) {
        Melder_assert (v.size == result.size);
        for (integer i = 1; i <= v.size; i ++) {
            result [i].number = v [i];
            result [i].index = i;
        }
    }
};

#endif // _InversionCounter_h_
