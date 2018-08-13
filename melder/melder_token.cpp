/* melder_token.cpp
 *
 * Copyright (C) 2006,2007,2009,2011,2012,2015-2017 Paul Boersma
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

#include "melder.h"

static integer countTokens (conststring32 string) {
	integer numberOfTokens = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		numberOfTokens ++;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
	}
	return numberOfTokens;
}

autostring32vector Melder_getTokens (conststring32 string) {
	if (! string)
		return autostring32vector();   // accept null pointer input
	integer n = countTokens (string);
	if (n == 0)
		return autostring32vector();
	autostring32vector result (n);

	integer itoken = 0;
	const char32 *p = & string [0];
	for (;;) {
		Melder_skipHorizontalOrVerticalSpace (& p);
		if (*p == U'\0')
			break;
		const char32 *beginOfInk = p;
		p ++;   // step over first nonspace
		p = Melder_findEndOfInk (p);
		integer numberOfCharacters = p - beginOfInk;
		autostring32 token (numberOfCharacters);
		str32ncpy (token.get(), beginOfInk, numberOfCharacters);
		result [++ itoken] = token.move();
	}
	return result;
}

/* End of file melder_token.cpp */
