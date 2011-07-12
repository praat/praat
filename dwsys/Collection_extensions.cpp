/* Collection_extensions.c
 *
 * Copyright (C) 1994-2011 David Weenink
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
 djmw 20040420 Fraction in OrderedOfString_difference must be double.
 djmw 20050511 Skip printing unique labels in OrderedOfString
 djmw 20061214 
 djmw 20061214 Changed info to Melder_writeLine<x> format.
 djmw 20110304 Thing_new
*/

#include "Collection_extensions.h"
#include "Simple_extensions.h"
#include "NUM2.h"

Collection Collection_and_Permutation_permuteItems (Collection me, Permutation him)
{
		if (my size != his numberOfElements) Melder_throw (me, "The number of elements are not equal.");
		autoNUMvector<long> pos (1, my size);
		autoCollection thee = static_cast<Collection>(Data_copy (me));
	
		for (long i = 1; i <= my size; i++) pos[i] = i;
		/* Dual meaning of array pos: */
		/* k <  i : position of item 'k' */
		/* k >= i : the item at position 'k' */
		for (long i = 1; i <= my size; i++)
		{
			long ti = pos[i], which = Permutation_getValueAtIndex (him, i);
			long where = pos[which]; /* where >= i */
			Data tmp =  static_cast<Data>(thy item[i]);
			if (i == where) continue;
			thy item[i] = thy item[ where ];
			thy item[where] = tmp;
			/* order is important !! */
			pos[ti] = where; 
			pos[where] = ti;
			pos[which] = which <= i ? i : ti;
		}
		return thee.transfer();
}

Collection Collection_permuteItems(Collection me)
{
	try {
		autoPermutation p = Permutation_create (my size);
		Permutation_permuteRandomly_inline (p.peek(), 0, 0); therror 
		autoCollection thee = Collection_and_Permutation_permuteItems (me, p.peek());
		return thee.transfer();
	} catch (MelderError) { Melder_thrown (me, ": items not permuted."); }
}

/****************** class OrderedOfString ******************/

static void info (I)
{
	iam (OrderedOfString); 
		classData -> info (me);
		MelderInfo_writeLine2 (L"Number of strings: ", Melder_integer (my size));
		autoOrderedOfString uStrings = OrderedOfString_selectUniqueItems(me, 1);
		MelderInfo_writeLine2 (L"Number of unique categories: ", Melder_integer (uStrings -> size));
}

class_methods (OrderedOfString, Ordered) {
    class_method (info)
	class_methods_end
}

int OrderedOfString_init (I, long initialCapacity)
{
	iam (OrderedOfString);
	Ordered_init (me, classSimpleString, initialCapacity); therror
	return 1;
}

OrderedOfString OrderedOfString_create (void)
{
	try {
		autoOrderedOfString me = Thing_new (OrderedOfString);
		OrderedOfString_init (me.peek(), 10); therror
		return me.transfer();
	} catch (MelderError) { Melder_thrown ("OrderedOfString not created."); }
}

int OrderedOfString_append (I, wchar_t *append)
{
	iam (OrderedOfString);
	try {
		if (append == 0) return 1; // BUG: lege string appenden??
		autoSimpleString item = SimpleString_create (append);
		Collection_addItem (me, item.transfer());
		return 1;
	} catch (MelderError) { Melder_thrown (me, ": text not appended."); }
}

OrderedOfString OrderedOfString_joinItems (I, thou)
{
	iam (OrderedOfString);
	thouart (OrderedOfString);
	
		if (my size != thy size) Melder_throw ("sizes must be equal.");
	
		autoOrderedOfString him = (OrderedOfString) Data_copy (me);
	
		for (long i = 1; i <= my size; i++)
		{
			SimpleString_append ((SimpleString) his item[i], (SimpleString)thy item[i]);
		}
		return him.transfer();
}


OrderedOfString OrderedOfString_selectUniqueItems (I, int sort)
{
	iam (OrderedOfString);
	try {
		if (! sort)
		{
			autoOrderedOfString him = OrderedOfString_create ();
			for (long i = 1; i <= my size; i++)
			{
				SimpleString ss = (SimpleString) my item[i];
				if (! OrderedOfString_indexOfItem_c (him.peek(), ss -> string))
				{
					autoSimpleString item = (SimpleString) Data_copy (ss);
					Collection_addItem (him.peek(), item.transfer()); therror
				}
			}
			Collection_shrinkToFit (him.peek());
			return him.transfer();
		}
		autoSortedSet thee = Thing_new (SortedSet);
		SortedSet_init (thee.peek(), classSimpleString, 10); therror
		classSortedSet->compare = (int (*)(Any, Any)) SimpleString_compare;

		/* Collection_to_SortedSet (I, int (*compare)(I, thou)) */
		for (long i = 1; i <= my size; i++)
		{
			if (! SortedSet_hasItem (thee.peek(), my item[i]))
			{
				autoSimpleString item = (SimpleString) Data_copy (my item[i]);
				Collection_addItem (thee.peek(), item.transfer()); therror
			}
		}
		autoOrderedOfString him = OrderedOfString_create ();
		for (long i = 1; i <= thy size; i++)
		{
			autoData item = (Data) Data_copy (thy item[i]);
			Collection_addItem (him.peek(), item.transfer()); therror
		}
		return him.transfer();
	} catch (MelderError) { Melder_thrown (me, ": unique items not selected."); }
}

