#ifndef _melder_kar_h_
#define _melder_kar_h_
/* melder_kar.h
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

#define kUCD_TOP_OF_ASCII  127
#define kUCD_TOP_OF_LIST  0x2FA1D
#define kUCD_UNASSIGNED  0

enum {
	mUCD_UPPERCASE_LETTER = (1 << 0),
	mUCD_LOWERCASE_LETTER = (1 << 1),
	mUCD_TITLECASE_LETTER = (1 << 2),
	mUCD_CASED_LETTER = (mUCD_UPPERCASE_LETTER | mUCD_LOWERCASE_LETTER | mUCD_TITLECASE_LETTER),
	mUCD_MODIFIER_LETTER = (1 << 3),
	mUCD_OTHER_LETTER = (1 << 4),
	mUCD_LETTER = (mUCD_CASED_LETTER | mUCD_MODIFIER_LETTER | mUCD_OTHER_LETTER),

	mUCD_NONSPACING_MARK = (1 << 5),
	mUCD_SPACING_MARK = (1 << 6),
	mUCD_ENCLOSING_MARK = (1 << 7),
	mUCD_MARK = (mUCD_NONSPACING_MARK | mUCD_SPACING_MARK | mUCD_ENCLOSING_MARK),

	mUCD_DECIMAL_NUMBER = (1 << 8),
	mUCD_LETTER_NUMBER = (1 << 9),
	mUCD_OTHER_NUMBER = (1 << 10),
	mUCD_NUMBER = (mUCD_DECIMAL_NUMBER | mUCD_LETTER_NUMBER | mUCD_OTHER_NUMBER),

	mUCD_CONNECTOR_PUNCTUATION = (1 << 11),
	mUCD_DASH_PUNCTUATION = (1 << 12),
	mUCD_OPEN_PUNCTUATION = (1 << 13),
	mUCD_CLOSE_PUNCTUATION = (1 << 14),
	mUCD_INITIAL_PUNCTUATION = (1 << 15),
	mUCD_FINAL_PUNCTUATION = (1 << 16),
	mUCD_OTHER_PUNCTUATION = (1 << 17),
	mUCD_PUNCTUATION = (mUCD_CONNECTOR_PUNCTUATION | mUCD_DASH_PUNCTUATION | mUCD_OPEN_PUNCTUATION | mUCD_CLOSE_PUNCTUATION | mUCD_INITIAL_PUNCTUATION | mUCD_FINAL_PUNCTUATION | mUCD_OTHER_PUNCTUATION),

	mUCD_MATH_SYMBOL = (1 << 18),
	mUCD_CURRENCY_SYMBOL = (1 << 19),
	mUCD_MODIFIER_SYMBOL = (1 << 20),
	mUCD_OTHER_SYMBOL = (1 << 21),
	mUCD_SYMBOL = (mUCD_MATH_SYMBOL | mUCD_CURRENCY_SYMBOL | mUCD_MODIFIER_SYMBOL | mUCD_OTHER_SYMBOL),

	mUCD_BREAKING_SPACE = (1 << 22),
	mUCD_NON_BREAKING_SPACE = (1 << 23),   // note: this keeps *lines* together; it still separates *words*, despite interpretations elsewhere
	mUCD_SPACE_SEPARATOR = (mUCD_BREAKING_SPACE | mUCD_NON_BREAKING_SPACE),
	mUCD_LINE_SEPARATOR = (1 << 24),
	mUCD_PARAGRAPH_SEPARATOR = (1 << 25),
	mUCD_NEWLINE = (mUCD_LINE_SEPARATOR | mUCD_PARAGRAPH_SEPARATOR),
	mUCD_SEPARATOR = (mUCD_SPACE_SEPARATOR | mUCD_NEWLINE),

	mUCD_CONTROL = (1 << 26),
	mUCD_FORMAT = (1 << 27),
	mUCD_PRIVATE_USE = (1 << 28),

	mUCD_WORD_CHARACTER = (1 << 29),
	mUCD_NULL = (1 << 30),

	mUCD_ALPHANUMERIC = (mUCD_LETTER | mUCD_NUMBER),
	mUCD_END_OF_INK = (mUCD_SEPARATOR | mUCD_NULL),
	mUCD_END_OF_LINE = (mUCD_NEWLINE | mUCD_NULL),
};

struct UCD_CodePointInfo {
	uint32 features;
	char32 upperCase, lowerCase, titleCase;
	conststring32 decomposed;
	char first, second;
};
extern UCD_CodePointInfo theUnicodeDatabase [1+kUCD_TOP_OF_LIST];

/*
	Praat is an internationalized program, which means it has to work in the same way
	wherever on earth it is used. This means that Praat has to be blind to localized settings,
	such as what counts as a space and what combinations of characters
	count as pairs of lower case and upper case.

	To be able to use Praat all over the world, we therefore define one single
	"international locale", which is simply based on the Unicode features of each code point.
*/

