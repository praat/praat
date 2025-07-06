/* LPCFrameIntoSampledFrame.cpp
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

#include "oo_DESTROY.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_COPY.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_EQUAL.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_READ_TEXT.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_READ_BINARY.h"
#include "LPCFrameIntoSampledFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "LPCFrameIntoSampledFrame_def.h"


Thing_implement (LPCFrameIntoSampledFrame, SampledFrameIntoSampledFrame, 0);

void LPCFrameIntoSampledFrame_init (mutableLPCFrameIntoSampledFrame me, constLPC input) {
	my inputlpc = input;
	my order = input -> maxnCoefficients;
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

/* End of file LPCFrameIntoSampledFrame.cpp */
