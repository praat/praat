#ifndef _EEGWindow_h_
#define _EEGWindow_h_
/* EEGWindow.h
 *
 * Copyright (C) 2011-2018,2022 Paul Boersma
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

#include "AnyTextGridEditor.h"
#include "EEGArea.h"

Thing_define (EEGWindow, AnyTextGridEditor) {
	EEG eeg() { return static_cast <EEG> (our data); }

	void v1_dataChanged () override {
		TRACE trace(1);
		our EEGWindow_Parent :: v1_dataChanged ();
		trace(2);
		Melder_assert (our eeg());
		trace(Thing_className(our eeg()));

		Sound sound = our eeg() -> sound.get();
		Melder_assert (sound);
		trace(Thing_className(sound));
		Melder_assert (Thing_isa (sound, classSound));
		trace (sound -> nx, U" ", sound -> ny);
		our soundArea -> functionChanged (our eeg() -> sound.get());
	}

	GuiMenuItem extractSelectedEEGPreserveTimesButton, extractSelectedEEGTimeFromZeroButton;

	bool v_hasPitch ()
		override { return false; }
	bool v_hasIntensity ()
		override { return false; }
	bool v_hasFormants ()
		override { return false; }
	bool v_hasPulses ()
		override { return false; }
	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_createMenuItems_file_extract (EditorMenu menu)
		override;
	void v_updateMenuItems_file ()
		override;

	#include "EEGWindow_prefs.h"
};

autoEEGWindow EEGWindow_create (conststring32 title, EEG eeg);

/* End of file EEGWindow.h */
#endif
