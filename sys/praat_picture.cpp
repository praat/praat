/* praat_picture.cpp
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

#include "praatP.h"
#include "Picture.h"
#include "Printer.h"
#include "machine.h"
#include "Formula.h"

#include "GuiP.h"

static bool praat_mouseSelectsInnerViewport;

void praat_picture_prefs (void) {
	Preferences_addEnum (L"Picture.font", & theCurrentPraatPicture -> font, kGraphics_font, kGraphics_font_DEFAULT);
	Preferences_addInt (L"Picture.fontSize", & theCurrentPraatPicture -> fontSize, 10);
	Preferences_addBool (L"Picture.mouseSelectsInnerViewport", & praat_mouseSelectsInnerViewport, false);
}

/***** static variable *****/

static Picture praat_picture;

/********** CALLBACKS OF THE PICTURE MENUS **********/

/***** "Font" MENU: font part *****/

static GuiMenuItem praatButton_fonts [1 + kGraphics_font_MAX];

static void updateFontMenu (void) {
	if (! theCurrentPraatApplication -> batch) {
		if (theCurrentPraatPicture -> font < kGraphics_font_MIN) theCurrentPraatPicture -> font = kGraphics_font_MIN;
		if (theCurrentPraatPicture -> font > kGraphics_font_MAX) theCurrentPraatPicture -> font = kGraphics_font_MAX;   // we no longer have New Century Schoolbook
		for (int i = kGraphics_font_MIN; i <= kGraphics_font_MAX; i ++) {
			praatButton_fonts [i] -> f_check (theCurrentPraatPicture -> font == i);
		}
	}
}
static void setFont (kGraphics_font font) {
	{
		autoPraatPicture picture;
		Graphics_setFont (GRAPHICS, font);
	}
	theCurrentPraatPicture -> font = font;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updateFontMenu ();
	}
}
DIRECT (Times) setFont (kGraphics_font_TIMES); END
DIRECT (Helvetica) setFont (kGraphics_font_HELVETICA); END
DIRECT (Palatino) setFont (kGraphics_font_PALATINO); END
DIRECT (Courier) setFont (kGraphics_font_COURIER); END

/***** "Font" MENU: size part *****/

static GuiMenuItem praatButton_10, praatButton_12, praatButton_14, praatButton_18, praatButton_24;
static void updateSizeMenu (void) {
	if (! theCurrentPraatApplication -> batch) {
		praatButton_10 -> f_check (theCurrentPraatPicture -> fontSize == 10);
		praatButton_12 -> f_check (theCurrentPraatPicture -> fontSize == 12);
		praatButton_14 -> f_check (theCurrentPraatPicture -> fontSize == 14);
		praatButton_18 -> f_check (theCurrentPraatPicture -> fontSize == 18);
		praatButton_24 -> f_check (theCurrentPraatPicture -> fontSize == 24);
	}
}
static void setFontSize (int fontSize) {
	//Melder_casual("Praat picture: set font size %d", fontSize);
	{
		autoPraatPicture picture;
		Graphics_setFontSize (GRAPHICS, fontSize);
	}
	theCurrentPraatPicture -> fontSize = fontSize;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updateSizeMenu ();
	}
}

DIRECT (10) setFontSize (10); END
DIRECT (12) setFontSize (12); END
DIRECT (14) setFontSize (14); END
DIRECT (18) setFontSize (18); END
DIRECT (24) setFontSize (24); END
FORM (Font_size, L"Praat picture: Font size", L"Font menu")
	NATURAL (L"Font size (points)", L"10")
OK
	SET_INTEGER (L"Font size", (long) theCurrentPraatPicture -> fontSize);
DO
	setFontSize (GET_INTEGER (L"Font size"));
END

/*static void setFontSize_keepInnerViewport (int fontSize) {
	double xmargin = praat_size * 4.2 / 72.0, ymargin = praat_size * 2.8 / 72.0;
	if (xmargin > 0.4 * (x2NDC - x1NDC)) xmargin = 0.4 * (x2NDC - x1NDC);
	if (ymargin > 0.4 * (y2NDC - y1NDC)) ymargin = 0.4 * (y2NDC - y1NDC);
	x1NDC += xmargin;
	x2NDC -= xmargin;
	y1NDC += ymargin;
	y2NDC -= ymargin;
	xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
	if (xmargin > 2 * (x2NDC - x1NDC)) xmargin = 2 * (x2NDC - x1NDC);
	if (ymargin > 2 * (y2NDC - y1NDC)) ymargin = 2 * (y2NDC - y1NDC);
	x1NDC -= xmargin;
	x2NDC += xmargin;
	y1NDC -= ymargin;
	y2NDC += ymargin;
	{
		autoPraatPicture picture;
		Graphics_setFontSize (GRAPHICS, praat_size = fontSize);
	}
	Picture_setSelection (praat_picture, x1NDC, x2NDC, y1NDC, y2NDC, False);
	updateSizeMenu ();
}*/

/***** "Select" MENU *****/

static GuiMenuItem praatButton_innerViewport, praatButton_outerViewport;
static void updateViewportMenu (void) {
	if (! theCurrentPraatApplication -> batch) {
		praatButton_innerViewport -> f_check (praat_mouseSelectsInnerViewport ? 1 : 0);
		praatButton_outerViewport -> f_check (praat_mouseSelectsInnerViewport ? 0 : 1);
	}
}

DIRECT (MouseSelectsInnerViewport)
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw ("Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture, praat_mouseSelectsInnerViewport = true);
	}
	updateViewportMenu ();
END

DIRECT (MouseSelectsOuterViewport)
	if (theCurrentPraatPicture != & theForegroundPraatPicture)
		Melder_throw ("Mouse commands are not available inside pictures.");
	{// scope
		autoPraatPicture picture;
		Picture_setMouseSelectsInnerViewport (praat_picture, praat_mouseSelectsInnerViewport = false);
	}
	updateViewportMenu ();
END

FORM (SelectInnerViewport, L"Praat picture: Select inner viewport", L"Select inner viewport...")
	LABEL (L"", L"The viewport is the selected rectangle in the Picture window.")
	LABEL (L"", L"It is where your next drawing will appear.")
	LABEL (L"", L"The rectangle you select here will not include the margins.")
	LABEL (L"", L"")
	REAL (L"left Horizontal range (inches)", L"0.0")
	REAL (L"right Horizontal range (inches)", L"6.0")
	REAL (L"left Vertical range (inches)", L"0.0")
	REAL (L"right Vertical range (inches)", L"6.0")
OK
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC)) ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
	if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC)) xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
	SET_REAL (L"left Horizontal range", theCurrentPraatPicture -> x1NDC + xmargin);
	SET_REAL (L"right Horizontal range", theCurrentPraatPicture -> x2NDC - xmargin);
	SET_REAL (L"left Vertical range", 12 - theCurrentPraatPicture -> y2NDC + ymargin);
	SET_REAL (L"right Vertical range", 12 - theCurrentPraatPicture -> y1NDC - ymargin);
