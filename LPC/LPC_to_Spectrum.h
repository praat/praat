#ifndef _LPC_to_Spectrum_h_
#define _LPC_to_Spectrum_h_
/* LPC_to_Spectrum.h
 *
 * Copyright (C) 1994-2011 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 19980224
 djmw 20020812 GPL header
 djmw 20110702 Latest modification
*/

#include "LPC.h"
#include "Spectrum.h"

#ifdef __cplusplus
	extern "C" {
#endif

void LPC_Frame_into_Spectrum (LPC_Frame me, Spectrum thee, double bandwidthReduction,
	double deEmphasisFrequency);
	
Spectrum LPC_to_Spectrum (LPC me, double t, double dfMin, double bandwidthReduction,
	double deEmphasisFrequency);
	
Spectrum LPC_to_Spectrum2 (LPC me, double t, double dfMin, double bandwidthReduction);
/* if(dfMin >= 0) df <= dfMin else df = NyquistFrequency / 512 */
/* integration radius r = exp (- pi * bandwidthReduction / samplingFrequency) */

#ifdef __cplusplus
	}
#endif

#endif /* _LPC_and_Spectrum_h_ */
