/* Formant_extensions.cpp
 *
 * Copyright (C) 2012-2021 David Weenink
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

#include "DataModeler.h"
#include "Formant_extensions.h"
#include "LPC_and_Formant.h"
#include "NUM2.h"

#include "enums_getText.h"
#include "Formant_extensions_enums.h"
#include "enums_getValue.h"
#include "Formant_extensions_enums.h"

void Formant_formula (Formant me, double tmin, double tmax, integer formantmin, integer formantmax, Interpreter interpreter, conststring32 expression) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		const integer numberOfPossibleFormants = my maxnFormants;
		if (formantmax >= formantmin) {
			formantmin = 1;
			formantmax = numberOfPossibleFormants;
		}
		Melder_clipLeft (1_integer, & formantmin);
		Melder_clipRight (& formantmax, numberOfPossibleFormants);

		autoMatrix fb = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, 2 * numberOfPossibleFormants, 2 * numberOfPossibleFormants, 1.0, 1.0);
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			const integer numberOfFormants = std::min (frame -> numberOfFormants, numberOfPossibleFormants);
			for (integer iformant = 1; iformant <= numberOfFormants; iformant++)
				if (iformant <= frame -> numberOfFormants) {
					fb -> z [2 * iformant - 1] [iframe] = frame -> formant [iformant]. frequency;
					fb -> z [2 * iformant    ] [iframe] = frame -> formant [iformant]. bandwidth;
				}
		}
		/*
			Apply the formaula
		*/
		const double ymin = 2.0 * formantmin - 1.0, ymax = 2.0 * formantmax;
		Matrix_formula_part (fb.get(), tmin, tmax, ymin, ymax, expression, interpreter, nullptr);
		/*
			Put results back in Formant
		*/
		integer ixmin, ixmax, iymin, iymax;
		(void) Matrix_getWindowSamplesX (fb.get(), tmin, tmax, & ixmin, & ixmax);
		(void) Matrix_getWindowSamplesY (fb.get(), ymin, ymax, & iymin, & iymax);

		for (integer iframe = ixmin; iframe <= ixmax; iframe ++) {
			/*
				If some of the formant frequencies are set to zero => remove the formant
			*/
			const Formant_Frame frame = & my frames [iframe];
			const integer numberOfFormants = std::min (frame -> numberOfFormants, formantmax);
			integer iformantto = ( formantmin > 1 ? formantmin - 1 : 0 );
			for (integer iformant = formantmin; iformant <= numberOfFormants; iformant++) {
				const double frequency = fb -> z [2 * iformant - 1] [iframe];
				const double bandWidth = fb -> z [2 * iformant    ] [iframe];
				if (frequency > 0 && bandWidth > 0) {
					iformantto ++;
					frame -> formant [iformantto]. frequency = frequency;
					frame -> formant [iformantto]. bandwidth = bandWidth;
				} else
					frame -> formant [iformant]. frequency = frame -> formant [iformant]. bandwidth = 0.0;
			}
			/*
				Shift the (higher) formants down if necessary.
			*/
			for (integer iformant = formantmax + 1; iformant <= frame -> numberOfFormants; iformant ++) {
				const double frequency = fb -> z [2 * iformant - 1] [iframe];
				const double bandWidth = fb -> z [2 * iformant    ] [iframe];
				if (frequency > 0 && bandWidth > 0) {
					iformantto ++;
					frame -> formant [iformantto]. frequency = frequency;
					frame -> formant [iformantto]. bandwidth = bandWidth;
				} else
					frame -> formant [iformant]. frequency = frame -> formant [iformant]. bandwidth = 0.0;
			}
			frame -> numberOfFormants = iformantto;
		}
	} catch (MelderError) {
		Melder_throw (me, U": not filtered.");
	}
}

