/* GuiText.cpp
 *
 * Copyright (C) 1993-2011,2012,2013,2014,2015 Paul Boersma, 2013 Tom Naughton
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
 * pb 2003/12/30 this file separated from motifEmulator.c
 * pb 2004/01/01 Mac: MLTE
 * pb 2004/03/11 Mac: tried to make compatible with MacOS X 10.1.x
 * pb 2005/09/01 Mac: GuiText_undo and GuiText_redo
 * pb 2006/10/29 Mac: erased MacOS 9 stuff
 * pb 2006/11/10 comments
 * pb 2007/05/30 GuiText_getStringW
 * pb 2007/05/31 Mac: CreateEditUnicodeTextControl
 * pb 2007/06/01 Mac: erased TextEdit stuff as well as changeCount
 * pb 2007/06/11 GuiText_getSelectionW, GuiText_replaceW
 * pb 2007/06/12 let command-key combinations pass through
 * pb 2007/12/15 erased ASCII versions
 * pb 2007/12/25 Gui
 * sdk 2007/12/27 first GTK version
 * pb 2008/10/05 better implicit selection (namely, none)
 * fb 2010/02/23 GTK
 * fb 2010/02/26 GTK & GuiText_set(Undo|Redo)Item() & history for GTK
 * fb 2010/03/02 history: merge same events together
 * pb 2010/03/11 support Unicode values above 0xFFFF
 * pb 2010/05/14 GTK changedCallback
 * pb 2010/05/30 GTK selections
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 * pb,tn 2013    Cocoa
 */

#include "GuiP.h"
#include <locale.h>

Thing_implement (GuiText, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac && useCarbon
	#define iam_text \
		Melder_assert (widget -> widgetClass == xmTextWidgetClass); \
		GuiText me = (GuiText) widget -> userData
#else
	#define iam_text \
		GuiText me = (GuiText) _GuiObject_getUserData (widget)
#endif

#if motif

#if mac
	#define isTextControl(w)  ((w) -> isControl != 0)
	#define isMLTE(w)  ((w) -> d_macMlteObject != nullptr)
#endif

#if win
	static HFONT font10, font12, font14, font18, font24;
#endif

/*
 * (1) KEYBOARD FOCUS
 *
 * (1.1) In Motif, the native GUI system handles all that we want:
 * every window with text widgets has one text focus widget,
 * which will receive global text focus when the window is activated.
 * The global text focus is visible to the user.
 * The focus changes whenever the user clicks in a text widget that does not have focus.
 *
 * (1.2) In Windows, the native GUI system handles almost all of the above.
 * The exception is that windows have no own text focus widgets;
 * there is only a single global text focus. This means that when the user
 * clicks on a non-active window, none of the text widgets in this window
 * will automatically receive text focus. Yet, the user expects automatic text focus behaviour
 * (click a window, then type immediately) in text edit windows (like Praat's script editor)
 * and in windows that have a single text widget (like Praat's TextGrid editor).
 * For this reason, the WM_COMMAND message handler in Gui.c intercepts the EN_SETFOCUS notification.
 * This handler calls _GuiText_handleFocusReception (), which records
 * the new text focus widget in its window. When a window is activated,
 * we set the global focus explicitly to this window's own text focus widget,
 * by calling _GuiText_setTheTextFocus ().
 *
 * (1.3) On Macintosh, we have to handle all of this explicitly.
 *
 * (1.4) On Win and Mac, we implement a feature not available in Motif:
 * the use of Command-X, Command-C, and Command-V to cut, copy, and paste in text widgets,
 * even in dialogs, where there are no menus for which these three commands could be keyboard shortcuts.
 * For this reason, _GuiText_handleFocusReception () also stores the global text focus,
 * so that the keyboard shortcut handler in Gui.c knows what widget to cut from, copy from, or paste into.
 * (It is true that Windows itself stores the global text focus, but this is not always a text widget;
 *  we want it to always be a text widget, e.g. in the TextGrid editor it is always the text widget,
 *  never the drawing area, that receives key strokes. In Motif, we will have to program this text
 *  preference explicitly; see the discussion in FunctionEditor.cpp.)
 */

void _GuiText_handleFocusReception (GuiObject widget) {
	/*
	 * On Windows, this is called:
	 * 1. on a user click in a text widget: WM_COMMAND -> EN_SETFOCUS;
	 * 2. on window activation: _GuiText_setTheTextFocus () -> SetFocus -> WM_COMMAND -> EN_SETFOCUS;
	 * 3. on a user click in a push button or toggle button, which would otherwise draw the
	 *    focus away from the text widgets: WM_COMMAND -> _GuiText_setTheTextFocus ().
	 *
	 * On Macintosh, this is called:
	 * 1. on a user click in a text widget: handleControlClick & handleTextEditClick -> _GuiText_setTheTextFocus ();
	 * 2. on window activation: handleActivateEvent -> _GuiText_setTheTextFocus ().
	 */
	widget -> shell -> textFocus = widget;   /* see (1.2) */
	theGui.textFocus = widget;   /* see (1.4) */
}

void _GuiText_handleFocusLoss (GuiObject widget) {
	/*
	 * me is going out of sight;
	 * it must stop having global focus.
	 */
	/*
	 * On Windows, this is called:
	 * 1. on window deactivation
	 * 2. on window closure
	 * 3. on text unmanaging
	 * 4. on window unmanaging
	 *
	 * On Macintosh, this is called:
	 * 1. on window deactivation
	 * 2. on window closure
	 * 3. on text unmanaging
	 * 4. on window unmanaging
	 */
	if (widget == theGui.textFocus)
		theGui.textFocus = nullptr;
}

#if mac
void _GuiMac_clearTheTextFocus () {
	if (theGui.textFocus) {
		GuiText textFocus = (GuiText) theGui.textFocus -> userData;
		_GuiMac_clipOnParent (theGui.textFocus);
		if (isTextControl (theGui.textFocus)) {
			ClearKeyboardFocus (theGui.textFocus -> macWindow);
		} else if (isMLTE (textFocus)) {
			TXNFocus (textFocus -> d_macMlteObject, 0);
			TXNActivate (textFocus -> d_macMlteObject, textFocus -> d_macMlteFrameId, 0);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusLoss (theGui.textFocus);
	}
}
#endif

void _GuiText_setTheTextFocus (GuiObject widget) {
	if (! widget || theGui.textFocus == widget
		|| ! widget -> managed) return;   // perhaps not-yet-managed; test: open Praat's DataEditor with a Sound, then type
	#if gtk
		gtk_widget_grab_focus (GTK_WIDGET (widget));   // not used: gtk is not 1 when motif is 1
	#elif win
		SetFocus (widget -> window);   // will send an EN_SETFOCUS notification, which will call _GuiText_handleFocusReception ()
	#elif mac
		iam_text;
		_GuiMac_clearTheTextFocus ();
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			SetKeyboardFocus (widget -> macWindow, widget -> nat.control.handle, kControlEditTextPart);
		} else if (isMLTE (me)) {
			TXNActivate (my d_macMlteObject, my d_macMlteFrameId, 1);
			TXNFocus (my d_macMlteObject, 1);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusReception (widget);
	#endif
}

/*
 * CHANGE NOTIFICATION
 */
void _GuiText_handleValueChanged (GuiObject widget) {
	iam_text;
	if (my d_changeCallback) {
		struct structGuiTextEvent event { me };
		my d_changeCallback (my d_changeBoss, & event);
	}
}

/*
 * EVENT HANDLING
 */

#if mac
	int _GuiMacText_tryToHandleReturnKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, EventRecord *event) {
		if (widget && widget -> activateCallback) {
			widget -> activateCallback (widget, widget -> activateClosure, (XtPointer) event);
				return 1;
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleClipboardShortcut (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, unsigned char charCode, EventRecord *event) {
		if (widget) {
			iam_text;
			if (isTextControl (widget)) {
				if (charCode == 'X' || charCode == 'C' || charCode == 'V') {
					if (! my d_editable && (charCode == 'X' || charCode == 'V')) return 0;
					CallNextEventHandler (eventHandlerCallRef, eventRef);
					_GuiText_handleValueChanged (widget);
					return 1;
				}
			} else if (isMLTE (me)) {
				if (charCode == 'X' && my d_editable) {
					if (event -> what != autoKey) GuiText_cut (me);
					return 1;
				}
				if (charCode == 'C') {
					if (event -> what != autoKey) GuiText_copy (me);
					return 1;
				}
				if (charCode == 'V' && my d_editable) {
					GuiText_paste (me);
					return 1;
				}
			}
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, GuiObject widget, unsigned char keyCode, unsigned char charCode, EventRecord *event) {
		(void) keyCode;
		if (widget) {
			iam_text;
			if (my d_editable) {
				_GuiMac_clipOnParent (widget);
				//Melder_casual (U"char code ", (int) charCode);
				if (isTextControl (widget)) {
					CallNextEventHandler (eventHandlerCallRef, eventRef);
				} else if (isMLTE (me)) {
					//static long key = 0; Melder_casual (U"key ", ++key);
					//TXNKeyDown (my macMlteObject, event);   // Tends never to be called.
					CallNextEventHandler (eventHandlerCallRef, eventRef);
				}
				GuiMac_clipOff ();
				if (charCode > 31 || charCode < 28) {   // arrows do not change the value of the text
					_GuiText_handleValueChanged (widget);
				}
				return 1;
			}
		}
		return 0;   // not handled
	}
	void _GuiMacText_handleClick (GuiObject widget, EventRecord *event) {
		iam_text;
		_GuiText_setTheTextFocus (widget);
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			HandleControlClick (widget -> nat.control.handle, event -> where, event -> modifiers, nullptr);
		} else if (isMLTE (me)) {
			LocalToGlobal (& event -> where);
			TXNClick (my d_macMlteObject, event);   // handles text selection and scrolling
			GlobalToLocal (& event -> where);
		}
		GuiMac_clipOff ();
	}
#endif

/*
 * LAYOUT
 */
#if mac
	void _GuiMacText_move (GuiObject widget) {
		iam_text;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			MoveControl (widget -> nat.control.handle, widget -> rect.left + 3, widget -> rect.top + 3);
			_GuiMac_clipOffValid (widget);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my d_macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my d_macMlteFrameId);
		}
	}
	void _GuiMacText_shellResize (GuiObject widget) {
		iam_text;
		/*
		 * Shell erasure, and therefore text erasure, has been handled by caller.
		 * Reshowing will be handled by caller.
		 */
		if (isTextControl (widget)) {
			MoveControl (widget -> nat.control.handle, widget -> rect.left + 3, widget -> rect.top + 3);
			SizeControl (widget -> nat.control.handle, widget -> width - 6, widget -> height - 6);
			/*
			 * Control reshowing will explicitly be handled by caller.
			 */
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my d_macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my d_macMlteFrameId);
		}
	}
	void _GuiMacText_resize (GuiObject widget) {
		iam_text;
		if (isTextControl (widget)) {
			SizeControl (widget -> nat.control.handle, widget -> width - 6, widget -> height - 6);
			/*
			 * Container widgets will have been invalidated.
			 * So in order not to make the control flash, we validate it.
			 */
			_Gui_validateWidget (widget);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my d_macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my d_macMlteFrameId);
		}
	}
