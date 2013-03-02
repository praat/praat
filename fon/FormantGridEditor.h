#ifndef _FormantGridEditor_h_
#define _FormantGridEditor_h_
/* FormantGridEditor.h
 *
 * Copyright (C) 2008-2011,2012,2013 Paul Boersma & David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include "FunctionEditor.h"
#include "FormantGrid.h"

Thing_define (FormantGridEditor, FunctionEditor) {
	// new data:
	public:
		bool editingBandwidths;
		long selectedFormant;
		double ycursor;
	// overridden methods:
		virtual void v_createMenus ();
		virtual void v_draw ();
		virtual int v_click (double xWC, double yWC, bool shiftKeyPressed);
		virtual void v_play (double tmin, double tmax);
	// new methods:
		virtual bool v_hasSourceMenu () { return true; }
	// preferences:
		#include "FormantGridEditor_prefs.h"
};

void FormantGridEditor_init (FormantGridEditor me, const wchar_t *title, FormantGrid data);

FormantGridEditor FormantGridEditor_create (const wchar_t *title, FormantGrid data);

/* End of file FormantGridEditor.h */
#endif
