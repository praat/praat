#ifndef _TimeSoundAnalysisEditor_h_
#define _TimeSoundAnalysisEditor_h_
/* TimeSoundAnalysisEditor.h
 *
 * Copyright (C) 1992-2007,2009-2016,2018,2020,2022 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

#include "TimeSoundEditor.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Intensity.h"
#include "Formant.h"
#include "PointProcess.h"

#include "TimeSoundAnalysisEditor_enums.h"

Thing_define (TimeSoundAnalysisEditor, TimeSoundEditor) {
	autoSpectrogram d_spectrogram;
	double d_spectrogram_cursor;
	autoPitch d_pitch;
	autoIntensity d_intensity;
	autoFormant d_formant;
	autoPointProcess d_pulses;
	GuiMenuItem spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;

	void v1_info ()
		override;
	void v_dataChanged ()
		override;
	void v_createMenuItems_query (EditorMenu menu)
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;
	void v_createMenuItems_view_sound (EditorMenu menu)
		override;
	double v_getBottomOfSoundArea ()
		override
	{
		return our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
				our instancePref_intensity_show() || our instancePref_formant_show() ? 0.5 : 0.0;
	}
	void v_windowChanged ()
		override
	{
		our v_reset_analysis ();
	}

	virtual bool v_hasAnalysis    () { return true; }
	virtual bool v_hasSpectrogram () { return true; }
	virtual bool v_hasPitch       () { return true; }
	virtual bool v_hasIntensity   () { return true; }
	virtual bool v_hasFormants    () { return true; }
	virtual bool v_hasPulses      () { return true; }
	virtual void v_reset_analysis ();
	virtual void v_createMenuItems_spectrum_picture (EditorMenu menu);
	virtual void v_createMenuItems_pitch_picture (EditorMenu menu);
	virtual void v_createMenuItems_intensity_picture (EditorMenu menu);
	virtual void v_createMenuItems_formant_picture (EditorMenu menu);
	virtual void v_createMenuItems_pulses_picture (EditorMenu menu);
	virtual void v_draw_analysis ();
	virtual void v_draw_analysis_pulses ();
	virtual void v_draw_analysis_formants ();
	virtual void v_createMenuItems_query_log (EditorMenu menu);
	virtual void v_createMenus_analysis ();
	virtual void v_createMenuItems_formant (EditorMenu menu);
	virtual void v_createMenuItems_view_sound_analysis (EditorMenu menu);

	#include "TimeSoundAnalysisEditor_prefs.h"
	void v_repairPreferences () override;
};

void TimeSoundAnalysisEditor_haveVisibleSpectrogram (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_haveVisiblePitch (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_haveVisibleIntensity (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_haveVisibleFormants (TimeSoundAnalysisEditor me);
void TimeSoundAnalysisEditor_haveVisiblePulses (TimeSoundAnalysisEditor me);

/* End of file TimeSoundAnalysisEditor.h */
#endif
