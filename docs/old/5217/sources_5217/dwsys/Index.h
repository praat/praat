#ifndef _Index_h_
#define _Index_h_
/* Index.h
 *
 * Copyright (C) 2005-2010 David Weenink
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
 djmw 20050724
 djmw 20101007 Latest modification.
*/

#ifndef _Data_h_
	#include "Data.h"
#endif

#ifndef _Distributions_h_
	#include "Collection.h"
#endif

#define Index_members Data_members \
	Ordered classes; \
	long numberOfElements; \
	long *classIndex;

#define Index_methods Data_methods
class_create (Index, Data);

#define StringsIndex_members Index_members

#define StringsIndex_methods Index_methods
class_create (StringsIndex, Index);


int Index_init (I, long numberOfElements);

Index Index_extractPart (I, long from, long to);

StringsIndex StringsIndex_create (long numberOfElements);

int StringsIndex_getClass (StringsIndex me, wchar_t *classLabel);

long StringsIndex_countItems (StringsIndex me, int iclas);


#endif /* _Index_h_ */
