/* RealTier.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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
 * pb 2002/07/16 GPL
 * pb 2003/05/31 RealTier_formula
 * pb 2003/11/20 interpolate quadratically
 * pb 2005/03/02 RealTier_multiplyPart
 * pb 2007/01/27 Vector_to_RealTier_peaks finds peaks only within channel
 * pb 2007/01/28 made compatible with new getVector and getFunction1 API
 * pb 2007/03/30 RealTier_downto_Table: include point numbers
 * pb 2007/04/19 RealTier_formula: defence against undefined values
 * pb 2007/08/12 wchar_t
 * pb 2007/10/01 can write as encoding
 * pb 2008/04/30 new Formula API
 */

#include "RealTier.h"
#include "Formula.h"

#include "oo_DESTROY.h"
#include "RealTier_def.h"
#include "oo_COPY.h"
#include "RealTier_def.h"
#include "oo_EQUAL.h"
#include "RealTier_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "RealTier_def.h"
#include "oo_WRITE_TEXT.h"
#include "RealTier_def.h"
#include "oo_READ_TEXT.h"
#include "RealTier_def.h"
#include "oo_WRITE_BINARY.h"
#include "RealTier_def.h"
#include "oo_READ_BINARY.h"
#include "RealTier_def.h"
#include "oo_DESCRIPTION.h"
#include "RealTier_def.h"

/********** class RealPoint **********/

class_methods (RealPoint, Data) {
	class_method_local (RealPoint, destroy)
	class_method_local (RealPoint, copy)
	class_method_local (RealPoint, equal)
	class_method_local (RealPoint, canWriteAsEncoding)
	class_method_local (RealPoint, writeText)
	class_method_local (RealPoint, readText)
	class_method_local (RealPoint, writeBinary)
	class_method_local (RealPoint, readBinary)
	class_method_local (RealPoint, description)
	class_methods_end
}

RealPoint RealPoint_create (double time, double value) {
	RealPoint me = new (RealPoint); cherror
	my time = time;
	my value = value;
end:
	return me;
}

/********** class RealTier **********/

static void info (I) {
	iam (RealTier);
	classFunction -> info (me);
	MelderInfo_writeLine2 (L"Number of points: ", Melder_integer (my points -> size));
	MelderInfo_writeLine2 (L"Minimum value: ", Melder_double (RealTier_getMinimumValue (me)));
	MelderInfo_writeLine2 (L"Maximum value: ", Melder_double (RealTier_getMaximumValue (me)));
}

static double getNx (I) { iam (RealTier); return my points -> size; }
static double getX (I, long ix) { iam (RealTier); return ((RealPoint) my points -> item [ix]) -> time; }
static double getNcol (I) { iam (RealTier); return my points -> size; }
static double getVector (I, long irow, long icol) { iam (RealTier); (void) irow; return RealTier_getValueAtIndex (me, icol); }
static double getFunction1 (I, long irow, double x) { iam (RealTier); (void) irow; return RealTier_getValueAtTime (me, x); }

static const wchar_t * getUnitText (I, long ilevel, int unit, unsigned long flags) {
	(void) void_me;
	(void) ilevel;
	(void) unit;
	(void) flags;
	return L"Time (s)";
}

class_methods (RealTier, Function) {
	class_method_local (RealTier, destroy)
	class_method_local (RealTier, copy)
	class_method_local (RealTier, equal)
	class_method_local (RealTier, canWriteAsEncoding)
	class_method_local (RealTier, writeText)
	class_method_local (RealTier, readText)
	class_method_local (RealTier, writeBinary)
	class_method_local (RealTier, readBinary)
	class_method_local (RealTier, description)
	class_method (info)
	class_method (getNx)
	class_method (getX)
	class_method (getNcol)
	class_method (getVector)
	class_method (getFunction1)
	class_method (getUnitText)
	class_methods_end
}

void RealTier_init_e (I, double tmin, double tmax) {
	iam (RealTier);
	my xmin = tmin;
	my xmax = tmax;
	my points = SortedSetOfDouble_create (); cherror
end:
	return;
}

