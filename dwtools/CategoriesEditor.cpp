/* CategoriesEditor.cpp
 *
 * Copyright (C) 1993-2019 David Weenink, 2008,2015-2018 Paul Boersma
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This code is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this work. If not, see <http://www.gnu.org/licenses/>.
 */

/*
 djmw 1995
 djmw 19980225 repaired a memory leak, caused by wrong inheritance for
 	 CategoriesEditorInsert command.
 djmw 20020408 GPL
 djmw 20020408 Modified 'createMenus'
 djmw 20060111 Replaced Resources.h with Preferences.h
 djmw 20060328 Changed last argument to 0 in XtVaSetValues, XtVaGetValues and XtVaCreateManagedWidget
 	for 64-bit compatibility.
 djmw 20070620 Latest modification.
 pb 20080320 split off Help menu
 pb 20080321 new Editor API
 djmw 20090107 Removed a bug in update that caused editor to crash on replace
 djmw 20090203 Removed potential crashes in CategoriesEditor<command>_create.
 djmw 20110304 Thing_new
 djmw 20111110 Use autostringvector
*/

#include "CategoriesEditor.h"
#include "EditorM.h"
#include "NUM2.h"

Thing_implement (CategoriesEditor, Editor, 0);

static const conststring32 CategoriesEditor_EMPTYLABEL = U"(empty)";

static void menu_cb_help (CategoriesEditor /* me */, EDITOR_ARGS) {
	Melder_help (U"CategoriesEditor");
}

#pragma mark - Collection extensions

/* Preconditions: */
/*	1 <= (position  [i], newpos) <= size; */
/*	newpos <= position  [1] || newpos >= position  [npos] */
static void Ordered_moveItems (Ordered me, constINTVEC position, integer newpos) {
	if (position.size < 1)
		return;
	const integer min = NUMmin_e (position), max = NUMmax_e (position);

	Melder_assert (min >= 1 && max <= my size && (newpos <= min || newpos >= max));

	autovector <Daata> tmp = newvectorzero <Daata> (position.size);

	/*
		Move some data from `me` into `tmp`, in a different order.
	*/
	for (integer i = 1; i <= position.size; i ++) {
		tmp [i] = (Daata) my at [position [i]];   // dangle
		my at [position [i]] = nullptr;   // undangle
	}

	// create a contiguous 'hole'

	integer pos;
	if (newpos <= min) {
		pos = max;
		for (integer i = max; i >= newpos; i --) {
			if (my at [i])
				my at [pos --] = my at [i];
		}
		pos = newpos;
	} else {
		pos = min;
		for (integer i = min; i <= newpos; i ++) {
			if (my at [i])
				my at [pos ++] = my at [i];
		}
		pos = newpos - position.size + 1;
	}

	// fill the 'hole'

	for (integer i = 1; i <= position.size; i ++)
		my at [pos ++] = tmp [i];
}

/* Remove the item at position 'from' and insert it at position 'to'. */
static void Ordered_moveItem (Ordered me, integer from, integer to) {
	if (from < 1 || from > my size)
		from = my size;

	if (to < 1 || to > my size)
		to = my size;

	if (from == to)
		return;

	Daata tmp = my at [from];
	if (from > to) {
		for (integer i = from; i > to; i --)
			my at [i] = my at [i - 1];
	} else {
		for (integer i = from; i < to; i ++)
			my at [i] = my at [i + 1];
	}
	my at [to] = tmp;
}

#pragma mark - Widget updates

static void notifyNumberOfSelected (CategoriesEditor me) {
	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size > 0) {
		autoMelderString tmp;
		MelderString_append (& tmp, posList.size, U" selection", ( posList.size > 1 ? U"s." : U"." ));
		GuiLabel_setText (my outOfView, tmp.string);
	} else 
		GuiLabel_setText (my outOfView, U"");
}

