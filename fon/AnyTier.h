#ifndef _AnyTier_h_
#define _AnyTier_h_
/* AnyTier.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "Collection.h"
#include "PointProcess.h"

/*
#include "AnyTier_def.h"
oo_CLASS_CREATE (AnyPoint, SimpleDouble);
oo_CLASS_CREATE (AnyTier, Function);
*/

Thing_define (AnyPoint, SimpleDouble) {
};

Thing_define (AnyTier, Function) {
	autoSortedSetOfDouble points;

	void v_shiftX (double xfrom, double xto)
		override;
	void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
		override;
};

long AnyTier_timeToLowIndex (AnyTier me, double time);
long AnyTier_timeToHighIndex (AnyTier me, double time);
long AnyTier_getWindowPoints (AnyTier me, double tmin, double tmax, long *imin, long *imax);
long AnyTier_timeToNearestIndex (AnyTier me, double time);
long AnyTier_hasPoint (AnyTier me, double t);
void AnyTier_addPoint (AnyTier me, Daata point);
void AnyTier_removePoint (AnyTier me, long i);
void AnyTier_removePointNear (AnyTier me, double time);
void AnyTier_removePointsBetween (AnyTier me, double tmin, double tmax);
PointProcess AnyTier_downto_PointProcess (AnyTier me);

#endif
/* End of file AnyTier.h */
