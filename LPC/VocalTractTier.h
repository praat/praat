#ifndef _VocalTractTier_h_
#define _VocalTractTier_h_
/* VocalTractTier.h
 *
 * Copyright (C) 2012-2020 David Weenink
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
#include "LPC.h"
#include "VocalTract.h"

#include "VocalTractTier_def.h"

void VocalTract_drawSegments (VocalTract me, Graphics g, double maxLength, double maxArea, bool closedAtGlottis);

autoVocalTractPoint VocalTractPoint_create (VocalTract me, double time);

autoVocalTractTier VocalTractTier_create (double fromTime, double toTime);

autoVocalTractTier VocalTract_to_VocalTractTier (VocalTract me, double startTime, double endTime, double time);

void VocalTractTier_addVocalTract (VocalTractTier me, double time, VocalTract thee);

autoLPC VocalTractTier_to_LPC (VocalTractTier me, double timeStep);

autoVocalTract VocalTractTier_to_VocalTract (VocalTractTier me, double time);

/* End of file VocalTractTier.h */
#endif