static void updateWidgets (CategoriesEditor me) {   // all buttons except undo & redo
	const integer size = my categories() -> size;

	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	
	GuiThing_setSensitive (my insert, ( posList.size == 1 ));
	GuiThing_setSensitive (my insertAtEnd, true);
	GuiThing_setSensitive (my replace, ( posList.size > 0 ));
	
	const bool removeSensitive = ! (posList.size == 1 && size == 1 && 
		str32equ (CategoriesEditor_EMPTYLABEL, my categories()->at [1] -> string.get()));
	GuiThing_setSensitive (my remove, removeSensitive);
	
	bool moveUpIsSensitive = false, moveDownIsSensitive = false;
	if (posList.size > 0) {
		const integer firstPos = posList [1], lastPos = posList [posList.size];
		const bool contiguous = ( lastPos - firstPos + 1 == posList.size );
		moveUpIsSensitive = contiguous && firstPos > 1;
		moveDownIsSensitive = contiguous && lastPos < size;
		my position = firstPos;
	}
	GuiThing_setSensitive (my moveUp,      moveUpIsSensitive);
	GuiThing_setSensitive (my moveDown,    moveDownIsSensitive);
	
	if (my history) {
		bool undoIsSensitive = true;
		conststring32 commandName = CommandHistory_commandName (my history.get(), 0);
		if (! commandName) {
			GuiButton_setText (my undo, U"Cannot undo");
			undoIsSensitive = false;
		} else
			GuiButton_setText (my undo, Melder_cat (U"Undo ", U"\"", commandName, U"\""));
		GuiThing_setSensitive (my undo, undoIsSensitive);

		bool redoIsSensitive = true;
		commandName = CommandHistory_commandName (my history.get(), 1);
		if (! commandName) {
			GuiButton_setText (my redo, U"Cannot redo");
			redoIsSensitive = false;
		} else
			GuiButton_setText (my redo, Melder_cat (U"Redo ", U"\"", commandName, U"\""));
		GuiThing_setSensitive (my redo, redoIsSensitive);
	}
	notifyNumberOfSelected (me);
}

static void update (CategoriesEditor me, integer from, integer to, constINTVEC select, integer nSelect) {
	const integer size = my categories() -> size;

	if (size == 0) {
		autoSimpleString str = SimpleString_create (CategoriesEditor_EMPTYLABEL);
		my categories() -> addItem_move (str.move());
		update (me, 0, 0, select, 0); // was nullptr
		return;
	}
	if (from == 0 && to == 0) {
		from = 1;
		to = size;
	}
	if (from < 1 || from > size)
		from = size;
	if (to < 1 || to > size)
		to = size;
	if (from > to) {
		integer tmp = from;
		from = to;
		to = tmp;
	}

	/*
		Begin optimization: add the items from a table instead of separately.
	*/
	try {
		const integer offset = from - 1, numberOfElements = to - from + 1;
		autoSTRVEC table (numberOfElements);
		integer itemCount = GuiList_getNumberOfItems (my list);
		for (integer i = from; i <= to; i ++) {
			SimpleString category = my categories()->at [i];
			table [i - offset] = Melder_dup_f (Melder_cat (i, U" ", category -> string.get()));
		}
		if (itemCount > size) {   // have any items been removed from the Categories?
			for (integer j = itemCount; j > size; j --)
				GuiList_deleteItem (my list, j);
			itemCount = size;
		}
		if (to > itemCount) {
			for (integer j = 1; j <= to - itemCount; j ++)
				GuiList_insertItem (my list, table [itemCount + j - offset].get(), 0);
		}
		if (from <= itemCount) {
			const integer n = ( to < itemCount ? to : itemCount );
			for (integer j = from; j <= n; j ++)
				GuiList_replaceItem (my list, table [j - offset].get(), j);
		}
	} catch (MelderError) {
		throw;
	}
	/*
		End of optimization
	*/
	
	// HIGHLIGHT

	GuiList_deselectAllItems (my list);
	if (size == 1) { // the only item is always selected
		SimpleString category = my categories()->at [1];
		GuiList_selectItem (my list, 1);
		updateWidgets (me);   // instead of "notify". BUG?
		GuiText_setString (my text, category -> string.get());
	} else if (nSelect > 0) {
		/*
			Select, but postpone highlighting.
		*/
		for (integer i = 1; i <= nSelect; i ++)
			GuiList_selectItem (my list, ( select [i] > size ? size : select [i] ));
	}

	// VIEWPORT

	{
		integer top = GuiList_getTopPosition (my list);
		const integer bottom = GuiList_getBottomPosition (my list);
		const integer visible = bottom - top + 1;
		if (nSelect == 0) {
			top = my position - visible / 2;
		} else if (select [nSelect] < top) {
			// selection above visible area
			top = select [1];
		} else if (select [1] > bottom) {
			// selection below visible area
			top = select [nSelect] - visible + 1;
		} else {
			integer deltaTopPos = -1, nUpdate = to - from + 1;
			if ((from == select [1] && to == select [nSelect]) ||   // replace
			    (nUpdate > 2 && nSelect == 1))   // insert
			{
				deltaTopPos = 0;
			} else if (nUpdate == nSelect + 1 && select [1] == from + 1) {   // down
				deltaTopPos = 1;
			}
			top += deltaTopPos;
		}
		Melder_clip (1_integer, & top, size - visible + 1);
		GuiList_setTopPosition (my list, top);
	}
}

