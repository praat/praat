/* PowerCepstrumWorkspace_def.h
 *
 * Copyright (C) 2025 David Weenink
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

#define ooSTRUCT PowerCepstrumWorkspace
oo_DEFINE_CLASS (PowerCepstrumWorkspace, Daata)
	oo_INTEGER (numberOfPoints)
	oo_INTEGER (imin) // [imin,imax] the range of quefrency indices for the slope calculation
	oo_INTEGER (imax)
	oo_VEC (x, numberOfPoints)
	oo_VEC (y, numberOfPoints)
	oo_OBJECT (Matrix, 2, asdBs) // for peak detection
	oo_OBJECT (SlopeSelector, 0, slopeSelector)
	oo_BOOLEAN (subtractTrendLine)
	oo_ENUM (kSlopeSelector_method, method)
	oo_ENUM (kCepstrum_trendType, trendLineType)
	oo_ENUM (kVector_peakInterpolation, peakInterpolationType)
	oo_DOUBLE (qminSearchInterval) // peak in [pitchFloor, pitchCeiling]
	oo_DOUBLE (qmaxSearchInterval)
	oo_DOUBLE (slope)
	oo_DOUBLE (intercept)
	oo_DOUBLE (cpp) // peakdB - background
	oo_DOUBLE (background)
	oo_DOUBLE (peakdB) // value of the peak in dB
	oo_DOUBLE (peakQuefrency)
oo_END_CLASS (PowerCepstrumWorkspace)
#undef ooSTRUCT

/* End of file PowerCepstrumToMatrixWorkspace_def.h */
