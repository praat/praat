#ifndef _ManipulationEditor_h_
#define _ManipulationEditor_h_
/* ManipulationEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2013,2015,2016,2018,2020-2022 Paul Boersma
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
	double soundmin, soundmax;
	void v_drawInside ()
		override;
};
DEFINE_FunctionArea_create (ManipulationSoundArea, Sound)

Thing_define (ManipulationPulsesArea, PointArea) {
	PointProcess pulses() { return static_cast <PointProcess> (our function()); }
	void v_drawInside ()
		override;
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

	/*
		Quick access to internal objects.
	*/
	Sound sound() { return our manipulation() -> sound.get(); }
	PointProcess pulses() { return our manipulation() -> pulses.get(); }
	PitchTier pitch() { return our manipulation() -> pitch.get(); }
	DurationTier duration() { return our manipulation() -> duration.get(); }

	void v1_dataChanged () override {
		ManipulationEditor_Parent :: v1_dataChanged ();
		our pulsesArea() -> functionChanged (our pulses());
		our soundArea() -> functionChanged (our sound());
		our pitchTierArea() -> functionChanged (our pitch());
		our durationTierArea() -> functionChanged (our duration());
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
	void v_saveData ()
		override;
	void v_restoreData ()
		override;
	void v_distributeAreas () override {
		our pulsesArea() -> setGlobalYRange_fraction (0.67, 1.00);
		our soundArea() -> setGlobalYRange_fraction (0.67, 1.00);
		our pitchTierArea() -> setGlobalYRange_fraction (( our duration() ? 0.17 : 0.0 ), 0.67);
		if (our duration())
			our durationTierArea() -> setGlobalYRange_fraction (0.0, 0.17);
	}
	void v_draw () override {
		FunctionArea_drawTwo (our pulsesArea().get(), our soundArea().get());
		if (our pitch())
			FunctionArea_drawOne (our pitchTierArea().get());
		if (our duration())
			FunctionArea_drawOne (our durationTierArea().get());
	}
	void v_updateMenuItems ()
		override;

	void v_play (double tmin, double tmax)
		override;
};

autoManipulationEditor ManipulationEditor_create (conststring32 title, Manipulation manipulation);

/* End of file ManipulationEditor.h */
#endif
