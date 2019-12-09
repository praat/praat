#ifndef _Collection_extensions_h_
#define _Collection_extensions_h_
/* Collection_extensions.h
 *
 * Copyright (C) 1994-2019 David Weenink, 2015,2018 Paul Boersma
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

#include "Collection.h"
#include "Graphics.h"
#include "Permutation.h"

autoCollection Collection_Permutation_permuteItems (Collection me, Permutation him);

autoCollection Collection_permuteItems (Collection me);
/* permute the order of my items */

int OrderedOfString_append (StringList me, conststring32 append);

autoStringList OrderedOfString_joinItems (StringList me, StringList thee);
/* Join each item */

autoStringSet StringList_to_StringSet (StringList me);

double OrderedOfString_getFractionDifferent (StringList me, StringList thee);

integer OrderedOfString_getNumberOfDifferences (StringList me, StringList thee);

integer OrderedOfString_indexOfItem_c (StringList me, conststring32 str);

void OrderedOfString_initWithSequentialNumbers (StringList me, integer n);

void OrderedOfString_removeOccurrences (StringList me, conststring32 search, bool use_regexp);

void OrderedOfString_changeStrings (StringList me, char32 *search, char32 *replace,
	integer maximumNumberOfReplaces, integer *out_numberOfMatches, integer *out_numberOfStringmatches, bool use_regexp);

bool OrderedOfString_containSameElements (StringList me, StringList thee);
/*
	Check whether my items are all in thy items.

	Postconditions:
	if (index.size == my size) the returned index table has the following property:
		if (index [i] > 0)
			my label [i] = thy label [index [i]];
		else if (index[i] == 0) my label[i] not in thy labels.
 */

#endif /* _Collection_extensions_h_ */