RealTier RealTier_create (double tmin, double tmax) {
	RealTier me = new (RealTier); cherror
	RealTier_init_e (me, tmin, tmax); cherror
end:
	iferror forget (me);
	return me;
}

int RealTier_addPoint (I, double t, double value) {
	iam (RealTier);
	RealPoint point = RealPoint_create (t, value); cherror
	Collection_addItem (my points, point); cherror
end:
	iferror return 0;
	return 1;
}

double RealTier_getValueAtIndex (I, long i) {
	iam (RealTier);
	if (i < 1 || i > my points -> size) return NUMundefined;
	return ((RealPoint) my points -> item [i]) -> value;
}

double RealTier_getValueAtTime (I, double t) {
	iam (RealTier);
	long n = my points -> size, ileft, iright;
	double tleft, tright, fleft, fright;
	RealPoint pointLeft, pointRight;
	if (n == 0) return NUMundefined;
	pointRight = my points -> item [1];
	if (t <= pointRight -> time) return pointRight -> value;   /* Constant extrapolation. */
	pointLeft = my points -> item [n];
	if (t >= pointLeft -> time) return pointLeft -> value;   /* Constant extrapolation. */
	Melder_assert (n >= 2);
	ileft = AnyTier_timeToLowIndex (me, t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = my points -> item [ileft];
	pointRight = my points -> item [iright];
	tleft = pointLeft -> time, fleft = pointLeft -> value;
	tright = pointRight -> time, fright = pointRight -> value;
	return t == tright ? fright   /* Be very accurate. */
		: tleft == tright ? 0.5 * (fleft + fright)   /* Unusual, but possible; no preference. */
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   /* Linear interpolation. */
}

double RealTier_getMaximumValue (I) {
	iam (RealTier);
	double result = NUMundefined;
	long n = my points -> size;
	for (long i = 1; i <= n; i ++) {
		RealPoint point = my points -> item [i];
		if (result == NUMundefined || point -> value > result)
			result = point -> value;
	}
	return result;
}

double RealTier_getMinimumValue (I) {
	iam (RealTier);
	double result = NUMundefined;
	long n = my points -> size;
	for (long i = 1; i <= n; i ++) {
		RealPoint point = my points -> item [i];
		if (result == NUMundefined || point -> value < result)
			result = point -> value;
	}
	return result;
}

double RealTier_getArea (I, double tmin, double tmax) {
	iam (RealTier);
	long n = my points -> size, imin, imax;
	RealPoint *points = (RealPoint *) my points -> item;
	if (n == 0) return NUMundefined;
	if (n == 1) return (tmax - tmin) * points [1] -> value;
	imin = AnyTier_timeToLowIndex (me, tmin);
	if (imin == n) return (tmax - tmin) * points [n] -> value;
	imax = AnyTier_timeToHighIndex (me, tmax);
	if (imax == 1) return (tmax - tmin) * points [1] -> value;
	Melder_assert (imin < n);
	Melder_assert (imax > 1);
	/*
	 * Sum the areas between the points.
	 * This works even if imin is 0 (offleft) and/or imax is n + 1 (offright).
	 */
	double area = 0.0;
	for (long i = imin; i < imax; i ++) {
		double tleft, fleft, tright, fright;
		if (i == imin) tleft = tmin, fleft = RealTier_getValueAtTime (me, tmin);
		else tleft = points [i] -> time, fleft = points [i] -> value;
		if (i + 1 == imax) tright = tmax, fright = RealTier_getValueAtTime (me, tmax);
		else tright = points [i + 1] -> time, fright = points [i + 1] -> value;
		area += 0.5 * (fleft + fright) * (tright - tleft);
	}
	return area;
}

double RealTier_getMean_curve (I, double tmin, double tmax) {
	iam (RealTier);
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	double area = RealTier_getArea (me, tmin, tmax);
	if (area == NUMundefined) return NUMundefined;
	return area / (tmax - tmin);
}

double RealTier_getStandardDeviation_curve (I, double tmin, double tmax) {
	iam (RealTier);
	long n = my points -> size, imin, imax;
	RealPoint *points = (RealPoint *) my points -> item;
	double mean, integral = 0.0;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	if (n == 0) return NUMundefined;
	if (n == 1) return 0.0;
	imin = AnyTier_timeToLowIndex (me, tmin);
	if (imin == n) return 0.0;
	imax = AnyTier_timeToHighIndex (me, tmax);
	if (imax == 1) return 0.0;
	Melder_assert (imin < n);
	Melder_assert (imax > 1);
	/*
	 * Add the areas between the points.
	 * This works even if imin is 0 (offleft) and/or imax is n + 1 (offright).
	 */
	mean = RealTier_getMean_curve (me, tmin, tmax);
	for (long i = imin; i < imax; i ++) {
		double tleft, fleft, tright, fright, sum, diff;
		if (i == imin) tleft = tmin, fleft = RealTier_getValueAtTime (me, tmin);
		else tleft = points [i] -> time, fleft = points [i] -> value - mean;
		if (i + 1 == imax) tright = tmax, fright = RealTier_getValueAtTime (me, tmax);
		else tright = points [i + 1] -> time, fright = points [i + 1] -> value - mean;
		/*
		 * The area is integral dt f^2
		 *   = integral dt [f1 + (f2-f1)/(t2-t1) (t-t1)]^2
		 *   = int dt f1^2 + int dt 2 f1 (f2-f1)/(t2-t1) (t-t1) + int dt [(f2-f1)/(t2-t1)]^2 (t-t1)^2
		 *   = f1^2 (t2-t1) + f1 (f2-f1)/(t2-t1) (t2-t1)^2 + 1/3 [(f2-f1)/(t2-t1)]^2 (t2-t1)^3
		 *   = (t2-t1) [f1 f2 + 1/3 (f2-f1)^2]
		 *   = (t2-t1) (f1^2 + f2^2 + 1/3 f1 f2)
		 *   = (t2-t1) [1/4 (f1+f2)^2 + 1/12 (f1-f2)^2]
		 * In the last expression, we have a sum of squares, which is computationally best.
		 */
		sum = fleft + fright;
		diff = fleft - fright;
		integral += (sum * sum + (1.0/3.0) * diff * diff) * (tright - tleft);
	}
	return sqrt (0.25 * integral / (tmax - tmin));
}

double RealTier_getMean_points (I, double tmin, double tmax) {
	iam (RealTier);
	long n = my points -> size, imin, imax;
	double sum = 0.0;
	RealPoint *points = (RealPoint *) my points -> item;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	n = AnyTier_getWindowPoints (me, tmin, tmax, & imin, & imax);
	if (n == 0) return NUMundefined;
	for (long i = imin; i <= imax; i ++)
		sum += points [i] -> value;
	return sum / n;
}

double RealTier_getStandardDeviation_points (I, double tmin, double tmax) {
	iam (RealTier);
	long n = my points -> size, imin, imax;
	double mean, sum = 0.0;
	RealPoint *points = (RealPoint *) my points -> item;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	n = AnyTier_getWindowPoints (me, tmin, tmax, & imin, & imax);
	if (n < 2) return NUMundefined;
	mean = RealTier_getMean_points (me, tmin, tmax);
	for (long i = imin; i <= imax; i ++) {
		double diff = points [i] -> value - mean;
		sum += diff * diff;
	}
	return sqrt (sum / (n - 1));
}

void RealTier_multiplyPart (I, double tmin, double tmax, double factor) {
	iam (RealTier);
	long ipoint;
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		RealPoint point = my points -> item [ipoint];
		double t = point -> time;
		if (t >= tmin && t <= tmax) {
			point -> value *= factor;
		}
	}
}

