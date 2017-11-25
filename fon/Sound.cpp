/* Sound.cpp
 *
 * Copyright (C) 1992-2012,2014,2015,2016,2017 Paul Boersma
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

/*
 * a selection of changes:
 * pb 2006/12/31 stereo
 * pb 2010/03/26 Sounds_convolve, Sounds_crossCorrelate, Sound_autocorrelate
 */

#include "Sound.h"
#include "Sound_extensions.h"
#include "NUM2.h"
#include "tensor.h"

#include "enums_getText.h"
#include "Sound_enums.h"
#include "enums_getValue.h"
#include "Sound_enums.h"

Thing_implement (Sound, Vector, 2);

autoSound Sound_clipboard;

void structSound :: v_info () {
	structDaata :: v_info ();
	const double rho_c = 400;   /* rho = 1.14 kg m-3; c = 353 m s-1; [rho c] = kg m-2 s-1 */
	double minimum = z [1] [1], maximum = minimum;
	MelderInfo_writeLine (U"Number of channels: ", ny, ny == 1 ? U" (mono)" : ny == 2 ? U" (stereo)" : U"");
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", xmax - xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of samples: ", nx);
	MelderInfo_writeLine (U"   Sampling period: ", dx, U" seconds");
	MelderInfo_writeLine (U"   Sampling frequency: ", Melder_single (1.0 / dx), U" Hz");
	MelderInfo_writeLine (U"   First sample centred at: ", x1, U" seconds");
	{// scope
		real80 sum = 0.0, sumOfSquares = 0.0;
		for (integer channel = 1; channel <= ny; channel ++) {
			double *amplitude = z [channel];
			for (integer i = 1; i <= nx; i ++) {
				double value = amplitude [i];
				sum += value;
				sumOfSquares += value * value;
				if (value < minimum) minimum = value;
				if (value > maximum) maximum = value;
			}
		}
		MelderInfo_writeLine (U"Amplitude:");
		MelderInfo_writeLine (U"   Minimum: ", Melder_single (minimum), U" Pascal");
		MelderInfo_writeLine (U"   Maximum: ", Melder_single (maximum), U" Pascal");
		double mean = (real) sum / (nx * ny);
		MelderInfo_writeLine (U"   Mean: ", Melder_single (mean), U" Pascal");
		MelderInfo_writeLine (U"   Root-mean-square: ", Melder_single (sqrt ((real) sumOfSquares / (nx * ny))), U" Pascal");
		double penergy = (real) sumOfSquares * dx / ny;   /* Pa2 s = kg2 m-2 s-3 */
		MelderInfo_write (U"Total energy: ", Melder_single (penergy), U" Pascal\u00B2 sec");
		double energy = penergy / rho_c;   /* kg s-2 = Joule m-2 */
		MelderInfo_writeLine (U" (energy in air: ", Melder_single (energy), U" Joule/m\u00B2)");
		double power = energy / (dx * nx);   /* kg s-3 = Watt/m2 */
		MelderInfo_write (U"Mean power (intensity) in air: ", Melder_single (power), U" Watt/m\u00B2");
		if (power != 0.0) {
			MelderInfo_writeLine (U" = ", Melder_half (10 * log10 (power / 1e-12)), U" dB");
		} else {
			MelderInfo_writeLine (U"");
		}
	}
	if (nx > 1) {
		for (integer channel = 1; channel <= ny; channel ++) {
			double stdev = stdev_scalar ({ z [channel], our nx });
			MelderInfo_writeLine (U"Standard deviation in channel ", channel, U": ", Melder_single (stdev), U" Pascal");
		}
	}
}

double structSound :: v_getMatrix (integer irow, integer icol) {
	if (irow < 1 || irow > ny) {
		if (irow == 0) {
			if (icol < 1 || icol > nx) return 0.0;
			if (ny == 1) return z [1] [icol];   // optimization
			if (ny == 2) return 0.5 * (z [1] [icol] + z [2] [icol]);   // optimization
			real80 sum = 0.0;
			for (integer channel = 1; channel <= ny; channel ++) {
				sum += z [channel] [icol];
			}
			return (real) sum / ny;
		}
		return 0.0;
	}
	if (icol < 1 || icol > nx) return 0.0;
	return z [irow] [icol];
}

double structSound :: v_getFunction2 (double x, double y) {
	integer channel = Melder_ifloor (y);
	if (channel < 0 || channel > ny || y != (double) channel) return 0.0;
	return v_getFunction1 (channel, x);
}

autoSound Sound_create (integer numberOfChannels, double xmin, double xmax, integer nx, double dx, double x1) {
	try {
		autoSound me = Thing_new (Sound);
		Matrix_init (me.get(), xmin, xmax, nx, dx, x1, 1, numberOfChannels, numberOfChannels, 1, 1);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created.");
	}
}

autoSound Sound_createSimple (integer numberOfChannels, double duration, double samplingFrequency) {
	Melder_assert (duration >= 0.0);
	Melder_assert (samplingFrequency > 0.0);
	double numberOfSamples_f = round (duration * samplingFrequency);
	if (numberOfSamples_f > (double) INT32_MAX)
		Melder_throw (U"Cannot create sounds with more than ", Melder_bigInteger (INT32_MAX), U" samples, because they cannot be saved to disk.");
	return Sound_create (numberOfChannels, 0.0, duration, (integer) (int32_t) numberOfSamples_f,
		1.0 / samplingFrequency, 0.5 / samplingFrequency);
}

