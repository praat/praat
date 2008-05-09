/* CategoriesEditor.c
 *
 * Copyright (C) 1993-2008 David Weenink
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at
 * your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

/*
 djmw 1995
 djmw 19980225 repaired a memory leak, caused by wrong inheritance for
 	 CategoriesEditorInsert command.
 djmw 20020408 GPL 
 djmw 20020408 Modified 'createMenus'
 djmw 20060111 Replaced Resources.h with Preferences.h
 djmw 20060328 Changed last argument to NULL in XtVaSetValues, XtVaGetValues and XtVaCreateManagedWidget
 	for 64-bit compatibility.
 djmw 20070620 Latest modification.
 pb 20080320 split off Help menu
 pb 20080321 new Editor API
*/

#define CategoriesEditor_TEXTMAXLENGTH 100

#include "CategoriesEditor.h"
//#include "Preferences.h"
#include "EditorM.h"


/* forward declarations */
static void update (I, long from, long to, const long *select, long nSelect);
static void update_dos (I);

wchar_t *CategoriesEditor_EMPTYLABEL = L"(empty)";

static int menu_cb_help (EDITOR_ARGS) { EDITOR_IAM (CategoriesEditor); Melder_help (L"CategoriesEditor"); return 1; }

/**************** Some methods for Collection  ****************/

/* Preconditions: */
/*	1 <= (position[i], newpos) <= size; */
/*	newpos <= position[1] || newpos >= position[npos] */
static void Ordered_moveItems (I, long position[], long npos, long newpos)
{
	iam (Ordered);
	Any *tmp = NULL;
	long i, pos, min = position[1], max = position[1];
	
	for (i=2; i <= npos; i++)
	{
		if (position[i] > max) max = position[i];
		else if (position[i] < min) min = position[i];
	}
	
	Melder_assert (min >= 1 && max <= my size && (newpos <= min || 
		newpos >= max));
	
	if (! (tmp = NUMpvector (1, npos))) return;
	
	/*
		'remove'
	*/
	
	for (i=1; i <= npos; i++)
	{
		tmp[i] = my item[position[i]];
		my item[position[i]] = NULL;
	}
	
	/*
		create a contiguous 'hole'
	*/
	
	if (newpos <= min)
	{
		for (pos=max, i=max; i >= newpos; i--)
		{
			if (my item[i]) my item[pos--]= my item[i];
		}
		pos = newpos;
	}
	else
	{
		for (pos=min, i=min; i <= newpos; i++)
		{
			if (my item[i]) my item[pos++]= my item[i];
		}
		pos = newpos - npos + 1;
	}
	
	/*
		fill the 'hole'
	*/
	
	for (i=1; i <= npos; i++)
	{
		my item[pos++] = tmp[i];
	}
	
	NUMpvector_free (tmp, 1);
}

static void Collection_replaceItemPos (I, Any item, long pos)
{
	iam (Collection);
	if (pos < 1 || pos > my size) return;
	forget (my item[pos]);
	my item[pos] = item;
}

/* Remove the item at position 'from' and insert it at position 'to'. */
static void Ordered_moveItem (I, long from, long to)
{
	iam (Ordered); Data tmp; long i;
	if (from < 1 || from > my size) from = my size;
	if (to < 1 || to > my size) to = my size;
	if (from == to) return;
	tmp = my item[from];
	if (from > to)
	{
		for (i=from; i > to; i--) my item[i] = my item[i-1];
	}
	else
	{
		for (i=from; i < to; i++) my item[i] = my item[i+1];
	}
	my item[to] = tmp;
}

/********************** General Command **********************/

#define CategoriesEditorCommand_members Command_members \
	Categories categories;  							\
	long *selection; long nSelected, newPos;
#define CategoriesEditorCommand_methods Command_methods
class_create (CategoriesEditorCommand, Command);

static void classCategoriesEditorCommand_destroy (I)
{
	iam (CategoriesEditorCommand);
	NUMlvector_free (my selection, 1);
	forget (my categories);
	inherited (CategoriesEditorCommand) destroy (me);
}

