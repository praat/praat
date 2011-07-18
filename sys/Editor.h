#ifndef _Editor_h_
#define _Editor_h_
/* Editor.h
 *
 * Copyright (C) 1992-2011 Paul Boersma
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

#include "Collection.h"
#include "Gui.h"
#include "Ui.h"
#include "Graphics.h"

#include "Editor_enums.h"

Thing_declare (Editor);
Thing_declare (EditorMenu);

Thing_define (EditorCommand, Thing) {
	Editor editor;
	EditorMenu menu;
	const wchar *itemTitle;
	GuiObject itemWidget;
	int (*commandCallback) (Editor editor_me, EditorCommand cmd, UiForm sendingForm, const wchar *sendingString, Interpreter interpreter);
	const wchar *script;
	Any dialog;
};

Thing_define (Editor, Thing) {
	GuiObject parent, shell, dialog, menuBar, undoButton, searchButton;
	Ordered menus;
	Data data, previousData;   // the data that can be displayed and edited
	wchar undoText [100];
	Graphics pictureGraphics;
	void (*destroyCallback) (I, void *closure);
	void *destroyClosure;
	void (*dataChangedCallback) (I, void *closure, Data data);
	void *dataChangedClosure;
	void (*publishCallback) (I, void *closure, Data publish);
	void *publishClosure;
	void (*publish2Callback) (I, void *closure, Data publish1, Data publish2);
	void *publish2Closure;
// overridden methods:
	void v_destroy ();
	void v_info ();
	void v_nameChanged ();
// new methods:
	virtual void v_goAway () { forget_cpp (this); }
	virtual bool v_hasMenuBar () { return true; }
	virtual bool v_canFullScreen () { return false; }
	virtual bool v_editable () { return true ; }
	virtual bool v_scriptable () { return true; }
	virtual void v_createMenuItems_file (EditorMenu menu);
	virtual void v_createMenuItems_edit (EditorMenu menu);
	virtual bool v_hasQueryMenu () { return true; }
	virtual void v_createMenuItems_query (EditorMenu menu);
	virtual void v_createMenuItems_query_info (EditorMenu menu);
	virtual void v_createMenus ();
	virtual void v_createHelpMenuItems (EditorMenu menu) { (void) menu; }
	virtual void v_createChildren () { }
	virtual void v_dataChanged () { }
	virtual void v_save ();
	virtual void v_restore ();
	virtual void v_clipboardChanged (Data data) { (void) data; }
	virtual void v_form_pictureWindow (EditorCommand cmd);
	virtual void v_ok_pictureWindow (EditorCommand cmd);
	virtual void v_do_pictureWindow (EditorCommand cmd);
	virtual void v_form_pictureMargins (EditorCommand cmd);
	virtual void v_ok_pictureMargins (EditorCommand cmd);
	virtual void v_do_pictureMargins (EditorCommand cmd);
};

GuiObject EditorMenu_addCommand (EditorMenu menu, const wchar *itemTitle, long flags,
	int (*commandCallback) (Editor editor_me, EditorCommand, UiForm, const wchar *, Interpreter));
GuiObject EditorCommand_getItemWidget (EditorCommand me);

EditorMenu Editor_addMenu (Editor editor, const wchar *menuTitle, long flags);
GuiObject EditorMenu_getMenuWidget (EditorMenu me);

#define Editor_HIDDEN  (1 << 14)
GuiObject Editor_addCommand (Editor editor, const wchar *menuTitle, const wchar *itemTitle, long flags,
	int (*commandCallback) (Editor editor_me, EditorCommand cmd, UiForm sendingForm, const wchar *sendingString, Interpreter interpreter));
GuiObject Editor_addCommandScript (Editor editor, const wchar *menuTitle, const wchar *itemTitle, long flags,
	const wchar *script);
void Editor_setMenuSensitive (Any editor, const wchar_t *menu, int sensitive);

/***** Public. *****/

/* Thing_setName () sets the window and icon titles. */

void Editor_raise (Editor me);
	/* Raises and deiconizes the editor window. */

void Editor_dataChanged (Editor me, Data data);
/* Tell the Editor that the data has changed.
   If 'data' is not NULL, this routine installs 'data' into the Editor's 'data' field.
   Regardless of 'data', this routine calls the Editor's dataChanged method.
*/

