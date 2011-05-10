/* Sound.c
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2006/12/31 stereo
 * pb 2007/01/26 more stereo
 * pb 2007/01/28 corrected a bug in Sound_extractChannel
 * pb 2007/01/28 more stereo
 * pb 2007/03/17 domain quantity
 * pb 2007/12/07 enums
 * pb 2010/03/26 Sounds_convolve, Sounds_crossCorrelate, Sound_autocorrelate
 */

#include "Sound.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "enums_getText.h"
#include "Sound_enums.h"
#include "enums_getValue.h"
#include "Sound_enums.h"

Sound Sound_clipboard;

static void info (I) {
	iam (Sound);
	const double rho_c = 400;   /* rho = 1.14 kg m-3; c = 353 m s-1; [rho c] = kg m-2 s-1 */
	long numberOfSamples = my nx;
	double minimum = my z [1] [1], maximum = minimum;
	classData -> info (me);
	MelderInfo_writeLine3 (L"Number of channels: ", Melder_integer (my ny),
		my ny == 1 ? L" (mono)" : my ny == 2 ? L" (stereo)" : L"");
	MelderInfo_writeLine1 (L"Time domain:");
	MelderInfo_writeLine3 (L"   Start time: ", Melder_double (my xmin), L" seconds");
	MelderInfo_writeLine3 (L"   End time: ", Melder_double (my xmax), L" seconds");
	MelderInfo_writeLine3 (L"   Total duration: ", Melder_double (my xmax - my xmin), L" seconds");
	MelderInfo_writeLine1 (L"Time sampling:");
	MelderInfo_writeLine2 (L"   Number of samples: ", Melder_integer (my nx));
	MelderInfo_writeLine3 (L"   Sampling period: ", Melder_double (my dx), L" seconds");
	MelderInfo_writeLine3 (L"   Sampling frequency: ", Melder_single (1.0 / my dx), L" Hz");
	MelderInfo_writeLine3 (L"   First sample centred at: ", Melder_double (my x1), L" seconds");
	double sum = 0.0, sumOfSquares = 0.0;
	for (long channel = 1; channel <= my ny; channel ++) {
		double *amplitude = my z [channel];
		for (long i = 1; i <= numberOfSamples; i ++) {
			double value = amplitude [i];
			sum += value;
			sumOfSquares += value * value;
			if (value < minimum) minimum = value;
			if (value > maximum) maximum = value;
		}
	}
	MelderInfo_writeLine1 (L"Amplitude:");
	MelderInfo_writeLine3 (L"   Minimum: ", Melder_single (minimum), L" Pascal");
	MelderInfo_writeLine3 (L"   Maximum: ", Melder_single (maximum), L" Pascal");
	double mean = sum / (my nx * my ny);
	MelderInfo_writeLine3 (L"   Mean: ", Melder_single (mean), L" Pascal");
	MelderInfo_writeLine3 (L"   Root-mean-square: ", Melder_single (sqrt (sumOfSquares / (my nx * my ny))), L" Pascal");
	double penergy = sumOfSquares * my dx / my ny;   /* Pa2 s = kg2 m-2 s-3 */
	MelderInfo_write3 (L"Total energy: ", Melder_single (penergy), L" Pascal\u00B2 sec");
	double energy = penergy / rho_c;   /* kg s-2 = Joule m-2 */
	MelderInfo_writeLine3 (L" (energy in air: ", Melder_single (energy), L" Joule/m\u00B2)");
	double power = energy / (my dx * my nx);   /* kg s-3 = Watt/m2 */
	MelderInfo_write3 (L"Mean power (intensity) in air: ", Melder_single (power), L" Watt/m\u00B2");
	if (power != 0.0) {
		MelderInfo_writeLine3 (L" = ", Melder_half (10 * log10 (power / 1e-12)), L" dB");
	} else {
		MelderInfo_writeLine1 (L"");
	}
	if (my nx > 1) {
		for (long channel = 1; channel <= my ny; channel ++) {
			double *amplitude = my z [channel];
			double sum = 0.0;
			for (long i = 1; i <= numberOfSamples; i ++) {
				double value = amplitude [i];
				sum += value;
			}
			double mean = sum / my nx, stdev = 0.0;
			for (long i = 1; i <= numberOfSamples; i ++) {
				double value = amplitude [i] - mean;
				stdev += value * value;
			}
			stdev = sqrt (stdev / (my nx - 1));
			MelderInfo_writeLine5 (L"Standard deviation in channel ", Melder_integer (channel), L": ", Melder_single (stdev), L" Pascal");
		}
	}
}

static double getMatrix (I, long irow, long icol) {
	iam (Sound);
	if (irow < 1 || irow > my ny) {
		if (irow == 0) {
			if (icol < 1 || icol > my nx) return 0.0;
			if (my ny == 1) return my z [1] [icol];   // Optimization.
			if (my ny == 2) return 0.5 * (my z [1] [icol] + my z [2] [icol]);   // Optimization.
			double sum = 0.0;
			for (long channel = 1; channel <= my ny; channel ++) {
				sum += my z [channel] [icol];
			}
			return sum / my ny;
		}
		return 0.0;
	}
	if (icol < 1 || icol > my nx) return 0.0;
	return my z [irow] [icol];
}

static double getFunction2 (I, double x, double y) {
	iam (Sound);
	long channel = (long) floor (y);
	if (channel < 0 || channel > my ny || y != (double) channel) return 0.0;
	return our getFunction1 (me, channel, x);
}

class_methods (Sound, Vector)
	class_method (info)
	class_method (getMatrix)
	class_method (getFunction2)
	us -> domainQuantity = MelderQuantity_TIME_SECONDS;
class_methods_end

Sound Sound_create (long numberOfChannels, double xmin, double xmax, long nx, double dx, double x1) {
	Sound me = Thing_new (Sound);
	if (! me || ! Matrix_init (me, xmin, xmax, nx, dx, x1, 1, numberOfChannels, numberOfChannels, 1, 1))
		forget (me);
	return me;
}

