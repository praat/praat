/* Index.cpp
 *
 * Copyright (C) 2005-2011, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20050724
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070102
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
*/

#include <time.h>
#include "Index.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "Index_def.h"
#include "oo_COPY.h"
#include "Index_def.h"
#include "oo_EQUAL.h"
#include "Index_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "Index_def.h"
#include "oo_WRITE_TEXT.h"
#include "Index_def.h"
#include "oo_WRITE_BINARY.h"
#include "Index_def.h"
#include "oo_READ_TEXT.h"
#include "Index_def.h"
#include "oo_READ_BINARY.h"
#include "Index_def.h"
#include "oo_DESCRIPTION.h"
#include "Index_def.h"


Thing_implement (Index, Daata, 0);

void structIndex :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of elements: ", our numberOfElements);
}

void Index_init (Index me, long numberOfElements) {
	if (numberOfElements < 1) {
		Melder_throw (U"Cannot create index without elements.");
	}
	my classes = Ordered_create ();
	my numberOfElements = numberOfElements;
	my classIndex = NUMvector<long> (1, numberOfElements);
}

autoIndex Index_extractPart (Index me, long from, long to) {
	try {
		if (from == 0) {
			from = 1;
		}
		if (to == 0) {
			to = my numberOfElements;
		}
		if (to < from || from < 1 || to > my numberOfElements) {
			Melder_throw (U"Range should be in interval [1,", my numberOfElements, U"].");
		}
		autoIndex thee = Data_copy (me);
		thy numberOfElements = to - from + 1;
		
		for (long i = 1; i <= thy numberOfElements; i ++) {
			thy classIndex [i] = my classIndex [from + i - 1];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

Thing_implement (StringsIndex, Index, 0);

autoStringsIndex StringsIndex_create (long numberOfElements) {
	try {
		autoStringsIndex me = Thing_new (StringsIndex);
		Index_init (me.peek(), numberOfElements);
		return me;
	} catch (MelderError) {
		Melder_throw (U"StringsIndex not created.");
	}
}

int StringsIndex_getClass (StringsIndex me, char32 *klasLabel) {
	for (long i = 1; i <= my classes->size; i ++) {
		SimpleString ss = (SimpleString) my classes->at [i];   // FIXME cast
		if (Melder_equ (ss -> string, klasLabel)) {
			return i;
		}
	}
	return 0;
}

long StringsIndex_countItems (StringsIndex me, int iclass) {
	long sum = 0;
	for (long i = 1; i <= my numberOfElements; i ++) {
		if (my classIndex [i] == iclass) {
			sum ++;
		}
	}
	return sum;
}

/* End of Index.cpp */
