/* Sound.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2003/03/09 Sounds_append: silenceDuration
 * pb 2003/07/02 checks on NUMrealft
 * pb 2003/07/10 NUMbessel_i0_f
 * pb 2004/05/05 Vector_draw
 * pb 2005/02/09 Sound_setZero
 */

#include "Sound.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "enum_c.h"
#include "Sound_enums.h"

Sound Sound_clipboard;

static void info (I) {
	iam (Sound);
	const double rho_c = 400;   /* rho = 1.14 kg m-3; c = 353 m s-1; [rho c] = kg m-2 s-1 */
	long i, numberOfSamples = my nx;
	float *amplitude = my z [1];
	double minimum = amplitude [1], maximum = minimum;
	double sum = 0.0, sumOfSquares = 0.0, mean, penergy, energy, power;
	classData -> info (me);
	MelderInfo_writeLine1 ("Time domain:");
	MelderInfo_writeLine3 ("   Start time: ", Melder_double (my xmin), " seconds");
	MelderInfo_writeLine3 ("   End time: ", Melder_double (my xmax), " seconds");
	MelderInfo_writeLine3 ("   Total duration: ", Melder_double (my xmax - my xmin), " seconds");
	MelderInfo_writeLine1 ("Time sampling:");
	MelderInfo_writeLine2 ("   Number of samples: ", Melder_integer (my nx));
	MelderInfo_writeLine3 ("   Sampling period: ", Melder_double (my dx), " seconds");
	MelderInfo_writeLine3 ("   Sampling frequency: ", Melder_single (1.0 / my dx), " Hz");
	MelderInfo_writeLine3 ("   First sample centred at: ", Melder_double (my x1), " seconds");
	for (i = 1; i <= numberOfSamples; i ++) {
		double value = amplitude [i];
		sum += value;
		sumOfSquares += value * value;
		if (value < minimum) minimum = value;
		if (value > maximum) maximum = value;
	}
	MelderInfo_writeLine3 ("Amplitude:\n   Minimum: ", Melder_single (minimum), " Pascal");
	MelderInfo_writeLine3 ("   Maximum: ", Melder_single (maximum), " Pascal");
	mean = sum / my nx;
	MelderInfo_writeLine3 ("   Mean: ", Melder_single (mean), " Pascal");
	MelderInfo_writeLine3 ("   Root-mean-square: ", Melder_single (sqrt (sumOfSquares / my nx)), " Pascal");
	penergy = sumOfSquares * my dx;   /* Pa2 s = kg2 m-2 s-3 */
	MelderInfo_write3 ("Total energy: ", Melder_single (penergy), " Pascal^2 sec");
	energy = penergy / rho_c;   /* kg s-2 = Joule m-2 */
	MelderInfo_writeLine3 (" (energy in air: ", Melder_single (energy), " Joule/m^2)");
	power = energy / (my dx * my nx);   /* kg s-3 = Watt/m2 */
	MelderInfo_write3 ("Mean power (intensity) in air: ", Melder_single (power), " Watt/m^2");
	if (power != 0.0) {
		MelderInfo_writeLine3 (" = ", Melder_half (10 * log10 (power / 1e-12)), " dB");
	} else {
		MelderInfo_writeLine1 ("");
	}
	if (my nx > 1) {
		double stdev = 0.0;
		for (i = 1; i <= numberOfSamples; i ++) {
			double value = amplitude [i] - mean;
			stdev += value * value;
		}
		stdev = sqrt (stdev / (my nx - 1));
		MelderInfo_write3 ("Standard deviation: ", Melder_single (stdev), " Pascal\n");
	}
}

class_methods (Sound, Vector)
	class_method (info)
class_methods_end

Sound Sound_create (double xmin, double xmax, long nx, double dx, double x1) {
	Sound me = new (Sound);
	if (! me || ! Matrix_init (me, xmin, xmax, nx, dx, x1, 1, 1, 1, 1, 1))
		forget (me);
	return me;
}

