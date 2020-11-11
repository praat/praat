/* FormantPath.cpp
 *
 * Copyright (C) 2020 David Weenink
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

#include "FormantPath.h"
#include "FormantModeler.h"
#include "Graphics_extensions.h"
#include "LPC_and_Formant.h"
#include "Matrix.h"
#include "Sound_to_Formant.h"
#include "Sound_and_LPC.h"
#include "Sound.h"
#include "Sound_and_LPC_robust.h"

#include "oo_DESTROY.h"
#include "FormantPath_def.h"
#include "oo_COPY.h"
#include "FormantPath_def.h"
#include "oo_EQUAL.h"
#include "FormantPath_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantPath_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantPath_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantPath_def.h"
#include "oo_READ_TEXT.h"
#include "FormantPath_def.h"
#include "oo_READ_BINARY.h"
#include "FormantPath_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantPath_def.h"

void structFormantPath :: v_info () {
	structDaata :: v_info ();
	MelderInfo_writeLine (U"Number of Formant objects: ", formants . size);
	for (integer ic = 1; ic <= ceilings.size; ic ++)
		MelderInfo_writeLine (U"Ceiling ", ic, U": ", ceilings [ic], U" Hz");
}

double structFormantPath :: v_getValueAtSample (integer iframe, integer which, int units) {
	const Formant formant = reinterpret_cast<Formant> (our formants.at [our path [iframe]]);
	return formant -> v_getValueAtSample (iframe, which, units);
}

conststring32 structFormantPath :: v_getUnitText (integer /*level*/, int /*unit*/, uint32 /*flags*/) {
	return U"Frequency (Hz)";
	
};

Thing_implement (FormantPath, Sampled, 0);

autoFormantPath FormantPath_create (double xmin, double xmax, integer nx, double dx, double x1, integer numberOfCeilings) {
	autoFormantPath me = Thing_new (FormantPath);
	Sampled_init (me.get (), xmin, xmax, nx, dx, x1);
	my ceilings = newVECzero (numberOfCeilings);
	my path = newINTVECzero (nx);
	return me;
}

void FormantPath_pathFinder (FormantPath me, double qWeight, double frequencyChangeWeight, double stressWeight, double ceilingChangeWeight, double intensityModulationStepSize, double windowLength, constINTVEC const& parameters, double powerf) {
	autoINTVEC path = FormantPath_getOptimumPath (me, qWeight, frequencyChangeWeight, stressWeight, ceilingChangeWeight, intensityModulationStepSize, windowLength,parameters, powerf, nullptr);
	my path = path.move();
}