#endif

void _GuiText_unmanage (GuiObject widget) {
	#if win
		_GuiText_handleFocusLoss (widget);
		_GuiNativeControl_hide (widget);
	#elif mac
		iam_text;
		/*
		 * Just _GuiText_handleFocusLoss () is not enough,
		 * because that can leave a visible blinking cursor.
		 */
		if (isTextControl (widget)) {
			if (widget == theGui.textFocus) _GuiMac_clearTheTextFocus ();   /* Remove visible blinking cursor. */
			_GuiNativeControl_hide (widget);
		} else if (isMLTE (me)) {
		}
	#endif
	/*
	 * The caller will set the unmanage flag to zero, and remanage the parent.
	 */
}

/*
 * VISIBILITY
 */

#if win
	void _GuiWinText_destroy (GuiObject widget) {
		if (widget == theGui.textFocus)
			theGui.textFocus = nullptr;   // remove dangling reference
		if (widget == widget -> shell -> textFocus)
			widget -> shell -> textFocus = nullptr;   // remove dangling reference
		iam_text;
		DestroyWindow (widget -> window);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinText_map (GuiObject widget) {
		iam_text;
		ShowWindow (widget -> window, SW_SHOW);
	}
#elif mac
	void _GuiMacText_destroy (GuiObject widget) {
		if (widget == theGui.textFocus)
			theGui.textFocus = nullptr;   // remove dangling reference
		if (widget == widget -> shell -> textFocus)
			widget -> shell -> textFocus = nullptr;   // remove dangling reference
		iam_text;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			DisposeControl (widget -> nat.control.handle);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNDeleteObject (my d_macMlteObject);
		}
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacText_update (GuiObject widget) {
		iam_text;
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			Draw1Control (widget -> nat.control.handle);
		} else if (isMLTE (me)) {
			TXNDraw (my d_macMlteObject, nullptr);
		}
		GuiMac_clipOff ();
	}
	void _GuiMacText_map (GuiObject widget) {
		iam_text;
		if (isTextControl (widget)) {
			_GuiNativeControl_show (widget);
		} else if (isMLTE (me)) {
		}
	}
#endif


#endif


#if motif

static long NativeText_getLength (GuiObject widget) {
	#if win
		return Edit_GetTextLength (widget -> window);   // in UTF-16 code units
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			Size size;
			CFStringRef cfString;
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, nullptr);
			size = CFStringGetLength (cfString);
			CFRelease (cfString);
			return size;
		} else if (isMLTE (me)) {
			/*
			 * From the reference page of TXNDataSize:
			 * "If you are using Unicode and you want to know the number of characters,
			 * divide the returned ByteCount value by sizeof(UniChar) or 2,
			 * since MLTE uses the 16-bit Unicode Transformation Format (UTF-16)."
			 */
			return TXNDataSize (my d_macMlteObject) / sizeof (UniChar);
		}
		return 0;   // should not occur
	#endif
}

#if mac
static void NativeText_getText (GuiObject widget, char32 *buffer, long length) {
	iam_text;
	if (isTextControl (widget)) {
		CFStringRef cfString;
		GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, nullptr);
		UniChar *macText = Melder_malloc_f (UniChar, length + 1);
		CFRange range = { 0, length };
		CFStringGetCharacters (cfString, range, macText);
		CFRelease (cfString);
		long j = 0;
		for (long i = 0; i < length; i ++) {
			char32 kar = macText [i];
			if (kar < 0x00D800 || kar > 0x00DFFF) {
				buffer [j ++] = kar;
			} else {
				Melder_assert (kar >= 0x00D800 && kar <= 0x00DBFF);
				char32 kar1 = macText [++ i];
				Melder_assert (kar1 >= 0x00DC00 && kar1 <= 0x00DFFF);
				buffer [j ++] = 0x010000 + ((kar & 0x0003FF) << 10) + (kar1 & 0x0003FF);
			}
		}
		buffer [j] = U'\0';
		Melder_free (macText);
	} else if (isMLTE (me)) {
		Handle han;
		TXNGetDataEncoded (my d_macMlteObject, 0, length, & han, kTXNUnicodeTextData);
		long j = 0;
		for (long i = 0; i < length; i ++) {
			char32 kar = ((UniChar *) *han) [i];
			if (kar < 0x00D800 || kar > 0x00DFFF) {
				buffer [j ++] = kar;
			} else {
				Melder_assert (kar >= 0x00D800 && kar <= 0x00DBFF);
				char32 kar1 = ((UniChar *) *han) [++ i];
				Melder_assert (kar1 >= 0x00DC00 && kar1 <= 0x00DFFF);
				buffer [j ++] = 0x010000 + ((kar & 0x0003FF) << 10) + (kar1 & 0x0003FF);
			}
		}
		buffer [j] = U'\0';
		DisposeHandle (han);
	}
	buffer [length] = L'\0';   // superfluous?
}
#endif

