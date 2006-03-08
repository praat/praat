/* GuiText.c
 *
 * Copyright (C) 1993-2005 Paul Boersma
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
 * pb 2004/01/01 MLTE
 * pb 2004/03/11 tried to make compatible with MacOS X 10.1.x
 * pb 2005/09/01 GuiText_undo and GuiText_redo
 */

#include "GuiP.h"
#ifndef UNIX

#if mac
	#define allowMLTE  1
	#define haveMLTE  (carbon && allowMLTE)
	#define isTextControl(w)  ((w) -> isControl != 0)
	#define isTE(w)  ((w) -> nat.text.handle != NULL)
	#define isMLTE(w)  (haveMLTE && (w) -> macMlteObject != NULL)
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
		} else if (isTE (theGui.textFocus)) {
			TEDeactivate (theGui.textFocus -> nat.text.handle);
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
		} else if (isTE (me)) {
			TEActivate (my nat.text.handle);
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
	#if mac
		if (isMLTE (me)) {
			my changeCount = TXNGetChangeCount (my macMlteObject);
		}
	#endif
	_Gui_callCallbacks (me, & my motif.text.valueChangedCallbacks, NULL);
}

/*
 * EVENT HANDLING
 */

#if mac
	int _GuiMacText_tryToHandleReturnKey (Widget me, EventRecord *event) {
		if (me && my activateCallback) {
			my activateCallback (me, my activateClosure, (XtPointer) event);
				return 1;
		}
		return 0;   /* Not handled. */
	}
	int _GuiMacText_tryToHandleClipboardShortcut (Widget me, unsigned char charCode, EventRecord *event) {
		if (me) {
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
		return 0;   /* Not handled. */
	}
	static void motif_mac_textFont (void) {
		static short font;
		if (! font) GetFNum ("\pMonaco", & font);
		TextFont (font);
		TextSize (9);
		TextFace (0);
	}
	int _GuiMacText_tryToHandleKey (Widget me, unsigned char keyCode, unsigned char charCode, EventRecord *event) {
		if (me && my motif.text.editable) {
			_GuiMac_clipOn (me);
			if (isTextControl (me)) {
				HandleControlKey (my nat.control.handle, keyCode, charCode, event -> modifiers);
			} else if (isTE (me)) {
				motif_mac_textFont ();
				TEKey (charCode, my nat.text.handle);
				motif_mac_defaultFont ();
			} else if (isMLTE (me)) {
				TXNKeyDown (my macMlteObject, event);
			}
			GuiMac_clipOff ();
			if (charCode > 31 || charCode < 28)   /* Arrows do not change the value of the text. */
				_Gui_callCallbacks (me, & my motif.text.valueChangedCallbacks, (XtPointer) & event);
			return 1;
		}
		return 0;   /* Not handled. */
	}
	static void scrollScrolledText (Widget me, short dv) {
		Widget scrollBar = my parent -> motif.scrolledWindow.verticalBar;
		int min = 0, max = scrollBar -> maximum - scrollBar -> sliderSize;
		int newValue = scrollBar -> value + dv;
		if (newValue > max) newValue = max;
		if (newValue < min) newValue = min;
		TEScroll (0, scrollBar -> value - newValue, my nat.text.handle);
		_GuiMac_clipOn (scrollBar);
		XtVaSetValues (scrollBar, XmNvalue, newValue, NULL);
		_GuiMac_clipOn (me);
	}
	static pascal unsigned char _motif_clickLoop (TEPtr pTE) {
		Widget me = theGui.textFocus;
		Point mouseLoc;
		short top, bottom;
		(void) pTE;
		if (! MEMBER (my parent, ScrolledWindow)) return 1;
		top = my parent -> motif.scrolledWindow.clipWindow -> rect. top;
		bottom = my parent -> motif.scrolledWindow.clipWindow -> rect. bottom;
		GetMouse (& mouseLoc);
		if (mouseLoc. v > bottom) scrollScrolledText (me, +6);
		if (mouseLoc. v < top) scrollScrolledText (me, -6);
		return 1;
	}
	void _GuiMacText_handleClick (Widget me, EventRecord *event) {
		_GuiText_setTheTextFocus (me);
		_GuiMac_clipOn (me);
		if (isTextControl (me)) {
			HandleControlClick (my nat.control.handle, event -> where, event -> modifiers, NULL);
		} else if (isTE (me)) {
			TEClick (event -> where, (event -> modifiers & shiftKey) != 0, my nat.text.handle);
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
		} else if (isTE (me)) {
			Rect r = my rect;
			InsetRect (& r, 3, 3);   /* Smaller than my rect! */
			(** my nat.text.handle). destRect = r;
			(** my nat.text.handle). viewRect = r;
			TECalText (my nat.text.handle);   /* IM I-390 */
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
		} else if (isTE (me)) {
			Rect r = my rect;
			InsetRect (& r, 3, 3);
			(** my nat.text.handle). destRect = r;
			(** my nat.text.handle). viewRect = r;
			/*
			 * Reshowing will implicitly occur because caller has called InvalWindowRect.
			 * But MacOS has to be told explicitly that the soft line breaks have to be recalculated.
			 * The test for this is the wrapping text widget in Praat's TextGrid editor.
			 */
			TECalText (my nat.text.handle);
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
		} else if (isTE (me)) {
			Rect r = my rect;
			InsetRect (& r, 3, 3);
			(** my nat.text.handle). destRect = r;
			(** my nat.text.handle). viewRect = r;
			TECalText (my nat.text.handle);   /* IM I-390 */
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
		} else if (isTE (me)) {
			_Gui_invalidateWidget (me);   /* Make sure that the text object will be redrawn unmanaged. */
			if (me == theGui.textFocus) _GuiMac_clearTheTextFocus ();   /* Remove visible blinking cursor. */
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
		} else if (isTE (me)) {
			_GuiMac_clipOn (me);
			DrawThemeEditTextFrame (& my rect, kThemeStateActive);
			TEUpdate (& my rect, my nat.text.handle);
			_GuiMac_clipOffValid (me);
		} else if (isMLTE (me)) {
		}
	}
	void _GuiMacText_update (Widget me) {
		_GuiMac_clipOn (me);
		if (isTextControl (me)) {
			Draw1Control (my nat.control.handle);
		} else if (isTE (me)) {
			EraseRect (& my rect);
			DrawThemeEditTextFrame (& my rect, kThemeStateActive);
			TEUpdate (& my rect, my nat.text.handle);
		} else if (isMLTE (me)) {
			TXNDraw (my macMlteObject, NULL);
		}
		GuiMac_clipOff ();
	}
	void _GuiMac_makeTextCaretBlink (void) {
		if (theGui.textFocus) {
		 	_GuiMac_clipOn (theGui.textFocus);
			if (isTextControl (theGui.textFocus)) {
				IdleControls (theGui.textFocus -> macWindow);
			} else if (isTE (theGui.textFocus)) {
				TEIdle (theGui.textFocus -> nat.text.handle);   /* Make caret blink. */
			} else if (isMLTE (theGui.textFocus)) {
				unsigned long changeCount = TXNGetChangeCount (theGui.textFocus -> macMlteObject);
				if (changeCount != theGui.textFocus -> changeCount) {
					_GuiText_handleValueChanged (theGui.textFocus);
				}
				TXNIdle (theGui.textFocus -> macMlteObject);   /* superfluous */
			}
			GuiMac_clipOff ();
		}
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
		if (haveMLTE && theScrolledHint) {
			TXNLongRect destRect;
			TXNMargins margins;
			TXNControlData controlData;
			TXNControlTag controlTag = kTXNMarginsTag;
			TXNNewObject (NULL,   /* No file. */
				my macWindow, & my rect, kTXNWantHScrollBarMask | kTXNWantVScrollBarMask
					#if carbon
						| kTXNMonostyledTextMask
					#endif
					| kTXNDrawGrowIconMask,
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
			static short font;
			Rect r = my rect;
			Size actualSize;
			ControlFontStyleRec fontStyle;
			if (! font) GetFNum ("\pMonaco", & font);
			fontStyle. flags = kControlUseFontMask | kControlUseSizeMask;
			fontStyle. font = font;
			fontStyle. size = 9;
			InsetRect (& r, 3, 3);
			my nat.control.handle = NewControl (my macWindow, & r,
				"\p", false, 0, 0, 0, kControlEditTextProc + kControlUsesOwningWindowsFontVariant, (long) me);
			/*SetControlFontStyle (my nat.control.handle, & fontStyle);*/
			GetControlData (my nat.control.handle, kControlEntireControl, kControlEditTextTEHandleTag,
				sizeof (TEHandle), & my nat.text.handle, & actualSize);
			my motif.text.editable = True;
			my isControl = TRUE;
		} else {
			Rect r = my rect;
			static TEClickLoopUPP theTEClickLoopUPP;
			InsetRect (& r, 3, 3);   /* BUG: this causes a border problem in autoscrolling. */
			motif_mac_textFont ();
			my nat.text.handle = TENew (& r, & r);   /* BUG: should clip on r, not my rect. */
			motif_mac_defaultFont ();
			my motif.text.editable = True;
			if (! theTEClickLoopUPP) theTEClickLoopUPP = NewTEClickLoopUPP (_motif_clickLoop);
			TESetClickLoop (theTEClickLoopUPP, my nat.text.handle);
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
		} else if (isTE (me)) {
			_GuiMac_clipOn (me);
			TEDispose (my nat.text.handle);
			EraseRect (& my rect);
			GuiMac_clipOff ();
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
		if (isTextControl (me) || isTE (me)) {
			length = (** my nat.text.handle). teLength;
		} else if (isMLTE (me)) {
			#if 1
				length = TXNDataSize (my macMlteObject) / 2;
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

static void NativeText_getText (Widget me, char *buffer, long length) {
	#if win
		Edit_GetText (my window, buffer, length + 1);
	#elif mac
		if (isTextControl (me) || isTE (me)) {
			strncpy (buffer, (char *) *(** my nat.text.handle). hText, length);
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
		} else if (isTE (me)) {
			if (length > 32000) { length = 32000; Melder_warning ("Text truncated to 32000 characters!!!!!\nDo not save!!!!!"); }
			TESetText (macText, length, my nat.text.handle);
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
				} else if (isTE (me)) {
					Rect r = my rect;
					InsetRect (& r, 1, 1);
					_GuiMac_clipOn (me);
					EraseRect (& r);
					TEUpdate (& my rect, my nat.text.handle);
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
		if (length > 32000) { length = 32000; Melder_warning ("Text truncated to 32000 characters!!!!!\nDo not save!!!!!"); }
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
		if (isTextControl (me) || isTE (me)) {
			TESetSelect (from_pos, to_pos, my nat.text.handle);
			TEDelete (my nat.text.handle);
			TEInsert (macText, length, my nat.text.handle);
		} else if (isMLTE (me)) {
			TXNSetData (my macMlteObject, kTXNTextData, macText, length, from_pos, to_pos);
		}
		Melder_free (macText);
		if (my managed) {
			if (isTextControl (me)) {
				Draw1Control (my nat.control.handle);
			} else if (isTE (me)) {
				Rect r = my rect;
				SetPortWindowPort (my macWindow);
				InsetRect (& r, 1, 1);
				EraseRect (& r);
				TEUpdate (& my rect, my nat.text.handle);
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
		} else if (isTE (me)) {
			left = (**my nat.text.handle). selStart;
			right = (**my nat.text.handle). selEnd;
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
		if (isTextControl (me) || isTE (me)) {
			TESetSelect (first, last, my nat.text.handle);
		} else if (isMLTE (me)) {
			TXNSetSelection (my macMlteObject, first, last);
		}
	#endif
}

void XmTextSetInsertionPosition (Widget me, XmTextPosition position) {
	#if win
		Edit_SetSel (my window, position, position);
	#elif mac
		if (isTextControl (me) || isTE (me)) {
			TESetSelect (position, position, my nat.text.handle);
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
		if (isTextControl (me) || isTE (me)) {
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
		} else if (isTE (me)) {
			#if carbon
				ClearCurrentScrap ();
			#else
				ZeroScrap ();
			#endif
			_GuiMac_clipOn (me);
			TECut (my nat.text.handle);
			GuiMac_clipOff ();
			TEToScrap ();
		} else if (isMLTE (me)) {
			TXNCut (my macMlteObject);
			TXNConvertToPublicScrap ();
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
		} else if (isTE (me)) {
			#if carbon
				ClearCurrentScrap ();
			#else
				ZeroScrap ();
			#endif
			TECopy (my nat.text.handle);
			TEToScrap ();
		} else if (isMLTE (me)) {
			TXNCopy (my macMlteObject);
			TXNConvertToPublicScrap ();
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
		} else if (isTE (me)) {
			TEFromScrap ();
			_GuiMac_clipOn (me);
			TEPaste (my nat.text.handle);
			GuiMac_clipOff ();
		} else if (isMLTE (me)) {
			TXNConvertFromPublicScrap ();
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
		} else if (isTE (me)) {
			_GuiMac_clipOn (me);
			TEDelete (my nat.text.handle);
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
		#if carbon
			#if haveMLTE
				short font;
				TXNMacOSPreferredFontDescription defaults;
				GetFNum ("\pMonaco", & font);
				defaults. fontID = font;
				defaults. pointSize = 0x000B0000;
				defaults. fontStyle = kTXNDefaultFontStyle;
				defaults. encoding  = kTXNMacOSEncoding /*kTXNSystemDefaultEncoding*/;
				TXNInitTextension (& defaults, 1, 0);
			#endif
		#else
			TEInit ();
		#endif
	#endif
}

void _GuiText_exit (void) {
	#if mac
		#if haveMLTE
			TXNTerminateTextension (); 
		#endif
	#endif
}

#endif

Widget GuiText_createScrolled (Widget parent, const char *name, int editable, int topOffset) {
	Widget me;
	#if defined (UNIX) || (mac && ! haveMLTE)
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

/* End of file GuiText.c */
