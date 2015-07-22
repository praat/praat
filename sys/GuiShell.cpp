/* GuiShell.cpp
 *
 * Copyright (C) 1993-2012,2015 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"
#include "UnicodeData.h"

Thing_implement (GuiShell, GuiForm, 0);

void structGuiShell :: v_destroy () {
	#if cocoa
		if (our d_cocoaWindow) {
			[our d_cocoaWindow setUserData: NULL];   // undangle reference to this
			Melder_fatal (U"ordering out?");
			[our d_cocoaWindow orderOut: nil];
			[our d_cocoaWindow close];
			[our d_cocoaWindow release];
			our d_cocoaWindow = NULL;   // undangle
		}
	#endif
	GuiShell_Parent :: v_destroy ();
}

int GuiShell_getShellWidth (GuiShell me) {
	int width = 0;
	#if gtk
		width = GTK_WIDGET (my d_gtkWindow) -> allocation.width;
	#elif cocoa
        return [my d_cocoaWindow frame].size.width;
	#elif motif
		width = my d_xmShell -> width;
	#endif
	return width;
}

int GuiShell_getShellHeight (GuiShell me) {
	int height = 0;
	#if gtk
		height = GTK_WIDGET (my d_gtkWindow) -> allocation.height;
	#elif cocoa
        return [my d_cocoaWindow frame].size.height;
	#elif motif
		height = my d_xmShell -> height;
	#endif
	return height;
}

void GuiShell_setTitle (GuiShell me, const char32 *title /* cattable */) {
	#if gtk
		gtk_window_set_title (my d_gtkWindow, Melder_peek32to8 (title));
	#elif cocoa
		[my d_cocoaWindow setTitle: (NSString *) Melder_peek32toCfstring (title)];
	#elif win
		SetWindowTextW (my d_xmShell -> window, Melder_peek32toW (title));
	#elif mac
		SetWindowTitleWithCFString (my d_xmShell -> nat.window.ptr, (CFStringRef) Melder_peek32toCfstring (title));
	#endif
}

void GuiShell_drain (GuiShell me) {
	#if gtk
		//gdk_window_flush (gtk_widget_get_window (my d_gtkWindow));
		gdk_flush ();
	#elif cocoa
        //[my d_cocoaWindow   displayIfNeeded];
        [my d_cocoaWindow   flushWindow];
		//[my d_cocoaWindow   display];
	#elif win
	#elif mac
		Melder_assert (my d_xmShell != NULL);
		Melder_assert (my d_xmShell -> nat.window.ptr != NULL);
		QDFlushPortBuffer (GetWindowPort (my d_xmShell -> nat.window.ptr), NULL);
		/*
		 * The following TRICK cost me half a day to work out.
		 * It turns out that after a call to QDFlushPortBuffer (),
		 * it takes MacOS ages to compute a new dirty region while
		 * the next graphics commands are executed. Such a dirty region
		 * could well be the region that includes all the pixels drawn by
		 * the graphics commands, and nothing else. One can imagine
		 * that such a thing takes five seconds when the graphics is
		 * a simple Graphics_function () of e.g. noise.
		 */
		static Rect bounds = { -32768, -32768, 32767, 32767 };
		QDAddRectToDirtyRegion (GetWindowPort (my d_xmShell -> nat.window.ptr), & bounds);
	#endif
}

/* End of file GuiShell.cpp */
