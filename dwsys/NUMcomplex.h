#ifndef _NUMcomplex_h_
#define _NUMcomplex_h_
/* NUMcomplex.h
 *
 * Copyright (C) 2017-2020 David Weenink
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

#include "NUM2.h"

/* Gamma[alpha,x] = integral{x, infty, t^(alpha-1)exp(-t)dt}, Gamma[alpha]= Gamma[alpha,0]
 * alpha and x are complex numbers with Re(alpha) > 0
 */
dcomplex NUMincompleteGammaFunction (const dcomplex alpha, const dcomplex x);

/*
	Get response of a truncated gammatone filter defined on the interval [0, truncationTime] at the resonance frequency
*/
dcomplex gammaToneFilterResponseAtCentreFrequency (double centre_frequency, double bandwidth, double gamma, double initialPhase, double truncationTime);

#endif

