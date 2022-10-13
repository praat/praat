/* Index.cpp
 *
 * Copyright (C) 2005-2022 David Weenink
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
 djmw 20050724
 djmw 20061212 Changed info to Melder_writeLine<x> format.
 djmw 20070102
 djmw 20071012 Added: o_CAN_WRITE_AS_ENCODING.h
 djmw 20110304 Thing_new
*/

#include <time.h>
#include "Index.h"
#include "NUM2.h"
#include "Permutation.h"
#include "STRVEC_sorting.h"

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

static const conststring32 undefinedClassLabel = U"";
Thing_implement (Index, Daata, 0);

void structIndex :: v1_info () {
	structDaata :: v1_info ();
	MelderInfo_writeLine (U"Number of items: ", our numberOfItems);
}

void Index_init (Index me, integer numberOfItems) {
	Melder_require (numberOfItems > 0,
		U"The index should not be empty.");
	my classes = Ordered_create ();
	my numberOfItems = numberOfItems;
	my classIndex = zero_INTVEC (numberOfItems);
}

autoIndex Index_extractPart (Index me, integer from, integer to) {
	try {
		if (from == 0)
			from = 1;
		if (to == 0)
			to = my numberOfItems;
		Melder_require (from <= to && from > 0 && to <= my numberOfItems,
			U"Range should be in interval [1,", my numberOfItems, U"].");
		
		autoIndex thee = Data_copy (me);
		thy numberOfItems = to - from + 1;
		
		for (integer i = 1; i <= thy numberOfItems; i ++)
			thy classIndex [i] = my classIndex [from + i - 1];
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

Thing_implement (StringsIndex, Index, 0);

autoStringsIndex StringsIndex_create (integer numberOfItems) {
	try {
		autoStringsIndex me = Thing_new (StringsIndex);
		Index_init (me.get(), numberOfItems);
		return me;
	} catch (MelderError) {
		Melder_throw (U"StringsIndex not created.");
	}
}


void MelderString_appendPart (MelderString *me, conststring32 source, integer first, integer last) {
	const integer length = str32len (source);
	Melder_assert (first <= length);
	if (last == 0)
		last = length;
	last = std::min (last, length);
	for (integer ichar = first; ichar <= last; ichar ++)
			MelderString_appendCharacter(me, source [ichar - 1]); // 0-based
	MelderString_appendCharacter (me, U'\0');
}

void MelderString_copyPart (MelderString *me, conststring32 source, integer first, integer last) {
	MelderString_empty (me);
	MelderString_appendPart (me, source, first, last);
}

/* copying of strings could be avoided by using a autovector<char32** ? */
autoStringsIndex StringsIndex_createFromSTRVEC (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint) {
	try {
		struct structSTRVECIndexer indexer;
		autoStringsIndex me =  indexer.index (strvec, breakAtDecimalPoint, sorting);	
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create StringsIndex from STRVEC.");
	}
}

autoPermutation Permutation_createFromSorted (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint) {
	try {
		struct structSTRVECIndexer indexer;
		autoPermutation me =  indexer.sortSimple (strvec, breakAtDecimalPoint, sorting);	
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create Permutaion from STRVEC.");
	}	
}

autoStringsIndex Strings_to_StringsIndex (Strings me, kStrings_sorting sorting) {
	try {
		autoStringsIndex thee = StringsIndex_createFromSTRVEC (my strings.get(), sorting, false);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

integer Index_getClassIndexFromItemIndex (Index me, integer itemIndex) {
	integer result = 0;
	if (itemIndex >= 0 && itemIndex <= my numberOfItems)
		result = my classIndex [itemIndex];
	return result;
}

int StringsIndex_getClassIndexFromClassLabel (StringsIndex me, conststring32 klasLabel) {
	for (integer i = 1; i <= my classes->size; i ++) {
		const SimpleString ss = (SimpleString) my classes->at [i];   // FIXME cast
		if (Melder_equ (ss -> string.get(), klasLabel))
			return i;
	}
	return 0;
}

conststring32 StringsIndex_getClassLabelFromClassIndex (StringsIndex me, integer klasIndex) {
	conststring32 result = undefinedClassLabel;
	if (klasIndex > 0 && klasIndex <= my classes -> size) {
		const SimpleString ss = (SimpleString) my classes->at [klasIndex];   // FIXME cast
		result = ss -> string.get();
	}
	return result;
}

conststring32 StringsIndex_getItemLabelFromItemIndex (StringsIndex me, integer itemNumber) {
	conststring32 result = undefinedClassLabel;
	if (itemNumber > 0 && itemNumber <= my numberOfItems) {
		const integer klas = my classIndex [itemNumber];
		const SimpleString ss = (SimpleString) my classes->at [klas];   // FIXME cast
		result = ss -> string.get();
	}
	return result;
}

integer StringsIndex_countItems (StringsIndex me, integer iclass) {
	integer sum = 0;
	for (integer i = 1; i <= my numberOfItems; i ++) {
		if (my classIndex [i] == iclass)
			sum ++;
	}
	return sum;
}

void StringsIndex_sortNumerically (StringsIndex me) {
	try {
		const integer numberOfClasses = my classes -> size;
		autoSTRVEC numberstrings (numberOfClasses);
		autoVEC numbers = raw_VEC (numberOfClasses);
		for (integer i = 1; i <= numberOfClasses; i ++) {
			const SimpleString ss = (SimpleString) my classes->at [i];
			numberstrings [i] = Melder_dup (ss -> string.get());
			numbers [i] = Melder_atof (ss -> string.get());
		}
		autoPermutation p = Permutation_create (numberOfClasses, true);
		NUMsortTogether (numbers.get(), p -> p.get());
		for (integer i = 1; i <= numberOfClasses; i ++) {
			autoSimpleString ss = SimpleString_create (numberstrings [i].get());
			my classes-> replaceItem_move (ss.move(), p -> p [i]);
		}
		for (integer item = 1; item <= classIndex->size; item ++)
			my classIndex [item] = p -> p [my classIndex [item]];
	} catch (MelderError) {
		Melder_throw (me, U": could not be sorted numerically.");
	}
}


/* End of Index.cpp */
