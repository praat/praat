/* GuiText.c
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
 */

#include "GuiP.h"
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_text \
		Melder_assert (widget -> widgetClass == xmTextWidgetClass); \
		GuiText me = widget -> userData
#else
	#define iam_text \
		GuiText me = _GuiObject_getUserData (widget)
#endif

#if !mac
	#if gtk
		typedef gchar * history_data;
	#elif win | motif
		typedef char * history_data;
	#endif

	typedef struct _history_entry_s history_entry;
	struct _history_entry_s {
		history_entry *prev, *next;
		long first, last;
		history_data text;
		bool type_del : 1;
	};
#endif

typedef struct structGuiText {
	Widget widget;
	void (*changeCallback) (void *boss, GuiTextEvent event);
	void *changeBoss;
	#if mac
		TXNObject macMlteObject;
		TXNFrameID macMlteFrameId;
	#else
		history_entry *prev, *next;
		Widget undo_item, redo_item;
		bool history_change : 1;
	#endif
	#if win || mac
		bool editable;
	#endif
} *GuiText;

#ifndef UNIX

#if mac
	#define isTextControl(w)  ((w) -> isControl != 0)
	#define isMLTE(w)  ((w) -> macMlteObject != NULL)
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
 *  preference explicitly, see the discussion in FunctionEditor.c.)
 */

void _GuiText_handleFocusReception (Widget widget) {
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

void _GuiText_handleFocusLoss (Widget widget) {
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
		GuiText textFocus = theGui.textFocus -> userData;
		_GuiMac_clipOnParent (theGui.textFocus);
		if (isTextControl (theGui.textFocus)) {
			ClearKeyboardFocus (theGui.textFocus -> macWindow);
		} else if (isMLTE (textFocus)) {
			TXNFocus (textFocus -> macMlteObject, 0);
			TXNActivate (textFocus -> macMlteObject, textFocus -> macMlteFrameId, 0);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusLoss (theGui.textFocus);
	}
}
#endif

void _GuiText_setTheTextFocus (Widget widget) {
	if (widget == NULL || theGui.textFocus == widget
		|| ! widget -> managed) return;   /* Perhaps not-yet-managed. Test: open Praat's DataEditor with a Sound, then type. */
	#if gtk
		gtk_widget_grab_focus (widget);
	#elif win
		SetFocus (widget -> window);   /* Will send an EN_SETFOCUS notification, which will call _GuiText_handleFocusReception (). */
	#elif mac
		iam_text;
		_GuiMac_clearTheTextFocus ();
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			SetKeyboardFocus (widget -> macWindow, widget -> nat.control.handle, kControlEditTextPart);
		} else if (isMLTE (me)) {
			TXNActivate (my macMlteObject, my macMlteFrameId, 1);
			TXNFocus (my macMlteObject, 1);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusReception (widget);
	#endif
}

/*
 * CHANGE NOTIFICATION
 */
void _GuiText_handleValueChanged (Widget widget) {
	iam_text;
	if (my changeCallback) {
		struct structGuiTextEvent event = { widget };
		my changeCallback (my changeBoss, & event);
	}
}

/*
 * EVENT HANDLING
 */

#if mac
	int _GuiMacText_tryToHandleReturnKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, EventRecord *event) {
		if (widget && widget -> activateCallback) {
			widget -> activateCallback (widget, widget -> activateClosure, (XtPointer) event);
				return 1;
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleClipboardShortcut (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, unsigned char charCode, EventRecord *event) {
		if (widget) {
			iam_text;
			if (isTextControl (widget)) {
				if (charCode == 'X' || charCode == 'C' || charCode == 'V') {
					if (! my editable && (charCode == 'X' || charCode == 'V')) return 0;
					CallNextEventHandler (eventHandlerCallRef, eventRef);
					_GuiText_handleValueChanged (widget);
					return 1;
				}
			} else if (isMLTE (me)) {
				if (charCode == 'X' && my editable) {
					if (event -> what != autoKey) GuiText_cut (widget);
					return 1;
				}
				if (charCode == 'C') {
					if (event -> what != autoKey) GuiText_copy (widget);
					return 1;
				}
				if (charCode == 'V' && my editable) {
					GuiText_paste (widget);
					return 1;
				}
			}
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget widget, unsigned char keyCode, unsigned char charCode, EventRecord *event) {
		(void) keyCode;
		if (widget) {
			iam_text;
			if (my editable) {
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
	void _GuiMacText_handleClick (Widget widget, EventRecord *event) {
		iam_text;
		_GuiText_setTheTextFocus (widget);
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			HandleControlClick (widget -> nat.control.handle, event -> where, event -> modifiers, NULL);
		} else if (isMLTE (me)) {
			LocalToGlobal (& event -> where);
			TXNClick (my macMlteObject, event);   /* Handles text selection and scrolling. */
			GlobalToLocal (& event -> where);
		}
		GuiMac_clipOff ();
	}
#endif

/*
 * LAYOUT
 */
#if mac
	void _GuiMacText_move (Widget widget) {
		iam_text;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			MoveControl (widget -> nat.control.handle, widget -> rect.left + 3, widget -> rect.top + 3);
			_GuiMac_clipOffValid (widget);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my macMlteFrameId);
		}
	}
	void _GuiMacText_shellResize (Widget widget) {
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
			TXNSetFrameBounds (my macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my macMlteFrameId);
		}
	}
	void _GuiMacText_resize (Widget widget) {
		iam_text;
		if (isTextControl (widget)) {
			SizeControl (widget -> nat.control.handle, widget -> width - 6, widget -> height - 6);
			/*
			 * Container widgets will have been invalidated.
			 * So in order not to make the control flash, we validate it.
			 */
			_Gui_validateWidget (widget);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my macMlteObject, widget -> rect. top, widget -> rect. left,
				widget -> rect. bottom, widget -> rect. right, my macMlteFrameId);
		}
	}
#endif

void _GuiText_unmanage (Widget widget) {
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

#if mac
	void _GuiMacText_update (Widget widget) {
		iam_text;
		_GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			Draw1Control (widget -> nat.control.handle);
		} else if (isMLTE (me)) {
			TXNDraw (my macMlteObject, NULL);
		}
		GuiMac_clipOff ();
	}
#endif

void _GuiWinMacText_destroy (Widget widget) {
	if (widget == theGui.textFocus)
		theGui.textFocus = NULL;   // remove dangling reference
	if (widget == widget -> shell -> textFocus)
		widget -> shell -> textFocus = NULL;   // remove dangling reference
	iam_text;
	#if win
		DestroyWindow (widget -> window);
	#elif mac
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			DisposeControl (widget -> nat.control.handle);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNDeleteObject (my macMlteObject);
		}
	#endif
	Melder_free (me);   // NOTE: my widget is not destroyed here
}