autoINTVEC FormantPath_getOptimumPath (FormantPath me, double qWeight, double frequencyChangeWeight, double stressWeight, double ceilingChangeWeight, double intensityModulationStepSize, double windowLength, constINTVEC const& parameters, double powerf, autoMatrix *out_delta) {
	constexpr double qCutoff = 20.0;
	constexpr double stressCutoff = 200.0;
	constexpr double frequencyChangeCutoff = 100.0;
	try {
		autoMatrix stresses, qsums;
		MelderExtremaWithInit intensities;
		const double ceilingsRange = NUMmax (my ceilings.get()) - NUMmin (my ceilings.get());
		const integer midformant = (my formants.size + 1) / 2;
		for (integer iframe = 1; iframe <= my nx; iframe ++) {
			const Formant_Frame frame = & my formants.at [midformant] -> frames [iframe];
			intensities.update (frame -> intensity);
		}
		const bool hasIntensityDifference = ( intensities.max - intensities.min > 0.0 );
		const double dbMid = 0.5 * 10.0 * log10 (intensities.max * intensities.min);
		const integer maxnFormants = my formants.at [1] -> maxnFormants;
		const integer numberOfTracks = std::min (maxnFormants, parameters.size);
		if (qWeight > 0.0)
			qsums = FormantPath_to_Matrix_qSums (me, numberOfTracks);
		if (stressWeight > 0.0)
			stresses = FormantPath_to_Matrix_stress (me, windowLength, parameters, powerf);

		/*
			Some options for assigning costs/benefits to states and state transitions:
			We have states s[i], where i = 1..  S (= my formants.size)
			Whithin each state i we can have j=1..F formant frequencies f[i][j] and bandwidths b[i][j].
			Benefits of a state could be expressed as:
			1. sum (j=1..F, 0.1*f[j]/b[j])/F, this has the advantage that states with large Q values (sharp peaks) have larger benefits
			(2?). -|log(min(max(f1-f2, 100),300)|, keep sufficient distance between f1 and f2
			Costs between successive states:
			3. -sum(j=1..F, ( (2|f[i][j]-f[i+1][j]|/(f[i][j]+f[i+1][j]))
			Global:
			4. -global measure like w ?
			We try to find the path that maximizes the benefits
		*/
		autoINTMAT psi = newINTMATzero (my formants.size, my nx);
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 1.0, my formants.size, my formants.size, 1.0, 1.0);
		MAT deltas (& thy z [1] [1], thy ny, thy nx);
		autoINTVEC path = newINTVECzero (my nx);
		autoVEC intensity = newVECraw (my nx);
		for (integer itime = 1; itime <= my nx; itime ++) {
			for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
				const Formant_Frame frame = & my formants.at [iformant] -> frames [itime];
				double wIntensity = 1.0, delta = 0.0;
				if (hasIntensityDifference) {
					if (frame -> intensity > 0.0) {
						const double dbi = 10.0 * log10 (frame -> intensity / 2e-5);
						wIntensity = NUMsigmoid ((dbi - dbMid) / intensityModulationStepSize);
					} else
						wIntensity = 0.0;
				}
				if (qWeight > 0.0)
					delta += qWeight * std::min (qsums -> z [iformant] [itime] / qCutoff, 1.0);
				double stress = 1.0;
				if (stressWeight > 0.0 && isdefined (stresses -> z [iformant] [itime]))
					stress = std::min (stresses -> z [iformant] [itime] / stressCutoff, 1.0);
				delta -= stressWeight * stress;

				deltas [iformant] [itime] += wIntensity * delta;
			}
		}
		for (integer itime = 2; itime <= my nx; itime ++) {
			for (integer iformant = 1; iformant <= my formants.size; iformant++) {
				const Formant_Frame ffi = & my formants.at [iformant] -> frames [itime];
				double deltamax = -1e100;
				integer maxPos = 0;
				for (integer jformant = 1; jformant <= my formants.size; jformant++) {
					const Formant_Frame ffj = & my formants.at [jformant] -> frames [itime - 1];
					double transitionCosts = 0.0;
					if (frequencyChangeWeight > 0.0) {
						const integer ntracks = std::min (numberOfTracks, ffi -> numberOfFormants);
						double frequencyChangeCosts = 0.0;
						for (integer itrack = 1; itrack <= std::min (ntracks, ffj -> numberOfFormants); itrack ++) {
							const double dif = fabs (ffi -> formant [itrack] . frequency - ffj -> formant [itrack] . frequency);
							const double sum = ffi -> formant [itrack] . frequency + ffj -> formant [itrack] . frequency;
							const double bw = sqrt (ffi -> formant [itrack] . bandwidth * ffj -> formant [itrack] . bandwidth);
							frequencyChangeCosts += bw * dif / sum;
						}
						frequencyChangeCosts = std::min (frequencyChangeCosts / frequencyChangeCutoff, 1.0);
						transitionCosts += frequencyChangeWeight * frequencyChangeCosts;
					}
					if (ceilingChangeWeight > 0.0) {
						const double ceilingChangeCosts = fabs (my ceilings [iformant] - my ceilings [jformant]) / ceilingsRange;
						transitionCosts += ceilingChangeCosts * ceilingChangeWeight;
					}
					const double deltaj = deltas [jformant] [itime - 1] - transitionCosts;
					if (deltaj > deltamax) {
						deltamax = deltaj;
						maxPos = jformant;
					}
				}
				deltas [iformant] [itime] += deltamax;
				psi [iformant] [itime] = maxPos;
			}
		}
		path [my nx] = NUMmaxPos (deltas.column (my nx));
		/*
			Backtrack
		*/
		for (integer itime = my nx; itime > 1; itime --) {
			path [itime - 1] = psi [path [itime]] [itime];
		}
		if (out_delta)
			*out_delta = thee.move();
		return path;
	} catch (MelderError) {
		Melder_throw (me, U": cannot find path.");
	}
}

