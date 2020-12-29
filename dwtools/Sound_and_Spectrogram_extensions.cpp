/* Sound_and_Spectrogram_extensions.cpp
 *
 * Copyright (C) 1993-2019 David Weenink
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

autoSound BandFilterSpectrogram_as_Sound (BandFilterSpectrogram me, int to_dB);

/*
	The gaussian(x) = (exp(-48*((i-(n+1)/2)/(n+1))^2)-exp(-12))/(1-exp(-12));
	For power we need the area under the square of this window:
	Integrate (gaussian(i)^2,i=1..n) =
		(sqrt(Pi)*sqrt(3)*sqrt(2)*erf(2*(n-1)*sqrt(3)*sqrt(2)/(n+1))*(n+1) + 24*exp(-24)*(n-1)+
		-4*sqrt(Pi)*sqrt(3)*exp(-12)*erf(2*(n-1)*sqrt(3)/(n+1))*(n+1))/ (24 * (-1+exp(-12))^2),
	where erf(x) = 1 - erfc(x) and n is the windowLength in samples.
	To compare with the rectangular window we need to divide this by the window width (n -1) x 1^2.
*/
static void _Spectrogram_windowCorrection (Spectrogram me, integer numberOfSamples_window) {
	double windowFactor = 1.0;
	if (numberOfSamples_window > 1) {
		const double e12 = exp (-12);
		const double denum = (e12 - 1) * (e12 - 1.0) * 24 * (numberOfSamples_window - 1);
		const double arg1 = 2.0 * NUMsqrt3 * (numberOfSamples_window - 1) / (numberOfSamples_window + 1);
		const double arg2 = arg1 * NUMsqrt2;
		const double p2 = NUMsqrtpi * NUMsqrt3 * NUMsqrt2 * (1 - NUMerfcc (arg2)) * (numberOfSamples_window + 1);
		const double p1 = 4 * NUMsqrtpi * NUMsqrt3 * e12 * (1 - NUMerfcc (arg1)) * (numberOfSamples_window + 1);
		windowFactor =  (p2 - p1 + 24 * (numberOfSamples_window - 1) * e12 * e12) / denum;
	}
	my z.get()  /=  windowFactor;
}

static autoSpectrum Sound_to_Spectrum_power (Sound me) {
	try {
		autoSpectrum thee = Sound_to_Spectrum (me, true);
		double scale = 2.0 * thy dx / (my xmax - my xmin);
		/*
			factor '2' because we combine positive and negative frequencies
			thy dx : width of frequency bin
			my xmax - my xmin : duration of sound
		*/
		VEC re = thy z.row (1), im = thy z.row (2);
		for (integer i = 1; i <= thy nx; i ++) {
			const double power = scale * (re [i] * re [i] + im [i] * im [i]);
			re [i] = power;
			im [i] = 0.0;
		}
		/*
			Correction of frequency bins at 0 Hz and nyquist: don't count for two.
		*/
		re [1] *= 0.5;
		re [thy nx] *= 0.5;
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrum with spectral power created.");
	}
}

static void Sound_into_BarkSpectrogram_frame (Sound me, BarkSpectrogram thee, integer frame) {
	autoSpectrum him = Sound_to_Spectrum_power (me);
	integer numberOfFrequencies = his nx;

	autoVEC z = raw_VEC (numberOfFrequencies);
	for (integer ifreq = 1; ifreq <= numberOfFrequencies; ifreq ++) {
		const double frequency_Hz = his x1 + (ifreq - 1) * his dx;
		z [ifreq] = thy v_hertzToFrequency (frequency_Hz);
	}

	for (integer i = 1; i <= thy ny; i ++) {
		const double z0 = thy y1 + (i - 1) * thy dy;
		constVEC pow = his z.row (1); // TODO ??
		longdouble p = 0.0;
		for (integer ifreq = 1; ifreq <= numberOfFrequencies; ifreq ++) {
			/*
				Sekey & Hanson filter is defined in the power domain.
				We therefore multiply the power with a (and not a^2).
				integral (F(z),z=0..25) = 1.58/9
			*/
			const double a = NUMsekeyhansonfilter_amplitude (z0, z [ifreq]);
			p += a * pow [ifreq] ;
		}
		thy z [i] [frame] = double (p);
	}
}

