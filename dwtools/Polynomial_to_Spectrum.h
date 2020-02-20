#ifndef _Polynomial_to_Spectrum_h_
#define _Polynomial_to_Spectrum_h_
/* Polynomial_to_Spectrum.h
 *
 * Copyright (C) 2020 David Weenink
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 */

#include "Polynomial.h"
#include "Spectrum.h"

autoSpectrum Polynomial_to_Spectrum (Polynomial me, double nyquistFrequency, integer numberOfFrequencies, double radius);

#endif /* _Polynomial_to_Spectrum_h_ */