#pragma mark - Commands for Undo and Redo

Thing_define (CategoriesEditorCommand, Command) {
	autoCategories categories;
	autoINTVEC selection;
	integer nSelected, newPos;
	
	void v_do ()
		override {};
	void v_undo ()
		override {};
};


Thing_implement (CategoriesEditorCommand, Command, 0);

static void CategoriesEditorCommand_init (CategoriesEditorCommand me, conststring32 name, Thing boss,
	integer /*nCategories*/, integer nSelected) 
{
	my nSelected = nSelected;
	Command_init (me, name, boss);
	my categories = Categories_create();
	my selection = zero_INTVEC (nSelected);
}

#pragma mark Insert

Thing_define (CategoriesEditorInsert, CategoriesEditorCommand) {
	void v_do () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		autoSimpleString str = Data_copy (categories->at [1]);
		editor -> categories() -> addItemAtPosition_move (str.move(), selection [1]);
		update (editor, selection [1], 0, selection.get(), 1);
	};

	void v_undo () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		editor -> categories() -> removeItem (selection [1]);
		update (editor, selection [1], 0, selection.get(), 1);
	};
};

Thing_implement (CategoriesEditorInsert, CategoriesEditorCommand, 0);

static autoCategoriesEditorInsert CategoriesEditorInsert_create (Thing boss, autoSimpleString str, integer position) {
	try {
		autoCategoriesEditorInsert me = Thing_new (CategoriesEditorInsert);
		CategoriesEditorCommand_init (me.get(), U"Insert", boss, 1, 1);
		my selection [1] = position;
		my categories -> addItem_move (str.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorInsert not created.");
	}
}

#pragma mark Remove

Thing_define (CategoriesEditorRemove, CategoriesEditorCommand) {
	void v_do () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		for (integer i = nSelected; i >= 1; i--) {
			autoSimpleString item = Data_copy (editor -> categories()->at [selection [i]]);   // FIXME this copy can probably be replaced with a move
			categories -> addItemAtPosition_move (item.move(), 1);
			editor -> categories() -> removeItem (selection [i]);
		}
		update (editor, selection [1], 0, selection.get(), 0); // was nullptr
	}

	void v_undo () {
		CategoriesEditor editor = (CategoriesEditor) boss;
		for (integer i = 1; i <= nSelected; i ++) {
			autoSimpleString item = Data_copy (categories->at [i]);
			editor -> categories() -> addItemAtPosition_move (item.move(), selection [i]);
		}
		update (editor, selection [1], 0, selection.get(), nSelected);
	}
};

Thing_implement (CategoriesEditorRemove, CategoriesEditorCommand, 0);

static autoCategoriesEditorRemove CategoriesEditorRemove_create (Thing boss, constINTVEC posList) {
	try {
		autoCategoriesEditorRemove me = Thing_new (CategoriesEditorRemove);
		CategoriesEditorCommand_init (me.get(), U"Remove", boss, posList.size, posList.size);
		my selection.all()  <<=  posList;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorRemove not created.");
	}
}

