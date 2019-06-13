#ifndef _ButtonEditor_h_
#define _ButtonEditor_h_
/* ButtonEditor.h
 *
 * Copyright (C) 1996-2005,2009-2012,2015,2016,2018,2019 Paul Boersma
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

#include "HyperPage.h"

Thing_define (ButtonEditor, HyperPage) {
	int show;
	GuiRadioButton button1, button2, button3, buttonAD, buttonEH, buttonIL, buttonMO, buttonPS, buttonTZ;

	bool v_scriptable ()
		override { return false; }
	void v_createChildren ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_draw ()
		override;
	int v_goToPage (conststring32 title)
		override;
};

autoButtonEditor ButtonEditor_create ();

/* End of file ButtonEditor.h */
#endif
