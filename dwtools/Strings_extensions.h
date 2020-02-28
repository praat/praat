#ifndef _Strings_extensions_h_
#define _Strings_extensions_h_
/* Strings_extensions.h
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
 djmw 20010114
 djmw 20020813 GPL header
 djmw 20040629 Strings_append now accepts an Ordered of Strings.
 djmw 20050714 Permutations
 djmw 20050724 Index
 djmw 20120813 Latest modification
*/

#include "Collection.h"
#include "Strings_.h"
#include "Permutation.h"
#include "Index.h"
#include "Table.h"

autoStrings Strings_createFixedLength (integer numberOfStrings);

autoStrings Strings_createAsCharacters (conststring32 string);

autoStrings Strings_createAsTokens (conststring32 string, conststring32 separators);

integer Strings_findString (Strings me, conststring32 string);

autoStrings Strings_append (OrderedOf<structStrings>* me);

autoStrings Strings_change (Strings me, conststring32 search, conststring32 replace,
	int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp);

autoStrings Strings_createFromSTRVEC (constSTRVEC const& strings, integer from, integer to);

autoStrings Strings_extractPart (Strings me, integer start, integer end);

autoStringsIndex Strings_to_StringsIndex (Strings me);

autoStringsIndex Stringses_to_StringsIndex (Strings me, Strings classes);
/* Construct the index with strings in classes, index[i]=0 when my strings[i] doesn't occur in classes */

autoStringsIndex Table_to_StringsIndex_column (Table me, integer column);

autoStrings StringsIndex_to_Strings (StringsIndex me);

autoPermutation Strings_to_Permutation (Strings me, bool sort);

autoStrings Strings_Permutation_permuteStrings (Strings me, Permutation thee);

#endif /* _Strings_extensions_h_ */
