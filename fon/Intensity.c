/* Intensity.c
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
 * pb 2002/07/16 GPL
 * pb 2004/10/23 Intensity_getQuantile
 * pb 2004/10/24 Sampled statistics
 * pb 2004/11/21 corrected sones bug
 */

#include "Intensity.h"

static void info (I) {
	iam (Intensity);
	classData -> info (me);
	Melder_info (
		"Time domain:\n"
		"   Starting time: %s seconds\n"
		"   Finishing time: %s seconds\n"
		"   Total duration: %s seconds",
		Melder_double (my xmin), Melder_double (my xmax), Melder_double (my xmax - my xmin));
	Melder_info (
		"Time sampling:\n"
		"   Number of frames: %ld\n"
		"   Time step: %s seconds\n"
		"   First frame centred at: %s seconds",
		my nx, Melder_double (my dx), Melder_double (my x1));
}

static double getValueAtSample (I, long isamp, long which, int units) {
	iam (Intensity);
	double value = my z [1] [isamp];
	(void) which;
	if (! NUMdefined (value)) return NUMundefined;
	if (units == 1) {
		return pow (10.0, 0.1 * value);   /* energy */
	} else if (units == 2) {
		return pow (2.0, 0.1 * value);   /* sones */
	}
	return value;   /* default, especially if units=0 (as in Vector_getMean) or units=3 (averaging_DB) */
}

static double backToStandardUnits (I, double value, long which, int units) {
	iam (Intensity);
	(void) which;
	return
		value == NUMundefined ?
			NUMundefined :
		units == 1 ?
			10.0 * log10 (value) :   /* value = energy */
		units == 2 ?
			10.0 * NUMlog2 (value) :   /* value = sones */
		value;   /* value = dB */
}

class_methods (Intensity, Vector)
	class_method (info)
	class_method (getValueAtSample)
	class_method (backToStandardUnits)
class_methods_end

int Intensity_init (Intensity me, double tmin, double tmax, long nt, double dt, double t1) {
	if (! Matrix_init (me, tmin, tmax, nt, dt, t1, 1.0, 1.0, 1, 1.0, 1.0)) return 0;
	return 1;
}

Intensity Intensity_create (double tmin, double tmax, long nt, double dt, double t1) {
	Intensity me = new (Intensity);
	if (! me || ! Intensity_init (me, tmin, tmax, nt, dt, t1)) forget (me);
	return me;
}

Matrix Intensity_to_Matrix (Intensity me) {
	Matrix thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classMatrix);
	return thee;
}

Intensity Matrix_to_Intensity (Matrix me) {
	Intensity thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classIntensity);
	return thee;
}

void Intensity_drawInside (Intensity me, Graphics g, double tmin, double tmax, double minimum, double maximum) {
	long itmin, itmax;
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindow. */
	Matrix_getWindowSamplesX (me, tmin, tmax, & itmin, & itmax);
	if (maximum <= minimum)
		Matrix_getWindowExtrema (me, itmin, itmax, 1, 1, & minimum, & maximum);   /* Autoscale. */
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
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, "Intensity (dB)");
	}
}

double Intensity_getQuantile (Intensity me, double tmin, double tmax, double quantile) {
	return Sampled_getQuantile (me, tmin, tmax, quantile, 0, Intensity_units_DB);
}

double Intensity_getAverage (Intensity me, double tmin, double tmax, int averagingMethod) {
	return
		averagingMethod == Intensity_averaging_MEDIAN ?
			Intensity_getQuantile (me, tmin, tmax, 0.50) :
			Sampled_getMean_standardUnits (me, tmin, tmax, 0, averagingMethod, TRUE);
}

/* End of file Intensity.c */
