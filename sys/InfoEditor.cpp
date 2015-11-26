/* InfoEditor.cpp
 *
 * Copyright (C) 2004-2011,2012,2013,2015 Paul Boersma
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

#include "InfoEditor.h"

Thing_implement (InfoEditor, TextEditor, 0);

static InfoEditor theReferenceToTheOnlyInfoEditor;

void structInfoEditor :: v_destroy () {
	theReferenceToTheOnlyInfoEditor = nullptr;   // undangle
	InfoEditor_Parent :: v_destroy ();
}

void structInfoEditor :: v_clear () {
	Melder_clearInfo ();
}

void gui_information (const char32 *message);   // BUG
void gui_information (const char32 *message) {
	if (! theReferenceToTheOnlyInfoEditor) {
		autoInfoEditor editor = Thing_new (InfoEditor);
		TextEditor_init (editor.get(), U"");
		Thing_setName (editor.get(), U"Praat Info");
		theReferenceToTheOnlyInfoEditor = editor.get();
		editor.releaseToUser();
	}
	GuiText_setString (theReferenceToTheOnlyInfoEditor -> textWidget, message);
	GuiThing_show (theReferenceToTheOnlyInfoEditor -> d_windowForm);
	/*
	 * Try to make sure that the invalidated text widget and the elements of the fronted window are redrawn before the next event.
	 */
	#if cocoa
		//[theReferenceToTheOnlyInfoEditor -> d_windowForm -> d_cocoaWindow   displayIfNeeded];   // apparently, this does not suffice
		//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   lockFocus];   // this displays the menu as well as the text
		[theReferenceToTheOnlyInfoEditor -> d_windowForm -> d_cocoaWindow   display];   // this displays the menu as well as the text
		//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   displayIfNeeded];   // this displays only the text
		//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   display];
		//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   unlockFocus];   // this displays the menu as well as the text
	#elif defined (macintosh)
		GuiShell_drain (theReferenceToTheOnlyInfoEditor -> d_windowForm);
	#endif
}

/* End of file InfoEditor.cpp */
