#ifndef _AnalyticSound_h_
#define _AnalyticSound_h_
/* AnalyticSound.h
 *
 * Copyright (C) 2021-2022 David Weenink
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

#include "Sound.h"
#include "Graphics.h"
#include "Intensity.h"

Thing_define (AnalyticSound, Sound) {
	double v_getValueAtSample (integer isample, integer which, int unit) const
		override;
};

autoAnalyticSound AnalyticSound_create (double xmin, double xmax, integer nx, double dx, double x1);

void Sound_into_AnalyticSound (Sound me, AnalyticSound thee);

autoAnalyticSound Sound_to_AnalyticSound (Sound me);

autoSound AnalyticSound_to_Sound (AnalyticSound me);

autoIntensity AnalyticSound_to_Intensity (AnalyticSound me);

#endif /* _AnalyticSound_h_ */
