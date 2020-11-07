/* Gui_messages.cpp
 *
 * Copyright (C) 1992-2018,2020 Paul Boersma,
 *               2008 Stefan de Konink, 2010 Franz Brausse, 2013 Tom Naughton
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

#include <time.h>

#include <assert.h>
#ifdef _WIN32
	#include <windows.h>
#endif

#include "melder.h"
#include "Graphics.h"
#include "Gui.h"

/********** Exported variable. **********/

static GuiWindow Melder_topShell;

/********** PROGRESS **********/

static bool theProgressCancelled = false;

static bool waitWhileProgress (double progress, conststring32 message, GuiDialog dia,
	GuiProgressBar scale, GuiLabel label1, GuiLabel label2, GuiButton cancelButton)
{
	#if gtk
		// Wait for all pending events to be processed. If anybody knows how to inspect GTK's
		// event queue for specific events, dump the code here, please.
		// Until then, the button click attaches a g_object data key named "pressed" to the cancelButton
		// which this function reads out in order to tell whether interruption has occurred
		while (gtk_events_pending ()) {
			trace (U"event pending");
			gtk_main_iteration ();
		}
	#elif motif
		XEvent event;
		while (PeekMessage (& event, 0, 0, 0, PM_REMOVE)) {
			if (event. message == WM_KEYDOWN) {
				/*
				 * Ignore all key-down messages, except Escape.
				 */
				if (LOWORD (event. wParam) == VK_ESCAPE) {
					XtUnmanageChild (dia -> d_widget);
					return false;   // don't continue
				}
			} else if (event. message == WM_LBUTTONDOWN) {
				/*
				 * Ignore all mouse-down messages, except click in Interrupt button.
				 */
				GuiObject me = (GuiObject) GetWindowLongPtr (event. hwnd, GWLP_USERDATA);
				if (me == cancelButton -> d_widget) {
					XtUnmanageChild (dia -> d_widget);
					return false;   // don't continue
				}
			} else if (event. message != WM_SYSKEYDOWN) {
				/*
				 * Process paint messages etc.
				 */
				DispatchMessage (& event);
			}
		}
	#elif cocoa
		while (NSEvent *nsEvent = [NSApp
			nextEventMatchingMask: NSAnyEventMask
			untilDate: [NSDate distantPast]
			inMode: NSDefaultRunLoopMode
			dequeue: YES
			])
		{
			NSUInteger nsEventType = [nsEvent type];
			if (nsEventType == NSKeyDown) NSBeep ();
			[[nsEvent window]  sendEvent: nsEvent];
		}
	#endif
	if (progress >= 1.0) {
		GuiThing_hide (dia);
	} else {
		if (progress <= 0.0) progress = 0.0;
		GuiThing_show (dia);   // TODO: prevent raising to the front
		const char32 *newline = str32chr (message, U'\n');
		if (newline) {
			static MelderString buffer;
			MelderString_copy (& buffer, message);
			buffer.string [newline - message] = U'\0';
			GuiLabel_setText (label1, buffer.string);
			buffer.string [newline - message] = U'\n';
			GuiLabel_setText (label2, buffer.string + (newline - message) + 1);
		} else {
			GuiLabel_setText (label1, message);
			GuiLabel_setText (label2, U"");
		}
		#if gtk
			trace (U"update the progress bar");
			GuiProgressBar_setValue (scale, progress);
			while (gtk_events_pending ()) {
				trace (U"event pending");
				gtk_main_iteration ();
			}
			trace (U"check whether the cancel button has the \"pressed\" key set");
			if (g_object_steal_data (G_OBJECT (cancelButton -> d_widget), "pressed")) {
				trace (U"the cancel button has been pressed");
				return false;   // don't continue
			}
		#elif motif
			GuiProgressBar_setValue (scale, progress);
			GdiFlush ();
		#elif cocoa
			GuiProgressBar_setValue (scale, progress);
			//[scale -> d_cocoaProgressBar   displayIfNeeded];
			if (theProgressCancelled) {
				theProgressCancelled = false;
				return false;
			}
		#endif
	}
	trace (U"continue");
	return true;
}

static GuiButton theProgressCancelButton = nullptr;

#if gtk || cocoa
	static void progress_dia_close (Thing /* boss */) {
		theProgressCancelled = true;
		#if gtk
			g_object_set_data (G_OBJECT (theProgressCancelButton -> d_widget), "pressed", (gpointer) 1);
		#endif
	}
	static void progress_cancel_btn_press (Thing /* boss */, GuiButtonEvent /* event */) {
		theProgressCancelled = true;
		#if gtk
			g_object_set_data (G_OBJECT (theProgressCancelButton -> d_widget), "pressed", (gpointer) 1);
		#endif
	}