autoSound Sound_convertToMono (Sound me) {
	if (my ny == 1) return Data_copy (me);   // optimization
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		if (my ny == 2) {   // Optimization.
			for (integer i = 1; i <= my nx; i ++) {
				thy z [1] [i] = 0.5 * (my z [1] [i] + my z [2] [i]);
			}
		} else {
			for (integer i = 1; i <= my nx; i ++) {
				double sum = my z [1] [i] + my z [2] [i] + my z [3] [i];
				for (integer channel = 4; channel <= my ny; channel ++) {
					sum += my z [channel] [i];
				}
				thy z [1] [i] = sum / my ny;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to mono.");
	}
}

autoSound Sound_convertToStereo (Sound me) {
	if (my ny == 2) return Data_copy (me);
	try {
		if (my ny > 2) {
			Melder_throw (U"The Sound has ", my ny, U" channels; don't know which to choose.");
		}
		Melder_assert (my ny == 1);
		autoSound thee = Sound_create (2, my xmin, my xmax, my nx, my dx, my x1);
		for (integer i = 1; i <= my nx; i ++) {
			thy z [1] [i] = thy z [2] [i] = my z [1] [i];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to stereo.");
	}
}

autoSound Sounds_combineToStereo (OrderedOf<structSound>* me) {
	try {
		integer totalNumberOfChannels = 0;
		double sharedSamplingPeriod = 0.0;
		for (integer isound = 1; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			totalNumberOfChannels += sound -> ny;
			if (sharedSamplingPeriod == 0.0) {
				sharedSamplingPeriod = sound -> dx;
			} else if (sound -> dx != sharedSamplingPeriod) {
				Melder_throw (U"To combine sounds, their sampling frequencies must be equal.\n"
						U"You could resample one or more of the sounds before combining.");
			}
		}
		Melder_assert (my size > 0);
		double sharedMinimumTime = my at [1] -> xmin;
		double sharedMaximumTime = my at [1] -> xmax;
		for (integer isound = 2; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			if (sound -> xmin < sharedMinimumTime) sharedMinimumTime = sound -> xmin;
			if (sound -> xmax > sharedMaximumTime) sharedMaximumTime = sound -> xmax;
		}
		autoNUMvector <integer> numberOfInitialZeroes (1, my size);
		integer sharedNumberOfSamples = 0;
		double sumOfFirstTimes = 0.0;
		for (integer isound = 1; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			numberOfInitialZeroes [isound] = Melder_ifloor ((sound -> xmin - sharedMinimumTime) / sharedSamplingPeriod);
			double newFirstTime = sound -> x1 - sound -> dx * numberOfInitialZeroes [isound];
			sumOfFirstTimes += newFirstTime;
			integer newNumberOfSamplesThroughLastNonzero = sound -> nx + numberOfInitialZeroes [isound];
			if (newNumberOfSamplesThroughLastNonzero > sharedNumberOfSamples) sharedNumberOfSamples = newNumberOfSamplesThroughLastNonzero;
		}
		double sharedTimeOfFirstSample = sumOfFirstTimes / my size;   // this is an approximation
		autoSound thee = Sound_create (totalNumberOfChannels, sharedMinimumTime, sharedMaximumTime,
			sharedNumberOfSamples, sharedSamplingPeriod, sharedTimeOfFirstSample);
		integer channelNumber = 0;
		for (integer isound = 1; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			integer offset = numberOfInitialZeroes [isound];
			for (integer ichan = 1; ichan <= sound -> ny; ichan ++) {
				channelNumber ++;
				for (integer isamp = 1; isamp <= sound -> nx; isamp ++) {
					thy z [channelNumber] [isamp + offset] = sound -> z [ichan] [isamp];
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sounds not combined to stereo.");
	}
}

autoSound Sound_extractChannel (Sound me, integer ichan) {
	try {
		if (ichan <= 0 || ichan > my ny)
			Melder_throw (U"There is no channel ", ichan, U".");
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			thy z [1] [isamp] = my z [ichan] [isamp];
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", ichan, U" not extracted.");
	}
}

static double getSumOfSquares (Sound me, double xmin, double xmax, integer *n) {
	if (xmax <= xmin) { xmin = my xmin; xmax = my xmax; }
	integer imin, imax;
	*n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (*n < 1) return undefined;
	real80 sum2 = 0.0;
	for (integer channel = 1; channel <= my ny; channel ++) {
		double *amplitude = my z [channel];
		for (integer i = imin; i <= imax; i ++) {
			double value = amplitude [i];
			sum2 += value * value;
		}
	}
	return (real) sum2;
}

double Sound_getRootMeanSquare (Sound me, double xmin, double xmax) {
	integer n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sum2) ? sqrt (sum2 / (n * my ny)) : undefined;
}

double Sound_getEnergy (Sound me, double xmin, double xmax) {
	integer n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sum2) ? sum2 * my dx / my ny : undefined;
}

double Sound_getPower (Sound me, double xmin, double xmax) {
	integer n;
	double sum2 = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sum2) ? sum2 / (n * my ny) : undefined;
}

double Sound_getEnergyInAir (Sound me) {
	integer n;
	double sum2 = getSumOfSquares (me, 0.0, 0.0, & n);
	return isdefined (sum2) ? sum2 * my dx / (400.0 * my ny) : undefined;
}

double Sound_getIntensity_dB (Sound me) {
	integer n;
	double sum2 = getSumOfSquares (me, 0.0, 0.0, & n);
	return isdefined (sum2) && sum2 != 0.0 ? 10.0 * log10 (sum2 / (n * my ny) / 4.0e-10) : undefined;
}

double Sound_getPowerInAir (Sound me) {
	integer n;
	double sum2 = getSumOfSquares (me, 0, 0, & n);
	return ( isdefined (sum2) ? sum2 / (n * my ny) / 400 : undefined );
}

autoSound Matrix_to_Sound_mono (Matrix me, integer row) {
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		if (row < 0) row = my ny + 1 + row;
		if (row < 1) row = 1;
		if (row > my ny) row = my ny;
		NUMvector_copyElements (my z [row], thy z [1], 1, my nx);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

autoSound Matrix_to_Sound (Matrix me) {
	try {
		autoSound thee = Thing_new (Sound);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Sound.");
	}
}

autoMatrix Sound_to_Matrix (Sound me) {
	try {
		autoMatrix thee = Thing_new (Matrix);
		my structMatrix :: v_copy (thee.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not converted to Matrix.");
	}
}

autoSound Sound_upsample (Sound me) {
	try {
		integer nfft = 1;
		while (nfft < my nx + 2000) nfft *= 2;
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx * 2, my dx / 2, my x1 - my dx / 4);
		for (integer channel = 1; channel <= my ny; channel ++) {
			autonumvec data (2 * nfft, kTensorInitializationType::ZERO);   // zeroing is important...
			NUMvector_copyElements (my z [channel], & data [1000], 1, my nx);   // ...because this fills only part of the sound
			NUMrealft (data.at, nfft, 1);
			integer imin = (integer) (nfft * 0.95);
			for (integer i = imin + 1; i <= nfft; i ++) {
				data [i] *= ((double) (nfft - i)) / (nfft - imin);
			}
			data [2] = 0.0;
			NUMrealft (data.at, 2 * nfft, -1);
			double factor = 1.0 / nfft;
			for (integer i = 1; i <= thy nx; i ++) {
				thy z [channel] [i] = data [i + 2000] * factor;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not upsampled.");
	}
}

autoSound Sound_resample (Sound me, double samplingFrequency, integer precision) {
	double upfactor = samplingFrequency * my dx;
	if (fabs (upfactor - 2) < 1e-6) return Sound_upsample (me);
	if (fabs (upfactor - 1) < 1e-6) return Data_copy (me);
	try {
		integer numberOfSamples = Melder_iround ((my xmax - my xmin) * samplingFrequency);
		if (numberOfSamples < 1)
			Melder_throw (U"The resampled Sound would have no samples.");
		autoSound filtered;
		if (upfactor < 1.0) {   // need anti-aliasing filter?
			integer nfft = 1, antiTurnAround = 1000;
			while (nfft < my nx + antiTurnAround * 2) nfft *= 2;
			autonumvec data (nfft, kTensorInitializationType::RAW);   // will be zeroed in every turn of the loop
			filtered = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
			for (integer channel = 1; channel <= my ny; channel ++) {
				for (integer i = 1; i <= nfft; i ++) {
					data [i] = 0.0;
				}
				NUMvector_copyElements (my z [channel], & data [antiTurnAround], 1, my nx);
				NUMrealft (data.at, nfft, 1);   // go to the frequency domain
				for (integer i = Melder_ifloor (upfactor * nfft); i <= nfft; i ++) {
					data [i] = 0.0;   // filter away high frequencies
				}
				data [2] = 0.0;
				NUMrealft (data.at, nfft, -1);   // return to the time domain
				double factor = 1.0 / nfft;
				double *to = filtered -> z [channel];
				for (integer i = 1; i <= my nx; i ++) {
					to [i] = data [i + antiTurnAround] * factor;
				}
			}
			me = filtered.get();   // reference copy; remove at end
		}
		autoSound thee = Sound_create (my ny, my xmin, my xmax, numberOfSamples, 1.0 / samplingFrequency,
			0.5 * (my xmin + my xmax - (numberOfSamples - 1) / samplingFrequency));
		for (integer channel = 1; channel <= my ny; channel ++) {
			double *from = my z [channel];
			double *to = thy z [channel];
			if (precision <= 1) {
				for (integer i = 1; i <= numberOfSamples; i ++) {
					double x = Sampled_indexToX (thee.get(), i);
					double index = Sampled_xToIndex (me, x);
					integer leftSample = Melder_ifloor (index);
					double fraction = index - leftSample;
					to [i] = leftSample < 1 || leftSample >= my nx ? 0.0 :
						(1 - fraction) * from [leftSample] + fraction * from [leftSample + 1];
				}
			} else {
				for (integer i = 1; i <= numberOfSamples; i ++) {
					double x = Sampled_indexToX (thee.get(), i);
					double index = Sampled_xToIndex (me, x);
					to [i] = NUM_interpolate_sinc (my z [channel], my nx, index, precision);
				}
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not resampled.");
	}
}

autoSound Sounds_append (Sound me, double silenceDuration, Sound thee) {
	try {
		integer nx_silence = Melder_iround (silenceDuration / my dx), nx = my nx + nx_silence + thy nx;
		if (my ny != thy ny)
			Melder_throw (U"The numbers of channels are not equal (e.g. one is mono, the other stereo).");
		if (my dx != thy dx)
			Melder_throw (U"The sampling frequencies are not equal.");
		autoSound him = Sound_create (my ny, 0.0, nx * my dx, nx, my dx, 0.5 * my dx);
		for (integer channel = 1; channel <= my ny; channel ++) {
			NUMvector_copyElements (my z [channel], his z [channel], 1, my nx);
			NUMvector_copyElements (thy z [channel], his z [channel] + my nx + nx_silence, 1, thy nx);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not appended.");
	}
}

autoSound Sounds_concatenate (OrderedOf<structSound>& list, double overlapTime) {
	try {
		integer numberOfChannels = 0, nx = 0, numberOfSmoothingSamples;
		double dx = 0.0;
		for (integer i = 1; i <= list.size; i ++) {
			Sound sound = list.at [i];
			if (numberOfChannels == 0) {
				numberOfChannels = sound -> ny;
			} else if (sound -> ny != numberOfChannels) {
				Melder_throw (U"To concatenate sounds, their numbers of channels (mono, stereo) must be equal.");
			}
			if (dx == 0.0) {
				dx = sound -> dx;
			} else if (sound -> dx != dx) {
				Melder_throw (U"To concatenate sounds, their sampling frequencies must be equal.\n"
						U"You could resample one or more of the sounds before concatenating.");
			}
			nx += sound -> nx;
		}
		numberOfSmoothingSamples = Melder_iround (overlapTime / dx);
		autoSound thee = Sound_create (numberOfChannels, 0.0, nx * dx, nx, dx, 0.5 * dx);
		autonumvec smoother;
		if (numberOfSmoothingSamples > 0) {
			smoother = autonumvec (numberOfSmoothingSamples, kTensorInitializationType::RAW);
			double factor = NUMpi / numberOfSmoothingSamples;
			for (integer i = 1; i <= numberOfSmoothingSamples; i ++) {
				smoother [i] = 0.5 - 0.5 * cos (factor * (i - 0.5));
			}
		}
		nx = 0;
		for (integer i = 1; i <= list.size; i ++) {
			Sound sound = list.at [i];
			if (numberOfSmoothingSamples > 2 * sound -> nx)
				Melder_throw (U"At least one of the sounds is shorter than twice the overlap time.\nChoose a shorter overlap time.");
			bool thisIsTheFirstSound = ( i == 1 );
			bool thisIsTheLastSound = ( i == list.size );
			bool weNeedSmoothingAtTheStartOfThisSound = ! thisIsTheFirstSound;
			bool weNeedSmoothingAtTheEndOfThisSound = ! thisIsTheLastSound;
			integer numberOfSmoothingSamplesAtTheStartOfThisSound = weNeedSmoothingAtTheStartOfThisSound ? numberOfSmoothingSamples : 0;
			integer numberOfSmoothingSamplesAtTheEndOfThisSound = weNeedSmoothingAtTheEndOfThisSound ? numberOfSmoothingSamples : 0;
			for (integer channel = 1; channel <= numberOfChannels; channel ++) {
				for (integer j = 1, mySample = 1, thySample = mySample + nx;
					 j <= numberOfSmoothingSamplesAtTheStartOfThisSound;
					 j ++, mySample ++, thySample ++)
				{
					thy z [channel] [thySample] += sound -> z [channel] [mySample] * smoother [j];   // add
				}
				NUMvector_copyElements (sound -> z [channel], thy z [channel] + nx,
					1 + numberOfSmoothingSamplesAtTheStartOfThisSound, sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound);
				for (integer j = 1, mySample = sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound + 1, thySample = mySample + nx;
					 j <= numberOfSmoothingSamplesAtTheEndOfThisSound;
					 j ++, mySample ++, thySample ++)
				{
					thy z [channel] [thySample] = sound -> z [channel] [mySample] * smoother [numberOfSmoothingSamplesAtTheEndOfThisSound + 1 - j];   // replace (or add, which is the same since it's all zeroes to start with)
				}
			}
			nx += sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound;
		}
		thy nx -= numberOfSmoothingSamples * (list.size - 1);
		Melder_assert (thy nx == nx);
		thy xmax = thy nx * dx;
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sounds not concatenated.");
	}
}

autoSound Sounds_convolve (Sound me, Sound thee, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		if (my ny > 1 && thy ny > 1 && my ny != thy ny)
			Melder_throw (U"The numbers of channels of the two sounds have to be equal or 1.");
		if (my dx != thy dx)
			Melder_throw (U"The sampling frequencies of the two sounds have to be equal.");
		integer n1 = my nx, n2 = thy nx;
		integer n3 = n1 + n2 - 1, nfft = 1;
		while (nfft < n3) nfft *= 2;
		autonumvec data1 (nfft, kTensorInitializationType::RAW);
		autonumvec data2 (nfft, kTensorInitializationType::RAW);
		integer numberOfChannels = my ny > thy ny ? my ny : thy ny;
		autoSound him = Sound_create (numberOfChannels, my xmin + thy xmin, my xmax + thy xmax, n3, my dx, my x1 + thy x1);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = my z [my ny == 1 ? 1 : channel];
			for (integer i = n1; i > 0; i --) data1 [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++) data1 [i] = 0.0;
			a = thy z [thy ny == 1 ? 1 : channel];
			for (integer i = n2; i > 0; i --) data2 [i] = a [i];
			for (integer i = n2 + 1; i <= nfft; i ++) data2 [i] = 0.0;
			NUMrealft (data1.at, nfft, 1);
			NUMrealft (data2.at, nfft, 1);
			data2 [1] *= data1 [1];
			data2 [2] *= data1 [2];
			for (integer i = 3; i <= nfft; i += 2) {
				double temp = data1 [i] * data2 [i] - data1 [i + 1] * data2 [i + 1];
				data2 [i + 1] = data1 [i] * data2 [i + 1] + data1 [i + 1] * data2 [i];
				data2 [i] = temp;
			}
			NUMrealft (data2.at, nfft, -1);
			a = him -> z [channel];
			for (integer i = 1; i <= n3; i ++) {
				a [i] = data2 [i];
			}
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double *a = his z [channel];
					double edge = n1 < n2 ? n1 : n2;
					for (integer i = 1; i < edge; i ++) {
						double factor = edge / i;
						a [i] *= factor;
						a [n3 + 1 - i] *= factor;
					}
				}
			} break;
			//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
				// do nothing
			//} break;
			default: Melder_fatal (U"Sounds_convolve: unimplemented outside-time-domain strategy ", (int) signalOutsideTimeDomain);
		}
		switch (scaling) {
			case kSounds_convolve_scaling::INTEGRAL: {
				Vector_multiplyByScalar (him.get(), my dx / nfft);
			} break;
			case kSounds_convolve_scaling::SUM: {
				Vector_multiplyByScalar (him.get(), 1.0 / nfft);
			} break;
			case kSounds_convolve_scaling::NORMALIZE: {
				double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (thee);
				if (normalizationFactor != 0.0) {
					Vector_multiplyByScalar (him.get(), 1.0 / nfft / normalizationFactor);
				}
			} break;
			case kSounds_convolve_scaling::PEAK_099: {
				Vector_scale (him.get(), 0.99);
			} break;
			default: Melder_fatal (U"Sounds_convolve: unimplemented scaling ", (int) scaling);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not convolved.");
	}
}

autoSound Sounds_crossCorrelate (Sound me, Sound thee, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		if (my ny > 1 && thy ny > 1 && my ny != thy ny)
			Melder_throw (U"The numbers of channels of the two sounds have to be equal or 1.");
		if (my dx != thy dx)
			Melder_throw (U"The sampling frequencies of the two sounds have to be equal.");
		integer numberOfChannels = my ny > thy ny ? my ny : thy ny;
		integer n1 = my nx, n2 = thy nx;
		integer n3 = n1 + n2 - 1, nfft = 1;
		while (nfft < n3) nfft *= 2;
		autonumvec data1 (nfft, kTensorInitializationType::RAW);
		autonumvec data2 (nfft, kTensorInitializationType::RAW);
		double my_xlast = my x1 + (n1 - 1) * my dx;
		autoSound him = Sound_create (numberOfChannels, thy xmin - my xmax, thy xmax - my xmin, n3, my dx, thy x1 - my_xlast);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = my z [my ny == 1 ? 1 : channel];
			for (integer i = n1; i > 0; i --) data1 [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++) data1 [i] = 0.0;
			a = thy z [thy ny == 1 ? 1 : channel];
			for (integer i = n2; i > 0; i --) data2 [i] = a [i];
			for (integer i = n2 + 1; i <= nfft; i ++) data2 [i] = 0.0;
			NUMrealft (data1.at, nfft, 1);
			NUMrealft (data2.at, nfft, 1);
			data2 [1] *= data1 [1];
			data2 [2] *= data1 [2];
			for (integer i = 3; i <= nfft; i += 2) {
				double temp = data1 [i] * data2 [i] + data1 [i + 1] * data2 [i + 1];   // reverse me by taking the conjugate of data1
				data2 [i + 1] = data1 [i] * data2 [i + 1] - data1 [i + 1] * data2 [i];   // reverse me by taking the conjugate of data1
				data2 [i] = temp;
			}
			NUMrealft (data2.at, nfft, -1);
			a = him -> z [channel];
			for (integer i = 1; i < n1; i ++) {
				a [i] = data2 [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data2
			}
			for (integer i = 1; i <= n2; i ++) {
				a [i + (n1 - 1)] = data2 [i];   // data for the second part ("positive lags") is at the beginning of data2
			}
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double *a = his z [channel];
					double edge = n1 < n2 ? n1 : n2;
					for (integer i = 1; i < edge; i ++) {
						double factor = edge / i;
						a [i] *= factor;
						a [n3 + 1 - i] *= factor;
					}
				}
			} break;
			//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
				// do nothing
			//} break;
			default: Melder_fatal (U"Sounds_crossCorrelate: unimplemented outside-time-domain strategy ", (int) signalOutsideTimeDomain);
		}
		switch (scaling) {
			case kSounds_convolve_scaling::INTEGRAL: {
				Vector_multiplyByScalar (him.get(), my dx / nfft);
			} break;
			case kSounds_convolve_scaling::SUM: {
				Vector_multiplyByScalar (him.get(), 1.0 / nfft);
			} break;
			case kSounds_convolve_scaling::NORMALIZE: {
				double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (thee);
				if (normalizationFactor != 0.0) {
					Vector_multiplyByScalar (him.get(), 1.0 / nfft / normalizationFactor);
				}
			} break;
			case kSounds_convolve_scaling::PEAK_099: {
				Vector_scale (him.get(), 0.99);
			} break;
			default: Melder_fatal (U"Sounds_crossCorrelate: unimplemented scaling ", (int) scaling);
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not cross-correlated.");
	}
}

autoSound Sound_autoCorrelate (Sound me, kSounds_convolve_scaling scaling, kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain) {
	try {
		integer numberOfChannels = my ny, n1 = my nx, n2 = n1 + n1 - 1, nfft = 1;
		while (nfft < n2) nfft *= 2;
		autonumvec data (nfft, kTensorInitializationType::RAW);
		double my_xlast = my x1 + (n1 - 1) * my dx;
		autoSound thee = Sound_create (numberOfChannels, my xmin - my xmax, my xmax - my xmin, n2, my dx, my x1 - my_xlast);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = my z [channel];
			for (integer i = n1; i > 0; i --) data [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++) data [i] = 0.0;
			NUMrealft (data.at, nfft, 1);
			data [1] *= data [1];
			data [2] *= data [2];
			for (integer i = 3; i <= nfft; i += 2) {
				data [i] = data [i] * data [i] + data [i + 1] * data [i + 1];
				data [i + 1] = 0.0;   // reverse me by taking the conjugate of data1
			}
			NUMrealft (data.at, nfft, -1);
			a = thy z [channel];
			for (integer i = 1; i < n1; i ++) {
				a [i] = data [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data
			}
			for (integer i = 1; i <= n1; i ++) {
				a [i + (n1 - 1)] = data [i];   // data for the second part ("positive lags") is at the beginning of data
			}
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double *a = thy z [channel];
					double edge = n1;
					for (integer i = 1; i < edge; i ++) {
						double factor = edge / i;
						a [i] *= factor;
						a [n2 + 1 - i] *= factor;
					}
				}
			} break;
			//case kSounds_convolve_signalOutsideTimeDomain_PERIODIC: {
				// do nothing
			//} break;
			default: Melder_fatal (U"Sounds_autoCorrelate: unimplemented outside-time-domain strategy ", (int) signalOutsideTimeDomain);
		}
		switch (scaling) {
			case kSounds_convolve_scaling::INTEGRAL: {
				Vector_multiplyByScalar (thee.get(), my dx / nfft);
			} break;
			case kSounds_convolve_scaling::SUM: {
				Vector_multiplyByScalar (thee.get(), 1.0 / nfft);
			} break;
			case kSounds_convolve_scaling::NORMALIZE: {
				double normalizationFactor = Matrix_getNorm (me) * Matrix_getNorm (me);
				if (normalizationFactor != 0.0) {
					Vector_multiplyByScalar (thee.get(), 1.0 / nfft / normalizationFactor);
				}
			} break;
			case kSounds_convolve_scaling::PEAK_099: {
				Vector_scale (thee.get(), 0.99);
			} break;
			default: Melder_fatal (U"Sounds_autoCorrelate: unimplemented scaling ", (int) scaling);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": autocorrelation not computed.");
	}
}

void Sound_draw (Sound me, Graphics g,
	double tmin, double tmax, double minimum, double maximum, bool garnish, const char32 *method)
{
	bool treversed = tmin > tmax;
	if (treversed) { double temp = tmin; tmin = tmax; tmax = temp; }
	/*
		Automatic domain.
	*/
	if (tmin == tmax) {
		tmin = my xmin;
		tmax = my xmax;
	}
	/*
		Domain expressed in sample numbers.
	*/
	integer ixmin, ixmax;
	Matrix_getWindowSamplesX (me, tmin, tmax, & ixmin, & ixmax);
	/*
		Automatic vertical range.
	*/
	if (minimum == maximum) {
		Matrix_getWindowExtrema (me, ixmin, ixmax, 1, my ny, & minimum, & maximum);
		if (minimum == maximum) {
			minimum -= 1.0;
			maximum += 1.0;
		}
	}
	/*
		Set coordinates for drawing.
	*/
	Graphics_setInner (g);
	for (integer channel = 1; channel <= my ny; channel ++) {
		Graphics_setWindow (g, treversed ? tmax : tmin, treversed ? tmin : tmax,
			minimum - (my ny - channel) * (maximum - minimum),
			maximum + (channel - 1) * (maximum - minimum));
		if (str32str (method, U"bars") || str32str (method, U"Bars")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
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
		} else if (str32str (method, U"poles") || str32str (method, U"Poles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				double x = Sampled_indexToX (me, ix);
				Graphics_line (g, x, 0, x, my z [channel] [ix]);
			}
		} else if (str32str (method, U"speckles") || str32str (method, U"Speckles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				double x = Sampled_indexToX (me, ix);
				Graphics_speckle (g, x, my z [channel] [ix]);
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
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_setWindow (g, tmin, tmax, minimum - (my ny - 1) * (maximum - minimum), maximum);
		Graphics_markLeft (g, minimum, true, true, false, nullptr);
		Graphics_markLeft (g, maximum, true, true, false, nullptr);
		if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		}
		if (my ny == 2) {
			Graphics_setWindow (g, treversed ? tmax : tmin, treversed ? tmin : tmax, minimum, maximum + (my ny - 1) * (maximum - minimum));
			Graphics_markRight (g, minimum, true, true, false, nullptr);
			Graphics_markRight (g, maximum, true, true, false, nullptr);
			if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0)) {
				Graphics_markRight (g, 0.0, true, true, true, nullptr);
			}
		}
	}
}

