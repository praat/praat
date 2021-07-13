/* NUMstring.cpp
 *
 * Copyright (C) 2012,2013,2015-2020 David Weenink, 2015-2021 Paul Boersma
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
 djmw 20121005 First version
*/

#include "Interpreter.h"
#include "NUM2.h"

char32 *strstr_regexp (conststring32 string, conststring32 search_regexp) {
	char32 *charp = nullptr;
	regexp *compiled_regexp = CompileRE_throwable (search_regexp, 0);
	if (ExecRE (compiled_regexp, nullptr, string, nullptr, false, U'\0', U'\0', nullptr, nullptr))
		charp = compiled_regexp -> startp [0];
	free (compiled_regexp);
	return charp;
}

static autoSTRVEC string32vector_searchAndReplace_literal (constSTRVEC me,
	conststring32 search, conststring32 replace, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	/*
		Sanitize input.
	*/
	if (! search)
		search = U"";
	if (! replace)
		replace = U"";

	autoSTRVEC result (me.size);

	integer nmatches_sub = 0, nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		result [i] = replace_STR (me [i], search, replace, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

static autoSTRVEC string32vector_searchAndReplace_regexp (constSTRVEC me,
	conststring32 searchRE, conststring32 replaceRE, int maximumNumberOfReplaces,
	integer *out_numberOfMatches, integer *out_numberOfStringMatches)
{
	/*
		Sanitize input.
	*/
	if (! searchRE)
		searchRE = U"";
	if (! replaceRE)
		replaceRE = U"";

	integer nmatches_sub = 0;

	regexp *compiledRE = CompileRE_throwable (searchRE, 0);

	autoSTRVEC result (me.size);

	integer nmatches = 0, nstringmatches = 0;
	for (integer i = 1; i <= me.size; i ++) {
		result [i] = replace_regex_STR (me [i], compiledRE, replaceRE, maximumNumberOfReplaces, & nmatches_sub);
		if (nmatches_sub > 0) {
			nmatches += nmatches_sub;
			nstringmatches ++;
		}
	}
	if (out_numberOfMatches)
		*out_numberOfMatches = nmatches;
	if (out_numberOfStringMatches)
		*out_numberOfStringMatches = nstringmatches;
	return result;
}

autoSTRVEC string32vector_searchAndReplace (constSTRVEC me,
	conststring32 search, conststring32 replace, integer maximumNumberOfReplaces,
	integer *nmatches, integer *nstringmatches, bool use_regexp)
{
	return use_regexp ?
		string32vector_searchAndReplace_regexp (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches) :
		string32vector_searchAndReplace_literal (me, search, replace, maximumNumberOfReplaces, nmatches, nstringmatches);
}

char32 * NUMnumber_as_stringWithDotReplacedByUnderscore (double time) {
	static char32 string [100];
	conststring32 time_string = Melder_double (time);
	const char32 *from = time_string;
	char32 *to = & string [0];
	while (*from != U'\0') {
		if (*from != U'.')
			*to = *from;
		else
			*to = U'_';
		to ++; from ++;
	}
	*to = U'\0';
	return string;
}

/* End of file NUMstring.cpp */