/*
 * SELECTION
 */

static int NativeText_getSelectionRange (GuiObject widget, long *out_left, long *out_right) {
	unsigned long left, right;
	#ifndef unix
	Melder_assert (MEMBER (widget, Text));
	#endif
	#if win
		SendMessage (widget -> window, EM_GETSEL, (WPARAM) & left, (LPARAM) & right);   // 32-bit (R&N: 579)
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			ControlEditTextSelectionRec rec;
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec, nullptr);
			left = rec.selStart;
			right = rec. selEnd;
		} else if (isMLTE (me)) {
			TXNGetSelection (my d_macMlteObject, & left, & right);
		}
	#endif
	if (out_left) *out_left = left;
	if (out_right) *out_right = right;
	return right > left;
}

/*
 * PACKAGE
 */

void _GuiText_init () {
	#if mac
		//short font;
		TXNMacOSPreferredFontDescription defaults = { 0 };
		//GetFNum ("\006Monaco", & font);
		//defaults. fontID = font;
		defaults. pointSize = 0x000B0000;
		defaults. fontStyle = kTXNDefaultFontStyle;
		defaults. encoding  = /*kTXNMacOSEncoding*/ kTXNSystemDefaultEncoding;
		TXNInitTextension (& defaults, 1, 0);
	#endif
}

void _GuiText_exit () {
	#if mac
		TXNTerminateTextension (); 
	#endif
}

#endif

#if !mac
	/*
	 * Undo/Redo history functions
	 */

	static void _GuiText_delete (GuiObject widget, int from_pos, int to_pos) {
		#if gtk
			if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
				gtk_editable_delete_text (GTK_EDITABLE (widget), from_pos, to_pos);
			} else {
				GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
				GtkTextIter from_it, to_it;
				gtk_text_buffer_get_iter_at_offset (buffer, &from_it, from_pos);
				gtk_text_buffer_get_iter_at_offset (buffer, &to_it, to_pos);
				gtk_text_buffer_delete_interactive (buffer, &from_it, &to_it,
					gtk_text_view_get_editable (GTK_TEXT_VIEW (widget)));
				gtk_text_buffer_place_cursor (buffer, &to_it);
			}
		#elif win
		#endif
	}

	static void _GuiText_insert (GuiObject widget, int from_pos, int to_pos, const history_data text) {
		#if gtk
			if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
				gint from_pos_gint = from_pos;
				gtk_editable_insert_text (GTK_EDITABLE (widget), text, to_pos - from_pos, &from_pos_gint);
			} else {
				GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
				GtkTextIter it;
				gtk_text_buffer_get_iter_at_offset (buffer, &it, from_pos);
				gtk_text_buffer_insert_interactive (buffer, &it, text, to_pos - from_pos,
					gtk_text_view_get_editable (GTK_TEXT_VIEW(widget)));
				gtk_text_buffer_get_iter_at_offset (buffer, &it, to_pos);
				gtk_text_buffer_place_cursor (buffer, &it);
			}
		#elif win
		#endif
	}

	/* Tests the previous elements of the history for mergability with the one to insert given via parameters.
	 * If successful, it returns a pointer to the last valid entry in the merged history list, otherwise
	 * returns nullptr.
	 * Specifically the function expands the previous insert/delete event(s)
	 *  - with the current, if current also is an insert/delete event and the ranges of previous and current event match
	 *  - with the previous delete and current insert event, in case the ranges of both event-pairs respectively match
	 */
	static history_entry * history_addAndMerge (void *void_me, history_data text_new, long first, long last, bool deleted) {
		iam(GuiText);
		history_entry *he = nullptr;
		
		if (! my d_prev)
			return nullptr;
		
		if (my d_prev->type_del == deleted) {
			// extend the last history event by this one
			if (my d_prev->first == last) {
				// most common for backspace key presses
				he = my d_prev;
				text_new = (char *) realloc (text_new, sizeof(*text_new) * (he->last - first + 1));
				memcpy (text_new + last - first, he->text, sizeof(*text_new) * (he->last - he->first + 1));
				free (he->text);
				he->text = text_new;
				he->first = first;
				
			} else if (my d_prev->last == first) {
				// most common for ordinary text insertion
				he = my d_prev;
				he->text = (char *) realloc (he->text, sizeof(*he->text) * (last - he->first + 1));
				memcpy (he->text + he->last - he->first, text_new, sizeof(*he->text) * (last - first + 1));
				free (text_new);
				he->last = last;
				
			} else if (deleted && my d_prev->first == first) {
				// most common for delete key presses
				he = my d_prev;
				he->text = (char *) realloc (he->text, sizeof(*he->text) * (last - first + he->last - he->first + 1));
				memcpy (he->text + he->last - he->first, text_new, sizeof(*he->text) * (last - first + 1));
				free (text_new);
				he->last = last + he->last - he->first;
			}
		} else {
			// prev->type_del != deleted, no simple expansion possible, check for double expansion
			if (!deleted && my d_prev->prev && my d_prev->prev->prev) {
				history_entry *del_one = my d_prev;
				history_entry *ins_mult = del_one->prev;
				history_entry *del_mult = ins_mult->prev;
				long from1 = del_mult->first, to1 = del_mult->last;
				long from2 = ins_mult->first, to2 = ins_mult->last;
				long from3 = del_one->first, to3 = del_one->last;
				if (from3 == first && to3 == last && from2 == from1 && to2 == to1 && to1 == first &&
						!ins_mult->type_del && del_mult->type_del) {
					// most common for overwriting text
					/* So the layout is as follows:
					 *
					 *        del_mult                  ins_mult               del_one        current (parameters)
					 * [del, from1, to1, "uvw"] [ins, from1, to1, "abc"] [del, to1, to3, "x"] [ins, to1, to3, "d"]
					 *     n >= 1 characters          n characters           1 character          1 character
					 *
					 * So merge those four events into two events by expanding del_mult by del_one and ins_mult by current */
					del_mult->text = (char *) realloc (del_mult->text, sizeof (*del_mult->text) * (to3 - from1 + 1));
					ins_mult->text = (char *) realloc (ins_mult->text, sizeof (*ins_mult->text) * (to3 - from1 + 1));
					memcpy (del_mult->text + to1 - from1, del_one->text, sizeof (*del_mult->text) * (to3 - to1 + 1));
					memcpy (ins_mult->text + to1 - from1, text_new     , sizeof (*del_mult->text) * (to3 - to1 + 1));
					del_mult->last = to3;
					ins_mult->last = to3;
					free (del_one->text);
					free (del_one);
					free (text_new);
					my d_prev = he = ins_mult;
				}
			}
		}
		
		return he;
	}

	/* Inserts a new history action, thereby removing any remaining 'redo' steps;
	 *   text_new  a newly allocated string that will be freed by a history function
	 *             (history_add or history_clear)
	 */
	static void history_add (void *void_me, history_data text_new, long first, long last, bool deleted) {
		iam (GuiText);
		
		// delete all newer entries; from here on there is no 'Redo' until the next 'Undo' is performed
		history_entry *old_hnext = my d_next, *hnext;
		while (old_hnext) {
			hnext = old_hnext->next;
			free (old_hnext->text);
			free (old_hnext);
			old_hnext = hnext;
		}
		my d_next = nullptr;
		
		history_entry *he = history_addAndMerge (void_me, text_new, first, last, deleted);
		if (! he) {
			he = (history_entry *) malloc (sizeof (history_entry));
			he->first = first;
			he->last = last;
			he->type_del = deleted;
			he->text = text_new;
			
			he->prev = my d_prev;
			he->next = nullptr;
			if (my d_prev)
				my d_prev->next = he;
		}
		my d_prev = he;
		he->next = nullptr;
		
		if (my d_undo_item) GuiThing_setSensitive (my d_undo_item, true);
		if (my d_redo_item) GuiThing_setSensitive (my d_redo_item, false);
	}

	static bool history_has_undo (void *void_me) {
		iam (GuiText);
		return my d_prev != nullptr;
	}

	static bool history_has_redo (void *void_me) {
		iam (GuiText);
		return my d_next != nullptr;
	}

	static void history_do (void *void_me, bool undo) {
		iam (GuiText);
		history_entry *he = undo ? my d_prev : my d_next;
		if (! he) // TODO: this function should not be called in that case
			return;
		
		my d_history_change = 1;
		if (undo ^ he->type_del) {
			_GuiText_delete (my d_widget, he->first, he->last);
		} else {
			_GuiText_insert (my d_widget, he->first, he->last, he->text);
		}
		my d_history_change = 0;
		
		if (undo) {
			my d_next = my d_prev;
			my d_prev = my d_prev->prev;
		} else {
			my d_prev = my d_next;
			my d_next = my d_next->next;
		}
		
		if (my d_undo_item) GuiThing_setSensitive (my d_undo_item, history_has_undo (me));
		if (my d_redo_item) GuiThing_setSensitive (my d_redo_item, history_has_redo (me));
	}

	static void history_clear(void *void_me) {
		iam(GuiText);
		history_entry *h1, *h2;
		
		h1 = my d_prev;
		while (h1) {
			h2 = h1->prev;
			free(h1->text);
			free(h1);
			h1 = h2;
		}
		my d_prev = nullptr;
		
		h1 = my d_next;
		while (h1) {
			h2 = h1->next;
			free(h1->text);
			free(h1);
			h1 = h2;
		}
		my d_next = nullptr;
		
		if (my d_undo_item) GuiThing_setSensitive (my d_undo_item, false);
		if (my d_redo_item) GuiThing_setSensitive (my d_redo_item, false);
	}
