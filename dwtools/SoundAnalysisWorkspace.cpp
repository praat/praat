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

void SoundAnalysisWorkspace_initWorkvectorPool (SoundAnalysisWorkspace me, INTVEC const& vectorSizes) {
	Melder_assert (vectorSizes.size > 0);
	my workvectorPool = WorkvectorPool_create (vectorSizes, true);
}

Thing_implement (SoundAnalysisWorkspace, Daata, 0);

integer structSoundAnalysisWorkspace :: getAnalysisFrameSize_uneven (constSoundAnalysisWorkspace me, double approximatePhysicalAnalysisWidth) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / my input -> dx);
	return 2 * halfFrameSamples + 1;
}

void getInputFrame_sound (mutableSampledAnalysisWorkspace ws, integer iframe) {
	SoundAnalysisWorkspace me = reinterpret_cast<SoundAnalysisWorkspace> (ws);
	constSound sound = reinterpret_cast<constSound> (my input);
	const double midTime = Sampled_indexToX (my output, iframe);
	const integer soundCentreSampleNumber = Sampled_xToNearestIndex (my input, midTime);   // time accuracy is half a sampling period
	integer soundIndex = soundCentreSampleNumber - my analysisFrameSize / 2;
	for (integer isample = 1; isample <= my analysisFrame.size; isample ++, soundIndex ++) {
		my analysisFrame [isample] = ( soundIndex > 0 && soundIndex <= my input -> nx ? sound -> z [1] [soundIndex] : 0.0 );
	}
	if (my subtractFrameMean)
		centre_VEC_inout (my analysisFrame.get(), nullptr);
	my analysisFrame.get()  *=  my windowFunction.get();
}

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( (windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING) ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth);
	return physicalAnalysisWidth;
}

void SoundAnalysisWorkspace_init (mutableSoundAnalysisWorkspace me, constSound input, mutableSampled output, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	Melder_assert (input -> xmin == output -> xmin && input -> xmax == output -> xmax); // equal domains
	SampledAnalysisWorkspace_init (me, input, output);
	my windowShape = windowShape;
	my getInputFrame = getInputFrame_sound;
	my physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	my analysisFrameSize = my getAnalysisFrameSize_uneven (me, my physicalAnalysisWidth);
	my analysisFrame = raw_VEC (my analysisFrameSize);
	my windowFunction = raw_VEC (my analysisFrameSize);
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

void SoundAnalysisWorkspace_replaceSound (mutableSoundAnalysisWorkspace me, Sound thee) {
	SampledAnalysisWorkspace_replaceInput (me, thee);
}

void SoundAnalysisWorkspace_analyseThreaded (mutableSoundAnalysisWorkspace me, constSound thee, double preEmphasisFrequency) {
	Melder_assert (my input == thee);
	
	autoSound sound;
	const double emphasisFactor = Sound_computeEmphasisFactor (thee, preEmphasisFrequency);
	if (emphasisFactor != 0.0) {   // OPTIMIZE; will happen for cut-off frequencies above 119 times the sampling frequency
		sound = Data_copy (thee);
		Sound_preEmphasize_inplace (sound.get(), preEmphasisFrequency);
		SoundAnalysisWorkspace_replaceSound (me, sound.get());
	}
	SampledAnalysisWorkspace_analyseThreaded (me);
}

/* End of file SoundAnalysisWorkspace.cpp */
