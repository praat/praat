#pragma once
/* STRVEC.h
 *
 * Copyright (C) 1992-2023 Paul Boersma
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

autoSTRVEC fileNames_STRVEC (conststring32 path /* cattable */);
autoSTRVEC folderNames_STRVEC (conststring32 path /* cattable */);
autoSTRVEC fileNames_caseInsensitive_STRVEC (conststring32 path /* cattable */);
autoSTRVEC folderNames_caseInsensitive_STRVEC (conststring32 path /* cattable */);

autoSTRVEC readLinesFromFile_STRVEC (MelderFile file);

autoSTRVEC shuffle_STRVEC (STRVEC const& x);
void shuffle_STRVEC_inout (STRVEC const& x);

autoSTRVEC sort_STRVEC (STRVEC const& a);
void sort_STRVEC_inout (STRVEC const& a) noexcept;

autoSTRVEC sort_numberAware_STRVEC (STRVEC const& a);
void sort_numberAware_STRVEC_inout (STRVEC const& a) noexcept;

/*
	Regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
*/
autoSTRVEC splitByWhitespace_STRVEC (conststring32 string);

/*
	Regard a string as a sequence of tokens,
	separated by commas (or another separator) according to RFC 1480.
	Each token is optionally surrounded by double quotes,
	in which case the token can contain a comma itself,
	or even a double quote (which should be doubled to escape it),
	or even a line separator.
	Well-formed:
		abc,"def" -> first token is abc, second token is def
		abc,"""def""" -> first token is abc, second token is "def"
		abc,"def""ghi" -> first token is abc, second token is def"ghi
	Ill-formed:
		abc,"def"ghi -> non-terminated quoted string
		abc,def"ghi -> a quote in an unquoted string
		abc,"def"ghi" -> second token def not followed by comma or end
	Note that in a quoted string in PraatScript,
		abc,"""def"""
	would have to be written as
		"abc,""""""def"""""""
	in which case it would be much easier (if possible) to use a string vector:
		{ "abc", """def""" }
*/
autoSTRVEC splitBy_STRVEC (conststring32 string, conststring32 separator);

/* End of file STRVEC.h */
