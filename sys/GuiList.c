/* GuiList.c
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
 * pb 2007/12/15 abstraction from motif
 */

#include "GuiP.h"
#include "NUM.h"
#define my  me ->
#define iam(x)  x me = (x) void_me

#if win
	#define CELL_HEIGHT  15
#elif mac
	#define CELL_HEIGHT  16
#endif

typedef struct structGuiList {
	void (*selectionChangedCallback) (Widget me, void *closure);
	void *selectionChangedClosure;
	#if gtk
		GtkListStore *liststore;
	#endif
} *GuiList;

#if gtk
	static void gtk_GuiList_destroyCallback (Widget widget, gpointer void_me) {
		(void) widget;
		iam (GuiList);
		Melder_free (me);
	}
	static void gtk_GuiList_selectionChangedCallback (GtkTreeSelection *sel, gpointer void_me) {
		iam (GuiList);
		// TODO
	}
#else
	static void motif_GuiList_destroyCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiList);
		Melder_free (me);
	}
	static void motif_GuiList_selectionChangedCallback (Widget widget, XtPointer void_me, XtPointer call) {
		(void) widget; (void) call;
		iam (GuiList);
		Melder_assert (me != NULL);
		if (my selectionChangedCallback != NULL) {
			my selectionChangedCallback (widget, my selectionChangedClosure);
		}
	}
#endif

Widget GuiList_create (Widget parent, int left, int right, int top, int bottom, bool allowMultipleSelection) {
	Widget me = NULL;
	GuiList guiList = Melder_calloc (struct structGuiList, 1);
	#if gtk
		g_object_set_data_full (widget, "guiList", guiList, destroyCallback); 
		// TODO Maken list store? Waar en wanneer.

		GtkTreeSelection *sel;
		GtkCellRenderer *renderer;
		GtkTreeViewColumn *col;
		
		me = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));

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

		sel = gtk_tree_view_get_selection (GTK_TREE_VIEW(me));
		if (allowMultiple) {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_MULTIPLE);
		} else {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_SINGLE);
		}
		g_signal_connect (sel, "changed", G_CALLBACK (gtk_GuiList_SelectionChangedCallback), NULL);
	#else
		#if win
			me = XtVaCreateWidget ("list", xmListWidgetClass, parent,
				XmNselectionPolicy, allowMultipleSelection ? XmEXTENDED_SELECT : XmBROWSE_SELECT, NULL);
			_GuiObject_position (me, left, right, top, bottom);
		#elif mac
			Widget scrolled = XmCreateScrolledWindow (parent, "scrolled", NULL, 0);
			_GuiObject_position (scrolled, left, right, top, bottom);
			me = XtVaCreateWidget ("list", xmListWidgetClass, scrolled,
				XmNselectionPolicy, allowMultipleSelection ? XmEXTENDED_SELECT : XmBROWSE_SELECT, NULL);
			XtVaSetValues (me, XmNwidth, right > 0 ? right - left + 100 : 530, NULL);
		#elif motif
			me = XmCreateScrolledList (parent, "list", NULL, 0);
			XtVaSetValues (me, XmNselectionPolicy, allowMultipleSelection ? XmEXTENDED_SELECT : XmBROWSE_SELECT, NULL);
			_GuiObject_position (XtParent (me), left, right, top, bottom);
		#endif
		_GuiObject_setUserData (me, guiList);
		XtAddCallback (me, XmNdestroyCallback, motif_GuiList_destroyCallback, guiList);
		if (allowMultipleSelection) {
			XtAddCallback (me, XmNextendedSelectionCallback, motif_GuiList_selectionChangedCallback, guiList);
		} else {
			XtAddCallback (me, XmNbrowseSelectionCallback, motif_GuiList_selectionChangedCallback, guiList);
		}
	#endif
	return me;
}

void GuiList_deleteAllItems (Widget me) {
	#if gtk
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		gtk_list_store_clear (list_store);
	#elif win
		ListBox_ResetContent (my window);
	#elif mac
		_GuiMac_clipOnParent (me);
		LDelRow (0, 0, my nat.list.handle);
		GuiMac_clipOff ();
	#elif motif
		XmListDeleteAllItems (me);
	#endif
}

void GuiList_deleteItem (Widget me, long position) {
	#if gtk
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);
		gtk_list_store_remove (list_store, & iter);
	#elif win
		ListBox_DeleteString (my window, position - 1);
	#elif mac
		_GuiMac_clipOnParent (me);
		LDelRow (1, position - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#elif motif
		XmListDeletePos (me, position);
	#endif
}

