/* Sound_to_Harmonicity_GNE.cpp
 *
 * Copyright (C) 1999-2012,2015-2020 Paul Boersma
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

/* a replication of:
    D. Michaelis, T. Gramss & H.W. Strube (1997):
       "Glottal-to-noise excitation ratio -- a new measure
        for describing pathological voices."
       ACUSTICA - acta acustica 83: 700-706.
 henceforth abbreviated as "MGS".
*/

#include "Sound_to_Harmonicity.h"
#include "Sound_and_LPC.h"
#include "Sound_and_Spectrum.h"

static void bandFilter (Spectrum me, double fmid, double bandwidth) {
	double *re = & my z [1] [0], *im = & my z [2] [0];
	double fmin = fmid - bandwidth / 2.0, fmax = fmid + bandwidth / 2.0;
	double twopibybandwidth = 2.0 * NUMpi / bandwidth;
	for (integer col = 1; col <= my nx; col ++) {
		double x = my x1 + (col - 1) * my dx;
		if (x < fmin || x > fmax) {
			re [col] = 0.0;
			im [col] = 0.0;
		} else {
			double factor = 0.5 + 0.5 * cos (twopibybandwidth * (x - fmid));
			re [col] *= factor;
			im [col] *= factor;
		}
	}
}

autoMatrix Sound_to_Harmonicity_GNE (Sound me,
	double fmin,   // 500 Hz
	double fmax,   // 4500 Hz
	double bandwidth,  // 1000 Hz
	double step)   // 80 Hz
{
	try {
		autoSound envelope [1+100];
		integer nenvelopes = Melder_ifloor ((fmax - fmin) / step);
		for (integer ienvelope = 1; ienvelope <= 100; ienvelope ++)
			Melder_assert (! envelope [ienvelope].get());

		/*
		 * Step 1: down-sampling to 10 kHz,
		 * in order to be able to flatten the spectrum
		 * (since the human voice does not contain much above 5 kHz).
		 */
		autoSound original10k = Sound_resample (me, 10000, 500);
		Vector_subtractMean (original10k.get());
		double duration = my xmax - my xmin;

		/*
		 * Step 2: inverse filtering of the speech signal
		 * by 13th-order "autocorrelation method"
		 * with a Gaussian (not Hann, like MGS!) window of 30 ms length
		 * and 10 ms shift between successive frames.
		 * Since we need a spectrally flat signal (not an approximation
		 * of the source signal), we must turn the pre-emphasis off
		 * (by setting its turnover point at 1,000,000,000 Hz);
		 * otherwise, the pre-emphasis would cause an overestimation
		 * in the LPC object of the high frequencies, so that inverse
		 * filtering would yield weakened high frequencies.
		 */
		autoLPC lpc = Sound_to_LPC_autocorrelation (original10k.get(), 13, 30e-3, 10e-3, 1e9);
		autoSound flat = LPC_Sound_filterInverse (lpc.get(), original10k.get());
		autoSpectrum flatSpectrum = Sound_to_Spectrum (flat.get(), true);
		autoSpectrum hilbertSpectrum = Data_copy (flatSpectrum.get());
		for (integer col = 1; col <= hilbertSpectrum -> nx; col ++) {
			hilbertSpectrum -> z [1] [col] = flatSpectrum -> z [2] [col];
			hilbertSpectrum -> z [2] [col] = - flatSpectrum -> z [1] [col];
		}
		double fmid = fmin;
		integer ienvelope = 1;
		autoMelderMonitor monitor (U"Computing Hilbert envelopes...");
		while (fmid <= fmax) {
			/*
			 * Step 3: calculate Hilbert envelopes of bands.
			 */
			autoSpectrum bandSpectrum = Data_copy (flatSpectrum.get());
			autoSpectrum hilbertBandSpectrum = Data_copy (hilbertSpectrum.get());
			/*
			 * 3a: Filter both the spectrum of the original flat sound and its Hilbert transform.
			 */
			bandFilter (bandSpectrum.get(), fmid, bandwidth);
			bandFilter (hilbertBandSpectrum.get(), fmid, bandwidth);
			/*
			 * 3b: Create both the band-filtered flat sound and its Hilbert transform.
			 */
			autoSound band = Spectrum_to_Sound (bandSpectrum.get());
			/*if (graphics) {
				Graphics_beginMovieFrame (graphics, & Melder_WHITE);
				Spectrum_draw (bandSpectrum, graphics, 0, 5000, 0, 0, true);
				Graphics_endMovieFrame (graphics, 0.0);
			}*/
			Melder_monitor (ienvelope / (nenvelopes + 1.0), U"Computing Hilbert envelope ", ienvelope, U"...");
			autoSound hilbertBand = Spectrum_to_Sound (hilbertBandSpectrum.get());
			envelope [ienvelope] = Sound_extractPart (band.get(), 0, duration, kSound_windowShape::RECTANGULAR, 1.0, true);
			/*
			 * 3c: Compute the Hilbert envelope of the band-passed flat signal.
			 */
			for (integer col = 1; col <= envelope [ienvelope] -> nx; col ++) {
				double self = envelope [ienvelope] -> z [1] [col], other = hilbertBand -> z [1] [col];
				envelope [ienvelope] -> z [1] [col] = sqrt (self * self + other * other);
			}
			Vector_subtractMean (envelope [ienvelope].get());
			/*
			 * Next band.
			 */
			fmid += step;
			ienvelope += 1;
		}

		/*
		 * Step 4: crosscorrelation
		 */
		nenvelopes = ienvelope - 1;
		autoMatrix cc = Matrix_createSimple (nenvelopes, nenvelopes);
		for (integer row = 2; row <= nenvelopes; row ++) {
			for (integer col = 1; col <= row - 1; col ++) {
				autoSound crossCorrelation = Sounds_crossCorrelate_short (envelope [row].get(), envelope [col].get(), -3.1e-4, 3.1e-4, true);
				/*
				 * Step 5: the maximum of each correlation function
				 */
				double ccmax = Vector_getMaximum (crossCorrelation.get(), 0.0, 0.0, kVector_peakInterpolation :: NONE);
				cc -> z [row] [col] = ccmax;
			}
		}

		/*
		 * Step 6: maximum of the maxima, ignoring those too close to the diagonal.
		 */	
		for (integer row = 2; row <= nenvelopes; row ++) {
			for (integer col = 1; col <= row - 1; col ++) {
				if (integer_abs (row - col) < bandwidth / 2.0 / step) {
					cc -> z [row] [col] = 0.0;
				}
			}
		}

		return cc;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Harmonicity (GNE).");
	}
}

/* End of file Sound_to_Harmonicity_GNE.cpp */
