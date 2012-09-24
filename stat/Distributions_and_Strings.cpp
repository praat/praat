/* Distributions_and_Strings.cpp
 *
 * Copyright (C) 1997-2011 Paul Boersma
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
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2003/07/28 factored out Distributions_peek
 * pb 2007/08/12 wchar_t
 * pb 2011/03/20 C++
 */

#include "Distributions_and_Strings.h"

Strings Distributions_to_Strings (Distributions me, long column, long numberOfStrings) {
	try {
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = numberOfStrings;
		thy strings = NUMvector <wchar_t*> (1, numberOfStrings);
		for (long istring = 1; istring <= numberOfStrings; istring ++) {
			wchar_t *string;
			Distributions_peek (me, column, & string);
			thy strings [istring] = Melder_wcsdup (string);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Strings not generated.");
	}
}

Strings Distributions_to_Strings_exact (Distributions me, long column) {
	try {
		long total = 0;
		long istring = 0;
		if (column > my numberOfColumns)
			Melder_throw ("No column ", column, ".");
		if (my numberOfRows < 1)
			Melder_throw ("No candidates.");
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			double value = my data [irow] [column];
			if (value != floor (value))
				Melder_throw ("Non-integer value ", value, " in row ", irow, ".");
			if (value < 0.0)
				Melder_throw ("Found a negative value ", value, " in row ", irow, ".");
			total += value;
		}
		if (total <= 0)
			Melder_throw ("Column total not positive.");
		autoStrings thee = Thing_new (Strings);
		thy numberOfStrings = total;
		thy strings = NUMvector <wchar_t*> (1, total);
		for (long irow = 1; irow <= my numberOfRows; irow ++) {
			long number = my data [irow] [column];
			wchar_t *string = my rowLabels [irow];
			if (! string)
				Melder_throw ("No string in row ", irow, ".");
			for (long i = 1; i <= number; i ++) {
				thy strings [++ istring] = Melder_wcsdup (string);
			}
		}
		Strings_randomize (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Strings not generated.");
	}
}

Distributions Strings_to_Distributions (Strings me) {
	try {
		autoDistributions thee = Distributions_create (my numberOfStrings, 1);
		long idist = 0;
		for (long i = 1; i <= my numberOfStrings; i ++) {
			wchar_t *string = my strings [i];
			long where = 0;
			long j = 1;
			for (; j <= idist; j ++)
				if (wcsequ (thy rowLabels [j], string))
					{ where = j; break; }
			if (where) {
				thy data [j] [1] += 1.0;
			} else {
				thy rowLabels [++ idist] = Melder_wcsdup (string);
				thy data [idist] [1] = 1.0;
			}
		}
		thy numberOfRows = idist;
		TableOfReal_sortByLabel (thee.peek(), 1, 0);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": distribution not computed.");
	}
}

/* End of file Distributions_and_Strings.cpp */
