#ifndef _SpellingChecker_h_
#define _SpellingChecker_h_
/* SpellingChecker.h
 *
 * Copyright (C) 1999-2002 Paul Boersma
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

/*
 * pb 1999/05/28
 * pb 2002/07/16 GPL
 */

#ifndef _WordList_h_
	#include "WordList.h"
#endif
#ifndef _Collection_h_
	#include "Collection.h"
#endif

#include "SpellingChecker_def.h"

#define SpellingChecker_methods Data_methods
oo_CLASS_CREATE (SpellingChecker, Data)

SpellingChecker WordList_upto_SpellingChecker (WordList me);
WordList SpellingChecker_extractWordList (SpellingChecker me);
int SpellingChecker_replaceWordList (SpellingChecker me, WordList list);
SortedSetOfString SpellingChecker_extractUserDictionary (SpellingChecker me);
int SpellingChecker_replaceUserDictionary (SpellingChecker me, SortedSetOfString userDictionary);

int SpellingChecker_isWordAllowed (SpellingChecker me, const char *word);
char * SpellingChecker_nextNotAllowedWord (SpellingChecker me, const char *sentence, int *start);

int SpellingChecker_addNewWord (SpellingChecker me, const char *word);

#endif
