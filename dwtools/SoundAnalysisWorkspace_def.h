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

/*
	These forwad declarations are needed because we define new classes with reference to each other
*/
typedef struct structExtraAnalysisData *ExtraAnalysisData; 
typedef const struct structExtraAnalysisData *constExtraAnalysisData;
typedef autoSomeThing<structExtraAnalysisData> autoExtraAnalysisData;

#define ooSTRUCT WorkvectorPool
oo_DEFINE_CLASS (WorkvectorPool, Daata)

	oo_INTEGER (poolMemorySize)
	oo_INTEGER (numberOfVectors)
	oo_INTVEC (vectorSizes, numberOfVectors)
	oo_MAT (domains, numberOfVectors, 2)
	oo_VEC (memoryPool, poolMemorySize)
	#if oo_DECLARING
		VEC getRawVEC (integer index, integer size) {
			Melder_assert (index > 0 && index <= numberOfVectors);
			Melder_assert (size <= vectorSizes [index]);
			return memoryPool.part (domains [index][1], domains [index][2]);
		}
		MAT getRawMAT (integer index, integer nrow, integer ncol) {
			Melder_assert (index > 0 && index <= numberOfVectors);
			const integer wantedSize = nrow * ncol;
			Melder_assert (wantedSize <= vectorSizes [index]);
			VEC pool = memoryPool.part (domains [index][1], domains [index][2]);
			return pool.asmatrix (nrow, ncol);
		}
	#endif

oo_END_CLASS (WorkvectorPool)
#undef ooSTRUCT

#define ooSTRUCT SoundAnalysisWorkspace
oo_DEFINE_CLASS (SoundAnalysisWorkspace, Daata)

	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Sound, sound)
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Sampled, result)
	
	oo_BOOLEAN (useMultiThreading)
	oo_INTEGER (minimumNumberOfFramesPerThread) // 50 ?
	
	oo_DOUBLE (physicalAnalysisWidth)
	oo_INTEGER (analysisFrameSize)
	oo_VEC (analysisFrame, analysisFrameSize)
	oo_ENUM (kSound_windowShape, windowShape)
	oo_VEC (windowFunction, analysisFrameSize)
	oo_BOOLEAN (subtractLocalMean)
	oo_BOOLEAN (frameAnalysisIsOK)
	oo_INTEGER (frameErrorCount)
	oo_INTEGER (currentFrame)
	
	oo_OBJECT (WorkvectorPool, 0, workvectorPool)
	
	oo_OBJECT (ExtraAnalysisData, 0, extraAnalysisData)
	
	#if oo_DECLARING
		void (*analyseOneFrame) (SoundAnalysisWorkspace me, integer iframe); // has to set frameAnalysisIsOK to true or false
		void (*allocateSampledFrames) (SoundAnalysisWorkspace me);
	#endif
	#if oo_COPYING
		thy analyseOneFrame = our analyseOneFrame;
		thy allocateSampledFrames = our allocateSampledFrames;
	#endif	
		
	#if oo_DECLARING
		
		void analyseManyFrames (SoundAnalysisWorkspace me, integer fromFrame, integer toFrame);

		/*
			Make the analysis frame size always uneven. This guarantees that in case of windowing
			the sample at the center gets weight 1.0
		*/
		integer getAnalysisFrameSize_uneven (SoundAnalysisWorkspace me, double approximatePhysicalAnalysisWidth);
		
		virtual void getSoundFrame (SoundAnalysisWorkspace me, integer iframe);
		
	#endif

oo_END_CLASS (SoundAnalysisWorkspace)
#undef ooSTRUCT


#define ooSTRUCT ExtraAnalysisData
oo_DEFINE_CLASS (ExtraAnalysisData, Daata)
	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (SoundAnalysisWorkspace, soundAnalysisWorkspace)
	oo_OBJECT (WorkvectorPool, 0, workvectorPool)
oo_END_CLASS (ExtraAnalysisData)
#undef ooSTRUCT

/* End of file SoundAnalysisWorkspace_def.h */
 
