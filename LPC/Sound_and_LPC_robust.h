#ifndef _Sound_and_LPC_robust_h_
#define _Sound_and_LPC_robust_h_
/* Sound_and_LPC_robust.h
 *
 * Copyright (C) 1993-2011 David Weenink
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
 djmw 20030815 GPL header
 djmw 20110307 Latest modification
*/

#ifndef _LPC_h_
	#include "LPC.h"
#endif
#ifndef _Sound_h_
	#include "Sound.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

int LPC_Frames_and_Sound_huber (LPC_Frame me, Sound thee,
	LPC_Frame him, void *huber);
/*
	The gnu c compiler (version 3.3.1) complaints about having two LPC_Frame types
	in the argument list:
	error: two or more data types in declaration of `LPC_Frame_and_Sound_into_LPC_Frame_huber
	By defining a void pointer we circumvent the complaint.
*/
	
LPC LPC_and_Sound_to_LPC_robust (LPC thee, Sound me, double analysisWidth,
	double preEmphasisFrequency, double k, int itermax, double tol, int wantlocation);

#ifdef __cplusplus
	}
#endif

#endif /* _Sound_and_LPC_robust_h_ */
