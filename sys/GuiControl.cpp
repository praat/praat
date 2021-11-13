/* GuiControl.cpp
 *
 * Copyright (C) 1993-2012,2013,2015,2017 Paul Boersma,
 *               2008 Stefan de Koninck, 2010 Franz Brausse, 2013 Tom Naughton
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

void structGuiControl :: v_positionInForm (GuiObject widget, int left, int right, int top, int bottom, GuiForm parent) {
	#if gtk
		/*
		 * Remember the location settings for resizing.
		 */
		d_left   = left;
		d_right  = right;
		d_top    = top;
		d_bottom = bottom;
		/*
		 */
		if (! parent)
			return;
		Melder_assert (parent -> d_widget);
		Melder_assert (GTK_IS_FIXED (parent -> d_widget));
		gint parentWidth, parentHeight;
		gtk_widget_get_size_request (GTK_WIDGET (parent -> d_widget), & parentWidth, & parentHeight);
		//parentWidth  = gtk_widget_get_allocated_width (GTK_WIDGET (parent -> d_widget));
		//parentHeight = gtk_widget_get_allocated_height (GTK_WIDGET (parent -> d_widget));
		//parentWidth = parent -> d_right;
		//parentHeight = parent -> d_top;
		if (left   <  0) left   += parentWidth;
		if (right  <= 0) right  += parentWidth;
        if (top    <  0) top    += parentHeight;
		if (bottom <= 0) bottom += parentHeight;
		trace (U"fixed: parent width ", parentWidth, U" height ", parentHeight);
		gtk_widget_set_size_request (GTK_WIDGET (widget), right - left, bottom - top);
		gtk_fixed_put (GTK_FIXED (parent -> d_widget), GTK_WIDGET (widget), left, top);
	#elif motif
		(void) parent;
		if (left >= 0) {
			if (right > 0) {
				XtVaSetValues (widget, XmNx, left, XmNwidth, right - left, nullptr);
			} else {
				XtVaSetValues (widget, XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, left, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, nullptr);
			}
		} else {
			Melder_assert (right <= 0);
			trace (U"parent width ", parent -> d_widget -> width);
			XtVaSetValues (widget, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, XmNwidth, right - left, nullptr);
			trace (U"parent width ", parent -> d_widget -> width);
		}
		if (top >= 0) {
			if (bottom > 0) {
				XtVaSetValues (widget, XmNy, top, XmNheight, bottom - top, nullptr);
			} else {
				XtVaSetValues (widget, XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, top, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, nullptr);
			}
		} else {
			Melder_assert (bottom <= 0);
			XtVaSetValues (widget, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, bottom - top, nullptr);
		}
	#elif cocoa
        NSView *superView = (NSView *) parent -> d_widget;
        NSView *widgetView = (NSView *) widget;
		NSRect parentRect = [superView frame];
        int parentWidth = parentRect.size.width;
        int parentHeight = parentRect.size.height;
    
        NSUInteger horizMask = 0;
        if (left >= 0) {
            if (right <= 0) {
                horizMask = NSViewWidthSizable;
            }
        } else {
            horizMask = NSViewMinXMargin;
        }
        
        NSUInteger vertMask = 0;
        if (top >= 0) {
            vertMask = NSViewMinYMargin;
            if (bottom <= 0) {
                vertMask = NSViewHeightSizable;
            }
        }

		if (left   <  0) left   += parentWidth;
		if (right  <= 0) right  += parentWidth;
		if (top    <  0) top    += parentHeight;
		if (bottom <= 0) bottom += parentHeight;
		top = parentHeight - top;         // flip
		bottom = parentHeight - bottom;   // flip
        int width = right - left;
        int height = top - bottom;
		if ([widgetView isKindOfClass: [NSButton class]]) {
			if (! [widgetView isKindOfClass: [NSPopUpButton class]]) {
				/*
				 * On Cocoa, NSButton views show up 12 pixels less wide and 5 pixels less high than their frame.
				 * Compensate for this (undocumented?) Cocoa phenomenon.
				 */
				left -= 6;
				width += 12;
				bottom -= 5;
				height += 5;
			}
		}
        NSRect rect = NSMakeRect (left, bottom, width, height);
        [widgetView setAutoresizingMask: horizMask | vertMask];
        [superView addSubview: widgetView];   // parent will retain the subview...
        [widgetView setFrame: rect];
		[widgetView release];   // ... so we can release the item already
	#endif
}

void structGuiControl :: v_positionInScrolledWindow (GuiObject widget, int width, int height, GuiScrolledWindow parent) {
	#if gtk
		if (! parent)
			return;
		Melder_assert (GTK_IS_SCROLLED_WINDOW (parent -> d_widget));
		gtk_widget_set_size_request (GTK_WIDGET (widget), width, height);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (parent -> d_widget), GTK_WIDGET (widget));
	#elif motif
		(void) parent;
		XtVaSetValues (widget, XmNwidth, width, XmNheight, height, nullptr);
	#elif cocoa
		GuiCocoaScrolledWindow *scrolledWindow = (GuiCocoaScrolledWindow *) parent -> d_widget;
		NSView *widgetView = (NSView *) widget;
		NSRect rect = NSMakeRect (0, 0, width, height);
		[widgetView initWithFrame: rect];
		[widgetView setBounds: rect];
		[scrolledWindow setDocumentView: widgetView];
		[widgetView release];   // ... so we can release the item already
	#endif
}

int GuiControl_getX (GuiControl me) {
	#if gtk
		//return GTK_WIDGET (my d_widget) -> allocation.x;
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_widget), & allocation);
		return allocation.x;
	#elif motif
		return my d_widget -> x;
	#elif cocoa
		return [(NSView *) my d_widget frame]. origin. x;
	#else
		return 0;
	#endif
}

int GuiControl_getY (GuiControl me) {
	#if gtk
		//return GTK_WIDGET (my d_widget) -> allocation.y;
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_widget), & allocation);
		return allocation.y;
	#elif motif
		return my d_widget -> y;
	#elif cocoa
		return [(NSView *) my d_widget frame]. origin. y;
	#else
		return 0;
	#endif
}

int GuiControl_getWidth (GuiControl me) {
	#if gtk
		//return GTK_WIDGET (my d_widget) -> allocation.width;
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_widget), & allocation);
		return allocation.width;
	#elif motif
		return my d_widget -> width;
	#elif cocoa
		return [(NSView *) my d_widget frame]. size. width;
	#else
		return 0;
	#endif
}

int GuiControl_getHeight (GuiControl me) {
	#if gtk
		//return GTK_WIDGET (my d_widget) -> allocation.height;
		GtkAllocation allocation;
		gtk_widget_get_allocation (GTK_WIDGET (my d_widget), & allocation);
		return allocation.height;
	#elif motif
		return my d_widget -> height;
	#elif cocoa
		return [(NSView *) my d_widget frame]. size. height;
	#else
		return 0;
	#endif
}

void GuiControl_move (GuiControl me, int x, int y) {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (my d_widget));
		if (GTK_IS_FIXED (parent)) {
			gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (my d_widget), x, y);
		}
	#elif motif
		XtVaSetValues (my d_widget, XmNx, (Position) x, XmNy, (Position) y, nullptr);   // 64-bit-compatible
	#elif cocoa
	#endif
}

void GuiControl_setSize (GuiControl me, int width, int height) {
	#if gtk
		gtk_widget_set_size_request (GTK_WIDGET (my d_widget), width, height);
	#elif motif
		XtVaSetValues (my d_widget, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, nullptr);   // 64-bit-compatible
	#elif cocoa
	#endif
}

/* End of file GuiControl.cpp */
