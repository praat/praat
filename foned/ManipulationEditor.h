#ifndef _ManipulationEditor_h_
#define _ManipulationEditor_h_
/* ManipulationEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2013,2015,2016,2018,2020-2023 Paul Boersma
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

#include "FunctionEditor.h"
#include "SoundArea.h"
#include "PointArea.h"
#include "PitchTierArea.h"
#include "DurationTierArea.h"
#include "Manipulation.h"

#include "ManipulationEditor_enums.h"

Thing_define (ManipulationSoundArea, SoundArea) {
};
DEFINE_FunctionArea_create (ManipulationSoundArea, Sound)

Thing_define (ManipulationPulsesArea, PointArea) {
	PointProcess pulses() { return static_cast <PointProcess> (our function()); }
	void v_createMenus ()
		override;
};
DEFINE_FunctionArea_create (ManipulationPulsesArea, PointProcess)

Thing_define (ManipulationPitchTierArea, PitchTierArea) {
	PitchTier pitch() { return static_cast <PitchTier> (our function()); }
	ManipulationPulsesArea borrowedPulsesArea;
	void v_drawInside ()
		override;
	void v_createMenus ()
		override;
	#include "ManipulationEditor_prefs.h"
};
DEFINE_FunctionArea_create (ManipulationPitchTierArea, PitchTier)

Thing_define (ManipulationDurationTierArea, DurationTierArea) {
	DurationTier duration() { return static_cast <DurationTier> (our function()); }
	void v_drawInside ()
		override;
	void v_createMenus ()
		override;
};
DEFINE_FunctionArea_create (ManipulationDurationTierArea, DurationTier)


Thing_define (ManipulationEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, ManipulationSoundArea, soundArea)
	DEFINE_FunctionArea (2, ManipulationPitchTierArea, pitchTierArea)
	DEFINE_FunctionArea (3, ManipulationDurationTierArea, durationTierArea)
	DEFINE_FunctionArea (4, ManipulationPulsesArea, pulsesArea)

	Manipulation manipulation() { return static_cast <Manipulation> (our data()); }

	void v1_dataChanged (Editor sender) override {
		ManipulationEditor_Parent :: v1_dataChanged (sender);
		our soundArea() -> functionChanged (our manipulation() -> sound.get());
		our pitchTierArea() -> functionChanged (our manipulation() -> pitch.get());
		if (! our manipulation() -> duration)   // repair an old-fashioned Manipulation that has a PitchTier only
			our manipulation() -> duration = DurationTier_create (our manipulation() -> xmin, our manipulation() -> xmax);
		our durationTierArea() -> functionChanged (our manipulation() -> duration.get());
		our pulsesArea() -> functionChanged (our manipulation() -> pulses.get());
	}

	autoPointProcess previousPulses;
	autoPitchTier previousPitch;
	autoDurationTier previousDuration;
	int synthesisMethod;
	GuiMenuItem synthPulsesButton, synthPulsesHumButton;
	GuiMenuItem synthPulsesLpcButton;
	GuiMenuItem synthPitchButton, synthPitchHumButton;
	GuiMenuItem synthPulsesPitchButton, synthPulsesPitchHumButton;
	GuiMenuItem synthOverlapAddNodurButton, synthOverlapAddButton;
	GuiMenuItem synthPitchLpcButton;

	Graphics_Viewport inset;

	void v_createMenus ()
		override;
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_distributeAreas () override {
		our pulsesArea() -> setGlobalYRange_fraction (0.67, 1.00);
		our soundArea() -> setGlobalYRange_fraction (0.67, 1.00);
		our pitchTierArea() -> setGlobalYRange_fraction (0.17, 0.67);
		our durationTierArea() -> setGlobalYRange_fraction (0.0, 0.17);
	}
	void v_draw () override {
		FunctionArea_drawTwo (our pulsesArea().get(), our soundArea().get());
		FunctionArea_drawOne (our pitchTierArea().get());
		FunctionArea_drawOne (our durationTierArea().get());
	}
	void v_updateMenuItems ()
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_drawLegends () override {
		FunctionArea_drawLegend (our soundArea().get(),
			FunctionArea_legend_WAVEFORM U" %%non-modifiable mono copy of sound",
			DataGui_defaultForegroundColour (our soundArea().get(), false),
			FunctionArea_legend_POLES U" ##modifiable pulses",
			DataGui_defaultForegroundColour (our pulsesArea().get(), false)
		);
		FunctionArea_drawLegend (our pitchTierArea().get(),
			FunctionArea_legend_SPECKLES U" %%pitch derived from pulses",
			Melder_GREY,
			FunctionArea_legend_LINES_SPECKLES U" ##manipulatable pitch",
			DataGui_defaultForegroundColour (our pitchTierArea().get(), false)
		);
		FunctionArea_drawLegend (our durationTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##manipulatable duration",
			DataGui_defaultForegroundColour (our durationTierArea().get(), false)
		);
	}
};

autoManipulationEditor ManipulationEditor_create (conststring32 title, Manipulation manipulation);

/* End of file ManipulationEditor.h */
#endif
