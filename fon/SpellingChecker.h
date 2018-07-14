#ifndef _SpellingChecker_h_
#define _SpellingChecker_h_
/* SpellingChecker.h
 *
 * Copyright (C) 1999-2011,2015,2017 Paul Boersma
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

#include "WordList.h"
#include "Collection.h"

#include "SpellingChecker_def.h"

autoSpellingChecker WordList_upto_SpellingChecker (WordList me);
autoWordList SpellingChecker_extractWordList (SpellingChecker me);
void SpellingChecker_replaceWordList (SpellingChecker me, WordList list);
autoStringSet SpellingChecker_extractUserDictionary (SpellingChecker me);
void SpellingChecker_replaceUserDictionary (SpellingChecker me, StringSet userDictionary);

bool SpellingChecker_isWordAllowed (SpellingChecker me, conststring32 word);
char32 * SpellingChecker_nextNotAllowedWord (SpellingChecker me, conststring32 sentence, integer *start);

void SpellingChecker_addNewWord (SpellingChecker me, conststring32 word);

/* End of file SpellingChecker.h */
#endif
