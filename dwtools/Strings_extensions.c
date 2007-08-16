/* Strings_extensions.c
 *
 * Copyright (C) 1993-2004 David Weenink
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
*/

#include "Strings_extensions.h"
#include "NUM2.h"

Strings Strings_createFixedLength (long numberOfStrings)
{
	Strings me = new (Strings);
	if (me == NULL || numberOfStrings <= 0) return NULL;
	my numberOfStrings = numberOfStrings;
	my strings = NUMpvector (1, numberOfStrings);
	if (my strings == NULL) forget (me);
	else my numberOfStrings = numberOfStrings;
	return me;
}

Strings Strings_append (Ordered me)
{
	long i, j, index = 1, numberOfStrings = 0;
	Strings thee, s;
	
	for (i = 1; i <= my size; i++)
	{
		s = my item[i];
		numberOfStrings += s -> numberOfStrings;
	}
	
	thee = Strings_createFixedLength (numberOfStrings);
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my size; i++)
	{
		s = my item[i];
		for (j = 1; j <= s -> numberOfStrings; j++, index++)
		{
			if (s -> strings[j] == NULL) continue;
			thy strings [index] = Melder_wcsdup (s -> strings[j]);
			if (thy strings[index] == NULL) goto end;
		}
	}
	
end:

	if (Melder_hasError ()) forget (thee);
	return thee;
}

/*Strings Strings_append (Strings me, Strings thee)
{
	long i, k, numberOfStrings = my numberOfStrings + thy numberOfStrings;
	Strings him = Strings_createFixedLength (numberOfStrings);
	
	if (him == NULL) return NULL;
	for (i = 1; i <= my numberOfStrings; i++)
	{
		if (my strings[i] == NULL) continue;
		his strings [i] = Melder_strdup (my strings[i]);
		if (his strings[i] == NULL) goto end;
	}
	for (i = 1; i <= thy numberOfStrings; i++)
	{
		if (thy strings[i] == NULL) continue;
		k = my numberOfStrings + i;
		his strings [k] = Melder_strdup (thy strings[i]);
		if (his strings[k] == NULL) goto end;
	}
end:
	if (Melder_hasError ()) forget (him);
	return him;	
}*/

Strings Strings_change (Strings me, wchar_t *search, wchar_t *replace, 
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, 
	int use_regexp)
{
	wchar_t **strings;
	Strings thee = new (Strings);
	if (thee == NULL) return NULL;
	
	strings = strs_replace (my strings, 1, my numberOfStrings, 
		search, replace, maximumNumberOfReplaces, nmatches, 
		nstringmatches, use_regexp);
		
	if (strings == NULL)
	{
		forget (thee);
		return NULL;
	}
	thy numberOfStrings = my numberOfStrings;
	thy strings = strings;
	return thee;
}

int Strings_setString (Strings me, wchar_t *new, long index)
{
	wchar_t *s;
	if (index < 1 || index > my numberOfStrings) return Melder_error 
		("Strings_setString: index must be in range [1, %d].", my numberOfStrings);

	s = Melder_wcsdup (new);
	if (my strings[index]) Melder_free (my strings[index]);
	my strings[index] = s;
	return 1;
}

Strings strings_to_Strings (wchar_t **strings, long from, long to)
{
	Strings thee;
	long i, k;
	
	thee = Strings_createFixedLength (to - from + 1);
	if (thee == NULL) goto end;
	
	for (i = from, k = 1; i <= to; i++, k++)
	{
		if (strings[i] && 
			((thy strings[k]  = Melder_wcsdup (strings[i])) == NULL)) goto end;
	}
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Strings Strings_extractPart (Strings me, long from, long to)
{
	if (from < 1 || to > my numberOfStrings || from > to) return Melder_errorp 
		("Strings_extractPart: begin and end must be in interval [1, %d].", my numberOfStrings);
	return strings_to_Strings (my strings, from, to);
}

Strings strings_to_Strings_link (wchar_t** strings, long n)
{
	long i;
	Strings me = Strings_createFixedLength (n);
	if (me == NULL) return NULL;
	for (i = 1; i <= n; i++)
	{
		my strings[i] = strings[i];
	}
	return me;
}

void _Strings_unlink (Strings me)
{
	my numberOfStrings = 0;
}

Permutation Strings_to_Permutation (Strings me, int sort)
{
	Permutation thee;
		
	thee = Permutation_create (my numberOfStrings);
	if (thee != NULL && sort != 0)
	{
		NUMindexx_s (my strings, my numberOfStrings, thy p);
	}
	return thee;
}

Strings Strings_and_Permutation_permuteStrings (Strings me, Permutation thee)
{
	long i;
	Strings him = NULL;
	if (my numberOfStrings != thy numberOfElements) return Melder_errorp ("Strings_and_Permutation_permuteStrings: "
		"The number of strings and the number of elements in the Permutation must be equal.");

	him = Strings_createFixedLength (my numberOfStrings);
	if (him == NULL) return NULL;
	for (i = 1; i <= thy numberOfElements; i++)
	{
		long index = thy p[i];
		if (my strings[index] != NULL &&
			(his strings[i] = Melder_wcsdup (my strings[index])) == NULL) break;
	}
	
	if (Melder_hasError ()) forget (him);
	return him;
	
}

StringsIndex Strings_to_StringsIndex (Strings me)
{
	Permutation sorted = NULL;
	StringsIndex thee = NULL;
	SimpleString him;
	wchar_t *strings = NULL;
	long i, numberOfClasses = 0;
		
	thee = StringsIndex_create (my numberOfStrings);
	if (thee == NULL) return NULL;
	
	sorted = Strings_to_Permutation (me, 1);
	if (sorted == NULL) goto end;

	for (i = 1; i <= sorted -> numberOfElements; i++)
	{
		long index = sorted -> p[i];
		wchar_t *stringsi = my strings[index];
		if (i == 1 || NUMwcscmp (strings, stringsi) != 0)
		{
			numberOfClasses++;
			if ((him = SimpleString_create (stringsi)) == NULL) goto end;
			if (! Collection_addItem (thy classes, him))
			{
				forget (him); goto end;
			}
			strings = stringsi;
		}
		thy classIndex[index] = numberOfClasses;
	}
	
end:
	forget (sorted);
	if (Melder_hasError ()) forget (thee);
	return thee;
}

Strings StringsIndex_to_Strings (StringsIndex me)
{
	Strings thee = Strings_createFixedLength (my numberOfElements);
	long i;

	if (thee == NULL) return NULL;
	for (i = 1; i <= thy numberOfStrings; i++)
	{
		SimpleString s = my classes -> item[my classIndex[i]];
		thy strings[i] = Melder_wcsdup (s -> string);
		if (thy strings[i] == NULL) break;
	}
	if (Melder_hasError ()) forget (thee);
	return thee;
}
/* End of file Strings_extensions.c */