void RealTier_draw (I, Graphics g, double tmin, double tmax, double fmin, double fmax,
	int garnish, const wchar_t *quantity)
{
	iam (RealTier);
	long n = my points -> size, imin, imax, i;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_setInner (g);
	imin = AnyTier_timeToHighIndex (me, tmin);
	imax = AnyTier_timeToLowIndex (me, tmax);
	if (n == 0) {
	} else if (imax < imin) {
		double fleft = RealTier_getValueAtTime (me, tmin);
		double fright = RealTier_getValueAtTime (me, tmax);
		Graphics_line (g, tmin, fleft, tmax, fright);
	} else for (i = imin; i <= imax; i ++) {
		RealPoint point = my points -> item [i];
		double t = point -> time, f = point -> value;
		Graphics_fillCircle_mm (g, t, f, 1);
		if (i == 1)
			Graphics_line (g, tmin, f, t, f);
		else if (i == imin)
			Graphics_line (g, t, f, tmin, RealTier_getValueAtTime (me, tmin));
		if (i == n)
			Graphics_line (g, t, f, tmax, f);
		else if (i == imax)
			Graphics_line (g, t, f, tmax, RealTier_getValueAtTime (me, tmax));
		else {
			RealPoint pointRight = my points -> item [i + 1];
			Graphics_line (g, t, f, pointRight -> time, pointRight -> value);
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, TRUE, L"Time (s)");
		Graphics_marksBottom (g, 2, TRUE, TRUE, FALSE);
		Graphics_marksLeft (g, 2, TRUE, TRUE, FALSE);
		if (quantity) Graphics_textLeft (g, TRUE, quantity);
	}
}

