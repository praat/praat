#ifndef _Index_h_
#define _Index_h_
/* Index.h
 *
 * Copyright (C) 2005-2011, 2016-2017 David Weenink
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

#include "Index_def.h"

/*
 * An Index is a summary of an array of strings. 
 * It has two parts: 
 * 1 an OrderedSet of the different strings that occur 
 * 2 an item array with indexes in this OrderedSet
 * Example:
 * Suppose we have a Strings with 7 elements/items "u", "o", "a", "u", "u", "o", "a"
 * The Index derived from this Strings will have 3 elements in its sorted set "a", "o" and "u"
 * The item array with indices will have the following content: 3, 2, 1, 3, 3, 2, 1
 */
void Index_init (Index me, integer numberOfElements);

autoIndex Index_extractPart (Index me, integer from, integer to);

integer Index_getClassIndexFromItemIndex (Index me, integer itemIndex);

autoStringsIndex StringsIndex_create (integer numberOfItems);

int StringsIndex_getClassIndexFromClassLabel (StringsIndex me, char32 *classLabel);

const char32 *StringsIndex_getClassLabelFromClassIndex (StringsIndex me, integer classNumber);

const char32 *StringsIndex_getItemLabelFromItemIndex (StringsIndex me, integer itemNumber);

integer StringsIndex_countItems (StringsIndex me, int iclas);

#endif /* _Index_h_ */
