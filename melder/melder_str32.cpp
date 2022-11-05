/* melder_str32.cpp
 *
 * Copyright (C) 1992-2018,2020-2022 Paul Boersma
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

int str32cmp_numberAware (conststring32 string1, conststring32 string2) noexcept {
	for (;;) {
		const char32 kar1 = string1 [0], kar2 = string2 [0];
		if (kar1 == U'\0')
			return kar2 == U'\0' ? 0 : -1;
		if (kar2 == U'\0')
			return +1;
		const bool isDigit1 = Melder_isDecimalNumber (kar1);
		const bool isDigit2 = Melder_isDecimalNumber (kar2);
		if (isDigit1 != isDigit2)
			return kar1 < kar2 ? -1 : +1;
		if (isDigit1) {
			integer leadingZeroes1 = 0, leadingZeroes2 = 0;
			while (string1 [leadingZeroes1] == U'0')
				leadingZeroes1 ++;
			while (string2 [leadingZeroes2] == U'0')
				leadingZeroes2 ++;
			string1 += leadingZeroes1;
			string2 += leadingZeroes2;
			integer digits1 = 0, digits2 = 0;
			while (Melder_isDecimalNumber (string1 [digits1]))
				digits1 ++;
			while (Melder_isDecimalNumber (string2 [digits2]))
				digits2 ++;
			if (digits1 < digits2)
				return -1;
			if (digits1 > digits2)
				return +1;
			/*
				The two digit spans are now equally long.
			*/
			for (integer idigit = 0; idigit < digits1; idigit ++) {
				const char32 digit1 = string1 [idigit];
				const char32 digit2 = string2 [idigit];
				if (digit1 < digit2)
					return -1;
				if (digit1 > digit2)
					return +1;
			}
			/*
				The two digit sequences are the same.
			*/
			if (leadingZeroes1 < leadingZeroes2)
				return -1;
			if (leadingZeroes1 > leadingZeroes2)
				return +1;
			string1 += digits1;
			string2 += digits1;
		} else {
			if (kar1 < kar2)
				return -1;
			if (kar1 > kar2)
				return +1;
			string1 ++;
			string2 ++;
		}
	}
}

int Melder_cmp (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp (string1, string2);
}

int Melder_cmp_caseInsensitive (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp_caseInsensitive (string1, string2);
}

int Melder_ncmp (conststring32 string1, conststring32 string2, integer n) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp (string1, string2, n);
}

int Melder_ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp_caseInsensitive (string1, string2, n);
}

bool Melder_equ_firstCharacterCaseInsensitive (conststring32 string1, conststring32 string2) {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	if (string1 [0] == U'\0')
		return string2 [0] == U'\0';
	if (Melder_toLowerCase (string1 [0]) != Melder_toLowerCase (string2 [0]))
		return false;
	return str32equ (string1 + 1, string2 + 1);
}

/* End of file melder_str32.cpp */
