#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
 *
 * Copyright (C) 2008-2011,2012,2013,2015,2017 Paul Boersma & David Weenink
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
#include "FormantGrid.h"

Thing_define (FormantGridEditor, FunctionEditor) {
	bool editingBandwidths;
	GuiMenuItem d_bandwidthsToggle;
	integer selectedFormant;
	double ycursor;

	void v_createMenus ()
		override;
	void v_draw ()
		override;
	bool v_click (double xWC, double yWC, bool shiftKeyPressed)
		override;
	void v_play (double tmin, double tmax)
		override;

	virtual bool v_hasSourceMenu () { return true; }

	#include "FormantGridEditor_prefs.h"
};

void FormantGridEditor_init (FormantGridEditor me, const char32 *title, FormantGrid data);

autoFormantGridEditor FormantGridEditor_create (const char32 *title, FormantGrid data);

/* End of file FormantGridEditor.h */
#endif
