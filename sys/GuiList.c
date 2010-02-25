/* GuiList.c
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
 * pb 2007/12/26 abstraction from Motif
 * pb 2009/01/31 NUMlvector_free has to be followed by assigning a NULL
 * fb 2010/02/23 GTK
 */

#include "GuiP.h"
#include "NUM.h"
#define my  me ->
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_list \
		Melder_assert (widget -> widgetClass == xmListWidgetClass); \
		GuiList me = widget -> userData
#else
	#define iam_list \
		GuiList me = _GuiObject_getUserData (widget)
#endif

#if win
	#define CELL_HEIGHT  15
#elif mac
	#define CELL_HEIGHT  16
	#define USE_MAC_LISTBOX_CONTROL  0
#endif

typedef struct structGuiList {
	Widget widget;
	bool allowMultipleSelection;
	void (*selectionChangedCallback) (void *boss, GuiListEvent event);
	void *selectionChangedBoss;
	void (*doubleClickCallback) (void *boss, GuiListEvent event);
	void *doubleClickBoss;
	#if gtk
		GtkListStore *liststore;
	#elif win
	#elif mac
		Widget scrolled;
		ListHandle macListHandle;
	#elif motif
	#endif
} *GuiList;

#if gtk
	static void _GuiGtkList_destroyCallback (gpointer void_me) {
		iam (GuiList);
		Melder_free (me);
	}
	static void _GuiGtkList_selectionChangedCallback (GtkTreeSelection *sel, gpointer void_me) {
		iam (GuiList);
		if (my selectionChangedCallback != NULL) {
			struct structGuiListEvent event = { GTK_WIDGET(gtk_tree_selection_get_tree_view(sel)) };
			my selectionChangedCallback (my selectionChangedBoss, & event);
		}
	}
