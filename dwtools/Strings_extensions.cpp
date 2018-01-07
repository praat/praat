/* Strings_extensions.cpp
 *
 * Copyright (C) 1993-2017 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 20011003
 djmw 20020813 GPL header
 djmw 20030107 Added Strings_setString
 djmw 20031212 Added Strings_extractPart
 djmw 20040301 Added Strings_createFixedLength.
 djmw 20040308 Corrected bug in strings_to_Strings.
 djmw 20040427 Strings_append added.
 djmw 20040629 Strings_append  now accepts an Ordered of Strings.
 djmw 20050714 New: Strings_to_Permutation, Strings_and_Permutation_permuteStrings.
 djmw 20050721 Extra argument in Strings_to_Permutation.
 djmw 20101007 StringsIndex Stringses_to_StringsIndex (Strings me, Strings classes)
 djmw 20120407 + Strings_createFromCharacters
 djmw 20120813 -Strings_setString
*/

#include "Strings_extensions.h"
#include "NUM2.h"

autoStrings Strings_createFixedLength (integer numberOfStrings) {
	try {
		Melder_require (numberOfStrings > 0, U"The number of strings should be positive.");
		
		autoStrings me = Thing_new (Strings);
		my strings = NUMvector<char32 *> (1, numberOfStrings);
		my numberOfStrings = numberOfStrings;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings not created.");
	}
}

