#pragma once
/* STR.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

autostring32 STRleft (conststring32 str, integer newLength = 1);

autostring32 STRmid (conststring32 str, integer startingPosition_1, integer numberOfCharacters = 1);

/**
	Look for occurrences of `search` in `str`, and replace them with `replace`.
*/
autostring32 STRreplace (conststring32 str, conststring32 search,
	conststring32 replace, integer maximumNumberOfReplacements,
	integer *out_numberOfReplacements = nullptr);

/**
	Search and replace `maximumNumberOfReplacements` times in `string` on
	the basis of regular expressions.
	If maximumNumberOfReplacements <= 0, the interpreted `replaceRE` replaces *all* occurrences.
	`search_compiled` is an efficient representation of the search regex and
	is the result of the compileRE-function which should be called before this function.
	The number of actual replacements performed is returned in `out_numberOfReplacements`.
*/
autostring32 STRreplace_regex (conststring32 string, regexp *search_compiled,
	conststring32 replace_regex, integer maximumNumberOfReplacements,
	integer *out_numberOfReplacements = nullptr);

autostring32 STRright (conststring32 str, integer newLength = 1);

/* End of file STR.h */
