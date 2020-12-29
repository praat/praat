/* PowerCepstrogram.cpp
 *
 * Copyright (C) 2013 - 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "PowerCepstrogram.h"
#include "Cepstrum_and_Spectrum.h"
#include "NUM2.h"
#include "Sound_and_Spectrum.h"
#include "Sound_extensions.h"


#define TOLOG(x) ((1 / NUMln10) * log ((x) + 1e-30))
#define TO10LOG(x) ((10 / NUMln10) * log ((x) + 1e-30))
#define FROMLOG(x) (exp ((x) * (NUMln10 / 10.0)) - 1e-30)

Thing_implement (PowerCepstrogram, Matrix, 2); // derives from Matrix -> also version 2

double structPowerCepstrogram :: v_getValueAtSample (integer sampleNumber, integer row, int unit) {
	double result = undefined;
	if (row >= 1 && row <= ny) {
		if (unit == 0)
			result = z [row] [sampleNumber];
		else
			result = 10.0 * log10 (z [row] [sampleNumber] + 1e-30); // always positive
	}
	return result;
}

autoPowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, integer nt, double dt, double t1,
	double qmin, double qmax, integer nq, double dq, double q1) {
	try {
		autoPowerCepstrogram me = Thing_new (PowerCepstrogram);
		Matrix_init (me.get(), tmin, tmax, nt, dt, t1, qmin, qmax, nq, dq, q1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"PowerCepstrogram not created.");
	}
}

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBmaximum, int autoscaling, double dynamicRangedB, double dynamicCompression, bool garnish) {
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	if (qmax <= qmin) {
		qmin = my ymin;
		qmax = my ymax;
	}
	integer itmin, itmax, ifmin, ifmax;
	if (Matrix_getWindowSamplesX (me, tmin - 0.49999 * my dx, tmax + 0.49999 * my dx, & itmin, & itmax) == 0 ||
			Matrix_getWindowSamplesY (me, qmin - 0.49999 * my dy, qmax + 0.49999 * my dy, & ifmin, & ifmax) == 0)
		return;
	autoMatrix thee = Data_copy (me);
	MelderExtremaWithInit extrema;
	for (integer irow = 1; irow <= my ny; irow ++) {
		for (integer icol = 1; icol <= my nx; icol ++) {
			double val = TO10LOG (my z [irow] [icol]);
			extrema.update (val);
			thy z [irow] [icol] = val;
		}
	}
	double dBminimum = dBmaximum - dynamicRangedB;
	if (autoscaling) {
		dBminimum = extrema.min;
		dBmaximum = extrema.max;
	}

	for (integer icol = 1; icol <= my nx; icol ++) {
		const double lmax = NUMmax (thy z.column (icol));
		const double factor = dynamicCompression * (extrema.max - lmax);
		thy z.column(icol) += factor;
	}
	
	Graphics_setInner (g);
	Graphics_setWindow (g, tmin, tmax, qmin, qmax);
	Graphics_image (g, thy z.part (ifmin, ifmax, itmin, itmax),
		Matrix_columnToX (thee.get(), itmin - 0.5),
		Matrix_columnToX (thee.get(), itmax + 0.5),
		Matrix_rowToY (thee.get(), ifmin - 0.5),
		Matrix_rowToY (thee.get(), ifmax + 0.5),
		dBminimum, dBmaximum);

	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_marksLeft (g, 2, true, true, false);
		Graphics_textLeft (g, true, U"Quefrency (s)");
	}
}

void PowerCepstrogram_subtractTrend_inplace (PowerCepstrogram me, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		for (integer icol = 1; icol <= my nx; icol ++) {
			thy z.row (1) <<= my z.column (icol);
			PowerCepstrum_subtractTrend_inplace (thee.get(), qstartFit, qendFit, lineType, fitMethod);
			my z.column (icol) <<= thy z.row (1);
		}
	} catch (MelderError) {
		Melder_throw (me, U": no tilt subtracted (inline).");
	}
}

autoPowerCepstrogram PowerCepstrogram_subtractTrend (PowerCepstrogram me, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		PowerCepstrogram_subtractTrend_inplace (thee.get(), qstartFit, qendFit, lineType, fitMethod);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no tilt subtracted.");
	}
	
}

autoTable PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling) {
	try {
		autoTable thee = Table_createWithColumnNames (my nx, U"time quefrency cpp f0");
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (integer icol = 1; icol <= my nx; icol ++) {
			his z.row (1) <<= my z.column (icol);
			double qpeak;
			const double cpp = PowerCepstrum_getPeakProminence_hillenbrand (him.get(), pitchFloor, pitchCeiling, & qpeak);
			const double time = Sampled_indexToX (me, icol);
			Table_setNumericValue (thee.get(), icol, 1, time);
			Table_setNumericValue (thee.get(), icol, 2, qpeak);
			Table_setNumericValue (thee.get(), icol, 3, cpp); // Cepstrogram_getCPPS depends on this index 3!!
			Table_setNumericValue (thee.get(), icol, 4, 1.0 / qpeak);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table with cepstral peak prominence values created.");
	}
}

autoTable PowerCepstrogram_to_Table_cpp (PowerCepstrogram me, double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	try {
		autoTable thee = Table_createWithColumnNames (my nx, U"time quefrency cpp f0 rnr");
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			his z.row (1) <<= my z.column (iframe);
			double qpeak, cpp = PowerCepstrum_getPeakProminence (him.get(), pitchFloor, pitchCeiling, peakInterpolationType,
				qstartFit, qendFit, lineType, fitMethod, & qpeak);
			double rnr = PowerCepstrum_getRNR (him.get(), pitchFloor, pitchCeiling, deltaF0);
			double time = Sampled_indexToX (me, iframe);
			Table_setNumericValue (thee.get(), iframe, 1, time);
			Table_setNumericValue (thee.get(), iframe, 2, qpeak);
			Table_setNumericValue (thee.get(), iframe, 3, cpp); // Cepstrogram_getCPPS depends on this index!!
			Table_setNumericValue (thee.get(), iframe, 4, 1.0 / qpeak);
			Table_setNumericValue (thee.get(), iframe, 5, rnr);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Table with cepstral peak prominence values created.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothRectangular (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		/*
			1. average across time
		*/
		integer numberOfFrames = Melder_ifloor (timeAveragingWindow / my dx);
		if (numberOfFrames > 1) {
			const double halfWindwow = 0.5 * timeAveragingWindow;
			autoVEC qout = raw_VEC (my nx);
			for (integer iq = 1; iq <= my ny; iq ++) {
				for (integer iframe = 1; iframe <= my nx; iframe ++) {
					const double xmid = Sampled_indexToX (me, iframe);
					qout [iframe] = Sampled_getMean (me, xmid - halfWindwow, xmid + halfWindwow, iq, 0, true);
				}
				thy z.row (iq)  <<=  qout.all();
			}
		}
		/*
			2. average across quefrencies
		*/
		integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dy);
		if (numberOfQuefrencyBins > 1) {
			autoPowerCepstrum smooth = PowerCepstrum_create (thy ymax, thy ny);
			for (integer iframe = 1; iframe <= thy nx; iframe ++) {
				smooth -> z.row (1)  <<=  thy z.column (iframe);
				PowerCepstrum_smooth_inplace (smooth.get(), quefrencyAveragingWindow, 1);
				thy z.column (iframe)  <<=  smooth -> z.row (1);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothRectangular_old (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		/*
			1. average across time
		*/
		integer numberOfFrames = Melder_ifloor (timeAveragingWindow / my dx);
		if (numberOfFrames > 1)
			for (integer iq = 1; iq <= my ny; iq ++)
				VECsmoothByMovingAverage_preallocated (thy z.row (iq), my z.row (iq), numberOfFrames);
		/*
			2. average across quefrencies
		*/
		integer numberOfQuefrencyBins = Melder_ifloor (quefrencyAveragingWindow / my dy);
		if (numberOfQuefrencyBins > 1) {
			autoVEC qin = raw_VEC (thy ny);
			for (integer iframe = 1; iframe <= my nx; iframe ++) {
				qin.all() <<= thy z.column (iframe);
				VECsmoothByMovingAverage_preallocated (thy z.column (iframe), qin.all(), numberOfQuefrencyBins);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

static autoPowerCepstrogram PowerCepstrogram_smoothGaussian (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);		
		/*
			1. average across time
		*/
		const double numberOfSigmasInWindow = 4.0;
		const double numberOfFrames = timeAveragingWindow / my dx;
		if (numberOfFrames > 1.0) {
			const double sigma = numberOfFrames / numberOfSigmasInWindow;  // 2sigma -> 95.4%, 3sigma -> 99.7 % of the data
			integer nfft = 2;
			while (nfft < my nx) 
				nfft *= 2;
			autoNUMfft_Table fourierTable;
			NUMfft_Table_init (& fourierTable, nfft);
			for (integer iq = 1; iq <= my ny; iq ++) {
				VECsmooth_gaussian (thy z .row (iq), my z.row (iq), sigma, & fourierTable);
				VECabs_inplace (thy z .row (iq));
			}
		}
		/*
			2. average across quefrencies
		*/
		const double numberOfQuefrencyBins = quefrencyAveragingWindow / my dy;
		if (numberOfQuefrencyBins > 1.0) {
			integer nfft = 2;
			while (nfft < my ny)
				nfft *= 2;
			autoNUMfft_Table fourierTable;
			NUMfft_Table_init (& fourierTable, nfft);
			const double sigma = numberOfQuefrencyBins / numberOfSigmasInWindow;  // 2sigma -> 95.4%, 3sigma -> 99.7 % of the data
			for (integer iframe = 1; iframe <= my nx; iframe ++) {
				VECsmooth_gaussian_inplace (thy z.column (iframe), sigma, & fourierTable);
				VECabs_inplace (thy z.column (iframe));
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not smoothed.");
	}
}

autoPowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	if (Melder_debug == -4)
		return PowerCepstrogram_smoothRectangular_old (me, timeAveragingWindow, quefrencyAveragingWindow);
	else if (Melder_debug == -5)
		return PowerCepstrogram_smoothGaussian (me, timeAveragingWindow, quefrencyAveragingWindow);
	else
		return PowerCepstrogram_smoothRectangular (me, timeAveragingWindow, quefrencyAveragingWindow);
}

autoMatrix PowerCepstrogram_to_Matrix (PowerCepstrogram me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoPowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time) {
	try {
		integer iframe = Sampled_xToNearestIndex (me, time);
		iframe = iframe < 1 ? 1 : iframe > my nx ? my nx : iframe;
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		thy z.row (1)  <<=  my z.column (iframe);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": Cepstrum not extracted.");
	}
}

autoPowerCepstrogram Matrix_to_PowerCepstrogram (Matrix me) {
	try {
		autoPowerCepstrogram thee = Thing_new (PowerCepstrogram);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to PowerCepstrogram.");
	}
}

autoPowerCepstrogram Sound_to_PowerCepstrogram (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		const double analysisWidth = 3.0  / pitchFloor; // minimum analysis window has 3 periods of lowest pitch
		double windowDuration = 2.0 * analysisWidth; // gaussian window
		integer nFrames;

		// Convenience: analyse the whole sound into one Cepstrogram_frame
		if (windowDuration > my dx * my nx)
			windowDuration = my dx * my nx;
		const double samplingFrequency = 2 * maximumFrequency;
		autoSound sound = Sound_resample (me, samplingFrequency, 50);
		Sound_preEmphasis (sound.get(), preEmphasisFrequency);
		double t1;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & nFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		/*
			Find out the size of the FFT
		*/
		integer nfft = 2;
		while (nfft < sframe -> nx)
			nfft *= 2;
		const integer nq = nfft / 2 + 1;
		const double qmax = 0.5 * nfft / samplingFrequency, dq = qmax / (nq - 1);
		autoPowerCepstrogram thee = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, nq, dq, 0);

		autoMelderProgress progress (U"Cepstrogram analysis");

		for (integer iframe = 1; iframe <= nFrames; iframe++) {
			const double t = Sampled_indexToX (thee.get(), iframe);
			Sound_into_Sound (sound.get(), sframe.get(), t - windowDuration / 2);
			Vector_subtractMean (sframe.get());
			Sounds_multiply (sframe.get(), window.get());
			autoSpectrum spec = Sound_to_Spectrum (sframe.get(), true);   // FFT yes
			autoPowerCepstrum cepstrum = Spectrum_to_PowerCepstrum (spec.get());
			for (integer i = 1; i <= nq; i ++)
				thy z [i] [iframe] = cepstrum -> z [1] [i];

			if ((iframe % 10) == 1)
				Melder_progress ((double) iframe / nFrames, U"PowerCepstrogram analysis of frame ",
					iframe, U" out of ", nFrames, U".");
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no PowerCepstrogram created.");
	}
}


//       1           2                          nfftdiv2
//    re   im    re     im                   re      im
// ((fft[1],0) (fft[2],fft[3]), (,), (,), (fft[nfft], 0))  nfft even
// ((fft[1],0) (fft[2],fft[3]), (,), (,), (fft[nfft-1], fft[nfft]))  nfft uneven
static void complexfftoutput_to_power (constVEC fft, VEC dbs, bool to_db) {
	double valsq = fft [1] * fft [1];
	dbs[1] = to_db ? TOLOG (valsq) : valsq;
	const integer nfftdiv2p1 = (fft.size + 2) / 2;
	const integer nend = fft.size % 2 == 0 ? nfftdiv2p1 : nfftdiv2p1 + 1;
	for (integer i = 2; i < nend; i ++) {
		const double re = fft [i + i - 2], im = fft [i + i - 1];
		valsq = re * re + im * im;
		dbs [i] = to_db ? TOLOG (valsq) : valsq;
	}
	if (fft.size % 2 == 0) {
		valsq = fft [fft.size] * fft [fft.size];
		dbs[nfftdiv2p1] = to_db ? TOLOG (valsq) : valsq;
	}
}

autoPowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (Sound me, double pitchFloor, double dt) {
	try {
		// minimum analysis window has 3 periods of lowest pitch
		const double physicalDuration = my dx * my nx;
		const double analysisWidth = std::min (3.0 / pitchFloor, physicalDuration);

		double samplingFrequency = 1.0 / my dx;
		autoSound thee;
		if (samplingFrequency > 30000.0) {
			samplingFrequency = samplingFrequency / 2.0;
			thee = Sound_resample (me, samplingFrequency, 1);
		} else {
			thee = Data_copy (me);
		}
		/*
			Pre-emphasis with fixed coefficient 0.9
		*/
		for (integer i = thy nx; i > 1; i --)
			thy z [1] [i] -= 0.9 * thy z [1] [i - 1];

		const integer nosInWindow = Melder_ifloor (analysisWidth * samplingFrequency);
		Melder_require (nosInWindow >= 8,
			U"Analysis window too short.");

		double t1;
		integer numberOfFrames;
		Sampled_shortTermAnalysis (thee.get(), analysisWidth, dt, & numberOfFrames, & t1);
		autoVEC hamming = raw_VEC (nosInWindow);
		for (integer i = 1; i <= nosInWindow; i ++)
			hamming [i] = 0.54 - 0.46 * cos (NUM2pi * (i - 1) / (nosInWindow - 1));

		integer nfft = 8; // minimum possible
		while (nfft < nosInWindow)
			nfft *= 2;
		const integer nfftdiv2 = nfft / 2;
		autoVEC fftbuf = zero_VEC (nfft); // "complex" array
		autoVEC spectrum = zero_VEC (nfftdiv2 + 1); // +1 needed 
		autoNUMfft_Table fftTable;
		NUMfft_Table_init (& fftTable, nfft); // sound to spectrum
		
		const double qmax = 0.5 * nfft / samplingFrequency, dq = qmax / (nfftdiv2 + 1);
		autoPowerCepstrogram him = PowerCepstrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, 0, qmax, nfftdiv2+1, dq, 0);
		
		autoMelderProgress progress (U"Cepstrogram analysis");
		
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double tbegin = std::max (thy xmin, t1 + (iframe - 1) * dt - analysisWidth / 2.0);
			const integer istart = std::max (1_integer, Sampled_xToLowIndex (thee.get(), tbegin));   // ppgb: afronding naar beneden?
			integer iend = istart + nosInWindow - 1;
			if (iend > thy nx)
				iend = thy nx;
			fftbuf.part (1, nosInWindow) <<= thy z.row (1).part (istart, iend) * hamming.all();
			fftbuf.part (nosInWindow + 1, nfft) <<= 0.0;
			
			NUMfft_forward (& fftTable, fftbuf.get());
			complexfftoutput_to_power (fftbuf.get(), spectrum.get(), true); // log10(|fft|^2)
		
			centre_VEC_inout (spectrum.get()); // subtract average
			/*
			 * Here we diverge from Hillenbrand as he takes the fft of half of the spectral values.
			 * H. forgets that the actual spectrum has nfft/2+1 values. Thefore, we take the inverse
			 * transform because this keeps the number of samples a power of 2.
			 * At the same time this results in twice as many numbers in the quefrency domain, i.e. we end up with nfft/2+1
			 * numbers while H. has only nfft/4!
			 */
			fftbuf [1] = spectrum [1];
			for (integer i = 2; i < nfftdiv2 + 1; i ++) {
				fftbuf [i+i-2] = spectrum [i];
				fftbuf [i+i-1] = 0.0;
			}
			fftbuf [nfft] = spectrum [nfftdiv2 + 1];
			NUMfft_backward (& fftTable, fftbuf.get());
			for (integer i = 1; i <= nfftdiv2 + 1; i ++)
				his z [i] [iframe] = fftbuf [i] * fftbuf [i];

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Cepstrogram analysis of frame ",
					 iframe, U" out of ", numberOfFrames, U".");
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": no Cepstrogram created.");
	}
}

