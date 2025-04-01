/* SoundFrameIntoSampledFrame_def.h
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

#define ooSTRUCT SoundFrameIntoSampledFrame
oo_DEFINE_CLASS (SoundFrameIntoSampledFrame, SampledFrameIntoSampledFrame)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Sound, sound)
	oo_DOUBLE (physicalAnalysisWidth) 			// depends on the effectiveAnalysiswidth and the window window shape
	oo_INTEGER (soundFrameSize) 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	oo_VEC (soundFrame, soundFrameSize)			// the sound samples to analyse TODO row from oo_MAT for multi channel
	oo_BOOLEAN (subtractFrameMean)				// if true, the frame mean will be subtracted before the windowing operation
	oo_DOUBLE (soundFrameExtremum)				// the largest amplitude in the sound frame either positive or negative
	oo_ENUM (kSound_windowShape, windowShape)	// Type: Rectangular, triangular, hamming, etc..
	oo_VEC (windowFunction, soundFrameSize)		// the actual window used

	#if oo_DECLARING
	
		void getInputFrame (void) override;
		integer (*getSoundFrameSize) (double approximatePhysicalAnalysisWidth, double samplingPeriod);

	#endif

oo_END_CLASS (SoundFrameIntoSampledFrame)
#undef ooSTRUCT

/* End of file SoundFrameIntoSampledFrame_def.h */
