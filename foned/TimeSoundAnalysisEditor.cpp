/* TimeSoundAnalysisEditor.cpp
 *
 * Copyright (C) 1992-2022 Paul Boersma
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

#include "TimeSoundAnalysisEditor.h"
#include "EditorM.h"
//#include <time.h>

Thing_implement_pureVirtual (TimeSoundAnalysisEditor, TimeSoundEditor, 0);

void structTimeSoundAnalysisEditor :: v1_info () {
	TimeSoundAnalysisEditor_Parent :: v1_info ();
	if (our soundAnalysisArea)
		our soundAnalysisArea -> v1_info ();
}

void structTimeSoundAnalysisEditor :: v1_dataChanged () {
	our TimeSoundAnalysisEditor_Parent :: v1_dataChanged ();
	if (our soundAnalysisArea)
		our soundAnalysisArea -> v_reset_analysis ();
}

bool structTimeSoundAnalysisEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		if (our soundAnalysisArea && our soundAnalysisArea -> instancePref_pitch_show()) {
			if (x_world >= our endWindow && y_fraction > 0.48 && y_fraction <= 0.50) {
				our soundAnalysisArea -> setInstancePref_pitch_ceiling (our soundAnalysisArea -> instancePref_pitch_ceiling() * 1.26);
				our soundAnalysisArea -> d_pitch. reset();
				our soundAnalysisArea -> d_intensity.reset();
				our soundAnalysisArea -> d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
			if (x_world >= our endWindow && y_fraction > 0.46 && y_fraction <= 0.48) {
				our soundAnalysisArea -> setInstancePref_pitch_ceiling (our soundAnalysisArea -> instancePref_pitch_ceiling() / 1.26);
				our soundAnalysisArea -> d_pitch. reset();
				our soundAnalysisArea -> d_intensity. reset();
				our soundAnalysisArea -> d_pulses. reset();
				return FunctionEditor_UPDATE_NEEDED;
			}
		}
	}
	return TimeSoundAnalysisEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);
}

/* End of file TimeSoundAnalysisEditor.cpp */
