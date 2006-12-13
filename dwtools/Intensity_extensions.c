/* Intensity_extensions.c
 *
 * Copyright (C) 2006 David Weenink
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
 djmw 20061204 Initial version
*/

#include "Intensity_extensions.h"
#include "TextGrid_extensions.h"

static int IntervalTier_addBoundaryUnsorted (IntervalTier me, long iinterval, double time, char *leftLabel)
{
	TextInterval ti, ti_new;
	if (time <= my xmin || time >= my xmax) return 0;
	
	/* Find interval to split */
	if (iinterval <= 0) iinterval = IntervalTier_timeToLowIndex (me, time);
	
	/* Modify end time of left label */
	ti = my intervals -> item[iinterval];
	ti -> xmax = time;
	if (! TextInterval_setText (ti, leftLabel)) return 0;
	
	ti_new = TextInterval_create (time, my xmax, "");
	if (ti_new == NULL || ! Sorted_addItem_unsorted (my intervals, ti_new)) return 0;
	return 1;
}

TextGrid Intensity_to_TextGrid_detectSilences (Intensity me, double silenceThreshold_dB, 
	double minSilenceDuration, double minSoundingDuration, char *silenceLabel, char *soundingLabel)
{
	TextGrid thee = NULL;
	IntervalTier it;
	int inSilenceInterval = 1, addBoundary; 
	long i, iinterval = 1;
	double duration = my xmax -my xmin, time;
	double intensity_max_db, intensity_min_db, intensity_dbRange;
	double intensityThreshold, xOfMaximum, xOfMinimum;
	
	if (silenceThreshold_dB >= 0) return Melder_errorp ("The silence threshold w.r.t. the maximum intensity should be a negative number.");
	
	thee = TextGrid_create (my xmin, my xmax, "silences", "");
	if (thee == NULL) return NULL;
	it = thy tiers -> item[1];
	if (! TextInterval_setText (it -> intervals -> item[1], soundingLabel)) goto end;
	if (minSilenceDuration > duration) return thee;
	
	Vector_getMaximumAndX (me, 0, 0, NUM_PEAK_INTERPOLATE_PARABOLIC, &intensity_max_db, &xOfMaximum);
	
	Vector_getMinimumAndX (me, 0, 0, NUM_PEAK_INTERPOLATE_PARABOLIC, &intensity_min_db, &xOfMinimum);
	intensity_dbRange = intensity_max_db - intensity_min_db;
	
	if (intensity_dbRange < 10) Melder_warning ("The loudest and softest part in your sound only differ by %lf dB.", intensity_dbRange);
	
	intensityThreshold = intensity_max_db - fabs (silenceThreshold_dB);
	
	if (minSilenceDuration > duration || intensityThreshold < intensity_min_db) return thee;
	
	inSilenceInterval = my z[1][1] < intensityThreshold;
	iinterval = 1;
	for (i = 2; i <= my nx; i++)
	{
		char *label; 
		addBoundary = 0;
		if (my z[1][i] < intensityThreshold)
		{
			if (!inSilenceInterval) /* Start of silence */
			{
				addBoundary = 1;
				inSilenceInterval = 1;
				label = soundingLabel;
			}
		}
		else
		{
			if (inSilenceInterval) /* End of silence */
			{
				addBoundary = 1;
				inSilenceInterval = 0;
				label = silenceLabel;
			}
		}
		
		if (addBoundary)
		{
			time = my x1 + (i - 1) * my dx;
			if (! IntervalTier_addBoundaryUnsorted (it, iinterval, time, label)) goto end;
			iinterval++;
		}
	}
	
	/* (re)label last interval */
	
	if (inSilenceInterval && 
		! TextInterval_setText (it -> intervals -> item[iinterval], silenceLabel)) goto end;
	Sorted_sort (it -> intervals);
	/*
		First remove short non-silence intervals in-between silence intervals and
		then remove the remaining short silence intervals.
		This works much better than first removing short silence intervals and 
		then short non-silence intervals.
	*/
	IntervalTier_removeBoundary_minimumDuration (it, soundingLabel, minSoundingDuration);
	IntervalTier_removeBoundary_equalLabels (it, silenceLabel);
	IntervalTier_removeBoundary_minimumDuration (it, silenceLabel, minSilenceDuration);
	IntervalTier_removeBoundary_equalLabels (it, soundingLabel);
	
end:
	if (Melder_hasError ()) forget (thee);
	return thee;
}