TableOfReal RealTier_downto_TableOfReal (I, const wchar_t *timeLabel, const wchar_t *valueLabel) {
	iam (RealTier);
	TableOfReal thee = TableOfReal_create (my points -> size, 2); cherror
	TableOfReal_setColumnLabel (thee, 1, timeLabel); cherror
	TableOfReal_setColumnLabel (thee, 2, valueLabel); cherror
	for (long i = 1; i <= my points -> size; i ++) {
		RealPoint point = my points -> item [i];
		thy data [i] [1] = point -> time;
		thy data [i] [2] = point -> value;
	}
end:
	iferror forget (thee);
	return thee;
}

int RealTier_interpolateQuadratically (I, long numberOfPointsPerParabola, int logarithmically) {
	iam (RealTier);
	RealTier thee = Data_copy (me); cherror
	for (long ipoint = 1; ipoint < my points -> size; ipoint ++) {
		RealPoint point1 = my points -> item [ipoint], point2 = my points -> item [ipoint + 1];
		double time1 = point1 -> time, time2 = point2 -> time, tmid = 0.5 * (time1 + time2);
		double value1 = point1 -> value, value2 = point2 -> value, valuemid;
		double timeStep = (tmid - time1) / (numberOfPointsPerParabola + 1);
		if (logarithmically) value1 = log (value1), value2 = log (value2);
		valuemid = 0.5 * (value1 + value2);
		/*
		 * Left from the midpoint.
		 */
		for (long inewpoint = 1; inewpoint <= numberOfPointsPerParabola; inewpoint ++) {
			double newTime = time1 + inewpoint * timeStep;
			double phase = (newTime - time1) / (tmid - time1);
			double newValue = value1 + (valuemid - value1) * phase * phase;
			if (logarithmically) newValue = exp (newValue);
			RealTier_addPoint (thee, newTime, newValue); cherror
		}
		/*
		 * The midpoint.
		 */
		RealTier_addPoint (thee, tmid, logarithmically ? exp (valuemid) : valuemid); cherror
		/*
		 * Right from the midpoint.
		 */
		for (long inewpoint = 1; inewpoint <= numberOfPointsPerParabola; inewpoint ++) {
			double newTime = tmid + inewpoint * timeStep;
			double phase = (time2 - newTime) / (time2 - tmid);
			double newValue = value2 + (valuemid - value2) * phase * phase;
			if (logarithmically) newValue = exp (newValue);
			RealTier_addPoint (thee, newTime, newValue); cherror
		}
	}
end:
	iferror { forget (thee); return 0; }
	Thing_swap (me, thee);
	forget (thee);
	return 1;
}

