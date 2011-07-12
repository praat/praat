#ifndef _AnyTier_h_
#define _AnyTier_h_
/* AnyTier.h
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

/*
 * pb 2011/07/11
 */

#include "Collection.h"
#include "PointProcess.h"

#include "AnyTier_def.h"

#ifdef __cplusplus
	extern "C" {
#endif

#define AnyPoint__methods(klas) SimpleDouble__methods(klas)
oo_CLASS_CREATE (AnyPoint, SimpleDouble);

#define AnyTier__methods(klas) Function__methods(klas)
oo_CLASS_CREATE (AnyTier, Function);

long AnyTier_timeToLowIndex (I, double time);
long AnyTier_timeToHighIndex (I, double time);
long AnyTier_getWindowPoints (I, double tmin, double tmax, long *imin, long *imax);
long AnyTier_timeToNearestIndex (I, double time);
long AnyTier_hasPoint (I, double t);
void AnyTier_addPoint (I, Any point);
void AnyTier_removePoint (I, long i);
void AnyTier_removePointNear (I, double time);
void AnyTier_removePointsBetween (I, double tmin, double tmax);
PointProcess AnyTier_downto_PointProcess (I);

#ifdef __cplusplus
	}
#endif

#endif
/* End of file AnyTier.h */
