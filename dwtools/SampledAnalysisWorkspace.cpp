/* SampledAnalysisWorkspace.cpp
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

#include "SampledAnalysisWorkspace.h"
#include "Sound_extensions.h"
#include <thread>
#include <atomic>
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_COPY.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SampledAnalysisWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledAnalysisWorkspace_def.h"

Thing_implement (SampledAnalysisWorkspace, Daata, 0);

void structSampledAnalysisWorkspace :: getInputFrame (integer /* iframe */) {
	return;
}

bool structSampledAnalysisWorkspace :: inputFrameToOutputFrame () {
	return true;
}

void structSampledAnalysisWorkspace :: saveOutputFrame () {
	return;
}

void structSampledAnalysisWorkspace :: inputFramesToOutputFrames (integer fromFrame, integer toFrame) {
	globalFrameErrorCount = 0;
	for (integer iframe = fromFrame; iframe <= toFrame; iframe ++) {
		currentFrame = iframe;
		getInputFrame (iframe);
		if (! inputFrameToOutputFrame ())
			globalFrameErrorCount ++;
		if (saveOutput)
			saveOutputFrame ();
	}	
}

void SampledAnalysisWorkspace_initWorkvectorPool (mutableSampledAnalysisWorkspace me, constINTVEC const& vectorSizes) {
	Melder_assert (vectorSizes.size > 0);
	my workvectorPool = WorkvectorPool_create (vectorSizes, true);
}


void SampledAnalysisWorkspace_init (mutableSampledAnalysisWorkspace me, constSampled input, mutableSampled output) {
	Sampled_assertEqualDomains (input, output);
	my input = input;
	my output = output;
	my saveOutput = true;
	my useMultiThreading = ( Melder_debug != -8 ? true : false );
	my minimumNumberOfFramesPerThread = 40;
}

autoSampledAnalysisWorkspace SampledAnalysisWorkspace_create (constSound input, mutableSampled output) {
	try {
		autoSampledAnalysisWorkspace me = Thing_new (SampledAnalysisWorkspace);
		SampledAnalysisWorkspace_init (me.get(), input, output);
		return me;
	} catch (MelderError) {
		Melder_throw (U"SampledAnalysisWorkspace not created.");
	}
}

void SampledAnalysisWorkspace_replaceInput (mutableSampledAnalysisWorkspace me, constSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my input, thee);
	my input = thee;
}

void SampledAnalysisWorkspace_replaceOutput (mutableSampledAnalysisWorkspace me, mutableSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my output, thee);
	my output = thee;
}


void SampledAnalysisWorkspace_getThreadingInfo (constSampledAnalysisWorkspace me, integer *out_numberOfThreads) {
	const integer numberOfProcessors = std::thread::hardware_concurrency ();
	/*
		Our processes are compute bound, therefore it probably makes no sense to start more than two threads on one processor
	*/
	integer maximumNumberOfThreads = 2 * numberOfProcessors;
	if (my maximumNumberOfThreads > 0)
		 maximumNumberOfThreads = std::min (my maximumNumberOfThreads, maximumNumberOfThreads);
	const integer numberOfFrames = my output -> nx;
	integer numberOfThreads = 1 + (numberOfFrames - 1) / my minimumNumberOfFramesPerThread;
	Melder_clip (1_integer, & numberOfThreads, maximumNumberOfThreads);
	if (out_numberOfThreads)
		*out_numberOfThreads = numberOfThreads;
}

void SampledAnalysisWorkspace_analyseThreaded (mutableSampledAnalysisWorkspace me)
{
	try {

		my allocateOutputFrames ();

		const integer numberOfFrames = my output -> nx;
		
		std::atomic<integer> globalFrameErrorCount (0);
		
		if (my useMultiThreading) {
			integer numberOfThreads;
			SampledAnalysisWorkspace_getThreadingInfo (me, & numberOfThreads);
			const integer numberOfFramesPerThread = my minimumNumberOfFramesPerThread;
			/*
				We have to reserve all the needed working memory for each thread beforehand.
			*/
			OrderedOf<structSampledAnalysisWorkspace> workspaces;
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				autoSampledAnalysisWorkspace threadWorkspace = Data_copy (me);
				workspaces.addItem_move (threadWorkspace.move());
			}
		
			autovector<std::thread> threads = autovector<std::thread> (numberOfThreads, MelderArray::kInitializationType::ZERO);
			
			try {
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
					SampledAnalysisWorkspace threadWorkspace = workspaces.at [ithread];
					const integer firstFrame = 1 + (ithread - 1) * numberOfFramesPerThread;
					const integer lastFrame = ( ithread == numberOfThreads ? numberOfFrames : firstFrame + numberOfFramesPerThread - 1 );
					
					auto analyseFrames = [&globalFrameErrorCount] (SampledAnalysisWorkspace threadWorkspace, integer fromFrame, integer toFrame) {
						threadWorkspace -> inputFramesToOutputFrames (fromFrame, toFrame);
						globalFrameErrorCount += threadWorkspace -> globalFrameErrorCount;
					};

					threads [ithread] = std::thread (analyseFrames, threadWorkspace, firstFrame, lastFrame);
				}
				
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
					threads [ithread]. join ();
				
			} catch (MelderError) {
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++)
					if (threads [ithread]. joinable ())
						threads [ithread]. join ();
				Melder_clearError ();
				throw;
			}
			my globalFrameErrorCount = globalFrameErrorCount;
		} else {
			my inputFramesToOutputFrames (1, numberOfFrames); // no threading
		}
	} catch (MelderError) {
		Melder_throw (me, U"The Sampled analysis could not be done.");
	}
}

/* End of file SampledAnalysisWorkspace.cpp */