double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, double deltaF0, kVector_peakInterpolation peakInterpolationType, double qstartFit, double qendFit, kCepstrumTrendType lineType, kCepstrumTrendFit fitMethod) {
	try {
		autoPowerCepstrogram flattened;
		if (subtractTiltBeforeSmoothing)
			flattened = PowerCepstrogram_subtractTrend (me, qstartFit, qendFit, lineType, fitMethod);

		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (flattened ? flattened.get() : me, timeAveragingWindow, quefrencyAveragingWindow);
		autoTable table = PowerCepstrogram_to_Table_cpp (smooth.get(), pitchFloor, pitchCeiling, deltaF0, peakInterpolationType, qstartFit, qendFit, lineType, fitMethod);
		const double cpps = Table_getMean (table.get(), 3);
		return cpps;
	} catch (MelderError) {
		Melder_throw (me, U": no CPPS value calculated.");
	}
}

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling) {
	try {
		autoPowerCepstrogram him;
		if (subtractTiltBeforeSmoothing)
			him = PowerCepstrogram_subtractTrend (me, 0.001, 0, kCepstrumTrendType::LINEAR, kCepstrumTrendFit::LEAST_SQUARES);

		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (subtractTiltBeforeSmoothing ? him.get() : me, timeAveragingWindow, quefrencyAveragingWindow);
		autoTable table = PowerCepstrogram_to_Table_hillenbrand (smooth.get(), pitchFloor, pitchCeiling);
		const double cpps = Table_getMean (table.get(), 3);
		return cpps;
	} catch (MelderError) {
		Melder_throw (me, U": no CPPS value calculated.");
	}
}

/* End of file PowerCepstrogram.cpp */
