/* GuiList.cpp
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
 * pb 2007/12/26 abstraction from Motif
 * pb 2009/01/31 NUMlvector_free has to be followed by assigning a NULL
 * fb 2010/02/23 GTK
 * pb 2010/06/14 HandleControlClick
 * pb 2010/07/05 blockSelectionChangedCallback
 * pb 2010/11/28 removed Motif
 * pb 2011/04/06 C++
 */

#include "GuiP.h"
#include "NUM.h"

Thing_implement (GuiList, GuiControl, 0);

#undef iam
#define iam(x)  x me = (x) void_me
#if win || mac
	#define iam_list \
		Melder_assert (widget -> widgetClass == xmListWidgetClass); \
		GuiList me = (GuiList) widget -> userData
#else
	#define iam_list \
		GuiList me = (GuiList) _GuiObject_getUserData (widget)
#endif

#if win
	#define CELL_HEIGHT  15
#elif mac
	#define CELL_HEIGHT  18
#endif

#if gtk
	static void _GuiGtkList_destroyCallback (gpointer void_me) {
		iam (GuiList);
		forget (me);
	}
	static void _GuiGtkList_selectionChangedCallback (GtkTreeSelection *sel, gpointer void_me) {
		iam (GuiList);
		if (my d_selectionChangedCallback != NULL && ! my d_blockValueChangedCallbacks) {
			//Melder_casual ("Selection changed.");
			struct structGuiListEvent event = { me };
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
		Melder_casual ("deleting a list");
		[super dealloc];
	}

	/*
	 * Override NSView methods.
	 */
	- (id) initWithFrame: (NSRect) frameRect {
		self = [super initWithFrame: frameRect];
		if (self) {
			_tableView = [[NSTableView alloc] initWithFrame: frameRect];
			Melder_assert ([_tableView retainCount] == 1);   // this asserts that ARC is off (if ARC were on, the retain count would be 2, because tableView is a retain property)
			NSTableColumn *tc = [[NSTableColumn alloc] initWithIdentifier: @"list"];
			tc.width = frameRect. size. width;
			[tc setEditable: NO];
			[_tableView addTableColumn: tc];
			
			_tableView. delegate = self;
			_tableView. dataSource = self;
			_tableView. allowsEmptySelection = YES;
			_tableView. headerView = nil;
			_tableView. target = self;
			_tableView. action = @selector (_GuiCocoaList_clicked:);
			
			NSScrollView *sv = [[NSScrollView alloc] initWithFrame: frameRect];
			[sv setAutoresizingMask: NSViewWidthSizable | NSViewHeightSizable];
			[sv setBorderType: NSGrooveBorder];
			[sv setDocumentView: _tableView];   // this retains the table view
			[sv setHasVerticalScroller: YES];
			//[sv setHasHorizontalScroller: YES];
			
			[self addSubview: sv];   // this retains the scroll view
			//Melder_assert ([sv retainCount] == 2);   // not always true on 10.6
			[sv release];
			Melder_assert ([_tableView retainCount] == 2);
			[_tableView release];
			
			_contents = [[NSMutableArray alloc] init];
		}
		return self;
	}

	/*
	 * Implement GuiCocoaAny protocol.
	 */
	- (GuiThing) userData {
		return d_userData;
	}
	- (void) setUserData: (GuiThing) userData {
		Melder_assert (userData == NULL || Thing_member (userData, classGuiList));
		d_userData = static_cast <GuiList> (userData);
	}

	/*
	 * Implement GuiCocaList methods.
	 */
	- (IBAction) _GuiCocoaList_clicked: (id) sender {
		(void) sender;
		GuiList me = d_userData;
		if (me && my d_selectionChangedCallback) {
			struct structGuiListEvent event = { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
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
		(void) notification;
		trace ("enter");
		GuiList me = d_userData;
		if (me && my d_selectionChangedCallback && ! my d_blockValueChangedCallbacks) {
			struct structGuiListEvent event = { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
	@end
#elif win
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
		if (my d_selectionChangedCallback != NULL) {
			struct structGuiListEvent event = { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
#elif mac
	void _GuiMacList_destroy (GuiObject widget) {
		iam_list;
		_GuiMac_clipOnParent (widget);
		if (widget -> isControl) {
			DisposeControl (widget -> nat.control.handle);
		} else {
			LDispose (my d_macListHandle);
		}
		GuiMac_clipOff ();
		forget (me);   // NOTE: my widget is not destroyed here
	}
	void _GuiMacList_map (GuiObject widget) {
		iam_list;
		if (widget -> isControl) {
			_GuiNativeControl_show (widget);
			Melder_casual ("showing a list");
			//_GuiMac_clipOnParent (widget);
			//LSetDrawingMode (true, my macListHandle);
			//_GuiMac_clipOffInvalid (widget);
		} else {
			_GuiMac_clipOnParent (widget);
			LSetDrawingMode (true, my d_macListHandle);
			_GuiMac_clipOffInvalid (widget);
		}
	}
	void _GuiMacList_activate (GuiObject widget, bool activate) {
		iam_list;
		_GuiMac_clipOnParent (widget);
		LActivate (activate, my d_macListHandle);
		GuiMac_clipOff ();
	}
	void _GuiMacList_handleControlClick (GuiObject widget, EventRecord *macEvent) {
		iam_list;
		_GuiMac_clipOnParent (widget);
		bool pushed = HandleControlClick (widget -> nat.control.handle, macEvent -> where, macEvent -> modifiers, NULL);
		GuiMac_clipOff ();
		if (pushed && my d_selectionChangedCallback) {
			struct structGuiListEvent event = { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
	}
	void _GuiMacList_handleClick (GuiObject widget, EventRecord *macEvent) {
		iam_list;
		_GuiMac_clipOnParent (widget);
		bool doubleClick = LClick (macEvent -> where, macEvent -> modifiers, my d_macListHandle);
		GuiMac_clipOff ();
		if (my d_selectionChangedCallback) {
			struct structGuiListEvent event = { me };
			my d_selectionChangedCallback (my d_selectionChangedBoss, & event);
		}
		if (doubleClick && my d_doubleClickCallback) {
			struct structGuiListEvent event = { me };
			my d_doubleClickCallback (my d_doubleClickBoss, & event);
		}
	}
	void _GuiMacList_move (GuiObject widget) {
		iam_list;
		(** my d_macListHandle). rView = widget -> rect;
	}
	void _GuiMacList_resize (GuiObject widget) {
		iam_list;
		(** my d_macListHandle). rView = widget -> rect;
		SetPortWindowPort (widget -> macWindow);
		(** my d_macListHandle). cellSize. h = widget -> width;
		if (widget -> parent -> widgetClass == xmScrolledWindowWidgetClass)
			_Gui_manageScrolledWindow (widget -> parent);
	}
	void _GuiMacList_shellResize (GuiObject widget) {
		iam_list;
		(** my d_macListHandle). rView = widget -> rect;
		(** my d_macListHandle). cellSize. h = widget -> width;
	}
	void _GuiMacList_update (GuiObject widget, RgnHandle visRgn) {
		iam_list;
		_GuiMac_clipOnParent (widget);
		if (widget -> isControl) {
			Draw1Control (widget -> nat.control.handle);
		} else {
			LUpdate (visRgn, my d_macListHandle);
		}
		GuiMac_clipOff ();
	}
#endif

#if mac && useCarbon
	static pascal void mac_listDefinition (short message, Boolean select, Rect *rect, Cell cell, short dataOffset, short dataLength, ListHandle handle) {
		GuiObject widget = (GuiObject) GetListRefCon (handle);
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
				Fixed fontSize = 13 << 16;
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

GuiList GuiList_create (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header) {
	GuiList me = Thing_new (GuiList);
	my d_shell = parent -> d_shell;
	my d_parent = parent;
	my d_allowMultipleSelection = allowMultipleSelection;
	#if gtk
		GtkCellRenderer *renderer = NULL;
		GtkTreeViewColumn *col = NULL;
		GtkTreeSelection *sel = NULL;
		GtkListStore *liststore = NULL;

		liststore = gtk_list_store_new (1, G_TYPE_STRING);   // 1 column, of type String (this is a vararg list)
		GuiObject scrolled = gtk_scrolled_window_new (NULL, NULL);
		gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		my d_widget = gtk_tree_view_new_with_model (GTK_TREE_MODEL (liststore));
		gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (my d_widget), FALSE);
		gtk_container_add (GTK_CONTAINER (scrolled), GTK_WIDGET (my d_widget));
		gtk_widget_show (GTK_WIDGET (scrolled));   // BUG
		gtk_tree_view_set_rubber_banding (GTK_TREE_VIEW (my d_widget), allowMultipleSelection ? GTK_SELECTION_MULTIPLE : GTK_SELECTION_SINGLE);
		g_object_unref (liststore);   // Destroys the widget after the list is destroyed

		_GuiObject_setUserData (my d_widget, me);
		_GuiObject_setUserData (scrolled, me);   // for resizing

		renderer = gtk_cell_renderer_text_new ();
		col = gtk_tree_view_column_new ();
		gtk_tree_view_column_pack_start (col, renderer, TRUE);
		gtk_tree_view_column_add_attribute (col, renderer, "text", 0);   // zeroeth column
		if (header != NULL) {
			//gtk_tree_view_column_set_title (col, Melder_peekWcsToUtf8 (header));
		}
		gtk_tree_view_append_column (GTK_TREE_VIEW (my d_widget), col);

		g_object_set_data_full (G_OBJECT (my d_widget), "guiList", me, (GDestroyNotify) _GuiGtkList_destroyCallback); 

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

		sel = gtk_tree_view_get_selection (GTK_TREE_VIEW (my d_widget));
		if (allowMultipleSelection) {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_MULTIPLE);
		} else {
			gtk_tree_selection_set_mode (sel, GTK_SELECTION_SINGLE);
		}
		my v_positionInForm (scrolled, left, right, top, bottom, parent);
		g_signal_connect (sel, "changed", G_CALLBACK (_GuiGtkList_selectionChangedCallback), me);
	#elif cocoa
		GuiCocoaList *list = [[GuiCocoaList alloc] init];
		my d_widget = (GuiObject) list;
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
		[[list tableView] setAllowsMultipleSelection: allowMultipleSelection];
		[list setUserData:me];
	#elif win
		my d_widget = _Gui_initializeWidget (xmListWidgetClass, parent -> d_widget, L"list");
		_GuiObject_setUserData (my d_widget, me);
		my d_widget -> window = CreateWindowEx (0, L"listbox", L"list",
			WS_CHILD | WS_BORDER | WS_VSCROLL | LBS_NOTIFY | WS_CLIPSIBLINGS |
			( allowMultipleSelection ? LBS_EXTENDEDSEL : 0 ),
			my d_widget -> x, my d_widget -> y, my d_widget -> width, my d_widget -> height,
			my d_widget -> parent -> window, NULL, theGui.instance, NULL);
		SetWindowLongPtr (my d_widget -> window, GWLP_USERDATA, (LONG_PTR) my d_widget);
		SetWindowFont (my d_widget -> window, GetStockFont (ANSI_VAR_FONT), FALSE);
		/*if (MEMBER (my parent, ScrolledWindow)) {
			XtDestroyWidget (my d_widget -> parent -> motiff.scrolledWindow.horizontalBar);
			my d_widget -> parent -> motiff.scrolledWindow.horizontalBar = NULL;
			XtDestroyWidget (my d_widget -> parent -> motiff.scrolledWindow.verticalBar);
			my d_widget -> parent -> motiff.scrolledWindow.verticalBar = NULL;
		}*/
		my v_positionInForm (my d_widget, left, right, top, bottom, parent);
	#elif mac
		my d_xmScrolled = XmCreateScrolledWindow (parent -> d_widget, "scrolled", NULL, 0);
		my v_positionInForm (my d_xmScrolled, left, right, top, bottom, parent);
		my d_xmList = my d_widget = _Gui_initializeWidget (xmListWidgetClass, my d_xmScrolled, L"list");
		_GuiObject_setUserData (my d_xmScrolled, me);
		_GuiObject_setUserData (my d_xmList, me);
		Rect dataBounds = { 0, 0, 0, 1 };
		Point cSize;
		SetPt (& cSize, my d_xmList -> rect.right - my d_xmList -> rect.left + 1, CELL_HEIGHT);
		static ListDefSpec listDefSpec;
		if (listDefSpec. u. userProc == NULL) {
			listDefSpec. defType = kListDefUserProcType;
			listDefSpec. u. userProc = mac_listDefinition;
		}
		CreateCustomList (& my d_xmList -> rect, & dataBounds, cSize, & listDefSpec, my d_xmList -> macWindow,
			false, false, false, false, & my d_macListHandle);
		SetListRefCon (my d_macListHandle, (long) my d_xmList);
		if (allowMultipleSelection)
			SetListSelectionFlags (my d_macListHandle, lExtendDrag | lNoRect);
		XtVaSetValues (my d_xmList, XmNwidth, right > 0 ? right - left + 100 : 530, NULL);
	#endif
	return me;
}

GuiList GuiList_createShown (GuiForm parent, int left, int right, int top, int bottom, bool allowMultipleSelection, const wchar_t *header) {
	GuiList me = GuiList_create (parent, left, right, top, bottom, allowMultipleSelection, header);
	my f_show ();
	return me;
}

void structGuiList :: f_deleteAllItems () {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		gtk_list_store_clear (list_store);
	#elif cocoa
        GuiCocoaList *list = (GuiCocoaList *) d_widget;
        [list.contents removeAllObjects];
        [list.tableView reloadData];
	#elif win
		ListBox_ResetContent (d_widget -> window);
	#elif mac
		_GuiMac_clipOnParent (d_widget);
		LDelRow (0, 0, d_macListHandle);
		GuiMac_clipOff ();
	#endif
}

void structGuiList :: f_deleteItem (long position) {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, NULL, (gint) (position - 1))) {
			gtk_list_store_remove (GTK_LIST_STORE (tree_model), & iter);
		}
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		[list. contents   removeObjectAtIndex: position - 1];
		[list. tableView   reloadData];
	#elif win
		ListBox_DeleteString (d_widget -> window, position - 1);
	#elif mac
		_GuiMac_clipOnParent (d_widget);
		LDelRow (1, position - 1, d_macListHandle);
		GuiMac_clipOff ();
		long n = (** d_macListHandle). dataBounds. bottom;
		XtVaSetValues (d_widget, XmNheight, n * CELL_HEIGHT, NULL);
	#endif
}

void structGuiList :: f_deselectAllItems () {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (d_widget));
		gtk_tree_selection_unselect_all (selection);
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		[list.tableView deselectAll:nil];
	#elif win
		ListBox_SetSel (d_widget -> window, False, -1);
	#elif mac
		long n = (** d_macListHandle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (d_widget);
		for (long i = 0; i < n; i ++) { cell.v = i; LSetSelect (false, cell, d_macListHandle); }
		GuiMac_clipOff ();
	#endif
}

void structGuiList :: f_deselectItem (long position) {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (d_widget));
/*		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);*/
		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_path_free (path);
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, NULL, (gint) (position - 1))) {
			gtk_tree_selection_unselect_iter (selection, & iter);
		}
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		[list. tableView   deselectRow: position - 1];
	#elif win
		ListBox_SetSel (d_widget -> window, False, position - 1);
	#elif mac
		Cell cell;
		cell. h = 0;
		cell. v = position - 1; 
		_GuiMac_clipOnParent (d_widget);
		LSetSelect (false, cell, d_macListHandle);
		GuiMac_clipOff ();
	#endif
}

long * structGuiList :: f_getSelectedPositions (long *numberOfSelectedPositions) {
	*numberOfSelectedPositions = 0;
	long *selectedPositions = NULL;
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (d_widget));
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		int n = gtk_tree_selection_count_selected_rows (selection);
		if (n > 0) {
			GList *list = gtk_tree_selection_get_selected_rows (selection, (GtkTreeModel **) & list_store);
			long ipos = 1;
			*numberOfSelectedPositions = n;
			selectedPositions = NUMvector <long> (1, *numberOfSelectedPositions);
			Melder_assert (selectedPositions != NULL);
			for (GList *l = g_list_first (list); l != NULL; l = g_list_next (l)) {
				gint *index = gtk_tree_path_get_indices ((GtkTreePath *) l -> data);
				selectedPositions [ipos] = index [0] + 1;
				ipos ++;
			}
			g_list_foreach (list, (GFunc) gtk_tree_path_free, NULL);
			g_list_free (list);
		}
		return selectedPositions;
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		NSIndexSet *indexSet = [list. tableView   selectedRowIndexes];
		*numberOfSelectedPositions = 0;
		selectedPositions = NUMvector <long> (1, [indexSet count]);   
		NSUInteger currentIndex = [indexSet firstIndex];
		while (currentIndex != NSNotFound) {
			selectedPositions [++ *numberOfSelectedPositions] = currentIndex + 1;
			currentIndex = [indexSet   indexGreaterThanIndex: currentIndex];
		}
	#elif win
		int n = ListBox_GetSelCount (d_widget -> window), *indices;
		if (n == 0) {
			return selectedPositions;
		}
		if (n == -1) {   // single selection
			int selection = ListBox_GetCurSel (d_widget -> window);
			if (selection == -1) return False;
			n = 1;
			indices = Melder_calloc_f (int, n);
			indices [0] = selection;
		} else {
			indices = Melder_calloc_f (int, n);
			ListBox_GetSelItems (d_widget -> window, n, indices);
		}
		*numberOfSelectedPositions = n;
		selectedPositions = NUMvector <long> (1, *numberOfSelectedPositions);
		Melder_assert (selectedPositions != NULL);
		for (long ipos = 1; ipos <= *numberOfSelectedPositions; ipos ++) {
			selectedPositions [ipos] = indices [ipos - 1] + 1;   // convert from zero-based list of zero-based indices
		}
		Melder_free (indices);
	#elif mac
		long n = (** d_macListHandle). dataBounds. bottom;
		Cell cell; cell.h = 0;
		if (n < 1) {
			return selectedPositions;
		}
		selectedPositions = NUMvector <long> (1, n);   // probably too big (ergo, probably reallocable), but the caller will throw it away anyway
		for (long i = 1; i <= n; i ++) {
			cell. v = i - 1;
			if (LGetSelect (false, & cell, d_macListHandle)) {
				selectedPositions [++ *numberOfSelectedPositions] = i;
			}
		}
		if (*numberOfSelectedPositions == 0) {
			NUMvector_free (selectedPositions, 1);
			selectedPositions = NULL;
		}
	#endif
	return selectedPositions;
}

long structGuiList :: f_getBottomPosition () {
	#if gtk
		// TODO
		return 1;
	#elif cocoa
		return 1;   // TODO
	#elif win
		long bottom = ListBox_GetTopIndex (d_widget -> window) + d_widget -> height / ListBox_GetItemHeight (d_widget -> window, 0);
		if (bottom < 1) bottom = 1;
		long n = ListBox_GetCount (d_widget -> window);
		if (bottom > n) bottom = n;
		return bottom;
	#elif mac
		Melder_assert (d_widget -> parent -> widgetClass == xmScrolledWindowWidgetClass);
		GuiObject clipWindow = d_widget -> parent -> motiff.scrolledWindow.clipWindow;
		GuiObject workWindow = d_widget -> parent -> motiff.scrolledWindow.workWindow;
		long top = (clipWindow -> rect.top - workWindow -> rect.top + 5) / CELL_HEIGHT + 1;
		long visible = (clipWindow -> rect.bottom - clipWindow -> rect.top - 5) / CELL_HEIGHT + 1;
		long n = (** d_macListHandle). dataBounds. bottom;
		if (visible > n) visible = n;
		long bottom = top + visible - 1;
		if (bottom < 1) bottom = 1;
		if (bottom > n) bottom = n;
		return bottom;
	#endif
}

long structGuiList :: f_getNumberOfItems () {
	long numberOfItems = 0;
	#if gtk
		GtkTreeModel *model = gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget));
		numberOfItems = gtk_tree_model_iter_n_children (model, NULL); 
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		numberOfItems = [[list contents] count];
	#elif win
		numberOfItems = ListBox_GetCount (d_widget -> window);
	#elif mac
		numberOfItems = (** d_macListHandle). dataBounds. bottom;
	#endif
	return numberOfItems;
}

long structGuiList :: f_getTopPosition () {
	#if gtk
		// TODO
		return 1;
	#elif cocoa
		return 1;   // TODO
	#elif win
		long top = ListBox_GetTopIndex (d_widget -> window);
		if (top < 1) top = 1;
		long n = ListBox_GetCount (d_widget -> window);
		if (top > n) top = 0;
		return top;
	#elif mac
		Melder_assert (d_widget -> parent -> widgetClass == xmScrolledWindowWidgetClass);
		GuiObject clipWindow = d_widget -> parent -> motiff.scrolledWindow.clipWindow;
		GuiObject workWindow = d_widget -> parent -> motiff.scrolledWindow.workWindow;
		long top = (clipWindow -> rect.top - workWindow -> rect.top + 5) / CELL_HEIGHT + 1;
		if (top < 1) top = 1;
		long n = (** d_macListHandle). dataBounds. bottom;
		if (top > n) top = 0;
		return top;
	#endif
}

void structGuiList :: f_insertItem (const wchar_t *itemText, long position) {
	GuiControlBlockValueChangedCallbacks block (this);
	/*
	 * 'position' is the position of the new item in the list after insertion:
	 * a value of 1 therefore puts the new item at the top of the list;
	 * a value of 0 is special: the item is put at the bottom of the list.
	 */
	#if gtk
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		gtk_list_store_insert_with_values (list_store, NULL, (gint) position - 1, COLUMN_STRING, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
		// does GTK know the '0' trick?
		// it does know about NULL, to append in another function
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		NSString *nsString = [[NSString alloc] initWithUTF8String: Melder_peekWcsToUtf8 (itemText)];
		if (position)
			[[list contents]   insertObject: nsString   atIndex: position - 1];   // cocoa lists start with item 0
		else
			[[list contents]   addObject: nsString];   // insert at end
		//Melder_assert ([nsString retainCount] == 2);
		[nsString release];
		[[list tableView] reloadData];
	#elif win
		if (position)
			ListBox_InsertString (d_widget -> window, position - 1, itemText);   // win lists start with item 0
		else
			ListBox_AddString (d_widget -> window, itemText);   // insert at end
	#elif mac
		long n = (** d_macListHandle). dataBounds. bottom;
		if (position == 0)
			position = n + 1;   // insert at end
		Cell cell;
		cell.h = 0; cell. v = position - 1;   // mac lists start with item 0
		_GuiMac_clipOnParent (d_widget);
		LAddRow (1, position - 1, d_macListHandle);
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);   // although defProc will convert again...
		LSetCell (itemText_utf8, (short) strlen (itemText_utf8), cell, d_macListHandle);
		(** d_macListHandle). visible. bottom = n + 1;
		_GuiMac_clipOffInvalid (d_widget);
		XtVaSetValues (d_widget, XmNheight, (n + 1) * CELL_HEIGHT, NULL);
	#endif
}

void structGuiList :: f_replaceItem (const wchar_t *itemText, long position) {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkTreeIter iter;
		GtkTreeModel *tree_model = gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget));
		if (gtk_tree_model_iter_nth_child (tree_model, & iter, NULL, (gint) (position - 1))) {
			gtk_list_store_set (GTK_LIST_STORE (tree_model), & iter, COLUMN_STRING, Melder_peekWcsToUtf8 (itemText), -1);
		}
