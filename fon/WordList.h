#ifndef _WordList_h_
#define _WordList_h_
/* WordList.h
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

#include "Strings_.h"

#include "WordList_def.h"
oo_CLASS_CREATE (WordList, Data);

WordList Strings_to_WordList (Strings me);
Strings WordList_to_Strings (WordList me);

bool WordList_hasWord (WordList me, const wchar_t *word);

/* End of file WordList.h */
#endif
