#ifndef _Collection_extensions_h_
#define _Collection_extensions_h_
/* Collection_extensions.h
 *
 * Copyright (C) 1994-2002 David Weenink
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

Collection Collection_and_Permutation_permuteItems (Collection me, Permutation him);
Collection Collection_permuteItems(Collection me);
/* permute the order of my items */

/****************** class OrderedOfString ******************/

Thing_define (OrderedOfString, Ordered) {
	// overridden methods:
		virtual void v_info ();
};

OrderedOfString OrderedOfString_create ();
int OrderedOfString_init (I, long initialCapacity);

int OrderedOfString_append (I, wchar_t *append);
OrderedOfString OrderedOfString_joinItems (I, thou);
/* Join each item */

OrderedOfString OrderedOfString_selectUniqueItems (I, int sort);
/* Postcondition: thy size <= my size */

void OrderedOfString_frequency (I, thou, long *count);
/* count how often the items in 'thee' occur in 'me' */
/* Precondition: count[1..thy size] exists */

/* To be removed Praat 4.2.4 2004040427 */
int OrderedOfString_difference (I, thou, long *ndif, double *fraction);

double OrderedOfString_getFractionDifferent (I, thou);

long OrderedOfString_getNumberOfDifferences (I, thou);

const wchar_t *OrderedOfString_itemAtIndex_c (I, long index);
long OrderedOfString_indexOfItem_c (I, const wchar_t *str);

void OrderedOfString_drawItem (I, Graphics g, long index, double xWC, double yWC);

void OrderedOfString_sequentialNumbers (I, long n);

void OrderedOfString_removeOccurrences (I, const wchar_t *search, int user_regexp);

void OrderedOfString_changeStrings (OrderedOfString me, wchar_t *search, wchar_t *replace,
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, int use_regexp);

long OrderedOfString_isSubsetOf (I, thou, long *translation);
/* Check whether my items are (a subset of)|(in) thy items.
 * Preconditions:
 *	if (translation) translation[1..my size] exists.
 * Postconditions:
 *  Return: the number of my labels that are in thee.
 *  if (translation) the returned translation table has the following property:
 *	if (translation[i] > 0) my label[i] = thy label[ translation[i] ];
 *	else if (translation[i] == 0) my label[i] not in thy labels.
 */

long OrderedOfString_getSize (I);
/* return my size */

#endif /* _Collection_extensions_h_ */
