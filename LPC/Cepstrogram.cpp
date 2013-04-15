/* Cepstrogram.cpp
 *
 * Copyright (C) 2013 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 20010514
 djmw 20020812 GPL header
 djmw 20080122 Version 1: float -> double
 djmw 20110304 Thing_new
*/

#include "Cepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "NUM2.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"

Thing_implement (Cepstrogram, Matrix, 2);

Cepstrogram Cepstrogram_create (double tmin, double tmax, long nt, double dt, double t1,
	double qmin, double qmax, long nq, double dq, double q1) {
	try {
		autoCepstrogram me = Thing_new (Cepstrogram);

		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, qmin, qmax, nq, dq, q1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Cepstrogram not created.");
	}
}

void Cepstrogram_paint (Cepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }
	if (qmax <= qmin) { qmin = my ymin; qmax = my ymax; }
	long itmin, itmax, ifmin, ifmax;
	if (! Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) ||
		 ! Matrix_getWindowSamplesY (me, qmin - 0.49999 * my dy, qmax + 0.49999 * my dy, & ifmin, & ifmax)) {
		return;
	}
	autoMatrix thee = (Matrix) Data_copy (me);
	double min = 1e38, max = -min;
	for (long i = 1; i <= my ny; i++) {
		for (long j = 1; j <= my nx; j++) {
			double val = thy z[i][j] = 20 * log10 (fabs (my z[i][j]));
			min = val < min ? val : min;
			max = val > max ? val : max;
		}
	}
	if (dBmaximum <= dBminimum) {
		dBminimum = min; dBmaximum = max;
	}
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, qmin, qmax);
	Graphics_image (g, thy z,
		itmin, itmax,
		Matrix_columnToX (thee.peek(), itmin - 0.5),
		Matrix_columnToX (thee.peek(), itmax + 0.5),
		ifmin, ifmax,
		Matrix_rowToY (thee.peek(), ifmin - 0.5),
		Matrix_rowToY (thee.peek(), ifmax + 0.5),
		dBminimum, dBmaximum);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		Graphics_textLeft (g, 1, L"Quefrency (s)");
	}
}