Sound Sound_createSimple (long numberOfChannels, double duration, double samplingFrequency) {
	return Sound_create (numberOfChannels, 0.0, duration, floor (duration * samplingFrequency + 0.5),
		1 / samplingFrequency, 0.5 / samplingFrequency);
}

Sound Sound_convertToMono (Sound me) {
	if (my ny == 1) return Data_copy (me);   // Optimization.
	Sound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1); cherror
	if (my ny == 2) {   // Optimization.
		for (long i = 1; i <= my nx; i ++) {
			thy z [1] [i] = 0.5 * (my z [1] [i] + my z [2] [i]);
		}
	} else {
		for (long i = 1; i <= my nx; i ++) {
			double sum = my z [1] [i] + my z [2] [i] + my z [3] [i];
			for (long channel = 4; channel <= my ny; channel ++) {
				sum += my z [channel] [i];
			}
			thy z [1] [i] = sum / my ny;
		}
	}
end:
	iferror forget (thee);
	return thee;
}

Sound Sound_convertToStereo (Sound me) {
	if (my ny == 2) return Data_copy (me);
	if (my ny > 2) {
		return Melder_errorp ("Don't know how to convert a Sound with %ld channels to stereo.", my ny);
	}
	Sound thee = NULL;
	Melder_assert (my ny == 1);
	thee = Sound_create (2, my xmin, my xmax, my nx, my dx, my x1); cherror
	for (long i = 1; i <= my nx; i ++) {
		thy z [1] [i] = thy z [2] [i] = my z [1] [i];
	}
end:
	iferror forget (thee);
	return thee;
}

Sound Sounds_combineToStereo (Sound me, Sound thee) {
	if (my ny != 1 || thy ny != 1) return Melder_errorp ("Can only combine mono sounds. Stereo sound not created.");
	if (my dx != thy dx) return Melder_errorp ("Sampling frequencies do not match. Sounds not combined.");
	double dx = my dx;   // or thy dx, which is the same
	double xmin = my xmin < thy xmin ? my xmin : thy xmin;
	double xmax = my xmax > thy xmax ? my xmax : thy xmax;
	long myInitialZeroes = floor ((my xmin - xmin) / dx);
	long thyInitialZeroes = floor ((thy xmin - xmin) / dx);
	double myx1 = my x1 - my dx * myInitialZeroes;
	double thyx1 = thy x1 - thy dx * thyInitialZeroes;
	double x1 = 0.5 * (myx1 + thyx1);
	long mynx = my nx + myInitialZeroes;
	long thynx = thy nx + thyInitialZeroes;
	long nx = mynx > thynx ? mynx : thynx;
	Sound him = Sound_create (2, xmin, xmax, nx, dx, x1); cherror
	for (long i = 1; i <= my nx; i ++) {
		his z [1] [i + myInitialZeroes] = my z [1] [i];
	}
	for (long i = 1; i <= thy nx; i ++) {
		his z [2] [i + thyInitialZeroes] = thy z [1] [i];
	}
end:
	iferror forget (him);
	return him;
}

Sound Sound_extractChannel (Sound me, long ichan) {
	Sound thee = NULL;
//start:
	if (ichan <= 0 || ichan > my ny) error3 (L"Cannot extract channel ", Melder_integer (ichan), L".");
	thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1); cherror
	for (long isamp = 1; isamp <= my nx; isamp ++) {
		thy z [1] [isamp] = my z [ichan] [isamp];
	}
end:
	iferror forget (thee);
	return thee;
}

static double getSumOfSquares (Sound me, double xmin, double xmax, long *n) {
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	long imin, imax;
	*n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (*n < 1) return NUMundefined;
	double sum2 = 0.0;
	for (long channel = 1; channel <= my ny; channel ++) {
		double *amplitude = my z [channel];
		for (long i = imin; i <= imax; i ++) {
			double value = amplitude [i];
			sum2 += value * value;
		}
	}
	return sum2;
}

double Sound_getRootMeanSquare (Sound me, double xmin, double xmax) {
	long n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return NUMdefined (sum2) ? sqrt (sum2 / (n * my ny)) : NUMundefined;
}

double Sound_getEnergy (Sound me, double xmin, double xmax) {
	long n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return NUMdefined (sum2) ? sum2 * my dx / my ny : NUMundefined;
}

double Sound_getPower (Sound me, double xmin, double xmax) {
	long n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return NUMdefined (sum2) ? sum2 / (n * my ny) : NUMundefined;
}

double Sound_getEnergyInAir (Sound me) {
	long n;
	double sum2 = getSumOfSquares (me, 0, 0, & n);
	return NUMdefined (sum2) ? sum2 * my dx / (400 * my ny) : NUMundefined;
}

double Sound_getIntensity_dB (Sound me) {
	long n;
	double sum2 = getSumOfSquares (me, 0, 0, & n);
	return NUMdefined (sum2) && sum2 != 0.0 ? 10 * log10 (sum2 / (n * my ny) / 4.0e-10) : NUMundefined;
}

double Sound_getPowerInAir (Sound me) {
	long n;
	double sum2 = getSumOfSquares (me, 0, 0, & n);
	return NUMdefined (sum2) ? sum2 / (n * my ny) / 400 : NUMundefined;
}

Sound Matrix_to_Sound_mono (Matrix me, long row) {
	Sound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
	if (! thee) return NULL;
	if (row < 0) row = my ny + 1 + row;
	if (row < 1) row = 1;
	if (row > my ny) row = my ny;
	NUMdvector_copyElements (my z [row], thy z [1], 1, my nx);
	return thee;
}

Matrix Sound_to_Matrix (Sound me) {
	Matrix thee = Data_copy (me);
	if (! thee) return NULL;
	Thing_overrideClass (thee, classMatrix);
	return thee;
}

