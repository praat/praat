/* Strings_extensions.cpp
 *
 * Copyright (C) 1993-2012 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
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

Strings Strings_createFixedLength (long numberOfStrings) {
	try {
		if (numberOfStrings <= 0) {
			Melder_throw ("The number of strings must be positive.");
		}
		autoStrings me = Thing_new (Strings);
		my strings = NUMvector<wchar_t *> (1, numberOfStrings);
		my numberOfStrings = numberOfStrings;
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings not created.");
	}
}

Strings Strings_createAsCharacters (const wchar_t *string) {
	try {
		autoStrings me = Thing_new (Strings);
		my numberOfStrings = wcslen (string);
		my strings = NUMvector<wchar_t *> (1, my numberOfStrings);
		autoMelderString s;
		for (long i = 1; i <= my numberOfStrings; i++) {
			MelderString_appendCharacter (&s, *string++);
			my strings[i] = Melder_wcsdup (s.string);
			MelderString_empty (&s);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings from characters not created.");
	}
}

Strings Strings_createAsTokens (const wchar_t *string) {
	try {
		autoStrings me = Thing_new (Strings);
		my numberOfStrings =  Melder_countTokens (string);
		my strings = NUMvector<wchar_t *> (1, my numberOfStrings);
		long i = 1;
		for (wchar_t *token = Melder_firstToken (string); token != 0; token = Melder_nextToken ()) {
			my strings[i++] = Melder_wcsdup (token);
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings from characters not created.");
	}
}

long Strings_findString (Strings me, const wchar_t *string) {
	for (long i = 1; i <= my numberOfStrings; i++) {
		if (Melder_wcsequ (my strings[i], string)) {
			return i;
		}
	}
	return 0;
}

Strings Strings_append (Collection me) {
	try {
		long index = 1, numberOfStrings = 0;

		for (long i = 1; i <= my size; i++) {
			Strings s = (Strings) my item[i];
			numberOfStrings += s -> numberOfStrings;
		}

		autoStrings thee = Strings_createFixedLength (numberOfStrings);

		for (long i = 1; i <= my size; i++) {
			Strings s = (Strings) my item[i];
			for (long j = 1; j <= s -> numberOfStrings; j++, index++) {
				if (s -> strings[j] == 0) {
					continue;
				}
				thy strings [index] = Melder_wcsdup (s -> strings[j]);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not appended.");
	}
}

Strings Strings_change (Strings me, const wchar_t *search, const wchar_t *replace, int maximumNumberOfReplaces,
                        long *nmatches, long *nstringmatches, int use_regexp) {
	try {
		autoStrings thee = Thing_new (Strings);
		wchar_t **strings = strs_replace (my strings, 1, my numberOfStrings, search, replace, maximumNumberOfReplaces,
		                                  nmatches, nstringmatches, use_regexp);
		thy numberOfStrings = my numberOfStrings;
		thy strings = strings;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not changed.");
	}
}

Strings strings_to_Strings (wchar_t **strings, long from, long to) {
	try {
		autoStrings thee = Strings_createFixedLength (to - from + 1);
		for (long i = from; i <= to; i++) {
			thy strings[i - from + 1]  = Melder_wcsdup (strings[i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings not created.");
	}
}

Strings Strings_extractPart (Strings me, long from, long to) {
	try {
		if (from < 1 || to > my numberOfStrings || from > to) Melder_throw
			("Strings_extractPart: begin and end must be in interval [1, ", my numberOfStrings, L"].");
		return strings_to_Strings (my strings, from, to);
	} catch (MelderError) {
		Melder_throw (me, ": no part extracted.");
	}
}

Strings strings_to_Strings_link (wchar_t **strings, long n) {
	try {
		autoStrings me = Strings_createFixedLength (n);
		for (long i = 1; i <= n; i++) {
			my strings[i] = strings[i];
		}
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Strings not linked.");
	}
}

void Strings_unlink (Strings me) {
	for (long i = 1; i <= my numberOfStrings; i++) {
		my strings[i] = 0;
	}
}

Permutation Strings_to_Permutation (Strings me, int sort) {
	try {
		autoPermutation thee = Permutation_create (my numberOfStrings);
		if (sort != 0) {
			NUMindexx_s (my strings, my numberOfStrings, thy p);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Permutation created.");
	}
}

Strings Strings_and_Permutation_permuteStrings (Strings me, Permutation thee) {
	try {
		if (my numberOfStrings != thy numberOfElements) Melder_throw ("Strings_and_Permutation_permuteStrings: "
			        "The number of strings and the number of elements in the Permutation must be equal.");
		autoStrings him = Strings_createFixedLength (my numberOfStrings);
		for (long i = 1; i <= thy numberOfElements; i++) {
			long index = thy p[i];
			his strings[i] = Melder_wcsdup (my strings[index]);
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no permuted Strings created.");
	}
}

StringsIndex Stringses_to_StringsIndex (Strings me, Strings classes) {
	try {
		autoStringsIndex tmp = Strings_to_StringsIndex (classes);
		long numberOfClasses = tmp -> classes -> size;

		autoStringsIndex him = StringsIndex_create (my numberOfStrings);
		for (long i = 1; i <= numberOfClasses; i++) {
			SimpleString t = (SimpleString) tmp -> classes -> item[i];
			autoSimpleString t2 = Data_copy (t);
			Collection_addItem (his classes, t2.transfer());
		}
		for (long j = 1; j <= my numberOfStrings; j++) {
			long index = 0;
			wchar_t *stringsj = my strings[j];
			for (long i = 1; i <= numberOfClasses; i++) {
				SimpleString ss = (SimpleString) his classes -> item[i];
				if (Melder_wcscmp (stringsj, ss -> string) == 0) {
					index = i;
					break;
				}
			}
			his classIndex[j] = index;
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no StringsIndex created.");
	}
}

StringsIndex Strings_to_StringsIndex (Strings me) {
	try {
		autoStringsIndex thee = StringsIndex_create (my numberOfStrings);
		autoPermutation sorted = Strings_to_Permutation (me, 1);
		long numberOfClasses = 0;
		wchar_t *strings = NULL;
		for (long i = 1; i <= sorted -> numberOfElements; i++) {
			long index = sorted -> p[i];
			wchar_t *stringsi = my strings[index];
			if (i == 1 || Melder_wcscmp (strings, stringsi) != 0) {
				numberOfClasses++;
				autoSimpleString him = SimpleString_create (stringsi);
				Collection_addItem (thy classes, him.transfer());
				strings = stringsi;
			}
			thy classIndex[index] = numberOfClasses;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no StringsIndex created.");
	}
}

Strings StringsIndex_to_Strings (StringsIndex me) {
	try {
		autoStrings thee = Strings_createFixedLength (my numberOfElements);
		for (long i = 1; i <= thy numberOfStrings; i++) {
			SimpleString s = (SimpleString) my classes -> item[my classIndex[i]];
			thy strings[i] = Melder_wcsdup (s -> string);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Strings created.");
	}
}

StringsIndex Table_to_StringsIndex_column (Table me, long column) {
	try {
		if (column < 1 || column > my numberOfColumns) {
			Melder_throw ("Invalid column number.");
		}
		long numberOfRows = my rows -> size;
		Table_numericize_Assert (me, column);
		autoNUMvector<wchar_t *> groupLabels (1, numberOfRows);
		for (long irow = 1; irow <= numberOfRows; irow++) {
			groupLabels[irow] = ((TableRow) my rows -> item [irow]) -> cells [column] .string;
		}
		autoStrings thee = strings_to_Strings (groupLabels.peek(), 1, numberOfRows);
		autoStringsIndex him = Strings_to_StringsIndex (thee.peek());
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, "no StringsIndex created from column ", Melder_integer (column));
	}
}

/* End of file Strings_extensions.cpp */
