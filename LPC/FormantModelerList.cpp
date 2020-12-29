/*FormantModelerList.cpp
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

#include "FormantModelerList.h"

#include "oo_DESTROY.h"
#include "FormantModelerList_def.h"
#include "oo_COPY.h"
#include "FormantModelerList_def.h"
#include "oo_EQUAL.h"
#include "FormantModelerList_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "FormantModelerList_def.h"
#include "oo_WRITE_BINARY.h"
#include "FormantModelerList_def.h"
#include "oo_READ_BINARY.h"
#include "FormantModelerList_def.h"
#include "oo_WRITE_TEXT.h"
#include "FormantModelerList_def.h"
#include "oo_READ_TEXT.h"
#include "FormantModelerList_def.h"
#include "oo_DESCRIPTION.h"
#include "FormantModelerList_def.h"
/*
void structFormantModelerListDrawingSpecification :: v_writeBinary (FILE *_filePointer_) {}
void structFormantModelerList :: v_writeBinary (FILE *_filePointer_) {}
void structFormantModelerListDrawingSpecification :: v_readBinary (FILE *_filePointer_, int _formatVersion_) {}
void structFormantModelerList :: v_readBinary (FILE *_filePointer_, int _formatVersion_) {}

*/
void structFormantModelerList :: v_info () {
	
};

Thing_implement (FormantModelerList, Function, 0);
Thing_implement (FormantModelerListDrawingSpecification, Daata, 0);

autoFormantModelerList FormantPath_to_FormantModelerList (FormantPath me, double startTime, double endTime, conststring32 numberOfParametersPerTrack_string) {
	try {
		autoFormantModelerList thee = Thing_new (FormantModelerList);
		thy xmin = startTime;
		thy xmax = endTime;
		autoINTVEC numberOfParametersPerTrack = newINTVECfromString (numberOfParametersPerTrack_string);
		Melder_require (numberOfParametersPerTrack.size > 0,
			U"The number of items in the parameter list should be larger than zero.");
		thy numberOfTracksPerModel = numberOfParametersPerTrack.size;
		integer numberOfZeros = 0;
		for (integer ipar = 1; ipar <= numberOfParametersPerTrack.size; ipar ++) {
			const integer value = numberOfParametersPerTrack [ipar];
			Melder_require (value >= 0,
				U"Numbers in the 'Number of parameter list' should be positive.");
			if (value == 0)
				numberOfZeros += 1;
		}
		thy numberOfParametersPerTrack = numberOfParametersPerTrack.move();
		thy numberOfTracksPerModel = thy numberOfParametersPerTrack.size;
		thy numberOfModelers = my formants . size;
		for (integer imodel = 1; imodel <= thy numberOfModelers; imodel ++) {
			Formant formanti = (Formant) my formants . at [imodel];
			autoFormantModeler fm = Formant_to_FormantModeler (formanti, startTime, endTime,  thy numberOfParametersPerTrack.get());
			Thing_setName (fm.get(), Melder_fixed (my ceilings [imodel], 0));
			thy formantModelers. addItem_move (fm.move());
		}
		thy drawingSpecification = FormantModelerList_to_FormantModelerListDrawingSpecification (thee.get(), 0);		
		return thee;
	} catch (MelderError) {
		Melder_throw (me, U": FormantModelerList not created.");
	}
}

void FormantModelerList_showBest3 (FormantModelerList me) {
	autoINTVEC best3 = FormantModelerList_getBest3 (me);
	INTVEC drawingOrder = my drawingSpecification -> drawingOrder.get();
	drawingOrder.part (1, 3) <<= best3.all();
	my drawingSpecification -> numberOfModelersToDraw = 3;
}

void FormantModelerList_markBest3 (FormantModelerList me) {
	/*
		3 The smoothest F1 score
		2 The smoothest F1 & F2 score
		1 the smoothest F1 & F2 & F3 score
	*/
	autoINTVEC best3 = FormantModelerList_getBest3 (me);
	for (integer imodel = 1; imodel <= my numberOfModelers; imodel ++) {
		autoMelderString best;
		if (imodel == best3 [1]) {
			MelderString_append (& best, U"F123");
		}
		if (imodel == best3 [2]) {
			MelderString_append (& best, ( best.string && best.string [0] ? U"&F12" : U"F12" ));
		}
		if (imodel == best3 [3]) {
			MelderString_append (& best, ( best.string && best.string [0] ? U"&F1" : U"F1" ));
		}
		my drawingSpecification -> midTopText [imodel] = Melder_dup (best.string);
	}
}

