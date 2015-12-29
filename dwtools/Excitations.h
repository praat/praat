#ifndef _Excitations_h_
#define _Excitations_h_
/* Excitations.h
 *
 * Copyright (C) 1993-2011,2015 David Weenink, 2015 Paul Boersma
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

#include "Collection.h"
#include "Excitation.h"
#include "Pattern.h"
#include "TableOfReal.h"

Collection_declare (OrderedOfExcitation, OrderedOf, Excitation);

Thing_define (ExcitationList, OrderedOfExcitation) {
	structExcitationList () {
		our classInfo = classExcitationList;
	}
};

inline static autoExcitationList ExcitationList_create () {
	return Thing_new (ExcitationList);
}

autoPattern ExcitationList_to_Pattern (ExcitationList me, long join);
/* Precondition: my size >= 1, all items have same dimension */

autoTableOfReal ExcitationList_to_TableOfReal (ExcitationList me);
/* Precondition: my size >= 1, all items have same dimension */

autoExcitation ExcitationList_getItem (ExcitationList m, long item);

#endif /* _Excitations_h_ */
