/* SoundToSampledWorkspace.cpp
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

#include "SoundToSampledWorkspace.h"
#include "Sound_extensions.h"
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_COPY.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SoundToSampledWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SoundToSampledWorkspace_def.h"

Thing_implement (SoundToSampledWorkspace, Daata, 0);

integer structSoundToSampledWorkspace :: getSoundFrameSize_uneven (double approximatePhysicalAnalysisWidth, double input_dx) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / input_dx);
	return 2 * halfFrameSamples + 1;
}

void structSoundToSampledWorkspace :: getInputFrame () {
	if (! inputObjectPresent)
		return;
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

void SoundToSampledWorkspace_init (mutableSoundToSampledWorkspace me, constSound input, mutableSampled output, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	SampledToSampledWorkspace_init (me, input, output);
	my windowShape = windowShape;
	my physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	if (! my inputObjectPresent)
		return;
	SoundToSampledWorkspace_initSoundFrame (me, my input -> dx);
}

void SoundToSampledWorkspace_initSoundFrame (mutableSoundToSampledWorkspace me, double sound_dx) {
	my soundFrameSize = my getSoundFrameSize_uneven (my physicalAnalysisWidth, sound_dx);
	my soundFrame = raw_VEC (my soundFrameSize);
	my windowFunction = raw_VEC (my soundFrameSize);
	my soundFrameVEC = my soundFrame.get();
	windowShape_into_VEC (my windowShape, my windowFunction.get());
}

autoSoundToSampledWorkspace SoundToSampledWorkspace_create (constSound thee, mutableSampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	try {
		Melder_assert (thee);
		autoSoundToSampledWorkspace me = Thing_new (SoundToSampledWorkspace);
		SoundToSampledWorkspace_init (me.get(), thee, him, effectiveAnalysisWidth, windowShape);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundToSampledWorkspace not created.");
	}
}

void SoundToSampledWorkspace_analyseThreaded (mutableSoundToSampledWorkspace me, constSound thee, double preEmphasisFrequency) {
	Melder_assert (my input == thee);
	
	autoSound sound;
	const double emphasisFactor = Sound_computeEmphasisFactor (thee, preEmphasisFrequency);
	if (emphasisFactor != 0.0) {   // OPTIMIZE; will happen for cut-off frequencies above 119 times the sampling frequency
		sound = Data_copy (thee);
		Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		SampledToSampledWorkspace_replaceInput (me, sound.get());
	}
	SampledToSampledWorkspace_analyseThreaded (me);
}

/* End of file SoundToSampledWorkspace.cpp */
