/* SoundAnalysisWorkspace.cpp
 *
 * Copyright (C) 2024 David Weenink
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

#include "SoundAnalysisWorkspace.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundAnalysisWorkspace_def.h"

Thing_implement (SoundAnalysisWorkspace, Daata, 0);

// TODO should physicalAnalysisWidth be modified to (2*halfFrameSamples + 1)*dx?
integer structSoundAnalysisWorkspace :: getSoundFrameSize_uneven (double approximatePhysicalAnalysisWidth) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / input -> dx);
	return 2 * halfFrameSamples + 1;
}

void structSoundAnalysisWorkspace :: getInputFrame () {
	if (! inputObjectPresent) return;
	constSound sound = reinterpret_cast<constSound> (input);
	const double midTime = Sampled_indexToX (output, currentFrame);
	const integer soundCentreSampleNumber = Sampled_xToNearestIndex (input, midTime);   // time accuracy is half a sampling period
	integer soundIndex = soundCentreSampleNumber - soundFrameSize / 2;
	for (integer isample = 1; isample <= soundFrame.size; isample ++, soundIndex ++) {
		soundFrame [isample] = ( soundIndex > 0 && soundIndex <= input -> nx ? sound -> z [1] [soundIndex] : 0.0 );
	}
	if (subtractFrameMean)
		centre_VEC_inout (soundFrame.get(), nullptr);
	soundFrame.get()  *=  windowFunction.get();
}

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( (windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING) ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth);
	return physicalAnalysisWidth;
}

void SoundAnalysisWorkspace_init (mutableSoundAnalysisWorkspace me, constSound input, mutableSampled output, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	Sampled_assertEqualDomains (input, output);
	SampledAnalysisWorkspace_init (me, input, output);
	my windowShape = windowShape;
	my physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	my soundFrameSize = my getSoundFrameSize_uneven (my physicalAnalysisWidth);
	my soundFrame = raw_VEC (my soundFrameSize);
	my windowFunction = raw_VEC (my soundFrameSize);
	windowShape_into_VEC (windowShape, my windowFunction.get());
}

autoSoundAnalysisWorkspace SoundAnalysisWorkspace_create (constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	try {
		autoSoundAnalysisWorkspace me = Thing_new (SoundAnalysisWorkspace);
		SoundAnalysisWorkspace_init (me.get(), thee, him, effectiveAnalysisWidth, windowShape);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundAnalysisWorkspace not created.");
	}
}

void SoundAnalysisWorkspace_analyseThreaded (mutableSoundAnalysisWorkspace me, constSound thee, double preEmphasisFrequency) {
	Melder_assert (my input == thee);
	
	autoSound sound;
	const double emphasisFactor = Sound_computeEmphasisFactor (thee, preEmphasisFrequency);
	if (emphasisFactor != 0.0) {   // OPTIMIZE; will happen for cut-off frequencies above 119 times the sampling frequency
		sound = Data_copy (thee);
		Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		SampledAnalysisWorkspace_replaceInput (me, sound.get());
	}
	SampledAnalysisWorkspace_analyseThreaded (me);
}

/* End of file SoundAnalysisWorkspace.cpp */
