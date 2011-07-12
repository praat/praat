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

static void readText (I, MelderReadText text)
{
	iam (Categories); 
	long size = texgeti4 (text);
	if (size == 0)
	{
		OrderedOfString_init (me, 1); therror
	}
	if (size < 0) Melder_throw ("Size cannot be negative.");
	OrderedOfString_init (me, size); therror
	for (long i = 1; i <= size; i++)
	{
		autoSimpleString item = Thing_new (SimpleString);
		((Data_Table) item -> methods) -> readText (item.peek(), text); therror
		Ordered_addItemPos (me, item.transfer(), i);
	}
} 

static void writeText (I, MelderFile file)
{
	iam (Categories);
	texputi4 (file, my size, L"size", 0,0,0,0,0);
	for (long i = 1; i <= my size; i++)
	{
		SimpleString data = (SimpleString) my item [i];
		texputintro (file, L"item" " [", Melder_integer (i), L"]:", 0,0,0);
		classSimpleString -> writeText (data, file);
		texexdent (file);
	}
}

class_methods (Categories, OrderedOfString)
    class_method (readText)
    class_method (writeText)
class_methods_end

void Categories_init (Categories me, long size)
{
	OrderedOfString_init (me, size);
}

Categories Categories_create (void)
{
	try {
		autoCategories me = Thing_new (Categories);
		Categories_init (me.peek(), 10);
		return me.transfer();
	} catch (MelderError) { Melder_thrown ("Categories not created."); }
}

Categories Categories_sequentialNumbers (long n)
{
	try {
		autoCategories me = Thing_new (Categories);
		OrderedOfString_init (me.peek(), 5); therror
		OrderedOfString_sequentialNumbers (me.peek(), n); therror
		return me.transfer();
	} catch (MelderError) { Melder_thrown ("Sequential number Categories not created."); }
}

Categories Categories_selectUniqueItems (Categories me, int sorted)
{
	try {
		autoOrderedOfString s = OrderedOfString_selectUniqueItems (me, sorted);
		autoCategories thee = OrderedOfString_to_Categories (s.peek());
		return thee.transfer();
	} catch (MelderError) { Melder_thrown (me, ": no unique categories created."); }
}

void Categories_drawItem (Categories me, Graphics g, long position, 
	double xWC, double yWC)
{
	if (position < 1 || position > my size) return;
	SimpleString_draw ((SimpleString) my item[position], g, xWC, yWC);
}

Categories OrderedOfString_to_Categories (I)
{
	iam (OrderedOfString);
	try {
		autoCategories thee = Categories_create();
		
		for (long i = 1; i <= my size; i++)
		{
			autoSimpleString item = (SimpleString) Data_copy (my item[i]);
			Collection_addItem (thee.peek(), item.transfer());
		}
		return thee.transfer();
	} catch (MelderError) { Melder_thrown (me, ": not converted to Categories."); }
}

long Categories_getSize (Categories me) { return my size; }

/* TableOfReal_Rowlabels_to_Categories  ??? */
Categories TableOfReal_to_CategoriesRow (I)
{
	iam (TableOfReal);
	try {
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
	} catch (MelderError) { Melder_thrown (me, ": row labels not converted to Categories."); }
}

Categories TableOfReal_to_CategoriesColumn (I)
{
		iam (TableOfReal); 
	try {
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
	} catch (MelderError) { Melder_thrown (me, ": columnlabels not converted to Categories."); }
}

/* End of file Categories.cpp */
