/* CategoriesEditor.cpp
 *
 * Copyright (C) 1993-2013 David Weenink, 2008,2015,2016,2017 Paul Boersma
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

#define CategoriesEditor_TEXTMAXLENGTH 100

#include "CategoriesEditor.h"
#include "EditorM.h"

Thing_implement (CategoriesEditor, Editor, 0);

const char32 *CategoriesEditor_EMPTYLABEL = U"(empty)";

static void menu_cb_help (CategoriesEditor /* me */, EDITOR_ARGS_DIRECT) {
	Melder_help (U"CategoriesEditor");
}

#pragma mark - Collection extensions

/* Preconditions: */
/*	1 <= (position  [i], newpos) <= size; */
/*	newpos <= position  [1] || newpos >= position  [npos] */
static void Ordered_moveItems (Ordered me, integer position [], integer npos, integer newpos) {
	integer pos, min = position [1], max = position [1];

	for (integer i = 2; i <= npos; i ++) {
		if (position [i] > max) {
			max = position [i];
		} else if (position [i] < min) {
			min = position [i];
		}
	}

	Melder_assert (min >= 1 && max <= my size && (newpos <= min || newpos >= max));

	autoNUMvector<Daata> tmp (1, npos);

	/*
		Move some data from `me` into `tmp`, in a different order.
	*/
	for (integer i = 1; i <= npos; i ++) {
		tmp [i] = (Daata) my at [position [i]];   // dangle
		my at [position [i]] = nullptr;   // undangle
	}

	// create a contiguous 'hole'

	if (newpos <= min) {
		pos = max;
		for (integer i = max; i >= newpos; i --) {
			if (my at [i]) {
				my at [pos --] = my at [i];
			}
		}
		pos = newpos;
	} else {
		pos = min;
		for (integer i = min; i <= newpos; i ++) {
			if (my at [i]) {
				my at [pos ++] = my at [i];
			}
		}
		pos = newpos - npos + 1;
	}

	// fill the 'hole'

	for (integer i = 1; i <= npos; i ++) {
		my at [pos ++] = tmp [i];
	}
}

/*
static void OrderedOfString_replaceItemPos (Collection me, autoSimpleString data, integer pos) {
	if (pos < 1 || pos > my size) {
		return;
	}
	my replaceItem_move (data.move(), pos);
}
*/

/* Remove the item at position 'from' and insert it at position 'to'. */
static void Ordered_moveItem (Ordered me, integer from, integer to) {
	if (from < 1 || from > my size) {
		from = my size;
	}
	if (to < 1 || to > my size) {
		to = my size;
	}
	if (from == to) {
		return;
	}
	Daata tmp = my at [from];
	if (from > to) {
		for (integer i = from; i > to; i --) {
			my at [i] = my at [i - 1];
		}
	} else {
		for (integer i = from; i < to; i ++) {
			my at [i] = my at [i + 1];
		}
	}
	my at [to] = tmp;
}

#pragma mark - Widget updates

static void notifyNumberOfSelected (CategoriesEditor me) {
	autoMelderString tmp;
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);  // waste
	if (posCount > 0) {
		MelderString_append (&tmp, posCount, U" selection", (posCount > 1 ? U"s." : U"."));
	}
	if (tmp.string) GuiLabel_setText (my outOfView, tmp.string);
}

static void updateUndoAndRedoMenuItems (CategoriesEditor me)
{
	const char32 *commandName;

	/*
	 * Menu item `Undo`.
	 */
	bool undoItemIsSensitive = true;
	if (commandName = CommandHistory_commandName (my history.get(), 0), ! commandName) {
		commandName = U"nothing";
		undoItemIsSensitive = false;
	}
	GuiButton_setText (my undo, Melder_cat (U"Undo ", U"\"", commandName, U"\""));
	GuiThing_setSensitive (my undo, undoItemIsSensitive);

	/*
	 * Menu item `Redo`.
	 */
	bool redoItemIsSensitive = true;
	if (commandName = CommandHistory_commandName (my history.get(), 1), ! commandName) {
		commandName = U"nothing";
		redoItemIsSensitive = false;
	}
	GuiButton_setText (my redo, Melder_cat (U"Redo ", U"\"", commandName, U"\""));
	GuiThing_setSensitive (my redo, redoItemIsSensitive);
}

