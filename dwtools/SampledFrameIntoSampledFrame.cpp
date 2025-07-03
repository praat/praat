/* SampledFrameIntoSampledFrame.cpp
 *
 * Copyright (C) 2024-2025 David Weenink
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

#include "Preferences.h"
#include "SampledFrameIntoSampledFrame.h"
#include "NUM2.h"
#include "melder_str32.h"

#include "oo_DESTROY.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_COPY.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_EQUAL.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SampledFrameIntoSampledFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledFrameIntoSampledFrame_def.h"

Thing_implement (SampledFrameIntoSampledFrame, Daata, 0);


void structSampledFrameIntoSampledFrame :: getInputFrame () {
	return;
}

bool structSampledFrameIntoSampledFrame :: inputFrameToOutputFrame () {
	return true;
}

void structSampledFrameIntoSampledFrame :: saveOutputFrame () {
	return;
}

void structSampledFrameIntoSampledFrame :: allocateOutputFrames () {
	return;
}

void structSampledFrameIntoSampledFrame :: allocateMemoryAfterThreadsAreKnown () {
	return;
}

void structSampledFrameIntoSampledFrame :: inputFramesToOutputFrames (integer fromFrame, integer toFrame) {
	for (integer iframe = fromFrame; iframe <= toFrame; iframe ++) {
		currentFrame = iframe;
		getInputFrame ();
		if (! inputFrameToOutputFrame ())
			framesErrorCount ++;
		saveOutputFrame ();
		if (updateStatus)
			status -> frameIntoFrameInfo [currentFrame] = frameAnalysisInfo;
	}
}

void SampledFrameIntoSampledFrame_init (SampledFrameIntoSampledFrame me, mutableSampled output) {
	my output = output;
}

void SampledFrameIntoSampledFrame_initForStatusUpdates (SampledFrameIntoSampledFrame me, mutableSampledIntoSampledStatus status, bool updateStatus) {
	my status = status;
	my updateStatus = updateStatus;
}

void SampledFrameIntoSampledFrame_saveLocalOutputFrames (SampledFrameIntoSampledFrame me) {
	my saveLocalOutputFrames ();
}

/* End of file SampledFrameIntoSampledFrame.cpp */
