#ifndef _Editor_h_
#define _Editor_h_
/* Editor.h
 *
 * Copyright (C) 1992-2006 Paul Boersma
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
 * pb 2006/08/09
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

#define EditorCommand_members Thing_members \
	Any editor, menu; \
	const char *itemTitle; \
	Widget itemWidget; \
	int (*commandCallback) (EditorCommand cmd, Any sender); \
	const char *script; \
	Any dialog;
#define EditorCommand_methods Thing_methods
class_create (EditorCommand, Thing)

typedef struct structEditorMenu *EditorMenu;
typedef struct structEditor *Editor;

Widget EditorMenu_addCommand (EditorMenu menu, const char *itemTitle, long flags,
	int (*commandCallback) (EditorCommand, Any));
Widget EditorCommand_getItemWidget (EditorCommand me);

EditorMenu Editor_addMenu (Any editor, const char *menuTitle, long flags);
Widget EditorMenu_getMenuWidget (EditorMenu me);

#define Editor_members Thing_members \
	Widget parent, shell, dialog, menuBar, undoButton, searchButton; \
	Ordered menus; \
	Any data, previousData;   /* The data that can be displayed and edited. */ \
	char undoText [100]; \
	void (*destroyCallback) (I, void *closure); \
	void *destroyClosure; \
	void (*dataChangedCallback) (I, void *closure, Any data); \
	void *dataChangedClosure; \
	void (*publishCallback) (I, void *closure, Any publish); \
	void *publishClosure; \
	void (*publish2Callback) (I, void *closure, Any publish1, Any publish2); \
	void *publish2Closure;
#define Editor_methods Thing_methods \
	void (*goAway) (I); \
	int editable, scriptable; \
	void (*createMenus) (I); \
	void (*createChildren) (I); \
	void (*dataChanged) (I); \
	void (*save) (I); \
	void (*restore) (I); \
	void (*clipboardChanged) (I, Any data);
class_create_opaque (Editor, Thing)

#define Editor_HIDDEN  (1 << 11)
Widget Editor_addCommand (Any editor, const char *menuTitle, const char *itemTitle, long flags,
	int (*commandCallback) (EditorCommand cmd, Any sender));
Widget Editor_addCommandScript (Any editor, const char *menuTitle, const char *itemTitle, long flags,
	const char *script);
void Editor_setMenuSensitive (Any editor, const char *menu, int sensitive);

/***** Public. *****/

/* Thing_setName () sets the window and icon titles. */

void Editor_raise (I);
	/* Raises and deiconizes the editor window. */

void Editor_dataChanged (I, Any data);
/* Tell the Editor that the data has changed.
   If 'data' is not NULL, this routine installs 'data' into the Editor's 'data' field.
   Regardless of 'data', this routine calls the Editor's dataChanged method.
*/

void Editor_clipboardChanged (I, Any data);
/* Tell the Editor that a clipboard has changed.
   Calls the Editor's clipboardChanged method.
*/

void Editor_setDestroyCallback (I, void (*cb) (I, void *closure), void *closure);
/* Makes the Editor notify client when user clicks "Close":	*/
/* the Editor will destroy itself.				*/
/* Use this callback to remove your references to "me".		*/

void Editor_setDataChangedCallback (I, void (*cb) (I, void *closure, Any data), void *closure);
/* Makes the Editor notify client (boss, creator, owner) when user changes data. */
/* 'data' is the new data (if not NULL). */
/* Most Editors will include the following line at several places, after 'data' or '*data' has changed:
	if (my dataChangedCallback)
		my dataChangedCallback (me, my dataChangedClosure, my data);
*/

void Editor_broadcastChange (I);
/* A shortcut for the line above, with NULL for the 'data' argument, i.e. only '*data' has changed. */

void Editor_setPublishCallback (I, void (*cb) (I, void *closure, Any publish), void *closure);
void Editor_setPublish2Callback (I, void (*cb) (I, void *closure, Any publish1, Any publish2), void *closure);
/*
	Makes the Editor notify client when user clicks a "Publish" button:
	the Editor should create some new Data ("publish").
	By registering this callback, the client takes responsibility for eventually removing "publish".
*/


/***** For inheritors. *****/

int Editor_init (I, Widget parent, int x, int y , int width, int height,
	const char *title, Any data);
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

void Editor_save (I, const char *text);   /* For Undo. */

Any UiForm_createE (EditorCommand cmd, const char *title, const char *helpTitle);
int UiForm_parseStringE (EditorCommand cmd, const char *arguments);
Any UiOutfile_createE (EditorCommand cmd, const char *title, const char *helpTitle);

EditorCommand Editor_getMenuCommand (I, const char *menuTitle, const char *itemTitle);
int Editor_doMenuCommand (Any editor, const char *command, const char *arguments);
int Editor_scriptCallback (EditorCommand cmd, Any sender);

#endif
/* End of file Editor.h */
