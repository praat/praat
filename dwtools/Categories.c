/* Categories.c
 * 
 * Copyright (C) 1993-2004 David Weenink
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
 djmw 20020315 GPL header
 */

#include "Categories.h"

static int readText (I, MelderFile file)
{
    iam (Categories); long size = texgeti4 (file), i;
	if (size == 0)
	{
		if (! OrderedOfString_init (me, 1)) return 0;
		return 1;	
	}
    if (size < 0 ||
    	! OrderedOfString_init (me, size)) return 0;
	for (i=1; i <= size; i++)
	{
		SimpleString item = Thing_new (classSimpleString);
		if (! item || ! item -> methods -> readText (item, file) ||
			! Ordered_addItemPos (me, item, i)) return 0;
	}
	return 1;
} 

static int writeText (I, MelderFile file)
{
    iam (Categories); long i;
	texputi4 (my size, file, "size");
	for (i = 1; i <= my size; i++)
	{
			SimpleString data = my item [i];
			texputintro (file, "item" " [%ld]:", i);
			if (! classSimpleString->writeText (data, file)) return 0;
			texexdent (file);
	}
    return 1;
}

class_methods (Categories, OrderedOfString)
    class_method (readText)
    class_method (writeText)
class_methods_end

int Categories_init (Categories me, long size)
{
	return OrderedOfString_init (me, size);
}

Categories Categories_create (void)
{
	Categories me = new (Categories);
	if (! me || ! Categories_init (me, 10)) forget (me);
	return me;
}

Categories Categories_sequentialNumbers (long n)
{
	Categories me = new (Categories);
	if (! me || ! OrderedOfString_init (me, 5) ||
		! OrderedOfString_sequentialNumbers (me, n)) forget (me);
	return me;
}

Categories Categories_selectUniqueItems (Categories me, int sorted)
{
	Categories thee = NULL; OrderedOfString s = NULL;
	if (! (s = OrderedOfString_selectUniqueItems (me, sorted))) return thee;
	thee = OrderedOfString_to_Categories (s);
	forget (s);
	return thee;
}

void Categories_drawItem (Categories me, Graphics g, long position, 
	double xWC, double yWC)
{
	if (position < 1 || position > my size) return;
	SimpleString_draw (my item[position], g, xWC, yWC);
}

Categories OrderedOfString_to_Categories (I)
{
	iam (OrderedOfString);
	long i;
	Categories thee = Categories_create();
	
	if (thee == NULL) return NULL;
	
	for (i = 1; i <= my size; i++)
	{
		SimpleString item = Data_copy (my item[i]);
		if (item == NULL || 
			! Collection_addItem (thee, item))
		{ forget (thee); return NULL; };
	}
	return thee;
}

long Categories_getSize (Categories me) { return my size; }

/* TableOfReal_Rowlabels_to_Categories  ??? */
Categories TableOfReal_to_CategoriesRow (I)
{
	iam (TableOfReal);
	long i;
	Categories thee = Categories_create ();
	
	if (! thee) return 0;
	for (i = 1; i <= my numberOfRows; i++)
	{
		if (my rowLabels[i])
		{
			SimpleString s = SimpleString_create (my rowLabels[i]);
			if (s == NULL ||
				! Collection_addItem (thee, s)) goto end;
		}
	}
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

Categories TableOfReal_to_CategoriesColumn (I)
{
	iam (TableOfReal); 
	long i;
	Categories thee = Categories_create ();
	 
	if (thee == NULL) return NULL;
	for (i=1; i <= my numberOfColumns; i++)
	{
		if (my columnLabels[i])
		{
			SimpleString s = SimpleString_create (my columnLabels[i]);
			if (s == NULL ||
				! Collection_addItem (thee, s)) goto end;
		}
	}
end:
	if (Melder_hasError()) forget (thee);
	return thee;
}

/* End of file Categories.c */