autoFormant FormantPath_extractFormant (FormantPath me) {
	Formant formant = my formants. at [1];
	autoFormant thee = Formant_create (my xmin, my xmax, my nx, my dx, my x1, formant -> maxnFormants);
	for (integer iframe = 1; iframe <= my path.size; iframe ++) {
		Formant source = reinterpret_cast <Formant> (my formants. at [my path [iframe]]);
		Formant_Frame targetFrame = & thy frames [iframe];
		Formant_Frame sourceFrame = & source -> frames [iframe];
		sourceFrame -> copy (targetFrame);
	}
	return thee;
}

autoFormantPath Sound_to_FormantPath_any (Sound me, kLPC_Analysis lpcType, double timeStep, double maximumNumberOfFormants,
	double middleCeiling, double analysisWidth, double preemphasisFrequency, double ceilingStepSize, 
	integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol,
	integer huber_maximumNumberOfIterations, autoSound *out_sourcesMultiChannel) {
	try {
		Melder_require (timeStep > 0.0,
			U"The timeStep needs to greater than zero seconds.");
		Melder_require (ceilingStepSize > 0.0,
			U"The ceiling step size should larger than 0.0.");
		const double nyquistFrequency = 0.5 / my dx;
		const integer numberOfCeilings = 2 * numberOfStepsToACeiling + 1;
		const double maximumCeiling = middleCeiling *  exp (ceilingStepSize * numberOfStepsToACeiling);
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should be smaller than ", nyquistFrequency, U" Hz. "
			"Decrease the 'ceiling step size' or the 'number of steps' or both.");
		volatile double windowDuration = 2.0 * analysisWidth;
		if (windowDuration > my dx * my nx)
			windowDuration = my dx * my nx;
		/*
			Get the data for the LPC from the resampled sound with 'middleCeiling' as maximum frequency
			to make the sampling exactly equal as if performed with a standard LPC analysis.
		*/
		integer numberOfFrames;
		double t1;
		autoSound midCeiling = Sound_resample (me, 2.0 * middleCeiling, 50);
		Sampled_shortTermAnalysis (midCeiling.get(), windowDuration, timeStep, & numberOfFrames, & t1); // Gaussian window
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		autoFormantPath thee = FormantPath_create (my xmin, my xmax, numberOfFrames, timeStep, t1, numberOfCeilings);
		autoSound multiChannelSound;
		if (out_sourcesMultiChannel)
			multiChannelSound = Sound_create (numberOfCeilings, midCeiling -> xmin, midCeiling -> xmax, midCeiling -> nx, midCeiling -> dx, midCeiling -> x1);
		const double formantSafetyMargin = 50.0;
		thy ceilings [numberOfStepsToACeiling + 1] = middleCeiling;
		for (integer ic  = 1; ic <= numberOfCeilings; ic ++) {
			autoFormant formant;
			if (ic <= numberOfStepsToACeiling)
				thy ceilings [ic] = middleCeiling * exp (-ceilingStepSize * (numberOfStepsToACeiling - ic + 1));
			else if (ic > numberOfStepsToACeiling + 1)
				thy ceilings [ic] = middleCeiling * exp ( ceilingStepSize * (ic - numberOfStepsToACeiling - 1));
			autoSound resampled;
			if (ic != numberOfStepsToACeiling + 1)
				resampled = Sound_resample (me, 2.0 * thy ceilings [ic], 50);
			else 
				resampled = midCeiling.move();
			autoLPC lpc = LPC_create (my xmin, my xmax, numberOfFrames, timeStep, t1, predictionOrder, resampled -> dx);
			if (lpcType != kLPC_Analysis::ROBUST) {
				Sound_into_LPC (resampled.get(), lpc.get(), analysisWidth, preemphasisFrequency, lpcType, marple_tol1, marple_tol2);
			} else {
				Sound_into_LPC (resampled.get(), lpc.get(), analysisWidth, preemphasisFrequency, kLPC_Analysis::AUTOCORRELATION, marple_tol1, marple_tol2);
				lpc = LPC_Sound_to_LPC_robust (lpc.get(), resampled.get(), analysisWidth, preemphasisFrequency, huber_numberOfStdDev, huber_maximumNumberOfIterations, huber_tol, true);
			}
			formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy formants . addItem_move (formant.move());
			if (out_sourcesMultiChannel) {
				autoSound source = LPC_Sound_filterInverse (lpc.get(), resampled.get ());
				autoSound source_resampled = Sound_resample (source.get(), 2.0 * middleCeiling, 50);
				const integer numberOfSamples = std::min (midCeiling -> nx, source_resampled -> nx);
				multiChannelSound -> z.row (ic).part (1, numberOfSamples) <<= source_resampled -> z.row (1).part (1, numberOfSamples);
			}
		}
		/*
			Maintain invariants
		*/
		Melder_assert (thy formants . size == numberOfCeilings);
		thy path = newINTVECraw (thy nx);
		for (integer i = 1; i <= thy path.size; i++)
			thy path [i] = numberOfStepsToACeiling + 1;
		if (out_sourcesMultiChannel)
			*out_sourcesMultiChannel = multiChannelSound.move();
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantPath not created.");
	}
}