#endif

/*
 * CALLBACKS
 */

#if gtk
	static void _GuiGtkEntry_history_delete_cb (GtkEditable *ed, gint from, gint to, gpointer void_me) {
		iam (GuiText);
		trace (U"begin");
		if (my d_history_change) return;
		history_add (me, gtk_editable_get_chars (GTK_EDITABLE (ed), from, to), from, to, 1);
	}
	
	static void _GuiGtkEntry_history_insert_cb (GtkEditable *ed, gchar *utf8_text, gint len, gint *from, gpointer void_me) {
		(void) ed;
		iam (GuiText);
		trace (U"begin");
		if (my d_history_change) return;
		gchar *text = (gchar *) malloc (sizeof (gchar) * (len + 1));
		strcpy (text, utf8_text);
		history_add (me, text, *from, *from + len, 0);
	}
	
	static void _GuiGtkTextBuf_history_delete_cb (GtkTextBuffer *buffer, GtkTextIter *from, GtkTextIter *to, gpointer void_me) {
		iam (GuiText);
		trace (U"begin");
		if (my d_history_change) return;
		int from_pos = gtk_text_iter_get_offset (from);
		int to_pos = gtk_text_iter_get_offset (to);
		history_add (me, gtk_text_buffer_get_text (buffer, from, to, false), from_pos, to_pos, 1);
	}
	
	static void _GuiGtkTextBuf_history_insert_cb (GtkTextBuffer *buffer, GtkTextIter *from, gchar *utf8_text, gint len, gpointer void_me) {
		(void) buffer;
		iam (GuiText);
		trace (U"begin");
		if (my d_history_change) return;
		int from_pos = gtk_text_iter_get_offset (from);
		gchar *text = (gchar *) malloc (sizeof (gchar) * (len + 1));
		strcpy (text, utf8_text);
		history_add (me, text, from_pos, from_pos + len, 0);
	}
	
	static void _GuiGtkText_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiText);
		trace (U"begin");
		Melder_assert (me);
		if (my d_changeCallback) {
			struct structGuiTextEvent event { me };
			my d_changeCallback (my d_changeBoss, & event);
		}
	}
	
	static void _GuiGtkText_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiText);
		Melder_assert (me);
		Melder_assert (my classInfo == classGuiText);
		trace (U"begin");
		if (my d_undo_item) {
			trace (U"undo");
			//g_object_unref (my d_undo_item -> d_widget);
		}
		if (my d_redo_item) {
			trace (U"redo");
			//g_object_unref (my d_redo_item -> d_widget);
		}
		my d_undo_item = nullptr;
		my d_redo_item = nullptr;
		trace (U"history");
		history_clear (me);
		forget (me);
	}
#elif cocoa
	@implementation GuiCocoaTextField {
		GuiText d_userData;
	}
	- (void) dealloc {   // override
		GuiText me = d_userData;
		forget (me);
		trace (U"deleting a text field");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiText));
		d_userData = static_cast <GuiText> (userData);
	}
	- (void) textDidChange: (NSNotification *) notification {
		(void) notification;
		GuiText me = d_userData;
		if (me && my d_changeCallback) {
			struct structGuiTextEvent event { me };
			my d_changeCallback (my d_changeBoss, & event);
		}
	}
	@end
	@implementation GuiCocoaTextView {
		GuiText d_userData;
	}
	- (void) dealloc {   // override
		GuiText me = d_userData;
		forget (me);
		trace (U"deleting a text view");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiText));
		d_userData = static_cast <GuiText> (userData);
	}
	/*
	 * The NSTextViewDelegate protocol.
	 * While NSTextDelegate simply has textDidChange:, that method doesn't seem to respond when the text is changed programmatically.
	 */
//	- (void) textDidChange: (NSNotification *) notification {
	- (BOOL) textView: (NSTextView *) aTextView   shouldChangeTextInRange: (NSRange) affectedCharRange   replacementString: (NSString *) replacementString {
		(void) aTextView;
		(void) affectedCharRange;
		(void) replacementString;
		trace (U"changing text to: ", Melder_peek8to32 ([replacementString UTF8String]));
		GuiText me = d_userData;
		if (me && my d_changeCallback) {
			struct structGuiTextEvent event { me };
			my d_changeCallback (my d_changeBoss, & event);
		}
		return YES;
	}
	@end
#elif win
#elif mac
#endif

