/* SoundAnalysisWorkspace_def.h
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

#define ooSTRUCT SoundAnalysisWorkspace
oo_DEFINE_CLASS (SoundAnalysisWorkspace, SampledAnalysisWorkspace)

	oo_DOUBLE (physicalAnalysisWidth) 			// depends on the effectiveAnalysiswidth and the window window shape
	oo_INTEGER (analysisFrameSize) 				// determined by the physicalAnalysisWidth and the samplingFrequency of the Sound
	oo_VEC (analysisFrame, analysisFrameSize)	// the sound samples to analyse
	oo_BOOLEAN (subtractFrameMean)				// if true, the frame mean will be subtracted before the windowing operation
	oo_ENUM (kSound_windowShape, windowShape)	// Type: Rectangular, triangular, hamming, etc..
	oo_VEC (windowFunction, analysisFrameSize)

	#if oo_DECLARING
		/*
			Make the analysis frame size always uneven. This guarantees that in case of windowing
			the sample at the center gets weight 1.0
		*/
		integer getAnalysisFrameSize_uneven (constSoundAnalysisWorkspace me, double approximatePhysicalAnalysisWidth);
		
	#endif

oo_END_CLASS (SoundAnalysisWorkspace)
#undef ooSTRUCT

/* End of file SoundAnalysisWorkspace_def.h */
