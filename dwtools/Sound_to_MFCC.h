#ifndef _Sound_to_MFCC_h_
#define _Sound_to_MFCC_h_
/* Sound_to_MFCC.h
 *
 * Copyright (C) 1993-2017 David Weenink
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
 djmw 20010410
 djmw 20020813 GPL header
 djmw 20110307 Latest modification
*/

#include "MFCC.h"
#include "Sound.h"

autoMFCC Sound_to_MFCC (Sound me, integer numberOfCoefficients, double analysisWidth,
	double dt, double f1_mel, double fmax_mel, double df_mel);

#endif /* _Sound_to_MFCC_h_ */
