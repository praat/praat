/* Sound_to_Harmonicity_GNE.c
 *
 * Copyright (C) 1999-2004 Paul Boersma
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
 * pb 2004/11/22 simplified Sound_to_Spectrum ()
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
	long col;
	float *re = my z [1], *im = my z [2];
	double fmin = fmid - bandwidth / 2, fmax = fmid + bandwidth / 2;
	double twopibybandwidth = 2 * NUMpi / bandwidth;
	for (col = 1; col <= my nx; col ++) {
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

Matrix Sound_to_Harmonicity_GNE (Sound me,
	double fmin,   /* 500 Hz */
	double fmax,   /* 4500 Hz */
	double bandwidth,  /* 1000 Hz */
	double step)   /* 80 Hz */
{
	Sound original10k = NULL, flat = NULL, envelope [1+100], crossCorrelation = NULL;
	Sound band = NULL, hilbertBand = NULL;
	LPC lpc = NULL;
	Spectrum flatSpectrum = NULL, hilbertSpectrum = NULL, bandSpectrum = NULL, hilbertBandSpectrum = NULL;
	Matrix cc = NULL;
	double duration, fmid;
	Graphics graphics;
	long ienvelope, row, col, nenvelopes = (fmax - fmin) / step, nsamp;
	for (ienvelope = 1; ienvelope <= 100; ienvelope ++)
		envelope [ienvelope] = NULL;

	/*
	 * Step 1: down-sampling to 10 kHz,
	 * in order to be able to flatten the spectrum
	 * (since the human voice does not contain much above 5 kHz).
	 */
	original10k = Sound_resample (me, 10000, 500); cherror
	Vector_subtractMean (original10k);
	duration = my xmax - my xmin;

	/*
	 * Step 2: inverse filtering of the speech signal
	 * by 13th-order "autocorrelation method"
	 * with a Gaussian (not Hann, like MGS!) window of 30 ms length
	 * and 10 ms shift between successive frames.
	 * Since we need a spectrally flat signal (not an approximation
	 * of the source signal), we must turn the pre-emphasis off
	 * (by setting its turnover point at 1,000,000,000 Hertz);
	 * otherwise, the pre-emphasis would cause an overestimation
	 * in the LPC object of the high frequencies, so that inverse
	 * filtering would yield weakened high frequencies.
	 */
	lpc = Sound_to_LPC_auto (original10k, 13, 30e-3, 10e-3, 1e9); cherror
	flat = LPC_and_Sound_filterInverse (lpc, original10k); cherror
	forget (original10k);
	forget (lpc);
	flatSpectrum = Sound_to_Spectrum (flat, TRUE); cherror
	hilbertSpectrum = Data_copy (flatSpectrum); cherror
	for (col = 1; col <= hilbertSpectrum -> nx; col ++) {
		hilbertSpectrum -> z [1] [col] = flatSpectrum -> z [2] [col];
		hilbertSpectrum -> z [2] [col] = - flatSpectrum -> z [1] [col];
	}
	fmid = fmin;
	ienvelope = 1;
	graphics = Melder_monitor (0.0, "Computing Hilbert envelopes...");
	while (fmid <= fmax) {
		/*
		 * Step 3: calculate Hilbert envelopes of bands.
		 */
		bandSpectrum = Data_copy (flatSpectrum);
		hilbertBandSpectrum = Data_copy (hilbertSpectrum);
		/*
		 * 3a: Filter both the spectrum of the original flat sound and its Hilbert transform.
		 */
		bandFilter (bandSpectrum, fmid, bandwidth);
		bandFilter (hilbertBandSpectrum, fmid, bandwidth);
		/*
		 * 3b: Create both the band-filtered flat sound and its Hilbert transform.
		 */
		band = Spectrum_to_Sound (bandSpectrum); cherror
		/*if (graphics) {
			Graphics_clearWs (graphics);
			Spectrum_draw (bandSpectrum, graphics, 0, 5000, 0, 0, TRUE);
		}*/
		if (! Melder_monitor (ienvelope / (nenvelopes + 1.0), "Computing Hilbert envelope %ld...", ienvelope))
			goto end;
		forget (bandSpectrum);
		hilbertBand = Spectrum_to_Sound (hilbertBandSpectrum); cherror
		forget (hilbertBandSpectrum);
		envelope [ienvelope] = Sound_extractPart (band, 0, duration, enumi (Sound_WINDOW, Rectangular), 1.0, TRUE); cherror
		/*
		 * 3c: Compute the Hilbert envelope of the band-passed flat signal.
		 */
		for (col = 1; col <= envelope [ienvelope] -> nx; col ++) {
			double self = envelope [ienvelope] -> z [1] [col], other = hilbertBand -> z [1] [col];
			envelope [ienvelope] -> z [1] [col] = sqrt (self * self + other * other);
		}
		Vector_subtractMean (envelope [ienvelope]);
		/*
		 * Clean up.
		 */
		forget (band);
		forget (hilbertBand);
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
	nsamp = envelope [1] -> nx;
	cc = Matrix_createSimple (nenvelopes, nenvelopes);
	for (row = 2; row <= nenvelopes; row ++) {
		for (col = 1; col <= row - 1; col ++) {
			double ccmax;
			crossCorrelation = Sounds_crossCorrelate (envelope [row], envelope [col], -3.1e-4, 3.1e-4, TRUE);
			/*
			 * Step 5: the maximum of each correlation function
			 */
			ccmax = Vector_getMaximum (crossCorrelation, 0, 0, 0);
			forget (crossCorrelation);
			cc -> z [row] [col] = ccmax;
		}
	}

	/*
	 * Step 6: maximum of the maxima, ignoring those too close to the diagonal.
	 */	
	for (row = 2; row <= nenvelopes; row ++) {
		for (col = 1; col <= row - 1; col ++) {
			if (abs (row - col) < bandwidth / 2 / step) {
				cc -> z [row] [col] = 0.0;
			}
		}
	}

end:
	Melder_monitor (1.0, NULL);
	forget (original10k);
	forget (lpc);
	forget (flat);
	forget (flatSpectrum);
	forget (hilbertSpectrum);
	forget (bandSpectrum);
	forget (hilbertBandSpectrum);
	forget (band);
	forget (hilbertBand);
	for (ienvelope = 1; ienvelope <= 100; ienvelope ++)
		forget (envelope [ienvelope]);
	forget (crossCorrelation);
	iferror forget (cc);
	return cc;
}

/* End of file Sound_to_Harmonicity_GNE.c */