void _GuiWinMacText_map (Widget widget) {
	iam_text;
	#if win
		ShowWindow (widget -> window, SW_SHOW);
	#elif mac
		if (isTextControl (widget)) {
			_GuiNativeControl_show (widget);
		} else if (isMLTE (me)) {
		}
	#endif
}
#endif


#if mac || win

static long NativeText_getLength (Widget widget) {
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
				return TXNDataSize (my macMlteObject) / sizeof (UniChar);
			#else
				long length = 0, dataSize = TXNDataSize (my macMlteObject);
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my macMlteObject, 0, dataSize, & numberOfRuns);
				for (long irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my macMlteObject, left, right, & han, kTXNUnicodeTextData);
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

static void NativeText_getText (Widget widget, wchar_t *buffer, long length) {
	#if win
		GetWindowText (widget -> window, buffer, length + 1);
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			CFStringRef cfString;
			GetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, NULL);
			UniChar *macText = Melder_malloc (UniChar, length + 1);
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
				TXNGetDataEncoded (my macMlteObject, 0, length, & han, kTXNUnicodeTextData);
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
				long dataSize = TXNDataSize (my macMlteObject);
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my macMlteObject, 0, dataSize, & numberOfRuns);
				for (long irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my macMlteObject, left, right, & han, kTXNUnicodeTextData);
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

