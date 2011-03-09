/* Sound_to_Formant.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2011/03/03
 */

#include "Sound.h"
#include "Formant.h"

#ifdef __cplusplus
	extern "C" {
#endif

Formant Sound_to_Formant_any (Sound me, double timeStep, int numberOfPoles, double maximumFrequency,
	double halfdt_window, int which, double preemphasisFrequency, double safetyMargin);
/*
	Which = 1: Burg.
	Which = 2: Split-Levinson
*/

Formant Sound_to_Formant_burg (Sound me, double timeStep, double maximumNumberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);
/* Throws away all formants below 50 Hz and above Nyquist minus 50 Hz. */

Formant Sound_to_Formant_keepAll (Sound me, double timeStep, double maximumNumberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);
/* Same as previous, but keeps all formants. Good for resynthesis. */

Formant Sound_to_Formant_willems (Sound me, double timeStep, double numberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);

#ifdef __cplusplus
	}
#endif

/* End of file Sound_to_Formant.h */
