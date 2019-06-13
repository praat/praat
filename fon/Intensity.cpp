/* Intensity.cpp
 *
 * Copyright (C) 1992-2012,2015,2016 Paul Boersma
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

#include "Intensity.h"

Thing_implement (Intensity, Vector, 2);

void structIntensity :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of frames: ", nx);
	MelderInfo_writeLine (U"   Time step: ", dx, U" seconds");
	MelderInfo_writeLine (U"   First frame centred at: ", x1, U" seconds");
}

double structIntensity :: v_convertStandardToSpecialUnit (double value, integer /* level */, int unit) {
	if (unit == 1) {
		return pow (10.0, 0.1 * value);   // energy
	} else if (unit == 2) {
		return pow (2.0, 0.1 * value);   // sones
	}
	return value;   // default, especially if units == 0 (as in Vector_getMean) or units == 3 (averaging_DB)
}

double structIntensity :: v_convertSpecialToStandardUnit (double value, integer /* level */, int unit) {
	return
		unit == 1 ?
			10.0 * log10 (value) :   // value = energy
		unit == 2 ?
			10.0 * NUMlog2 (value) :   // value = sones
		value;   // value = dB
}

void Intensity_init (Intensity me, double tmin, double tmax, integer nt, double dt, double t1) {
	Matrix_init (me, tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0);
}

autoIntensity Intensity_create (double tmin, double tmax, integer nt, double dt, double t1) {
	try {
		autoIntensity me = Thing_new (Intensity);
		Intensity_init (me.get(), tmin, tmax, nt, dt, t1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Intensity not created.");
	}
}

autoMatrix Intensity_to_Matrix (Intensity me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoIntensity Matrix_to_Intensity (Matrix me) {
	try {
		autoIntensity thee = Thing_new (Intensity);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Intensity.");
	}
}

void Intensity_drawInside (Intensity me, Graphics g, double tmin, double tmax, double minimum, double maximum) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
	if (maximum <= minimum)
		Matrix_getWindowExtrema (me, itmin, itmax, 1, 1, & minimum, & maximum);   // autoscale
	if (maximum <= minimum) { minimum -= 10; maximum += 10; }
	Graphics_setWindow (g, tmin, tmax, minimum, maximum);
	Graphics_function (g, & my z [1] [0], itmin, itmax, Matrix_columnToX (me, itmin), Matrix_columnToX (me, itmax));
}

void Intensity_draw (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum, bool garnish)
{
	Graphics_setInner (g);
	Intensity_drawInside (me, g, tmin, tmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Intensity (dB)");
	}
}

double Intensity_getQuantile (Intensity me, double tmin, double tmax, double quantile) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, 0, Intensity_units_DB);
}

double Intensity_getAverage (Intensity me, double tmin, double tmax, int averagingMethod) {
	return
		averagingMethod == Intensity_averaging_MEDIAN ?
			Intensity_getQuantile (me, tmin, tmax, 0.50) :
			Sampled_getMean_standardUnit (me, tmin, tmax, 0, averagingMethod, true);
}

/* End of file Intensity.cpp */