static int CategoriesEditorCommand_init (I, wchar_t *name,  Any data,
	int (*execute) (Any), int (*undo) (Any), int nCategories, int nSelected)
{
	iam (CategoriesEditorCommand);
	(void) nCategories;
	
	my nSelected = nSelected;
	return Command_init (me, name, data, execute, undo) &&
			(my categories = Categories_create()) &&
			(my selection = NUMlvector (1, nSelected));
}

class_methods (CategoriesEditorCommand, Command)
    class_method_local (CategoriesEditorCommand, destroy)
class_methods_end

/*********************** Insert Command ***********************/

#define CategoriesEditorInsert_members CategoriesEditorCommand_members
#define CategoriesEditorInsert_methods CategoriesEditorCommand_methods
class_create (CategoriesEditorInsert, CategoriesEditorCommand);

static int CategoriesEditorInsert_execute (I)
{
	iam (CategoriesEditorInsert); 
	CategoriesEditor editor = my data; 
	SimpleString str;
	if (! (str = Data_copy (((Categories) my categories)->item[1])) ||
		! Ordered_addItemPos (editor->data, str, my selection[1]))
	{
		forget (str); return 0;
	}
	update (editor, my selection[1], 0, my selection, 1);
	return 1;
}

static int CategoriesEditorInsert_undo (I)
{
	iam (CategoriesEditorInsert); 
	CategoriesEditor editor = my data;
	Collection_removeItem (editor->data, my selection[1]);
	update (editor, my selection[1], 0, my selection, 1);
	return 1;
}

static Any CategoriesEditorInsert_create (Any data, Any str, int position)
{
	CategoriesEditorInsert me = new (CategoriesEditorInsert);
	if (! me || ! CategoriesEditorCommand_init (me, L"Insert", data,
		CategoriesEditorInsert_execute, CategoriesEditorInsert_undo, 1, 1))
	{
		forget (me); return NULL;
	}
	my selection[1] = position;
	Collection_addItem (my categories, str);
	return me;
}

class_methods (CategoriesEditorInsert, CategoriesEditorCommand)
class_methods_end

/*********************** Remove Command ***********************/

#define CategoriesEditorRemove_members CategoriesEditorCommand_members
#define CategoriesEditorRemove_methods CategoriesEditorCommand_methods
class_create (CategoriesEditorRemove, CategoriesEditorCommand);

static int CategoriesEditorRemove_execute (I)
{
	iam (CategoriesEditorRemove); 
	long i;
	CategoriesEditor editor = my data;
	Categories categories = editor->data;
	
	for (i = my nSelected; i >= 1; i--)
	{
		Ordered_addItemPos (my categories, categories->item[my selection[i]], 1);
		categories->item[my selection[i]] = NULL;
		Collection_removeItem (categories, my selection[i]);
	}
	update (editor, my selection[1], 0, NULL, 0);
	return 1;
}

static int CategoriesEditorRemove_undo (I)
{
	iam (CategoriesEditorRemove);
	int i;
	CategoriesEditor editor = my data; 
	Categories categories = editor->data;
	
	for (i = 1; i <= my nSelected; i++)
	{
		Data item = Data_copy (my categories->item[i]);
		Ordered_addItemPos (categories, item, my selection[i]);
	}
	update (editor, my selection[1], 0, my selection, my nSelected);
	return 1;
}

static Any CategoriesEditorRemove_create (Any data, long *posList, long posCount)
{
	CategoriesEditorRemove me = new (CategoriesEditorRemove); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, L"Remove", data,
		CategoriesEditorRemove_execute, CategoriesEditorRemove_undo, 
		posCount, posCount)) forget (me);
	for (i = 1; i <= posCount; i++) my selection[i] = posList[i];
	return me;
}

class_methods (CategoriesEditorRemove, CategoriesEditorCommand)
class_methods_end
//update (me);
/*********************** Replace Command ***********************/
		
#define CategoriesEditorReplace_members CategoriesEditorCommand_members
#define CategoriesEditorReplace_methods CategoriesEditorCommand_methods
class_create (CategoriesEditorReplace, CategoriesEditorCommand);

static int CategoriesEditorReplace_execute (I)
{
	iam (CategoriesEditorReplace); 
	long i;
	CategoriesEditor editor = my data;
	Categories categories = editor -> data;
	
	for (i = my nSelected; i >= 1; i--)
	{
		Data str = Data_copy (my categories -> item[1]);
		Ordered_addItemPos (my categories, 
			categories -> item[my selection[i]], 2);
		categories -> item[my selection[i]] =  str;
	}
	update (editor, my selection[1], my selection[my nSelected], 
		my selection, my nSelected);
	return 1;
}

