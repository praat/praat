/* DemoEditor.c
 *
 * Copyright (C) 2009 Paul Boersma
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
 * pb 2009/05/04 created
 * pb 2009/05/06 Demo_waitForInput ()
 */

#include "DemoEditor.h"
#include "machine.h"
#include "praatP.h"

#define DemoEditor__members(Klas) Editor__members(Klas) \
	Widget drawingArea; \
	Graphics graphics; \
	void *praatPicture; \
	bool clicked, keyPressed, shiftKeyPressed, commandKeyPressed, optionKeyPressed, extraControlKeyPressed; \
	long x, y; \
	wchar_t key; \
	bool waitingForInput, userWantsToClose; \
	Interpreter interpreter;
#define DemoEditor__methods(Klas) Editor__methods(Klas)
Thing_declare2 (DemoEditor, Editor);

static DemoEditor theDemoEditor;

/***** DemoEditor methods *****/

static void destroy (I) {
	iam (DemoEditor);
	Melder_free (my praatPicture);
	forget (my graphics);
	theDemoEditor = NULL;
	inherited (DemoEditor) destroy (me);
}

static void info (I) {
	iam (DemoEditor);
	inherited (DemoEditor) info (me);
	MelderInfo_writeLine2 (L"Colour: ", Melder_integer (((PraatPicture) my praatPicture) -> colour));
	MelderInfo_writeLine2 (L"Font: ", kGraphics_font_getText (((PraatPicture) my praatPicture) -> font));
	MelderInfo_writeLine2 (L"Font size: ", Melder_integer (((PraatPicture) my praatPicture) -> fontSize));
}

static void goAway (DemoEditor me) {
	if (my waitingForInput) {
		my userWantsToClose = true;
	} else {
		forget (me);
	}
}

