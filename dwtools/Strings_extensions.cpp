/* Strings_extensions.cpp
 *
 * Copyright (C) 1993-2020 David Weenink
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
*/

#include "Strings_extensions.h"
#include "NUM2.h"

autoStrings Strings_createFixedLength (integer numberOfStrings) {
	try {
		Melder_require (numberOfStrings > 0,
			U"The number of strings should be positive.");
		autoStrings me = Thing_new (Strings);
		my strings = autoSTRVEC (numberOfStrings);
		my numberOfStrings = numberOfStrings;
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings not created.");
	}
}

autoStrings Strings_createAsCharacters (conststring32 string) {
	try {
		autoStrings me = Thing_new (Strings);
		my numberOfStrings = str32len (string);
		my strings = autoSTRVEC (my numberOfStrings);
		for (integer i = 1; i <= my numberOfStrings; i ++)
			my strings [i] = Melder_dup (Melder_character (*string ++));
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings from characters not created.");
	}
}
	
autoStrings Strings_createAsTokens (conststring32 token_string, conststring32 separator_string) {
	try {
		autoStrings me = Thing_new (Strings);
		/*
			1. make a copy
			2. replace all separators by 0 in the copy
			3. count the items in the copy
			4. copy the tokens from the copy to the Strings object

			The algorithm is not the most efficient one since the token string is processed 4 times.
			However the steps taken are easy to follow.
		 */
		if (! token_string || token_string [0] == U'\0')
			return me;

		const conststring32 separators = ( separator_string && separator_string [0] != U'\0' ? separator_string : U" " );
		autostring32 copy = Melder_dup (token_string);
		mutablestring32 tokens = copy.get();
		char32 *index = & tokens [0];
		integer numberOfTokens = 0;
		for (const char32 *indexs = & token_string [0]; *indexs != U'\0'; indexs ++, index ++) {
			for (const char32 *s = & separators [0]; *s != U'\0'; s ++) {
				if (*index == *s) {
					*index = U'\0';
					if (index > tokens && *(index - 1) != U'\0')
						numberOfTokens ++;
					break;
				}
			}
		}
		if (*(index - 1) != U'\0') // if token_string ends with a non-separator
			numberOfTokens ++;
		my numberOfStrings = numberOfTokens;
		my strings = autoSTRVEC (my numberOfStrings);
		numberOfTokens = 0;
		char32 *start = tokens;
		index = & tokens [0];
		for (const char32 *indexs = & token_string [0]; *indexs != U'\0'; indexs ++, index ++) {
			if (*index == U'\0' && index > tokens && *(index - 1) != U'\0')
				my strings [++ numberOfTokens] = Melder_dup (start);
			if (*index != U'\0' && index > tokens && *(index - 1) == U'\0')
				start = index;
		}
		if (*(index - 1) != U'\0')
			my strings [++ numberOfTokens] = Melder_dup (start);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Strings as tokens not created.");
	}
}

integer Strings_findString (Strings me, conststring32 string) {
	for (integer i = 1; i <= my numberOfStrings; i ++)
		if (Melder_equ (my strings [i].get(), string))
			return i;
	return 0;
}

