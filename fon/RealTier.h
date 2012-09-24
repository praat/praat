#ifndef _RealTier_h_
#define _RealTier_h_
/* RealTier.h
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

#include "AnyTier.h"
#include "Graphics.h"
#include "TableOfReal.h"
#include "Vector.h"
#include "Interpreter_decl.h"

/********** class RealPoint **********/

#include "RealTier_def.h"
oo_CLASS_CREATE (RealPoint, AnyPoint);

RealPoint RealPoint_create (double time, double value);
/*
	Postconditions:
		result -> time == time;
		result -> value == value;
*/

/********** class RealTier **********/

oo_CLASS_CREATE (RealTier, Function);

void RealTier_init (RealTier me, double tmin, double tmax);
RealTier RealTier_create (double tmin, double tmax);
RealTier RealTier_createWithClass (double tmin, double tmax, ClassInfo klas);
/*
	Postconditions:
		result -> xmin == tmin;
		result -> xmax == tmax;
		result -> points -> size == 0;
*/

double RealTier_getValueAtIndex (RealTier me, long point);
/* No points or 'point' out of range: NUMundefined. */

double RealTier_getValueAtTime (RealTier me, double t);
/* Inside points: linear intrapolation. */
/* Outside points: constant extrapolation. */
/* No points: NUMundefined. */

double RealTier_getMinimumValue (RealTier me);
double RealTier_getMaximumValue (RealTier me);
double RealTier_getArea (RealTier me, double tmin, double tmax);
double RealTier_getMean_curve (RealTier me, double tmin, double tmax);
double RealTier_getMean_points (RealTier me, double tmin, double tmax);
double RealTier_getStandardDeviation_curve (RealTier me, double tmin, double tmax);
double RealTier_getStandardDeviation_points (RealTier me, double tmin, double tmax);

void RealTier_addPoint (RealTier me, double t, double value);
void RealTier_draw (RealTier me, Graphics g, double tmin, double tmax,
	double ymin, double ymax, int garnish, const wchar_t *method, const wchar_t *quantity);
TableOfReal RealTier_downto_TableOfReal (RealTier me, const wchar_t *timeLabel, const wchar_t *valueLabel);

void RealTier_interpolateQuadratically (RealTier me, long numberOfPointsPerParabola, int logarithmically);

Table RealTier_downto_Table (RealTier me, const wchar_t *indexText, const wchar_t *timeText, const wchar_t *valueText);
RealTier Vector_to_RealTier (Vector me, long channel, ClassInfo klas);
RealTier Vector_to_RealTier_peaks (Vector me, long channel, ClassInfo klas);
RealTier Vector_to_RealTier_valleys (Vector me, long channel, ClassInfo klas);
RealTier PointProcess_upto_RealTier (PointProcess me, double value, ClassInfo klas);

void RealTier_formula (RealTier me, const wchar_t *expression, Interpreter interpreter, RealTier thee);
void RealTier_multiplyPart (RealTier me, double tmin, double tmax, double factor);
void RealTier_removePointsBelow (RealTier me, double level);

/* End of file RealTier.h */
#endif
