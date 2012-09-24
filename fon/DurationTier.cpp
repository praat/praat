/* DurationTier.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

#include "DurationTier.h"

Thing_implement (DurationTier, RealTier, 0);

void structDurationTier :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total original duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"Number of points: ", Melder_integer (points -> size));
	MelderInfo_writeLine (L"Minimum relative duration value: ", Melder_double (RealTier_getMinimumValue (this)));
	MelderInfo_writeLine (L"Maximum relative duration value: ", Melder_double (RealTier_getMaximumValue (this)));
}

DurationTier DurationTier_create (double tmin, double tmax) {
	try {
		autoDurationTier me = Thing_new (DurationTier);
		RealTier_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("DurationTier not created.");
	}
}

void DurationTier_draw (DurationTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, const wchar_t *method, int garnish)
{
	RealTier_draw (me, g, tmin, tmax, ymin, ymax, garnish, method, L"Relative duration");
}

DurationTier PointProcess_upto_DurationTier (PointProcess me) {
	try {
		autoDurationTier thee = DurationTier_create (my xmin, my xmax);
		for (long i = 1; i <= my nt; i ++) {
			RealTier_addPoint (thee.peek(), my t [i], 1.0);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to DurationTier.");
	}
}

/* End of file DurationTier.cpp */
