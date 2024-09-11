#ifndef _melder_str32_h_
#define _melder_str32_h_
/* melder_str32.h
 *
 * Copyright (C) 1992-2018,2020-2023 Paul Boersma
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

integer Melder8_length (conststring8 stringOrNull);   // can assert on 32-bit platforms if the string is longer than 2^31-1 characters

integer Melder16_length (conststring16 stringOrNull) noexcept;
mutablestring16 str16cpy (mutablestring16 target, conststring16 source) noexcept;

integer constexpr Melder_length (const conststring32 stringOrNull) noexcept {
	if (! stringOrNull)
		return 0;
	const char32 *p = & stringOrNull [0];
	while (*p != U'\0')
		++ p;
	return p - stringOrNull;
}
mutablestring32 str32cpy (mutablestring32 target, conststring32 source) noexcept;
mutablestring32 str32cat (mutablestring32 target, conststring32 source) noexcept;
char32 * stp32cpy (mutablestring32 target, conststring32 source) noexcept;
mutablestring32 str32ncpy (mutablestring32 target, conststring32 source, integer n) noexcept;

/*
	str32cmp() mimics the standard library strcmp() function.
	As str32cmp() works on strings with 32-bit *unsigned* characters,
	we had to decide whether str32cmp() should work correctly even for characters that have bit 31 set.
	We decided that it should.
	str32cmp() returns -1 if `string1` sorts before `string2`, 0 if `string1` and `string2` are equal,
	and +1 if `string1` sorts after `string2` (there is no fourth option, because we assume total ordering).
	As std::strcmp(), str32cmp() exhibits undefined behaviour if `string1` and/or `string2` is null.
	By contrast, Melder_cmp() does allow null strings, by regarding them as equal to the empty string.
*/
int str32cmp (conststring32 string1, conststring32 string2) noexcept;
int Melder_cmp (conststring32 string1orNull, conststring32 string2orNull) noexcept;

/*
	Are strings identical?

	str32equ() simply computes whether str32cmp() would return 0 or not.
*/
//bool str32equ (conststring32 string1, conststring32 string2) noexcept;
//bool Melder_equ (conststring32 string1orNull, conststring32 string2orNull) noexcept;

/*
	What is the sorting order of two strings?

	str32coll() simply calls str32cmp() and returns its result.
	It may look like str32coll() is superfluous, but with additional options,
	as below, str32cmp_XXX() and str32coll_XXX() will diverge.
*/
int str32coll (conststring32 string1, conststring32 string2) noexcept;
int Melder_coll (conststring32 string1orNull, conststring32 string2orNull) noexcept;

