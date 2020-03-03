#ifndef _LPC_and_Formant_h_
#define  _LPC_and_Formant_h_
/* LPC_and_Formant.h
 *
 * Copyright (C) 1994-2020 David Weenink
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

#include "LPC.h"
//#include "Polynomial.h"
#include "Roots.h"
#include "Formant.h"

autoFormant LPC_to_Formant (LPC me, double margin);

autoLPC Formant_to_LPC (Formant me, double samplingPeriod);

void LPC_Frame_into_Formant_Frame (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin);

/*
	No extra memory allocations
	The workspace size is at least 
*/
void LPC_Frame_into_Formant_Frame_mt (LPC_Frame me, Formant_Frame thee, double samplingPeriod, double margin, Polynomial p, Roots r, VEC const& workspace);


void Formant_Frame_into_LPC_Frame (Formant_Frame me, LPC_Frame thee, double samplingPeriod);

void Formant_Frame_scale (Formant_Frame me, double scale);

void Roots_into_Formant_Frame (Roots me, Formant_Frame thee, double samplingFrequency, double margin);

void Formant_Frame_init (Formant_Frame me, integer nFormants);

#endif /* _LPC_and_Formant_h_ */