static double interpolate (Sound me, integer i1, integer channel)
/* Precondition: my z [1] [i1] != my z [1] [i1 + 1]; */
{
	integer i2 = i1 + 1;
	double x1 = Sampled_indexToX (me, i1), x2 = Sampled_indexToX (me, i2);
	double y1 = my z [channel] [i1], y2 = my z [channel] [i2];
	return x1 + (x2 - x1) * y1 / (y1 - y2);   // linear
}
double Sound_getNearestZeroCrossing (Sound me, double position, integer channel) {
	double *amplitude = my z [channel];
	integer leftSample = Sampled_xToLowIndex (me, position);
	integer rightSample = leftSample + 1, ileft, iright;
	double leftZero, rightZero;
	/* Are we already at a zero crossing? */
	if (leftSample >= 1 && rightSample <= my nx &&
		(amplitude [leftSample] >= 0.0) !=
		(amplitude [rightSample] >= 0.0))
	{
		return interpolate (me, leftSample, channel);
	}
	/* Search to the left. */
	if (leftSample > my nx) return undefined;
	for (ileft = leftSample - 1; ileft >= 1; ileft --)
		if ((amplitude [ileft] >= 0.0) != (amplitude [ileft + 1] >= 0.0))
		{
			leftZero = interpolate (me, ileft, channel);
			break;
		}
	/* Search to the right. */
	if (rightSample < 1) return undefined;
	for (iright = rightSample + 1; iright <= my nx; iright ++)
		if ((amplitude [iright] >= 0.0) != (amplitude [iright - 1] >= 0.0))
		{
			rightZero = interpolate (me, iright - 1, channel);
			break;
		}
	if (ileft < 1 && iright > my nx) return undefined;
	return ileft < 1 ? rightZero : iright > my nx ? leftZero :
		position - leftZero < rightZero - position ? leftZero : rightZero;
}