autoBarkSpectrogram Sound_to_BarkSpectrogram (Sound me, double analysisWidth, double dt, double f1_bark, double fmax_bark, double df_bark) {
	try {
		const double samplingFrequency = 1.0 / my dx, nyquist = 0.5 * samplingFrequency;
		const double windowDuration = 2.0 * analysisWidth; /* gaussian window */
		const double zmax = NUMhertzToBark2 (nyquist);
		double fmin_bark = 0.0;

		// Check defaults.

		if (f1_bark <= 0.0)
			f1_bark = 1.0;
		if (fmax_bark <= 0.0)
			fmax_bark = zmax;
		if (df_bark <= 0.0)
			df_bark = 1.0;

		fmax_bark = std::min (fmax_bark, zmax);
		const integer numberOfFilters = Melder_iround ( (fmax_bark - f1_bark) / df_bark);
		Melder_require (numberOfFilters > 0,
			U"The combination of filter parameters is not valid.");

		integer numberOfFrames;
		double t1;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoBarkSpectrogram thee = BarkSpectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_bark, fmax_bark, numberOfFilters, df_bark, f1_bark);

		autoMelderProgress progess (U"BarkSpectrogram analysis");

		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (thee.get(), iframe);

			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2.0);
			Sounds_multiply (sframe.get(), window.get());
			Sound_into_BarkSpectrogram_frame (sframe.get(), thee.get(), iframe);

			if (iframe % 10 == 1)
				Melder_progress ( (double) iframe / numberOfFrames,  U"BarkSpectrogram analysis: frame ",
					iframe, U" from ", numberOfFrames, U".");
		}
		
		_Spectrogram_windowCorrection ((Spectrogram) thee.get(), window -> nx);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no BarkSpectrogram created.");
	}
}

static void Sound_into_MelSpectrogram_frame (Sound me, MelSpectrogram thee, integer frame) {
	autoSpectrum him = Sound_to_Spectrum_power (me);

	for (integer ifilter = 1; ifilter <= thy ny; ifilter ++) {
		longdouble power = 0.0;
		const double fc_mel = thy y1 + (ifilter - 1) * thy dy;
		const double fc_hz = thy v_frequencyToHertz (fc_mel);
		const double fl_hz = thy v_frequencyToHertz (fc_mel - thy dy);
		const double fh_hz =  thy v_frequencyToHertz (fc_mel + thy dy);
		integer ifrom, ito;
		Sampled_getWindowSamples (him.get(), fl_hz, fh_hz, & ifrom, & ito);
		for (integer i = ifrom; i <= ito; i ++) {
			/*
				Bin with a triangular filter the power (= amplitude-squared)
			*/
			const double f = his x1 + (i - 1) * his dx;
			const double a = NUMtriangularfilter_amplitude (fl_hz, fc_hz, fh_hz, f);
			power += a * his z [1] [i];
		}
		thy z [ifilter] [frame] = double (power);
	}
}

autoMelSpectrogram Sound_to_MelSpectrogram (Sound me, double analysisWidth, double dt, double f1_mel, double fmax_mel, double df_mel) {
	try {
		const double samplingFrequency = 1.0 / my dx, nyquist = 0.5 * samplingFrequency;
		const double windowDuration = 2.0 * analysisWidth;   // Gaussian window
		double fmin_mel = 0.0;
		const double fbottom = NUMhertzToMel2 (100.0), fceiling = NUMhertzToMel2 (nyquist);

		// Check defaults.

		if (fmax_mel <= 0.0 || fmax_mel > fceiling)
			fmax_mel = fceiling;
		if (fmax_mel <= f1_mel) {
			f1_mel = fbottom;
			fmax_mel = fceiling;
		}
		if (f1_mel <= 0.0)
			f1_mel = fbottom;
		if (df_mel <= 0.0)
			df_mel = 100.0;

		// Determine the number of filters.

		const integer numberOfFilters = Melder_iround ((fmax_mel - f1_mel) / df_mel);
		fmax_mel = f1_mel + numberOfFilters * df_mel;

		integer numberOfFrames;
		double t1;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelSpectrogram thee = MelSpectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_mel, fmax_mel, numberOfFilters, df_mel, f1_mel);

		autoMelderProgress progress (U"MelSpectrograms analysis");

		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (thee.get(), iframe);
			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2.0);
			Sounds_multiply (sframe.get(), window.get());
			Sound_into_MelSpectrogram_frame (sframe.get(), thee.get(), iframe);
			
			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Frame ", iframe, U" out of ", numberOfFrames, U".");
		}
		
		_Spectrogram_windowCorrection ((Spectrogram) thee.get(), window -> nx);

		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no MelSpectrogram created.");
	}
}

/*
	Analog formant filter response :
	H(f) = i f B / (f1^2 - f^2 + i f B)
*/
static int Sound_into_Spectrogram_frame (Sound me, Spectrogram thee, integer frame, double bw) {
	Melder_assert (bw > 0.0);
	autoSpectrum him = Sound_to_Spectrum_power (me);

	for (integer ifilter = 1; ifilter <= thy ny; ifilter ++) {
		const double fc = thy y1 + (ifilter - 1) * thy dy;
		constVEC pow = his z.row (1);
		double p = 0.0;
		for (integer ifreq = 1; ifreq <= his nx; ifreq ++) {
			/*
				H(f) = ifB / (fc^2 - f^2 + ifB)
				H(f)| = fB / sqrt ((fc^2 - f^2)^2 + f^2B^2)
				|H(f)|^2 = f^2B^2 / ((fc^2 - f^2)^2 + f^2B^2)
						 = 1 / (((fc^2 - f^2) /fB)^2 + 1)
			*/
			const double f = his x1 + (ifreq - 1) * his dx;
			const double a = NUMformantfilter_amplitude (fc, bw, f);
			p += a * pow [ifreq];
		}
		thy z [ifilter] [frame] = p;
	}
	return 1;
}