DO
	//if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw ("Viewport commands are not available inside manuals.");
	double left = GET_REAL (L"left Horizontal range"), right = GET_REAL (L"right Horizontal range");
	double top = GET_REAL (L"left Vertical range"), bottom = GET_REAL (L"right Vertical range");
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		long x1DC, x2DC, y1DC, y2DC;
		Graphics_inqWsViewport (GRAPHICS, & x1DC, & x2DC, & y1DC, & y2DC);
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double wDC = (x2DC - x1DC) / (x2wNDC - x1wNDC);
		double hDC = abs (y2DC - y1DC) / (y2wNDC - y1wNDC);
		xmargin *= Graphics_getResolution (GRAPHICS) / wDC;
		ymargin *= Graphics_getResolution (GRAPHICS) / hDC;
	}
	if (xmargin > 2 * (right - left)) xmargin = 2 * (right - left);
	if (ymargin > 2 * (bottom - top)) ymargin = 2 * (bottom - top);
	if (left == right) {
		Melder_throw ("The left and right edges of the viewport cannot be equal.\nPlease change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_throw ("The top and bottom edges of the viewport cannot be equal.\nPlease change the vertical range.");
	}
	theCurrentPraatPicture -> x1NDC = left - xmargin;
	theCurrentPraatPicture -> x2NDC = right + xmargin;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = 12-bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = 12-top + ymargin;
		Picture_setSelection (praat_picture, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC, false);
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects) {   // in manual?
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double height_NDC = y2wNDC - y1wNDC;
		theCurrentPraatPicture -> y1NDC = height_NDC-bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = height_NDC-top + ymargin;
	} else {
		if (top < bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = bottom - ymargin;
		theCurrentPraatPicture -> y2NDC = top + ymargin;
	}
END

FORM (SelectOuterViewport, L"Praat picture: Select outer viewport", L"Select outer viewport...")
	LABEL (L"", L"The viewport is the selected rectangle in the Picture window.")
	LABEL (L"", L"It is where your next drawing will appear.")
	LABEL (L"", L"The rectangle you select here will include the margins.")
	LABEL (L"", L"")
	REAL (L"left Horizontal range (inches)", L"0.0")
	REAL (L"right Horizontal range (inches)", L"6.0")
	REAL (L"left Vertical range (inches)", L"0.0")
	REAL (L"right Vertical range (inches)", L"6.0")
OK
	SET_REAL (L"left Horizontal range", theCurrentPraatPicture -> x1NDC);
	SET_REAL (L"right Horizontal range", theCurrentPraatPicture -> x2NDC);
	SET_REAL (L"left Vertical range", 12 - theCurrentPraatPicture -> y2NDC);
	SET_REAL (L"right Vertical range", 12 - theCurrentPraatPicture -> y1NDC);
DO
	//if (theCurrentPraatObjects != & theForegroundPraatObjects) Melder_throw ("Viewport commands are not available inside manuals.");
	double left = GET_REAL (L"left Horizontal range"), right = GET_REAL (L"right Horizontal range");
	double top = GET_REAL (L"left Vertical range"), bottom = GET_REAL (L"right Vertical range");
	if (left == right) {
		Melder_throw ("The left and right edges of the viewport cannot be equal.\nPlease change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_throw ("The top and bottom edges of the viewport cannot be equal.\nPlease change the vertical range.");
	}
	theCurrentPraatPicture -> x1NDC = left;
	theCurrentPraatPicture -> x2NDC = right;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = 12-bottom;
		theCurrentPraatPicture -> y2NDC = 12-top;
		Picture_setSelection (praat_picture, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC, false);
	} else if (theCurrentPraatObjects != & theForegroundPraatObjects) {   // in manual?
		if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		double x1wNDC, x2wNDC, y1wNDC, y2wNDC;
		Graphics_inqWsWindow (GRAPHICS, & x1wNDC, & x2wNDC, & y1wNDC, & y2wNDC);
		double height_NDC = y2wNDC - y1wNDC;
		theCurrentPraatPicture -> y1NDC = height_NDC-bottom;
		theCurrentPraatPicture -> y2NDC = height_NDC-top;
	} else {
		if (top < bottom) { double temp; temp = top; top = bottom; bottom = temp; }
		theCurrentPraatPicture -> y1NDC = bottom;
		theCurrentPraatPicture -> y2NDC = top;
	}
END

FORM (ViewportText, L"Praat picture: Viewport text", L"Viewport text...")
	RADIO (L"Horizontal alignment", 2)
		RADIOBUTTON (L"Left")
		RADIOBUTTON (L"Centre")
		RADIOBUTTON (L"Right")
	RADIO (L"Vertical alignment", 2)
		RADIOBUTTON (L"Bottom")
		RADIOBUTTON (L"Half")
		RADIOBUTTON (L"Top")
	REAL (L"Rotation (degrees)", L"0")
	TEXTFIELD (L"text", L"")
OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	int hor = GET_INTEGER (L"Horizontal alignment") - 1;
	int vert = GET_INTEGER (L"Vertical alignment") - 1;
	autoPraatPicture picture;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, 0, 1, 0, 1);
	Graphics_setTextAlignment (GRAPHICS, hor, vert);
	Graphics_setTextRotation (GRAPHICS, GET_REAL (L"Rotation"));
	Graphics_text (GRAPHICS, hor == 0 ? 0 : hor == 1 ? 0.5 : 1,
		vert == 0 ? 0 : vert == 1 ? 0.5 : 1, GET_STRING (L"text"));
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
END

/***** "Pen" MENU *****/

static GuiMenuItem praatButton_lines [4];
static GuiMenuItem praatButton_black, praatButton_white, praatButton_red, praatButton_green, praatButton_blue,
	praatButton_yellow, praatButton_cyan, praatButton_magenta, praatButton_maroon, praatButton_lime, praatButton_navy,
	praatButton_teal, praatButton_purple, praatButton_olive, praatButton_pink, praatButton_silver, praatButton_grey;


static void updatePenMenu (void) {
	if (! theCurrentPraatApplication -> batch) {
		for (int i = Graphics_DRAWN; i <= Graphics_DASHED; i ++) {
			praatButton_lines [i] -> f_check (theCurrentPraatPicture -> lineType == i);
		}
		praatButton_black   -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_BLACK));
		praatButton_white   -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_WHITE));
		praatButton_red     -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_RED));
		praatButton_green   -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_GREEN));
		praatButton_blue    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_BLUE));
		praatButton_yellow  -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_YELLOW));
		praatButton_cyan    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_CYAN));
		praatButton_magenta -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_MAGENTA));
		praatButton_maroon  -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_MAROON));
		praatButton_lime    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_LIME));
		praatButton_navy    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_NAVY));
		praatButton_teal    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_TEAL));
		praatButton_purple  -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_PURPLE));
		praatButton_olive   -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_OLIVE));
		praatButton_pink    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_PINK));
		praatButton_silver  -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_SILVER));
		praatButton_grey    -> f_check (Graphics_Colour_equal (theCurrentPraatPicture -> colour, Graphics_GREY));
	}
}
static void setLineType (int lineType) {
	{
		autoPraatPicture picture;
		Graphics_setLineType (GRAPHICS, lineType);
	}
	theCurrentPraatPicture -> lineType = lineType;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
}
DIRECT (Solid_line) setLineType (Graphics_DRAWN); END
DIRECT (Dotted_line) setLineType (Graphics_DOTTED); END
DIRECT (Dashed_line) setLineType (Graphics_DASHED); END
DIRECT (Dashed_dotted_line) setLineType (Graphics_DASHED_DOTTED); END

FORM (Line_width, L"Praat picture: Line width", 0)
	POSITIVE (L"Line width", L"1.0")
OK
	SET_REAL (L"Line width", theCurrentPraatPicture -> lineWidth);
DO
	double lineWidth = GET_REAL (L"Line width");
	{
		autoPraatPicture picture;
		Graphics_setLineWidth (GRAPHICS, lineWidth);
	}
	theCurrentPraatPicture -> lineWidth = lineWidth;
END

FORM (Arrow_size, L"Praat picture: Arrow size", 0)
	POSITIVE (L"Arrow size", L"1.0")
OK
	SET_REAL (L"Arrow size", theCurrentPraatPicture -> arrowSize);
DO
	double arrowSize = GET_REAL (L"Arrow size");
	{
		autoPraatPicture picture;
		Graphics_setArrowSize (GRAPHICS, arrowSize);
	}
	theCurrentPraatPicture -> arrowSize = arrowSize;
END

static void setColour (Graphics_Colour colour) {
	{
		autoPraatPicture picture;
		Graphics_setColour (GRAPHICS, colour);
	}
	theCurrentPraatPicture -> colour = colour;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
}
DIRECT (Black) setColour (Graphics_BLACK); END
DIRECT (White) setColour (Graphics_WHITE); END
DIRECT (Red) setColour (Graphics_RED); END
DIRECT (Green) setColour (Graphics_GREEN); END
DIRECT (Blue) setColour (Graphics_BLUE); END
DIRECT (Yellow) setColour (Graphics_YELLOW); END
DIRECT (Cyan) setColour (Graphics_CYAN); END
DIRECT (Magenta) setColour (Graphics_MAGENTA); END
DIRECT (Maroon) setColour (Graphics_MAROON); END
DIRECT (Lime) setColour (Graphics_LIME); END
DIRECT (Navy) setColour (Graphics_NAVY); END
DIRECT (Teal) setColour (Graphics_TEAL); END
DIRECT (Purple) setColour (Graphics_PURPLE); END
DIRECT (Olive) setColour (Graphics_OLIVE); END
DIRECT (Pink) setColour (Graphics_PINK); END
DIRECT (Silver) setColour (Graphics_SILVER); END
DIRECT (Grey) setColour (Graphics_GREY); END

FORM (Colour, L"Praat picture: Colour", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.0")
OK
DO
	Graphics_Colour colour = GET_COLOUR (L"Colour");
	{
		autoPraatPicture picture;
		Graphics_setColour (GRAPHICS, colour);
	}
	theCurrentPraatPicture -> colour = colour;
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		updatePenMenu ();
	}
END

/***** "File" MENU *****/

FORM_READ (Picture_readFromPraatPictureFile, L"Read picture from praat picture file", 0, false)
	Picture_readFromPraatPictureFile (praat_picture, file);
END

static void DO_Picture_writeToEpsFile (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	static Any dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as EPS file",
		DO_Picture_writeToEpsFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, L"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture, file, TRUE, FALSE);
	}
}
/*FORM_WRITE (Picture_writeToEpsFile, L"Save picture as Encapsulated PostScript file", 0, L"praat.eps")
	if (! Picture_writeToEpsFile (praat_picture, fileName, TRUE)) return 0;
END*/

static void DO_Picture_writeToFontlessEpsFile_xipa (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	static Any dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as fontless EPS file",
		DO_Picture_writeToFontlessEpsFile_xipa, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, L"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture, file, FALSE, FALSE);
	}
}

static void DO_Picture_writeToFontlessEpsFile_silipa (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	static Any dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as fontless EPS file",
		DO_Picture_writeToFontlessEpsFile_silipa, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, L"praat.eps");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToEpsFile (praat_picture, file, FALSE, TRUE);
	}
}

static void DO_Picture_writeToPdfFile (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	static Any dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as PDF file",
		DO_Picture_writeToPdfFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, L"praat.pdf");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToPdfFile (praat_picture, file);
	}
}

static void DO_Picture_writeToPraatPictureFile (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
	static Any dia;
	(void) narg;
	(void) interpreter;
	(void) modified;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as Praat picture file",
		DO_Picture_writeToPraatPictureFile, NULL, invokingButtonTitle, NULL);
	if (sendingForm == NULL && args == NULL && sendingString == NULL) {
		UiOutfile_do (dia, L"praat.prapic");
	} else { MelderFile file; structMelderFile file2 = { 0 };
		if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
		else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
		Picture_writeToPraatPictureFile (praat_picture, file);
	}
}

