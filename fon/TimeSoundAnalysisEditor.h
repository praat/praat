#ifndef _TimeSoundAnalysisEditor_h_
#define _TimeSoundAnalysisEditor_h_
/* TimeSoundAnalysisEditor.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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
 * pb 2007/06/10 wchar
 * pb 2007/09/02 direct drawing to picture window
 * pb 2007/09/08 inherit from TimeSoundEditor
 * pb 2007/11/01 direct intensity, formants, and pulses drawing
 * pb 2007/12/02 split off TimeSoundAnalysisEditor_enums.h
 * pb 2011/03/23 C++
 * pb 2011/07/15 C++
 */

#include "TimeSoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Intensity.h"
#include "Formant.h"
#include "PointProcess.h"

#include "TimeSoundAnalysisEditor_enums.h"

struct FunctionEditor_spectrogram {
	Spectrogram data; bool show;
	/* Spectrogram settings: */
	double viewFrom, viewTo;   // Hz
	double windowLength;   // seconds
	double dynamicRange;   // dB
	/* Advanced spectrogram settings: */
	long timeSteps, frequencySteps;
	enum kSound_to_Spectrogram_method method;
	enum kSound_to_Spectrogram_windowShape windowShape;
	bool autoscaling;
	double maximum;   // dB/Hz
	double preemphasis;   // dB/octave
	double dynamicCompression;   // 0..1
	/* Dynamic information: */
	double cursor;
	struct { bool garnish; } picture;
};

struct FunctionEditor_pitch {
	Pitch data; bool show;
	/* Pitch settings: */
	double floor, ceiling;
	enum kPitch_unit unit;
	enum kTimeSoundAnalysisEditor_pitch_drawingMethod drawingMethod;
	/* Advanced pitch settings: */
	double viewFrom, viewTo;
	enum kTimeSoundAnalysisEditor_pitch_analysisMethod method;
	bool veryAccurate;
	long maximumNumberOfCandidates; double silenceThreshold, voicingThreshold;
	double octaveCost, octaveJumpCost, voicedUnvoicedCost;
	struct { bool speckle, garnish; } picture;
};
struct FunctionEditor_intensity {
	Intensity data; bool show;
	/* Intensity settings: */
	double viewFrom, viewTo;
	enum kTimeSoundAnalysisEditor_intensity_averagingMethod averagingMethod;
	bool subtractMeanPressure;
	struct { bool garnish; } picture;
};
struct FunctionEditor_formant {
	Formant data; bool show;
	/* Formant settings: */
	double maximumFormant; long numberOfPoles;
	double windowLength;
	double dynamicRange, dotSize;
	/* Advanced formant settings: */
	enum kTimeSoundAnalysisEditor_formant_analysisMethod method;
	double preemphasisFrom;
	struct { bool garnish; } picture;
};
struct FunctionEditor_pulses {
	PointProcess data; bool show;
	/* Pulses settings: */
	double maximumPeriodFactor, maximumAmplitudeFactor;
	struct { bool garnish; } picture;
};

Thing_define (TimeSoundAnalysisEditor, TimeSoundEditor) {
	// new data:
	public:
		double longestAnalysis;
		enum kTimeSoundAnalysisEditor_timeStepStrategy timeStepStrategy;
		double fixedTimeStep;
		long numberOfTimeStepsPerView;
		struct FunctionEditor_spectrogram spectrogram;
		struct FunctionEditor_pitch pitch;
		struct FunctionEditor_intensity intensity;
		struct FunctionEditor_formant formant;
		struct FunctionEditor_pulses pulses;
		GuiObject spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_info ();
		virtual void v_createMenuItems_query (EditorMenu menu);
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_createMenuItems_view_sound (EditorMenu menu);
	// new methods:
		virtual void v_destroy_analysis ();
		virtual void v_createMenuItems_spectrum_picture (EditorMenu menu);
		virtual void v_createMenuItems_pitch_picture (EditorMenu menu);
		virtual void v_createMenuItems_intensity_picture (EditorMenu menu);
		virtual void v_createMenuItems_formant_picture (EditorMenu menu);
		virtual void v_createMenuItems_pulses_picture (EditorMenu menu);
		virtual void v_draw_analysis ();
		virtual void v_draw_analysis_pulses ();
		virtual void v_createMenuItems_query_log (EditorMenu menu);
		virtual void v_createMenus_analysis ();
		virtual void v_createMenuItems_view_sound_analysis (EditorMenu menu);
};

void TimeSoundAnalysisEditor_init (TimeSoundAnalysisEditor me, GuiObject parent, const wchar *title, Function data, Sampled sound, bool ownSound);

void TimeSoundAnalysisEditor_computeSpectrogram (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computePitch (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computeIntensity (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computeFormants (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computePulses (TimeSoundAnalysisEditor me);

void TimeSoundAnalysisEditor_prefs (void);

/* End of file TimeSoundAnalysisEditor.h */
#endif