Table RealTier_downto_Table (I, const wchar_t *indexText, const wchar_t *timeText, const wchar_t *valueText) {
	iam (RealTier);
	Table thee = Table_createWithoutColumnNames (my points -> size,
		(indexText != NULL) + (timeText != NULL) + (valueText != NULL)); cherror
	long icol = 0;
	if (indexText != NULL) { Table_setColumnLabel (thee, ++ icol, indexText); cherror }
	if (timeText != NULL) { Table_setColumnLabel (thee, ++ icol, timeText); cherror }
	if (valueText != NULL) { Table_setColumnLabel (thee, ++ icol, valueText); cherror }
	for (long ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		RealPoint point = my points -> item [ipoint];
		icol = 0;
		if (indexText != NULL) { Table_setNumericValue (thee, ipoint, ++ icol, ipoint); cherror }
		if (timeText != NULL) { Table_setNumericValue (thee, ipoint, ++ icol, point -> time); cherror }
		if (valueText != NULL) { Table_setNumericValue (thee, ipoint, ++ icol, point -> value); cherror }
	}
end:
	iferror forget (thee);
	return thee;
}

RealTier Vector_to_RealTier (I, long channel) {
	iam (Vector);
	RealTier thee = RealTier_create (my xmin, my xmax); cherror
	for (long i = 1; i <= my nx; i ++) {
		RealTier_addPoint (thee, Sampled_indexToX (me, i), my z [channel] [i]); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

RealTier Vector_to_RealTier_peaks (I, long channel) {
	iam (Vector);
	RealTier thee = RealTier_create (my xmin, my xmax); cherror
	for (long i = 2; i < my nx; i ++) {
		double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
		if (left <= centre && right < centre) {
			double x, maximum;
			Vector_getMaximumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
				channel, NUM_PEAK_INTERPOLATE_PARABOLIC, & maximum, & x);
			RealTier_addPoint (thee, x, maximum); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

RealTier Vector_to_RealTier_valleys (I, long channel) {
	iam (Vector);
	RealTier thee = RealTier_create (my xmin, my xmax); cherror
	for (long i = 2; i < my nx; i ++) {
		double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
		if (left >= centre && right > centre) {
			double x, minimum;
			Vector_getMinimumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
				channel, NUM_PEAK_INTERPOLATE_PARABOLIC, & minimum, & x);
			RealTier_addPoint (thee, x, minimum); cherror
		}
	}
end:
	iferror forget (thee);
	return thee;
}

RealTier PointProcess_upto_RealTier (PointProcess me, double value) {
	RealTier thee = RealTier_create (my xmin, my xmax); cherror
	for (long i = 1; i <= my nt; i ++) {
		RealTier_addPoint (thee, my t [i], value); cherror
	}
end:
	iferror forget (thee);
	return thee;
}

int RealTier_formula (I, const wchar_t *expression, thou) {
	iam (RealTier);
	thouart (RealTier);
	Formula_compile (NULL, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE); cherror
	if (thee == NULL) thee = me;
	for (long icol = 1; icol <= my points -> size; icol ++) {
		struct Formula_Result result;
		Formula_run (0, icol, & result); cherror
		if (result. result.numericResult == NUMundefined)
			error1 (L"Cannot put an undefined value into the tier.\nFormula not finished.")
		((RealPoint) thy points -> item [icol]) -> value = result. result.numericResult;
	}
end:
	iferror return 0;
	return 1;
}

void RealTier_removePointsBelow (RealTier me, double level) {
	for (long ipoint = my points -> size; ipoint > 0; ipoint --) {
		RealPoint point = my points -> item [ipoint];
		if (point -> value < level) {
			AnyTier_removePoint (me, ipoint);
		}
	}
}

/* End of file RealTier.c */
