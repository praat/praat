/* Collection_extensions.cpp
 *
 * Copyright (C) 1994-2011, 2015-2017 David Weenink
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
 djmw 20040420 Fraction in OrderedOfString_difference must be double.
 djmw 20050511 Skip printing unique labels in OrderedOfString
 djmw 20061214
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20110304 Thing_new
*/

#include "Collection_extensions.h"
#include "Simple_extensions.h"
#include "NUM2.h"

autoCollection Collection_and_Permutation_permuteItems (Collection me, Permutation him) {
	try {
		if (my size != his numberOfElements) {
			Melder_throw (me, U"The number of elements are not equal.");
		}
		autoNUMvector<long> pos (1, my size);
		autoCollection thee = Data_copy (me);

		for (long i = 1; i <= my size; i ++) {
			pos [i] = i;
		}
		/* Dual meaning of array pos: */
		/* k <  i : position of item 'k' */
		/* k >= i : the item at position 'k' */
		for (long i = 1; i <= my size; i++) {
			long ti = pos [i], which = Permutation_getValueAtIndex (him, i);
			long where = pos [which];   // where >= i
			Daata tmp = static_cast<Daata> (thy at [i]);
			if (i == where) {
				continue;
			}
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
		Permutation_permuteRandomly_inline (p.get(), 0, 0);
		autoCollection thee = Collection_and_Permutation_permuteItems (me, p.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": items not permuted.");
	}
}

/****************** class OrderedOfString ******************/

void structOrderedOfString :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of strings: ", our size);
	autoOrderedOfString uStrings = OrderedOfString_selectUniqueItems (this);
	MelderInfo_writeLine (U"Number of unique categories: ", uStrings->size);   // FIXME: "categories"?, and why mention a Set property?
}

Thing_implement (OrderedOfString, Ordered, 0);

int OrderedOfString_append (OrderedOfString me, const char32 *append) {
	try {
		if (! append) {
			return 1;    // BUG: lege string appenden??
		}
		autoSimpleString item = SimpleString_create (append);
		my addItem_move (item.move());
		return 1;
	} catch (MelderError) {
		Melder_throw (me, U": text not appended.");
	}
}

autoOrderedOfString OrderedOfString_joinItems (OrderedOfString me, OrderedOfString thee) {
	try {
		if (my size != thy size) {
			Melder_throw (U"sizes must be equal.");
		}
		autoOrderedOfString him = Data_copy (me);

		for (long i = 1; i <= my size; i ++) {
			SimpleString_append (his at [i], thy at [i]);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"Items not joinmed.");
	}
}

autoOrderedOfString OrderedOfString_selectUniqueItems (OrderedOfString me) {
	try {
		autoStringSet thee = StringSet_create ();
		for (long i = 1; i <= my size; i ++) {
			if (! thy hasItem (my at [i])) {   // FIXME: first sort, then unicize
				autoSimpleString item = Data_copy (my at [i]);
				thy addItem_move (item.move());
			}
		}
		autoOrderedOfString him = OrderedOfString_create ();
		for (long i = 1; i <= thy size; i ++) {
			autoSimpleString item = Data_copy (thy at [i]);
			his addItem_move (item.move());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": unique items not selected.");
	}
}

void OrderedOfString_frequency (OrderedOfString me, OrderedOfString thee, long *count) {
	for (long i = 1; i <= my size; i ++) {
		for (long j = 1; j <= thy size; j ++) {
			if (Data_equal (my at [i], thy at [j])) {
				count[j]++;
				break;
			}
		}
	}
}

long OrderedOfString_getNumberOfDifferences (OrderedOfString me, OrderedOfString thee) {
	long numberOfDifferences = 0;

	if (my size != thy size) {
		return -1;
	}
	for (long i = 1; i <= my size; i ++) {
		if (! Data_equal (my at [i], thy at [i])) {
			numberOfDifferences++;
		}
	}
	return numberOfDifferences;
}

