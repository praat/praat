/* SampledFrameIntoSampledFrame_def.h
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
	
/*
	A separate deep copy of the SampledIntoSampled is needed for each thread
*/

#define ooSTRUCT SampledFrameIntoSampledFrame
oo_DEFINE_CLASS (SampledFrameIntoSampledFrame, Daata)
	oo_INTEGER (startFrame)
	oo_INTEGER (numberOfFrames)
	oo_INTEGER (currentFrame)				// the frame we are working on
	oo_INTEGER (frameAnalysisInfo)			// signals different "error" conditions etc in a frame analysis
	oo_BOOLEAN (frameAnalysisIsOK)			// signals whether the analysis is OK or not on the basis of the frameAnalysisInfo
	oo_INTEGER (framesErrorCount)
	oo_INTVEC (framesInfo, numberOfFrames)
	oo_DOUBLE (tol1)
	
	#if oo_DECLARING

		virtual void getInputFrame (void);
		
		virtual bool inputFrameToOutputFrame (void); // the analysis, also sets the frameAnalysisInfo and also frameAnalysisIsOK
		
		virtual void saveOutputFrame (void);
		
		virtual void inputFramesToOutputFrames (integer fromFrame, integer toFrame); // sets currentFrame

		virtual void allocateOutputFrames (void);

	#endif
oo_END_CLASS (SampledFrameIntoSampledFrame)
#undef ooSTRUCT

/* End of file SampledInToSampledWorkspace_def.h */
 
