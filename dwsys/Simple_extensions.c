/* Simple_extensions.c
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
*/

#include "Simple_extensions.h"
#include "longchar.h"

int SimpleString_init (SimpleString me, const char *string)
{
    if ((my string = Melder_strdup (string))  == NULL) return 0; 
    return 1;
}

int SimpleString_compare (SimpleString me, SimpleString thee)
{
	return strcmp (my string, thy string);
}

const char *SimpleString_c (SimpleString me)
{
    return my string;
}

int SimpleString_append (SimpleString me, SimpleString thee)
{
    return SimpleString_append_c (me, thy string); 
}

int SimpleString_append_c (SimpleString me, const char *str)
{
	long myLength; char *ptr;
	if (! str) return 1;
	myLength = strlen (my string);
	if ((ptr = Melder_realloc (my string, myLength + strlen (str) + 1)) == NULL) return 0;
	my string = ptr;
	strcpy (& my string[myLength], str);
	return 1;	
}

SimpleString SimpleString_concat (SimpleString me, SimpleString thee)
{
	SimpleString him = Data_copy (me);
	if (! him || ! SimpleString_append_c (him, thy string)) forget (him);
	return him; 		
}

SimpleString SimpleString_concat_c (SimpleString me, const char *str)
{
	SimpleString him = Data_copy (me);
	if (! him || ! SimpleString_append_c (him, str)) forget (him);
	return him; 		
}

int SimpleString_replace_c (SimpleString me, const char *str)
{
	char *ptr;
    if (! str || ((ptr = Melder_strdup (str)) == NULL)) return 0;
    Melder_free (my string);
    my string = ptr;
    return 1;
}

long SimpleString_length (SimpleString me)
{
    return strlen (my string);
}

void SimpleString_draw (SimpleString me, Any g, double xWC, double yWC)
{
    Graphics_text (g, xWC, yWC, my string);
}

const char * SimpleString_nativize_c (SimpleString me, int educateQuotes)
{
	SimpleString thee = Data_copy (me);
	if (! thee) return NULL;
	(void) Longchar_nativize (thy string, my string, educateQuotes);
	forget (thee);
	return my string;
}

const char * SimpleString_genericize_c (SimpleString me)
{
	SimpleString thee = Data_copy (me);
	char *ptr;
	if (thee == NULL ||
		((ptr = Melder_realloc (my string, 3 * strlen (my string))) == NULL))
	{
		forget (thee); return NULL;
	}
	my string = ptr;
	(void) Longchar_genericize (thy string, my string);
	forget (thee);
	return my string;
}

/* End of file Simple_extensions.c */
