/* Collection_extensions.c
 *
 * Copyright (C) 1994-2002 David Weenink
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
*/

#include "Collection_extensions.h"
#include "Simple_extensions.h"
#include "NUM2.h"

Collection Collection_and_Permutation_permuteItems (Collection me, Permutation him)
{
	Collection thee = NULL;
	long i, *pos = NULL;

	if (my size != his numberOfElements) return Melder_errorp1 (L"The number of elements are not equal.");
	if (! (pos = NUMlvector (1, my size)) ||
		! (thee = Data_copy (me)))
	{
		forget (thee); goto end;
	}
	
	for (i=1; i <= my size; i++) pos[i] = i;
	/* Dual meaning of array pos: */
	/* k <  i : position of item 'k' */
	/* k >= i : the item at position 'k' */
	for (i = 1; i <= my size; i++)
	{
		long ti = pos[i], which = Permutation_getValueAtIndex (him, i);
		long where = pos[which]; /* where >= i */
		Data tmp = thy item[i];
		if (i == where) continue;
		thy item[i] = thy item[ where ];
		thy item[where] = tmp;
		/* order is important !! */
		pos[ti] = where; 
		pos[where] = ti;
		pos[which] = which <= i ? i : ti;
	}
end:
	NUMlvector_free (pos, 1);
	return thee;
}

Collection Collection_permuteItems(Collection me)
{
	Collection thee = NULL;
	Permutation p = Permutation_create (my size);
	if (p == NULL) return NULL;
	if (Permutation_permuteRandomly_inline (p, 0, 0))
	{
		thee = Collection_and_Permutation_permuteItems (me, p);
		forget (p);
	}
	return thee;
}

/****************** class OrderedOfString ******************/

static void info (I)
{
	iam (OrderedOfString); 
	OrderedOfString uStrings = NULL;
	
	classData -> info (me);
	MelderInfo_writeLine2 (L"Number of strings: ", Melder_integer (my size));
	uStrings = OrderedOfString_selectUniqueItems (me, 1);
    if (uStrings != NULL)
	{
		MelderInfo_writeLine2 (L"Number of unique categories: ", Melder_integer (uStrings -> size));
		forget (uStrings);
	}
}

class_methods (OrderedOfString, Ordered)
    class_method (info)
class_methods_end

int OrderedOfString_init (I, long initialCapacity)
{
	iam (OrderedOfString);
	return Ordered_init (me, classSimpleString, initialCapacity);
}

Any OrderedOfString_create (void)
{
    OrderedOfString me = new (OrderedOfString);
    if (! me || ! OrderedOfString_init (me, 10)) forget (me);
    return me;
}

int OrderedOfString_append (I, wchar_t *append)
{
	iam (OrderedOfString);
	SimpleString item;
	
	if (append == NULL) return 1;
	
	item = SimpleString_create (append);
	if (item == NULL) return 0;
	return Collection_addItem (me, item);
}

Any OrderedOfString_joinItems (I, thou)
{
	iam (OrderedOfString);
	thouart (OrderedOfString);
	OrderedOfString him;
	long i;
	
	if (my size != thy size) return NULL;
	
	him = Data_copy (me);
	if (him == NULL) return NULL;
	
	for (i=1; i <= my size; i++)
	{
		if (! SimpleString_append (his item[i], thy item[i])) break;
	}
	if (Melder_hasError()) forget (him);
	return him;
}


Any OrderedOfString_selectUniqueItems (I, int sort)
{
	iam (OrderedOfString); 
	SortedSet thee = NULL; 
	OrderedOfString him = NULL;
	
	SimpleString item; long i;
	if (! sort)
	{
		if (! (him = OrderedOfString_create ())) goto error;
		for (i=1; i <= my size; i++)
		{
			if (! OrderedOfString_indexOfItem_c (him, 
				((SimpleString) my item[i])->string) &&
		   		! (item = Data_copy (my item[i])) &&
				! Collection_addItem (him, item)) goto error;
		}
		Collection_shrinkToFit (him);
		return him;
	}
	thee = new (SortedSet);
	SortedSet_init (thee, classSimpleString, 10);
	classSortedSet->compare = (int (*)(Any, Any)) SimpleString_compare;

	/* Collection_to_SortedSet (I, int (*compare)(I, thou)) */
	for (i=1; i <= my size; i++)
	{
		if (! SortedSet_hasItem (thee, my item[i]) &&
		   (! (item = Data_copy (my item[i])) ||
			! Collection_addItem (thee, item))) goto error;
	}
	if (! (him = OrderedOfString_create ())) goto error;
	for (i=1; i <= thy size; i++)
	{
		Data item = Data_copy (thy item[i]);
		if (! item || ! Collection_addItem (him, item)) goto error;
	}
	forget (thee);
	return him;
error:
	forget (thee); forget (him);
	return Melder_errorp1 (L"OrderedOfString_selectUniqueItems: not performed");
}