Sound Sound_upsample (Sound me) {
	double *data = NULL;
	Sound thee = NULL;

	long nfft = 1;
	while (nfft < my nx + 2000) nfft *= 2;
	thee = Sound_create (my ny, my xmin, my xmax, my nx * 2, my dx / 2, my x1); cherror
	data = NUMdvector (1, 2 * nfft); cherror
	for (long channel = 1; channel <= my ny; channel ++) {
		NUMdvector_copyElements (my z [channel], data + 1000, 1, my nx);
		NUMrealft (data, nfft, 1); cherror
		long imin = (long) (nfft * 0.95);
		for (long i = imin + 1; i <= nfft; i ++) {
			data [i] *= ((double) (nfft - i)) / (nfft - imin);
		}
		data [2] = 0.0;
		NUMrealft (data, 2 * nfft, -1); cherror
		double factor = 1.0 / nfft;
		for (long i = 1; i <= thy nx; i ++) {
			thy z [channel] [i] = data [i + 2000] * factor;
		}
	}
end:
	NUMdvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

Sound Sound_resample (Sound me, double samplingFrequency, long precision) {
	double *data = NULL;
	double upfactor = samplingFrequency * my dx;
	long numberOfSamples = floor ((my xmax - my xmin) * samplingFrequency + 0.5), i;
	Sound thee = NULL, filtered = NULL;
	if (fabs (upfactor - 2) < 1e-6) return Sound_upsample (me);
	if (fabs (upfactor - 1) < 1e-6) return Data_copy (me);
	if (numberOfSamples < 1)
		return Melder_errorp ("Cannot resample to 0 samples.");
	thee = Sound_create (my ny, my xmin, my xmax, numberOfSamples, 1.0 / samplingFrequency,
		0.5 * (my xmin + my xmax - (numberOfSamples - 1) / samplingFrequency)); cherror
	if (upfactor < 1.0) {   /* Need anti-aliasing filter? */
		long nfft = 1, antiTurnAround = 1000;
		while (nfft < my nx + antiTurnAround * 2) nfft *= 2;
		data = NUMdvector (1, nfft); cherror
		filtered = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1); cherror
		for (long channel = 1; channel <= my ny; channel ++) {
			for (long i = 1; i <= nfft; i ++) {
				data [i] = 0;
			}
			NUMdvector_copyElements (my z [channel], data + antiTurnAround, 1, my nx);
			NUMrealft (data, nfft, 1); cherror   /* Go to the frequency domain. */
			for (long i = floor (upfactor * nfft); i <= nfft; i ++) {
				data [i] = 0;   /* Filter away high frequencies. */
			}
			data [2] = 0.0;
			NUMrealft (data, nfft, -1); cherror   /* Return to the time domain. */
			double factor = 1.0 / nfft;
			double *to = filtered -> z [channel];
			for (long i = 1; i <= my nx; i ++) {
				to [i] = data [i + antiTurnAround] * factor;
			}
		}
		me = filtered;   /* Reference copy. Remove at end. */
	}
	for (long channel = 1; channel <= my ny; channel ++) {
		double *from = my z [channel];
		double *to = thy z [channel];
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
				to [i] = NUM_interpolate_sinc (my z [channel], my nx, index, precision);
			}
		}
	}
end:
	forget (filtered);
	NUMdvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

Sound Sounds_append (Sound me, double silenceDuration, Sound thee) {
	Sound him;
	long nx_silence = floor (silenceDuration / my dx + 0.5), nx = my nx + nx_silence + thy nx;
	if (my ny != thy ny)
		return Melder_errorp ("Sounds_append: numbers of channels do not match (e.g. one is mono, the other stereo).");
	if (my dx != thy dx)
		return Melder_errorp ("Sounds_append: sampling frequencies do not match.");
	him = Sound_create (my ny, 0, nx * my dx, nx, my dx, 0.5 * my dx);
	if (! him) return NULL;
	for (long channel = 1; channel <= my ny; channel ++) {
		NUMdvector_copyElements (my z [channel], his z [channel], 1, my nx);
		NUMdvector_copyElements (thy z [channel], his z [channel] + my nx + nx_silence, 1, thy nx);
	}
	return him;
}