static int CategoriesEditorReplace_undo (I)
{
	iam (CategoriesEditorReplace); long i;
	CategoriesEditor editor = my data;
	Categories categories = editor -> data;
	
	for (i = 1; i <= my nSelected; i++)
	{
		Data str = Data_copy (my categories -> item[i+1]);
		Collection_replaceItemPos (categories, str, my selection[i]);
	}
	update (editor, my selection[1], my selection[my nSelected], 
		my selection, my nSelected);
	return 1;
}

static Any CategoriesEditorReplace_create (Any data, Any str, long *posList, long posCount)
{
	CategoriesEditorReplace me = new (CategoriesEditorReplace);
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, L"Replace", data,
		CategoriesEditorReplace_execute, CategoriesEditorReplace_undo, 
			posCount + 1, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i];
	}
	Collection_addItem (my categories, str);	
	return me;
}

class_methods (CategoriesEditorReplace, CategoriesEditorCommand)
class_methods_end

/*********************** MoveUp Command ***********************/
		
#define CategoriesEditorMoveUp_members CategoriesEditorCommand_members
#define CategoriesEditorMoveUp_methods CategoriesEditorCommand_methods
class_create (CategoriesEditorMoveUp, CategoriesEditorCommand);

static int CategoriesEditorMoveUp_execute (I)
{
	iam (CategoriesEditorMoveUp);
	CategoriesEditor editor = my data;
	long i, *selection;
	
	Ordered_moveItems (editor->data, my selection, my nSelected, my newPos);
	if (! (selection = NUMlvector (1, my nSelected))) return 0;
	for (i = 1; i <= my nSelected; i++)
	{
		selection[i] = my newPos + i - 1;
	}
	update (editor, my newPos, my selection[my nSelected], selection,
		my nSelected);
	NUMlvector_free (selection, 1);
	return 1;
}

static int CategoriesEditorMoveUp_undo (I)
{
	iam (CategoriesEditorMoveUp); long i;
	CategoriesEditor editor = my data;
	
	for (i = 1; i <= my nSelected; i++)
	{
		Ordered_moveItem (editor->data, my newPos, my selection[my nSelected]);
	}
	update (editor, my newPos, my selection[my nSelected], my selection,
		my nSelected);
	return 1;
}

static Any CategoriesEditorMoveUp_create (Any data, long *posList, 
	long posCount, long newPos)
{
	CategoriesEditorMoveUp me = new (CategoriesEditorMoveUp); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, L"Move up", data,
		CategoriesEditorMoveUp_execute, CategoriesEditorMoveUp_undo, 
		0, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i];
	}
	my newPos = newPos;
	return me;
}

class_methods (CategoriesEditorMoveUp, CategoriesEditorCommand)
class_methods_end

/*********************** MoveDown Command ***********************/

#define CategoriesEditorMoveDown_members CategoriesEditorCommand_members
#define CategoriesEditorMoveDown_methods CategoriesEditorCommand_methods
class_create (CategoriesEditorMoveDown, CategoriesEditorCommand);

static int CategoriesEditorMoveDown_execute (I)
{
	iam (CategoriesEditorMoveDown);
	CategoriesEditor editor = my data;
	long i, *selection;
	
	Ordered_moveItems (editor->data, my selection, my nSelected, my newPos);
	if (! (selection = NUMlvector (1, my nSelected))) return 0;
	for (i = 1; i <= my nSelected; i++)
	{
		selection[i] = my newPos - my nSelected + i;
	}
	update (editor, my selection[1], my newPos, selection, my nSelected);
	NUMlvector_free (selection, 1);
	return 1;
}

static int CategoriesEditorMoveDown_undo (I)
{
	iam (CategoriesEditorMoveDown); long i, from = my selection[1];
	CategoriesEditor editor = my data;
	for (i=1; i <= my nSelected; i++)
	{
		Ordered_moveItem (editor->data, my newPos, my selection[1]);
	}
	update (editor, (from > 1 ? from-- : from), my newPos, my selection, 
		my nSelected);
	return 1;
}

