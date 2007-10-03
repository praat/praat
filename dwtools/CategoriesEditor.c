/* CategoriesEditor.c
 *
 * Copyright (C) 1993-2007 David Weenink
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
*/

#define CategoriesEditor_TEXTMAXLENGTH 100

#include "CategoriesEditor.h"
#include "Preferences.h"
#include "EditorM.h"


/* forward declarations */
static void cb_extended (Widget, XtPointer, XtPointer);
static void update (I, long from, long to, const long *select, long nSelect);
static void update_dos (I);

wchar_t *CategoriesEditor_EMPTYLABEL = L"(empty)";

DIRECT (CategoriesEditor, cb_help)
	(void) me;
	Melder_help (L"CategoriesEditor");
END

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

static int CategoriesEditorCommand_init (I, char *name,  Any data,
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
	if (! me || ! CategoriesEditorCommand_init (me, "Insert", data,
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

static Any CategoriesEditorRemove_create (Any data, int *posList, int posCount)
{
	CategoriesEditorRemove me = new (CategoriesEditorRemove); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, "Remove", data,
		CategoriesEditorRemove_execute, CategoriesEditorRemove_undo, 
		posCount, posCount)) forget (me);
	for (i = 1; i <= posCount; i++) my selection[i] = posList[i-1];
	return me;
}

class_methods (CategoriesEditorRemove, CategoriesEditorCommand)
class_methods_end

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

static Any CategoriesEditorReplace_create (Any data, Any str, int *posList, int posCount)
{
	CategoriesEditorReplace me = new (CategoriesEditorReplace);
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, "Replace", data,
		CategoriesEditorReplace_execute, CategoriesEditorReplace_undo, 
			posCount + 1, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i-1];
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

static Any CategoriesEditorMoveUp_create (Any data, int *posList, 
	int posCount, int newPos)
{
	CategoriesEditorMoveUp me = new (CategoriesEditorMoveUp); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, "Move up", data,
		CategoriesEditorMoveUp_execute, CategoriesEditorMoveUp_undo, 
		0, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i-1];
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

static Any CategoriesEditorMoveDown_create (Any data, int *posList, 
	int posCount, int newPos)
{
	CategoriesEditorMoveDown me = new (CategoriesEditorMoveDown); 
	long i;
	
	if (! me || ! CategoriesEditorCommand_init (me, "Move down", data,
		CategoriesEditorMoveDown_execute, CategoriesEditorMoveDown_undo, 
			0, posCount)) forget (me);
	for (i = 1; i <= posCount; i++)
	{
		my selection[i] = posList[i-1];
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
	int *posList, posCount; 
	char tmp[40] = "";
	XmString outOfViewLabel;
	
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		int i, outOfView = 0, bottom, topItemPosition;
		int visibleItemCount, itemCount;
		
		XtVaGetValues (my list, XmNtopItemPosition, & topItemPosition,
			XmNvisibleItemCount, & visibleItemCount, XmNitemCount, 
			& itemCount, NULL);
		bottom = topItemPosition + visibleItemCount - 1;
		for (i = posCount - 1; i >= 0; i--)
		{ 
			if (posList[i] < topItemPosition || posList[i] > bottom) outOfView++;
		}
		XtFree ((XtPointer) posList);
		if (outOfView > 0)
		{
			sprintf (tmp, "%d selection(s) out of view", outOfView);
		}
	}
	outOfViewLabel = XmStringCreateSimple (tmp);
	XtVaSetValues (my outOfView, XmNlabelString, outOfViewLabel, NULL);
	XmStringFree (outOfViewLabel);
}

static void update_dos (I)
{
	iam (CategoriesEditor);
	char tmp[50], *name;
	XmString commandName;
	Boolean undoSense = True, redoSense = True;
	
	/*
		undo
	*/
	
	if (! (name = CommandHistory_commandName (my history, 0)))
	{
			name = "nothing"; undoSense = False;
	}
	
	sprintf (tmp, "Undo `%.40s'", name);
	commandName = XmStringCreateSimple (tmp); 
	XtVaSetValues (my undo, XmNlabelString, commandName, NULL);
	XtSetSensitive (my undo, undoSense);
	
	/*
		redo
	*/
	
	if (! (name = CommandHistory_commandName (my history, 1)))
	{
		name = "nothing"; redoSense = False;
	}
	
	sprintf (tmp, "Redo `%.40s'", name);
	commandName = XmStringCreateSimple (tmp); 
	XtVaSetValues (my redo, XmNlabelString, commandName, NULL);
	XtSetSensitive (my redo, redoSense);
	XmStringFree (commandName);
}

static void updateWidgets (I) /*all buttons except undo & redo */
{
	iam (CategoriesEditor); 
	int *posList, posCount, size = ((Categories) my data)->size;
	Boolean insert = False, insertAtEnd = True, replace = False, remove = False;
	Boolean moveUp = False, moveDown = False;
	
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		int firstPos = posList[0], lastPos = posList[posCount-1];
		int contiguous = lastPos - firstPos + 1 == posCount;
		moveUp = contiguous && firstPos > 1;
		moveDown = contiguous && lastPos < size;
		my position = firstPos;	
		remove = True; replace = True; insertAtEnd = False;
		if (posCount == 1)
		{
			insert = True;
			if (posList[0] == size) insertAtEnd = True;
			if (size == 1 && ! wcscmp (CategoriesEditor_EMPTYLABEL,
				OrderedOfString_itemAtIndex_c (my data, 1))) remove = False; 
		}
		XtFree ((XtPointer) posList);
	}
	XtSetSensitive (my insert, insert); XtSetSensitive (my insertAtEnd, insertAtEnd);
	XtSetSensitive (my replace, replace); XtSetSensitive (my remove, remove);
	XtSetSensitive (my moveUp, moveUp); XtSetSensitive (my moveDown, moveDown);
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
		Begin optimalization: add the items from a table instead of separately.
	*/
	
	{
		XmString *table = NULL; 
		int k;
		
		if (! (table = Melder_malloc (XmString, to - from + 1))) return;
		XtVaGetValues (my list, XmNitemCount, & itemCount, NULL);
		for (k = 0, i = from; i <= to; i++)
		{
			wchar_t itemText[CategoriesEditor_TEXTMAXLENGTH+10]; 
			swprintf (itemText, CategoriesEditor_TEXTMAXLENGTH+10, L"%6d     %.*ls", i, CategoriesEditor_TEXTMAXLENGTH,
				OrderedOfString_itemAtIndex_c (my data, i));
			table[k++] = XmStringCreateSimple (Melder_peekWcsToUtf8 (itemText));
		}
		if (itemCount > size) /* some items have been removed from Categories? */
		{
			XmListDeleteItemsPos (my list, itemCount - size, size + 1);
			itemCount = size;
		}
		if (to > itemCount)
		{
			XmListAddItemsUnselected (my list, & table[itemCount - from + 1],
				to - itemCount, 0);
		}
		if (from <= itemCount)
		{
			XmListReplaceItemsPosUnselected (my list, table, 
				(to < itemCount ? to : itemCount) - from + 1, from);
		}
		for (k = 0, i = from; i <= to; i++)
		{
			XmStringFree (table[k++]);
		}
		Melder_free (table); 
	}
	
	/*
		End of optimalization
	*/
	
	/*
		HIGHLIGHT
	*/

	XmListDeselectAllItems (my list);
	if (size == 1) /* the only item is allways selected */
	{
		const wchar_t *catg = OrderedOfString_itemAtIndex_c (my data, 1);
		XmListSelectPos (my list, 1, True);
		GuiText_setStringW (my text, catg);
	}
	else if (nSelect > 0)
	{
		/*
			Trick required, or the other items will be deselected.
		*/
		
		#ifndef macintosh
			XtVaSetValues (my list, XmNselectionPolicy, XmMULTIPLE_SELECT, NULL);
		#endif
		/*
			Select but postpone highlighting
		*/
		for (i = 1; i <= nSelect; i++)
		{
			XmListSelectPos (my list, select[i] > size ? size : select[i], False);
		}
		#ifndef macintosh
			XtVaSetValues (my list, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
		#endif
	}
	
	/*
		VIEWPORT
	*/
	
	{
		int top, visible, itemCount;
		XtVaGetValues (my list, XmNtopItemPosition, & top,
			XmNvisibleItemCount, & visible, XmNitemCount, & itemCount, NULL);
		if (nSelect == 0)
		{
			top = my position - visible / 2;
		}
		else if (select[nSelect] < top)
		{
			/* selection above visible area*/
			top = select[1];
		}
		else if (select[1] >= top + visible - 1)
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
		XmListSetPos (my list, top);
	}
}

MOTIF_CALLBACK(cb_remove)
	iam (CategoriesEditor);
	int *posList, posCount;
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		CategoriesEditorRemove command = CategoriesEditorRemove_create 
			(me, posList, posCount);
		if (! command || ! Command_do (command))
		{
			forget (command); XtFree ((XtPointer) posList); return;
		}
		if (my history) CommandHistory_insertItem (my history, command);
		XtFree ((XtPointer) posList);	
		updateWidgets (me);
	}