Sound Sounds_concatenate_e (Collection me, double overlapTime) {
	Sound thee = NULL;
	double *smoother = NULL;
//start:
	long numberOfChannels = 0, nx = 0, numberOfSmoothingSamples;
	double dx = 0.0;
	for (long i = 1; i <= my size; i ++) {
		Sound sound = my item [i];
		if (numberOfChannels == 0) {
			numberOfChannels = sound -> ny;
		} else if (sound -> ny != numberOfChannels) {
			error1 (L"To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
		}
		if (dx == 0.0) {
			dx = sound -> dx;
		} else if (sound -> dx != dx) {
			error1 (L"To concatenate sounds, their sampling frequencies must be equal.\n"
					"You could resample one or more of the sounds before concatenating.");
		}
		nx += sound -> nx;
	}
	numberOfSmoothingSamples = round (overlapTime / dx);
	thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx); cherror
	if (numberOfSmoothingSamples > 0) {
		smoother = NUMdvector (1, numberOfSmoothingSamples); cherror
		double factor = NUMpi / numberOfSmoothingSamples;
		for (long i = 1; i <= numberOfSmoothingSamples; i ++) {
			smoother [i] = 0.5 - 0.5 * cos (factor * (i - 0.5));
		}
	}
	nx = 0;
	double time = 0.0;
	for (long i = 1; i <= my size; i ++) {
		Sound sound = my item [i];
		if (numberOfSmoothingSamples > 2 * sound -> nx)
			error1 (L"At least one of the sounds is shorter than twice the overlap time.\nChoose a shorter overlap time.")
		bool thisIsTheFirstSound = ( i == 1 );
		bool thisIsTheLastSound = ( i == my size );
		bool weNeedSmoothingAtTheStartOfThisSound = ! thisIsTheFirstSound;
		bool weNeedSmoothingAtTheEndOfThisSound = ! thisIsTheLastSound;
		long numberOfSmoothingSamplesAtTheStartOfThisSound = weNeedSmoothingAtTheStartOfThisSound ? numberOfSmoothingSamples : 0;
		long numberOfSmoothingSamplesAtTheEndOfThisSound = weNeedSmoothingAtTheEndOfThisSound ? numberOfSmoothingSamples : 0;
		for (long channel = 1; channel <= numberOfChannels; channel ++) {
			for (long j = 1, mySample = 1, thySample = mySample + nx;
				 j <= numberOfSmoothingSamplesAtTheStartOfThisSound;
				 j ++, mySample ++, thySample ++)
			{
				thy z [channel] [thySample] += sound -> z [channel] [mySample] * smoother [j];   // add
			}
			NUMdvector_copyElements (sound -> z [channel], thy z [channel] + nx,
				1 + numberOfSmoothingSamplesAtTheStartOfThisSound, sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound);
			for (long j = 1, mySample = sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound + 1, thySample = mySample + nx;
				 j <= numberOfSmoothingSamplesAtTheEndOfThisSound;
				 j ++, mySample ++, thySample ++)
			{
				thy z [channel] [thySample] = sound -> z [channel] [mySample] * smoother [numberOfSmoothingSamplesAtTheEndOfThisSound + 1 - j];   // replace (or add, which is the same since it's all zeroes to start with)
			}
		}
		nx += sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound;
	}
	thy nx -= numberOfSmoothingSamples * (my size - 1);
	Melder_assert (thy nx == nx);
	thy xmax = thy nx * dx;
end:
	NUMdvector_free (smoother, 1);
	iferror {
		forget (thee);   // defensive
		Melder_error1 (L"Sounds not concatenated.");
	}
	return thee;
}

Sound Sounds_convolve (Sound me, Sound thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	Sound him = NULL;
	if (my ny > 1 && thy ny > 1 && my ny != thy ny)
		return Melder_errorp ("Sounds_convolve: the numbers of channels of the two sounds have to be equal or 1.");
	long numberOfChannels = my ny > thy ny ? my ny : thy ny;
	long n1 = my nx, n2 = thy nx;
	long n3 = n1 + n2 - 1, nfft = 1;
	double *data1 = NULL, *data2 = NULL;
	if (my dx != thy dx)
		return Melder_errorp ("Sounds_convolve: the sampling frequencies of the two sounds have to be equal.");
	while (nfft < n3) nfft *= 2;
	data1 = NUMdvector (1, nfft); cherror
	data2 = NUMdvector (1, nfft); cherror
	him = Sound_create (numberOfChannels, my xmin + thy xmin, my xmax + thy xmax, n3, my dx, my x1 + thy x1); cherror
	for (long channel = 1; channel <= numberOfChannels; channel ++) {
		double *a = my z [my ny == 1 ? 1 : channel];
		for (long i = n1; i > 0; i --) data1 [i] = a [i];
		for (long i = n1 + 1; i <= nfft; i ++) data1 [i] = 0.0;
		a = thy z [thy ny == 1 ? 1 : channel];
		for (long i = n2; i > 0; i --) data2 [i] = a [i];
		for (long i = n2 + 1; i <= nfft; i ++) data2 [i] = 0.0;
		NUMrealft (data1, nfft, 1); cherror
		NUMrealft (data2, nfft, 1); cherror
		data2 [1] *= data1 [1];
		data2 [2] *= data1 [2];
		for (long i = 3; i <= nfft; i += 2) {
			double temp = data1 [i] * data2 [i] - data1 [i + 1] * data2 [i + 1];
			data2 [i + 1] = data1 [i] * data2 [i + 1] + data1 [i + 1] * data2 [i];
			data2 [i] = temp;
		}
		NUMrealft (data2, nfft, -1); cherror
		a = him -> z [channel];
		for (long i = 1; i <= n3; i ++) {
			a [i] = data2 [i];
		}
	}
	switch (signalOutsideTimeDomain) {
		case kSounds_convolve_signalOutsideTimeDomain_ZERO: {
			// do nothing
		} break;
		case kSounds_convolve_signalOutsideTimeDomain_SIMILAR: {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double *a = his z [channel];
				double edge = n1 < n2 ? n1 : n2;
				for (long i = 1; i < edge; i ++) {
					double factor = edge / i;
					a [i] *= factor;
					a [n3 + 1 - i] *= factor;
				}
			}
		} break;
		//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
			// do nothing
		//} break;
		default: Melder_fatal ("Sounds_convolve: unimplemented outside-time-domain strategy %d", signalOutsideTimeDomain);
	}
	switch (scaling) {
		case kSounds_convolve_scaling_INTEGRAL: {
			Vector_multiplyByScalar (him, my dx / nfft);
		} break;
		case kSounds_convolve_scaling_SUM: {
			Vector_multiplyByScalar (him, 1.0 / nfft);
		} break;
		case kSounds_convolve_scaling_NORMALIZE: {
			double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (thee);
			if (normalizationFactor != 0.0) {
				Vector_multiplyByScalar (him, 1.0 / nfft / normalizationFactor);
			}
		} break;
		case kSounds_convolve_scaling_PEAK_099: {
			Vector_scale (him, 0.99);
		} break;
		default: Melder_fatal ("Sounds_convolve: unimplemented scaling %d", scaling);
	}
end:
	NUMdvector_free (data1, 1);
	NUMdvector_free (data2, 1);
	iferror forget (him);
	return him;
}

