/* FunctionArea.cpp
 *
 * Copyright (C) 2022 Paul Boersma
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

#include "FunctionArea.h"
#include "EditorM.h"

Thing_implement (FunctionArea, Thing, 0);

#include "Prefs_define.h"
#include "FunctionArea_prefs.h"
#include "Prefs_install.h"
#include "FunctionArea_prefs.h"
#include "Prefs_copyToInstance.h"
#include "FunctionArea_prefs.h"

GuiMenuItem FunctionAreaMenu_addCommand (EditorMenu me, conststring32 itemTitle /* cattable */, uint32 flags,
		FunctionAreaCommandCallback commandCallback, FunctionArea commandBoss)
{
	return DataGuiMenu_addCommand (me, itemTitle, flags, commandCallback.get(), commandBoss);
}

static bool v_form_pictureSelection__drawSelectionTimes;
static bool v_form_pictureSelection__drawSelectionHairs;
void structFunctionArea :: v_form_pictureSelection (EditorCommand cmd) {
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection__drawSelectionTimes, nullptr, U"Draw selection times", true);
	UiForm_addBoolean (cmd -> d_uiform.get(), & v_form_pictureSelection__drawSelectionHairs, nullptr, U"Draw selection hairs", true);
}
void structFunctionArea :: v_ok_pictureSelection (EditorCommand cmd) {
	SET_BOOLEAN (v_form_pictureSelection__drawSelectionTimes, our classPref_picture_drawSelectionTimes())
	SET_BOOLEAN (v_form_pictureSelection__drawSelectionHairs, our classPref_picture_drawSelectionHairs())
}
void structFunctionArea :: v_do_pictureSelection (EditorCommand /* cmd */) {
	our setClassPref_picture_drawSelectionTimes (v_form_pictureSelection__drawSelectionTimes);
	our setClassPref_picture_drawSelectionHairs (v_form_pictureSelection__drawSelectionHairs);
}

void FunctionArea_drawRightMark (FunctionArea me, double yWC, conststring32 yWC_string, conststring32 units, int verticalAlignment) {
	static MelderString text;
	MelderString_copy (& text, yWC_string, units);
	double textWidth = Graphics_textWidth (my graphics(), text.string) + Graphics_dxMMtoWC (my graphics(), 0.5);
	if (verticalAlignment != Graphics_HALF)
		Graphics_line (my graphics(), my endWindow(), yWC, my endWindow() + textWidth, yWC);
	Graphics_setTextAlignment (my graphics(), Graphics_LEFT, verticalAlignment);
	if (verticalAlignment == Graphics_BOTTOM)
		yWC -= Graphics_dyMMtoWC (my graphics(), 0.5);
	Graphics_text (my graphics(), my endWindow(), yWC, text.string);
}

