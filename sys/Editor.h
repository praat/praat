#ifndef _Editor_h_
#define _Editor_h_
/* Editor.h
 *
 * Copyright (C) 1992-2018 Paul Boersma
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

#include "Collection.h"
#include "Gui.h"
#include "Ui.h"
#include "Graphics.h"
#include "prefs.h"

#include "Editor_enums.h"

Thing_declare (Editor);

Thing_define (EditorMenu, Thing) {
	Editor d_editor;
	autostring32 menuTitle;
	GuiMenu menuWidget;
	OrderedOf<structEditorCommand> commands;
};

typedef MelderCallback <void, structEditor, EditorCommand, UiForm, integer /*narg*/, Stackel /*args*/, conststring32, Interpreter> EditorCommandCallback;

Thing_define (EditorCommand, Thing) {
	Editor d_editor;
	EditorMenu menu;
	autostring32 itemTitle;
	GuiMenuItem itemWidget;
	EditorCommandCallback commandCallback;
	autostring32 script;
	autoUiForm d_uiform;
};

typedef MelderCallback <void, structEditor> Editor_DataChangedCallback;
typedef MelderCallback <void, structEditor> Editor_DestructionCallback;

typedef MelderCallback <void, structEditor, autoDaata /* publication */> Editor_PublicationCallback;

Thing_define (Editor, Thing) {
	GuiWindow windowForm;
	GuiMenuItem undoButton, searchButton;
	OrderedOf<structEditorMenu> menus;
	Daata data;   // the data that can be displayed and edited
	autoDaata previousData;   // the data that can be displayed and edited
	bool ownData;
	char32 undoText [100];
	Graphics pictureGraphics;
	Editor_DataChangedCallback d_dataChangedCallback;
	Editor_DestructionCallback d_destructionCallback;
	Editor_PublicationCallback d_publicationCallback;
	autostring32 callbackSocket;

	void v_destroy () noexcept
		override;
	void v_info ()
		override;
	void v_nameChanged ()
		override;   // sets the window and icon titles to reflect the new name

	virtual void v_goAway () { forget_nozero (this); }
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
	virtual void v_saveData ();
	virtual void v_restoreData ();
	virtual void v_form_pictureWindow (EditorCommand cmd);
	virtual void v_ok_pictureWindow (EditorCommand cmd);
	virtual void v_do_pictureWindow (EditorCommand cmd);
	virtual void v_form_pictureMargins (EditorCommand cmd);
	virtual void v_ok_pictureMargins (EditorCommand cmd);
	virtual void v_do_pictureMargins (EditorCommand cmd);

	#include "Editor_prefs.h"
};

GuiMenuItem EditorMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags, EditorCommandCallback commandCallback);
GuiMenuItem EditorCommand_getItemWidget (EditorCommand me);

EditorMenu Editor_addMenu (Editor me, conststring32 menuTitle, uint32 flags);
GuiObject EditorMenu_getMenuWidget (EditorMenu me);

#define Editor_HIDDEN  (1 << 14)
GuiMenuItem Editor_addCommand (Editor me, conststring32 menuTitle, conststring32 itemTitle, uint32 flags, EditorCommandCallback commandCallback);
GuiMenuItem Editor_addCommandScript (Editor me, conststring32 menuTitle, conststring32 itemTitle, uint32 flags,
	conststring32 script);
void Editor_setMenuSensitive (Editor me, conststring32 menu, bool sensitive);

inline void Editor_raise (Editor me)
	/*
	 * Message: "move your window to the front", i.e.
	 *    if you are invisible, then make your window visible at the front;
	 *    if you are iconized, then deiconize yourself at the front;
	 *    if you are already visible, just move your window to the front."
	 */
	{
		GuiThing_show (my windowForm);
	}
inline void Editor_dataChanged (Editor me)
	/*
	 * Message: "your data has changed by an action from *outside* yourself,
	 *    so you may e.g. like to redraw yourself."
	 */
	{
		my v_dataChanged ();
	}
inline void Editor_setDataChangedCallback (Editor me, Editor_DataChangedCallback dataChangedCallback)
	/*
	 * Message from boss: "notify me by calling this dataChangedCallback every time your data is changed from *inside* yourself."
	 *
	 * In Praat, the dataChangedCallback is useful if there is more than one editor
	 * with the same data; in this case, the owner of all those editors will
	 * (in the dataChangedCallback it installed) notify the change to the other editors
	 * by sending them a dataChanged () message.
	 */
	{
		my d_dataChangedCallback = dataChangedCallback;
	}