static int NativeText_getSelectionRange (Widget widget, long *out_left, long *out_right) {
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
			TXNGetSelection (my macMlteObject, & left, & right);
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

	static void _GuiText_delete(Widget widget, int from_pos, int to_pos) {
		#if gtk
			if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
				gtk_editable_delete_text (GTK_EDITABLE (widget), from_pos, to_pos);
			} else {
				GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
				GtkTextIter from_it, to_it;
				gtk_text_buffer_get_iter_at_offset(buffer, &from_it, from_pos);
				gtk_text_buffer_get_iter_at_offset(buffer, &to_it, to_pos);
				gtk_text_buffer_delete_interactive(buffer, &from_it, &to_it,
					gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
				gtk_text_buffer_place_cursor(buffer, &to_it);
			}
		#elif win
		#elif motif
			XmTextReplace(widget, from_pos, to_pos, "");
		#endif
	}

	static void _GuiText_insert(Widget widget, int from_pos, int to_pos, const history_data text) {
		#if gtk
			if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
				gint from_pos_gint = from_pos;
				gtk_editable_insert_text (GTK_EDITABLE (widget), text, to_pos - from_pos, &from_pos_gint);
			} else {
				GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
				GtkTextIter it;
				gtk_text_buffer_get_iter_at_offset(buffer, &it, from_pos);
				gtk_text_buffer_insert_interactive(buffer, &it, text, to_pos - from_pos,
					gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
				gtk_text_buffer_get_iter_at_offset(buffer, &it, to_pos);
				gtk_text_buffer_place_cursor(buffer, &it);
			}
		#elif win
		#elif motif
			XmTextInsert(widget, from_pos, text);
		#endif
	}

	/* Tests the previous elements of the history for mergability with the one to insert given via parameters.
	 * If successful, it returns a pointer to the last valid entry in the merged history list, otherwise
	 * returns NULL.
	 * Specifically the function expands the previous insert/delete event(s)
	 *  - with the current, if current also is an insert/delete event and the ranges of previous and current event match
	 *  - with the previous delete and current insert event, in case the ranges of both event-pairs respectively match
	 */
	static history_entry * history_addAndMerge(void *void_me, history_data text_new, long first, long last, bool deleted) {
		iam(GuiText);
		history_entry *he = NULL;
		
		if (!(my prev))
			return NULL;
		
		if (my prev->type_del == deleted) {
			// extend the last history event by this one
			if (my prev->first == last) {
				// most common for backspace key presses
				he = my prev;
				text_new = realloc(text_new, sizeof(*text_new) * (he->last - first + 1));
				memcpy(text_new + last - first, he->text, sizeof(*text_new) * (he->last - he->first + 1));
				free(he->text);
				he->text = text_new;
				he->first = first;
				
			} else if (my prev->last == first) {
				// most common for ordinary text insertion
				he = my prev;
				he->text = realloc(he->text, sizeof(*he->text) * (last - he->first + 1));
				memcpy(he->text + he->last - he->first, text_new, sizeof(*he->text) * (last - first + 1));
				free(text_new);
				he->last = last;
				
			} else if (deleted && my prev->first == first) {
				// most common for delete key presses
				he = my prev;
				he->text = realloc(he->text, sizeof(*he->text) * (last - first + he->last - he->first + 1));
				memcpy(he->text + he->last - he->first, text_new, sizeof(*he->text) * (last - first + 1));
				free(text_new);
				he->last = last + he->last - he->first;
			}
		} else {
			// prev->type_del != deleted, no simple expansion possible, check for double expansion
			if (!deleted && my prev->prev && my prev->prev->prev) {
				history_entry *del_one = my prev;
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
					del_mult->text = realloc(del_mult->text, sizeof(*del_mult->text) * (to3 - from1 + 1));
					ins_mult->text = realloc(ins_mult->text, sizeof(*ins_mult->text) * (to3 - from1 + 1));
					memcpy(del_mult->text + to1 - from1, del_one->text, sizeof(*del_mult->text) * (to3 - to1 + 1));
					memcpy(ins_mult->text + to1 - from1, text_new     , sizeof(*del_mult->text) * (to3 - to1 + 1));
					del_mult->last = to3;
					ins_mult->last = to3;
					free(del_one->text);
					free(del_one);
					free(text_new);
					my prev = he = ins_mult;
				}
			}
		}
		
		return he;
	}

	/* Inserts a new history action, thereby removing any remaining 'redo' steps;
	 *   text_new  a newly allocated string that will be freed by a history function
	 *             (history_add or history_clear)
	 */
	static void history_add(void *void_me, history_data text_new, long first, long last, bool deleted) {
		iam(GuiText);
		
		// delete all newer entries; from here on there is no 'Redo' until the next 'Undo' is performed
		history_entry *old_hnext = my next, *hnext;
		while (old_hnext) {
			hnext = old_hnext->next;
			free(old_hnext->text);
			free(old_hnext);
			old_hnext = hnext;
		}
		my next = NULL;
		
		history_entry *he = history_addAndMerge(void_me, text_new, first, last, deleted);
		if (he == NULL) {
			he = malloc(sizeof(history_entry));
			he->first = first;
			he->last = last;
			he->type_del = deleted;
			he->text = text_new;
			
			he->prev = my prev;
			he->next = NULL;
			if (my prev)
				my prev->next = he;
		}
		my prev = he;
		he->next = NULL;
		
		if (my undo_item) GuiObject_setSensitive(my undo_item, TRUE);
		if (my redo_item) GuiObject_setSensitive(my redo_item, FALSE);
	}

	static bool history_has_undo(void *void_me) {
		iam(GuiText);
		return my prev != NULL;
	}

	static bool history_has_redo(void *void_me) {
		iam(GuiText);
		return my next != NULL;
	}

	static void history_do(void *void_me, bool undo) {
		iam(GuiText);
		history_entry *he = undo ? my prev : my next;
		if (he == NULL) // TODO: this function should not be called in that case
			return;
		
		my history_change = 1;
		if (undo ^ he->type_del) {
			_GuiText_delete(my widget, he->first, he->last);
		} else {
			_GuiText_insert(my widget, he->first, he->last, he->text);
		}
		my history_change = 0;
		
		if (undo) {
			my next = my prev;
			my prev = my prev->prev;
		} else {
			my prev = my next;
			my next = my next->next;
		}
		
		if (my undo_item) GuiObject_setSensitive(my undo_item, history_has_undo(me));
		if (my redo_item) GuiObject_setSensitive(my redo_item, history_has_redo(me));
	}

	static void history_clear(void *void_me) {
		iam(GuiText);
		history_entry *h1, *h2;
		
		h1 = my prev;
		while (h1) {
			h2 = h1->prev;
			free(h1->text);
			free(h1);
			h1 = h2;
		}
		my prev = NULL;
		
		h1 = my next;
		while (h1) {
			h2 = h1->next;
			free(h1->text);
			free(h1);
			h1 = h2;
		}
		my next = NULL;
		
		if (my undo_item) GuiObject_setSensitive(my undo_item, FALSE);
		if (my redo_item) GuiObject_setSensitive(my redo_item, FALSE);
	}
#endif

/*
 * CALLBACKS
 */

