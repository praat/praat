/* Categories.cpp
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

/*
 djmw 20020315 GPL header
 djmw 20110304 Thing_new
 */

#include "Categories.h"

static int readText (I, MelderReadText text)
{
	try {
		iam (Categories); 
		long size = texgeti4 (text);
		if (size == 0)
		{
			OrderedOfString_init (me, 1); therror
		}
		if (size < 0) rethrowzero;
    	OrderedOfString_init (me, size); therror
		for (long i = 1; i <= size; i++)
		{
			autoSimpleString item = Thing_new (SimpleString);
			item -> methods -> readText (item.peek(), text); therror
			Ordered_addItemPos (me, item.transfer(), i);
		}
		return 1;
	} catch (MelderError) { rethrowzero; }
} 

static int writeText (I, MelderFile file)
{
	try {
		iam (Categories);
		texputi4 (file, my size, L"size", 0,0,0,0,0);
		for (long i = 1; i <= my size; i++)
		{
			SimpleString data = (SimpleString) my item [i];
			texputintro (file, L"item" " [", Melder_integer (i), L"]:", 0,0,0);
			classSimpleString -> writeText (data, file);
			texexdent (file);
		}
		return 1;
	} catch (MelderError) { rethrowzero; }
}

class_methods (Categories, OrderedOfString)
    class_method (readText)
    class_method (writeText)
class_methods_end

void Categories_init (Categories me, long size)
{
	try { OrderedOfString_init (me, size);} catch (MelderError) { rethrow; }
}

Categories Categories_create (void)
{
	try {
		autoCategories me = Thing_new (Categories);
		Categories_init (me.peek(), 10);
		return me.transfer();
	} catch (MelderError) { rethrowmzero ("categories not created."); }
}

Categories Categories_sequentialNumbers (long n)
{
	try {
		autoCategories me = Thing_new (Categories);
		OrderedOfString_init (me.peek(), 5); therror
		OrderedOfString_sequentialNumbers (me.peek(), n); therror
		return me.transfer();
	} catch (MelderError) { rethrowmzero ("Categories with sequential numbers not created."); }
}

Categories Categories_selectUniqueItems (Categories me, int sorted)
{
	try {
		autoOrderedOfString s = OrderedOfString_selectUniqueItems (me, sorted);
		autoCategories thee = OrderedOfString_to_Categories (s.peek());
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Categories not created."); }
}

void Categories_drawItem (Categories me, Graphics g, long position, 
	double xWC, double yWC)
{
	if (position < 1 || position > my size) return;
	SimpleString_draw ((SimpleString) my item[position], g, xWC, yWC);
}

Categories OrderedOfString_to_Categories (I)
{
	try {
		iam (OrderedOfString);
		autoCategories thee = Categories_create();
		
		for (long i = 1; i <= my size; i++)
		{
			autoSimpleString item = (SimpleString) Data_copy (my item[i]);
			Collection_addItem (thee.peek(), item.transfer());
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Categories not created."); }
}

long Categories_getSize (Categories me) { return my size; }

/* TableOfReal_Rowlabels_to_Categories  ??? */
Categories TableOfReal_to_CategoriesRow (I)
{
	try {
		iam (TableOfReal);
		autoCategories thee = Categories_create ();
	
		for (long i = 1; i <= my numberOfRows; i++)
		{
			if (my rowLabels[i])
			{
				autoSimpleString s = SimpleString_create (my rowLabels[i]);
				Collection_addItem (thee.peek(), s.transfer()); therror
			}
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Categories not created."); }
}

Categories TableOfReal_to_CategoriesColumn (I)
{
	try {
		iam (TableOfReal); 
		autoCategories thee = Categories_create ();
	 
		for (long i = 1; i <= my numberOfColumns; i++)
		{
			if (my columnLabels[i])
			{
				autoSimpleString s = SimpleString_create (my columnLabels[i]);
				Collection_addItem (thee.peek(), s.transfer());
			}
		}
		return thee.transfer();
	} catch (MelderError) { rethrowmzero ("Categories not created."); }
}

/* End of file Categories.cpp */