#elif win || mac
	void _GuiWinMacList_destroy (Widget widget) {
		iam_list;
		#if win
			DestroyWindow (widget -> window);
		#elif mac
			_GuiMac_clipOnParent (widget);
			if (widget -> isControl) {
				DisposeControl (widget -> nat.control.handle);
			} else {
				LDispose (my macListHandle);
			}
			GuiMac_clipOff ();
		#endif
		Melder_free (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinMacList_map (Widget widget) {
		iam_list;
		#if win
			ShowWindow (widget -> window, SW_SHOW);
		#elif mac
			if (widget -> isControl) {
				_GuiNativeControl_show (widget);
				Melder_casual ("showing a list");
				//_GuiMac_clipOnParent (widget);
				//LSetDrawingMode (true, my macListHandle);
				//_GuiMac_clipOffInvalid (widget);
			} else {
				_GuiMac_clipOnParent (widget);
				LSetDrawingMode (true, my macListHandle);
				_GuiMac_clipOffInvalid (widget);
			}
		#endif
	}
	#if win
		void _GuiWinList_handleClick (Widget widget) {
			iam_list;
			if (my selectionChangedCallback != NULL) {
				struct structGuiListEvent event = { widget };
				my selectionChangedCallback (my selectionChangedBoss, & event);
			}
		}
	#elif mac
		void _GuiMacList_activate (Widget widget, bool activate) {
			iam_list;
			_GuiMac_clipOnParent (widget);
			LActivate (activate, my macListHandle);
			GuiMac_clipOff ();
		}
		void _GuiMacList_handleControlClick (Widget widget, EventRecord *macEvent) {
			iam_list;
			_GuiMac_clipOnParent (widget);
			bool pushed = TrackControl (widget -> nat.control.handle, macEvent -> where, NULL);
			GuiMac_clipOff ();
			if (pushed && my selectionChangedCallback) {
				struct structGuiListEvent event = { widget };
				my selectionChangedCallback (my selectionChangedBoss, & event);
			}
		}
		void _GuiMacList_handleClick (Widget widget, EventRecord *macEvent) {
			iam_list;
			_GuiMac_clipOnParent (widget);
			bool doubleClick = LClick (macEvent -> where, macEvent -> modifiers, my macListHandle);
			GuiMac_clipOff ();
			if (my selectionChangedCallback) {
				struct structGuiListEvent event = { widget };
				my selectionChangedCallback (my selectionChangedBoss, & event);
			}
			if (doubleClick && my doubleClickCallback) {
				struct structGuiListEvent event = { widget };
				my doubleClickCallback (my doubleClickBoss, & event);
			}
		}
		void _GuiMacList_move (Widget widget) {
			iam_list;
			(** my macListHandle). rView = widget -> rect;
		}
		void _GuiMacList_resize (Widget widget) {
			iam_list;
			(** my macListHandle). rView = widget -> rect;
			SetPortWindowPort (widget -> macWindow);
			(** my macListHandle). cellSize. h = widget -> width;
			if (widget -> parent -> widgetClass == xmScrolledWindowWidgetClass)
				_Gui_manageScrolledWindow (widget -> parent);
		}
		void _GuiMacList_shellResize (Widget widget) {
			iam_list;
			(** my macListHandle). rView = widget -> rect;
			(** my macListHandle). cellSize. h = widget -> width;
		}
		void _GuiMacList_update (Widget widget, RgnHandle visRgn) {
			iam_list;
			_GuiMac_clipOnParent (widget);
			if (widget -> isControl) {
				Draw1Control (widget -> nat.control.handle);
			} else {
				LUpdate (visRgn, my macListHandle);
			}
			GuiMac_clipOff ();
		}
	#endif
#elif motif
	static void _GuiMotifList_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiList);
		Melder_free (me);
	}
	static void _GuiMotifList_selectionChangedCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) call;
		iam (GuiList);
		Melder_assert (me != NULL);
		if (my selectionChangedCallback != NULL) {
			struct structGuiListEvent event = { widget };
			my selectionChangedCallback (my selectionChangedBoss, & event);
		}
	}
	static void _GuiMotifList_defaultActionCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) call;
		iam (GuiList);
		Melder_assert (me != NULL);
		if (my doubleClickCallback != NULL) {
			struct structGuiListEvent event = { widget };
			my doubleClickCallback (my doubleClickBoss, & event);
		}
	}
#endif