Sound Sound_createSimple (double duration, double samplingFrequency) {
	return Sound_create (0.0, duration, floor (duration * samplingFrequency + 0.5),
		1 / samplingFrequency, 0.5 / samplingFrequency);
}

double Sound_getEnergyInAir (Sound me) {
	long i, n = my nx;
	double sum2 = 0.0; for (i = 1; i <= n; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sum2 * my dx / 400;
}

double Sound_getIntensity_dB (Sound me) {
	long i, n = my nx;
	double sum2 = 0.0; for (i = 1; i <= n; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sum2 == 0.0 ? NUMundefined : 10 * log10 (sum2 / n / 4.0e-10);
}

double Sound_getPowerInAir (Sound me) {
	long i, n = my nx;
	double sum2 = 0.0; for (i = 1; i <= n; i ++) sum2 += my z [1] [i] * my z [1] [i];
	return sum2 / n / 400;
}

Sound Matrix_to_Sound (Matrix me, long row) {
	Sound thee = Sound_create (my xmin, my xmax, my nx, my dx, my x1);
	if (! thee) return NULL;
	if (row < 0) row = my ny + 1 + row;
	if (row < 1) row = 1;
	if (row > my ny) row = my ny;
	NUMfvector_copyElements (my z [row], thy z [1], 1, my nx);
	return thee;
}

Matrix Sound_to_Matrix (Sound me) {
	Matrix thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classMatrix);
	return thee;
}

Sound Sound_upsample (Sound me) {
	long nfft = 1, i, imin;
	float *data = NULL, factor;
	Sound thee = NULL;
	while (nfft < my nx + 2000) nfft *= 2;
	thee = Sound_create (my xmin, my xmax, my nx * 2, my dx / 2, my x1); cherror
	data = NUMfvector (1, 2 * nfft); cherror
	NUMfvector_copyElements (my z [1], data + 1000, 1, my nx);
	NUMrealft (data, nfft, 1); cherror
	imin = nfft * 0.95;
	for (i = imin + 1; i <= nfft; i ++)
		data [i] *= ((double) (nfft - i)) / (nfft - imin);
	data [2] = 0.0;
	NUMrealft (data, 2 * nfft, -1); cherror
	factor = 1.0 / nfft;
	for (i = 1; i <= thy nx; i ++)
		thy z [1] [i] = data [i + 2000] * factor;
end:
	NUMfvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

Sound Sound_resample (Sound me, double samplingFrequency, long precision) {
	float *from, *to, *data = NULL;
	double upfactor = samplingFrequency * my dx;
	long numberOfSamples = floor ((my xmax - my xmin) * samplingFrequency + 0.5), i;
	Sound thee = NULL, filtered = NULL;
	if (fabs (upfactor - 2) < 1e-6) return Sound_upsample (me);
	if (fabs (upfactor - 1) < 1e-6) return Data_copy (me);
	if (numberOfSamples < 1)
		return Melder_errorp ("Cannot resample to 0 samples.");
	thee = Sound_create (my xmin, my xmax, numberOfSamples, 1.0 / samplingFrequency,
		0.5 * (my xmin + my xmax - (numberOfSamples - 1) / samplingFrequency)); cherror
	if (upfactor < 1.0) {   /* Need anti-aliasing filter? */
		long nfft = 1, i, antiTurnAround = 1000;
		float factor;
		while (nfft < my nx + antiTurnAround * 2) nfft *= 2;
		data = NUMfvector (1, nfft); cherror
		filtered = Sound_create (my xmin, my xmax, my nx, my dx, my x1); cherror
		NUMfvector_copyElements (my z [1], data + antiTurnAround, 1, my nx);
		NUMrealft (data, nfft, 1); cherror   /* Go to the frequency domain. */
		for (i = floor (upfactor * nfft); i <= nfft; i ++)
			data [i] = 0;   /* Filter away high frequencies. */
		data [2] = 0.0;
		NUMrealft (data, nfft, -1); cherror   /* Return to the time domain. */
		factor = 1.0 / nfft;
		to = filtered -> z [1];
		for (i = 1; i <= my nx; i ++)
			to [i] = data [i + antiTurnAround] * factor;
		me = filtered;   /* Reference copy. Remove at end. */
	}
	from = my z [1];
	to = thy z [1];
	if (precision <= 1) {
		for (i = 1; i <= numberOfSamples; i ++) {
			double x = thy x1 + (i - 1) * thy dx;   /* Sampled_indexToX (thee, i); */
			double index = (x - my x1) / my dx + 1;   /* Sampled_xToIndex (me, x); */
			long leftSample = floor (index);
			double fraction = index - leftSample;
			to [i] = leftSample < 1 || leftSample >= my nx ? 0.0 :
				(1 - fraction) * from [leftSample] + fraction * from [leftSample + 1];
		}
	} else {
		for (i = 1; i <= numberOfSamples; i ++) {
			double x = thy x1 + (i - 1) * thy dx;   /* Sampled_indexToX (thee, i); */
			double index = (x - my x1) / my dx + 1;   /* Sampled_xToIndex (me, x); */
			to [i] = NUM_interpolate_sinc (my z [1], my nx, index, precision);
		}
	}
end:
	forget (filtered);
	NUMfvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

Sound Sounds_append (Sound me, double silenceDuration, Sound thee) {
	Sound him;
	long nx_silence = floor (silenceDuration / my dx + 0.5), nx = my nx + nx_silence + thy nx;
	if (my dx != thy dx)
		return Melder_errorp ("Sounds_append: sampling frequencies do not match.");
	him = Sound_create (0, nx * my dx, nx, my dx, 0.5 * my dx);
	if (! him) return NULL;
	NUMfvector_copyElements (my z [1], his z [1], 1, my nx);
	NUMfvector_copyElements (thy z [1], his z [1] + my nx + nx_silence, 1, thy nx);
	return him;
}

Sound Sounds_convolve (Sound me, Sound thee) {
	Sound him = NULL;
	long n1 = my nx, n2 = thy nx;
	long n3 = n1 + n2 - 1, nfft = 1, i;
	float *data1 = NULL, *data2 = NULL, *a, scale;
	if (my dx != thy dx)
		return Melder_errorp ("Sounds_convolve: sampling frequencies do not match.");
	while (nfft < n3) nfft *= 2;
	data1 = NUMfvector (1, nfft); cherror
	data2 = NUMfvector (1, nfft); cherror
	him = Sound_create (my xmin + thy xmin, my xmax + thy xmax, n3, my dx, my x1 + thy x1); cherror
	a = my z [1];
	for (i = n1; i > 0; i --) data1 [i] = a [i];
	a = thy z [1];
	for (i = n2; i > 0; i --) data2 [i] = a [i];
	NUMrealft (data1, nfft, 1); cherror
	NUMrealft (data2, nfft, 1); cherror
	data2 [1] *= data1 [1];
	data2 [2] *= data1 [2];
	for (i = 3; i <= nfft; i += 2) {
		float temp = data1 [i] * data2 [i] - data1 [i + 1] * data2 [i + 1];
		data2 [i + 1] = data1 [i] * data2 [i + 1] + data1 [i + 1] * data2 [i];
		data2 [i] = temp;
	}
	NUMrealft (data2, nfft, -1); cherror
	scale = 1.0 / nfft;
	a = him -> z [1];
	for (i = 1; i <= n3; i ++)
		a [i] = data2 [i] * scale;
end:
	NUMfvector_free (data1, 1);
	NUMfvector_free (data2, 1);
	iferror forget (him);
	return him;
}

void Sound_draw (Sound me, Graphics g,
	double tmin, double tmax, double minimum, double maximum, int garnish, const char *method)
{
	Vector_draw (me, g, & tmin, & tmax, & minimum, & maximum, 0.5, method);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, "Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_marksLeft (g, 2, 1, 1, 0);
		if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
			Graphics_markLeft (g, 0.0, 1, 1, 1, NULL);
		}
	}
}

static double interpolate (Sound me, long i1)
/* Precondition: my z [1] [i1] != my z [1] [i1 + 1]; */
{
	long i2 = i1 + 1;
	double x1 = Sampled_indexToX (me, i1), x2 = Sampled_indexToX (me, i2);
	double y1 = my z [1] [i1], y2 = my z [1] [i2];
	return x1 + (x2 - x1) * y1 / (y1 - y2);   /* Linear. */
}
double Sound_getNearestZeroCrossing (Sound me, double position) {
	float *amplitude = my z [1];
	long leftSample = Sampled_xToLowIndex (me, position);
	long rightSample = leftSample + 1, ileft, iright;
	double leftZero, rightZero;
	/* Are we already at a zero crossing? */
	if (leftSample >= 1 && rightSample <= my nx &&
		(amplitude [leftSample] >= 0.0) !=
		(amplitude [rightSample] >= 0.0))
	{
		return interpolate (me, leftSample);
	}
	/* Search to the left. */
	if (leftSample > my nx) return NUMundefined;
	for (ileft = leftSample - 1; ileft >= 1; ileft --)
		if ((amplitude [ileft] >= 0.0) != (amplitude [ileft + 1] >= 0.0))
		{
			leftZero = interpolate (me, ileft);
			break;
		}
	/* Search to the right. */
	if (rightSample < 1) return NUMundefined;
	for (iright = rightSample + 1; iright <= my nx; iright ++)
		if ((amplitude [iright] >= 0.0) != (amplitude [iright - 1] >= 0.0))
		{
			rightZero = interpolate (me, iright - 1);
			break;
		}
	if (ileft < 1 && iright > my nx) return NUMundefined;
	return ileft < 1 ? rightZero : iright > my nx ? leftZero :
		position - leftZero < rightZero - position ? leftZero : rightZero;
}

void Sound_setZero (Sound me, double tmin, double tmax, int roundTimesToNearestZeroCrossing) {
	long imin, imax, i;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	Function_intersectRangeWithDomain (me, & tmin, & tmax);
	if (roundTimesToNearestZeroCrossing) {
		if (tmin > my xmin) tmin = Sound_getNearestZeroCrossing (me, tmin);
		if (tmax < my xmax) tmax = Sound_getNearestZeroCrossing (me, tmax);
	}
	if (tmin == NUMundefined) tmin = my xmin;
	if (tmax == NUMundefined) tmax = my xmax;
	Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
	for (i = imin; i <= imax; i ++) {
		my z [1] [i] = 0.0;
	}
}

Sound Sound_createFromToneComplex (double startingTime, double endTime, double sampleRate,
	int phase, double frequencyStep, double firstFrequency, double ceiling, long numberOfComponents)
{
	Sound me = NULL;
	long maximumNumberOfComponents, icomp, isamp;
	float *amplitude;
	double omegaStep = 2 * NUMpi * frequencyStep, firstOmega, factor, nyquistFrequency = 0.5 * sampleRate;
	if (frequencyStep == 0.0)
		return Melder_errorp ("(Sound_createFromToneComplex:) Frequency step must not be zero.");
	/*
	 * Translate default firstFrequency.
	 */
	if (firstFrequency <= 0.0) firstFrequency = frequencyStep;
	firstOmega = 2 * NUMpi * firstFrequency;
	/*
	 * Translate default ceiling.
	 */
	if (ceiling <= 0.0 || ceiling > nyquistFrequency) ceiling = nyquistFrequency;
	/*
	 * Translate number of components.
	 */
	maximumNumberOfComponents = floor ((ceiling - firstFrequency) / frequencyStep) + 1;
	if (numberOfComponents <= 0 || numberOfComponents > maximumNumberOfComponents)
		numberOfComponents = maximumNumberOfComponents;
	if (numberOfComponents < 1)
		return Melder_errorp ("(Sound_createFromToneComplex:) Zero sine waves.");
	/*
	 * Generate the Sound.
	 */
	factor = 0.99 / numberOfComponents;
	me = Sound_create (startingTime, endTime, floor ((endTime - startingTime) * sampleRate + 0.5),
		1 / sampleRate, startingTime + 0.5 / sampleRate);
	if (! me) return NULL;
	amplitude = my z [1];
	for (isamp = 1; isamp <= my nx; isamp ++) {
		double value = 0.0, t = Sampled_indexToX (me, isamp);
		double omegaStepT = omegaStep * t, firstOmegaT = firstOmega * t;
		if (phase == Sound_TONE_COMPLEX_SINE)
			for (icomp = 1; icomp <= numberOfComponents; icomp ++)
				value += sin (firstOmegaT + (icomp - 1) * omegaStepT);
		else
			for (icomp = 1; icomp <= numberOfComponents; icomp ++)
				value += cos (firstOmegaT + (icomp - 1) * omegaStepT);
		amplitude [isamp] = value * factor;
	}
	return me;
}

void Sound_multiplyByWindow (Sound me, int windowType) {
	long i, n = my nx;
	float *amp = my z [1];
	double imid, edge, onebyedge1, factor;
	switch (windowType) {
		case enumi (Sound_WINDOW, Rectangular):
			;
		break;
		case enumi (Sound_WINDOW, Triangular):   /* "Bartlett" */
			for (i = 1; i <= n; i ++) { double phase = (double) i / n;   /* 0..1 */
				amp [i] *= 1.0 - fabs ((2.0 * phase - 1.0)); }
		break;
		case enumi (Sound_WINDOW, Parabolic):   /* "Welch" */
			for (i = 1; i <= n; i ++) { double phase = (double) i / n;
				amp [i] *= 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0); }
		break;
		case enumi (Sound_WINDOW, Hanning):
			for (i = 1; i <= n; i ++) { double phase = (double) i / n;
				amp [i] *= 0.5 * (1.0 - cos (2.0 * NUMpi * phase)); }
		break;
		case enumi (Sound_WINDOW, Hamming):
			for (i = 1; i <= n; i ++) { double phase = (double) i / n;
				amp [i] *= 0.54 - 0.46 * cos (2.0 * NUMpi * phase); }
		break;
		case enumi (Sound_WINDOW, Gaussian1):
			imid = 0.5 * (n + 1), edge = exp (-3.0), onebyedge1 = 1 / (1.0 - edge);   /* -0.5..+0.5 */
			for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
				amp [i] *= (exp (-12.0 * phase * phase) - edge) * onebyedge1; }
		break;
		case enumi (Sound_WINDOW, Gaussian2):
			imid = 0.5 * (double) (n + 1), edge = exp (-12.0), onebyedge1 = 1 / (1.0 - edge);
			for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
				amp [i] *= (exp (-48.0 * phase * phase) - edge) * onebyedge1; }
		break;
		case enumi (Sound_WINDOW, Gaussian3):
			imid = 0.5 * (double) (n + 1), edge = exp (-27.0), onebyedge1 = 1 / (1.0 - edge);
			for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
				amp [i] *= (exp (-108.0 * phase * phase) - edge) * onebyedge1; }
		break;
		case enumi (Sound_WINDOW, Gaussian4):
			imid = 0.5 * (double) (n + 1), edge = exp (-48.0), onebyedge1 = 1 / (1.0 - edge);
			for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
				amp [i] *= (exp (-192.0 * phase * phase) - edge) * onebyedge1; }
		break;
		case enumi (Sound_WINDOW, Gaussian5):
			imid = 0.5 * (double) (n + 1), edge = exp (-75.0), onebyedge1 = 1 / (1.0 - edge);
			for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
				amp [i] *= (exp (-300.0 * phase * phase) - edge) * onebyedge1; }
		break;
		case enumi (Sound_WINDOW, Kaiser1):
			imid = 0.5 * (double) (n + 1);
			factor = 1 / NUMbessel_i0_f (2 * NUMpi);
			for (i = 1; i <= n; i ++) { double phase = 2 * ((double) i - imid) / n;   /* -1..+1 */
				double root = 1 - phase * phase;
				amp [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2 * NUMpi * sqrt (root)); }
		break;
		case enumi (Sound_WINDOW, Kaiser2):
			imid = 0.5 * (double) (n + 1);
			factor = 1 / NUMbessel_i0_f (2 * NUMpi * NUMpi + 0.5);
			for (i = 1; i <= n; i ++) { double phase = 2 * ((double) i - imid) / n;   /* -1..+1 */
				double root = 1 - phase * phase;
				amp [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((2 * NUMpi * NUMpi + 0.5) * sqrt (root)); }
		break;
		default:
		break;
	}
}