inline void Editor_broadcastDataChanged (Editor me)
	/*
	 * Message to boss: "my data has changed by an action from inside myself."
	 *
	 * The editor has to call this after every menu command, click or key press that causes a change in the data.
	 */
	{
		if (my d_dataChangedCallback)
			my d_dataChangedCallback (me);
	}
inline void Editor_setDestructionCallback (Editor me, Editor_DestructionCallback destructionCallback)
	/*
	 * Message from observer: "notify me by calling this destructionCallback every time you destroy yourself."
	 *
	 * In Praat, "destroying yourself" typically happens when the user closes the editor window
	 * or when an object that is being viewed in an editor window is "Remove"d.
	 * Typically, the observer will (in the destructionCallback it installed) remove all dangling references to this editor.
	 */
	{
		my d_destructionCallback = destructionCallback;
	}
inline void Editor_broadcastDestruction (Editor me)
	/*
	 * Message to boss: "I am destroying all my members and will free myself shortly."
	 *
	 * The editor calls this once, namely in Editor::v_destroy().
	 */
	{
		if (my d_destructionCallback)
			my d_destructionCallback (me);
	}
inline void Editor_setPublicationCallback (Editor me, Editor_PublicationCallback publicationCallback)
	/*
	 * Message from boss: "notify me by calling this publicationCallback every time you have a piece of data to publish."
	 *
	 * In Praat, editors typically "publish" a piece of data when the user chooses
	 * things like "Extract selected sound", "Extract visible pitch curve", or "View spectral slice".
	 * Typically, the boss will (in the publicationCallback it installed) install the published data in Praat's object list,
	 * but the editor doesn't have to know that.
	 */
	{
		my d_publicationCallback = publicationCallback;
	}
inline void Editor_broadcastPublication (Editor me, autoDaata publication)
	/*
	 * Message to boss: "I have a piece of data for you to publish."
	 *
	 * The editor has to call this every time the user wants to "publish" ("Extract") some data from the editor.
	 *
	 * Constraint: "publication" has to be new data, either "extracted" or "copied" from data in the editor.
	 * The boss becomes the owner of this published data,
	 * so the call to broadcastPublication() has to transfer ownership of "publication".
	 */
	{
		if (my d_publicationCallback)
			my d_publicationCallback (me, publication.move());
	}

/***** For inheritors. *****/

void Editor_init (Editor me, int x, int y , int width, int height,
	conststring32 title, Daata data);
/*
	This creates my shell and my windowForm,
	calls the v_createMenus and v_createChildren methods,
	and manages my shell and my windowForm.
	'width' and 'height' determine the dimensions of the editor:
	if 'width' < 0, the width of the screen is added to it;
	if 'height' < 0, the height of the screeen is added to it;
	if 'width' is 0, the width is based on the children;
	if 'height' is 0, the height is base on the children.
	'x' and 'y' determine the position of the editor:
	if 'x' > 0, 'x' is the distance to the left edge of the screen;
	if 'x' < 0, |'x'| is the distance to the right edge of the screen;
	if 'x' is 0, the editor is horizontally centred on the screen;
	if 'y' > 0, 'y' is the distance to the top of the screen;
	if 'y' < 0, |'y'| is the distance to the bottom of the screen;
	if 'y' is 0, the editor is vertically centred on the screen;
	This routine does not transfer ownership of 'data' to the Editor,
	and the Editor will not destroy 'data' when the Editor itself is destroyed.
*/

void Editor_save (Editor me, conststring32 text);   // for Undo

autoUiForm UiForm_createE (EditorCommand cmd, conststring32 title, conststring32 invokingButtonTitle, conststring32 helpTitle);
void UiForm_parseStringE (EditorCommand cmd, integer narg, Stackel args, conststring32 arguments, Interpreter interpreter);
autoUiForm UiOutfile_createE (EditorCommand cmd, conststring32 title, conststring32 invokingButtonTitle, conststring32 helpTitle);
autoUiForm UiInfile_createE (EditorCommand cmd, conststring32 title, conststring32 invokingButtonTitle, conststring32 helpTitle);

EditorCommand Editor_getMenuCommand (Editor me, conststring32 menuTitle, conststring32 itemTitle);
void Editor_doMenuCommand (Editor me, conststring32 command, integer narg, Stackel args, conststring32 arguments, Interpreter interpreter);

/*
 * The following two procedures are in praat_picture.cpp.
 * They allow editors to draw into the Picture window.
 */
Graphics praat_picture_editor_open (bool eraseFirst);
void praat_picture_editor_close ();
void Editor_openPraatPicture (Editor me);
void Editor_closePraatPicture (Editor me);

#endif
/* End of file Editor.h */