void Sound_setZero (Sound me, double tmin_in, double tmax_in, bool roundTimesToNearestZeroCrossing) {
	Function_unidirectionalAutowindow (me, & tmin_in, & tmax_in);
	Function_intersectRangeWithDomain (me, & tmin_in, & tmax_in);
	for (integer channel = 1; channel <= my ny; channel ++) {
		double tmin = tmin_in, tmax = tmax_in;
		if (roundTimesToNearestZeroCrossing) {
			if (tmin > my xmin) tmin = Sound_getNearestZeroCrossing (me, tmin_in, channel);
			if (tmax < my xmax) tmax = Sound_getNearestZeroCrossing (me, tmax_in, channel);
		}
		if (isundef (tmin)) tmin = my xmin;
		if (isundef (tmax)) tmax = my xmax;
		integer imin, imax;
		Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		for (integer i = imin; i <= imax; i ++) {
			my z [channel] [i] = 0.0;
		}
	}
}

autoSound Sound_createAsPureTone (integer numberOfChannels, double startingTime, double endTime,
	double sampleRate, double frequency, double amplitude, double fadeInDuration, double fadeOutDuration)
{
	try {
		double numberOfSamples_f = round ((endTime - startingTime) * sampleRate);
		if (numberOfSamples_f > (double) INT32_MAX)
			Melder_throw (U"Cannot create sounds with more than ", Melder_bigInteger (INT32_MAX), U" samples, because they cannot be saved to disk.");
		autoSound me = Sound_create (numberOfChannels, startingTime, endTime, (integer) numberOfSamples_f,
			1.0 / sampleRate, startingTime + 0.5 / sampleRate);
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			double time = my x1 + (isamp - 1) * my dx;
			double value = amplitude * sin (NUM2pi * frequency * time);
			double timeFromStart = time - startingTime;
			if (timeFromStart < fadeInDuration)
				value *= 0.5 - 0.5 * cos (NUMpi * timeFromStart / fadeInDuration);
			double timeFromEnd = endTime - time;
			if (timeFromEnd < fadeOutDuration)
				value *= 0.5 - 0.5 * cos (NUMpi * timeFromEnd / fadeOutDuration);
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				my z [ichan] [isamp] = value;
			}
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created from tone complex.");
	}
}

