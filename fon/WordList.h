#ifndef _WordList_h_
#define _WordList_h_
/* WordList.h
 *
 * Copyright (C) 1999-2011,2015 Paul Boersma
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

#include "Strings_.h"

#include "WordList_def.h"

autoWordList Strings_to_WordList (Strings me);
autoStrings WordList_to_Strings (WordList me);

bool WordList_hasWord (WordList me, conststring32 word);

/* End of file WordList.h */
#endif