#if gtk
	static void _GuiGtkEntry_history_delete_cb(GtkEditable *ed, gint from, gint to, gpointer void_me) {
		iam(GuiText);
		if (my history_change) return;
		
		history_add(me, gtk_editable_get_chars(GTK_EDITABLE(ed), from, to), from, to, 1);
	}
	
	static void _GuiGtkEntry_history_insert_cb(GtkEditable *ed, gchar *utf8_text, gint len, gint *from, gpointer void_me) {
		(void)ed;
		iam(GuiText);
		if (my history_change) return;
		
		gchar *text = malloc(sizeof(gchar) * (len + 1));
		strcpy(text, utf8_text);
		history_add(me, text, *from, *from + len, 0);
	}
	
	static void _GuiGtkTextBuf_history_delete_cb(GtkTextBuffer *buffer, GtkTextIter *from, GtkTextIter *to, gpointer void_me) {
		iam(GuiText);
		if (my history_change) return;
		
		int from_pos = gtk_text_iter_get_offset(from);
		int to_pos = gtk_text_iter_get_offset(to);
		history_add(me, gtk_text_buffer_get_text(buffer, from, to, FALSE), from_pos, to_pos, 1);
	}
	
	static void _GuiGtkTextBuf_history_insert_cb(GtkTextBuffer *buffer, GtkTextIter *from, gchar *utf8_text, gint len, gpointer void_me) {
		(void)buffer;
		iam(GuiText);
		if (my history_change) return;
		
		int from_pos = gtk_text_iter_get_offset(from);
		gchar *text = malloc(sizeof(gchar) * (len + 1));
		strcpy(text, utf8_text);
		history_add(me, text, from_pos, from_pos + len, 0);
	}
	
	static void _GuiGtkEntry_valueChangedCallback (Widget widget, gpointer void_me) {
		// TODO: ugh!
		iam (GuiText);
		Melder_assert (me != NULL);
		if (my changeCallback != NULL) {
			struct structGuiTextEvent event = { widget };
			my changeCallback (my changeBoss, & event);
		}
	}
	
	static void _GuiGtkEntry_destroyCallback(Widget widget, gpointer void_me) {
		(void)widget;
		iam(GuiText);
		if (my undo_item) g_object_unref(my undo_item);
		if (my redo_item) g_object_unref(my redo_item);
		my undo_item = NULL;
		my redo_item = NULL;
		history_clear(me);
		Melder_free(me);
	}
#elif win
#elif mac
#elif motif
	static void _GuiMotifText_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiText);
		Melder_free (me);
	}
	static void _GuiMotifText_valueChangedCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) call;
		iam (GuiText);
		Melder_assert (me != NULL);
		if (my changeCallback != NULL) {
			struct structGuiTextEvent event = { widget };
			my changeCallback (my changeBoss, & event);
		}
	}
#endif

Widget GuiText_create (Widget parent, int left, int right, int top, int bottom, unsigned long flags) {
	GuiText me = Melder_calloc (struct structGuiText, 1);
	#if gtk
		if (flags & GuiText_SCROLLED) {
			GtkWrapMode ww;
			my widget = gtk_text_view_new ();
			gtk_text_view_set_editable (GTK_TEXT_VIEW (my widget), (flags & GuiText_NONEDITABLE) == 0);
			if ((flags & GuiText_WORDWRAP) != 0) 
				ww = GTK_WRAP_WORD_CHAR;
			else
				ww = GTK_WRAP_NONE;
			gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (my widget), ww);
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(my widget));
			g_signal_connect(G_OBJECT(buffer), "delete-range", G_CALLBACK(_GuiGtkTextBuf_history_delete_cb), me);
			g_signal_connect(G_OBJECT(buffer), "insert-text", G_CALLBACK(_GuiGtkTextBuf_history_insert_cb), me);
		} else {
			my widget = gtk_entry_new ();
			gtk_editable_set_editable (GTK_EDITABLE (my widget), (flags & GuiText_NONEDITABLE) == 0);
			g_signal_connect(G_OBJECT(my widget), "delete-text", G_CALLBACK(_GuiGtkEntry_history_delete_cb), me);
			g_signal_connect(G_OBJECT(my widget), "insert-text", G_CALLBACK(_GuiGtkEntry_history_insert_cb), me);
		}
		_GuiObject_setUserData (my widget, me);
		_GuiObject_position (my widget, left, right, top, bottom);
		
		my prev = NULL;
		my next = NULL;
		my history_change = 0;
		my undo_item = NULL;
		my redo_item = NULL;
		
		if (parent)
			gtk_container_add (GTK_CONTAINER (parent), my widget);
		g_signal_connect (G_OBJECT (my widget), "destroy",
			G_CALLBACK (_GuiGtkEntry_destroyCallback), me);
