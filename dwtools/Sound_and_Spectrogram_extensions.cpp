/* Sound_and_FilterBank.cpp
 *
 * Copyright (C) 1993-2014 David Weenink
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
 djmw 20010718
 djmw 20020813 GPL header.
 djmw 20041124 Changed call to Sound_to_Spectrum.
 djmw 20070103 Sound interface changes
 djmw 20071107 Errors/warnings text changes
 djmw 20071202 Melder_warning<n>
*/

#include "Sound_and_Spectrogram_extensions.h"
#include "Sound_extensions.h"
#include "Sound_and_Spectrum.h"
#include "Sound_to_Pitch.h"
#include "Vector.h"
#include "NUM2.h"

#define MIN(m,n) ((m) < (n) ? (m) : (n))
// prototypes
Sound BandFilterSpectrogram_as_Sound (BandFilterSpectrogram me, int to_dB);

/*
	The gaussian(x) = (exp(-48*((i-(n+1)/2)/(n+1))^2)-exp(-12))/(1-exp(-12));
	For power we need the area under the square of this window:
	Integrate (gaussian(i)^2,i=1..n) =
		(sqrt(Pi)*sqrt(3)*sqrt(2)*erf(2*(n-1)*sqrt(3)*sqrt(2)/(n+1))*(n+1) + 24*exp(-24)*(n-1)+
		-4*sqrt(Pi)*sqrt(3)*exp(-12)*erf(2*(n-1)*sqrt(3)/(n+1))*(n+1))/ (24 * (-1+exp(-12))^2),
	where erf(x) = 1 - erfc(x) and n is the windowLength in samples.
	To compare with the rectangular window we need to divide this by the window width (n -1) x 1^2.
*/
static void _Spectrogram_windowCorrection (Spectrogram me, long numberOfSamples_window) {
	double windowFactor = 1;
	if (numberOfSamples_window > 1) {
		double e12 = exp (-12);
		double denum = (e12 - 1) * (e12 - 1) * 24 * (numberOfSamples_window - 1);
		double arg1 = 2 * NUMsqrt3 * (numberOfSamples_window - 1) / (numberOfSamples_window + 1);
		double arg2 = arg1 * NUMsqrt2;
		double p2 = NUMsqrtpi * NUMsqrt3 * NUMsqrt2 * (1 - NUMerfcc (arg2)) * (numberOfSamples_window + 1);
		double p1 = 4 * NUMsqrtpi * NUMsqrt3 * e12 * (1 - NUMerfcc (arg1)) * (numberOfSamples_window + 1);
		windowFactor =  (p2 - p1 + 24 * (numberOfSamples_window - 1) * e12 * e12) / denum;
	}
	for (long i = 1; i <= my ny; i++) {
		for (long j = 1; j <= my nx; j++) {
			my z[i][j] /= windowFactor;
		}
	}
}

static Spectrum Sound_to_Spectrum_power (Sound me) {
	try {
		autoSpectrum thee = Sound_to_Spectrum (me, TRUE);
		double scale = 2.0 * thy dx / (my xmax - my xmin);

		// factor '2' because we combine positive and negative frequencies
		// thy dx : width of frequency bin
		// my xmax - my xmin : duration of sound

		double *re = thy z[1], *im = thy z[2];
		for (long i = 1; i <= thy nx; i++) {
			double power = scale * (re[i] * re[i] + im[i] * im [i]);
			re[i] = power; im[i] = 0;
		}

		// Correction of frequency bins at 0 Hz and nyquist: don't count for two.

		re[1] *= 0.5; re[thy nx] *= 0.5;
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Spectrum with spectral power created.");
	}
}

