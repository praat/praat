/* PointProcess_and_Sound.h
 *
 * Copyright (C) 1992-2011,2015 Paul Boersma
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

#include "PointProcess.h"
#include "Sound.h"

autoSound PointProcess_to_Sound_pulseTrain (PointProcess me, double samplingFrequency,
	double adaptFactor, double adaptTime, integer interpolationDepth);
/*
	Function:
		create a time signal out of a point process.
	Preconditions:
		samplingFrequency > 0.0;
	Arguments:
		"adaptFactor" defaults to 1.
		"adaptTime" defaults to 0.
		"interpolationDepth" is the number of samples for sinxx interpolation in each direction.
*/

autoSound PointProcess_to_Sound_phonation
	(PointProcess me, double samplingFrequency, double adaptFactor, double maximumPeriod,
	 double openPhase, double collisionPhase, double power1, double power2);
#define PointProcess_to_Sound_phonation_DEFAULT_ADAPT_FACTOR  1.0
#define PointProcess_to_Sound_phonation_DEFAULT_MAXIMUM_PERIOD  0.05
#define PointProcess_to_Sound_phonation_DEFAULT_OPEN_PHASE  0.7
#define PointProcess_to_Sound_phonation_DEFAULT_COLLISION_PHASE  0.03
#define PointProcess_to_Sound_phonation_DEFAULT_POWER_1  3.0
#define PointProcess_to_Sound_phonation_DEFAULT_POWER_2  4.0

void PointProcess_playPart (PointProcess me, double tmin, double tmax);
void PointProcess_play (PointProcess me);
void PointProcess_hum (PointProcess me, double tmin, double tmax);
autoSound PointProcess_to_Sound_hum (PointProcess me);

/* End of file PointProcess_and_Sound.h */
