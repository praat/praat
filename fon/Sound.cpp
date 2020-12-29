/* Sound.cpp
 *
 * Copyright (C) 1992-2020 Paul Boersma
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

#include "enums_getText.h"
#include "Sound_enums.h"
#include "enums_getValue.h"
#include "Sound_enums.h"

Thing_implement (Sound, Vector, 2);

autoSound Sound_clipboard;

void structSound :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of channels: ", our ny, our ny == 1 ? U" (mono)" : our ny == 2 ? U" (stereo)" : U"");
	MelderInfo_writeLine (U"Time domain:");
	MelderInfo_writeLine (U"   Start time: ", our xmin, U" seconds");
	MelderInfo_writeLine (U"   End time: ", our xmax, U" seconds");
	MelderInfo_writeLine (U"   Total duration: ", our xmax - our xmin, U" seconds");
	MelderInfo_writeLine (U"Time sampling:");
	MelderInfo_writeLine (U"   Number of samples: ", our nx);
	MelderInfo_writeLine (U"   Sampling period: ", our dx, U" seconds");
	MelderInfo_writeLine (U"   Sampling frequency: ", Melder_single (1.0 / our dx), U" Hz");
	MelderInfo_writeLine (U"   First sample centred at: ", our x1, U" seconds");
	integer numberOfCells = our nx * our ny;
	bool thereAreEnoughObservationsToComputeFirstOrderOverallStatistics = ( numberOfCells >= 1 );
	if (thereAreEnoughObservationsToComputeFirstOrderOverallStatistics) {
		double minimum_Pa = our z [1] [1], maximum_Pa = minimum_Pa;
		longdouble sum_Pa = 0.0, sumOfSquares_Pa2 = 0.0;
		for (integer channel = 1; channel <= our ny; channel ++) {
			constVEC const& waveform_Pa = our z [channel];
			for (integer i = 1; i <= our nx; i ++) {
				double value_Pa = waveform_Pa [i];
				sum_Pa += value_Pa;
				sumOfSquares_Pa2 += value_Pa * value_Pa;
				if (value_Pa < minimum_Pa) minimum_Pa = value_Pa;
				if (value_Pa > maximum_Pa) maximum_Pa = value_Pa;
			}
		}
		MelderInfo_writeLine (U"Amplitude:");
		MelderInfo_writeLine (U"   Minimum: ", Melder_single (minimum_Pa), U" Pascal");
		MelderInfo_writeLine (U"   Maximum: ", Melder_single (maximum_Pa), U" Pascal");
		double mean_Pa = (double) sum_Pa / numberOfCells;
		MelderInfo_writeLine (U"   Mean: ", Melder_single (mean_Pa), U" Pascal");
		double meanSquare_Pa2 = (double) sumOfSquares_Pa2 / numberOfCells;
		double rootMeanSquare_Pa = sqrt (meanSquare_Pa2);
		MelderInfo_writeLine (U"   Root-mean-square: ", Melder_single (rootMeanSquare_Pa), U" Pascal");
		double energy_Pa2s = (double) sumOfSquares_Pa2 * our dx / our ny;   // Pa2 s = kg2 m-2 s-3
		MelderInfo_write (U"Total energy: ", Melder_single (energy_Pa2s), U" Pascal\u00B2 sec");
		const double rho_c = 400.0;   // rho = 1.14 kg m-3; c = 353 m s-1; [rho c] = kg m-2 s-1
		double energy_J_m2 = energy_Pa2s / rho_c;   // kg s-2 = Joule m-2
		MelderInfo_writeLine (U" (energy in air: ", Melder_single (energy_J_m2), U" Joule/m\u00B2)");
		const double physicalDuration_s = our dx * our nx;
		double power_W_m2 = energy_J_m2 / physicalDuration_s;   // kg s-3 = Watt/m2
		MelderInfo_write (U"Mean power (intensity) in air: ", Melder_single (power_W_m2), U" Watt/m\u00B2");
		if (power_W_m2 != 0.0) {
			const double referencePower_W_m2 = 1.0e-12;   // this equals the square of 2.0e-5 Pa, divided by rho c
			double power_dB = 10.0 * log10 (power_W_m2 / referencePower_W_m2);
			MelderInfo_writeLine (U" = ", Melder_half (power_dB), U" dB");
		} else {
			MelderInfo_writeLine (U"");
		}
	}
	bool thereAreEnoughObservationsToComputeSecondOrderChannelStatistics = ( our nx >= 2 );
	if (thereAreEnoughObservationsToComputeSecondOrderChannelStatistics) {
		for (integer channel = 1; channel <= our ny; channel ++) {
			double stdev = NUMstdev (our z [channel]);
			MelderInfo_writeLine (U"Standard deviation in channel ", channel, U": ", Melder_single (stdev), U" Pascal");
		}
	}
}

double structSound :: v_getMatrix (integer irow, integer icol) {
	if (irow < 1 || irow > our ny) {
		if (irow == 0) {
			if (icol < 1 || icol > nx) return 0.0;
			if (our ny == 1) return our z [1] [icol];   // optimization
			if (our ny == 2) return 0.5 * (our z [1] [icol] + our z [2] [icol]);   // optimization
			longdouble sum = 0.0;
			for (integer channel = 1; channel <= ny; channel ++)
				sum += our z [channel] [icol];
			return (double) sum / our ny;
		}
		return 0.0;
	}
	if (icol < 1 || icol > nx) return 0.0;
	return our z [irow] [icol];
}

double structSound :: v_getFunction2 (double x, double y) {
	integer channel = Melder_ifloor (y);
	if (channel < 0 || channel > our ny || y != (double) channel) return 0.0;
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
	return Sound_create (numberOfChannels, 0.0, duration, (integer) (int32) numberOfSamples_f,
		1.0 / samplingFrequency, 0.5 / samplingFrequency);
}

autoSound Sound_convertToMono (Sound me) {
	if (my ny == 1) return Data_copy (me);   // optimization
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		if (my ny == 2) {   // optimization
			for (integer i = 1; i <= my nx; i ++)
				thy z [1] [i] = 0.5 * (my z [1] [i] + my z [2] [i]);
		} else {
			for (integer i = 1; i <= my nx; i ++) {
				longdouble sum = my z [1] [i] + my z [2] [i] + my z [3] [i];
				for (integer channel = 4; channel <= my ny; channel ++)
					sum += my z [channel] [i];
				thy z [1] [i] = double (sum / my ny);
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
		for (integer i = 1; i <= my nx; i ++)
			thy z [1] [i] = thy z [2] [i] = my z [1] [i];
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
				Melder_throw (U"To combine sounds, their sampling frequencies should be equal.\n"
						U"You could resample one or more of the sounds before combining.");
			}
		}
		Melder_assert (my size > 0);
		double sharedMinimumTime = my at [1] -> xmin;
		double sharedMaximumTime = my at [1] -> xmax;
		for (integer isound = 2; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			if (sound -> xmin < sharedMinimumTime)
				sharedMinimumTime = sound -> xmin;
			if (sound -> xmax > sharedMaximumTime)
				sharedMaximumTime = sound -> xmax;
		}
		autoINTVEC numberOfInitialZeroes = zero_INTVEC (my size);
		integer sharedNumberOfSamples = 0;
		double sumOfFirstTimes = 0.0;
		for (integer isound = 1; isound <= my size; isound ++) {
			Sound sound = my at [isound];
			numberOfInitialZeroes [isound] = Melder_ifloor ((sound -> xmin - sharedMinimumTime) / sharedSamplingPeriod);
			double newFirstTime = sound -> x1 - sound -> dx * numberOfInitialZeroes [isound];
			sumOfFirstTimes += newFirstTime;
			integer newNumberOfSamplesThroughLastNonzero = sound -> nx + numberOfInitialZeroes [isound];
			if (newNumberOfSamplesThroughLastNonzero > sharedNumberOfSamples)
				sharedNumberOfSamples = newNumberOfSamplesThroughLastNonzero;
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
				for (integer isamp = 1; isamp <= sound -> nx; isamp ++)
					thy z [channelNumber] [isamp + offset] = sound -> z [ichan] [isamp];
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Sounds not combined to stereo.");
	}
}

autoSound Sound_extractChannel (Sound me, integer channelNumber) {
	try {
		Melder_require (channelNumber >= 1 && channelNumber <= my ny,
			U"There is no channel ", channelNumber, U".");
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		thy z.row (1)  <<=  my z.row (channelNumber);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": channel ", channelNumber, U" not extracted.");
	}
}

autoSound Sound_extractChannels (Sound me, constVECVU const& channelNumbers) {
	try {
		integer numberOfChannels = channelNumbers.size;
		Melder_require (numberOfChannels > 0,
			U"The number of channels should be greater than 0.");
		autoSound you = Sound_create (numberOfChannels, my xmin, my xmax, my nx, my dx, my x1);
		for (integer ichan = 1; ichan <= numberOfChannels; ichan ++) {
			integer originalChannelNumber = Melder_iround (channelNumbers [ichan]);
			Melder_require (originalChannelNumber > 0,
				U"Your channel number is ", originalChannelNumber,
				U", but it should be positive."
			);
			Melder_require (originalChannelNumber <= my ny,
				U"Your channel number is ", originalChannelNumber,
				U", but it should not be greater than my number of channels, which is ",
				my ny, U"."
			);
			your z.row (ichan)  <<=  my z.row (originalChannelNumber);
		}
		return you;
	} catch (MelderError) {
		Melder_throw (me, U": channels not extracted.");
	}
}

static double getSumOfSquares (Sound me, double xmin, double xmax, integer *n) {
	Melder_assert (me);
	Function_unidirectionalAutowindow (me, & xmin, & xmax);
	integer imin, imax;
	*n = Sampled_getWindowSamples (me, xmin, xmax, & imin, & imax);
	if (*n <= 0)
		return undefined;
	longdouble sumOfSquares = 0.0;
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		constVECVU const& channel = my z.row (ichan);
		for (integer i = imin; i <= imax; i ++) {
			longdouble value = channel [i];
			sumOfSquares += value * value;
		}
	}
	return double (sumOfSquares);
}

double Sound_getRootMeanSquare (Sound me, double xmin, double xmax) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sumOfSquares) ? sqrt (sumOfSquares / (n * my ny)) : undefined;
}

double Sound_getEnergy (Sound me, double xmin, double xmax) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sumOfSquares) ? sumOfSquares * my dx / my ny : undefined;
}

double Sound_getPower (Sound me, double xmin, double xmax) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, xmin, xmax, & n);
	return isdefined (sumOfSquares) ? sumOfSquares / (n * my ny) : undefined;
}

double Sound_getEnergyInAir (Sound me) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, 0.0, 0.0, & n);
	return isdefined (sumOfSquares) ? sumOfSquares * my dx / (400.0 * my ny) : undefined;
}

double Sound_getIntensity_dB (Sound me) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, 0.0, 0.0, & n);
	return isdefined (sumOfSquares) && sumOfSquares != 0.0 ? 10.0 * log10 (sumOfSquares / (n * my ny) / 4.0e-10) : undefined;
}

double Sound_getPowerInAir (Sound me) {
	integer n;
	double sumOfSquares = getSumOfSquares (me, 0, 0, & n);
	return ( isdefined (sumOfSquares) ? sumOfSquares / (n * my ny) / 400.0 : undefined );
}

autoSound Matrix_to_Sound_mono (Matrix me, integer rowNumber) {
	try {
		autoSound thee = Sound_create (1, my xmin, my xmax, my nx, my dx, my x1);
		if (rowNumber < 0)
			rowNumber = my ny + 1 + rowNumber;
		Melder_clip (1_integer, & rowNumber, my ny);
		thy z.row (1)  <<=  my z.row (rowNumber);
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
		constexpr integer antiTurnAround = 1000;
		constexpr integer sampleRateFactor = 2;
		constexpr integer numberOfPaddingSides = 2;   // namely beginning and end
		integer nfft = 1;
		while (nfft < my nx + antiTurnAround * numberOfPaddingSides) nfft *= 2;
		const double newDx = my dx / sampleRateFactor;
		/*
			The computation of the new x1 relies on the idea that the left edge
			of the old first sample should coincide with the left edge of the new first sample
			(typically, e.g. if the old first sample starts at zero, which is usual,
			then the new first sample should also start at zero):
			old x1 - 0.5 * old dx == new x1 - 0.5 * new dx
			==>
			new x1 == old x1 - 0.5 * (old dx - new dx)
		*/
		autoSound thee = Sound_create (my ny, my xmin, my xmax, my nx * sampleRateFactor,
				newDx, my x1 - 0.5 * (my dx - newDx));
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			autoVEC data = zero_VEC (sampleRateFactor * nfft);   // zeroing is important...
			data.part (antiTurnAround + 1, antiTurnAround + my nx)  <<=  my z.row (ichan);   // ...because this fills only part of the sound
			NUMrealft (data.part (1, nfft), 1);
			integer imin = (integer) (nfft * 0.95);
			for (integer i = imin + 1; i <= nfft; i ++)
				data [i] *= ((double) (nfft - i)) / (nfft - imin);
			data [2] = 0.0;
			NUMrealft (data.get(), -1);
			double factor = 1.0 / nfft;
			for (integer i = 1; i <= thy nx; i ++)
				thy z [ichan] [i] = data [i + sampleRateFactor * antiTurnAround] * factor;
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": not upsampled.");
	}
}

