#ifndef _VocalTractTier_h_
#define _VocalTractTier_h_
/* VocalTractTier.h
 *
 * Copyright (C) 2012 David Weenink
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
#include "LPC.h"
#include "VocalTract.h"

#include "VocalTractTier_def.h"
oo_CLASS_CREATE (VocalTractPoint, AnyPoint);
oo_CLASS_CREATE (VocalTractTier, Function);

void VocalTract_drawSegments (VocalTract me, Graphics g, double maxLength, double maxArea, bool closedAtGlottis);

VocalTractPoint VocalTract_to_VocalTractPoint (VocalTract me, double time);

VocalTractTier VocalTractTier_create (double fromTime, double toTime);
VocalTractTier VocalTract_to_VocalTractTier (VocalTract me, double startTime, double endTime, double time);

void VocalTractTier_addVocalTract (VocalTractTier me, double time, VocalTract thee);

LPC VocalTractTier_to_LPC (VocalTractTier me, double timeStep);
VocalTract VocalTractTier_to_VocalTract (VocalTractTier me, double time);

/* End of file VocalTractTier.h */
#endif
