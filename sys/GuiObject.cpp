/* GuiObject.cpp
 *
 * Copyright (C) 1993-2012,2013 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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
#include "machine.h"

#if cocoa
	@interface GuiCocoaView : NSView
		- (GuiThing) userData;
		- (void) setUserData: (GuiThing) userData;
	@end
#endif

void * _GuiObject_getUserData (GuiObject widget) {
	void *userData = NULL;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (widget), "praat");
	#elif cocoa
		userData = [(GuiCocoaView *) widget   userData];
	#elif motif
		XtVaGetValues (widget, XmNuserData, & userData, NULL);
	#endif
	return userData;
}

void _GuiObject_setUserData (GuiObject widget, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (widget), "praat", userData);
	#elif cocoa
		[(GuiCocoaView *) widget   setUserData: (GuiThing) userData];
	#elif motif
		XtVaSetValues (widget, XmNuserData, userData, NULL);
	#endif
}

void GuiObject_destroy (GuiObject widget) {
	#if gtk
		gtk_widget_destroy (GTK_WIDGET (widget));
	#elif cocoa
		if ([widget isKindOfClass: [NSMenuItem class]]) {
			NSMenuItem *cocoaMenuItem = (NSMenuItem *) widget;
			[[cocoaMenuItem menu] removeItem: cocoaMenuItem];   // this also releases the item
		} else {
			Melder_assert ([widget isKindOfClass: [NSView class]]);
			NSView *cocoaView = (NSView *) widget;
			if (cocoaView == [[cocoaView window] contentView]) {
				[[cocoaView window] orderOut: nil];
				[[cocoaView window] close];
				[[cocoaView window] release];
			} else {
				[cocoaView removeFromSuperview];   // this also releases the view
			}
		}
	#elif motif
		XtDestroyWidget (widget);
	#endif
}

/* End of file GuiObject.cpp */