#if mac
	static pascal void mac_listDefinition (short message, Boolean select, Rect *rect, Cell cell, short dataOffset, short dataLength, ListHandle handle) {
		Widget widget = (Widget) GetListRefCon (handle);
		(void) cell;
		switch (message) {
			case lDrawMsg:
			case lHiliteMsg:   // We redraw everything, even when just highlighting. The reason is anti-aliasing.
				Melder_assert (widget != NULL);
				SetPortWindowPort (widget -> macWindow);
				_GuiMac_clipOnParent (widget);
				/*
				 * In order that highlighting (which by default turns only the white pixels into pink)
				 * does not leave light-grey specks around the glyphs (in the anti-aliasing regions),
				 * we simply draw the glyphs on a pink background if the item is selected.
				 */
				/*
				 * Erase the background.
				 */
				static RGBColor whiteColour = { 0xFFFF, 0xFFFF, 0xFFFF }, blackColour = { 0, 0, 0 };
				RGBForeColor (& whiteColour);
				PaintRect (rect);
				RGBForeColor (& blackColour);
				/*
				 * Pink (or any other colour the user prefers) if the item is selected.
				 */
				if (select) {
					LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
					InvertRect (rect);
				}
				/*
				 * Draw the text on top of this.
				 */
				CGContextRef macGraphicsContext;
				QDBeginCGContext (GetWindowPort (widget -> macWindow), & macGraphicsContext);
				int shellHeight = GuiMac_clipOn_graphicsContext (widget, macGraphicsContext);
				static ATSUFontFallbacks fontFallbacks = NULL;
				if (fontFallbacks == NULL) {
					ATSUCreateFontFallbacks (& fontFallbacks);
					ATSUSetObjFontFallbacks (fontFallbacks, 0, NULL, kATSUDefaultFontFallbacks);
				}
				char *text_utf8 = (char *) *(*handle) -> cells + dataOffset;
				strncpy (Melder_buffer1, text_utf8, dataLength);
				Melder_buffer1 [dataLength] = '\0';
				wchar_t *text_wcs = Melder_peekUtf8ToWcs (Melder_buffer1);
				const MelderUtf16 *text_utf16 = Melder_peekWcsToUtf16 (text_wcs);
				UniCharCount runLength = wcslen (text_wcs);   // BUG
				ATSUTextLayout textLayout;
				ATSUStyle style;
				ATSUCreateStyle (& style);
				Fixed fontSize = 12 << 16;
				Boolean boldStyle = 0;
				Boolean italicStyle = 0;
				ATSUAttributeTag styleAttributeTags [] = { kATSUSizeTag, kATSUQDBoldfaceTag, kATSUQDItalicTag };
				ByteCount styleValueSizes [] = { sizeof (Fixed), sizeof (Boolean), sizeof (Boolean) };
				ATSUAttributeValuePtr styleValues [] = { & fontSize, & boldStyle, & italicStyle };
				ATSUSetAttributes (style, 3, styleAttributeTags, styleValueSizes, styleValues);
				OSStatus err = ATSUCreateTextLayoutWithTextPtr (text_utf16, kATSUFromTextBeginning, kATSUToTextEnd, runLength,
					1, & runLength, & style, & textLayout);
				Melder_assert (err == 0);
				ATSUAttributeTag attributeTags [] = { kATSUCGContextTag, kATSULineFontFallbacksTag };
				ByteCount valueSizes [] = { sizeof (CGContextRef), sizeof (ATSUFontFallbacks) };
				ATSUAttributeValuePtr values [] = { & macGraphicsContext, & fontFallbacks };
				ATSUSetLayoutControls (textLayout, 2, attributeTags, valueSizes, values);
				ATSUSetTransientFontMatching (textLayout, true);
				CGContextTranslateCTM (macGraphicsContext, rect -> left, shellHeight - rect -> bottom + 4);
				err = ATSUDrawText (textLayout, kATSUFromTextBeginning, kATSUToTextEnd, 0 /*xDC << 16*/, 0 /*(shellHeight - yDC) << 16*/);
				Melder_assert (err == 0);
				CGContextSynchronize (macGraphicsContext);
				ATSUDisposeTextLayout (textLayout);
				ATSUDisposeStyle (style);
				QDEndCGContext (GetWindowPort (widget -> macWindow), & macGraphicsContext);
				GuiMac_clipOff ();
				break;
	/*		case lHiliteMsg:
				Melder_assert (me != NULL);
				SetPortWindowPort (my macWindow);
				_GuiMac_clipOnParent (me);
				LMSetHiliteMode (LMGetHiliteMode () & ~ 128L);
				InvertRect (rect);
				GuiMac_clipOff ();
				break;*/
		}
	}
#endif

#if gtk
enum {
  COLUMN_STRING,
  N_COLUMNS
};
#endif

Widget GuiList_create (Widget parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header) {
	GuiList me = Melder_calloc (struct structGuiList, 1);
	my allowMultipleSelection = allowMultipleSelection;
	#if gtk
		enum
		{
			COL_OBJECTS = 0,
			NUM_COLS
		};

		GtkCellRenderer *renderer;
		GtkTreeViewColumn *col;
		GtkTreeSelection *sel;
		GtkListStore *liststore;

		liststore = gtk_list_store_new (N_COLUMNS, G_TYPE_STRING);
		my widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));
		g_object_unref(liststore); // Destroys the widget after the list is destroyed

		_GuiObject_setUserData (my widget, me); /* nog een functie die je niet moet vergeten */

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, TRUE);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_OBJECTS);
		if (header != NULL)
			gtk_tree_view_column_set_title (col, Melder_peekWcsToUtf8 (header));
		gtk_tree_view_append_column (GTK_TREE_VIEW (my widget), col);

		g_object_set_data_full (G_OBJECT (my widget), "guiList",  me, (GDestroyNotify) _GuiGtkList_destroyCallback); 