GuiText GuiText_create (GuiForm parent, int left, int right, int top, int bottom, uint32 flags) {
	GuiText me = Thing_new (GuiText);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		trace (U"before creating a GTK text widget: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		if (flags & GuiText_SCROLLED) {
			GtkWrapMode ww;
			GuiObject scrolled = gtk_scrolled_window_new (nullptr, nullptr);
			gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
			my d_widget = gtk_text_view_new ();
			gtk_container_add (GTK_CONTAINER (scrolled), GTK_WIDGET (my d_widget));
			gtk_widget_show (GTK_WIDGET (scrolled));
			gtk_text_view_set_editable (GTK_TEXT_VIEW (my d_widget), (flags & GuiText_NONEDITABLE) == 0);
			if ((flags & GuiText_WORDWRAP) != 0) 
				ww = GTK_WRAP_WORD_CHAR;
			else
				ww = GTK_WRAP_NONE;
			gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (my d_widget), ww);
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			g_signal_connect (G_OBJECT (buffer), "delete-range", G_CALLBACK (_GuiGtkTextBuf_history_delete_cb), me);
			g_signal_connect (G_OBJECT (buffer), "insert-text", G_CALLBACK (_GuiGtkTextBuf_history_insert_cb), me);
			g_signal_connect (G_OBJECT (buffer), "changed", G_CALLBACK (_GuiGtkText_valueChangedCallback), me);
			_GuiObject_setUserData (my d_widget, me);
			_GuiObject_setUserData (scrolled, me);
			my v_positionInForm (scrolled, left, right, top, bottom, parent);
		} else {
			my d_widget = gtk_entry_new ();
			gtk_editable_set_editable (GTK_EDITABLE (my d_widget), (flags & GuiText_NONEDITABLE) == 0);
			g_signal_connect (G_OBJECT (my d_widget), "delete-text", G_CALLBACK (_GuiGtkEntry_history_delete_cb), me);
			g_signal_connect (G_OBJECT (my d_widget), "insert-text", G_CALLBACK (_GuiGtkEntry_history_insert_cb), me);
			g_signal_connect (GTK_EDITABLE (my d_widget), "changed", G_CALLBACK (_GuiGtkText_valueChangedCallback), me);
			//GTK_WIDGET_UNSET_FLAGS (my d_widget, GTK_CAN_DEFAULT);
			_GuiObject_setUserData (my d_widget, me);
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
			gtk_entry_set_activates_default (GTK_ENTRY (my d_widget), true);
		}
		trace (U"after creating a GTK text widget: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		my d_prev = nullptr;
		my d_next = nullptr;
		my d_history_change = 0;
		my d_undo_item = nullptr;
		my d_redo_item = nullptr;
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkText_destroyCallback), me);
	#elif cocoa
		if (flags & GuiText_SCROLLED) {
			my d_cocoaScrollView = [[GuiCocoaScrolledWindow alloc] init];
			[my d_cocoaScrollView setUserData: nullptr];   // because those user data can only be GuiScrolledWindow
			my d_widget = my d_cocoaScrollView;
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
			[my d_cocoaScrollView setBorderType: NSNoBorder];
			[my d_cocoaScrollView setHasHorizontalScroller: YES];
			[my d_cocoaScrollView setHasVerticalScroller:   YES];
			[my d_cocoaScrollView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			NSSize contentSize = [my d_cocoaScrollView contentSize];
			my d_cocoaTextView = [[GuiCocoaTextView alloc] initWithFrame: NSMakeRect (0, 0, contentSize. width, contentSize. height)];
			[my d_cocoaTextView setUserData: me];
			if (Melder_systemVersion < 101100) {
				[my d_cocoaTextView setMinSize: NSMakeSize (0.0, contentSize.height)];
			} else {
				[my d_cocoaTextView setMinSize: NSMakeSize (contentSize. width, contentSize.height)];    // El Capitan Developer Beta 2
			}
			[my d_cocoaTextView setMaxSize: NSMakeSize (FLT_MAX, FLT_MAX)];
			[my d_cocoaTextView setVerticallyResizable: YES];
			[my d_cocoaTextView setHorizontallyResizable: YES];
			[my d_cocoaTextView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			[[my d_cocoaTextView textContainer] setContainerSize: NSMakeSize (FLT_MAX, FLT_MAX)];
			[[my d_cocoaTextView textContainer] setWidthTracksTextView: NO];
			[my d_cocoaScrollView setDocumentView: my d_cocoaTextView];   // the scroll view will own the text view?
			[my d_cocoaTextView release];   // so we release the text view itself
			[[my d_cocoaScrollView window] makeFirstResponder: my d_cocoaTextView];
			static NSFont *theTextFont;
			if (! theTextFont) {
				theTextFont = [[NSFont systemFontOfSize: 13.0] retain];
				theTextFont = [[NSFont fontWithName: @"Menlo"   size: 12.0] retain];
			}
			[my d_cocoaTextView setFont: theTextFont];
			[my d_cocoaTextView setAllowsUndo: YES];
			[my d_cocoaTextView turnOffLigatures: nil];
			[my d_cocoaTextView setSmartInsertDeleteEnabled: NO];
			[my d_cocoaTextView setAutomaticQuoteSubstitutionEnabled: NO];
			[my d_cocoaTextView setAutomaticTextReplacementEnabled: NO];
			[my d_cocoaTextView setAutomaticDashSubstitutionEnabled: NO];
			[my d_cocoaTextView setDelegate: my d_cocoaTextView];
		} else {
			my d_widget = [[GuiCocoaTextField alloc] init];
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
			[(GuiCocoaTextField *) my d_widget   setUserData: me];
			[(NSTextField *) my d_widget   setEditable: YES];
			static NSFont *theTextFont;
			if (! theTextFont) {
				theTextFont = [[NSFont systemFontOfSize: 13.0] retain];
			}
			[(NSTextField *) my d_widget   setFont: theTextFont];
		}
	#elif win
		my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, flags & GuiText_SCROLLED ? U"scrolledText" : U"text");
		_GuiObject_setUserData (my d_widget, me);
		my d_editable = (flags & GuiText_NONEDITABLE) == 0;
		my d_widget -> window = CreateWindow (L"edit", nullptr, WS_CHILD | WS_BORDER
			| ( flags & GuiText_WORDWRAP ? ES_AUTOVSCROLL : ES_AUTOHSCROLL )
			| ES_MULTILINE | WS_CLIPSIBLINGS
			| ( flags & GuiText_SCROLLED ? WS_HSCROLL | WS_VSCROLL : 0 ),
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		if (! font10) {
			font10 = CreateFont (13, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font12 = CreateFont (16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font14 = CreateFont (19, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font18 = CreateFont (24, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font24 = CreateFont (32, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
		}
		SetWindowFont (my d_widget -> window, font12 /*theScrolledHint ? font : GetStockFont (ANSI_VAR_FONT)*/, false);
		Edit_LimitText (my d_widget -> window, 0);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my d_widget -> shell -> textFocus) {
			my d_widget -> shell -> textFocus = my d_widget;   // even if not-yet-managed. But in that case it will not receive global focus
		}
	#elif mac
		if (flags & GuiText_SCROLLED) {
			my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, U"scrolledText");
			_GuiObject_setUserData (my d_widget, me);
			my d_editable = (flags & GuiText_NONEDITABLE) == 0;
			TXNLongRect destRect;
			TXNMargins margins;
			TXNControlData controlData;
			TXNControlTag controlTag = kTXNMarginsTag;
			TXNNewObject (nullptr,   // no file
				my d_widget -> macWindow, & my d_widget -> rect, kTXNWantHScrollBarMask | kTXNWantVScrollBarMask
					| kTXNMonostyledTextMask | kTXNDrawGrowIconMask,
				kTXNTextEditStyleFrameType, kTXNTextensionFile,
				/*kTXNMacOSEncoding*/ kTXNSystemDefaultEncoding, & my d_macMlteObject, & my d_macMlteFrameId, me);
			destRect. left = 0;
			destRect. top = 0;
			destRect. right = 10000;
			destRect. bottom = 2000000000;
			TXNSetRectBounds (my d_macMlteObject, nullptr, & destRect, false);
			margins. leftMargin = 3;
			margins. topMargin = 3;
			margins. rightMargin = 0;
			margins. bottomMargin = 0;
			controlData. marginsPtr = & margins;
			TXNSetTXNObjectControls (my d_macMlteObject, false, 1, & controlTag, & controlData);
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		} else {
			my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, U"text");
			_GuiObject_setUserData (my d_widget, me);
			my d_editable = (flags & GuiText_NONEDITABLE) == 0;
			Rect r = my d_widget -> rect;
			InsetRect (& r, 3, 3);
			CreateEditUnicodeTextControl (my d_widget -> macWindow, & r, nullptr, false, nullptr, & my d_widget -> nat.control.handle);
			SetControlReference (my d_widget -> nat.control.handle, (long) my d_widget);
			my d_widget -> isControl = true;
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		}
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my d_widget -> shell -> textFocus) {
			my d_widget -> shell -> textFocus = my d_widget;   // even if not-yet-managed; but in that case it will not receive global focus
		}
	#endif
	
	return me;
}

GuiText GuiText_createShown (GuiForm parent, int left, int right, int top, int bottom, uint32 flags) {
	GuiText me = GuiText_create (parent, left, right, top, bottom, flags);
	GuiThing_show (me);
	return me;
}

void GuiText_copy (GuiText me) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_copy_clipboard (GTK_EDITABLE (my d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_copy_clipboard (buffer, cb);
		}
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   copy: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] copy: nil];
		}
	#elif win
		if (! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_COPY, 0, 0);
	#elif mac
		if (! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		if (isTextControl (my d_widget)) {
			HandleControlKey (my d_widget -> nat.control.handle, 0, 'C', cmdKey);
		} else if (isMLTE (me)) {
			TXNCopy (my d_macMlteObject);
		}
	#endif
}

