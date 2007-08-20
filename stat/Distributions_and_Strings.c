/* Distributions_and_Strings.c
 *
 * Copyright (C) 1997-2007 Paul Boersma
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
 */

#include "Distributions_and_Strings.h"

Strings Distributions_to_Strings (Distributions me, long column, long numberOfStrings) {
	Strings thee = new (Strings);
	long istring;
	thy numberOfStrings = numberOfStrings;
	thy strings = NUMpvector (1, numberOfStrings); cherror
	for (istring = 1; istring <= numberOfStrings; istring ++) {
		wchar_t *string;
		Distributions_peek (me, column, & string); cherror
		thy strings [istring] = Melder_wcsdup (string); cherror
	}
end:
	iferror { forget (thee); return Melder_errorp ("(Distributions_to_Strings:) Not performed."); }
	return thee;
}

Strings Distributions_to_Strings_exact (Distributions me, long column) {
	Strings thee = NULL;
	long total = 0;
	long istring = 0, irow, i;
	if (column > my numberOfColumns)
		error3 (L"No column ", Melder_integer (column), L".")
	if (my numberOfRows < 1)
		error1 (L"No candidates.")
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		double value = my data [irow] [column];
		if (value != floor (value))
			error5 (L"Non-integer value ", Melder_double (value), L" in row ", Melder_integer (irow), L".")
		if (value < 0.0)
			error5 (L"Found a negative value ", Melder_double (value), L" in row ", Melder_integer (irow), L".")
		total += value;
	}
	if (total <= 0)
		error1 (L"Column total not positive.")
	thee = new (Strings);
	thy numberOfStrings = total;
	thy strings = NUMpvector (1, total); cherror
	for (irow = 1; irow <= my numberOfRows; irow ++) {
		long number = my data [irow] [column];
		wchar_t *string = my rowLabels [irow];
		if (! string)
			error3 (L"No string in row ", Melder_integer (irow), L".")
		for (i = 1; i <= number; i ++)
			thy strings [++ istring] = Melder_wcsdup (string); cherror
	}
	Strings_randomize (thee);
end:
	iferror { forget (thee); return Melder_errorp ("(Distributions_to_Strings_exact:) Not performed."); }
	return thee;
}

Distributions Strings_to_Distributions (Strings me) {
	Distributions thee = NULL;
	long i, idist = 0, j;
	thee = Distributions_create (my numberOfStrings, 1); cherror
	for (i = 1; i <= my numberOfStrings; i ++) {
		wchar_t *string = my strings [i];
		long where = 0;
		for (j = 1; j <= idist; j ++)
			if (wcsequ (thy rowLabels [j], string))
				{ where = j; break; }
		if (where) {
			thy data [j] [1] += 1.0;
		} else {
			thy rowLabels [++ idist] = Melder_wcsdup (string); cherror
			thy data [idist] [1] = 1.0;
		}
	}
	thy numberOfRows = idist;
	TableOfReal_sortByLabel (thee, 1, 0);
end:
	iferror forget (thee);
	return thee;
}

/* End of file Distributions_and_Strings.c */
