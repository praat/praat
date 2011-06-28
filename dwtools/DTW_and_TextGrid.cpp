/* DTW_and_TextGrid.cpp
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
 djmw 20060906
 djmw 20070306: Reverse x and y. Reference should always be vertical!
 djmw 20110304 Thing_new
*/

#include "DTW_and_TextGrid.h"

// begin old prototypes for compatibility reasons with the past
TextGrid DTW_and_TextGrid_to_TextGrid_old (DTW me, TextGrid thee);
IntervalTier DTW_and_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee);
TextTier DTW_and_TextTier_to_TextTier_old (DTW me, TextTier thee);
// end old

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
TextTier DTW_and_TextTier_to_TextTier (DTW me, TextTier thee)
{
	try {
		if (my ymin != thy xmin || my ymax != thy xmax) Melder_throw
			("The domain of the TextTier and the y-domain of the DTW must be equal.");
		
		autoTextTier him = (TextTier) Data_copy (thee);

		his xmin = my xmin;
		his xmax = my xmax;
		for (long i = 1; i <= his points -> size; i++)
		{
			TextPoint textpoint = (TextPoint) his points -> item[i];
			double time = DTW_getXTimeFromYTime (me, textpoint -> time);
		
			textpoint -> time = time;
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("TextTier not created from DTW & TextTier."); }
}

IntervalTier DTW_and_IntervalTier_to_IntervalTier (DTW me, IntervalTier thee)
{
	try {
		if ((my ymin != thy xmin) || my ymax != thy xmax) Melder_throw 
			("The domain of the IntervalTier and the y-domain of the DTW must be equal.");
	
		autoIntervalTier him = (IntervalTier) Data_copy (thee);
	
		his xmin = my xmin;
		his xmax = my xmax;
		for (long i = 1; i <= his intervals -> size; i++)
		{
			TextInterval textinterval = (TextInterval) his intervals -> item[i];
			double xmin = DTW_getXTimeFromYTime (me, textinterval -> xmin);
			textinterval -> xmin = xmin;
			double xmax = DTW_getXTimeFromYTime (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("IntervalTier not created from DTW & IntervalTier."); }
}

TextGrid DTW_and_TextGrid_to_TextGrid (DTW me, TextGrid thee)
{
	try {
		autoTextGrid him = Thing_new (TextGrid);
		if ((my ymin != thy xmin) || my ymax != thy xmax) Melder_throw ("The domain of the TextGrid and the y-domain of the DTW must be equal.");
	
		his xmin = my xmin;
		his xmax = my xmax;
		his tiers = Ordered_create ();
	
		for (long i = 1; i <= thy tiers -> size; i++)
		{
			Data anyTier = (Data) thy tiers -> item[i];
		 
			if (anyTier -> methods == (Data_Table) classIntervalTier)
			{
				autoIntervalTier tier = DTW_and_IntervalTier_to_IntervalTier (me, (IntervalTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek()); therror
			}
			else if (anyTier -> methods == (Data_Table) classTextTier)
			{
				autoTextTier tier = DTW_and_TextTier_to_TextTier (me, (TextTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek()); therror
			}
			else
			{
				Melder_throw ("Unknown tier.");
			}
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("TextGrid not created from DTW & TextGrid."); }	
}

/* Get times from TextGrid and substitute new time form the y-times of the DTW. */
TextTier DTW_and_TextTier_to_TextTier_old (DTW me, TextTier thee)
{
	try {
		if ((my xmin != thy xmin) || my xmax != thy xmax) Melder_throw 
			("The domain of the TextTier and the DTW must be equal.");
		autoTextTier him =  (TextTier) Data_copy (thee);
		his xmin = my ymin;
		his xmax = my ymax;
	
		for (long i = 1; i <= his points -> size; i++)
		{
			TextPoint textpoint = (TextPoint) his points -> item[i];
			double time = DTW_getPathY (me, textpoint -> time);
			textpoint -> time = time;
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("TextTier not created."); }
}

IntervalTier DTW_and_IntervalTier_to_IntervalTier_old (DTW me, IntervalTier thee)
{
	try {
		if ((my xmin != thy xmin) || my xmax != thy xmax) Melder_throw 
			("The domain of the IntervalTier and the DTW must be equal.");
	
		autoIntervalTier him = (IntervalTier) Data_copy (thee);
	
		his xmin = my ymin;
		his xmax = my ymax;
	
		for (long i = 1; i <= his intervals -> size; i++)
		{
			TextInterval textinterval = (TextInterval) his intervals -> item[i];
			double xmin = DTW_getPathY (me, textinterval -> xmin);
			textinterval -> xmin = xmin;
			double xmax = DTW_getPathY (me, textinterval -> xmax);
			textinterval -> xmax = xmax;
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("IntervalTier not created."); }
}

TextGrid DTW_and_TextGrid_to_TextGrid_old (DTW me, TextGrid thee)
{
	try {
		autoTextGrid him = Thing_new (TextGrid);
		if (my xmin != thy xmin || my xmax != thy xmax) Melder_throw ("The domain of the TextGrid and the y-domain of the DTW must be equal.");
	
		his xmin = my ymin;
		his xmax = my ymax;
		his tiers = Ordered_create ();
	
		for (long i = 1; i <= thy tiers -> size; i++)
		{
			Data anyTier = (Data) thy tiers -> item[i];
		 
			if (anyTier -> methods == (Data_Table) classIntervalTier)
			{
			
				autoIntervalTier tier = DTW_and_IntervalTier_to_IntervalTier_old (me, (IntervalTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek()); therror
			}
			else if (anyTier -> methods == (Data_Table) classTextTier)
			{
				autoTextTier tier = DTW_and_TextTier_to_TextTier_old (me, (TextTier) anyTier);
				TextGrid_addTier (him.peek(), tier.peek()); therror
			}
			else
			{
				Melder_throw (L"Unknown tier.");
			}
		}
		return him.transfer();
	} catch (MelderError) { rethrowmzero ("TextGrid not created."); }
}

/* End of file DTW_and_TextGrid.cpp */