autoMatrix FormantPath_to_Matrix_qSums (FormantPath me, integer numberOfTracks) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, my formants.size + 0.5, my formants.size, 1.0, 1.0);
		const integer maxnFormants = my formants.at [1] -> maxnFormants;
		if (numberOfTracks == 0)
			numberOfTracks = maxnFormants;
		for (integer itime = 1; itime <= my nx; itime ++) {
			for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
				const Formant_Frame frame = & my formants.at [iformant] -> frames [itime];
				double qsum = 0.0;
				for (integer itrack = 1; itrack <= std::min (numberOfTracks, frame -> numberOfFormants); itrack ++)
					qsum += frame -> formant [itrack] . frequency / frame-> formant [itrack]. bandwidth;
				qsum /= frame -> numberOfFormants;
				thy z [iformant] [itime] = qsum;
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot calculate qsum.");
	}
}

autoMatrix FormantPath_to_Matrix_transition (FormantPath me, bool maximumCosts) {
	try {
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, my formants.size + 0.5, my formants.size, 1.0, 1.0);
		for (integer itime = 2; itime <= my nx; itime ++) {
			for (integer iformant = 1; iformant <= my formants.size; iformant++) {
				const Formant_Frame ffi = & my formants.at [iformant] -> frames [itime];
				MelderExtremaWithInit costs;
				for (integer jformant = 1; jformant <= my formants.size; jformant++) {
					const Formant_Frame ffj = & my formants.at [jformant] -> frames [itime - 1];
					long double transitionCosts = 0.0;
					const integer ntracks = std::min (ffj -> numberOfFormants, ffi -> numberOfFormants);
					for (integer itrack = 1; itrack <= ntracks; itrack ++) {
						const double dif = fabs (ffi -> formant [itrack] . frequency - ffj -> formant [itrack] . frequency);
						const double sum = ffi -> formant [itrack] . frequency + ffj -> formant [itrack] . frequency;
						const double bw = sqrt (ffi -> formant [itrack] . bandwidth * ffj -> formant [itrack] . bandwidth);
						transitionCosts += bw * dif / sum;
					}
					costs.update ((double) transitionCosts);
				}
				thy z [iformant] [itime] = ( maximumCosts ? costs.max : costs.min );
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot calculate transition costs.");
	}
}

autoMatrix FormantPath_to_Matrix_stress (FormantPath me, double windowLength, constINTVEC const& parameters, double powerf) {
	try {
		const integer numberOfFormants = my formants.size;
		Melder_require (parameters.size > 0 && parameters.size <= numberOfFormants,
			U"The number of parameters should be between 1 and ", numberOfFormants, U".");
		integer fromFormant = 1;
		const integer maximum = NUMmax (parameters);
		const integer numberOfDataPoints = (windowLength + 0.5 * my dx) / my dx;
		Melder_require (numberOfDataPoints >= maximum,
			U"The window length is too short for the number of coefficients you use in the stress determination (",
			maximum, U"). Either increase your window length or decrease the number of coefficents per track.");
		while (fromFormant <= parameters.size && parameters [fromFormant] <= 0)
			fromFormant ++;
		integer toFormant = std::min (numberOfFormants, parameters.size);
		while (toFormant > 0 && parameters [toFormant] <= 0)
			toFormant --;
		Melder_require (fromFormant <= toFormant,
			U"Not all the parameter values should equal zero.");
		autoMatrix thee = Matrix_create (my xmin, my xmax, my nx, my dx, my x1, 0.5, numberOfFormants + 0.5, numberOfFormants, 1.0, 1.0);
		for (integer iformant = 1; iformant <= numberOfFormants; iformant ++) {
			const Formant formanti = (Formant) my formants . at [iformant];
			for (integer iframe = 1; iframe <= my nx; iframe ++) {
				const double time = my x1 + (iframe - 1) * my dx;
				const double startTime = time - 0.5 * windowLength;
				const double endTime = time + 0.5 * windowLength;
				autoFormantModeler fm = Formant_to_FormantModeler (formanti, startTime, endTime,  parameters);
				thy z [iformant] [iframe] = FormantModeler_getStress (fm.get(), fromFormant, toFormant, 0, powerf);
			}
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": cannot create stress Matrix");
	}
}

autoVEC FormantPath_getSmootness (FormantPath me, double tmin, double tmax, integer fromFormant, integer toFormant, constINTVEC const& parameters, double powerf) {
	autoVEC stress = newVECraw (my formants.size);
	for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
		const Formant formanti = (Formant) my formants . at [iformant];
		autoFormantModeler fm = Formant_to_FormantModeler (formanti, tmin, tmax,  parameters);
		stress [iformant] = FormantModeler_getStress (fm.get(), fromFormant, toFormant, 0, powerf);
	}
	return stress;
}

