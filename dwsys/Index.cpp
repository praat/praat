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

static autoSTRVEC STRVEC_unicize (constSTRVEC const& strvec, constINTVEC const& permutation) {
	Melder_assert (strvec.size == permutation.size);
	integer count = 1;
	autoSTRVEC unique;
	unique.insert (count, strvec [permutation [1]]);
	for (integer i = 2; i <= strvec.size; i ++) {
		conststring32 str = strvec [permutation [i]];
		if (Melder_cmp (unique [count].get(), str) == 0)
			continue;
		count ++;
		unique.insert (count, str);
	}
	return unique;
}

/*
	Find the first and last index of the first numeric part in a string.
	The allowed numeric part is either <d> or <d>.<d>, where <d> a sequence of one or more digits
	Examples with breakAtDecimalPoint
				false 		true
	s1d 		-> 2,2		2,2
	2.222		-> 1,5		1,1
	abc123		-> 4,6		4,6
	a.3			-> 3,3		3,3
	a..3		-> 4,4		4,4
	a..3.		-> 4,4		4,4
	b33.33.4	-> 2,6		2,3
*/
static integer getIndexOfStartOfNumericPart (conststring32 string) {
	const char32 *p = string;
	while (*p != U'\0' && (*p < U'0' || *p > U'9'))
		p ++;
	const integer relPos = p - string;
	return relPos;
}

static integer getIndexOfEndOfNumericPart (conststring32 string, integer startIndex, bool breakAtDecimalPoint) {
	if (startIndex < 0)
		return 0;
	const char32 *pstart = string + startIndex - 1;
	const char32 *decimalPoint = 0, *p = pstart;
	while (*p != U'\0') {
		if (*p == U'.') {
			if (decimalPoint || breakAtDecimalPoint)
				break;
			else
				decimalPoint = p;
		} else if (*p < U'0' || *p > U'9')
			if (p - decimalPoint == 1) {
				p --; // digit string should not end with a point!
				break;
			}
		p ++;
	}
	const integer relPos = p - pstart;
	return startIndex + relPos;
}

void MelderString_copyPart (MelderString *me, conststring32 source, integer first, integer last) {
	MelderString_empty (me);
	const integer length = str32len (source);
	Melder_assert (first <= length);
	if (last == 0)
		last = length;
	last = std::min (last, length);
	for (integer ichar = first; ichar <= last; ichar ++)
			MelderString_appendCharacter(me, source [ichar - 1]); // 0-based
	MelderString_appendCharacter (me, U'\0');
}

static autoINTMAT indexRangeOfNumericParts_INTMAT (constSTRVEC const & strvec, bool breakAtDecimalPoint) {
	autoINTMAT result = zero_INTMAT (strvec.size, 2);
	for (integer istring = 1; istring <= strvec.size; istring ++) {
		const integer first = getIndexOfStartOfNumericPart (strvec [istring]);
		if (first == 0)
			continue;
		const integer last = getIndexOfEndOfNumericPart (strvec [istring], first, breakAtDecimalPoint);
		Melder_assert (last >= first);
		result [istring] [1] = first;
		result [istring] [2] = last;
	}
	return result;
}

autoSTRVEC STRVEC_copyParts (constSTRVEC const & strvec, constINTMAT const& indicesOfParts) {
	Melder_assert (strvec.size == indicesOfParts.nrow);
	Melder_assert (indicesOfParts.ncol >= 2);
	autoSTRVEC result (strvec.size);
	autoMelderString part;
	for (integer istring = 1; istring <= strvec.size; istring ++) {
		const integer first = indicesOfParts [istring] [1];
		if (first > 0)
			MelderString_copyPart (& part, strvec [istring], first, indicesOfParts [istring] [2]);
		result [istring] = Melder_dup (part.string);
	}
	return result;
}

autoVEC VEC_from_STRVEC (constSTRVEC const & strvec) {
	autoVEC result = raw_VEC (strvec.size);
	for (integer i = 1; i <= strvec.size; i ++) {
		result [i] = Melder_atof (strvec [i]);
	}
	return result;
}

static autoVEC partsAsNumber_VEC (constSTRVEC const & strvec, constINTMAT const& partIndices) {
	Melder_assert (strvec.size == partIndices.nrow);
	autoVEC numbers = raw_VEC (strvec.size);
	autoMelderString numericPart;
	for (integer istring = 1; istring <= strvec.size; istring ++) {
		double number = std::numeric_limits<double>::max();
		const integer first = partIndices [istring] [1];
		if (first > 0) {
			const integer last = partIndices [istring] [2];
			MelderString_copyPart (& numericPart, strvec [istring], first, last);
			number = Melder_atof (numericPart.string);
		};
		numbers [istring] = number;
	}
	return numbers;
}

autoStringsIndex StringsIndex_createFrom_STRVEC (constSTRVEC const& strvec, kStrings_sorting sorting, bool breakAtDecimalPoint) {
	try {
		autoStringsIndex me = StringsIndex_create (strvec.size);
		autoPermutation p = Permutation_create (strvec.size, true);
		autoSTRVEC classes;
		if (sorting == kStrings_sorting::ALPHABETICAL) {
			INTVECindex (p -> p.get(), strvec, false); // determine sorting
			classes = STRVEC_unicize (strvec, p -> p.get());
//		else if (sorting == kStrings_sorting::NUMERICAL)
//			INTVECindex (p -> p.get(), strvec, true);
		} else if (sorting == kStrings_sorting::NUMERICAL_PART) {
			autoINTMAT partIndices = indexRangeOfNumericParts_INTMAT (strvec, breakAtDecimalPoint);
			autoVEC numbers = partsAsNumber_VEC (strvec, partIndices.get());
			INTVECindex (p -> p.get(), numbers.get()); // determine sorting
			classes = STRVEC_unicize (strvec, p -> p.get());
			autoPermutation pc = Permutation_create (classes.size, true);
			integer start = 1, end = start;
			while (end <= classes.size && (getIndexOfStartOfNumericPart (classes [end].get()) == 1))
				end ++;
			autoINTMAT partIndicesClass = indexRangeOfNumericParts_INTMAT (strvec, breakAtDecimalPoint);
			if (-- end > 1) {
				// We are sure that the first part is numeric, sort the trailing alpha part
				const integer numberOfItems = end - start + 1;
				MelderString part;
				autoSTRVEC trailing (numberOfItems);
				autoPermutation pt = Permutation_create (numberOfItems, true);
				for (integer i = 1; i <= numberOfItems; i ++) {
					const integer last = partIndicesClass [i] [2];
					MelderString_copyPart (& part, classes [i].get(), last, 0_integer);
				}
				INTVECindex (pt -> p.get(), classes.get(), false);
				
			}
			if (end < classes.size) {
				integer start = end;
			}
		}
		
		for (integer i = 1; i <= classes.size; i ++) {
			autoSimpleString ss = SimpleString_create (classes [i].get());
			my classes -> addItem_move (ss.move());
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Could not create StringsIndex from STRVEC.");
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