static void createMenus (DemoEditor me) {
	inherited (DemoEditor) createMenus (DemoEditor_as_Editor (me));
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (DemoEditor);
	(void) event;
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	/*
	 * Erase the background. Don't record this erasure!
	 */
	Graphics_stopRecording (my graphics);   // the only place in Praat (the Picture window has a separate Graphics for erasing)?
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_startRecording (my graphics);
	Graphics_play (my graphics, my graphics);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (DemoEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	my clicked = true;
	my x = event -> x;
	my y = event -> y;
	my shiftKeyPressed = event -> shiftKeyPressed;
	my commandKeyPressed = event -> commandKeyPressed;
	my optionKeyPressed = event -> optionKeyPressed;
	my extraControlKeyPressed = event -> extraControlKeyPressed;
}

static void gui_drawingarea_cb_key (I, GuiDrawingAreaKeyEvent event) {
	iam (DemoEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	my keyPressed = true;
	my key = event -> key;
	my shiftKeyPressed = event -> shiftKeyPressed;
	my commandKeyPressed = event -> commandKeyPressed;
	my optionKeyPressed = event -> optionKeyPressed;
	my extraControlKeyPressed = event -> extraControlKeyPressed;
}

static void gui_drawingarea_cb_resize (I, GuiDrawingAreaResizeEvent event) {
	iam (DemoEditor);
	if (my graphics == NULL) return;   // Could be the case in the very beginning.
	Dimension marginWidth = 0, marginHeight = 0;
	Graphics_setWsViewport (my graphics, marginWidth, event -> width - marginWidth, marginHeight, event -> height - marginHeight);
	Graphics_setWsWindow (my graphics, 0, 100, 0, 100);
	Graphics_setViewport (my graphics, 0, 100, 0, 100);
	Graphics_updateWs (my graphics);
}

static void createChildren (DemoEditor me) {
	my drawingArea = GuiDrawingArea_createShown (my dialog, 0, 0, Machine_getMenuBarHeight (), 0,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, gui_drawingarea_cb_key, gui_drawingarea_cb_resize, me, 0);
	my graphics = Graphics_create_xmdrawingarea (my drawingArea);
	Graphics_setColour (my graphics, Graphics_WHITE);
	Graphics_setWindow (my graphics, 0, 1, 0, 1);
	Graphics_fillRectangle (my graphics, 0, 1, 0, 1);
	Graphics_setColour (my graphics, Graphics_BLACK);
	Graphics_startRecording (my graphics);
	//Graphics_setViewport (my graphics, 0, 100, 0, 100);
	//Graphics_setWindow (my graphics, 0, 100, 0, 100);
	//Graphics_line (my graphics, 0, 100, 100, 0);
}

static void clear (DemoEditor me) {
	(void) me;
}

class_methods (DemoEditor, Editor) {
	class_method (destroy)
	class_method (info)
	class_method (goAway)
	class_method (createChildren)
	class_method (createMenus)
	us -> scriptable = false;
	class_methods_end
}

int DemoEditor_init (DemoEditor me, Widget parent) {
	Editor_init (DemoEditor_as_parent (me), parent, 0, 0, 600, 400, NULL, NULL); cherror

struct structGuiDrawingAreaResizeEvent event = { my drawingArea, 0 };
event. width = GuiObject_getWidth (my drawingArea);
event. height = GuiObject_getHeight (my drawingArea);
gui_drawingarea_cb_resize (me, & event);

end:
	iferror return 0;
	return 1;
}

DemoEditor DemoEditor_create (Widget parent) {
	DemoEditor me = new (DemoEditor);
	if (! me || ! DemoEditor_init (me, parent)) { forget (me); return NULL; }
	return me;
}

void Demo_open (Interpreter interpreter) {
	#ifndef CONSOLE_APPLICATION
		if (Melder_batch) return;
		if (theDemoEditor == NULL) {
			theDemoEditor = DemoEditor_create (Melder_topShell);
			Melder_assert (theDemoEditor != NULL);
			theDemoEditor -> praatPicture = Melder_calloc (structPraatPicture, 1);
			theCurrentPraatPicture = theDemoEditor -> praatPicture;
			theCurrentPraatPicture -> graphics = theDemoEditor -> graphics;
			theCurrentPraatPicture -> font = kGraphics_font_HELVETICA;
			theCurrentPraatPicture -> fontSize = 10;
			theCurrentPraatPicture -> lineType = Graphics_DRAWN;
			theCurrentPraatPicture -> colour = Graphics_BLACK;
			theCurrentPraatPicture -> lineWidth = 1.0;
			theCurrentPraatPicture -> arrowSize = 1.0;
			theCurrentPraatPicture -> x1NDC = 0;
			theCurrentPraatPicture -> x2NDC = 100;
			theCurrentPraatPicture -> y1NDC = 0;
			theCurrentPraatPicture -> y2NDC = 100;
		}
		theDemoEditor -> interpreter = interpreter;
		theCurrentPraatPicture = theDemoEditor -> praatPicture;
	#endif
}

void Demo_close (void) {
	theCurrentPraatPicture = & theForegroundPraatPicture;
}

void Demo_show (void) {
	if (theDemoEditor == NULL) return;
	Demo_open (theDemoEditor -> interpreter);
	GuiWindow_show (theDemoEditor -> dialog);
	GuiWindow_drain (theDemoEditor -> shell);
	Demo_close ();
}

bool Demo_waitForInput (void) {
	if (theDemoEditor == NULL) return false;
	theDemoEditor -> clicked = false;
	theDemoEditor -> keyPressed = false;
	theDemoEditor -> waitingForInput = true;
	#ifndef CONSOLE_APPLICATION
		int wasBackgrounding = Melder_backgrounding;
		structMelderDir dir = { { 0 } };
		Melder_getDefaultDir (& dir);
		if (wasBackgrounding) praat_foreground ();
		do {
			XEvent event;
			XtAppNextEvent (Melder_appContext, & event);
			XtDispatchEvent (& event);
		} while (! theDemoEditor -> clicked && ! theDemoEditor -> keyPressed && ! theDemoEditor -> userWantsToClose);
		if (wasBackgrounding) praat_background ();
		Melder_setDefaultDir (& dir);
	#endif
	theDemoEditor -> waitingForInput = false;
	if (theDemoEditor -> userWantsToClose) {
		Interpreter_stop (theDemoEditor -> interpreter);
		forget (theDemoEditor);
		return false;
	}
	return true;
}

bool Demo_clicked (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> clicked;
}

double Demo_x (void) {
	if (theDemoEditor == NULL) return NUMundefined;
	Graphics_setInner (theDemoEditor -> graphics);
	double xWC, yWC;
	Graphics_DCtoWC (theDemoEditor -> graphics, theDemoEditor -> x, theDemoEditor -> y, & xWC, & yWC);
	Graphics_unsetInner (theDemoEditor -> graphics);
	return xWC;
}

double Demo_y (void) {
	if (theDemoEditor == NULL) return NUMundefined;
	Graphics_setInner (theDemoEditor -> graphics);
	double xWC, yWC;
	Graphics_DCtoWC (theDemoEditor -> graphics, theDemoEditor -> x, theDemoEditor -> y, & xWC, & yWC);
	Graphics_unsetInner (theDemoEditor -> graphics);
	return yWC;
}

bool Demo_keyPressed (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> keyPressed;
}

wchar_t Demo_key (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> key;
}

bool Demo_shiftKeyPressed (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> shiftKeyPressed;
}

bool Demo_commandKeyPressed (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> commandKeyPressed;
}

bool Demo_optionKeyPressed (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> optionKeyPressed;
}

bool Demo_extraControlKeyPressed (void) {
	if (theDemoEditor == NULL) return false;
	return theDemoEditor -> extraControlKeyPressed;
}

/* End of file DemoEditor.c */