void Sound_scaleIntensity (Sound me, double newAverageIntensity) {
	double currentIntensity = Sound_getIntensity_dB (me), factor;
	long i;
	if (currentIntensity == NUMundefined) return;
	factor = pow (10, (newAverageIntensity - currentIntensity) / 20.0);
	for (i = 1; i <= my nx; i ++) {
		my z [1] [i] *= factor;
	}
}

void Sound_overrideSamplingFrequency (Sound me, double rate) {
	my dx = 1 / rate;
	my x1 = my xmin + 0.5 * my dx;
	my xmax = my xmin + my nx * my dx;
}

Sound Sound_extractPart (Sound me, double t1, double t2, int windowType, double relativeWidth, int preserveTimes) {
	Sound thee = NULL;
	long ix1, ix2;
	/*
	 * We do not clip to the Sound's time domain.
	 * Any samples outside it are taken to be zero.
	 */

	/*
	 * Autowindow.
	 */
	if (t1 == t2) { t1 = my xmin; t2 = my xmax; };
	/*
	 * Allow window tails outside specified domain.
	 */
	if (relativeWidth != 1.0) {
		double margin = 0.5 * (relativeWidth - 1) * (t2 - t1);
		t1 -= margin;
		t2 += margin;
	}
	/*
	 * Determine index range. We use all the real or virtual samples that fit within [t1..t2].
	 */
	ix1 = 1 + (long) ceil ((t1 - my x1) / my dx);
	ix2 = 1 + (long) floor ((t2 - my x1) / my dx);
	if (ix2 < ix1) { Melder_error ("Extracted Sound would contain no samples."); goto end; }
	/*
	 * Create sound, optionally shifted to [0..t2-t1].
	 */
	thee = Sound_create (t1, t2, ix2 - ix1 + 1, my dx, my x1 + (ix1 - 1) * my dx); cherror
	if (! preserveTimes) { thy xmin = 0.0; thy xmax -= t1; thy x1 -= t1; }
	/*
	 * Copy only *real* samples into the new sound.
	 * The *virtual* samples will remain at zero.
	 */
	NUMfvector_copyElements (my z [1], thy z [1] + 1 - ix1,
			( ix1 < 1 ? 1 : ix1 ), ( ix2 > my nx ? my nx : ix2 ));
	/*
	 * Multiply by a window that extends throughout the target domain.
	 */
	Sound_multiplyByWindow (thee, windowType);
end:
	iferror { forget (thee); return Melder_errorp ("(Sound_extractPart:) Not performed."); }
	return thee;
}