static void Formant_speckles_inside (Formant me, Graphics g, double tmin, double tmax, double fmin, double fmax, integer fromFormant, integer toFormant, double suppress_dB, bool drawBandWidths, MelderColour odd, MelderColour even)
{
	double maximumIntensity = 0.0, minimumIntensity;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	if (fromFormant == 0 && toFormant == 0) {
		fromFormant = 1;
		toFormant = my maxnFormants;
	}
	Graphics_setWindow (g, tmin, tmax, fmin, fmax);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		if (frame -> intensity > maximumIntensity)
			maximumIntensity = frame -> intensity;
	}
	if (maximumIntensity == 0.0 || suppress_dB <= 0.0)
		minimumIntensity = 0.0;   // ignore
	else
		minimumIntensity = maximumIntensity / pow (10.0, suppress_dB / 10.0);

	for (integer iframe = itmin; iframe <= itmax; iframe ++) {
		const Formant_Frame frame = & my frames [iframe];
		const double x = Sampled_indexToX (me, iframe);
		if (frame -> intensity < minimumIntensity)
			continue;
		/*
			Higher formants in general have larger bandwidths. Draw them first to show lower formants clearer.
		*/
		for (integer iformant = std::min (frame -> numberOfFormants, toFormant); iformant >= fromFormant; iformant --) {
			const double frequency = frame -> formant [iformant]. frequency;
			Graphics_setColour (g, iformant % 2 == 1 ? odd : even );
			if (frequency >= fmin && frequency <= fmax) {
				Graphics_speckle (g, x, frequency);
				if (drawBandWidths) {
					const double bandwidth = frame -> formant [iformant]. bandwidth;
					const double upper = std::min (frequency + 0.5 * bandwidth, fmax);
					const double lower = std::max (frequency - 0.5 * bandwidth, fmin);
					Graphics_line (g, x, upper, x, lower);
				}
			}
		}
	}
}

