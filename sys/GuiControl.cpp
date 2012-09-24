/* GuiControl.cpp
 *
 * Copyright (C) 1993-2012 Paul Boersma, 2008 Stefan de Koninck, 2010 Franz Brausse
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

int structGuiControl :: f_getX () {
	#if gtk
		return GTK_WIDGET (d_widget) -> allocation.x;
	#elif cocoa
		return 100;//[(NSView *) d_widget x];
	#elif motif
		return d_widget -> x;
	#endif
}

int structGuiControl :: f_getY () {
	#if gtk
		return GTK_WIDGET (d_widget) -> allocation.y;
	#elif cocoa
		return 100;//[(NSView *) d_widget y];
	#elif motif
		return d_widget -> y;
	#endif
}

int structGuiControl :: f_getWidth () {
	#if gtk
		return GTK_WIDGET (d_widget) -> allocation.width;
	#elif cocoa
		return 100;//[(NSView *) d_widget width];
	#elif motif
		return d_widget -> width;
	#endif
}

int structGuiControl :: f_getHeight () {
	#if gtk
		return GTK_WIDGET (d_widget) -> allocation.height;
	#elif cocoa
		return 100;//[(NSView *) d_widget height];
	#elif motif
		return d_widget -> height;
	#endif
}

void structGuiControl :: f_move (int x, int y) {
	#if gtk
		GuiObject parent = gtk_widget_get_parent (GTK_WIDGET (d_widget));
		if (GTK_IS_FIXED (parent)) {
			gtk_fixed_move (GTK_FIXED (parent), GTK_WIDGET (d_widget), x, y);
		}
	#elif cocoa
	#elif motif
		XtVaSetValues (d_widget, XmNx, (Position) x, XmNy, (Position) y, NULL);   // 64-bit-compatible
	#endif
}

void structGuiControl :: f_setSize (int width, int height) {
	#if gtk
		gtk_widget_set_size_request (GTK_WIDGET (d_widget), width, height);
	#elif cocoa
	#elif motif
		XtVaSetValues (d_widget, XmNwidth, (Dimension) width, XmNheight, (Dimension) height, NULL);   // 64-bit-compatible
	#endif
}

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
		if (! parent) return;
		gint parentWidth, parentHeight;
		gtk_widget_get_size_request (GTK_WIDGET (parent -> d_widget), & parentWidth, & parentHeight);
		if (left   <  0) left   += parentWidth;
		if (right  <= 0) right  += parentWidth;
		if (top    <  0) top    += parentHeight;
		if (bottom <= 0) bottom += parentHeight;
		Melder_assert (parent -> d_widget);
		Melder_assert (GTK_IS_FIXED (parent -> d_widget));
		if (GTK_IS_FIXED (parent -> d_widget)) {
			trace ("fixed: parent width %d height %d", parentWidth, parentHeight);
			gtk_widget_set_size_request (GTK_WIDGET (widget), right - left, bottom - top);
			gtk_fixed_put (GTK_FIXED (parent -> d_widget), GTK_WIDGET (widget), left, top);
		} else if (GTK_IS_BOX (parent -> d_widget)) {
			trace ("box: parent width %d height %d", parentWidth, parentHeight);
			gtk_box_pack_start (GTK_BOX (parent -> d_widget), GTK_WIDGET (widget), TRUE, TRUE, 0);
		} else {
			trace ("container: parent width %d height %d", parentWidth, parentHeight);
			gtk_widget_set_size_request (GTK_WIDGET (widget), right - left, bottom - top);
			gtk_container_add (GTK_CONTAINER (parent -> d_widget), GTK_WIDGET (widget));
		}
	#elif cocoa
		NSRect parentRect = [(NSView *) parent frame];
		int parentWidth = parentRect.size.width, parentHeight = parentRect.size.height;
		if (left   <  0) left   += parentWidth;
		if (right  <= 0) right  += parentWidth;
		if (top    <  0) top    += parentHeight;
		if (bottom <= 0) bottom += parentHeight;
		top = parentHeight - top;         // flip
		bottom = parentHeight - bottom;   // flip
		NSRect rect = { { left, bottom }, { right - left, top - bottom } };
		[(NSView *) widget initWithFrame: rect];
		[(NSView *) widget setBounds: rect];
		[(NSView *) parent -> d_widget addSubview: (NSView *) widget];   // parent will retain the subview...
		[(NSButton *) widget release];   // ... so we can release the item already
	#elif motif
		(void) parent;
		if (left >= 0) {
			if (right > 0) {
				XtVaSetValues (widget, XmNx, left, XmNwidth, right - left, NULL);
			} else {
				XtVaSetValues (widget, XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, left, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, NULL);
			}
		} else {
			Melder_assert (right <= 0);
			XtVaSetValues (widget, XmNrightAttachment, XmATTACH_FORM, XmNrightOffset, - right, XmNwidth, right - left, NULL);
		}
		if (top >= 0) {
			if (bottom > 0) {
				XtVaSetValues (widget, XmNy, top, XmNheight, bottom - top, NULL);
			} else {
				XtVaSetValues (widget, XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, top, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, NULL);
			}
		} else {
			Melder_assert (bottom <= 0);
			XtVaSetValues (widget, XmNbottomAttachment, XmATTACH_FORM, XmNbottomOffset, - bottom, XmNheight, bottom - top, NULL);
		}
	#endif
}

void structGuiControl :: v_positionInScrolledWindow (GuiObject widget, int width, int height, GuiScrolledWindow parent) {
	#if gtk
		if (! parent) return;
		Melder_assert (GTK_IS_SCROLLED_WINDOW (parent -> d_widget));
		gtk_widget_set_size_request (GTK_WIDGET (widget), width, height);
		gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (parent -> d_widget), GTK_WIDGET (widget));
	#elif cocoa
	#elif motif
		(void) parent;
		XtVaSetValues (widget, XmNwidth, width, XmNheight, height, NULL);
	#endif
}

/* End of file GuiControl.cpp */
