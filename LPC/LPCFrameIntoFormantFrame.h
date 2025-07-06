#ifndef _LPCFrameIntoFormantFrame_h_
#define _LPCFrameIntoFormantFrame_h_
/* LPCFrameIntoFormantFrame.h
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

#include "Formant.h"
#include "LPC.h"
#include "Polynomial.h"
#include "Roots.h"
#include "LPCFrameIntoSampledFrame.h"

#include "LPCFrameIntoFormantFrame_def.h"

void LPCFrameIntoFormantFrame_init (LPCFrameIntoFormantFrame me, constLPC input, mutableFormant output, double margin);

autoLPCFrameIntoFormantFrame LPCFrameIntoFormantFrame_create (constLPC input, mutableFormant output, double margin);

inline integer numberOfFormantsFromNumberOfCoefficients (integer maxnCoefficients, double margin) {
	return ( margin == 0.0 ? maxnCoefficients : (maxnCoefficients + 1) / 2 );
}

inline integer numberOfPolesFromNumberOfFormants (double numberOfFormants) {
	return 2.0 * numberOfFormants;
}

#endif /*_LPCFrameIntoFormantFrame_h_ */
