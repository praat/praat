/* GuiText.cpp
 *
 * Copyright (C) 1993-2017 Paul Boersma, 2013 Tom Naughton
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

#include "GuiP.h"
#include <locale.h>

Thing_implement (GuiText, GuiControl, 0);

#if motif
	#define iam_text \
		Melder_assert (widget -> widgetClass == xmTextWidgetClass); \
		GuiText me = (GuiText) widget -> userData
#else
	#define iam_text \
		GuiText me = (GuiText) _GuiObject_getUserData (widget)
#endif

#if motif

static HFONT font10, font12, font14, font18, font24;

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

void _GuiText_setTheTextFocus (GuiObject widget) {
	if (! widget || theGui.textFocus == widget
		|| ! widget -> managed) return;   // perhaps not-yet-managed; test: open Praat's DataEditor with a Sound, then type
	#if gtk
		gtk_widget_grab_focus (GTK_WIDGET (widget));   // not used: gtk is not 1 when motif is 1
	#elif motif
		SetFocus (widget -> window);   // will send an EN_SETFOCUS notification, which will call _GuiText_handleFocusReception ()
	#endif
}

/*
 * CHANGE NOTIFICATION
 */
void _GuiText_handleValueChanged (GuiObject widget) {
	iam_text;
	if (my d_changedCallback) {
		struct structGuiTextEvent event { me };
		my d_changedCallback (my d_changedBoss, & event);
	}
}

void _GuiText_unmanage (GuiObject widget) {
	_GuiText_handleFocusLoss (widget);
	_GuiNativeControl_hide (widget);
	/*
	 * The caller will set the unmanage flag to zero, and remanage the parent.
	 */
}

/*
 * VISIBILITY
 */

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

static integer NativeText_getLength (GuiObject widget) {
	return Edit_GetTextLength (widget -> window);   // in UTF-16 code units
}

/*
 * SELECTION
 */

static bool NativeText_getSelectionRange (GuiObject widget, integer *out_left, integer *out_right) {
	Melder_assert (MEMBER (widget, Text));
	DWORD left, right;
	SendMessage (widget -> window, EM_GETSEL, (WPARAM) & left, (LPARAM) & right);
	if (out_left) *out_left = left;
	if (out_right) *out_right = right;
	return right > left;
}

/*
 * PACKAGE
 */

void _GuiText_init () {
}

void _GuiText_exit () {
}

#endif