Table Cepstrogram_to_Table_cpp (Cepstrogram me, double lowestQuefrency, double highestQuefrency, int interpolation, double qstartFit, double qendFit, int method) {
	try {
		autoTable thee = Table_createWithColumnNames (my nx, L"time quefrency cpp f0");
		autoCepstrum him = Cepstrum_create (my ymin, my ymax, my ny);
		for (long i = 1; i <= my nx; i++) {
			for (long j = 1; j <= my ny; j++) {
				his z[1][j] = my z[j][i];
			}
			double qpeak, cpp = Cepstrum_getPeakProminence (him.peek(), lowestQuefrency, highestQuefrency, interpolation,
				qstartFit, qendFit, method, &qpeak);
			double time = Sampled_indexToX (me, i);
			Table_setNumericValue (thee.peek(), i, 1, time);
			Table_setNumericValue (thee.peek(), i, 2, qpeak);
			Table_setNumericValue (thee.peek(), i, 3, cpp);
			Table_setNumericValue (thee.peek(), i, 4, 1.0 / qpeak);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Table with cepstral peak prominence values created.");
	}
}

static void NUMvector_smoothByMovingAverage (double *xin, long n, long nwindow, double *xout) {
// simple averaging, out of bound values are zero
	for (long i = 1; i <= n; i++) {
		long jfrom = i - nwindow / 2, jto = i + nwindow / 2;
		jfrom = jfrom < 1 ? 1 : jfrom;
		jto = jto > n ? n : jto;
		xout[i] = 0;
		for (long j = jfrom; j <= jto; j++) {
			xout[i] += xin[j];
		}
		xout[i] /= jto - jfrom + 1;
	}
}

Cepstrogram Cepstrogram_smooth (Cepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoCepstrogram thee = (Cepstrogram) Data_copy (me);
		long numberOfFrames = timeAveragingWindow / my dx;
		if (numberOfFrames > 1) {
			// 1. average across time
			if (numberOfFrames % 2 == 0) { // make symmetric
				numberOfFrames++;
			}
			autoNUMvector<double> qin (1, my nx);
			autoNUMvector<double> qout (1, my nx);
			for (long iq = 1; iq <= my ny; iq++) {
				for (long iframe = 1; iframe <= my nx; iframe++) {
					qin[iframe] = my z[iq][iframe] * my z[iq][iframe];
				}
				NUMvector_smoothByMovingAverage (qin.peek(), my nx, numberOfFrames, qout.peek());
				for (long iframe = 1; iframe <= my nx; iframe++) {
					thy z[iq][iframe] = sqrt (qout[iframe]);
				}
			}
		}
		// 2. average across quefrencies
		long numberOfQuefrencyBins = quefrencyAveragingWindow / my dy;
		if (numberOfQuefrencyBins > 0) {
			if (numberOfQuefrencyBins % 2 == 0) {
				numberOfQuefrencyBins++;
			}
			autoNUMvector<double> qin (1, thy ny);
			autoNUMvector<double> qout (1, thy ny);
			for (long iframe = 1; iframe <= my nx; iframe++) {
				for (long iq = 1; iq <= thy ny; iq++) {
					qin[iq] = thy z[iq][iframe] * thy z[iq][iframe]; //fabs (thy z[iq][iframe]);
				}
				NUMvector_smoothByMovingAverage (qin.peek(), thy ny, numberOfQuefrencyBins, qout.peek());
				for (long iq = 1; iq <= thy ny; iq++) {
					thy z[iq][iframe] = sqrt (qout[iq]);
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

Matrix Cepstrogram_to_Matrix (Cepstrogram me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix created.");
	}
}

Cepstrum Cepstrogram_to_Cepstrum_slice (Cepstrogram me, double time) {
	try {
		long iframe = Sampled_xToNearestIndex (me, time);
		iframe = iframe < 1 ? 1 : iframe > my nx ? my nx : iframe;
		autoCepstrum thee = Cepstrum_create (my ymin, my ymax, my ny);
		for (long i = 1; i <= my ny; i++) {
			thy z[1][i] = my z[i][iframe];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Cepstrum not extracted.");
	}
}

Cepstrogram Matrix_to_Cepstrogram (Matrix me) {
	try {
		autoCepstrogram thee = Thing_new (Cepstrogram);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrogram created.");
	}
}

Cepstrogram Sound_to_Cepstrogram (Sound me, double analysisWidth, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		double windowDuration = 2 * analysisWidth; /* gaussian window */
		long nFrames;

		// Convenience: analyse the whole sound into one Cepstrogram_frame
		if (windowDuration > my dx * my nx) {
			windowDuration = my dx * my nx;
		}
		double t1, samplingFrequency = 2 * maximumFrequency;
		autoSound sound = Sound_resample (me, samplingFrequency, 50);
		Sound_preEmphasis (sound.peek(), preEmphasisFrequency);
		Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		double qmin, qmax, dq, q1;
		long nq;
		{ // laziness: find out the proper dimensions
			autoSpectrum spec = Sound_to_Spectrum (sframe.peek(), 1);
			autoCepstrum cepstrum = Spectrum_to_Cepstrum (spec.peek());
			qmin = cepstrum -> xmin; qmax = cepstrum -> xmax; dq = cepstrum -> dx;
			q1 = cepstrum -> x1; nq = cepstrum -> nx;
		}
		autoCepstrogram thee = Cepstrogram_create (my xmin, my xmax, nFrames, dt, t1, qmin, qmax, nq, dq, q1);

		autoMelderProgress progress (L"Cepstrogram analysis");

		for (long iframe = 1; iframe <= nFrames; iframe++) {
			double t = Sampled_indexToX (thee.peek(), iframe);
			Sound_into_Sound (sound.peek(), sframe.peek(), t - windowDuration / 2);
			Vector_subtractMean (sframe.peek());
			Sounds_multiply (sframe.peek(), window.peek());
			autoSpectrum spec = Sound_to_Spectrum (sframe.peek(), 1);
			autoCepstrum cepstrum = Spectrum_to_Cepstrum (spec.peek());   
			for (long i = 1; i <= nq; i++) {
				thy z[i][iframe] = cepstrum -> z[1][i];
			}
			if ((iframe % 10) == 1) {
				Melder_progress ((double) iframe / nFrames, L"Cepstrogram analysis of frame ",
					Melder_integer (iframe), L" out of ", Melder_integer (nFrames), L".");
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrogram created.");
	}
}

/* End of file Cepstrogram.cpp */
