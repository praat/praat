#ifndef _LPC_and_LFCC_h_
#define _LPC_and_LFCC_h_
/* LPC_and_LFCC.h
 *
 * Copyright (C) 1994-2017 David Weenink
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
 djmw 20001228
 djmw 20020812 GPL header
 djmw 20110307 Latest modification
*/

#include "LPC.h"
#include "LFCC.h"

autoLFCC LPC_to_LFCC (LPC me, integer numberOfCoefficients);

autoLPC LFCC_to_LPC (LFCC me, integer numberOfCoefficients);

void LPC_Frame_into_CC_Frame (LPC_Frame me, CC_Frame thee);
/*
	Transformation of lp-coefficients to cepstral coefficients.
	
	The number of cepstral coefficients may be larger than
	the number of lp-coefficients (equation 5). 
	
	Reference: Digital Signal Processing Committee (eds.),
	Programs for Digital Signal Processing, IEEE Press,
	1979 (Formulas 4a, 4b, 4c and 5, page 4.3-2).
*/

void CC_Frame_into_LPC_Frame (CC_Frame me, LPC_Frame thee);
/*
	Transformation of cepstral coefficients to lp-coefficients.
	
	The number of lp-coefficients can never exceed the number
	of cepstral coefficients.
	
	Reference: Digital Signal Processing Committee (eds.),
	Programs for Digital Signal Processing, IEEE Press,
	1979 (page 4.3-6).
*/

#endif /* _LPC_and_LFCC_h_ */
