/* Spectrogram.cpp
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

#include "Spectrogram.h"

Thing_implement (Spectrogram, Matrix, 2);

void structSpectrogram :: v_info () {
	structData :: v_info ();
	MelderInfo_writeLine (L"Time domain:");
	MelderInfo_writeLine (L"   Start time: ", Melder_double (xmin), L" seconds");
	MelderInfo_writeLine (L"   End time: ", Melder_double (xmax), L" seconds");
	MelderInfo_writeLine (L"   Total duration: ", Melder_double (xmax - xmin), L" seconds");
	MelderInfo_writeLine (L"Time sampling:");
	MelderInfo_writeLine (L"   Number of time slices (frames): ", Melder_integer (nx));
	MelderInfo_writeLine (L"   Time step (frame distance): ", Melder_double (dx), L" seconds");
	MelderInfo_writeLine (L"   First time slice (frame centre) at: ", Melder_double (x1), L" seconds");
	MelderInfo_writeLine (L"Frequency domain:");
	MelderInfo_writeLine (L"   Lowest frequency: ", Melder_double (ymin), L" Hz");
	MelderInfo_writeLine (L"   Highest frequency: ", Melder_double (ymax), L" Hz");
	MelderInfo_writeLine (L"   Total bandwidth: ", Melder_double (ymax - ymin), L" Hz");
	MelderInfo_writeLine (L"Frequency sampling:");
	MelderInfo_writeLine (L"   Number of frequency bands (bins): ", Melder_integer (ny));
	MelderInfo_writeLine (L"   Frequency step (bin width): ", Melder_double (dy), L" Hz");
	MelderInfo_writeLine (L"   First frequency band around (bin centre at): ", Melder_double (y1), L" Hz");
}

Spectrogram Spectrogram_create (double tmin, double tmax, long nt, double dt, double t1,
	double fmin, double fmax, long nf, double df, double f1)
{
	try {
		autoSpectrogram me = Thing_new (Spectrogram);
		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, fmin, fmax, nf, df, f1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Spectrogram not created.");
	}
}

void Spectrogram_paintInside (Spectrogram me, Graphics g, double tmin, double tmax, double fmin, double fmax,
	double maximum, int autoscaling, double dynamic, double preemphasis, double dynamicCompression)
{
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (fmax <= fmin) { fmin = my ymin; fmax = my ymax; }
	long itmin, itmax, ifmin, ifmax;
	if (! Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) ||
		 ! Matrix_getWindowSamplesY (me, fmin - 0.49999 * my dy, fmax + 0.49999 * my dy, & ifmin, & ifmax))
		return;
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);
	autoNUMvector <double> preemphasisFactor (ifmin, ifmax);
	autoNUMvector <double> dynamicFactor (itmin, itmax);
	/* Pre-emphasis in place; also compute maximum after pre-emphasis. */
	for (long ifreq = ifmin; ifreq <= ifmax; ifreq ++) {
		preemphasisFactor [ifreq] = (preemphasis / NUMln2) * log (ifreq * my dy / 1000.0);
		for (long itime = itmin; itime <= itmax; itime ++) {
			double value = my z [ifreq] [itime];   /* Power. */
			value = (10.0/NUMln10) * log ((value + 1e-30) / 4.0e-10) + preemphasisFactor [ifreq];   /* dB */
			if (value > dynamicFactor [itime]) dynamicFactor [itime] = value;   /* Local maximum. */
			my z [ifreq] [itime] = value;
		}
	}
	/* Compute global maximum. */
	if (autoscaling) {
		maximum = 0.0;
		for (long itime = itmin; itime <= itmax; itime ++)
			if (dynamicFactor [itime] > maximum) maximum = dynamicFactor [itime];
	}
	/* Dynamic compression in place. */
	for (long itime = itmin; itime <= itmax; itime ++) {
		dynamicFactor [itime] = dynamicCompression * (maximum - dynamicFactor [itime]);
		for (long ifreq = ifmin; ifreq <= ifmax; ifreq ++)
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
	for (long ifreq = ifmin; ifreq <= ifmax; ifreq ++)
		for (long itime = itmin; itime <= itmax; itime ++) {
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
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Frequency (Hz)");
	}
}

Spectrogram Matrix_to_Spectrogram (Matrix me) {
	try {
		autoSpectrogram thee = Spectrogram_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Spectrogram.");
	}
}

Matrix Spectrogram_to_Matrix (Spectrogram me) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, my ymin, my ymax, my ny, my dy, my y1);
		NUMmatrix_copyElements (my z, thy z, 1, my ny, 1, my nx);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Matrix.");
	}
}

/* End of Spectrogram.cpp */ 
