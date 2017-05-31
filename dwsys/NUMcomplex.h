#ifndef _NUMcomplex_h_
#define _NUMcomplex_h_
/* NUMcomplex.h
 *
 * Copyright (C) 2017 David Weenink
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

/* alpha_re > 0 */
void NUMincompleteGammaFunction (double alpha_re, double alpha_im, double z_re, double z_im, double *result_re, double *result_im);

void gammaToneFilterResponseAtResonance (double centre_frequency, double bandwidth, long gamma, double initialPhase, double t0, double *response_re, double *response_im);

#endif