int Sound_filterWithFormants (Sound me, double tmin, double tmax,
	int numberOfFormants, float formant [], float bandwidth [])
{
	float *amplitude;
	int iformant;
	long itmin, itmax, n;

	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
	if ((n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) <= 2)
		return Melder_error ("Sound too short.");
	amplitude = my z [1] + itmin - 1;   /* Base 1. */
	NUMdeemphasize_f (amplitude, n, my dx, 50.0);
	for (iformant = 1; iformant <= numberOfFormants; iformant ++)
		NUMfilterSecondOrderSection_fb (amplitude, n, my dx, formant [iformant], bandwidth [iformant]);
	NUMautoscale (amplitude, n, 0.9);
	return 1;
}

Sound Sound_filter_oneFormant (Sound me, double frequency, double bandwidth) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_filterWithOneFormantInline (thee, frequency, bandwidth);
	return thee;
}

int Sound_filterWithOneFormantInline (Sound me, double frequency, double bandwidth) {
	NUMfilterSecondOrderSection_fb (my z [1], my nx, my dx, frequency, bandwidth);
	NUMautoscale (my z [1], my nx, 0.9);
	return 1;
}

Sound Sound_filter_preemphasis (Sound me, double frequency) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_preEmphasis (thee, frequency);
	Vector_scale (thee, 0.99);
	return thee;
}