MOTIF_CALLBACK_END

static void insert (I, int position)
{
	iam (CategoriesEditor);
	SimpleString str = NULL;
	CategoriesEditorInsert command = NULL;
	wchar_t *text = GuiText_getStringW (my text);
	
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

MOTIF_CALLBACK (cb_insert)
	iam (CategoriesEditor);
	insert (me, my position);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_insertAtEnd)
	iam (CategoriesEditor);
	Categories categories = my data;
	insert (me, categories->size + 1);
	my position = categories->size;
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_replace)
	iam (CategoriesEditor);
	int *posList, posCount;
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		CategoriesEditorReplace command = NULL;
		wchar_t *text = GuiText_getStringW (my text);
		SimpleString str = NULL;
		
		if (wcslen (text) == 0 || ! (str = SimpleString_create (text)) ||
			! (command = CategoriesEditorReplace_create (me, str, posList,
				posCount)) ||
			! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		XtFree ((XtPointer) posList);
		Melder_free (text);
		updateWidgets (me);
		return;
end:
		Melder_free (text);
		forget (str);
		forget (command);
	}
MOTIF_CALLBACK_END

/* Precondition: contiguous selection */
MOTIF_CALLBACK (cb_moveUp)
	iam (CategoriesEditor);
	int *posList, posCount;
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		CategoriesEditorMoveUp command = CategoriesEditorMoveUp_create 
			(me, posList, posCount, posList[0]-1);
		if (! command || ! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		XtFree ((XtPointer) posList);
		updateWidgets (me);
		return;
end:
		XtFree ((XtPointer) posList);
		forget (command);
	}