/*		GtkCellRenderer *renderer;
		GtkTreeViewColumn *col;
		
		my widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, TRUE);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_ID);
		gtk_tree_view_column_set_title (col, " ID ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);
		
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, TRUE);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_TYPE);
		gtk_tree_view_column_set_title (col, " Type ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, TRUE);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_NAME);
		gtk_tree_view_column_set_title (col, " Name ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);
*/

		sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (my widget));
		if (allowMultipleSelection) {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_MULTIPLE);
		} else {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_SINGLE);
		}
		gtk_box_pack_start (GTK_BOX (parent), my widget, TRUE, TRUE, 0);
		g_signal_connect (sel, "changed", G_CALLBACK (_GuiGtkList_selectionChangedCallback), me);
	#elif win
		my widget = _Gui_initializeWidget (xmListWidgetClass, parent, L"list");
		_GuiObject_setUserData (my widget, me);
		my widget -> window = CreateWindowEx (0, L"listbox", L"list",
			WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | WS_CLIPSIBLINGS |
			( allowMultipleSelection ? LBS_EXTENDEDSEL : 0 ),
			my widget -> x, my widget -> y, my widget -> width, my widget -> height,
			my widget -> parent -> window, NULL, theGui.instance, NULL);
		SetWindowLong (my widget -> window, GWL_USERDATA, (long) my widget);
		SetWindowFont (my widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		/*if (MEMBER (my parent, ScrolledWindow)) {
			XtDestroyWidget (my widget -> parent -> motiff.scrolledWindow.horizontalBar);
			my widget -> parent -> motiff.scrolledWindow.horizontalBar = NULL;
			XtDestroyWidget (my widget -> parent -> motiff.scrolledWindow.verticalBar);
			my widget -> parent -> motiff.scrolledWindow.verticalBar = NULL;
		}*/
		_GuiObject_position (my widget, left, right, top, bottom);
	#elif mac
		my scrolled = XmCreateScrolledWindow (parent, "scrolled", NULL, 0);
		_GuiObject_position (my scrolled, left, right, top, bottom);
		my widget = _Gui_initializeWidget (xmListWidgetClass, my scrolled, L"list");
		_GuiObject_setUserData (my widget, me);
		if (USE_MAC_LISTBOX_CONTROL) {
			CreateListBoxControl (my widget -> macWindow, & my widget -> rect, false, 1000, 1, true, true,
				CELL_HEIGHT, 400, false, NULL, & my widget -> nat.control.handle);
			GetControlData (my widget -> nat.control.handle, kControlEntireControl, kControlListBoxListHandleTag,
				sizeof (my macListHandle), & my macListHandle, NULL);
			SetControlReference (my widget -> nat.control.handle, (long) my widget);
			my widget -> isControl = TRUE;
			_GuiNativeControl_setFont (my widget, 12);
		} else {
			Rect dataBounds = { 0, 0, 0, 1 };
			Point cSize;
			SetPt (& cSize, my widget -> rect.right - my widget -> rect.left + 1, CELL_HEIGHT);
			static ListDefSpec listDefSpec;
			if (listDefSpec. u. userProc == NULL) {
				listDefSpec. defType = kListDefUserProcType;
				listDefSpec. u. userProc = mac_listDefinition;
			}
			CreateCustomList (& my widget -> rect, & dataBounds, cSize, & listDefSpec, my widget -> macWindow,
				false, false, false, false, & my macListHandle);
			SetListRefCon (my macListHandle, (long) my widget);
		}
		if (allowMultipleSelection)
			SetListSelectionFlags (my macListHandle, lExtendDrag | lNoRect);
		XtVaSetValues (my widget, XmNwidth, right > 0 ? right - left + 100 : 530, NULL);
	#elif motif
		my widget = XmCreateScrolledList (parent, "list", NULL, 0);
		_GuiObject_setUserData (my widget, me);
		XtVaSetValues (my widget, XmNselectionPolicy, allowMultipleSelection ? XmEXTENDED_SELECT : XmBROWSE_SELECT, NULL);
		_GuiObject_position (XtParent (my widget), left, right, top, bottom);
		XtAddCallback (my widget, XmNdestroyCallback, _GuiMotifList_destroyCallback, me);
		if (allowMultipleSelection) {
			XtAddCallback (my widget, XmNextendedSelectionCallback, _GuiMotifList_selectionChangedCallback, me);
		} else {
			XtAddCallback (my widget, XmNbrowseSelectionCallback, _GuiMotifList_selectionChangedCallback, me);
		}
		XtAddCallback (my widget, XmNdefaultActionCallback, _GuiMotifList_defaultActionCallback, me);
	#endif
	return my widget;
}