autoStrings Strings_append (OrderedOf<structStrings>* me) {
	try {
		integer index = 1, numberOfStrings = 0;
		for (integer i = 1; i <= my size; i ++) {
			const Strings s = my at [i];
			numberOfStrings += s -> numberOfStrings;
		}
		autoStrings thee = Strings_createFixedLength (numberOfStrings);
		for (integer i = 1; i <= my size; i ++) {
			const Strings s = my at [i];
			for (integer j = 1; j <= s -> numberOfStrings; j ++, index ++)
				thy strings [index] = Melder_dup (s -> strings [j].get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not appended.");
	}
}

autoStrings Strings_change (Strings me, conststring32 search, conststring32 replace, int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp) {
	try {
		autoStrings thee = Thing_new (Strings);
		autoSTRVEC strings = string32vector_searchAndReplace (my strings.get(),
				search, replace, maximumNumberOfReplaces, nmatches, nstringmatches, use_regexp);
		thy numberOfStrings = my numberOfStrings;
		thy strings = std::move (strings);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not changed.");
	}
}

autoStrings Strings_createFromSTRVEC (constSTRVEC const& strings, integer from, integer to) {
	try {
		autoStrings thee = Strings_createFixedLength (to - from + 1);
		for (integer i = from; i <= to; i ++)
			thy strings [i - from + 1] = Melder_dup (strings [i]);

		return thee;
	} catch (MelderError) {
		Melder_throw (U"Strings not created.");
	}
}

autoStrings Strings_extractPart (Strings me, integer from, integer to) {
	try {
		Melder_require (from > 0 && from <= to && to <= my numberOfStrings,
			U"Strings_extractPart: begin and end should be in interval [1, ", my numberOfStrings, U"].");
		return Strings_createFromSTRVEC (my strings.get(), from, to);
	} catch (MelderError) {
		Melder_throw (me, U": no part extracted.");
	}
}

autoPermutation Strings_to_Permutation (Strings me, bool sort) {
	try {
		autoPermutation thee = Permutation_create (my numberOfStrings);
		if (sort)
			INTVECindex (thy p.get(), my strings.get());
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
			const integer index = thy p [i];
			his strings [i] = Melder_dup (my strings [index].get());
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no permuted Strings created.");
	}
}

autoStringsIndex Stringses_to_StringsIndex (Strings me, Strings classes) {
	try {
		autoStringsIndex tmp = Strings_to_StringsIndex (classes);
		const integer numberOfClasses = tmp -> classes->size;

		autoStringsIndex him = StringsIndex_create (my numberOfStrings);
		for (integer i = 1; i <= numberOfClasses; i ++) {
			const SimpleString t = (SimpleString) tmp -> classes->at [i];   // FIXME cast
			autoSimpleString t2 = Data_copy (t);
			his classes -> addItem_move (t2.move());
		}
		for (integer j = 1; j <= my numberOfStrings; j ++) {
			integer index = 0;
			const conststring32 stringsj = my strings [j].get();
			for (integer i = 1; i <= numberOfClasses; i ++) {
				SimpleString ss = (SimpleString) his classes->at [i];   // FIXME cast
				if (Melder_equ (stringsj, ss -> string.get())) {
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
		autoPermutation sorted = Strings_to_Permutation (me, true);
		integer numberOfClasses = 0;
		conststring32 strings = nullptr;
		for (integer i = 1; i <= sorted -> numberOfElements; i ++) {
			const integer index = sorted -> p [i];
			const conststring32 stringsi = my strings [index].get();
			if (i == 1 || ! Melder_equ (strings, stringsi)) {
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
			const SimpleString s = (SimpleString) my classes->at [my classIndex [i]];   // FIXME cast, FIXME classIndex
			thy strings [i] = Melder_dup (s -> string.get());
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Strings created.");
	}
}

autoStringsIndex Table_to_StringsIndex_column (Table me, integer column) {
	try {
		Melder_require (column > 0 && column <= my numberOfColumns,
			U"Invalid column number.");
		const integer numberOfRows = my rows.size;
		Table_numericize_Assert (me, column);
		autoSTRVEC groupLabels (numberOfRows);
		for (integer irow = 1; irow <= numberOfRows; irow ++)
			groupLabels [irow] = Melder_dup(my rows.at [irow] -> cells [column]. string.get()); //TODO no dup

		autoStrings thee = Strings_createFromSTRVEC (groupLabels.get(), 1, numberOfRows);
		autoStringsIndex him = Strings_to_StringsIndex (thee.get());
		return him;
	} catch (MelderError) {
		Melder_throw (me, U"No StringsIndex created from column ", column, U".");
	}
}

/* End of file Strings_extensions.cpp */