MOTIF_CALLBACK_END

/* Precondition: contiguous selection */
MOTIF_CALLBACK (cb_moveDown)
	iam (CategoriesEditor);
	int *posList, posCount;
	if (XmListGetSelectedPos (my list, & posList, & posCount))
	{
		CategoriesEditorMoveDown command = CategoriesEditorMoveDown_create 
			(me, posList, posCount, posList[posCount-1] + 1);
		if (! command || ! Command_do (command)) goto end;
		if (my history) CommandHistory_insertItem (my history, command);
		XtFree ((XtPointer) posList);
		updateWidgets (me);
		return;
end:
		XtFree ((XtPointer) posList);
		forget (command);
	}
MOTIF_CALLBACK_END


MOTIF_CALLBACK (cb_scroll)
	iam (CategoriesEditor);
	notifyOutOfView (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_default)
	iam (CategoriesEditor);
	const wchar_t *catg = OrderedOfString_itemAtIndex_c (my data, my position);
	GuiText_setStringW (my text, catg);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_extended)
	iam (CategoriesEditor);
	updateWidgets (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_undo)
	iam (CategoriesEditor);
	if (CommandHistory_offleft (my history)) return;
	Command_undo (CommandHistory_getItem (my history));
	CommandHistory_back (my history);
	updateWidgets (me);
MOTIF_CALLBACK_END

MOTIF_CALLBACK (cb_redo)
	iam (CategoriesEditor);
	CommandHistory_forth (my history);	
	if (CommandHistory_offright (my history)) return;
	Command_do (CommandHistory_getItem (my history));
	updateWidgets (me);
MOTIF_CALLBACK_END

static void destroy (I)
{
	iam (CategoriesEditor);
	forget (my history); /* !! Editor */
	inherited (CategoriesEditor) destroy (me);
}

static void createMenus (I)
{
	iam (CategoriesEditor);
	inherited (CategoriesEditor) createMenus (me);
	Editor_addCommand (me, L"Help", L"CategoriesEditor help", '?', cb_help);
}

