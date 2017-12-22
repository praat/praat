#ifndef _Collection_extensions_h_
#define _Collection_extensions_h_
/* Collection_extensions.h
 *
 * Copyright (C) 1994-2017 David Weenink, 2015 Paul Boersma
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

/****************** class OrderedOfString ******************/

Collection_define (OrderedOfString, OrderedOf, SimpleString) {
	void v_info ()
		override;
};

int OrderedOfString_append (OrderedOfString me, const char32 *append);

autoOrderedOfString OrderedOfString_joinItems (OrderedOfString me, OrderedOfString thee);
/* Join each item */

autoOrderedOfString OrderedOfString_selectUniqueItems (OrderedOfString me);
/* Postcondition: thy size <= my size */

void OrderedOfString_frequency (OrderedOfString me, OrderedOfString thee, integer *count);
/* count how often the items in 'thee' occur in 'me' */
/* Precondition: count[1..thy size] exists */

/* To be removed Praat 4.2.4 2004040427 */
int OrderedOfString_difference (OrderedOfString me, OrderedOfString thee, integer *ndif, double *fraction);

double OrderedOfString_getFractionDifferent (OrderedOfString me, OrderedOfString thee);

integer OrderedOfString_getNumberOfDifferences (OrderedOfString me, OrderedOfString thee);

const char32 *OrderedOfString_itemAtIndex_c (OrderedOfString me, integer index);
integer OrderedOfString_indexOfItem_c (OrderedOfString me, const char32 *str);

void OrderedOfString_drawItem (OrderedOfString me, Graphics g, integer index, double xWC, double yWC);

void OrderedOfString_initWithSequentialNumbers (OrderedOfString me, integer n);

void OrderedOfString_removeOccurrences (OrderedOfString me, const char32 *search, bool use_regexp);

void OrderedOfString_changeStrings (OrderedOfString me, char32 *search, char32 *replace,
	int maximumNumberOfReplaces, integer *nmatches, integer *nstringmatches, bool use_regexp);

integer OrderedOfString_isSubsetOf (OrderedOfString me, OrderedOfString thee, integer *translation);
/* Check whether my items are (a subset of)|(in) thy items.
 * Preconditions:
 *	if (translation) translation[1..my size] exists.
 * Postconditions:
 *  Return: the number of my labels that are in thee.
 *  if (translation) the returned translation table has the following property:
 *	if (translation[i] > 0) my label[i] = thy label[ translation[i] ];
 *	else if (translation[i] == 0) my label[i] not in thy labels.
 */

integer OrderedOfString_getSize (OrderedOfString me);
/* return my size */

#endif /* _Collection_extensions_h_ */
