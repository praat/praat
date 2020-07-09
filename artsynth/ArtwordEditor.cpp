/* ArtwordEditor.cpp
 *
 * Copyright (C) 1992-2013,2015-2020 Paul Boersma
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

#include "ArtwordEditor.h"
#include "machine.h"

Thing_implement (ArtwordEditor, Editor, 0);

void structArtwordEditor :: v_destroy () noexcept {
	ArtwordEditor_Parent :: v_destroy ();
}

static void updateList (ArtwordEditor me) {
	Artword artword = (Artword) my data;
	ArtwordData a = & artword -> data [(int) my muscle];
	GuiList_deleteAllItems (my list);
	for (int16 i = 1; i <= a -> numberOfTargets; i ++) {
		GuiList_insertItem (my list,
			Melder_cat (Melder_single (a -> times [i]), U"  ", Melder_single (a -> targets [i])),
			i);
	}
	Graphics_updateWs (my graphics.get());
}

static void gui_button_cb_removeTarget (ArtwordEditor me, GuiButtonEvent /* event */) {
	Artword artword = (Artword) my data;
	autoINTVEC selectedPositions = GuiList_getSelectedPositions (my list);
	for (integer ipos = selectedPositions.size; ipos > 0; ipos --) {
		integer position = selectedPositions [ipos];
		Melder_assert (position >= 1 && position <= INT16_MAX);
		Artword_removeTarget (artword, my muscle, (int16) position);   // guarded conversion
	}
	updateList (me);
	Editor_broadcastDataChanged (me);
}

static void gui_button_cb_addTarget (ArtwordEditor me, GuiButtonEvent /* event */) {
	Artword artword = (Artword) my data;
	autostring32 timeText = GuiText_getString (my time);
	double tim = Melder_atof (timeText.get());
	autostring32 valueText = GuiText_getString (my value);
	const double value = Melder_atof (valueText.get());
	ArtwordData a = & artword -> data [(int) my muscle];
	int i = 1, oldCount = a -> numberOfTargets;
	Artword_setTarget (artword, my muscle, tim, value);

	/* Optimization instead of "updateList (me)". */

	Melder_clip (0.0, & tim, artword -> totalTime);
	while (tim != a -> times [i]) {
		i ++;
		Melder_assert (i <= a -> numberOfTargets);   // can fail if tim is in an extended precision register
	}
	const conststring32 itemText = Melder_cat (Melder_single (tim), U"  ", Melder_single (value));
	if (a -> numberOfTargets == oldCount)
		GuiList_replaceItem (my list, itemText, i);
	else
		GuiList_insertItem (my list, itemText, i);
	Graphics_updateWs (my graphics.get());
	Editor_broadcastDataChanged (me);
}

static void gui_radiobutton_cb_toggle (ArtwordEditor me, GuiRadioButtonEvent event) {
	my muscle = (kArt_muscle) event -> position;
	Melder_assert ((int) my muscle > 0);
	Melder_assert (my muscle <= kArt_muscle::MAX);
	updateList (me);
}

static void gui_drawingarea_cb_expose (ArtwordEditor me, GuiDrawingArea_ExposeEvent /* event */) {
	if (! my graphics)
		return;
	Artword artword = (Artword) my data;
	Graphics_clearWs (my graphics.get());
	Artword_draw (artword, my graphics.get(), my muscle, true);
}

static void gui_drawingarea_cb_mouse (ArtwordEditor me, GuiDrawingArea_MouseEvent event) {
	if (! my graphics)
		return;
	Artword artword = (Artword) my data;
	Graphics_setWindow (my graphics.get(), 0, artword -> totalTime, -1.0, 1.0);
	Graphics_setInner (my graphics.get());
	double xWC, yWC;
	Graphics_DCtoWC (my graphics.get(), event -> x, event -> y, & xWC, & yWC);
	Graphics_unsetInner (my graphics.get());
	GuiText_setString (my time,  Melder_fixed (xWC, 6));
	GuiText_setString (my value, Melder_fixed (yWC, 6));
}

void structArtwordEditor :: v_dataChanged () {
	updateList (this);
	Graphics_updateWs (graphics.get());
}

void structArtwordEditor :: v_createChildren () {
	int dy = Machine_getMenuBarHeight ();
	GuiLabel_createShown (our windowForm, 40, 100, dy + 3, dy + 3 + Gui_LABEL_HEIGHT, U"Targets:", 0);
	GuiLabel_createShown (our windowForm, 5, 65, dy + 20, dy + 20 + Gui_LABEL_HEIGHT, U"Times:", 0);
	GuiLabel_createShown (our windowForm, 80, 140, dy + 20, dy + 20 + Gui_LABEL_HEIGHT, U"Values:", 0);
	list = GuiList_createShown (our windowForm, 0, 140, dy + 40, dy + 340, true, nullptr);

	GuiButton_createShown (our windowForm, 10, 130, dy + 410, dy + 410 + Gui_PUSHBUTTON_HEIGHT, U"Remove target", gui_button_cb_removeTarget, this, 0);

	drawingArea = GuiDrawingArea_createShown (our windowForm, 170, 470, dy + 10, dy + 310,
		gui_drawingarea_cb_expose, gui_drawingarea_cb_mouse,
		nullptr, nullptr, this, 0
	);

	GuiLabel_createShown (our windowForm, 220, 270, dy + 340, dy + 340 + Gui_LABEL_HEIGHT, U"Time:", 0);
	time = GuiText_createShown (our windowForm, 270, 370, dy + 340, dy + 340 + Gui_TEXTFIELD_HEIGHT, 0);

	GuiLabel_createShown (our windowForm, 220, 270, dy + 370, dy + 370 + Gui_LABEL_HEIGHT, U"Value:", 0);
	value = GuiText_createShown (our windowForm, 270, 370, dy + 370, dy + 370 + Gui_TEXTFIELD_HEIGHT, 0);

	GuiButton_createShown (our windowForm, 240, 360, dy + 410, dy + 410 + Gui_PUSHBUTTON_HEIGHT, U"Add target", gui_button_cb_addTarget, this, GuiButton_DEFAULT);

	dy = Machine_getMenuBarHeight ();
	GuiRadioGroup_begin ();
	for (int i = 1; i <= (int) kArt_muscle::MAX; i ++) {
		button [i] = GuiRadioButton_createShown (our windowForm,
			480, 0, dy, dy + Gui_RADIOBUTTON_HEIGHT,
			kArt_muscle_getText ((kArt_muscle) i), gui_radiobutton_cb_toggle, this, 0);
		dy += Gui_RADIOBUTTON_HEIGHT + Gui_RADIOBUTTON_SPACING - 2;
	}
	GuiRadioGroup_end ();
	muscle = (kArt_muscle) 1;
	GuiRadioButton_set (button [(int) muscle]);
}

autoArtwordEditor ArtwordEditor_create (conststring32 title, Artword data) {
	try {
		autoArtwordEditor me = Thing_new (ArtwordEditor);
		Editor_init (me.get(), 20, 40, 650, 600, title, data);
		//XtUnmanageChild (my menuBar);
		my graphics = Graphics_create_xmdrawingarea (my drawingArea);
		updateList (me.get());
		return me;
	} catch (MelderError) {
		Melder_throw (U"Artword window not created.");
	}
}

/* End of file ArtwordEditor.cpp */
