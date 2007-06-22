/* AnyTier.c
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
 * pb 2001/07/18
 * pb 2002/07/16 GPL
 */

#include "AnyTier.h"

#include "oo_DESTROY.h"
#include "AnyTier_def.h"
#include "oo_COPY.h"
#include "AnyTier_def.h"
#include "oo_EQUAL.h"
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

class_methods (AnyPoint, Data)
	class_method_local (AnyPoint, destroy)
	class_method_local (AnyPoint, copy)
	class_method_local (AnyPoint, equal)
	class_method_local (AnyPoint, writeText)
	class_method_local (AnyPoint, readText)
	class_method_local (AnyPoint, writeBinary)
	class_method_local (AnyPoint, readBinary)
	class_method_local (AnyPoint, description)
class_methods_end

class_methods (AnyTier, Function)
	class_method_local (AnyTier, destroy)
	class_method_local (AnyTier, copy)
	class_method_local (AnyTier, equal)
	class_method_local (AnyTier, writeText)
	class_method_local (AnyTier, readText)
	class_method_local (AnyTier, writeBinary)
	class_method_local (AnyTier, readBinary)
	class_method_local (AnyTier, description)
class_methods_end

long AnyTier_timeToLowIndex (I, double time) {
	iam (AnyTier);
	long i;
	if (my points -> size == 0) return 0;
	for (i = 1; i <= my points -> size; i ++) {
		AnyPoint point = my points -> item [i];
		if (point -> time > time) return i - 1;   /* If i == 1: offleft. */
	}
	return my points -> size;
}

long AnyTier_timeToHighIndex (I, double time) {
	iam (AnyTier);
	long i;
	if (my points -> size == 0) return 0;   /* BUG ? Should it be 1? */
	for (i = 1; i <= my points -> size; i ++) {
		AnyPoint point = my points -> item [i];
		if (point -> time >= time) return i;
	}
	return my points -> size + 1;   /* Offright. */
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
	long i;
	if (my points -> size == 0) return 0;
	for (i = 1; i <= my points -> size; i ++) {
		AnyPoint point = my points -> item [i];
		if (point -> time >= time) {
			AnyPoint previousPoint;
			if (i == 1) return 1;
			previousPoint = my points -> item [i - 1];
			if (point -> time - time < time - previousPoint -> time)
				return i;
			else
				return i - 1;
		}
	}
	return my points -> size;
}

long AnyTier_hasPoint (I, double t) {
	iam (AnyTier);
	long ipoint;
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		AnyPoint point = my points -> item [ipoint];
		if (point -> time == t) {
			return ipoint;   /* Point found. */
		}
	}
	return 0;   /* Point not found. */
}

int AnyTier_addPoint (I, Any point) {
	iam (AnyTier);
	if (! point || ! Collection_addItem (my points, point)) return 0;
	return 1;
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
	long ileft, iright, i;
	if (my points -> size == 0) return;
	ileft = AnyTier_timeToHighIndex (me, tmin);
	iright = AnyTier_timeToLowIndex (me, tmax);
	for (i = iright; i >= ileft; i --)
		Collection_removeItem (my points, i);
}

PointProcess AnyTier_downto_PointProcess (I) {
	iam (AnyTier);
	long numberOfPoints = my points -> size, i;
	AnyPoint *points = (AnyPoint *) my points -> item;
	PointProcess thee = PointProcess_create (my xmin, my xmax, numberOfPoints);
	if (! thee) return NULL;
	/* OPTIMIZATION, bypassing PointProcess_addTime: */
	for (i = 1; i <= numberOfPoints; i ++)
		thy t [i] = points [i] -> time;
	thy nt = numberOfPoints;
	return thee;
}

/* End of file AnyTier.c */