integer FormantModelerList_getBestModelIndex (FormantModelerList me, integer fromTrack, integer toTrack) {
	double wmin = std::numeric_limits<double>::max();
	integer best = 0;
	for (integer imodel = 1; imodel <= my numberOfModelers; imodel ++) {
		FormantModeler fm = my formantModelers.at [imodel];
		double w = FormantModeler_getStress (fm, fromTrack, toTrack, 0, my varianceExponent);
		if (w < wmin) {
			wmin = w;
			best = imodel;
		}
	}
	return best;
}

autoINTVEC FormantModelerList_getBest3 (FormantModelerList me) {
	/*
		3 The least stress F1 score
		2 The least (summed) stress F1 & F2 score
		1 the least (summed) stress F1 & F2 & F3 score
	*/
	autoINTVEC best = raw_INTVEC (3);
	double stressF1, stressF1F2, stressF1F2F3;
	stressF1 = stressF1F2 = stressF1F2F3 = std::numeric_limits<double>::max();
	for (integer imodel = 1; imodel <= my numberOfModelers; imodel ++) {
		FormantModeler fm = my formantModelers.at [imodel];
		double stress = FormantModeler_getStress (fm, 1, 1, 0, my varianceExponent);
		if (stress < stressF1) {
			stressF1 = stress;
			best [3] = imodel;
		}
		stress = FormantModeler_getStress (fm, 1, 2, 0, my varianceExponent);
		if (stress < stressF1F2) {
			stressF1F2 = stress;
			best [2]  = imodel;
		}
		stress = FormantModeler_getStress (fm, 1, 3, 0, my varianceExponent);
		if (stress < stressF1F2F3) {
			stressF1F2F3 = stress;
			best [1]  = imodel;
		}
	}
	return best;
}

static void getMatrixGridLayout (integer numberOfModels, integer *out_numberOfRows, integer *out_numberOfColums) {
	integer ncol = 1;
	integer nrow = 3;
	if (numberOfModels > 3) {
		nrow = 1 + Melder_ifloor (sqrt (numberOfModels - 0.5));
		ncol = 1 + Melder_ifloor ((numberOfModels - 1) / nrow);
	}
	if (out_numberOfRows)
		*out_numberOfRows = nrow;
	if (out_numberOfColums)
		*out_numberOfColums = ncol;
}

void FormantModelerList_getMatrixGridLayout (FormantModelerList me, integer *out_numberOfRows, integer *out_numberOfColums) {
	getMatrixGridLayout (my drawingSpecification -> numberOfModelersToDraw, out_numberOfRows, out_numberOfColums);
}

void FormantModelerListDrawingSpecification_showAll (FormantModelerListDrawingSpecification me) {
	my numberOfModelersToDraw = my numberOfModelers;
	to_INTVEC_out (my drawingOrder.get());
}

integer FormantModelerListDrawingSpecification_getNumberOfShown (FormantModelerListDrawingSpecification me) {
	return my numberOfModelersToDraw;
}

void FormantModelerListDrawingSpecification_setModelerColours (FormantModelerListDrawingSpecification me, conststring32 oddFormantColour_string, conststring32 evenFormantColour_string, conststring32 selectedCandidateColour_string) {
	my oddFormantColour = MelderColour_fromColourNameOrNumberStringOrRGBString (oddFormantColour_string);
	my evenFormantColour = MelderColour_fromColourNameOrNumberStringOrRGBString (evenFormantColour_string);
	my selectedCandidateColour = MelderColour_fromColourNameOrNumberStringOrRGBString (selectedCandidateColour_string);
}

