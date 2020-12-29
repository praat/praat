/* GuiList.cpp
 *
 * Copyright (C) 1993-2020 Paul Boersma, 2013 Tom Naughton
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

Thing_implement (GuiList, GuiControl, 0);

#if motif
	#define iam_list \
		Melder_assert (widget -> widgetClass == xmListWidgetClass); \
		GuiList me = (GuiList) widget -> userData
#endif

#if gtk
	static void _GuiGtkList_destroyCallback (gpointer void_me) {
		iam (GuiList);
		forget (me);
	}
	static void _GuiGtkList_selectionChangedCallback (GtkTreeSelection *sel, gpointer void_me) {
		iam (GuiList);
		if (my d_selectionChangedCallback && ! my d_blockValueChangedCallbacks) {
			trace (U"Selection changed.");
			struct structGuiList_SelectionChangedEvent event { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
#elif motif
	void _GuiWinList_destroy (GuiObject widget) {
		iam_list;
		DestroyWindow (widget -> window);
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiWinList_map (GuiObject widget) {
		iam_list;
		ShowWindow (widget -> window, SW_SHOW);
	}
	void _GuiWinList_handleClick (GuiObject widget) {
		iam_list;
		if (my d_selectionChangedCallback) {
			struct structGuiList_SelectionChangedEvent event { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
#elif cocoa
	@implementation GuiCocoaList {
		GuiList d_userData;
	}

	/*
	 * Override NSObject methods.
	 */
	- (void) dealloc {
		[_contents release];
		GuiThing me = d_userData;
		forget (me);
		//Melder_casual (U"deleting a list");
		[super dealloc];
	}

	/*
	 * Override NSView methods.
	 */
	- (id) initWithFrame: (NSRect) frameRect {
		self = [super initWithFrame: frameRect];
		if (self) {
			_tableView = [[NSTableView alloc] initWithFrame: frameRect];
			NSTableColumn *tableColumn = [[NSTableColumn alloc] initWithIdentifier: @"list"];
			tableColumn.width = frameRect. size. width;
			[tableColumn setEditable: NO];
			[_tableView addTableColumn: tableColumn];

			_tableView. delegate = self;
			_tableView. dataSource = self;
			_tableView. allowsEmptySelection = YES;
			_tableView. headerView = nil;
			_tableView. target = self;
			_tableView. action = @selector (_GuiCocoaList_clicked:);

			NSScrollView *scrollView = [[NSScrollView alloc] initWithFrame: frameRect];
			[scrollView setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			[scrollView setBorderType: NSBezelBorder];
			[scrollView setDocumentView: _tableView];   // this retains the table view
			[scrollView setHasVerticalScroller: YES];
			//[scrollView setHasHorizontalScroller: YES];

			[self addSubview: scrollView];   // this retains the scroll view
			[scrollView release];
			[_tableView release];

			_contents = [[NSMutableArray alloc] init];
		}
		return self;
	}

	/*
	 * Implement GuiCocoaAny protocol.
	 */
	- (GuiThing) getUserData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == nullptr || Thing_isa (userData, classGuiList));
		d_userData = static_cast <GuiList> (userData);
	}

	/*
	 * Implement GuiCocoaList methods.
	 */
	- (IBAction) _GuiCocoaList_clicked: (id) sender {
		/*
		 * This method probably shouldn't do anything,
		 * because tableViewSelectionDidChange will already have been called at this point.
		 */
		(void) sender;
		trace (U"enter");
		GuiList me = d_userData;
		if (me && my d_selectionChangedCallback) {
			//struct structGuiList_SelectionChangedEvent event { me };
			//my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}

	/*
	 * Override TableViewDataSource methods.
	 */
	- (NSInteger) numberOfRowsInTableView: (NSTableView *) tableView {
		(void) tableView;
		return [_contents count];
	}
	- (id) tableView:  (NSTableView *) tableView   objectValueForTableColumn: (NSTableColumn *) tableColumn   row: (NSInteger) row {
		(void) tableColumn;
		(void) tableView;
		return [_contents   objectAtIndex: row];
	}

	/*
	 * Override TableViewDelegate methods.
	 */
	- (void) tableViewSelectionDidChange: (NSNotification *) notification {
		/*
		 * This is invoked when the user clicks in the table or uses the arrow keys.
		 */
		(void) notification;
		trace (U"enter");
		GuiList me = d_userData;
		if (me && my d_selectionChangedCallback && ! my d_blockValueChangedCallbacks) {
			struct structGuiList_SelectionChangedEvent event { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
	@end
#endif

#if gtk
	enum {
	  COLUMN_STRING,
	  N_COLUMNS
	};
#endif

GuiList GuiList_create (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, conststring32 header) {
	autoGuiList me = Thing_new (GuiList);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_allowMultipleSelection = allowMultipleSelection;
	#if gtk
		GtkCellRenderer *renderer = nullptr;
		GtkTreeViewColumn *col = nullptr;
		GtkTreeSelection *sel = nullptr;
		GtkListStore *liststore = nullptr;

		liststore = gtk_list_store_new (1, G_TYPE_STRING);   // 1 column, of type String (this is a vararg list)
		GuiObject scrolled = gtk_scrolled_window_new (nullptr, nullptr);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		my d_widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (my d_widget), false);
		gtk_container_add (GTK_CONTAINER (scrolled), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (scrolled));   // BUG
		gtk_tree_view_set_rubber_banding (GTK_TREE_VIEW (my d_widget), allowMultipleSelection ? GTK_SELECTION_MULTIPLE : GTK_SELECTION_SINGLE);
		g_object_unref (liststore);   // Destroys the widget after the list is destroyed

		_GuiObject_setUserData (my d_widget, me.get());
		_GuiObject_setUserData (scrolled, me.get());   // for resizing

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, true);
		gtk_tree_view_column_add_attribute (col, renderer, "text", 0);   // zeroeth column
		if (header) {
			//gtk_tree_view_column_set_title (col, Melder_peek32to8 (header));
		}
		gtk_tree_view_append_column (GTK_TREE_VIEW (my d_widget), col);

		g_object_set_data_full (G_OBJECT (my d_widget), "guiList", me.get(), (GDestroyNotify) _GuiGtkList_destroyCallback);

/*		GtkCellRenderer *renderer;
		GtkTreeViewColumn *col;
		
		my widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, true);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_ID);
		gtk_tree_view_column_set_title (col, " ID ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);
		
		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, true);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_TYPE);
		gtk_tree_view_column_set_title (col, " Type ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, true);
		gtk_tree_view_column_add_attribute (col, renderer, "text", COL_NAME);
		gtk_tree_view_column_set_title (col, " Name ");
		gtk_tree_view_append_column (GTK_TREE_VIEW (view), col);
*/

		sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
		if (allowMultipleSelection) {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_MULTIPLE);
		} else {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_SINGLE);
		}
		my v_positionInForm (scrolled, left, right, top, bottom, parent);
		g_signal_connect (sel, "changed", G_CALLBACK (_GuiGtkList_selectionChangedCallback), me.get());
	#elif motif
		my d_widget = _Gui_initializeWidget (xmListWidgetClass, parent -> d_widget, U"list");
		_GuiObject_setUserData (my d_widget, me.get());
		my d_widget -> window = CreateWindowEx (0, L"listbox", L"list",
			WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | WS_CLIPSIBLINGS |
			( allowMultipleSelection ? LBS_EXTENDEDSEL : 0 ),
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, nullptr, theGui.instance, nullptr);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), false);
		/*if (MEMBER (my parent, ScrolledWindow)) {
			XtDestroyWidget (my d_widget -> parent -> motiff.scrolledWindow.horizontalBar);
			my d_widget -> parent -> motiff.scrolledWindow.horizontalBar = nullptr;
			XtDestroyWidget (my d_widget -> parent -> motiff.scrolledWindow.verticalBar);
			my d_widget -> parent -> motiff.scrolledWindow.verticalBar = nullptr;
		}*/
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif cocoa
		(void) header;
		GuiCocoaList *list = [[GuiCocoaList alloc] init];
		my d_widget = (GuiObject) list;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[[list tableView] setAllowsMultipleSelection: allowMultipleSelection];
		[list setUserData: me.get()];
	#endif
	return me.releaseToAmbiguousOwner();
}