/*
	HTK parameter files have (almost) no signature to recognize them except their *.fb file extension. 
	We can only read them and hope for the best.
*/
autoFormant Formant_readFromHTKParameterFile (MelderFile file) {
	try {
		autofile f = Melder_fopen (file, "rb");
		const integer numberOfFrames = bingetinteger32BE (f);
		const integer samplePeriodTimes100ns = bingetinteger32BE (f);
		/*
			For the VTR HTK parameter formant files of Deng et al., this
			number is 10000. According to the HTK file specification this is in units
			of 100 ns which result in 10000 /(1000000000 /100) = 1 / 1000 = 1 ms.
			What does this number mean? Are they a factor 10 off if they used a sampling
			frequency of 10 kHz? 
		*/
		const integer frameSize = bingetinteger16BE (f);
		Melder_require (frameSize % 8 == 0, 
			U"The vector size in bytes needs to be divisible by 8.");
		const integer htkType = bingetinteger16BE (f);
		Melder_require (htkType == 9, // user defined type
			U"The HTK type ID (", htkType, U") of the file needs to be 9.");
		const integer numberOfFormants = frameSize / 8; // r32, formants + bandwidths
		const double tmin = 0.0, timeStep = 0.01;
		const double tmax = numberOfFrames * timeStep; // the best guess we can make
		autoFormant me = Formant_create (tmin, tmax, numberOfFrames, timeStep, timeStep, numberOfFormants);
		for (integer iframe = 1; iframe <= numberOfFrames; iframe ++) {
			const Formant_Frame ffi = & my frames [iframe];
			ffi -> formant = newvectorzero <structFormant_Formant> (numberOfFormants);
			ffi -> numberOfFormants = numberOfFormants;
			for (integer inum = 1; inum <= numberOfFormants; inum ++)
				ffi -> formant [inum].frequency = bingetr32 (f) * 1000.0;				
			for (integer inum = 1; inum <= numberOfFormants; inum ++)
				ffi -> formant [inum].bandwidth = bingetr32 (f) * 1000.0;
		}
		f.close (file);
		return me;
	} catch (MelderError) {
		Melder_throw (U"Formant not read from HTK parameter file ", MelderFile_messageName (file), U".");
	}
}

autoVEC Formant_listFormantSlope (Formant me, integer iformant, double tmin, double tmax, kSlopeCurve curveType) {
	integer itmin, itmax;
	Function_bidirectionalAutowindow (me, & tmin, & tmax);
	Melder_require (Function_intersectRangeWithDomain (me, & tmin, & tmax),
		U"The requested time range should be within the domain of the Formant.");
	integer numberOfParameters = 3, numberOfSlopeParameters = 7;
	kDataModelerFunction modelerFunctionType;
	if (curveType == kSlopeCurve::PARABOLIC)
		modelerFunctionType = kDataModelerFunction::POLYNOME;
	else if (curveType == kSlopeCurve::EXPONENTIAL_PLUS_CONSTANT)
		modelerFunctionType = kDataModelerFunction::EXPONENTIAL_PLUS_CONSTANT;
	else if (curveType == kSlopeCurve::SIGMOID_PLUS_CONSTANT) {
		modelerFunctionType = kDataModelerFunction::SIGMOID_PLUS_CONSTANT;
		numberOfParameters = 4;
		numberOfSlopeParameters = 8;
	}
	autoVEC fitResults = raw_VEC (numberOfSlopeParameters);
	fitResults.get()  <<=  undefined;
	const integer numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax);
	if (numberOfFrames < numberOfParameters)
		return fitResults;
	autoDataModeler dm = DataModeler_create (tmin, tmax, numberOfFrames, numberOfParameters, modelerFunctionType);
	integer numberOfDataPoints = 0;
	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		const integer numberOfFormants = frame -> numberOfFormants;
		const double frequency = frame -> formant [iformant]. frequency;
		const double bandwidth = frame -> formant [iformant]. bandwidth;
		if (iformant > numberOfFormants || ! isdefined (frequency) || ! isdefined (bandwidth))
			continue;
		dm -> data [++ numberOfDataPoints].x = Sampled_indexToX (me, iframe);
		dm -> data [numberOfDataPoints].y = frequency;
		dm -> data [numberOfDataPoints] .sigmaY = bandwidth;
		dm -> data [numberOfDataPoints] .status = kDataModelerData::VALID;
	}
	Melder_require (numberOfDataPoints >= numberOfParameters,
		U"Not enough valid data points.");
	if (numberOfDataPoints != numberOfFrames) {
		dm -> data.resize (numberOfDataPoints);
		dm -> numberOfDataPoints = numberOfDataPoints;
	}
	DataModeler_fit (dm.get());
	autoVEC parameters  = DataModeler_listParameterValues (dm.get());
	const double flocus = DataModeler_getModelValueAtX (dm.get(), tmin);
	const double ftarget = DataModeler_getModelValueAtX (dm.get(), tmax);
	const double rSquared = DataModeler_getCoefficientOfDetermination (dm.get(), nullptr, nullptr);
	const double averageSlope =  (ftarget - flocus) / (tmax - tmin);
	fitResults [1] = averageSlope;
	fitResults [2] = rSquared;
	fitResults [3] = flocus;
	fitResults [4] = ftarget;
	fitResults.part (5, 5 + numberOfParameters - 1)  <<=  parameters.get();
	return fitResults;
}