#if gtk || motif
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
				gtk_text_buffer_get_iter_at_offset (buffer, & from_it, from_pos);
				gtk_text_buffer_get_iter_at_offset (buffer, & to_it, to_pos);
				gtk_text_buffer_delete_interactive (buffer, & from_it, & to_it,
					gtk_text_view_get_editable (GTK_TEXT_VIEW (widget)));
				gtk_text_buffer_place_cursor (buffer, & to_it);
			}
		#elif motif
		#endif
	}

	static void _GuiText_insert (GuiObject widget, int from_pos, int to_pos, const history_data text) {
		#if gtk
			if (G_OBJECT_TYPE (G_OBJECT (widget)) == GTK_TYPE_ENTRY) {
				gint from_pos_gint = from_pos;
				gtk_editable_insert_text (GTK_EDITABLE (widget), text, to_pos - from_pos, & from_pos_gint);
			} else {
				GtkTextBuffer *buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (widget));
				GtkTextIter it;
				gtk_text_buffer_get_iter_at_offset (buffer, & it, from_pos);
				gtk_text_buffer_insert_interactive (buffer, & it, text, to_pos - from_pos,
					gtk_text_view_get_editable (GTK_TEXT_VIEW (widget)));
				gtk_text_buffer_get_iter_at_offset (buffer, & it, to_pos);
				gtk_text_buffer_place_cursor (buffer, & it);
			}
		#elif motif
		#endif
	}

	/* Tests the previous elements of the history for mergability with the one to insert given via parameters.
	 * If successful, it returns a pointer to the last valid entry in the merged history list, otherwise
	 * returns nullptr.
	 * Specifically the function expands the previous insert/delete event(s)
	 *  - with the current, if current also is an insert/delete event and the ranges of previous and current event match
	 *  - with the previous delete and current insert event, in case the ranges of both event-pairs respectively match
	 */
	static history_entry * history_addAndMerge (GuiText me, history_data text_new, integer first, integer last, bool deleted) {
		history_entry *he = nullptr;
		
		if (! my d_prev)
			return nullptr;
		
		if (my d_prev->type_del == deleted) {
			// extend the last history event by this one
			if (my d_prev->first == last) {
				// most common for backspace key presses
				he = my d_prev;
				text_new = (char *) realloc (text_new, sizeof (*text_new) * (he->last - first + 1));
				memcpy (text_new + last - first, he->text, sizeof (*text_new) * (he->last - he->first + 1));
				free (he->text);
				he->text = text_new;
				he->first = first;
				
			} else if (my d_prev->last == first) {
				// most common for ordinary text insertion
				he = my d_prev;
				he->text = (char *) realloc (he->text, sizeof (*he->text) * (last - he->first + 1));
				memcpy (he->text + he->last - he->first, text_new, sizeof (*he->text) * (last - first + 1));
				free (text_new);
				he->last = last;
				
			} else if (deleted && my d_prev->first == first) {
				// most common for delete key presses
				he = my d_prev;
				he->text = (char *) realloc (he->text, sizeof (*he->text) * (last - first + he->last - he->first + 1));
				memcpy (he->text + he->last - he->first, text_new, sizeof (*he->text) * (last - first + 1));
				free (text_new);
				he->last = last + he->last - he->first;
			}
		} else {
			// prev->type_del != deleted, no simple expansion possible, check for double expansion
			if (! deleted && my d_prev->prev && my d_prev->prev->prev) {
				history_entry *del_one = my d_prev;
				history_entry *ins_mult = del_one->prev;
				history_entry *del_mult = ins_mult->prev;
				integer from1 = del_mult->first, to1 = del_mult->last;
				integer from2 = ins_mult->first, to2 = ins_mult->last;
				integer from3 = del_one->first, to3 = del_one->last;
				if (from3 == first && to3 == last && from2 == from1 && to2 == to1 && to1 == first &&
						! ins_mult->type_del && del_mult->type_del) {
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
	static void history_add (GuiText me, history_data text_new, integer first, integer last, bool deleted) {

		// delete all newer entries; from here on there is no 'Redo' until the next 'Undo' is performed
		history_entry *old_hnext = my d_next, *hnext;
		while (old_hnext) {
			hnext = old_hnext->next;
			free (old_hnext->text);
			free (old_hnext);
			old_hnext = hnext;
		}
		my d_next = nullptr;
		
		history_entry *he = history_addAndMerge (me, text_new, first, last, deleted);
		if (! he) {
			he = (history_entry *) malloc (sizeof *he);
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

	static bool history_has_undo (GuiText me) {
		return !! my d_prev;
	}

	static bool history_has_redo (GuiText me) {
		return !! my d_next;
	}

	static void history_do (GuiText me, bool undo) {
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

	static void history_clear (GuiText me) {
		history_entry *h1, *h2;
		
		h1 = my d_prev;
		while (h1) {
			h2 = h1->prev;
			free (h1->text);
			free (h1);
			h1 = h2;
		}
		my d_prev = nullptr;
		
		h1 = my d_next;
		while (h1) {
			h2 = h1->next;
			free (h1->text);
			free (h1);
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
		if (my d_changedCallback) {
			struct structGuiTextEvent event { me };
			my d_changedCallback (my d_changedBoss, & event);
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
#elif motif
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
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiText));
		d_userData = static_cast <GuiText> (userData);
	}
	- (void) textDidChange: (NSNotification *) notification {
		(void) notification;
		GuiText me = d_userData;
		if (me && my d_changedCallback) {
			struct structGuiTextEvent event { me };
			my d_changedCallback (my d_changedBoss, & event);
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
	- (GuiThing) getUserData {
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
		if (me && my d_changedCallback) {
			struct structGuiTextEvent event { me };
			my d_changedCallback (my d_changedBoss, & event);
		}
		return YES;
	}
	@end
#endif

GuiText GuiText_create (GuiForm parent, int left, int right, int top, int bottom, uint32 flags) {
	autoGuiText me = Thing_new (GuiText);
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
			g_signal_connect (G_OBJECT (buffer), "delete-range", G_CALLBACK (_GuiGtkTextBuf_history_delete_cb), me.get());
			g_signal_connect (G_OBJECT (buffer), "insert-text", G_CALLBACK (_GuiGtkTextBuf_history_insert_cb), me.get());
			g_signal_connect (G_OBJECT (buffer), "changed", G_CALLBACK (_GuiGtkText_valueChangedCallback), me.get());
			_GuiObject_setUserData (my d_widget, me.get());
			_GuiObject_setUserData (scrolled, me.get());
			my v_positionInForm (scrolled, left, right, top, bottom, parent);
		} else {
			my d_widget = gtk_entry_new ();
			gtk_editable_set_editable (GTK_EDITABLE (my d_widget), (flags & GuiText_NONEDITABLE) == 0);
			g_signal_connect (G_OBJECT (my d_widget), "delete-text", G_CALLBACK (_GuiGtkEntry_history_delete_cb), me.get());
			g_signal_connect (G_OBJECT (my d_widget), "insert-text", G_CALLBACK (_GuiGtkEntry_history_insert_cb), me.get());
			g_signal_connect (GTK_EDITABLE (my d_widget), "changed", G_CALLBACK (_GuiGtkText_valueChangedCallback), me.get());
			//GTK_WIDGET_UNSET_FLAGS (my d_widget, GTK_CAN_DEFAULT);
			_GuiObject_setUserData (my d_widget, me.get());
			my v_positionInForm (my d_widget, left, right, top, bottom, parent);
			gtk_entry_set_activates_default (GTK_ENTRY (my d_widget), true);
		}
		trace (U"after creating a GTK text widget: locale is ", Melder_peek8to32 (setlocale (LC_ALL, nullptr)));
		my d_prev = nullptr;
		my d_next = nullptr;
		my d_history_change = 0;
		my d_undo_item = nullptr;
		my d_redo_item = nullptr;
		g_signal_connect (G_OBJECT (my d_widget), "destroy", G_CALLBACK (_GuiGtkText_destroyCallback), me.get());
	#elif motif
		my d_widget = _Gui_initializeWidget (xmTextWidgetClass, parent -> d_widget, flags & GuiText_SCROLLED ? U"scrolledText" : U"text");
		_GuiObject_setUserData (my d_widget, me.get());
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
			[my d_cocoaTextView setUserData: me.get()];
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
			[(GuiCocoaTextField *) my d_widget   setUserData: me.get()];
			[(NSTextField *) my d_widget   setEditable: YES];
			static NSFont *theTextFont;
			if (! theTextFont) {
				theTextFont = [[NSFont systemFontOfSize: 13.0] retain];
			}
			[(NSTextField *) my d_widget   setFont: theTextFont];
		}
	#endif
	
	return me.releaseToAmbiguousOwner();
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
	#elif motif
		if (! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_COPY, 0, 0);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   copy: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] copy: nil];
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
	#elif motif
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_CUT, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   cut: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] cut: nil];
		}
	#endif
}

autostring32 GuiText_getSelection (GuiText me) {
	#if gtk
		// first = gtk_text_iter_get_offset (& start);
		// last = gtk_text_iter_get_offset (& end);
		if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_ENTRY) {
			gint start, end;
			gtk_editable_get_selection_bounds (GTK_EDITABLE (my d_widget), & start, & end);
			if (end > start) {   // at least one character selected?
				gchar *text = gtk_editable_get_chars (GTK_EDITABLE (my d_widget), start, end);
				autostring32 result = Melder_8to32 (text);
				g_free (text);
				return result;
			}
		} else if (G_OBJECT_TYPE (G_OBJECT (my d_widget)) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			if (gtk_text_buffer_get_has_selection (textBuffer)) {   // at least one character selected?
				GtkTextIter start, end;
				gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
				gchar *text = gtk_text_buffer_get_text (textBuffer, & start, & end, true);
				autostring32 result = Melder_8to32 (text);
				g_free (text);
				return result;
			}
		}
	#elif motif
		integer startW, endW;
		(void) NativeText_getSelectionRange (my d_widget, & startW, & endW);
		if (endW > startW) {   // at least one character selected?
			/*
				Get all text.
			*/
			integer lengthW = NativeText_getLength (my d_widget);   // in UTF-16 code units
			WCHAR *bufferW = Melder_malloc_f (WCHAR, lengthW + 1);
			GetWindowTextW (my d_widget -> window, bufferW, lengthW + 1);
			/*
				Zoom in on selection.
			*/
			lengthW = endW - startW;
			memmove (bufferW, bufferW + startW, lengthW * sizeof (WCHAR));   // not because of realloc, but because of free!
			bufferW [lengthW] = U'\0';
			autostring32 result = Melder_dup_f (Melder_peekWto32 (bufferW));
			(void) Melder_killReturns_inplace (result.get());   // AFTER zooming!
			return result;
		}
	#elif cocoa
		integer start, end;
		autostring32 selection = GuiText_getStringAndSelectionPosition (me, & start, & end);
		integer length = end - start;
		if (length > 0) {
			autostring32 result (length, true);
			memcpy (result.get(), & selection [start], integer_to_uinteger (length) * sizeof (char32));
			result [length] = U'\0';
			(void) Melder_killReturns_inplace (result.get());
			return result;
		}
	#endif
	return autostring32();   // zero characters selected
}

autostring32 GuiText_getString (GuiText me) {
	integer first, last;
	return GuiText_getStringAndSelectionPosition (me, & first, & last);
}

autostring32 GuiText_getStringAndSelectionPosition (GuiText me, integer *first, integer *last) {
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
			autostring32 result = Melder_8to32 (text);
			g_free (text);
			gtk_text_buffer_get_selection_bounds (textBuffer, & start, & end);
			*first = gtk_text_iter_get_offset (& start);
			*last = gtk_text_iter_get_offset (& end);
			return result;
		}
		return autostring32();
	#elif motif
		integer lengthW = NativeText_getLength (my d_widget);
		autostringW bufferW (lengthW , true);
		GetWindowTextW (my d_widget -> window, bufferW.get(), lengthW + 1);
		integer firstW, lastW;
		(void) NativeText_getSelectionRange (my d_widget, & firstW, & lastW);

		integer differenceFirst = 0;
		for (integer i = 0; i < firstW; i ++) {
			if (bufferW [i] == 13 && (bufferW [i + 1] == L'\n' || bufferW [i + 1] == 0x0085))
				differenceFirst ++;
			if (bufferW [i] >= 0xDC00 && bufferW [i] <= 0xDFFF)
				differenceFirst ++;
		}
		*first = firstW - differenceFirst;

		integer differenceLast = differenceFirst;
		for (integer i = firstW; i < lastW; i ++) {
			if (bufferW [i] == 13 && (bufferW [i + 1] == L'\n' || bufferW [i + 1] == 0x0085))
				differenceLast ++;
			if (bufferW [i] >= 0xDC00 && bufferW [i] <= 0xDFFF)
				differenceLast ++;
		}
		*last = lastW - differenceLast;

		autostring32 result = Melder_dup_f (Melder_peekWto32 (bufferW.get()));
		(void) Melder_killReturns_inplace (result.get());
		return result;
	#elif cocoa
		if (my d_cocoaTextView) {
			NSString *nsString = [my d_cocoaTextView   string];
			autostring32 result = Melder_8to32 ([nsString UTF8String]);
			trace (U"string ", result.get());
			NSRange nsRange = [my d_cocoaTextView   selectedRange];
			*first = uinteger_to_integer (nsRange. location);
			*last = *first + uinteger_to_integer (nsRange. length);
			for (integer i = 0; i < *first; i ++) {
				if (result [i] > 0xFFFF) {
					(*first) --;
					(*last) --;
				}
			}
			for (integer i = *first; i < *last; i ++) {
				if (result [i] > 0xFFFF)
					(*last) --;
			}
			return result;
		} else {
			NSString *nsString = [(NSTextField *) my d_widget   stringValue];
			autostring32 result = Melder_8to32 ([nsString UTF8String]);
			trace (U"string ", result.get());
			NSRange nsRange = [[[(NSTextField *) my d_widget   window] fieldEditor: NO forObject: nil] selectedRange];
			*first = uinteger_to_integer (nsRange. location);
			*last = *first + uinteger_to_integer (nsRange. length);
			for (integer i = 0; i < *first; i ++) {
				if (result [i] > 0xFFFF) {
					(*first) --;
					(*last) --;
				}
			}
			for (integer i = *first; i < *last; i ++) {
				if (result [i] > 0xFFFF)
					(*last) --;
			}
			return result;
		}
	#else
		return autostring32();
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
	#elif motif
		if (! my d_editable) return;
		SendMessage (my d_widget -> window, WM_PASTE, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   pasteAsPlainText: nil];
		} else {
			[[[(GuiCocoaTextField *) my d_widget   window]   fieldEditor: NO   forObject: nil] pasteAsPlainText: nil];
		}
	#endif
}

