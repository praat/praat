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
#include "PitchTierArea.h"
#include "DurationTierArea.h"
#include "Manipulation.h"

#include "ManipulationEditor_enums.h"

Thing_define (ManipulationPitchTierArea, PitchTierArea) {
	void v_drawOverFrame ()
		override;
};
DEFINE_FunctionArea_create (ManipulationPitchTierArea, PitchTier)

Thing_define (ManipulationDurationTierArea, DurationTierArea) {
	void v_drawOverFrame ()
		override;
};
DEFINE_FunctionArea_create (ManipulationDurationTierArea, DurationTier)


Thing_define (ManipulationEditor, FunctionEditor) {
	Manipulation manipulation() { return static_cast <Manipulation> (our data()); }

	/*
		Quick access to internal objects.
	*/
	Sound sound() { return our manipulation() -> sound.get(); }
	PointProcess pulses() { return our manipulation() -> pulses.get(); }
	PitchTier pitch() { return our manipulation() -> pitch.get(); }
	DurationTier duration() { return our manipulation() -> duration.get(); }

	/*
		Areas.
	*/
	autoPitchTierArea pitchTierArea;
	autoDurationTierArea durationTierArea;
	bool clickedInWidePitchArea = false;
	bool clickedInWideDurationArea = false;

	void v1_dataChanged () override {
		ManipulationEditor_Parent :: v1_dataChanged ();
		our pitchTierArea -> functionChanged (our pitch());
		our durationTierArea -> functionChanged (our duration());
	}

	autoPointProcess previousPulses;
	autoPitchTier previousPitch;
	autoDurationTier previousDuration;
	double soundmin, soundmax;
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
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_saveData ()
		override;
	void v_restoreData ()
		override;
	void v_distributeAreas ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;
	void v_play (double tmin, double tmax)
		override;

	#include "ManipulationEditor_prefs.h"
};

autoManipulationEditor ManipulationEditor_create (conststring32 title, Manipulation manipulation);

/* End of file ManipulationEditor.h */
#endif
