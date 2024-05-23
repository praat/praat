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
#include <thread>
#include <atomic>
#include <functional>
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

Thing_implement (WorkvectorPool, Daata, 0);

autoWorkvectorPool WorkvectorPool_create (INTVEC const& vectorSizes) {
	try {
		autoWorkvectorPool me = Thing_new (WorkvectorPool);
		my numberOfVectors = vectorSizes.size;
		Melder_assert (my numberOfVectors > 0);

		my poolMemorySize = 0;
		my domains = raw_MAT (my numberOfVectors, 2);
		integer vecstart = 1;
		for (integer ivec = 1; ivec <= my numberOfVectors; ivec ++) {
			const integer vecsize = vectorSizes [ivec];
			Melder_assert (vecsize > 0);
			my poolMemorySize += vecsize;
			my domains [ivec] [1] = vecstart;
			my domains [ivec] [2] = vecstart + vecsize - 1;
			vecstart = my domains [ivec] [2] + 1;
		}
		my vectorSizes = copy_INTVEC (vectorSizes);
		my memoryPool = raw_VEC (my poolMemorySize);
		return me;
	} catch (MelderError) {
		Melder_throw (U"WorkvectorPool not created.");
	}	
}

void SoundAnalysisWorkspace_initWorkvectorPool (SoundAnalysisWorkspace me, INTVEC const& vectorSizes) {
	try {
		Melder_assert (vectorSizes.size > 0);
		my workvectorPool = WorkvectorPool_create (vectorSizes);
	} catch (MelderError) {
		Melder_throw (U"Could not initialize the WorkvectorPool.");
	}
}
Thing_implement (ExtraAnalysisData, Daata, 0);

void ExtraAnalysisData_init (ExtraAnalysisData me, SoundAnalysisWorkspace thee) {
	Melder_assert (my soundAnalysisWorkspace != nullptr);
	my soundAnalysisWorkspace = thee;
}

Thing_implement (SoundAnalysisWorkspace, Daata, 0);

void structSoundAnalysisWorkspace :: analyseManyFrames (SoundAnalysisWorkspace me, integer fromFrame, integer toFrame) {
	my frameErrorCount = 0;
	for (integer iframe = fromFrame; iframe <= toFrame; iframe ++) {
		my currentFrame = iframe;
		my getSoundFrame (me, iframe);
		my analysisFrame.get()  *=  my windowFunction.get();
		my analyseOneFrame (me, iframe);
		if (! my frameAnalysisIsOK)
			my frameErrorCount ++;
	}	
}

integer structSoundAnalysisWorkspace :: getAnalysisFrameSize_uneven (SoundAnalysisWorkspace me, double approximatePhysicalAnalysisWidth) {
	const double halfFrameDuration = 0.5 * approximatePhysicalAnalysisWidth;
	const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / my sound -> dx);
	return 2 * halfFrameSamples + 1;
}

void structSoundAnalysisWorkspace :: getSoundFrame (SoundAnalysisWorkspace me, integer iframe) {
	const double midTime = Sampled_indexToX (my result, iframe);
	const integer soundCentreSampleNumber = Sampled_xToNearestIndex (my sound, midTime);   // time accuracy is half a sampling period
	integer soundIndex = soundCentreSampleNumber - my analysisFrameSize / 2;
	for (integer isample = 1; isample <= my analysisFrame.size; isample ++, soundIndex ++) {
		my analysisFrame [isample] = ( soundIndex > 0 && soundIndex <= my sound -> nx ? my sound -> z [1] [soundIndex] : 0.0 );
	}
	if (my subtractLocalMean)
		centre_VEC_inout (my analysisFrame.get(), nullptr);
}

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	const double physicalAnalysisWidth = ( (windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING) ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth);
	return physicalAnalysisWidth;
}

void SoundAnalysisWorkspace_init (SoundAnalysisWorkspace me, Sound thee, Sampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	Melder_assert (thy xmin == his xmin && thy xmax == his xmax); // equal domains
	my sound = thee;
	my result = him;
	my useMultiThreading = true;
	my minimumNumberOfFramesPerThread = 40;
	my windowShape = windowShape;
	my physicalAnalysisWidth = getPhysicalAnalysisWidth (effectiveAnalysisWidth, windowShape);
	my analysisFrameSize = my getAnalysisFrameSize_uneven (me, my physicalAnalysisWidth);
	my analysisFrame = raw_VEC (my analysisFrameSize);
	my windowFunction = raw_VEC (my analysisFrameSize);
	windowShape_into_VEC (windowShape, my windowFunction.get());
}