Sound Sound_filter_deemphasis (Sound me, double frequency) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_deEmphasis (thee, frequency);
	Vector_scale (thee, 0.99);
	return thee;
}

void Sound_reverse (Sound me, double tmin, double tmax) {
	long itmin, itmax, n, i;
	float *amp = my z [1];
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
	n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax) / 2;
	for (i = 0; i < n; i ++) {
		float dummy = amp [itmin + i];
		amp [itmin + i] = amp [itmax - i];
		amp [itmax - i] = dummy;
	}
}

Sound Sounds_crossCorrelate (Sound me, Sound thee, double tmin, double tmax, int normalize) {
	Sound him = NULL;
	double dt, dphase, t1;
	long i1, i2, nt, i;
	if (my dx != thy dx)
		return Melder_errorp ("(Sounds_crossCorrelation:) Sampling frequencies do not match.");
	dt = my dx;
	dphase = (thy x1 - my x1) / dt;
	dphase -= floor (dphase);   /* A number between 0 and 1. */
	i1 = ceil (tmin / dt - dphase);   /* Index of first sample if sample at dphase has index 0. */
	i2 = floor (tmax / dt - dphase);   /* Index of last sample if sample at dphase has index 0. */
	nt = i2 - i1 + 1;
	if (nt < 1)
		return Melder_errorp ("(Sounds_crossCorrelation:) Window too small.");
	t1 = (dphase + i1) * dt;
	him = Sound_create (tmin, tmax, nt, dt, t1);
	if (him == NULL) return NULL;
	for (i = 1; i <= nt; i ++) {
		long di = i - 1 + i1, ime;
		for (ime = 1; ime <= my nx; ime ++) {
			if (ime + di < 1) continue;
			if (ime + di > thy nx) break;
			his z [1] [i] += my z [1] [ime] * thy z [1] [ime + di];
		}
	}
	if (normalize) {
		double mypower = 0.0, thypower = 0.0;
		for (i = 1; i <= my nx; i ++) {
			double value = my z [1] [i];
			mypower += value * value;
		}
		for (i = 1; i <= thy nx; i ++) {
			double value = thy z [1] [i];
			thypower += value * value;
		}
		if (mypower != 0.0 && thypower != 0.0) {
			double factor = 1.0 / (sqrt (mypower) * sqrt (thypower));
			for (i = 1; i <= nt; i ++)
				his z [1] [i] *= factor;
		}
	} else {
		for (i = 1; i <= nt; i ++)
			his z [1] [i] *= dt;
	}
	return him;
}

/* End of file Sound.c */