void FunctionArea_drawLegend_ (FunctionArea me,
	conststring32 cattableText1, MelderColour colour1,
	conststring32 cattableText2, MelderColour colour2,
	conststring32 cattableText3, MelderColour colour3,
	conststring32 cattableText4, MelderColour colour4
) {
	FunctionArea_setViewport (me);
	Graphics_setWindow (my graphics(), 0.0, 1.0, my bottom_pxlt(), my top_pxlt());
	Graphics_setColour (my graphics(), colour1);
	Graphics_setTextAlignment (my graphics(), kGraphics_horizontalAlignment::RIGHT, Graphics_BASELINE);
	double fontSize = Graphics_inqFontSize (my graphics()), oldFontSize = fontSize;
	Graphics_setFont (my graphics(), kGraphics_font::TIMES);
	//Graphics_setFontStyle (my graphics(), Graphics_ITALIC);
	if (! cattableText1) cattableText1 = U"";
	if (! cattableText2) cattableText2 = U"";
	if (! cattableText3) cattableText3 = U"";
	if (! cattableText4) cattableText4 = U"";
	double lengthText1 = Graphics_textWidth (my graphics(), cattableText1);
	double lengthText2 = Graphics_textWidth (my graphics(), cattableText2);
	double lengthText3 = Graphics_textWidth (my graphics(), cattableText3);
	double lengthText4 = Graphics_textWidth (my graphics(), cattableText4);
	conststring32 separator1 = ( cattableText1 [0] != U'\0' &&
			(cattableText2 [0] != U'\0' || cattableText3 [0] != U'\0' || cattableText4 [0] != U'\0') ? U"    " : U"" );
	conststring32 separator2 = ( cattableText2 [0] != U'\0' &&
			(cattableText3 [0] != U'\0' || cattableText4 [0] != U'\0') ? U"    " : U"" );
	conststring32 separator3 = ( cattableText3 [0] != U'\0' &&
			cattableText4 [0] != U'\0' ? U"    " : U"" );
	double lengthSep1 = Graphics_textWidth (my graphics(), separator1);
	double lengthSep2 = Graphics_textWidth (my graphics(), separator2);
	double lengthSep3 = Graphics_textWidth (my graphics(), separator3);
	double totalLength = lengthText1 + lengthSep1 + lengthText2 + lengthSep2 +
			lengthText3 + lengthSep3 + lengthText4;
	if (totalLength > 1.0) {
		fontSize = floor (fontSize / totalLength);
		Graphics_setFontSize (my graphics(), fontSize);
		lengthText1 = Graphics_textWidth (my graphics(), cattableText1);
		lengthText2 = Graphics_textWidth (my graphics(), cattableText2);
		lengthText3 = Graphics_textWidth (my graphics(), cattableText3);
		lengthText4 = Graphics_textWidth (my graphics(), cattableText4);
		lengthSep1 = Graphics_textWidth (my graphics(), separator1);
		lengthSep2 = Graphics_textWidth (my graphics(), separator2);
		lengthSep3 = Graphics_textWidth (my graphics(), separator3);
	}
	totalLength = lengthText1 + lengthSep1 + lengthText2 + lengthSep2 +
			lengthText3 + lengthSep3 + lengthText4;
	if (totalLength > 1.0) {
		fontSize -= 1.0;
		Graphics_setFontSize (my graphics(), fontSize);
		lengthText1 = Graphics_textWidth (my graphics(), cattableText1);
		lengthText2 = Graphics_textWidth (my graphics(), cattableText2);
		lengthText3 = Graphics_textWidth (my graphics(), cattableText3);
		lengthText4 = Graphics_textWidth (my graphics(), cattableText4);
		lengthSep1 = Graphics_textWidth (my graphics(), separator1);
		lengthSep2 = Graphics_textWidth (my graphics(), separator2);
		lengthSep3 = Graphics_textWidth (my graphics(), separator3);
	}
	Graphics_setColour (my graphics(), colour1);
	const double y = my top_pxlt() + 2;
	Graphics_text (my graphics(), 1.0 - lengthText4 - lengthSep3 - lengthText3 - lengthSep2 - lengthText2 - lengthSep1,
			y, cattableText1);
	Graphics_setColour (my graphics(), colour2);
	Graphics_text (my graphics(), 1.0 - lengthText4 - lengthSep3 - lengthText3 - lengthSep2,
			y, cattableText2);
	Graphics_setColour (my graphics(), colour3);
	Graphics_text (my graphics(), 1.0 - lengthText4 - lengthSep3,
			y, cattableText3);
	Graphics_setColour (my graphics(), colour4);
	Graphics_text (my graphics(), 1.0,
			y, cattableText4);
	Graphics_setColour (my graphics(), Melder_BLACK);
	Graphics_text (my graphics(), 1.0 - lengthText4 - lengthSep3 - lengthText3 - lengthSep2 - lengthText2,
			y, separator1);
	Graphics_text (my graphics(), 1.0 - lengthText4 - lengthSep3 - lengthText3,
			y, separator2);
	Graphics_text (my graphics(), 1.0 - lengthText4,
			y, separator3);
	Graphics_setFont (my graphics(), kGraphics_font::HELVETICA);
	Graphics_setFontStyle (my graphics(), Graphics_NORMAL);
	Graphics_setFontSize (my graphics(), oldFontSize);
}

void FunctionArea_garnishPicture (FunctionArea me) {
	if (my classPref_picture_drawSelectionTimes()) {
		if (my startSelection() >= my startWindow() && my startSelection() <= my endWindow())
			Graphics_markTop (my pictureGraphics(), my startSelection(), true, true, false, nullptr);
		if (my endSelection() != my startSelection() && my endSelection() >= my startWindow() && my endSelection() <= my endWindow())
			Graphics_markTop (my pictureGraphics(), my endSelection(), true, true, false, nullptr);
	}
	if (my classPref_picture_drawSelectionHairs()) {
		if (my startSelection() >= my startWindow() && my startSelection() <= my endWindow())
			Graphics_markTop (my pictureGraphics(), my startSelection(), false, false, true, nullptr);
		if (my endSelection() != my startSelection() && my endSelection() >= my startWindow() && my endSelection() <= my endWindow())
			Graphics_markTop (my pictureGraphics(), my endSelection(), false, false, true, nullptr);
	}
}

/* End of file FunctionArea.cpp */
