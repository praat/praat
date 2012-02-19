#ifndef _Strings_extensions_h_
#define _Strings_extensions_h_
/* Strings_extensions.h
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20010114
 djmw 20020813 GPL header
 djmw 20040629 Strings_append now accepts an Ordered of Strings.
 djmw 20050714 Permutations
 djmw 20050724 Index
 djmw 20120128 Latest modification
*/

#include "Collection.h"
#include "Strings.h"
#include "Permutation.h"
#include "Index.h"

#ifdef __cplusplus
	extern "C" {
#endif

Strings Strings_createFixedLength (long numberOfStrings);
long Strings_findString (Strings me, const wchar_t *string);
Strings Strings_append (Collection me);

Strings Strings_change (Strings me, const wchar_t *search, const wchar_t *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches,
	int use_regexp);

void Strings_setString (Strings me, const wchar_t *newstr, long index);

Strings strings_to_Strings (wchar_t **strings, long from, long to);

Strings Strings_extractPart (Strings me, long start, long end);

Strings strings_to_Strings_link (wchar_t** strings, long n);
/* for (i=1; i<= n; i++) my strings[i] = strings[i]; */
void _Strings_unlink (Strings me);
/* for (i=1; i<= my numberOfStrings; i++) my strings[i] = NULL; */

StringsIndex Strings_to_StringsIndex (Strings me);
StringsIndex Stringses_to_StringsIndex (Strings me, Strings classes);
/* Construct the index with strings in classes, index[i]=0 when my strings[i] doesn't occur in classes */


Strings StringsIndex_to_Strings (StringsIndex me);

Permutation Strings_to_Permutation (Strings me, int sort);

Strings Strings_and_Permutation_permuteStrings (Strings me, Permutation thee);

#ifdef __cplusplus
	}
#endif

#endif /* _Strings_extensions_h_ */
