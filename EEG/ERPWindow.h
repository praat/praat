#ifndef _ERPWindow_h_
#define _ERPWindow_h_
/* ERPWindow.h
 *
 * Copyright (C) 2012-2018 Paul Boersma
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

#include "SoundEditor.h"
#include "ERP.h"

Thing_define (ERPWindow, SoundEditor) {
	conststring32 v_getChannelName (integer channelNumber)
		override {
			ERP erp = (ERP) our data;
			return erp -> channelNames [channelNumber].get();
		}
	void v_drawSelectionViewer ()
		override;
	bool v_hasPitch ()
		override { return false; }
	bool v_hasIntensity ()
		override { return false; }
	bool v_hasFormants ()
		override { return false; }
	bool v_hasPulses ()
		override { return false; }
	void v_prefs_addFields (EditorCommand cmd)
		override;
	void v_prefs_setValues (EditorCommand cmd)
		override;
	void v_prefs_getValues (EditorCommand cmd)
		override;

	#include "ERPWindow_prefs.h"
};

/**
	Create an ERPWindow.
*/
autoERPWindow ERPWindow_create (conststring32 title, ERP data);

/* End of file ERPWindow.h */
#endif
