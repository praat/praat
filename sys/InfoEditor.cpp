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
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
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
				The following is an attempt to explicitly perform the actions that event polling is supposed to perform.
				It would be nice not to actually have to poll events (with nextEventMatchingMask),
				because we are not interested in the events; we're interested only in the graphics update.
			*/
			//[theReferenceToTheOnlyInfoEditor -> d_windowForm -> d_cocoaWindow   displayIfNeeded];   // apparently, this does not suffice
			//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   lockFocus];   // this displays the menu as well as the text
			[theReferenceToTheOnlyInfoEditor -> d_windowForm -> d_cocoaWindow   display];   // this displays the menu as well as the text
			//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   displayIfNeeded];   // this displays only the text
			//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   display];
			//[theReferenceToTheOnlyInfoEditor -> textWidget -> d_cocoaTextView   unlockFocus];   // this displays the menu as well as the text
			[theReferenceToTheOnlyInfoEditor -> d_windowForm -> d_cocoaWindow   flushWindow];
			[NSApp  updateWindows];   // called automatically?
		#endif
	#elif defined (macintosh)
		GuiShell_drain (theReferenceToTheOnlyInfoEditor -> d_windowForm);
	#endif
}

/* End of file InfoEditor.cpp */