autoStrings Strings_createAsCharacters (const char32 *string) {
	try {
		autoStrings me = Thing_new (Strings);
		my numberOfStrings = str32len (string);
		my strings = NUMvector<char32 *> (1, my numberOfStrings);
		for (integer i = 1; i <= my numberOfStrings; i ++) {
			my strings [i] = Melder_dup (Melder_character (*string ++));
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings from characters not created.");
	}
}
	
autoStrings Strings_createAsTokens (const char32 *token_string, const char32 *separator_string) {	
	try {
		autoStrings me = Thing_new (Strings);
		/*
		 * 1. make a copy
		 * 2. replace all separators by 0 in the copy
		 * 3. count the items in the copy
		 * 4. copy the tokens from the copy to the Strings object
		 * 
		 * The algorithm is not the most efficient one since the token string is processed 4 times.
		 * However the steps taken are easy to follow.
		 */
		
		if (token_string == nullptr || str32len (token_string) == 0) {
			return me;
		}
		const char32 *separators = (separator_string == nullptr || str32len (separator_string) == 0) ? U" " : separator_string;
		autostring32 copy = Melder_dup (token_string);
		char32 *index, *tokens = copy.peek();
		const char32 *indexs;
		integer numberOfTokens = 0;
		for (index = tokens, indexs = token_string; *indexs != U'\0'; indexs ++, index ++) {
			for (const char32 *s = separators; *s != U'\0'; s ++) {
				if (*index == *s) {
					*index = U'\0';
					if (index > tokens && *(index - 1) != U'\0') {
						numberOfTokens ++;
					}
					break;
				}
			}
		}
		if (*(index - 1) != U'\0') { // if token_string ends with a non-separator
			numberOfTokens ++;
		}
		my numberOfStrings = numberOfTokens;
		my strings = NUMvector<char32 *> (1, my numberOfStrings);
		numberOfTokens = 0;
		char32 *start = tokens;
		for (index = tokens, indexs = token_string; *indexs != U'\0'; indexs ++, index ++) {
			if (*index == U'\0' && index > tokens && *(index - 1) != U'\0') {
				my strings [ ++ numberOfTokens] = Melder_dup (start);
			}
			if (*index != U'\0' && index > tokens && *(index - 1) == U'\0') {
				start = index;
			}
		}
		if (*(index - 1) != U'\0') {
			my strings [ ++ numberOfTokens] = Melder_dup (start);
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings as tokens not created.");
	}
}

integer Strings_findString (Strings me, const char32 *string) {
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		if (Melder_equ (my strings [i], string)) {
			return i;
		}
	}
	return 0;
}

autoStrings Strings_append (OrderedOf<structStrings>* me) {
	try {
		integer index = 1, numberOfStrings = 0;

		for (integer i = 1; i <= my size; i ++) {
			Strings s = my at [i];
			numberOfStrings += s -> numberOfStrings;
		}

		autoStrings thee = Strings_createFixedLength (numberOfStrings);

		for (integer i = 1; i <= my size; i ++) {
			Strings s = my at [i];
			for (integer j = 1; j <= s -> numberOfStrings; j ++, index ++) {
				thy strings [index] = Melder_dup (s -> strings [j]);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not appended.");
	}
}

autoStrings Strings_change (Strings me, const char32 *search, const char32 *replace, int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp) {
	try {
		autoStrings thee = Thing_new (Strings);
		char32 **strings = strs_replace (my strings, 1, my numberOfStrings, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp);
		thy numberOfStrings = my numberOfStrings;
		thy strings = strings;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not changed.");
	}
}

autoStrings strings_to_Strings (char32 **strings, integer from, integer to) {
	try {
		autoStrings thee = Strings_createFixedLength (to - from + 1);
		for (integer i = from; i <= to; i ++) {
			thy strings [i - from + 1]  = Melder_dup (strings [i]);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Strings not created.");
	}
}

autoStrings Strings_extractPart (Strings me, integer from, integer to) {
	try {
		Melder_require (from > 0 && from <= to && to <= my numberOfStrings,
			U"Strings_extractPart: begin and end should be in interval [1, ", my numberOfStrings, U"].");
		return strings_to_Strings (my strings, from, to);
	} catch (MelderError) {
		Melder_throw (me, U": no part extracted.");
	}
}

autoStrings strings_to_Strings_link (char32 **strings, integer n) {
	try {
		autoStrings me = Strings_createFixedLength (n);
		for (integer i = 1; i <= n; i ++) {
			my strings [i] = strings [i];
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings not linked.");
	}
}

void Strings_unlink (Strings me) {
	for (integer i = 1; i <= my numberOfStrings; i ++) {
		my strings [i] = nullptr;
	}
}

autoPermutation Strings_to_Permutation (Strings me, int sort) {
	try {
		autoPermutation thee = Permutation_create (my numberOfStrings);
		if (sort != 0) {
			NUMindexx_s (my strings, my numberOfStrings, thy p);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Permutation created.");
	}
}

autoStrings Strings_Permutation_permuteStrings (Strings me, Permutation thee) {
	try {
		Melder_require (my numberOfStrings == thy numberOfElements,
			U"Strings_Permutation_permuteStrings: The number of strings and the number of elements in the Permutation should be equal.");
		autoStrings him = Strings_createFixedLength (my numberOfStrings);
		for (integer i = 1; i <= thy numberOfElements; i ++) {
			integer index = thy p [i];
			his strings [i] = Melder_dup (my strings [index]);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no permuted Strings created.");
	}
}

autoStringsIndex Stringses_to_StringsIndex (Strings me, Strings classes) {
	try {
		autoStringsIndex tmp = Strings_to_StringsIndex (classes);
		integer numberOfClasses = tmp -> classes->size;

		autoStringsIndex him = StringsIndex_create (my numberOfStrings);
		for (integer i = 1; i <= numberOfClasses; i ++) {
			SimpleString t = (SimpleString) tmp -> classes->at [i];   // FIXME cast
			autoSimpleString t2 = Data_copy (t);
			his classes -> addItem_move (t2.move());
		}
		for (integer j = 1; j <= my numberOfStrings; j ++) {
			integer index = 0;
			char32 *stringsj = my strings [j];
			for (integer i = 1; i <= numberOfClasses; i ++) {
				SimpleString ss = (SimpleString) his classes->at [i];   // FIXME cast
				if (Melder_equ (stringsj, ss -> string)) {
					index = i;
					break;
				}
			}
			his classIndex [j] = index;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoStringsIndex Strings_to_StringsIndex (Strings me) {
	try {
		autoStringsIndex thee = StringsIndex_create (my numberOfStrings);
		autoPermutation sorted = Strings_to_Permutation (me, 1);
		integer numberOfClasses = 0;
		char32 *strings = nullptr;
		for (integer i = 1; i <= sorted -> numberOfElements; i ++) {
			integer index = sorted -> p [i];
			char32 *stringsi = my strings [index];
			if (i == 1 || Melder_cmp (strings, stringsi) != 0) {
				numberOfClasses ++;
				autoSimpleString him = SimpleString_create (stringsi);
				thy classes -> addItem_move (him.move());
				strings = stringsi;
			}
			thy classIndex [index] = numberOfClasses;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no StringsIndex created.");
	}
}

autoStrings StringsIndex_to_Strings (StringsIndex me) {
	try {
		autoStrings thee = Strings_createFixedLength (my numberOfItems);
		for (integer i = 1; i <= thy numberOfStrings; i ++) {
			SimpleString s = (SimpleString) my classes->at [my classIndex [i]];   // FIXME cast, FIXME classIndex
			thy strings [i] = Melder_dup (s -> string);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}

autoStringsIndex Table_to_StringsIndex_column (Table me, integer column) {
	try {
		Melder_require (column > 0 && column <= my numberOfColumns, U"Invalid column number.");

		integer numberOfRows = my rows.size;
		Table_numericize_Assert (me, column);
		autoNUMvector<char32 *> groupLabels (1, numberOfRows);
		for (integer irow = 1; irow <= numberOfRows; irow ++) {
			groupLabels [irow] = my rows.at [irow] -> cells [column] .string;
		}
		autoStrings thee = strings_to_Strings (groupLabels.peek(), 1, numberOfRows);
		autoStringsIndex him = Strings_to_StringsIndex (thee.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"No StringsIndex created from column ", column, U".");
	}
}

/* End of file Strings_extensions.cpp */
