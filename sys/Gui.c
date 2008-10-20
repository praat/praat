/* Gui.c
 *
 * Copyright (C) 1992-2008 Paul Boersma
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

/*
 * pb 2002/03/07 GPL
 * pb 2002/03/11 Mach
 * pb 2004/10/21 on Unix, Ctrl becomes the command key
 * pb 2007/06/09 wchar_t
 * pb 2007/12/13 Gui
 * pb 2007/12/30 Gui
 * sdk 2008/02/08 GTK
 * sdk 2008/03/24 GDK
 */

#include "Gui.h"
#include <math.h>   // floor

Widget Gui_addMenuBar (Widget form) {
	Widget menuBar;
	#if gtk
		menuBar = gtk_menu_bar_new ();
		gtk_box_pack_start (GTK_BOX (form), menuBar, FALSE, FALSE, 0);
	#elif motif
		menuBar = XmCreateMenuBar (form, "menuBar", NULL, 0);
		XtVaSetValues (menuBar, XmNleftAttachment, XmATTACH_FORM, XmNrightAttachment, XmATTACH_FORM, NULL);
	#endif
	return menuBar;
}

int Gui_getResolution (Widget widget) {
	static int resolution = 0;
	if (resolution == 0) {
		#if defined (macintosh)
			(void) widget;
			CGSize size = CGDisplayScreenSize (kCGDirectMainDisplay);
			resolution = floor (25.4 * (double) CGDisplayPixelsWide (kCGDirectMainDisplay) / size.width + 0.5);
			//resolution = 100;
		#elif defined (_WIN32)
			(void) widget;
			resolution = 100;
		#else
			#if gtk
				resolution = gdk_screen_get_resolution (gdk_display_get_default_screen (gtk_widget_get_display (widget)));
			#elif motif
				Display *display = XtDisplay (widget);
				double width_pixels = DisplayWidth (display, DefaultScreen (display));
				double width_mm = DisplayWidthMM (display, DefaultScreen (display));
				resolution = floor (25.4 * width_pixels / width_mm + 0.5);
				//Melder_casual ("Gui_getResolution: display width %g %g %d", width_pixels, width_mm, resolution);
			#else
				Melder_fatal ("Gui_getResolution: unknown framework.");
			#endif
		#endif
	}
	return resolution;
}

/* End of file Gui.c */
