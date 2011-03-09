#ifndef _LPC_and_Tube_h_
#define _LPC_and_Tube_h_
/* LPC_and_Tube.h
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
 djmw 20030612 GPL header
 djmw 20110307 Latest modification
*/

#ifndef _LPC_h_
	#include "LPC.h"
#endif
#ifndef _Tube_h_
	#include "Tube.h"
#endif
#ifndef _VocalTract_h_
	#include "VocalTract.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

int LPC_Frame_into_Tube_Frame_rc (LPC_Frame me, Tube_Frame thee);
int LPC_Frame_into_Tube_Frame_area (LPC_Frame me, Tube_Frame thee);

double LPC_Frame_getVTL_wakita (LPC_Frame me, double samplingPeriod, double refLength);

int Tube_Frame_into_LPC_Frame_area (Tube_Frame me, LPC_Frame thee);

int Tube_Frame_into_LPC_Frame_rc (Tube_Frame me, LPC_Frame thee);

VocalTract LPC_to_VocalTract (LPC me, double time, double length, int wakitaLength);
/* Calculate a vocal tract via the transformation of lpc coeffs to area functions.
 *	`length' will be the length of the vocal tract.
 * if wakitaLength != 0 then the vocal tract length is calculated according to:
 *      H. Wakita (1976), Normalization of vowels by vocal-tract length and its
 *      application to vowel identification, IEEE on ASSP 25, 183-192.
 * Here `length' will be used as the reference length for the calculations. */

#ifdef __cplusplus
	}
#endif

#endif /* _LPC_and_Tube_h_ */