static void Sound_into_BarkSpectrogram_frame (Sound me, BarkSpectrogram thee, long frame) {
	autoSpectrum him = Sound_to_Spectrum_power (me);
	long numberOfFrequencies = his nx;
	autoNUMvector<double> z (1, numberOfFrequencies);

	for (long ifreq = 1; ifreq <= numberOfFrequencies; ifreq++) {
		double fhz = his x1 + (ifreq - 1) * his dx;
		z[ifreq] = thy v_hertzToFrequency (fhz);
	}

	for (long i = 1; i <= thy ny; i++) {
		double p = 0;
		double z0 = thy y1 + (i - 1) * thy dy;
		double *pow = his z[1]; // TODO ??
		for (long ifreq = 1; ifreq <= numberOfFrequencies; ifreq++) {
			// Sekey & Hanson filter is defined in the power domain.
			// We therefore multiply the power with a (and not a^2).
			// integral (F(z),z=0..25) = 1.58/9

			double a = NUMsekeyhansonfilter_amplitude (z0, z[ifreq]);
			p += a * pow[ifreq] ;
		}
		thy z[i][frame] = p;
	}
}

BarkSpectrogram Sound_to_BarkSpectrogram (Sound me, double analysisWidth, double dt, double f1_bark, double fmax_bark, double df_bark) {
	try {
		double nyquist = 0.5 / my dx, samplingFrequency = 2 * nyquist;
		double windowDuration = 2 * analysisWidth; /* gaussian window */
		double zmax = NUMhertzToBark2 (nyquist);
		double fmin_bark = 0;

		// Check defaults.

		if (f1_bark <= 0) {
			f1_bark = 1;
		}
		if (fmax_bark <= 0) {
			fmax_bark = zmax;
		}
		if (df_bark <= 0) {
			df_bark = 1;
		}

		fmax_bark = MIN (fmax_bark, zmax);
		long numberOfFilters = floor ( (fmax_bark - f1_bark) / df_bark + 0.5);
		if (numberOfFilters <= 0) {
			Melder_throw ("The combination of filter parameters is not valid.");
		}

		long numberOfFrames; double t1;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoBarkSpectrogram thee = BarkSpectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_bark, fmax_bark, numberOfFilters, df_bark, f1_bark);

		autoMelderProgress progess (L"BarkSpectrogram analysis");

		for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
			double t = Sampled_indexToX (thee.peek(), iframe);

			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);
			Sounds_multiply (sframe.peek(), window.peek());
			Sound_into_BarkSpectrogram_frame (sframe.peek(), thee.peek(), iframe);

			if ((iframe % 10) == 1) {
				Melder_progress ( (double) iframe / numberOfFrames,  L"BarkSpectrogram analysis: frame ",
					Melder_integer (iframe), L" from ", Melder_integer (numberOfFrames), L".");
			}
		}
		
		_Spectrogram_windowCorrection ((Spectrogram) thee.peek(), window -> nx);

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no BarkSpectrogram created.");
	}
}

static void Sound_into_MelSpectrogram_frame (Sound me, MelSpectrogram thee, long frame) {
	autoSpectrum him = Sound_to_Spectrum_power (me);

	for (long ifilter = 1; ifilter <= thy ny; ifilter++) {
		double power = 0;
		double fc_mel = thy y1 + (ifilter - 1) * thy dy;
		double fc_hz = thy v_frequencyToHertz (fc_mel);
		double fl_hz = thy v_frequencyToHertz (fc_mel - thy dy);
		double fh_hz =  thy v_frequencyToHertz (fc_mel + thy dy);
		long ifrom, ito;
		Sampled_getWindowSamples (him.peek(), fl_hz, fh_hz, &ifrom, &ito);
		for (long i = ifrom; i <= ito; i++) {
			// Bin with a triangular filter the power (=amplitude-squared)

			double f = his x1 + (i - 1) * his dx;
			double a = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, f);
			power += a * his z[1][i];
		}
		thy z[ifilter][frame] = power;
	}
}