static void updateWidgets (CategoriesEditor me) {   // all buttons except undo & redo
	Categories data = (Categories) my data;
	integer size = data->size;
	bool insert = false, insertAtEnd = true, replace = false, remove = false;
	bool moveUp = false, moveDown = false;
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posList.peek()) {
		integer firstPos = posList[1], lastPos = posList[posCount];
		bool contiguous = ( lastPos - firstPos + 1 == posCount );
		moveUp = contiguous && firstPos > 1;
		moveDown = contiguous && lastPos < size;
		my position = firstPos;
		remove = true;
		replace = true;
		//insertAtEnd = false;
		if (posCount == 1) {
			insert = true;
			//if (posList[1] == size) insertAtEnd = true;
			if (size == 1 && ! str32cmp (CategoriesEditor_EMPTYLABEL,
			                           OrderedOfString_itemAtIndex_c ((OrderedOfString) my data, 1))) {
				remove = false;
			}
		}
	}
	GuiThing_setSensitive (my insert,      insert);
	GuiThing_setSensitive (my insertAtEnd, insertAtEnd);
	GuiThing_setSensitive (my replace,     replace);
	GuiThing_setSensitive (my remove,      remove);
	GuiThing_setSensitive (my moveUp,      moveUp);
	GuiThing_setSensitive (my moveDown,    moveDown);
	if (my history) {
		updateUndoAndRedoMenuItems (me);
	}
	notifyNumberOfSelected (me);
}

static void update (CategoriesEditor me, integer from, integer to, const integer *select, integer nSelect) {
	Categories data = (Categories) my data;
	integer size = data->size;

	if (size == 0) {
		autoSimpleString str = SimpleString_create (CategoriesEditor_EMPTYLABEL);
		data -> addItem_move (str.move());
		update (me, 0, 0, nullptr, 0);
		return;
	}
	if (from == 0 && from == to) {
		from = 1; to = size;
	}
	if (from < 1 || from > size) {
		from = size;
	}
	if (to < 1 || to > size) {
		to = size;
	}
	if (from > to) {
		integer ti = from; from = to; to = ti;
	}

	// Begin optimization: add the items from a table instead of separately.
	try {
		autostring32vector table (from, to);
		integer itemCount = GuiList_getNumberOfItems (my list);
		for (integer i = from; i <= to; i++) {
			char wcindex[20];
			snprintf (wcindex,20, "%5ld ", i);
			table[i] = Melder_dup_f (Melder_cat (Melder_peek8to32 (wcindex), OrderedOfString_itemAtIndex_c ((OrderedOfString) my data, i)));
		}
		if (itemCount > size) { // some items have been removed from Categories?
			for (integer j = itemCount; j > size; j --) {
				GuiList_deleteItem (my list, j);
			}
			itemCount = size;
		}
		if (to > itemCount) {
			for (integer j = 1; j <= to - itemCount; j ++) {
				GuiList_insertItem (my list, table [itemCount + j], 0);
			}
		}
		if (from <= itemCount) {
			integer n = (to < itemCount ? to : itemCount);
			for (integer j = from; j <= n; j++) {
				GuiList_replaceItem (my list, table[j], j);
			}
		}
	} catch (MelderError) {
		throw;
	}

	// End of optimization

	// HIGHLIGHT

	GuiList_deselectAllItems (my list);
	if (size == 1) { /* the only item is always selected */
		const char32 *catg = OrderedOfString_itemAtIndex_c ((OrderedOfString) my data, 1);
		GuiList_selectItem (my list, 1);
		updateWidgets (me);   // instead of "notify". BUG?
		GuiText_setString (my text, catg);
	} else if (nSelect > 0) {
		// Select but postpone highlighting

		for (integer i = 1; i <= nSelect; i++) {
			GuiList_selectItem (my list, select[i] > size ? size : select[i]);
		}
	}

	// VIEWPORT

	{
		integer top = GuiList_getTopPosition (my list), bottom = GuiList_getBottomPosition (my list);
		integer visible = bottom - top + 1;
		if (nSelect == 0) {
			top = my position - visible / 2;
		} else if (select[nSelect] < top) {
			// selection above visible area
			top = select[1];
		} else if (select[1] > bottom) {
			// selection below visible area
			top = select[nSelect] - visible + 1;
		} else {
			integer deltaTopPos = -1, nUpdate = to - from + 1;
			if ( (from == select[1] && to == select[nSelect]) || // Replace
			        (nUpdate > 2 && nSelect == 1) /* Inserts */) {
				deltaTopPos = 0;
			} else if (nUpdate == nSelect + 1 && select[1] == from + 1) { // down
				deltaTopPos = 1;
			}
			top += deltaTopPos;
		}
		if (top + visible > size) {
			top = size - visible + 1;
		}
		if (top < 1) {
			top = 1;
		}
		GuiList_setTopPosition (my list, top);
	}
}