autoSpectrogram Sound_to_Spectrogram_pitchDependent (Sound me, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw, double minimumPitch, double maximumPitch) {
	try {
		constexpr double floor = 80.0, ceiling = 600.0;
		if (minimumPitch >= maximumPitch) {
			minimumPitch = floor;
			maximumPitch = ceiling;
		}
		if (minimumPitch <= 0.0)
			minimumPitch = floor;
		if (maximumPitch <= 0.0)
			maximumPitch = ceiling;

		autoPitch thee = Sound_to_Pitch (me, dt, minimumPitch, maximumPitch);
		autoSpectrogram ff = Sound_Pitch_to_Spectrogram (me, thee.get(), analysisWidth, dt, f1_hz, fmax_hz, df_hz, relative_bw);
		return ff;
	} catch (MelderError) {
		Melder_throw (me, U": no Spectrogram created.");
	}
}

autoSpectrogram Sound_Pitch_to_Spectrogram (Sound me, Pitch thee, double analysisWidth, double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw) {
	try {
		const double windowDuration = 2.0 * analysisWidth; /* gaussian window */
		const double nyquist = 0.5 / my dx, samplingFrequency = 1.0 / my dx, fmin_hz = 0.0;

		Melder_require (my xmin >= thy xmin && my xmax <= thy xmax,
			U"The domain of the Sound should be included in the domain of the Pitch.");

		double f0_median = Pitch_getQuantile (thee, thy xmin, thy xmax, 0.5, kPitch_unit::HERTZ);

		if (isundef (f0_median) || f0_median == 0.0) {
			f0_median = 100.0;
			Melder_warning (U"Pitch values undefined. Bandwith fixed to 100 Hz. ");
		}

		if (f1_hz <= 0.0)
			f1_hz = 100.0;
		if (fmax_hz <= 0.0)
			fmax_hz = nyquist;
		if (df_hz <= 0.0)
			df_hz = f0_median / 2.0;
		if (relative_bw <= 0.0)
			relative_bw = 1.1;

		fmax_hz = std::min (fmax_hz, nyquist);
		const integer numberOfFilters = Melder_iround ( (fmax_hz - f1_hz) / df_hz);

		double t1;
		integer numberOfFrames, numberOfUndefinedPitchFrames = 0;
		Sampled_shortTermAnalysis (me, windowDuration, dt, & numberOfFrames, & t1);
		autoSpectrogram him = Spectrogram_create (my xmin, my xmax, numberOfFrames, dt, t1, fmin_hz, fmax_hz, numberOfFilters, df_hz, f1_hz);

		// Temporary objects

		autoSound sframe = Sound_createSimple (1, windowDuration, samplingFrequency);
		autoSound window = Sound_createGaussian (windowDuration, samplingFrequency);
		autoMelderProgress progress (U"Sound & Pitch: To FormantFilter");
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const double t = Sampled_indexToX (him.get(), iframe);
			double f0 = Pitch_getValueAtTime (thee, t, kPitch_unit::HERTZ, 0);

			if (isundef (f0) || f0 == 0.0) {
				numberOfUndefinedPitchFrames ++;
				f0 = f0_median;
			}
			const double b = relative_bw * f0;
			Sound_into_Sound (me, sframe.get(), t - windowDuration / 2.0);
			Sounds_multiply (sframe.get(), window.get());

			Sound_into_Spectrogram_frame (sframe.get(), him.get(), iframe, b);

			if (iframe % 10 == 1)
				Melder_progress ((double) iframe / numberOfFrames, U"Frame ", iframe, U" out of ",
					numberOfFrames, U".");
		}
		
		_Spectrogram_windowCorrection (him.get(), window -> nx);

		return him;
	} catch (MelderError) {
		Melder_throw (U"FormantFilter not created from Pitch & FormantFilter.");
	}
}

autoSound BandFilterSpectrogram_as_Sound (BandFilterSpectrogram me, int unit) {
	try {
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my ny; i ++)
			for (integer j = 1; j <= my nx; j ++)
				thy z [i] [j] = my v_getValueAtSample (j, i, unit);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": no Sound created.");
	}
}

autoSound BandFilterSpectrograms_crossCorrelate (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound sme = BandFilterSpectrogram_as_Sound (me, 1);   // to dB
		autoSound sthee = BandFilterSpectrogram_as_Sound (thee, 1);
		autoSound cc = Sounds_crossCorrelate (sme.get(), sthee.get(), scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not cross-correlated.");
	}
}

autoSound BandFilterSpectrograms_convolve (BandFilterSpectrogram me, BandFilterSpectrogram thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		autoSound sme = BandFilterSpectrogram_as_Sound (me, 1);   // to dB
		autoSound sthee = BandFilterSpectrogram_as_Sound (thee, 1);
		autoSound cc = Sounds_convolve (sme.get(), sthee.get(), scaling, signalOutsideTimeDomain);
		return cc;
	} catch (MelderError) {
		Melder_throw (me, U" and ", thee, U" not convolved.");
	}
}

/* End of file Sound_and_Spectrogram_extensions.cpp */
