/* SampledFrameIntoMatrixFrame.cpp
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
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_COPY.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_EQUAL.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SampledFrameIntoMatrixFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledFrameIntoMatrixFrame_def.h"

Thing_implement (SampledFrameIntoMatrixFrame, SampledFrameIntoSampledFrame, 0);

void structSampledFrameIntoMatrixFrame :: allocateMemoryAfterThreadsAreKnown () {
	Melder_assert (maximumNumberOfFrames > 0);
	localOutput = zero_MAT (matrix -> ny, maximumNumberOfFrames);
}

void SampledFrameIntoMatrixFrame_init (SampledFrameIntoMatrixFrame me, mutableMatrix output) {
	SampledFrameIntoSampledFrame_init (me, output);
	my matrix = output;
}

/* End of file SampledFrameIntoMatrixFrame.cpp */
