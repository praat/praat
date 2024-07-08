/* WorkvectorPool.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "WorkvectorPool.h"

#include "oo_DESTROY.h"
#include "WorkvectorPool_def.h"
#include "oo_COPY.h"
#include "WorkvectorPool_def.h"
#include "oo_EQUAL.h"
#include "WorkvectorPool_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "WorkvectorPool_def.h"
#include "oo_READ_TEXT.h"
#include "WorkvectorPool_def.h"
#include "oo_WRITE_TEXT.h"
#include "WorkvectorPool_def.h"
#include "oo_READ_BINARY.h"
#include "WorkvectorPool_def.h"
#include "oo_WRITE_BINARY.h"
#include "WorkvectorPool_def.h"
#include "oo_DESCRIPTION.h"
#include "WorkvectorPool_def.h"

Thing_implement (WorkvectorPool, Daata, 0);

autoWorkvectorPool WorkvectorPool_create (constINTVEC const& vectorSizes, bool reusable) {
	try {
		autoWorkvectorPool me = Thing_new (WorkvectorPool);
		my numberOfVectors = vectorSizes.size;
		Melder_assert (my numberOfVectors > 0);
		my reusable = reusable;

		my vectorStart = raw_INTVEC (my numberOfVectors);
		integer vecstart = 1;
		for (integer ivec = 1; ivec <= my numberOfVectors; ivec ++) {
			Melder_assert (vectorSizes [ivec] > 0);
			my vectorStart [ivec] = vecstart;
			vecstart += vectorSizes [ivec];
		}
		my poolMemorySize = vecstart - 1;
		my vectorSizes = copy_INTVEC (vectorSizes);
		my inuse = zero_BOOLVEC (my numberOfVectors);
		my memoryPool = raw_VEC (my poolMemorySize);
		return me;
	} catch (MelderError) {
		Melder_throw (U"WorkvectorPool not created.");
	}	
}

 /* End of file WorkvectorPool.cpp */
