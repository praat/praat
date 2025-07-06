/* SoundFrameIntoSampledFrame.cpp
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

#include "SoundFrameIntoSampledFrame.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_COPY.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_EQUAL.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_READ_TEXT.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_READ_BINARY.h"
#include "SoundFrameIntoSampledFrame_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundFrameIntoSampledFrame_def.h"

Thing_implement (SoundFrameIntoSampledFrame, SampledFrameIntoSampledFrame, 0);

integer getSoundFrameSize_uneven (double approximatePhysicalAnalysisWidth, double samplingPeriod) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / samplingPeriod);
	return 2 * halfFrameSamples + 1;
}

integer getSoundFrameSize (double physicalAnalysisWidth, double samplingPeriod) {
	Melder_assert (physicalAnalysisWidth > 0.0);
	Melder_assert (samplingPeriod > 0.0);
	const double numberOfSamples_real = round (physicalAnalysisWidth / samplingPeriod);
	return (integer) numberOfSamples_real;
}

void structSoundFrameIntoSampledFrame :: getInputFrame () {
	const double midTime = Sampled_indexToX (output, currentFrame);
	integer soundFrameBegin = Sampled_xToNearestIndex (sound, midTime - 0.5 * physicalAnalysisWidth); // approximation
	if (updateStatus) {
		SoundIntoSampledStatus soundIntoSampledStatus = reinterpret_cast<SoundIntoSampledStatus> (status);
		soundIntoSampledStatus -> soundFrameBegins [currentFrame] = soundFrameBegin;
	}
	for (integer isample = 1; isample <= soundFrame.size; isample ++, soundFrameBegin ++) {
		soundFrame [isample] = ( soundFrameBegin > 0 && soundFrameBegin <= sound -> nx ? sound -> z [1] [soundFrameBegin] : 0.0 );
	}
	if (subtractFrameMean)
		centre_VEC_inout (soundFrame, nullptr);
	soundFrameExtremum = NUMextremum_u (soundFrame);
	soundFrame  *=  windowFunction.get();
}

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( (windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING) ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth);
	return physicalAnalysisWidth;
}

static void windowShape_into_VEC_GAUSSIAN2_OLD (kSound_windowShape /* windowShape*/ , VEC inout_window) {
	const integer size = inout_window.size;
	const double imid = 0.5 * (double) (size + 1);
	double edge, onebyedge, factor;

	edge = exp (-12.0);
	onebyedge = 1.0 / (1.0 - edge);
	for (integer i = 1; i <= size; i ++) {
		const double phase = ((double) i - imid) / (size + 1); // 
		inout_window [i] = (exp (-48.0 * phase * phase) - edge) * onebyedge;
	}
}

void SoundFrameIntoSampledFrame_init (mutableSoundFrameIntoSampledFrame me, constSound input, mutableSampled output,
	double effectiveAnalysisWidth, kSound_windowShape windowShape)
{
	SampledFrameIntoSampledFrame_init (me, output);
	my sound = input;
	my windowShape = windowShape;
	my physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	my getSoundFrameSize = getSoundFrameSize;
	my soundFrameSize = my getSoundFrameSize (my physicalAnalysisWidth, input -> dx);
	my windowFunction = raw_VEC (my soundFrameSize);
	windowShape_into_VEC (my windowShape, my windowFunction.get());
	my frameAsSound = Sound_create (1_integer, 0.0, my soundFrameSize * input -> dx, my soundFrameSize, input -> dx, 0.5 * input -> dx); //
	my soundFrame = my frameAsSound -> z.row (1);
	Melder_assert (my soundFrame.size == my soundFrameSize);
}

/* End of file SoundFrameIntoSampledFrame.cpp */