void FormantPath_drawAsGrid_inside (FormantPath me, Graphics g, double tmin, double tmax, double fmax,
	integer fromFormant, integer toFormant, bool showBandwidths, MelderColour odd, MelderColour even,
	integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y, double yGridLineEvery_Hz,
	double xCursor, double yCursor, integer iselected, MelderColour selected, constINTVEC const & parameters,
	bool markWithinPath, bool showStress, double powerf, bool showEstimatedModels, bool garnish)
{
	MelderColour singleSelectionColour = MelderColour (0.984,0.984, 0.7);
	//MelderColour multipleSelectionsColour = MelderColour (0.984,0.984, 0.9);
	constexpr double fmin = 0.0;
	if (nrow <= 0 || ncol <= 0)
		NUMgetGridDimensions (my formants.size, & nrow, & ncol);
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (g, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	const double fontSize_old = Graphics_inqFontSize (g), newFontSize = 8.0;
	const double vp_width = x2NDC - x1NDC, vp_height = y2NDC - y1NDC;
	const double vpi_width = vp_width / (ncol + (ncol - 1) * spaceBetweenFraction_x);
	const double vpi_height = vp_height / (nrow + (nrow - 1) * spaceBetweenFraction_y);
	integer itmin, itmax;
	if (Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax) == 0)
		return;
	
	for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
		const integer irow = 1 + (iformant - 1) / ncol; // left-to-right + top-to-bottom
		const integer icol = 1 + (iformant - 1) % ncol;
		const double vpi_x1 = x1NDC + (icol - 1) * vpi_width * (1.0 + spaceBetweenFraction_x);
		const double vpi_x2 = vpi_x1 + vpi_width;
		const double vpi_y2 = y2NDC - (irow - 1) * vpi_height * (1.0 + spaceBetweenFraction_y);
		const double vpi_y1 = vpi_y2 - vpi_height;
		const Formant formant = my formants.at [iformant];
		autoFormantModeler fm = Formant_to_FormantModeler (formant, tmin, tmax, parameters);
		Graphics_setViewport (g, vpi_x1, vpi_x2, vpi_y1, vpi_y2);
		Graphics_setWindow (g, tmin, tmax, fmin, fmax);
		if (garnish && markWithinPath) {
			MelderColour colourCopy = Graphics_inqColour (g);
			Graphics_setColour (g, singleSelectionColour);
			bool startFound = false;
			double endTime, startTime = std::max (tmin, Sampled_indexToX (me, itmin) - 0.5 * my dx);
			if (my path [itmin] == iformant) {
				startFound = true;
				if (itmin == 1)
					startTime = std::min (tmin, startTime);
			} else {
				if (itmin > 1 && my path [itmin - 1] == iformant && tmin < startTime)
					Graphics_fillRectangle (g, tmin, startTime, 0, fmax);
			}
			for (integer iframe = itmin + 1; iframe <= itmax - 1; iframe ++) {
				if (my path [iframe] == iformant) {
					if (! startFound) {
						startTime = Sampled_indexToX (me, iframe) - 0.5 * my dx;
						startFound = true;
					}
				} else {
					if (startFound) {
						endTime = Sampled_indexToX (me, iframe - 1) + 0.5 * my dx;
						Graphics_fillRectangle (g, startTime, endTime, 0.0, fmax);
						startFound = false;
					}
				}
			}
			endTime = Sampled_indexToX (me, itmax) + 0.5 * my dx;
			if (my path [itmax] == iformant) {
				if (itmax == my nx || my path [itmax + 1] == iformant)
					endTime = tmax;
				if (! startFound) {
					startTime = Sampled_indexToX (me, itmax) - 0.5 * my dx;
					Graphics_fillRectangle (g, startTime, endTime, 0.0, fmax);
				} else {
					Graphics_fillRectangle (g, startTime, endTime, 0.0, fmax);
				}
			} else {
				if (startFound) {
					const double t2 = Sampled_indexToX (me, itmax - 1);
					Graphics_fillRectangle (g, startTime, std::min (t2 + 0.5 * my dx, tmax), 0.0, fmax);
					startFound = false;
				}
				if (itmax < my nx && my path [itmax + 1] == iformant && tmax > endTime) {
					Graphics_fillRectangle (g, endTime, tmax, 0.0, fmax);
				}
			}
			Graphics_setColour (g, colourCopy);
		}
		Formant_speckles_inside (formant, g, tmin, tmax, fmin, fmax, fromFormant, toFormant, 100.0, showBandwidths, odd, even);
		if (showEstimatedModels)
			FormantModeler_drawModel_inside (fm.get(), g, tmin, tmax, fmax, fromFormant, toFormant, odd, even, 100_integer);
		Graphics_setColour (g, Melder_BLACK);
		if (garnish)
			Graphics_rectangle (g, tmin, tmax, fmin, fmax);
		
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setLineWidth (g, 1.0);
		/*
			Mark ceiling & stress
		*/
		autoMelderString info;
		const double tLeftPos = tmin - 0.01 * (tmax - tmin), tRightPos = tmax + 0.01 * (tmax - tmin);
		if (garnish) {
			if (showStress) {
				const double stress = FormantModeler_getStress (fm.get(), fromFormant, toFormant, 0, powerf);
				MelderString_append (& info, U"Fit=", Melder_fixed (stress, 2));
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_BOTTOM);
				Graphics_text (g, tLeftPos, fmax, info.string);
			}
			MelderString_empty (& info);
			Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_BOTTOM);
			MelderString_append (& info, U"Ceiling=", Melder_fixed (my ceilings [iformant], 0), U" Hz");
			Graphics_text (g, tRightPos, fmax, info.string);
		}
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		if (garnish) {
			auto getXtick = [] (Graphics gg, double fontSize) {
				const double margin = 2.8 * fontSize * gg -> resolution / 72.0;
				const double wDC = (gg -> d_x2DC - gg -> d_x1DC) / (gg -> d_x2wNDC - gg -> d_x1wNDC) * (gg -> d_x2NDC - gg -> d_x1NDC);
				double dx = 1.5 * margin / wDC;
				double xTick = 0.06 * dx;
				if (dx > 0.4) dx = 0.4;
				return xTick /= 1.0 - 2.0 * dx;
			};
			auto getYtick = [] (Graphics gg, double fontSize) {
				const double margin = 2.8 * fontSize * gg -> resolution / 72.0;
				const double hDC = integer_abs (gg->d_y2DC - gg->d_y1DC) / (gg->d_y2wNDC - gg->d_y1wNDC) * (gg->d_y2NDC - gg-> d_y1NDC);
				double dy = margin / hDC;
				double yTick = 0.09 * dy;
				if (dy > 0.4) dy = 0.4;
				yTick /= 1.0 - 2.0 * dy;
				return yTick;
			};
			const double xTick = (double) getXtick (g, newFontSize) * (tmax - tmin);
			const double yTick = (double) getYtick (g, newFontSize) * (fmax - fmin);
			if (irow == nrow) {
				MelderString_empty (& info);
				MelderString_append (& info, Melder_fixed (tmin, 3), U" s");
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_TOP);
				Graphics_line (g, tmin, fmin, tmin, fmin - yTick);
				Graphics_text (g, tmin , fmin - yTick, info.string);
				MelderString_empty (& info);
				MelderString_append (& info, Melder_fixed (tmax, 3), U" s");
				Graphics_line (g, tmax, fmin, tmax, fmin - yTick);
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_TOP);
				Graphics_text (g, tmax, fmin - yTick, info.string);
			}
			if (icol == 1) {
				MelderString_empty (& info);
				MelderString_append (& info, Melder_iround (fmin), U" Hz");
				Graphics_line (g, tmin - xTick, fmin, tmin, fmin);
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_HALF);
				Graphics_text (g, tmin - xTick, fmin, info.string);
				MelderString_empty (& info);
				MelderString_append (& info, Melder_iround (fmax), U" Hz");
				Graphics_text (g, tmin - xTick, fmax, info.string);
				
			}
			double yGridLine_Hz = yGridLineEvery_Hz;
			Graphics_setLineType (g, Graphics_DOTTED);
			while (yGridLine_Hz < 0.95 * fmax) {
				Graphics_line (g, tmin, yGridLine_Hz, tmax, yGridLine_Hz);
				yGridLine_Hz += yGridLineEvery_Hz;
			}
			/*
				Cursors
			*/
			Graphics_setColour (g, Melder_RED);
			Graphics_setLineType (g, Graphics_DASHED);
			if (xCursor > tmin && xCursor <= tmax)
				Graphics_line (g, xCursor, 0.0, xCursor, fmax);
			if (yCursor > 0.0 && yCursor < fmax)
				Graphics_line (g, tmin, yCursor, tmax, yCursor);
			Graphics_setColour (g, Melder_BLACK);
			Graphics_setLineType (g, Graphics_DRAWN);
		}
	}
	Graphics_setFontSize (g, fontSize_old);
	Graphics_setViewport (g, x1NDC, x2NDC, y1NDC, y2NDC);
	
}

void FormantPath_drawAsGrid (FormantPath me, Graphics g, double tmin, double tmax, double fmax, 
	integer fromFormant, integer toFormant, bool showBandwidths, MelderColour odd, MelderColour even, 
	integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y, double yGridLineEvery_Hz,
	double xCursor, double yCursor, integer iselected, MelderColour selected, constINTVEC const & parameters,
	bool markWithinPath, bool showStress, double powerf, bool showEstimatedModels, bool garnish)
{
	Function_bidirectionalAutowindow (me, & tmin, & tmax);
	Graphics_setInner (g);
	FormantPath_drawAsGrid_inside (me, g, tmin, tmax, fmax, fromFormant, toFormant, showBandwidths, odd, even, nrow, ncol, spaceBetweenFraction_x, spaceBetweenFraction_y, yGridLineEvery_Hz, xCursor, yCursor, iselected, selected, parameters, markWithinPath, showStress, powerf, showEstimatedModels, garnish);
	Graphics_unsetInner (g);
}	
	
	
	
/* End of file FormantPath.cpp */