void GuiList_deselectAllItems (Widget me) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (me);
		gtk_tree_selection_unselect_all (selection);
	#elif win
		ListBox_SetSel (my window, False, -1);
	#elif mac
		int i, n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (me);
		for (i = 0; i < n; i ++) { cell.v = i; LSetSelect (false, cell, my nat.list.handle); }
		GuiMac_clipOff ();
	#elif motif
		XmListDeselectAllItems (me);
	#endif
}

void GuiList_deselectItem (Widget me, long position) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (me);
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);
		gtk_tree_selection_unselect_iter (selection, & iter);
	#elif win
		ListBox_SetSel (my window, False, position - 1);
	#elif mac
		Cell cell;
		cell. h = 0;
		cell. v = position - 1; 
		_GuiMac_clipOnParent (me);
		LSetSelect (false, cell, my nat.list.handle);
		GuiMac_clipOff ();
	#elif motif
		XmListDeselectPos (me, position);
	#endif
}

long * GuiList_getSelectedPositions (Widget me, long *numberOfSelectedPositions) {
	*numberOfSelectedPositions = 0;
	long *selectedPositions = NULL;
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (me);
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		int n = gtk_tree_selection_count_selected_rows (selection);
		if (n > 0) {
			GList *list = gtk_tree_selection_get_selected_rows (selection, & GTK_TREE_MODEL (list_store));
			long ipos = 1;
			*numberOfSelectedPositions = n;
			selectedPositions = NUMlvector (1, *numberOfSelectedPositions);
			Melder_assert (selectedPositions != NULL);
			g_list_first (list);
			while (g_list_next (list) != NULL) {
				gint *index = gtk_tree_path_get_indices (list -> data);
				selectedPositions [ipos] = index [0];
				ipos ++;
			}
			g_list_foreach (list, gtk_tree_path_free, NULL);
			g_list_free (list);

			// TODO: probably one big bug
			// Much nicer is: gtk_tree_selection_selected_foreach ()
			// But requires a structure + function
			// Structure must contain the iterator (ipos) and
			// selectedPositions
		}
		return selectedPositions;
	#elif win
		int n = ListBox_GetSelCount (my window), *indices;
		if (n == 0) {
			return selectedPositions;
		}
		if (n == -1) {   // single selection
			int selection = ListBox_GetCurSel (my window);
			if (selection == -1) return False;
			n = 1;
			indices = Melder_calloc (int, n);
			indices [0] = selection;
		} else {
			indices = Melder_calloc (int, n);
			ListBox_GetSelItems (my window, n, indices);
		}
		*numberOfSelectedPositions = n;
		selectedPositions = NUMlvector (1, *numberOfSelectedPositions);
		Melder_assert (selectedPositions != NULL);
		for (long ipos = 1; ipos <= *numberOfSelectedPositions; ipos ++) {
			selectedPositions [ipos] = indices [ipos - 1] + 1;   // convert from zero-based list of zero-based indices
		}
		Melder_free (indices);
	#elif mac
		int n = (** my nat.list.handle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		if (n < 1) {
			return selectedPositions;
		}
		selectedPositions = NUMlvector (1, n);   // probably too big (ergo, probably reallocable), but the caller will throw it away anyway
		for (long i = 1; i <= n; i ++) {
			cell. v = i - 1;
			if (LGetSelect (false, & cell, my nat.list.handle)) {
				selectedPositions [++ *numberOfSelectedPositions] = i;
			}
		}
		if (*numberOfSelectedPositions == 0) {
			NUMlvector_free (selectedPositions, 1);
		}
	#elif motif
		int position_count, *position_list;
		if (! XmListGetSelectedPos (me, & position_list, & position_count)) return NULL;
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

void GuiList_insertItem (Widget me, const wchar_t *itemText, long position) {
	/*
	 * 'position' is the position of the new item in the list after insertion:
	 * a value of 1 therefore puts the new item at the top of the list;
	 * a value of 0 is special: the item is put at the bottom of the list.
	 */
	#if gtk
		GtkTreeIter iter;
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		gtk_list_store_insert_with_values (list_store, & iter, (gint) position, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
		// does GTK know the '0' trick?
		// it does know about NULL, to append in another function
	#elif win
		if (position)
			ListBox_InsertString (my window, position - 1, itemText);   // win lists start with item 0
		else
			ListBox_AddString (my window, itemText);   // insert at end
	#elif mac
		Cell cell;
		int n = (** my nat.list.handle). dataBounds. bottom;
		if (position == 0)
			position = n + 1;   // insert at end
		cell.h = 0; cell. v = position - 1;   // mac lists start with item 0
		_GuiMac_clipOnParent (me);
		LAddRow (1, position - 1, my nat.list.handle);
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);   // although defProc will convert again...
		LSetCell (itemText_utf8, (short) strlen (itemText_utf8), cell, my nat.list.handle);
		(** my nat.list.handle). visible. bottom = n + 1;
		_GuiMac_clipOffInvalid (me);
		XtVaSetValues (me, XmNheight, (n + 1) * CELL_HEIGHT, NULL);
	#elif motif
		XmString itemText_xmstring = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		XmListAddItemUnselected (me, itemText_xmstring, position);   // Xm knows the '0' trick
		XmStringFree (itemText_xmstring);
	#endif
}

void GuiList_replaceItem (Widget me, const wchar_t *itemText, long position) {
	#if gtk
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);
		gtk_list_store_set (list_store, & iter, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
	#elif win
		long nativePosition = position - 1;   // convert from 1-based to zero-based
		ListBox_DeleteString (my window, nativePosition);
		ListBox_InsertString (my window, nativePosition, itemText);
	#elif mac
		Cell cell;
		cell.h = 0;
		cell.v = position - 1;
		_GuiMac_clipOnParent (me);
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);
		LSetCell (itemText_utf8, strlen (itemText_utf8), cell, my nat.list.handle);
		LDraw (cell, my nat.list.handle);
		GuiMac_clipOff ();
	#elif motif
		XmString itemText_xmstring = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		XmListReplaceItemsPos (me, & itemText_xmstring, 1, position);
		XmStringFree (itemText_xmstring);
	#endif
}

void GuiList_scroll (Widget me, long topPosition) {
	#if gtk
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) topPosition);
		gtk_tree_view_scroll_to_cell (me, path, NULL, FALSE, 0.0, 0.0);
		gtk_tree_path_free (path);
	#elif win
		ListBox_SetTopIndex (my window, topPosition - 1);
	#elif mac
		_GuiMac_clipOnParent (me);
		LScroll (0, topPosition - (** my nat.list.handle). visible. top - 1, my nat.list.handle);
		GuiMac_clipOff ();
	#elif motif
		XmListSetPos (me, topPosition);
	#endif
}

