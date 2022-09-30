#ifndef _RealTierEditor_h_
#define _RealTierEditor_h_
/* RealTierEditor.h
 *
 * Copyright (C) 1992-2005,2007-2012,2015-2018,2020-2022 Paul Boersma
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
#include "RealTierArea.h"
#include "SoundArea.h"

Thing_define (RealTierEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, RealTierArea, realTierArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)

	void v1_dataChanged (Editor sender) override {
		our RealTierEditor_Parent :: v1_dataChanged (sender);
		our realTierArea() -> functionChanged (static_cast <RealTier> (our data()));
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);   // BUG: the copy probably doesn't change
	}
	void v_distributeAreas () override {
		if (our soundArea()) {
			our realTierArea() -> setGlobalYRange_fraction (0.0, 0.618);
			our soundArea() -> setGlobalYRange_fraction (0.618, 1.0);
		} else {
			our realTierArea() -> setGlobalYRange_fraction (0.0, 1.0);
		}
	}
	void v_play (double startTime, double endTime) override {
		if (our soundArea())
			Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our realTierArea().get(),
			FunctionArea_legend_LINES_SPECKLES U" ##modifiable RealTier",
			DataGui_defaultForegroundColour (our realTierArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
};

autoRealTierEditor RealTierEditor_create (conststring32 title, RealTier tier, Sound optionalSoundToCopy);

/* End of file RealTierEditor.h */
#endif
