/* Sound_to_Formant.h
 *
 * Copyright (C) 1992-2011,2015,2019 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "Sound.h"
#include "Formant.h"

autoFormant Sound_to_Formant_any (Sound me, double timeStep, integer numberOfPoles, double maximumFrequency,
	double halfdt_window, int which, double preemphasisFrequency, double safetyMargin);
/*
	Which = 1: Burg.
	Which = 2: Split-Levinson
*/

autoFormant Sound_to_Formant_burg (Sound me, double timeStep, double maximumNumberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);
/* Throws away all formants below 50 Hz and above Nyquist minus 50 Hz. */

autoFormant Sound_to_Formant_keepAll (Sound me, double timeStep, double maximumNumberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);
/* Same as previous, but keeps all formants. Good for resynthesis. */

autoFormant Sound_to_Formant_willems (Sound me, double timeStep, double numberOfFormants,
	double maximumFormantFrequency, double windowLength, double preemphasisFrequency);

/* End of file Sound_to_Formant.h */
