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

#include "Preferences.h"
#include "SampledToSampledWorkspace.h"
#include "Sound_and_LPC.h"
#include "Sound_extensions.h"
#include <thread>
#include <atomic>
#include "NUM2.h"
#include "melder_str32.h"

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


static struct ThreadingPreferences {
	bool useMultiThreading = true;
	integer numberOfConcurrentThreadsAvailable = 20;
	integer numberOfConcurrentThreadsToUse = 20;
	integer maximumNumberOfFramesPerThread = 0; // 0: signals no limit
	integer minimumNumberOfFramesPerThread = 40;
} preferences;

void SampledToSampledWorkspace_preferences () {
	Preferences_addBool    (U"SampledToSampledWorkspace.useMultiThreading", & preferences.useMultiThreading, true);
	Preferences_addInteger (U"SampledToSampledWorkspace.numberOfConcurrentThreadsAvailable", & preferences.numberOfConcurrentThreadsAvailable, 20);
	Preferences_addInteger (U"SampledToSampledWorkspace.numberOfConcurrentThreadsToUse", & preferences.numberOfConcurrentThreadsToUse, 20);
	Preferences_addInteger (U"SampledToSampledWorkspace.maximumNumberOfFramesPerThread", & preferences.maximumNumberOfFramesPerThread, 40);
	Preferences_addInteger (U"SampledToSampledWorkspace.minimumNumberOfFramesPerThread", & preferences.maximumNumberOfFramesPerThread, 40);
}

bool SampledToSampledWorkspace_useMultiThreading () {
	return preferences.useMultiThreading;
}

void SampledToSampledWorkspace_setMultiThreading (bool useMultiThreading) {
	preferences.useMultiThreading = useMultiThreading;
}
integer SampledToSampledWorkspace_getNumberOfConcurrentThreadsAvailable () {
	return std::thread::hardware_concurrency ();
}

conststring32 SampledToSampledWorkspace_getNumberOfConcurrentThreadsAvailableInfo () {
	static char32 threadingInfoString [80];
	MelderString info;
	MelderString_append (& info, U"The maximum number of concurrent threads available on your machine is ",
		Melder_integer (SampledToSampledWorkspace_getNumberOfConcurrentThreadsAvailable ()), U".");
	str32cpy (threadingInfoString, info.string);
	MelderString_free (& info);
	return threadingInfoString;
}

integer SampledToSampledWorkspace_getNumberOfConcurrentThreadsToUse () {
	return preferences.numberOfConcurrentThreadsToUse;
}

void SampledToSampledWorkspace_setNumberOfConcurrentThreadsToUse (integer numberOfConcurrentThreadsToUse) {
	Melder_require (numberOfConcurrentThreadsToUse <= preferences.numberOfConcurrentThreadsAvailable,
		U"The number of threads to use should not exceed the number of concurrent threads available (",
			preferences.numberOfConcurrentThreadsAvailable, U"),");
	preferences.numberOfConcurrentThreadsToUse = numberOfConcurrentThreadsToUse;
}

integer SampledToSampledWorkspace_getMaximumNumberOfFramesPerThread () {
	return preferences.maximumNumberOfFramesPerThread;
}

void SampledToSampledWorkspace_setMaximumNumberOfFramesPerThread (integer maximumNumberOfFramesPerThread) {
	preferences.maximumNumberOfFramesPerThread = maximumNumberOfFramesPerThread;
}

integer SampledToSampledWorkspace_getMinimumNumberOfFramesPerThread () {
	return preferences.minimumNumberOfFramesPerThread;
}

void SampledToSampledWorkspace_setMinimumNumberOfFramesPerThread (integer minimumNumberOfFramesPerThread) {
	preferences.minimumNumberOfFramesPerThread = minimumNumberOfFramesPerThread;
}

