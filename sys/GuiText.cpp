/* GuiText.cpp
 *
 * Copyright (C) 1993-2011,2012,2013 Paul Boersma, 2013 Tom Naughton
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
 * pb 2007/02/15 Mac: GuiText_updateChangeCountAfterSave
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
 * pb,tm 2013    Cocoa
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
	#define isMLTE(w)  ((w) -> d_macMlteObject != NULL)
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
		theGui.textFocus = NULL;
}

#if mac
void _GuiMac_clearTheTextFocus (void) {
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
	if (widget == NULL || theGui.textFocus == widget
		|| ! widget -> managed) return;   /* Perhaps not-yet-managed. Test: open Praat's DataEditor with a Sound, then type. */
	#if gtk
		gtk_widget_grab_focus (GTK_WIDGET (widget));
	#elif win
		SetFocus (widget -> window);   /* Will send an EN_SETFOCUS notification, which will call _GuiText_handleFocusReception (). */
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
		struct structGuiTextEvent event = { me };
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
					if (event -> what != autoKey) my f_cut ();
					return 1;
				}
				if (charCode == 'C') {
					if (event -> what != autoKey) my f_copy ();
					return 1;
				}
				if (charCode == 'V' && my d_editable) {
					my f_paste ();
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
				//Melder_casual ("char code %d", charCode);
				if (isTextControl (widget)) {
					CallNextEventHandler (eventHandlerCallRef, eventRef);
				} else if (isMLTE (me)) {
					//static long key = 0; Melder_casual ("key %ld", ++key);
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
		return 0;   /* Not handled. */
	}
	void _GuiMacText_handleClick (GuiObject widget, EventRecord *event) {
		iam_text;
		_GuiText_setTheTextFocus (widget);
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			HandleControlClick (widget -> nat.control.handle, event -> where, event -> modifiers, NULL);
		} else if (isMLTE (me)) {
			LocalToGlobal (& event -> where);
			TXNClick (my d_macMlteObject, event);   /* Handles text selection and scrolling. */
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
			theGui.textFocus = NULL;   // remove dangling reference
		if (widget == widget -> shell -> textFocus)
			widget -> shell -> textFocus = NULL;   // remove dangling reference
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
			theGui.textFocus = NULL;   // remove dangling reference
		if (widget == widget -> shell -> textFocus)
			widget -> shell -> textFocus = NULL;   // remove dangling reference
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
			TXNDraw (my d_macMlteObject, NULL);
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
		return Edit_GetTextLength (widget -> window);
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			Size size;
			CFStringRef cfString;
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, NULL);
			size = CFStringGetLength (cfString);
			CFRelease (cfString);
			return size;
		} else if (isMLTE (me)) {
			#if 1
				/*
				 * From the reference page of TXNDataSize:
				 * "If you are using Unicode and you want to know the number of characters,
				 * divide the returned ByteCount value by sizeof(UniChar) or 2,
				 * since MLTE uses the 16-bit Unicode Transformation Format (UTF-16)."
				 */
				return TXNDataSize (my d_macMlteObject) / sizeof (UniChar);
			#else
				long length = 0, dataSize = TXNDataSize (my d_macMlteObject);
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my d_macMlteObject, 0, dataSize, & numberOfRuns);
				for (long irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my d_macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my d_macMlteObject, left, right, & han, kTXNUnicodeTextData);
						if (han) {
							long size = GetHandleSize (han) / 2;
							length += size;
							DisposeHandle (han);
						}
					}
				}
				return length;
			#endif
		}
		return 0;   // Should not occur.
	#endif
}

static void NativeText_getText (GuiObject widget, wchar_t *buffer, long length) {
	#if win
		GetWindowText (widget -> window, buffer, length + 1);
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			CFStringRef cfString;
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, NULL);
			UniChar *macText = Melder_malloc_f (UniChar, length + 1);
			CFRange range = { 0, length };
			CFStringGetCharacters (cfString, range, macText);
			CFRelease (cfString);
			long j = 0;
			for (long i = 0; i < length; i ++) {
				unsigned long kar = macText [i];
				if (kar < 0xD800 || kar > 0xDFFF) {
					buffer [j ++] = kar;
				} else {
					Melder_assert (kar >= 0xD800 && kar <= 0xDBFF);
					unsigned long kar1 = macText [++ i];
					Melder_assert (kar1 >= 0xDC00 && kar1 <= 0xDFFF);
					buffer [j ++] = 0x10000 + ((kar & 0x3FF) << 10) + (kar1 & 0x3FF);
				}
			}
			buffer [j] = '\0';
			Melder_free (macText);
		} else if (isMLTE (me)) {
			#if 1
				Handle han;
				TXNGetDataEncoded (my d_macMlteObject, 0, length, & han, kTXNUnicodeTextData);
				long j = 0;
				for (long i = 0; i < length; i ++) {
					unsigned long kar = ((UniChar *) *han) [i];
					if (kar < 0xD800 || kar > 0xDFFF) {
						buffer [j ++] = kar;
					} else {
						Melder_assert (kar >= 0xD800 && kar <= 0xDBFF);
						unsigned long kar1 = ((UniChar *) *han) [++ i];
						Melder_assert (kar1 >= 0xDC00 && kar1 <= 0xDFFF);
						buffer [j ++] = 0x10000 + ((kar & 0x3FF) << 10) + (kar1 & 0x3FF);
					}
				}
				buffer [j] = '\0';
				DisposeHandle (han);
			#else
				long dataSize = TXNDataSize (my d_macMlteObject);
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my d_macMlteObject, 0, dataSize, & numberOfRuns);
				for (long irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my d_macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my d_macMlteObject, left, right, & han, kTXNUnicodeTextData);
						if (han) {
							long size = GetHandleSize (han) / 2;
							wcsncpy (buffer, (wchar_t *) *han, size);
							buffer += size;
							DisposeHandle (han);
						}
					}
				}
				buffer [0] = '\0';
				return;
			#endif
		}
	#endif
	buffer [length] = '\0';   // superfluous?
}

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
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec, NULL);
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