static Any CategoriesEditorMoveDown_create (Any data, long *posList, 
	long posCount, long newPos)
{
	CategoriesEditorMoveDown me = new (CategoriesEditorMoveDown); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, L"Move down", data,
		CategoriesEditorMoveDown_execute, CategoriesEditorMoveDown_undo, 
			0, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i];
	}
	my newPos = newPos;
	return me;
}

class_methods (CategoriesEditorMoveDown, CategoriesEditorCommand)
class_methods_end
				
/********************* Commands (End)  *************************************/

static void notifyOutOfView (I)
{
	iam (CategoriesEditor); 
	MelderString tmp = { 0 };
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount); 
	if (posList != NULL)
	{
		long outOfView = 0, top = GuiList_getTopPosition (my list), bottom = GuiList_getBottomPosition (my list);
		
		for (long i = posCount; i > 0; i--)
		{ 
			if (posList[i] < top || posList[i] > bottom) outOfView++;
		}
		NUMlvector_free (posList, 1);
		if (outOfView > 0)
		{
			MelderString_append2 (&tmp, Melder_integer (outOfView), L" selection(s) out of view");
		}
	}
	GuiLabel_setString (my outOfView, tmp.string);
	MelderString_free (&tmp);
}

static void update_dos (I)
{
	iam (CategoriesEditor);
	wchar_t *name;
	MelderString tmp = { 0 };
	Boolean undoSense = True, redoSense = True;
	/*
		undo
	*/
	
	if (! (name = CommandHistory_commandName (my history, 0)))
	{
			name = L"nothing"; undoSense = False;
	}
	
	MelderString_append4 (&tmp, L"Undo ", L"\"", name, L"\"");
	GuiButton_setString (my undo, tmp.string);
	GuiObject_setSensitive (my undo, undoSense);
	
	/*
		redo
	*/
	
	if (! (name = CommandHistory_commandName (my history, 1)))
	{
		name = L"nothing"; redoSense = False;
	}
	MelderString_empty (&tmp);
	MelderString_append4 (&tmp, L"Redo ", L"\"", name, L"\"");
	GuiButton_setString (my redo, tmp.string);
	GuiObject_setSensitive (my redo, redoSense);
	MelderString_free (&tmp);
}

static void updateWidgets (I) /*all buttons except undo & redo */
{
	iam (CategoriesEditor); 
	long size = ((Categories) my data)->size;
	Boolean insert = False, insertAtEnd = True, replace = False, remove = False;
	Boolean moveUp = False, moveDown = False;
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount);
	if (posList != NULL)
	{
		int firstPos = posList[1], lastPos = posList[posCount];
		int contiguous = lastPos - firstPos + 1 == posCount;
		moveUp = contiguous && firstPos > 1;
		moveDown = contiguous && lastPos < size;
		my position = firstPos;	
		remove = True; replace = True; insertAtEnd = False;
		if (posCount == 1)
		{
			insert = True;
			if (posList[1] == size) insertAtEnd = True;
			if (size == 1 && ! wcscmp (CategoriesEditor_EMPTYLABEL,
				OrderedOfString_itemAtIndex_c (my data, 1))) remove = False; 
		}
		NUMlvector_free (posList, 1);
	}
	GuiObject_setSensitive (my insert, insert); GuiObject_setSensitive (my insertAtEnd, insertAtEnd);
	GuiObject_setSensitive (my replace, replace); GuiObject_setSensitive (my remove, remove);
	GuiObject_setSensitive (my moveUp, moveUp); GuiObject_setSensitive (my moveDown, moveDown);
	if (my history) update_dos (me);
	notifyOutOfView (me);
}

