#pragma once
/* STRVEC.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

inline STRVEC STRVECfromTo (STRVEC strvec, integer fromIndex, integer toIndex) {
	integer offsetIndex = fromIndex - 1;
	Melder_assert (offsetIndex >= 0);
	Melder_assert (toIndex <= strvec.size);
	integer rangeSize = toIndex - offsetIndex;
	if (rangeSize <= 0) return STRVEC();
	return STRVEC (& strvec [offsetIndex], toIndex - offsetIndex);
}

inline constSTRVEC STRVECfromTo (constSTRVEC strvec, integer fromIndex, integer toIndex) {
	integer offsetIndex = fromIndex - 1;
	Melder_assert (offsetIndex >= 0);
	Melder_assert (toIndex <= strvec.size);
	integer rangeSize = toIndex - offsetIndex;
	if (rangeSize <= 0) return constSTRVEC();
	return constSTRVEC (& strvec [offsetIndex], rangeSize);
}

/*
	Regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
*/
autostring32vector newSTRVECtokenize (conststring32 string);

/* End of file STRVEC.h */
