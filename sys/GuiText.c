/* GuiText.c
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
 */

#include "GuiP.h"
#ifndef UNIX

#if mac
	#define isTextControl(w)  ((w) -> isControl != 0)
	#define isMLTE(w)  ((w) -> macMlteObject != NULL)
#endif

static int theScrolledHint;

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

void _GuiText_handleFocusReception (Widget me) {
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
	my shell -> textFocus = me;   /* see (1.2) */
	theGui.textFocus = me;   /* see (1.4) */
}

void _GuiText_handleFocusLoss (Widget me) {
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
	if (me == theGui.textFocus)
		theGui.textFocus = NULL;
}

#if mac
void _GuiMac_clearTheTextFocus (void) {
	if (theGui.textFocus) {
		_GuiMac_clipOn (theGui.textFocus);
		if (isTextControl (theGui.textFocus)) {
			ClearKeyboardFocus (theGui.textFocus -> macWindow);
		} else if (isMLTE (theGui.textFocus)) {
			TXNFocus (theGui.textFocus -> macMlteObject, 0);
			TXNActivate (theGui.textFocus -> macMlteObject, theGui.textFocus -> macMlteFrameId, 0);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusLoss (theGui.textFocus);
	}
}
#endif

void _GuiText_setTheTextFocus (Widget me) {
	if (me == NULL || theGui.textFocus == me
		|| ! my managed) return;   /* Perhaps not-yet-managed. Test: open Praat's DataEditor with a Sound, then type. */
	#if win
		SetFocus (my window);   /* Will send an EN_SETFOCUS notification, which will call _GuiText_handleFocusReception (). */
	#elif mac
		_GuiMac_clearTheTextFocus ();
		_GuiMac_clipOn (me);
		if (isTextControl (me)) {
			SetKeyboardFocus (my macWindow, my nat.control.handle, kControlEditTextPart);
		} else if (isMLTE (me)) {
			TXNActivate (my macMlteObject, my macMlteFrameId, 1);
			TXNFocus (my macMlteObject, 1);
		}
		GuiMac_clipOff ();
		_GuiText_handleFocusReception (me);
	#endif
}

/*
 * CHANGE NOTIFICATION
 */
void _GuiText_handleValueChanged (Widget me) {
	_Gui_callCallbacks (me, & my motif.text.valueChangedCallbacks, NULL);
}

/*
 * EVENT HANDLING
 */

#if mac
	int _GuiMacText_tryToHandleReturnKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget me, EventRecord *event) {
		if (me && my activateCallback) {
			my activateCallback (me, my activateClosure, (XtPointer) event);
				return 1;
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleClipboardShortcut (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget me, unsigned char charCode, EventRecord *event) {
		if (me) {
			if (isTextControl (me)) {
				CallNextEventHandler (eventHandlerCallRef, eventRef);
				_GuiText_handleValueChanged (me);
				return 1;
			} else if (isMLTE (me)) {
				if (charCode == 'X' && my motif.text.editable) {
					if (event -> what != autoKey) XmTextCut (me, 0);
					return 1;
				}
				if (charCode == 'C') {
					if (event -> what != autoKey) XmTextCopy (me, 0);
					return 1;
				}
				if (charCode == 'V' && my motif.text.editable) {
					XmTextPaste (me);
					return 1;
				}
			}
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleKey (EventHandlerCallRef eventHandlerCallRef, EventRef eventRef, Widget me, unsigned char keyCode, unsigned char charCode, EventRecord *event) {
		(void) keyCode;
		if (me && my motif.text.editable) {
			_GuiMac_clipOn (me);
			if (isTextControl (me)) {
				CallNextEventHandler (eventHandlerCallRef, eventRef);
			} else if (isMLTE (me)) {
				//static long key = 0; Melder_casual ("key %ld", ++key);
				//TXNKeyDown (my macMlteObject, event);   // Tends never to be called.
				CallNextEventHandler (eventHandlerCallRef, eventRef);
			}
			GuiMac_clipOff ();
			if (charCode > 31 || charCode < 28)   /* Arrows do not change the value of the text. */
				_Gui_callCallbacks (me, & my motif.text.valueChangedCallbacks, (XtPointer) & event);
			return 1;
		}
		return 0;   /* Not handled. */
	}
	void _GuiMacText_handleClick (Widget me, EventRecord *event) {
		_GuiText_setTheTextFocus (me);
		_GuiMac_clipOn (me);
		if (isTextControl (me)) {
			HandleControlClick (my nat.control.handle, event -> where, event -> modifiers, NULL);
		} else if (isMLTE (me)) {
			LocalToGlobal (& event -> where);
			TXNClick (my macMlteObject, event);   /* Handles text selection and scrolling. */
			GlobalToLocal (& event -> where);
		}
		GuiMac_clipOff ();
		_Gui_callCallbacks (me, & my motif.text.motionVerifyCallbacks, 0);
	}
#endif

/*
 * LAYOUT
 */
#if mac
	void _GuiMacText_move (Widget me) {
		if (isTextControl (me)) {
			_GuiMac_clipOn (me);
			MoveControl (my nat.control.handle, my rect.left + 3, my rect.top + 3);
			_GuiMac_clipOffValid (me);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my macMlteObject, my rect. top, my rect. left, my rect. bottom, my rect. right, my macMlteFrameId);
		}
	}
	void _GuiMacText_shellResize (Widget me) {
		/*
		 * Shell erasure, and therefore text erasure, has been handled by caller.
		 * Reshowing will be handled by caller.
		 */
		if (isTextControl (me)) {
			MoveControl (my nat.control.handle, my rect.left + 3, my rect.top + 3);
			SizeControl (my nat.control.handle, my width - 6, my height - 6);
			/*
			 * Control reshowing will explicitly be handled by caller.
			 */
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my macMlteObject, my rect. top, my rect. left, my rect. bottom, my rect. right, my macMlteFrameId);
		}
	}
	void _GuiMacText_resize (Widget me) {
		if (isTextControl (me)) {
			SizeControl (my nat.control.handle, my width - 6, my height - 6);
			/*
			 * Container widgets will have been invalidated.
			 * So in order not to make the control flash, we validate it.
			 */
			_Gui_validateWidget (me);
		} else if (isMLTE (me)) {
			TXNSetFrameBounds (my macMlteObject, my rect. top, my rect. left, my rect. bottom, my rect. right, my macMlteFrameId);
		}
	}
#endif

void _GuiText_unmanage (Widget me) {
	#if win
		_GuiText_handleFocusLoss (me);
		_GuiNativeControl_hide (me);
	#elif mac
		/*
		 * Just _GuiText_handleFocusLoss () is not enough,
		 * because that can leave a visible blinking cursor.
		 */
		if (isTextControl (me)) {
			if (me == theGui.textFocus) _GuiMac_clearTheTextFocus ();   /* Remove visible blinking cursor. */
			_GuiNativeControl_hide (me);
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
	void _GuiMacText_map (Widget me) {
		if (isTextControl (me)) {
			_GuiNativeControl_show (me);
		} else if (isMLTE (me)) {
		}
	}
	void _GuiMacText_update (Widget me) {
		_GuiMac_clipOn (me);
		if (isTextControl (me)) {
			Draw1Control (my nat.control.handle);
		} else if (isMLTE (me)) {
			TXNDraw (my macMlteObject, NULL);
		}
		GuiMac_clipOff ();
	}
#endif

void _GuiText_nativizeWidget (Widget me) {
	/*
	 *    Create native text object;
	 *    install pointers back and forth.
	 */
	#if win
		my window = CreateWindow ("edit", NULL, WS_CHILD | WS_BORDER | ES_AUTOHSCROLL | ES_MULTILINE | WS_CLIPSIBLINGS
			| ( theScrolledHint ? WS_HSCROLL | WS_VSCROLL : 0 ),
			my x, my y, my width, my height, my parent -> window, (HMENU) 1, theGui.instance, NULL);
		SetWindowLong (my window, GWL_USERDATA, (long) me);
		SetWindowFont (my window, GetStockFont (theScrolledHint ? ANSI_FIXED_FONT : ANSI_VAR_FONT), FALSE);
		my motif.text.editable = TRUE;
		Edit_LimitText (my window, 0);
	#elif mac
	{
		/*
		 * Determine whether the text object should be a control, a TextEdit object, or an MLTE object.
		 */
		if (theScrolledHint) {
			TXNLongRect destRect;
			TXNMargins margins;
			TXNControlData controlData;
			TXNControlTag controlTag = kTXNMarginsTag;
			TXNNewObject (NULL,   /* No file. */
				my macWindow, & my rect, kTXNWantHScrollBarMask | kTXNWantVScrollBarMask
					| kTXNMonostyledTextMask | kTXNDrawGrowIconMask,
				kTXNTextEditStyleFrameType, kTXNTextensionFile,
				kTXNMacOSEncoding, & my macMlteObject, & my macMlteFrameId, me);
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
		} else if (my parent -> widgetClass != xmScrolledWindowWidgetClass) {
			//static short font;
			Rect r = my rect;
			//Size actualSize;
			//ControlFontStyleRec fontStyle;
			//if (! font) GetFNum ("\pMonaco", & font);
			//fontStyle. flags = kControlUseFontMask | kControlUseSizeMask;
			//fontStyle. font = font;
			//fontStyle. size = 9;
			InsetRect (& r, 3, 3);
			CreateEditUnicodeTextControl (my macWindow, & r, NULL, false, NULL, & my nat.control.handle);
			SetControlReference (my nat.control.handle, (long) me);
			my motif.text.editable = True;
			my isControl = TRUE;
		} else {
			Melder_fatal ("Old style TE record.");
		}
	}
	#endif
	/*
	 * The first created text widget shall attract the input focus.
	 */
	if (! my shell -> textFocus) {
		my shell -> textFocus = me;   /* Even if not-yet-managed. But in that case it will not receive global focus. */
	}
}

Widget XmCreateText (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget me = _Gui_initializeWidget (xmTextWidgetClass, parent, name);
	(void) dum1;
	(void) dum2;
	_GuiText_nativizeWidget (me);
	return me;
}

Widget XmCreateScrolledText (Widget parent, const char *name, ArgList dum1, int dum2) {
	Widget scrolled = XmCreateScrolledWindow (parent, name, NULL, 0);
	(void) dum1;
	(void) dum2;
	return XmCreateText (scrolled, name, NULL, 0);
}

void _GuiText_destroyWidget (Widget me) {
	if (me == theGui.textFocus)
		theGui.textFocus = NULL;   /* Remove dangling reference. */
	if (me == my shell -> textFocus)
		my shell -> textFocus = NULL;   /* Remove dangling reference. */
	#if win
		DestroyWindow (my window);
	#elif mac
		if (isTextControl (me)) {
			_GuiNativeControl_destroy (me);
		} else if (isMLTE (me)) {
			TXNDeleteObject (my macMlteObject);
		}
	#endif
}

/*
 * CONTENT () {}
 */

static long NativeText_getLength (Widget me) {
	#if win
		return Edit_GetTextLength (my window);
	#elif mac
		long length = 0;
		if (isTextControl (me)) {
			Size size;
			GetControlDataSize (my nat.control.handle, kControlEntireControl, kControlEditTextTextTag, & size);
			length = size;
		} else if (isMLTE (me)) {
			#if 1
				/*
				 * From the reference page of TXNDataSize:
				 * "If you are using Unicode and you want to know the number of characters,
				 * divide the returned ByteCount value by sizeof(UniChar) or 2,
				 * since MLTE uses the 16-bit Unicode Transformation Format (UTF-16)."
				 */
				length = TXNDataSize (my macMlteObject) / sizeof (UniChar);
			#else
				long dataSize = TXNDataSize (my macMlteObject), irun;
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my macMlteObject, 0, dataSize, & numberOfRuns);
				for (irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my macMlteObject, left, right, & han, kTXNTextData);
						if (han) {
							long size = GetHandleSize (han);
							length += size;
							DisposeHandle (han);
						}
					}
				}
			#endif
		}
		return length;
	#endif
}

static long NativeText_getLengthW (Widget me) {
	#if win
		return Edit_GetTextLength (my window);
	#elif mac
		if (isTextControl (me)) {
			Size size;
			GetControlDataSize (my nat.control.handle, kControlEntireControl, kControlEditTextTextTag, & size);
			return size;
		} else if (isMLTE (me)) {
			return TXNDataSize (my macMlteObject) / sizeof (UniChar);
		}
		return 0;   // Should not occur.
	#endif
}

static void NativeText_getText (Widget me, char *buffer, long length) {
	#if win
		Edit_GetText (my window, buffer, length + 1);
	#elif mac
		if (isTextControl (me)) {
			GetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextTextTag, length, buffer, NULL);
			buffer [length] = '\0';
		} else if (isMLTE (me)) {
			#if 1
				Handle han;
				TXNGetDataEncoded (my macMlteObject, 0, length, & han, kTXNTextData);
				strncpy (buffer, (char *) *han, length);
				DisposeHandle (han);
			#else
				long dataSize = TXNDataSize (my macMlteObject), irun;
				ItemCount numberOfRuns;
				TXNCountRunsInRange (my macMlteObject, 0, dataSize, & numberOfRuns);
				for (irun = 0; irun < numberOfRuns; irun ++) {
					unsigned long left, right;
					TXNDataType dataType;
					TXNGetIndexedRunInfoFromRange (my macMlteObject, irun, 0, dataSize,
						& left, & right, & dataType, 0, NULL);
					if (dataType == kTXNTextData || dataType == kTXNUnicodeTextData) {
						Handle han;
						TXNGetDataEncoded (my macMlteObject, left, right, & han, kTXNTextData);
						if (han) {
							long size = GetHandleSize (han);
							strncpy (buffer, (char *) *han, size);
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
	buffer [length] = '\0';
}

static void NativeText_getTextW (Widget me, wchar_t *buffer, long length) {
	#if win
		char *bufferA = Melder_malloc (length + 1);
		Edit_GetText (my window, bufferA, length + 1);
		for (long i = 0; i <= length; i ++) {
			buffer [i] = (unsigned char) bufferA [i];
		}
		Melder_free (bufferA);
	#elif mac
		if (isTextControl (me)) {
			CFStringRef cfString;
			GetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString, NULL);
			UniChar *macText = Melder_malloc ((length + 1) * sizeof (UniChar));
			CFRange range = { 0, length };
			CFStringGetCharacters (cfString, range, macText);
			for (long i = 0; i < length; i ++) {
				buffer [i] = macText [i];
			}
			buffer [length] = '\0';
		} else if (isMLTE (me)) {
			Handle han;
			TXNGetDataEncoded (my macMlteObject, 0, length, & han, kTXNUnicodeTextData);
			for (long i = 0; i < length; i ++) {
				buffer [i] = ((UniChar *) *han) [i];
			}
			DisposeHandle (han);
		}
	#endif
	buffer [length] = '\0';
}

char *XmTextGetString (Widget me) {
	long length = NativeText_getLength (me);
	char *result = Melder_malloc (1 + length);
	NativeText_getText (me, result, length);
	Melder_killReturns_inline (result);
	return result;
}

void XmTextSetString (Widget me, const char *text) {
	#if win
		const char *from;
		char *winText = Melder_malloc (2 * strlen (text) + 1), *to;   /* All new lines plus one null byte. */
		if (! winText) return;
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != '\0'; from ++, to ++)
			if (*from == '\n') { *to = 13; * ++ to = '\n'; } else *to = *from;
		*to = '\0';
		Edit_SetText (my window, winText);
		Melder_free (winText);
	#elif mac
		long length = strlen (text), i;
		char *macText;
		macText = Melder_malloc (length + 1);
		Melder_assert (my widgetClass == xmTextWidgetClass);
		strncpy (macText, text, length);
		macText [length] = '\0';
		/*
		 * Replace all LF with CR.
		 */
		for (i = 0; i < length; i ++) if (macText [i] == '\n') macText [i] = 13;
		if (isTextControl (me)) {
			SetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextTextTag, length, macText);
		} else if (isMLTE (me)) {
			TXNSetData (my macMlteObject, kTXNTextData, macText, length, 0, NativeText_getLength (me));
		}
		Melder_free (macText);
		if (my managed) {
			if (theGui.duringUpdate) {
				_Gui_invalidateWidget (me);   /* HACK: necessary because VisRgn has temporarily been changed (not used in Praat any longer). */
			} else {
				if (isTextControl (me)) {
					_GuiMac_clipOn (me);
					Draw1Control (my nat.control.handle);
					GuiMac_clipOff ();
				} else if (isMLTE (me)) {
				}
			}
		}
		_GuiText_handleValueChanged (me);
	#endif
}

void XmTextReplace (Widget me, XmTextPosition from_pos, XmTextPosition to_pos, char *text) {
	#if win
		const char *from;
		char *winText = Melder_malloc (2 * strlen (text) + 1), *to;   /* All new lines plus one null byte. */
		if (! winText) return;
		Melder_assert (MEMBER (me, Text));
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
		Edit_SetSel (my window, from_pos, to_pos);
		Edit_ReplaceSel (my window, winText);
		Melder_free (winText);
	#elif mac
		long length = strlen (text), i;
		char *macText;
		macText = Melder_malloc (length + 1);
		Melder_assert (my widgetClass == xmTextWidgetClass);
		strncpy (macText, text, length);
		macText [length] = '\0';
		/*
		 * Replace all LF with CR.
		 */
		for (i = 0; i < length; i ++) if (macText [i] == '\n') macText [i] = 13;
		/*
		 * We DON'T replace any text without selecting it, so we can deselect any other text,
		 * thus allowing ourselves to select [from_pos, to_pos] and use selection replacement.
		 */
		if (my managed) _GuiMac_clipOn (me);
		if (isTextControl (me)) {
			long oldLength = NativeText_getLength (me);
			char *totalText = Melder_malloc (oldLength - (to_pos - from_pos) + length + 1);
			char *oldText = Melder_malloc (oldLength + 1);
			NativeText_getText (me, oldText, oldLength);
			strncpy (totalText, oldText, from_pos);
			strcpy (totalText + from_pos, macText);
			strcpy (totalText + from_pos + length, oldText + to_pos);
			SetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextTextTag, strlen (totalText), totalText);
			Melder_free (oldText);
			Melder_free (totalText);
		} else if (isMLTE (me)) {
			TXNSetData (my macMlteObject, kTXNTextData, macText, length, from_pos, to_pos);
		}
		Melder_free (macText);
		if (my managed) {
			if (isTextControl (me)) {
				Draw1Control (my nat.control.handle);
			} else if (isMLTE (me)) {
			}
			GuiMac_clipOff ();
		}
		_GuiText_handleValueChanged (me);
	#endif
}

