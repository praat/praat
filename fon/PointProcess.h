#ifndef _PointProcess_h_
#define _PointProcess_h_
/* PointProcess.h
 *
 * Copyright (C) 1992-2004 Paul Boersma
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
 * pb 2004/04/16
 */

#ifndef _Function_h_
	#include "Function.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#define PointProcess_members  Function_members \
	long maxnt, nt; \
	double *t;
#define PointProcess_methods  Function_methods
class_create (PointProcess, Function)

PointProcess PointProcess_create (double startingTime, double finishingTime, long initialMaxnt);
PointProcess PointProcess_createPoissonProcess (double startingTime, double finishingTime, double density);
int PointProcess_init (I, double startingTime, double finishingTime, long initialMaxnt);
long PointProcess_getLowIndex (PointProcess me, double t);
long PointProcess_getHighIndex (PointProcess me, double t);
long PointProcess_getNearestIndex (PointProcess me, double t);
long PointProcess_getWindowPoints (PointProcess me, double tmin, double tmax, long *imin, long *imax);
int PointProcess_addPoint (PointProcess me, double t);
long PointProcess_findPoint (PointProcess me, double t);
void PointProcess_removePoint (PointProcess me, long index);
void PointProcess_removePointNear (PointProcess me, double t);
void PointProcess_removePoints (PointProcess me, long first, long last);
void PointProcess_removePointsBetween (PointProcess me, double fromTime, double toTime);
void PointProcess_draw (PointProcess me, Graphics g, double fromTime, double toTime, int garnish);
double PointProcess_getInterval (PointProcess me, double t);
PointProcess PointProcesses_union (PointProcess me, PointProcess thee);
PointProcess PointProcesses_intersection (PointProcess me, PointProcess thee);
PointProcess PointProcesses_difference (PointProcess me, PointProcess thee);
int PointProcess_fill (PointProcess me, double tmin, double tmax, double period);
int PointProcess_voice (PointProcess me, double period, double maxT);

long PointProcess_getNumberOfPeriods (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getMeanPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);
double PointProcess_getStdevPeriod (PointProcess me, double tmin, double tmax,
	double minimumPeriod, double maximumPeriod, double maximumPeriodFactor);

/* End of file PointProcess.h */
#endif
