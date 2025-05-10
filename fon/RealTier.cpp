/* RealTier.cpp
 *
 * Copyright (C) 1992-2012,2014-2024 Paul Boersma
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

autoRealPoint RealPoint_create (double time, double value) {
	autoRealPoint me = Thing_new (RealPoint);
	my number = time;
	my value = value;
	return me;
}

/********** class RealTier **********/

void structRealTier :: v1_info () {
	structFunction :: v1_info ();
	MelderInfo_writeLine (U"Number of points: ", our points.size);
	MelderInfo_writeLine (U"Minimum value: ", RealTier_getMinimumValue (this));
	MelderInfo_writeLine (U"Maximum value: ", RealTier_getMaximumValue (this));
}

double structRealTier :: v_getVector (const integer /*irow*/, const integer icol) const {
	return RealTier_getValueAtIndex (this, icol);
}

double structRealTier :: v_getFunction1 (const integer /*irow*/, const double x) const {
	return RealTier_getValueAtTime (this, x);
}

Thing_implement (RealTier, AnyTier, 0);   // the semantic superclass (see RealTier_def.h for explanation)

void RealTier_init (RealTier me, double tmin, double tmax) {
	my xmin = tmin;
	my xmax = tmax;
}

autoRealTier RealTier_create (double tmin, double tmax) {
	try {
		autoRealTier me = Thing_new (RealTier);
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"RealTier not created.");
	}
}

autoRealTier RealTier_createWithClass (double tmin, double tmax, ClassInfo klas) {
	try {
		autoRealTier me = Thing_newFromClass (klas).static_cast_move <structRealTier>();
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (klas -> className, U" not created.");
	}
}

template <typename T> autoSomeThing <T> Thing_create () {
	return Thing_newFromClass (nullptr);
}

template <>
autoSomeThing <structRealTier> Thing_create <structRealTier> () {
	return Thing_newFromClass (classRealTier).static_cast_move <structRealTier>();
}

template <typename structSomeRealTier>
autoSomeThing <structSomeRealTier> SomeRealTier_create (const double tmin, const double tmax) {
	try {
		autoSomeThing <structSomeRealTier> me = Thing_create <structSomeRealTier> ();
		RealTier_init (me.get(), tmin, tmax);
		return me;
	} catch (MelderError) {
		Melder_throw (U"RealTier not created.");
	}
}

void RealTier_addPoint (const mutableRealTier me, const double t, const double value) {
	try {
		autoRealPoint point = RealPoint_create (t, value);
		my points. addItem_move (point.move());
	} catch (MelderError) {
		Melder_throw (me, U": point not added.");
	}
}

double RealTier_getValueAtIndex (const constRealTier me, const integer i) {
	if (i < 1 || i > my points.size)
		return undefined;
	return my points.at [i] -> value;
}

double RealTier_getValueAtTime (const constRealTier me, const double t) {
	const integer n = my points.size;
	if (n == 0)
		return undefined;
	const constRealPoint firstPoint = my points.at [1];
	if (t <= firstPoint -> number)
		return firstPoint -> value;   // constant extrapolation
	const constRealPoint lastPoint = my points.at [n];
	if (t >= lastPoint -> number)
		return lastPoint -> value;   // constant extrapolation
	Melder_assert (n >= 2);
	const integer ileft = AnyTier_timeToLowIndex (me->asConstAnyTier(), t), iright = ileft + 1;
	Melder_assert (ileft >= 1 && iright <= n);
	const constRealPoint pointLeft = my points.at [ileft];
	const constRealPoint pointRight = my points.at [iright];
	const double tleft = pointLeft -> number, fleft = pointLeft -> value;
	const double tright = pointRight -> number, fright = pointRight -> value;
	return t == tright ? fright   // be very accurate
		: tleft == tright ? 0.5 * (fleft + fright)   // unusual, but possible; no preference
		: fleft + (t - tleft) * (fright - fleft) / (tright - tleft);   // linear interpolation
}

