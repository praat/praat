/* RealTier.cpp
 *
 * Copyright (C) 1992-2012 Paul Boersma
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

Thing_implement (RealPoint, AnyPoint, 0);

RealPoint RealPoint_create (double time, double value) {
	autoRealPoint me = Thing_new (RealPoint);
	my number = time;
	my value = value;
	return me.transfer();
}

/********** class RealTier **********/

void structRealTier :: v_info () {
	structFunction :: v_info ();
	MelderInfo_writeLine (L"Number of points: ", Melder_integer (points -> size));
	MelderInfo_writeLine (L"Minimum value: ", Melder_double (RealTier_getMinimumValue (this)));
	MelderInfo_writeLine (L"Maximum value: ", Melder_double (RealTier_getMaximumValue (this)));
}

double structRealTier :: v_getVector (long irow, long icol) {
	(void) irow;
	return RealTier_getValueAtIndex (this, icol);
}

double structRealTier :: v_getFunction1 (long irow, double x) {
	(void) irow;
	return RealTier_getValueAtTime (this, x);
}

void structRealTier :: v_shiftX (double xfrom, double xto) {
	RealTier_Parent :: v_shiftX (xfrom, xto);
	for (long i = 1; i <= points -> size; i ++) {
		RealPoint point = (RealPoint) points -> item [i];
		NUMshift (& point -> number, xfrom, xto);
	}
}

void structRealTier :: v_scaleX (double xminfrom, double xmaxfrom, double xminto, double xmaxto) {
	RealTier_Parent :: v_scaleX (xminfrom, xmaxfrom, xminto, xmaxto);
	for (long i = 1; i <= points -> size; i ++) {
		RealPoint point = (RealPoint) points -> item [i];
		NUMscale (& point -> number, xminfrom, xmaxfrom, xminto, xmaxto);
	}
}

Thing_implement (RealTier, Function, 0);

void RealTier_init (RealTier me, double tmin, double tmax) {
	my xmin = tmin;
	my xmax = tmax;
	my points = SortedSetOfDouble_create ();
}

RealTier RealTier_create (double tmin, double tmax) {
	try {
		autoRealTier me = Thing_new (RealTier);
		RealTier_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("RealTier not created.");
	}
}

RealTier RealTier_createWithClass (double tmin, double tmax, ClassInfo klas) {
	try {
		autoRealTier me = (RealTier) _Thing_new (klas);
		RealTier_init (me.peek(), tmin, tmax);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw (klas -> className, " not created.");
	}
}

void RealTier_addPoint (RealTier me, double t, double value) {
	try {
		autoRealPoint point = RealPoint_create (t, value);
		Collection_addItem (my points, point.transfer());
	} catch (MelderError) {
		Melder_throw (me, ": point not added.");
	}
}

double RealTier_getValueAtIndex (RealTier me, long i) {
	if (i < 1 || i > my points -> size) return NUMundefined;
	return ((RealPoint) my points -> item [i]) -> value;
}

double RealTier_getValueAtTime (RealTier me, double t) {
	long n = my points -> size;
	if (n == 0) return NUMundefined;
	RealPoint pointRight = (RealPoint) my points -> item [1];
	if (t <= pointRight -> number) return pointRight -> value;   /* Constant extrapolation. */
	RealPoint pointLeft = (RealPoint) my points -> item [n];
	if (t >= pointLeft -> number) return pointLeft -> value;   /* Constant extrapolation. */
	Melder_assert (n >= 2);
	long ileft = AnyTier_timeToLowIndex (me, t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	pointLeft = (RealPoint) my points -> item [ileft];
	pointRight = (RealPoint) my points -> item [iright];
	double tleft = pointLeft -> number, fleft = pointLeft -> value;
	double tright = pointRight -> number, fright = pointRight -> value;
	return t == tright ? fright   /* Be very accurate. */
		: tleft == tright ? 0.5 * (fleft + fright)   /* Unusual, but possible; no preference. */
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   /* Linear interpolation. */
}

double RealTier_getMaximumValue (RealTier me) {
	double result = NUMundefined;
	long n = my points -> size;
	for (long i = 1; i <= n; i ++) {
		RealPoint point = (RealPoint) my points -> item [i];
		if (result == NUMundefined || point -> value > result)
			result = point -> value;
	}
	return result;
}

double RealTier_getMinimumValue (RealTier me) {
	double result = NUMundefined;
	long n = my points -> size;
	for (long i = 1; i <= n; i ++) {
		RealPoint point = (RealPoint) my points -> item [i];
		if (result == NUMundefined || point -> value < result)
			result = point -> value;
	}
	return result;
}

double RealTier_getArea (RealTier me, double tmin, double tmax) {
	long n = my f_getNumberOfPoints (), imin, imax;
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
		else tleft = points [i] -> number, fleft = points [i] -> value;
		if (i + 1 == imax) tright = tmax, fright = RealTier_getValueAtTime (me, tmax);
		else tright = points [i + 1] -> number, fright = points [i + 1] -> value;
		area += 0.5 * (fleft + fright) * (tright - tleft);
	}
	return area;
}

