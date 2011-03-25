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

/*
 * pb 2011/03/22
 */

#include "FunctionEditor.h"
#include "Manipulation.h"

#ifdef __cplusplus
	extern "C" {
#endif

#include "ManipulationEditor_enums.h"

#define ManipulationEditor__parents(Klas) FunctionEditor__parents(Klas) Thing_inherit (Klas, FunctionEditor)
Thing_declare1 (ManipulationEditor);

#define ManipulationEditor__members(Klas) FunctionEditor__members(Klas) \
	PointProcess previousPulses; \
	PitchTier previousPitch; \
	DurationTier previousDuration; \
	double soundmin, soundmax; \
	int synthesisMethod; \
	GuiObject synthPulsesButton, synthPulsesHumButton; \
	GuiObject synthPulsesLpcButton; \
	GuiObject synthPitchButton, synthPitchHumButton; \
	GuiObject synthPulsesPitchButton, synthPulsesPitchHumButton; \
	GuiObject synthOverlapAddNodurButton, synthOverlapAddButton; \
	GuiObject synthPitchLpcButton; \
	struct { enum kManipulationEditor_pitchUnits units; enum kManipulationEditor_draggingStrategy draggingStrategy; double minimum, minPeriodic, maximum, cursor; } pitchTier; \
	struct { double minimum, maximum, cursor;  } duration; \
	Graphics_Viewport inset;
#define ManipulationEditor__methods(Klas) FunctionEditor__methods(Klas)
Thing_declare2 (ManipulationEditor, FunctionEditor);

ManipulationEditor ManipulationEditor_create (GuiObject parent, const wchar_t *title, Manipulation ana);

void ManipulationEditor_prefs (void);

#ifdef __cplusplus
	}
#endif

/* End of file ManipulationEditor.h */
#endif
