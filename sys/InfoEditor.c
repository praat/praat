/* InfoEditor.c
 *
 * Copyright (C) 2004-2007 Paul Boersma
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
 * pb 2004/09/13
 * pb 2004/10/21 clear method also clears the info buffer, not just the visible text
 * pb 2007/05/24 wchar_t
 */

#include "TextEditor.h"

#define InfoEditor_members TextEditor_members
#define InfoEditor_methods TextEditor_methods
class_create (InfoEditor, TextEditor)

static InfoEditor theInfoEditor;

static void destroy (I) {
	iam (InfoEditor);
	theInfoEditor = NULL;
	inherited (InfoEditor) destroy (me);
}

static void clear (I) {
	iam (TextEditor);
	(void) me;
	Melder_clearInfo ();
}

class_methods (InfoEditor, TextEditor)
	class_method (destroy)
	us -> scriptable = FALSE;
	us -> fileBased = FALSE;
	class_method (clear)
class_methods_end

void motif_information (wchar_t *message);
void motif_information (wchar_t *message) {
	if (! theInfoEditor) {
		theInfoEditor = new (InfoEditor);
		TextEditor_init (theInfoEditor, Melder_topShell, "");
		Thing_setName (theInfoEditor, "Praat: Info");
	}
	static MelderStringA messageA = { 0 };
	MelderStringA_copyW (& messageA, message);
	XmTextSetString (theInfoEditor -> textWidget, messageA.string);
	XMapRaised (XtDisplay (theInfoEditor -> shell), XtWindow (theInfoEditor -> shell));
	GuiWindow_drain (theInfoEditor -> shell);
}

/* End of file InfoEditor.c */