void OrderedOfString_frequency (I, thou, long *count)
{
	iam (OrderedOfString);
	thouart (OrderedOfString);
	long i, j;
	
    for (i = 1; i <= my size; i++)
	{
		for (j = 1; j <= thy size; j++)
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
	long i, numberOfDifferences = 0;
	
	if (my size != thy size) return -1;
	for (i = 1; i <= my size; i++)
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
	long i;
	
	*ndif = 0; *fraction = 1;
	if (my size != thy size)
	{
		Melder_flushError ("OrderedOfString_difference: the number of items differ");
		return 0;
	}
	for (i = 1; i <= my size; i++)
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
	long i, index = 0;
	SimpleString s = SimpleString_create (str);
	
	if (s == NULL) return 0;
	for (i=1; i <= my size; i++)
	{
		if (Data_equal (my item[i], s)) { index = i; break; }
	}
	forget (s);
	return index;
}

const wchar_t *OrderedOfString_itemAtIndex_c (I, long index)
{
	iam (OrderedOfString);
	return index > 0 && index <= my size ? SimpleString_c (my item[index]) : NULL;
}

int OrderedOfString_sequentialNumbers (I, long n)
{
	iam (OrderedOfString); long i;
	Collection_removeAllItems (me);
    for (i = 1; i <= n; i++)
    {
		wchar_t s[20]; SimpleString str = NULL;
		if (swprintf (s, 20, L"%ld", i) == EOF ||
			! (str = SimpleString_create (s)) ||
			! Collection_addItem (me, str)) return 0;
    }
    return 1;	
}

int OrderedOfString_changeStrings (I, wchar_t *search, wchar_t *replace, 
	int maximumNumberOfReplaces, long *nmatches, long *nstringmatches, 
	int use_regexp)
{
	iam (OrderedOfString);
	regexp *compiled_search = NULL;
	char *compileMsg, *rA;
	wchar_t *r;
	long i;

	if (search == NULL || replace == NULL) return 0;
	
	if (use_regexp)
	{			
		compiled_search = CompileRE (Melder_peekWcsToUtf8 (search), &compileMsg, 0);
		if (compiled_search == NULL) return Melder_error1 (Melder_utf8ToWcs (compileMsg));
	}
	for (i = 1; i <= my size; i++)
	{
		SimpleString ss = my item[i];
		long nmatches_sub;
		
		if (use_regexp) {
			rA = str_replace_regexp (Melder_peekWcsToUtf8 (ss -> string), compiled_search, 
				Melder_peekWcsToUtf8 (replace), maximumNumberOfReplaces, &nmatches_sub);
			if (rA == NULL) goto end;
			r = Melder_utf8ToWcs (rA);
			Melder_free (rA);
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
 

long OrderedOfString_isSubsetOf (I, thou, long *translation)
{
    iam (OrderedOfString); 
	thouart (OrderedOfString); 
	long i, j, nStrings = 0;
	
    for (i=1; i <= my size; i++)
    {
    	if (translation) translation[i] = 0;
    	for (j=1; j <= thy size; j++)
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
		SimpleString_draw (my item[index], g, xWC, yWC);
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
	long i;
	if (search == NULL) return;
	for (i=my size; i >=1; i--)
	{
		SimpleString ss = my item[i];
		if ((use_regexp && strstr_regexp (ss -> string, search)) ||
			(!use_regexp && wcsstr (ss -> string, search)))
		{
			Collection_removeItem (me, i);
		}
	}
}

/* End of file Collection_extensions.c */
