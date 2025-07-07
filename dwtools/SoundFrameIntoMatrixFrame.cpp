/* SoundFrameIntoMatrixFrame.cpp
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

#include "SoundFrameIntoMatrixFrame.h"
#include "Matrix_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_COPY.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_EQUAL.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SoundFrameIntoMatrixFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundFrameIntoMatrixFrame_def.h"

Thing_implement (SoundFrameIntoMatrixFrame, SoundFrameIntoSampledFrame, 0);

void SoundFrameIntoMatrixFrame_init (mutableSoundFrameIntoMatrixFrame me, constSound input, mutableMatrix output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	SoundFrameIntoSampledFrame_init (me, input, output, effectiveAnalysisWidth, windowShape);
	my matrix = output;
}

void structSoundFrameIntoMatrixFrame :: allocateOutputFrames () {
}

void structSoundFrameIntoMatrixFrame :: allocateMemoryAfterThreadsAreKnown () {
	Melder_assert (maximumNumberOfFrames > 0);
	localOutput = zero_MAT (matrix -> ny, maximumNumberOfFrames);
}

void structSoundFrameIntoMatrixFrame :: saveLocalOutputFrames (void) {
	Melder_assert (localOutput.ncol == maximumNumberOfFrames);
	Melder_assert (localOutput.nrow == matrix -> ny);
	integer outputcol = startFrame;
	for (integer icol = 1; icol <= currentNumberOfFrames; icol ++, outputcol ++) {
		for (integer irow = 1; irow <= localOutput.nrow; irow ++)
			matrix -> z [irow] [outputcol] = localOutput [irow] [icol];
	}
}

/* End of file SoundFrameIntoMatrixFrame.cpp */
