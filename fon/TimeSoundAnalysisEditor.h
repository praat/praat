#ifndef _TimeSoundAnalysisEditor_h_
#define _TimeSoundAnalysisEditor_h_
/* TimeSoundAnalysisEditor.h
 *
 * Copyright (C) 1992-2007 Paul Boersma
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 * pb 2002/07/16 GPL
 * pb 2002/11/19 added show-widgets
 * pb 2002/11/19 added pulses
 * pb 2003/05/20 longestAnalysis replaces pitch.timeSteps, pitch.speckle, formant.maximumDuration
 * pb 2003/05/21 pitch floor and ceiling replace the view and analysis ranges
 * pb 2003/05/27 spectrogram maximum and autoscaling
 * pb 2003/08/23 formant.numberOfTimeSteps
 * pb 2003/09/16 advanced pitch settings: pitch.timeStep, pitch.timeStepsPerView, pitch.viewFrom, pitch.viewTo
 * pb 2003/09/18 advanced formant settings: formant.timeStep, formant.timeStepsPerView
 * pb 2003/10/01 time step settings: timeStepStrategy, fixedTimeStep, numberOfTimeStepsPerView
 * pb 2004/02/15 highlight methods
 * pb 2004/07/14 pulses.maximumAmplitudeFactor
 * pb 2004/10/24 intensity.averagingMethod
 * pb 2004/10/27 intensity.subtractMeanPressure
 * pb 2005/01/11 getBottomOfSoundAndAnalysisArea
 * pb 2005/06/16 units
 * pb 2005/12/07 arrowScrollStep
 * pb 2007/06/10 wchar_t
 * pb 2007/09/02 direct drawing to picture window
 */

#ifndef _FunctionEditor_h_
	#include "FunctionEditor.h"
#endif
#ifndef _Spectrogram_h_
	#include "Spectrogram.h"
#endif
#ifndef _Pitch_h_
	#include "Pitch.h"
#endif
#ifndef _Intensity_h_
	#include "Intensity.h"
#endif
#ifndef _Formant_h_
	#include "Formant.h"
#endif
#ifndef _PointProcess_h_
	#include "PointProcess.h"
#endif

struct FunctionEditor_spectrogram {
	/* KEEP IN SYNC WITH PREFS. */
	Spectrogram data; int show;
	/* Spectrogram settings: */
	double viewFrom, viewTo;   /* Hertz */
	double windowLength;   /* seconds */
	double dynamicRange;   /* dB */
	/* Advanced spectrogram settings: */
	long timeSteps, frequencySteps;
	int method;   /* Fourier */
	int windowShape;   /* 0=Square 1=Hamming 2=Bartlett 3=Welch 4=Hanning 5=Gaussian */
	int autoscaling;   /* yes/no */
	double maximum;   /* dB/Hz */
	double preemphasis;   /* dB/octave */
	double dynamicCompression;   /* 0..1 */
	/* Dynamic information: */
	double cursor;
};
struct FunctionEditor_pitch {
	/* KEEP IN SYNC WITH PREFS. */
	Pitch data; int show;
	/* Pitch settings: */
	double floor, ceiling; int unit;
	#define FunctionEditor_pitch_DRAWING_METHOD_CURVE  1
	#define FunctionEditor_pitch_DRAWING_METHOD_SPECKLE  2
	#define FunctionEditor_pitch_DRAWING_METHOD_AUTOMATIC  3
	int drawingMethod;
	/* Advanced pitch settings: */
	double viewFrom, viewTo;
	int method, veryAccurate;
	long maximumNumberOfCandidates; double silenceThreshold, voicingThreshold;
	double octaveCost, octaveJumpCost, voicedUnvoicedCost;
	struct { bool speckle; } picture;
};
struct FunctionEditor_intensity {
	/* KEEP IN SYNC WITH PREFS. */
	Intensity data; int show;
	/* Intensity settings: */
	double viewFrom, viewTo;
	int averagingMethod, subtractMeanPressure;
};
struct FunctionEditor_formant {
	/* KEEP IN SYNC WITH PREFS. */
	Formant data; int show;
	/* Formant settings: */
	double maximumFormant; long numberOfPoles;
	double windowLength;
	double dynamicRange, dotSize;
	/* Advanced formant settings: */
	int method; double preemphasisFrom;
};
struct FunctionEditor_pulses {
	/* KEEP IN SYNC WITH PREFS. */
	PointProcess data; int show;
	/* Pulses settings: */
	double maximumPeriodFactor, maximumAmplitudeFactor;
};

#define TimeSoundAnalysisEditor_members FunctionEditor_members \
	double longestAnalysis; \
	int timeStepStrategy; double fixedTimeStep; long numberOfTimeStepsPerView; \
	struct FunctionEditor_spectrogram spectrogram; \
	struct FunctionEditor_pitch pitch; \
	struct FunctionEditor_intensity intensity; \
	struct FunctionEditor_formant formant; \
	struct FunctionEditor_pulses pulses; \
	Widget spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;

#define TimeSoundAnalysisEditor_methods FunctionEditor_methods \
	struct { struct { struct { bool garnish; } pitch; } picture; } preferences; \
	void (*createMenus_pitch_picture) (I); \
	void (*destroy_analysis) (I); \
	void (*draw_analysis) (I); \
	void (*draw_analysis_pulses) (I); \
	void (*createMenuItems_query_selection) (I); \
	void (*createMenus_analysis) (I); \
	void (*createMenuItems_view_analysis) (I, EditorMenu menu);

class_create (TimeSoundAnalysisEditor, FunctionEditor);

void TimeSoundAnalysisEditor_init (I);
void FunctionEditor_SoundAnalysis_computeSpectrogram (I);
void FunctionEditor_SoundAnalysis_computePitch (I);
void FunctionEditor_SoundAnalysis_computeIntensity (I);
void FunctionEditor_SoundAnalysis_computeFormants (I);
void FunctionEditor_SoundAnalysis_computePulses (I);

void TimeSoundAnalysisEditor_prefs (void);

/* End of file TimeSoundAnalysisEditor.h */
#endif
