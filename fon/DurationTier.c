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
 */

#include "DurationTier.h"

class_methods (DurationTier, RealTier)
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
