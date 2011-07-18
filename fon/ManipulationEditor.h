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

Thing_declare1cpp (ManipulationEditor);
struct structManipulationEditor : structFunctionEditor {
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
		void v_destroy ();
		void v_createMenus ();
		void v_createHelpMenuItems (EditorMenu menu);
		void v_save ();
		void v_restore ();
};
#define ManipulationEditor__methods(Klas) FunctionEditor__methods(Klas)
Thing_declare2cpp (ManipulationEditor, FunctionEditor);

ManipulationEditor ManipulationEditor_create (GuiObject parent, const wchar *title, Manipulation ana);

void ManipulationEditor_prefs (void);

/* End of file ManipulationEditor.h */
#endif