//		g_signal_connect (GTK_EDITABLE (my widget), "changed",
//			G_CALLBACK (_GuiGtkEntry_valueChangedCallback), me);
		// TODO: First input focus verhaal? *check*
	#elif win
		my widget = _Gui_initializeWidget (xmTextWidgetClass, parent, flags & GuiText_SCROLLED ? L"scrolledText" : L"text");
		_GuiObject_setUserData (my widget, me);
		my editable = (flags & GuiText_NONEDITABLE) == 0;
		my widget -> window = CreateWindow (L"edit", NULL, WS_CHILD | WS_BORDER
			| ( flags & GuiText_WORDWRAP ? ES_AUTOVSCROLL : ES_AUTOHSCROLL )
			| ES_MULTILINE | WS_CLIPSIBLINGS
			| ( flags & GuiText_SCROLLED ? WS_HSCROLL | WS_VSCROLL : 0 ),
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		static HFONT font;
		if (! font) font = CreateFont (16, 0, 0, 0, 0, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0/*FIXED_PITCH | FF_MODERN*/, /*L"Doulos SIL"*/L"Courier New");
		SetWindowFont (my widget -> window, font /*theScrolledHint ? font : GetStockFont (ANSI_VAR_FONT)*/, FALSE);
		Edit_LimitText (my widget -> window, 0);
		_GuiObject_position (my widget, left, right, top, bottom);
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my widget -> shell -> textFocus) {
			my widget -> shell -> textFocus = my widget;   /* Even if not-yet-managed. But in that case it will not receive global focus. */
		}
	#elif mac
		if (flags & GuiText_SCROLLED) {
			my widget = _Gui_initializeWidget (xmTextWidgetClass, parent, L"scrolledText");
			_GuiObject_setUserData (my widget, me);
			my editable = (flags & GuiText_NONEDITABLE) == 0;
			TXNLongRect destRect;
			TXNMargins margins;
			TXNControlData controlData;
			TXNControlTag controlTag = kTXNMarginsTag;
			TXNNewObject (NULL,   /* No file. */
				my widget -> macWindow, & my widget -> rect, kTXNWantHScrollBarMask | kTXNWantVScrollBarMask
					| kTXNMonostyledTextMask | kTXNDrawGrowIconMask,
				kTXNTextEditStyleFrameType, kTXNTextensionFile,
				/*kTXNMacOSEncoding*/ kTXNSystemDefaultEncoding, & my macMlteObject, & my macMlteFrameId, me);
			destRect. left = 0;
			destRect. top = 0;
			destRect. right = 10000;
			destRect. bottom = 2000000000;
			TXNSetRectBounds (my macMlteObject, NULL, & destRect, FALSE);
			margins. leftMargin = 3;
			margins. topMargin = 3;
			margins. rightMargin = 0;
			margins. bottomMargin = 0;
			controlData. marginsPtr = & margins;
			TXNSetTXNObjectControls (my macMlteObject, FALSE, 1, & controlTag, & controlData);
			_GuiObject_position (my widget, left, right, top, bottom);
		} else {
			my widget = _Gui_initializeWidget (xmTextWidgetClass, parent, L"text");
			_GuiObject_setUserData (my widget, me);
			my editable = (flags & GuiText_NONEDITABLE) == 0;
			Rect r = my widget -> rect;
			InsetRect (& r, 3, 3);
			CreateEditUnicodeTextControl (my widget -> macWindow, & r, NULL, false, NULL, & my widget -> nat.control.handle);
			SetControlReference (my widget -> nat.control.handle, (long) my widget);
			my widget -> isControl = TRUE;
			_GuiObject_position (my widget, left, right, top, bottom);
		}
		/*
		 * The first created text widget shall attract the input focus.
		 */
		if (! my widget -> shell -> textFocus) {
			my widget -> shell -> textFocus = my widget;   /* Even if not-yet-managed. But in that case it will not receive global focus. */
		}
	#elif motif
		if (flags & GuiText_SCROLLED) {
			Arg arg [4];
			arg [0]. name = XmNscrollingPolicy; arg [0]. value = XmAUTOMATIC;
			arg [1]. name = XmNscrollBarDisplayPolicy; arg [1]. value = XmAS_NEEDED;
			arg [2]. name = XmNeditable; arg [2]. value = (flags & GuiText_NONEDITABLE) == 0;
			arg [3]. name = XmNeditMode; arg [3]. value = XmMULTI_LINE_EDIT;   /* On Linux, this must go before creation. */
			my widget = XmCreateScrolledText (parent, "scrolledText", arg, 4);
			_GuiObject_setUserData (my widget, me);
			_GuiObject_position (XtParent (my widget), left, right, top, bottom);
			XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifText_destroyCallback, me);
			XtVaSetValues (my widget,
				XmNeditable, (flags & GuiText_NONEDITABLE) == 0,
				XmNeditMode, XmMULTI_LINE_EDIT,   // "results of ScrolledWindow when XmNeditMode is XmSINGLE_LINE_EDIT are undefined"
			#ifndef macintosh
				XmNrows, 33, XmNcolumns, 90,
			#endif
				NULL);
		} else {
			my widget = XmCreateText (parent, "text", NULL, 0);
			_GuiObject_setUserData (my widget, me);
			XtVaSetValues (my widget,
				XmNeditable, (flags & GuiText_NONEDITABLE) == 0,
				XmNeditMode, ( flags & GuiText_MULTILINE ? XmMULTI_LINE_EDIT : XmSINGLE_LINE_EDIT ),
				XmNwordWrap, (flags & GuiText_WORDWRAP) != 0,   // "ignored if XmNeditMode is XmSINGLE_LINE_EDIT"
				NULL);
			_GuiObject_position (my widget, left, right, top, bottom);
		}
		XtAddCallback (my widget, XmNvalueChangedCallback, _GuiMotifText_valueChangedCallback, me);
		// TODO: for history functions, a XmNmodifyVerifyCallback needs to be added, which calls history_add()
		#endif
	
	return my widget;
}

Widget GuiText_createShown (Widget parent, int left, int right, int top, int bottom, unsigned long flags) {
	Widget me = GuiText_create (parent, left, right, top, bottom, flags);
	GuiObject_show (me);
	return me;
}

