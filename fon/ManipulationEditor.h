#ifndef _ManipulationEditor_h_
#define _ManipulationEditor_h_
/* ManipulationEditor.h
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

#include "FunctionEditor.h"
#include "Manipulation.h"

#include "ManipulationEditor_enums.h"

Thing_define (ManipulationEditor, FunctionEditor) {
	// new data:
		PointProcess previousPulses;
		PitchTier previousPitch;
		DurationTier previousDuration;
		double soundmin, soundmax;
		int synthesisMethod;
		GuiObject synthPulsesButton, synthPulsesHumButton;
		GuiObject synthPulsesLpcButton;
		GuiObject synthPitchButton, synthPitchHumButton;
		GuiObject synthPulsesPitchButton, synthPulsesPitchHumButton;
		GuiObject synthOverlapAddNodurButton, synthOverlapAddButton;
		GuiObject synthPitchLpcButton;
		struct { enum kManipulationEditor_pitchUnits units; enum kManipulationEditor_draggingStrategy draggingStrategy; double minimum, minPeriodic, maximum, cursor; } pitchTier;
		struct { double minimum, maximum, cursor;  } duration;
		Graphics_Viewport inset;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_createMenus ();
		virtual void v_createHelpMenuItems (EditorMenu menu);
		virtual void v_save ();
		virtual void v_restore ();
		virtual void v_draw ();
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_play (double tmin, double tmax);
};

ManipulationEditor ManipulationEditor_create (GuiObject parent, const wchar *title, Manipulation ana);

void ManipulationEditor_prefs (void);

/* End of file ManipulationEditor.h */
#endif