Sound Sounds_crossCorrelate (Sound me, Sound thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	Sound him = NULL;
	if (my ny > 1 && thy ny > 1 && my ny != thy ny)
		return Melder_errorp ("Sounds_crossCorrelate: the numbers of channels of the two sounds have to be equal or 1.");
	long numberOfChannels = my ny > thy ny ? my ny : thy ny;
	long n1 = my nx, n2 = thy nx;
	long n3 = n1 + n2 - 1, nfft = 1;
	double *data1 = NULL, *data2 = NULL;
	if (my dx != thy dx)
		return Melder_errorp ("Sounds_crossCorrelate: the sampling frequencies of the two sounds have to be equal.");
	while (nfft < n3) nfft *= 2;
	data1 = NUMdvector (1, nfft); cherror
	data2 = NUMdvector (1, nfft); cherror
	double my_xlast = my x1 + (n1 - 1) * my dx;
	him = Sound_create (numberOfChannels, thy xmin - my xmax, thy xmax - my xmin, n3, my dx, thy x1 - my_xlast); cherror
	for (long channel = 1; channel <= numberOfChannels; channel ++) {
		double *a = my z [my ny == 1 ? 1 : channel];
		for (long i = n1; i > 0; i --) data1 [i] = a [i];
		for (long i = n1 + 1; i <= nfft; i ++) data1 [i] = 0.0;
		a = thy z [thy ny == 1 ? 1 : channel];
		for (long i = n2; i > 0; i --) data2 [i] = a [i];
		for (long i = n2 + 1; i <= nfft; i ++) data2 [i] = 0.0;
		NUMrealft (data1, nfft, 1); cherror
		NUMrealft (data2, nfft, 1); cherror
		data2 [1] *= data1 [1];
		data2 [2] *= data1 [2];
		for (long i = 3; i <= nfft; i += 2) {
			double temp = data1 [i] * data2 [i] + data1 [i + 1] * data2 [i + 1];   // reverse me by taking the conjugate of data1
			data2 [i + 1] = data1 [i] * data2 [i + 1] - data1 [i + 1] * data2 [i];   // reverse me by taking the conjugate of data1
			data2 [i] = temp;
		}
		NUMrealft (data2, nfft, -1); cherror
		a = him -> z [channel];
		for (long i = 1; i < n1; i ++) {
			a [i] = data2 [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data2
		}
		for (long i = 1; i <= n2; i ++) {
			a [i + (n1 - 1)] = data2 [i];   // data for the second part ("positive lags") is at the beginning of data2
		}
	}
	switch (signalOutsideTimeDomain) {
		case kSounds_convolve_signalOutsideTimeDomain_ZERO: {
			// do nothing
		} break;
		case kSounds_convolve_signalOutsideTimeDomain_SIMILAR: {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double *a = his z [channel];
				double edge = n1 < n2 ? n1 : n2;
				for (long i = 1; i < edge; i ++) {
					double factor = edge / i;
					a [i] *= factor;
					a [n3 + 1 - i] *= factor;
				}
			}
		} break;
		//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
			// do nothing
		//} break;
		default: Melder_fatal ("Sounds_crossCorrelate: unimplemented outside-time-domain strategy %d", signalOutsideTimeDomain);
	}
	switch (scaling) {
		case kSounds_convolve_scaling_INTEGRAL: {
			Vector_multiplyByScalar (him, my dx / nfft);
		} break;
		case kSounds_convolve_scaling_SUM: {
			Vector_multiplyByScalar (him, 1.0 / nfft);
		} break;
		case kSounds_convolve_scaling_NORMALIZE: {
			double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (thee);
			if (normalizationFactor != 0.0) {
				Vector_multiplyByScalar (him, 1.0 / nfft / normalizationFactor);
			}
		} break;
		case kSounds_convolve_scaling_PEAK_099: {
			Vector_scale (him, 0.99);
		} break;
		default: Melder_fatal ("Sounds_crossCorrelate: unimplemented scaling %d", scaling);
	}
end:
	NUMdvector_free (data1, 1);
	NUMdvector_free (data2, 1);
	iferror forget (him);
	return him;
}

