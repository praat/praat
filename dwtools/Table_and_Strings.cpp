/* Table_and_Strings.cpp
 *
 * Copyright (C) 2018 David Weenink
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

#include "Table_and_Strings.h"

autoStrings Table_column_to_Strings (Table me, integer columnNumber) {
	try {
		Table_checkSpecifiedColumnNumberWithinRange (me, columnNumber);
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my rows.size);
		thy numberOfStrings = 0;
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			thy strings [irow] = Melder_dup (Table_getStringValue_Assert (me, irow, columnNumber));
			thy numberOfStrings ++;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"A Strings could not be created from the column in the Table.");
	}
}

/* End of file Table_and_Strings.cpp */
