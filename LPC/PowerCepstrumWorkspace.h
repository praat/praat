#ifndef _PowerCepstrumWorkspace_h_
#define _PowerCepstrumWorkspace_h_
/* PowerCepstrumWorkspace_def.h
 *
 * Copyright (C) 2025 David Weenink
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


#include "PowerCepstrum.h"
#include "SlopeSelector.h"

#include "PowerCepstrumWorkspace_def.h"

void PowerCepstrumWorkspace_init (mutablePowerCepstrumWorkspace me, constPowerCepstrum thee, double qminFit, double qmaxFit,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);

autoPowerCepstrumWorkspace PowerCepstrumWorkspace_create (constPowerCepstrum thee, double qminSearchInterval, double qmaxSearchInterval,
	kCepstrum_trendType trendLineType, kCepstrum_trendFit method);

void PowerCepstrumWorkspace_initPeakSearchPart (mutablePowerCepstrumWorkspace me, double qminSearchInterval,
	double qmaxSearchInterval, kVector_peakInterpolation peakInterpolationType);

#endif / * PowerCepstrumWorkspace_h_ */
