/* DTW_and_TextGrid.c
 *
 * Copyright (C) 1993-2006 David Weenink
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
 djmw 20060906
 TODO: reverse x and y. Reference should always be vertical!
*/

#include "DTW_and_TextGrid.h"


/* Get times from TextGrid and substitute new time form the y-ties of the DTW. */
TextTier DTW_and_TextTier_to_TextTier (DTW me, TextTier thee)
{
	TextTier him;
	long i;
	
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_errorp 
		("The domain of the TextTier and the DTW must be equal.");
		
	him = Data_copy (thee);
	if (him == NULL) return NULL;
	
	his xmin = my ymin;
	his xmax = my ymax;
	
	for (i = 1; i <= his points -> size; i++)
	{
		TextPoint textpoint = his points -> item[i];
		double time = DTW_getPathY (me, textpoint -> time);
		
		textpoint -> time = time;
	}

	return him;
}

IntervalTier DTW_and_IntervalTier_to_IntervalTier (DTW me, IntervalTier thee)
{
	IntervalTier him;
	double xmin, xmax;
	long i;
	
	if (my xmin != thy xmin || my xmax != thy xmax) return Melder_errorp 
		("The domain of the TextTier and the DTW must be equal.");
	
	him = Data_copy (thee);
	if (him == NULL) return NULL;
	
	his xmin = my ymin;
	his xmax = my ymax;
	
	for (i = 1; i <= his intervals -> size; i++)
	{
		TextInterval textinterval = his intervals -> item[i];
		xmin = DTW_getPathY (me, textinterval -> xmin);
		textinterval -> xmin = xmin;
		xmax = DTW_getPathY (me, textinterval -> xmax);
		textinterval -> xmax = xmax;
	}
	
	return him;
}

TextGrid DTW_and_TextGrid_to_TextGrid (DTW me, TextGrid thee)
{
	char *proc = "DTW_and_TextGrid_to_TextGrid";
	TextGrid him = new (TextGrid);
	long i;

	if (him == NULL) return NULL;
	if (my xmin != thy xmin || my xmax != thy xmax)
	{
		(void) Melder_error ("%s: The domain of the TextGrid and the y-domain of the DTW must be equal.", proc);
		goto end;
	}
	
	his xmin = my ymin;
	his xmax = my ymax;
	his tiers = Ordered_create ();
	if (his tiers == NULL) goto end;
	
	for (i = 1; i <= thy tiers -> size; i++)
	{
		Data anyTier = thy tiers -> item[i];
		 
		if (anyTier -> methods == (Data_Table) classIntervalTier)
		{
			
			IntervalTier tier = DTW_and_IntervalTier_to_IntervalTier (me, (IntervalTier) anyTier);
			if (tier == NULL || ! TextGrid_add (him, (IntervalTier) tier)) goto end;
		}
		else if (anyTier -> methods == (Data_Table) classTextTier)
		{
			TextTier tier = DTW_and_TextTier_to_TextTier (me, (TextTier) anyTier);
			if (tier == NULL || ! TextGrid_add (him, (TextTier) tier)) goto end;
		}
		else
		{
			(void) Melder_error ("Unknown tier.");
			goto end;
			/* error */
		}
	}

end:
	if (Melder_hasError()) forget (him);
	return him;
}


/* End of file DTW_and_TextGrid.c */