MelSpectrogram Sound_to_MelSpectrogram (Sound me, double analysisWidth, double dt, double f1_mel, double fmax_mel, double df_mel) {
	try {
		double t1, samplingFrequency = 1 / my dx, nyquist = 0.5 * samplingFrequency;
		double windowDuration = 2 * analysisWidth; /* gaussian window */
		double fmin_mel = 0;
		double fbottom = NUMhertzToMel2 (100.0), fceiling = NUMhertzToMel2 (nyquist);
		long numberOfFrames;

		// Check defaults.

		if (fmax_mel <= 0 || fmax_mel > fceiling) {
			fmax_mel = fceiling;
		}
		if (fmax_mel <= f1_mel) {
			f1_mel = fbottom; fmax_mel = fceiling;
		}
		if (f1_mel <= 0) {
			f1_mel = fbottom;
		}
		if (df_mel <= 0) {
			df_mel = 100.0;
		}

		// Determine the number of filters.

		long numberOfFilters = floor ((fmax_mel - f1_mel) / df_mel + 0.5);
		fmax_mel = f1_mel + numberOfFilters * df_mel;

		Sampled_shortTermAnalysis (me, windowDuration, dt, &numberOfFrames, &t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_mel, fmax_mel, numberOfFilters, df_mel, f1_mel);

		autoMelderProgress progress (L"MelSpectrograms analysis");

		for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
			double t = Sampled_indexToX (thee.peek(), iframe);
			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);
			Sounds_multiply (sframe.peek(), window.peek());
			Sound_into_MelSpectrogram_frame (sframe.peek(), thee.peek(), iframe);
			
			if ((iframe % 10) == 1) {
				Melder_progress ((double) iframe / numberOfFrames, L"Frame ", Melder_integer (iframe), L" out of ", Melder_integer (numberOfFrames), L".");
			}
		}
		
		_Spectrogram_windowCorrection ((Spectrogram) thee.peek(), window -> nx);

		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no MelSpectrogram created.");
	}
}

/*
	Analog formant filter response :
	H(f) = i f B / (f1^2 - f^2 + i f B)
*/
static int Sound_into_Spectrogram_frame (Sound me, Spectrogram thee, long frame, double bw) {
	Melder_assert (bw > 0);
	autoSpectrum him = Sound_to_Spectrum_power (me);

	for (long ifilter = 1; ifilter <= thy ny; ifilter++) {
		double p = 0;
		double fc = thy y1 + (ifilter - 1) * thy dy;
		double *pow = his z[1];
		for (long ifreq = 1; ifreq <= his nx; ifreq++) {
			// H(f) = ifB / (fc^2 - f^2 + ifB)
			// H(f)| = fB / sqrt ((fc^2 - f^2)^2 + f^2B^2)
			//|H(f)|^2 = f^2B^2 / ((fc^2 - f^2)^2 + f^2B^2)
			//         = 1 / (((fc^2 - f^2) /fB)^2 + 1)

			double f = his x1 + (ifreq - 1) * his dx;
			double a = NUMformantfilter_amplitude (fc, bw, f);
			p += a * pow[ifreq];
		}
		thy z[ifilter][frame] = p;
	}
	return 1;
}

Spectrogram Sound_to_Spectrogram_pitchDependent (Sound me, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw, double minimumPitch, double maximumPitch) {
	try {
		double floor = 80, ceiling = 600;
		if (minimumPitch >= maximumPitch) {
			minimumPitch = floor; maximumPitch = ceiling;
		}
		if (minimumPitch <= 0) {
			minimumPitch = floor;
		}
		if (maximumPitch <= 0) {
			maximumPitch = ceiling;
		}

		autoPitch thee = Sound_to_Pitch (me, dt, minimumPitch, maximumPitch);
		autoSpectrogram ff = Sound_and_Pitch_to_Spectrogram (me, thee.peek(), analysisWidth, dt, f1_hz, fmax_hz, df_hz, relative_bw);
		return ff.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Spectrogram created.");
	}
}

