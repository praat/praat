#ifndef _AnyTier_h_
#define _AnyTier_h_
/* AnyTier.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015-2017,2020 Paul Boersma
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

#include "Collection.h"
#include "PointProcess.h"

Thing_define (AnyPoint, SimpleDouble) {
};

Thing_define (AnyTier, Function) {
	SortedSetOfDoubleOf <structAnyPoint> points;

	void v_shiftX (double xfrom, double xto)
		override;
	void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto)
		override;
};

integer AnyTier_timeToLowIndex (AnyTier me, double time);

integer AnyTier_timeToHighIndex (AnyTier me, double time);

integer AnyTier_getWindowPoints (AnyTier me, double tmin, double tmax, integer *imin, integer *imax);

integer AnyTier_timeToNearestIndex (AnyTier me, double time);
integer AnyTier_timeToNearestIndexInIndexWindow (AnyTier me, double time, integer imin, integer imax);
integer AnyTier_timeToNearestIndexInTimeWindow (AnyTier me, double time, double tmin, double tmax);

integer AnyTier_hasPoint (AnyTier me, double t);

void AnyTier_addPoint_move (AnyTier me, autoAnyPoint point);

void AnyTier_removePoint (AnyTier me, integer i);

void AnyTier_removePointNear (AnyTier me, double time);

void AnyTier_removePointsBetween (AnyTier me, double tmin, double tmax);

autoPointProcess AnyTier_downto_PointProcess (AnyTier me);

#define AnyTier_METHODS \
	AnyTier asAnyTier () { return reinterpret_cast <AnyTier> (this); } \
	void v_shiftX (double xfrom, double xto) \
		override { ((AnyTier) this) -> structAnyTier::v_shiftX (xfrom, xto); } \
	void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) \
		override { ((AnyTier) this) -> structAnyTier::v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto); };

#endif
/* End of file AnyTier.h */
