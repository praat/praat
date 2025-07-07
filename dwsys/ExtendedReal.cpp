/* ExtendedReal.cpp
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

#include "ExtendedReal.h"
#include "melder.h"
#include "median_of_ninthers.h"


#include "oo_DESTROY.h"
#include "ExtendedReal_def.h"
#include "oo_COPY.h"
#include "ExtendedReal_def.h"
#include "oo_EQUAL.h"
#include "ExtendedReal_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ExtendedReal_def.h"
#include "oo_WRITE_TEXT.h"
#include "ExtendedReal_def.h"
#include "oo_READ_TEXT.h"
#include "ExtendedReal_def.h"
#include "oo_WRITE_BINARY.h"
#include "ExtendedReal_def.h"
#include "oo_READ_BINARY.h"
#include "ExtendedReal_def.h"
#include "oo_DESCRIPTION.h"
#include "ExtendedReal_def.h"

/*
autoVEC vector_to_VEC (constvectorview<structExtendedReal> const& v) {
	autoVEC vec = raw_VEC (v.size);
	for (integer i = 1; i <= v.size; i ++)
		vec [i] = v [i].real;
	return vec;
}


template <typename structT>
void NUMselect (vector<structT> const& v, integer kth_asIfSorted) {	
	adaptiveQuickselect (v.asArgumentToFunctionThatExpectsZeroBasedArray(), kth_asIfSorted - 1, v.size);
}

template <typename structT>
structT& NUMget_kth_inplace (vector<structT> const& v, integer kth_asIfSorted) {
	NUMselect (v, kth_asIfSorted);
	return v[kth_asIfSorted];
}

template <typename structT>
structT& NUMget_kth (constvectorview<structT> const& v, integer kth_asIfSorted) {
	autovector<structT> vcopy = newvectorcopy (v);
	return NUMget_kth_inplace (vcopy.get(), kth_asIfSorted);
}

template <typename structT>
void NUMselect (constvectorview<structT> const& v, integer kth_asIfSorted) {
	autovector<structT> vcopy = newvectorcopy (v);
	NUMselect (vcopy.get(), kth_asIfSorted);
}
*/

/* End of file ExtendedReal.cpp */
