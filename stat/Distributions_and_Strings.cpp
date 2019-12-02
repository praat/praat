/* Distributions_and_Strings.cpp
 *
 * Copyright (C) 1997-2011,2014,2015,2016,2017 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Distributions_and_Strings.h"

autoStrings Distributions_to_Strings (Distributions me, integer column, integer numberOfStrings) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = numberOfStrings;
		thy strings = autoSTRVEC (numberOfStrings);
		for (integer istring = 1; istring <= numberOfStrings; istring ++) {
			conststring32 string;
			Distributions_peek (me, column, & string, nullptr);
			thy strings [istring] = Melder_dup (string);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Strings not generated.");
	}
}

autoStrings Distributions_to_Strings_exact (Distributions me, integer column) {
	try {
		integer total = 0;
		integer istring = 0;
		if (column > my numberOfColumns)
			Melder_throw (U"No column ", column, U".");
		if (my numberOfRows < 1)
			Melder_throw (U"No candidates.");
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			double value = my data [irow] [column];
			if (value != Melder_roundDown (value))
				Melder_throw (U"Non-integer value ", value, U" in row ", irow, U".");
			if (value < 0.0)
				Melder_throw (U"Found a negative value ", value, U" in row ", irow, U".");
			total += value;
		}
		if (total <= 0)
			Melder_throw (U"Column total not positive.");
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = total;
		thy strings = autoSTRVEC (total);
		for (integer irow = 1; irow <= my numberOfRows; irow ++) {
			integer number = my data [irow] [column];
			conststring32 string = my rowLabels [irow].get();
			if (! string)
				Melder_throw (U"No string in row ", irow, U".");
			for (integer i = 1; i <= number; i ++)
				thy strings [++ istring] = Melder_dup (string);
		}
		Strings_randomize (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Strings not generated.");
	}
}

autoDistributions Strings_to_Distributions (Strings me) {
	try {
		autoDistributions thee = Distributions_create (my numberOfStrings, 1);
		integer idist = 0;
		for (integer i = 1; i <= my numberOfStrings; i ++) {
			conststring32 string = my strings [i].get();
			integer where = 0;
			integer j = 1;
			for (; j <= idist; j ++) {
				if (str32equ (thy rowLabels [j].get(), string)) {
					where = j;
					break;
				}
			}
			if (where) {
				thy data [j] [1] += 1.0;
			} else {
				thy rowLabels [++ idist] = Melder_dup (string);
				thy data [idist] [1] = 1.0;
			}
		}
		thy numberOfRows = idist;
		TableOfReal_sortByLabel (thee.get(), 1, 0);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": distribution not computed.");
	}
}

/* End of file Distributions_and_Strings.cpp */