#ifdef macintosh
DIRECT (Page_setup)
	Printer_pageSetup ();
END
#endif

DIRECT (PostScript_settings)
	Printer_postScriptSettings ();
END

DIRECT (Print)
	Picture_print (praat_picture);
END

#ifdef _WIN32
	static void DO_Picture_writeToWindowsMetafile (UiForm sendingForm, int narg, Stackel args, const wchar_t *sendingString, Interpreter interpreter, const wchar_t *invokingButtonTitle, bool modified, void *dummy) {
		static Any dia;
		(void) narg;
		(void) interpreter;
		(void) modified;
		(void) dummy;
		if (! dia) dia = UiOutfile_create (theCurrentPraatApplication -> topShell, L"Save as Windows metafile",
			DO_Picture_writeToWindowsMetafile, NULL, invokingButtonTitle, NULL);
		if (sendingForm == NULL && args == NULL && sendingString == NULL) {
			UiOutfile_do (dia, L"praat.emf");
		} else { MelderFile file; structMelderFile file2 = { 0 };
			if (args == NULL && sendingString == NULL) file = UiFile_getFile (dia);
			else { Melder_relativePathToFile (args ? args [1]. string : sendingString, & file2); file = & file2; }
			Picture_writeToWindowsMetafile (praat_picture, file);
		}
	}
#endif

#if defined (_WIN32) || defined (macintosh)
	DIRECT (Copy_picture_to_clipboard)
		Picture_copyToClipboard (praat_picture);
	END
#endif

/***** "Edit" MENU *****/

DIRECT (Undo)
	Graphics_undoGroup (GRAPHICS);
	if (theCurrentPraatPicture != & theForegroundPraatPicture) {
		Graphics_play (GRAPHICS, GRAPHICS);
	}
	Graphics_updateWs (GRAPHICS);
END

DIRECT (Erase_all)
	if (theCurrentPraatPicture == & theForegroundPraatPicture) {
		Picture_erase (praat_picture);   /* This kills the recording. */
	} else {
		Graphics_clearRecording (GRAPHICS);
		Graphics_clearWs (GRAPHICS);
	}
END

/***** "World" MENU *****/

FORM (Text, L"Praat picture: Text", L"Text...")
	REAL (L"Horizontal position", L"0.0")
	OPTIONMENU (L"Horizontal alignment", 2)
		OPTION (L"Left")
		OPTION (L"Centre")
		OPTION (L"Right")
	REAL (L"Vertical position", L"0.0")
	OPTIONMENU (L"Vertical alignment", 2)
		OPTION (L"Bottom")
		OPTION (L"Half")
		OPTION (L"Top")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
