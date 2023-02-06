#ifndef _NoulliGridEditor_h_
#define _NoulliGridEditor_h_
/* NoulliGridEditor.h
 *
 * Copyright (C) 2018,2020-2023 Paul Boersma
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
#include "NoulliGridArea.h"
#include "SoundArea.h"

#include "NoulliGridEditor_enums.h"

Thing_define (NoulliGridEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, NoulliGridArea, noulliGridArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)
	NoulliGrid noulliGrid () { return static_cast <NoulliGrid> (our data()); }

	void v1_dataChanged (Editor sender) override {
		NoulliGridEditor_Parent :: v1_dataChanged (sender);
		our noulliGridArea() -> functionChanged (static_cast <NoulliGrid> (our data()));
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);
	}
	void v_distributeAreas () override {
		if (our soundArea()) {
			our noulliGridArea() -> setGlobalYRange_fraction (0.0, 0.8);
			our soundArea() -> setGlobalYRange_fraction (0.8, 1.0);
		} else {
			our noulliGridArea() -> setGlobalYRange_fraction (0.0, 1.0);
		}
	}
	void v_play (double startTime, double endTime) override {
		if (our soundArea())
			Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our noulliGridArea().get(),
			U" NoulliGrid",
			DataGui_defaultForegroundColour (our noulliGridArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
	void v_prefs_addFields (EditorCommand cmd)
		override;
	void v_prefs_setValues (EditorCommand cmd)
		override;
	void v_prefs_getValues (EditorCommand cmd)
		override;
	bool v_hasSelectionViewer ()
		override { return true; }
	void v_drawSelectionViewer ()
		override;
	void v_drawRealTimeSelectionViewer (double time)
		override;

	#include "NoulliGridEditor_prefs.h"
};

autoNoulliGridEditor NoulliGridEditor_create (conststring32 title, NoulliGrid grid, Sound optionalSoundToCopy);

/* End of file NoulliGridEditor.h */
#endif