double RealTier_getMaximumValue (const constRealTier me) {
	/* mutable */ double result = undefined;
	const integer n = my points.size;
	for (integer i = 1; i <= n; i ++) {
		const constRealPoint point = my points.at [i];
		if (isundef (result) || point -> value > result)
			result = point -> value;
	}
	return result;
}

double RealTier_getMinimumValue (const constRealTier me) {
	Melder_assert (me);
	/* mutable */ double result = undefined;
	const integer n = my points.size;
	for (integer i = 1; i <= n; i ++) {
		Melder_assert (my points.at._elements);
		RealPoint point = my points.at [i];
		Melder_assert (point);
		if (isundef (result) || point -> value < result)
			result = point -> value;
	}
	return result;
}

double RealTier_getArea (const constRealTier me, const double tmin, const double tmax) {
	const integer n = my points.size;
	if (n == 0)
		return undefined;
	if (n == 1)
		return (tmax - tmin) * my points.at [1] -> value;
	const integer imin = AnyTier_timeToLowIndex (me->asConstAnyTier(), tmin);
	if (imin == n)
		return (tmax - tmin) * my points.at [n] -> value;
	const integer imax = AnyTier_timeToHighIndex (me->asConstAnyTier(), tmax);
	if (imax == 1)
		return (tmax - tmin) * my points.at [1] -> value;
	Melder_assert (imin < n);
	Melder_assert (imax > 1);
	/*
		Sum the areas between the points.
		This works even if imin is 0 (offleft) and/or imax is n + 1 (offright).
	*/
	/* mutable loop */ longdouble area = 0.0;
	for (integer i = imin; i < imax; i ++) {
		/* mutable conditional init */ double tleft, fleft, tright, fright;
		if (i == imin) {
			tleft = tmin;
			fleft = RealTier_getValueAtTime (me, tmin);
		} else {
			tleft = my points.at [i] -> number;
			fleft = my points.at [i] -> value;
		}
		if (i + 1 == imax) {
			tright = tmax;
			fright = RealTier_getValueAtTime (me, tmax);
		} else {
			tright = my points.at [i + 1] -> number;
			fright = my points.at [i + 1] -> value;
		}
		area += 0.5 * (fleft + fright) * (tright - tleft);
	}
	return (double) area;
}

double RealTier_getMean_curve (const constRealTier me, /* mutable */ double tmin, /* mutable */ double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const double area = RealTier_getArea (me, tmin, tmax);
	if (isundef (area))
		return undefined;
	return area / (tmax - tmin);
}

double RealTier_getStandardDeviation_curve (const constRealTier me, /* mutable */ double tmin, /* mutable */ double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const integer n = my points.size;
	if (n == 0)
		return undefined;
	if (n == 1)
		return 0.0;
	const integer imin = AnyTier_timeToLowIndex (me->asConstAnyTier(), tmin);
	if (imin == n)
		return 0.0;
	const integer imax = AnyTier_timeToHighIndex (me->asConstAnyTier(), tmax);
	if (imax == 1)
		return 0.0;
	Melder_assert (imin < n);
	Melder_assert (imax > 1);
	/*
		Add the areas between the points.
		This works even if imin is 0 (offleft) and/or imax is n + 1 (offright).
	*/
	const double mean = RealTier_getMean_curve (me, tmin, tmax);
	/* mutable loop */ longdouble integral = 0.0;
	for (integer i = imin; i < imax; i ++) {
		double tleft, fleft, tright, fright;
		if (i == imin) {
			tleft = tmin;
			fleft = RealTier_getValueAtTime (me, tmin);
		} else {
			tleft = my points.at [i] -> number;
			fleft = my points.at [i] -> value - mean;
		}
		if (i + 1 == imax) {
			tright = tmax;
			fright = RealTier_getValueAtTime (me, tmax);
		} else {
			tright = my points.at [i + 1] -> number;
			fright = my points.at [i + 1] -> value - mean;
		}
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
		const double sum = fleft + fright;
		const double diff = fleft - fright;
		integral += (sum * sum + (1.0/3.0) * diff * diff) * (tright - tleft);
	}
	return sqrt (0.25 * (double) integral / (tmax - tmin));
}