/*
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
		GtkTreeIter iter;
		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
		gtk_tree_path_free (path);*/
		// gtk_list_store_set (list_store, & iter, 0, Melder_peekWcsToUtf8 (itemText), -1);
		// TODO: Tekst opsplitsen
	#elif cocoa
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		NSString *nsString = [[NSString alloc] initWithUTF8String: Melder_peekWcsToUtf8 (itemText)];
		[[list contents]   replaceObjectAtIndex: position - 1   withObject: nsString];
		Melder_assert ([nsString retainCount] == 2);
		[nsString release];
		[[list tableView] reloadData];
	#elif win
		long nativePosition = position - 1;   // convert from 1-based to zero-based
		ListBox_DeleteString (d_widget -> window, nativePosition);
		ListBox_InsertString (d_widget -> window, nativePosition, itemText);
	#elif mac
		_GuiMac_clipOnParent (d_widget);
		Cell cell;
		cell.h = 0;
		cell.v = position - 1;
		const char *itemText_utf8 = Melder_peekWcsToUtf8 (itemText);
		LSetCell (itemText_utf8, strlen (itemText_utf8), cell, d_macListHandle);
		LDraw (cell, d_macListHandle);
		GuiMac_clipOff ();
	#endif
}

void structGuiList :: f_selectItem (long position) {
	GuiControlBlockValueChangedCallbacks block (this);
	#if gtk
		GtkTreeSelection *selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (d_widget));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position - 1, -1);
		gtk_tree_selection_select_path (selection, path);
		gtk_tree_path_free (path);