GuiList GuiList_createShown (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, conststring32 header) {
	GuiList me = GuiList_create (parent, left, right, top, bottom, allowMultipleSelection, header);
	GuiThing_show (me);
	return me;
}

void GuiList_deleteAllItems (GuiList me) {
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
		gtk_list_store_clear (list_store);
	#elif motif
		ListBox_ResetContent (my d_widget -> window);
	#elif cocoa
        GuiCocoaList *list = (GuiCocoaList *) my d_widget;
        [list. contents   removeAllObjects];
        [list. tableView   reloadData];
	#endif
}

void GuiList_deleteItem (GuiList me, integer position) {
	Melder_assert (position >= 1);   // so that we can subtract 1 even if the result has to be unsigned
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, nullptr, (gint) (position - 1))) {
			gtk_list_store_remove (GTK_LIST_STORE (tree_model), & iter);
		}
	#elif motif
		ListBox_DeleteString (my d_widget -> window, position - 1);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		[list. contents   removeObjectAtIndex: (NSUInteger) (position - 1)];
		[list. tableView   reloadData];
	#endif
}

void GuiList_deselectAllItems (GuiList me) {
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
		gtk_tree_selection_unselect_all (selection);
	#elif motif
		ListBox_SetSel (my d_widget -> window, False, -1);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		[list. tableView   deselectAll: nil];
	#endif
}