void SampledToSampledWorkspace_getThreadingInfo (constSampledToSampledWorkspace me, integer *out_numberOfThreadsNeeded, integer *out_numberOfFramesPerThread) {
	const integer numberOfConcurrentThreadsAvailable = SampledToSampledWorkspace_getNumberOfConcurrentThreadsAvailable ();
	const integer numberOfConcurrentThreadsToUse = SampledToSampledWorkspace_getNumberOfConcurrentThreadsToUse ();
	const integer minimumNumberOfFramesPerThread = SampledToSampledWorkspace_getMinimumNumberOfFramesPerThread ();
	const integer maximumNumberOfFramesPerThread = SampledToSampledWorkspace_getMaximumNumberOfFramesPerThread ();
	const integer numberOfFrames = my output -> nx;
	integer numberOfThreads = 0, numberOfFramesPerThread = numberOfFrames;
	if (SampledToSampledWorkspace_useMultiThreading () && numberOfConcurrentThreadsToUse > 0) {
		numberOfFramesPerThread = Melder_iroundUp ((double) numberOfFrames / numberOfConcurrentThreadsToUse);
		if (maximumNumberOfFramesPerThread > 0)
			numberOfFramesPerThread = std::min (numberOfFramesPerThread, maximumNumberOfFramesPerThread);
		if (minimumNumberOfFramesPerThread > 0)
			numberOfFramesPerThread = std::max (numberOfFramesPerThread, minimumNumberOfFramesPerThread);
		numberOfThreads = Melder_iroundUp ((double) numberOfFrames / numberOfFramesPerThread);
		numberOfThreads = std::max (1L, numberOfThreads);
	}
	if (out_numberOfThreadsNeeded)
		*out_numberOfThreadsNeeded = numberOfThreads;
	if (out_numberOfFramesPerThread)
		*out_numberOfFramesPerThread = numberOfFramesPerThread;
}

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
}

void SampledToSampledWorkspace_replaceInput (mutableSampledToSampledWorkspace me, constSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my input, thee);
	my input = thee;
}

void SampledToSampledWorkspace_replaceOutput (mutableSampledToSampledWorkspace me, mutableSampled thee) {
	Sampled_assertEqualDomainsAndSampling (my output, thee);
	my output = thee;
}