/*
	Internationalize std::isblank ():
*/
inline bool Melder_isHorizontalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_SPACE_SEPARATOR) != 0;
}
inline void Melder_skipHorizontalSpace (char32 **p_text) {
	while (Melder_isHorizontalSpace (**p_text)) (*p_text) ++;
}
inline char32 * Melder_findEndOfHorizontalSpace (char32 *p) {
	while (Melder_isHorizontalSpace (*p)) p ++;
	return p;
}
inline const char32 * Melder_findEndOfHorizontalSpace (const char32 *p) {
	while (Melder_isHorizontalSpace (*p)) p ++;
	return p;
}

inline bool Melder_isAsciiHorizontalSpace (char32 kar) {
	return kar == U'\t' || kar == U' ';
}

inline bool Melder_isVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_NEWLINE) != 0;
}
inline bool Melder_isAsciiVerticalSpace (char32 kar) {
	return kar >= 10 && kar <= 13;   // \n, \v, \f, \r
}

/*
	Internationalize std::isspace ():
*/
inline bool Melder_isHorizontalOrVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_SEPARATOR) != 0;
}
inline bool Melder_isAsciiHorizontalOrVerticalSpace (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_SEPARATOR) != 0;
}
inline void Melder_skipHorizontalOrVerticalSpace (char32 **p_text) {
	while (Melder_isHorizontalOrVerticalSpace (**p_text)) (*p_text) ++;
}
inline void Melder_skipHorizontalOrVerticalSpace (const char32 **p_text) {
	while (Melder_isHorizontalOrVerticalSpace (**p_text)) (*p_text) ++;
}

inline bool Melder_isEndOfInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_INK) != 0;
}
inline bool Melder_isEndOfLine (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_LINE) != 0;
}
inline bool Melder_isEndOfText (char32 kar) {
	return kar == U'\0';
}
inline bool Melder_staysWithinInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_INK) == 0;
}
inline bool Melder_staysWithinLine (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_END_OF_LINE) == 0;
}
inline void Melder_skipToEndOfLine (char32 **p_text) {
	while (Melder_staysWithinLine (**p_text)) (*p_text) ++;
}
inline char32 * Melder_findEndOfInk (char32 *p) {
	while (Melder_staysWithinInk (*p)) p ++;
	return p;
}
inline const char32 * Melder_findEndOfInk (const char32 *p) {
	while (Melder_staysWithinInk (*p)) p ++;
	return p;
}
inline char32 * Melder_findEndOfLine (char32 *p) {
	while (Melder_staysWithinLine (*p)) p ++;
	return p;
}
inline const char32 * Melder_findEndOfLine (const char32 *p) {
	while (Melder_staysWithinLine (*p)) p ++;
	return p;
}

/*
	Internationalize std::isalpha ():
*/
inline bool Melder_isLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_LETTER) != 0;
}
inline bool Melder_isAsciiLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_LETTER) != 0;
}

/*
	Internationalize std::isupper ():
*/
inline bool Melder_isUpperCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_UPPERCASE_LETTER) != 0;
}
inline bool Melder_isAsciiUpperCaseLetter (char32 kar) {
	return kar >= U'A' && kar <= U'Z';
}

