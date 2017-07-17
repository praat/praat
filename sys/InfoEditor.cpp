/* InfoEditor.cpp
 *
 * Copyright (C) 2004-2011,2012,2013,2015,2017 Paul Boersma
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

#include "InfoEditor.h"

Thing_implement (InfoEditor, TextEditor, 0);

static InfoEditor theReferenceToTheOnlyInfoEditor;

void structInfoEditor :: v_destroy () noexcept {
	theReferenceToTheOnlyInfoEditor = nullptr;   // undangle
	InfoEditor_Parent :: v_destroy ();
}

void structInfoEditor :: v_clear () {
	Melder_clearInfo ();
}

InfoEditor InfoEditor_getTheReferenceToTheOnlyInstance () {
	if (! theReferenceToTheOnlyInfoEditor) {
		autoInfoEditor editor = Thing_new (InfoEditor);
		TextEditor_init (editor.get(), U"");
		Thing_setName (editor.get(), U"Praat Info");
		theReferenceToTheOnlyInfoEditor = editor.get();
		editor.releaseToUser();
	}
	return theReferenceToTheOnlyInfoEditor;
}

void gui_information (const char32 *message);   // BUG
void gui_information (const char32 *message) {
	InfoEditor editor = InfoEditor_getTheReferenceToTheOnlyInstance ();
	GuiText_setString (editor -> textWidget, message);
	GuiThing_show (editor -> windowForm);
	/*
		Try to make sure that the invalidated text widget and the elements of the fronted window are
		redrawn before the next event.

		The following Praat script can test this:

		writeInfoLine: "hoi"
		for i to 100
			appendInfoLine: i
		endfor
		
		The Info window should scroll continuously while the lines are added,
		not just show the end result.
	*/
	#if cocoa
		#if 1
			NSEvent *nsEvent = [NSApp
				nextEventMatchingMask: NSAnyEventMask
				untilDate: [NSDate distantPast]
				inMode: NSDefaultRunLoopMode
				dequeue: NO
				];
			if (nsEvent) {
				NSUInteger nsEventType = [nsEvent type];
				if (nsEventType == NSKeyDown) NSBeep ();
				//[[nsEvent window]  sendEvent: nsEvent];
			}
		#else
			/*
				The following is an attempt to explicitly perform the actions that event waiting is supposed to perform.
				It would be nice not to actually have to wait for events (with nextEventMatchingMask),
				because we are not interested in the events; we're interested only in the graphics update.
			*/
			//[editor -> windowForm -> d_cocoaShell   displayIfNeeded];   // apparently, this does not suffice
			//[editor -> textWidget -> d_cocoaTextView   lockFocus];   // this displays the menu as well as the text
			[editor -> windowForm -> d_cocoaShell   display];   // this displays the menu as well as the text
			//[editor -> textWidget -> d_cocoaTextView   displayIfNeeded];   // this displays only the text
			//[editor -> textWidget -> d_cocoaTextView   display];
			//[editor -> textWidget -> d_cocoaTextView   unlockFocus];   // this displays the menu as well as the text
			[editor -> windowForm -> d_cocoaShell   flushWindow];
			[NSApp  updateWindows];   // called automatically?
		#endif
	#elif defined (macintosh)
		GuiShell_drain (editor -> windowForm);
	#endif
}

/* End of file InfoEditor.cpp */
