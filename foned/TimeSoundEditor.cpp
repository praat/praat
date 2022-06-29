/* TimeSoundEditor.cpp
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

#include "NUM2.h"
#include "TimeSoundEditor.h"
#include "EditorM.h"

Thing_implement_pureVirtual (TimeSoundEditor, FunctionEditor, 0);

/*
	TimeSoundEditor is designed to be removed.
	Most of its remaining functionality is to just delegate everything to SoundArea.
	At some point, editors will just have to include a SoundArea instead of deriving from TimeSoundEditor.
*/

void structTimeSoundEditor :: v1_info () {
	TimeSoundEditor_Parent :: v1_info ();
	if (our soundArea)
		our soundArea -> v1_info ();
}

void structTimeSoundEditor:: v_createMenus () {
	TimeSoundEditor_Parent :: v_createMenus ();
	if (our soundArea)
		our soundArea -> v_createMenus ();
}
void structTimeSoundEditor :: v_createMenuItems_file (EditorMenu menu) {
	our TimeSoundEditor_Parent :: v_createMenuItems_file (menu);
	our v_createMenuItems_file_draw (menu);
	EditorMenu_addCommand (menu, U"-- after file draw --", 0, nullptr);
	our v_createMenuItems_file_extract (menu);
	EditorMenu_addCommand (menu, U"-- after file extract --", 0, nullptr);
	our v_createMenuItems_file_write (menu);
	if (our soundArea)
		our soundArea -> v_createMenuItems_file (menu);
	EditorMenu_addCommand (menu, U"-- after file write --", 0, nullptr);
}
void structTimeSoundEditor :: v_updateMenuItems_file () {
	if (! our soundOrLongSound())
		return;
	if (our soundArea)
		our soundArea -> v_updateMenuItems_file ();
}

bool structTimeSoundEditor :: v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction) {
	if (event -> isClick()) {
		if (our soundOrLongSound()) {
			y_fraction = our soundArea -> y_fraction_globalToLocal (y_fraction);
			const integer numberOfChannels = our soundOrLongSound() -> ny;
			if (event -> commandKeyPressed) {
				if (numberOfChannels > 1) {
					const integer numberOfVisibleChannels = Melder_clippedRight (numberOfChannels, 8_integer);
					Melder_assert (numberOfVisibleChannels >= 1);   // for Melder_clipped
					const integer clickedChannel = our soundArea -> channelOffset +
							Melder_clipped (1_integer, Melder_ifloor ((1.0 - y_fraction) * numberOfVisibleChannels + 1), numberOfVisibleChannels);
					const integer firstVisibleChannel = our soundArea -> channelOffset + 1;
					const integer lastVisibleChannel = Melder_clippedRight (our soundArea -> channelOffset + numberOfVisibleChannels, numberOfChannels);
					if (clickedChannel >= firstVisibleChannel && clickedChannel <= lastVisibleChannel) {
						our soundArea -> muteChannels [clickedChannel] = ! our soundArea -> muteChannels [clickedChannel];
						return FunctionEditor_UPDATE_NEEDED;
					}
				}
			} else {
				if (numberOfChannels > 8) {
					if (x_world >= our endWindow && y_fraction > 0.875 && y_fraction <= 1.000 && our soundArea -> channelOffset > 0) {
						our soundArea -> channelOffset -= 8;
						return FunctionEditor_UPDATE_NEEDED;
					}
					if (x_world >= our endWindow && y_fraction > 0.000 && y_fraction <= 0.125 && our soundArea -> channelOffset < numberOfChannels - 8) {
						our soundArea -> channelOffset += 8;
						return FunctionEditor_UPDATE_NEEDED;
					}
				}
			}
		}
	}
	return TimeSoundEditor_Parent :: v_mouseInWideDataView (event, x_world, y_fraction);   // BUG: use FunctionEditor_defaultMouseInWideDataView()
}

/* End of file TimeSoundEditor.cpp */