Widget GuiList_createShown (Widget parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header) {
	Widget widget = GuiList_create (parent, left, right, top, bottom, allowMultipleSelection, header);
	GuiObject_show (widget);
	return widget;
}

void GuiList_deleteAllItems (Widget widget) {
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW(widget)));
		gtk_list_store_clear (list_store);
	#elif win
		ListBox_ResetContent (widget -> window);
	#elif mac
		iam_list;
		_GuiMac_clipOnParent (widget);
		LDelRow (0, 0, my macListHandle);
		GuiMac_clipOff ();
	#elif motif
		XmListDeleteAllItems (widget);
	#endif
}

void GuiList_deleteItem (Widget widget, long position) {
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW(widget));
		if (gtk_tree_model_iter_nth_child (tree_model, &iter, NULL, (gint) (position - 1))) {
			gtk_list_store_remove (GTK_LIST_STORE(tree_model), & iter);
		}
	#elif win
		ListBox_DeleteString (widget -> window, position - 1);
	#elif mac
		iam_list;
		_GuiMac_clipOnParent (widget);
		LDelRow (1, position - 1, my macListHandle);
		GuiMac_clipOff ();
		long n = (** my macListHandle). dataBounds. bottom;
		XtVaSetValues (widget, XmNheight, n * CELL_HEIGHT, NULL);
	#elif motif
		XmListDeletePos (widget, position);
	#endif
}

void GuiList_deselectAllItems (Widget widget) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
		gtk_tree_selection_unselect_all (selection);
	#elif win
		ListBox_SetSel (widget -> window, False, -1);
	#elif mac
		iam_list;
		long n = (** my macListHandle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (widget);
		for (long i = 0; i < n; i ++) { cell.v = i; LSetSelect (false, cell, my macListHandle); }
		GuiMac_clipOff ();
	#elif motif
		XmListDeselectAllItems (widget);
	#endif
}

void GuiList_deselectItem (Widget widget, long position) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
/*		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);*/
		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_path_free (path);
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW(widget));
		if (gtk_tree_model_iter_nth_child (tree_model, &iter, NULL, (gint) (position - 1))) {
			gtk_tree_selection_unselect_iter (selection, & iter);
		}
	#elif win
		ListBox_SetSel (widget -> window, False, position - 1);
	#elif mac
		iam_list;
		Cell cell;
		cell. h = 0;
		cell. v = position - 1; 
		_GuiMac_clipOnParent (widget);
		LSetSelect (false, cell, my macListHandle);
		GuiMac_clipOff ();
	#elif motif
		XmListDeselectPos (widget, position);
	#endif
}