#pragma mark Replace

Thing_define (CategoriesEditorReplace, CategoriesEditorCommand) {
	void v_do () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		for (integer i = nSelected; i >= 1; i --) {
			/*
				Swap categories->at [1] with categories->at [my selection [i]] under ambiguous ownership.
			*/
			autoSimpleString tmp = Data_copy (categories->at [1]);
			autoSimpleString other;
			other. adoptFromAmbiguousOwner (editor -> categories()->at [selection [i]]);
			categories -> addItemAtPosition_move (other.move(), 2);   // YUCK
			editor -> categories()->at [selection [i]] = tmp.releaseToAmbiguousOwner();
		}
		update (editor, selection [1], selection [nSelected], selection.get(), nSelected);
	}

	void v_undo () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		for (integer i = 1; i <= nSelected; i ++) {
			autoSimpleString str = Data_copy (categories->at [i + 1]);
			editor -> categories() -> replaceItem_move (str.move(), selection [i]);
		}
		update (editor, selection [1], selection [nSelected], selection.get(), nSelected);
	}
};

Thing_implement (CategoriesEditorReplace, CategoriesEditorCommand, 0);

static autoCategoriesEditorReplace CategoriesEditorReplace_create (Thing boss, autoSimpleString str, constINTVEC posList) {
	try {
		autoCategoriesEditorReplace me = Thing_new (CategoriesEditorReplace);
		CategoriesEditorCommand_init (me.get(), U"Replace", boss, posList.size + 1, posList.size);
		my selection.all()  <<=  posList;
		my categories -> addItem_move (str.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorReplace not created.");
	}
}

#pragma mark MoveUp

Thing_define (CategoriesEditorMoveUp, CategoriesEditorCommand) {
	void v_do () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		Ordered_moveItems ((Ordered) editor -> categories(), selection.get(), newPos);   // FIXME cast
		autoINTVEC selectionUpdate = raw_INTVEC (nSelected);
		for (integer i = 1; i <= nSelected; i ++)
			selectionUpdate [i] = newPos + i - 1;
		update (editor, newPos, selection [nSelected], selectionUpdate.get(), nSelected);
	}

	void v_undo () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		for (integer i = 1; i <= nSelected; i ++)
			Ordered_moveItem ((Ordered) editor -> categories(), newPos, selection [nSelected]);   // FIXME cast
		update (editor, newPos, selection [nSelected], selection.get(), nSelected);
	}
};

Thing_implement (CategoriesEditorMoveUp, CategoriesEditorCommand, 0);

static autoCategoriesEditorMoveUp CategoriesEditorMoveUp_create (Thing boss, constINTVEC posList, integer newPos) {
	try {
		autoCategoriesEditorMoveUp me = Thing_new (CategoriesEditorMoveUp);
		CategoriesEditorCommand_init (me.get(), U"Move up", boss, 0, posList.size);
		my selection.all()  <<=  posList;
		my newPos = newPos;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorMoveUp not created.");
	}
}

#pragma mark MoveDown

Thing_define (CategoriesEditorMoveDown, CategoriesEditorCommand) {

	void v_do () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		Ordered_moveItems ((Ordered) editor -> categories(), selection.get(), newPos);   // FIXME cast
		autoINTVEC selectionUpdate = raw_INTVEC (nSelected);
		for (integer i = 1; i <= nSelected; i ++)
			selectionUpdate [i] = newPos - nSelected + i;
		update (editor, selection [1], newPos, selectionUpdate.get(), nSelected);
	}

	void v_undo () {
		CategoriesEditor editor = static_cast<CategoriesEditor> (boss);
		for (integer i = 1; i <= nSelected; i ++)
			Ordered_moveItem ((Ordered) editor -> categories(), newPos, selection [1]); // TODO 1 or i ??     // FIXME cast
		integer from = selection [1];
		update (editor, ( from > 1 ? from -- : from ), newPos, selection.get(), nSelected);
	}
};

Thing_implement (CategoriesEditorMoveDown, CategoriesEditorCommand, 0);

