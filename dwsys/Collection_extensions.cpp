/* Collection_extensions.cpp
 *
 * Copyright (C) 1994-2018 David Weenink, 2018 Paul Boersma
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
		if (my size != his numberOfElements) {
			Melder_throw (me, U"The number of elements are not equal.");
		}
		autoINTVEC pos = newINTVECraw (my size);
		autoCollection thee = Data_copy (me);

		for (integer i = 1; i <= my size; i ++)
			pos [i] = i;

		/* Dual meaning of array pos: */
		/* k <  i : position of item 'k' */
		/* k >= i : the item at position 'k' */
		for (integer i = 1; i <= my size; i++) {
			integer ti = pos [i], which = Permutation_getValueAtIndex (him, i);
			integer where = pos [which];   // where >= i
			Daata tmp = static_cast<Daata> (thy at [i]);
			if (i == where)
				continue;

			thy at [i] = thy at [where];
			thy at [where] = tmp;
			/* order is important !! */
			pos [ti] = where;
			pos [where] = ti;
			pos [which] = ( which <= i ? i : ti );
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
		if (my size != thy size) {
			Melder_throw (U"sizes should be equal.");
		}
		autoStringList him = Data_copy (me);   // FIXME: this copies *all* the data from me, and only the strings from thee

		for (integer i = 1; i <= my size; i ++) {
			SimpleString hisCategory = his at [i], thyCategory = thy at [i];
			integer hisLength = str32len (hisCategory -> string.get()), thyLength = str32len (thyCategory -> string.get());
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
	integer numberOfDifferences = 0;
	if (my size != thy size)
		return -1;   // FIXME: this is arbitrary and unexpected
	for (integer i = 1; i <= my size; i ++) {
		if (! Data_equal (my at [i], thy at [i]))   // FIXME: this compares all the data, instead of just the strings
			numberOfDifferences ++;
	}
	return numberOfDifferences;
}

double OrderedOfString_getFractionDifferent (StringList me, StringList thee) {
	integer numberOfDifferences = OrderedOfString_getNumberOfDifferences (me, thee);
	if (numberOfDifferences < 0) {
		return undefined;
	}
	return my size == 0 ? 0.0 : (double) numberOfDifferences / my size;
}

integer OrderedOfString_indexOfItem_c (StringList me, conststring32 str) {
	integer index = 0;
	autoSimpleString s = SimpleString_create (str);

	for (integer i = 1; i <= my size; i ++) {
		if (Data_equal (my at [i], s.get())) {
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

void OrderedOfString_changeStrings (StringList me, char32 *search, char32 *replace, int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp) {
	regexp *compiled_search = nullptr;
	try {
		Melder_require (search, U"The search string should not be empty.");
		Melder_require (replace, U"The replace string should not be empty.");

		if (use_regexp)
			compiled_search = CompileRE_throwable (search, 0);
		for (integer i = 1; i <= my size; i ++) {
			SimpleString ss = my at [i];
			integer nmatches_sub;
			autostring32 r = use_regexp ?
					newSTRreplace_regex (ss -> string.get(), compiled_search, replace, maximumNumberOfReplaces, & nmatches_sub) :
					newSTRreplace (ss -> string.get(), search, replace, maximumNumberOfReplaces, & nmatches_sub);

			/*
				Change without error.
			*/
			ss -> string = r.move();

			if (nmatches_sub > 0) {
				*nmatches += nmatches_sub;
				(*nstringmatches) ++;
			}
		}
		if (use_regexp)
			free (compiled_search);
	} catch (MelderError) {
		if (use_regexp)
			free (compiled_search);
		Melder_throw (U"Replace not completed.");
	}
}

integer OrderedOfString_isSubsetOf (StringList me, StringList thee, integer *translation) { // ?? test and give number
	integer nStrings = 0;

	for (integer i = 1; i <= my size; i ++) {
		if (translation)
			translation [i] = 0;
		for (integer j = 1; j <= thy size; j ++)
			if (Data_equal (my at [i], thy at [j])) {
				if (translation)
					translation [i] = j;
				nStrings ++;
				break;
			}
	}
	return nStrings;
}

void OrderedOfString_removeOccurrences (StringList me, conststring32 search, bool use_regexp) {
	if (! search)
		return;
	for (integer i = my size; i >= 1; i --) {
		SimpleString ss = my at [i];
		if ((use_regexp && strstr_regexp (ss -> string.get(), search)) ||
		        (! use_regexp && str32str (ss -> string.get(), search)))
		{
			my removeItem (i);
		}
	}
}

/* End of file Collection_extensions.cpp */
