#ifndef _SoundFrameIntoMatrixFrame_h_
#define _SoundFrameIntoMatrixFrame_h_
/* SoundFrameIntoMatrixFrame.h
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

#include "Matrix.h"
#include "SoundFrameIntoSampledFrame.h"

#include "SoundFrameIntoMatrixFrame_def.h"

void SoundFrameIntoMatrixFrame_init (mutableSoundFrameIntoMatrixFrame me, constSound input, mutableMatrix output, double effectiveAnalysisWidth, kSound_windowShape windowShape);

#endif /* _SoundFrameIntoMatrixFrame_h_ */
 
