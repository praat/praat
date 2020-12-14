/* AnyTier.cpp
 *
 * Copyright (C) 1992-2005,2007,2008,2011,2015-2018,2020 Paul Boersma
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
	for (integer i = 1; i <= our points.size; i ++) {
		AnyPoint point = our points.at [i];
		NUMshift (& point -> number, xfrom, xto);
	}
}

void structAnyTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	AnyTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (integer i = 1; i <= our points.size; i ++) {
		AnyPoint point = our points.at [i];
		NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

integer AnyTier_timeToLowIndex (AnyTier me, double time) {
	if (my points.size == 0)
		return 0;   // undefined
	integer ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (time < tleft)
		return 0;   // offleft
	double tright = my points.at [iright] -> number;
	if (time >= tright)
		return iright;
	Melder_assert (time >= tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		const integer imid = (ileft + iright) / 2;
		const double tmid = my points.at [imid] -> number;
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

integer AnyTier_timeToHighIndex (AnyTier me, double time) {
	if (my points.size == 0)
		return 0;   // undefined; is this right?
	integer ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (time <= tleft)
		return 1;
	double tright = my points.at [iright] -> number;
	if (time > tright)
		return iright + 1;   // offright
	Melder_assert (time > tleft && time <= tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		const integer imid = (ileft + iright) / 2;
		const double tmid = my points.at [imid] -> number;
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
	if (my points.size == 0)
		return 0;
	*imin = AnyTier_timeToHighIndex (me, tmin);
	*imax = AnyTier_timeToLowIndex (me, tmax);
	if (*imax < *imin)
		return 0;
	return *imax - *imin + 1;
}
	
integer AnyTier_timeToNearestIndexInIndexWindow (AnyTier me, double time, integer imin, integer imax) {
	Melder_assert (imin >= 1);
	Melder_assert (imax <= my points.size);
	if (imax < imin)
		return 0;   // undefined
	integer ileft = imin, iright = imax;
	double tleft = my points.at [ileft] -> number;
	if (time <= tleft)
		return ileft;
	double tright = my points.at [iright] -> number;
	if (time >= tright)
		return iright;
	Melder_assert (time > tleft && time < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		const integer imid = (ileft + iright) / 2;
		const double tmid = my points.at [imid] -> number;
		if (time < tmid) {
			iright = imid;
			tright = tmid;
		} else {
			ileft = imid;
			tleft = tmid;
		}
	}
	Melder_assert (iright == ileft + 1);
	Melder_assert (ileft >= imin);
	Melder_assert (iright <= imax);
	Melder_assert (time >= my points.at [ileft] -> number);
	Melder_assert (time <= my points.at [iright] -> number);
	return time - tleft <= tright - time ? ileft : iright;
}

integer AnyTier_timeToNearestIndex (AnyTier me, double time) {
	return AnyTier_timeToNearestIndexInIndexWindow (me, time, 1, my points.size);
}

integer AnyTier_timeToNearestIndexInTimeWindow (AnyTier me, double time, double tmin, double tmax) {
	integer imin, imax, n = AnyTier_getWindowPoints (me, tmin, tmax, & imin, & imax);
	return n == 0 ? 0 : AnyTier_timeToNearestIndexInIndexWindow (me, time, imin, imax);
}

integer AnyTier_hasPoint (AnyTier me, double t) {
	if (my points.size == 0)
		return 0;   // point not found
	integer ileft = 1, iright = my points.size;
	double tleft = my points.at [ileft] -> number;
	if (t < tleft)
		return 0;   // offleft
	double tright = my points.at [iright] -> number;
	if (t > tright)
		return 0;   // offright
	if (t == tleft)
		return 1;
	if (t == tright)
		return iright;
	Melder_assert (isdefined (t));
	Melder_assert (t > tleft && t < tright);
	Melder_assert (iright > ileft);
	while (iright > ileft + 1) {
		const integer imid = (ileft + iright) / 2;
		const double tmid = my points.at [imid] -> number;
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

void AnyTier_removePoint (AnyTier me, integer i) {
	if (i >= 1 && i <= my points.size)
		my points. removeItem (i);
}

void AnyTier_removePointNear (AnyTier me, double time) {
	integer ipoint = AnyTier_timeToNearestIndex (me, time);
	if (ipoint > 0)
		my points.removeItem (ipoint);
}

void AnyTier_removePointsBetween (AnyTier me, double tmin, double tmax) {
	if (my points.size == 0)
		return;
	integer ileft = AnyTier_timeToHighIndex (me, tmin);
	integer iright = AnyTier_timeToLowIndex (me, tmax);
	for (integer i = iright; i >= ileft; i --)
		my points. removeItem (i);
}

autoPointProcess AnyTier_downto_PointProcess (AnyTier me) {
	try {
		const integer numberOfPoints = my points.size;
		autoPointProcess thee = PointProcess_create (my xmin, my xmax, numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++)
			PointProcess_addPoint (thee.get(), my points.at [i] -> number);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PointProcess.");
	}
}

/* End of file AnyTier.cpp */
