/* GuiRowColumn.c
 *
 * Copyright (C) 1993-2007 Paul Boersma
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
 * pb 2007/12/30
 */

#include "GuiP.h"
#define my  me ->
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_rowcolumn \
		GuiRowColumn me = widget -> userData
#else
	#define iam_rowcolumn \
		GuiRowColumn me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiRowColumn {
	Widget widget;
} *GuiRowColumn;

#if gtk
#elif motif
	static void _GuiMotifRowColumn_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiRowColumn);
		Melder_free (me);
	}
#endif

Widget GuiColumn_createShown (Widget parent, unsigned long flags) {
	GuiRowColumn me = Melder_calloc (struct structGuiRowColumn, 1);
	#if gtk
	#elif win || mac
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent, XmNorientation, XmVERTICAL, NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#elif motif
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent,
			XmNorientation, XmVERTICAL,
			XmNisAligned, False,
			XmNpacking, (flags & Gui_HOMOGENEOUS) != 0 ? XmPACK_COLUMN : XmPACK_TIGHT,
			NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#endif
	return my widget;
}

Widget GuiRow_createShown (Widget parent, unsigned long flags) {
	GuiRowColumn me = Melder_calloc (struct structGuiRowColumn, 1);
	#if gtk
	#elif win || mac
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent, XmNorientation, XmHORIZONTAL, NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#elif motif
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent,
			XmNorientation, XmHORIZONTAL,
			XmNisAligned, False,
			XmNpacking, (flags & Gui_HOMOGENEOUS) != 0 ? XmPACK_COLUMN : XmPACK_TIGHT,
			NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#endif
	return my widget;
}

/* End of file GuiRowColumn.c */
