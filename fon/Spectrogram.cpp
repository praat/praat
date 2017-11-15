/* Spectrogram.cpp
 *
 * Copyright (C) 1992-2012,2015,2016,2017 Paul Boersma
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

#include "Spectrogram.h"

Thing_implement (Spectrogram, Matrix, 2);

void structSpectrogram :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of time slices (frames): ", nx);
	MelderInfo_writeLine (U"   Time step (frame distance): ", dx, U" seconds");
	MelderInfo_writeLine (U"   First time slice (frame centre) at: ", x1, U" seconds");
	MelderInfo_writeLine (U"Frequency domain:");
	MelderInfo_writeLine (U"   Lowest frequency: ", ymin, U" Hz");
	MelderInfo_writeLine (U"   Highest frequency: ", ymax, U" Hz");
	MelderInfo_writeLine (U"   Total bandwidth: ", ymax - ymin, U" Hz");
	MelderInfo_writeLine (U"Frequency sampling:");
	MelderInfo_writeLine (U"   Number of frequency bands (bins): ", ny);
	MelderInfo_writeLine (U"   Frequency step (bin width): ", dy, U" Hz");
	MelderInfo_writeLine (U"   First frequency band around (bin centre at): ", y1, U" Hz");
}

autoSpectrogram Spectrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double fmin, double fmax, integer nf, double df, double f1)
{
	try {
		autoSpectrogram me = Thing_new (Spectrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Spectrogram not created.");
	}
}

void Spectrogram_paintInside (Spectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double maximum, int autoscaling, double dynamic, double preemphasis, double dynamicCompression)
{
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (fmax <= fmin) { fmin = my ymin; fmax = my ymax; }
	integer itmin, itmax, ifmin, ifmax;
	if (! Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) ||
		 ! Matrix_getWindowSamplesY (me, fmin - 0.49999 * my dy, fmax + 0.49999 * my dy, & ifmin, & ifmax))
		return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	autoNUMvector <double> preemphasisFactor (ifmin, ifmax);
	autoNUMvector <double> dynamicFactor (itmin, itmax);
	/* Pre-emphasis in place; also compute maximum after pre-emphasis. */
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		preemphasisFactor [ifreq] = (preemphasis / NUMln2) * log (ifreq * my dy / 1000.0);
		for (integer itime = itmin; itime <= itmax; itime ++) {
			double value = my z [ifreq] [itime];   // power
			value = (10.0/NUMln10) * log ((value + 1e-30) / 4.0e-10) + preemphasisFactor [ifreq];   // dB
			if (value > dynamicFactor [itime]) dynamicFactor [itime] = value;   // local maximum
			my z [ifreq] [itime] = value;
		}
	}
	/* Compute global maximum. */
	if (autoscaling) {
		maximum = 0.0;
		for (integer itime = itmin; itime <= itmax; itime ++)
			if (dynamicFactor [itime] > maximum) maximum = dynamicFactor [itime];
	}
	/* Dynamic compression in place. */
	for (integer itime = itmin; itime <= itmax; itime ++) {
		dynamicFactor [itime] = dynamicCompression * (maximum - dynamicFactor [itime]);
		for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++)
			my z [ifreq] [itime] += dynamicFactor [itime];
	}
	Graphics_image (g, my z,
		itmin, itmax,
		Matrix_columnToX (me, itmin - 0.5),
		Matrix_columnToX (me, itmax + 0.5),
		ifmin, ifmax,
		Matrix_rowToY (me, ifmin - 0.5),
		Matrix_rowToY (me, ifmax + 0.5),
		maximum - dynamic, maximum);
	for (integer ifreq = ifmin; ifreq <= ifmax; ifreq ++)
		for (integer itime = itmin; itime <= itmax; itime ++) {
			double value = 4.0e-10 * exp ((my z [ifreq] [itime] - dynamicFactor [itime]
				- preemphasisFactor [ifreq]) * (NUMln10 / 10.0)) - 1e-30;
			my z [ifreq] [itime] = value > 0.0 ? value : 0.0;
		}
}

void Spectrogram_paint (Spectrogram me, Graphics g,
	double tmin, double tmax, double fmin, double fmax, double maximum, int autoscaling,
	double dynamic, double preemphasis, double dynamicCompression,
	int garnish)
{
	Graphics_setInner (g);
	Spectrogram_paintInside (me, g, tmin, tmax, fmin, fmax, maximum, autoscaling, dynamic, preemphasis, dynamicCompression);
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Frequency (Hz)");
	}
}

autoSpectrogram Matrix_to_Spectrogram (Matrix me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Spectrogram.");
	}
}

autoMatrix Spectrogram_to_Matrix (Spectrogram me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

/* End of Spectrogram.cpp */ 