void GuiList_deselectItem (GuiList me, integer position) {
	Melder_assert (position >= 1);   // so that we can subtract 1 even if the result has to be unsigned
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
//		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position, -1 /* terminator */);
		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_path_free (path);
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, nullptr, (gint) (position - 1))) {
			gtk_tree_selection_unselect_iter (selection, & iter);
		}
	#elif motif
		ListBox_SetSel (my d_widget -> window, False, position - 1);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		[list. tableView   deselectRow: position - 1];
	#endif
}

autoINTVEC GuiList_getSelectedPositions (GuiList me) {
	autoINTVEC selectedPositions;
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
		int n = gtk_tree_selection_count_selected_rows (selection);
		if (n > 0) {
			GList *list = gtk_tree_selection_get_selected_rows (selection, (GtkTreeModel **) & list_store);
			integer ipos = 1;
			selectedPositions = zero_INTVEC (n);
			for (GList *l = g_list_first (list); l != nullptr; l = g_list_next (l)) {
				gint *index = gtk_tree_path_get_indices ((GtkTreePath *) l -> data);
				selectedPositions [ipos] = index [0] + 1;
				ipos ++;
			}
			g_list_foreach (list, (GFunc) gtk_tree_path_free, nullptr);
			g_list_free (list);
		}
		return selectedPositions;
	#elif motif
		int n = ListBox_GetSelCount (my d_widget -> window), *indices;
		if (n == 0)
			return selectedPositions;
		if (n == -1) {   // single selection
			int selection = ListBox_GetCurSel (my d_widget -> window);
			if (selection == -1)
				return selectedPositions;
			n = 1;
			indices = Melder_calloc_f (int, n);
			indices [0] = selection;
		} else {
			indices = Melder_calloc_f (int, n);
			ListBox_GetSelItems (my d_widget -> window, n, indices);
		}
		selectedPositions = zero_INTVEC (n);
		for (integer ipos = 1; ipos <= n; ipos ++)
			selectedPositions [ipos] = indices [ipos - 1] + 1;   // convert from zero-based list of zero-based indices
		Melder_free (indices);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		NSIndexSet *indexSet = [list. tableView   selectedRowIndexes];
		selectedPositions = zero_INTVEC (uinteger_to_integer ([indexSet count]));
		NSUInteger currentIndex = [indexSet firstIndex];
		integer ipos = 0;
		while (currentIndex != NSNotFound) {
			selectedPositions [++ ipos] = uinteger_to_integer (currentIndex + 1);
			currentIndex = [indexSet   indexGreaterThanIndex: currentIndex];
		}
		Melder_assert (ipos == selectedPositions.size);
	#endif
	return selectedPositions;
}