OK
DO
	autoPraatPicture picture;
	Graphics_setTextAlignment (GRAPHICS,
		GET_INTEGER (L"Horizontal alignment") - 1, GET_INTEGER (L"Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_text (GRAPHICS, GET_REAL (L"Horizontal position"),
		GET_REAL (L"Vertical position"), GET_STRING (L"text"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (Text_special, L"Praat picture: Text special", 0)
	REAL (L"Horizontal position", L"0.0")
	OPTIONMENU (L"Horizontal alignment", 2)
		OPTION (L"left")
		OPTION (L"centre")
		OPTION (L"right")
	REAL (L"Vertical position", L"0.0")
	OPTIONMENU (L"Vertical alignment", 2)
		OPTION (L"bottom")
		OPTION (L"half")
		OPTION (L"top")
	OPTIONMENU_ENUM (L"Font", kGraphics_font, DEFAULT)
	NATURAL (L"Font size", L"10")
	SENTENCE (L"Rotation (degrees or dx;dy)", L"0")
	LABEL (L"", L"Text:")
	TEXTFIELD (L"text", L"")
OK
DO
	kGraphics_font currentFont = Graphics_inqFont (GRAPHICS);
	int currentSize = Graphics_inqFontSize (GRAPHICS);
	autoPraatPicture picture;
	Graphics_setTextAlignment (GRAPHICS, GET_INTEGER (L"Horizontal alignment") - 1, GET_INTEGER (L"Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_setFont (GRAPHICS, GET_ENUM (kGraphics_font, L"Font"));
	Graphics_setFontSize (GRAPHICS, GET_INTEGER (L"Font size"));
	wchar_t *rotation = GET_STRING (L"Rotation"), *semicolon;
	if ((semicolon = wcschr (rotation, ';')) != NULL)
		Graphics_setTextRotation_vector (GRAPHICS, Melder_atof (rotation), Melder_atof (semicolon + 1));
	else
		Graphics_setTextRotation (GRAPHICS, Melder_atof (rotation));
	Graphics_text (GRAPHICS, GET_REAL (L"Horizontal position"), GET_REAL (L"Vertical position"), GET_STRING (L"text"));
	Graphics_setFont (GRAPHICS, currentFont);
	Graphics_setFontSize (GRAPHICS, currentSize);
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_unsetInner (GRAPHICS);
END

static void dia_line (Any dia) {
	REAL (L"From x", L"0.0")
	REAL (L"From y", L"0.0")
	REAL (L"To x", L"1.0")
	REAL (L"To y", L"1.0")
}
FORM (DrawLine, L"Praat picture: Draw line", 0)
	dia_line (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_line (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"From y"), GET_REAL (L"To x"),
		GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawArrow, L"Praat picture: Draw arrow", 0)
	dia_line (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_arrow (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"From y"), GET_REAL (L"To x"),
		GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawDoubleArrow, L"Praat picture: Draw double arrow", 0)
	dia_line (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_doubleArrow (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"From y"), GET_REAL (L"To x"),
		GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

Thing_define (PraatPictureFunction, Data) {
	// new data:
	public:
		double xmin, xmax, dx, x1;
		long nx;
	// overridden methods:
		virtual bool v_hasGetXmin () { return true; }   virtual double v_getXmin ()        { return xmin; }
		virtual bool v_hasGetXmax () { return true; }   virtual double v_getXmax ()        { return xmax; }
		virtual bool v_hasGetNx   () { return true; }   virtual double v_getNx   ()        { return nx; }
		virtual bool v_hasGetDx   () { return true; }   virtual double v_getDx   ()        { return dx; }
		virtual bool v_hasGetX    () { return true; }   virtual double v_getX    (long ix) { return x1 + (ix - 1) * dx; }
};
Thing_implement (PraatPictureFunction, Data, 0);

FORM (DrawFunction, L"Praat picture: Draw function", 0)
	LABEL (L"", L"This command assumes that the x and y axes")
	LABEL (L"", L"have been set by a Draw command or by \"Axes...\".")
	REAL (L"From x", L"0.0")
	REAL (L"To x", L"0.0 (= all)")
	NATURAL (L"Number of horizontal steps", L"1000")
	LABEL (L"", L"Formula:")
	TEXTFIELD (L"formula", L"x^2 - x^4")
OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	double fromX = GET_REAL (L"From x"), toX = GET_REAL (L"To x");
	long n = GET_INTEGER (L"Number of horizontal steps");
	wchar_t *formula = GET_STRING (L"formula");
	if (n < 2) return;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	if (fromX == toX) fromX = x1WC, toX = x2WC;
	autoNUMvector <double> y (1, n);
	autoPraatPictureFunction function = Thing_new (PraatPictureFunction);
	function -> xmin = x1WC;
	function -> xmax = x2WC;
	function -> nx = n;
	function -> x1 = fromX;
	function -> dx = (toX - fromX) / (n - 1);
	Formula_compile (interpreter, function.peek(), formula, kFormula_EXPRESSION_TYPE_NUMERIC, TRUE);
	for (long i = 1; i <= n; i ++) {
		struct Formula_Result result;
		Formula_run (1, i, & result);
		y [i] = result. result.numericResult;
	}
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_function (GRAPHICS, y.peek(), 1, n, fromX, toX);
	Graphics_unsetInner (GRAPHICS);
END

static void dia_rectangle (Any dia) {
	REAL (L"From x", L"0.0")
	REAL (L"To x", L"1.0")
	REAL (L"From y", L"0.0")
	REAL (L"To y", L"1.0")
}
FORM (DrawRectangle, L"Praat picture: Draw rectangle", 0)
	dia_rectangle (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_rectangle (GRAPHICS,
		GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (PaintRectangle, L"Praat picture: Paint rectangle", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	dia_rectangle (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (L"Colour"));
	Graphics_fillRectangle (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawRoundedRectangle, L"Praat picture: Draw rounded rectangle", 0)
	dia_rectangle (dia);
	POSITIVE (L"Radius (mm)", L"3.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_roundedRectangle (GRAPHICS,
		GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"), GET_REAL (L"Radius"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (PaintRoundedRectangle, L"Praat picture: Paint rounded rectangle", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	dia_rectangle (dia);
	POSITIVE (L"Radius (mm)", L"3.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (L"Colour"));
	Graphics_fillRoundedRectangle (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"), GET_REAL (L"Radius"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawArc, L"Praat picture: Draw arc", 0)
	REAL (L"Centre x", L"0.0")
	REAL (L"Centre y", L"0.0")
	POSITIVE (L"Radius (along x)", L"1.0")
	REAL (L"From angle (degrees)", L"0.0")
	REAL (L"To angle (degrees)", L"90.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_arc (GRAPHICS, GET_REAL (L"Centre x"), GET_REAL (L"Centre y"), GET_REAL (L"Radius"),
		GET_REAL (L"From angle"), GET_REAL (L"To angle"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawEllipse, L"Praat picture: Draw ellipse", 0)
	dia_rectangle (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_ellipse (GRAPHICS,
		GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (PaintEllipse, L"Praat picture: Paint ellipse", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	dia_rectangle (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (L"Colour"));
	Graphics_fillEllipse (GRAPHICS, GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawCircle, L"Praat picture: Draw circle", 0)
	REAL (L"Centre x", L"0.0")
	REAL (L"Centre y", L"0.0")
	POSITIVE (L"Radius (along x)", L"1.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_circle (GRAPHICS, GET_REAL (L"Centre x"), GET_REAL (L"Centre y"), GET_REAL (L"Radius"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (PaintCircle, L"Praat picture: Paint circle", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	REAL (L"Centre x", L"0")
	REAL (L"Centre y", L"0")
	POSITIVE (L"Radius (along x)", L"1.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (L"Colour"));
	Graphics_fillCircle (GRAPHICS, GET_REAL (L"Centre x"), GET_REAL (L"Centre y"), GET_REAL (L"Radius"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (DrawCircle_mm, L"Praat picture: Draw circle (mm)", 0)
	REAL (L"Centre x", L"0.0")
	REAL (L"Centre y", L"0.0")
	POSITIVE (L"Diameter (mm)", L"5.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_circle_mm (GRAPHICS, GET_REAL (L"Centre x"), GET_REAL (L"Centre y"), GET_REAL (L"Diameter"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (PaintCircle_mm, L"Praat picture: Paint circle (mm)", 0)
	COLOUR (L"Colour (0-1, name, or {r,g,b})", L"0.5")
	REAL (L"Centre x", L"0.0")
	REAL (L"Centre y", L"0.0")
	POSITIVE (L"Diameter (mm)", L"5.0")
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_setColour (GRAPHICS, GET_COLOUR (L"Colour"));
	Graphics_fillCircle_mm (GRAPHICS, GET_REAL (L"Centre x"), GET_REAL (L"Centre y"), GET_REAL (L"Diameter"));
	Graphics_unsetInner (GRAPHICS);
END

FORM (InsertPictureFromFile, L"Praat picture: Insert picture from file", L"Insert picture from file...")
	LABEL (L"", L"File name:")
	TEXTFIELD (L"fileName", L"~/Desktop/paul.jpg")
	dia_rectangle (dia);
OK
DO
	autoPraatPicture picture;
	Graphics_setInner (GRAPHICS);
	Graphics_imageFromFile (GRAPHICS, GET_STRING (L"fileName"), GET_REAL (L"From x"), GET_REAL (L"To x"), GET_REAL (L"From y"), GET_REAL (L"To y"));
	Graphics_unsetInner (GRAPHICS);
END


FORM (Axes, L"Praat picture: Axes", L"Axes...")
	REAL (L"left Left and right", L"0.0")
	REAL (L"right Left and right", L"1.0")
	REAL (L"left Bottom and top", L"0.0")
	REAL (L"right Bottom and top", L"1.0")
OK
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	SET_REAL (L"left Left and right", x1WC);
	SET_REAL (L"right Left and right", x2WC);
	SET_REAL (L"left Bottom and top", y1WC);
	SET_REAL (L"right Bottom and top", y2WC);
DO
	double left = GET_REAL (L"left Left and right"), right = GET_REAL (L"right Left and right");
	double top = GET_REAL (L"right Bottom and top"), bottom = GET_REAL (L"left Bottom and top");
	REQUIRE (right != left, L"Left and right must not be equal.")
	REQUIRE (top != bottom, L"Top and bottom must not be equal.")
	autoPraatPicture picture;
	Graphics_setWindow (GRAPHICS, left, right, bottom, top);
END

/***** "Margins" MENU *****/

DIRECT (DrawInnerBox)
	autoPraatPicture picture;
	Graphics_drawInnerBox (GRAPHICS);
END

FORM (Text_left, L"Praat picture: Text left", L"Text left/right/top/bottom...")
	BOOLEAN (L"Far", 1)
	TEXTFIELD (L"text", L"")
OK
DO
	autoPraatPicture picture;
	Graphics_textLeft (GRAPHICS, GET_INTEGER (L"Far"), GET_STRING (L"text"));
END

FORM (Text_right, L"Praat picture: Text right", L"Text left/right/top/bottom...")
	BOOLEAN (L"Far", 1)
	TEXTFIELD (L"text", L"")
OK
DO
	autoPraatPicture picture;
	Graphics_textRight (GRAPHICS, GET_INTEGER (L"Far"), GET_STRING (L"text"));
END

FORM (Text_top, L"Praat picture: Text top", L"Text left/right/top/bottom...")
	BOOLEAN (L"Far", 0)
	TEXTFIELD (L"text", L"")
OK
DO
	autoPraatPicture picture;
	Graphics_textTop (GRAPHICS, GET_INTEGER (L"Far"), GET_STRING (L"text"));
END

FORM (Text_bottom, L"Praat picture: Text bottom", L"Text left/right/top/bottom...")
	BOOLEAN (L"Far", 1)
	TEXTFIELD (L"text", L"")
OK
DO
	autoPraatPicture picture;
	Graphics_textBottom (GRAPHICS, GET_INTEGER (L"Far"), GET_STRING (L"text"));
END

static void dia_marksEvery (Any dia) {
	POSITIVE (L"Units", L"1.0")
	POSITIVE (L"Distance", L"0.1")
	BOOLEAN (L"Write numbers", 1)
	BOOLEAN (L"Draw ticks", 1)
	BOOLEAN (L"Draw dotted lines", 1)
}
static void do_marksEvery (Any dia, void (*Graphics_marksEvery) (Graphics, double, double, bool, bool, bool)) {
	autoPraatPicture picture;
	Graphics_marksEvery (GRAPHICS, GET_REAL (L"Units"), GET_REAL (L"Distance"),
		GET_INTEGER (L"Write numbers"),
		GET_INTEGER (L"Draw ticks"), GET_INTEGER (L"Draw dotted lines"));
}
FORM (Marks_left_every, L"Praat picture: Marks left every...", L"Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksLeftEvery); END
FORM (Marks_right_every, L"Praat picture: Marks right every...", L"Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksRightEvery); END
FORM (Marks_bottom_every, L"Praat picture: Marks bottom every...", L"Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksBottomEvery); END
FORM (Marks_top_every, L"Praat picture: Marks top every...", L"Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksTopEvery); END

static void dia_marks (Any dia) {
	NATURAL (L"Number of marks", L"6")
	BOOLEAN (L"Write numbers", 1)
	BOOLEAN (L"Draw ticks", 1)
	BOOLEAN (L"Draw dotted lines", 1)
}
static void do_marks (Any dia, void (*Graphics_marks) (Graphics, int, bool, bool, bool)) {
	long numberOfMarks = GET_INTEGER (L"Number of marks");
	REQUIRE (numberOfMarks >= 2, L"`Number of marks' must be at least 2.")
	autoPraatPicture picture;
	Graphics_marks (GRAPHICS, numberOfMarks, GET_INTEGER (L"Write numbers"),
		GET_INTEGER (L"Draw ticks"), GET_INTEGER (L"Draw dotted lines"));
}
FORM (Marks_left, L"Praat picture: Marks left", L"Marks left/right/top/bottom...")
	dia_marks (dia); OK DO do_marks (dia, Graphics_marksLeft); END
FORM (Marks_right, L"Praat picture: Marks right", L"Marks left/right/top/bottom...")
	dia_marks (dia); OK DO do_marks (dia, Graphics_marksRight); END
FORM (Marks_bottom, L"Praat picture: Marks bottom", L"Marks left/right/top/bottom...")
	dia_marks (dia); OK DO do_marks (dia, Graphics_marksBottom); END
FORM (Marks_top, L"Praat picture: Marks top", L"Marks left/right/top/bottom...")
	dia_marks (dia); OK DO do_marks (dia, Graphics_marksTop); END

static void dia_marksLogarithmic (Any dia) {
	NATURAL (L"Marks per decade", L"3")
	BOOLEAN (L"Write numbers", 1)
	BOOLEAN (L"Draw ticks", 1)
	BOOLEAN (L"Draw dotted lines", 1)
}
static void do_marksLogarithmic (Any dia, void (*Graphics_marksLogarithmic) (Graphics, int, bool, bool, bool)) {
	long numberOfMarksPerDecade = GET_INTEGER (L"Marks per decade");
	autoPraatPicture picture;
	Graphics_marksLogarithmic (GRAPHICS, numberOfMarksPerDecade, GET_INTEGER (L"Write numbers"),
		GET_INTEGER (L"Draw ticks"), GET_INTEGER (L"Draw dotted lines"));
}
FORM (marksLeftLogarithmic, L"Praat picture: Logarithmic marks left", L"Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksLeftLogarithmic); END
FORM (marksRightLogarithmic, L"Praat picture: Logarithmic marks right", L"Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksRightLogarithmic); END
FORM (marksBottomLogarithmic, L"Praat picture: Logarithmic marks bottom", L"Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksBottomLogarithmic); END
FORM (marksTopLogarithmic, L"Praat picture: Logarithmic marks top", L"Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksTopLogarithmic); END

static void sortBoundingBox (double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	double temp;
	if (*x1WC > *x2WC) temp = *x1WC, *x1WC = *x2WC, *x2WC = temp;
	if (*y1WC > *y2WC) temp = *y1WC, *y1WC = *y2WC, *y2WC = temp;
}

static void dia_oneMark (Any dia) {
	REAL (L"Position", L"0.0")
	BOOLEAN (L"Write number", 1)
	BOOLEAN (L"Draw tick", 1)
	BOOLEAN (L"Draw dotted line", 1)
	LABEL (L"", L"Draw text:")
	TEXTFIELD (L"text", L"")
}
FORM (Mark_left, L"Praat picture: One mark left", L"One mark left/right/top/bottom...")
	dia_oneMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw ("\"Position\" must be between ", y1WC, " and ", y2WC, ".");
	autoPraatPicture picture;
	Graphics_markLeft (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (Mark_right, L"Praat picture: One mark right", L"One mark left/right/top/bottom...")
	dia_oneMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy)
		Melder_throw ("\"Position\" must be between ", y1WC, " and ", y2WC, ".");
	autoPraatPicture picture;
	Graphics_markRight (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (Mark_top, L"Praat picture: One mark top", L"One mark left/right/top/bottom...")
	dia_oneMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{
		autoPraatPicture picture;   // WHY?
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw ("\"Position\" must be between ", x1WC, " and ", x2WC, ".");
	autoPraatPicture picture;
	Graphics_markTop (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (Mark_bottom, L"Praat picture: One mark bottom", L"One mark left/right/top/bottom...")
	dia_oneMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx)
		Melder_throw ("\"Position\" must be between ", x1WC, " and ", x2WC, ".");
	autoPraatPicture picture;
	Graphics_markBottom (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

static void dia_oneLogarithmicMark (Any dia) {
	REAL (L"Position", L"1.0")
	BOOLEAN (L"Write number", 1)
	BOOLEAN (L"Draw tick", 1)
	BOOLEAN (L"Draw dotted line", 1)
	LABEL (L"", L"Draw text:")
	TEXTFIELD (L"text", L"")
}
FORM (LogarithmicMark_left, L"Praat picture: One logarithmic mark left", L"One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw ("\"Position\" must be between ", pow (10, y1WC), " and ", pow (10, y2WC), ".");
	autoPraatPicture picture;
	Graphics_markLeftLogarithmic (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (LogarithmicMark_right, L"Praat picture: One logarithmic mark right", L"One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.2 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy))
		Melder_throw ("\"Position\" must be between ", pow (10, y1WC), " and ", pow (10, y2WC), ".");
	autoPraatPicture picture;
	Graphics_markRightLogarithmic (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (LogarithmicMark_top, L"Praat picture: One logarithmic mark top", L"One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw ("\"Position\" must be between ", pow (10, x1WC), " and ", pow (10, x2WC), ".");
	autoPraatPicture picture;
	Graphics_markTopLogarithmic (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (LogarithmicMark_bottom, L"Praat picture: One logarithmic mark bottom", L"One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
OK
DO
	double position = GET_REAL (L"Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	{
		autoPraatPicture picture;
		Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	}
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.2 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx))
		Melder_throw ("\"Position\" must be between ", pow (10, x1WC), " and ", pow (10, x2WC), ".");
	autoPraatPicture picture;
	Graphics_markBottomLogarithmic (GRAPHICS, position, GET_INTEGER (L"Write number"),
		GET_INTEGER (L"Draw tick"), GET_INTEGER (L"Draw dotted line"),
		GET_STRING (L"text"));
END

FORM (dxMMtoWC, L"Compute horizontal distance in world coordinates", 0)
	REAL (L"Distance (mm)", L"10.0")
OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dxMMtoWC (GRAPHICS, GET_REAL (L"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, L"(world coordinates)");
END

FORM (dxWCtoMM, L"Compute horizontal distance in millimetres", 0)
	REAL (L"Distance (wc)", L"0.1")
OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, GET_REAL (L"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, L"mm");
END

FORM (dyMMtoWC, L"Compute vertical distance in world coordinates", 0)
	REAL (L"Distance (mm)", L"10.0")
OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_dyMMtoWC (GRAPHICS, GET_REAL (L"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, L"(world coordinates)");
END

FORM (dyWCtoMM, L"Compute vertical distance in millimetres", 0)
	REAL (L"Distance (wc)", L"1.0")
OK
DO
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dyWCtoMM (GRAPHICS, GET_REAL (L"Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, L"mm");
END

FORM (textWidth_wc, L"Text width in world coordinates", 0)
	TEXTFIELD (L"text", L"Hello world")
OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth (GRAPHICS, GET_STRING (L"text"));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, L"(world coordinates)");
END

FORM (textWidth_mm, L"Text width in millimetres", 0)
	TEXTFIELD (L"text", L"Hello world")
OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_dxWCtoMM (GRAPHICS, Graphics_textWidth (GRAPHICS, GET_STRING (L"text")));
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, L"mm");
END

FORM (textWidth_ps_wc, L"PostScript text width in world coordinates", 0)
	RADIO (L"Phonetic font", 1)
		RADIOBUTTON (L"XIPA")
		RADIOBUTTON (L"SILIPA")
	TEXTFIELD (L"text", L"Hello world")
OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double wc = Graphics_textWidth_ps (GRAPHICS, GET_STRING (L"text"), GET_INTEGER (L"Phonetic font") - 1);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (wc, L"(world coordinates)");
END

FORM (textWidth_ps_mm, L"PostScript text width in millimetres", 0)
	RADIO (L"Phonetic font", 1)
		RADIOBUTTON (L"XIPA")
		RADIOBUTTON (L"SILIPA")
	TEXTFIELD (L"text", L"Hello world")
OK
DO
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	Graphics_setInner (GRAPHICS);
	double mm = Graphics_textWidth_ps_mm (GRAPHICS, GET_STRING (L"text"), GET_INTEGER (L"Phonetic font") - 1);
	Graphics_unsetInner (GRAPHICS);
	Melder_informationReal (mm, L"mm");
END

DIRECT (SearchManual) Melder_search (); END
DIRECT (PictureWindowHelp) Melder_help (L"Picture window"); END
DIRECT (AboutSpecialSymbols) Melder_help (L"Special symbols"); END
DIRECT (AboutTextStyles) Melder_help (L"Text styles"); END
DIRECT (PhoneticSymbols) Melder_help (L"Phonetic symbols"); END
DIRECT (Picture_settings_report)
	MelderInfo_open ();
	MelderInfo_writeLine (L"Outer viewport left: ", Melder_double (theCurrentPraatPicture -> x1NDC), L" inches");
	MelderInfo_writeLine (L"Outer viewport right: ", Melder_double (theCurrentPraatPicture -> x2NDC), L" inches");
	MelderInfo_writeLine (L"Outer viewport top: ", Melder_double (12 - theCurrentPraatPicture -> y2NDC), L" inches");
	MelderInfo_writeLine (L"Outer viewport bottom: ", Melder_double (12 - theCurrentPraatPicture -> y1NDC), L" inches");
	MelderInfo_writeLine (L"Font size: ", Melder_double (theCurrentPraatPicture -> fontSize), L" points");
	double xmargin = theCurrentPraatPicture -> fontSize * 4.2 / 72.0, ymargin = theCurrentPraatPicture -> fontSize * 2.8 / 72.0;
	if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC)) ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
	if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC)) xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
	MelderInfo_writeLine (L"Inner viewport left: ", Melder_double (theCurrentPraatPicture -> x1NDC + xmargin), L" inches");
	MelderInfo_writeLine (L"Inner viewport right: ", Melder_double (theCurrentPraatPicture -> x2NDC - xmargin), L" inches");
	MelderInfo_writeLine (L"Inner viewport top: ", Melder_double (12 - theCurrentPraatPicture -> y2NDC + ymargin), L" inches");
	MelderInfo_writeLine (L"Inner viewport bottom: ", Melder_double (12 - theCurrentPraatPicture -> y1NDC - ymargin), L" inches");
	MelderInfo_writeLine (L"Font: ", kGraphics_font_getText (theCurrentPraatPicture -> font));
	MelderInfo_writeLine (L"Line type: ",
		theCurrentPraatPicture -> lineType == Graphics_DRAWN ? L"Solid" :
		theCurrentPraatPicture -> lineType == Graphics_DOTTED ? L"Dotted" :
		theCurrentPraatPicture -> lineType == Graphics_DASHED ? L"Dashed" :
		theCurrentPraatPicture -> lineType == Graphics_DASHED_DOTTED ? L"Dashed-dotted" :
		L"(unknown)");
	MelderInfo_writeLine (L"Line width: ", Melder_double (theCurrentPraatPicture -> lineWidth));
	MelderInfo_writeLine (L"Arrow size: ", Melder_double (theCurrentPraatPicture -> arrowSize));
	MelderInfo_writeLine (L"Colour: ", Graphics_Colour_name (theCurrentPraatPicture -> colour));
	MelderInfo_writeLine (L"Red: ", Melder_double (theCurrentPraatPicture -> colour. red));
	MelderInfo_writeLine (L"Green: ", Melder_double (theCurrentPraatPicture -> colour. green));
	MelderInfo_writeLine (L"Blue: ", Melder_double (theCurrentPraatPicture -> colour. blue));
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	MelderInfo_writeLine (L"Axis left: ", Melder_double (x1WC));
	MelderInfo_writeLine (L"Axis right: ", Melder_double (x2WC));
	MelderInfo_writeLine (L"Axis bottom: ", Melder_double (y1WC));
	MelderInfo_writeLine (L"Axis top: ", Melder_double (y2WC));
	MelderInfo_close ();
END


/**********   **********/

static void cb_selectionChanged (Picture p, void *closure,
	double selx1, double selx2, double sely1, double sely2)
	/* The user selected a new viewport in the picture window. */
{
	(void) closure;
	Melder_assert (p == praat_picture);
	theCurrentPraatPicture -> x1NDC = selx1;
	theCurrentPraatPicture -> x2NDC = selx2;
	theCurrentPraatPicture -> y1NDC = sely1;
	theCurrentPraatPicture -> y2NDC = sely2;
	if (praat_mouseSelectsInnerViewport) {
		int fontSize = Graphics_inqFontSize (GRAPHICS);
		double xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
		if (ymargin > 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC)) ymargin = 0.4 * (theCurrentPraatPicture -> y2NDC - theCurrentPraatPicture -> y1NDC);
		if (xmargin > 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC)) xmargin = 0.4 * (theCurrentPraatPicture -> x2NDC - theCurrentPraatPicture -> x1NDC);
		UiHistory_write (L"\ndo (\"Select inner viewport...\", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x1NDC + xmargin));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x2NDC - xmargin));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y2NDC + ymargin));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y1NDC - ymargin));
		UiHistory_write (L")");
	} else {
		UiHistory_write (L"\ndo (\"Select outer viewport...\", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x1NDC));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (theCurrentPraatPicture -> x2NDC));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y2NDC));
		UiHistory_write (L", ");
		UiHistory_write (Melder_single (12 - theCurrentPraatPicture -> y1NDC));
		UiHistory_write (L")");
	}
}

/***** Public functions. *****/

static GuiWindow dialog;

static GuiMenu fileMenu, editMenu, marginsMenu, worldMenu, selectMenu, fontMenu, penMenu, helpMenu;

GuiMenu praat_picture_resolveMenu (const wchar_t *menu) {
	return
		wcsequ (menu, L"File") ? fileMenu :
		wcsequ (menu, L"Edit") ? editMenu :
		wcsequ (menu, L"Margins") ? marginsMenu :
		wcsequ (menu, L"World") ? worldMenu :
		wcsequ (menu, L"Select") ? selectMenu :
		wcsequ (menu, L"Font") ? fontMenu :
		wcsequ (menu, L"Pen") ? penMenu :
		wcsequ (menu, L"Help") ? helpMenu :
		editMenu;   /* Default. */
}

void praat_picture_exit (void) {
	Picture_remove (& praat_picture);
}

void praat_picture_open (void) {
	Graphics_markGroup (GRAPHICS);   // we start a group of graphics output here
	if (theCurrentPraatPicture == & theForegroundPraatPicture && ! theCurrentPraatApplication -> batch) {
		#if gtk
			gtk_window_present (GTK_WINDOW (dialog -> d_gtkWindow));
		#elif cocoa
			dialog -> f_show ();
		#elif motif
			XtMapWidget (dialog -> d_xmShell);
			XMapRaised (XtDisplay (dialog -> d_xmShell), XtWindow (dialog -> d_xmShell));
		#endif
		Picture_unhighlight (praat_picture);
	}
	/* Foregoing drawing routines may have changed some of the output attributes */
	/* that can be set by the user. */
	/* Make sure that they have the right values before every drawing. */
	/* This is especially necessary after an 'erase picture': */
	/* the output attributes that were set by the user before the 'erase' */
	/* must be recorded before copying to a PostScript file. */
	Graphics_setFont (GRAPHICS, static_cast<kGraphics_font> (theCurrentPraatPicture -> font));
	Graphics_setFontSize (GRAPHICS, theCurrentPraatPicture -> fontSize);
	Graphics_setLineType (GRAPHICS, theCurrentPraatPicture -> lineType);
	Graphics_setLineWidth (GRAPHICS, theCurrentPraatPicture -> lineWidth);
	Graphics_setArrowSize (GRAPHICS, theCurrentPraatPicture -> arrowSize);
	Graphics_setColour (GRAPHICS, theCurrentPraatPicture -> colour);

	Graphics_setViewport (GRAPHICS, theCurrentPraatPicture -> x1NDC, theCurrentPraatPicture -> x2NDC, theCurrentPraatPicture -> y1NDC, theCurrentPraatPicture -> y2NDC);
	/* The following will dump the axes to the PostScript file after Erase all. BUG: should be somewhere else. */
	double x1WC, x2WC, y1WC, y2WC;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
}

void praat_picture_close (void) {
	if (theCurrentPraatPicture != & theForegroundPraatPicture) return;
	if (! theCurrentPraatApplication -> batch) {
		Picture_highlight (praat_picture);
	}
}

Graphics praat_picture_editor_open (bool eraseFirst) {
	if (eraseFirst) Picture_erase (praat_picture);
	praat_picture_open ();
	return GRAPHICS;
}

void praat_picture_editor_close (void) {
	praat_picture_close ();
}

static Any pictureRecognizer (int nread, const char *header, MelderFile file) {
	if (nread < 2) return NULL;
	if (strnequ (header, "PraatPictureFile", 16))
	{
		Picture_readFromPraatPictureFile (praat_picture, file); return (Any) 1;
	}
	return NULL;
}

void praat_picture_init (void) {
	GuiScrolledWindow scrollWindow;
	GuiDrawingArea drawingArea = NULL;
	int margin, width, height, resolution, x, y;
	static MelderString itemTitle_search = { 0 };
	theCurrentPraatPicture -> lineType = Graphics_DRAWN;
	theCurrentPraatPicture -> colour = Graphics_BLACK;
	theCurrentPraatPicture -> lineWidth = 1.0;
	theCurrentPraatPicture -> arrowSize = 1.0;
	theCurrentPraatPicture -> x1NDC = 0.0;
	theCurrentPraatPicture -> x2NDC = 6.0;
	theCurrentPraatPicture -> y1NDC = 8.0;
	theCurrentPraatPicture -> y2NDC = 12.0;

	Data_recognizeFileType (pictureRecognizer);

	if (! theCurrentPraatApplication -> batch) {
		char pictureWindowTitle [100];
		// Ook al eerder gezien... Migreren naar UI?
		double screenX, screenY, screenWidth, screenHeight;
		Gui_getWindowPositioningBounds (& screenX, & screenY, & screenWidth, & screenHeight);
		resolution = Gui_getResolution (NULL);
		#if defined (macintosh)
			margin = 2, width = 6 * resolution + 20;
			height = 9 * resolution + Machine_getMenuBarHeight () + 24;
			x = screenX + screenWidth - width - 14;
			y = screenY + 0;
			width += margin * 2;
		#elif defined (_WIN32)
			margin = 2, width = 6 * resolution + 22;
			height = 9 * resolution + 24;
			x = screenX + screenWidth - width - 17;
			y = screenY + 0;
		#else
			margin = 0, width = 6 * resolution + 30;
			height = width * 3 / 2 + Machine_getTitleBarHeight ();
			x = screenX + screenWidth - width - 10;
			y = screenY + 0;
			width += margin * 2;
		#endif
		sprintf (pictureWindowTitle, "%s Picture", praatP.title);
		dialog = GuiWindow_create (x, y, width, height, Melder_peekUtf8ToWcs (pictureWindowTitle), NULL, NULL, 0);
		dialog -> f_addMenuBar ();
	}
	if (! theCurrentPraatApplication -> batch) {
		fileMenu =    GuiMenu_createInWindow (dialog, L"File", 0);
		editMenu =    GuiMenu_createInWindow (dialog, L"Edit", 0);
		marginsMenu = GuiMenu_createInWindow (dialog, L"Margins", 0);
		worldMenu =   GuiMenu_createInWindow (dialog, L"World", 0);
		selectMenu =  GuiMenu_createInWindow (dialog, L"Select", 0);
		penMenu =     GuiMenu_createInWindow (dialog, L"Pen", 0);
		fontMenu =    GuiMenu_createInWindow (dialog, L"Font", 0);
		helpMenu =    GuiMenu_createInWindow (dialog, L"Help", 0);
	}

	praat_addMenuCommand (L"Picture", L"File", L"Picture info", 0, 0, DO_Picture_settings_report);
	praat_addMenuCommand (L"Picture", L"File", L"Picture settings report", 0, praat_HIDDEN, DO_Picture_settings_report);
	praat_addMenuCommand (L"Picture", L"File", L"-- read --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"File", L"Read from praat picture file...", 0, 0, DO_Picture_readFromPraatPictureFile);
	praat_addMenuCommand (L"Picture", L"File", L"-- write --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"File", L"Save as praat picture file...", 0, 0, DO_Picture_writeToPraatPictureFile);
	praat_addMenuCommand (L"Picture", L"File", L"Write to praat picture file...", 0, praat_HIDDEN, DO_Picture_writeToPraatPictureFile);
	#ifdef _WIN32
	praat_addMenuCommand (L"Picture", L"File", L"Save as Windows metafile...", 0, 0, DO_Picture_writeToWindowsMetafile);
	praat_addMenuCommand (L"Picture", L"File", L"Write to Windows metafile...", 0, praat_HIDDEN, DO_Picture_writeToWindowsMetafile);
	#endif
	#if defined (macintosh)
		praat_addMenuCommand (L"Picture", L"File", L"Save as PDF file...", 0, 'S', DO_Picture_writeToPdfFile);
		praat_addMenuCommand (L"Picture", L"File", L"Write to PDF file...", 0, praat_HIDDEN, DO_Picture_writeToPdfFile);
		praat_addMenuCommand (L"Picture", L"File", L"Save EPS file", 0, 0, NULL);
			praat_addMenuCommand (L"Picture", L"File", L"PostScript settings...", 0, 1, DO_PostScript_settings);
			praat_addMenuCommand (L"Picture", L"File", L"Save as EPS file...", 0, 1, DO_Picture_writeToEpsFile);
			praat_addMenuCommand (L"Picture", L"File", L"Write to EPS file...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToEpsFile);
			praat_addMenuCommand (L"Picture", L"File", L"Save as fontless EPS file (XIPA)...", 0, 1, DO_Picture_writeToFontlessEpsFile_xipa);
			praat_addMenuCommand (L"Picture", L"File", L"Write to fontless EPS file (XIPA)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToFontlessEpsFile_xipa);
			praat_addMenuCommand (L"Picture", L"File", L"Save as fontless EPS file (SILIPA)...", 0, 1, DO_Picture_writeToFontlessEpsFile_silipa);
			praat_addMenuCommand (L"Picture", L"File", L"Write to fontless EPS file (SILIPA)...", 0, praat_HIDDEN + praat_DEPTH_1, DO_Picture_writeToFontlessEpsFile_silipa);
	#else
		praat_addMenuCommand (L"Picture", L"File", L"PostScript settings...", 0, 0, DO_PostScript_settings);
		praat_addMenuCommand (L"Picture", L"File", L"Save as EPS file...", 0, 'S', DO_Picture_writeToEpsFile);
		praat_addMenuCommand (L"Picture", L"File", L"Write to EPS file...", 0, praat_HIDDEN, DO_Picture_writeToEpsFile);
		praat_addMenuCommand (L"Picture", L"File", L"Save as fontless EPS file (XIPA)...", 0, 0, DO_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (L"Picture", L"File", L"Write to fontless EPS file (XIPA)...", 0, praat_HIDDEN, DO_Picture_writeToFontlessEpsFile_xipa);
		praat_addMenuCommand (L"Picture", L"File", L"Save as fontless EPS file (SILIPA)...", 0, 0, DO_Picture_writeToFontlessEpsFile_silipa);
		praat_addMenuCommand (L"Picture", L"File", L"Write to fontless EPS file (SILIPA)...", 0, praat_HIDDEN, DO_Picture_writeToFontlessEpsFile_silipa);
	#endif
	praat_addMenuCommand (L"Picture", L"File", L"-- print --", 0, 0, 0);
	#if defined (macintosh)
		praat_addMenuCommand (L"Picture", L"File", L"Page setup...", 0, 0, DO_Page_setup);
	#endif
	praat_addMenuCommand (L"Picture", L"File", L"Print...", 0, 'P', DO_Print);

	praat_addMenuCommand (L"Picture", L"Edit", L"Undo", 0, 'Z', DO_Undo);
	#if defined (macintosh) || defined (_WIN32)
		praat_addMenuCommand (L"Picture", L"Edit", L"-- clipboard --", 0, 0, 0);
		praat_addMenuCommand (L"Picture", L"Edit", L"Copy to clipboard", 0, 'C', DO_Copy_picture_to_clipboard);
	#endif
	praat_addMenuCommand (L"Picture", L"Edit", L"-- erase --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Edit", L"Erase all", 0, 'E', DO_Erase_all);

	praat_addMenuCommand (L"Picture", L"Margins", L"Draw inner box", 0, 0, DO_DrawInnerBox);
	praat_addMenuCommand (L"Picture", L"Margins", L"-- text --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Text left...", 0, 0, DO_Text_left);
	praat_addMenuCommand (L"Picture", L"Margins", L"Text right...", 0, 0, DO_Text_right);
	praat_addMenuCommand (L"Picture", L"Margins", L"Text top...", 0, 0, DO_Text_top);
	praat_addMenuCommand (L"Picture", L"Margins", L"Text bottom...", 0, 0, DO_Text_bottom);
	praat_addMenuCommand (L"Picture", L"Margins", L"-- marks every --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks left every...", 0, 0, DO_Marks_left_every);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks right every...", 0, 0, DO_Marks_right_every);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks bottom every...", 0, 0, DO_Marks_bottom_every);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks top every...", 0, 0, DO_Marks_top_every);
	praat_addMenuCommand (L"Picture", L"Margins", L"-- one mark --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"One mark left...", 0, 0, DO_Mark_left);
	praat_addMenuCommand (L"Picture", L"Margins", L"One mark right...", 0, 0, DO_Mark_right);
	praat_addMenuCommand (L"Picture", L"Margins", L"One mark bottom...", 0, 0, DO_Mark_bottom);
	praat_addMenuCommand (L"Picture", L"Margins", L"One mark top...", 0, 0, DO_Mark_top);
	praat_addMenuCommand (L"Picture", L"Margins", L"-- marks --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks left...", 0, 1, DO_Marks_left);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks right...", 0, 1, DO_Marks_right);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks bottom...", 0, 1, DO_Marks_bottom);
	praat_addMenuCommand (L"Picture", L"Margins", L"Marks top...", 0, 1, DO_Marks_top);
	praat_addMenuCommand (L"Picture", L"Margins", L"Logarithmic marks", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Logarithmic marks left...", 0, 1, DO_marksLeftLogarithmic);
	praat_addMenuCommand (L"Picture", L"Margins", L"Logarithmic marks right...", 0, 1, DO_marksRightLogarithmic);
	praat_addMenuCommand (L"Picture", L"Margins", L"Logarithmic marks bottom...", 0, 1, DO_marksBottomLogarithmic);
	praat_addMenuCommand (L"Picture", L"Margins", L"Logarithmic marks top...", 0, 1, DO_marksTopLogarithmic);
	praat_addMenuCommand (L"Picture", L"Margins", L"One logarithmic mark", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"One logarithmic mark left...", 0, 1, DO_LogarithmicMark_left);
	praat_addMenuCommand (L"Picture", L"Margins", L"One logarithmic mark right...", 0, 1, DO_LogarithmicMark_right);
	praat_addMenuCommand (L"Picture", L"Margins", L"One logarithmic mark bottom...", 0, 1, DO_LogarithmicMark_bottom);
	praat_addMenuCommand (L"Picture", L"Margins", L"One logarithmic mark top...", 0, 1, DO_LogarithmicMark_top);
	praat_addMenuCommand (L"Picture", L"Margins", L"-- axes --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Margins", L"Axes...", 0, 0, DO_Axes);

	praat_addMenuCommand (L"Picture", L"World", L"Text...", 0, 0, DO_Text);
	praat_addMenuCommand (L"Picture", L"World", L"Text special...", 0, 0, DO_Text_special);
	praat_addMenuCommand (L"Picture", L"World", L"-- line --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Draw line...", 0, 0, DO_DrawLine);
	praat_addMenuCommand (L"Picture", L"World", L"Draw arrow...", 0, 0, DO_DrawArrow);
	praat_addMenuCommand (L"Picture", L"World", L"Draw two-way arrow...", 0, 0, DO_DrawDoubleArrow);
	praat_addMenuCommand (L"Picture", L"World", L"-- function --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Draw function...", 0, 0, DO_DrawFunction);
	praat_addMenuCommand (L"Picture", L"World", L"-- rectangle --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Draw rectangle...", 0, 0, DO_DrawRectangle);
	praat_addMenuCommand (L"Picture", L"World", L"Paint rectangle...", 0, 0, DO_PaintRectangle);
	praat_addMenuCommand (L"Picture", L"World", L"Draw rounded rectangle...", 0, 0, DO_DrawRoundedRectangle);
	praat_addMenuCommand (L"Picture", L"World", L"Paint rounded rectangle...", 0, 0, DO_PaintRoundedRectangle);
	praat_addMenuCommand (L"Picture", L"World", L"-- arc --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Draw arc...", 0, 0, DO_DrawArc);
	praat_addMenuCommand (L"Picture", L"World", L"Draw ellipse...", 0, 0, DO_DrawEllipse);
	praat_addMenuCommand (L"Picture", L"World", L"Paint ellipse...", 0, 0, DO_PaintEllipse);
	praat_addMenuCommand (L"Picture", L"World", L"Draw circle...", 0, 0, DO_DrawCircle);
	praat_addMenuCommand (L"Picture", L"World", L"Paint circle...", 0, 0, DO_PaintCircle);
	praat_addMenuCommand (L"Picture", L"World", L"Draw circle (mm)...", 0, 0, DO_DrawCircle_mm);
	praat_addMenuCommand (L"Picture", L"World", L"Paint circle (mm)...", 0, 0, DO_PaintCircle_mm);
	#if defined (macintosh) || defined (_WIN32)
		praat_addMenuCommand (L"Picture", L"World", L"-- picture --", 0, 0, 0);
		praat_addMenuCommand (L"Picture", L"World", L"Insert picture from file...", 0, 0, DO_InsertPictureFromFile);
	#endif
	praat_addMenuCommand (L"Picture", L"World", L"-- axes --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Axes...", 0, 0, DO_Axes);
	praat_addMenuCommand (L"Picture", L"World", L"Measure", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Horizontal mm to wc...", 0, 1, DO_dxMMtoWC);
	praat_addMenuCommand (L"Picture", L"World", L"Horizontal wc to mm...", 0, 1, DO_dxWCtoMM);
	praat_addMenuCommand (L"Picture", L"World", L"Vertical mm to wc...", 0, 1, DO_dyMMtoWC);
	praat_addMenuCommand (L"Picture", L"World", L"Vertical wc to mm...", 0, 1, DO_dyWCtoMM);
	praat_addMenuCommand (L"Picture", L"World", L"-- text measure --", 0, 1, 0);
	praat_addMenuCommand (L"Picture", L"World", L"Text width (wc)...", 0, 1, DO_textWidth_wc);
	praat_addMenuCommand (L"Picture", L"World", L"Text width (mm)...", 0, 1, DO_textWidth_mm);
	praat_addMenuCommand (L"Picture", L"World", L"PostScript text width (wc)...", 0, 1, DO_textWidth_ps_wc);
	praat_addMenuCommand (L"Picture", L"World", L"PostScript text width (mm)...", 0, 1, DO_textWidth_ps_mm);

	praatButton_innerViewport = praat_addMenuCommand (L"Picture", L"Select", L"Mouse selects inner viewport", 0, praat_RADIO_FIRST, DO_MouseSelectsInnerViewport);
	praatButton_outerViewport = praat_addMenuCommand (L"Picture", L"Select", L"Mouse selects outer viewport", 0, praat_RADIO_NEXT, DO_MouseSelectsOuterViewport);
	praat_addMenuCommand (L"Picture", L"Select", L"-- select --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Select", L"Select inner viewport...", 0, 0, DO_SelectInnerViewport);
	praat_addMenuCommand (L"Picture", L"Select", L"Select outer viewport...", 0, 0, DO_SelectOuterViewport);
	praat_addMenuCommand (L"Picture", L"Select", L"Viewport...", 0, praat_HIDDEN, DO_SelectOuterViewport);
	praat_addMenuCommand (L"Picture", L"Select", L"-- viewport drawing --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Select", L"Viewport text...", 0, 0, DO_ViewportText);

	praatButton_lines [Graphics_DRAWN] = praat_addMenuCommand (L"Picture", L"Pen", L"Solid line", 0, praat_RADIO_FIRST, DO_Solid_line);
	praat_addMenuCommand (L"Picture", L"Pen", L"Plain line", 0, praat_RADIO_NEXT | praat_HIDDEN, DO_Solid_line);
	praatButton_lines [Graphics_DOTTED] = praat_addMenuCommand (L"Picture", L"Pen", L"Dotted line", 0, praat_RADIO_NEXT, DO_Dotted_line);
	praatButton_lines [Graphics_DASHED] = praat_addMenuCommand (L"Picture", L"Pen", L"Dashed line", 0, praat_RADIO_NEXT, DO_Dashed_line);
	praatButton_lines [Graphics_DASHED_DOTTED] = praat_addMenuCommand (L"Picture", L"Pen", L"Dashed-dotted line", 0, praat_RADIO_NEXT, DO_Dashed_dotted_line);
	praat_addMenuCommand (L"Picture", L"Pen", L"-- line width --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Pen", L"Line width...", 0, 0, DO_Line_width);
	praat_addMenuCommand (L"Picture", L"Pen", L"Arrow size...", 0, 0, DO_Arrow_size);
	praat_addMenuCommand (L"Picture", L"Pen", L"-- colour --", 0, 0, 0);
	praat_addMenuCommand (L"Picture", L"Pen", L"Colour...", 0, 0, DO_Colour);
	praatButton_black = praat_addMenuCommand (L"Picture", L"Pen", L"Black", 0, praat_CHECKBUTTON, DO_Black);
	praatButton_white = praat_addMenuCommand (L"Picture", L"Pen", L"White", 0, praat_CHECKBUTTON, DO_White);
	praatButton_red = praat_addMenuCommand (L"Picture", L"Pen", L"Red", 0, praat_CHECKBUTTON, DO_Red);
	praatButton_green = praat_addMenuCommand (L"Picture", L"Pen", L"Green", 0, praat_CHECKBUTTON, DO_Green);
	praatButton_blue = praat_addMenuCommand (L"Picture", L"Pen", L"Blue", 0, praat_CHECKBUTTON, DO_Blue);
	praatButton_yellow = praat_addMenuCommand (L"Picture", L"Pen", L"Yellow", 0, praat_CHECKBUTTON, DO_Yellow);
	praatButton_cyan = praat_addMenuCommand (L"Picture", L"Pen", L"Cyan", 0, praat_CHECKBUTTON, DO_Cyan);
	praatButton_magenta = praat_addMenuCommand (L"Picture", L"Pen", L"Magenta", 0, praat_CHECKBUTTON, DO_Magenta);
	praatButton_maroon = praat_addMenuCommand (L"Picture", L"Pen", L"Maroon", 0, praat_CHECKBUTTON, DO_Maroon);
	praatButton_lime = praat_addMenuCommand (L"Picture", L"Pen", L"Lime", 0, praat_CHECKBUTTON, DO_Lime);
	praatButton_navy = praat_addMenuCommand (L"Picture", L"Pen", L"Navy", 0, praat_CHECKBUTTON, DO_Navy);
	praatButton_teal = praat_addMenuCommand (L"Picture", L"Pen", L"Teal", 0, praat_CHECKBUTTON, DO_Teal);
	praatButton_purple = praat_addMenuCommand (L"Picture", L"Pen", L"Purple", 0, praat_CHECKBUTTON, DO_Purple);
	praatButton_olive = praat_addMenuCommand (L"Picture", L"Pen", L"Olive", 0, praat_CHECKBUTTON, DO_Olive);
	praatButton_pink = praat_addMenuCommand (L"Picture", L"Pen", L"Pink", 0, praat_CHECKBUTTON, DO_Pink);
	praatButton_silver = praat_addMenuCommand (L"Picture", L"Pen", L"Silver", 0, praat_CHECKBUTTON, DO_Silver);
	praatButton_grey = praat_addMenuCommand (L"Picture", L"Pen", L"Grey", 0, praat_CHECKBUTTON, DO_Grey);

	praat_addMenuCommand (L"Picture", L"Font", L"Font size...", 0, 0, DO_Font_size);
	praatButton_10 = praat_addMenuCommand (L"Picture", L"Font", L"10", 0, praat_CHECKBUTTON, DO_10);
	praatButton_12 = praat_addMenuCommand (L"Picture", L"Font", L"12", 0, praat_CHECKBUTTON, DO_12);
	praatButton_14 = praat_addMenuCommand (L"Picture", L"Font", L"14", 0, praat_CHECKBUTTON, DO_14);
	praatButton_18 = praat_addMenuCommand (L"Picture", L"Font", L"18", 0, praat_CHECKBUTTON, DO_18);
	praatButton_24 = praat_addMenuCommand (L"Picture", L"Font", L"24", 0, praat_CHECKBUTTON, DO_24);
	praat_addMenuCommand (L"Picture", L"Font", L"-- font ---", 0, 0, 0);
	praatButton_fonts [kGraphics_font_TIMES] = praat_addMenuCommand (L"Picture", L"Font", L"Times", 0, praat_RADIO_FIRST, DO_Times);
	praatButton_fonts [kGraphics_font_HELVETICA] = praat_addMenuCommand (L"Picture", L"Font", L"Helvetica", 0, praat_RADIO_NEXT, DO_Helvetica);
	praatButton_fonts [kGraphics_font_PALATINO] = praat_addMenuCommand (L"Picture", L"Font", L"Palatino", 0, praat_RADIO_NEXT, DO_Palatino);
	praatButton_fonts [kGraphics_font_COURIER] = praat_addMenuCommand (L"Picture", L"Font", L"Courier", 0, praat_RADIO_NEXT, DO_Courier);

	praat_addMenuCommand (L"Picture", L"Help", L"Picture window help", 0, '?', DO_PictureWindowHelp);
	praat_addMenuCommand (L"Picture", L"Help", L"About special symbols", 0, 0, DO_AboutSpecialSymbols);
	praat_addMenuCommand (L"Picture", L"Help", L"About text styles", 0, 0, DO_AboutTextStyles);
	praat_addMenuCommand (L"Picture", L"Help", L"Phonetic symbols", 0, 0, DO_PhoneticSymbols);
	praat_addMenuCommand (L"Picture", L"Help", L"-- manual --", 0, 0, 0);
	MelderString_empty (& itemTitle_search);
	MelderString_append (& itemTitle_search, L"Search ", Melder_peekUtf8ToWcs (praatP.title), L" manual...");
	praat_addMenuCommand (L"Picture", L"Help", itemTitle_search.string, 0, 'M', DO_SearchManual);

	if (! theCurrentPraatApplication -> batch) {
		width = height = resolution * 12;
		scrollWindow = GuiScrolledWindow_createShown (dialog, margin, 0, Machine_getMenuBarHeight () + margin, 0, 1, 1, 0);
		drawingArea = GuiDrawingArea_createShown (scrollWindow, width, height, NULL, NULL, NULL, NULL, NULL, 0);
		dialog -> f_show ();
	}

	// TODO: Paul: deze moet VOOR de update functies anders krijgen die void_me 0x0
	praat_picture = Picture_create (drawingArea, ! theCurrentPraatApplication -> batch);	
	// READ THIS!
	Picture_setSelectionChangedCallback (praat_picture, cb_selectionChanged, NULL);
	theCurrentPraatPicture -> graphics = static_cast<Graphics> (Picture_getGraphics (praat_picture));

	updatePenMenu ();
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
}

void praat_picture_prefsChanged (void) {
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
	Graphics_setFontSize (theCurrentPraatPicture -> graphics, theCurrentPraatPicture -> fontSize);   /* So that the thickness of the selection rectangle is correct. */
	Picture_setMouseSelectsInnerViewport (praat_picture, praat_mouseSelectsInnerViewport);
}

void praat_picture_background (void) {
	/*praat_picture_open ();
	Picture_background (praat_picture);*/
}

void praat_picture_foreground (void) {
	/*praat_picture_close ();
	Picture_foreground (praat_picture);*/
}

/* End of file praat_picture.cpp */
