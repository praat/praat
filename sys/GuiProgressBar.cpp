/* GuiProgressBar.cpp
 *
 * Copyright (C) 1993-2012,2013,2015 Paul Boersma, 2008 Stefan de Konink
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

Thing_implement (GuiProgressBar, GuiControl, 0);

#if gtk
	static void _guiGtkProgressBar_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiProgressBar);
		Melder_free (me);
	}
#elif cocoa
	@implementation GuiCocoaProgressBar {
		GuiProgressBar d_userData;
	}
	- (void) dealloc {   // override
		GuiProgressBar me = d_userData;
		forget (me);
		trace (U"deleting a progress bar");
		[super dealloc];
	}
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiProgressBar));
		d_userData = static_cast <GuiProgressBar> (userData);
	}
	@end
#elif motif
	static void _guiMotifProgressBar_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		trace (U"destroying a progress bar");
		iam (GuiProgressBar);
		//forget (me);   // because I am already forgotten in the scale::destroy callback
		trace (U"destroyed a progress bar");
	}
#endif

GuiProgressBar GuiProgressBar_create (GuiForm parent, int left, int right, int top, int bottom, uint32 /* flags */)
{
	autoGuiProgressBar me = Thing_new (GuiProgressBar);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		my d_widget = gtk_progress_bar_new ();
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif cocoa
		my d_cocoaProgressBar = [[GuiCocoaProgressBar alloc] init];
		my d_widget = my d_cocoaProgressBar;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[my d_cocoaProgressBar   setUserData: me.get()];
		[my d_cocoaProgressBar   setIndeterminate: false];
		[my d_cocoaProgressBar   setMaxValue: 1.0];
	#elif motif
		my d_widget = XmCreateScale (parent -> d_widget, "scale", nullptr, 0);
		_GuiObject_setUserData (my d_widget, me.get());
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		XtVaSetValues (my d_widget, XmNorientation, XmHORIZONTAL,
			XmNminimum, 0, XmNmaximum, 10000, XmNvalue, 0,
			#if ! defined (macintosh)
				//XmNscaleHeight, 20,
			#endif
			#ifdef macintosh
				//XmNscaleWidth, 340,
			#endif
			nullptr);
	#endif

	#if gtk
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_guiGtkProgressBar_destroyCallback), me.get());
	#elif cocoa
	#elif motif
		XtAddCallback (my d_widget, XmNdestroyCallback, _guiMotifProgressBar_destroyCallback, me.get());
	#endif

	return me.releaseToAmbiguousOwner();
}

GuiProgressBar GuiProgressBar_createShown (GuiForm parent, int left, int right, int top, int bottom, uint32 flags)
{
	GuiProgressBar me = GuiProgressBar_create (parent, left, right, top, bottom, flags);
	GuiThing_show (me);
	return me;
}

void GuiProgressBar_setValue (GuiProgressBar me, double value) {
	#if gtk
		gtk_progress_bar_set_fraction (GTK_PROGRESS_BAR (my d_widget), value);
	#elif cocoa
		[my d_cocoaProgressBar   setDoubleValue: value];
	#elif motif
		XmScaleSetValue (my d_widget, round (value * 10000));
	#endif
}

/* End of file GuiProgressBar.cpp */
