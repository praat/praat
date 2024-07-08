/* SoundToSampledWorkspace_def.h
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

#define ooSTRUCT SoundToSampledWorkspace
oo_DEFINE_CLASS (SoundToSampledWorkspace, SampledToSampledWorkspace)

	oo_DOUBLE (physicalAnalysisWidth) 			// depends on the effectiveAnalysiswidth and the window window shape
	oo_INTEGER (soundFrameSize) 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	oo_VEC (soundFrame, soundFrameSize)			// the sound samples to analyse
	oo_BOOLEAN (subtractFrameMean)				// if true, the frame mean will be subtracted before the windowing operation
	oo_ENUM (kSound_windowShape, windowShape)	// Type: Rectangular, triangular, hamming, etc..
	oo_VEC (windowFunction, soundFrameSize)

	#if oo_DECLARING
		/*
			Make the sound frame size always uneven. This guarantees that in case of windowing
			the sample at the center gets weight 1.0
		*/
		integer getSoundFrameSize_uneven (double approximatePhysicalAnalysisWidth, double samplingPeriod);

		void getInputFrame () override;

		VEC soundFrameVEC = soundFrame.get();

	#endif

	#if oo_COPYING

		thy soundFrameVEC = thy soundFrame.get();

	#endif

oo_END_CLASS (SoundToSampledWorkspace)
#undef ooSTRUCT

/* End of file SoundToSampledWorkspace_def.h */
