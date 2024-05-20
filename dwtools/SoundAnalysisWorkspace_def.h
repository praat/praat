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
oo_DEFINE_CLASS (SoundAnalysisWorkspace, Daata)

//	oo_UNSAFE_BORROWED_TRANSIENT_CONST_OBJECT_REFERENCE (Sound, sound) // Needs too many casts now, TODO Sampled_getNearestIndex (constSampled,...)
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Sound, sound)
	oo_UNSAFE_BORROWED_TRANSIENT_MUTABLE_OBJECT_REFERENCE (Sampled, result)
	oo_DOUBLE (physicalAnalysisWidth)
	oo_INTEGER (analysisFrameSize)
	oo_VEC (analysisFrame, analysisFrameSize)
	oo_ENUM (kSound_windowShape, windowShape)
	oo_VEC (windowFunction, analysisFrameSize)
	oo_BOOLEAN (subtractLocalMean)
	oo_BOOLEAN (frameAnalysisIsOK)
	oo_INTEGER (frameErrorCount)
	oo_INTEGER (currentFrame)
	
	#if oo_DECLARING || oo_COPYING
		void (*analyseOneFrame) (SoundAnalysisWorkspace me, integer iframe); // also has to set frameAnalysisIsOK to true or false
	#endif
		
	#if oo_DECLARING	
		void analyseManyFrames (SoundAnalysisWorkspace me, integer fromFrame, integer toFrame);

		/*
			Make the analysis frame size always uneven. This guarantees that in case of windowing
			the sample at the center gets weight 1.0
		*/
		integer getAnalysisFrameSize_uneven (SoundAnalysisWorkspace me, double approximatePhysicalAnalysisWidth);
		
		virtual void getSoundFrame (SoundAnalysisWorkspace me, integer iframe);
		
		virtual void allocateSampledFrames (SoundAnalysisWorkspace me) {
			(void) me;
		};
	#endif

oo_END_CLASS (SoundAnalysisWorkspace)
#undef ooSTRUCT

/*
Thing_define (SoundAnalysisWorkspace, Daata) {
	Sound sound;	// a link to the sound to analyze
	Sampled result; // a link the result of the analysis
	double physicalAnalysisWidth;
	integer analysisFrameSize;
	autoVEC analysisFrame;
	bool subtractLocalMean = true;
	
	kSound_windowShape windowShape;
	autoVEC windowFunction; // size == analysisFrameSize
	
	integer currentFrame, frameErrorCount = 0;
	bool frameAnalysisIsOK;
	
	void setSubtractLocalMean (bool subtractMean) {
		subtractLocalMean = subtractMean;
	}
	
	virtual double getPhysicalAnalysisWidth (SoundAnalysisWorkspace me, double effectiveAnalysisWidth, kSound_windowShape windowShape) {
		(void) me;
		const double physicalAnalysisWidth = ( (windowShape == kSound_windowShape::RECTANGULAR ||
		windowShape == kSound_windowShape::TRIANGULAR || windowShape == kSound_windowShape::HAMMING ||
		windowShape == kSound_windowShape::HANNING) ? effectiveAnalysisWidth : 2.0 * effectiveAnalysisWidth);
		return physicalAnalysisWidth;
	}
	
	virtual integer getAnalysisFrameSize_uneven (SoundAnalysisWorkspace me, double physicalAnalysisWidth) {
		const double halfFrameDuration = 0.5 * physicalAnalysisWidth;
		const integer halfFrameSamples = Melder_ifloor (halfFrameDuration / my sound -> dx);
		return 2 * halfFrameSamples + 1;
	}
	
	virtual void getSoundFrame2 (SoundAnalysisWorkspace me, integer iframe) {
		const double t = Sampled_indexToX (my result, iframe);
		const double startTime = t - 0.5 * my physicalAnalysisWidth;
		const integer mystartIndex = Sampled_xToNearestIndex (my sound, startTime);
		for (integer isample = 1; isample <= my analysisFrame.size; isample ++) {
			const integer myindex = mystartIndex + isample - 1;
			my analysisFrame [isample] = ( myindex > 0 && myindex <= my sound -> nx ? my sound -> z [1] [myindex] : 0.0 );
		}
		if (my subtractLocalMean)
			centre_VEC_inout (my analysisFrame.get(), nullptr);
	}
	
	virtual void getSoundFrame (SoundAnalysisWorkspace me, integer iframe) {
		const double midTime = Sampled_indexToX (my result, iframe);
		const integer soundCentreSampleNumber = Sampled_xToNearestIndex (my sound, midTime);   // time accuracy is half a sampling period
		integer soundIndex = soundCentreSampleNumber - my analysisFrameSize / 2;
		for (integer isample = 1; isample <= my analysisFrame.size; isample ++, soundIndex ++) {
			my analysisFrame [isample] = ( soundIndex > 0 && soundIndex <= my sound -> nx ? my sound -> z [1] [soundIndex] : 0.0 );
		}
		if (my subtractLocalMean)
			centre_VEC_inout (my analysisFrame.get(), nullptr);
	}

	void (*analyseOneFrame) (SoundAnalysisWorkspace me, integer iframe);
	
	virtual void analyseManyFrames (SoundAnalysisWorkspace me, integer fromFrame, integer toFrame) {
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
	
	void replaceSound (Sound s) {
		Melder_assert (sound -> xmin == s -> xmin && sound -> xmax == s -> xmax);
		Melder_assert (sound -> y1 == s -> y1 && sound -> nx == s -> nx);
		Melder_assert (sound -> dx == s -> dx);
		sound = s;
	}
	
	virtual void allocateSampledFrames (SoundAnalysisWorkspace me);
	
	void v1_copy (Daata data_to) const override {
		SoundAnalysisWorkspace thee = reinterpret_cast<SoundAnalysisWorkspace> (data_to);
		thy sound = sound;
		thy result = result;
		thy physicalAnalysisWidth = physicalAnalysisWidth;
		thy analysisFrameSize = analysisFrameSize;
		thy analysisFrame = copy_VEC (analysisFrame.get());
		thy subtractLocalMean = subtractLocalMean;
		thy windowShape = windowShape;
		thy windowFunction = copy_VEC (windowFunction.get());
		thy currentFrame = currentFrame;
		thy frameErrorCount = frameErrorCount;
		thy frameAnalysisIsOK = frameAnalysisIsOK;
		thy analyseOneFrame = analyseOneFrame;
	}
};*/

void SoundAnalysisWorkspace_init (SoundAnalysisWorkspace me, Sound thee, double effectiveAnalysisWidth, kSound_windowShape windowShape);

void SoundAnalysisWorkspace_analyseThreaded (SoundAnalysisWorkspace me, Sound thee, double preEmphasisFrequency);

double getPhysicalAnalysisWidth (double effectiveAnalysisWidth, kSound_windowShape windowShape);


/* End of file SoundAnalysisWorkspace_def.h */
 
