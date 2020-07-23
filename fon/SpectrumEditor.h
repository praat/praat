#ifndef _SpectrumEditor_h_
#define _SpectrumEditor_h_
/* SpectrumEditor.h
 *
 * Copyright (C) 1992-2005,2007-2013,2015,2016,2018-2020 Paul Boersma
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
#include "Spectrum.h"

Thing_define (SpectrumEditor, FunctionEditor) {
	double minimum, maximum, cursorHeight;
	GuiMenuItem publishBandButton, publishSoundButton;

	void v_createMenus ()
		override;
	void v_createHelpMenuItems (EditorMenu menu)
		override;
	void v_dataChanged ()
		override;
	void v_draw ()
		override;
	bool v_mouseInWideDataView (GuiDrawingArea_MouseEvent event, double x_world, double y_fraction)
		override;
	void v_play (double tmin, double tmax)
		override;
	void v_createMenuItems_view (EditorMenu menu)
		override;
	conststring32 v_domainName ()
		override { return U"frequency"; }
	conststring32 v_format_domain ()
		override { return U"Frequency domain:"; }
	const char * v_format_short ()
		override { return "%.0f"; }
	const char * v_format_long ()
		override { return "%.2f"; }
	int v_fixedPrecision_long ()
		override { return 2; }
	conststring32 v_format_units_long ()
		override { return U"hertz"; }
	conststring32 v_format_units_short ()
		override { return U"Hz"; }
	const char * v_format_totalDuration ()
		override { return u8"Total bandwidth %.2f hertz"; }
	const char * v_format_window ()
		override { return u8"Visible part %.2f hertz"; }
	const char * v_format_selection ()
		override { return u8"%.2f Hz"; }

	#include "SpectrumEditor_prefs.h"
};

autoSpectrumEditor SpectrumEditor_create (conststring32 title, Spectrum data);

/* End of file SpectrumEditor.h */
#endif
