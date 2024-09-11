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

#pragma mark - first one 8-bit string function...

integer Melder8_length (const conststring8 stringOrNull) {
	if (! stringOrNull)
		return 0;
	const char *p = & stringOrNull [0];
	while (*p != '\0')
		++ p;
	const integer result = p - stringOrNull;
	if (sizeof (integer) == 4)
		Melder_assert (result >= 0);
	return result;
}

#pragma mark - ...then two 16-bit string functions...

integer Melder16_length (const conststring16 stringOrNull) noexcept {
	if (! stringOrNull)
		return 0;
	const char16 *p = & stringOrNull [0];
	while (*p != u'\0')
		++ p;
	return p - stringOrNull;
}
mutablestring16 str16cpy (const mutablestring16 target, /*mut*/ conststring16 source) noexcept {
	char16 *p = & target [0];
	while (* source != u'\0')
		* p ++ = * source ++;
	*p = u'\0';
	return target;
}

#pragma mark - ...and the remainder are 32-bit string functions

mutablestring32 str32cpy (const mutablestring32 target, /*mut*/ conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0')
		* p ++ = * source ++;
	*p = U'\0';
	return target;
}
mutablestring32 str32cat (const mutablestring32 target, /*mut*/ conststring32 source) noexcept {
	char32 *p = & target [0];
	while (*p != U'\0')
		++ p;
	while (* source != U'\0')
		* p ++ = * source ++;
	*p = U'\0';
	return target;
}
char32 * stp32cpy (const mutablestring32 target, /*mut*/ conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0')
		* p ++ = * source ++;
	*p = U'\0';
	return p;
}
mutablestring32 str32ncpy (const mutablestring32 target, /*mut*/ conststring32 source, /*mut*/ integer n) noexcept {
	char32 *p = & target [0];
	for (; n > 0 && *source != U'\0'; -- n)
		* p ++ = * source ++;
	for (; n > 0; -- n)
		* p ++ = U'\0';
	return target;
}

#pragma mark - SEARCHING

char32 * str32str (/*mut*/ conststring32 string, /*mut*/ conststring32 find) noexcept {
	const integer length = Melder_length (find);
	if (length == 0)
		return (char32 *) string;
	char32 firstCharacter = * find ++;   // optimization
	do {
		char32 kar;
		do {
			kar = * string ++;
			if (kar == U'\0')
				return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp (string, find, length - 1));
	return (char32 *) (string - 1);
}
char32 * str32str_caseInsensitive (/*mut*/ conststring32 string, /*mut*/ conststring32 find) noexcept {
	const integer length = Melder_length (find);
	if (length == 0)
		return (char32 *) string;
	char32 firstCharacter = Melder_toLowerCase (* find ++);   // optimization
	do {
		char32 kar;
		do {
			kar = Melder_toLowerCase (* string ++);
			if (kar == U'\0')
				return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp_caseInsensitive (string, find, length - 1));
	return (char32 *) (string - 1);
}
char32 * str32str_optionallyCaseSensitive (conststring32 string, conststring32 find, bool caseSensitive) noexcept {
	return caseSensitive ? str32str (string, find) : str32str_caseInsensitive (string, find);
}

#pragma mark - EQUIVALENCE OF TWO STRINGS

int str32cmp (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		const char32 kar1 = *string1, kar2 = *string2;
		if (kar1 < kar2)
			return -1;
		if (kar1 > kar2)
			return +1;
		/*
			kar1 is now equal to kar2
		*/
		if (kar1 == U'\0')
			return 0;
	}
}
int Melder_cmp (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp (string1, string2);
}

#pragma mark - SORTING OF TWO STRINGS

int str32coll (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		const char32 kar1 = *string1, kar2 = *string2;
		if (kar1 < kar2)
			return -1;
		if (kar1 > kar2)
			return +1;
		/*
			kar1 is now equal to kar2
		*/
		if (kar1 == U'\0')
			return 0;
	}
}
int Melder_coll (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp (string1, string2);
}

