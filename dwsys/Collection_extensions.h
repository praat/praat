#ifndef _Collection_extensions_h_
#define _Collection_extensions_h_
/* Collection_extensions.h
 *
 * Copyright (C) 1994-2017 David Weenink, 2015,2018 Paul Boersma
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

int OrderedOfString_append (StringList me, const char32 *append);

autoStringList OrderedOfString_joinItems (StringList me, StringList thee);
/* Join each item */

autoStringSet StringList_to_StringSet (StringList me);

autoStringList OrderedOfString_selectUniqueItems (StringList me);
/* Postcondition: thy size <= my size */

double OrderedOfString_getFractionDifferent (StringList me, StringList thee);

integer OrderedOfString_getNumberOfDifferences (StringList me, StringList thee);

integer OrderedOfString_indexOfItem_c (StringList me, const char32 *str);

void OrderedOfString_initWithSequentialNumbers (StringList me, integer n);

void OrderedOfString_removeOccurrences (StringList me, const char32 *search, bool use_regexp);

void OrderedOfString_changeStrings (StringList me, char32 *search, char32 *replace,
	int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp);

integer OrderedOfString_isSubsetOf (StringList me, StringList thee, integer *translation);
/* Check whether my items are (a subset of)|(in) thy items.
 * Preconditions:
 *	if (translation) translation[1..my size] exists.
 * Postconditions:
 *  Return: the number of my labels that are in thee.
 *  if (translation) the returned translation table has the following property:
 *	if (translation[i] > 0) my label[i] = thy label[ translation[i] ];
 *	else if (translation[i] == 0) my label[i] not in thy labels.
 */

#endif /* _Collection_extensions_h_ */