void GuiText_redo (GuiText me) {
	#if gtk || motif
		history_do (me, 0);
	#elif cocoa
		if (my d_cocoaTextView) {
			[[my d_cocoaTextView   undoManager] redo];
		}
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
	#elif motif
		if (! my d_editable || ! NativeText_getSelectionRange (my d_widget, nullptr, nullptr)) return;
		SendMessage (my d_widget -> window, WM_CLEAR, 0, 0);   // this will send the EN_CHANGE message, hence no need to call the valueChangedCallbacks
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   delete: nil];
		}
	#endif
}

void GuiText_replace (GuiText me, integer from_pos, integer to_pos, conststring32 text) {
	#if gtk
		const gchar *newText = Melder_peek32to8 (text);
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
	#elif motif
		Melder_assert (MEMBER (my d_widget, Text));
		autostring32 winText (2 * str32len (text), true);   // all newlines
		char32 *to = & winText [0];
		/*
			Replace all LF with CR/LF.
		*/
		for (const char32 *from = & text [0]; *from != U'\0'; from ++, to ++)
			if (*from == U'\n') { *to = 13; * ++ to = U'\n'; } else *to = *from;
		*to = U'\0';
		/*
			We DON'T replace any text without selecting it, so we can deselect any other text,
			thus allowing ourselves to select [from_pos, to_pos] and use the REPLACESEL message.
		 */
		GuiText_setSelection (me, from_pos, to_pos);
		Edit_ReplaceSel (my d_widget -> window, Melder_peek32toW (winText.get()));
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView) {
			integer numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
			{// scope
				autostring32 oldText = GuiText_getString (me);
				for (integer i = 0; i < from_pos; i ++) if (oldText [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
				for (integer i = from_pos; i < to_pos; i ++) if (oldText [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
			}
			from_pos += numberOfLeadingHighUnicodeValues;
			to_pos += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;
			NSRange nsRange = NSMakeRange (integer_to_uinteger (from_pos), integer_to_uinteger (to_pos - from_pos));
			NSString *nsString = (NSString *) Melder_peek32toCfstring (text);
			[my d_cocoaTextView   shouldChangeTextInRange: nsRange   replacementString: nsString];   // ignore the returned BOOL: only interested in the side effect of having undo support
			[[my d_cocoaTextView   textStorage] replaceCharactersInRange: nsRange   withString: nsString];
		}
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
	#elif motif
		Edit_ScrollCaret (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView)
			[my d_cocoaTextView   scrollRangeToVisible: [my d_cocoaTextView   selectedRange]];
	#endif
}

void GuiText_setChangedCallback (GuiText me, GuiText_ChangedCallback changedCallback, Thing changedBoss) {
	my d_changedCallback = changedCallback;
	my d_changedBoss = changedBoss;
}

void GuiText_setFontSize (GuiText me, double size) {
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
	#elif motif
		// a trick to update the window. BUG: why doesn't UpdateWindow seem to suffice?
		integer first, last;
		autostring32 text = GuiText_getStringAndSelectionPosition (me, & first, & last);
		GuiText_setString (me, U"");   // erase all
		UpdateWindow (my d_widget -> window);
		if (size <= 10.0) {
			SetWindowFont (my d_widget -> window, font10, false);
		} else if (size <= 12.0) {
			SetWindowFont (my d_widget -> window, font12, false);
		} else if (size <= 14.0) {
			SetWindowFont (my d_widget -> window, font14, false);
		} else if (size <= 18.0) {
			SetWindowFont (my d_widget -> window, font18, false);
		} else {
			SetWindowFont (my d_widget -> window, font24, false);
		}
		GuiText_setString (me, text.get());
		GuiText_setSelection (me, first, last);
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		if (my d_cocoaTextView) {
			[my d_cocoaTextView   setFont: [NSFont fontWithName: @"Menlo"   size: size]];
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
	#elif motif
	#elif cocoa
	#endif
}

void GuiText_setSelection (GuiText me, integer first, integer last) {
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
	#elif motif
		autostring32 text = GuiText_getString (me);
		if (first < 0) first = 0;
		if (last < 0) last = 0;
		integer length = str32len (text.get());
		if (first >= length) first = length;
		if (last >= length) last = length;
		/*
		 * 'first' and 'last' are the positions of the selection in the text when separated by LF alone.
		 * We have to convert this to the positions that the selection has in a text separated by CR/LF sequences.
		 */
		integer numberOfLeadingLineBreaks = 0, numberOfSelectedLineBreaks = 0;
		for (integer i = 0; i < first; i ++) if (text [i] == U'\n') numberOfLeadingLineBreaks ++;
		for (integer i = first; i < last; i ++) if (text [i] == U'\n') numberOfSelectedLineBreaks ++;
		/*
			On Windows, characters are counted in UTF-16 units, whereas 'first' and 'last' are in UTF-32 units. Convert.
		*/
		integer numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (integer i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (integer i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;

		first += numberOfLeadingLineBreaks;
		last += numberOfLeadingLineBreaks + numberOfSelectedLineBreaks;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;

		Edit_SetSel (my d_widget -> window, first, last);
		UpdateWindow (my d_widget -> window);
	#elif cocoa
		/*
			On Cocoa, characters are counted in UTF-16 units, whereas 'first' and 'last' are in UTF-32 units. Convert.
		*/
		autostring32 text = GuiText_getString (me);
		integer numberOfLeadingHighUnicodeValues = 0, numberOfSelectedHighUnicodeValues = 0;
		for (integer i = 0; i < first; i ++) if (text [i] > 0xFFFF) numberOfLeadingHighUnicodeValues ++;
		for (integer i = first; i < last; i ++) if (text [i] > 0xFFFF) numberOfSelectedHighUnicodeValues ++;
		first += numberOfLeadingHighUnicodeValues;
		last += numberOfLeadingHighUnicodeValues + numberOfSelectedHighUnicodeValues;

		if (my d_cocoaTextView) {
			[my d_cocoaTextView   setSelectedRange: NSMakeRange (integer_to_uinteger (first), integer_to_uinteger (last - first))];
		}
	#endif
	}
}

void GuiText_setString (GuiText me, conststring32 text) {
	#if gtk
		if (G_OBJECT_TYPE (my d_widget) == GTK_TYPE_ENTRY) {
			gtk_entry_set_text (GTK_ENTRY (my d_widget), Melder_peek32to8 (text));
		} else if (G_OBJECT_TYPE (my d_widget) == GTK_TYPE_TEXT_VIEW) {
			GtkTextBuffer *textBuffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (my d_widget));
			const gchar *textUtf8 = Melder_peek32to8 (text);
			//gtk_text_buffer_set_text (textBuffer, textUtf8, strlen (textUtf8));   // length in bytes!
			GtkTextIter start, end;
			gtk_text_buffer_get_start_iter (textBuffer, & start);
			gtk_text_buffer_get_end_iter (textBuffer, & end);
			gtk_text_buffer_delete_interactive (textBuffer, & start, & end, gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
			gtk_text_buffer_insert_interactive (textBuffer, & start, textUtf8, strlen (textUtf8), gtk_text_view_get_editable (GTK_TEXT_VIEW (my d_widget)));
		}
	#elif motif
		autostring32 winText (2 * str32len (text), true);   // all new lines
		char32 *to = & winText [0];
		/*
			Replace all LF with CR/LF.
		*/
		for (const char32 *from = & text [0]; *from != U'\0'; from ++, to ++)
			if (*from == U'\n') { *to = 13; * ++ to = U'\n'; } else *to = *from;
		*to = U'\0';
		SetWindowTextW (my d_widget -> window, Melder_peek32toW (winText.get()));
		UpdateWindow (my d_widget -> window);
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
	#elif motif
	#elif cocoa
	#endif
}

void GuiText_undo (GuiText me) {
	#if gtk
		history_do (me, 1);
	#elif motif
	#elif cocoa
		if (my d_cocoaTextView) {
			[[my d_cocoaTextView   undoManager] undo];
		}
	#endif
}

/* End of file GuiText.cpp */
