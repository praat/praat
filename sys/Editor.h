#ifndef _Editor_h_
#define _Editor_h_
/* Editor.h
 *
 * Copyright (C) 1992-2012,2013 Paul Boersma
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
#include "prefs.h"

#include "Editor_enums.h"

Thing_declare (Editor);

Thing_define (EditorMenu, Thing) {
	// new data:
	public:
		Editor d_editor;
		const wchar_t *menuTitle;
		GuiMenu menuWidget;
		Ordered commands;
	// overridden methods:
		virtual void v_destroy ();
};

Thing_define (EditorCommand, Thing) {
	// new data:
	public:
		Editor d_editor;
		EditorMenu menu;
		const wchar_t *itemTitle;
		GuiMenuItem itemWidget;
		void (*commandCallback) (Editor editor_me, EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter);
		const wchar_t *script;
		UiForm d_uiform;
	// overridden methods:
		virtual void v_destroy ();
};

Thing_define (Editor, Thing) {

	// new data:
	public:
		GuiWindow d_windowForm;
		GuiMenuItem undoButton, searchButton;
		Ordered menus;
		Data data, previousData;   // the data that can be displayed and edited
		bool d_ownData;
		wchar_t undoText [100];
		Graphics pictureGraphics;
		void (*d_dataChangedCallback) (Editor me, void *closure);                   void *d_dataChangedClosure;
		void (*d_destructionCallback) (Editor me, void *closure);                   void *d_destructionClosure;
		void (*d_publicationCallback) (Editor me, void *closure, Data publication); void *d_publicationClosure;

	// new messages:
	public:

		void raise ()
			/*
			 * Message: "move your window to the front", i.e.
			 *    if you are invisible, then make your window visible at the front;
			 *    if you are iconized, then deiconize yourself at the front;
			 *    if you are already visible, just move your window to the front."
			 */
			{
				d_windowForm -> f_show ();
			}

		void dataChanged ()
			/*
			 * Message: "your data has changed by an action from *outside* yourself,
			 *    so you may e.g. like to redraw yourself."
			 */
			{
				v_dataChanged ();
			}

		void setDataChangedCallback (void (*dataChangedCallback) (Editor me, void *closure), void *dataChangedClosure)
			/*
			 * Message from boss: "notify me by calling this dataChangedCallback every time your data is changed from *inside* yourself."
			 *
			 * In Praat, the dataChangedCallback is useful if there is more than one editor
			 * with the same data; in this case, the owner of all those editors will
			 * (in the dataChangedCallback it installed) notify the change to the other editors
			 * by sending them a dataChanged () message.
			 */
			{
				d_dataChangedCallback = dataChangedCallback;
				d_dataChangedClosure = dataChangedClosure;
			}

		void broadcastDataChanged ()
			/*
			 * Message to boss: "my data has changed by an action from inside myself."
			 *
			 * The editor has to call this after every menu command, click or key press that causes a change in the data.
			 */
			{
				if (d_dataChangedCallback)
					d_dataChangedCallback (this, d_dataChangedClosure);
			}

		void setDestructionCallback (void (*destructionCallback) (Editor me, void *closure), void *destructionClosure)
			/*
			 * Message from boss: "notify me by calling this destructionCallback every time you destroy yourself."
			 *
			 * In Praat, "destroying yourself" typically happens when the user closes the editor window
			 * or when an object that is being viewed in an editor window is "Remove"d.
			 * Typically, the boss will (in the destructionCallback it installed) remove all dangling references to this editor.
			 */
			{
				d_destructionCallback = destructionCallback;
				d_destructionClosure = destructionClosure;
			}

		void broadcastDestruction ()
			/*
			 * Message to boss: "I am destroying all my members and will free myself shortly."
			 *
			 * The editor calls this once, namely in Editor::v_destroy().
			 */
			{
				if (d_destructionCallback)
					d_destructionCallback (this, d_destructionClosure);
			}

		void setPublicationCallback (void (*publicationCallback) (Editor me, void *closure, Data publication), void *publicationClosure)
			/*
			 * Message from boss: "notify me by calling this publicationCallback every time you have a piece of data to publish."
			 *
			 * In Praat, editors typically "publish" a piece of data when the user chooses
			 * things like "Extract selected sound", "Extract visible pitch curve", or "View spectral slice".
			 * Typically, the boss will (in the publicationCallback it installed) install the published data in Praat's object list,
			 * but the editor doesn't have to know that.
			 */
			{
				d_publicationCallback = publicationCallback;
				d_publicationClosure = publicationClosure;
			}

		void broadcastPublication (Data publication)
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
				if (d_publicationCallback)
					d_publicationCallback (this, d_publicationClosure, publication);
			}

	// overridden methods:
	protected:
		virtual void v_destroy ();
		virtual void v_info ();
		virtual void v_nameChanged ();   // sets the window and icon titles to reflect the new name
	// new methods:
	public:
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

GuiMenuItem EditorMenu_addCommand (EditorMenu me, const wchar_t *itemTitle, long flags,
	void (*commandCallback) (Editor me, EditorCommand, UiForm, int, Stackel, const wchar_t *, Interpreter));
GuiMenuItem EditorCommand_getItemWidget (EditorCommand me);

EditorMenu Editor_addMenu (Editor me, const wchar_t *menuTitle, long flags);
GuiObject EditorMenu_getMenuWidget (EditorMenu me);

#define Editor_HIDDEN  (1 << 14)
GuiMenuItem Editor_addCommand (Editor me, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	void (*commandCallback) (Editor me, EditorCommand cmd, UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter));
GuiMenuItem Editor_addCommandScript (Editor me, const wchar_t *menuTitle, const wchar_t *itemTitle, long flags,
	const wchar_t *script);
void Editor_setMenuSensitive (Editor me, const wchar_t *menu, int sensitive);

/***** For inheritors. *****/

void Editor_init (Editor me, int x, int y , int width, int height,
	const wchar_t *title, Data data);
/*
	This creates my shell and my d_windowForm,
	calls the v_createMenus and v_createChildren methods,
	and manages my shell and my d_windowForm.
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

void Editor_save (Editor me, const wchar_t *text);   // for Undo

UiForm UiForm_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle);
void UiForm_parseStringE (EditorCommand cmd, int narg, Stackel args, const wchar_t *arguments, Interpreter interpreter);
UiForm UiOutfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle);
UiForm UiInfile_createE (EditorCommand cmd, const wchar_t *title, const wchar_t *invokingButtonTitle, const wchar_t *helpTitle);

EditorCommand Editor_getMenuCommand (Editor me, const wchar_t *menuTitle, const wchar_t *itemTitle);
void Editor_doMenuCommand (Editor me, const wchar_t *command, int narg, Stackel args, const wchar_t *arguments, Interpreter interpreter);

/*
 * The following two procedures are in praat_picture.cpp.
 * They allow editors to draw into the Picture window.
 */
Graphics praat_picture_editor_open (bool eraseFirst);
void praat_picture_editor_close (void);
void Editor_openPraatPicture (Editor me);
void Editor_closePraatPicture (Editor me);

#endif
/* End of file Editor.h */
