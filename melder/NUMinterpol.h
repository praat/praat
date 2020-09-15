#ifndef _NUMinterpol_h_
#define _NUMinterpol_h_
/* NUMinterpol.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

/********** Interpolation and optimization **********/

// Special values for interpolationDepth:
#define NUM_VALUE_INTERPOLATE_NEAREST  0
#define NUM_VALUE_INTERPOLATE_LINEAR  1
#define NUM_VALUE_INTERPOLATE_CUBIC  2
// Higher values than 2 yield a true sinc interpolation. Here are some examples:
#define NUM_VALUE_INTERPOLATE_SINC70  70
#define NUM_VALUE_INTERPOLATE_SINC700  700
double NUM_interpolate_sinc (constVEC const& y, double x, integer interpolationDepth);

#define NUM_PEAK_INTERPOLATE_NONE  0
#define NUM_PEAK_INTERPOLATE_PARABOLIC  1
#define NUM_PEAK_INTERPOLATE_CUBIC  2
#define NUM_PEAK_INTERPOLATE_SINC70  3
#define NUM_PEAK_INTERPOLATE_SINC700  4

double NUMimproveExtremum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real, bool isMaximum);
double NUMimproveMaximum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real);
double NUMimproveMinimum (constVEC const& y, integer ixmid, integer interpolationDepth, double *ixmid_real);

void NUM_viterbi (
	integer numberOfFrames, integer maxnCandidates,
	integer (*getNumberOfCandidates) (integer iframe, void *closure),
	double (*getLocalCost) (integer iframe, integer icand, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, void *closure),
	void (*putResult) (integer iframe, integer place, void *closure),
	void *closure);

void NUM_viterbi_multi (
	integer nframe, integer ncand, integer ntrack,
	double (*getLocalCost) (integer iframe, integer icand, integer itrack, void *closure),
	double (*getTransitionCost) (integer iframe, integer icand1, integer icand2, integer itrack, void *closure),
	void (*putResult) (integer iframe, integer place, integer itrack, void *closure),
	void *closure);

/* End of file NUMinterpol.h */
#endif
