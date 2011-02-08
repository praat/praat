#ifndef _Editor_h_
#define _Editor_h_
/* Editor.h
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2009/12/22
 */

#ifndef _Collection_h_
	#include "Collection.h"
#endif
#ifndef _Gui_h_
	#include "Gui.h"
#endif
#ifndef _Ui_h_
	#include "Ui.h"
#endif
#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

#include "Editor_enums.h"

#define Editor__parents(Klas) Thing_inherit (Klas, Thing)
Thing_declare1 (Editor);

#define EditorCommand_members Thing_members \
	Any editor, menu; \
	const wchar_t *itemTitle; \
	GuiObject itemWidget; \
	int (*commandCallback) (Any editor_me, EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter); \
	const wchar_t *script; \
	Any dialog;
#define EditorCommand_methods Thing_methods
class_create (EditorCommand, Thing);

typedef struct structEditorMenu *EditorMenu;

GuiObject EditorMenu_addCommand (EditorMenu menu, const wchar_t *itemTitle, long flags,
	int (*commandCallback) (Any editor_me, EditorCommand, UiForm, const wchar_t *, Interpreter));
GuiObject EditorCommand_getItemWidget (EditorCommand me);

EditorMenu Editor_addMenu (Any editor, const wchar_t *menuTitle, long flags);
GuiObject EditorMenu_getMenuWidget (EditorMenu me);

#define Editor__members(Klas) Thing_members \
	GuiObject parent, shell, dialog, menuBar, undoButton, searchButton; \
	Ordered menus; \
	Any data, previousData;   /* The data that can be displayed and edited. */ \
	wchar_t undoText [100]; \
	Graphics pictureGraphics; \
	void (*destroyCallback) (I, void *closure); \
	void *destroyClosure; \
	void (*dataChangedCallback) (I, void *closure, Any data); \
	void *dataChangedClosure; \
	void (*publishCallback) (I, void *closure, Any publish); \
	void *publishClosure; \
	void (*publish2Callback) (I, void *closure, Any publish1, Any publish2); \
	void *publish2Closure;
#define Editor__methods(Klas) Thing_methods \
	void (*goAway) (Klas me); \
	bool hasMenuBar, canFullScreen, editable, scriptable; \
	void (*createMenuItems_file) (Klas me, EditorMenu menu); \
	void (*createMenuItems_edit) (Klas me, EditorMenu menu); \
	void (*createMenuItems_query) (Klas me, EditorMenu menu); \
	void (*createMenuItems_query_info) (Klas me, EditorMenu menu); \
	void (*createMenus) (Klas me); \
	void (*createHelpMenuItems) (Klas me, EditorMenu menu); \
	void (*createChildren) (Klas me); \
	void (*dataChanged) (Klas me); \
	void (*save) (Klas me); \
	void (*restore) (Klas me); \
	void (*clipboardChanged) (Klas me, Any data); \
	void (*form_pictureWindow) (Klas me, EditorCommand cmd); \
	void (*ok_pictureWindow) (Klas me, EditorCommand cmd); \
	void (*do_pictureWindow) (Klas me, EditorCommand cmd); \
	void (*form_pictureMargins) (Klas me, EditorCommand cmd); \
	void (*ok_pictureMargins) (Klas me, EditorCommand cmd); \
	void (*do_pictureMargins) (Klas me, EditorCommand cmd);
Thing_declare2 (Editor, Thing);

#define Editor_HIDDEN  (1 << 14)
GuiObject Editor_addCommand (Any editor, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	int (*commandCallback) (Any editor_me, EditorCommand cmd, UiForm sendingForm, const wchar_t *sendingString, Interpreter interpreter));
GuiObject Editor_addCommandScript (Any editor, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	const wchar_t *script);
void Editor_setMenuSensitive (Any editor, const wchar_t *menu, int sensitive);

/***** Public. *****/

/* Thing_setName () sets the window and icon titles. */

void Editor_raise (Editor me);
	/* Raises and deiconizes the editor window. */

void Editor_dataChanged (Editor me, Any data);
/* Tell the Editor that the data has changed.
   If 'data' is not NULL, this routine installs 'data' into the Editor's 'data' field.
   Regardless of 'data', this routine calls the Editor's dataChanged method.
*/

void Editor_clipboardChanged (Editor me, Any data);
/* Tell the Editor that a clipboard has changed.
   Calls the Editor's clipboardChanged method.
*/

void Editor_setDestroyCallback (Editor me, void (*cb) (I, void *closure), void *closure);
/* Makes the Editor notify client when user clicks "Close":	*/
/* the Editor will destroy itself.				*/
/* Use this callback to remove your references to "me".		*/

void Editor_setDataChangedCallback (Editor me, void (*cb) (I, void *closure, Any data), void *closure);
/* Makes the Editor notify client (boss, creator, owner) when user changes data. */
/* 'data' is the new data (if not NULL). */
/* Most Editors will include the following line at several places, after 'data' or '*data' has changed:
	if (my dataChangedCallback)
		my dataChangedCallback (me, my dataChangedClosure, my data);
*/

void Editor_broadcastChange (Editor me);
/* A shortcut for the line above, with NULL for the 'data' argument, i.e. only '*data' has changed. */

void Editor_setPublishCallback (Editor me, void (*cb) (I, void *closure, Any publish), void *closure);
void Editor_setPublish2Callback (Editor me, void (*cb) (I, void *closure, Any publish1, Any publish2), void *closure);
/*
	Makes the Editor notify client when user clicks a "Publish" button:
	the Editor should create some new Data ("publish").
	By registering this callback, the client takes responsibility for eventually removing "publish".
*/


/***** For inheritors. *****/

int Editor_init (Editor me, GuiObject parent, int x, int y , int width, int height,
	const wchar_t *title, Any data);
/*
	This creates my shell and my dialog,
	calls the createMenus and createChildren methods,
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

void Editor_save (Editor me, const wchar_t *text);   /* For Undo. */

Any UiForm_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle);
int UiForm_parseStringE (EditorCommand cmd, const wchar_t *arguments, Interpreter interpreter);
Any UiOutfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle);

EditorCommand Editor_getMenuCommand (Editor me, const wchar_t *menuTitle, const wchar_t *itemTitle);
int Editor_doMenuCommand (Editor me, const wchar_t *command, const wchar_t *arguments, Interpreter interpreter);

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
