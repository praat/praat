/* Sound_to_PointProcess.h
 *
 * Copyright (C) 1992-2005,2007,2011,2015-2017,2020 Paul Boersma
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
#include "PointProcess.h"

autoPointProcess Sound_to_PointProcess_extrema (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType, bool includeMaxima, bool includeMinima);
autoPointProcess Sound_to_PointProcess_maxima (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType);
autoPointProcess Sound_to_PointProcess_minima (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType);
autoPointProcess Sound_to_PointProcess_allExtrema (Sound me, integer channel, kVector_peakInterpolation peakInterpolationType);

autoPointProcess Sound_to_PointProcess_zeroes (Sound me, integer channel, bool includeRaisers, bool includeFallers);

autoPointProcess Sound_to_PointProcess_periodic_cc (Sound me, double fmin, double fmax);

autoPointProcess Sound_to_PointProcess_periodic_peaks (Sound me, double fmin, double fmax, bool includeMaxima, bool includeMinima);

/* End of file Sound_to_PointProcess.h */
