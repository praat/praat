/* Categories_and_Strings.cpp
 *
 * Copyright (C) 1993-2018 David Weenink
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
 djmw 20020315 GPL header
 djmw 20110304 Thing_new
 */

#include "Categories_and_Strings.h"

autoStrings Categories_to_Strings (Categories me) {
	try {
		Melder_require (my size > 0,
			U"There should be at least one category present.");
		
		autoStrings thee = Thing_new (Strings);
		thy strings = autoSTRVEC (my size);
		thy numberOfStrings = my size;

		for (integer i = 1; i <= my size; i ++) {
			const SimpleString s = my at [i];
			thy strings [i] = Melder_dup (s -> string.get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Strings.");
	}
}

autoCategories Strings_to_Categories (Strings me) {
	try {
		Melder_require (my numberOfStrings > 0, U"Empty strings.");
		
		autoCategories thee = Thing_new (Categories);
		thy _grow (my numberOfStrings);

		for (integer i = 1; i <= my numberOfStrings; i ++) {
			autoSimpleString s = SimpleString_create (my strings [i].get());
			thy addItem_move (s.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted.");
	}
}

/* End of file Categories_and_Strings.cpp */