double RealTier_getMean_points (const constRealTier me, /* mutable */ double tmin, /* mutable */ double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	/* mutable init */ integer imin, imax;
	const integer n = AnyTier_getWindowPoints (me->asConstAnyTier(), tmin, tmax, & imin, & imax);
	if (n == 0)
		return undefined;
	/* mutable loop */ longdouble sum = 0.0;
	for (integer i = imin; i <= imax; i ++)
		sum += my points.at [i] -> value;
	return (double) sum / n;
}

double RealTier_getStandardDeviation_points (const constRealTier me, /* mutable */ double tmin, /* mutable */ double tmax) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	/* mutable init */ integer imin, imax;
	const integer n = AnyTier_getWindowPoints (me->asConstAnyTier(), tmin, tmax, & imin, & imax);
	if (n < 2)
		return undefined;
	const double mean = RealTier_getMean_points (me, tmin, tmax);
	/* mutable loop */ longdouble sum = 0.0;
	for (integer i = imin; i <= imax; i ++) {
		const double diff = my points.at [i] -> value - mean;
		sum += diff * diff;
	}
	return sqrt ((double) sum / (n - 1));
}

void RealTier_multiplyPart (RealTier me, double tmin, double tmax, double factor) {
	for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
		const RealPoint point = my points.at [ipoint];
		const double t = point -> number;
		if (t >= tmin && t <= tmax)
			point -> value *= factor;
	}
}

void RealTier_draw (
	const constRealTier me,
	const Graphics g,
	/* mutable */ double tmin, /* mutable */ double tmax,
	const double fmin, const double fmax,
	const bool garnish,
	const conststring32 method,
	const conststring32 quantity
) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	const bool drawLines = str32str (method, U"lines") || str32str (method, U"Lines");
	const bool drawSpeckles = str32str (method, U"speckles") || str32str (method, U"Speckles");
	const integer n = my points.size;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	Graphics_setInner (g);
	const integer imin = AnyTier_timeToHighIndex (me->asConstAnyTier(), tmin);
	const integer imax = AnyTier_timeToLowIndex (me->asConstAnyTier(), tmax);
	if (n == 0) {
	} else if (imax < imin) {
		const double fleft = RealTier_getValueAtTime (me, tmin);
		const double fright = RealTier_getValueAtTime (me, tmax);
		if (drawLines)
			Graphics_line (g, tmin, fleft, tmax, fright);
	} else for (integer i = imin; i <= imax; i ++) {
		const constRealPoint point = my points.at [i];
		const double t = point -> number, f = point -> value;
		if (drawSpeckles)
			Graphics_speckle (g, t, f);
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
				const constRealPoint pointRight = my points.at [i + 1];
				Graphics_line (g, t, f, pointRight -> number, pointRight -> value);
			}
		}
	}
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, my v_getUnitText (0, 0, 0));
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		if (quantity)
			Graphics_textLeft (g, true, quantity);
	}
}