integer GuiList_getBottomPosition (GuiList me) {
	#if gtk
		GtkTreePath *path;
		integer position = 1;
		if (gtk_tree_view_get_visible_range (GTK_TREE_VIEW (my d_widget), nullptr, & path)) {
			int *indices = gtk_tree_path_get_indices (path);
			position = indices ? indices[0] + 1 : 1;
			gtk_tree_path_free (path); // also frees indices !!
		}
		trace (U"bottom: ", position);
		return position;
	#elif motif
		integer bottom = ListBox_GetTopIndex (my d_widget -> window) + my d_widget -> height / ListBox_GetItemHeight (my d_widget -> window, 0);
		if (bottom < 1) bottom = 1;
		integer n = ListBox_GetCount (my d_widget -> window);
		if (bottom > n) bottom = n;
		return bottom;
	#elif cocoa
		return 1;   // TODO
	#else
		return 0;
	#endif
}

integer GuiList_getNumberOfItems (GuiList me) {
	integer numberOfItems = 0;
	#if gtk
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget));
		numberOfItems = gtk_tree_model_iter_n_children (model, nullptr);
	#elif motif
		numberOfItems = ListBox_GetCount (my d_widget -> window);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		numberOfItems = [[list contents] count];
	#endif
	return numberOfItems;
}

integer GuiList_getTopPosition (GuiList me) {
	#if gtk
		GtkTreePath *path;
		integer position = 1;
		if (gtk_tree_view_get_visible_range (GTK_TREE_VIEW (my d_widget), & path, nullptr)) {
			int *indices = gtk_tree_path_get_indices (path);
			position = indices ? indices[0] + 1 : 1;
			gtk_tree_path_free (path); // also frees indices !!
		}
		trace (U"top: ", position);
		return position;
	#elif motif
		integer top = ListBox_GetTopIndex (my d_widget -> window);
		if (top < 1) top = 1;
		integer n = ListBox_GetCount (my d_widget -> window);
		if (top > n) top = 0;
		return top;
	#elif cocoa
		return 1;   // TODO
	#else
		return 0;
	#endif
}

void GuiList_insertItem (GuiList me, conststring32 itemText /* cattable */, integer position_base1) {
	bool explicitlyInsertAtEnd = ( position_base1 <= 0 );
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
		gtk_list_store_insert_with_values (list_store, nullptr, explicitlyInsertAtEnd ? 1000000000 : (gint) position_base1 - 1, COLUMN_STRING, Melder_peek32to8 (itemText), -1);
		// TODO: Tekst opsplitsen
		// does GTK know the '0' trick?
		// it does know about nullptr, to append in another function
	#elif motif
		HWND nativeList = my d_widget -> window;
		conststringW nativeItemText = Melder_peek32toW (itemText);
		if (explicitlyInsertAtEnd) {
			ListBox_AddString (nativeList, nativeItemText);
		} else {
			int nativePosition_base0 = position_base1 - 1;
			ListBox_InsertString (nativeList, nativePosition_base0, nativeItemText);
		}
	#elif cocoa
		GuiCocoaList *nativeList = (GuiCocoaList *) my d_widget;
		NSString *nativeItemText = [[NSString alloc] initWithUTF8String: Melder_peek32to8 (itemText)];
		if (explicitlyInsertAtEnd) {
			[[nativeList contents]   addObject: nativeItemText];
		} else {
			NSUInteger nativePosition_base0 = (uinteger) position_base1 - 1;
			[[nativeList contents]   insertObject: nativeItemText   atIndex: nativePosition_base0];
		}
		[nativeItemText release];
		[[nativeList tableView] reloadData];
	#endif
}