/*
	Internationalize std::islower ():
*/
inline bool Melder_isLowerCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_LOWERCASE_LETTER) != 0;
}
inline bool Melder_isAsciiLowerCaseLetter (char32 kar) {
	return kar >= U'a' && kar <= U'z';
}

inline bool Melder_isTitleCaseLetter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_TITLECASE_LETTER) != 0;
}
inline bool Melder_isAsciiTitleCaseLetter (char32 kar) {
	return kar >= U'A' && kar <= U'Z';
}

/*
	Internationalize std::isdigit ():
*/
inline bool Melder_isDecimalNumber (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_DECIMAL_NUMBER) != 0;
}
inline bool Melder_isAsciiDecimalNumber (char32 kar) {
	return kar >= U'0' && kar <= U'9';
}

/*
	We cannot really internationalize std::isxdigit ():
*/
inline bool Melder_isHexadecimalDigit (char32 kar) {
	return (kar >= U'0' && kar <= U'9') || (kar >= U'A' && kar <= U'Z') || (kar >= U'a' && kar <= U'z');
}

/*
	Internationalize std::isalnum ():
*/
inline bool Melder_isAlphanumeric (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_ALPHANUMERIC) != 0;
}
inline bool Melder_isAsciiAlphanumeric (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_ALPHANUMERIC) != 0;
}

inline bool Melder_isWordCharacter (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_WORD_CHARACTER) != 0;
}
inline bool Melder_isAsciiWordCharacter (char32 kar) {
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_WORD_CHARACTER) != 0;
}

/*
	The standard library further contains std::ispunct (), std::iscntrl (), std::isprint (), std::isgraph ().
	These have very little use nowadays, so only for completeness do we include versions of them here,
	which are correct at least for ASCII arguments.
	Of these four functions, Melder_hasInk () is not yet correct for all Unicode points,
	as approximately one half of the mUCD_FORMAT points are inkless as well.
*/
inline bool Melder_isPunctuationOrSymbol (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & (mUCD_PUNCTUATION | mUCD_SYMBOL)) != 0;
}
inline bool Melder_isAsciiPunctuationOrSymbol (char32 kar) {   // same as std::ispunct() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & (mUCD_PUNCTUATION | mUCD_SYMBOL)) != 0;
}
inline bool Melder_isControl (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) != 0;
}
inline bool Melder_isAsciiControl (char32 kar) {   // same as std::iscntrl() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) != 0;
}
inline bool Melder_isPrintable (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) == 0;
}
inline bool Melder_isAsciiPrintable (char32 kar) {   // same as std::isprint() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & mUCD_CONTROL) == 0;
}
inline bool Melder_hasInk (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST && (theUnicodeDatabase [kar]. features & (mUCD_CONTROL | mUCD_SEPARATOR)) == 0;
}
inline bool Melder_hasAsciiInk (char32 kar) {   // same as std::isgraph() with default C locale
	return kar <= kUCD_TOP_OF_ASCII && (theUnicodeDatabase [kar]. features & (mUCD_CONTROL | mUCD_SEPARATOR)) == 0;
}

/*
	Internationalize std::toupper () and std::tolower ():
*/
inline char32 Melder_toUpperCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. upperCase : kar;
}
inline char32 Melder_toLowerCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. lowerCase : kar;
}
inline char32 Melder_toTitleCase (char32 kar) {
	return kar <= kUCD_TOP_OF_LIST ? theUnicodeDatabase [kar]. titleCase : kar;
}

/*
	Search functions instantiating strspn() but much faster (CHECK).
*/
inline const char32 * Melder_findInk (conststring32 str) noexcept {
	if (! str)
		return nullptr;
	const char32 *p = & str [0];
	for (; ! Melder_hasInk (*p); p ++) {
		if (*p == U'\0')
			return nullptr;   // not found
	}
	return p;
}
inline const char32 * Melder_findHorizontalOrVerticalSpace (conststring32 str) noexcept {
	if (! str)
		return nullptr;
	const char32 *p = & str [0];
	for (; ! Melder_isHorizontalOrVerticalSpace (*p); p ++)
		if (*p == U'\0')
			return nullptr;   // not found
	return p;
}

/* End of file melder_kar.h */
#endif
