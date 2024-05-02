#ifndef _Sound_and_WarpedLPC_h_
#define _Sound_and_WarpedLPC_h_
/* Sound_and_WarpedLPC.h
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

#include "WarpedLPC.h"
#include "Sound.h"

autoWarpedLPC Sound_to_WarpedLPC_PLP (Sound me, int predictionOrder, double analysisWidth, double dt, double preEmphasisFrequency);

void Sound_into_WarpedLPC (Sound me, WarpedLPC thee, double analysisWidth, double preEmphasisFrequency, kWarpedLPC_Analysis method);

#endif /* _Sound_and_WarpedLPC_h_ */