void GuiText_copy (Widget widget) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_copy_clipboard (GTK_EDITABLE (widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			GtkClipboard *cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_copy_clipboard(buffer, cb);
		}
	#elif win
		if (! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		SendMessage (widget -> window, WM_COPY, 0, 0);
	#elif mac
		iam_text;
		if (! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		if (isTextControl (widget)) {
			HandleControlKey (widget -> nat.control.handle, 0, 'C', cmdKey);
		} else if (isMLTE (me)) {
			TXNCopy (my macMlteObject);
		}
	#elif motif
		XmTextCopy (widget, 0);   // BUG: time
	#endif
}

void GuiText_cut (Widget widget) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_cut_clipboard (GTK_EDITABLE (widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			GtkClipboard *cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_cut_clipboard(buffer, cb, gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
		}
	#elif win
		iam_text;
		if (! my editable || ! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		SendMessage (widget -> window, WM_CUT, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		iam_text;
		if (! my editable || ! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			HandleControlKey (widget -> nat.control.handle, 0, 'X', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNCut (my macMlteObject);
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
		XmTextCut (widget, 0);   // BUG: time
	#endif
}

wchar_t * GuiText_getSelection (Widget widget) {
	#if gtk
		// first = gtk_text_iter_get_offset (& start);
		// last = gtk_text_iter_get_offset (& end);
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gint start, end;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (widget), & start, & end); 
			if (end <= start) return NULL;
			return Melder_utf8ToWcs (gtk_editable_get_chars (GTK_EDITABLE (widget), start, end));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
			if (gtk_text_buffer_get_has_selection (textBuffer)) {
				GtkTextIter start, end;
				gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
				gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, TRUE);
				wchar_t *temp =  Melder_utf8ToWcs (text);
				g_free (text);
				return temp;
			}
		}
		return NULL;
	#elif win || mac
		long length, start, end;
		wchar_t *result;
		NativeText_getSelectionRange (widget, & start, & end);
		if (end <= start) return NULL;
		/*
		 * Get all text.
		 */
		length = NativeText_getLength (widget);
		result = Melder_malloc (wchar_t, length + 1);
		NativeText_getText (widget, result, length);
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
		result = Melder_realloc (result, (length + 1) * sizeof (wchar_t));   /* Optional. */
		Melder_killReturns_inlineW (result);   /* AFTER zooming! */
		return result;
	#elif motif
		char *selectionUtf8 = XmTextGetSelection (widget);
		wchar_t *selection = Melder_8bitToWcs (selectionUtf8, kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1);
		XtFree (selectionUtf8);
		return selection;
	#endif
}

wchar_t * GuiText_getString (Widget widget) {
	long first, last;
	return GuiText_getStringAndSelectionPosition (widget, & first, & last);
}

wchar_t * GuiText_getStringAndSelectionPosition (Widget widget, long *first, long *last) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gint first_gint, last_gint;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (widget), & first_gint, & last_gint);
			*first = first_gint;
			*last = last_gint;
			return Melder_utf8ToWcs (gtk_entry_get_text (GTK_ENTRY (widget)));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, TRUE); // TODO: Hidden chars ook maar doen he?
			wchar_t *temp = Melder_utf8ToWcs (text);
			g_free (text);
			if (gtk_text_buffer_get_has_selection (textBuffer)) {
				gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
				*first = gtk_text_iter_get_offset (& start);
				*last = gtk_text_iter_get_offset (& end);
			}
			return temp;
		}
		return NULL;
	#elif win
		long length = NativeText_getLength (widget);
		wchar_t *result = Melder_malloc (wchar_t, length + 1);
		NativeText_getText (widget, result, length);
		NativeText_getSelectionRange (widget, first, last);
		long numberOfLeadingLineBreaks = 0, numberOfSelectedLineBreaks = 0;
		for (long i = 0; i < *first; i ++) if (result [i] == 13) numberOfLeadingLineBreaks ++;
		for (long i = *first; i < *last; i ++) if (result [i] == 13) numberOfSelectedLineBreaks ++;
		*first -= numberOfLeadingLineBreaks;
		*last -= numberOfLeadingLineBreaks + numberOfSelectedLineBreaks;
		Melder_killReturns_inlineW (result);
		return result;
	#elif mac
		long length = NativeText_getLength (widget);   // UTF-16 length; should be enough for UTF-32 buffer
		wchar_t *result = Melder_malloc (wchar_t, length + 1);
		NativeText_getText (widget, result, length);
		NativeText_getSelectionRange (widget, first, last);   // 'first' and 'last' are expressed in UTF-16 words
		for (long i = 0; i < *first; i ++) if (result [i] > 0xFFFF) { (*first) --; (*last) --; }
		for (long i = *first; i < *last; i ++) if (result [i] > 0xFFFF) { (*last) --; }
		Melder_killReturns_inlineW (result);
		return result;
	#elif motif
		char *textUtf8 = XmTextGetString (widget);
		wchar_t *result = Melder_8bitToWcs (textUtf8, kMelder_textInputEncoding_UTF8_THEN_ISO_LATIN1);
		XtFree (textUtf8);
		XmTextPosition first_motif, last_motif;
		if (! XmTextGetSelectionPosition (widget, & first_motif, & last_motif))
			first_motif = last_motif = XmTextGetInsertionPosition (widget);
		*first = first_motif;
		*last = last_motif;
		return result;
	#endif
}

