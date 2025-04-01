/* LPCFrameIntoFormantFrame.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "LPCFrameIntoFormantFrame.h"
#include "Roots_and_Formant.h"

#include "oo_DESTROY.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_COPY.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_EQUAL.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_READ_TEXT.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_READ_BINARY.h"
#include "LPCFrameIntoFormantFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCFrameIntoFormantFrame_def.h"

Thing_implement (LPCFrameIntoFormantFrame, LPCFrameIntoSampledFrame, 0);

static void Formant_Frame_init (Formant_Frame me, integer numberOfFormants) {
	if (numberOfFormants > 0)
		my formant = newvectorzero <structFormant_Formant> (numberOfFormants);
	my numberOfFormants = my formant.size; // maintain invariant
}

static void LPC_Frame_into_Polynomial (constLPC_Frame me, mutablePolynomial p) {
	/*
		The lpc coefficients are a[1..nCoefficients], a[0] == 1, is not stored
		For the polynomial we therefore need one extra coefficient since
		the a's are stored in reverse order in the polynomial and a[0]
		represents the highest power and is stored into the last position
		of the polynomial.
	*/
	Melder_assert (my nCoefficients  == my a.size); // check invariant
	const integer highestPolynomialCoefficientNumber = my nCoefficients + 1;
	Melder_assert (p -> _capacity >= highestPolynomialCoefficientNumber);
	
	p -> coefficients.resize (highestPolynomialCoefficientNumber);
	p -> numberOfCoefficients = p -> coefficients.size; // maintain invariant
	p -> coefficients [highestPolynomialCoefficientNumber] = 1.0;
	for (integer icof = 1; icof <= my nCoefficients; icof ++)
		p -> coefficients [icof] = my a [highestPolynomialCoefficientNumber - icof];
}

void structLPCFrameIntoFormantFrame :: allocateOutputFrames () {
	for (integer iframe = 1; iframe <= formant -> nx; iframe ++) {
		const Formant_Frame formantFrame = & formant -> frames [iframe];
		Formant_Frame_init (formantFrame, formant -> maxnFormants);
	}
}

void structLPCFrameIntoFormantFrame :: getInputFrame () {
}

bool structLPCFrameIntoFormantFrame :: inputFrameToOutputFrame () {
	Formant_Frame formantf = & formant -> frames [currentFrame];
	LPC_Frame inputlpcf = & inputlpc -> d_frames [currentFrame];
	formantf -> intensity = inputlpcf -> gain;
	if (inputlpcf -> nCoefficients == 0) {
		formantf -> numberOfFormants = 0;
		formantf -> formant.resize (formantf -> numberOfFormants); // maintain invariant
		frameAnalysisInfo = 1;	
		return true;
	}
	frameAnalysisInfo = 0;
	const double samplingFrequency = 1.0 / inputlpc -> samplingPeriod;
	LPC_Frame_into_Polynomial (inputlpcf, p.get());
	Polynomial_into_Roots (p.get(), roots.get(), buffer.get());
	Roots_fixIntoUnitCircle (roots.get());
	Roots_into_Formant_Frame (roots.get(), formantf, samplingFrequency, margin);
	return true;
}

void structLPCFrameIntoFormantFrame :: saveOutputFrame () {
}

void LPCFrameIntoFormantFrame_init(LPCFrameIntoFormantFrame me, constLPC input, mutableFormant output, double margin) {
	LPCFrameIntoSampledFrame_init (me, input);
	my margin = margin;
	my formant = output;
	my bufferSize = my order * my order + my order + my order + 11 * my order;
	my buffer = raw_VEC (my bufferSize);		
	my p = Polynomial_create (-1.0, 1.0, my order);
	my roots = Roots_create (my order);
}

autoLPCFrameIntoFormantFrame LPCFrameIntoFormantFrame_create (constLPC input, mutableFormant output, double margin) {
	try {
		autoLPCFrameIntoFormantFrame me = Thing_new (LPCFrameIntoFormantFrame);
		LPCFrameIntoFormantFrame_init(me.get(), input, output, margin);
		return me;
	} catch (MelderError) {
		Melder_throw (U"LPCFrameIntoFormantFrame not created.");
	}
}

/* End of file LPCFrameIntoFormantFrame.cpp */

