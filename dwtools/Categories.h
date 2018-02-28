#ifndef _Categories_h_
#define _Categories_h_
/* Categories.h
 *
 * Copyright (C) 1993-2017 David Weenink, 2015,2018 Paul Boersma
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

#include "Collection_extensions.h"
#include "TableOfReal.h"
#include "Graphics.h"

Thing_define (Categories, StringList) {
	void v_info ()
		override;
	void v_readText (MelderReadText text, int formatVersion)
		override;
	void v_writeText (MelderFile file)
		override;
};

autoCategories Categories_create ();

autoCategories Categories_createWithSequentialNumbers (integer n);

autoCategories Categories_selectUniqueItems (Categories me);

void Categories_drawItem (Categories me, Graphics g, integer position, double xWC, double yWC);

autoCategories TableOfReal_to_CategoriesRow (TableOfReal me);

autoCategories TableOfReal_to_CategoriesColumn (TableOfReal me);

#endif /* _Categories_h_ */
