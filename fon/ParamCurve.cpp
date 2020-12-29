/* ParamCurve.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016,2017 Paul Boersma
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

#include "ParamCurve.h"

#include "oo_DESTROY.h"
#include "ParamCurve_def.h"
#include "oo_COPY.h"
#include "ParamCurve_def.h"
#include "oo_EQUAL.h"
#include "ParamCurve_def.h"
#include "oo_DESCRIPTION.h"
#include "ParamCurve_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "ParamCurve_def.h"

Thing_implement (ParamCurve, Function, 2);

void structParamCurve :: v_info () {
	const MelderRealRange xextrema = NUMextrema (our x -> z.row (1));
	const MelderRealRange yextrema = NUMextrema (our y -> z.row (1));
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Domain:");
	MelderInfo_writeLine (U"   tmin: ", our xmin);
	MelderInfo_writeLine (U"   tmax: ", our xmax);
	MelderInfo_writeLine (U"x sampling:");
	MelderInfo_writeLine (U"   Number of values of t in x: ", our x -> nx);
	MelderInfo_writeLine (U"   t step in x: ", our x -> dx, U" (sampling rate ", 1.0 / our x -> dx, U")");
	MelderInfo_writeLine (U"   First t in x: ", our x -> x1);
	MelderInfo_writeLine (U"x values:");
	MelderInfo_writeLine (U"   Minimum x: ", xextrema.min);
	MelderInfo_writeLine (U"   Maximum x: ", xextrema.max);
	MelderInfo_writeLine (U"y sampling:");
	MelderInfo_writeLine (U"   Number of values of t in y: ", our y -> nx);
	MelderInfo_writeLine (U"   t step in y: ", our y -> dx, U" (sampling rate ", 1.0 / our y -> dx, U")");
	MelderInfo_writeLine (U"   First t in y: ", our y -> x1);
	MelderInfo_writeLine (U"y values:");
	MelderInfo_writeLine (U"   Minimum y: ", yextrema.min);
	MelderInfo_writeLine (U"   Maximum y: ", yextrema.max);
}

void structParamCurve :: v_writeText (MelderFile file) {
	Data_writeText (our x.get(), file);
	Data_writeText (our y.get(), file);
}

void structParamCurve :: v_readText (MelderReadText text, int formatVersion) {
	our x = Thing_new (Sound);
	our y = Thing_new (Sound);
	Data_readText (our x.get(), text, formatVersion);
	Data_readText (our y.get(), text, formatVersion);
	our xmin = our x -> xmin > our y -> xmin ? our x -> xmin : our y -> xmin;
	our xmax = our x -> xmax < our y -> xmax ? our x -> xmax : our y -> xmax;
}

void structParamCurve :: v_writeBinary (FILE *f) {
	Data_writeBinary (x.get(), f);
	Data_writeBinary (y.get(), f);
}

void structParamCurve :: v_readBinary (FILE *f, int /*formatVersion*/) {
	our x = Thing_new (Sound);
	our y = Thing_new (Sound);
	Data_readBinary (our x.get(), f, 2);
	Data_readBinary (our y.get(), f, 2);
	our xmin = our x -> xmin > our y -> xmin ? our x -> xmin : our y -> xmin;
	our xmax = our x -> xmax < our y -> xmax ? our x -> xmax : our y -> xmax;
}

void ParamCurve_init (ParamCurve me, Sound x, Sound y) {
	if (x -> xmax <= y -> xmin || x -> xmin >= y -> xmax)
		Melder_throw (U"Domains do not overlap.");
	my x = Data_copy (x);
	my y = Data_copy (y);
	my xmin = x -> xmin > y -> xmin ? x -> xmin : y -> xmin;
	my xmax = x -> xmax < y -> xmax ? x -> xmax : y -> xmax;
}

autoParamCurve ParamCurve_create (Sound x, Sound y) {
	try {
		autoParamCurve me = Thing_new (ParamCurve);
		ParamCurve_init (me.get(), x, y);
		return me;
	} catch (MelderError) {
		Melder_throw (U"ParamCurve not created.");
	}
}

void ParamCurve_draw (ParamCurve me, Graphics g, double t1, double t2, double dt,
	double x1, double x2, double y1, double y2, bool garnish)
{
	if (t2 <= t1) {
		double tx1 = my x -> x1;
		double ty1 = my y -> x1;
		double tx2 = my x -> x1 + (my x -> nx - 1) * my x -> dx;
		double ty2 = my y -> x1 + (my y -> nx - 1) * my y -> dx;
		t1 = tx1 > ty1 ? tx1 : ty1;
		t2 = tx2 < ty2 ? tx2 : ty2;
	}
	if (x2 <= x1) Matrix_getWindowExtrema (my x.get(), 0, 0, 1, 1, & x1, & x2);
	if (x1 == x2) { x1 -= 1.0; x2 += 1.0; }
	if (y2 <= y1) Matrix_getWindowExtrema (my y.get(), 0, 0, 1, 1, & y1, & y2);
	if (y1 == y2) { y1 -= 1.0; y2 += 1.0; }
	if (dt <= 0.0)
		dt = my x -> dx < my y -> dx ? my x -> dx : my y -> dx;
	integer numberOfPoints = Melder_iceiling ((t2 - t1) / dt) + 1;
	if (numberOfPoints > 0) {
		autoVEC x = raw_VEC (numberOfPoints);
		autoVEC y = raw_VEC (numberOfPoints);
		for (integer i = 1; i <= numberOfPoints; i ++) {
			double t = i == numberOfPoints ? t2 : t1 + (i - 1) * dt;
			double index = Sampled_xToIndex (my x.get(), t);
			x [i] = NUM_interpolate_sinc (my x -> z.row (1), index, 50);
			index = Sampled_xToIndex (my y.get(), t);
			y [i] = NUM_interpolate_sinc (my y -> z.row (1), index, 50);
		}
		Graphics_setWindow (g, x1, x2, y1, y2);
		Graphics_setInner (g);
		Graphics_polyline (g, numberOfPoints, & x [1], & y [1]);
		Graphics_unsetInner (g);
	}
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
	}
}

void ParamCurve_swapXY (ParamCurve me) {
	autoSound help = my x.move();
	my x = my y.move();
	my y = help.move();
}

/* End of file ParamCurve.cpp */
