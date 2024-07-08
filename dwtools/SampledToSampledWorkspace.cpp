/* SampledToSampledWorkspace.cpp
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

#include "SampledToSampledWorkspace.h"
#include "Sound_extensions.h"
#include <thread>
#include <atomic>
#include "NUM2.h"

#include "oo_DESTROY.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_COPY.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_EQUAL.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_CAN_WRITE_AS_ENCODING.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_WRITE_TEXT.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_WRITE_BINARY.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_READ_TEXT.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_READ_BINARY.h"
#include "SampledToSampledWorkspace_def.h"
#include "oo_DESCRIPTION.h"
#include "SampledToSampledWorkspace_def.h"

Thing_implement (SampledToSampledWorkspace, Daata, 0);

void structSampledToSampledWorkspace :: getInputFrame (void) {
	return;
}

bool structSampledToSampledWorkspace :: inputFrameToOutputFrame (void) {
	return true;
}

void structSampledToSampledWorkspace :: saveOutputFrame (void) {
	return;
}

void structSampledToSampledWorkspace :: allocateOutputFrames (void) {
	return;
}

void structSampledToSampledWorkspace :: inputFramesToOutputFrames (integer fromFrame, integer toFrame) {
	globalFrameErrorCount = 0;
	for (integer iframe = fromFrame; iframe <= toFrame; iframe ++) {
		currentFrame = iframe;
		getInputFrame ();
		if (! inputFrameToOutputFrame ())
			globalFrameErrorCount ++;
		saveOutputFrame ();
	}	
}

void SampledToSampledWorkspace_init (mutableSampledToSampledWorkspace me, constSampled input, mutableSampled output) {
	if (input && output)
		Sampled_assertEqualDomains (input, output);
	if (input) {
		my input = input;
		my inputObjectPresent = true;
	}
	if (output) {
		my output = output;
		my outputObjectPresent = true;
	}
	my useMultiThreading = ( Melder_debug != -8 ? true : false );
	my minimumNumberOfFramesPerThread = 40;
}

void SampledToSampledWorkspace_replaceInput (mutableSampledToSampledWorkspace me, constSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my input, thee);
	my input = thee;
}

void SampledToSampledWorkspace_replaceOutput (mutableSampledToSampledWorkspace me, mutableSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my output, thee);
	my output = thee;
}


void SampledToSampledWorkspace_getThreadingInfo (constSampledToSampledWorkspace me, integer *out_numberOfThreads) {
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

void SampledToSampledWorkspace_analyseThreaded (mutableSampledToSampledWorkspace me)
{
	try {

		my allocateOutputFrames ();

		const integer numberOfFrames = my output -> nx;
		
		std::atomic<integer> globalFrameErrorCount (0);
		
		if (my useMultiThreading) {
			integer numberOfThreads;
			SampledToSampledWorkspace_getThreadingInfo (me, & numberOfThreads);
			const integer numberOfFramesPerThread = my minimumNumberOfFramesPerThread;
			/*
				We have to reserve all the needed working memory for each thread beforehand.
			*/
			OrderedOf<structSampledToSampledWorkspace> workspaces;
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				autoSampledToSampledWorkspace threadWorkspace = Data_copy (me);
				workspaces.addItem_move (threadWorkspace.move());
			}
		
			autovector<std::thread> threads = autovector<std::thread> (numberOfThreads, MelderArray::kInitializationType::ZERO);
			
			try {
				for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
					SampledToSampledWorkspace threadWorkspace = workspaces.at [ithread];
					const integer firstFrame = 1 + (ithread - 1) * numberOfFramesPerThread;
					const integer lastFrame = ( ithread == numberOfThreads ? numberOfFrames : firstFrame + numberOfFramesPerThread - 1 );
					
					auto analyseFrames = [&globalFrameErrorCount] (SampledToSampledWorkspace threadWorkspace, integer fromFrame, integer toFrame) {
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

/* End of file SampledToSampledWorkspace.cpp */