static autoCategoriesEditorMoveDown CategoriesEditorMoveDown_create (Thing boss, constINTVEC posList, integer newPos) {
	try {
		autoCategoriesEditorMoveDown me = Thing_new (CategoriesEditorMoveDown);
		CategoriesEditorCommand_init (me.get(), U"Move down", boss, 0, posList.size);
		my selection.all()  <<=  posList;
		my newPos = newPos;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorMoveDown not created.");
	}
}

#pragma mark - Callbacks

static void gui_button_cb_remove (CategoriesEditor me, GuiButtonEvent /* event */) {
	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size > 0) {
		autoCategoriesEditorRemove command = CategoriesEditorRemove_create (me, posList.get());
		command -> v_do ();
		if (my history)
			CommandHistory_insertItem_move (my history.get(), command.move());
		updateWidgets (me);
	}
}

static void insert (CategoriesEditor me, integer position) {
	autostring32 text = GuiText_getString (my text);
	if (text && text [0] != U'\0') {
		autoSimpleString str = SimpleString_create (text.get());
		autoCategoriesEditorInsert command = CategoriesEditorInsert_create (me, str.move(), position);
		command -> v_do ();
		if (my history)
			CommandHistory_insertItem_move (my history.get(), command.move());
		updateWidgets (me);
	}
}

static void gui_button_cb_insert (CategoriesEditor me, GuiButtonEvent /* event */) {
	insert (me, my position);
}

static void gui_button_cb_insertAtEnd (CategoriesEditor me, GuiButtonEvent /* event */) {
	insert (me, my categories()->size + 1);
	my position = my categories()->size;
}

