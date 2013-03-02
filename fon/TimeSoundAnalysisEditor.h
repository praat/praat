#ifndef _TimeSoundAnalysisEditor_h_
#define _TimeSoundAnalysisEditor_h_
/* TimeSoundAnalysisEditor.h
 *
 * Copyright (C) 1992-2011,2012,2013 Paul Boersma
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

Thing_define (TimeSoundAnalysisEditor, TimeSoundEditor) { public:
	// new data:
		Spectrogram d_spectrogram;
		double d_spectrogram_cursor;
		Pitch d_pitch;
		Intensity d_intensity;
		Formant d_formant;
		PointProcess d_pulses;
		GuiMenuItem spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;
	// functions:
		void f_init (const wchar_t *title, Function data, Sampled sound, bool ownSound);
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_info ();
		virtual void v_createMenuItems_query (EditorMenu menu);
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_createMenuItems_view_sound (EditorMenu menu);
		virtual double v_getBottomOfSoundArea () {
			return p_spectrogram_show || p_pitch_show || p_intensity_show || p_formant_show ? 0.5 : 0.0;
		}
	// new methods:
		virtual bool v_hasAnalysis    () { return true; }
		virtual bool v_hasSpectrogram () { return true; }
		virtual bool v_hasPitch       () { return true; }
		virtual bool v_hasIntensity   () { return true; }
		virtual bool v_hasFormants    () { return true; }
		virtual bool v_hasPulses      () { return true; }
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
	// preferences:
		#include "TimeSoundAnalysisEditor_prefs.h"
};

void TimeSoundAnalysisEditor_computeSpectrogram (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computePitch (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computeIntensity (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computeFormants (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_computePulses (TimeSoundAnalysisEditor me);

/* End of file TimeSoundAnalysisEditor.h */
#endif
