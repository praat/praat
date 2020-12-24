#ifndef _melder_str32_h_
#define _melder_str32_h_
/* melder_str32.h
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma
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

inline integer str16len (conststring16 string) noexcept {
	const char16 *p = & string [0];
	while (*p != u'\0') ++ p;
	return p - string;
}
inline mutablestring16 str16cpy (mutablestring16 target, conststring16 source) noexcept {
	char16 *p = & target [0];
	while (* source != u'\0') * p ++ = * source ++;
	*p = u'\0';
	return target;
}

inline integer str32len (conststring32 string) noexcept {
	const char32 *p = & string [0];
	while (*p != U'\0') ++ p;
	return p - string;
}
inline mutablestring32 str32cpy (mutablestring32 target, conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}
inline mutablestring32 str32cat (mutablestring32 target, conststring32 source) noexcept {
	char32 *p = & target [0];
	while (*p != U'\0') ++ p;
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return target;
}
inline char32 * stp32cpy (mutablestring32 target, conststring32 source) noexcept {
	char32 *p = & target [0];
	while (* source != U'\0') * p ++ = * source ++;
	*p = U'\0';
	return p;
}
inline mutablestring32 str32ncpy (mutablestring32 target, conststring32 source, integer n) noexcept {
	char32 *p = & target [0];
	for (; n > 0 && *source != U'\0'; -- n) * p ++ = * source ++;
	for (; n > 0; -- n) * p ++ = U'\0';
	return target;
}

inline int str32cmp (conststring32 string1, conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
inline int str32cmp_caseInsensitive (conststring32 string1, conststring32 string2) noexcept {
	for (;; ++ string1, ++ string2) {
		int32 diff = (int32) Melder_toLowerCase (*string1) - (int32) Melder_toLowerCase (*string2);
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
}
inline int str32cmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, bool caseSensitive) noexcept {
	return caseSensitive ? str32cmp (string1, string2) : str32cmp_caseInsensitive (string1, string2);
}
inline int str32ncmp (conststring32 string1, conststring32 string2, integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) *string1 - (int32) *string2;
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
inline int str32ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n) noexcept {
	for (; n > 0; -- n, ++ string1, ++ string2) {
		int32 diff = (int32) Melder_toLowerCase (*string1) - (int32) Melder_toLowerCase (*string2);
		if (diff) return (int) diff;
		if (*string1 == U'\0') return 0;
	}
	return 0;
}
inline int str32ncmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, integer n, bool caseSensitive) noexcept {
	return caseSensitive ? str32ncmp (string1, string2, n) : str32ncmp_caseInsensitive (string1, string2, n);
}

int Melder_cmp (conststring32 string1, conststring32 string2);   // regards null string as empty string
int Melder_cmp_caseInsensitive (conststring32 string1, conststring32 string2);
int Melder_ncmp (conststring32 string1, conststring32 string2, integer n);
int Melder_ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n);

#define str32equ  ! str32cmp
#define str32nequ  ! str32ncmp
#define Melder_equ  ! Melder_cmp
#define str32equ_caseInsensitive  ! str32cmp_caseInsensitive
#define str32nequ_caseInsensitive  ! str32ncmp_caseInsensitive
#define Melder_equ_caseInsensitive  ! Melder_cmp_caseInsensitive
#define str32equ_optionallyCaseSensitive  ! str32cmp_optionallyCaseSensitive
#define str32nequ_optionallyCaseSensitive  ! str32ncmp_optionallyCaseSensitive
bool Melder_equ_firstCharacterCaseInsensitive (conststring32 string1, conststring32 string2);
#define Melder_nequ  ! Melder_ncmp
#define Melder_nequ_caseInsensitive  ! Melder_ncmp_caseInsensitive

inline char32 * str32chr (conststring32 string, char32 kar) noexcept {
	for (; *string != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline char32 * str32chr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	for (; Melder_toLowerCase (*string) != kar; ++ string) {
		if (*string == U'\0')
			return nullptr;
	}
	return (char32 *) string;
}
inline char32 * str32rchr (conststring32 string, char32 kar) noexcept {
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (*string == kar) result = (char32 *) string;
	}
	return result;
}
inline char32 * str32rchr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string) {
		if (Melder_toLowerCase (*string) == kar) result = (char32 *) string;
	}
	return result;
}
inline char32 * str32str (conststring32 string, conststring32 find) noexcept {
	integer length = str32len (find);
	if (length == 0) return (char32 *) string;
	char32 firstCharacter = * find ++;   // optimization
	do {
		char32 kar;
		do {
			kar = * string ++;
			if (kar == U'\0') return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp (string, find, length - 1));
	return (char32 *) (string - 1);
}
inline char32 * str32str_caseInsensitive (conststring32 string, conststring32 find) noexcept {
	integer length = str32len (find);
	if (length == 0) return (char32 *) string;
	char32 firstCharacter = Melder_toLowerCase (* find ++);   // optimization
	do {
		char32 kar;
		do {
			kar = Melder_toLowerCase (* string ++);
			if (kar == U'\0') return nullptr;
		} while (kar != firstCharacter);
	} while (str32ncmp_caseInsensitive (string, find, length - 1));
	return (char32 *) (string - 1);
}
inline char32 * str32str_optionallyCaseSensitive (conststring32 string, conststring32 find, bool caseSensitive) noexcept {
	return caseSensitive ? str32str (string, find) : str32str_caseInsensitive (string, find);
}
inline integer str32spn (conststring32 string1, conststring32 string2) noexcept {
	const char32 *p = & string1 [0];
	char32 kar1, kar2;
cont:
	kar1 = * p ++;
	for (const char32 *q = & string2 [0]; (kar2 = * q ++) != U'\0';)
		if (kar2 == kar1)
			goto cont;
	return p - 1 - string1;
}

/* End of file melder_str32.h */
#endif