#pragma mark - Commands for Undo and Redo

Thing_define (CategoriesEditorCommand, Command) {
	autoCategories categories;
	integer *selection;
	integer nSelected, newPos;

	void v_destroy () noexcept
		override;
};

Thing_implement (CategoriesEditorCommand, Command, 0);

void structCategoriesEditorCommand :: v_destroy () noexcept {
	NUMvector_free (selection, 1);
	CategoriesEditorCommand_Parent :: v_destroy ();
}

static void CategoriesEditorCommand_init (CategoriesEditorCommand me, const char32 *name, Thing boss,
        Command_Callback execute, Command_Callback undo, integer /*nCategories*/, integer nSelected) {

	my nSelected = nSelected;
	Command_init (me, name, boss, execute, undo);
	my categories = Categories_create();
	my selection = NUMvector <integer> (1, nSelected);
}

#pragma mark Insert

Thing_define (CategoriesEditorInsert, CategoriesEditorCommand) {
};

Thing_implement (CategoriesEditorInsert, CategoriesEditorCommand, 0);

static int CategoriesEditorInsert_execute (CategoriesEditorInsert me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);
	{// scope
		autoSimpleString str = Data_copy (my categories->at [1]);
		categories -> addItemAtPosition_move (str.move(), my selection [1]);
	}
	update (editor, my selection [1], 0, my selection, 1);
	return 1;
}

static int CategoriesEditorInsert_undo (CategoriesEditorInsert me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	categories -> removeItem (my selection [1]);
	update (editor, my selection [1], 0, my selection, 1);
	return 1;
}

