/* AnyTier.cpp
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "AnyTier.h"

/*
#include "oo_DESTROY.h"
#include "AnyTier_def.h"
#include "oo_COPY.h"
#include "AnyTier_def.h"
#include "oo_EQUAL.h"
#include "AnyTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "AnyTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "AnyTier_def.h"
#include "oo_READ_TEXT.h"
#include "AnyTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "AnyTier_def.h"
#include "oo_READ_BINARY.h"
#include "AnyTier_def.h"
#include "oo_DESCRIPTION.h"
#include "AnyTier_def.h"
*/

Thing_implement (AnyPoint, SimpleDouble, 0);

Thing_implement (AnyTier, Function, 0);

void structAnyTier :: v_shiftX (double xfrom, double xto) {
	AnyTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= our points.size; i ++) {
		AnyPoint point = our points.at [i];
		NUMshift (& point -> number, xfrom, xto);
	}
}

void structAnyTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	AnyTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= our points.size; i ++) {
		AnyPoint point = our points.at [i];
		NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

long AnyTier_timeToLowIndex (AnyTier me, double time) {
	if (my points.size == 0) return 0;   // undefined
	long ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (time < tleft) return 0;   // offleft
	double tright = my points.at [iright] -> number;
	if (time >= tright) return iright;
	Melder_assert (time >= tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = my points.at [imid] -> number;
		if (time < tmid) {
			iright = imid;
			tright = tmid;
		} else {
			ileft = imid;
			tleft = tmid;
		}
	}
	Melder_assert (iright == ileft + 1);
	Melder_assert (ileft >= 1);
	Melder_assert (iright <= my points.size);
	Melder_assert (time >= my points.at [ileft] -> number);
	Melder_assert (time <= my points.at [iright] -> number);
	return ileft;
}

long AnyTier_timeToHighIndex (AnyTier me, double time) {
	if (my points.size == 0) return 0;   // undefined; is this right?
	long ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (time <= tleft) return 1;
	double tright = my points.at [iright] -> number;
	if (time > tright) return iright + 1;   // offright
	Melder_assert (time > tleft && time <= tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = my points.at [imid] -> number;
		if (time <= tmid) {
			iright = imid;
			tright = tmid;
		} else {
			ileft = imid;
			tleft = tmid;
		}
	}
	Melder_assert (iright == ileft + 1);
	Melder_assert (ileft >= 1);
	Melder_assert (iright <= my points.size);
	Melder_assert (time >= my points.at [ileft] -> number);
	Melder_assert (time <= my points.at [iright] -> number);
	return iright;
}

integer AnyTier_getWindowPoints (AnyTier me, double tmin, double tmax, integer *imin, integer *imax) {
	if (my points.size == 0) return 0;
	*imin = AnyTier_timeToHighIndex (me, tmin);
	*imax = AnyTier_timeToLowIndex (me, tmax);
	if (*imax < *imin) return 0;
	return *imax - *imin + 1;
}
	
long AnyTier_timeToNearestIndex (AnyTier me, double time) {
	if (my points.size == 0) return 0;   // undefined
	long ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (time <= tleft) return 1;
	double tright = my points.at [iright] -> number;
	if (time >= tright) return iright;
	Melder_assert (time > tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = my points.at [imid] -> number;
		if (time < tmid) {
			iright = imid;
			tright = tmid;
		} else {
			ileft = imid;
			tleft = tmid;
		}
	}
	Melder_assert (iright == ileft + 1);
	Melder_assert (ileft >= 1);
	Melder_assert (iright <= my points.size);
	Melder_assert (time >= my points.at [ileft] -> number);
	Melder_assert (time <= my points.at [iright] -> number);
	return time - tleft <= tright - time ? ileft : iright;
}

long AnyTier_hasPoint (AnyTier me, double t) {
	if (my points.size == 0) return 0;   // point not found
	long ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (t < tleft) return 0;   // offleft
	double tright = my points.at [iright] -> number;
	if (t > tright) return 0;   // offright
	if (t == tleft) return 1;
	if (t == tright) return iright;
	Melder_assert (t > tleft && t < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = my points.at [imid] -> number;
		if (t < tmid) {
			iright = imid;
			tright = tmid;
		} else if (t == tmid) {
			return imid;   // point found
		} else {
			ileft = imid;
			tleft = tmid;
		}
	}
	Melder_assert (iright == ileft + 1);
	Melder_assert (ileft >= 1);
	Melder_assert (iright <= my points.size);
	Melder_assert (t > my points.at [ileft] -> number);
	Melder_assert (t < my points.at [iright] -> number);
	return 0;   // point not found
}

void AnyTier_addPoint_move (AnyTier me, autoAnyPoint point) {
	try {
		my points. addItem_move (point.move());
	} catch (MelderError) {
		Melder_throw (me, U": point not added.");
	}
}

void AnyTier_removePoint (AnyTier me, long i) {
	if (i >= 1 && i <= my points.size) my points. removeItem (i);
}

void AnyTier_removePointNear (AnyTier me, double time) {
	long ipoint = AnyTier_timeToNearestIndex (me, time);
	if (ipoint) my points.removeItem (ipoint);
}

void AnyTier_removePointsBetween (AnyTier me, double tmin, double tmax) {
	if (my points.size == 0) return;
	long ileft = AnyTier_timeToHighIndex (me, tmin);
	long iright = AnyTier_timeToLowIndex (me, tmax);
	for (long i = iright; i >= ileft; i --)
		my points. removeItem (i);
}

autoPointProcess AnyTier_downto_PointProcess (AnyTier me) {
	try {
		long numberOfPoints = my points.size;
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfPoints);
		/* OPTIMIZATION, bypassing PointProcess_addTime: */
		for (long i = 1; i <= numberOfPoints; i ++)
			thy t [i] = my points.at [i] -> number;
		thy nt = numberOfPoints;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PointProcess.");
	}
}

/* End of file AnyTier.cpp */