int str32coll_numberAware (const conststring32 string1, const conststring32 string2, const bool caseAware) noexcept {
	auto isZero = [] (const char32 kar) -> bool {
		return Melder_isDecimalNumber (kar) && (kar & 0xF) == 0;
	};
	/*
		First round.
	*/
	conststring32 pstring1 = string1, pstring2 = string2;
	integer totalLeadingZeroes1 = 0, totalLeadingZeroes2 = 0;
	for (;;) {
		const char32 kar1 = ( caseAware ? Melder_toLowerCase (*pstring1) : *pstring1 );
		const char32 kar2 = ( caseAware ? Melder_toLowerCase (*pstring2) : *pstring2 );
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
		The two strings are equivalent numberwise,
		and even have the same total number of leading zeroes.
		Cycle again from left to right,
		now returning as soon as the local numbers of leading zeroes are different.
	*/
	pstring1 = string1;
	pstring2 = string2;
	for (;;) {
		const char32 kar1 = ( caseAware ? Melder_toLowerCase (*pstring1) : *pstring1 );
		const char32 kar2 = ( caseAware ? Melder_toLowerCase (*pstring2) : *pstring2 );
		if (kar1 == U'\0') {
			Melder_assert (kar2 == U'\0');
			/*
				The two strings are identical (not just equivalent numberwise).
				They could still differ in case.
			*/
			return caseAware ? str32coll (string1, string2) : 0;
		}
		const bool isDigit = Melder_isDecimalNumber (kar1);
		if (isDigit) {
			integer leadingZeroes1 = 0, leadingZeroes2 = 0;
			while (isZero (pstring1 [leadingZeroes1]))
				leadingZeroes1 ++;
			while (isZero (pstring2 [leadingZeroes2]))
				leadingZeroes2 ++;
			if (leadingZeroes1 < leadingZeroes2)
				return -1;
			if (leadingZeroes1 > leadingZeroes2)
				return +1;
			pstring1 += leadingZeroes1;
			pstring2 += leadingZeroes1;
			integer digits = 0;
			while (Melder_isDecimalNumber (pstring1 [digits]))
				digits ++;
			pstring1 += digits;
			pstring2 += digits;
		} else {
			Melder_assert (kar1 == kar2);
			pstring1 ++;
			pstring2 ++;
		}
	}
}
int Melder_coll_numberAware (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32coll_numberAware (string1, string2);
}

int str32cmp_caseInsensitive (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		const char32 kar1 = Melder_toLowerCase (*string1);
		const char32 kar2 = Melder_toLowerCase (*string2);
		if (kar1 < kar2)
			return -1;
		if (kar1 > kar2)
			return +1;
		if (kar1 == U'\0')
			return 0;
	}
}
int Melder_cmp_caseInsensitive (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32cmp_caseInsensitive (string1, string2);
}

int str32cmp_optionallyCaseSensitive (const conststring32 string1, const conststring32 string2, bool caseSensitive) noexcept {
	return caseSensitive ? str32cmp (string1, string2) : str32cmp_caseInsensitive (string1, string2);
}

int str32ncmp (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2, /*mut*/ integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		const char32 kar1 = *string1, kar2 = *string2;
		if (kar1 < kar2)
			return -1;
		if (kar1 > kar2)
			return +1;
		if (kar1 == U'\0')
			return 0;
	}
	return 0;
}
int Melder_ncmp (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2, const integer n) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp (string1, string2, n);
}

int str32ncmp_caseInsensitive (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2, /*mut*/ integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		const char32 kar1 = Melder_toLowerCase (*string1);
		const char32 kar2 = Melder_toLowerCase (*string2);
		if (kar1 < kar2)
			return -1;
		if (kar1 > kar2)
			return +1;
		if (kar1 == U'\0')
			return 0;
	}
	return 0;
}
int Melder_ncmp_caseInsensitive (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2, const integer n) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	return str32ncmp_caseInsensitive (string1, string2, n);
}

int str32ncmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, integer n, bool caseSensitive) noexcept {
	return caseSensitive ? str32ncmp (string1, string2, n) : str32ncmp_caseInsensitive (string1, string2, n);
}

bool Melder_equ_firstCharacterCaseInsensitive (/*mut*/ conststring32 string1, /*mut*/ conststring32 string2) noexcept {
	if (! string1) string1 = U"";
	if (! string2) string2 = U"";
	if (string1 [0] == U'\0')
		return string2 [0] == U'\0';
	if (Melder_toLowerCase (string1 [0]) != Melder_toLowerCase (string2 [0]))
		return false;
	return str32equ (string1 + 1, string2 + 1);
}

/* End of file melder_str32.cpp */
