/* SpellingChecker_def.h
 *
 * Copyright (C) 1999-2007,2011,2015,2016,2018-2020 Paul Boersma
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


#define ooSTRUCT SpellingChecker
oo_DEFINE_CLASS (SpellingChecker, Daata)           // CGN:

	oo_STRING (forbiddenStrings)              // : ; "
	oo_BOOLEAN (checkMatchingParentheses)     // true
	oo_STRING (separatingCharacters)          // .,:;()!?
	oo_BOOLEAN (allowAllParenthesized)        // true
	oo_BOOLEAN (allowAllNames)                // true
	oo_STRING (namePrefixes)                  // 's- d' l'
	oo_BOOLEAN (allowAllAbbreviations)        // true
	oo_BOOLEAN (allowCapsSentenceInitially)   // false
	oo_BOOLEAN (allowCapsAfterColon)          // false
	oo_STRING (allowAllWordsContaining)       // * xxx
	oo_STRING (allowAllWordsStartingWith)
	oo_STRING (allowAllWordsEndingIn)         // -
	#if oo_READING
		oo_VERSION_UNTIL (1)
			oo_OBJECT (WordList, 0, wordList)
		oo_VERSION_ELSE
			oo_OBJECT (WordList, 1, wordList)
		oo_VERSION_END
	#else
		oo_OBJECT (WordList, 1, wordList)
	#endif
	oo_COLLECTION (StringSet, userDictionary, SimpleString, 0)

	#if oo_READING
	{
		autoStrings strings = WordList_to_Strings (our wordList.get());
		oo_VERSION_UNTIL (1)
			Strings_nativize (strings.get());
		oo_VERSION_END
		our wordList = Strings_to_WordList (strings.get());
	}
	#endif

oo_END_CLASS (SpellingChecker)
#undef ooSTRUCT


/* End of file SpellingChecker_def.h */
