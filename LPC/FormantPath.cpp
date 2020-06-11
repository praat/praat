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

conststring32 structFormantPath :: v_getUnitText (integer level, int unit, uint32 flags) {
	return U"Frequency (Hz)";
	
};

Thing_implement (FormantPath, Function, 0);

autoFormantPath FormantPath_create (double xmin, double xmax, integer nx, double dx, double x1, integer numberOfCeilings) {
	autoFormantPath me = Thing_new (FormantPath);
	Sampled_init (me.get (), xmin, xmax, nx, dx, x1);
	my ceilings = newVECzero (numberOfCeilings);
	my path = newINTVECzero (nx);
	return me;
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
	double formantCeiling, double windowLength, double preemphasisFrequency, double ceilingStepFraction, 
	integer numberOfStepsToACeiling, double marple_tol1, double marple_tol2, double huber_numberOfStdDev, double huber_tol,
	integer huber_maximumNumberOfIterations, autoSound *sourcesMultiChannel) {
	try {
		Melder_require (ceilingStepFraction > 0.0 && ceilingStepFraction < 1.0,
			U"The ceiling step fraction should be a number between 0.0 and 1.0");
		const double nyquistFrequency = 0.5 / my dx;
		const integer numberOfCeilings = 2 * numberOfStepsToACeiling + 1;
		const double minimumCeiling = formantCeiling * pow (1.0 - ceilingStepFraction, numberOfStepsToACeiling);
		Melder_require (minimumCeiling > 0.0,
			U"Your minimum ceiling is ", minimumCeiling, U" Hz, but it should be positive.\n"
			"We computed it as your middle ceiling (", formantCeiling, U" Hz) times (1.0 - ", ceilingStepFraction, 
			U")^", numberOfStepsToACeiling, U" Hz. Decrease the ceiling step or the number of steps or both.");
		const double maximumCeiling = formantCeiling * pow (1.0 +  ceilingStepFraction, numberOfStepsToACeiling);		
		Melder_require (maximumCeiling <= nyquistFrequency,
			U"The maximum ceiling should be smaller than ", nyquistFrequency, U" Hz. "
			"Decrease the 'ceiling step' or the 'number of steps' or both.");
		integer fake_nx = 1; double fake_x1 = 0.005, fake_dx = 0.001; // we know them after the analyses
		autoFormantPath thee = FormantPath_create (my xmin, my xmax, fake_nx, fake_dx, fake_x1, numberOfCeilings);
		autoSound sources [1 + numberOfCeilings];
		const double formantSafetyMargin = 50.0;
		const integer predictionOrder = Melder_iround (2.0 * maximumNumberOfFormants);
		for (integer ic  = 1; ic <= numberOfCeilings; ic ++) {
			autoLPC lpc;
			double factor = 1.0;
			if (ic <= numberOfStepsToACeiling)
				factor = pow (1.0 - ceilingStepFraction, numberOfStepsToACeiling + 1 - ic);
			else if (ic > numberOfStepsToACeiling + 1)
				factor = pow (1.0 + ceilingStepFraction, ic - numberOfStepsToACeiling - 1);
			thy ceilings [ic] = formantCeiling * factor;
			autoSound resampled = Sound_resample (me, 2.0 * thy ceilings [ic], 50);
			if (lpcType == kLPC_Analysis::BURG)
				lpc = Sound_to_LPC_burg (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::AUTOCORRELATION)
				lpc = Sound_to_LPC_autocorrelation (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::COVARIANCE)
				lpc = Sound_to_LPC_covariance (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
			else if (lpcType == kLPC_Analysis::MARPLE)
				lpc = Sound_to_LPC_marple (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency, marple_tol1, marple_tol2);
			else if (lpcType == kLPC_Analysis::ROBUST) {
				autoLPC lpc_in = Sound_to_LPC_autocorrelation (resampled.get(), predictionOrder, windowLength, timeStep, preemphasisFrequency);
				lpc = LPC_Sound_to_LPC_robust (lpc_in.get(), resampled.get(), windowLength, preemphasisFrequency, huber_numberOfStdDev, huber_maximumNumberOfIterations, huber_tol, true);
			}
			autoFormant formant = LPC_to_Formant (lpc.get(), formantSafetyMargin);
			thy formants . addItem_move (formant.move());
			if (sourcesMultiChannel) {
				autoSound source = LPC_Sound_filterInverse (lpc.get(), resampled.get ());
				sources [ic] = Sound_resample (source.get(), 2.0 * formantCeiling, 50).move();
			}
		}
		/*
			Maintain invariants
		*/
		Melder_assert (thy formants . size == numberOfCeilings);
		Formant formant = thy formants . at [numberOfStepsToACeiling + 1];
		thy nx = formant -> nx;
		thy dx = formant -> dx;
		thy x1 = formant -> x1;
		thy path = newINTVECraw (thy nx);
		for (integer i = 1; i <= thy path.size; i++)
			thy path [i] = numberOfStepsToACeiling + 1;
		if (sourcesMultiChannel) {
			Sound mid = sources [numberOfStepsToACeiling + 1].get();
			autoSound multiChannel = Sound_create (numberOfCeilings, mid -> xmin, mid -> xmax, mid -> nx, mid -> dx, mid -> x1);
			for (integer ic = 1; ic <= numberOfCeilings; ic ++) {
				Sound him = sources [ic] . get();
				const integer numberOfSamples = std::min (mid -> nx, his nx);
				multiChannel -> z.row (ic).part (1, numberOfSamples) <<= his z.row (1).part (1, numberOfSamples);
			}
			*sourcesMultiChannel = multiChannel.move();
		}
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantPath not created.");
	}
}

autoVEC FormantPath_getSmootness (FormantPath me, double tmin, double tmax, integer fromFormant, integer toFormant, constINTVEC const& parameters, double powerf) {
	autoVEC smoothness = newVECraw (my formants.size);
	for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
		Formant formanti = (Formant) my formants . at [iformant];
		autoFormantModeler fm = Formant_to_FormantModeler (formanti, tmin, tmax,  parameters);
		smoothness [iformant] = FormantModeler_getSmoothnessValue (fm.get(), fromFormant, toFormant, 0, powerf);
	}
	return smoothness;
}