double RealTier_getMean_curve (RealTier me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	double area = RealTier_getArea (me, tmin, tmax);
	if (area == NUMundefined) return NUMundefined;
	return area / (tmax - tmin);
}

double RealTier_getStandardDeviation_curve (RealTier me, double tmin, double tmax) {
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
		else tleft = points [i] -> number, fleft = points [i] -> value - mean;
		if (i + 1 == imax) tright = tmax, fright = RealTier_getValueAtTime (me, tmax);
		else tright = points [i + 1] -> number, fright = points [i + 1] -> value - mean;
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

double RealTier_getMean_points (RealTier me, double tmin, double tmax) {
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

double RealTier_getStandardDeviation_points (RealTier me, double tmin, double tmax) {
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

void RealTier_multiplyPart (RealTier me, double tmin, double tmax, double factor) {
	long ipoint;
	for (ipoint = 1; ipoint <= my points -> size; ipoint ++) {
		RealPoint point = (RealPoint) my points -> item [ipoint];
		double t = point -> number;
		if (t >= tmin && t <= tmax) {
			point -> value *= factor;
		}
	}
}

void RealTier_draw (RealTier me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	int garnish, const wchar_t *method, const wchar_t *quantity)
{
	bool drawLines = wcsstr (method, L"lines") || wcsstr (method, L"Lines");
	bool drawSpeckles = wcsstr (method, L"speckles") || wcsstr (method, L"Speckles");
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
		if (drawLines) Graphics_line (g, tmin, fleft, tmax, fright);
	} else for (i = imin; i <= imax; i ++) {
		RealPoint point = (RealPoint) my points -> item [i];
		double t = point -> number, f = point -> value;
		if (drawSpeckles) Graphics_fillCircle_mm (g, t, f, 1);
		if (drawLines) {
			if (i == 1)
				Graphics_line (g, tmin, f, t, f);
			else if (i == imin)
				Graphics_line (g, t, f, tmin, RealTier_getValueAtTime (me, tmin));
			if (i == n)
				Graphics_line (g, t, f, tmax, f);
			else if (i == imax)
				Graphics_line (g, t, f, tmax, RealTier_getValueAtTime (me, tmax));
			else {
				RealPoint pointRight = (RealPoint) my points -> item [i + 1];
				Graphics_line (g, t, f, pointRight -> number, pointRight -> value);
			}
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

TableOfReal RealTier_downto_TableOfReal (RealTier me, const wchar_t *timeLabel, const wchar_t *valueLabel) {
	try {
		autoTableOfReal thee = TableOfReal_create (my points -> size, 2);
		TableOfReal_setColumnLabel (thee.peek(), 1, timeLabel);
		TableOfReal_setColumnLabel (thee.peek(), 2, valueLabel);
		for (long i = 1; i <= my points -> size; i ++) {
			RealPoint point = (RealPoint) my points -> item [i];
			thy data [i] [1] = point -> number;
			thy data [i] [2] = point -> value;
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to TableOfReal.");
	}
}

void RealTier_interpolateQuadratically (RealTier me, long numberOfPointsPerParabola, int logarithmically) {
	try {
		autoRealTier thee = Data_copy (me);
		for (long ipoint = 1; ipoint < my points -> size; ipoint ++) {
			RealPoint point1 = (RealPoint) my points -> item [ipoint], point2 = (RealPoint) my points -> item [ipoint + 1];
			double time1 = point1 -> number, time2 = point2 -> number, tmid = 0.5 * (time1 + time2);
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
				RealTier_addPoint (thee.peek(), newTime, newValue);
			}
			/*
			 * The midpoint.
			 */
			RealTier_addPoint (thee.peek(), tmid, logarithmically ? exp (valuemid) : valuemid);
			/*
			 * Right from the midpoint.
			 */
			for (long inewpoint = 1; inewpoint <= numberOfPointsPerParabola; inewpoint ++) {
				double newTime = tmid + inewpoint * timeStep;
				double phase = (time2 - newTime) / (time2 - tmid);
				double newValue = value2 + (valuemid - value2) * phase * phase;
				if (logarithmically) newValue = exp (newValue);
				RealTier_addPoint (thee.peek(), newTime, newValue);
			}
		}
		Thing_swap (me, thee.peek());
	} catch (MelderError) {
		Melder_throw (me, ": not interpolated quadratically.");
	}
}

Table RealTier_downto_Table (RealTier me, const wchar_t *indexText, const wchar_t *timeText, const wchar_t *valueText) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my points -> size,
			(indexText != NULL) + (timeText != NULL) + (valueText != NULL));
		long icol = 0;
		if (indexText != NULL) Table_setColumnLabel (thee.peek(), ++ icol, indexText);
		if (timeText  != NULL) Table_setColumnLabel (thee.peek(), ++ icol, timeText);
		if (valueText != NULL) Table_setColumnLabel (thee.peek(), ++ icol, valueText);
		for (long ipoint = 1; ipoint <= my points -> size; ipoint ++) {
			RealPoint point = my f_peekPoint (ipoint);
			icol = 0;
			if (indexText != NULL) Table_setNumericValue (thee.peek(), ipoint, ++ icol, ipoint);
			if (timeText != NULL)  Table_setNumericValue (thee.peek(), ipoint, ++ icol, point -> number);
			if (valueText != NULL) Table_setNumericValue (thee.peek(), ipoint, ++ icol, point -> value);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Table.");
	}
}

RealTier Vector_to_RealTier (Vector me, long channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (long i = 1; i <= my nx; i ++) {
			RealTier_addPoint (thee.peek(), Sampled_indexToX (me, i), my z [channel] [i]);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to ", klas -> className, ".");
	}
}

RealTier Vector_to_RealTier_peaks (Vector me, long channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (long i = 2; i < my nx; i ++) {
			double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
			if (left <= centre && right < centre) {
				double x, maximum;
				Vector_getMaximumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
					channel, NUM_PEAK_INTERPOLATE_PARABOLIC, & maximum, & x);
				RealTier_addPoint (thee.peek(), x, maximum);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to ", klas -> className, " (peaks).");
	}
}

RealTier Vector_to_RealTier_valleys (Vector me, long channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (long i = 2; i < my nx; i ++) {
			double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
			if (left >= centre && right > centre) {
				double x, minimum;
				Vector_getMinimumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
					channel, NUM_PEAK_INTERPOLATE_PARABOLIC, & minimum, & x);
				RealTier_addPoint (thee.peek(), x, minimum);
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to ", klas -> className, " (valleys).");
	}
}

RealTier PointProcess_upto_RealTier (PointProcess me, double value, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (long i = 1; i <= my nt; i ++) {
			RealTier_addPoint (thee.peek(), my t [i], value);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to RealTier.");
	}
}

void RealTier_formula (RealTier me, const wchar_t *expression, Interpreter interpreter, RealTier thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
		if (thee == NULL) thee = me;
		for (long icol = 1; icol <= my points -> size; icol ++) {
			struct Formula_Result result;
			Formula_run (0, icol, & result);
			if (result. result.numericResult == NUMundefined)
				Melder_throw ("Cannot put an undefined value into the tier.");
			((RealPoint) thy points -> item [icol]) -> value = result. result.numericResult;
		}
	} catch (MelderError) {
		Melder_throw (me, ": formula not completed.");
	}
}

void RealTier_removePointsBelow (RealTier me, double level) {
	for (long ipoint = my points -> size; ipoint > 0; ipoint --) {
		RealPoint point = (RealPoint) my points -> item [ipoint];
		if (point -> value < level) {
			AnyTier_removePoint (me, ipoint);
		}
	}
}

/* End of file RealTier.cpp */