static void update (I, long from, long to, const long *select, long nSelect)
{
	iam (CategoriesEditor);
	int i, itemCount, size = ((Categories) my data)->size;
	
	if (size == 0)
	{
		SimpleString str = SimpleString_create (CategoriesEditor_EMPTYLABEL);
		if (! str || ! Collection_addItem (my data, str)) return;
		update (me, 0, 0, NULL, 0);
		return;
	}
	if (from == 0 && from == to)
	{
		from = 1; to = size;
	} 
	if (from < 1 || from > size) from = size;
	if (to < 1 || to > size) to = size;
	if (from > to)
	{
		i = from; from = to; to = i;
	}
	
	/*
		Begin optimization: add the items from a table instead of separately.
	*/
	
	{
		const wchar_t **table = NULL; 
		MelderString itemText  = { 0 };
		int k;

		if (! (table = Melder_malloc (const wchar_t *, to - from + 1))) return;
		itemCount = GuiList_getNumberOfItems (my list);
		/*for (k = 0, i = from; i <= to; i++)
		{
			wchar_t itemText[CategoriesEditor_TEXTMAXLENGTH+10]; 
			swprintf (itemText, CategoriesEditor_TEXTMAXLENGTH+10, L"%6d     %.*ls", i, CategoriesEditor_TEXTMAXLENGTH,
				OrderedOfString_itemAtIndex_c (my data, i));
			table[k++] = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		}*/
		for (k = 0, i = from; i <= to; i++)
		{
			MelderString_empty (&itemText);
			MelderString_append2 (&itemText, Melder_integer (i), OrderedOfString_itemAtIndex_c (my data, i));
			table[k++] = Melder_wcsdup (itemText.string);
		}
		if (itemCount > size) /* some items have been removed from Categories? */
		{
			for (long j = itemCount; j > size; j --) {
				GuiList_deleteItem (my list, j);
			}
			itemCount = size;
		}
		if (to > itemCount)
		{
			for (long j = 1; j <= to - itemCount; j ++) {
				GuiList_insertItem (my list, table [itemCount - from + j], 0);
			}
		}
		if (from <= itemCount)
		{
			long n = ( to < itemCount ? to : itemCount );
			for (long j = 0; j < n; j ++) {
				GuiList_replaceItem (my list, table [j], from + j);
			}
		}
		for (k = 0, i = from; i <= to; i++)
		{
			Melder_free (table[k++]);
		}
		Melder_free (table);
		MelderString_free (&itemText);
	}
	
	/*
		End of optimization
	*/
	
	/*
		HIGHLIGHT
	*/

	GuiList_deselectAllItems (my list);
	if (size == 1) /* the only item is always selected */
	{
		const wchar_t *catg = OrderedOfString_itemAtIndex_c (my data, 1);
		GuiList_selectItem (my list, 1);
		updateWidgets (me);   // instead of "notify". BUG?
		GuiText_setString (my text, catg);
	}
	else if (nSelect > 0)
	{
		/*
			Select but postpone highlighting
		*/
		for (i = 1; i <= nSelect; i++)
		{
			GuiList_selectItem (my list, select[i] > size ? size : select[i]);
		}
	}
	
	/*
		VIEWPORT
	*/
	
	{
		long top = GuiList_getTopPosition (my list), bottom = GuiList_getBottomPosition (my list);
		long visible = bottom - top + 1;
		if (nSelect == 0)
		{
			top = my position - visible / 2;
		}
		else if (select[nSelect] < top)
		{
			/* selection above visible area*/
			top = select[1];
		}
		else if (select[1] > bottom)
		{
			/* selection below visible area */
			top = select[nSelect] - visible + 1;
		}
		else
		{
			int deltaTopPos = -1, nUpdate = to - from + 1;
			if ((from == select[1] && to == select[nSelect]) /* Replace */ ||
				(nUpdate > 2 && nSelect == 1) /* Inserts */) deltaTopPos = 0;
			else if (nUpdate == nSelect + 1 && select[1] == from + 1) /* down */
				deltaTopPos = 1;
			top += deltaTopPos;
		}
		if (top + visible > size) top = size - visible + 1;
		if (top < 1) top = 1;
		GuiList_setTopPosition (my list, top);
	}
}

static void gui_button_cb_remove (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount);
	if (posList != NULL)
	{
		CategoriesEditorRemove command = CategoriesEditorRemove_create 
			(me, posList, posCount);
		if (! command || ! Command_do (command))
		{
			forget (command); NUMlvector_free (posList, 1); return;
		}
		if (my history) CommandHistory_insertItem (my history, command);
		NUMlvector_free (posList, 1);	
		updateWidgets (me);
	}
}