long * GuiList_getSelectedPositions (Widget widget, long *numberOfSelectedPositions) {
	*numberOfSelectedPositions = 0;
	long *selectedPositions = NULL;
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
		int n = gtk_tree_selection_count_selected_rows (selection);
		if (n > 0) {
			GList *list = gtk_tree_selection_get_selected_rows (selection, (GtkTreeModel **) &list_store);
			long ipos = 1;
			*numberOfSelectedPositions = n;
			selectedPositions = NUMlvector (1, *numberOfSelectedPositions);
			Melder_assert (selectedPositions != NULL);
			
			GList *l;
			for (l = g_list_first(list); l != NULL; l = g_list_next(l)) {
				gint *index = gtk_tree_path_get_indices (l -> data);
				selectedPositions [ipos] = index [0] + 1;
				ipos ++;
			}
			g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
			g_list_free (list);

			// TODO: probably one big bug
			// Much nicer is: gtk_tree_selection_selected_foreach ()
			// But requires a structure + function
			// Structure must contain the iterator (ipos) and
			// selectedPositions
			// fb: don't think that using the above function would be nicer,
			//     the code is not that confusing  -- 20100223
		}
		return selectedPositions;
	#elif win
		int n = ListBox_GetSelCount (widget -> window), *indices;
		if (n == 0) {
			return selectedPositions;
		}
		if (n == -1) {   // single selection
			int selection = ListBox_GetCurSel (widget -> window);
			if (selection == -1) return False;
			n = 1;
			indices = Melder_calloc (int, n);
			indices [0] = selection;
		} else {
			indices = Melder_calloc (int, n);
			ListBox_GetSelItems (widget -> window, n, indices);
		}
		*numberOfSelectedPositions = n;
		selectedPositions = NUMlvector (1, *numberOfSelectedPositions);
		Melder_assert (selectedPositions != NULL);
		for (long ipos = 1; ipos <= *numberOfSelectedPositions; ipos ++) {
			selectedPositions [ipos] = indices [ipos - 1] + 1;   // convert from zero-based list of zero-based indices
		}
		Melder_free (indices);
	#elif mac
		iam_list;
		long n = (** my macListHandle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		if (n < 1) {
			return selectedPositions;
		}
		selectedPositions = NUMlvector (1, n);   // probably too big (ergo, probably reallocable), but the caller will throw it away anyway
		for (long i = 1; i <= n; i ++) {
			cell. v = i - 1;
			if (LGetSelect (false, & cell, my macListHandle)) {
				selectedPositions [++ *numberOfSelectedPositions] = i;
			}
		}
		if (*numberOfSelectedPositions == 0) {
			NUMlvector_free (selectedPositions, 1);
			selectedPositions = NULL;
		}
	#elif motif
		int position_count, *position_list;
		if (! XmListGetSelectedPos (widget, & position_list, & position_count)) return NULL;
		*numberOfSelectedPositions = position_count;
		selectedPositions = NUMlvector (1, *numberOfSelectedPositions);
		Melder_assert (selectedPositions != NULL);
		for (long ipos = 1; ipos <= *numberOfSelectedPositions; ipos ++) {
			selectedPositions [ipos] = position_list [ipos - 1];
		}
		XtFree ((XtPointer) position_list);
	#endif
	return selectedPositions;
}

long GuiList_getBottomPosition (Widget widget) {
	#if gtk
		// TODO
		return 1;
	#elif win
		long bottom = ListBox_GetTopIndex (widget -> window) + widget -> height / ListBox_GetItemHeight (widget -> window, 0);
		if (bottom < 1) bottom = 1;
		long n = ListBox_GetCount (widget -> window);
		if (bottom > n) bottom = n;
		return bottom;
	#elif mac
		iam_list;
		Melder_assert (widget -> parent -> widgetClass == xmScrolledWindowWidgetClass);
		Widget clipWindow = widget -> parent -> motiff.scrolledWindow.clipWindow;
		Widget workWindow = widget -> parent -> motiff.scrolledWindow.workWindow;
		long top = (clipWindow -> rect.top - workWindow -> rect.top + 5) / CELL_HEIGHT + 1;
		long visible = (clipWindow -> rect.bottom - clipWindow -> rect.top - 5) / CELL_HEIGHT + 1;
		long n = (** my macListHandle). dataBounds. bottom;
		if (visible > n) visible = n;
		long bottom = top + visible - 1;
		if (bottom < 1) bottom = 1;
		if (bottom > n) bottom = n;
		return bottom;
	#elif motif
		int top, visible;
		XtVaGetValues (widget, XmNtopItemPosition, & top, XmNvisibleItemCount, & visible, NULL);
		return top + visible - 1;
	#endif
}

long GuiList_getNumberOfItems (Widget widget) {
	long numberOfItems = 0;
	#if gtk
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (widget));
		numberOfItems = gtk_tree_model_iter_n_children (model, NULL); 
	#elif win
		numberOfItems = ListBox_GetCount (widget -> window);
	#elif mac
		iam_list;
		numberOfItems = (** my macListHandle). dataBounds. bottom;
	#elif motif
		int itemCount;
		XtVaGetValues (widget, XmNitemCount, & itemCount, NULL);
		numberOfItems = itemCount;
	#endif
	return numberOfItems;
}