autoSoundAnalysisWorkspace SoundAnalysisWorkspace_create (Sound thee, Sampled him, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
	try {
		autoSoundAnalysisWorkspace me = Thing_new (SoundAnalysisWorkspace);
		SoundAnalysisWorkspace_init (me.get(), thee, him, effectiveAnalysisWidth, windowShape);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SoundAnalysisWorkspace not created.");
	}
}

void SoundAnalysisWorkspace_replaceSound (SoundAnalysisWorkspace me, Sound thee) {
	Melder_assert (my sound -> xmin == thy xmin && my sound -> xmax == thy xmax);
	Melder_assert (my sound -> x1 == thy x1 && my sound -> nx == thy nx);
	Melder_assert (my sound -> dx == thy dx);
	my sound = thee;
}


void SoundAnalysisWorkspace_getThreadingInfo (SoundAnalysisWorkspace me, integer *out_numberOfThreads) {
	const integer numberOfProcessors = std::thread::hardware_concurrency ();
	/*
		Our processes are compute bound, therefore it makes no sense to start more than two threads on one processor
	*/
	if (my minimumNumberOfFramesPerThread <= 0)
		my minimumNumberOfFramesPerThread = 40;
	const integer maximumNumberOfThreads = 2 * numberOfProcessors;
	const integer numberOfFrames = my result -> nx;
	integer numberOfThreads = (numberOfFrames - 1) / my minimumNumberOfFramesPerThread + 1;
	Melder_clip (1_integer, & numberOfThreads, maximumNumberOfThreads);
	if (out_numberOfThreads)
		*out_numberOfThreads = numberOfThreads;
}

void SoundAnalysisWorkspace_analyseThreaded (SoundAnalysisWorkspace me, Sound thee, double preEmphasisFrequency)
{
	try {
		autoSound sound;
		const double nyQuistFrequency = 0.5 / thy dx;
		if (preEmphasisFrequency < nyQuistFrequency) {
			sound = Data_copy (thee);
			Sound_preEmphasis (sound.get(), preEmphasisFrequency);
			SoundAnalysisWorkspace_replaceSound (me, sound.get());
		}	

		my allocateSampledFrames (me);

		const integer numberOfFrames = my result -> nx;
		
		std::atomic<integer> frameErrorCount (0);
		
		if (my useMultiThreading) {
			integer numberOfThreads;
			SoundAnalysisWorkspace_getThreadingInfo (me, & numberOfThreads);
			const integer numberOfFramesPerThread = my minimumNumberOfFramesPerThread;
			/*
				We have to reserve all the needed working memory for each thread beforehand.
			*/
			OrderedOf<structSoundAnalysisWorkspace> workspaces;
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				autoSoundAnalysisWorkspace threadWorkspace = Data_copy (me);
				workspaces.addItem_move (threadWorkspace.move());
			}
		
			autovector<std::thread> threads = autovector<std::thread> (numberOfThreads, MelderArray::kInitializationType::ZERO);
			
			try {
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
					SoundAnalysisWorkspace threadWorkspace = workspaces.at [ithread];
					const integer firstFrame = 1 + (ithread - 1) * numberOfFramesPerThread;
					const integer lastFrame = ( ithread == numberOfThreads ? numberOfFrames : firstFrame + numberOfFramesPerThread - 1 );
					
					auto analyseFrames = [&frameErrorCount] (SoundAnalysisWorkspace threadWorkspace, integer fromFrame, integer toFrame) {
						threadWorkspace -> analyseManyFrames (threadWorkspace, fromFrame, toFrame);
						frameErrorCount += threadWorkspace -> frameErrorCount;
					};

					threads [ithread] = std::thread (analyseFrames, threadWorkspace, firstFrame, lastFrame);
				}
				
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
					threads [ithread]. join ();
			} catch (MelderError) {
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
					if (threads [ithread]. joinable ())
						threads [ithread]. join ();
					}
				Melder_clearError ();
				throw;
			}
		} else {
			my analyseManyFrames (me, 1, numberOfFrames);
			frameErrorCount += my frameErrorCount;
		}
	} catch (MelderError) {
			Melder_throw (me, U"The sound analysis could not be done.");
	}
}

/* End of file SoundAnalysisWorkspace.cpp */