autoSound Sound_createAsToneComplex (double startTime, double endTime, double samplingFrequency,
	int phase, double frequencyStep, double firstFrequency, double ceiling, integer numberOfComponents)
{
	try {
		if (frequencyStep == 0.0)
			Melder_throw (U"The frequency step should not be zero.");
		/*
		 * Translate default firstFrequency.
		 */
		if (firstFrequency <= 0.0) firstFrequency = frequencyStep;
		double firstOmega = 2 * NUMpi * firstFrequency;
		/*
		 * Translate default ceiling.
		 */
		double omegaStep = 2 * NUMpi * frequencyStep, nyquistFrequency = 0.5 * samplingFrequency;
		if (ceiling <= 0.0 || ceiling > nyquistFrequency) ceiling = nyquistFrequency;
		/*
		 * Translate number of components.
		 */
		integer maximumNumberOfComponents = Melder_ifloor ((ceiling - firstFrequency) / frequencyStep) + 1;
		if (numberOfComponents <= 0 || numberOfComponents > maximumNumberOfComponents)
			numberOfComponents = maximumNumberOfComponents;
		if (numberOfComponents < 1)
			Melder_throw (U"There would be zero sine waves.");
		/*
		 * Generate the Sound.
		 */
		double factor = 0.99 / numberOfComponents;
		autoSound me = Sound_create (1, startTime, endTime, Melder_iround ((endTime - startTime) * samplingFrequency),
			1.0 / samplingFrequency, startTime + 0.5 / samplingFrequency);
		double *amplitude = my z [1];
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			double value = 0.0, t = Sampled_indexToX (me.get(), isamp);
			double omegaStepT = omegaStep * t, firstOmegaT = firstOmega * t;
			if (phase == Sound_TONE_COMPLEX_SINE) {
				for (integer icomp = 1; icomp <= numberOfComponents; icomp ++)
					value += sin (firstOmegaT + (icomp - 1) * omegaStepT);
			} else {
				for (integer icomp = 1; icomp <= numberOfComponents; icomp ++)
					value += cos (firstOmegaT + (icomp - 1) * omegaStepT);
			}
			amplitude [isamp] = value * factor;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created as tone complex.");
	}
}