#endif

static void _Melder_dia_init (GuiDialog *dia, GuiProgressBar *scale, GuiLabel *label1, GuiLabel *label2, GuiButton *cancelButton, bool hasMonitor) {
	trace (U"creating the dialog");
	*dia = GuiDialog_create (Melder_topShell, 200, 100, 400, hasMonitor ? 430 : 200, U"Work in progress",
		#if gtk || cocoa
			progress_dia_close, nullptr,
		#else
			nullptr, nullptr,
		#endif
		0);

	trace (U"creating the labels");
	*label1 = GuiLabel_createShown (*dia, 3, 403, 0, Gui_LABEL_HEIGHT, U"label1", 0);
	*label2 = GuiLabel_createShown (*dia, 3, 403, 30, 30 + Gui_LABEL_HEIGHT, U"label2", 0);

	trace (U"creating the scale");
	*scale = GuiProgressBar_createShown (*dia, 3, -3, 70, 110, 0);

	trace (U"creating the cancel button");
	*cancelButton = GuiButton_createShown (*dia, 0, 400, 170, 170 + Gui_PUSHBUTTON_HEIGHT,
		U"Interrupt",
		#if gtk
			progress_cancel_btn_press, nullptr,
		#elif cocoa
			progress_cancel_btn_press, nullptr,
		#else
			nullptr, nullptr,
		#endif
		0);
	trace (U"end");
}

