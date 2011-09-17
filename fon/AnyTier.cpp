/* AnyTier.cpp
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "AnyTier.h"

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

Thing_implement (AnyPoint, SimpleDouble, 0);

Thing_implement (AnyTier, Function, 0);

void structAnyTier :: v_shiftX (double xfrom, double xto) {
	AnyTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= points -> size; i ++) {
		AnyPoint point = (AnyPoint) points -> item [i];
		NUMshift (& point -> number, xfrom, xto);
	}
}

void structAnyTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	AnyTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= points -> size; i ++) {
		AnyPoint point = (AnyPoint) points -> item [i];
		NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

long AnyTier_timeToLowIndex (I, double time) {
	iam (AnyTier);
	if (my points -> size == 0) return 0;   // undefined
	long ileft = 1, iright = my points -> size;
	AnyPoint *points = (AnyPoint *) my points -> item;
	double tleft = points [ileft] -> number;
	if (time < tleft) return 0;   // offleft
	double tright = points [iright] -> number;
	if (time >= tright) return iright;
	Melder_assert (time >= tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = points [imid] -> number;
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
	Melder_assert (iright <= my points -> size);
	Melder_assert (time >= points [ileft] -> number);
	Melder_assert (time <= points [iright] -> number);
	return ileft;
}

long AnyTier_timeToHighIndex (I, double time) {
	iam (AnyTier);
	if (my points -> size == 0) return 0;   // undefined; is this right?
	long ileft = 1, iright = my points -> size;
	AnyPoint *points = (AnyPoint *) my points -> item;
	double tleft = points [ileft] -> number;
	if (time <= tleft) return 1;
	double tright = points [iright] -> number;
	if (time > tright) return iright + 1;   // offright
	Melder_assert (time > tleft && time <= tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = points [imid] -> number;
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
	Melder_assert (iright <= my points -> size);
	Melder_assert (time >= points [ileft] -> number);
	Melder_assert (time <= points [iright] -> number);
	return iright;
}

long AnyTier_getWindowPoints (I, double tmin, double tmax, long *imin, long *imax) {
	iam (AnyTier);
	if (my points -> size == 0) return 0;
	*imin = AnyTier_timeToHighIndex (me, tmin);
	*imax = AnyTier_timeToLowIndex (me, tmax);
	if (*imax < *imin) return 0;
	return *imax - *imin + 1;
}
	
long AnyTier_timeToNearestIndex (I, double time) {
	iam (AnyTier);
	if (my points -> size == 0) return 0;   // undefined
	long ileft = 1, iright = my points -> size;
	AnyPoint *points = (AnyPoint *) my points -> item;
	double tleft = points [ileft] -> number;
	if (time <= tleft) return 1;
	double tright = points [iright] -> number;
	if (time >= tright) return iright;
	Melder_assert (time > tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = points [imid] -> number;
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
	Melder_assert (iright <= my points -> size);
	Melder_assert (time >= points [ileft] -> number);
	Melder_assert (time <= points [iright] -> number);
	return time - tleft <= tright - time ? ileft : iright;
}

long AnyTier_hasPoint (I, double t) {
	iam (AnyTier);
	if (my points -> size == 0) return 0;   // point not found
	long ileft = 1, iright = my points -> size;
	AnyPoint *points = (AnyPoint *) my points -> item;
	double tleft = points [ileft] -> number;
	if (t < tleft) return 0;   // offleft
	double tright = points [iright] -> number;
	if (t > tright) return 0;   // offright
	if (t == tleft) return 1;
	if (t == tright) return iright;
	Melder_assert (t > tleft && t < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		long imid = (ileft + iright) / 2;
		double tmid = points [imid] -> number;
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
	Melder_assert (iright <= my points -> size);
	Melder_assert (t > points [ileft] -> number);
	Melder_assert (t < points [iright] -> number);
	return 0;   /* Point not found. */
}

void AnyTier_addPoint (I, Data point) {
	iam (AnyTier);
	try {
		Collection_addItem (my points, point);
	} catch (MelderError) {
		Melder_throw (me, ": point not added.");
	}
}

void AnyTier_removePoint (I, long i) {
	iam (AnyTier);
	if (i >= 1 && i <= my points -> size) Collection_removeItem (my points, i);
}

void AnyTier_removePointNear (I, double time) {
	iam (AnyTier);
	long ipoint = AnyTier_timeToNearestIndex (me, time);
	if (ipoint) Collection_removeItem (my points, ipoint);
}

void AnyTier_removePointsBetween (I, double tmin, double tmax) {
	iam (AnyTier);
	if (my points -> size == 0) return;
	long ileft = AnyTier_timeToHighIndex (me, tmin);
	long iright = AnyTier_timeToLowIndex (me, tmax);
	for (long i = iright; i >= ileft; i --)
		Collection_removeItem (my points, i);
}

PointProcess AnyTier_downto_PointProcess (I) {
	iam (AnyTier);
	try {
		long numberOfPoints = my points -> size;
		AnyPoint *points = (AnyPoint *) my points -> item;
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfPoints);
		/* OPTIMIZATION, bypassing PointProcess_addTime: */
		for (long i = 1; i <= numberOfPoints; i ++)
			thy t [i] = points [i] -> number;
		thy nt = numberOfPoints;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to PointProcess.");
	}
}

/* End of file AnyTier.cpp */