void Sound_multiplyByWindow (Sound me, kSound_windowShape windowShape) {
	for (integer channel = 1; channel <= my ny; channel ++) {
		integer n = my nx;
		double *amp = my z [channel];
		switch (windowShape) {
			case kSound_windowShape::RECTANGULAR: {
				;
			} break; case kSound_windowShape::TRIANGULAR: {   // "Bartlett"
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;   // 0..1
					amp [i] *= 1.0 - fabs ((2.0 * phase - 1.0)); }
			} break; case kSound_windowShape::PARABOLIC: {   // "Welch"
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0); }
			} break; case kSound_windowShape::HANNING: {
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 0.5 * (1.0 - cos (2.0 * NUMpi * phase)); }
			} break; case kSound_windowShape::HAMMING: {
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					amp [i] *= 0.54 - 0.46 * cos (2.0 * NUMpi * phase); }
			} break; case kSound_windowShape::GAUSSIAN_1: {
				real imid = 0.5 * (n + 1), edge = exp (-3.0), onebyedge1 = 1.0 / (1.0 - edge);   // -0.5..+0.5
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-12.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_2: {
				real imid = 0.5 * (double) (n + 1), edge = exp (-12.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-48.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_3: {
				real imid = 0.5 * (double) (n + 1), edge = exp (-27.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-108.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_4: {
				real imid = 0.5 * (double) (n + 1), edge = exp (-48.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-192.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_5: {
				real imid = 0.5 * (double) (n + 1), edge = exp (-75.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					amp [i] *= (exp (-300.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::KAISER_1: {
				real imid = 0.5 * (double) (n + 1);
				real factor = 1.0 / NUMbessel_i0_f (2 * NUMpi);
				for (integer i = 1; i <= n; i ++) { double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
					double root = 1.0 - phase * phase;
					amp [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2.0 * NUMpi * sqrt (root)); }
			} break; case kSound_windowShape::KAISER_2: {
				real imid = 0.5 * (double) (n + 1);
				real factor = 1.0 / NUMbessel_i0_f (2 * NUMpi * NUMpi + 0.5);
				for (integer i = 1; i <= n; i ++) { double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
					double root = 1.0 - phase * phase;
					amp [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * sqrt (root)); }
			} break; default: {
			}
		}
	}
}

void Sound_scaleIntensity (Sound me, double newAverageIntensity) {
	double currentIntensity = Sound_getIntensity_dB (me), factor;
	if (isundef (currentIntensity)) return;
	factor = pow (10, (newAverageIntensity - currentIntensity) / 20.0);
	for (integer channel = 1; channel <= my ny; channel ++) {
		for (integer i = 1; i <= my nx; i ++) {
			my z [channel] [i] *= factor;
		}
	}
}

void Sound_overrideSamplingFrequency (Sound me, double rate) {
	my dx = 1 / rate;
	my x1 = my xmin + 0.5 * my dx;
	my xmax = my xmin + my nx * my dx;
}

autoSound Sound_extractPart (Sound me, double t1, double t2, kSound_windowShape windowShape, double relativeWidth, bool preserveTimes) {
	try {
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
		integer ix1 = 1 + Melder_iceiling ((t1 - my x1) / my dx);
		integer ix2 = 1 + Melder_ifloor   ((t2 - my x1) / my dx);
		if (ix2 < ix1) Melder_throw (U"Extracted Sound would contain no samples.");
		/*
		 * Create sound, optionally shifted to [0..t2-t1].
		 */
		autoSound thee = Sound_create (my ny, t1, t2, ix2 - ix1 + 1, my dx, my x1 + (ix1 - 1) * my dx);
		if (! preserveTimes) { thy xmin = 0.0; thy xmax -= t1; thy x1 -= t1; }
		/*
		 * Copy only *real* samples into the new sound.
		 * The *virtual* samples will remain at zero.
		 */
		for (integer channel = 1; channel <= my ny; channel ++) {
			NUMvector_copyElements (my z [channel], thy z [channel] + 1 - ix1,
					( ix1 < 1 ? 1 : ix1 ), ( ix2 > my nx ? my nx : ix2 ));
		}
		/*
		 * Multiply by a window that extends throughout the target domain.
		 */
		Sound_multiplyByWindow (thee.get(), windowShape);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

autoSound Sound_extractPartForOverlap (Sound me, double t1, double t2, double overlap) {
	try {
		if (t1 == t2) { t1 = my xmin; t2 = my xmax; };   // autowindow
		if (overlap > 0.0) {
			double margin = 0.5 * overlap;
			t1 -= margin;
			t2 += margin;
		}
		if (t1 < my xmin) t1 = my xmin;   // clip to my time domain
		if (t2 > my xmax) t2 = my xmax;
		/*
		 * Determine index range. We use all the real or virtual samples that fit within [t1..t2].
		 */
		integer ix1 = 1 + Melder_iceiling ((t1 - my x1) / my dx);
		integer ix2 = 1 + Melder_ifloor   ((t2 - my x1) / my dx);
		if (ix2 < ix1) Melder_throw (U"Extracted Sound would contain no samples.");
		/*
		 * Create sound.
		 */
		autoSound thee = Sound_create (my ny, t1, t2, ix2 - ix1 + 1, my dx, my x1 + (ix1 - 1) * my dx);
		thy xmin = 0.0;
		thy xmax -= t1;
		thy x1 -= t1;
		/*
		 * Copy only *real* samples into the new sound.
		 * The *virtual* samples will remain at zero.
		 */
		for (integer channel = 1; channel <= my ny; channel ++) {
			NUMvector_copyElements (my z [channel], thy z [channel] + 1 - ix1,
					( ix1 < 1 ? 1 : ix1 ), ( ix2 > my nx ? my nx : ix2 ));
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

void Sound_filterWithFormants (Sound me, double tmin, double tmax,
	int numberOfFormants, double formant [], double bandwidth [])
{
	try {
		for (integer channel = 1; channel <= my ny; channel ++) {
			if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // autowindowing
			integer itmin, itmax;
			integer n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax);
			if (n <= 2)
				Melder_throw (U"Sound too short.");
			double *amplitude = my z [channel] + itmin - 1;   // base 1
			NUMdeemphasize_f (amplitude, n, my dx, 50.0);
			for (int iformant = 1; iformant <= numberOfFormants; iformant ++) {
				NUMfilterSecondOrderSection_fb (amplitude, n, my dx, formant [iformant], bandwidth [iformant]);
			}
		}
		Matrix_scaleAbsoluteExtremum (me, 0.99);
	} catch (MelderError) {
		Melder_throw (me, U": not filtered.");
	}
}

autoSound Sound_filter_oneFormant (Sound me, double frequency, double bandwidth) {
	try {
		autoSound thee = Data_copy (me);
		Sound_filterWithOneFormantInplace (thee.get(), frequency, bandwidth);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (one formant).");
	}
}

void Sound_filterWithOneFormantInplace (Sound me, double frequency, double bandwidth) {
	for (integer channel = 1; channel <= my ny; channel ++) {
		NUMfilterSecondOrderSection_fb (my z [channel], my nx, my dx, frequency, bandwidth);
	}
	Matrix_scaleAbsoluteExtremum (me, 0.99);
}

autoSound Sound_filter_preemphasis (Sound me, double frequency) {
	try {
		autoSound thee = Data_copy (me);
		Sound_preEmphasis (thee.get(), frequency);
		Matrix_scaleAbsoluteExtremum (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (pre-emphasis).");
	}
}

autoSound Sound_filter_deemphasis (Sound me, double frequency) {
	try {
		autoSound thee = Data_copy (me);
		Sound_deEmphasis (thee.get(), frequency);
		Matrix_scaleAbsoluteExtremum (thee.get(), 0.99);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not filtered (de-emphasis).");
	}
}

void Sound_reverse (Sound me, double tmin, double tmax) {
	if (tmax <= tmin) { tmin = my xmin; tmax = my xmax; }   // autowindowing
	integer itmin, itmax;
	integer n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax) / 2;
	for (integer channel = 1; channel <= my ny; channel ++) {
		double *amp = my z [channel];
		for (integer i = 0; i < n; i ++) {
			double dummy = amp [itmin + i];
			amp [itmin + i] = amp [itmax - i];
			amp [itmax - i] = dummy;
		}
	}
}

autoSound Sounds_crossCorrelate_short (Sound me, Sound thee, double tmin, double tmax, bool normalize) {
	try {
		if (my dx != thy dx)
			Melder_throw (U"Sampling frequencies are not equal.");
		if (my ny != thy ny)
			Melder_throw (U"Numbers of channels are not equal.");
		double dt = my dx;
		double dphase = (thy x1 - my x1) / dt;
		dphase -= Melder_roundDown (dphase);   // a number between 0 and 1
		integer i1 = Melder_iceiling (tmin / dt - dphase);   // index of first sample if sample at dphase has index 0
		integer i2 = Melder_ifloor   (tmax / dt - dphase);   // index of last sample if sample at dphase has index 0
		integer nt = i2 - i1 + 1;
		if (nt < 1)
			Melder_throw (U"Window too small.");
		double t1 = (dphase + i1) * dt;
		autoSound him = Sound_create (1, tmin, tmax, nt, dt, t1);
		for (integer i = 1; i <= nt; i ++) {
			integer di = i - 1 + i1;
			for (integer ime = 1; ime <= my nx; ime ++) {
				if (ime + di < 1) continue;
				if (ime + di > thy nx) break;
				for (integer channel = 1; channel <= my ny; channel ++) {
					his z [1] [i] += my z [channel] [ime] * thy z [channel] [ime + di];
				}
			}
		}
		if (normalize) {
			double mypower = 0.0, thypower = 0.0;
			for (integer channel = 1; channel <= my ny; channel ++) {
				for (integer i = 1; i <= my nx; i ++) {
					double value = my z [channel] [i];
					mypower += value * value;
				}
				for (integer i = 1; i <= thy nx; i ++) {
					double value = thy z [channel] [i];
					thypower += value * value;
				}
			}
			if (mypower != 0.0 && thypower != 0.0) {
				double factor = 1.0 / (sqrt (mypower) * sqrt (thypower));
				for (integer i = 1; i <= nt; i ++) {
					his z [1] [i] *= factor;
				}
			}
		} else {
			double factor = dt / my ny;
			for (integer i = 1; i <= nt; i ++) {
				his z [1] [i] *= factor;
			}
		}
		return him;
	} catch (MelderError) {
		Melder_throw (me, U": not cross-correlated.");
	}
}

/* End of file Sound.cpp */