static void gui_progress (double progress, conststring32 message) {
	static clock_t lastTime;
	static GuiDialog dia = nullptr;
	static GuiProgressBar scale = nullptr;
	static GuiLabel label1 = nullptr, label2 = nullptr;
	clock_t now = clock ();
	if (progress <= 0.0 || progress >= 1.0 ||
		now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
	{
		if (! dia)
			_Melder_dia_init (& dia, & scale, & label1, & label2, & theProgressCancelButton, false);
		if (! waitWhileProgress (progress, message, dia, scale, label1, label2, theProgressCancelButton))
			Melder_throw (U"Interrupted!");
		lastTime = now;
	}
}

static autoGraphics graphics;

static void gui_drawingarea_cb_expose (Thing /* boss */, GuiDrawingArea_ExposeEvent /* event */) {
	if (! graphics)
		return;
	Graphics_play (graphics.get(), graphics.get());
}

static void * gui_monitor (double progress, conststring32 message) {
	static clock_t lastTime;
	static GuiDialog dia = nullptr;
	static GuiProgressBar scale = nullptr;
	static GuiDrawingArea drawingArea = nullptr;
	static GuiButton cancelButton = nullptr;
	static GuiLabel label1 = nullptr, label2 = nullptr;
	clock_t now = clock ();
	if (progress <= 0.0 || progress >= 1.0 ||
		now - lastTime > CLOCKS_PER_SEC / 4)   // this time step must be much longer than the null-event waiting time
	{
		if (! dia) {
			_Melder_dia_init (& dia, & scale, & label1, & label2, & cancelButton, true);
			drawingArea = GuiDrawingArea_createShown (dia, 0, 400, 230, 430,
					gui_drawingarea_cb_expose, nullptr, nullptr, nullptr, nullptr, 0);
			GuiThing_show (dia);
			graphics = Graphics_create_xmdrawingarea (drawingArea);
		}
		if (progress <= 0.0 && graphics) {
			Graphics_clearRecording (graphics.get());
			Graphics_startRecording (graphics.get());
			Graphics_clearWs (graphics.get());
		}
		if (! waitWhileProgress (progress, message, dia, scale, label1, label2, cancelButton))
			Melder_throw (U"Interrupted!");
		lastTime = now;
		if (progress == 0.0)
			return graphics.get();
	}
	return nullptr;
}

#if cocoa
	static void mac_message (NSAlertStyle macAlertType, conststring32 message32) {
		static char16 message16 [4000];
		integer messageLength = str32len (message32);
		uinteger j = 0;
		for (int i = 0; i < messageLength && j <= 4000 - 3; i ++) {
			char32 kar = message32 [i];
			if (kar <= 0x00'FFFF) {
				message16 [j ++] = (char16) kar;
			} else if (kar <= 0x10'FFFF) {
				kar -= 0x01'0000;
				message16 [j ++] = (char16) (0x00'D800 | (kar >> 10));
				message16 [j ++] = (char16) (0x00'DC00 | (kar & 0x00'03FF));
			}
		}
		message16 [j] = u'\0';   // append null byte because we are going to search this string

		/*
			Split up the message between header (will appear in bold) and rest.
			The split is done at the first line break, except if the first line ends in a colon,
			in which case the split is done at the second line break.
		*/
		const char16 *lineBreak = & message16 [0];
		for (; *lineBreak != u'\0'; lineBreak ++) {
			if (*lineBreak == u'\n') {
				break;
			}
		}
		if (*lineBreak == u'\n' && lineBreak - message16 > 0 && lineBreak [-1] == u':') {
			for (lineBreak ++; *lineBreak != u'\0'; lineBreak ++) {
				if (*lineBreak == u'\n') {
					break;
				}
			}
		}
		uinteger lengthOfFirstSentence = (uinteger) (lineBreak - message16);
		/*
			Create an alert dialog with an icon that is appropriate for the level.
		*/
		NSAlert *alert = [[NSAlert alloc] init];
		[alert setAlertStyle: macAlertType];
		/*
			Add the header in bold.
		*/
		NSString *header = [[NSString alloc] initWithCharacters: (const unichar *) & message16 [0]   length: lengthOfFirstSentence];   // note: init can change the object pointer!
		if (header) {   // make this very safe, because we can be at error time or at fatal time
			[alert setMessageText: header];
			[header release];
		}
		/*
			Add the rest of the message in small type.
		*/
		if (lengthOfFirstSentence < j) {
			NSString *rest = [[NSString alloc] initWithCharacters: (const unichar *) & lineBreak [1]   length: j - 1 - lengthOfFirstSentence];
			if (rest) {   // make this very safe, because we can be at error time or at fatal time
				[alert setInformativeText: rest];
				[rest release];
			}
		}
		/*
			Display the alert dialog and synchronously wait for the user to click OK.
			But: it is not impossible that the program crashes during `runModal`,
			especially if `runModal` is called at expose time.
			Write the message to stdout just in case.
		*/
		Melder_casual (message32);
		[alert runModal];
		[alert release];
	}
#endif

#define theMessageFund_SIZE  100'000
static char * theMessageFund = nullptr;

static void gui_fatal (conststring32 message) {
	free (theMessageFund);
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif motif
		MessageBox (nullptr, Melder_peek32toW (message), L"Fatal error", MB_OK | MB_TOPMOST | MB_ICONSTOP);
	#elif cocoa
		mac_message (NSCriticalAlertStyle, message);
		SysError (11);
	#endif
}

static void gui_error (conststring32 message) {
	bool memoryIsLow = str32str (message, U"Out of memory");
	if (memoryIsLow)
		free (theMessageFund);
	#if gtk
		trace (U"create dialog");
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_WARNING, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
		trace (U"run dialog");
		gtk_dialog_run (GTK_DIALOG (dialog));
		trace (U"destroy dialog");
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif motif
		MessageBox (nullptr, Melder_peek32toW (message), L"Message", MB_OK | MB_TOPMOST | MB_ICONWARNING);   // or (HWND) XtWindow ((GuiObject) Melder_topShell)
	#elif cocoa
		mac_message (NSWarningAlertStyle, message);
	#endif
	if (memoryIsLow) {
		theMessageFund = (char *) malloc (theMessageFund_SIZE);
		if (! theMessageFund) {
			#if gtk
				GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
					GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
				gtk_dialog_run (GTK_DIALOG (dialog));
				gtk_widget_destroy (GTK_WIDGET (dialog));
			#elif motif
				MessageBox (nullptr, L"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.", L"Message", MB_OK);
			#elif cocoa
				mac_message (NSCriticalAlertStyle, U"Praat is very low on memory.\nSave your work and quit Praat.\nIf you don't do that, Praat may crash.");
			#endif
		}
	}
}

static void gui_warning (conststring32 message) {
	#if gtk
		GuiObject dialog = gtk_message_dialog_new (GTK_WINDOW (Melder_topShell -> d_gtkWindow), GTK_DIALOG_DESTROY_WITH_PARENT,
			GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "%s", Melder_peek32to8 (message));
		gtk_dialog_run (GTK_DIALOG (dialog));
		gtk_widget_destroy (GTK_WIDGET (dialog));
	#elif motif
		MessageBox (nullptr, Melder_peek32toW (message), L"Warning", MB_OK | MB_TOPMOST | MB_ICONINFORMATION);
	#elif cocoa
		mac_message (NSInformationalAlertStyle, message);
	#endif
}

void Gui_injectMessageProcs (GuiWindow parent) {
	theMessageFund = (char *) malloc (theMessageFund_SIZE);
	assert (theMessageFund);
	Melder_topShell = parent;
	Melder_setFatalProc (gui_fatal);
	Melder_setErrorProc (gui_error);
	Melder_setWarningProc (gui_warning);
	Melder_setProgressProc (gui_progress);
	Melder_setMonitorProc (gui_monitor);
}

/* End of file Gui_messages.cpp */