void OrderedOfString_frequency (I, thou, long *count)
{
	iam (OrderedOfString);
	thouart (OrderedOfString);
	
    for (long i = 1; i <= my size; i++)
	{
		for (long j = 1; j <= thy size; j++)
		{
			if (Data_equal (my item[i], thy item[j]))
			{
				count[j]++; 
				break;
			}
		}
	}
}

long OrderedOfString_getNumberOfDifferences (I, thou)
{
	iam (OrderedOfString); 
	thouart (OrderedOfString); 
	long numberOfDifferences = 0;
	
	if (my size != thy size) return -1;
	for (long i = 1; i <= my size; i++)
	{
		if (! Data_equal (my item[i], thy item[i])) numberOfDifferences++;
	}
	return numberOfDifferences;
}

double OrderedOfString_getFractionDifferent (I, thou)
{
	iam (OrderedOfString); 
	thouart (OrderedOfString); 
	long numberOfDifferences = OrderedOfString_getNumberOfDifferences (me, thee);
	
	if (numberOfDifferences < 0) return NUMundefined;
	return my size == 0 ? 0 : (0.0 + numberOfDifferences) / my size;
}

int OrderedOfString_difference (I, thou, long *ndif, double *fraction)
{
	iam (OrderedOfString); 
	thouart (OrderedOfString); 
	
	*ndif = 0; *fraction = 1;
	if (my size != thy size)
	{
		Melder_flushError ("OrderedOfString_difference: the number of items differ");
		return 0;
	}
	for (long i = 1; i <= my size; i++)
	{
		if (! Data_equal (my item[i], thy item[i])) (*ndif)++;
	}
	*fraction = *ndif; 
	*fraction /= my size;
	return 1;
}

long OrderedOfString_indexOfItem_c (I, const wchar_t *str)
{
	iam (OrderedOfString);
		long index = 0;
		autoSimpleString s = SimpleString_create (str);
	
		for (long i = 1; i <= my size; i++)
		{
			if (Data_equal (my item[i], s.peek())) { index = i; break; }
		}
		return index;
}

const wchar_t *OrderedOfString_itemAtIndex_c (I, long index)
{
	iam (OrderedOfString);
	return index > 0 && index <= my size ? SimpleString_c ((SimpleString)my item[index]) : NULL;
}

int OrderedOfString_sequentialNumbers (I, long n)
{
	iam (OrderedOfString);
		Collection_removeAllItems (me);
		for (long i = 1; i <= n; i++)
		{
			wchar_t s[20];
			swprintf (s, 20, L"%ld", i);
			autoSimpleString str = SimpleString_create (s);
			Collection_addItem (me, str.transfer());
		}
		return 1;
}

int OrderedOfString_changeStrings (I, wchar_t *search, wchar_t *replace, 
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, 
	int use_regexp)
{
	iam (OrderedOfString);
	regexp *compiled_search = NULL;
	regularExp_CHAR *compileMsg;
	wchar_t *r;

	if (search == NULL || replace == NULL) return 0;
	
	if (use_regexp)
	{			
		compiled_search = CompileRE ((regularExp_CHAR *) search, &compileMsg, 0);
		if (compiled_search == NULL) return Melder_error1 ((wchar_t*) compileMsg);
	}
	for (long i = 1; i <= my size; i++)
	{
		SimpleString ss = (SimpleString) my item[i];
		long nmatches_sub;
		
		if (use_regexp) {
			r = str_replace_regexp (ss -> string, compiled_search, 
				replace, maximumNumberOfReplaces, &nmatches_sub);
			if (r == NULL) goto end;
		}
		else r = str_replace_literal (ss -> string, search, replace,
			maximumNumberOfReplaces, &nmatches_sub);
			
		if (r == NULL) goto end;
		Melder_free (ss -> string);
		ss -> string = r;
		if (nmatches_sub > 0)
		{
			*nmatches += nmatches_sub;
			(*nstringmatches)++;
		}
	}

end:
	if (use_regexp) free (compiled_search);
	return ! Melder_hasError ();
}
 

long OrderedOfString_isSubsetOf (I, thou, long *translation) // ?? test and give number
{
    iam (OrderedOfString); 
	thouart (OrderedOfString); 
	long nStrings = 0;
	
    for (long i = 1; i <= my size; i++)
    {
    	if (translation) translation[i] = 0;
    	for (long j = 1; j <= thy size; j++)
			if (Data_equal (my item[i], thy item[j]))
			{
	    		if (translation) translation[i] = j;
	    		nStrings++; break;
			}
	}
    return nStrings;
}

void OrderedOfString_drawItem (I, Any g, long index, double xWC, double yWC)
{
	iam (OrderedOfString);
    if (index > 0 && index <= my size) 
	{
		SimpleString_draw ((SimpleString)my item[index], g, xWC, yWC);
	}
}

long OrderedOfString_getSize (I)
{
	iam (OrderedOfString);
	return my size;
}

void OrderedOfString_removeOccurrences (I, const wchar_t *search, int use_regexp)
{
	iam (OrderedOfString); 
	if (search == NULL) return;
	for (long i = my size; i >=1; i--)
	{
		SimpleString ss = (SimpleString) my item[i];
		if ((use_regexp && strstr_regexp (ss -> string, search)) ||
			(!use_regexp && wcsstr (ss -> string, search)))
		{
			Collection_removeItem (me, i);
		}
	}
}

/* End of file Collection_extensions.c */