void _GuiText_init (void) {
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

void _GuiText_exit (void) {
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
	 * returns NULL.
	 * Specifically the function expands the previous insert/delete event(s)
	 *  - with the current, if current also is an insert/delete event and the ranges of previous and current event match
	 *  - with the previous delete and current insert event, in case the ranges of both event-pairs respectively match
	 */
	static history_entry * history_addAndMerge (void *void_me, history_data text_new, long first, long last, bool deleted) {
		iam(GuiText);
		history_entry *he = NULL;
		
		if (!(my d_prev))
			return NULL;
		
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
		my d_next = NULL;
		
		history_entry *he = history_addAndMerge (void_me, text_new, first, last, deleted);
		if (he == NULL) {
			he = (history_entry *) malloc (sizeof (history_entry));
			he->first = first;
			he->last = last;
			he->type_del = deleted;
			he->text = text_new;
			
			he->prev = my d_prev;
			he->next = NULL;
			if (my d_prev)
				my d_prev->next = he;
		}
		my d_prev = he;
		he->next = NULL;
		
		if (my d_undo_item) my d_undo_item -> f_setSensitive (true);
		if (my d_redo_item) my d_redo_item -> f_setSensitive (false);
	}

	static bool history_has_undo (void *void_me) {
		iam (GuiText);
		return my d_prev != NULL;
	}

	static bool history_has_redo (void *void_me) {
		iam (GuiText);
		return my d_next != NULL;
	}

	static void history_do (void *void_me, bool undo) {
		iam (GuiText);
		history_entry *he = undo ? my d_prev : my d_next;
		if (he == NULL) // TODO: this function should not be called in that case
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
		
		if (my d_undo_item) my d_undo_item -> f_setSensitive (history_has_undo(me));
		if (my d_redo_item) my d_redo_item -> f_setSensitive (history_has_redo(me));
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
		my d_prev = NULL;
		
		h1 = my d_next;
		while (h1) {
			h2 = h1->next;
			free(h1->text);
			free(h1);
			h1 = h2;
		}
		my d_next = NULL;
		
		if (my d_undo_item) my d_undo_item -> f_setSensitive (false);
		if (my d_redo_item) my d_redo_item -> f_setSensitive (false);
	}
#endif

/*
 * CALLBACKS
 */

#if gtk
	static void _GuiGtkEntry_history_delete_cb (GtkEditable *ed, gint from, gint to, gpointer void_me) {
		iam (GuiText);
		trace ("begin");
		if (my d_history_change) return;
		history_add (me, gtk_editable_get_chars (GTK_EDITABLE (ed), from, to), from, to, 1);
	}
	
	static void _GuiGtkEntry_history_insert_cb (GtkEditable *ed, gchar *utf8_text, gint len, gint *from, gpointer void_me) {
		(void) ed;
		iam (GuiText);
		trace ("begin");
		if (my d_history_change) return;
		gchar *text = (gchar *) malloc (sizeof (gchar) * (len + 1));
		strcpy (text, utf8_text);
		history_add (me, text, *from, *from + len, 0);
	}
	
	static void _GuiGtkTextBuf_history_delete_cb (GtkTextBuffer *buffer, GtkTextIter *from, GtkTextIter *to, gpointer void_me) {
		iam (GuiText);
		trace ("begin");
		if (my d_history_change) return;
		int from_pos = gtk_text_iter_get_offset (from);
		int to_pos = gtk_text_iter_get_offset (to);
		history_add (me, gtk_text_buffer_get_text (buffer, from, to, FALSE), from_pos, to_pos, 1);
	}
	
	static void _GuiGtkTextBuf_history_insert_cb (GtkTextBuffer *buffer, GtkTextIter *from, gchar *utf8_text, gint len, gpointer void_me) {
		(void) buffer;
		iam (GuiText);
		trace ("begin");
		if (my d_history_change) return;
		int from_pos = gtk_text_iter_get_offset (from);
		gchar *text = (gchar *) malloc (sizeof (gchar) * (len + 1));
		strcpy (text, utf8_text);
		history_add (me, text, from_pos, from_pos + len, 0);
	}
	
	static void _GuiGtkText_valueChangedCallback (GuiObject widget, gpointer void_me) {
		iam (GuiText);
		trace ("begin");
		Melder_assert (me != NULL);
		if (my d_changeCallback != NULL) {
			struct structGuiTextEvent event = { me };
			my d_changeCallback (my d_changeBoss, & event);
		}
	}
	
	static void _GuiGtkText_destroyCallback (GuiObject widget, gpointer void_me) {
		(void) widget;
		iam (GuiText);
		Melder_assert (me != NULL);
		Melder_assert (my classInfo == classGuiText);
		trace ("begin");
		if (my d_undo_item) {
			trace ("undo");
			//g_object_unref (my d_undo_item -> d_widget);
		}
		if (my d_redo_item) {
			trace ("redo");
			//g_object_unref (my d_redo_item -> d_widget);
		}
		my d_undo_item = NULL;
		my d_redo_item = NULL;
		trace ("history");
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
		trace ("deleting a text field");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiText));
		d_userData = static_cast <GuiText> (userData);
	}
	- (void) textDidChange: (NSNotification *) notification {
		(void) notification;
		GuiText me = d_userData;
		if (me && my d_changeCallback) {
			struct structGuiTextEvent event = { me };
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
		trace ("deleting a text view");
		[super dealloc];
	}
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiText));
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
		GuiText me = d_userData;
		if (me && my d_changeCallback) {
			struct structGuiTextEvent event = { me };
			my d_changeCallback (my d_changeBoss, & event);
		}
		return YES;
	}
	@end
