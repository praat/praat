#ifndef _Sound_and_FilterBank_h_
#define _Sound_and_FilterBank_h_
/* Sound_and_FilterBank.h
 *
 * Copyright (C) 1993-2012 David Weenink
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
 djmw 20010404
 djmw 20020813 GPL header
 djmw 20120508 Latest modification
*/

#include "FilterBank.h"
#include "Pitch.h"
#include "Sound.h"

BarkFilter Sound_to_BarkFilter (Sound me, double analysisWidth, double dt,
	double f1_bark, double fmax_bark, double df_bark);
/*
	Filtering with filters on a Bark scale as defined by
		Andrew Sekey & Brian Hanson (1984), "Improved 1-Bark bandwidth
		"auditory filter", Jasa 75, 1902-1904.
	Although not explicitely stated the filter function is defined in the
	power domain.
	10 log F(z) = 15.8 + 7.5(z + 0.5) - 17.5 * sqrt(1 + (z + 0.5)^2)
*/

MelFilter Sound_to_MelFilter (Sound me, double analysisWidth, double dt,
	double f1_mel, double fmax_mel, double df_mel);

FormantFilter Sound_to_FormantFilter (Sound me, double analysisWidth,
	double dt, double f1_hz, double fmax_hz, double df_hz, double relative_bw,
	double minimumPitch, double maximumPitch);

FormantFilter Sound_and_Pitch_to_FormantFilter (Sound me, Pitch thee,
	double analysisWidth, double dt, double f1_hz, double fmax_hz,
	double df_hz, double relative_bw);

Sound FilterBanks_crossCorrelate (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);
Sound FilterBanks_convolve (FilterBank me, FilterBank thee, enum kSounds_convolve_scaling scaling, enum kSounds_convolve_signalOutsideTimeDomain signalOutsideTimeDomain);


#endif /* _Sound_and_FilterBank_h_ */
