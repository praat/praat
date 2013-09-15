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

#define TOLOG(x) ((1 / NUMln10) * log ((x) + 1e-30))
#define TO10LOG(x) ((10 / NUMln10) * log ((x) + 1e-30))
#define FROMLOG(x) (exp ((x) * (NUMln10 / 10.0)) - 1e-30)

Thing_implement (Cepstrogram, Matrix, 2);
Thing_implement (PowerCepstrogram, Cepstrogram, 2); // derives from Matrix -> also version 2

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

PowerCepstrogram PowerCepstrogram_create (double tmin, double tmax, long nt, double dt, double t1,
	double qmin, double qmax, long nq, double dq, double q1) {
	try {
		autoPowerCepstrogram me = Thing_new (PowerCepstrogram);

		Matrix_init (me.peek(), tmin, tmax, nt, dt, t1, qmin, qmax, nq, dq, q1);
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("PowerCepstrogram not created.");
	}
}

void PowerCepstrogram_paint (PowerCepstrogram me, Graphics g, double tmin, double tmax, double qmin, double qmax, double dBminimum, double dBmaximum, int garnish) {
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
			double val = TO10LOG (my z[i][j]);
			min = val < min ? val : min;
			max = val > max ? val : max;
			thy z[i][j] = val;
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

void PowerCepstrogram_subtractTilt_inline (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		for (long i = 1; i <= my nx; i++) {
			for (long j = 1; j <= my ny; j++) {
				thy z[1][j] = my z[j][i];
			}
			PowerCepstrum_subtractTilt_inline (thee.peek(), qstartFit, qendFit, lineType, fitMethod);
			for (long j = 1; j <= my ny; j++) {
				my z[j][i] = thy z[1][j];
			}
		}
	} catch (MelderError) {
		Melder_throw (me, ": no tilt subtracted (inline).");
	}
}

PowerCepstrogram PowerCepstrogram_subtractTilt (PowerCepstrogram me, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoPowerCepstrogram thee = Data_copy (me);
		PowerCepstrogram_subtractTilt_inline (thee.peek(), qstartFit, qendFit, lineType, fitMethod);
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no tilt subtracted.");
	}
	
}

