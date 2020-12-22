/* melder_search.cpp
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

#include "melder.h"

/********** NUMBER AND STRING COMPARISONS **********/

bool Melder_numberMatchesCriterion (double value, kMelder_number which, double criterion) {
	return
		(which == kMelder_number::EQUAL_TO && value == criterion) ||
		(which == kMelder_number::NOT_EQUAL_TO && value != criterion) ||
		(which == kMelder_number::LESS_THAN && value < criterion) ||
		(which == kMelder_number::LESS_THAN_OR_EQUAL_TO && value <= criterion) ||
		(which == kMelder_number::GREATER_THAN && value > criterion) ||
		(which == kMelder_number::GREATER_THAN_OR_EQUAL_TO && value >= criterion);
}

inline char32 * str32str_word_optionallyCaseSensitive (conststring32 string, conststring32 find,
	bool ink, bool caseSensitive, bool startFree, bool endFree) noexcept
{
	integer length = str32len (find);
	if (length == 0)
		return (char32 *) string;
	conststring32 movingString = string;
	do {
		conststring32 movingFind = find;
		char32 firstCharacter = ( caseSensitive ? * movingFind ++ : Melder_toLowerCase (* movingFind ++) );   // optimization
		do {
			char32 kar;
			do {
				kar = ( caseSensitive ? * movingString ++ : Melder_toLowerCase (* movingString ++) );
				if (kar == U'\0') return nullptr;
			} while (kar != firstCharacter);
		} while (caseSensitive ? str32ncmp (movingString, movingFind, length - 1) : str32ncmp_caseInsensitive (movingString, movingFind, length - 1));
		/*
			We found a match.
		*/
		movingString --;
		if ((startFree || movingString == string || ( ink ? Melder_isHorizontalOrVerticalSpace (movingString [-1]) : ! Melder_isWordCharacter (movingString [-1]) )) &&
			(endFree || movingString [length] == U'\0' || (ink ? Melder_isHorizontalOrVerticalSpace (movingString [length]) : ! Melder_isWordCharacter (movingString [length]) )))
		{
			return (char32 *) movingString;
		} else {
			movingString ++;
		}
	} while (true);
	return nullptr;   // can never occur
}

bool Melder_stringMatchesCriterion (conststring32 value, kMelder_string which, conststring32 criterion, bool caseSensitive) {
	if (! value)
		value = U"";   // regard null strings as empty strings, as is usual in Praat
	if (! criterion)
		criterion = U"";   // regard null strings as empty strings, as is usual in Praat
	switch (which)
	{
		case kMelder_string::UNDEFINED:
		{
			Melder_fatal (U"Melder_stringMatchesCriterion: unknown criterion.");
		}
		case kMelder_string::EQUAL_TO:
		case kMelder_string::NOT_EQUAL_TO:
		{
			bool doesMatch = str32equ_optionallyCaseSensitive (value, criterion, caseSensitive);
			return which == kMelder_string::EQUAL_TO ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS:
		case kMelder_string::DOES_NOT_CONTAIN:
		{
			bool doesMatch = !! str32str_optionallyCaseSensitive (value, criterion, caseSensitive);
			return which == kMelder_string::CONTAINS ? doesMatch : ! doesMatch;
		}
		case kMelder_string::STARTS_WITH:
		case kMelder_string::DOES_NOT_START_WITH:
		{
			bool doesMatch = str32nequ_optionallyCaseSensitive (value, criterion, str32len (criterion), caseSensitive);
			return which == kMelder_string::STARTS_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::ENDS_WITH:
		case kMelder_string::DOES_NOT_END_WITH:
		{
			integer criterionLength = str32len (criterion), valueLength = str32len (value);
			bool doesMatch = criterionLength <= valueLength &&
				str32equ_optionallyCaseSensitive (value + valueLength - criterionLength, criterion, caseSensitive);
			return which == kMelder_string::ENDS_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD:
		case kMelder_string::DOES_NOT_CONTAIN_WORD:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, false, false);
			return which == kMelder_string::CONTAINS_WORD ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD_STARTING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_WORD_STARTING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, false, true);
			return which == kMelder_string::CONTAINS_WORD_STARTING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_WORD_ENDING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_WORD_ENDING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, false, caseSensitive, true, false);
			return which == kMelder_string::CONTAINS_WORD_ENDING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK:
		case kMelder_string::DOES_NOT_CONTAIN_INK:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, false, false);
			return which == kMelder_string::CONTAINS_INK ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK_STARTING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_INK_STARTING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, false, true);
			return which == kMelder_string::CONTAINS_INK_STARTING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::CONTAINS_INK_ENDING_WITH:
		case kMelder_string::DOES_NOT_CONTAIN_INK_ENDING_WITH:
		{
			bool doesMatch = !! str32str_word_optionallyCaseSensitive (value, criterion, true, caseSensitive, true, false);
			return which == kMelder_string::CONTAINS_INK_ENDING_WITH ? doesMatch : ! doesMatch;
		}
		case kMelder_string::MATCH_REGEXP:
		{
			char32 *place = nullptr;
			regexp *compiled_regexp = CompileRE_throwable (criterion, ! REDFLT_CASE_INSENSITIVE);
			if (ExecRE (compiled_regexp, nullptr, value, nullptr, 0, U'\0', U'\0', nullptr, nullptr))
				place = compiled_regexp -> startp [0];
			free (compiled_regexp);
			return !! place;
		}
	}
	//return false;   // should not occur
}

/* End of file melder_search.cpp */
