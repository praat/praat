#ifndef _PowerCepstrogramFrameIntoMatrixFrame_h_
#define _PowerCepstrogramFrameIntoMatrixFrame_h_
/* PowerCepstrogramFrameIntoMatrixFrame.h
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

#include "SampledFrameIntoMatrixFrame.h"
#include "PowerCepstrumWorkspace.h"
#include "PowerCepstrogram.h"
#include "Matrix.h"

#include "PowerCepstrogramFrameIntoMatrixFrame_def.h"

autoPowerCepstrogramFrameIntoMatrixFrame PowerCepstrogramFrameIntoMatrixFrame_create (constPowerCepstrogram thee, mutableMatrix matrix,
	double qminFit, double qmaxFit,	kCepstrum_trendType trendLineType, kCepstrum_trendFit fitMethod);

#endif /* _PowerCepstrogramFrameIntoMatrixFrame_h_ */