static void gui_button_cb_replace (CategoriesEditor me, GuiButtonEvent /* event */) {
	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size > 0) {
		autostring32 text = GuiText_getString (my text);
		if (text && text [0] != U'\0') {
			autoSimpleString str = SimpleString_create (text.get());
			autoCategoriesEditorReplace command = CategoriesEditorReplace_create (me, str.move(), posList.get());
			command -> v_do ();
			if (my history)
				CommandHistory_insertItem_move (my history.get(), command.move());
			updateWidgets (me);
		}
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveUp (CategoriesEditor me, GuiButtonEvent /* event */) {
	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size > 0) {
		autoCategoriesEditorMoveUp command = CategoriesEditorMoveUp_create (me, posList.get(), posList [1] - 1);
		command -> v_do ();
		if (my history)
			CommandHistory_insertItem_move (my history.get(), command.move());
		updateWidgets (me);
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveDown (CategoriesEditor me, GuiButtonEvent /* event */) {
	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size > 0) {
		autoCategoriesEditorMoveDown command = CategoriesEditorMoveDown_create (me, posList.get(), posList [posList.size] + 1);
		command -> v_do ();
		if (my history)
			CommandHistory_insertItem_move (my history.get(), command.move());
		updateWidgets (me);
	}
}

static void gui_list_cb_selectionChanged (CategoriesEditor me, GuiList_SelectionChangedEvent /* event */) {
	updateWidgets (me);
}

static void gui_list_cb_doubleClick (CategoriesEditor me, GuiList_DoubleClickEvent event) {
	Melder_assert (event -> list == my list);
	//  `my position` should just have been updated by the selectionChanged callback.

	autoINTVEC posList = GuiList_getSelectedPositions (my list);
	if (posList.size == 1   // often or even usually true when double-clicking?
	    && posList [1] == my position)   // should be true, but we don't crash if it's false
	{
		SimpleString category = my categories()->at [my position];
		GuiText_setString (my text, ( category -> string ? category -> string.get() : U"" ));
	}
}

static void gui_list_cb_scroll (CategoriesEditor me, GuiList_ScrollEvent /* event */) {
	notifyNumberOfSelected (me);
}

static void gui_button_cb_undo (CategoriesEditor me, GuiButtonEvent /* event */) {
	if (CommandHistory_isOffleft (my history.get()))
		return;
	Command command = CommandHistory_getItem (my history.get());
	command -> v_undo ();
	CommandHistory_back (my history.get());
	updateWidgets (me);
}

static void gui_button_cb_redo (CategoriesEditor me, GuiButtonEvent /* event */) {
	CommandHistory_forth (my history.get());
	if (CommandHistory_isOffright (my history.get()))
		return;
	Command command = CommandHistory_getItem (my history.get());
	command -> v_do ();
	updateWidgets (me);
}

#pragma mark - Editor methods

void structCategoriesEditor :: v_createMenuItems_help (EditorMenu menu) {
	CategoriesEditor_Parent :: v_createMenuItems_help (menu);
	EditorMenu_addCommand (menu, U"CategoriesEditor help", '?', menu_cb_help);
}

// origin is at top left.
void structCategoriesEditor :: v_createChildren () {
	constexpr int menuBarOffset { 40 };
	constexpr int button_width { 130 }, button_height { menuBarOffset }, list_width { 260 }, list_height { 420 };
	constexpr int delta_x { 15 }, delta_y { menuBarOffset / 2 }, text_button_height { button_height / 2 };

	int left = 5, right = left + button_width, top = 3 + menuBarOffset, bottom = top + text_button_height;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Positions:", 0);
	left = right + delta_x ;
	right = left + button_width;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Values:", 0);

	left = 0;
	right = left + list_width;
	// int buttons_top = (top = bottom + delta_y);
	int list_bottom = bottom = top + list_height;
	list = GuiList_create (our windowForm, left, right, top, bottom, true, 0);
	GuiList_setSelectionChangedCallback (list, gui_list_cb_selectionChanged, this);
	GuiList_setDoubleClickCallback (list, gui_list_cb_doubleClick, this);
	GuiList_setScrollCallback (list, gui_list_cb_scroll, this);
	GuiThing_show (list);

	int buttons_left = left = right + 2 * delta_x;
	right = left + button_width;
	bottom = top + button_height;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Value:", 0);
	left = right + delta_x;
	right = left + button_width;
	text = GuiText_createShown (our windowForm, left, right, top, bottom, 0);
	GuiText_setString (text, CategoriesEditor_EMPTYLABEL);

	left = buttons_left;
	right = left + button_width;
	top = bottom + delta_y;
	bottom = top + button_height;
	insert = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Insert", gui_button_cb_insert, this, GuiButton_DEFAULT);
	left = right + delta_x;
	right = left + button_width;
	replace = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Replace", gui_button_cb_replace, this, 0);
	left = buttons_left;
	right = left + int (1.5 * button_width);
	top = bottom + delta_y;
	bottom = top + button_height;
	insertAtEnd = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Insert at end", gui_button_cb_insertAtEnd, this, 0);
	top = bottom + delta_y;
	bottom = top + button_height;
	undo = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Undo", gui_button_cb_undo, this, 0);
	top = bottom + delta_y;
	bottom = top + button_height;
	redo = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Redo", gui_button_cb_redo, this, 0);
	top = bottom + delta_y;
	bottom = top + button_height;
	remove = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Remove", gui_button_cb_remove, this, 0);
	top = bottom + delta_y;
	bottom = top + button_height;
	moveUp = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Move selection up", gui_button_cb_moveUp, this, 0);
	top = bottom + delta_y;
	bottom = top + button_height;
	moveDown = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Move selection down", gui_button_cb_moveDown, this, 0);

	top = list_bottom + delta_y;
	bottom = top + button_height;
	left = 5;
	right = left + 200;
	outOfView = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);
}

void structCategoriesEditor :: v1_dataChanged (Editor /* sender */) {
	autoINTVEC select;
	update (this, 0, 0, select.get(), 0);
	updateWidgets (this);
}

#pragma mark -

autoCategoriesEditor CategoriesEditor_create (conststring32 title, Categories categories) {
	try {
		autoCategoriesEditor me = Thing_new (CategoriesEditor);
		Editor_init (me.get(), 20, 40, 600, 600, title, categories);
		my history = CommandHistory_create ();
		autoINTVEC select;
		update (me.get(), 0, 0, select.get(), 0);
		updateWidgets (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Categories window not created.");
	}
}

/* End of file CategoriesEditor.cpp */
