#ifndef _CubeGridEditor_h_
#define _CubeGridEditor_h_
/* CubeGridEditor.h
 *
 * Copyright (C) 2023 Paul Boersma
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
#include "CubeGridArea.h"
#include "SoundArea.h"

Thing_define (CubeGridEditor, FunctionEditor) {
	DEFINE_FunctionArea (1, CubeGridArea, cubeGridArea)
	DEFINE_FunctionArea (2, SoundArea, soundArea)
	CubeGrid cubeGrid () { return static_cast <CubeGrid> (our data()); }

	void v1_dataChanged (Editor sender) override {
		CubeGridEditor_Parent :: v1_dataChanged (sender);
		our cubeGridArea() -> functionChanged (static_cast <CubeGrid> (our data()));
		if (our soundArea())
			our soundArea() -> functionChanged (nullptr);
	}
	void v_distributeAreas () override {
		if (our soundArea()) {
			our cubeGridArea() -> setGlobalYRange_fraction (0.0, 0.8);
			our soundArea() -> setGlobalYRange_fraction (0.8, 1.0);
		} else {
			our cubeGridArea() -> setGlobalYRange_fraction (0.0, 1.0);
		}
	}
	void v_play (double startTime, double endTime) override {
		if (our soundArea())
			Sound_playPart (our soundArea() -> sound(), startTime, endTime, theFunctionEditor_playCallback, this);
	}
	void v_drawLegends () override {
		FunctionArea_drawLegend (our cubeGridArea().get(),
			U" CubeGrid",
			DataGui_defaultForegroundColour (our cubeGridArea().get(), false)
		);
		if (our soundArea())
			FunctionArea_drawLegend (our soundArea().get(),
				FunctionArea_legend_WAVEFORM U" %%non-modifiable copy of sound",
				DataGui_defaultForegroundColour (our soundArea().get(), false)
			);
	}
	bool v_hasSelectionViewer ()
		override { return true; }
	void v_drawSelectionViewer ()
		override;
	void v_drawRealTimeSelectionViewer (double time)
		override;

	#include "CubeGridEditor_prefs.h"
};

autoCubeGridEditor CubeGridEditor_create (conststring32 title, CubeGrid grid, Sound optionalSoundToCopy);

/* End of file CubeGridEditor.h */
#endif