static void Formant_speckles_inside (Formant me, Graphics g, double tmin, double tmax, double fmin, double fmax, integer fromFormant, integer toFormant, double suppress_dB, bool drawBandWidths, MelderColour odd, MelderColour even)
{
	double maximumIntensity = 0.0, minimumIntensity;
	Function_unidirectionalAutowindow (me, & tmin, & tmax);
	integer itmin, itmax;
	if (! Sampled_getWindowSamples (me, tmin, tmax, & itmin, & itmax))
		return;
	if (fromFormant == toFormant && fromFormant == 0) {
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

void FormantPath_drawAsGrid_inside (FormantPath me, Graphics g, double tmin, double tmax, double fmax, integer fromFormant, integer toFormant, bool showBandwidths, MelderColour odd, MelderColour even, integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y,  double yGridLineEvery_Hz, double xCursor, double yCursor, integer iselected, MelderColour selected, bool showSmoothness, constINTVEC const & parameters, double powerf,  bool garnish) {
	const double fmin = 0.0;
	if (nrow <= 0 || ncol <= 0)
		NUMgetGridDimensions (my formants.size, & nrow, & ncol);
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (g, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	const double fontSize_old = Graphics_inqFontSize (g), newFontSize = 8.0;
	const double vp_width = x2NDC - x1NDC, vp_height = y2NDC - y1NDC;
	const double vpi_width = vp_width / (ncol + (ncol - 1) * spaceBetweenFraction_x);
	const double vpi_height = vp_height / (nrow + (nrow - 1) * spaceBetweenFraction_y);
	autoVEC smoothness;
	if (showSmoothness)
		smoothness = FormantPath_getSmootness (me, tmin, tmax, fromFormant, toFormant, parameters, powerf);
	for (integer iformant = 1; iformant <= my formants.size; iformant ++) {
		const integer irow = 1 + (iformant - 1) / ncol; // left-to-right + top-to-bottom
		const integer icol = 1 + (iformant - 1) % ncol;
		double vpi_x1 = x1NDC + (icol - 1) * vpi_width * (1.0 + spaceBetweenFraction_x);
		double vpi_x2 = vpi_x1 + vpi_width;
		double vpi_y2 = y2NDC - (irow - 1) * vpi_height * (1.0 + spaceBetweenFraction_y);
		double vpi_y1 = vpi_y2 - vpi_height;
		Formant formant = my formants.at [iformant];
		Graphics_setViewport (g, vpi_x1, vpi_x2, vpi_y1, vpi_y2);
		Graphics_setWindow (g, tmin, tmax, fmin, fmax);
		Formant_speckles_inside (formant, g, tmin, tmax, fmin, fmax, fromFormant, toFormant, 100.0, showBandwidths, odd, even);

		Graphics_setLineWidth (g, 2.0);
		Graphics_setColour (g, ( iformant == iselected ? selected : Melder_BLACK ));
		Graphics_rectangle (g, tmin, tmax, fmin, fmax);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setColour (g, Melder_BLACK);
		Graphics_setLineWidth (g, 1.0);
		/*
			Mark name & smoothness
		*/
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_HALF);
		Graphics_text (g, tmax - 0.05 * (tmax - tmin),
			fmax - 0.05 * fmax, Melder_fixed (my ceilings [iformant], 0));
		if (showSmoothness) {
			Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_HALF);
			Graphics_text (g, tmin + 0.05 * (tmax - tmin), fmax - 0.05 * fmax, Melder_fixed (smoothness [iformant], 2));
		}
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		conststring32 midTopText = U"";
		if (midTopText && midTopText [0]) {
			Graphics_setColour (g, Melder_BLUE);
			Graphics_text (g, tmin + 0.5 * (tmax - tmin),
				fmax - 0.05 * fmax, midTopText);
			Graphics_setColour (g, Melder_BLACK);
		}

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
				double margin = 2.8 * fontSize * gg -> resolution / 72.0;
				double hDC = integer_abs (gg->d_y2DC - gg->d_y1DC) / (gg->d_y2wNDC - gg->d_y1wNDC) * (gg->d_y2NDC - gg-> d_y1NDC);
				double dy = margin / hDC;
				double yTick = 0.09 * dy;
				if (dy > 0.4) dy = 0.4;
				yTick /= 1.0 - 2.0 * dy;
				return yTick;
			};
			double xTick = (double) getXtick (g, newFontSize) * (tmax - tmin);
			double yTick = (double) getYtick (g, newFontSize) * (fmax - fmin);
			if (icol == 1 && irow % 2 == 1) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_HALF);
				Graphics_line (g, tmin - xTick, fmax, tmin, fmax);
				Graphics_text (g, tmin - xTick, fmax, Melder_iround (fmax));
				Graphics_line (g, tmin - xTick, fmin, tmin, fmin);
				Graphics_text (g, tmin - xTick, fmin, Melder_fixed (fmin, 0));
			} else if (icol == ncol && irow % 2 == 0) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_HALF);
				Graphics_text (g, tmax, fmax, Melder_iround (fmax));
				Graphics_text (g, tmax, fmin, Melder_fixed (fmin, 0));
			}
			if (irow == 1 && icol % 2 == 0) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
				Graphics_line (g, tmin, fmax, tmin, fmax + yTick);
				Graphics_text (g, tmin, fmax + yTick, Melder_fixed (tmin, 3));
				Graphics_line (g, tmax, fmax, tmax, fmax + yTick);
				Graphics_text (g, tmax, fmax + yTick, Melder_fixed (tmax, 3));
			} else if (irow == nrow && icol % 2 == 1) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
				Graphics_line (g, tmin, fmin, tmin, fmin - yTick);
				Graphics_text (g, tmin, fmin - yTick, Melder_fixed (tmin, 3));
				Graphics_line (g, tmax, fmin, tmax, fmin - yTick);
				Graphics_text (g, tmax, fmin - yTick, Melder_fixed (tmax, 3));
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

void FormantPath_drawAsGrid (FormantPath me, Graphics g, double tmin, double tmax, double fmax, integer fromFormant, integer toFormant, bool showBandwidths, MelderColour odd, MelderColour even, integer nrow, integer ncol, double spaceBetweenFraction_x, double spaceBetweenFraction_y,  double yGridLineEvery_Hz, double xCursor, double yCursor, integer iselected, MelderColour selected, 
bool showSmoothness, constINTVEC const & parameters, double powerf, bool garnish) {
	Graphics_setInner (g);
	FormantPath_drawAsGrid_inside (me, g, tmin, tmax, fmax, fromFormant, toFormant, showBandwidths, odd, even, nrow, ncol, spaceBetweenFraction_x, spaceBetweenFraction_y, yGridLineEvery_Hz, xCursor, yCursor, iselected, selected, showSmoothness, parameters,  powerf, garnish);
	Graphics_unsetInner (g);
}	
	
	
	
/* End of file FormantPath.cpp */
