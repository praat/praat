/* Sound_to_Cochleagram.cpp
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
 * pb 1997/09/03 BUG: exponential decay only for iframe > 1
 * pb 1998/01/05 forward masking time
 * pb 2002/07/16 GPL
 * pb 2002/08/26 correct handling of zero forwardMaskingTime (bug found by djmw)
 * pb 2004/11/22 simplified Sound_to_Spectrum ()
 * pb 2006/12/30 new Sound_create API
 * pb 2007/01/28 made compatible with stereo sounds
 * pb 2008/01/19 double
 * pb 2011/06/08 C++
 */

#include "Sound_to_Cochleagram.h"
#include "Sound_and_Spectrum.h"
#include "Spectrum_to_Excitation.h"

Cochleagram Sound_to_Cochleagram (Sound me, double dt, double df, double dt_window, double forwardMaskingTime) {
	try {
		double duration = my nx * my dx;
		long nFrames = 1 + floor ((duration - dt_window) / dt);
		long nsamp_window = floor (dt_window / my dx), halfnsamp_window = nsamp_window / 2 - 1;
		long nf = floor (25.6 / df + 0.5);
		double dampingFactor = forwardMaskingTime > 0.0 ? exp (- dt / forwardMaskingTime) : 0.0;   /* Default 30 ms. */
		double integrationCorrection = 1.0 - dampingFactor;

		nsamp_window = halfnsamp_window * 2;
		if (nFrames < 2) return NULL;
		double t1 = my x1 + 0.5 * (duration - my dx - (nFrames - 1) * dt); /* Centre of first frame. */
		autoCochleagram thee = Cochleagram_create (my xmin, my xmax, nFrames, dt, t1, df, nf);
		autoSound window = Sound_createSimple (1, nsamp_window * my dx, 1.0 / my dx);
		for (long iframe = 1; iframe <= nFrames; iframe ++) {
			double t = Sampled_indexToX (thee.peek(), iframe);
			long leftSample = Sampled_xToLowIndex (me, t);
			long rightSample = leftSample + 1;
			long startSample = rightSample - halfnsamp_window;
			long endSample = rightSample + halfnsamp_window;
			if (startSample < 1) {
				Melder_casual ("Start sample too small: %ld instead of 1.", startSample);
				startSample = 1;
			}
			if (endSample > my nx) {
				Melder_casual ("End sample too small: %ld instead of %ld.", endSample, my nx);
				endSample = my nx;
			}

			/* Copy a window to a frame. */
			for (long i = 1; i <= nsamp_window; i ++)
				window -> z [1] [i] =
					( my ny == 1 ? my z[1][i+startSample-1] : 0.5 * (my z[1][i+startSample-1] + my z[2][i+startSample-1]) ) *
					(0.5 - 0.5 * cos (2 * NUMpi * i / (nsamp_window + 1)));
			autoSpectrum spec = Sound_to_Spectrum (window.peek(), TRUE);
			autoExcitation excitation = Spectrum_to_Excitation (spec.peek(), df);
			for (long ifreq = 1; ifreq <= nf; ifreq ++)
				thy z [ifreq] [iframe] = excitation -> z [1] [ifreq] + ( iframe > 1 ? dampingFactor * thy z [ifreq] [iframe - 1] : 0 );
		}
		for (long iframe = 1; iframe <= nFrames; iframe ++)
			for (long ifreq = 1; ifreq <= nf; ifreq ++)
				thy z [ifreq] [iframe] *= integrationCorrection;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Cochleagram.");
	}
}

static Sound createGammatone (double midFrequency_Hertz, double samplingFrequency) {
	double lengthOfGammatone_seconds = 50.0 / midFrequency_Hertz;   // 50 periods
	long lengthOfGammatone_samples;
	/* EdB's alfa1: */
	double latency = 1.95e-3 * pow (midFrequency_Hertz / 1000, -0.725) + 0.6e-3;
	/* EdB's beta: */
	double decayTime = 1e-3 * pow (midFrequency_Hertz / 1000, -0.663);
	/* EdB's omega: */
	double midFrequency_radPerSecond = 2 * NUMpi * midFrequency_Hertz;
	autoSound gammatone = Sound_createSimple (1, lengthOfGammatone_seconds, samplingFrequency);
	lengthOfGammatone_samples = gammatone -> nx;
	for (long itime = 1; itime <= lengthOfGammatone_samples; itime ++) {
		double time_seconds = (itime - 0.5) / samplingFrequency;
		double timeAfterLatency = time_seconds - latency;
		double x = timeAfterLatency / decayTime;
		if (time_seconds > latency) gammatone -> z [1] [itime] =
			x * x * x * exp (- x) * cos (midFrequency_radPerSecond * timeAfterLatency);
	}
	return gammatone.transfer();
}

