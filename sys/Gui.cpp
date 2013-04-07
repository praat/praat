/* Gui.cpp
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma
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
 * sdk 2008/02/08 GTK
 * sdk 2008/03/24 GDK
 * pb 2010/11/29 removed explicit Motif
 * pb 2011/04/06 C++
 */

#include "GuiP.h"
#include "machine.h"
#include <math.h>   // floor
#include <locale.h>

Thing_implement (GuiThing, Thing, 0);
Thing_implement (GuiControl, GuiThing, 0);

int Gui_getResolution (GuiObject widget) {
	static int resolution = 0;
	if (0) {
		#if defined (macintosh)
			(void) widget;
			CGDirectDisplayID display = CGMainDisplayID ();
			CGSize size = CGDisplayScreenSize (display);
			resolution = floor (25.4 * (double) CGDisplayPixelsWide (display) / size.width + 0.5);
			//resolution = 72;
		#elif defined (_WIN32)
			(void) widget;
			resolution = 100;
		#elif gtk
			resolution = gdk_screen_get_resolution (gdk_display_get_default_screen (gtk_widget_get_display (GTK_WIDGET (widget))));
		#else
			Melder_fatal ("Gui_getResolution: unknown platform.");
		#endif
	}
	return 100;   // in conformance with most other applications; and so that fonts always look the same size in the Demo window
	return resolution;
}

#if gtk
void GuiGtk_initialize () {
	static bool gtkHasBeenInitialized = false;
	if (! gtkHasBeenInitialized) {
		trace ("before initing GTK: locale is %s", setlocale (LC_ALL, NULL));
		gtk_disable_setlocale ();   // otherwise 1.5 will be written "1,5" on computers with a French or German locale
		trace ("during initing GTK: locale is %s", setlocale (LC_ALL, NULL));
		gtk_init_check (NULL, NULL);
		trace ("after initing GTK: locale is %s", setlocale (LC_ALL, NULL));
		gtkHasBeenInitialized = true;
	}
	}
#endif

void Gui_getWindowPositioningBounds (double *x, double *y, double *width, double *height) {
	#if defined (macintosh)
		#if useCarbon
			HIRect rect;
			HIWindowGetAvailablePositioningBounds (kCGNullDirectDisplay, kHICoordSpaceScreenPixel, & rect);
			if (x) *x = rect. origin. x;
			if (y) *y = rect. origin. y;
			if (width) *width = rect. size. width;
			if (height) *height = rect. size. height - 22;   // subtract title bar height (or is it the menu height?)
		#else
    
            NSRect rect;
            NSArray *screenArray = [NSScreen screens];
            NSInteger screenCount = [screenArray count];
            NSInteger index  = 0;
            
            for (index = 0; index < screenCount; index++)
            {
                NSScreen *screen = [screenArray objectAtIndex: index];
                rect = [screen visibleFrame];
            }
        
		#endif
		if (x) *x = rect. origin. x;
		if (y) *y = rect. origin. y;
		if (width) *width = rect. size. width;
		if (height) *height = rect. size. height - 22;   // subtract title bar height (or is it the menu height?)
	#elif defined (_WIN32)
		#if 1
		RECT rect;
		SystemParametersInfo (SPI_GETWORKAREA, 0, & rect, 0);   // BUG: use GetMonitorInfo instead
		if (x) *x = rect. left;
		if (y) *y = rect. top;
		if (width) *width = rect. right - rect. left - 2 * GetSystemMetrics (SM_CXSIZEFRAME);
		if (height) *height = rect.bottom - rect.top - GetSystemMetrics (SM_CYCAPTION) - 2 * GetSystemMetrics (SM_CYSIZEFRAME);
		#else
		HMONITOR monitor = MonitorFromWindow (HWND window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO monitorInfo;
		monitorInfo. cbSize = sizeof (MONITORINFO);
		GetMonitorInfo (monitor, & monitorInfo);
		if (x) *x = monitorInfo. rcWork. left;
		if (y) *y = monitorInfo. rcWork. top;
		if (width) *width = monitorInfo. rcWork. right - monitorInfo. rcWork. left;
		if (height) *height = monitorInfo. rcWork.bottom - monitorInfo. rcWork.top /*- GetSystemMetrics (SM_CYMINTRACK)*/;   // SM_CXSIZEFRAME  SM_CYCAPTION
		#endif
	#elif gtk
		GuiGtk_initialize ();
		GdkScreen *screen = gdk_screen_get_default ();
		/*
		if (parent != NULL) {
			GuiObject parent_win = gtk_widget_get_ancestor (GTK_WIDGET (parent), GTK_TYPE_WINDOW);
			if (parent_win != NULL) {
				screen = gtk_window_get_screen (GTK_WINDOW (parent_win));
			}
		}
		*/
		if (x) *x = 0;
		if (y) *y = 0;
		if (width) *width = gdk_screen_get_width (screen);
		if (height) *height = gdk_screen_get_height (screen);
	#else
		if (x) *x = 0;
		if (y) *y = 0;
		if (width) *width = WidthOfScreen (DefaultScreenOfDisplay (XtDisplay (parent)));
		if (height) *height = HeightOfScreen (DefaultScreenOfDisplay (XtDisplay (parent)));
	#endif
}

/* End of file Gui.cpp */
