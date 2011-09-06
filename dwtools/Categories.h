#ifndef _Categories_h_
#define _Categories_h_
/* Categories.h
 *
 * Copyright (C) 1993-2011 David Weenink
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

#include "Collection_extensions.h"
#include "Simple_extensions.h"
#include "TableOfReal.h"
#include "Graphics.h"

Thing_define (Categories, OrderedOfString) {
	// overridden methods:
	public:
		virtual void v_readText (MelderReadText text);
		virtual void v_writeText (MelderFile file);
};

void Categories_init (Categories me, long size);

Categories Categories_create ();
Categories Categories_sequentialNumbers (long n);

Categories Categories_selectUniqueItems (Categories me, int sorted);

void Categories_drawItem (Categories me, Graphics g, long position, double xWC, double yWC);

Categories OrderedOfString_to_Categories (I);

long Categories_getSize (Categories me);
/* return my size */

Categories TableOfReal_to_CategoriesRow (I);
Categories TableOfReal_to_CategoriesColumn (I);

#endif /* _Categories_h_ */
