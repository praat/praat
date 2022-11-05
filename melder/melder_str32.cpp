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

int str32cmp_numberAware (conststring32 string1_in, conststring32 string2_in) noexcept {
	auto isZero = [] (const char32 kar) -> bool {
		return Melder_isDecimalNumber (kar) && (kar & 0xF) == 0;
	};
	/*
		First round.
	*/
	conststring32 pstring1 = string1_in, pstring2 = string2_in;
	integer totalLeadingZeroes1 = 0, totalLeadingZeroes2 = 0;
	for (;;) {
		const char32 kar1 = *pstring1, kar2 = *pstring2;
		if (kar1 == U'\0') {
			if (kar2 != U'\0')
				return -1;
			break;   // the two strings are "equivalent", i.e. equal except perhaps with respect to leading zeroes
		}
		if (kar2 == U'\0')
			return +1;
		const bool isDigit1 = Melder_isDecimalNumber (kar1);
		const bool isDigit2 = Melder_isDecimalNumber (kar2);
		if (isDigit1 != isDigit2)
			return kar1 < kar2 ? -1 : +1;
		if (isDigit1) {
			integer leadingZeroes1 = 0, leadingZeroes2 = 0;
			while (isZero (pstring1 [leadingZeroes1]))
				leadingZeroes1 ++;
			while (isZero (pstring2 [leadingZeroes2]))
				leadingZeroes2 ++;
			totalLeadingZeroes1 += leadingZeroes1;
			totalLeadingZeroes2 += leadingZeroes2;
			pstring1 += leadingZeroes1;
			pstring2 += leadingZeroes2;
			integer digits1 = 0, digits2 = 0;
			while (Melder_isDecimalNumber (pstring1 [digits1]))
				digits1 ++;
			while (Melder_isDecimalNumber (pstring2 [digits2]))
				digits2 ++;
			if (digits1 < digits2)
				return -1;
			if (digits1 > digits2)
				return +1;
			/*
				The two digit spans are now equally long.
			*/
			for (integer idigit = 0; idigit < digits1; idigit ++) {
				const char32 digit1 = pstring1 [idigit];
				const char32 digit2 = pstring2 [idigit];
				if (digit1 < digit2)
					return -1;
				if (digit1 > digit2)
					return +1;
			}
			/*
				The two digit sequences are the same.
			*/
			pstring1 += digits1;
			pstring2 += digits1;
		} else {
			if (kar1 < kar2)
				return -1;
			if (kar1 > kar2)
				return +1;
			pstring1 ++;
			pstring2 ++;
		}
	}
	/*
		Second round.
	*/
	if (totalLeadingZeroes1 < totalLeadingZeroes2)
		return -1;
	if (totalLeadingZeroes1 > totalLeadingZeroes2)
		return +1;
	/*
		The two strings are equivalent,
		and even have the same total number of leading zeroes.
		Cycle again from left to right,
		now returning as soon as the local numbers of leading zeroes are different.
	*/
	pstring1 = string1_in;
	pstring2 = string2_in;
	for (;;) {
		const char32 kar1 = *pstring1, kar2 = *pstring2;
		if (kar1 == U'\0') {
			Melder_assert (kar2 == U'\0');
			return 0;   // the two strings are identical (not just equivalent)
		}
		const bool isDigit1 = Melder_isDecimalNumber (kar1);
		const bool isDigit2 = Melder_isDecimalNumber (kar2);
		Melder_assert (isDigit1 == isDigit2);
		if (isDigit1) {
			integer leadingZeroes1 = 0, leadingZeroes2 = 0;
			while (isZero (pstring1 [leadingZeroes1]))
				leadingZeroes1 ++;
			while (isZero (pstring2 [leadingZeroes2]))
				leadingZeroes2 ++;
			pstring1 += leadingZeroes1;
			pstring2 += leadingZeroes2;
			integer digits1 = 0, digits2 = 0;
			while (Melder_isDecimalNumber (pstring1 [digits1]))
				digits1 ++;
			while (Melder_isDecimalNumber (pstring2 [digits2]))
				digits2 ++;
			Melder_assert (digits1 == digits2);
			if (leadingZeroes1 < leadingZeroes2)
				return -1;
			if (leadingZeroes1 > leadingZeroes2)
				return +1;
			pstring1 += digits1;
			pstring2 += digits1;
		} else {
			Melder_assert (kar1 == kar2);
			pstring1 ++;
			pstring2 ++;
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