static autoCategoriesEditorInsert CategoriesEditorInsert_create (Thing boss, autoSimpleString str, int position) {
	try {
		autoCategoriesEditorInsert me = Thing_new (CategoriesEditorInsert);
		CategoriesEditorCommand_init (me.get(), U"Insert", boss, CategoriesEditorInsert_execute, CategoriesEditorInsert_undo, 1, 1);
		my selection [1] = position;
		my categories -> addItem_move (str.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorInsert not created.");
	}
}

#pragma mark Remove

Thing_define (CategoriesEditorRemove, CategoriesEditorCommand) {
};

Thing_implement (CategoriesEditorRemove, CategoriesEditorCommand, 0);

static int CategoriesEditorRemove_execute (CategoriesEditorRemove me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	for (integer i = my nSelected; i >= 1; i--) {
		autoSimpleString item = Data_copy (categories->at [my selection [i]]);   // FIXME this copy can probably be replaced with a move
		my categories -> addItemAtPosition_move (item.move(), 1);
		categories -> removeItem (my selection [i]);
	}
	update (editor, my selection [1], 0, nullptr, 0);
	return 1;
}

static int CategoriesEditorRemove_undo (CategoriesEditorRemove me) {
	CategoriesEditor editor = (CategoriesEditor) my boss;
	Categories categories = (Categories) editor -> data;

	for (integer i = 1; i <= my nSelected; i ++) {
		autoSimpleString item = Data_copy (my categories->at [i]);
		categories -> addItemAtPosition_move (item.move(), my selection [i]);
	}
	update (editor, my selection [1], 0, my selection, my nSelected);
	return 1;
}

static autoCategoriesEditorRemove CategoriesEditorRemove_create (Thing boss, integer *posList, integer posCount) {
	try {
		autoCategoriesEditorRemove me = Thing_new (CategoriesEditorRemove);
		CategoriesEditorCommand_init (me.get(), U"Remove", boss, CategoriesEditorRemove_execute,
		                              CategoriesEditorRemove_undo, posCount, posCount);
		for (integer i = 1; i <= posCount; i ++) {
			my selection [i] = posList [i];
		}
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorRemove not created.");
	}
}

#pragma mark Replace

Thing_define (CategoriesEditorReplace, CategoriesEditorCommand) {
};

Thing_implement (CategoriesEditorReplace, CategoriesEditorCommand, 0);

static int CategoriesEditorReplace_execute (CategoriesEditorReplace me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	for (integer i = my nSelected; i >= 1; i --) {
		autoSimpleString str = Data_copy (my categories->at [1]);
		my categories -> addItemAtPosition_move (autoSimpleString (categories->at [my selection [i]]), 2);   // YUCK
		categories->at [my selection [i]] = str.releaseToAmbiguousOwner();
	}
	update (editor, my selection [1], my selection [my nSelected], my selection, my nSelected);
	return 1;
}

static int CategoriesEditorReplace_undo (CategoriesEditorReplace me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	for (integer i = 1; i <= my nSelected; i ++) {
		autoSimpleString str = Data_copy (my categories->at [i + 1]);
		categories -> replaceItem_move (str.move(), my selection [i]);
	}
	update (editor, my selection [1], my selection[my nSelected], my selection, my nSelected);
	return 1;
}

static autoCategoriesEditorReplace CategoriesEditorReplace_create (Thing boss, autoSimpleString str, integer *posList, integer posCount) {
	try {
		autoCategoriesEditorReplace me = Thing_new (CategoriesEditorReplace);
		CategoriesEditorCommand_init (me.get(), U"Replace", boss, CategoriesEditorReplace_execute,
		                              CategoriesEditorReplace_undo, posCount + 1, posCount);
		for (integer i = 1; i <= posCount; i ++) {
			my selection [i] = posList [i];
		}
		my categories -> addItem_move (str.move());
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorReplace not created.");
	}
}

#pragma mark MoveUp

Thing_define (CategoriesEditorMoveUp, CategoriesEditorCommand) {
};

Thing_implement (CategoriesEditorMoveUp, CategoriesEditorCommand, 0);

static int CategoriesEditorMoveUp_execute (CategoriesEditorMoveUp me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	Ordered_moveItems ((Ordered) categories, my selection, my nSelected, my newPos);   // FIXME cast
	autoNUMvector<integer> selection (1, my nSelected);
	for (integer i = 1; i <= my nSelected; i ++) {
		selection[i] = my newPos + i - 1;
	}
	update (editor, my newPos, my selection[my nSelected], selection.peek(), my nSelected);
	return 1;
}

static int CategoriesEditorMoveUp_undo (CategoriesEditorMoveUp me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	for (integer i = 1; i <= my nSelected; i ++) {
		Ordered_moveItem ((Ordered) categories, my newPos, my selection [my nSelected]);   // FIXME cast
	}
	update (editor, my newPos, my selection[my nSelected], my selection, my nSelected);
	return 1;
}

static autoCategoriesEditorMoveUp CategoriesEditorMoveUp_create (Thing boss, integer *posList, integer posCount, integer newPos) {

	try {
		autoCategoriesEditorMoveUp me = Thing_new (CategoriesEditorMoveUp);
		CategoriesEditorCommand_init (me.get(), U"Move up", boss, CategoriesEditorMoveUp_execute, CategoriesEditorMoveUp_undo, 0, posCount);
		for (integer i = 1; i <= posCount; i ++) {
			my selection [i] = posList [i];
		}
		my newPos = newPos;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorMoveUp not created.");
	}
}

#pragma mark MoveDown

Thing_define (CategoriesEditorMoveDown, CategoriesEditorCommand) {
};

Thing_implement (CategoriesEditorMoveDown, CategoriesEditorCommand, 0);

static int CategoriesEditorMoveDown_execute (CategoriesEditorMoveDown me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	Ordered_moveItems ((Ordered) categories, my selection, my nSelected, my newPos);   // FIXME cast
	autoNUMvector<integer> selection (1, my nSelected);
	for (integer i = 1; i <= my nSelected; i++) {
		selection[i] = my newPos - my nSelected + i;
	}
	update (editor, my selection[1], my newPos, selection.peek(), my nSelected);
	return 1;
}

static int CategoriesEditorMoveDown_undo (CategoriesEditorMoveDown me) {
	CategoriesEditor editor = static_cast<CategoriesEditor> (my boss);
	Categories categories = static_cast<Categories> (editor -> data);

	for (integer i = 1; i <= my nSelected; i ++) {
		Ordered_moveItem ((Ordered) categories, my newPos, my selection [1]); // TODO 1 or i ??     // FIXME cast
	}
	integer from = my selection[1];
	update (editor, ( from > 1 ? from -- : from ), my newPos, my selection, my nSelected);
	return 1;
}

static autoCategoriesEditorMoveDown CategoriesEditorMoveDown_create (Thing boss, integer *posList,
        integer posCount, integer newPos) {
	try {
		autoCategoriesEditorMoveDown me = Thing_new (CategoriesEditorMoveDown);
		CategoriesEditorCommand_init (me.get(), U"Move down", boss, CategoriesEditorMoveDown_execute, CategoriesEditorMoveDown_undo, 0, posCount);
		for (integer i = 1; i <= posCount; i++) {
			my selection[i] = posList[i];
		}
		my newPos = newPos;
		return me;
	} catch (MelderError) {
		Melder_throw (U"CategoriesEditorMoveDown not created.");
	}
}

#pragma mark - Callbacks

static void gui_button_cb_remove (CategoriesEditor me, GuiButtonEvent /* event */) {
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posList.peek()) {
		autoCategoriesEditorRemove command = CategoriesEditorRemove_create (me, posList.peek(), posCount);
		if (! Command_do (command.get())) {
			return;
		}
		if (my history) {
			CommandHistory_insertItem_move (my history.get(), command.move());
		}
		updateWidgets (me);
	}
}