double OrderedOfString_getFractionDifferent (OrderedOfString me, OrderedOfString thee) {
	long numberOfDifferences = OrderedOfString_getNumberOfDifferences (me, thee);

	if (numberOfDifferences < 0) {
		return undefined;
	}
	return my size == 0 ? 0.0 : (0.0 + numberOfDifferences) / my size;
}

int OrderedOfString_difference (OrderedOfString me, OrderedOfString thee, long *ndif, double *fraction) {
	*ndif = 0;
	*fraction = 1.0;
	if (my size != thy size) {
		Melder_flushError (U"OrderedOfString_difference: the numbers of items differ");
		return 0;
	}
	for (long i = 1; i <= my size; i ++) {
		if (! Data_equal (my at [i], thy at [i])) {
			(*ndif) ++;
		}
	}
	*fraction = *ndif;
	*fraction /= my size;
	return 1;
}

long OrderedOfString_indexOfItem_c (OrderedOfString me, const char32 *str) {
	long index = 0;
	autoSimpleString s = SimpleString_create (str);

	for (long i = 1; i <= my size; i ++) {
		if (Data_equal (my at [i], s.get())) {
			index = i;
			break;
		}
	}
	return index;
}

const char32 *OrderedOfString_itemAtIndex_c (OrderedOfString me, long index) {
	return index > 0 && index <= my size ? SimpleString_c (my at [index]) : nullptr;
}

void OrderedOfString_initWithSequentialNumbers (OrderedOfString me, long n) {
	for (long i = 1; i <= n; i ++) {
		my addItem_move (SimpleString_create (Melder_integer (i)));
	}
}

void OrderedOfString_changeStrings (OrderedOfString me, char32 *search, char32 *replace, int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp) {
	regexp *compiled_search = nullptr;
	try {
		if (! search) {
			Melder_throw (U"Missing search string.");
		}
		if (! replace) {
			Melder_throw (U"Missing replace string.");
		}

		if (use_regexp) {
			compiled_search = CompileRE_throwable (search, 0);
		}
		for (long i = 1; i <= my size; i ++) {
			SimpleString ss = my at [i];
			long nmatches_sub;
			char32 *r = use_regexp ? str_replace_regexp (ss -> string, compiled_search, replace, maximumNumberOfReplaces, &nmatches_sub) : str_replace_literal (ss -> string, search, replace, maximumNumberOfReplaces, &nmatches_sub);

			// Change without error:
			Melder_free (ss -> string);
			ss -> string = r;
			if (nmatches_sub > 0) {
				*nmatches += nmatches_sub;
				(*nstringmatches) ++;
			}
		}
		if (use_regexp) {
			free (compiled_search);
		}
	} catch (MelderError) {
		if (use_regexp) {
			free (compiled_search);
		}
		Melder_throw (U"Replace not completed.");
	}
}

long OrderedOfString_isSubsetOf (OrderedOfString me, OrderedOfString thee, long *translation) { // ?? test and give number
	long nStrings = 0;

	for (long i = 1; i <= my size; i ++) {
		if (translation) {
			translation [i] = 0;
		}
		for (long j = 1; j <= thy size; j ++)
			if (Data_equal (my at [i], thy at [j])) {
				if (translation) {
					translation[i] = j;
				}
				nStrings++; break;
			}
	}
	return nStrings;
}

void OrderedOfString_drawItem (OrderedOfString me, Graphics g, long index, double xWC, double yWC) {
	if (index > 0 && index <= my size) {
		SimpleString_draw (my at [index], g, xWC, yWC);
	}
}

long OrderedOfString_getSize (OrderedOfString me) {
	return my size;
}

void OrderedOfString_removeOccurrences (OrderedOfString me, const char32 *search, int use_regexp) {
	if (! search) {
		return;
	}
	for (long i = my size; i >= 1; i --) {
		SimpleString ss = my at [i];
		if ( (use_regexp && strstr_regexp (ss -> string, search)) ||
		        (!use_regexp && str32str (ss -> string, search))) {
			my removeItem (i);
		}
	}
}

/* End of file Collection_extensions.cpp */
