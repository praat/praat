#ifndef _ComplexSpectrogram_h_
#define _ComplexSpectrogram_h_

/* ComplexSpectrogram.h
 * 
 * Copyright (C) 2014 David Weenink
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

#include "Sound.h"
#include "Spectrum.h"
#include "Spectrogram.h"

#include "ComplexSpectrogram_def.h"

autoComplexSpectrogram ComplexSpectrogram_create (double tmin, double tmax, long nt, double dt,
	double t1, double fmin, double fmax, long nf, double df, double f1);

autoComplexSpectrogram Sound_to_ComplexSpectrogram (Sound me, double windowLength, double timeStep);
/* Hann window because we want to reconstruct */

autoSound ComplexSpectrogram_to_Sound (ComplexSpectrogram me, double stretchFactor);

autoSpectrogram ComplexSpectrogram_to_Spectrogram (ComplexSpectrogram me);

autoSpectrum ComplexSpectrogram_to_Spectrum (ComplexSpectrogram me, double time);

#endif  /* _ComplexSpectrogram_h_ */