Table PowerCepstrogram_to_Table_hillenbrand (PowerCepstrogram me, double pitchFloor, double pitchCeiling) {
	try {
		autoTable thee = Table_createWithColumnNames (my nx, L"time quefrency cpp f0");
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (long i = 1; i <= my nx; i++) {
			for (long j = 1; j <= my ny; j++) {
				his z[1][j] = my z[j][i];
			}
			double qpeak, cpp = PowerCepstrum_getPeakProminence_hillenbrand (him.peek(), pitchFloor, pitchCeiling, &qpeak);
			double time = Sampled_indexToX (me, i);
			Table_setNumericValue (thee.peek(), i, 1, time);
			Table_setNumericValue (thee.peek(), i, 2, qpeak);
			Table_setNumericValue (thee.peek(), i, 3, cpp); // Cepstrogram_getCPPS depends on this index 3!!
			Table_setNumericValue (thee.peek(), i, 4, 1.0 / qpeak);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Table with cepstral peak prominence values created.");
	}
}

Table PowerCepstrogram_to_Table_cpp (PowerCepstrogram me, double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoTable thee = Table_createWithColumnNames (my nx, L"time quefrency cpp f0 rnr");
		autoPowerCepstrum him = PowerCepstrum_create (my ymax, my ny);
		for (long i = 1; i <= my nx; i++) {
			for (long j = 1; j <= my ny; j++) {
				his z[1][j] = my z[j][i];
			}
			double qpeak, z, cpp = PowerCepstrum_getPeakProminence (him.peek(), pitchFloor, pitchCeiling, interpolation,
				qstartFit, qendFit, lineType, fitMethod, &qpeak);
			double rnr = PowerCepstrum_getRNR (him.peek(), pitchFloor, pitchCeiling, deltaF0);
			double time = Sampled_indexToX (me, i);
			Table_setNumericValue (thee.peek(), i, 1, time);
			Table_setNumericValue (thee.peek(), i, 2, qpeak);
			Table_setNumericValue (thee.peek(), i, 3, cpp); // Cepstrogram_getCPPS depends on this index!!
			Table_setNumericValue (thee.peek(), i, 4, 1.0 / qpeak);
			Table_setNumericValue (thee.peek(), i, 5, rnr);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Table with cepstral peak prominence values created.");
	}
}

PowerCepstrogram PowerCepstrogram_smooth (PowerCepstrogram me, double timeAveragingWindow, double quefrencyAveragingWindow) {
	try {
		autoPowerCepstrogram thee = (PowerCepstrogram) Data_copy (me);
		// 1. average across time
		long numberOfFrames = timeAveragingWindow / my dx;
		if (numberOfFrames > 1) {
			autoNUMvector<double> qin (1, my nx);
			autoNUMvector<double> qout (1, my nx);
			for (long iq = 1; iq <= my ny; iq++) {
				for (long iframe = 1; iframe <= my nx; iframe++) {
					//qin[iframe] = TO10LOG (my z[iq][iframe]);
					qin[iframe] = thy z[iq][iframe];
				}
				NUMvector_smoothByMovingAverage (qin.peek(), my nx, numberOfFrames, qout.peek());
				for (long iframe = 1; iframe <= my nx; iframe++) {
					//thy z[iq][iframe] = FROMLOG (qout[iframe]); // inverse 
					thy z[iq][iframe] = qout[iframe]; // inverse 
				}
			}
		}
		// 2. average across quefrencies
		long numberOfQuefrencyBins = quefrencyAveragingWindow / my dy;
		if (numberOfQuefrencyBins > 1) {
			autoNUMvector<double> qin (1, thy ny);
			autoNUMvector<double> qout (1, thy ny);
			for (long iframe = 1; iframe <= my nx; iframe++) {
				for (long iq = 1; iq <= thy ny; iq++) {
					//qin[iq] = TO10LOG (my z[iq][iframe]);
					qin[iq] = thy z[iq][iframe];
				}
				NUMvector_smoothByMovingAverage (qin.peek(), thy ny, numberOfQuefrencyBins, qout.peek());
				for (long iq = 1; iq <= thy ny; iq++) {
					//thy z[iq][iframe] = FROMLOG (qout[iq]);
					thy z[iq][iframe] = qout[iq];
				}
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not smoothed.");
	}
}

Matrix PowerCepstrogram_to_Matrix (PowerCepstrogram me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Matrix created.");
	}
}

PowerCepstrum PowerCepstrogram_to_PowerCepstrum_slice (PowerCepstrogram me, double time) {
	try {
		long iframe = Sampled_xToNearestIndex (me, time);
		iframe = iframe < 1 ? 1 : iframe > my nx ? my nx : iframe;
		autoPowerCepstrum thee = PowerCepstrum_create (my ymax, my ny);
		for (long i = 1; i <= my ny; i++) {
			thy z[1][i] = my z[i][iframe];
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": Cepstrum not extracted.");
	}
}

PowerCepstrogram Matrix_to_PowerCepstrogram (Matrix me) {
	try {
		autoPowerCepstrogram thee = Thing_new (PowerCepstrogram);
		my structMatrix :: v_copy (thee.peek());
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PowerCepstrogram created.");
	}
}

PowerCepstrogram Sound_to_PowerCepstrogram (Sound me, double pitchFloor, double dt, double maximumFrequency, double preEmphasisFrequency) {
	try {
		// minimum analysis window has 3 periods of lowest pitch
		double analysisWidth = 3  / pitchFloor;
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
		// find out the size of the FFT
		long nfft = 2;
		while (nfft < sframe -> nx) nfft *= 2;
		long nq = nfft / 2 + 1;
		double qmax = 0.5 * nfft / samplingFrequency, dq = qmax / (nq - 1);
		autoPowerCepstrogram thee = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, nq, dq, 0);

		autoMelderProgress progress (L"Cepstrogram analysis");

		for (long iframe = 1; iframe <= nFrames; iframe++) {
			double t = Sampled_indexToX (thee.peek(), iframe);
			Sound_into_Sound (sound.peek(), sframe.peek(), t - windowDuration / 2);
			Vector_subtractMean (sframe.peek());
			Sounds_multiply (sframe.peek(), window.peek());
			autoSpectrum spec = Sound_to_Spectrum (sframe.peek(), 1); // FFT yes
			autoPowerCepstrum cepstrum = Spectrum_to_PowerCepstrum (spec.peek());
			for (long i = 1; i <= nq; i++) {
				thy z[i][iframe] = cepstrum -> z[1][i];
			}
			if ((iframe % 10) == 1) {
				Melder_progress ((double) iframe / nFrames, L"PowerCepstrogram analysis of frame ",
					Melder_integer (iframe), L" out of ", Melder_integer (nFrames), L".");
			}
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no PowerCepstrogram created.");
	}
}

Cepstrum Spectrum_to_Cepstrum_hillenbrand (Spectrum me) {
	try {
		autoNUMfft_Table fftTable;
		// originalNumberOfSamplesProbablyOdd irrelevant
		if (my x1 != 0.0) {
			Melder_throw ("A Fourier-transformable Spectrum must have a first frequency of 0 Hz, not ", my x1, L" Hz.");
		}
		long numberOfSamples = my nx - 1;
		autoCepstrum thee = Cepstrum_create (0.5 / my dx, my nx);
		NUMfft_Table_init (&fftTable, my nx);
		autoNUMvector<double> amp (1, my nx);
		
		for (long i = 1; i <= my nx; i++) {
			amp [i] = my v_getValueAtSample (i, 0, 2);
		}
		NUMfft_forward (&fftTable, amp.peek());
		
		for (long i = 1; i <= my nx; i++) {
			double val = amp[i] / numberOfSamples;// scaling 1/n because ifft(fft(1))= n;
			thy z[1][i] = val * val; // power cepstrum
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": not converted to Sound.");
	}
}

//       1           2                          nfftdiv2
//    re   im    re     im                   re      im
// ((fft[1],0) (fft[2],fft[3]), (,), (,), (fft[nfft], 0))  nfft even
// ((fft[1],0) (fft[2],fft[3]), (,), (,), (fft[nfft-1], fft[nfft]))  nfft uneven
static void complexfftoutput_to_power (double *fft, long nfft, double *dbs, bool to_db) {
	double valsq = fft[1] * fft[1];
	dbs[1] = to_db ? TOLOG (valsq) : valsq;
	long nfftdiv2p1 = (nfft + 2) / 2;
	long nend = nfft % 2 == 0 ? nfftdiv2p1 : nfftdiv2p1 + 1;
	for (long i = 2; i < nend; i++) {
		double re = fft[i + i - 2], im = fft[i + i - 1];
		valsq = re * re + im * im;
		dbs[i] = to_db ? TOLOG (valsq) : valsq;
	}
	if (nfft % 2 == 0) {
		valsq = fft[nfft] * fft[nfft];
		dbs[nfftdiv2p1] = to_db ? TOLOG (valsq) : valsq;
	}
}


PowerCepstrogram Sound_to_PowerCepstrogram_hillenbrand (Sound me, double minimumPitch, double dt) {
	try {
		// minimum analysis window has 3 periods of lowest pitch
		double analysisWidth = 3  / minimumPitch;
		if (analysisWidth > my dx * my nx) {
			analysisWidth = my dx * my nx;
		}
		double t1, samplingFrequency = 1 / my dx;
		autoSound thee;
		if (samplingFrequency > 30000) {
			samplingFrequency = samplingFrequency / 2;
			thee.reset (Sound_resample (me, samplingFrequency, 1));
		} else {
			thee.reset (Data_copy (me));
		}
		// pre-emphasis with fixed coefficient 0.9
		for (long i = thy nx; i > 1; i--) {
			thy z[1][i] -= 0.9 * thy z[1][i - 1];
		}
		long nosInWindow = analysisWidth * samplingFrequency, nFrames;
		if (nosInWindow < 8) {
			Melder_throw ("Analysis window too short.");
		}
		Sampled_shortTermAnalysis (thee.peek(), analysisWidth, dt, & nFrames, & t1);
		autoNUMvector<double> hamming (1, nosInWindow);
		for (long i = 1; i <= nosInWindow; i++) {
			hamming[i] = 0.54 -0.46 * cos(2 * NUMpi * (i - 1) / (nosInWindow - 1));
		}
		long nfft = 8; // minimum possible
		while (nfft < nosInWindow) { nfft *= 2; }
		long nfftdiv2 = nfft / 2;
		autoNUMvector<double> fftbuf (1, nfft); // "complex" array
		autoNUMvector<double> spectrum (1, nfftdiv2 + 1); // +1 needed 
		autoNUMfft_Table fftTable;
		NUMfft_Table_init (&fftTable, nfft); // sound to spectrum
		
		double qmax = 0.5 * nfft / samplingFrequency, dq = qmax / (nfftdiv2 + 1);
		autoPowerCepstrogram him = PowerCepstrogram_create (my xmin, my xmax, nFrames, dt, t1, 0, qmax, nfftdiv2+1, dq, 0);
		
		autoMelderProgress progress (L"Cepstrogram analysis");
		
		for (long iframe = 1; iframe <= nFrames; iframe++) {
			double tbegin = t1 + (iframe - 1) * dt - analysisWidth / 2;
			tbegin = tbegin < thy xmin ? thy xmin : tbegin;
			long istart = Sampled_xToIndex (thee.peek(), tbegin);
			istart = istart < 1 ? 1 : istart;
			long iend = istart + nosInWindow - 1;
			iend = iend > thy nx ? thy nx : iend;
			for (long i = 1; i <= nosInWindow; i++) {
				fftbuf[i] = thy z[1][istart + i - 1] * hamming[i];
			}
			for (long i = nosInWindow + 1; i <= nfft; i++) { 
				fftbuf[i] = 0;
			}
			NUMfft_forward (&fftTable, fftbuf.peek());
			complexfftoutput_to_power (fftbuf.peek(), nfft, spectrum.peek(), true); // log10(|fft|^2)
			// subtract average
			double specmean = spectrum[1];
			for (long i = 2; i <= nfftdiv2 + 1; i++) {
				specmean += spectrum[i];
			}
			specmean /= nfftdiv2 + 1;
			for (long i = 1; i <= nfftdiv2 + 1; i++) {
				spectrum[i] -= specmean;
			}
			/*
			 * Here we diverge from Hillenbrand as he takes the fft of half of the spectral values.
			 * H. forgets that the actual spectrum has nfft/2+1 values. Thefore, we take the inverse
			 * transform because this keeps the number of samples a power of 2.
			 * At the same time this results in twice as much numbers in the quefrency domain, i.e. we end with nfft/2+1
			 * numbers while H. has only nfft/4!
			 */
			fftbuf[1] = spectrum[1];
			for (long i = 2; i < nfftdiv2 + 1; i++) {
				fftbuf[i+i-2] = spectrum[i];
				fftbuf[i+i-1] = 0;
			}
			fftbuf[nfft] = spectrum[nfftdiv2 + 1];
			NUMfft_backward (&fftTable, fftbuf.peek());
			for (long i = 1; i <= nfftdiv2 + 1; i++) {
				his z[i][iframe] = fftbuf[i] * fftbuf[i];
			}
			if ((iframe % 10) == 1) {
				Melder_progress ((double) iframe / nFrames, L"Cepstrogram analysis of frame ",
					 Melder_integer (iframe), L" out of ", Melder_integer (nFrames), L".");
			}
		}
		return him.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Cepstrogram created.");
	}
}

double PowerCepstrogram_getCPPS (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling, double deltaF0, int interpolation, double qstartFit, double qendFit, int lineType, int fitMethod) {
	try {
		autoPowerCepstrogram him;
		if (subtractTiltBeforeSmoothing) {
			him.reset(PowerCepstrogram_subtractTilt (me, qstartFit, qendFit, lineType, fitMethod));
		}
		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (subtractTiltBeforeSmoothing ? him.peek() : me, timeAveragingWindow, quefrencyAveragingWindow);
		autoTable table = PowerCepstrogram_to_Table_cpp (smooth.peek(), pitchFloor, pitchCeiling, deltaF0, interpolation, qstartFit, qendFit, lineType, fitMethod);
		double cpps = Table_getMean (table.peek(), 3);
		return cpps;
	} catch (MelderError) {
		Melder_throw (me, ": no CPPS value calculated.");
	}
}

double PowerCepstrogram_getCPPS_hillenbrand (PowerCepstrogram me, bool subtractTiltBeforeSmoothing, double timeAveragingWindow, double quefrencyAveragingWindow, double pitchFloor, double pitchCeiling) {
	try {
		autoPowerCepstrogram him;
		if (subtractTiltBeforeSmoothing) {
			him.reset(PowerCepstrogram_subtractTilt (me, 0.001, 0, 1, 1));
		}
		autoPowerCepstrogram smooth = PowerCepstrogram_smooth (subtractTiltBeforeSmoothing ? him.peek() : me, timeAveragingWindow, quefrencyAveragingWindow);
		autoTable table = PowerCepstrogram_to_Table_hillenbrand (smooth.peek(), pitchFloor, pitchCeiling);
		double cpps = Table_getMean (table.peek(), 3);
		return cpps;
	} catch (MelderError) {
		Melder_throw (me, ": no CPPS value calculated.");
	}
}

/* End of file Cepstrogram.cpp */