void GuiText_cut (GuiText me) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_cut_clipboard (GTK_EDITABLE (my d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_cut_clipboard (buffer, cb, gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   cut: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] cut: nil];
		}
	#elif win
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_CUT, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif mac
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		if (isTextControl (my d_widget)) {
			_GuiMac_clipOnParent (my d_widget);
			HandleControlKey (my d_widget -> nat.control.handle, 0, 'X', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNCut (my d_macMlteObject);
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

char32 * GuiText_getSelection (GuiText me) {
	#if gtk
		// first = gtk_text_iter_get_offset (& start);
		// last = gtk_text_iter_get_offset (& end);
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gint start, end;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (my d_widget), & start, & end);
			if (end > start) {   // at least one character selected?
				gchar *text = gtk_editable_get_chars (GTK_EDITABLE (my d_widget), start, end);
				char32 *result = Melder_8to32 (text);
				g_free (text);
				return result;
			}
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			if (gtk_text_buffer_get_has_selection (textBuffer)) {   // at least one character selected?
				GtkTextIter start, end;
				gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
				gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, true);
				char32 *result = Melder_8to32 (text);
				g_free (text);
				return result;
			}
		}
	#elif cocoa
		long start, end;
		autostring32 selection = GuiText_getStringAndSelectionPosition (me, & start, & end);
		long length = end - start;
		if (length > 0) {
			char32 *result = Melder_malloc_f (char32, length + 1);
			memcpy (result, & selection [start], length * sizeof (char32));
			result [length] = '\0';
			Melder_killReturns_inline (result);
			return result;
		}
	#elif win
		long startW, endW;
		NativeText_getSelectionRange (my d_widget, & startW, & endW);
		if (endW > startW) {   // at least one character selected?
			/*
			 * Get all text.
			 */
			long lengthW = NativeText_getLength (my d_widget);   // in UTF-16 code units
			WCHAR *bufferW = Melder_malloc_f (WCHAR, lengthW + 1);
			GetWindowTextW (my d_widget -> window, bufferW, lengthW + 1);
			/*
			 * Zoom in on selection.
			 */
			lengthW = endW - startW;
			memmove (bufferW, bufferW + startW, lengthW * sizeof (WCHAR));   // not because of realloc, but because of free!
			bufferW [lengthW] = U'\0';
			char32 *result = Melder_dup_f (Melder_peekWto32 (bufferW));
			Melder_killReturns_inline (result);   // AFTER zooming!
			return result;
		}
	#elif mac
		long start, end;
		NativeText_getSelectionRange (my d_widget, & start, & end);
		if (end > start) {   // at least one character selected?
			/*
			 * Get all text.
			 */
			long length = NativeText_getLength (my d_widget);
			char32 *result = Melder_malloc_f (char32, length + 1);
			NativeText_getText (my d_widget, result, length);
			/*
			 * Zoom in on selection.
			 */
			#if mac
				for (long i = 0; i < start; i ++) if (result [i] > 0xFFFF) { start --; end --; }
				for (long i = start; i < end; i ++) if (result [i] > 0xFFFF) { end --; }
			#endif
			length = end - start;
			memmove (result, result + start, length * sizeof (char32));   // not because of realloc, but because of free!
			result [length] = '\0';
			result = (char32 *) Melder_realloc_f (result, (length + 1) * sizeof (char32));   // optional
			Melder_killReturns_inline (result);   // AFTER zooming!
			return result;
		}
	#endif
	return nullptr;   // zero characters selected
}

char32 * GuiText_getString (GuiText me) {
	long first, last;
	return GuiText_getStringAndSelectionPosition (me, & first, & last);
}

char32 * GuiText_getStringAndSelectionPosition (GuiText me, long *first, long *last) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gint first_gint, last_gint;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (my d_widget), & first_gint, & last_gint);   // expressed in Unicode code points!
			*first = first_gint;
			*last = last_gint;
			return Melder_8to32 (gtk_entry_get_text (GTK_ENTRY (my d_widget)));
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, true);   // TODO: Hidden chars ook maar doen he?
			char32 *result = Melder_8to32 (text);
			g_free (text);
			gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
			*first = gtk_text_iter_get_offset (& start);
			*last = gtk_text_iter_get_offset (& end);
			return result;
		}
		return nullptr;
	#elif cocoa
		if (my d_cocoaTextView) {
			NSString *nsString = [my d_cocoaTextView   string];
			char32 *result = Melder_8to32 ([nsString UTF8String]);
			trace (U"string ", result);
			NSRange nsRange = [my d_cocoaTextView   selectedRange];
			*first = nsRange. location;
			*last = *first + nsRange. length;
			for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
			for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
			return result;
		} else {
			NSString *nsString = [(NSTextField *) my d_widget   stringValue];
			char32 *result = Melder_8to32 ([nsString UTF8String]);
			trace (U"string ", result);
			NSRange nsRange = [[[(NSTextField *) my d_widget   window] fieldEditor: NO forObject: nil] selectedRange];
			*first = nsRange. location;
			*last = *first + nsRange. length;
			for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
			for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
			return result;
		}
	#elif win
		long lengthW = NativeText_getLength (my d_widget);
		WCHAR *bufferW = Melder_malloc_f (WCHAR, lengthW + 1);
		GetWindowTextW (my d_widget -> window, bufferW, lengthW + 1);
		long firstW, lastW;
		NativeText_getSelectionRange (my d_widget, & firstW, & lastW);

		long differenceFirst = 0;
		for (long i = 0; i < firstW; i ++) {
			if (bufferW [i] == 13 && (bufferW [i + 1] == L'\n' || bufferW [i + 1] == 0x0085)) differenceFirst ++;
			if (bufferW [i] >= 0xDC00 && bufferW [i] <= 0xDFFF) differenceFirst ++;
		}
		*first = firstW - differenceFirst;

		long differenceLast = differenceFirst;
		for (long i = firstW; i < lastW; i ++) {
			if (bufferW [i] == 13 && (bufferW [i + 1] == L'\n' || bufferW [i + 1] == 0x0085)) differenceLast ++;
			if (bufferW [i] >= 0xDC00 && bufferW [i] <= 0xDFFF) differenceLast ++;
		}
		*last = lastW - differenceLast;

		char32 *result = Melder_dup_f (Melder_peekWto32 (bufferW));
		Melder_free (bufferW);
		Melder_killReturns_inline (result);
		return result;
	#elif mac
		long length = NativeText_getLength (my d_widget);   // UTF-16 length; should be enough for UTF-32 buffer
		char32 *result = Melder_malloc_f (char32, length + 1);
		NativeText_getText (my d_widget, result, length);
		NativeText_getSelectionRange (my d_widget, first, last);   // 'first' and 'last' are expressed in UTF-16 words
		for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
		for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
		Melder_killReturns_inline (result);
		return result;
	#endif
}