static void createChildren (I)
{
	iam (CategoriesEditor);
	Widget scrolled, vertScrollBar;
	int menuBarOffset = 40;
	
	XtVaCreateManagedWidget ("Positions:", xmLabelGadgetClass, my dialog,
		XmNx, 5, XmNy, 3+menuBarOffset, XmNwidth, 95, NULL);
	XtVaCreateManagedWidget ("Values:", xmLabelGadgetClass, my dialog,
		XmNx, 100, XmNy, 3+menuBarOffset, XmNwidth, 90, NULL);
	
	scrolled = XmCreateScrolledWindow (my dialog, "listWindow", NULL, 0);
	XtVaSetValues (scrolled, XmNy, 40+menuBarOffset, XmNwidth, 260,
		#ifdef macintosh
			XmNheight, 30000,
		#else
			XmNheight, 100,
		#endif
		NULL);
	my list = XtVaCreateManagedWidget ("list", xmListWidgetClass, scrolled,
		 XmNvisibleItemCount, 20, XmNlistSizePolicy, XmCONSTANT,
		 XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
	XtAddCallback (my list, XmNextendedSelectionCallback, cb_extended, 
		(XtPointer) me);
	XtAddCallback (my list, XmNdefaultActionCallback, cb_default, (XtPointer) me);
	XtManageChild (scrolled);

	/*
		The valueChangedCallback does not get any notification in case of:
			drag, decrement, increment, pageIncrement & pageDecrement
	*/
	
	XtVaGetValues (scrolled, XmNverticalScrollBar, & vertScrollBar, NULL);
	XtAddCallback (vertScrollBar, XmNvalueChangedCallback, cb_scroll, 
		(XtPointer) me);
	XtAddCallback (vertScrollBar, XmNdragCallback, cb_scroll, (XtPointer) me);
	XtAddCallback (vertScrollBar, XmNdecrementCallback, cb_scroll, 
		(XtPointer) me);
	XtAddCallback (vertScrollBar, XmNincrementCallback, cb_scroll, 
		(XtPointer) me);
	XtAddCallback (vertScrollBar, XmNpageIncrementCallback, cb_scroll, 
		(XtPointer) me);
	XtAddCallback (vertScrollBar, XmNpageDecrementCallback, cb_scroll, 
		(XtPointer) me);
 			 
	XtVaCreateManagedWidget ("Value:", xmLabelGadgetClass, my dialog,
		XmNx, 280, XmNy, 3+menuBarOffset, XmNwidth, 90, NULL);
	my text = XtVaCreateManagedWidget("Text", xmTextWidgetClass, my dialog,
		 XmNx, 370, XmNy, 3+menuBarOffset, XmNwidth, 140, NULL);
	XmTextSetMaxLength (my text, CategoriesEditor_TEXTMAXLENGTH);
	GuiText_setStringW (my text, CategoriesEditor_EMPTYLABEL);
				 
	my insert = XtVaCreateManagedWidget ("Insert", xmPushButtonGadgetClass, 
		my dialog,  XmNx, 280, XmNy, 43+menuBarOffset, XmNwidth, 90, NULL);
	XtAddCallback (my insert, XmNactivateCallback, cb_insert, (XtPointer) me);
	
	my replace = XtVaCreateManagedWidget ("Replace", xmPushButtonGadgetClass,
		my dialog, XmNx, 380, XmNy, 43+menuBarOffset, XmNwidth, 90, NULL);
	XtAddCallback (my replace, XmNactivateCallback, cb_replace, (XtPointer) me);

	my insertAtEnd = XtVaCreateManagedWidget ("Insert at end",
		xmPushButtonGadgetClass, my dialog,
		XmNx, 280, XmNy, 83+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my insertAtEnd, XmNactivateCallback, cb_insertAtEnd,
		(XtPointer) me);

	my undo = XtVaCreateManagedWidget ("Undo", xmPushButtonGadgetClass, my dialog,
		XmNx, 280, XmNy, 140+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my undo, XmNactivateCallback, cb_undo, (XtPointer) me);

	my redo = XtVaCreateManagedWidget ("Redo", xmPushButtonGadgetClass, my dialog,
		XmNx, 280, XmNy, 180+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my redo, XmNactivateCallback, cb_redo, (XtPointer) me);

	my remove = XtVaCreateManagedWidget ("Remove", xmPushButtonGadgetClass, 
		my dialog, XmNx, 280, XmNy, 240+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my remove, XmNactivateCallback, cb_remove, (XtPointer) me);
		 	 
	XtVaSetValues(my dialog, XmNdefaultButton, my insert, NULL);
	
	my moveUp = XtVaCreateManagedWidget ("Move selection up",
		xmPushButtonGadgetClass, my dialog,
		XmNx, 280, XmNy, 280+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my moveUp, XmNactivateCallback, cb_moveUp, (XtPointer) me);
		
	my moveDown = XtVaCreateManagedWidget ("Move selection down",
		xmPushButtonGadgetClass, my dialog,
		XmNx, 280, XmNy, 320+menuBarOffset, XmNwidth, 190, NULL);
	XtAddCallback (my moveDown, XmNactivateCallback, cb_moveDown, (XtPointer) me);

	my outOfView = XtVaCreateManagedWidget ("", xmLabelGadgetClass, my dialog,
		XmNx, 5, XmNy, 450, XmNwidth, 200, NULL);
}

static void dataChanged (I)
{
	iam (CategoriesEditor);
	update (me, 0, 0, NULL, 0);
	updateWidgets (me);
}

class_methods (CategoriesEditor, Editor)
	class_method (destroy)
	class_method (dataChanged)
	class_method (createChildren)
	class_method (createMenus)
class_methods_end

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
