/* GuiRowColumn.c
 *
 * Copyright (C) 1993-2010 Paul Boersma
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
 * pb 2010/11/28 removed Motif
 */

#include "GuiP.h"
#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_rowcolumn \
		GuiRowColumn me = widget -> userData
#else
	#define iam_rowcolumn \
		GuiRowColumn me = _GuiObject_getUserData (widget)
#endif

typedef struct structGuiRowColumn {
	GuiObject widget;
} *GuiRowColumn;

#if win || mac
	static void _GuiMotifRowColumn_destroyCallback (GuiObject widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiRowColumn);
		Melder_free (me);
	}
#endif

GuiObject GuiColumn_createShown (GuiObject parent, unsigned long flags) {
	GuiRowColumn me = Melder_calloc (struct structGuiRowColumn, 1);
	#if gtk
	#elif win || mac
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent, XmNorientation, XmVERTICAL, NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#endif
	return my widget;
}

GuiObject GuiRow_createShown (GuiObject parent, unsigned long flags) {
	GuiRowColumn me = Melder_calloc (struct structGuiRowColumn, 1);
	#if gtk
	#elif win || mac
		my widget = XtVaCreateManagedWidget ("column1", xmRowColumnWidgetClass, parent, XmNorientation, XmHORIZONTAL, NULL);
		_GuiObject_setUserData (my widget, me);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifRowColumn_destroyCallback, me);
	#endif
	return my widget;
}

/* End of file GuiRowColumn.c */
