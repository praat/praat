/* Intensity.cpp
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

/*
 * pb 2002/07/16 GPL
 * pb 2004/10/23 Intensity_getQuantile
 * pb 2004/10/24 Sampled statistics
 * pb 2004/11/21 corrected sones bug
 * pb 2006/12/08 MelderInfo
 * pb 2007/03/17 domain quantity
 * pb 2011/05/31 C++
 * pb 2011/07/14 C++
 */

#include "Intensity.h"

void structIntensity :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine1 (L"Time domain:");
	MelderInfo_writeLine3 (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine3 (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine3 (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine1 (L"Time sampling:");
	MelderInfo_writeLine2 (L"   Number of frames: ", Melder_integer (nx));
	MelderInfo_writeLine3 (L"   Time step: ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine3 (L"   First frame centred at: ", Melder_double (x1), L" seconds");
}

static double convertStandardToSpecialUnit (I, double value, long ilevel, int unit) {
	iam (Intensity);
	(void) me;
	(void) ilevel;
	if (unit == 1) {
		return pow (10.0, 0.1 * value);   /* energy */
	} else if (unit == 2) {
		return pow (2.0, 0.1 * value);   /* sones */
	}
	return value;   /* default, especially if units=0 (as in Vector_getMean) or units=3 (averaging_DB) */
}

static double convertSpecialToStandardUnit (I, double value, long ilevel, int unit) {
	iam (Intensity);
	(void) ilevel;
	(void) me;
	return
		unit == 1 ?
			10.0 * log10 (value) :   /* value = energy */
		unit == 2 ?
			10.0 * NUMlog2 (value) :   /* value = sones */
		value;   /* value = dB */
}

class_methods (Intensity, Vector) {
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
	class_method (convertStandardToSpecialUnit)
	class_method (convertSpecialToStandardUnit)
	class_methods_end
}

void Intensity_init (Intensity me, double tmin, double tmax, long nt, double dt, double t1) {
	Matrix_init (me, tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0);
}

Intensity Intensity_create (double tmin, double tmax, long nt, double dt, double t1) {
	try {
		autoIntensity me = Thing_new (Intensity);
		Intensity_init (me.peek(), tmin, tmax, nt, dt, t1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Intensity not created.");
	}
}

Matrix Intensity_to_Matrix (Intensity me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		((Data_Table) my methods) -> copy (me, thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Intensity.");
	}
}

Intensity Matrix_to_Intensity (Matrix me) {
	try {
		autoIntensity thee = Thing_new (Intensity);
		((Data_Table) my methods) -> copy (me, thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

void Intensity_drawInside (Intensity me, Graphics g, double tmin, double tmax, double minimum, double maximum) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // autowindow
	long itmin, itmax;
	Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
	if (maximum <= minimum)
		Matrix_getWindowExtrema (me, itmin, itmax, 1, 1, & minimum, & maximum);   // autoscale
	if (maximum <= minimum) { minimum -= 10; maximum += 10; }
	Graphics_setWindow (g, tmin, tmax, minimum, maximum);
	Graphics_function (g, my z [1], itmin, itmax, Matrix_columnToX (me, itmin), Matrix_columnToX (me, itmax));
}

void Intensity_draw (Intensity me, Graphics g, double tmin, double tmax,
	double minimum, double maximum, int garnish)
{
	Graphics_setInner (g);
	Intensity_drawInside (me, g, tmin, tmax, minimum, maximum);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Intensity (dB)");
	}
}

double Intensity_getQuantile (Intensity me, double tmin, double tmax, double quantile) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, 0, Intensity_units_DB);
}

double Intensity_getAverage (Intensity me, double tmin, double tmax, int averagingMethod) {
	return
		averagingMethod == Intensity_averaging_MEDIAN ?
			Intensity_getQuantile (me, tmin, tmax, 0.50) :
			Sampled_getMean_standardUnit (me, tmin, tmax, 0, averagingMethod, TRUE);
}

/* End of file Intensity.cpp */
