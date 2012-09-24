#ifndef _DemoEditor_h_
#define _DemoEditor_h_
/* DemoEditor.h
 *
 * Copyright (C) 2009-2011,2012 Paul Boersma
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

#include "Editor.h"

Thing_define (DemoEditor, Editor) {
	// new data:
	public:
		GuiDrawingArea drawingArea;
		Graphics graphics;
		void *praatPicture;
		bool clicked, keyPressed, shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed;
		long x, y;
		wchar_t key;
		bool waitingForInput, userWantsToClose, fullScreen;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_info ();
		virtual void v_goAway ();
		virtual bool v_hasMenuBar () { return false; }
		virtual bool v_canFullScreen () { return true; }
		virtual bool v_scriptable () { return false; }
		virtual void v_createChildren ();
		virtual void v_createMenus ();
};

void DemoEditor_init (DemoEditor me);
DemoEditor DemoEditor_create ();

void Demo_open (void);
void Demo_close (void);
struct autoDemoOpen {
	autoDemoOpen () { Demo_open (); }
	~autoDemoOpen () { Demo_close (); }
};

int Demo_windowTitle (const wchar_t *title);
int Demo_show (void);
void Demo_waitForInput (Interpreter interpreter);
bool Demo_clicked (void);
double Demo_x (void);
double Demo_y (void);
bool Demo_keyPressed (void);
wchar_t Demo_key (void);
bool Demo_shiftKeyPressed (void);
bool Demo_commandKeyPressed (void);
bool Demo_optionKeyPressed (void);
bool Demo_extraControlKeyPressed (void);
/* Shortcuts: */
bool Demo_input (const wchar_t *keys);
bool Demo_clickedIn (double left, double right, double bottom, double top);

/* End of file DemoEditor.h */
#endif
