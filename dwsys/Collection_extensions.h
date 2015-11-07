#ifndef _Collection_extensions_h_
#define _Collection_extensions_h_
/* Collection_extensions.h
 *
 * Copyright (C) 1994-2002, 2015 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20020812 GPL header
 djmw 20110414 Latest modification.
*/

#include "Collection.h"
#include "Graphics.h"
#include "Permutation.h"

autoCollection Collection_and_Permutation_permuteItems (Collection me, Permutation him);

autoCollection Collection_permuteItems (Collection me);
/* permute the order of my items */

/****************** class OrderedOfString ******************/

Thing_define (OrderedOfString, Ordered) {
	void v_info ()
		override;
};

autoOrderedOfString OrderedOfString_create ();

int OrderedOfString_init (OrderedOfString me, long initialCapacity);

int OrderedOfString_append (OrderedOfString me, char32 *append);

autoOrderedOfString OrderedOfString_joinItems (OrderedOfString me, OrderedOfString thee);
/* Join each item */

autoOrderedOfString OrderedOfString_selectUniqueItems (OrderedOfString me, int sort);
/* Postcondition: thy size <= my size */

void OrderedOfString_frequency (OrderedOfString me, OrderedOfString thee, long *count);
/* count how often the items in 'thee' occur in 'me' */
/* Precondition: count[1..thy size] exists */

/* To be removed Praat 4.2.4 2004040427 */
int OrderedOfString_difference (OrderedOfString me, OrderedOfString thee, long *ndif, double *fraction);

double OrderedOfString_getFractionDifferent (OrderedOfString me, OrderedOfString thee);

long OrderedOfString_getNumberOfDifferences (OrderedOfString me, OrderedOfString thee);

const char32 *OrderedOfString_itemAtIndex_c (OrderedOfString me, long index);
long OrderedOfString_indexOfItem_c (OrderedOfString me, const char32 *str);

void OrderedOfString_drawItem (OrderedOfString me, Graphics g, long index, double xWC, double yWC);

void OrderedOfString_sequentialNumbers (OrderedOfString me, long n);

void OrderedOfString_removeOccurrences (OrderedOfString me, const char32 *search, int user_regexp);

void OrderedOfString_changeStrings (OrderedOfString me, char32 *search, char32 *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp);

long OrderedOfString_isSubsetOf (OrderedOfString me, OrderedOfString thee, long *translation);
/* Check whether my items are (a subset of)|(in) thy items.
 * Preconditions:
 *	if (translation) translation[1..my size] exists.
 * Postconditions:
 *  Return: the number of my labels that are in thee.
 *  if (translation) the returned translation table has the following property:
 *	if (translation[i] > 0) my label[i] = thy label[ translation[i] ];
 *	else if (translation[i] == 0) my label[i] not in thy labels.
 */

long OrderedOfString_getSize (OrderedOfString me);
/* return my size */

#endif /* _Collection_extensions_h_ */