Spectrogram Sound_and_Pitch_to_Spectrogram (Sound me, Pitch thee, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw) {
	try {
		double t1, windowDuration = 2 * analysisWidth; /* gaussian window */
		double nyquist = 0.5 / my dx, samplingFrequency = 2 * nyquist, fmin_hz = 0;
		long numberOfFrames, f0_undefined = 0;

		if (my xmin > thy xmin || my xmax > thy xmax) Melder_throw
			("The domain of the Sound is not included in the domain of the Pitch.");

		double f0_median = Pitch_getQuantile (thee, thy xmin, thy xmax, 0.5, kPitch_unit_HERTZ);

		if (f0_median == NUMundefined || f0_median == 0) {
			f0_median = 100;
			Melder_warning (L"Pitch values undefined. Bandwith fixed to 100 Hz. ");
		}

		if (f1_hz <= 0) {
			f1_hz = 100;
		}
		if (fmax_hz <= 0) {
			fmax_hz = nyquist;
		}
		if (df_hz <= 0) {
			df_hz = f0_median / 2;
		}
		if (relative_bw <= 0) {
			relative_bw = 1.1;
		}

		fmax_hz = MIN (fmax_hz, nyquist);
		long numberOfFilters = floor ( (fmax_hz - f1_hz) / df_hz + 0.5);

		Sampled_shortTermAnalysis (me, windowDuration, dt, &numberOfFrames, &t1);
		autoSpectrogram him = Spectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_hz, fmax_hz, numberOfFilters, df_hz, f1_hz);

		// Temporary objects

		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelderProgress progress (L"Sound & Pitch: To FormantFilter");
		for (long iframe = 1; iframe <= numberOfFrames; iframe++) {
			double t = Sampled_indexToX (him.peek(), iframe);
			double b, f0 = Pitch_getValueAtTime (thee, t, kPitch_unit_HERTZ, 0);

			if (f0 == NUMundefined || f0 == 0) {
				f0_undefined++; f0 = f0_median;
			}
			b = relative_bw * f0;
			Sound_into_Sound (me, sframe.peek(), t - windowDuration / 2);
			Sounds_multiply (sframe.peek(), window.peek());

			Sound_into_Spectrogram_frame (sframe.peek(), him.peek(), iframe, b);

			if ((iframe % 10) == 1) {
				Melder_progress ( (double) iframe / numberOfFrames, L"Frame ", Melder_integer (iframe), L" out of ", 
					Melder_integer (numberOfFrames), L".");
			}
		}
		
		_Spectrogram_windowCorrection (him.peek(), window -> nx);

		return him.transfer();
	} catch (MelderError) {
		Melder_throw ("FormantFilter not created from Pitch & FormantFilter.");
	}
}

Sound BandFilterSpectrogram_as_Sound (BandFilterSpectrogram me, int unit) {
	try {
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		for (long i = 1; i <= my ny; i++) {
			for (long j = 1; j <= my nx; j++)
				thy z[i][j] = my v_getValueAtSample (j, i, unit);
		}
		return thee.transfer();
	} catch (MelderError) {
		Melder_throw (me, ": no Sound created.");
	}
}

Sound BandFilterSpectrograms_crossCorrelate (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound sme = BandFilterSpectrogram_as_Sound (me, 1) ; // to dB
		autoSound sthee = BandFilterSpectrogram_as_Sound (thee, 1) ;
		autoSound cc = Sounds_crossCorrelate (sme.peek(), sthee.peek(), scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not cross-correlated.");
	}
}

Sound BandFilterSpectrograms_convolve (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound sme = BandFilterSpectrogram_as_Sound (me, 1) ; // to dB
		autoSound sthee = BandFilterSpectrogram_as_Sound (thee, 1) ;
		autoSound cc = Sounds_convolve (sme.peek(), sthee.peek(), scaling, signalOutsideTimeDomain);
		return cc.transfer();
	} catch (MelderError) {
		Melder_throw (me, " and ", thee, " not convolved.");
	}
}

/* End of file Sound_and_Spectrogram_extensions.cpp */
