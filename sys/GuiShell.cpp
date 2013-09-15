/* GuiShell.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2013 Tom Naughton
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
		if (d_cocoaWindow) {
			[d_cocoaWindow setUserData: NULL];   // undangle reference to this
			Melder_fatal ("ordering out?");
			[d_cocoaWindow orderOut: nil];
			[d_cocoaWindow close];
			[d_cocoaWindow release];
			d_cocoaWindow = NULL;   // undangle
		}
	#endif
	GuiShell_Parent :: v_destroy ();
}

int structGuiShell :: f_getShellWidth () {
	int width = 0;
	#if gtk
		width = GTK_WIDGET (d_gtkWindow) -> allocation.width;
	#elif cocoa
        return [d_cocoaWindow frame].size.width;
	#elif motif
		width = d_xmShell -> width;
	#endif
	return width;
}

int structGuiShell :: f_getShellHeight () {
	int height = 0;
	#if gtk
		height = GTK_WIDGET (d_gtkWindow) -> allocation.height;
	#elif cocoa
        return [d_cocoaWindow frame].size.height;
	#elif motif
		height = d_xmShell -> height;
	#endif
	return height;
}

void structGuiShell :: f_setTitle (const wchar_t *title) {
	#if gtk
		gtk_window_set_title (d_gtkWindow, Melder_peekWcsToUtf8 (title));
	#elif cocoa
		[d_cocoaWindow setTitle: (NSString *) Melder_peekWcsToCfstring (title)];
	#elif win
		SetWindowText (d_xmShell -> window, title);
	#elif mac
		SetWindowTitleWithCFString (d_xmShell -> nat.window.ptr, (CFStringRef) Melder_peekWcsToCfstring (title));
	#endif
}

void structGuiShell :: f_drain () {
	#if gtk
		//gdk_window_flush (gtk_widget_get_window (me));
		gdk_flush ();
	#elif cocoa
        //[d_cocoaWindow displayIfNeeded];
        [d_cocoaWindow flushWindow];
		[d_cocoaWindow display];
	#elif win
	#elif mac
		Melder_assert (d_xmShell != NULL);
		Melder_assert (d_xmShell -> nat.window.ptr != NULL);
		QDFlushPortBuffer (GetWindowPort (d_xmShell -> nat.window.ptr), NULL);
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
		QDAddRectToDirtyRegion (GetWindowPort (d_xmShell -> nat.window.ptr), & bounds);
	#endif
}

/* End of file GuiShell.cpp */