autoIntensityTier Formant_Spectrogram_to_IntensityTier (Formant me, Spectrogram thee, integer iformant) {
	try {
		Melder_require (my xmin == thy xmin && my xmax == thy xmax,
			U"The start and end times of the Formant and the Spectrogram should be equal.");
		Melder_require (iformant > 0 && iformant <= my maxnFormants,
			U"Formant number should be in the range [1, ", my maxnFormants, U"].");
		autoIntensityTier him = IntensityTier_create (my xmin, my xmax);
		double previousValue_dB = -80000.0; // can never occur
		double previousTime = my xmin;
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my frames [iframe];
			const integer numberOfFormants = frame -> numberOfFormants;
			const double time = Sampled_indexToX (me, iframe);
			double value = 0.0;
			if (iformant <= numberOfFormants) {
				const double f = frame -> formant [iformant]. frequency;
				value = Matrix_getValueAtXY (thee, time, f);
				value = isdefined (value) ? value : 0.0;
			}
			const double value_dB = 10.0 * log10 ((value + 1e-30) / 4.0e-10); /* dB / Hz */
			if (value_dB != previousValue_dB) {
				if (iframe > 1 && previousTime < time - 1.5 * my dx)   // mark the end of the same interval
					RealTier_addPoint (him.get(), time - my dx, previousValue_dB);
				RealTier_addPoint (him.get(), time, value_dB);
				previousTime = time;
			}
			previousValue_dB = value_dB;
		}
		return him;
	} catch (MelderError) {
		Melder_throw (U"IntensityTier not created from ", me, U" and ", thee, U".");
	}
}

autoFormant Formant_extractPart (Formant me, double tmin, double tmax) {
	try {
		Function_unidirectionalAutowindow (me, & tmin, & tmax);
		Melder_require (tmin < my xmax && tmax > my xmin,
			U"Your start and end time should be between ", my xmin, U" and ", my xmax, U".");
		integer ifmin, ifmax;
		const integer numberOfFrames = Sampled_getWindowSamples (me, tmin, tmax, & ifmin, & ifmax);
		const double t1 = Sampled_indexToX (me, ifmin);
		autoFormant thee = Formant_create (tmin, tmax, numberOfFrames, my dx, t1, my maxnFormants);
		for (integer iframe = ifmin; iframe <= ifmax; iframe ++) {
			const Formant_Frame myFrame = & my frames [iframe];
			const Formant_Frame thyFrame = & thy frames [iframe - ifmin + 1];
			myFrame -> copy (thyFrame);
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (U"Formant part could not be extracted.");
	}
}

/* End of file Formant_extensions.cpp */
