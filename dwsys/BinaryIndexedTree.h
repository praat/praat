#ifndef _BinaryIndexedTree_h_
#define _BinaryIndexedTree_h_
/* BinaryIndexedTree.h
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

#include "melder.h"

class BinaryIndexedTree {
private:
	autoINTVEC bit;
public:

	BinaryIndexedTree () {	}

	BinaryIndexedTree (integer size) {
		bit = zero_INTVEC (size);
	}

	/*
		This elegant initialization in O(n) time I found at sdnr1's blog at
		https://codeforces.com/blog/entry/63064
	 */
	BinaryIndexedTree (constINTVEC const& v) {
		Melder_assert (v.size > 0);
		bit = zero_INTVEC (v.size);
		for (integer i = 1; i <= v.size; i ++) {
			bit [i] += v [i];
			const integer next = i + (i & -i);
			if (next <= v.size)
				bit [next] += bit [i];
		}
	}

	// update bit if we put a new 'value' at 'index'
	void update (integer index, integer value) {
		for (integer i = index; i <= bit.size; i += i & -i)
			bit [i] += value;
	}

	integer query (integer index) {
		integer sum = 0;
		for (integer i = index; i >= 1; i -= i & -i)
			sum += bit [i];
		return sum;
	}
};


#endif /* _BinaryIndexedTree_h_ */