void XmTextSetMaxLength (Widget widget, int max_length) {
	(void) widget;
	(void) max_length;
}

/*
 * SELECTION
 */

static int NativeText_getSelectionRange (Widget me, long *out_left, long *out_right) {
	unsigned long left, right;
	Melder_assert (MEMBER (me, Text));
	#if win
		SendMessage (my window, EM_GETSEL, (WPARAM) & left, (LPARAM) & right);   // 32-bit (R&N: 579)
	#elif mac
		if (isTextControl (me)) {
			ControlEditTextSelectionRec rec;
			GetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec, NULL);
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

char *XmTextGetSelection (Widget me) {
	long length, start, end;
	char *result;
	NativeText_getSelectionRange (me, & start, & end);
	if (end <= start) return NULL;
	/*
	 * Get all text.
	 */
	length = NativeText_getLength (me);
	result = Melder_malloc (1 + length);
	NativeText_getText (me, result, length);
	/*
	 * Zoom in on selection.
	 */
	length = end - start;
	memmove (result, result + start, length);   /* Not because of realloc, but because of free! */
	result [length] = '\0';
	result = Melder_realloc (result, length + 1);   /* Optional. */
	Melder_killReturns_inline (result);   /* AFTER zooming! */
	return result;
}

Boolean XmTextGetSelectionPosition (Widget me, XmTextPosition *left, XmTextPosition *right) {
	return NativeText_getSelectionRange (me, left, right);
	#if win
		/* BUG: not corrected for CR/LF versus LF. */
	#endif
}

XmTextPosition XmTextGetInsertionPosition (Widget me) {
	long left, right;
	NativeText_getSelectionRange (me, & left, & right);
	#if win
		/* BUG: not corrected for CR/LF versus LF. */
	#endif
	return left;
}

void XmTextSetSelection (Widget me, XmTextPosition first, XmTextPosition last, long time) {
	(void) time;
	#if win
		Edit_SetSel (my window, first, last);
	#elif mac
		if (isTextControl (me)) {
			ControlEditTextSelectionRec rec = { first, last };
			SetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec);
		} else if (isMLTE (me)) {
			TXNSetSelection (my macMlteObject, first, last);
		}
	#endif
}