long GuiList_getTopPosition (Widget widget) {
	#if gtk
		// TODO
		return 1;
	#elif win
		long top = ListBox_GetTopIndex (widget -> window);
		if (top < 1) top = 1;
		long n = ListBox_GetCount (widget -> window);
		if (top > n) top = 0;
		return top;
	#elif mac
		iam_list;
		Melder_assert (widget -> parent -> widgetClass == xmScrolledWindowWidgetClass);
		Widget clipWindow = widget -> parent -> motiff.scrolledWindow.clipWindow;
		Widget workWindow = widget -> parent -> motiff.scrolledWindow.workWindow;
		long top = (clipWindow -> rect.top - workWindow -> rect.top + 5) / CELL_HEIGHT + 1;
		if (top < 1) top = 1;
		long n = (** my macListHandle). dataBounds. bottom;
		if (top > n) top = 0;
		return top;
	#elif motif
		int topPosition;
		XtVaGetValues (widget, XmNtopItemPosition, & topPosition, NULL);
		return topPosition;
	#endif
}

void GuiList_insertItem (Widget widget, const wchar_t *itemText, long position) {
	/*
	 * 'position' is the position of the new item in the list after insertion:
	 * a value of 1 therefore puts the new item at the top of the list;
	 * a value of 0 is special: the item is put at the bottom of the list.
	 */
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
		gtk_list_store_insert_with_values (list_store, NULL, (gint) position - 1, COLUMN_STRING, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
		// does GTK know the '0' trick?
		// it does know about NULL, to append in another function
	#elif win
		if (position)
			ListBox_InsertString (widget -> window, position - 1, itemText);   // win lists start with item 0
		else
			ListBox_AddString (widget -> window, itemText);   // insert at end
	#elif mac
		iam_list;
		long n = (** my macListHandle). dataBounds. bottom;
		if (position == 0)
			position = n + 1;   // insert at end
		Cell cell;
		cell.h = 0; cell. v = position - 1;   // mac lists start with item 0
		_GuiMac_clipOnParent (widget);
		LAddRow (1, position - 1, my macListHandle);
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);   // although defProc will convert again...
		LSetCell (itemText_utf8, (short) strlen (itemText_utf8), cell, my macListHandle);
		(** my macListHandle). visible. bottom = n + 1;
		_GuiMac_clipOffInvalid (widget);
		XtVaSetValues (widget, XmNheight, (n + 1) * CELL_HEIGHT, NULL);
	#elif motif
		XmString itemText_xmstring = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		XmListAddItemUnselected (widget, itemText_xmstring, position);   // Xm knows the '0' trick
		XmStringFree (itemText_xmstring);
	#endif
}

void GuiList_replaceItem (Widget widget, const wchar_t *itemText, long position) {
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW(widget));
		if (gtk_tree_model_iter_nth_child (tree_model, &iter, NULL, (gint) (position - 1))) {
			gtk_list_store_set(GTK_LIST_STORE(tree_model), COLUMN_STRING, Melder_peekWcsToUtf8 (itemText), -1);
		}
