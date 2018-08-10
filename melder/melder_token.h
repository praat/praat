#ifndef _melder_token_h_
#define _melder_token_h_
/* melder_token.h
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

/********** STRING PARSING **********/

/*
	These functions regard a string as a sequence of tokens,
	separated (and perhaps preceded and followed) by white space.
	The tokens cannot contain spaces themselves (there are no escapes).
	Typical use:
		for (token = Melder_firstToken (string); token != nullptr; token = Melder_nextToken ()) {
			... do something with the token ...
		}
*/

integer Melder_countTokens (conststring32 string);
char32 *Melder_firstToken (conststring32 string);
char32 *Melder_nextToken ();
char32 ** Melder_getTokens (conststring32 string, integer *n);
void Melder_freeTokens (char32 ***tokens);
integer Melder_searchToken (conststring32 string, char32 **tokens, integer n);

class autoMelderTokens {
	char32 **tokens;
	integer numberOfTokens;
public:
	autoMelderTokens () {
		tokens = nullptr;
	}
	autoMelderTokens (conststring32 string) {
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
	~autoMelderTokens () {
		if (tokens) {
			for (integer itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
	}
	char32*& operator[] (integer i) {
		return tokens [i];
	}
	char32 ** peek () const {
		return tokens;
	}
	integer count () const {
		return numberOfTokens;
	}
	void reset (conststring32 string) {
		if (tokens) {
			for (integer itoken = 1; itoken <= numberOfTokens; itoken ++)
				Melder_free (tokens [itoken]);
			Melder_freeTokens (& tokens);
		}
		tokens = Melder_getTokens (string, & numberOfTokens);
	}
};

/* End of file melder_token.h */
#endif