// TODO: check of het bovenstaande werkt, dan kan dit weg
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
//		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) position);
//		GtkTreeIter iter;
//		gtk_tree_model_get_iter (GTK_TREE_MODEL (list_store), & iter, path);
//		gtk_tree_selection_select_iter (selection, & iter);
	#elif cocoa
		NSIndexSet *indexSet = [[NSIndexSet alloc] initWithIndex: position - 1];
		GuiCocoaList *list = (GuiCocoaList *) d_widget;
		[[list tableView]   selectRowIndexes: indexSet   byExtendingSelection: d_allowMultipleSelection];
		[indexSet release];
	#elif win
		if (! d_allowMultipleSelection) {
			ListBox_SetCurSel (d_widget -> window, position - 1);
		} else {
			ListBox_SetSel (d_widget -> window, True, position - 1);
		}
	#elif mac
		Cell cell; cell.h = 0;
		_GuiMac_clipOnParent (d_widget);
		if (! d_allowMultipleSelection) {
			long n = (** d_macListHandle). dataBounds. bottom;
			for (long i = 0; i < n; i ++) if (i != position - 1) {
				cell.v = i;
				LSetSelect (false, cell, d_macListHandle);
			}
		}
		cell.v = position - 1; 
		LSetSelect (true, cell, d_macListHandle);
		GuiMac_clipOff ();
	#endif
}