#elif win
#elif mac
#endif

GuiText GuiText_create (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags) {
	GuiText me = Thing_new (GuiText);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	#if gtk
		trace ("before creating a GTK text widget: locale is %s", setlocale (LC_ALL, NULL));
		if (flags & GuiText_SCROLLED) {
			GtkWrapMode ww;
			GuiObject scrolled = gtk_scrolled_window_new (NULL, NULL);
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
		trace ("after creating a GTK text widget: locale is %s", setlocale (LC_ALL, NULL));
		my d_prev = NULL;
		my d_next = NULL;
		my d_history_change = 0;
		my d_undo_item = NULL;
		my d_redo_item = NULL;
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkText_destroyCallback), me);
	#elif cocoa
		if (flags & GuiText_SCROLLED) {
			my d_cocoaScrollView = [[GuiCocoaScrolledWindow alloc] init];
			[my d_cocoaScrollView setUserData: NULL];   // because those user data can only be GuiScrolledWindow
			my d_widget = my d_cocoaScrollView;
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
			[my d_cocoaScrollView setBorderType: NSNoBorder];
			[my d_cocoaScrollView setHasHorizontalScroller: YES];
			[my d_cocoaScrollView setHasVerticalScroller:   YES];
			[my d_cocoaScrollView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			NSSize contentSize = [my d_cocoaScrollView contentSize];
			my d_cocoaTextView = [[GuiCocoaTextView alloc] initWithFrame: NSMakeRect (0, 0, contentSize. width, contentSize. height)];
			[my d_cocoaTextView setUserData: me];
			[my d_cocoaTextView setMinSize: NSMakeSize (0.0, contentSize.height)];
			[my d_cocoaTextView setMaxSize: NSMakeSize (FLT_MAX, FLT_MAX)];
			[my d_cocoaTextView setVerticallyResizable: YES];
			[my d_cocoaTextView setHorizontallyResizable: YES];
			[my d_cocoaTextView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			[[my d_cocoaTextView textContainer] setContainerSize: NSMakeSize (FLT_MAX, FLT_MAX)];
			[[my d_cocoaTextView textContainer] setWidthTracksTextView: NO];
			[my d_cocoaScrollView setDocumentView: my d_cocoaTextView];
			[[my d_cocoaScrollView window] makeFirstResponder: my d_cocoaTextView];
			static NSFont *theTextFont;
			if (! theTextFont) {
				theTextFont = [[NSFont systemFontOfSize: 13.0] retain];
			}
			[my d_cocoaTextView setFont: theTextFont];
			[my d_cocoaTextView setAllowsUndo: YES];
			//[my d_cocoaTextView turnOffLigatures: nil];
			[my d_cocoaTextView setSmartInsertDeleteEnabled: NO];
			//[my d_cocoaTextView setAutomaticQuoteSubstitutionEnabled: NO];
			[my d_cocoaTextView setAutomaticTextReplacementEnabled: NO];
			//[my d_cocoaTextView setAutomaticDashSubstitutionEnabled: NO];
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
		my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, flags & GuiText_SCROLLED ? L"scrolledText" : L"text");
		_GuiObject_setUserData (my d_widget, me);
		my d_editable = (flags & GuiText_NONEDITABLE) == 0;
		my d_widget -> window = CreateWindow (L"edit", NULL, WS_CHILD | WS_BORDER
			| ( flags & GuiText_WORDWRAP ? ES_AUTOVSCROLL : ES_AUTOHSCROLL )
			| ES_MULTILINE | WS_CLIPSIBLINGS
			| ( flags & GuiText_SCROLLED ? WS_HSCROLL | WS_VSCROLL : 0 ),
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		if (! font10) {
			font10 = CreateFont (13, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font12 = CreateFont (16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font14 = CreateFont (19, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font18 = CreateFont (24, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
			font24 = CreateFont (32, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
		}
		SetWindowFont (my d_widget -> window, font12 /*theScrolledHint ? font : GetStockFont (ANSI_VAR_FONT)*/, FALSE);
		Edit_LimitText (my d_widget -> window, 0);
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my d_widget -> shell -> textFocus) {
			my d_widget -> shell -> textFocus = my d_widget;   /* Even if not-yet-managed. But in that case it will not receive global focus. */
		}
	#elif mac
		if (flags & GuiText_SCROLLED) {
			my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, L"scrolledText");
			_GuiObject_setUserData (my d_widget, me);
			my d_editable = (flags & GuiText_NONEDITABLE) == 0;
			TXNLongRect destRect;
			TXNMargins margins;
			TXNControlData controlData;
			TXNControlTag controlTag = kTXNMarginsTag;
			TXNNewObject (NULL,   /* No file. */
				my d_widget -> macWindow, & my d_widget -> rect, kTXNWantHScrollBarMask | kTXNWantVScrollBarMask
					| kTXNMonostyledTextMask | kTXNDrawGrowIconMask,
				kTXNTextEditStyleFrameType, kTXNTextensionFile,
				/*kTXNMacOSEncoding*/ kTXNSystemDefaultEncoding, & my d_macMlteObject, & my d_macMlteFrameId, me);
			destRect. left = 0;
			destRect. top = 0;
			destRect. right = 10000;
			destRect. bottom = 2000000000;
			TXNSetRectBounds (my d_macMlteObject, NULL, & destRect, FALSE);
			margins. leftMargin = 3;
			margins. topMargin = 3;
			margins. rightMargin = 0;
			margins. bottomMargin = 0;
			controlData. marginsPtr = & margins;
			TXNSetTXNObjectControls (my d_macMlteObject, FALSE, 1, & controlTag, & controlData);
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		} else {
			my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, L"text");
			_GuiObject_setUserData (my d_widget, me);
			my d_editable = (flags & GuiText_NONEDITABLE) == 0;
			Rect r = my d_widget -> rect;
			InsetRect (& r, 3, 3);
			CreateEditUnicodeTextControl (my d_widget -> macWindow, & r, NULL, false, NULL, & my d_widget -> nat.control.handle);
			SetControlReference (my d_widget -> nat.control.handle, (long) my d_widget);
			my d_widget -> isControl = TRUE;
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		}
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my d_widget -> shell -> textFocus) {
			my d_widget -> shell -> textFocus = my d_widget;   /* Even if not-yet-managed. But in that case it will not receive global focus. */
		}
	#endif
	
	return me;
}

GuiText GuiText_createShown (GuiForm parent, int left, int right, int top, int bottom, unsigned long flags) {
	GuiText me = GuiText_create (parent, left, right, top, bottom, flags);
	my f_show ();
	return me;
}

void structGuiText :: f_copy () {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_copy_clipboard (GTK_EDITABLE (d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_copy_clipboard (buffer, cb);
		}
	#elif cocoa
		if (d_cocoaTextView) {
			[d_cocoaTextView copy: nil];
		} else {
			[[[(GuiCocoaTextField *) d_widget window]   fieldEditor: NO   forObject: nil] copy: nil];
		}
	#elif win
		if (! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		SendMessage (d_widget -> window, WM_COPY, 0, 0);
	#elif mac
		if (! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		if (isTextControl (d_widget)) {
			HandleControlKey (d_widget -> nat.control.handle, 0, 'C', cmdKey);
		} else if (isMLTE (this)) {
			TXNCopy (d_macMlteObject);
		}
	#endif
}

void structGuiText :: f_cut () {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_cut_clipboard (GTK_EDITABLE (d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_cut_clipboard (buffer, cb, gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
		}
	#elif cocoa
		if (d_cocoaTextView) {
			[d_cocoaTextView cut: nil];
		} else {
			[[[(GuiCocoaTextField *) d_widget window]   fieldEditor: NO   forObject: nil] cut: nil];
		}
	#elif win
		if (! d_editable || ! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		SendMessage (d_widget -> window, WM_CUT, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (d_widget -> window);
	#elif mac
		if (! d_editable || ! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		if (isTextControl (d_widget)) {
			_GuiMac_clipOnParent (d_widget);
			HandleControlKey (d_widget -> nat.control.handle, 0, 'X', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (this)) {
			TXNCut (d_macMlteObject);
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

wchar_t * structGuiText :: f_getSelection () {
	#if gtk
		// first = gtk_text_iter_get_offset (& start);
		// last = gtk_text_iter_get_offset (& end);
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gint start, end;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (d_widget), & start, & end); 
			if (end > start) {   // at least one character selected?
				gchar *text = gtk_editable_get_chars (GTK_EDITABLE (d_widget), start, end);
				wchar_t *result = Melder_utf8ToWcs (text);
				g_free (text);
				return result;
			}
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			if (gtk_text_buffer_get_has_selection (textBuffer)) {   // at least one character selected?
				GtkTextIter start, end;
				gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
				gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, TRUE);
				wchar_t *result = Melder_utf8ToWcs (text);
				g_free (text);
				return result;
			}
		}
	#elif cocoa
		long start, end;
		autostring selection = f_getStringAndSelectionPosition (& start, & end);
		long length = end - start;
		if (length > 0) {
			wchar_t *result = Melder_malloc_f (wchar_t, length + 1);
			memcpy (result, & selection [start], length * sizeof (wchar_t));
			result [length] = '\0';
			Melder_killReturns_inlineW (result);
			return result;
		}
	#elif win
		long start, end;
		NativeText_getSelectionRange (d_widget, & start, & end);
		if (end > start) {   // at least one character selected?
			/*
			 * Get all text.
			 */
			long length = NativeText_getLength (d_widget);
			wchar_t *result = Melder_malloc_f (wchar_t, length + 1);
			NativeText_getText (d_widget, result, length);
			/*
			 * Zoom in on selection.
			 */
			length = end - start;
			memmove (result, result + start, length * sizeof (wchar_t));   /* Not because of realloc, but because of free! */
			result [length] = '\0';
			result = (wchar_t *) Melder_realloc_f (result, (length + 1) * sizeof (wchar_t));   /* Optional. */
			Melder_killReturns_inlineW (result);   /* AFTER zooming! */
			return result;
		}
	#elif mac
		long start, end;
		NativeText_getSelectionRange (d_widget, & start, & end);
		if (end > start) {   // at least one character selected?
			/*
			 * Get all text.
			 */
			long length = NativeText_getLength (d_widget);
			wchar_t *result = Melder_malloc_f (wchar_t, length + 1);
			NativeText_getText (d_widget, result, length);
			/*
			 * Zoom in on selection.
			 */
			#if mac
				for (long i = 0; i < start; i ++) if (result [i] > 0xFFFF) { start --; end --; }
				for (long i = start; i < end; i ++) if (result [i] > 0xFFFF) { end --; }
			#endif
			length = end - start;
			memmove (result, result + start, length * sizeof (wchar_t));   /* Not because of realloc, but because of free! */
			result [length] = '\0';
			result = (wchar_t *) Melder_realloc_f (result, (length + 1) * sizeof (wchar_t));   /* Optional. */
			Melder_killReturns_inlineW (result);   /* AFTER zooming! */
			return result;
		}
	#endif
	return NULL;   // zero characters selected
}

wchar_t * structGuiText :: f_getString () {
	long first, last;
	return f_getStringAndSelectionPosition (& first, & last);
}

wchar_t * structGuiText :: f_getStringAndSelectionPosition (long *first, long *last) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gint first_gint, last_gint;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (d_widget), & first_gint, & last_gint);
			*first = first_gint;
			*last = last_gint;
			return Melder_utf8ToWcs (gtk_entry_get_text (GTK_ENTRY (d_widget)));
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, TRUE); // TODO: Hidden chars ook maar doen he?
			wchar_t *result = Melder_utf8ToWcs (text);
			g_free (text);
			gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
			*first = gtk_text_iter_get_offset (& start);
			*last = gtk_text_iter_get_offset (& end);
			return result;
		}
		return NULL;
	#elif cocoa
		if (d_cocoaTextView) {
			NSString *nsString = [d_cocoaTextView string];
			wchar_t *result = Melder_utf8ToWcs ([nsString UTF8String]);
			trace ("string %ls", result);
			NSRange nsRange = [d_cocoaTextView selectedRange];
			*first = nsRange. location;
			*last = *first + nsRange. length;
			for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
			for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
			return result;
		} else {
			NSString *nsString = [(NSTextField *) d_widget   stringValue];
			wchar_t *result = Melder_utf8ToWcs ([nsString UTF8String]);
			trace ("string %ls", result);
			NSRange nsRange = [[[(NSTextField *) d_widget window] fieldEditor: NO forObject: nil] selectedRange];
			*first = nsRange. location;
			*last = *first + nsRange. length;
			for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
			return result;
		}
	#elif win
		long length = NativeText_getLength (d_widget);
		wchar_t *result = Melder_malloc_f (wchar_t, length + 1);
		NativeText_getText (d_widget, result, length);
		NativeText_getSelectionRange (d_widget, first, last);
		long numberOfLeadingLineBreaks = 0, numberOfSelectedLineBreaks = 0;
		for (long i = 0; i < *first; i ++) if (result [i] == 13) numberOfLeadingLineBreaks ++;
		for (long i = *first; i < *last; i ++) if (result [i] == 13) numberOfSelectedLineBreaks ++;
		*first -= numberOfLeadingLineBreaks;
		*last -= numberOfLeadingLineBreaks + numberOfSelectedLineBreaks;
		Melder_killReturns_inlineW (result);
		return result;
	#elif mac
		long length = NativeText_getLength (d_widget);   // UTF-16 length; should be enough for UTF-32 buffer
		wchar_t *result = Melder_malloc_f (wchar_t, length + 1);
		NativeText_getText (d_widget, result, length);
		NativeText_getSelectionRange (d_widget, first, last);   // 'first' and 'last' are expressed in UTF-16 words
		for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
		for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
		Melder_killReturns_inlineW (result);
		return result;
	#endif
}

void structGuiText :: f_paste () {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_paste_clipboard (GTK_EDITABLE (d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkClipboard *cb = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_paste_clipboard (buffer, cb, NULL, gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
		}
	#elif cocoa
		if (d_cocoaTextView) {
			[d_cocoaTextView pasteAsPlainText: nil];
		} else {
			[[[(GuiCocoaTextField *) d_widget window]   fieldEditor: NO   forObject: nil] pasteAsPlainText: nil];
		}
	#elif win
		if (! d_editable) return;
		SendMessage (d_widget -> window, WM_PASTE, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (d_widget -> window);
	#elif mac
		if (! d_editable) return;
		if (isTextControl (d_widget)) {
			_GuiMac_clipOnParent (d_widget);
			HandleControlKey (d_widget -> nat.control.handle, 0, 'V', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (this)) {
			TXNPaste (d_macMlteObject);
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

void structGuiText :: f_redo () {
	#if cocoa
		if (d_cocoaTextView) {
			[[d_cocoaTextView undoManager] redo];
		}
	#elif mac
		if (isMLTE (this)) {
			TXNRedo (d_macMlteObject);
		}
		_GuiText_handleValueChanged (d_widget);
	#else
		history_do (this, 0);
	#endif
}

void structGuiText :: f_remove () {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_selection (GTK_EDITABLE (d_widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			gtk_text_buffer_delete_selection (buffer, TRUE, gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
		}
	#elif cocoa
	#elif win
		if (! d_editable || ! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		SendMessage (d_widget -> window, WM_CLEAR, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
		UpdateWindow (d_widget -> window);
	#elif mac
		if (! d_editable || ! NativeText_getSelectionRange (d_widget, NULL, NULL)) return;
		if (isTextControl (d_widget)) {
			_GuiMac_clipOnParent (d_widget);
			HandleControlKey (d_widget -> nat.control.handle, 0, 8, 0);   // backspace key
			GuiMac_clipOff ();
		} else if (isMLTE (this)) {
			TXNClear (d_macMlteObject);
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

void structGuiText :: f_replace (long from_pos, long to_pos, const wchar_t *text) {
	#if gtk
		gchar *newText = Melder_peekWcsToUtf8 (text);
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_text (GTK_EDITABLE (d_widget), from_pos, to_pos);
			gint from_pos_gint = from_pos;
			gtk_editable_insert_text (GTK_EDITABLE (d_widget), newText, g_utf8_strlen (newText, -1), & from_pos_gint);
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset (buffer, & from_it, from_pos);
			gtk_text_buffer_get_iter_at_offset (buffer, & to_it, to_pos);
			gtk_text_buffer_delete_interactive (buffer, & from_it, & to_it,
				gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
			gtk_text_buffer_insert_interactive (buffer, & from_it, newText, g_utf8_strlen (newText, -1),
				gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
		}
	#elif cocoa
		if (d_cocoaTextView) {
			long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			{// scope
				autostring oldText = f_getString ();
				for (long i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
				for (long i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			}
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			NSRange nsRange = NSMakeRange (from_pos, to_pos - from_pos);
			NSString *nsString = (NSString *) Melder_peekWcsToCfstring (text);
			[d_cocoaTextView shouldChangeTextInRange: nsRange replacementString: nsString];   // ignore the returned BOOL: only interested in the side effect of having undo support
			[[d_cocoaTextView textStorage] replaceCharactersInRange: nsRange withString: nsString];
		}
	#elif win
		const wchar_t *from;
		wchar_t *winText = Melder_malloc_f (wchar_t, 2 * wcslen (text) + 1), *to;   /* All new lines plus one null byte. */
		Melder_assert (MEMBER (d_widget, Text));
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != '\0'; from ++, to ++)
			if (*from == '\n') { *to = 13; * ++ to = '\n'; } else *to = *from;
		*to = '\0';
		/*
		 * We DON'T replace any text without selecting it, so we can deselect any other text,
		 * thus allowing ourselves to select [from_pos, to_pos] and use the REPLACESEL message.
		 */
		f_setSelection (from_pos, to_pos);
		Edit_ReplaceSel (d_widget -> window, winText);
		Melder_free (winText);
		UpdateWindow (d_widget -> window);
	#elif mac
		long length = wcslen (text), i;
		wchar_t *macText = Melder_malloc_f (wchar_t, length + 1);
		Melder_assert (d_widget -> widgetClass == xmTextWidgetClass);
		wcsncpy (macText, text, length);
		macText [length] = '\0';
		/*
		 * Replace all LF with CR.
		 */
		for (i = 0; i < length; i ++) if (macText [i] == '\n') macText [i] = 13;
		/*
		 * We DON'T replace any text without selecting it, so we can deselect any other text,
		 * thus allowing ourselves to select [from_pos, to_pos] and use selection replacement.
		 */
		if (d_widget -> managed) _GuiMac_clipOnParent (d_widget);
		if (isTextControl (d_widget)) {
			// BUG: this is not UTF-32-savvy; this is acceptable because it isn't used in Praat
			long oldLength = NativeText_getLength (d_widget);
			wchar_t *totalText = Melder_malloc_f (wchar_t, oldLength - (to_pos - from_pos) + length + 1);
			wchar_t *oldText = Melder_malloc_f (wchar_t, oldLength + 1);
			NativeText_getText (d_widget, oldText, oldLength);
			wcsncpy (totalText, oldText, from_pos);
			wcscpy (totalText + from_pos, macText);
			wcscpy (totalText + from_pos + length, oldText + to_pos);
			CFStringRef totalText_cfstring = (CFStringRef) Melder_peekWcsToCfstring (totalText);
			SetControlData (d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & totalText_cfstring);
			Melder_free (oldText);
			Melder_free (totalText);
		} else if (isMLTE (this)) {
			long oldLength = NativeText_getLength (d_widget);
			wchar_t *oldText = Melder_malloc_f (wchar_t, oldLength + 1);
			NativeText_getText (d_widget, oldText, oldLength);
			long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			for (long i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
			for (long i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			const UniChar *macText_utf16 = Melder_peekWcsToUtf16 (macText);
			TXNSetData (d_macMlteObject, kTXNUnicodeTextData, macText_utf16, wcslen_utf16 (macText, 0) * 2, from_pos, to_pos);
		}
		Melder_free (macText);
		if (d_widget -> managed) {
			if (isTextControl (d_widget)) {
				Draw1Control (d_widget -> nat.control.handle);
			} else if (isMLTE (this)) {
			}
			GuiMac_clipOff ();
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

void structGuiText :: f_scrollToSelection () {
	#if gtk
		GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
		GtkTextIter start, end;
		gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
		//GtkTextMark *mark = gtk_text_buffer_create_mark (textBuffer, NULL, & start, true);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (d_widget), & start, 0.1, false, 0.0, 0.0); 
		//gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW (d_widget), mark, 0.1, false, 0.0, 0.0);
	#elif cocoa
	#elif win
		Edit_ScrollCaret (d_widget -> window);
	#elif mac
		if (isTextControl (d_widget)) {
			;
		} else if (isMLTE (this)) {
			TXNShowSelection (d_macMlteObject, false);
		}
	#endif
}

void structGuiText :: f_setChangeCallback (void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss) {
	d_changeCallback = changeCallback;
	d_changeBoss = changeBoss;
}

void structGuiText :: f_setFontSize (int size) {
	#if gtk
		GtkRcStyle *modStyle = gtk_widget_get_modifier_style (GTK_WIDGET (d_widget));
		trace ("before initializing Pango: locale is %s", setlocale (LC_ALL, NULL));
		PangoFontDescription *fontDesc = modStyle -> font_desc != NULL ? modStyle->font_desc : pango_font_description_copy (GTK_WIDGET (d_widget) -> style -> font_desc);
		trace ("during initializing Pango: locale is %s", setlocale (LC_ALL, NULL));
		pango_font_description_set_absolute_size (fontDesc, size * PANGO_SCALE);
		trace ("after initializing Pango: locale is %s", setlocale (LC_ALL, NULL));
		modStyle -> font_desc = fontDesc;
		gtk_widget_modify_style (GTK_WIDGET (d_widget), modStyle);
	#elif cocoa
		if (d_cocoaTextView) {
			[d_cocoaTextView setFont: [NSFont systemFontOfSize: size]];
		}
	#elif win
		// a trick to update the window. BUG: why doesn't UpdateWindow seem to suffice?
		long first, last;
		wchar_t *text = f_getStringAndSelectionPosition (& first, & last);
		f_setString (L"");   // erase all
		UpdateWindow (d_widget -> window);
		if (size <= 10) {
			SetWindowFont (d_widget -> window, font10, FALSE);
		} else if (size <= 12) {
			SetWindowFont (d_widget -> window, font12, FALSE);
		} else if (size <= 14) {
			SetWindowFont (d_widget -> window, font14, FALSE);
		} else if (size <= 18) {
			SetWindowFont (d_widget -> window, font18, FALSE);
		} else {
			SetWindowFont (d_widget -> window, font24, FALSE);
		}
		f_setString (text);
		Melder_free (text);
		f_setSelection (first, last);
		UpdateWindow (d_widget -> window);
	#elif mac
		if (isMLTE (this)) {
			TXNTypeAttributes attr;
			attr. tag = kTXNQDFontSizeAttribute;
			attr. size = kTXNFontSizeAttributeSize;
			attr. data. dataValue = (unsigned long) size << 16;
			TXNSetTypeAttributes (d_macMlteObject, 1, & attr, 0, 2000000000);
		}
	#endif
}

void structGuiText :: f_setRedoItem (GuiMenuItem item) {
	#if gtk
		if (d_redo_item)
			//g_object_unref (d_redo_item -> d_widget);
		d_redo_item = item;
		if (d_redo_item) {
			//g_object_ref (d_redo_item -> d_widget);
			d_redo_item -> f_setSensitive (history_has_redo (this));
		}
	#elif cocoa
	#elif win
	#elif mac
	#endif
}

void structGuiText :: f_setSelection (long first, long last) {
	if (d_widget != NULL) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_select_region (GTK_EDITABLE (d_widget), first, last);
		} else if (G_OBJECT_TYPE (G_OBJECT (d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset (buffer, & from_it, first);
			gtk_text_buffer_get_iter_at_offset (buffer, & to_it, last);
			gtk_text_buffer_select_range (buffer, & from_it, & to_it);
		}
	#elif cocoa
		/*
		 * On Cocoa, characters are counted in UTF-16 units, whereas 'first' and 'last' are in UTF-32 units. Convert.
		 */
		wchar_t *text = f_getString ();
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		long length = wcslen (text);
		if (first >= length) first = length;
		if (last >= length) last = length;
		long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (long i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (long i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
		Melder_free (text);
		if (d_cocoaTextView) {
			[d_cocoaTextView setSelectedRange: NSMakeRange (first, last - first)];
		}
	#elif win
		/* 'first' and 'last' are the positions of the selection in the text when separated by LF alone. */
		/* We have to convert this to the positions that the selection has in a text separated by CR/LF sequences. */
		wchar_t *text = f_getString ();
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		long length = wcslen (text);
		if (first >= length) first = length;
		if (last >= length) last = length;
		long numberOfLeadingLineBreaks = 0, numberOfSelectedLineBreaks = 0;
		for (long i = 0; i < first; i ++) if (text [i] == '\n') numberOfLeadingLineBreaks ++;
		for (long i = first; i < last; i ++) if (text [i] == '\n') numberOfSelectedLineBreaks ++;
		first += numberOfLeadingLineBreaks;
		last += numberOfLeadingLineBreaks + numberOfSelectedLineBreaks;
		Melder_free (text);
		Edit_SetSel (d_widget -> window, first, last);
		UpdateWindow (d_widget -> window);
	#elif mac
		wchar_t *text = f_getString ();
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		long length = wcslen (text);
		if (first >= length) first = length;
		if (last >= length) last = length;
		long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (long i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (long i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
		Melder_free (text);
		if (isTextControl (d_widget)) {
			ControlEditTextSelectionRec rec = { first, last };
			SetControlData (d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec);
		} else if (isMLTE (this)) {
			TXNSetSelection (d_macMlteObject, first, last);
		}
	#endif
	}
}

void structGuiText :: f_setString (const wchar_t *text) {
	#if gtk
		if (G_OBJECT_TYPE (d_widget) == GTK_TYPE_ENTRY) {
			gtk_entry_set_text (GTK_ENTRY (d_widget), Melder_peekWcsToUtf8 (text));
		} else if (G_OBJECT_TYPE (d_widget) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (d_widget));
			gchar *textUtf8 = Melder_peekWcsToUtf8 (text);
			//gtk_text_buffer_set_text (textBuffer, textUtf8, strlen (textUtf8));   // length in bytes!
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gtk_text_buffer_delete_interactive (textBuffer, & start, & end, gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
			gtk_text_buffer_insert_interactive (textBuffer, & start, textUtf8, strlen (textUtf8), gtk_text_view_get_editable (GTK_TEXT_VIEW (d_widget)));
		}
	#elif cocoa
		trace ("title");
		if (d_cocoaTextView) {
			NSRange nsRange = NSMakeRange (0, [[d_cocoaTextView textStorage] length]);
			NSString *nsString = (NSString *) Melder_peekWcsToCfstring (text);
			[d_cocoaTextView shouldChangeTextInRange: nsRange replacementString: nsString];   // to make this action undoable
			//[[d_cocoaTextView textStorage] replaceCharactersInRange: nsRange withString: nsString];
			[d_cocoaTextView setString: nsString];
			//[[d_cocoaTextView window] setViewsNeedDisplay: YES];
			//[[d_cocoaTextView window] display];
		} else {
			[(NSTextField *) d_widget   setStringValue: (NSString *) Melder_peekWcsToCfstring (text)];
		}
	#elif win
		const wchar_t *from;
		wchar_t *winText = Melder_malloc_f (wchar_t, 2 * wcslen (text) + 1), *to;   /* All new lines plus one null byte. */
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != '\0'; from ++, to ++)
			if (*from == '\n') { *to = 13; * ++ to = '\n'; } else *to = *from;
		*to = '\0';
		SetWindowText (d_widget -> window, winText);
		Melder_free (winText);
		UpdateWindow (d_widget -> window);
	#elif mac
		long length_utf32 = wcslen (text), length_utf16 = wcslen_utf16 (text, false);
		UniChar *macText = Melder_malloc_f (UniChar, length_utf16 + 1);
		Melder_assert (d_widget -> widgetClass == xmTextWidgetClass);
		/*
		 * Convert from UTF-32 to UTF-16 and replace all LF with CR.
		 */
		long j = 0;
		for (long i = 0; i < length_utf32; i ++) {
			MelderUtf32 kar = text [i];
			if (kar == '\n') {   // LF
				macText [j ++] = 13;   // CR
			} else if (kar <= 0xFFFF) {
				macText [j ++] = kar;
			} else {
				Melder_assert (kar <= 0x10FFFF);
				kar -= 0x10000;
				macText [j ++] = 0xD800 | (kar >> 10);   // first UTF-16 surrogate character
				macText [j ++] = 0xDC00 | (kar & 0x3FF);   // second UTF-16 surrogate character
			}
		}
		macText [j] = '\0';
		if (j != length_utf16)
			Melder_fatal ("GuiText_setString: incorrect number of UTF-16 words (%ld instead of %ld): <<%ls>>.", j, length_utf16, text);
		if (isTextControl (d_widget)) {
			CFStringRef cfString = CFStringCreateWithCharacters (NULL, macText, length_utf16);
			SetControlData (d_widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString);
			CFRelease (cfString);
		} else if (isMLTE (this)) {
			TXNSetData (d_macMlteObject, kTXNUnicodeTextData, macText, length_utf16*2, 0, NativeText_getLength (d_widget));
		}
		Melder_free (macText);
		if (d_widget -> managed) {
			if (theGui.duringUpdate) {
				_Gui_invalidateWidget (d_widget);   /* HACK: necessary because VisRgn has temporarily been changed (not used in Praat any longer). */
			} else {
				if (isTextControl (d_widget)) {
					_GuiMac_clipOnParent (d_widget);
					Draw1Control (d_widget -> nat.control.handle);
					GuiMac_clipOff ();
				} else if (isMLTE (this)) {
				}
			}
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

void structGuiText :: f_setUndoItem (GuiMenuItem item) {
	#if gtk
		if (d_undo_item) {
			//g_object_unref (d_undo_item -> d_widget);
		}
		d_undo_item = item;
		if (d_undo_item) {
			//g_object_ref (d_undo_item -> d_widget);
			d_undo_item -> f_setSensitive (history_has_undo (this));
		}
	#elif cocoa
	#elif win
	#elif mac
	#endif
}

void structGuiText :: f_undo () {
	#if gtk
		history_do (this, 1);
	#elif cocoa
		if (d_cocoaTextView) {
			[[d_cocoaTextView undoManager] undo];
		}
	#elif win
	#elif mac
		if (isMLTE (this)) {
			TXNUndo (d_macMlteObject);
		}
		_GuiText_handleValueChanged (d_widget);
	#endif
}

/* End of file GuiText.cpp */