static void insert (CategoriesEditor me, int position) {
	autostring32 text = GuiText_getString (my text);
	if (str32len (text.peek()) != 0) {
		autoSimpleString str = SimpleString_create (text.peek());
		autoCategoriesEditorInsert command = CategoriesEditorInsert_create (me, str.move(), position);
		Command_do (command.get());
		if (my history) {
			CommandHistory_insertItem_move (my history.get(), command.move());
		}
		updateWidgets (me);
	}
}

static void gui_button_cb_insert (CategoriesEditor me, GuiButtonEvent /* event */) {
	insert (me, my position);
}

static void gui_button_cb_insertAtEnd (CategoriesEditor me, GuiButtonEvent /* event */) {
	Categories categories = (Categories) my data;
	insert (me, categories->size + 1);
	my position = categories->size;
}

static void gui_button_cb_replace (CategoriesEditor me, GuiButtonEvent /* event */) {
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posCount > 0) {
		autostring32 text = GuiText_getString (my text);
		if (str32len (text.peek()) != 0) {
			autoSimpleString str = SimpleString_create (text.peek());
			autoCategoriesEditorReplace command = CategoriesEditorReplace_create (me, str.move(), posList.peek(), posCount);
			Command_do (command.get());
			if (my history) {
				CommandHistory_insertItem_move (my history.get(), command.move());
			}
			updateWidgets (me);
		}
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveUp (CategoriesEditor me, GuiButtonEvent /* event */) {
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posCount > 0) {
		autoCategoriesEditorMoveUp command = CategoriesEditorMoveUp_create (me, posList.peek(), posCount, posList[1] - 1);
		Command_do (command.get());
		if (my history) {
			CommandHistory_insertItem_move (my history.get(), command.move());
		}
		updateWidgets (me);
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveDown (CategoriesEditor me, GuiButtonEvent /* event */) {
	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posCount > 0) {
		autoCategoriesEditorMoveDown command = CategoriesEditorMoveDown_create (me, posList.peek(), posCount, posList[posCount] + 1);
		Command_do (command.get());
		if (my history) {
			CommandHistory_insertItem_move (my history.get(), command.move());
		}
		updateWidgets (me);
	}
}

static void gui_list_cb_selectionChanged (CategoriesEditor me, GuiList_SelectionChangedEvent /* event */) {
	updateWidgets (me);
}

static void gui_list_cb_doubleClick (CategoriesEditor me, GuiList_DoubleClickEvent event) {
	Melder_assert (event -> list == my list);

	//  `my position` should just have been updated by the selectionChanged callback.

	integer posCount;
	autoNUMvector <integer> posList (GuiList_getSelectedPositions (my list, & posCount), 1);
	if (posCount == 1   // often or even usually true when double-clicking?
	    && posList [1] == my position)   // should be true, but we don't crash if it's false
	{
		const char32 *catg = OrderedOfString_itemAtIndex_c ((OrderedOfString) my data, my position);
		if (catg) {   // should be non-null, but we don't crash if not
			GuiText_setString (my text, catg);
		}
	}
}

static void gui_list_cb_scroll (CategoriesEditor me, GuiList_ScrollEvent /* event */) {
	notifyNumberOfSelected (me);
}

static void gui_button_cb_undo (CategoriesEditor me, GuiButtonEvent /* event */) {
	if (CommandHistory_offleft (my history.get())) {
		return;
	}
	Command command = CommandHistory_getItem (my history.get());
	Command_undo (command);
	CommandHistory_back (my history.get());
	updateWidgets (me);
}

static void gui_button_cb_redo (CategoriesEditor me, GuiButtonEvent /* event */) {
	CommandHistory_forth (my history.get());
	if (CommandHistory_offright (my history.get())) {
		return;
	}
	Command command = CommandHistory_getItem (my history.get());
	Command_do (command);
	updateWidgets (me);
}

#pragma mark - Editor methods

void structCategoriesEditor :: v_destroy () noexcept {
	CategoriesEditor_Parent :: v_destroy ();
}

void structCategoriesEditor :: v_createHelpMenuItems (EditorMenu menu) {
	CategoriesEditor_Parent :: v_createHelpMenuItems (menu);
	EditorMenu_addCommand (menu, U"CategoriesEditor help", '?', menu_cb_help);
}

// origin is at top left.
void structCategoriesEditor :: v_createChildren () {
	constexpr int menuBarOffset { 40 };
	constexpr int button_width { 130 }, button_height { menuBarOffset }, list_width { 260 }, list_height { 420 };
	constexpr int delta_x { 15 }, delta_y { menuBarOffset / 2 }, text_button_height { button_height / 2 };

	int left = 5, right = left + button_width, top = 3 + menuBarOffset, bottom = top + text_button_height;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Positions:", 0);
	left = right + delta_x ; right = left + button_width;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Values:", 0);

	left = 0; right = left + list_width; 
	// int buttons_top = (top = bottom + delta_y);
	int list_bottom = bottom = top + list_height;
	list = GuiList_create (our windowForm, left, right, top, bottom, true, 0);
	GuiList_setSelectionChangedCallback (list, gui_list_cb_selectionChanged, this);
	GuiList_setDoubleClickCallback (list, gui_list_cb_doubleClick, this);
	GuiList_setScrollCallback (list, gui_list_cb_scroll, this);
	GuiThing_show (list);

	int buttons_left = left = right + 2 * delta_x; right = left + button_width; bottom = top + button_height;
	GuiLabel_createShown (our windowForm, left, right, top, bottom, U"Value:", 0);
	left = right + delta_x; right = left + button_width;
	text = GuiText_createShown (our windowForm, left, right, top, bottom, 0);
	GuiText_setString (text, CategoriesEditor_EMPTYLABEL);

	left = buttons_left; right = left + button_width; top = bottom + delta_y; bottom = top + button_height;
	insert = GuiButton_createShown (our windowForm, left, right, top, bottom,	U"Insert", gui_button_cb_insert, this, GuiButton_DEFAULT);
	left = right + delta_x; right = left + button_width;
	replace = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Replace", gui_button_cb_replace, this, 0);
	left = buttons_left; right = left + int (1.5 * button_width); top = bottom + delta_y; bottom = top + button_height;
	insertAtEnd = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Insert at end", gui_button_cb_insertAtEnd, this, 0);
	top = bottom + delta_y; bottom = top + button_height;
	undo = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Undo", gui_button_cb_undo, this, 0);
	top = bottom + delta_y; bottom = top + button_height;
	redo = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Redo", gui_button_cb_redo, this, 0);
	top = bottom + delta_y; bottom = top + button_height;
	remove = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Remove", gui_button_cb_remove, this, 0);
	top = bottom + delta_y; bottom = top + button_height;
	moveUp = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Move selection up", gui_button_cb_moveUp, this, 0);
	top = bottom + delta_y; bottom = top + button_height;
	moveDown = GuiButton_createShown (our windowForm, left, right, top, bottom, U"Move selection down", gui_button_cb_moveDown, this, 0);

	top = list_bottom + delta_y; bottom = top + button_height; left = 5; right = left + 200;
	outOfView = GuiLabel_createShown (our windowForm, left, right, top, bottom, U"", 0);
}

void structCategoriesEditor :: v_dataChanged () {
	update (this, 0, 0, nullptr, 0);
	updateWidgets (this);
}

#pragma mark -

autoCategoriesEditor CategoriesEditor_create (const char32 *title, Categories data) {
	try {
		autoCategoriesEditor me = Thing_new (CategoriesEditor);
		Editor_init (me.get(), 20, 40, 600, 600, title, data);
		my history = CommandHistory_create ();
		update (me.get(), 0, 0, nullptr, 0);
		updateWidgets (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Categories window not created.");
	}
}

/* End of file CategoriesEditor.cpp */