void structGuiList :: f_setDoubleClickCallback (void (*callback) (void *boss, GuiListEvent event), void *boss) {
	d_doubleClickCallback = callback;
	d_doubleClickBoss = boss;
}

void structGuiList :: f_setSelectionChangedCallback (void (*callback) (void *boss, GuiListEvent event), void *boss) {
	d_selectionChangedCallback = callback;
	d_selectionChangedBoss = boss;
}

void structGuiList :: f_setTopPosition (long topPosition) {
	//Melder_casual ("Set top position %ld", topPosition);
	#if gtk
//		GtkListStore *list_store = GTK_LIST_STORE (gtk_tree_view_get_model (GTK_TREE_VIEW (d_widget)));
		GtkTreePath *path = gtk_tree_path_new_from_indices ((gint) topPosition);
		gtk_tree_view_scroll_to_cell (GTK_TREE_VIEW (d_widget), path, NULL, FALSE, 0.0, 0.0);
		gtk_tree_path_free (path);
	#elif cocoa
	#elif win
		ListBox_SetTopIndex (d_widget -> window, topPosition - 1);
	#elif mac
		//_GuiMac_clipOnParent (d_widget);
		//LScroll (0, topPosition - (** d_macListHandle). visible. top - 1, d_macListHandle);   // TODO: implement
		//GuiMac_clipOff ();
		//my d_scrolled -> motiff.scrolledWindow.verticalBar;   // TODO: implement
		XtVaSetValues (d_widget, XmNy, - (topPosition - 1) * CELL_HEIGHT, NULL);
	#endif
}

/* End of file GuiList.cpp */