void GuiList_replaceItem (GuiList me, conststring32 itemText, integer position) {
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, nullptr, (gint) (position - 1))) {
			gtk_list_store_set (GTK_LIST_STORE (tree_model), & iter, COLUMN_STRING, Melder_peek32to8 (itemText), -1);
		}
/*
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position, -1);   // -1 = terminator
		GtkTreeIter iter;
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);*/
		// gtk_list_store_set (list_store, & iter, 0, Melder_peek32to8 (itemText), -1);
		// TODO: Tekst opsplitsen
	#elif motif
		integer nativePosition = position - 1;   // convert from 1-based to zero-based
		ListBox_DeleteString (my d_widget -> window, nativePosition);
		ListBox_InsertString (my d_widget -> window, nativePosition, Melder_peek32toW (itemText));
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		NSString *nsString = [[NSString alloc] initWithUTF8String: Melder_peek32to8 (itemText)];
		[[list contents]   replaceObjectAtIndex: position - 1   withObject: nsString];
		[nsString release];
		[[list tableView] reloadData];
	#endif
}

void GuiList_selectItem (GuiList me, integer position) {
	Melder_assert (position >= 1);   // so that we can subtract 1 even if the result has to be unsigned
	GuiControlBlockValueChangedCallbacks block (me);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position - 1, -1 /* terminator */);
		gtk_tree_selection_select_path (selection, path);
		gtk_tree_path_free (path);

// TODO: check of het bovenstaande werkt, dan kan dit weg
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my d_widget)));
//		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position, -1 /* terminator */);
//		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_selection_select_iter (selection, & iter);
	#elif motif
		if (! my d_allowMultipleSelection) {
			ListBox_SetCurSel (my d_widget -> window, position - 1);
		} else {
			ListBox_SetSel (my d_widget -> window, True, position - 1);
		}
	#elif cocoa
		NSIndexSet *indexSet = [[NSIndexSet alloc] initWithIndex: NSUInteger (position - 1)];
		GuiCocoaList *list = (GuiCocoaList *) my d_widget;
		[[list tableView]   selectRowIndexes: indexSet   byExtendingSelection: my d_allowMultipleSelection];
		[indexSet release];
	#endif
}

void GuiList_setSelectionChangedCallback (GuiList me, GuiList_SelectionChangedCallback callback, Thing boss) {
	my d_selectionChangedCallback = callback;
	my d_selectionChangedBoss = boss;
}

void GuiList_setDoubleClickCallback (GuiList me, GuiList_DoubleClickCallback callback, Thing boss) {
	my d_doubleClickCallback = callback;
	my d_doubleClickBoss = boss;
}

void GuiList_setScrollCallback (GuiList me, GuiList_ScrollCallback callback, Thing boss) {
	my d_scrollCallback = callback;
	my d_scrollBoss = boss;
}

void GuiList_setTopPosition (GuiList me, integer topPosition) {
	trace (U"Set top position ", topPosition);
	#if gtk
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (my md_widget)));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) topPosition, -1 /* terminator */);   // BUG?
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (my d_widget), path, nullptr, false, 0.0, 0.0);
		gtk_tree_path_free (path);
	#elif motif
		ListBox_SetTopIndex (my d_widget -> window, topPosition - 1);
	#elif cocoa
	 // TODO: implement
	#endif
}

/* End of file GuiList.cpp */