/*
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);*/
		// gtk_list_store_set (list_store, & iter, 0, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
	#elif win
		long nativePosition = position - 1;   // convert from 1-based to zero-based
		ListBox_DeleteString (widget -> window, nativePosition);
		ListBox_InsertString (widget -> window, nativePosition, itemText);
	#elif mac
		iam_list;
		_GuiMac_clipOnParent (widget);
		Cell cell;
		cell.h = 0;
		cell.v = position - 1;
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);
		LSetCell (itemText_utf8, strlen (itemText_utf8), cell, my macListHandle);
		LDraw (cell, my macListHandle);
		GuiMac_clipOff ();
	#elif motif
		XmString itemText_xmstring = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		XmListReplaceItemsPos (widget, & itemText_xmstring, 1, position);
		XmStringFree (itemText_xmstring);
	#endif
}

void GuiList_selectItem (Widget widget, long position) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (widget));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position - 1, -1);
		gtk_tree_selection_select_path(selection, path);
		gtk_tree_path_free (path);

// TODO: check of het bovenstaande werkt, dan kan dit weg
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
//		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
//		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_selection_select_iter (selection, & iter);
	#elif win
		iam_list;
		if (! my allowMultipleSelection) {
			ListBox_SetCurSel (widget -> window, position - 1);
		} else {
			ListBox_SetSel (widget -> window, True, position - 1);
		}
	#elif mac
		iam_list;
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (widget);
		if (! my allowMultipleSelection) {
			long n = (** my macListHandle). dataBounds. bottom;
			for (long i = 0; i < n; i ++) if (i != position - 1) {
				cell.v = i;
				LSetSelect (false, cell, my macListHandle);
			}
		}
		cell.v = position - 1; 
		LSetSelect (true, cell, my macListHandle);
		GuiMac_clipOff ();
	#elif motif
		/*
		 * The following hack is necessary in order to ensure
		 * that already selected items are not deselected.
		 */
		unsigned char selectionPolicy;
		XtVaGetValues (widget, XmNselectionPolicy, & selectionPolicy, NULL);
		bool hackNeeded = ( selectionPolicy == XmEXTENDED_SELECT );
		if (hackNeeded) XtVaSetValues (widget, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
		XmListSelectPos (widget, position, False);
		if (hackNeeded) XtVaSetValues (widget, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
	#endif
}

void GuiList_setDoubleClickCallback (Widget widget, void (*callback) (void *boss, GuiListEvent event), void *boss) {
	GuiList me = _GuiObject_getUserData (widget);
	if (me != NULL) {
		my doubleClickCallback = callback;
		my doubleClickBoss = boss;
	}
}

void GuiList_setSelectionChangedCallback (Widget widget, void (*callback) (void *boss, GuiListEvent event), void *boss) {
	GuiList me = _GuiObject_getUserData (widget);
	if (me != NULL) {
		my selectionChangedCallback = callback;
		my selectionChangedBoss = boss;
	}
}

void GuiList_setTopPosition (Widget widget, long topPosition) {
	//Melder_casual ("Set top position %ld", topPosition);
	#if gtk
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (widget)));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) topPosition);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (widget), path, NULL, FALSE, 0.0, 0.0);
		gtk_tree_path_free (path);
	#elif win
		ListBox_SetTopIndex (widget -> window, topPosition - 1);
	#elif mac
		iam_list;
		//_GuiMac_clipOnParent (widget);
		//LScroll (0, topPosition - (** my macListHandle). visible. top - 1, my macListHandle);   // TODO: implement
		//GuiMac_clipOff ();
		//my scrolled -> motiff.scrolledWindow.verticalBar;   // TODO: implement
		XtVaSetValues (widget, XmNy, - (topPosition - 1) * CELL_HEIGHT, NULL);
	#elif motif
		XmListSetPos (widget, topPosition);
	#endif
}

/* End of file GuiList.c */
