/* wctype_portable.cpp
 *
 * Copyright (C) 2024 Paul Boersma
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

#include "wctype_portable.h"
#include "melder.h"

int iswalpha_portable (wint_t wc) {
	return Melder_isLetter ((char32) wc);
}

int iswalnum_portable (wint_t wc) {
	return Melder_isAlphanumeric ((char32) wc);
}

int iswpunct_portable (wint_t wc) {
	return Melder_isPunctuationOrSymbol ((char32) wc);
}

int iswdigit_portable (wint_t wc) {
	return Melder_isDecimalNumber ((char32) wc);
}

int iswspace_portable (wint_t wc) {
	return Melder_isHorizontalOrVerticalSpace ((char32) wc);
}

int iswlower_portable (wint_t wc) {
	return Melder_isLowerCaseLetter ((char32) wc);
}

int iswupper_portable (wint_t wc) {
	return Melder_isUpperCaseLetter ((char32) wc);
}

/* End of file wctype_portable.cpp */