Sound Sound_autoCorrelate (Sound me, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	Sound thee = NULL;
	long numberOfChannels = my ny, n1 = my nx, n2 = n1 + n1 - 1, nfft = 1;
	while (nfft < n2) nfft *= 2;
	double *data = NUMdvector (1, nfft); cherror
	double my_xlast = my x1 + (n1 - 1) * my dx;
	thee = Sound_create (numberOfChannels, my xmin - my xmax, my xmax - my xmin, n2, my dx, my x1 - my_xlast); cherror
	for (long channel = 1; channel <= numberOfChannels; channel ++) {
		double *a = my z [channel];
		for (long i = n1; i > 0; i --) data [i] = a [i];
		for (long i = n1 + 1; i <= nfft; i ++) data [i] = 0.0;
		NUMrealft (data, nfft, 1); cherror
		data [1] *= data [1];
		data [2] *= data [2];
		for (long i = 3; i <= nfft; i += 2) {
			data [i] = data [i] * data [i] + data [i + 1] * data [i + 1];
			data [i + 1] = 0.0;   // reverse me by taking the conjugate of data1
		}
		NUMrealft (data, nfft, -1); cherror
		a = thy z [channel];
		for (long i = 1; i < n1; i ++) {
			a [i] = data [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data
		}
		for (long i = 1; i <= n1; i ++) {
			a [i + (n1 - 1)] = data [i];   // data for the second part ("positive lags") is at the beginning of data
		}
	}
	switch (signalOutsideTimeDomain) {
		case kSounds_convolve_signalOutsideTimeDomain_ZERO: {
			// do nothing
		} break;
		case kSounds_convolve_signalOutsideTimeDomain_SIMILAR: {
			for (long channel = 1; channel <= numberOfChannels; channel ++) {
				double *a = thy z [channel];
				double edge = n1;
				for (long i = 1; i < edge; i ++) {
					double factor = edge / i;
					a [i] *= factor;
					a [n2 + 1 - i] *= factor;
				}
			}
		} break;
		//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
			// do nothing
		//} break;
		default: Melder_fatal ("Sounds_autoCorrelate: unimplemented outside-time-domain strategy %d", signalOutsideTimeDomain);
	}
	switch (scaling) {
		case kSounds_convolve_scaling_INTEGRAL: {
			Vector_multiplyByScalar (thee, my dx / nfft);
		} break;
		case kSounds_convolve_scaling_SUM: {
			Vector_multiplyByScalar (thee, 1.0 / nfft);
		} break;
		case kSounds_convolve_scaling_NORMALIZE: {
			double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (me);
			if (normalizationFactor != 0.0) {
				Vector_multiplyByScalar (thee, 1.0 / nfft / normalizationFactor);
			}
		} break;
		case kSounds_convolve_scaling_PEAK_099: {
			Vector_scale (thee, 0.99);
		} break;
		default: Melder_fatal ("Sounds_autoCorrelate: unimplemented scaling %d", scaling);
	}
end:
	NUMdvector_free (data, 1);
	iferror forget (thee);
	return thee;
}

void Sound_draw (Sound me, Graphics g,
	double tmin, double tmax, double minimum, double maximum, bool garnish, const wchar_t *method)
{
	long ixmin, ixmax, ix;
	bool treversed = tmin > tmax;
	if (treversed) { double temp = tmin; tmin = tmax; tmax = temp; }
	/*
	 * Automatic domain.
	 */
	if (tmin == tmax) {
		tmin = my xmin;
		tmax = my xmax;
	}
	/*
	 * Domain expressed in sample numbers.
	 */
	Matrix_getWindowSamplesX (me, tmin, tmax, & ixmin, & ixmax);
	/*
	 * Automatic vertical range.
	 */
	if (minimum == maximum) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, 1, my ny, & minimum, & maximum);
		if (minimum == maximum) {
			minimum -= 1.0;
			maximum += 1.0;
		}
	}
	/*
	 * Set coordinates for drawing.
	 */
	Graphics_setInner (g);
	for (long channel = 1; channel <= my ny; channel ++) {
		Graphics_setWindow (g, treversed ? tmax : tmin, treversed ? tmin : tmax,
			minimum - (my ny - channel) * (maximum - minimum),
			maximum + (channel - 1) * (maximum - minimum));
		if (wcsstr (method, L"bars") || wcsstr (method, L"Bars")) {
			for (ix = ixmin; ix <= ixmax; ix ++) {
				double x = Sampled_indexToX (me, ix);
				double y = my z [channel] [ix];
				double left = x - 0.5 * my dx, right = x + 0.5 * my dx;
				if (y > maximum) y = maximum;
				if (left < tmin) left = tmin;
				if (right > tmax) right = tmax;
				Graphics_line (g, left, y, right, y);
				Graphics_line (g, left, y, left, minimum);
				Graphics_line (g, right, y, right, minimum);
			}
		} else if (wcsstr (method, L"poles") || wcsstr (method, L"Poles")) {
			for (ix = ixmin; ix <= ixmax; ix ++) {
				double x = Sampled_indexToX (me, ix);
				Graphics_line (g, x, 0, x, my z [channel] [ix]);
			}
		} else if (wcsstr (method, L"speckles") || wcsstr (method, L"Speckles")) {
			for (ix = ixmin; ix <= ixmax; ix ++) {
				double x = Sampled_indexToX (me, ix);
				Graphics_fillCircle_mm (g, x, my z [channel] [ix], 1.0);
			}
		} else {
			/*
			 * The default: draw as a curve.
			 */
			Graphics_function (g, my z [channel], ixmin, ixmax,
				Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
		}
	}
	Graphics_setWindow (g, treversed ? tmax : tmin, treversed ? tmin : tmax, minimum, maximum);
	if (garnish && my ny == 2) Graphics_line (g, tmin, 0.5 * (minimum + maximum), tmax, 0.5 * (minimum + maximum));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, 1, L"Time (s)");
		Graphics_marksBottom (g, 2, 1, 1, 0);
		Graphics_setWindow (g, tmin, tmax, minimum - (my ny - 1) * (maximum - minimum), maximum);
		Graphics_markLeft (g, minimum, 1, 1, 0, NULL);
		Graphics_markLeft (g, maximum, 1, 1, 0, NULL);
		if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
			Graphics_markLeft (g, 0.0, 1, 1, 1, NULL);
		}
		if (my ny == 2) {
			Graphics_setWindow (g, treversed ? tmax : tmin, treversed ? tmin : tmax, minimum, maximum + (my ny - 1) * (maximum - minimum));
			Graphics_markRight (g, minimum, 1, 1, 0, NULL);
			Graphics_markRight (g, maximum, 1, 1, 0, NULL);
			if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
				Graphics_markRight (g, 0.0, 1, 1, 1, NULL);
			}
		}
	}
}

