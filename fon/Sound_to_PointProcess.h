/* Sound_to_PointProcess.h
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

#include "Sound.h"
#include "PointProcess.h"

PointProcess Sound_to_PointProcess_extrema (Sound me, long channel, int interpolation, bool includeMaxima, bool includeMinima);
PointProcess Sound_to_PointProcess_maxima (Sound me, long channel, int interpolation);
PointProcess Sound_to_PointProcess_minima (Sound me, long channel, int interpolation);
PointProcess Sound_to_PointProcess_allExtrema (Sound me, long channel, int interpolation);

PointProcess Sound_to_PointProcess_zeroes (Sound me, long channel, bool includeRaisers, bool includeFallers);

PointProcess Sound_to_PointProcess_periodic_cc (Sound me, double fmin, double fmax);

PointProcess Sound_to_PointProcess_periodic_peaks (Sound me, double fmin, double fmax, bool includeMaxima, bool includeMinima);

/* End of file Sound_to_PointProcess.h */
