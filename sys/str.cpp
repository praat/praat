/* str.cpp
 *
 * Copyright (C) 2012-2017 David Weenink, 2008,2018 Paul Boersma
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

autostring32 leftStr (conststring32 str, integer newLength) {
	integer length = str32len (str);
	if (newLength < 0)
		newLength = 0;
	if (newLength > length)
		newLength = length;
	autostring32 result (newLength);
	str32ncpy (result.get(), str, newLength);
	return result;
}

autostring32 rightStr (conststring32 str, integer newLength) {
	integer length = str32len (str);
	if (newLength < 0)
		newLength = 0;
	if (newLength > length)
		newLength = length;
	return Melder_dup (str + length - newLength);
}

autostring32 midStr (conststring32 str, integer startingPosition_1, integer numberOfCharacters) {
	integer length = str32len (str), endPosition_1 = startingPosition_1 + numberOfCharacters - 1;
	if (startingPosition_1 < 1)
		startingPosition_1 = 1;
	if (endPosition_1 > length)
		endPosition_1 = length;
	integer newLength = endPosition_1 - startingPosition_1 + 1;
	if (newLength <= 0)
		return Melder_dup (U"");
	autostring32 result (newLength);
	str32ncpy (result.get(), & str [startingPosition_1-1], newLength);
	return result;
}

autostring32 replaceStr (conststring32 string,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches)
{
	if (string == 0 || search == 0 || replace == 0)
		return autostring32();

	integer len_string = str32len (string);
	if (len_string == 0)
		maximumNumberOfReplaces = 1;

	integer len_search = str32len (search);
	if (len_search == 0)
		maximumNumberOfReplaces = 1;

	/*
		To allocate memory for 'result' only once, we have to know how many
		matches will occur.
	*/

	const char32 *pos = & string [0];   // current position / start of current match
	*nmatches = 0;
	if (maximumNumberOfReplaces <= 0)
		maximumNumberOfReplaces = INTEGER_MAX;

	if (len_search == 0) {   /* Search is empty string... */
		if (len_string == 0)
			*nmatches = 1;   /* ...only matches empty string */
	} else {
		if (len_string != 0) {   /* Because empty string always matches */
			while (!! (pos = str32str (pos, search)) && *nmatches < maximumNumberOfReplaces) {
				pos += len_search;
				(*nmatches) ++;
			}
		}
	}

	integer len_replace = str32len (replace);
	integer len_result = len_string + *nmatches * (len_replace - len_search);
	autostring32 result (len_result);

	const char32 *posp = pos = & string [0];
	integer nchar = 0, result_nchar = 0;
	for (integer i = 1; i <= *nmatches; i ++) {
		pos = str32str (pos, search);

		/*
			Copy gap between end of previous match and start of current.
		*/
		nchar = pos - posp;
		if (nchar > 0) {
			str32ncpy (& result [result_nchar], posp, nchar);
			result_nchar += nchar;
		}

		/*
			Insert the replace string in result.
		*/
		str32ncpy (& result [result_nchar], replace, len_replace);
		result_nchar += len_replace;

		/*
			Next search starts after the match.
		*/
		pos += len_search;
		posp = pos;
	}

	/*
		Copy gap between end of match and end of string.
	*/
	pos = string + len_string;
	nchar = pos - posp;
	if (nchar > 0)
		str32ncpy (& result [result_nchar], posp, nchar);
	return result;
}

autostring32 replace_regexStr (conststring32 string,
	regexp *compiledSearchRE, conststring32 replaceRE, integer maximumNumberOfReplaces,
	integer *nmatches)
{
	integer buf_nchar = 0;   // number of characters in 'buf'
	integer gap_copied = 0;
	integer nchar;
	bool reverse = false;
	int errorType;
	char32 prev_char = U'\0';
	const char32 *pos;   // current position in 'string' / start of current match
	const char32 *posp;   // end of previous match
	autostring32 buf;

	*nmatches = 0;
	if (string == 0 || compiledSearchRE == 0 || replaceRE == 0)
		return 0;

	integer string_length = str32len (string);
	//int replace_length = str32len (replaceRE);
	if (string_length == 0)
		maximumNumberOfReplaces = 1;

	integer i = maximumNumberOfReplaces > 0 ? 0 : - string_length;

	/*
		We do not know the size of the replaced string in advance,
		therefore we allocate a replace buffer twice the size of the
		original string. After all replaces have taken place we do a
		final realloc to the then exactly known size.
		If during the replace, the size of the buffer happens to be too
		small (this is signalled by the replaceRE function),
		we double its size and restart the replace.
	*/

	integer bufferLength = 2 * string_length;
	bufferLength = bufferLength < 100 ? 100 : bufferLength;
	buf.resize (bufferLength);

	pos = posp = string;
	while (ExecRE (compiledSearchRE, nullptr, pos, nullptr, reverse, prev_char, U'\0', nullptr, nullptr) &&
			i ++ < maximumNumberOfReplaces) {
		/*
			Copy gap between the end of the previous match and the start
			of the current match.
			Check buffer overflow. pos == posp ? '\0' : pos [-1],
		*/
		pos = compiledSearchRE -> startp [0];
		nchar = pos - posp;
		if (nchar > 0 && ! gap_copied) {
			if (buf_nchar + nchar > bufferLength) {
				bufferLength *= 2;
				buf.resize (bufferLength);
			}
			str32ncpy (buf.get() + buf_nchar, posp, nchar);
			buf_nchar += nchar;
		}

		gap_copied = 1;

		/*
			Do the substitution. We can only check afterwards for buffer overflow.
			SubstituteRE puts null byte at last replaced position and signals when overflow.
		*/
		if (! SubstituteRE (compiledSearchRE, replaceRE, buf.get() + buf_nchar, bufferLength + 1 - buf_nchar, & errorType)) {
			if (errorType == 1) {   // not enough memory
				bufferLength *= 2;
				buf.resize (bufferLength);
				Melder_clearError ();
				i --;   // retry
				continue;
			}
			Melder_throw (U"Error during substitution.");
		}

		// Buffer is not full, get number of characters added;

		nchar = str32len (buf.get() + buf_nchar);
		buf_nchar += nchar;

		// Update next start position in search string.

		posp = pos;
		pos = (char32 *) compiledSearchRE -> endp [0];
		if (pos != posp)
			prev_char = pos [-1];
		gap_copied = 0;
		posp = pos; //pb 20080121
		(*nmatches) ++;
		// at end of string?
		// we need this because .* matches at the end of a string
		if (pos - string == string_length)
			break;
	}

	// Copy last part of string to destination string

	nchar = (string + string_length) - pos;
	bufferLength = buf_nchar + nchar;
	buf.resize (bufferLength);
	str32ncpy (buf.get() + buf_nchar, pos, nchar);
	return buf;
}

/* End of file str.cpp */