void GuiText_paste (GuiText me) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_paste_clipboard (GTK_EDITABLE (my d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_paste_clipboard (buffer, cb, nullptr, gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   pasteAsPlainText: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] pasteAsPlainText: nil];
		}
	#elif win
		if (! my d_editable) return;
		SendMessage (my d_widget -> window, WM_PASTE, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif mac
		if (! my d_editable) return;
		if (isTextControl (my d_widget)) {
			_GuiMac_clipOnParent (my d_widget);
			HandleControlKey (my d_widget -> nat.control.handle, 0, 'V', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNPaste (my d_macMlteObject);
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

void GuiText_redo (GuiText me) {
	#if cocoa
		if (my d_cocoaTextView) {
			[[my d_cocoaTextView   undoManager] redo];
		}
	#elif mac
		if (isMLTE (me)) {
			TXNRedo (my d_macMlteObject);
		}
		_GuiText_handleValueChanged (my d_widget);
	#else
		history_do (me, 0);
	#endif
}

void GuiText_remove (GuiText me) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_selection (GTK_EDITABLE (my d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			gtk_text_buffer_delete_selection (buffer, true, gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   delete: nil];
		}
	#elif win
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_CLEAR, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif mac
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		if (isTextControl (my d_widget)) {
			_GuiMac_clipOnParent (my d_widget);
			HandleControlKey (my d_widget -> nat.control.handle, 0, 8, 0);   // backspace key
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNClear (my d_macMlteObject);
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

void GuiText_replace (GuiText me, long from_pos, long to_pos, const char32 *text) {
	#if gtk
		gchar *newText = Melder_peek32to8 (text);
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_text (GTK_EDITABLE (my d_widget), from_pos, to_pos);
			gint from_pos_gint = from_pos;
			gtk_editable_insert_text (GTK_EDITABLE (my d_widget), newText, g_utf8_strlen (newText, -1), & from_pos_gint);
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset (buffer, & from_it, from_pos);
			gtk_text_buffer_get_iter_at_offset (buffer, & to_it, to_pos);
			gtk_text_buffer_delete_interactive (buffer, & from_it, & to_it,
				gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
			gtk_text_buffer_insert_interactive (buffer, & from_it, newText, g_utf8_strlen (newText, -1),
				gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif cocoa
		if (my d_cocoaTextView) {
			long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			{// scope
				autostring32 oldText = GuiText_getString (me);
				for (long i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
				for (long i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			}
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			NSRange nsRange = NSMakeRange (from_pos, to_pos - from_pos);
			NSString *nsString = (NSString *) Melder_peek32toCfstring (text);
			[my d_cocoaTextView   shouldChangeTextInRange: nsRange   replacementString: nsString];   // ignore the returned BOOL: only interested in the side effect of having undo support
			[[my d_cocoaTextView   textStorage] replaceCharactersInRange: nsRange   withString: nsString];
		}
	#elif win
		const char32 *from;
		char32 *winText = Melder_malloc_f (char32, 2 * str32len (text) + 1), *to;   // all new lines plus one null byte
		Melder_assert (MEMBER (my d_widget, Text));
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != U'\0'; from ++, to ++)
			if (*from == U'\n') { *to = 13; * ++ to = U'\n'; } else *to = *from;
		*to = U'\0';
		/*
		 * We DON'T replace any text without selecting it, so we can deselect any other text,
		 * thus allowing ourselves to select [from_pos, to_pos] and use the REPLACESEL message.
		 */
		GuiText_setSelection (me, from_pos, to_pos);
		Edit_ReplaceSel (my d_widget -> window, Melder_peek32toW (winText));
		Melder_free (winText);
		UpdateWindow (my d_widget -> window);
	#elif mac
		size_t length = str32len (text);
		char32 *macText = Melder_malloc_f (char32, length + 1);
		Melder_assert (my d_widget -> widgetClass == xmTextWidgetClass);
		str32ncpy (macText, text, length);
		macText [length] = '\0';
		/*
		 * Replace all LF with CR.
		 */
		for (size_t i = 0; i < length; i ++) if (macText [i] == '\n') macText [i] = 13;
		/*
		 * We DON'T replace any text without selecting it, so we can deselect any other text,
		 * thus allowing ourselves to select [from_pos, to_pos] and use selection replacement.
		 */
		if (my d_widget -> managed) _GuiMac_clipOnParent (my d_widget);
		if (isTextControl (my d_widget)) {
			// BUG: this is not UTF-32-savvy; this is acceptable because it isn't used in Praat
			long oldLength = NativeText_getLength (my d_widget);
			char32 *totalText = Melder_malloc_f (char32, oldLength - (to_pos - from_pos) + length + 1);
			char32 *oldText = Melder_malloc_f (char32, oldLength + 1);
			NativeText_getText (my d_widget, oldText, oldLength);
			str32ncpy (totalText, oldText, from_pos);
			str32cpy (totalText + from_pos, macText);
			str32cpy (totalText + from_pos + length, oldText + to_pos);
			CFStringRef totalText_cfstring = (CFStringRef) Melder_peek32toCfstring (totalText);
			SetControlData (my d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & totalText_cfstring);
			Melder_free (oldText);
			Melder_free (totalText);
		} else if (isMLTE (me)) {
			long oldLength = NativeText_getLength (my d_widget);
			char32 *oldText = Melder_malloc_f (char32, oldLength + 1);
			NativeText_getText (my d_widget, oldText, oldLength);
			long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			for (long i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
			for (long i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			const char16 *macText_utf16 = (const char16 *) Melder_peek32to16 (macText);
			TXNSetData (my d_macMlteObject, kTXNUnicodeTextData, macText_utf16, str32len_utf16 (macText, 0) * 2, from_pos, to_pos);
		}
		Melder_free (macText);
		if (my d_widget -> managed) {
			if (isTextControl (my d_widget)) {
				Draw1Control (my d_widget -> nat.control.handle);
			} else if (isMLTE (me)) {
			}
			GuiMac_clipOff ();
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

void GuiText_scrollToSelection (GuiText me) {
	#if gtk
		GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
		GtkTextIter start, end;
		gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
		//GtkTextMark *mark = gtk_text_buffer_create_mark (textBuffer, nullptr, & start, true);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (my d_widget), & start, 0.1, false, 0.0, 0.0);
		//gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (my d_widget), mark, 0.1, false, 0.0, 0.0);
	#elif cocoa
		if (my d_cocoaTextView)
			[my d_cocoaTextView   scrollRangeToVisible: [my d_cocoaTextView   selectedRange]];
	#elif win
		Edit_ScrollCaret (my d_widget -> window);
	#elif mac
		if (isTextControl (my d_widget)) {
			;
		} else if (isMLTE (me)) {
			TXNShowSelection (my d_macMlteObject, false);
		}
	#endif
}

void GuiText_setChangeCallback (GuiText me, void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss) {
	my d_changeCallback = changeCallback;
	my d_changeBoss = changeBoss;
}

void GuiText_setFontSize (GuiText me, int size) {
	#if gtk
		GtkRcStyle *modStyle = gtk_widget_get_modifier_style (GTK_WIDGET (my d_widget));
		trace (U"before initializing Pango: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		PangoFontDescription *fontDesc = modStyle -> font_desc != nullptr ? modStyle -> font_desc :
			#if ALLOW_GDK_DRAWING
				pango_font_description_copy (GTK_WIDGET (my d_widget) -> style -> font_desc);
			#else
				nullptr;
			#endif
		trace (U"during initializing Pango: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		pango_font_description_set_absolute_size (fontDesc, size * PANGO_SCALE);
		trace (U"after initializing Pango: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		modStyle -> font_desc = fontDesc;
		gtk_widget_modify_style (GTK_WIDGET (my d_widget), modStyle);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   setFont: [NSFont fontWithName: @"Menlo"   size: size]];
		}
	#elif win
		// a trick to update the window. BUG: why doesn't UpdateWindow seem to suffice?
		long first, last;
		char32 *text = GuiText_getStringAndSelectionPosition (me, & first, & last);
		GuiText_setString (me, U"");   // erase all
		UpdateWindow (my d_widget -> window);
		if (size <= 10) {
			SetWindowFont (my d_widget -> window, font10, false);
		} else if (size <= 12) {
			SetWindowFont (my d_widget -> window, font12, false);
		} else if (size <= 14) {
			SetWindowFont (my d_widget -> window, font14, false);
		} else if (size <= 18) {
			SetWindowFont (my d_widget -> window, font18, false);
		} else {
			SetWindowFont (my d_widget -> window, font24, false);
		}
		GuiText_setString (me, text);
		Melder_free (text);
		GuiText_setSelection (me, first, last);
		UpdateWindow (my d_widget -> window);
	#elif mac
		if (isMLTE (me)) {
			TXNTypeAttributes attr;
			attr. tag = kTXNQDFontSizeAttribute;
			attr. size = kTXNFontSizeAttributeSize;
			attr. data. dataValue = (unsigned long) size << 16;
			TXNSetTypeAttributes (my d_macMlteObject, 1, & attr, 0, 2000000000);
		}
	#endif
}

void GuiText_setRedoItem (GuiText me, GuiMenuItem item) {
	#if gtk
		if (my d_redo_item)
			//g_object_unref (my d_redo_item -> d_widget);
		my d_redo_item = item;
		if (my d_redo_item) {
			//g_object_ref (my d_redo_item -> d_widget);
			GuiThing_setSensitive (my d_redo_item, history_has_redo (me));
		}
	#elif cocoa
	#elif win
	#elif mac
	#endif
}

void GuiText_setSelection (GuiText me, long first, long last) {
	if (my d_widget) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_select_region (GTK_EDITABLE (my d_widget), first, last);
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset (buffer, & from_it, first);
			gtk_text_buffer_get_iter_at_offset (buffer, & to_it, last);
			gtk_text_buffer_select_range (buffer, & from_it, & to_it);
		}
	#elif cocoa
		/*
		 * On Cocoa, characters are counted in UTF-16 units, whereas 'first' and 'last' are in UTF-32 units. Convert.
		 */
		char32 *text = GuiText_getString (me);
		long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (long i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (long i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
		Melder_free (text);

		if (my d_cocoaTextView) {
			[my d_cocoaTextView   setSelectedRange: NSMakeRange (first, last - first)];
		}
	#elif win
		char32 *text = GuiText_getString (me);
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		long length = str32len (text);
		if (first >= length) first = length;
		if (last >= length) last = length;
		/*
		 * 'first' and 'last' are the positions of the selection in the text when separated by LF alone.
		 * We have to convert this to the positions that the selection has in a text separated by CR/LF sequences.
		 */
		long numberOfLeadingLineBreaks = 0, numberOfSelectedLineBreaks = 0;
		for (long i = 0; i < first; i ++) if (text [i] == U'\n') numberOfLeadingLineBreaks ++;
		for (long i = first; i < last; i ++) if (text [i] == U'\n') numberOfSelectedLineBreaks ++;
		/*
		 * On Windows, characters are counted in UTF-16 units, whereas 'first' and 'last' are in UTF-32 units. Convert.
		 */
		long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (long i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (long i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;

		first += numberOfLeadingLineBreaks;
		last += numberOfLeadingLineBreaks + numberOfSelectedLineBreaks;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
		Melder_free (text);

		Edit_SetSel (my d_widget -> window, first, last);
		UpdateWindow (my d_widget -> window);
	#elif mac
		char32 *text = GuiText_getString (me);
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		long length = str32len (text);
		if (first >= length) first = length;
		if (last >= length) last = length;
		long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (long i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (long i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
		Melder_free (text);
		if (isTextControl (my d_widget)) {
			ControlEditTextSelectionRec rec = { (int16_t) first, (int16_t) last };
			SetControlData (my d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec);
		} else if (isMLTE (me)) {
			TXNSetSelection (my d_macMlteObject, first, last);
		}
	#endif
	}
}

void GuiText_setString (GuiText me, const char32 *text) {
	#if gtk
		if (G_OBJECT_TYPE (my d_widget) == GTK_TYPE_ENTRY) {
			gtk_entry_set_text (GTK_ENTRY (my d_widget), Melder_peek32to8 (text));
		} else if (G_OBJECT_TYPE (my d_widget) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			gchar *textUtf8 = Melder_peek32to8 (text);
			//gtk_text_buffer_set_text (textBuffer, textUtf8, strlen (textUtf8));   // length in bytes!
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gtk_text_buffer_delete_interactive (textBuffer, & start, & end, gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
			gtk_text_buffer_insert_interactive (textBuffer, & start, textUtf8, strlen (textUtf8), gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif cocoa
		trace (U"title");
		if (my d_cocoaTextView) {
			NSRange nsRange = NSMakeRange (0, [[my d_cocoaTextView   textStorage] length]);
			NSString *nsString = (NSString *) Melder_peek32toCfstring (text);
			[my d_cocoaTextView   shouldChangeTextInRange: nsRange   replacementString: nsString];   // to make this action undoable
			//[[my d_cocoaTextView   textStorage] replaceCharactersInRange: nsRange   withString: nsString];
			[my d_cocoaTextView   setString: nsString];
			[my d_cocoaTextView   scrollRangeToVisible: NSMakeRange ([[my d_cocoaTextView   textStorage] length], 0)];   // to the end
			//[[my d_cocoaTextView   window] setViewsNeedDisplay: YES];
			//[[my d_cocoaTextView   window] display];
		} else {
			[(NSTextField *) my d_widget   setStringValue: (NSString *) Melder_peek32toCfstring (text)];
		}
	#elif win
		const char32 *from;
		char32 *winText = Melder_malloc_f (char32, 2 * str32len (text) + 1), *to;   /* All new lines plus one null byte. */
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != U'\0'; from ++, to ++)
			if (*from == U'\n') { *to = 13; * ++ to = U'\n'; } else *to = *from;
		*to = U'\0';
		SetWindowTextW (my d_widget -> window, Melder_peek32toW (winText));
		Melder_free (winText);
		UpdateWindow (my d_widget -> window);
	#elif mac
		long length_utf32 = str32len (text), length_utf16 = str32len_utf16 (text, false);
		UniChar *macText = Melder_malloc_f (UniChar, length_utf16 + 1);
		//Melder_assert (macText);
		//Melder_assert (my d_widget);
		//Melder_assert (my d_widget -> widgetClass == xmTextWidgetClass);
		/*
		 * Convert from UTF-32 to UTF-16 and replace all LF with CR.
		 */
		long j = 0;
		for (long i = 0; i < length_utf32; i ++) {
			char32 kar = (char32) text [i];   // reinterpret sign bit
			if (kar == '\n') {   // LF
				macText [j ++] = 13;   // CR
			} else if (kar <= 0x00FFFF) {
				macText [j ++] = kar;
			} else {
				Melder_assert (kar <= 0x10FFFF);
				kar -= 0x010000;
				macText [j ++] = (UniChar) (0x00D800 | (kar >> 10));   // first UTF-16 surrogate character
				macText [j ++] = (UniChar) (0x00DC00 | (kar & 0x0003FF));   // second UTF-16 surrogate character
			}
		}
		macText [j] = '\0';
		if (j != length_utf16)
			Melder_fatal (U"GuiText_setString: incorrect number of UTF-16 words (", j, U" instead of ", length_utf16, U"): <<", text, U">>.");
		if (isTextControl (my d_widget)) {
			CFStringRef cfString = CFStringCreateWithCharacters (nullptr, macText, length_utf16);
			SetControlData (my d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString);
			CFRelease (cfString);
		} else if (isMLTE (me)) {
			TXNSetData (my d_macMlteObject, kTXNUnicodeTextData, macText, length_utf16*2, 0, NativeText_getLength (my d_widget));
		}
		Melder_free (macText);
		if (my d_widget -> managed) {
			if (theGui.duringUpdate) {
				_Gui_invalidateWidget (my d_widget);   // HACK: necessary because VisRgn has temporarily been changed (not used in Praat any longer)
			} else {
				if (isTextControl (my d_widget)) {
					_GuiMac_clipOnParent (my d_widget);
					Draw1Control (my d_widget -> nat.control.handle);
					GuiMac_clipOff ();
				} else if (isMLTE (me)) {
				}
			}
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

void GuiText_setUndoItem (GuiText me, GuiMenuItem item) {
	#if gtk
		if (my d_undo_item) {
			//g_object_unref (my d_undo_item -> d_widget);
		}
		my d_undo_item = item;
		if (my d_undo_item) {
			//g_object_ref (my d_undo_item -> d_widget);
			GuiThing_setSensitive (my d_undo_item, history_has_undo (me));
		}
	#elif cocoa
	#elif win
	#elif mac
	#endif
}

void GuiText_undo (GuiText me) {
	#if gtk
		history_do (me, 1);
	#elif cocoa
		if (my d_cocoaTextView) {
			[[my d_cocoaTextView   undoManager] undo];
		}
	#elif win
	#elif mac
		if (isMLTE (me)) {
			TXNUndo (my d_macMlteObject);
		}
		_GuiText_handleValueChanged (my d_widget);
	#endif
}

/* End of file GuiText.cpp */
