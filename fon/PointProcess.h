#ifndef _PointProcess_h_
#define _PointProcess_h_
/* PointProcess.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015-2020 Paul Boersma
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

#include "Function.h"
#include "Graphics.h"

#include "PointProcess_def.h"

autoPointProcess PointProcess_create (double startingTime, double finishingTime, integer initialMaxnt);
autoPointProcess PointProcess_createPoissonProcess (double startingTime, double finishingTime, double density);
void PointProcess_init (PointProcess me, double startingTime, double finishingTime, integer initialMaxnt);
integer PointProcess_getLowIndex (PointProcess me, double t);
integer PointProcess_getHighIndex (PointProcess me, double t);
integer PointProcess_getNearestIndex (PointProcess me, double t);
MelderIntegerRange PointProcess_getWindowPoints (PointProcess me, double tmin, double tmax);
void PointProcess_addPoint (PointProcess me, double t);
void PointProcess_addPoints (PointProcess me, constVECVU const& times);
integer PointProcess_findPoint (PointProcess me, double t);
void PointProcess_removePoint (PointProcess me, integer index);
void PointProcess_removePointNear (PointProcess me, double t);
void PointProcess_removePoints (PointProcess me, integer first, integer last);
void PointProcess_removePointsBetween (PointProcess me, double fromTime, double toTime);
void PointProcess_draw (PointProcess me, Graphics g, double fromTime, double toTime, bool garnish);
double PointProcess_getInterval (PointProcess me, double t);
autoPointProcess PointProcesses_union (PointProcess me, PointProcess thee);
autoPointProcess PointProcesses_intersection (PointProcess me, PointProcess thee);
autoPointProcess PointProcesses_difference (PointProcess me, PointProcess thee);
void PointProcess_fill (PointProcess me, double tmin, double tmax, double period);
void PointProcess_voice (PointProcess me, double period, double maxT);

integer PointProcess_getNumberOfPeriods (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getMeanPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getStdevPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
MelderCountAndFraction PointProcess_getCountAndFractionOfVoiceBreaks (PointProcess me, double tmin, double tmax,
	double maximumPeriod);

/* End of file PointProcess.h */
#endif