void GuiList_selectItem (Widget me, long position, bool notify) {
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (me);
		GtkListStore list_store = GTK_LISTSTORE (gtk_tree_view_get_model (me));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);
		gtk_tree_selection_select_iter (selection, & iter);

		// extendedSelectionCallback needed? <- TODO: ?
	#elif win
		if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT) {
			ListBox_SetCurSel (my window, position - 1);
		} else {
			ListBox_SetSel (my window, True, position - 1);
		}
		if (notify && my extendedSelectionCallback)
			my extendedSelectionCallback (me, my extendedSelectionClosure, NULL);
	#elif mac
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (me);
		if (my selectionPolicy == XmSINGLE_SELECT || my selectionPolicy == XmBROWSE_SELECT) {
			int i, n = (** my nat.list.handle). dataBounds. bottom;
			for (i = 0; i < n; i ++) if (i != position - 1) {
				cell.v = i;
				LSetSelect (false, cell, my nat.list.handle);
			}
		}
		cell.v = position - 1; 
		LSetSelect (true, cell, my nat.list.handle);
		GuiMac_clipOff ();
		if (notify && my extendedSelectionCallback)
			my extendedSelectionCallback (me, my extendedSelectionClosure, NULL);
	#elif motif
		/*
		 * The following hack is necessary in order to ensure
		 * that already selected items are not deselected.
		 */
		unsigned char selectionPolicy;
		XtVaGetValues (me, XmNselectionPolicy, & selectionPolicy, NULL);
		bool hackNeeded = ( selectionPolicy == XmEXTENDED_SELECT );
		if (hackNeeded) XtVaSetValues (me, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
		XmListSelectPos (me, position, notify);
		if (hackNeeded) XtVaSetValues (me, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
	#endif
}

void GuiList_setSelectionChangedCallback (Widget me, void (*callback) (Widget widget, void *closure), XtPointer closure) {
	GuiList guiList = _GuiObject_getUserData (me);
	if (guiList != NULL) {
		guiList -> selectionChangedCallback = callback;
		guiList -> selectionChangedClosure = closure;
	}
}

/* End of file GuiList.c */