void XmTextSetInsertionPosition (Widget me, XmTextPosition position) {
	#if win
		Edit_SetSel (my window, position, position);
	#elif mac
		if (isTextControl (me)) {
			ControlEditTextSelectionRec rec = { position, position };
			SetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextSelectionTag, sizeof (rec), & rec);
		} else if (isMLTE (me)) {
			TXNSetSelection (my macMlteObject, position, position);
		}
	#endif
}

void XmTextShowPosition (Widget me, XmTextPosition position) {
	#if win
		(void) position;
		Edit_ScrollCaret (my window);
	#elif mac
		if (isTextControl (me)) {
			;
		} else if (isMLTE (me)) {
			TXNShowSelection (my macMlteObject, position);
		}
	#endif
}

/*
 * CLIPBOARD
 */

Boolean XmTextCut (Widget me, long time) {
	(void) time;
	if (! my motif.text.editable || ! NativeText_getSelectionRange (me, NULL, NULL)) return False;
	#if win
		SendMessage (my window, WM_CUT, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		if (isTextControl (me)) {
			_GuiMac_clipOn (me);
			HandleControlKey (my nat.control.handle, 0, 'X', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNCut (my macMlteObject);
		}
		_GuiText_handleValueChanged (me);
	#endif
	return True;
}

Boolean XmTextCopy (Widget me, long time) {
	if (! NativeText_getSelectionRange (me, NULL, NULL)) return False;
	#if win
		SendMessage (my window, WM_COPY, 0, 0);
	#elif mac
		if (isTextControl (me)) {
			HandleControlKey (my nat.control.handle, 0, 'C', cmdKey);
		} else if (isMLTE (me)) {
			TXNCopy (my macMlteObject);
		}
	#endif
	(void) time;
	return True;
}

Boolean XmTextPaste (Widget me) {
	if (! my motif.text.editable) return False;
	#if win
		SendMessage (my window, WM_PASTE, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		if (isTextControl (me)) {
			_GuiMac_clipOn (me);
			HandleControlKey (my nat.control.handle, 0, 'V', cmdKey);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNPaste (my macMlteObject);
		}
		_GuiText_handleValueChanged (me);
	#endif
	return True;
}

Boolean XmTextRemove (Widget me) {
	if (! my motif.text.editable || ! NativeText_getSelectionRange (me, NULL, NULL)) return False;
	#if win
		SendMessage (my window, WM_CLEAR, 0, 0);   /* This will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks. */
	#elif mac
		if (isTextControl (me)) {
			_GuiMac_clipOn (me);
			HandleControlKey (my nat.control.handle, 0, 8, 0);   /* Backspace key. */
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNClear (my macMlteObject);
		}
		_GuiText_handleValueChanged (me);
	#endif
	return True;
}

/*
 * PACKAGE
 */

void _GuiText_init (void) {
	#if mac
		//short font;
		TXNMacOSPreferredFontDescription defaults = { 0 };
		//GetFNum ("\pMonaco", & font);
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

Widget GuiText_createScrolled (Widget parent, const char *name, int editable, int topOffset) {
	Widget me;
	#if defined (UNIX)
		Arg arg [4];
		arg [0]. name = XmNscrollingPolicy; arg [0]. value = XmAUTOMATIC;
		arg [1]. name = XmNscrollBarDisplayPolicy; arg [1]. value = XmAS_NEEDED;
		arg [2]. name = XmNeditable; arg [2]. value = editable;
		arg [3]. name = XmNeditMode; arg [3]. value = XmMULTI_LINE_EDIT;   /* On Linux, this must go before creation. */
		me = XmCreateScrolledText (parent, (char *) name, arg, 4);
		XtVaSetValues (XtParent (me),
			XmNleftAttachment, XmATTACH_FORM,
			XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, topOffset,
			XmNbottomAttachment, XmATTACH_FORM,
			NULL);
		XtVaSetValues (me, XmNeditable, editable, XmNeditMode, XmMULTI_LINE_EDIT,
		#ifndef macintosh
			XmNrows, 33, XmNcolumns, 90,
		#endif
			NULL);
	#else
		theScrolledHint = TRUE;
		me = _Gui_initializeWidget (xmTextWidgetClass, parent, name);
		my motif.text.editable = editable;
		_GuiText_nativizeWidget (me);
		XtVaSetValues (me,
			XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 0,
			XmNrightAttachment, XmATTACH_FORM,
			XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, topOffset,
			XmNbottomAttachment, XmATTACH_FORM,
			NULL);
		theScrolledHint = FALSE;
	#endif
	return me;
}

void GuiText_setFontSize (Widget me, int size) {
	#if mac
		if (isMLTE (me)) {
			TXNTypeAttributes attr;
			attr. tag = kTXNQDFontSizeAttribute;
			attr. size = kTXNFontSizeAttributeSize;
			attr. data. dataValue = (unsigned long) size << 16;
			TXNSetTypeAttributes (my macMlteObject, 1, & attr, 0, 2000000000);
		}
	#endif
}

void GuiText_undo (Widget me) {
	#if mac
		if (isMLTE (me)) {
			TXNUndo (my macMlteObject);
		}
		_GuiText_handleValueChanged (me);
	#endif
}

void GuiText_redo (Widget me) {
	#if mac
		if (isMLTE (me)) {
			TXNRedo (my macMlteObject);
		}
		_GuiText_handleValueChanged (me);
	#endif
}

wchar_t *GuiText_getStringW (Widget me) {
	#if defined (UNIX)
		char *textUtf8 = XmTextGetString (me);
		wchar_t *result = Melder_utf8ToWcs (textUtf8);
		XtFree (textUtf8);
		return result;
	#else
		long length = NativeText_getLengthW (me);
		wchar_t *result = Melder_malloc ((1 + length) * sizeof (wchar_t));
		NativeText_getTextW (me, result, length);
		//Melder_killReturns_inline (result);
		return result;
	#endif
}

void GuiText_setStringW (Widget me, wchar_t *text) {
	#if defined (UNIX)
		char *textUtf8 = Melder_wcsToUtf8 (text);
		XmTextSetString (me, textUtf8);
		Melder_free (textUtf8);
	#elif win
		const wchar_t *from;
		char *winText = Melder_malloc (4 * wcslen (text) + 1), *to;   /* All new lines plus one null byte. */
		if (! winText) return;
		/*
		 * Replace all LF with CR/LF.
		 */
		for (from = text, to = winText; *from != '\0'; from ++, to ++)
			if (*from == '\n') { *to = 13; * ++ to = '\n'; } else *to = *from;
		*to = '\0';
		Edit_SetText (my window, winText);
		Melder_free (winText);
	#elif mac
		long length = wcslen (text), i;
		UniChar *macText;
		macText = Melder_malloc ((length + 1) * sizeof (UniChar));
		Melder_assert (my widgetClass == xmTextWidgetClass);
		/*
		 * Replace all LF with CR.
		 */
		for (i = 0; i <= length; i ++) {
			macText [i] = text [i] == '\n' ? 13 : text [i];
		}
		if (isTextControl (me)) {
			CFStringRef cfString = CFStringCreateWithCharacters (NULL, macText, length);
			SetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextCFStringTag, sizeof (CFStringRef), & cfString);
			CFRelease (cfString);
		} else if (isMLTE (me)) {
			TXNSetData (my macMlteObject, kTXNUnicodeTextData, macText, length*2, 0, NativeText_getLengthW (me));
		}
		Melder_free (macText);
		if (my managed) {
			if (theGui.duringUpdate) {
				_Gui_invalidateWidget (me);   /* HACK: necessary because VisRgn has temporarily been changed (not used in Praat any longer). */
			} else {
				if (isTextControl (me)) {
					_GuiMac_clipOn (me);
					Draw1Control (my nat.control.handle);
					GuiMac_clipOff ();
				} else if (isMLTE (me)) {
				}
			}
		}
		_GuiText_handleValueChanged (me);
	#endif
}

/* End of file GuiText.c */
