#ifndef _LPC_to_Spectrum_h_
#define _LPC_to_Spectrum_h_
/* LPC_to_Spectrum.h
 *
 * Copyright (C) 1994-2011, 2015-2016 David Weenink
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

/*
 djmw 19980224
 djmw 20020812 GPL header
 djmw 20110702 Latest modification
*/

#include "LPC.h"
#include "Spectrum.h"

void LPC_Frame_into_Spectrum (LPC_Frame me, Spectrum thee, double bandwidthReduction, double deEmphasisFrequency);
	
autoSpectrum LPC_to_Spectrum (LPC me, double t, double dfMin, double bandwidthReduction, double deEmphasisFrequency);
	
autoSpectrum LPC_to_Spectrum2 (LPC me, double t, double dfMin, double bandwidthReduction);
/* if(dfMin >= 0) df <= dfMin else df = NyquistFrequency / 512 */
/* integration radius r = exp (- pi * bandwidthReduction / samplingFrequency) */

#endif /* _LPC_and_Spectrum_h_ */
