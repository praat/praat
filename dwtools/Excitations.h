#ifndef _Excitations_h_
#define _Excitations_h_
/* Excitations.h
 *
 * Copyright (C) 1993-2017 David Weenink, 2015,2016 Paul Boersma
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
#include "Excitation.h"
#include "PatternList.h"
#include "TableOfReal.h"


#pragma mark - class ExcitationList

Collection_define (ExcitationList, OrderedOf, Excitation) {
};

void ExcitationList_addItem_copy (ExcitationList me, Excitation you);

void ExcitationList_addItems (ExcitationList me, OrderedOf <structExcitation> * list);

autoExcitationList Excitations_to_ExcitationList (OrderedOf <structExcitation> * me);

autoPatternList ExcitationList_to_PatternList (ExcitationList me, integer join);
/* Precondition: my size >= 1, all items have same dimension */

autoTableOfReal ExcitationList_to_TableOfReal (ExcitationList me);
/* Precondition: my size >= 1, all items have same dimension */

autoExcitation ExcitationList_extractItem (ExcitationList m, integer item);

/* End of file Excitations.h */
#endif