autoSound Sound_resample (Sound me, double samplingFrequency, integer precision) {
	double upfactor = samplingFrequency * my dx;
	if (fabs (upfactor - 2.0) < 1e-6)
		return Sound_upsample (me);
	if (fabs (upfactor - 1.0) < 1e-6)
		return Data_copy (me);
	try {
		integer numberOfSamples = Melder_iround ((my xmax - my xmin) * samplingFrequency);
		if (numberOfSamples < 1)
			Melder_throw (U"The resampled Sound would have no samples.");
		autoSound filtered;
		bool weNeedAnAntiAliasingFilter = ( upfactor < 1.0 );
		if (weNeedAnAntiAliasingFilter) {
			constexpr integer antiTurnAround = 1000;
			constexpr integer numberOfPaddingSides = 2;   // namely beginning and end
			integer nfft = 1;
			while (nfft < my nx + antiTurnAround * numberOfPaddingSides) nfft *= 2;
			autoVEC data = raw_VEC (nfft);   // will be zeroed in every turn of the loop
			filtered = Sound_create (my ny, my xmin, my xmax, my nx, my dx, my x1);
			for (integer ichan = 1; ichan <= my ny; ichan ++) {
				for (integer i = 1; i <= nfft; i ++)
					data [i] = 0.0;
				data.part (antiTurnAround + 1, antiTurnAround + my nx)  <<=  my z.row (ichan);
				NUMrealft (data.get(), 1);   // go to the frequency domain
				for (integer i = Melder_ifloor (upfactor * nfft); i <= nfft; i ++)
					data [i] = 0.0;   // filter away high frequencies
				data [2] = 0.0;
				NUMrealft (data.get(), -1);   // return to the time domain
				double factor = 1.0 / nfft;
				VEC to = filtered -> z.row (ichan);
				for (integer i = 1; i <= my nx; i ++)
					to [i] = data [i + antiTurnAround] * factor;
			}
			me = filtered.get();   // reference copy; remove at end
		}
		autoSound thee = Sound_create (my ny, my xmin, my xmax, numberOfSamples, 1.0 / samplingFrequency,
				0.5 * (my xmin + my xmax - (numberOfSamples - 1) / samplingFrequency));
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			if (precision <= 1) {
				for (integer i = 1; i <= numberOfSamples; i ++) {
					double x = Sampled_indexToX (thee.get(), i);
					double index = Sampled_xToIndex (me, x);
					integer leftSample = Melder_ifloor (index);
					double fraction = index - leftSample;
					thy z [ichan] [i] = ( leftSample < 1 || leftSample >= my nx ? 0.0 :
							(1 - fraction) * my z [ichan] [leftSample] + fraction * my z [ichan] [leftSample + 1] );
				}
			} else {
				for (integer i = 1; i <= numberOfSamples; i ++) {
					double x = Sampled_indexToX (thee.get(), i);
					double index = Sampled_xToIndex (me, x);
					thy z [ichan] [i] = NUM_interpolate_sinc (my z.row (ichan), index, precision);
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
		his z.verticalBand (1, my nx)  <<=  my z.all();
		his z.verticalBand (my nx + nx_silence + 1, nx)  <<=  thy z.all();
		return him;
	} catch (MelderError) {
		Melder_throw (me, U" & ", thee, U": not appended.");
	}
}

autoSound Sounds_concatenate (OrderedOf<structSound>& list, double overlapTime) {
	try {
		integer sharedNumberOfChannels = 0, totalNumberOfSamples = 0;
		double sharedTimeStep = 0.0;
		for (integer i = 1; i <= list.size; i ++) {
			const Sound sound = list.at [i];
			if (sharedNumberOfChannels == 0) {
				sharedNumberOfChannels = sound -> ny;
			} else if (sound -> ny != sharedNumberOfChannels) {
				Melder_throw (U"To concatenate sounds, their numbers of channels (mono, stereo) should be equal.");
			}
			if (sharedTimeStep == 0.0) {
				sharedTimeStep = sound -> dx;
			} else if (sound -> dx != sharedTimeStep) {
				Melder_throw (U"To concatenate sounds, their sampling frequencies should be equal.\n"
						U"You could resample one or more of the sounds before concatenating.");
			}
			totalNumberOfSamples += sound -> nx;
		}
		const integer numberOfSmoothingSamples = Melder_iround (overlapTime / sharedTimeStep);
		totalNumberOfSamples -= numberOfSmoothingSamples * (list.size - 1);
		autoSound thee = Sound_create (sharedNumberOfChannels, 0.0, totalNumberOfSamples * sharedTimeStep,
				totalNumberOfSamples, sharedTimeStep, 0.5 * sharedTimeStep);
		autoVEC smoother;
		if (numberOfSmoothingSamples > 0) {
			smoother = raw_VEC (numberOfSmoothingSamples);
			const double factor = NUMpi / numberOfSmoothingSamples;
			for (integer i = 1; i <= numberOfSmoothingSamples; i ++)
				smoother [i] = 0.5 - 0.5 * cos (factor * (i - 0.5));
		}
		integer sampleOffset = 0;
		for (integer i = 1; i <= list.size; i ++) {
			const Sound sound = list.at [i];
			if (numberOfSmoothingSamples > 2 * sound -> nx)
				Melder_throw (U"At least one of the sounds is shorter than twice the overlap time.\nChoose a shorter overlap time.");
			const bool thisIsTheFirstSound = ( i == 1 );
			const bool thisIsTheLastSound = ( i == list.size );
			const bool weNeedSmoothingAtTheStartOfThisSound = ! thisIsTheFirstSound;
			const bool weNeedSmoothingAtTheEndOfThisSound = ! thisIsTheLastSound;
			const integer numberOfSmoothingSamplesAtTheStartOfThisSound = ( weNeedSmoothingAtTheStartOfThisSound ? numberOfSmoothingSamples : 0 );
			const integer numberOfSmoothingSamplesAtTheEndOfThisSound = ( weNeedSmoothingAtTheEndOfThisSound ? numberOfSmoothingSamples : 0 );
			for (integer channel = 1; channel <= sharedNumberOfChannels; channel ++) {
				for (integer j = 1, mySample = 1, thySample = mySample + sampleOffset;
					 j <= numberOfSmoothingSamplesAtTheStartOfThisSound;
					 j ++, mySample ++, thySample ++)
				{
					thy z [channel] [thySample] += sound -> z [channel] [mySample] * smoother [j];   // add
				}
				thy z.row (channel).part (sampleOffset + 1 + numberOfSmoothingSamplesAtTheStartOfThisSound,
						sampleOffset + sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound)  <<=
						sound -> z.row (channel).part (1 + numberOfSmoothingSamplesAtTheStartOfThisSound,
						sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound);
				for (integer j = 1, mySample = sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound + 1, thySample = mySample + sampleOffset;
					 j <= numberOfSmoothingSamplesAtTheEndOfThisSound;
					 j ++, mySample ++, thySample ++)
				{
					thy z [channel] [thySample] = sound -> z [channel] [mySample] * smoother [numberOfSmoothingSamplesAtTheEndOfThisSound + 1 - j];   // replace (or add, which is the same since it's all zeroes to start with)
				}
			}
			sampleOffset += sound -> nx - numberOfSmoothingSamplesAtTheEndOfThisSound;
		}
		Melder_assert (sampleOffset == totalNumberOfSamples);
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
		while (nfft < n3)
			nfft *= 2;
		autoVEC data1 = raw_VEC (nfft);
		autoVEC data2 = raw_VEC (nfft);
		integer numberOfChannels = std::max (my ny, thy ny);
		autoSound him = Sound_create (numberOfChannels, my xmin + thy xmin, my xmax + thy xmax, n3, my dx, my x1 + thy x1);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = & my z [my ny == 1 ? 1 : channel] [0];
			for (integer i = n1; i > 0; i --)
				data1 [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++)
				data1 [i] = 0.0;
			a = & thy z [thy ny == 1 ? 1 : channel] [0];
			for (integer i = n2; i > 0; i --)
				data2 [i] = a [i];
			for (integer i = n2 + 1; i <= nfft; i ++)
				data2 [i] = 0.0;
			NUMrealft (data1.get(), 1);
			NUMrealft (data2.get(), 1);
			data2 [1] *= data1 [1];
			data2 [2] *= data1 [2];
			for (integer i = 3; i <= nfft; i += 2) {
				double temp = data1 [i] * data2 [i] - data1 [i + 1] * data2 [i + 1];
				data2 [i + 1] = data1 [i] * data2 [i + 1] + data1 [i + 1] * data2 [i];
				data2 [i] = temp;
			}
			NUMrealft (data2.get(), -1);
			a = & him -> z [channel] [0];
			for (integer i = 1; i <= n3; i ++)
				a [i] = data2 [i];
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double * const a = & his z [channel] [0];
					double const edge = std::min (n1, n2);
					for (integer i = 1; i < edge; i ++) {
						double const factor = edge / i;
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
				if (normalizationFactor != 0.0)
					Vector_multiplyByScalar (him.get(), 1.0 / nfft / normalizationFactor);
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
		while (nfft < n3)
			nfft *= 2;
		autoVEC data1 = raw_VEC (nfft);
		autoVEC data2 = raw_VEC (nfft);
		double my_xlast = my x1 + (n1 - 1) * my dx;
		autoSound him = Sound_create (numberOfChannels, thy xmin - my xmax, thy xmax - my xmin, n3, my dx, thy x1 - my_xlast);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = & my z [my ny == 1 ? 1 : channel] [0];
			for (integer i = n1; i > 0; i --)
				data1 [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++)
				data1 [i] = 0.0;
			a = & thy z [thy ny == 1 ? 1 : channel] [0];
			for (integer i = n2; i > 0; i --)
				data2 [i] = a [i];
			for (integer i = n2 + 1; i <= nfft; i ++)
				data2 [i] = 0.0;
			NUMrealft (data1.get(), 1);
			NUMrealft (data2.get(), 1);
			data2 [1] *= data1 [1];
			data2 [2] *= data1 [2];
			for (integer i = 3; i <= nfft; i += 2) {
				double temp = data1 [i] * data2 [i] + data1 [i + 1] * data2 [i + 1];   // reverse me by taking the conjugate of data1
				data2 [i + 1] = data1 [i] * data2 [i + 1] - data1 [i + 1] * data2 [i];   // reverse me by taking the conjugate of data1
				data2 [i] = temp;
			}
			NUMrealft (data2.get(), -1);
			a = & him -> z [channel] [0];
			for (integer i = 1; i < n1; i ++)
				a [i] = data2 [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data2
			for (integer i = 1; i <= n2; i ++)
				a [i + (n1 - 1)] = data2 [i];   // data for the second part ("positive lags") is at the beginning of data2
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double * const a = & his z [channel] [0];
					double const edge = std::min (n1, n2);
					for (integer i = 1; i < edge; i ++) {
						const double factor = edge / i;
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
				if (normalizationFactor != 0.0)
					Vector_multiplyByScalar (him.get(), 1.0 / nfft / normalizationFactor);
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
		while (nfft < n2)
			nfft *= 2;
		autoVEC data = raw_VEC (nfft);
		double my_xlast = my x1 + (n1 - 1) * my dx;
		autoSound thee = Sound_create (numberOfChannels, my xmin - my xmax, my xmax - my xmin, n2, my dx, my x1 - my_xlast);
		for (integer channel = 1; channel <= numberOfChannels; channel ++) {
			double *a = & my z [channel] [0];
			for (integer i = n1; i > 0; i --)
				data [i] = a [i];
			for (integer i = n1 + 1; i <= nfft; i ++)
				data [i] = 0.0;
			NUMrealft (data.get(), 1);
			data [1] *= data [1];
			data [2] *= data [2];
			for (integer i = 3; i <= nfft; i += 2) {
				data [i] = data [i] * data [i] + data [i + 1] * data [i + 1];
				data [i + 1] = 0.0;   // reverse me by taking the conjugate of data1
			}
			NUMrealft (data.get(), -1);
			a = & thy z [channel] [0];
			for (integer i = 1; i < n1; i ++)
				a [i] = data [i + (nfft - (n1 - 1))];   // data for the first part ("negative lags") is at the end of data
			for (integer i = 1; i <= n1; i ++)
				a [i + (n1 - 1)] = data [i];   // data for the second part ("positive lags") is at the beginning of data
		}
		switch (signalOutsideTimeDomain) {
			case kSounds_convolve_signalOutsideTimeDomain::ZERO: {
				// do nothing
			} break;
			case kSounds_convolve_signalOutsideTimeDomain::SIMILAR: {
				for (integer channel = 1; channel <= numberOfChannels; channel ++) {
					double * const a = & thy z [channel] [0];
					const double edge = n1;
					for (integer i = 1; i < edge; i ++) {
						const double factor = edge / i;
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
				if (normalizationFactor != 0.0)
					Vector_multiplyByScalar (thee.get(), 1.0 / nfft / normalizationFactor);
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
	double tmin, double tmax, double minimum, double maximum, bool garnish, conststring32 method)
{
	const bool timesAreReversed = ( tmin > tmax );
	if (timesAreReversed) std::swap (tmin, tmax);
	Function_bidirectionalAutowindow (me, & tmin, & tmax);
	/*
		Domain expressed in sample numbers.
	*/
	integer ixmin, ixmax;
	integer n = Matrix_getWindowSamplesX (me, tmin, tmax, & ixmin, & ixmax);
	if (n < 1) return;
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
		Graphics_setWindow (g, timesAreReversed ? tmax : tmin, timesAreReversed ? tmin : tmax,
			minimum - (my ny - channel) * (maximum - minimum),
			maximum + (channel - 1) * (maximum - minimum)
		);
		if (str32str (method, U"bars") || str32str (method, U"Bars")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				const double x = Sampled_indexToX (me, ix);
				const double y = std::min (my z [channel] [ix], maximum);
				const double left = std::max (x - 0.5 * my dx, tmin);
				const double right = std::min (x + 0.5 * my dx, tmax);
				Graphics_line (g, left, y, right, y);
				Graphics_line (g, left, y, left, minimum);
				Graphics_line (g, right, y, right, minimum);
			}
		} else if (str32str (method, U"poles") || str32str (method, U"Poles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				const double x = Sampled_indexToX (me, ix);
				Graphics_line (g, x, 0, x, my z [channel] [ix]);
			}
		} else if (str32str (method, U"speckles") || str32str (method, U"Speckles")) {
			for (integer ix = ixmin; ix <= ixmax; ix ++) {
				const double x = Sampled_indexToX (me, ix);
				Graphics_speckle (g, x, my z [channel] [ix]);
			}
		} else {
			/*
				The default: draw as a curve.
			*/
			Graphics_function (g, & my z [channel] [0], ixmin, ixmax,
					Matrix_columnToX (me, ixmin), Matrix_columnToX (me, ixmax));
		}
	}
	Graphics_setWindow (g, timesAreReversed ? tmax : tmin, timesAreReversed ? tmin : tmax, minimum, maximum);
	if (garnish && my ny == 2)
		Graphics_line (g, tmin, 0.5 * (minimum + maximum), tmax, 0.5 * (minimum + maximum));
	Graphics_unsetInner (g);
	if (garnish) {
		Graphics_drawInnerBox (g);
		Graphics_textBottom (g, true, U"Time (s)");
		Graphics_marksBottom (g, 2, true, true, false);
		Graphics_setWindow (g, tmin, tmax, minimum - (my ny - 1) * (maximum - minimum), maximum);
		Graphics_markLeft (g, minimum, true, true, false, nullptr);
		Graphics_markLeft (g, maximum, true, true, false, nullptr);
		if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0))
			Graphics_markLeft (g, 0.0, true, true, true, nullptr);
		if (my ny == 2) {
			Graphics_setWindow (g, timesAreReversed ? tmax : tmin, timesAreReversed ? tmin : tmax,
					minimum, maximum + (my ny - 1) * (maximum - minimum));
			Graphics_markRight (g, minimum, true, true, false, nullptr);
			Graphics_markRight (g, maximum, true, true, false, nullptr);
			if (minimum != 0.0 && maximum != 0.0 && (minimum > 0.0) != (maximum > 0.0))
				Graphics_markRight (g, 0.0, true, true, true, nullptr);
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
	double *amplitude = & my z [channel] [0];
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
			if (tmin > my xmin)
				tmin = Sound_getNearestZeroCrossing (me, tmin_in, channel);
			if (tmax < my xmax)
				tmax = Sound_getNearestZeroCrossing (me, tmax_in, channel);
		}
		if (isundef (tmin))
			tmin = my xmin;
		if (isundef (tmax))
			tmax = my xmax;
		integer imin, imax;
		Sampled_getWindowSamples (me, tmin, tmax, & imin, & imax);
		for (integer i = imin; i <= imax; i ++)
			my z [channel] [i] = 0.0;
	}
}

autoSound Sound_createAsPureTone (integer numberOfChannels, double startingTime, double endTime,
	double sampleRate, double frequency, double amplitude, double fadeInDuration, double fadeOutDuration)
{
	try {
		Melder_require (numberOfChannels >= 1,
			U"The number of channels should be at least 1.");
		double numberOfSamples_f = round ((endTime - startingTime) * sampleRate);
		if (numberOfSamples_f > (double) INT32_MAX)
			Melder_throw (U"Cannot create sounds with more than ", Melder_bigInteger (INT32_MAX), U" samples, because they cannot be saved to disk.");
		autoSound me = Sound_create (numberOfChannels, startingTime, endTime, (integer) numberOfSamples_f,
			1.0 / sampleRate, startingTime + 0.5 / sampleRate);
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			double time = my x1 + (isamp - 1) * my dx;
			double value = amplitude * sin (2.0 * NUMpi * frequency * time);
			double timeFromStart = time - startingTime;
			if (timeFromStart < fadeInDuration)
				value *= 0.5 - 0.5 * cos (NUMpi * timeFromStart / fadeInDuration);
			double timeFromEnd = endTime - time;
			if (timeFromEnd < fadeOutDuration)
				value *= 0.5 - 0.5 * cos (NUMpi * timeFromEnd / fadeOutDuration);
			for (integer ichan = 1; ichan <= my ny; ichan ++)
				my z [ichan] [isamp] = value;
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
			Translate default firstFrequency.
		*/
		if (firstFrequency <= 0.0) firstFrequency = frequencyStep;
		double const firstOmega = 2 * NUMpi * firstFrequency;
		/*
			Translate default ceiling.
		*/
		double const omegaStep = 2 * NUMpi * frequencyStep;
		double const nyquistFrequency = 0.5 * samplingFrequency;
		if (ceiling <= 0.0 || ceiling > nyquistFrequency) ceiling = nyquistFrequency;
		/*
			Translate number of components.
		*/
		integer const maximumNumberOfComponents = Melder_ifloor ((ceiling - firstFrequency) / frequencyStep) + 1;
		if (numberOfComponents <= 0 || numberOfComponents > maximumNumberOfComponents)
			numberOfComponents = maximumNumberOfComponents;
		if (numberOfComponents < 1)
			Melder_throw (U"There would be zero sine waves.");
		/*
			Generate the Sound.
		*/
		double const factor = 0.99 / numberOfComponents;
		autoSound me = Sound_create (1, startTime, endTime, Melder_iround ((endTime - startTime) * samplingFrequency),
			1.0 / samplingFrequency, startTime + 0.5 / samplingFrequency);
		for (integer isamp = 1; isamp <= my nx; isamp ++) {
			double const t = Sampled_indexToX (me.get(), isamp);
			double const omegaStepT = omegaStep * t;
			double const firstOmegaT = firstOmega * t;
			longdouble value = 0.0;
			if (phase == Sound_TONE_COMPLEX_SINE) {
				for (integer icomp = 1; icomp <= numberOfComponents; icomp ++)
					value += sin (firstOmegaT + (icomp - 1) * omegaStepT);
			} else {
				for (integer icomp = 1; icomp <= numberOfComponents; icomp ++)
					value += cos (firstOmegaT + (icomp - 1) * omegaStepT);
			}
			my z [1] [isamp] = double (value) * factor;
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"Sound not created as tone complex.");
	}
}

void Sound_multiplyByWindow (Sound me, kSound_windowShape windowShape) {
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		integer n = my nx;
		VEC channel = my z.row (ichan);
		switch (windowShape) {
			case kSound_windowShape::RECTANGULAR: {
				;
			} break; case kSound_windowShape::TRIANGULAR: {   // "Bartlett"
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;   // 0..1
					channel [i] *= 1.0 - fabs ((2.0 * phase - 1.0)); }
			} break; case kSound_windowShape::PARABOLIC: {   // "Welch"
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					channel [i] *= 1.0 - (2.0 * phase - 1.0) * (2.0 * phase - 1.0); }
			} break; case kSound_windowShape::HANNING: {
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					channel [i] *= 0.5 * (1.0 - cos (2.0 * NUMpi * phase)); }
			} break; case kSound_windowShape::HAMMING: {
				for (integer i = 1; i <= n; i ++) { double phase = (double) i / n;
					channel [i] *= 0.54 - 0.46 * cos (2.0 * NUMpi * phase); }
			} break; case kSound_windowShape::GAUSSIAN_1: {
				double imid = 0.5 * (n + 1), edge = exp (-3.0), onebyedge1 = 1.0 / (1.0 - edge);   // -0.5..+0.5
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					channel [i] *= (exp (-12.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_2: {
				double imid = 0.5 * (double) (n + 1), edge = exp (-12.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					channel [i] *= (exp (-48.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_3: {
				double imid = 0.5 * (double) (n + 1), edge = exp (-27.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					channel [i] *= (exp (-108.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_4: {
				double imid = 0.5 * (double) (n + 1), edge = exp (-48.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					channel [i] *= (exp (-192.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::GAUSSIAN_5: {
				double imid = 0.5 * (double) (n + 1), edge = exp (-75.0), onebyedge1 = 1.0 / (1.0 - edge);
				for (integer i = 1; i <= n; i ++) { double phase = ((double) i - imid) / n;
					channel [i] *= (exp (-300.0 * phase * phase) - edge) * onebyedge1; }
			} break; case kSound_windowShape::KAISER_1: {
				double imid = 0.5 * (double) (n + 1);
				double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi);
				for (integer i = 1; i <= n; i ++) { double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
					double root = 1.0 - phase * phase;
					channel [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f (2.0 * NUMpi * sqrt (root)); }
			} break; case kSound_windowShape::KAISER_2: {
				double imid = 0.5 * (double) (n + 1);
				double factor = 1.0 / NUMbessel_i0_f (2 * NUMpi * NUMpi + 0.5);
				for (integer i = 1; i <= n; i ++) { double phase = 2.0 * ((double) i - imid) / n;   // -1..+1
					double root = 1.0 - phase * phase;
					channel [i] *= root <= 0.0 ? 0.0 : factor * NUMbessel_i0_f ((2.0 * NUMpi * NUMpi + 0.5) * sqrt (root)); }
			} break; default: {
			}
		}
	}
}

void Sound_scaleIntensity (Sound me, double newAverageIntensity) {
	const double currentIntensity = Sound_getIntensity_dB (me);
	if (isundef (currentIntensity)) return;
	const double factor = pow (10.0, (newAverageIntensity - currentIntensity) / 20.0);
	my z.all()  *=  factor;
}

void Sound_overrideSamplingFrequency (Sound me, double rate) {
	my dx = 1.0 / rate;
	my x1 = my xmin + 0.5 * my dx;
	my xmax = my xmin + my nx * my dx;
}

autoSound Sound_extractPart (Sound me, double tmin, double tmax, kSound_windowShape windowShape, double relativeWidth, bool preserveTimes) {
	try {
		/*
			We do not clip to the Sound's time domain.
			Any samples outside it are taken to be zero.
		*/

		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		/*
			Allow window tails outside specified domain.
		*/
		if (relativeWidth != 1.0) {
			const double margin = 0.5 * (relativeWidth - 1) * (tmax - tmin);
			tmin -= margin;
			tmax += margin;
		}
		/*
			Determine index range. We use all the real or virtual samples that fit within [t1..t2].
		*/
		const integer itmin = 1 + Melder_iceiling ((tmin - my x1) / my dx);
		const integer itmax = 1 + Melder_ifloor   ((tmax - my x1) / my dx);
		Melder_require (itmax >= itmin,
			U"Extracted Sound would contain no samples.");
		/*
			Create sound, optionally shifted to [0..t2-t1].
		*/
		autoSound thee = Sound_create (my ny, tmin, tmax, itmax - itmin + 1, my dx, my x1 + (itmin - 1) * my dx);
		if (! preserveTimes) {
			thy xmin = 0.0;
			thy xmax -= tmin;
			thy x1 -= tmin;
		}
		/*
			Copy only *real* samples into the new sound.
			The *virtual* samples will remain at zero.
		*/
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			const integer itmin_clipped = std::max (1_integer, itmin);
			const integer itmax_clipped = std::min (itmax, my nx);
			thy z.row (ichan). part (1 - itmin + itmin_clipped, 1 - itmin + itmax_clipped)
					<<=  my z.row (ichan). part (itmin_clipped, itmax_clipped);
		}
		/*
			Multiply by a window that extends throughout the target domain.
		*/
		Sound_multiplyByWindow (thee.get(), windowShape);
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": part not extracted.");
	}
}

autoSound Sound_extractPartForOverlap (Sound me, double tmin, double tmax, double overlap) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		if (overlap > 0.0) {
			double margin = 0.5 * overlap;
			tmin -= margin;
			tmax += margin;
		}
		if (tmin < my xmin) tmin = my xmin;   // clip to my time domain
		if (tmax > my xmax) tmax = my xmax;
		/*
			Determine index range. We use all the real or virtual samples that fit within [t1..t2].
		*/
		integer itmin = 1 + Melder_iceiling ((tmin - my x1) / my dx);
		integer itmax = 1 + Melder_ifloor   ((tmax - my x1) / my dx);
		Melder_require (itmax >= itmin,
			U"Extracted Sound would contain no samples.");
		/*
			Create sound.
		*/
		autoSound thee = Sound_create (my ny, tmin, tmax, itmax - itmin + 1, my dx, my x1 + (itmin - 1) * my dx);
		thy xmin = 0.0;
		thy xmax -= tmin;
		thy x1 -= tmin;
		/*
			Copy only *real* samples into the new sound.
			The *virtual* samples will remain at zero.
		*/
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			const integer itmin_clipped = std::max (1_integer, itmin);
			const integer itmax_clipped = std::min (itmax, my nx);
			thy z.row (ichan). part (1 - itmin + itmin_clipped, 1 - itmin + itmax_clipped)
					<<=  my z.row (ichan). part (itmin_clipped, itmax_clipped);
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
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		integer itmin, itmax;
		integer n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax);
		Melder_require (n >= 3,
			U"Sound too short.");
		for (integer ichan = 1; ichan <= my ny; ichan ++) {
			VECVU const& channel = my z.row (ichan);
			VECVU const& window = channel.part (itmin, itmax);
			VECdeemphasize_f_inplace (window, my dx, 50.0);
			for (int iformant = 1; iformant <= numberOfFormants; iformant ++)
				VECfilterSecondOrderSection_fb_inplace (window, my dx, formant [iformant], bandwidth [iformant]);
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
	for (integer ichan = 1; ichan <= my ny; ichan ++) {
		VEC channel = my z.row (ichan);
		VECfilterSecondOrderSection_fb_inplace (channel, my dx, frequency, bandwidth);
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
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	integer n = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax) / 2;
	for (integer channel = 1; channel <= my ny; channel ++) {
		double *amp = & my z [channel] [0];
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

Thing_implement (SoundList, Ordered, 0);

/* End of file Sound.cpp */
