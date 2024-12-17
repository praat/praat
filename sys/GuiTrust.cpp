/* GuiTrust.cpp
 *
 * Copyright (C) 2024 Paul Boersma
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

#include "GuiTrust.h"
#include "praatP.h"
#include "Gui.h"

GuiDialog GuiTrust_createDialog (GuiWindow optionalParent,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5
) {
	constexpr int DIALOG_WIDTH = 700;
	constexpr int BUTTON_HEIGHT = 60;
	/*
		Compute the height of the trust form.
	*/
	constexpr int SHORT_LABEL_HEIGHT = 25;
	constexpr int STEP_LABEL_HEIGHT = 15;
	int dialogHeight = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = 1 + !! str32chr (message, U'\n') + !! str32chr (message, U'“');
			const int labelHeight = SHORT_LABEL_HEIGHT + (numberOfLines - 1) * STEP_LABEL_HEIGHT;
			dialogHeight += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	dialogHeight += Gui_VERTICAL_DIALOG_SPACING_DIFFERENT;
	for (int i = 1; i <= 5; i ++) {
		conststring32 option = ( i == 1 ? option1 : i == 2 ? option2 : i == 3 ? option3 : i == 4 ? option4 : option5 );
		if (option)
			dialogHeight += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
	}
	dialogHeight += 60;   // the Help button
	/*
		Create the dialog.
	*/
	int x = Gui_LEFT_DIALOG_SPACING, buttonWidth = DIALOG_WIDTH - x - Gui_RIGHT_DIALOG_SPACING;
	GuiDialog me = GuiDialog_create (optionalParent, 150, 70, DIALOG_WIDTH, dialogHeight,
			U"Praat Trust window: checking for security and safety", nullptr, nullptr, GuiDialog_Modality::BLOCKING);
	/*
		Add the labels.
	*/
	int y = Gui_TOP_DIALOG_SPACING;
	for (int i = 1; i <= 5; i ++) {
		conststring32 message = ( i == 1 ? message1 : i == 2 ? message2 : i == 3 ? message3 : i == 4 ? message4 : message5 );
		if (message) {
			const int numberOfLines = 1 + !! str32chr (message, U'\n') + !! str32chr (message, U'“');
			const int labelHeight = SHORT_LABEL_HEIGHT + (numberOfLines - 1) * STEP_LABEL_HEIGHT;
			uint32 labelFlags = GuiLabel_CENTRE | GuiLabel_MULTILINE;
			if (str32chr (message, U'“'))
				labelFlags |= GuiLabel_BOLD;
			GuiLabel_createShown (me, x, x + buttonWidth, y, y + labelHeight, message, labelFlags);
			y += labelHeight + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	/*
		Add the buttons.
	*/
	y += Gui_VERTICAL_DIALOG_SPACING_DIFFERENT;
	for (int i = 1; i <= 5; i ++) {
		conststring32 option = ( i == 1 ? option1 : i == 2 ? option2 : i == 3 ? option3 : i == 4 ? option4 : option5 );
		if (option) {
			GuiButton_createShown (me, x, x + buttonWidth, y, y + BUTTON_HEIGHT,
					option, nullptr, nullptr, GuiButton_MULTILINE + ( i == 1 ? GuiButton_DEFAULT : 0 ));
			y += BUTTON_HEIGHT + Gui_VERTICAL_DIALOG_SPACING_SAME;
		}
	}
	return me;
}

integer GuiTrust_get (GuiWindow optionalParent, Editor optionalTrustWindowOwningEditor,
	conststring32 message1, conststring32 message2, conststring32 message3, conststring32 message4, conststring32 message5,
	conststring32 option1, conststring32 option2, conststring32 option3, conststring32 option4, conststring32 option5,
	Interpreter interpreter
) {
	Melder_assert (interpreter);
	GuiDialog me = GuiTrust_createDialog (optionalParent,
			message1, message2, message3, message4, message5, option1, option2, option3, option4, option5);
	GuiThing_show (me);
	const integer clickedButtonId = GuiDialog_run (me);
	GuiThing_hide (me);
	GuiObject_destroy (my d_widget);
	const bool sheClickedOnClose = ( clickedButtonId == 0 );
	const bool sheClickedOnCancel = ( clickedButtonId == 1 );
	if (sheClickedOnClose || sheClickedOnCancel) {
		Interpreter_stop (interpreter);
		Melder_throw (U"You interrupted the script.");
	}
	return clickedButtonId;
}

/* End of file GuiTrust.cpp */
