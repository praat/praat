#ifndef _PitchTierEditor_h_
#define _PitchTierEditor_h_
/* PitchTierEditor.h
 *
 * Copyright (C) 1992-2005,2007,2009-2012,2015-2018,2020-2022 Paul Boersma
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
#include "SoundArea.h"
#include "PitchTier_to_Sound.h"

Thing_define (PitchTierEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, PitchTierArea, pitchTierArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)

	void v1_dataChanged (Editor sender) override {
		our PitchTierEditor_Parent :: v1_dataChanged (sender);
		our pitchTierArea() -> functionChanged (static_cast <PitchTier> (our data()));
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);   // BUG: the copy probably doesn't change
	}
	void v_distributeAreas () override {
		if (our soundArea()) {
			our pitchTierArea() -> setGlobalYRange_fraction (0.0, 0.618);
			our soundArea() -> setGlobalYRange_fraction (0.618, 1.0);
		} else {
			our pitchTierArea() -> setGlobalYRange_fraction (0.0, 1.0);
		}
	}
	void v_createMenuItems_help (EditorMenu menu)
		override;
	void v_play (double startTime, double endTime) override {
		if (our soundArea())
			Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
		else
			PitchTier_playPart (our pitchTierArea() -> pitchTier(), startTime, endTime, false, theFunctionEditor_playCallback, this);
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our pitchTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable PitchTier",
			DataGui_defaultForegroundColour (our pitchTierArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
};

autoPitchTierEditor PitchTierEditor_create (conststring32 title, PitchTier pitch, Sound optionalSoundToCopy);

/* End of file PitchTierEditor.h */
#endif