static double interpolate (Sound me, long i1, long channel)
/* Precondition: my z [1] [i1] != my z [1] [i1 + 1]; */
{
	long i2 = i1 + 1;
	double x1 = Sampled_indexToX (me, i1), x2 = Sampled_indexToX (me, i2);
	double y1 = my z [channel] [i1], y2 = my z [channel] [i2];
	return x1 + (x2 - x1) * y1 / (y1 - y2);   /* Linear. */
}
double Sound_getNearestZeroCrossing (Sound me, double position, long channel) {
	double *amplitude = my z [channel];
	long leftSample = Sampled_xToLowIndex (me, position);
	long rightSample = leftSample + 1, ileft, iright;
	double leftZero, rightZero;
	/* Are we already at a zero crossing? */
	if (leftSample >= 1 && rightSample <= my nx &&
		(amplitude [leftSample] >= 0.0) !=
		(amplitude [rightSample] >= 0.0))
	{
		return interpolate (me, leftSample, channel);
	}
	/* Search to the left. */
	if (leftSample > my nx) return NUMundefined;
	for (ileft = leftSample - 1; ileft >= 1; ileft --)
		if ((amplitude [ileft] >= 0.0) != (amplitude [ileft + 1] >= 0.0))
		{
			leftZero = interpolate (me, ileft, channel);
			break;
		}
	/* Search to the right. */
	if (rightSample < 1) return NUMundefined;
	for (iright = rightSample + 1; iright <= my nx; iright ++)
		if ((amplitude [iright] >= 0.0) != (amplitude [iright - 1] >= 0.0))
		{
			rightZero = interpolate (me, iright - 1, channel);
			break;
		}
	if (ileft < 1 && iright > my nx) return NUMundefined;
	return ileft < 1 ? rightZero : iright > my nx ? leftZero :
		position - leftZero < rightZero - position ? leftZero : rightZero;
}

void Sound_setZero (Sound me, double tmin_in, double tmax_in, int roundTimesToNearestZeroCrossing) {
	Function_unidirectionalAutowindow (me, & tmin_in, & tmax_in);
	Function_intersectRangeWithDomain (me, & tmin_in, & tmax_in);
	for (long channel = 1; channel <= my ny; channel ++) {
		double tmin = tmin_in, tmax = tmax_in;
		if (roundTimesToNearestZeroCrossing) {
			if (tmin > my xmin) tmin = Sound_getNearestZeroCrossing (me, tmin_in, channel);
			if (tmax < my xmax) tmax = Sound_getNearestZeroCrossing (me, tmax_in, channel);
		}
		if (tmin == NUMundefined) tmin = my xmin;
		if (tmax == NUMundefined) tmax = my xmax;
		long imin, imax;
		Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		for (long i = imin; i <= imax; i ++) {
			my z [channel] [i] = 0.0;
		}
	}
}

Sound Sound_createFromToneComplex (double startingTime, double endTime, double sampleRate,
	int phase, double frequencyStep, double firstFrequency, double ceiling, long numberOfComponents)
{
	Sound me = NULL;
	long maximumNumberOfComponents, icomp, isamp;
	double *amplitude;
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
	me = Sound_create (1, startingTime, endTime, floor ((endTime - startingTime) * sampleRate + 0.5),
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

void Sound_multiplyByWindow (Sound me, enum kSound_windowShape windowShape) {
	for (long channel = 1; channel <= my ny; channel ++) {
		long i, n = my nx;
		double *amp = my z [channel];
		double imid, edge, onebyedge1, factor;
		switch (windowShape) {
			case kSound_windowShape_RECTANGULAR:
				;
			break;
			case kSound_windowShape_TRIANGULAR:   /* "Bartlett" */
				for (i = 1; i <= n; i ++) { double phase = (double) i / n;   /* 0..1 */
					amp [i] *= 1.0 - fabs ((2.0 * phase - 1.0)); }
			break;
			case kSound_windowShape_PARABOLIC:   /* "Welch" */
				for (i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0); }
			break;
			case kSound_windowShape_HANNING:
				for (i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 0.5 * (1.0 - cos (2.0 * NUMpi * phase)); }
			break;
			case kSound_windowShape_HAMMING:
				for (i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 0.54 - 0.46 * cos (2.0 * NUMpi * phase); }
			break;
			case kSound_windowShape_GAUSSIAN_1:
				imid = 0.5 * (n + 1), edge = exp (-3.0), onebyedge1 = 1 / (1.0 - edge);   /* -0.5..+0.5 */
				for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-12.0 * phase * phase) - edge) * onebyedge1; }
			break;
			case kSound_windowShape_GAUSSIAN_2:
				imid = 0.5 * (double) (n + 1), edge = exp (-12.0), onebyedge1 = 1 / (1.0 - edge);
				for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-48.0 * phase * phase) - edge) * onebyedge1; }
			break;
			case kSound_windowShape_GAUSSIAN_3:
				imid = 0.5 * (double) (n + 1), edge = exp (-27.0), onebyedge1 = 1 / (1.0 - edge);
				for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-108.0 * phase * phase) - edge) * onebyedge1; }
			break;
			case kSound_windowShape_GAUSSIAN_4:
				imid = 0.5 * (double) (n + 1), edge = exp (-48.0), onebyedge1 = 1 / (1.0 - edge);
				for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-192.0 * phase * phase) - edge) * onebyedge1; }
			break;
			case kSound_windowShape_GAUSSIAN_5:
				imid = 0.5 * (double) (n + 1), edge = exp (-75.0), onebyedge1 = 1 / (1.0 - edge);
				for (i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-300.0 * phase * phase) - edge) * onebyedge1; }
			break;
			case kSound_windowShape_KAISER_1:
				imid = 0.5 * (double) (n + 1);
				factor = 1 / NUMbessel_i0_f (2 * NUMpi);
				for (i = 1; i <= n; i ++) { double phase = 2 * ((double) i - imid) / n;   /* -1..+1 */
					double root = 1 - phase * phase;
					amp [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2 * NUMpi * sqrt (root)); }
			break;
			case kSound_windowShape_KAISER_2:
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
}

void Sound_scaleIntensity (Sound me, double newAverageIntensity) {
	double currentIntensity = Sound_getIntensity_dB (me), factor;
	if (currentIntensity == NUMundefined) return;
	factor = pow (10, (newAverageIntensity - currentIntensity) / 20.0);
	for (long channel = 1; channel <= my ny; channel ++) {
		for (long i = 1; i <= my nx; i ++) {
			my z [channel] [i] *= factor;
		}
	}
}

void Sound_overrideSamplingFrequency (Sound me, double rate) {
	my dx = 1 / rate;
	my x1 = my xmin + 0.5 * my dx;
	my xmax = my xmin + my nx * my dx;
}