autoTableOfReal RealTier_downto_TableOfReal (const constRealTier me, const conststring32 timeLabel, const conststring32 valueLabel) {
	try {
		autoTableOfReal thee = TableOfReal_create (my points.size, 2);
		TableOfReal_setColumnLabel (thee.get(), 1, timeLabel);
		TableOfReal_setColumnLabel (thee.get(), 2, valueLabel);
		for (integer i = 1; i <= my points.size; i ++) {
			RealPoint point = my points.at [i];
			thy data [i] [1] = point -> number;
			thy data [i] [2] = point -> value;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to TableOfReal.");
	}
}

void RealTier_interpolateQuadratically (const RealTier me, const integer numberOfPointsPerParabola, const bool logarithmically) {
	try {
		autoRealTier thee = Data_copy (me);
		for (integer ipoint = 1; ipoint < my points.size; ipoint ++) {
			const constRealPoint point1 = my points.at [ipoint], point2 = my points.at [ipoint + 1];
			const double time1 = point1 -> number, time2 = point2 -> number, tmid = 0.5 * (time1 + time2);
			double value1 = point1 -> value, value2 = point2 -> value, valuemid;
			const double timeStep = (tmid - time1) / (numberOfPointsPerParabola + 1);
			if (logarithmically) {
				value1 = log (value1);
				value2 = log (value2);
			}
			valuemid = 0.5 * (value1 + value2);
			/*
				Left from the midpoint.
			*/
			for (integer inewpoint = 1; inewpoint <= numberOfPointsPerParabola; inewpoint ++) {
				const double newTime = time1 + inewpoint * timeStep;
				const double phase = (newTime - time1) / (tmid - time1);
				double newValue = value1 + (valuemid - value1) * phase * phase;
				if (logarithmically)
					newValue = exp (newValue);
				RealTier_addPoint (thee.get(), newTime, newValue);
			}
			/*
				The midpoint.
			*/
			RealTier_addPoint (thee.get(), tmid, logarithmically ? exp (valuemid) : valuemid);
			/*
				Right from the midpoint.
			*/
			for (integer inewpoint = 1; inewpoint <= numberOfPointsPerParabola; inewpoint ++) {
				const double newTime = tmid + inewpoint * timeStep;
				const double phase = (time2 - newTime) / (time2 - tmid);
				double newValue = value2 + (valuemid - value2) * phase * phase;
				if (logarithmically)
					newValue = exp (newValue);
				RealTier_addPoint (thee.get(), newTime, newValue);
			}
		}
		Thing_swap (me, thee.get());
	} catch (MelderError) {
		Melder_throw (me, U": not interpolated quadratically.");
	}
}

autoTable RealTier_downto_Table (RealTier me, conststring32 indexText, conststring32 timeText, conststring32 valueText) {
	try {
		autoTable thee = Table_createWithoutColumnNames (my points.size,
			(!! indexText) + (!! timeText) + (!! valueText));
		integer icol = 0;
		if (indexText) Table_renameColumn_e (thee.get(), ++ icol, indexText);
		if (timeText ) Table_renameColumn_e (thee.get(), ++ icol, timeText);
		if (valueText) Table_renameColumn_e (thee.get(), ++ icol, valueText);
		for (integer ipoint = 1; ipoint <= my points.size; ipoint ++) {
			RealPoint point = my points.at [ipoint];
			icol = 0;
			if (indexText) Table_setNumericValue (thee.get(), ipoint, ++ icol, ipoint);
			if (timeText)  Table_setNumericValue (thee.get(), ipoint, ++ icol, point -> number);
			if (valueText) Table_setNumericValue (thee.get(), ipoint, ++ icol, point -> value);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Table.");
	}
}

autoRealTier Vector_to_RealTier (Vector me, integer channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (integer i = 1; i <= my nx; i ++)
			RealTier_addPoint (thee.get(), Sampled_indexToX (me, i), my z [channel] [i]);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ", klas -> className, U".");
	}
}

autoRealTier Vector_to_RealTier_peaks (Vector me, integer channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (integer i = 2; i < my nx; i ++) {
			double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
			if (left <= centre && right < centre) {
				double x, maximum;
				Vector_getMaximumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
						channel, kVector_peakInterpolation :: PARABOLIC, & maximum, & x);
				RealTier_addPoint (thee.get(), x, maximum);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ", klas -> className, U" (peaks).");
	}
}

autoRealTier Vector_to_RealTier_valleys (Vector me, integer channel, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (integer i = 2; i < my nx; i ++) {
			double left = my z [channel] [i - 1], centre = my z [channel] [i], right = my z [channel] [i + 1];
			if (left >= centre && right > centre) {
				double x, minimum;
				Vector_getMinimumAndX (me, my x1 + (i - 2.5) * my dx, my x1 + (i + 0.5) * my dx,
						channel, kVector_peakInterpolation :: PARABOLIC, & minimum, & x);
				RealTier_addPoint (thee.get(), x, minimum);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to ", klas -> className, U" (valleys).");
	}
}

autoRealTier PointProcess_upto_RealTier (PointProcess me, double value, ClassInfo klas) {
	try {
		autoRealTier thee = RealTier_createWithClass (my xmin, my xmax, klas);
		for (integer i = 1; i <= my nt; i ++)
			RealTier_addPoint (thee.get(), my t [i], value);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to RealTier.");
	}
}

void RealTier_formula (RealTier me, conststring32 expression, Interpreter interpreter, RealTier thee) {
	try {
		Formula_compile (interpreter, me, expression, kFormula_EXPRESSION_TYPE_NUMERIC, true);
		Formula_Result result;
		if (! thee)
			thee = me;
		for (integer icol = 1; icol <= my points.size; icol ++) {
			Formula_run (0, icol, & result);
			if (isundef (result. numericResult))
				Melder_throw (U"Cannot put an undefined value into the tier.");
			thy points.at [icol] -> value = result. numericResult;
		}
	} catch (MelderError) {
		Melder_throw (me, U": formula not completed.");
	}
}

void RealTier_removePointsBelow (RealTier me, double level) {
	for (integer ipoint = my points.size; ipoint > 0; ipoint --) {
		RealPoint point = my points.at [ipoint];
		if (point -> value < level)
			AnyTier_removePoint (me->asAnyTier(), ipoint);
	}
}

void RealTier_PointProcess_into_RealTier (RealTier me, PointProcess pp, RealTier thee) {
	for (integer i = 1; i <= pp -> nt; i ++) {
		const double time = pp -> t [i];
		const double value = RealTier_getValueAtTime (me, time);
		RealTier_addPoint (thee, time, value);
	}
}

autoRealTier RealTier_PointProcess_to_RealTier (RealTier me, PointProcess pp) {
	try {
		if (my points.size == 0)
			Melder_throw (U"No points.");
		autoRealTier thee = RealTier_create (pp -> xmin, pp -> xmax);
		RealTier_PointProcess_into_RealTier (me, pp, thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U" & ", pp, U": not converted to RealTier.");
	}
}

autoRealTier AnyRealTier_downto_RealTier (RealTier me) {
	try {
		autoRealTier thee = Thing_new (RealTier);
		my structRealTier :: v1_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to RealTier.");
	}
}

static void RealTier_checkThatNoPointFallsOutsideDefinedTimeDomain (RealTier me) {
	if (my points.size == 0) {
		// nothing to check
	} else if (my points.size == 1) {
		const double onlyTime = my points.at [1] -> number;
		if (isdefined (my xmin))
			Melder_require (my xmin <= onlyTime,
				U"The only point (at time ", onlyTime, U" seconds) falls outside the time domain, i.e. before ", my xmin, U" seconds.");
		if (isdefined (my xmax))
			Melder_require (my xmax >= onlyTime,
				U"The only point (at time ", onlyTime, U" seconds) falls outside the time domain, i.e. after ", my xmax, U" seconds.");
	} else {
		const double firstTime = my points.at [1] -> number;
		const double lastTime = my points.at [my points.size] -> number;
		if (isdefined (my xmin))
			Melder_require (my xmin <= firstTime,
				U"The first point (at time ", firstTime, U" seconds) falls outside the time domain, i.e. before ", my xmin, U" seconds.");
		if (isdefined (my xmax))
			Melder_require (my xmax >= lastTime,
				U"The last point (at time ", lastTime, U" seconds) falls outside the time domain, i.e. after ", my xmax, U" seconds.");
	}
}

static void RealTier_fitUndefinedTimeDomainToData (RealTier me) {
	if (my points.size == 0) {
		if (isundef (my xmin) && isundef (my xmax)) {
			my xmin = 0.0;
			my xmax = 1.0;
		} else if (isundef (my xmin)) {
			my xmin = my xmax - 1.0;
		} else if (isundef (my xmax)) {
			my xmax = my xmin + 1.0;
		}
	} else if (my points.size == 1) {
		const double onlyTime = my points.at [1] -> number;
		if (isundef (my xmin) && isundef (my xmax)) {
			my xmin = onlyTime - 1.0;
			my xmax = onlyTime + 1.0;
		} else if (isundef (my xmin)) {
			my xmin = onlyTime - 1.0 * ( my xmax == onlyTime );
		} else if (isundef (my xmax)) {
			my xmax = onlyTime + 1.0 * ( my xmin == onlyTime );
		}
	} else {
		const double firstTime = my points.at [1] -> number;
		const double lastTime = my points.at [my points.size] -> number;
		if (isundef (my xmin))
			my xmin = firstTime;
		if (isundef (my xmax))
			my xmax = lastTime;
	}
}

autoRealTier Table_to_RealTier (Table me, integer timeColumn, integer valueColumn, double tmin, double tmax) {
	try {
		Melder_require (timeColumn >= 1 && timeColumn <= my numberOfColumns,
			U"The column number (for the times) should be between 1 and ", my numberOfColumns);
		Melder_require (valueColumn >= 1 && valueColumn <= my numberOfColumns,
			U"The column number (for the values) should be between 1 and ", my numberOfColumns);
		Melder_require (! (tmax <= tmin),   // NaN-safe
			U"The end of the time domain (", tmax, U") should be greater than the start of the time domain (", tmin, U").");
		autoRealTier thee = RealTier_create (tmin, tmax);
		Table_numericize_a (me, timeColumn);
		Table_numericize_a (me, valueColumn);
		for (integer irow = 1; irow <= my rows.size; irow ++) {
			TableRow row = my rows.at [irow];
			RealTier_addPoint (thee.get(), row -> cells [timeColumn]. number, row -> cells [valueColumn]. number);
		}
		/*
			At this point, all times are in sorted order and unique,
			because RealTier_addPoint inserts its time in order and complains if the time already exists.
			The data-dependent tests therefore need to be only about the time domain.
		*/
		RealTier_checkThatNoPointFallsOutsideDefinedTimeDomain (thee.get());
		RealTier_fitUndefinedTimeDomainToData (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to RealTier.");
	}
}

autoRealTier Matrix_to_RealTier (Matrix me, integer timeColumn, integer valueColumn, double tmin, double tmax) {
	try {
		Melder_require (timeColumn >= 1 && timeColumn <= my nx,
			U"The column number (for the times) should be between 1 and ", my nx);
		Melder_require (valueColumn >= 1 && valueColumn <= my nx,
			U"The column number (for the values) should be between 1 and ", my nx);
		Melder_require (! (tmax <= tmin),   // NaN-safe
			U"The end of the time domain (", tmax, U") should be greater than the start of the time domain (", tmin, U").");
		autoRealTier thee = RealTier_create (tmin, tmax);
		for (integer irow = 1; irow <= my ny; irow ++)
			RealTier_addPoint (thee.get(), my z [irow] [timeColumn], my z [irow] [valueColumn]);
		/*
			At this point, all times are in sorted order and unique,
			because RealTier_addPoint inserts its time in order and complains if the time already exists.
			The data-dependent tests therefore need to be only about the time domain.
		*/
		RealTier_checkThatNoPointFallsOutsideDefinedTimeDomain (thee.get());
		RealTier_fitUndefinedTimeDomainToData (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to RealTier.");
	}
}

/* End of file RealTier.cpp */
