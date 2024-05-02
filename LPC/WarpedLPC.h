#ifndef _WarpedLPC_h_
#define _WarpedLPC_h_
/* WarpedLPC.h
 *
 * Copyright (C) 2024 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Sound_and_LPC.h"
#include "WarpedLPC_enums.h"

#include "WarpedLPC_def.h"

void WarpedLPC_init (WarpedLPC me, double tmin, double tmax, integer nt, double dt, double t1, integer predictionOrder, double samplingPeriod);

autoWarpedLPC WarpedLPC_create (double tmin, double tmax, integer nt, double dt, double t1, integer predictionOrder, double samplingPeriod);

autoWarpedLPC Sound_to_WarpedLPC (Sound me, double tmin, double tmax, integer nt, double dt, double t1, integer predictionOrder, double samplingPeriod);


#endif /* _WarpedLPC_h_ */