autoFormantModelerListDrawingSpecification FormantModelerList_to_FormantModelerListDrawingSpecification (FormantModelerList me, integer defaultModeler) {
	try {
		autoFormantModelerListDrawingSpecification thee = Thing_new (FormantModelerListDrawingSpecification);
		thy numberOfModelers = my numberOfModelers;
		thy drawingOrder = to_INTVEC (my numberOfModelers);
		thy numberOfModelersToDraw = my numberOfModelers;
		thy boxLineWidth = 4.0;
		thy oddFormantColour = Melder_RED;
		thy evenFormantColour = Melder_MAROON;
		thy selectedCandidateColour = Melder_RED;
		thy midTopText_colour = Melder_PURPLE;
		autoSTRVEC midTopText (my numberOfModelers);
		for (integer imodel = 1; imodel <= my numberOfModelers; imodel ++)
			midTopText [imodel] = Melder_dup (U"");
		thy midTopText = newSTRVECcopy (midTopText.get());
		return thee;
	} catch (MelderError) {
		Melder_throw (U"No FormantModelerListDrawingSpecification created.");
	}
}

void FormantModelerList_drawInMatrixGrid (FormantModelerList me, Graphics g, integer nrow, integer ncol, kGraphicsMatrixOrigin origin, double spaceBetweenFraction_x, double spaceBetweenFraction_y, integer fromFormant, integer toFormant, double fmax, double yGridLineEvery_Hz, double xCursor, double yCursor, integer numberOfParameters, bool drawErrorBars, double barwidth_s, bool drawEstimated, bool garnish) {
	if (nrow <= 0 || ncol <= 0)
		FormantModelerList_getMatrixGridLayout (me, & nrow, & ncol);
	const double fmin = 0.0;
	double x1NDC, x2NDC, y1NDC, y2NDC;
	Graphics_inqViewport (g, & x1NDC, & x2NDC, & y1NDC, & y2NDC);
	const double fontSize_old = Graphics_inqFontSize (g), newFontSize = 8.0;
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
	const bool fillUp = ( origin == kGraphicsMatrixOrigin::BOTTOM_LEFT || origin == kGraphicsMatrixOrigin::BOTTOM_RIGHT );
	const bool rightToLeft = ( origin == kGraphicsMatrixOrigin::TOP_RIGHT || origin ==kGraphicsMatrixOrigin:: BOTTOM_RIGHT );
	const double vp_width = x2NDC - x1NDC, vp_height = y2NDC - y1NDC;
	const double vpi_width = vp_width / (ncol + (ncol - 1) * spaceBetweenFraction_x);
	const double vpi_height = vp_height / (nrow + (nrow - 1) * spaceBetweenFraction_y);
	for (integer index = 1; index <= my drawingSpecification->numberOfModelersToDraw; index ++) {
		const integer irow1 = 1 + (index - 1) / ncol; // left-to-right + top-to-bottom
		const integer icol1 = 1 + (index - 1) % ncol;
		const integer icol = ( rightToLeft ? ncol - icol1 + 1 : icol1 );
		const integer irow = ( fillUp ? nrow - irow1 + 1 : irow1 );
		double vpi_x1 = x1NDC + (icol - 1) * vpi_width * (1.0 + spaceBetweenFraction_x);
		double vpi_x2 = vpi_x1 + vpi_width;
		double vpi_y2 = y2NDC - (irow - 1) * vpi_height * (1.0 + spaceBetweenFraction_y);
		double vpi_y1 = vpi_y2 - vpi_height;
		integer imodel = my drawingSpecification -> drawingOrder [index];
		FormantModeler fm = my formantModelers.at [imodel];
		Graphics_setViewport (g, vpi_x1, vpi_x2, vpi_y1, vpi_y2);
		Graphics_setWindow (g, fm -> xmin, fm -> xmax, 0.0, fmax);
		FormantModeler_speckle_inside (fm, g, fm -> xmin, fm -> xmax, fmax, fromFormant, toFormant,
			drawEstimated, 0.0, drawErrorBars, my drawingSpecification -> oddFormantColour, my drawingSpecification -> evenFormantColour);

		Graphics_setLineWidth (g, my drawingSpecification -> boxLineWidth);
		Graphics_setColour (g, (imodel == my drawingSpecification -> selectedCandidate ?
			my drawingSpecification -> selectedCandidateColour : Melder_BLACK ));
		Graphics_rectangle (g, fm -> xmin, fm -> xmax, fmin, fmax);
		Graphics_setLineType (g, Graphics_DRAWN);
		Graphics_setColour (g, Melder_BLACK);
		Graphics_setLineWidth (g, 1.0);
		/*
			Mark name & roughness
		*/
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_HALF);
		Graphics_text (g, fm -> xmax - 0.05 * (fm -> xmax - fm -> xmin),
			fmax - 0.05 * fmax, fm -> name.get());
		double w = FormantModeler_getStress (fm, fromFormant, toFormant, 0, my varianceExponent);
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_HALF);
		Graphics_text (g, fm -> xmin + 0.05 * (fm -> xmax - fm -> xmin),
			fmax - 0.05 * fmax, Melder_fixed (w, 2));
		Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_HALF);
		conststring32 midTopText = my drawingSpecification -> midTopText [imodel].get();
		if (midTopText && midTopText [0]) { 
			Graphics_setColour (g,my  drawingSpecification -> midTopText_colour);
			Graphics_text (g, fm -> xmin + 0.5 * (fm -> xmax - fm -> xmin),
				fmax - 0.05 * fmax, my drawingSpecification -> midTopText [imodel].get());
			Graphics_setColour (g, Melder_BLACK);
		}

		if (garnish) {
			double xTick = (double) getXtick (g, newFontSize) * (fm -> xmax - fm -> xmin);
			double yTick = (double) getYtick (g, newFontSize) * (fmax - 0.0);
			if (icol == 1 && irow % 2 == 1) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::RIGHT, Graphics_HALF);
				Graphics_line (g, fm -> xmin - xTick, fmax, fm -> xmin, fmax);
				Graphics_text (g, fm -> xmin - xTick, fmax, Melder_iround (fmax));
				Graphics_line (g, fm -> xmin - xTick, 0.0, fm -> xmin, 0.0);
				Graphics_text (g, fm -> xmin - xTick, 0.0, U"0.0");
			} else if (icol == ncol && irow % 2 == 0) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::LEFT, Graphics_HALF);
				Graphics_text (g, fm -> xmax, fmax, Melder_iround (fmax));
				Graphics_text (g, fm -> xmax, 0.0, U"0.0");
			}
			if (irow == 1 && icol % 2 == 0) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_BOTTOM);
				Graphics_line (g, fm -> xmin, fmax, fm -> xmin, fmax + yTick);
				Graphics_text (g, fm -> xmin, fmax + yTick, Melder_fixed (fm -> xmin, 3));
				Graphics_line (g, fm -> xmax, fmax, fm -> xmax, fmax + yTick);
				Graphics_text (g, fm -> xmax, fmax + yTick, Melder_fixed (fm -> xmax, 3));
			} else if (irow == nrow && icol % 2 == 1) {
				Graphics_setTextAlignment (g, kGraphics_horizontalAlignment::CENTRE, Graphics_TOP);
				Graphics_line (g, fm -> xmin, 0.0, fm -> xmin, 0.0 - yTick);
				Graphics_text (g, fm -> xmin, 0.0 - yTick, Melder_fixed (fm -> xmin, 3));
				Graphics_line (g, fm -> xmax, 0.0, fm -> xmax, 0.0 - yTick);
				Graphics_text (g, fm -> xmax, 0.0 - yTick, Melder_fixed (fm -> xmax, 3));
			}
			double yGridLine_Hz = yGridLineEvery_Hz;
			Graphics_setLineType (g, Graphics_DOTTED);
			while (yGridLine_Hz < 0.95 * fmax) {
				Graphics_line (g, fm -> xmin, yGridLine_Hz, fm -> xmax, yGridLine_Hz);
				yGridLine_Hz += yGridLineEvery_Hz;
			}
			/*
				Cursors
			*/
			Graphics_setColour (g, Melder_RED);
			Graphics_setLineType (g, Graphics_DASHED);
			if (xCursor > fm -> xmin && xCursor <= fm -> xmax)
				Graphics_line (g, xCursor, 0.0, xCursor, fmax);
			if (yCursor > 0.0 && yCursor < fmax)
				Graphics_line (g, fm -> xmin, yCursor, fm -> xmax, yCursor);
			Graphics_setColour (g, Melder_BLACK);
			Graphics_setLineType (g, Graphics_DRAWN);
		}
	}
	Graphics_setFontSize (g, fontSize_old);
	Graphics_setViewport (g, x1NDC, x2NDC, y1NDC, y2NDC);
}

/* End of file FormantModelerList.cpp */
