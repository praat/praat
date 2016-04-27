/* Distributions_and_Strings.cpp
 *
 * Copyright (C) 1997-2011,2014,2015,2016 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Distributions_and_Strings.h"

autoStrings Distributions_to_Strings (Distributions me, long column, long numberOfStrings) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = numberOfStrings;
		thy strings = NUMvector <char32*> (1, numberOfStrings);
		for (long istring = 1; istring <= numberOfStrings; istring ++) {
			char32 *string;
			Distributions_peek (me, column, & string, nullptr);
			thy strings [istring] = Melder_dup (string);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Strings not generated.");
	}
}

autoStrings Distributions_to_Strings_exact (Distributions me, long column) {
	try {
		long total = 0;
		long istring = 0;
		if (column > my numberOfColumns)
			Melder_throw (U"No column ", column, U".");
		if (my numberOfRows < 1)
			Melder_throw (U"No candidates.");
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			double value = my data [irow] [column];
			if (value != floor (value))
				Melder_throw (U"Non-integer value ", value, U" in row ", irow, U".");
			if (value < 0.0)
				Melder_throw (U"Found a negative value ", value, U" in row ", irow, U".");
			total += value;
		}
		if (total <= 0)
			Melder_throw (U"Column total not positive.");
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = total;
		thy strings = NUMvector <char32*> (1, total);
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			long number = my data [irow] [column];
			char32 *string = my rowLabels [irow];
			if (! string)
				Melder_throw (U"No string in row ", irow, U".");
			for (long i = 1; i <= number; i ++) {
				thy strings [++ istring] = Melder_dup (string);
			}
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
		long idist = 0;
		for (long i = 1; i <= my numberOfStrings; i ++) {
			char32 *string = my strings [i];
			long where = 0;
			long j = 1;
			for (; j <= idist; j ++)
				if (str32equ (thy rowLabels [j], string))
					{ where = j; break; }
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
