/* Categories_and_Strings.cpp
 *
 * Copyright (C) 1993-2011, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020315 GPL header
 djmw 20110304 Thing_new
 */

#include "Categories_and_Strings.h"

autoStrings Categories_to_Strings (Categories me) {
	try {
		if (my size() < 1) {
			Melder_throw (U"No elements.");
		}
		autoStrings thee = Thing_new (Strings);
		thy strings = NUMvector<char32 *> (1, my size());
		thy numberOfStrings = my size();

		for (long i = 1; i <= my size(); i ++) {
			SimpleString s = my _item [i];
			thy strings [i] = Melder_dup (s -> string);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Strings.");
	}
}

autoCategories Strings_to_Categories (Strings me) {
	try {
		if (my numberOfStrings < 1) {
			Melder_throw (U"Empty strings.");
		}
		autoCategories thee = Thing_new (Categories);
		thy _grow (my numberOfStrings);

		for (long i = 1; i <= my numberOfStrings; i++) {
			autoSimpleString s = SimpleString_create (my strings [i]);
			thy addItem_move (s.move());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted.");
	}
}

/* End of file Categories_and_Strings.cpp */
