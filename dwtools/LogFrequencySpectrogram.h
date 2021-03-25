#ifndef _LogFrequencySpectrogram_h_
#define _LogFrequencySpectrogram_h_
/* LogFrequencySpectrogram.h
 *
 * Copyright (C) 2021 David Weenink
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

#include "melder.h"
#include "Matrix.h"
#include "Sound.h"

Thing_define (LogFrequencySpectrogram, Matrix) {
	void v_info ()
		override;
};

/*
Interpretation:
	xmin, xmax, x1, dx, nx like Sampled.
	ymin, ymax lowest and highest log2 (frequency).
    y1 mid of first frequency bin as log (f).
    dy distance between bins on log(f)
    ny the number of frequency bins.
 */

autoLogFrequencySpectrogram LogFrequencySpectrogram_create (double xmin, double xmax, integer nx, double dx, double x1, double f1, integer numberOfStepsPerOctave, integer numberOfSteps);

void LogFrequencySpectrogram_paint (LogFrequencySpectrogram me, Graphics g, double xmin, double xmax, double ymin, double ymax, double minimum, double maximum, bool garnish);

autoLogFrequencySpectrogram Sound_to_LogFrequencySpectrogram_constantQ (Sound me, double timeStep, double f1, integer numberOfStepsPerOctave, integer numberOfSteps);

#endif /* _LogFrequencySpectrogram_h_ */
