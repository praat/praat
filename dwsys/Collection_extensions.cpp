/* Collection_extensions.cpp
 *
 * Copyright (C) 1994-2019 David Weenink, 2018 Paul Boersma
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
 djmw 20020812 GPL header
 djmw 20040420 Fraction in OrderedOfString_difference should be double.
 djmw 20050511 Skip printing unique labels in OrderedOfString
 djmw 20061214
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20110304 Thing_new
*/

#include "Collection_extensions.h"
#include "NUM2.h"


autoCollection Collection_Permutation_permuteItems (Collection me, Permutation him) {
	try {
		Melder_require (my size == his numberOfElements,
			U"The number of elements must be equal.");

		autoCollection thee = Collection_create ();
		thy classInfo = my classInfo;
		thy _initializeOwnership (my _ownItems);
		thy _grow (my size);
		for (integer pos = 1; pos <= my size; pos ++) {
			const integer oldPos = Permutation_getValueAtIndex (him, pos);
			if (thy _ownItems) {
				autoDaata data = Data_copy ((Daata) my at [oldPos]);
				thy _insertItem_move (data.move(), pos);
			} else
				thy _insertItem_ref (my at [oldPos], pos);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not permuted.");
	}
}

autoCollection Collection_permuteItems (Collection me) {
	try {
		autoPermutation p = Permutation_create (my size);
		Permutation_permuteRandomly_inplace (p.get(), 0, 0);
		autoCollection thee = Collection_Permutation_permuteItems (me, p.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": items not permuted.");
	}
}

/****************** class OrderedOfString ******************/

int OrderedOfString_append (StringList me, conststring32 append) {
	try {
		if (! append)
			return 1;    // BUG: lege string appenden??
		autoSimpleString item = SimpleString_create (append);
		my addItem_move (item.move());
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": text not appended.");
	}
}

autoStringList OrderedOfString_joinItems (StringList me, StringList thee) {
	try {
		Melder_require (my size == thy size,
			U"sizes should be equal.");
		
		autoStringList him = Data_copy (me);   // FIXME: this copies *all* the data from me, and only the strings from thee

		for (integer i = 1; i <= my size; i ++) {
			const SimpleString hisCategory = his at [i], thyCategory = thy at [i];
			const integer hisLength = str32len (hisCategory -> string.get()), thyLength = str32len (thyCategory -> string.get());
			hisCategory -> string. resize (hisLength + thyLength);
			str32cpy (& hisCategory -> string [hisLength], thyCategory -> string.get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Items not joined.");
	}
}

autoStringSet StringList_to_StringSet (StringList me) {
	try {
		autoStringSet you = StringSet_create ();
		for (integer i = 1; i <= my size; i ++) {
			autoSimpleString item = SimpleString_create (my at [i] -> string.get());
			your addItem_unsorted_move (item.move());
		}
		your sort ();
		your unicize ();
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to StringSet.");
	}
}

integer OrderedOfString_getNumberOfDifferences (StringList me, StringList thee) {
	Melder_require (my size == thy size,
		U"The two lists should have equal sizes.");
	integer numberOfDifferences = 0;
	for (integer i = 1; i <= my size; i ++) {
		const SimpleString mystring = my at [i];
		const SimpleString thystring = thy at [i];
		if (Melder_cmp (mystring->string.get(), thystring->string.get()) != 0)
			numberOfDifferences ++;
	}
	return numberOfDifferences;
}

double OrderedOfString_getFractionDifferent (StringList me, StringList thee) {
	integer numberOfDifferences = OrderedOfString_getNumberOfDifferences (me, thee);
	return my size == 0 ? 0.0 : (double) numberOfDifferences / my size;
}

integer OrderedOfString_indexOfItem_c (StringList me, conststring32 str) {
	integer index = 0;
	for (integer i = 1; i <= my size; i ++) {
		SimpleString mystring = my at [i];
		if (Melder_cmp (mystring->string.get(), str) == 0) {
			index = i;
			break;
		}
	}
	return index;
}

void OrderedOfString_initWithSequentialNumbers (StringList me, integer n) {
	for (integer i = 1; i <= n; i ++)
		my addItem_move (SimpleString_create (Melder_integer (i)));
}

void OrderedOfString_changeStrings (StringList me, char32 *search, char32 *replace, int maximumNumberOfReplaces, integer *out_numberOfMatches, integer *out_numberOfStringMatches, bool use_regexp) {
	regexp *compiled_search = nullptr;
	try {
		Melder_require (search,
			U"The search string should not be empty.");
		Melder_require (replace,
			U"The replace string should not be empty.");

		if (use_regexp)
			compiled_search = CompileRE_throwable (search, 0);
		integer numberOfMatches = 0, numberOfStringMatches = 0;
		for (integer i = 1; i <= my size; i ++) {
			SimpleString ss = my at [i];
			integer nmatches_sub;
			autostring32 r = use_regexp ?
					replace_regex_STR (ss -> string.get(), compiled_search, replace, maximumNumberOfReplaces, & nmatches_sub) :
					replace_STR (ss -> string.get(), search, replace, maximumNumberOfReplaces, & nmatches_sub);
			/*
				Change without error.
			*/
			ss -> string = r.move();

			if (nmatches_sub > 0) {
				numberOfMatches += nmatches_sub;
				numberOfStringMatches ++;
			}
		}
		if (use_regexp)
			free (compiled_search);
		if (out_numberOfMatches)
			*out_numberOfMatches= numberOfMatches;
		if (out_numberOfStringMatches)
			*out_numberOfStringMatches= numberOfStringMatches;
	} catch (MelderError) {
		if (use_regexp)
			free (compiled_search);
		Melder_throw (U"Replace not completed.");
	}
}

bool OrderedOfString_containSameElements (StringList me, StringList thee) {
	autoStringSet myset = StringList_to_StringSet (me);
	autoStringSet thyset = StringList_to_StringSet (thee);
	return Data_equal (myset.get(), thyset.get());
}

void OrderedOfString_removeOccurrences (StringList me, conststring32 search, bool use_regexp) {
	if (! search)
		return;
	for (integer i = my size; i >= 1; i --) {
		const SimpleString ss = my at [i];
		if ((use_regexp && strstr_regexp (ss -> string.get(), search)) ||
		        (! use_regexp && str32str (ss -> string.get(), search))) {
			my removeItem (i);
		}
	}
}

/* End of file Collection_extensions.cpp */