void GuiText_paste (Widget widget) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_paste_clipboard (GTK_EDITABLE (widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			GtkClipboard *cb = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
			gtk_text_buffer_paste_clipboard(buffer, cb, NULL, gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
		}
	#elif win
		iam_text;
		if (! my editable) return;
		SendMessage (widget -> window, WM_PASTE, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		iam_text;
		if (! my editable) return;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			HandleControlKey (widget -> nat.control.handle, 0, 'V', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNPaste (my macMlteObject);
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
		XmTextPaste (widget);
	#endif
}

void GuiText_redo (Widget widget) {
	#if mac
		iam_text;
		if (isMLTE (me)) {
			TXNRedo (my macMlteObject);
		}
		_GuiText_handleValueChanged (widget);
	#else
		iam_text;
		history_do(me, 0);
	#endif
}

void GuiText_remove (Widget widget) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_selection (GTK_EDITABLE (widget));
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			gtk_text_buffer_delete_selection(buffer, TRUE, gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
		}
	#elif win
		iam_text;
		if (! my editable || ! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		SendMessage (widget -> window, WM_CLEAR, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		iam_text;
		if (! my editable || ! NativeText_getSelectionRange (widget, NULL, NULL)) return;
		if (isTextControl (widget)) {
			_GuiMac_clipOnParent (widget);
			HandleControlKey (widget -> nat.control.handle, 0, 8, 0);   /* Backspace key. */
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNClear (my macMlteObject);
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
		XmTextRemove (widget);
	#endif
}

void GuiText_replace (Widget widget, long from_pos, long to_pos, const wchar_t *text) {
	#if gtk
		gchar *new = Melder_peekWcsToUtf8 (text);
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_delete_text (GTK_EDITABLE (widget), from_pos, to_pos);
			gint from_pos_gint = from_pos;
			gtk_editable_insert_text (GTK_EDITABLE (widget), new, g_utf8_strlen (new, -1), & from_pos_gint);
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset(buffer, &from_it, from_pos);
			gtk_text_buffer_get_iter_at_offset(buffer, &to_it, to_pos);
			gtk_text_buffer_delete_interactive(buffer, &from_it, &to_it,
				gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
			gtk_text_buffer_insert_interactive(buffer, &from_it, new, g_utf8_strlen (new, -1),
				gtk_text_view_get_editable(GTK_TEXT_VIEW(widget)));
		}
		// TODO: Wat is dit lelijk... en fout was het ook nog!
	#elif win
		const wchar_t *from;
		wchar_t *winText = Melder_malloc (wchar_t, 2 * wcslen (text) + 1), *to;   /* All new lines plus one null byte. */
		if (! winText) return;
		Melder_assert (MEMBER (widget, Text));
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
		GuiText_setSelection (widget, from_pos, to_pos);
		Edit_ReplaceSel (widget -> window, winText);
		Melder_free (winText);
	#elif mac
		iam_text;
		long length = wcslen (text), i;
		wchar_t *macText = Melder_malloc (wchar_t, length + 1);
		Melder_assert (widget -> widgetClass == xmTextWidgetClass);
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
		if (my widget -> managed) _GuiMac_clipOnParent (widget);
		if (isTextControl (widget)) {
			// BUG: this is not UTF-32-savvy; this is acceptable because it isn't used in Praat
			long oldLength = NativeText_getLength (widget);
			wchar_t *totalText = Melder_malloc (wchar_t, oldLength - (to_pos - from_pos) + length + 1);
			wchar_t *oldText = Melder_malloc (wchar_t, oldLength + 1);
			NativeText_getText (widget, oldText, oldLength);
			wcsncpy (totalText, oldText, from_pos);
			wcscpy (totalText + from_pos, macText);
			wcscpy (totalText + from_pos + length, oldText + to_pos);
			CFStringRef totalText_cfstring = Melder_peekWcsToCfstring (totalText);
			SetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & totalText_cfstring);
			Melder_free (oldText);
			Melder_free (totalText);
		} else if (isMLTE (me)) {
			long oldLength = NativeText_getLength (widget);
			wchar_t *oldText = Melder_malloc (wchar_t, oldLength + 1);
			NativeText_getText (widget, oldText, oldLength);
			long numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			for (long i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
			for (long i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			const UniChar *macText_utf16 = Melder_peekWcsToUtf16 (macText);
			TXNSetData (my macMlteObject, kTXNUnicodeTextData, macText_utf16, wcslen_utf16 (macText, 0) * 2, from_pos, to_pos);
		}
		Melder_free (macText);
		if (widget -> managed) {
			if (isTextControl (widget)) {
				Draw1Control (widget -> nat.control.handle);
			} else if (isMLTE (me)) {
			}
			GuiMac_clipOff ();
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
		XmTextReplace (widget, from_pos, to_pos, Melder_peekWcsToUtf8 (text));
	#endif
}

void GuiText_scrollToSelection (Widget widget) {
	#if gtk
		GtkTextBuffer *textBuffer;
		GtkTextIter start, end;
		textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
		gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
		gtk_text_view_scroll_to_iter (GTK_TEXT_VIEW (widget), & start, 0.0, TRUE, 0.0, 0.0); 
	#elif win
		Edit_ScrollCaret (widget -> window);
	#elif mac
		iam_text;
		if (isTextControl (widget)) {
			;
		} else if (isMLTE (me)) {
			TXNShowSelection (my macMlteObject, false);
		}
	#elif motif
		XmTextPosition left, right;
		if (! XmTextGetSelectionPosition (widget, & left, & right))
			left = right = XmTextGetInsertionPosition (widget);
		XmTextShowPosition (widget, left);
		(void) right;
	#endif
}

void GuiText_setChangeCallback (Widget widget, void (*changeCallback) (void *boss, GuiTextEvent event), void *changeBoss) {
	iam_text;
	my changeCallback = changeCallback;
	my changeBoss = changeBoss;
}

void GuiText_setFontSize (Widget widget, int size) {
	#if gtk
		GtkRcStyle *modStyle = gtk_widget_get_modifier_style(widget);
		PangoFontDescription *fontDesc = (modStyle->font_desc != NULL) ? modStyle->font_desc
			: pango_font_description_copy(widget->style->font_desc);
		pango_font_description_set_absolute_size(fontDesc, size * PANGO_SCALE);
		modStyle->font_desc = fontDesc;
		gtk_widget_modify_style(widget, modStyle);
	#elif win
	#elif mac
		iam_text;
		if (isMLTE (me)) {
			TXNTypeAttributes attr;
			attr. tag = kTXNQDFontSizeAttribute;
			attr. size = kTXNFontSizeAttributeSize;
			attr. data. dataValue = (unsigned long) size << 16;
			TXNSetTypeAttributes (my macMlteObject, 1, & attr, 0, 2000000000);
		}
	#elif motif
	#endif
}

void GuiText_setRedoItem(Widget widget, Widget item) {
	#if gtk
		iam_text;
		if (my redo_item)
			g_object_unref(my redo_item);
		my redo_item = item;
		if (my redo_item) {
			g_object_ref(my redo_item);
			GuiObject_setSensitive(my redo_item, history_has_redo(me));
		}
	#elif win
	#elif mac
	#elif motif
	#endif
}

void GuiText_setSelection (Widget widget, long first, long last) {
	if (widget != NULL) {
	#if gtk
		if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
			gtk_editable_select_region (GTK_EDITABLE (widget), first, last);
		} else if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(widget));
			GtkTextIter from_it, to_it;
			gtk_text_buffer_get_iter_at_offset(buffer, &from_it, first);
			gtk_text_buffer_get_iter_at_offset(buffer, &to_it, last);
			gtk_text_buffer_select_range(buffer, &from_it, &to_it);
		}
	#elif win
		/* 'first' and 'last' are the positions of the selection in the text when separated by LF alone. */
		/* We have to convert this to the positions that the selection has in a text separated by CR/LF sequences. */
		wchar_t *text = GuiText_getString (widget);
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
		Edit_SetSel (widget -> window, first, last);
	#elif mac
		iam_text;
		wchar_t *text = GuiText_getString (widget);
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
		if (isTextControl (widget)) {
			ControlEditTextSelectionRec rec = { first, last };
			SetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec);
		} else if (isMLTE (me)) {
			TXNSetSelection (my macMlteObject, first, last);
		}
	#elif motif
		XmTextSetSelection (widget, first, last, 0);   // BUG: should have a real time, not 0
		if (first == last)
			XmTextSetInsertionPosition (widget, first);
	#endif
	}
}

void GuiText_setString (Widget widget, const wchar_t *text) {
	#if gtk
		if (G_OBJECT_TYPE (widget) == GTK_TYPE_ENTRY) {
			gtk_entry_set_text (GTK_ENTRY (widget), Melder_peekWcsToUtf8 (text));
		} else if (G_OBJECT_TYPE (widget) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
			gchar *new = Melder_peekWcsToUtf8 (text);
			gtk_text_buffer_set_text (textBuffer, new, strlen (new));   // length in bytes!
		}
	#elif win
		const wchar_t *from;
		wchar_t *winText = Melder_malloc (wchar_t, 2 * wcslen (text) + 1), *to;   /* All new lines plus one null byte. */
		if (! winText) return;
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != '\0'; from ++, to ++)
			if (*from == '\n') { *to = 13; * ++ to = '\n'; } else *to = *from;
		*to = '\0';
		SetWindowText (widget -> window, winText);
		Melder_free (winText);
	#elif mac
		iam_text;
		long length_utf32 = wcslen (text), length_utf16 = wcslen_utf16 (text, false);
		UniChar *macText = Melder_malloc (UniChar, length_utf16 + 1);
		Melder_assert (widget -> widgetClass == xmTextWidgetClass);
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
		if (isTextControl (widget)) {
			CFStringRef cfString = CFStringCreateWithCharacters (NULL, macText, length_utf16);
			SetControlData (widget -> nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString);
			CFRelease (cfString);
		} else if (isMLTE (me)) {
			TXNSetData (my macMlteObject, kTXNUnicodeTextData, macText, length_utf16*2, 0, NativeText_getLength (widget));
		}
		Melder_free (macText);
		if (widget -> managed) {
			if (theGui.duringUpdate) {
				_Gui_invalidateWidget (widget);   /* HACK: necessary because VisRgn has temporarily been changed (not used in Praat any longer). */
			} else {
				if (isTextControl (widget)) {
					_GuiMac_clipOnParent (widget);
					Draw1Control (widget -> nat.control.handle);
					GuiMac_clipOff ();
				} else if (isMLTE (me)) {
				}
			}
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
		XmTextSetString (widget, Melder_peekWcsToUtf8 (text));
		/*
		 * At some point perhaps to be replaced with:
		 */
		//XmTextSetStringWcs (me, text);
	#endif
}

void GuiText_setUndoItem(Widget widget, Widget item) {
	#if gtk
		iam_text;
		if (my undo_item)
			g_object_unref(my undo_item);
		my undo_item = item;
		if (my undo_item) {
			g_object_ref(my undo_item);
			GuiObject_setSensitive(my undo_item, history_has_undo(me));
		}
	#elif win
	#elif mac
	#elif motif
	#endif
}

void GuiText_undo (Widget widget) {
	#if gtk
		iam_text;
		history_do(me, 1);
	#elif win
	#elif mac
		iam_text;
		if (isMLTE (me)) {
			TXNUndo (my macMlteObject);
		}
		_GuiText_handleValueChanged (widget);
	#elif motif
	#endif
}

/* End of file GuiText.c */
