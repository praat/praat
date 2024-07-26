#ifndef _SoundAnalysisArea_h_
#define _SoundAnalysisArea_h_
/* SoundAnalysisArea.h
 *
 * Copyright (C) 1992-2005,2007-2024 Paul Boersma
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

#include "FunctionArea.h"
#include "Sound_and_Spectrogram.h"
#include "Pitch.h"
#include "Intensity.h"
#include "Formant.h"
#include "PointProcess.h"
#include "LongSound.h"

#include "SoundAnalysisArea_enums.h"

Thing_define (SoundAnalysisArea, FunctionArea) {
	SampledXY soundOrLongSound() const { return static_cast <SampledXY> (our function()); }
	Sound sound() const {
		return our soundOrLongSound() && Thing_isa (our soundOrLongSound(), classSound) ? (Sound) our soundOrLongSound() : nullptr;
	}
	LongSound longSound() const {
		return our soundOrLongSound() && Thing_isa (our soundOrLongSound(), classLongSound) ? (LongSound) our soundOrLongSound() : nullptr;
	}

	autoSpectrogram d_spectrogram;
	double d_spectrogram_cursor;
	autoPitch d_pitch;
	autoIntensity d_intensity;
	autoFormant d_formant;
	autoPointProcess d_pulses;
	GuiMenuItem spectrogramToggle, pitchToggle, intensityToggle, formantToggle, pulsesToggle;
	GuiMenuItem pitchFilteredAutocorrelationToggle, pitchRawCrossCorrelationToggle, pitchRawAutocorrelationToggle,
			pitchFilteredCrossCorrelationToggle;

	virtual bool v_hasSpectrogram () { return true; }
	virtual bool v_hasPitch       () { return true; }
	virtual bool v_hasIntensity   () { return true; }
	virtual bool v_hasFormants    () { return true; }
	virtual bool v_hasPulses      () { return true; }
	virtual void v_reset_analysis ();

	virtual void v_spectrogramInfo () const;
	virtual void v_pitchInfo       () const;
	virtual void v_intensityInfo   () const;
	virtual void v_formantsInfo    () const;
	virtual void v_pulsesInfo      () const;

protected:
	void v_computeAuxiliaryData () override {
		our v_reset_analysis ();
	}

public:
	void v_windowChanged () override {
		our v_reset_analysis ();
	}
	bool hasContentToShow () {
		return our instancePref_spectrogram_show() || our instancePref_pitch_show() ||
				our instancePref_intensity_show() || our instancePref_formant_show();
	}
	bool hasPulsesToShow () {
		return our instancePref_pulses_show() && our endWindow() - our startWindow() <= our instancePref_longestAnalysis() && our d_pulses;
	}

public:
	void v1_info ()
		override;
	bool v_mouse (GuiDrawingArea_MouseEvent event, double x_world, double localY_fraction)
		override;
	virtual void v_draw_analysis ();
	virtual void v_draw_analysis_pulses ();
	virtual void v_draw_analysis_formants ();
	void v_createMenus ()
		override;
	virtual void v_createMenuItems_formant (EditorMenu menu);
	void v_updateMenuItems ()
		override;

	#include "SoundAnalysisArea_prefs.h"
	double dynamic_instancePref_pitch_floor ();
	double dynamic_instancePref_pitch_ceilingOrTop ();
	kPitch_unit dynamic_instancePref_pitch_unit ();
	kSoundAnalysisArea_pitch_drawingMethod dynamic_instancePref_pitch_drawingMethod ();
	double dynamic_instancePref_pitch_viewFrom ();
	double dynamic_instancePref_pitch_viewTo ();
	bool dynamic_instancePref_pitch_veryAccurate ();
	integer dynamic_instancePref_pitch_maximumNumberOfCandidates ();
	double dynamic_instancePref_pitch_silenceThreshold ();
	double dynamic_instancePref_pitch_voicingThreshold ();

	void v9_repairPreferences () override;
};

DEFINE_FunctionArea_create (SoundAnalysisArea, Sound)

void SoundAnalysisArea_haveVisibleSpectrogram (SoundAnalysisArea me);
void SoundAnalysisArea_haveVisiblePitch (SoundAnalysisArea me);
void SoundAnalysisArea_haveVisibleIntensity (SoundAnalysisArea me);
void SoundAnalysisArea_haveVisibleFormants (SoundAnalysisArea me);
void SoundAnalysisArea_haveVisiblePulses (SoundAnalysisArea me);

bool SoundAnalysisArea_mouse (SoundAnalysisArea me, GuiDrawingArea_MouseEvent event, double x_world, double y_fraction);

inline void SoundAnalysisArea_drawDefaultLegends (SoundAnalysisArea me) {
	if (my hasContentToShow () && my endWindow() - my startWindow() <= my instancePref_longestAnalysis())
		FunctionArea_drawLegend (me,
			my instancePref_spectrogram_show() ? FunctionArea_legend_GREYS U" %%derived spectrogram" : U"", 1.2 * Melder_BLACK,
			my instancePref_formant_show()     ? FunctionArea_legend_SPECKLES U" %%derived formants"    : U"", 1.2 * Melder_RED,
			my instancePref_intensity_show()   ? FunctionArea_legend_LINES U" %%derived intensity"   : U"", 1.2 * Melder_GREEN,
			my instancePref_pitch_show()       ? FunctionArea_legend_LINES_SPECKLES U" %%derived pitch"       : U"", 1.2 * Melder_BLUE
		);
}

/* End of file SoundAnalysisArea.h */
#endif
