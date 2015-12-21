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

Thing_define (AnyPoint, SimpleDouble) {
};

Thing_define (AnyTier, Function) {
	SortedSetOfDoubleOf <structAnyPoint> points;

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
void AnyTier_addPoint_move (AnyTier me, autoAnyPoint point);
void AnyTier_removePoint (structAnyTier& me, long i);
void AnyTier_removePointNear (structAnyTier& me, double time);
void AnyTier_removePointsBetween (structAnyTier& me, double tmin, double tmax);
autoPointProcess AnyTier_downto_PointProcess (structAnyTier& me);

#define AnyTier_METHODS \
	AnyTier asAnyTier () { \
		return reinterpret_cast <AnyTier> (this); \
	} \
	operator structAnyTier& () { return *this; } \
	void v_shiftX (double xfrom, double xto) \
		override { ((AnyTier) this) -> structAnyTier::v_shiftX (xfrom, xto); } \
	void v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) \
		override { ((AnyTier) this) -> structAnyTier::v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto); };

#endif
/* End of file AnyTier.h */
