/* DurationTier.c
 *
 * Copyright (C) 1992-2002 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 1997/04/08
 * pb 2002/07/16 GPL
 * pb 2006/12/08 info
 */

#include "DurationTier.h"

static void info (I) {
	iam (RealTier);
	classData -> info (me);
	MelderInfo_writeLine1 ("Time domain:");
	MelderInfo_writeLine3 ("   Start time: ", Melder_double (my xmin), " seconds");
	MelderInfo_writeLine3 ("   End time: ", Melder_double (my xmax), " seconds");
	MelderInfo_writeLine3 ("   Total original duration: ", Melder_double (my xmax - my xmin), " seconds");
	MelderInfo_writeLine2 ("Number of points: ", Melder_integer (my points -> size));
	MelderInfo_writeLine2 ("Minimum relative duration value: ", Melder_double (RealTier_getMinimumValue (me)));
	MelderInfo_writeLine2 ("Maximum relative duration value: ", Melder_double (RealTier_getMaximumValue (me)));
}

class_methods (DurationTier, RealTier)
	class_method (info)
class_methods_end

DurationTier DurationTier_create (double tmin, double tmax) {
	DurationTier me = new (DurationTier);
	if (! me || ! RealTier_init (me, tmin, tmax)) { forget (me); return NULL; }
	return me;
}

void DurationTier_draw (DurationTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, int garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, "Relative duration");
}

DurationTier PointProcess_upto_DurationTier (PointProcess me) {
	long i;
	DurationTier thee = DurationTier_create (my xmin, my xmax);
	if (! thee) return NULL;
	for (i = 1; i <= my nt; i ++)
		if (! RealTier_addPoint (thee, my t [i], 1.0)) { forget (thee); return NULL; }
	return thee;
}

/* End of file DurationTier.c */