/*
	str32coll_numberAware() sorts "hello5" before "hello21",
	i.e. every sequence of digits sorts as its integer value.

	SPECIAL CASE: LEADING ZEROES

	For some different digit sequences, the integer value will be the same,
	namely whenever there are leading zeroes. That is, "hello005" is equivalent
	to "hello5" and should therefore sort before "hello5a", "hello6" and "hello10".
	Within equivalence classes, sorting is determined by the number of leading zeroes;
	thus, "hello005" sorts after "hello5", and "hello05" sorts in between the two.
	With multiple digit sequences, sorting is determined by the *global* number
	of leading zeroes, so that "hello05a5" sorts before "hello5a005".
	One could have a case of unequal strings that have the same global number of
	leading zeroes; in that case, sorting is determined by the *local* number of
	leading zeroes; for instance, within the equivalence class "hello5a5",
	"hello5a005" comes before "hello05a05", which comes before "hello005a5",
	while "hello05a5" comes before all three, and "hello5a0005 comes after all three.

	CAN THIS HANDLE NEGATIVE NUMBERS?

	No, it cannot, because of an ambiguity between hyphens and minus signs.
	After all, "hello-8" could have to be interpreted as 'hello, version 8',
	with the hyphen separating the word from the number. If that is indeed
	the interpretation, "hello-9" should sort after "hello-8", and this is what happens.
	Basically, str32coll_numberAware() simply regards hyphens as non-digits.

	CAN THIS HANDLE FLOATING-POINT NUMBERS?

	No, it cannot, because of an ambiguity between separating dots and decimal points.
	After all, "file8.2" could have to be interpreted as 'chapter 8, section 2'
	and in that case will have to sort before "file8.11".
	If you want to interpret 8.2 and 8.11 as floating-point numbers,
	which sort in the opposite order from chapter–section semantics,
	you will have to make sure that the number of digits after the decimal point
	is the same for both strings, so that "file8.11" will sort before "file8.20".
	By supporting integers, we do support *fixed*-point numbers:
	even without special trickery, "file8.11" will nicely sort before "file14.03".
	Basically, str32coll_numberAware() simply regards dots as non-digits.
*/
int str32coll_numberAware (conststring32 string1, conststring32 string2, bool caseAware = false) noexcept;
int Melder_coll_numberAware (conststring32 string1orNull, conststring32 string2orNull) noexcept;
int str32cmp_caseInsensitive (conststring32 string1, conststring32 string2) noexcept;
int Melder_cmp_caseInsensitive (conststring32 string1orNull, conststring32 string2orNull) noexcept;
int str32cmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, bool caseSensitive) noexcept;

int str32ncmp (conststring32 string1, conststring32 string2, integer n) noexcept;
int Melder_ncmp (conststring32 string1orNull, conststring32 string2orNull, integer n) noexcept;
int str32ncmp_caseInsensitive (conststring32 string1, conststring32 string2, integer n) noexcept;
int Melder_ncmp_caseInsensitive (conststring32 string1orNull, conststring32 string2orNull, integer n) noexcept;
int str32ncmp_optionallyCaseSensitive (conststring32 string1, conststring32 string2, integer n, bool caseSensitive) noexcept;

#define str32equ  ! str32cmp
#define str32nequ  ! str32ncmp
#define Melder_equ  ! Melder_cmp
#define str32equ_caseInsensitive  ! str32cmp_caseInsensitive
#define str32nequ_caseInsensitive  ! str32ncmp_caseInsensitive
#define Melder_equ_caseInsensitive  ! Melder_cmp_caseInsensitive
#define str32equ_optionallyCaseSensitive  ! str32cmp_optionallyCaseSensitive
#define str32nequ_optionallyCaseSensitive  ! str32ncmp_optionallyCaseSensitive
bool Melder_equ_firstCharacterCaseInsensitive (conststring32 string1orNull, conststring32 string2orNull) noexcept;
#define Melder_nequ  ! Melder_ncmp
#define Melder_nequ_caseInsensitive  ! Melder_ncmp_caseInsensitive

inline char32 * str32chr (conststring32 string, char32 kar) noexcept {
	for (; *string != kar; ++ string)
		if (*string == U'\0')
			return nullptr;
	return (char32 *) string;
}
inline char32 * str32chr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	for (; Melder_toLowerCase (*string) != kar; ++ string)
		if (*string == U'\0')
			return nullptr;
	return (char32 *) string;
}
inline char32 * str32rchr (conststring32 string, char32 kar) noexcept {
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string)
		if (*string == kar)
			result = (char32 *) string;
	return result;
}
inline char32 * str32rchr_caseInsensitive (conststring32 string, char32 kar) noexcept {
	kar = Melder_toLowerCase (kar);
	char32 *result = nullptr;
	for (; *string != U'\0'; ++ string)
		if (Melder_toLowerCase (*string) == kar)
			result = (char32 *) string;
	return result;
}
char32 * str32str (conststring32 string, conststring32 find) noexcept;
char32 * str32str_caseInsensitive (conststring32 string, conststring32 find) noexcept;
char32 * str32str_optionallyCaseSensitive (conststring32 string, conststring32 find, bool caseSensitive) noexcept;

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
