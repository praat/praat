/* ArtwordEditor.cpp
 *
 * Copyright (C) 1992-2011,2013 Paul Boersma
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

#include "ArtwordEditor.h"
#include "machine.h"

Thing_implement (ArtwordEditor, Editor, 0);

void structArtwordEditor :: v_destroy () {
	forget (graphics);
	ArtwordEditor_Parent :: v_destroy ();
}

static void updateList (ArtwordEditor me) {
	Artword artword = (Artword) my data;
	ArtwordData a = & artword -> data [my feature];
	my list -> f_deleteAllItems ();
	for (int i = 1; i <= a -> numberOfTargets; i ++) {
		static MelderString itemText = { 0 };
		MelderString_empty (& itemText);
		MelderString_append (& itemText, Melder_single (a -> times [i]), L"  ", Melder_single (a -> targets [i]));
		my list -> f_insertItem (itemText.string, i);
	}
	Graphics_updateWs (my graphics);
}

static void gui_button_cb_removeTarget (I, GuiButtonEvent event) {
	(void) event;
	iam (ArtwordEditor);
	Artword artword = (Artword) my data;
	long numberOfSelectedPositions, *selectedPositions = my list -> f_getSelectedPositions (& numberOfSelectedPositions);   // BUG memory
	if (selectedPositions != NULL) {
		for (long ipos = numberOfSelectedPositions; ipos > 0; ipos --)
			Artword_removeTarget (artword, my feature, selectedPositions [ipos]);
	}
	NUMvector_free <long> (selectedPositions, 1);
	updateList (me);
	my broadcastDataChanged ();
}

static void gui_button_cb_addTarget (I, GuiButtonEvent event) {
	(void) event;
	iam (ArtwordEditor);
	Artword artword = (Artword) my data;
	wchar_t *timeText = my time -> f_getString ();
	double tim = Melder_atof (timeText);
	wchar_t *valueText = my value -> f_getString ();
	double value = Melder_atof (valueText);
	ArtwordData a = & artword -> data [my feature];
	int i = 1, oldCount = a -> numberOfTargets;
	Melder_free (timeText);
	Melder_free (valueText);
	Artword_setTarget (artword, my feature, tim, value);

	/* Optimization instead of "updateList (me)". */

	if (tim < 0) tim = 0;
	if (tim > artword -> totalTime) tim = artword -> totalTime;
	while (tim != a -> times [i]) {
		i ++;
		Melder_assert (i <= a -> numberOfTargets);   // can fail if tim is in an extended precision register
	}
	static MelderString itemText = { 0 };
	MelderString_empty (& itemText);
	MelderString_append (& itemText, Melder_single (tim), L"  ", Melder_single (value));
	if (a -> numberOfTargets == oldCount) {
		my list -> f_replaceItem (itemText.string, i);
	} else {
		my list -> f_insertItem (itemText.string, i);
	}
	Graphics_updateWs (my graphics);
	my broadcastDataChanged ();
}

static void gui_radiobutton_cb_toggle (I, GuiRadioButtonEvent event) {
	iam (ArtwordEditor);
	my feature = event -> position;
	Melder_assert (my feature > 0);
	Melder_assert (my feature <= kArt_muscle_MAX);
	updateList (me);
}

static void gui_drawingarea_cb_expose (I, GuiDrawingAreaExposeEvent event) {
	iam (ArtwordEditor);
	(void) event;
	if (my graphics == NULL) return;
	Artword artword = (Artword) my data;
	Graphics_clearWs (my graphics);
	Artword_draw (artword, my graphics, my feature, TRUE);
}

static void gui_drawingarea_cb_click (I, GuiDrawingAreaClickEvent event) {
	iam (ArtwordEditor);
	if (my graphics == NULL) return;
	Artword artword = (Artword) my data;
	Graphics_setWindow (my graphics, 0, artword -> totalTime, -1.0, 1.0);
	Graphics_setInner (my graphics);
	double xWC, yWC;
	Graphics_DCtoWC (my graphics, event -> x, event -> y, & xWC, & yWC);
	Graphics_unsetInner (my graphics);
	my time  -> f_setString (Melder_fixed (xWC, 6));
	my value -> f_setString (Melder_fixed (yWC, 6));
}

void structArtwordEditor :: v_dataChanged () {
	updateList (this);
	Graphics_updateWs (graphics);
}

void structArtwordEditor :: v_createChildren () {
	int dy = Machine_getMenuBarHeight ();
	GuiLabel_createShown (d_windowForm, 40, 100, dy + 3, dy + 3 + Gui_LABEL_HEIGHT, L"Targets:", 0);
	GuiLabel_createShown (d_windowForm, 5, 65, dy + 20, dy + 20 + Gui_LABEL_HEIGHT, L"Times:", 0);
	GuiLabel_createShown (d_windowForm, 80, 140, dy + 20, dy + 20 + Gui_LABEL_HEIGHT, L"Values:", 0);
	list = GuiList_createShown (d_windowForm, 0, 140, dy + 40, dy + 340, true, NULL);

	GuiButton_createShown (d_windowForm, 10, 130, dy + 410, dy + 410 + Gui_PUSHBUTTON_HEIGHT, L"Remove target", gui_button_cb_removeTarget, this, 0);

	drawingArea = GuiDrawingArea_createShown (d_windowForm, 170, 470, dy + 10, dy + 310,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_click, NULL, NULL, this, 0);

	GuiLabel_createShown (d_windowForm, 220, 270, dy + 340, dy + 340 + Gui_LABEL_HEIGHT, L"Time:", 0);
	time = GuiText_createShown (d_windowForm, 270, 370, dy + 340, dy + 340 + Gui_TEXTFIELD_HEIGHT, 0);

	GuiLabel_createShown (d_windowForm, 220, 270, dy + 370, dy + 370 + Gui_LABEL_HEIGHT, L"Value:", 0);
	value = GuiText_createShown (d_windowForm, 270, 370, dy + 370, dy + 370 + Gui_TEXTFIELD_HEIGHT, 0);

	GuiButton_createShown (d_windowForm, 240, 360, dy + 410, dy + 410 + Gui_PUSHBUTTON_HEIGHT, L"Add target", gui_button_cb_addTarget, this, GuiButton_DEFAULT);

	dy = Machine_getMenuBarHeight ();
	GuiRadioGroup_begin ();
	for (int i = 1; i <= kArt_muscle_MAX; i ++) {
		button [i] = GuiRadioButton_createShown (d_windowForm,
			480, 0, dy, dy + Gui_RADIOBUTTON_HEIGHT,
			kArt_muscle_getText (i), gui_radiobutton_cb_toggle, this, 0);
		dy += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING - 2;
	}
	GuiRadioGroup_end ();
	feature = 1;
	button [feature] -> f_set ();
}

ArtwordEditor ArtwordEditor_create (const wchar_t *title, Artword data) {
	try {
		autoArtwordEditor me = Thing_new (ArtwordEditor);
		Editor_init (me.peek(), 20, 40, 650, 600, title, data);
		//XtUnmanageChild (my menuBar);
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		updateList (me.peek());
		return me.transfer();
	} catch (MelderError) {
		Melder_throw ("Artword window not created.");
	}
}

/* End of file ArtwordEditor.cpp */
