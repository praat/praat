#ifndef _SpellingChecker_h_
#define _SpellingChecker_h_
/* SpellingChecker.h
 *
 * Copyright (C) 1999-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "WordList.h"
#include "Collection.h"

#include "SpellingChecker_def.h"
oo_CLASS_CREATE (SpellingChecker, Data);

SpellingChecker WordList_upto_SpellingChecker (WordList me);
WordList SpellingChecker_extractWordList (SpellingChecker me);
void SpellingChecker_replaceWordList (SpellingChecker me, WordList list);
SortedSetOfString SpellingChecker_extractUserDictionary (SpellingChecker me);
void SpellingChecker_replaceUserDictionary (SpellingChecker me, SortedSetOfString userDictionary);

bool SpellingChecker_isWordAllowed (SpellingChecker me, const wchar_t *word);
wchar_t * SpellingChecker_nextNotAllowedWord (SpellingChecker me, const wchar_t *sentence, long *start);

void SpellingChecker_addNewWord (SpellingChecker me, const wchar_t *word);

/* End of file SpellingChecker.h */
#endif