static void insert (I, int position)
{
	iam (CategoriesEditor);
	SimpleString str = NULL;
	CategoriesEditorInsert command = NULL;
	wchar_t *text = GuiText_getString (my text);
	
	if (wcslen (text) == 0 || ! (str = SimpleString_create (text)) ||
		! (command = CategoriesEditorInsert_create (me, str, position)) ||
		! Command_do (command)) goto end;
	if (my history) CommandHistory_insertItem (my history, command);
	Melder_free (text);
	updateWidgets (me);
	return;
end:
	Melder_free (text);
	forget (str);
	forget (command);
}

static void gui_button_cb_insert (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	insert (me, my position);
}

static void gui_button_cb_insertAtEnd (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	Categories categories = my data;
	insert (me, categories->size + 1);
	my position = categories->size;
}

static void gui_button_cb_replace (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount);
	if (posList != NULL)
	{
		CategoriesEditorReplace command = NULL;
		wchar_t *text = GuiText_getString (my text);
		SimpleString str = NULL;
		
		if (wcslen (text) == 0 || ! (str = SimpleString_create (text)) ||
			! (command = CategoriesEditorReplace_create (me, str, posList,
				posCount)) ||
			! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		NUMlvector_free (posList, 1);
		Melder_free (text);
		updateWidgets (me);
		return;
end:
		Melder_free (text);
		forget (str);
		forget (command);
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveUp (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount);
	if (posList != NULL)
	{
		CategoriesEditorMoveUp command = CategoriesEditorMoveUp_create 
			(me, posList, posCount, posList[1]-1);
		if (! command || ! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		NUMlvector_free (posList, 1);
		updateWidgets (me);
		return;
end:
		NUMlvector_free (posList, 1);
		forget (command);
	}
}

/* Precondition: contiguous selection */
static void gui_button_cb_moveDown (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	long posCount, *posList = GuiList_getSelectedPositions (my list, & posCount);
	if (posList != NULL)
	{
		CategoriesEditorMoveDown command = CategoriesEditorMoveDown_create 
			(me, posList, posCount, posList[posCount] + 1);
		if (! command || ! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		NUMlvector_free (posList, 1);
		updateWidgets (me);
		return;
end:
		NUMlvector_free (posList, 1);
		forget (command);
	}
}


static void gui_cb_scroll (GUI_ARGS) {
	GUI_IAM (CategoriesEditor);
	notifyOutOfView (me);
}

static void gui_list_cb_double_click (void *void_me, GuiListEvent event) {
	(void) event;
	iam (CategoriesEditor);
	const wchar_t *catg = OrderedOfString_itemAtIndex_c (my data, my position);
	GuiText_setString (my text, catg);
}

static void gui_list_cb_extended (void *void_me, GuiListEvent event) {
	(void) event;
	iam (CategoriesEditor);
	updateWidgets (me);
}

static void gui_button_cb_undo (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	if (CommandHistory_offleft (my history)) return;
	Command_undo (CommandHistory_getItem (my history));
	CommandHistory_back (my history);
	updateWidgets (me);
}

static void gui_button_cb_redo (I, GuiButtonEvent event) {
	(void) event;
	iam (CategoriesEditor);
	CommandHistory_forth (my history);	
	if (CommandHistory_offright (my history)) return;
	Command_do (CommandHistory_getItem (my history));
	updateWidgets (me);
}

static void destroy (I)
{
	iam (CategoriesEditor);
	forget (my history); /* !! Editor */
	inherited (CategoriesEditor) destroy (me);
}

static void createHelpMenuItems (I, EditorMenu menu)
{
	iam (CategoriesEditor);
	inherited (CategoriesEditor) createHelpMenuItems (me, menu);
	EditorMenu_addCommand (menu, L"CategoriesEditor help", '?', menu_cb_help);
}

static void createChildren (I)
{
	iam (CategoriesEditor);
	Widget vertScrollBar;
	int menuBarOffset = 40;
	
	GuiLabel_createShown (my dialog, 5, 100, 3+menuBarOffset, Gui_AUTOMATIC, L"Positions:", 0);
	GuiLabel_createShown (my dialog, 100, 190, 3+menuBarOffset, Gui_AUTOMATIC, L"Values:", 0);

	my list = GuiList_create (my dialog, 0, 260, 40+menuBarOffset, 140+menuBarOffset, true, NULL);
	GuiList_setSelectionChangedCallback (my list, gui_list_cb_extended, me);
	GuiList_setDoubleClickCallback (my list, gui_list_cb_double_click, me);
	GuiObject_show (my list);

	/*
		The valueChangedCallback does not get any notification in case of:
			drag, decrement, increment, pageIncrement & pageDecrement
	*/

	#ifndef _WIN32
		XtVaGetValues (GuiObject_parent (my list), XmNverticalScrollBar, & vertScrollBar, NULL);
		XtAddCallback (vertScrollBar, XmNvalueChangedCallback, gui_cb_scroll, 
			(XtPointer) me);
		XtAddCallback (vertScrollBar, XmNdragCallback, gui_cb_scroll, (XtPointer) me);
		XtAddCallback (vertScrollBar, XmNdecrementCallback, gui_cb_scroll, 
			(XtPointer) me);
		XtAddCallback (vertScrollBar, XmNincrementCallback, gui_cb_scroll, 
			(XtPointer) me);
		XtAddCallback (vertScrollBar, XmNpageIncrementCallback, gui_cb_scroll, 
			(XtPointer) me);
		XtAddCallback (vertScrollBar, XmNpageDecrementCallback, gui_cb_scroll, 
			(XtPointer) me);
	#endif

	GuiLabel_createShown (my dialog, 280, 370, 3+menuBarOffset, Gui_AUTOMATIC, L"Value:", 0);
	my text = GuiText_createShown (my dialog, 370, 510, 3+menuBarOffset, Gui_AUTOMATIC, 0);
	GuiText_setString (my text, CategoriesEditor_EMPTYLABEL);
				 
	my insert = GuiButton_createShown (my dialog, 280, 370, 43+menuBarOffset, Gui_AUTOMATIC,
		L"Insert", gui_button_cb_insert, me, GuiButton_DEFAULT);
	my replace = GuiButton_createShown (my dialog, 380, 470, 43+menuBarOffset, Gui_AUTOMATIC,
		L"Replace", gui_button_cb_replace, me, 0);
	my insertAtEnd = GuiButton_createShown (my dialog, 280, 470, 83+menuBarOffset, Gui_AUTOMATIC,
		L"Insert at end", gui_button_cb_insertAtEnd, me, 0);
	my undo = GuiButton_createShown (my dialog, 280, 470, 140+menuBarOffset, Gui_AUTOMATIC,
		L"Undo", gui_button_cb_undo, me, 0);
	my redo = GuiButton_createShown (my dialog, 280, 470, 180+menuBarOffset, Gui_AUTOMATIC,
		L"Redo", gui_button_cb_redo, me, 0);
	my remove = GuiButton_createShown (my dialog, 280, 470, 240+menuBarOffset, Gui_AUTOMATIC,
		L"Remove", gui_button_cb_remove, me, 0);	
	my moveUp = GuiButton_createShown (my dialog, 280, 470, 280+menuBarOffset, Gui_AUTOMATIC,
		L"Move selection up", gui_button_cb_moveUp, me, 0);	
	my moveDown = GuiButton_createShown (my dialog, 280, 470, 320+menuBarOffset, Gui_AUTOMATIC,
		L"Move selection down", gui_button_cb_moveDown, me, 0);	

	my outOfView = GuiLabel_createShown (my dialog, 5, 205, 450, Gui_AUTOMATIC, L"", 0);
}

static void dataChanged (I)
{
	iam (CategoriesEditor);
	update (me, 0, 0, NULL, 0);
	updateWidgets (me);
}

class_methods (CategoriesEditor, Editor) {
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createHelpMenuItems)
	class_methods_end
}

Any CategoriesEditor_create (Widget parent, wchar_t *title, Any data)
{
	CategoriesEditor me = new (CategoriesEditor);
	if (me && Editor_init (me, parent, 20, 40, 600, 600, title, data) &&
		(my history = CommandHistory_create (100)))
	{
		update (me, 0, 0, NULL, 0); 
		updateWidgets (me);
	}
	else forget (me);
	return me;
}

/*
	19980225 djmw Inserting items resulted in a memory leak.
		Cause: wrong inheritance for CategoriesEditorInsert command.
*/

/* End of file CategoriesEditor.c */