void Editor_clipboardChanged (Editor me, Data data);
/* Tell the Editor that a clipboard has changed.
   Calls the Editor's clipboardChanged method.
*/

void Editor_setDestroyCallback (Editor me, void (*cb) (I, void *closure), void *closure);
/* Makes the Editor notify client when user clicks "Close":	*/
/* the Editor will destroy itself.				*/
/* Use this callback to remove your references to "me".		*/

void Editor_setDataChangedCallback (Editor me, void (*cb) (I, void *closure, Data data), void *closure);
/* Makes the Editor notify client (boss, creator, owner) when user changes data. */
/* 'data' is the new data (if not NULL). */
/* Most Editors will include the following line at several places, after 'data' or '*data' has changed:
	if (my dataChangedCallback)
		my dataChangedCallback (me, my dataChangedClosure, my data);
*/

void Editor_broadcastChange (Editor me);
/* A shortcut for the line above, with NULL for the 'data' argument, i.e. only '*data' has changed. */

void Editor_setPublishCallback (Editor me, void (*cb) (I, void *closure, Data publish), void *closure);
void Editor_setPublish2Callback (Editor me, void (*cb) (I, void *closure, Data publish1, Data publish2), void *closure);
/*
	Makes the Editor notify client when user clicks a "Publish" button:
	the Editor should create some new Data ("publish").
	By registering this callback, the client takes responsibility for eventually removing "publish".
*/


/***** For inheritors. *****/

void Editor_init (Editor me, GuiObject parent, int x, int y , int width, int height,
	const wchar *title, Data data);
/*
	This creates my shell and my dialog,
	calls the v_createMenus and v_createChildren methods,
	and manages my shell and my dialog.
	'width' and 'height' determine the dimensions of the editor:
	if 'width' < 0, the width of the screen is added to it;
	if 'height' < 0, the height of the screeen is added to it;
	if 'width' is 0, the width is based on the children;
	if 'height' is 0, the height is base on the children.
	'x' and 'y' determine the position of the editor:
	if 'x' > 0, 'x' is the distance to the left edge of the screen;
	if 'x' < 0, |'x'| is the diatnce to the right edge of the screen;
	if 'x' is 0, the editor is horizontally centred on the screen;
	if 'y' > 0, 'y' is the distance to the top of the screen;
	if 'y' < 0, |'y'| is the diatnce to the bottom of the screen;
	if 'y' is 0, the editor is vertically centred on the screen;
	This routine does not transfer ownership of 'data' to the Editor,
	and the Editor will not destroy 'data' when the Editor itself is destroyed;
	however, some Editors may change 'data' (and not only '*data'),
	in which case the original 'data' IS destroyed,
	so the creator will have to install the dataChangedCallback in order to be notified,
	and replace its now dangling pointers with the new one.
	To prevent synchronicity problems, the Editor should destroy the old 'data'
	immediately AFTER calling its dataChangedCallback.
	Most Editors, by the way, will not need to change 'data'; they only change '*data',
	but the dataChangedCallback may still be useful if there is more than one editor
	with the same data; in this case, the owner of all those editors will
	(in the dataChangedCallback it installed) broadcast the change to the other editors
	by sending them an Editor_dataChanged () message.
*/

void Editor_save (Editor me, const wchar *text);   // for Undo

UiForm UiForm_createE (EditorCommand cmd, const wchar *title, const wchar *invokingButtonTitle, const wchar *helpTitle);
void UiForm_parseStringE (EditorCommand cmd, const wchar *arguments, Interpreter interpreter);
UiForm UiOutfile_createE (EditorCommand cmd, const wchar *title, const wchar *invokingButtonTitle, const wchar *helpTitle);

EditorCommand Editor_getMenuCommand (Editor me, const wchar *menuTitle, const wchar *itemTitle);
void Editor_doMenuCommand (Editor me, const wchar *command, const wchar *arguments, Interpreter interpreter);

/*
 * The following two procedures are in praat_picture.c.
 * They allow editors to draw into the Picture window.
 */
Graphics praat_picture_editor_open (bool eraseFirst);
void praat_picture_editor_close (void);
void Editor_openPraatPicture (Editor me);
void Editor_closePraatPicture (Editor me);

void Editor_prefs (void);

#endif
/* End of file Editor.h */