Cochleagram Sound_to_Cochleagram_edb
	(Sound me, double dtime, double dfreq, int hasSynapse, double replenishmentRate,
	 double lossRate, double returnRate, double reprocessingRate)
{
	try {
		double duration_seconds = my xmax;
		if (dtime < my dx) dtime = my dx;
		long ntime = floor (duration_seconds / dtime + 0.5);
		if (ntime < 2) return NULL;
		long nfreq = floor (25.6 / dfreq + 0.5);   // 25.6 Bark = highest frequency

		autoCochleagram thee = Cochleagram_create (my xmin, my xmax, ntime, dtime, 0.5 * dtime, dfreq, nfreq);

		/* Stages 1 and 2: outer- and middle-ear filtering. */
		/* From acoustic sound to oval window. */

		for (long ifreq = 1; ifreq <= nfreq; ifreq ++) {
			double *response = thy z [ifreq];

			/* Stage 3: basilar membrane filtering by gammatones. */
			/* From oval window to basilar membrane response. */

			double midFrequency_Bark = (ifreq - 0.5) * dfreq;
			double midFrequency_Hertz = Excitation_barkToHertz (midFrequency_Bark);
			autoSound gammatone = createGammatone (midFrequency_Hertz, 1 / my dx);
			autoSound basil = Sounds_convolve (me, gammatone.peek(), kSounds_convolve_scaling_SUM, kSounds_convolve_signalOutsideTimeDomain_ZERO);

			/* Stage 4: detection = rectify + integrate + low-pass 500 Hz. */
			/* From basilar membrane response to firing rate. */

			if (hasSynapse) {
				double dt = my dx;
				double M = 1;   /* Maximum free transmitter. */
				double A = 5, B = 300, g = 2000;   /* Determine permeability. */
				double y = replenishmentRate;          /* Meddis: 5.05 */
				double l = lossRate, r = returnRate;   /* Meddis: 2500, 6580 */
				double x = reprocessingRate;           /* Meddis: 66.31 */
				double h = 50000;   /* Convert cleft contents to firing rate. */
				double gdt = 1 - exp (- g * dt), ydt = 1 - exp (- y * dt),
						 ldt = (1 - exp (- (l + r) * dt)) * l / (l + r),
						 rdt = (1 - exp (- (l + r) * dt)) * r / (l + r),
						 xdt = 1 - exp (- x * dt);
				double kt = g * A / (A + B);   /* Membrane permeability. */
				double c = M * y * kt / (l * kt + y * (l + r));   /* Cleft contents. */
				double q = c * (l + r) / kt;   /* Free transmitter. */
				double w = c * r / x;   /* Reprocessing store. */
				for (long itime = 1; itime <= basil -> nx; itime ++) {
					double splusA = basil -> z [1] [itime] * 10 + A;
					double replenish = M > q ? ydt * (M - q) : 0;
					double eject, loss, reuptake, reprocess;
					kt = splusA > 0 ? gdt * splusA / (splusA + B) : 0;
					eject = kt * q;
					loss = ldt * c;
					reuptake = rdt * c;
					reprocess = xdt * w;
					q = q + replenish - eject + reprocess;
					c = c + eject - loss - reuptake;
					w = w + reuptake - reprocess;
					basil -> z [1] [itime] = h * c;
				}
			}
			
			if (dtime == my dx) {
				for (long itime = 1; itime <= ntime; itime ++)
					response [itime] = basil -> z [1] [itime];
			} else {
				double d = dtime / basil -> dx / 2;
				double factor = -6 / d / d;
				double area = d * sqrt (NUMpi / 6);
				double expmin6 = exp (-6), onebyoneminexpmin6 = 1 / (1 - expmin6);
				for (long itime = 1; itime <= ntime; itime ++) {
					double t1 = (itime - 1) * dtime, t2 = t1 + dtime, mean = 0;
					long i1, i2;
					long n = Matrix_getWindowSamplesX (basil.peek(), t1, t2, & i1, & i2);
					Melder_assert (n >= 1);
					if (n <= 2) {
						for (long isamp = i1; isamp <= i2; isamp ++)
							mean += basil -> z [1] [isamp];
						mean /= n;
					} else {
						double mu = floor ((i1 + i2) / 2.0);
						long muint = mu, dint = d;
						for (long isamp = muint - dint; isamp <= muint + dint; isamp ++) {
							double y = 0;
							if (isamp < 1 || isamp > basil -> nx)
								Melder_casual ("isamp %ld", isamp);
							else
								y = basil -> z [1] [isamp];
							mean += y * onebyoneminexpmin6 * (exp (factor * (isamp - muint) *
								(isamp - muint)) - expmin6);
						}
						mean /= area;
					}
					response [itime] = mean;
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Cochleagram (edb).");
	}
}

/* End of file Sound_to_Cochleagram.cpp */
