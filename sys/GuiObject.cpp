/* GuiObject.cpp
 *
 * Copyright (C) 1993-2012,2013,2017 Paul Boersma, 2008 Stefan de Konink, 2010 Franz Brausse
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

#include "GuiP.h"
#include "machine.h"

#if cocoa
	@interface GuiCocoaView : NSView
		- (GuiThing) getUserData;
		- (void) setUserData: (GuiThing) userData;
	@end
#endif

void * _GuiObject_getUserData (GuiObject widget) {
	void *userData = nullptr;
	#if gtk
		userData = (void *) g_object_get_data (G_OBJECT (widget), "praat");
	#elif motif
		XtVaGetValues (widget, XmNuserData, & userData, nullptr);
	#elif cocoa
		userData = [(GuiCocoaView *) widget   getUserData];
	#endif
	return userData;
}

void _GuiObject_setUserData (GuiObject widget, void *userData) {
	#if gtk
		g_object_set_data (G_OBJECT (widget), "praat", userData);
	#elif motif
		XtVaSetValues (widget, XmNuserData, userData, nullptr);
	#elif cocoa
		[(GuiCocoaView *) widget   setUserData: (GuiThing) userData];
	#endif
}

void GuiObject_destroy (GuiObject widget) {
	#if gtk
		gtk_widget_destroy (GTK_WIDGET (widget));
	#elif motif
		XtDestroyWidget (widget);
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
	#endif
}

/* End of file GuiObject.cpp */