Sound Sound_extractPart (Sound me, double t1, double t2, enum kSound_windowShape windowShape, double relativeWidth, bool preserveTimes) {
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
	if (ix2 < ix1) error1 (L"Extracted Sound would contain no samples.")
	/*
	 * Create sound, optionally shifted to [0..t2-t1].
	 */
	thee = Sound_create (my ny, t1, t2, ix2 - ix1 + 1, my dx, my x1 + (ix1 - 1) * my dx); cherror
	if (! preserveTimes) { thy xmin = 0.0; thy xmax -= t1; thy x1 -= t1; }
	/*
	 * Copy only *real* samples into the new sound.
	 * The *virtual* samples will remain at zero.
	 */
	for (long channel = 1; channel <= my ny; channel ++) {
		NUMdvector_copyElements (my z [channel], thy z [channel] + 1 - ix1,
				( ix1 < 1 ? 1 : ix1 ), ( ix2 > my nx ? my nx : ix2 ));
	}
	/*
	 * Multiply by a window that extends throughout the target domain.
	 */
	Sound_multiplyByWindow (thee, windowShape);
end:
	iferror { forget (thee); return Melder_errorp ("(Sound_extractPart:) Not performed."); }
	return thee;
}

int Sound_filterWithFormants (Sound me, double tmin, double tmax,
	int numberOfFormants, double formant [], double bandwidth [])
{
	for (long channel = 1; channel <= my ny; channel ++) {
		long itmin, itmax, n;
		if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   /* Autowindowing. */
		if ((n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax)) <= 2)
			return Melder_error1 (L"Sound too short.");
		double *amplitude = my z [channel] + itmin - 1;   /* Base 1. */
		NUMdeemphasize_f (amplitude, n, my dx, 50.0);
		for (int iformant = 1; iformant <= numberOfFormants; iformant ++) {
			NUMfilterSecondOrderSection_fb (amplitude, n, my dx, formant [iformant], bandwidth [iformant]);
		}
	}
	Matrix_scaleAbsoluteExtremum (me, 0.99);
	return 1;
}

Sound Sound_filter_oneFormant (Sound me, double frequency, double bandwidth) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_filterWithOneFormantInline (thee, frequency, bandwidth);
	return thee;
}

int Sound_filterWithOneFormantInline (Sound me, double frequency, double bandwidth) {
	for (long channel = 1; channel <= my ny; channel ++) {
		NUMfilterSecondOrderSection_fb (my z [channel], my nx, my dx, frequency, bandwidth);
	}
	Matrix_scaleAbsoluteExtremum (me, 0.99);
	return 1;
}

Sound Sound_filter_preemphasis (Sound me, double frequency) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_preEmphasis (thee, frequency);
	Matrix_scaleAbsoluteExtremum (me, 0.99);
	return thee;
}

Sound Sound_filter_deemphasis (Sound me, double frequency) {
	Sound thee = Data_copy (me);
	if (! thee) return NULL;
	Sound_deEmphasis (thee, frequency);
	Matrix_scaleAbsoluteExtremum (me, 0.99);
	return thee;
}

void Sound_reverse (Sound me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // Autowindowing.
	long itmin, itmax;
	long n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax) / 2;
	for (long channel = 1; channel <= my ny; channel ++) {
		double *amp = my z [channel];
		for (long i = 0; i < n; i ++) {
			double dummy = amp [itmin + i];
			amp [itmin + i] = amp [itmax - i];
			amp [itmax - i] = dummy;
		}
	}
}

Sound Sounds_crossCorrelate_short (Sound me, Sound thee, double tmin, double tmax, int normalize) {
	Sound him = NULL;
	double dt, dphase, t1;
	long i1, i2, nt, i;
	if (my dx != thy dx)
		return Melder_errorp ("(Sounds_crossCorrelation:) Sampling frequencies do not match.");
	if (my ny != thy ny)
		return Melder_errorp ("(Sounds_crossCorrelation:) Numbers of channels do not match.");
	dt = my dx;
	dphase = (thy x1 - my x1) / dt;
	dphase -= floor (dphase);   /* A number between 0 and 1. */
	i1 = ceil (tmin / dt - dphase);   /* Index of first sample if sample at dphase has index 0. */
	i2 = floor (tmax / dt - dphase);   /* Index of last sample if sample at dphase has index 0. */
	nt = i2 - i1 + 1;
	if (nt < 1)
		return Melder_errorp ("(Sounds_crossCorrelation:) Window too small.");
	t1 = (dphase + i1) * dt;
	him = Sound_create (1, tmin, tmax, nt, dt, t1);
	if (him == NULL) return NULL;
	for (i = 1; i <= nt; i ++) {
		long di = i - 1 + i1, ime;
		for (ime = 1; ime <= my nx; ime ++) {
			if (ime + di < 1) continue;
			if (ime + di > thy nx) break;
			for (long channel = 1; channel <= my ny; channel ++) {
				his z [1] [i] += my z [channel] [ime] * thy z [channel] [ime + di];
			}
		}
	}
	if (normalize) {
		double mypower = 0.0, thypower = 0.0;
		for (long channel = 1; channel <= my ny; channel ++) {
			for (i = 1; i <= my nx; i ++) {
				double value = my z [channel] [i];
				mypower += value * value;
			}
			for (i = 1; i <= thy nx; i ++) {
				double value = thy z [channel] [i];
				thypower += value * value;
			}
		}
		if (mypower != 0.0 && thypower != 0.0) {
			double factor = 1.0 / (sqrt (mypower) * sqrt (thypower));
			for (i = 1; i <= nt; i ++) {
				his z [1] [i] *= factor;
			}
		}
	} else {
		double factor = dt / my ny;
		for (i = 1; i <= nt; i ++) {
			his z [1] [i] *= factor;
		}
	}
	return him;
}

/* End of file Sound.c */