void SampledToSampledWorkspace_analyseThreaded (mutableSampledToSampledWorkspace me)
{
	try {

		my allocateOutputFrames ();

		const integer numberOfFrames = my output -> nx;
		
		std::atomic<integer> globalFrameErrorCount (0);
		
		if (SampledToSampledWorkspace_useMultiThreading ()) {
			integer numberOfThreadsNeeded, numberOfFramesPerThread;
			SampledToSampledWorkspace_getThreadingInfo (me, & numberOfThreadsNeeded, & numberOfFramesPerThread);

			/*
				We need to reserve all the working memory for each thread beforehand.
			*/
			const integer numberOfThreadsToUse = SampledToSampledWorkspace_getNumberOfConcurrentThreadsToUse ();
			const integer numberOfThreads = std::min (numberOfThreadsToUse, numberOfThreadsNeeded);
			OrderedOf<structSampledToSampledWorkspace> workspaces;
			for (integer ithread = 1; ithread <= numberOfThreads; ithread ++) {
				autoSampledToSampledWorkspace threadWorkspace = Data_copy (me);
				workspaces.addItem_move (threadWorkspace.move());
			}
		
			autovector<std::thread> threads = autovector<std::thread> (numberOfThreads, MelderArray::kInitializationType::ZERO);
			integer numberOfThreadsInRun;
			try {
				const integer numberOfThreadRuns = Melder_iroundUp ((double) numberOfThreadsNeeded / numberOfThreads);
				const integer numberOfFramesInRun = numberOfThreads * numberOfFramesPerThread;
				const integer remainingThreads = numberOfThreadsNeeded % numberOfThreads;
				const integer numberOfThreadsInLastRun = ( remainingThreads == 0 ? numberOfThreads : remainingThreads);
				for (integer irun = 1; irun <= numberOfThreadRuns; irun ++) {
					numberOfThreadsInRun = ( irun < numberOfThreadRuns ? numberOfThreads : numberOfThreadsInLastRun );
					const integer lastFrameInRun = ( irun < numberOfThreadRuns ? numberOfFramesInRun * irun : numberOfFrames);
					for (integer ithread = 1; ithread <= numberOfThreadsInRun; ithread ++) {
						SampledToSampledWorkspace threadWorkspace = workspaces.at [ithread];
						const integer firstFrame = numberOfFramesInRun * (irun - 1) + 1 + (ithread - 1) * numberOfFramesPerThread;
						const integer lastFrame = ( ithread == numberOfThreadsInRun ? lastFrameInRun : firstFrame + numberOfFramesPerThread - 1 );
						
						auto analyseFrames = [&globalFrameErrorCount] (SampledToSampledWorkspace threadWorkspace, integer fromFrame, integer toFrame) {
							threadWorkspace -> inputFramesToOutputFrames (fromFrame, toFrame);
							globalFrameErrorCount += threadWorkspace -> globalFrameErrorCount;
						};

						threads [ithread] = std::thread (analyseFrames, threadWorkspace, firstFrame, lastFrame);
					}
					for (integer ithread = 1; ithread <= numberOfThreadsInRun; ithread ++)
						threads [ithread]. join ();
				}
			} catch (MelderError) {
				for (integer ithread = 1; ithread <= numberOfThreadsInRun; ithread ++)
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

/*
	Performs timing of a number of scenarios for multi-threading.
	This timing is performed on the LPC analysis with the Burg algorithm on a sound file of a given duration
	and a sampling frequency of 11000 Hz.
	The workspace for the Burg algorithm needs more memory for its analyses than the other LPC algorithms (it needs
	n samples for the windowed sound frame and at least 2 vectors of length n for buffering).
	It varies the number of threads from 1 to the maximum number of concurrency available on the hardware.
	It varies, for each number of threads separately, the frame sizes (50, 100, 200, 400, 800, 1600, 3200)
	The data is represented in the info window as a space separated table with 4 columns:
	duration(s) nThread nFrames/thread toLPC(s)
	Saving this data, except for the last line, as a csv file and next reading this file as a Table,
	the best way to show the results would be
	Table > Scatter plot: "nFrames/thread", 0, 0, toLPC(s), 0, 0, nThread, 8, "yes"
*/
void timeMultiThreading (double soundDuration) {
	/*
		Save current multi-threading situation
	*/
	struct ThreadingPreferences savedPreferences = preferences;
	try {
		Melder_require (preferences.numberOfConcurrentThreadsAvailable > 1,
			U"No multi-threading possible.");
		autoVEC framesPerThread {50, 100, 200, 400, 800, 1600, 3200};
		const integer maximumNumberOfThreads = std::thread::hardware_concurrency ();
		autoSound me = Sound_createSimple (1_integer, soundDuration, 5500.0);
		for (integer i = 1; i <= my nx; i++) {
			const double time = my x1 + (i - 1) * my dx;
			my z[1][i] = sin(2.0 * NUMpi * 377 * time) + NUMrandomGauss (0.0, 0.1);
		}
		preferences.useMultiThreading = true;
		const int predictionOrder = 10;
		const double effectiveAnalysisWidth = 0.025, dt = 0.05, preEmphasisFrequency = 50;
		autoMelderProgress progress (U"Test multi-threading times...");
		Melder_clearInfo ();
		MelderInfo_writeLine (U"duration(s) nThread nFrames/thread toLPC(s)");
		integer numberOfThreads = maximumNumberOfThreads;
		for (integer nThread = 1; nThread <= maximumNumberOfThreads; nThread ++) {
			preferences.numberOfConcurrentThreadsToUse = nThread;
			for (integer index = 1; index <= framesPerThread.size; index ++) {
				const integer numberOfFramesPerThread = framesPerThread [index];
				preferences.maximumNumberOfFramesPerThread = numberOfFramesPerThread;
				preferences.minimumNumberOfFramesPerThread = numberOfFramesPerThread;
				Melder_stopwatch ();
					autoLPC lpc = Sound_to_LPC_burg (me.get(), predictionOrder, effectiveAnalysisWidth, dt, preEmphasisFrequency);
				double t = Melder_stopwatch ();
				MelderInfo_writeLine (soundDuration, U" ", nThread, U" ", numberOfFramesPerThread, U" ", t);
			}
			MelderInfo_drain ();
			try {
				Melder_progress (((double) nThread) / maximumNumberOfThreads, U"Number of threads: ", nThread);
			} catch (MelderError) {
				numberOfThreads = nThread;
				Melder_clearError ();
				break;
			}
		}
		MelderInfo_close ();
		preferences = savedPreferences;
	} catch (MelderError) {
		preferences = savedPreferences;
		Melder_throw (U"Could not perform timing.");
	}
}

/* End of file SampledToSampledWorkspace.cpp */
