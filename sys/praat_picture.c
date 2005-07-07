/* praat_picture.c
 *
 * Copyright (C) 1992-2005 Paul Boersma
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
 * pb 2002/03/07 GPL
 * pb 2003/05/19 Melder_atof
 * pb 2003/12/03 nicer dialogs
 * pb 2004/04/02 nicer dialogs
 * pb 2004/06/18 allow reversed axes
 * pb 2004/09/05 allow selection of inner viewport
 * pb 2005/05/07 inManual
 * pb 2005/07/05 Draw function...
 */

#include "praatP.h"
#include "Picture.h"
#include "Printer.h"
#include "machine.h"
#include "Formula.h"

/***** static variable *****/

static Picture praat_picture;

/********** CALLBACKS OF THE PICTURE MENUS **********/

static int praat_mouseSelectsInnerViewport = FALSE;
static double x1NDC = 0.0, x2NDC = 6.0, y1NDC = 8.0, y2NDC = 12.0;

/***** "Font" MENU: font part *****/

static int praat_font = Graphics_HELVETICA;
static Widget praatButton_times, praatButton_helvetica,
	praatButton_newCenturySchoolbook, praatButton_palatino, praatButton_courier;
static void updateFontMenu (void) {
	if (praat.batch) return;
	XmToggleButtonGadgetSetState (praatButton_times, praat_font == Graphics_TIMES, 0);
	XmToggleButtonGadgetSetState (praatButton_newCenturySchoolbook, praat_font == Graphics_NEWCENTURYSCHOOLBOOK, 0);
	XmToggleButtonGadgetSetState (praatButton_helvetica, praat_font == Graphics_HELVETICA, 0);
	XmToggleButtonGadgetSetState (praatButton_palatino, praat_font == Graphics_PALATINO, 0);
	XmToggleButtonGadgetSetState (praatButton_courier, praat_font == Graphics_COURIER, 0);
}
static void setFont (int font) {
	praat_picture_open ();
	Graphics_setFont (GRAPHICS, font);
	praat_picture_close ();
	if (! praatP.inManual) {
		praat_font = font;
		updateFontMenu ();
	}
}
DIRECT (Times) setFont (Graphics_TIMES); END
DIRECT (Helvetica) setFont (Graphics_HELVETICA); END
DIRECT (New_Century_Schoolbook) setFont (Graphics_NEWCENTURYSCHOOLBOOK); END
DIRECT (Palatino) setFont (Graphics_PALATINO); END
DIRECT (Courier) setFont (Graphics_COURIER); END

/***** "Font" MENU: size part *****/

static int praat_size = 10;
static Widget praatButton_10, praatButton_12, praatButton_14, praatButton_18, praatButton_24;
static void updateSizeMenu (void) {
	if (praat.batch) return;
	XmToggleButtonGadgetSetState (praatButton_10, praat_size == 10, 0);
	XmToggleButtonGadgetSetState (praatButton_12, praat_size == 12, 0);
	XmToggleButtonGadgetSetState (praatButton_14, praat_size == 14, 0);
	XmToggleButtonGadgetSetState (praatButton_18, praat_size == 18, 0);
	XmToggleButtonGadgetSetState (praatButton_24, praat_size == 24, 0);
}
static void setFontSize (int fontSize) {
	praat_picture_open ();
	Graphics_setFontSize (GRAPHICS, fontSize);
	praat_picture_close ();
	if (! praatP.inManual) {
		praat_size = fontSize;
		updateSizeMenu ();
	}
}
DIRECT (10) setFontSize (10); END
DIRECT (12) setFontSize (12); END
DIRECT (14) setFontSize (14); END
DIRECT (18) setFontSize (18); END
DIRECT (24) setFontSize (24); END
FORM (Font_size, "Praat picture: Font size", "Font menu")
	NATURAL ("Font size (points)", "10")
	OK
SET_INTEGER ("Font size", (long) praat_size);
DO
	setFontSize (GET_INTEGER ("Font size"));
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
	praat_picture_open ();
	Graphics_setFontSize (GRAPHICS, praat_size = fontSize);
	praat_picture_close ();
	Picture_setSelection (praat_picture, x1NDC, x2NDC, y1NDC, y2NDC, False);
	updateSizeMenu ();
}*/

/***** "Select" MENU *****/

static Widget praatButton_innerViewport, praatButton_outerViewport;
static void updateViewportMenu (void) {
	if (praat.batch) return;
	XmToggleButtonGadgetSetState (praatButton_innerViewport, praat_mouseSelectsInnerViewport ? 1 : 0, 0);
	XmToggleButtonGadgetSetState (praatButton_outerViewport, praat_mouseSelectsInnerViewport ? 0 : 1, 0);
}

DIRECT (MouseSelectsInnerViewport)
	praat_picture_open ();
	Picture_setMouseSelectsInnerViewport (praat_picture, praat_mouseSelectsInnerViewport = TRUE);
	praat_picture_close ();
	updateViewportMenu ();
END

DIRECT (MouseSelectsOuterViewport)
	praat_picture_open ();
	Picture_setMouseSelectsInnerViewport (praat_picture, praat_mouseSelectsInnerViewport = FALSE);
	praat_picture_close ();
	updateViewportMenu ();
END

FORM (SelectInnerViewport, "Praat picture: Select inner viewport", "Select inner viewport...")
	LABEL ("", "The viewport is the selected rectangle in the Picture window.")
	LABEL ("", "It is where your next drawing will appear.")
	LABEL ("", "The rectangle you select here will not include the margins.")
	LABEL ("", "")
	REAL ("left Horizontal range (inches)", "0.0")
	REAL ("right Horizontal range (inches)", "6.0")
	REAL ("left Vertical range (inches)", "0.0")
	REAL ("right Vertical range (inches)", "6.0")
	OK
double xmargin = praat_size * 4.2 / 72.0, ymargin = praat_size * 2.8 / 72.0;
if (ymargin > 0.4 * (y2NDC - y1NDC)) ymargin = 0.4 * (y2NDC - y1NDC);
if (xmargin > 0.4 * (x2NDC - x1NDC)) xmargin = 0.4 * (x2NDC - x1NDC);
SET_REAL ("left Horizontal range", x1NDC + xmargin);
SET_REAL ("right Horizontal range", x2NDC - xmargin);
SET_REAL ("left Vertical range", 12-y2NDC + ymargin);
SET_REAL ("right Vertical range", 12-y1NDC - ymargin);
DO
	double left = GET_REAL ("left Horizontal range"), right = GET_REAL ("right Horizontal range");
	double top = GET_REAL ("left Vertical range"), bottom = GET_REAL ("right Vertical range");
	double xmargin = praat_size * 4.2 / 72.0, ymargin = praat_size * 2.8 / 72.0;
	if (xmargin > 2 * (right - left)) xmargin = 2 * (right - left);
	if (ymargin > 2 * (bottom - top)) ymargin = 2 * (bottom - top);
	if (left == right) {
		Melder_error ("The left and right edges of the viewport cannot be equal.");
		return Melder_error ("Please change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_error ("The top and bottom edges of the viewport cannot be equal.");
		return Melder_error ("Please change the vertical range.");
	}
	if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
	x1NDC = left - xmargin;
	x2NDC = right + xmargin;
	y1NDC = 12-bottom - ymargin;
	y2NDC = 12-top + ymargin;
	Picture_setSelection (praat_picture, x1NDC, x2NDC, y1NDC, y2NDC, False);
END

FORM (SelectOuterViewport, "Praat picture: Select outer viewport", "Select outer viewport...")
	LABEL ("", "The viewport is the selected rectangle in the Picture window.")
	LABEL ("", "It is where your next drawing will appear.")
	LABEL ("", "The rectangle you select here will include the margins.")
	LABEL ("", "")
	REAL ("left Horizontal range (inches)", "0.0")
	REAL ("right Horizontal range (inches)", "6.0")
	REAL ("left Vertical range (inches)", "0.0")
	REAL ("right Vertical range (inches)", "6.0")
	OK
SET_REAL ("left Horizontal range", x1NDC);
SET_REAL ("right Horizontal range", x2NDC);
SET_REAL ("left Vertical range", 12-y2NDC);
SET_REAL ("right Vertical range", 12-y1NDC);
DO
	double left = GET_REAL ("left Horizontal range"), right = GET_REAL ("right Horizontal range");
	double top = GET_REAL ("left Vertical range"), bottom = GET_REAL ("right Vertical range");
	if (left == right) {
		Melder_error ("The left and right edges of the viewport cannot be equal.");
		return Melder_error ("Please change the horizontal range.");
	}
	if (left > right) { double temp; temp = left; left = right; right = temp; }
	if (top == bottom) {
		Melder_error ("The top and bottom edges of the viewport cannot be equal.");
		return Melder_error ("Please change the vertical range.");
	}
	if (top > bottom) { double temp; temp = top; top = bottom; bottom = temp; }
	x1NDC = left;
	x2NDC = right;
	y1NDC = 12-bottom;
	y2NDC = 12-top;
	Picture_setSelection (praat_picture, x1NDC, x2NDC, y1NDC, y2NDC, False);
END

FORM (ViewportText, "Praat picture: Viewport text", "Viewport text...")
	RADIO ("Horizontal alignment", 2)
		RADIOBUTTON ("Left")
		RADIOBUTTON ("Centre")
		RADIOBUTTON ("Right")
	RADIO ("Vertical alignment", 2)
		RADIOBUTTON ("Bottom")
		RADIOBUTTON ("Half")
		RADIOBUTTON ("Top")
	REAL ("Rotation (degrees)", "0")
	TEXTFIELD ("text", "")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	int hor = GET_INTEGER ("Horizontal alignment") - 1;
	int vert = GET_INTEGER ("Vertical alignment") - 1;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, 0, 1, 0, 1);
	Graphics_setTextAlignment (GRAPHICS, hor, vert);
	Graphics_setTextRotation (GRAPHICS, GET_REAL ("Rotation"));
	Graphics_text (GRAPHICS, hor == 0 ? 0 : hor == 1 ? 0.5 : 1,
		vert == 0 ? 0 : vert == 1 ? 0.5 : 1, GET_STRING ("text"));
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
	praat_picture_close ();
END

/***** "Pen" MENU *****/

static int praat_lineType = Graphics_DRAWN;
static int praat_colour = Graphics_BLACK;
static float praat_lineWidth = 1.0;

static Widget praatButton_plainLine, praatButton_dottedLine, praatButton_dashedLine;
static Widget praatButton_black, praatButton_white, praatButton_red, praatButton_green, praatButton_blue,
	praatButton_yellow, praatButton_cyan, praatButton_magenta, praatButton_maroon, praatButton_lime,
	praatButton_navy, praatButton_teal, praatButton_purple, praatButton_olive, praatButton_silver, praatButton_grey;
static void updatePenMenu (void) {
	if (praat.batch) return;
	XmToggleButtonGadgetSetState (praatButton_plainLine, praat_lineType == Graphics_DRAWN, 0);
	XmToggleButtonGadgetSetState (praatButton_dottedLine, praat_lineType == Graphics_DOTTED, 0);
	XmToggleButtonGadgetSetState (praatButton_dashedLine, praat_lineType == Graphics_DASHED, 0);
	XmToggleButtonGadgetSetState (praatButton_black, praat_colour == Graphics_BLACK, 0);
	XmToggleButtonGadgetSetState (praatButton_white, praat_colour == Graphics_WHITE, 0);
	XmToggleButtonGadgetSetState (praatButton_red, praat_colour == Graphics_RED, 0);
	XmToggleButtonGadgetSetState (praatButton_green, praat_colour == Graphics_GREEN, 0);
	XmToggleButtonGadgetSetState (praatButton_blue, praat_colour == Graphics_BLUE, 0);
	XmToggleButtonGadgetSetState (praatButton_yellow, praat_colour == Graphics_YELLOW, 0);
	XmToggleButtonGadgetSetState (praatButton_cyan, praat_colour == Graphics_CYAN, 0);
	XmToggleButtonGadgetSetState (praatButton_magenta, praat_colour == Graphics_MAGENTA, 0);
	XmToggleButtonGadgetSetState (praatButton_maroon, praat_colour == Graphics_MAROON, 0);
	XmToggleButtonGadgetSetState (praatButton_lime, praat_colour == Graphics_LIME, 0);
	XmToggleButtonGadgetSetState (praatButton_navy, praat_colour == Graphics_NAVY, 0);
	XmToggleButtonGadgetSetState (praatButton_teal, praat_colour == Graphics_TEAL, 0);
	XmToggleButtonGadgetSetState (praatButton_purple, praat_colour == Graphics_PURPLE, 0);
	XmToggleButtonGadgetSetState (praatButton_olive, praat_colour == Graphics_OLIVE, 0);
	XmToggleButtonGadgetSetState (praatButton_silver, praat_colour == Graphics_SILVER, 0);
	XmToggleButtonGadgetSetState (praatButton_grey, praat_colour == Graphics_GREY, 0);
}
static void setLineType (int lineType) {
	praat_picture_open ();
	Graphics_setLineType (GRAPHICS, lineType);
	praat_picture_close ();
	if (! praatP.inManual) {
		praat_lineType = lineType;
		updatePenMenu ();
	}
}
DIRECT (Plain_line) setLineType (Graphics_DRAWN); END
DIRECT (Dotted_line) setLineType (Graphics_DOTTED); END
DIRECT (Dashed_line) setLineType (Graphics_DASHED); END

FORM (Line_width, "Praat picture: Line width", 0)
	POSITIVE ("Line width", "1.0")
	OK
SET_REAL ("Line width", praat_lineWidth);
DO
	double lineWidth = GET_REAL ("Line width");
	praat_picture_open ();
	Graphics_setLineWidth (GRAPHICS, lineWidth);
	praat_picture_close ();
	if (! praatP.inManual) {
		praat_lineWidth = lineWidth;
	}
END

static void setColour (int colour) {
	praat_picture_open ();
	Graphics_setColour (GRAPHICS, colour);
	praat_picture_close ();
	if (! praatP.inManual) {
		praat_colour = colour;
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
DIRECT (Silver) setColour (Graphics_SILVER); END
DIRECT (Grey) setColour (Graphics_GREY); END

/***** "File" MENU *****/

FORM_READ (Picture_readFromPraatPictureFile, "Read picture from praat picture file", 0)
	return Picture_readFromPraatPictureFile (praat_picture, file);
END

FORM_READ (Picture_readFromOldPraatPictureFile, "Read picture from old praat picture file", 0)
	int result;
	Graphics_setWsWindow (GRAPHICS, 0, 2, -1, 1);
	result = Picture_readFromPraatPictureFile (praat_picture, file);
	Graphics_setWsWindow (GRAPHICS, 0, 12, 0, 12);
	return result;
END

#ifdef _WIN32
FORM_READ (Picture_readFromOldWindowsPraatPictureFile, "Read picture from praat picture file", 0)
	return Picture_readFromOldWindowsPraatPictureFile (praat_picture, file);
END
#endif

static int DO_Picture_writeToEpsFile (Any sender, void *dummy) {
	static Any dia;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (praat.topShell, "Write to EPS file",
		DO_Picture_writeToEpsFile, NULL, NULL);
	if (! sender) {
		UiOutfile_do (dia, "praat.eps");
	} else { MelderFile file;
		if (sender == dia) file = UiFile_getFile (sender);
		else { structMelderFile file2; if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
		return Picture_writeToEpsFile (praat_picture, file, TRUE); }
	}
	return 1;
}
/*FORM_WRITE (Picture_writeToEpsFile, "Write picture to Encapsulated PostScript file", 0, "praat.eps")
	if (! Picture_writeToEpsFile (praat_picture, fileName, TRUE)) return 0;
END*/

static int DO_Picture_writeToFontlessEpsFile (Any sender, void *dummy) {
	static Any dia;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (praat.topShell, "Write to fontless EPS file",
		DO_Picture_writeToFontlessEpsFile, NULL, NULL);
	if (! sender) {
		UiOutfile_do (dia, "praat.eps");
	} else { MelderFile file;
		if (sender == dia) file = UiFile_getFile (sender);
		else { structMelderFile file2; if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
		return Picture_writeToEpsFile (praat_picture, file, FALSE); }
	}
	return 1;
}

static int DO_Picture_writeToPraatPictureFile (Any sender, void *dummy) {
	static Any dia;
	(void) dummy;
	if (! dia) dia = UiOutfile_create (praat.topShell, "Write to Praat picture file",
		DO_Picture_writeToPraatPictureFile, NULL, NULL);
	if (! sender) {
		UiOutfile_do (dia, "praat.prapic");
	} else { MelderFile file;
		if (sender == dia) file = UiFile_getFile (sender);
		else { structMelderFile file2; if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
		return Picture_writeToPraatPictureFile (praat_picture, file); }
	}
	return 1;
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

#ifdef macintosh
	static int DO_Picture_writeToMacPictFile (Any sender, void *dummy) {
		static Any dia;
		(void) dummy;
		if (! dia) dia = UiOutfile_create (praat.topShell, "Write to Mac PICT file",
			DO_Picture_writeToMacPictFile, NULL, NULL);
		if (! sender) {
			UiOutfile_do (dia, "praat.pict");
		} else { MelderFile file;
			if (sender == dia) file = UiFile_getFile (sender);
			else { structMelderFile file2; if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
			return Picture_writeToMacPictFile (praat_picture, file); }
		}
		return 1;
	}
#endif
#ifdef _WIN32
	static int DO_Picture_writeToWindowsMetafile (Any sender, void *dummy) {
		static Any dia;
		(void) dummy;
		if (! dia) dia = UiOutfile_create (praat.topShell, "Write to Windows metafile",
			DO_Picture_writeToWindowsMetafile, NULL, NULL);
		if (! sender) {
			UiOutfile_do (dia, "praat.emf");
		} else { MelderFile file;
			if (sender == dia) file = UiFile_getFile (sender);
			else { structMelderFile file2; if (! Melder_relativePathToFile (sender, & file2)) return 0; file = & file2; } {
			return Picture_writeToWindowsMetafile (praat_picture, file); }
		}
		return 1;
	}
#endif

#if defined (_WIN32) || defined (macintosh)
	DIRECT (Copy_picture_to_clipboard)
		Picture_copyToClipboard (praat_picture);
	END
#endif
#if defined (macintosh)
	DIRECT (Copy_screen_image_to_clipboard)
		Picture_copyToClipboard_screenImage (praat_picture);
	END
#endif

/***** "Edit" MENU *****/

DIRECT (Undo)
	Graphics_undoGroup (GRAPHICS);
	Graphics_updateWs (GRAPHICS);
END

DIRECT (Erase_all)
	Picture_erase (praat_picture);   /* This kills the recording. */
END

/***** "World" MENU *****/

FORM (Text, "Praat picture: Text", "Text...")
	REAL ("Horizontal position", "0.0")
	OPTIONMENU ("Horizontal alignment", 2)
		OPTION ("Left")
		OPTION ("Centre")
		OPTION ("Right")
	REAL ("Vertical position", "0.0")
	OPTIONMENU ("Vertical alignment", 2)
		OPTION ("Bottom")
		OPTION ("Half")
		OPTION ("Top")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	praat_picture_open ();
	Graphics_setTextAlignment (GRAPHICS,
		GET_INTEGER ("Horizontal alignment") - 1, GET_INTEGER ("Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_text (GRAPHICS, GET_REAL ("Horizontal position"),
		GET_REAL ("Vertical position"), GET_STRING ("text"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (Text_special, "Praat picture: Text special", 0)
	REAL ("Horizontal position", "0.0")
	OPTIONMENU ("Horizontal alignment", 2)
		OPTION ("Left")
		OPTION ("Centre")
		OPTION ("Right")
	REAL ("Vertical position", "0.0")
	OPTIONMENU ("Vertical alignment", 2)
		OPTION ("Bottom")
		OPTION ("Half")
		OPTION ("Top")
	OPTIONMENU ("Font", 1)
		OPTION ("Times")
		OPTION ("New_Century_Schoolbook")
		OPTION ("Helvetica")
		OPTION ("Courier")
	NATURAL ("Font size", "10")
	SENTENCE ("Rotation (degrees or dx;dy)", "0")
	LABEL ("", "Text:")
	TEXTFIELD ("text", "")
	OK
DO
	int currentFont = Graphics_inqFont (GRAPHICS);
	int currentSize = Graphics_inqFontSize (GRAPHICS);
	int requiredFont = GET_INTEGER ("Font");
	praat_picture_open ();
	Graphics_setTextAlignment (GRAPHICS,
		GET_INTEGER ("Horizontal alignment") - 1, GET_INTEGER ("Vertical alignment") - 1);
	Graphics_setInner (GRAPHICS);
	Graphics_setFont (GRAPHICS,
		requiredFont == 1 ? Graphics_TIMES :
		requiredFont == 2 ? Graphics_NEWCENTURYSCHOOLBOOK :
		requiredFont == 3 ? Graphics_HELVETICA :
		Graphics_COURIER);
	Graphics_setFontSize (GRAPHICS, GET_INTEGER ("Font size"));
	{
		char *rotation = GET_STRING ("Rotation"), *semicolon;
		if ((semicolon = strchr (rotation, ';')) != NULL)
			Graphics_setTextRotation_vector (GRAPHICS,
				Melder_atof (rotation), Melder_atof (semicolon + 1));
		else
			Graphics_setTextRotation (GRAPHICS, Melder_atof (rotation));
	}
	Graphics_text (GRAPHICS, GET_REAL ("Horizontal position"),
		GET_REAL ("Vertical position"), GET_STRING ("text"));
	Graphics_setFont (GRAPHICS, currentFont);
	Graphics_setFontSize (GRAPHICS, currentSize);
	Graphics_setTextRotation (GRAPHICS, 0.0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

static void dia_line (Any dia) {
	REAL ("From x", "0.0")
	REAL ("From y", "0.0")
	REAL ("To x", "1.0")
	REAL ("To y", "1.0")
}
FORM (DrawLine, "Praat picture: Draw line", 0)
	dia_line (dia);
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_line (GRAPHICS, GET_REAL ("From x"), GET_REAL ("From y"), GET_REAL ("To x"),
		GET_REAL ("To y"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawArrow, "Praat picture: Draw arrow", 0)
	dia_line (dia);
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_arrow (GRAPHICS, GET_REAL ("From x"), GET_REAL ("From y"), GET_REAL ("To x"),
		GET_REAL ("To y"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawDoubleArrow, "Praat picture: Draw double arrow", 0)
	dia_line (dia);
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_arrow (GRAPHICS, GET_REAL ("From x"), GET_REAL ("From y"), GET_REAL ("To x"),
		GET_REAL ("To y"));
	Graphics_arrow (GRAPHICS, GET_REAL ("To x"), GET_REAL ("To y"), GET_REAL ("From x"),
		GET_REAL ("From y"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

#define PraatPictureFunction_members Data_members \
	double xmin, xmax, dx, x1; \
	long nx;
#define PraatPictureFunction_methods Data_methods
class_create (PraatPictureFunction, Data)

static double getXmin (I) { iam (PraatPictureFunction); return my xmin; }
static double getXmax (I) { iam (PraatPictureFunction); return my xmax; }
static double getNx (I) { iam (PraatPictureFunction); return my nx; }
static double getDx (I) { iam (PraatPictureFunction); return my dx; }
static double getX (I, long ix) { iam (PraatPictureFunction); return my x1 + (ix - 1) * my dx; }

class_methods (PraatPictureFunction, Data)
	class_method (getXmin)
	class_method (getXmax)
	class_method (getNx)
	class_method (getDx)
	class_method (getX)
class_methods_end

FORM (DrawFunction, "Praat picture: Draw function", 0)
	REAL ("From x", "0.0")
	REAL ("To x", "0.0 (= all)")
	NATURAL ("Number of horizontal steps", "1000")
	LABEL ("", "Formula:")
	TEXTFIELD ("formula", "x^2 - x^4")
	OK
DO
	double x1WC, x2WC, y1WC, y2WC;
	double fromX = GET_REAL ("From x"), toX = GET_REAL ("To x");
	long n = GET_INTEGER ("Number of horizontal steps"), i;
	float *y = NULL;
	PraatPictureFunction function = NULL;
	if (n < 2) return 1;
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	if (fromX == toX) fromX = x1WC, toX = x2WC;
	y = NUMfvector (1, n); cherror
	function = new (PraatPictureFunction); cherror
	function -> xmin = x1WC;
	function -> xmax = x2WC;
	function -> nx = n;
	function -> x1 = fromX;
	function -> dx = (toX - fromX) / (n - 1);
	if (! Formula_compile (NULL, function, GET_STRING ("formula"), FALSE, TRUE)) return 0;
	for (i = 1; i <= n; i ++) {
		double result;
		if (! Formula_run (1, i, & result, NULL)) return 0;
		y [i] = result;
	}
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_function (GRAPHICS, y, 1, n, fromX, toX);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
end:
	NUMfvector_free (y, 1);
	forget (function);
	iferror return 0;
END

static void dia_rectangle (Any dia) {
	REAL ("From x", "0.0")
	REAL ("To x", "1.0")
	REAL ("From y", "0.0")
	REAL ("To y", "1.0")
}
FORM (DrawRectangle, "Praat picture: Draw rectangle", 0)
	dia_rectangle (dia);
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_rectangle (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (PaintRectangle, "Praat picture: Paint rectangle", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	dia_rectangle (dia);
	OK
DO
	double realColour = GET_REAL ("Colour");
	int integerColour = floor (realColour);
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	if (integerColour)
		Graphics_setColour (GRAPHICS, integerColour);
	else
		Graphics_setGrey (GRAPHICS, realColour);
	Graphics_fillRectangle (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"));
	if (! integerColour) Graphics_setGrey (GRAPHICS, 0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawRoundedRectangle, "Praat picture: Draw rounded rectangle", 0)
	dia_rectangle (dia);
	POSITIVE ("Radius (mm)", "3.0")
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_roundedRectangle (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"), GET_REAL ("Radius"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (PaintRoundedRectangle, "Praat picture: Paint rounded rectangle", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	dia_rectangle (dia);
	POSITIVE ("Radius (mm)", "3.0")
	OK
DO
	double realColour = GET_REAL ("Colour");
	int integerColour = floor (realColour);
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	if (integerColour)
		Graphics_setColour (GRAPHICS, integerColour);
	else
		Graphics_setGrey (GRAPHICS, realColour);
	Graphics_fillRoundedRectangle (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"), GET_REAL ("Radius"));
	if (! integerColour) Graphics_setGrey (GRAPHICS, 0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawArc, "Praat picture: Draw arc", 0)
	REAL ("Centre x", "0.0")
	REAL ("Centre y", "0.0")
	POSITIVE ("Radius (along x)", "1.0")
	REAL ("From angle (degrees)", "0.0")
	REAL ("To angle (degrees)", "90.0")
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_arc (GRAPHICS, GET_REAL ("Centre x"), GET_REAL ("Centre y"), GET_REAL ("Radius"),
		GET_REAL ("From angle"), GET_REAL ("To angle"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawEllipse, "Praat picture: Draw ellipse", 0)
	dia_rectangle (dia);
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_ellipse (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (PaintEllipse, "Praat picture: Paint ellipse", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	dia_rectangle (dia);
	OK
DO
	double realColour = GET_REAL ("Colour");
	int integerColour = floor (realColour);
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	if (integerColour)
		Graphics_setColour (GRAPHICS, integerColour);
	else
		Graphics_setGrey (GRAPHICS, realColour);
	Graphics_fillEllipse (GRAPHICS,
		GET_REAL ("From x"), GET_REAL ("To x"), GET_REAL ("From y"), GET_REAL ("To y"));
	if (! integerColour) Graphics_setGrey (GRAPHICS, 0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawCircle, "Praat picture: Draw circle", 0)
	REAL ("Centre x", "0.0")
	REAL ("Centre y", "0.0")
	POSITIVE ("Radius (along x)", "1.0")
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_circle (GRAPHICS, GET_REAL ("Centre x"), GET_REAL ("Centre y"), GET_REAL ("Radius"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (PaintCircle, "Praat picture: Paint circle", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	REAL ("Centre x", "0")
	REAL ("Centre y", "0")
	POSITIVE ("Radius (along x)", "1.0")
	OK
DO
	double realColour = GET_REAL ("Colour");
	int integerColour = floor (realColour);
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	if (integerColour)
		Graphics_setColour (GRAPHICS, integerColour);
	else
		Graphics_setGrey (GRAPHICS, realColour);
	Graphics_fillCircle (GRAPHICS, GET_REAL ("Centre x"), GET_REAL ("Centre y"), GET_REAL ("Radius"));
	if (! integerColour) Graphics_setGrey (GRAPHICS, 0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (DrawCircle_mm, "Praat picture: Draw circle (mm)", 0)
	REAL ("Centre x", "0.0")
	REAL ("Centre y", "0.0")
	POSITIVE ("Diameter (mm)", "5.0")
	OK
DO
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	Graphics_circle_mm (GRAPHICS, GET_REAL ("Centre x"), GET_REAL ("Centre y"), GET_REAL ("Diameter"));
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END

FORM (PaintCircle_mm, "Praat picture: Paint circle (mm)", 0)
	COLOUR ("Colour (0-1 or name)", "0.5")
	REAL ("Centre x", "0.0")
	REAL ("Centre y", "0.0")
	POSITIVE ("Diameter (mm)", "5.0")
	OK
DO
	double realColour = GET_REAL ("Colour");
	int integerColour = floor (realColour);
	praat_picture_open ();
	Graphics_setInner (GRAPHICS);
	if (integerColour)
		Graphics_setColour (GRAPHICS, integerColour);
	else
		Graphics_setGrey (GRAPHICS, realColour);
	Graphics_fillCircle_mm (GRAPHICS, GET_REAL ("Centre x"), GET_REAL ("Centre y"), GET_REAL ("Diameter"));
	if (! integerColour) Graphics_setGrey (GRAPHICS, 0);
	Graphics_unsetInner (GRAPHICS);
	praat_picture_close ();
END


FORM (Axes, "Praat picture: Axes", "Axes...")
	REAL ("left Left and right", "0.0")
	REAL ("right Left and right", "1.0")
	REAL ("left Bottom and top", "0.0")
	REAL ("right Bottom and top", "1.0")
	OK
double x1WC, x2WC, y1WC, y2WC;
Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
SET_REAL ("left Left and right", x1WC);
SET_REAL ("right Left and right", x2WC);
SET_REAL ("left Bottom and top", y1WC);
SET_REAL ("right Bottom and top", y2WC);
DO
	double left = GET_REAL ("left Left and right"), right = GET_REAL ("right Left and right");
	double top = GET_REAL ("right Bottom and top"), bottom = GET_REAL ("left Bottom and top");
	REQUIRE (right != left, "Left and right must not be equal.")
	REQUIRE (top != bottom, "Top and bottom must not be equal.")
	praat_picture_open ();
	Graphics_setWindow (GRAPHICS, left, right, bottom, top);
	praat_picture_close ();
END

/***** "Margins" MENU *****/

DIRECT (DrawInnerBox)
	praat_picture_open ();
	Graphics_drawInnerBox (GRAPHICS);
	praat_picture_close ();
END

FORM (Text_left, "Praat picture: Text left", "Text left/right/top/bottom...")
	BOOLEAN ("Far", 1)
	TEXTFIELD ("text", "")
	OK
DO
	praat_picture_open ();
	Graphics_textLeft (GRAPHICS, GET_INTEGER ("Far"), GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Text_right, "Praat picture: Text right", "Text left/right/top/bottom...")
	BOOLEAN ("Far", 1)
	TEXTFIELD ("text", "")
	OK
DO
	praat_picture_open ();
	Graphics_textRight (GRAPHICS, GET_INTEGER ("Far"), GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Text_top, "Praat picture: Text top", "Text left/right/top/bottom...")
	BOOLEAN ("Far", 0)
	TEXTFIELD ("text", "")
	OK
DO
	praat_picture_open ();
	Graphics_textTop (GRAPHICS, GET_INTEGER ("Far"), GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Text_bottom, "Praat picture: Text bottom", "Text left/right/top/bottom...")
	BOOLEAN ("Far", 1)
	TEXTFIELD ("text", "")
	OK
DO
	praat_picture_open ();
	Graphics_textBottom (GRAPHICS, GET_INTEGER ("Far"), GET_STRING ("text"));
	praat_picture_close ();
END

static void dia_marksEvery (Any dia) {
	POSITIVE ("Units", "1.0")
	POSITIVE ("Distance", "0.1")
	BOOLEAN ("Write numbers", 1)
	BOOLEAN ("Draw ticks", 1)
	BOOLEAN ("Draw dotted lines", 1)
}
static void do_marksEvery (Any dia, void (*Graphics_marksEvery) (void *, double, double, int, int, int)) {
	praat_picture_open ();
	Graphics_marksEvery (GRAPHICS, GET_REAL ("Units"), GET_REAL ("Distance"),
		GET_INTEGER ("Write numbers"),
		GET_INTEGER ("Draw ticks"), GET_INTEGER ("Draw dotted lines"));
	praat_picture_close ();
}
FORM (Marks_left_every, "Praat picture: Marks left every...", "Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksLeftEvery); END
FORM (Marks_right_every, "Praat picture: Marks right every...", "Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksRightEvery); END
FORM (Marks_bottom_every, "Praat picture: Marks bottom every...", "Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksBottomEvery); END
FORM (Marks_top_every, "Praat picture: Marks top every...", "Marks left/right/top/bottom every...")
	dia_marksEvery (dia); OK DO do_marksEvery (dia, Graphics_marksTopEvery); END

static void dia_marks (Any dia) {
	NATURAL ("Number of marks", "6")
	BOOLEAN ("Write numbers", 1)
	BOOLEAN ("Draw ticks", 1)
	BOOLEAN ("Draw dotted lines", 1)
}
static int do_marks (Any dia, void (*Graphics_marks) (void *, int, int, int, int)) {
	long numberOfMarks = GET_INTEGER ("Number of marks");
	REQUIRE (numberOfMarks >= 2, "`Number of marks' must be at least 2.")
	praat_picture_open ();
	Graphics_marks (GRAPHICS, numberOfMarks, GET_INTEGER ("Write numbers"),
		GET_INTEGER ("Draw ticks"), GET_INTEGER ("Draw dotted lines"));
	praat_picture_close ();
	return 1;
}
FORM (Marks_left, "Praat picture: Marks left", "Marks left/right/top/bottom...")
	dia_marks (dia); OK DO if (! do_marks (dia, Graphics_marksLeft)) return 0; END
FORM (Marks_right, "Praat picture: Marks right", "Marks left/right/top/bottom...")
	dia_marks (dia); OK DO if (! do_marks (dia, Graphics_marksRight)) return 0; END
FORM (Marks_bottom, "Praat picture: Marks bottom", "Marks left/right/top/bottom...")
	dia_marks (dia); OK DO if (! do_marks (dia, Graphics_marksBottom)) return 0; END
FORM (Marks_top, "Praat picture: Marks top", "Marks left/right/top/bottom...")
	dia_marks (dia); OK DO if (! do_marks (dia, Graphics_marksTop)) return 0; END

static void dia_marksLogarithmic (Any dia) {
	NATURAL ("Marks per decade", "3")
	BOOLEAN ("Write numbers", 1)
	BOOLEAN ("Draw ticks", 1)
	BOOLEAN ("Draw dotted lines", 1)
}
static void do_marksLogarithmic (Any dia, void (*Graphics_marksLogarithmic) (void *, int, int, int, int)) {
	long numberOfMarksPerDecade = GET_INTEGER ("Marks per decade");
	praat_picture_open ();
	Graphics_marksLogarithmic (GRAPHICS, numberOfMarksPerDecade, GET_INTEGER ("Write numbers"),
		GET_INTEGER ("Draw ticks"), GET_INTEGER ("Draw dotted lines"));
	praat_picture_close ();
}
FORM (marksLeftLogarithmic, "Praat picture: Logarithmic marks left", "Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksLeftLogarithmic); END
FORM (marksRightLogarithmic, "Praat picture: Logarithmic marks right", "Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksRightLogarithmic); END
FORM (marksBottomLogarithmic, "Praat picture: Logarithmic marks bottom", "Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksBottomLogarithmic); END
FORM (marksTopLogarithmic, "Praat picture: Logarithmic marks top", "Logarithmic marks left/right/top/bottom...")
	dia_marksLogarithmic (dia); OK DO do_marksLogarithmic (dia, Graphics_marksTopLogarithmic); END

static void sortBoundingBox (double *x1WC, double *x2WC, double *y1WC, double *y2WC) {
	double temp;
	if (*x1WC > *x2WC) temp = *x1WC, *x1WC = *x2WC, *x2WC = temp;
	if (*y1WC > *y2WC) temp = *y1WC, *y1WC = *y2WC, *y2WC = temp;
}

static void dia_oneMark (Any dia) {
	REAL ("Position", "0.0")
	BOOLEAN ("Write number", 1)
	BOOLEAN ("Draw tick", 1)
	BOOLEAN ("Draw dotted line", 1)
	LABEL ("", "Draw text:")
	TEXTFIELD ("text", "")
}
FORM (Mark_left, "Praat picture: One mark left", "One mark left/right/top/bottom...")
	dia_oneMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.01 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy) return Melder_error (
		"`Position' must be between %.15g and %.15g.", y1WC, y2WC);
	praat_picture_open ();
	Graphics_markLeft (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Mark_right, "Praat picture: One mark right", "One mark left/right/top/bottom...")
	dia_oneMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.01 * (y2WC - y1WC);
	if (position < y1WC - dy || position > y2WC + dy) return Melder_error (
		"`Position' must be between %.15g and %.15g.", y1WC, y2WC);
	praat_picture_open ();
	Graphics_markRight (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Mark_top, "Praat picture: One mark top", "One mark left/right/top/bottom...")
	dia_oneMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.01 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx) return Melder_error (
		"`Position' must be between %.15g and %.15g.", x1WC, x2WC);
	praat_picture_open ();
	Graphics_markTop (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (Mark_bottom, "Praat picture: One mark bottom", "One mark left/right/top/bottom...")
	dia_oneMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.01 * (x2WC - x1WC);
	if (position < x1WC - dx || position > x2WC + dx) return Melder_error (
		"`Position' must be between %.15g and %.15g.", x1WC, x2WC);
	praat_picture_open ();
	Graphics_markBottom (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

static void dia_oneLogarithmicMark (Any dia) {
	REAL ("Position", "1.0")
	BOOLEAN ("Write number", 1)
	BOOLEAN ("Draw tick", 1)
	BOOLEAN ("Draw dotted line", 1)
	LABEL ("", "Draw text:")
	TEXTFIELD ("text", "")
}
FORM (LogarithmicMark_left, "Praat picture: One logarithmic mark left", "One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.01 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy)) return Melder_error (
		"`Position' must be between %.15g and %.15g.", pow (10, y1WC), pow (10, y2WC));
	praat_picture_open ();
	Graphics_markLeftLogarithmic (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (LogarithmicMark_right, "Praat picture: One logarithmic mark right", "One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dy;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dy = 0.01 * (y2WC - y1WC);
	if (position < pow (10, y1WC - dy) || position > pow (10, y2WC + dy)) return Melder_error (
		"`Position' must be between %.15g and %.15g.", pow (10, y1WC), pow (10, y2WC));
	praat_picture_open ();
	Graphics_markRightLogarithmic (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (LogarithmicMark_top, "Praat picture: One logarithmic mark top", "One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.01 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx)) return Melder_error (
		"`Position' must be between %.15g and %.15g.", pow (10, x1WC), pow (10, x2WC));
	praat_picture_open ();
	Graphics_markTopLogarithmic (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (LogarithmicMark_bottom, "Praat picture: One logarithmic mark bottom", "One logarithmic mark left/right/top/bottom...")
	dia_oneLogarithmicMark (dia);
	OK
DO
	double position = GET_REAL ("Position");
	double x1WC, x2WC, y1WC, y2WC, dx;
	praat_picture_open ();
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	praat_picture_close ();
	sortBoundingBox (& x1WC, & x2WC, & y1WC, & y2WC);
	dx = 0.01 * (x2WC - x1WC);
	if (position < pow (10, x1WC - dx) || position > pow (10, x2WC + dx)) return Melder_error (
		"`Position' must be between %.15g and %.15g.", pow (10, x1WC), pow (10, x2WC));
	praat_picture_open ();
	Graphics_markBottomLogarithmic (GRAPHICS, position, GET_INTEGER ("Write number"),
		GET_INTEGER ("Draw tick"), GET_INTEGER ("Draw dotted line"),
		GET_STRING ("text"));
	praat_picture_close ();
END

FORM (dxMMtoWC, "Compute horizontal distance in world coordinates", 0)
	REAL ("Distance (mm)", "10.0")
	OK
DO
	double wc;
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	wc = Graphics_dxMMtoWC (GRAPHICS, GET_REAL ("Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g", wc);
END

FORM (dxWCtoMM, "Compute horizontal distance in millimetres", 0)
	REAL ("Distance (wc)", "0.1")
	OK
DO
	double mm;
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	mm = Graphics_dxWCtoMM (GRAPHICS, GET_REAL ("Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g mm", mm);
END

FORM (dyMMtoWC, "Compute vertical distance in world coordinates", 0)
	REAL ("Distance (mm)", "10.0")
	OK
DO
	double wc;
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	wc = Graphics_dyMMtoWC (GRAPHICS, GET_REAL ("Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g", wc);
END

FORM (dyWCtoMM, "Compute vertical distance in millimetres", 0)
	REAL ("Distance (wc)", "1.0")
	OK
DO
	double mm;
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	mm = Graphics_dyWCtoMM (GRAPHICS, GET_REAL ("Distance"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g mm", mm);
END

FORM (textWidth_wc, "Text width in world coordinates", 0)
	TEXTFIELD ("text", "Hello world")
	OK
DO
	double wc;
	Graphics_setFont (GRAPHICS, praat_font);
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	wc = Graphics_textWidth (GRAPHICS, GET_STRING ("text"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g", wc);
END

FORM (textWidth_mm, "Text width in millimetres", 0)
	TEXTFIELD ("text", "Hello world")
	OK
DO
	double mm;
	Graphics_setFont (GRAPHICS, praat_font);
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	mm = Graphics_dxWCtoMM (GRAPHICS, Graphics_textWidth (GRAPHICS, GET_STRING ("text")));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g mm", mm);
END

FORM (textWidth_ps_wc, "PostScript text width in world coordinates", 0)
	TEXTFIELD ("text", "Hello world")
	OK
DO
	double wc;
	Graphics_setFont (GRAPHICS, praat_font);
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	wc = Graphics_textWidth_ps (GRAPHICS, GET_STRING ("text"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g", wc);
END

FORM (textWidth_ps_mm, "PostScript text width in millimetres", 0)
	TEXTFIELD ("text", "Hello world")
	OK
DO
	double mm;
	Graphics_setFont (GRAPHICS, praat_font);
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	Graphics_setInner (GRAPHICS);
	mm = Graphics_textWidth_ps_mm (GRAPHICS, GET_STRING ("text"));
	Graphics_unsetInner (GRAPHICS);
	Melder_information ("%.17g mm", mm);
END

DIRECT (SearchManual) Melder_search (); END
DIRECT (PictureWindowHelp) Melder_help ("Picture window"); END
DIRECT (AboutSpecialSymbols) Melder_help ("Special symbols"); END
DIRECT (AboutTextStyles) Melder_help ("Text styles"); END
DIRECT (PhoneticSymbols) Melder_help ("Phonetic symbols"); END

/**********   **********/

static void cb_selectionChanged (Picture p, XtPointer closure,
	double selx1, double selx2, double sely1, double sely2)
	/* The user selected a new viewport in the picture window. */
{
	(void) closure;
	Melder_assert (p == praat_picture);
	x1NDC = selx1;
	x2NDC = selx2;
	y1NDC = sely1;
	y2NDC = sely2;
	if (praat_mouseSelectsInnerViewport) {
		int fontSize = Graphics_inqFontSize (GRAPHICS);
		double xmargin = fontSize * 4.2 / 72.0, ymargin = fontSize * 2.8 / 72.0;
		if (ymargin > 0.4 * (y2NDC - y1NDC)) ymargin = 0.4 * (y2NDC - y1NDC);
		if (xmargin > 0.4 * (x2NDC - x1NDC)) xmargin = 0.4 * (x2NDC - x1NDC);
		UiHistory_write ("\nSelect inner viewport... %g %g %g %g", x1NDC + xmargin, x2NDC - xmargin, 12-y2NDC + ymargin, 12-y1NDC - ymargin);
	} else {
		UiHistory_write ("\nSelect outer viewport... %g %g %g %g", x1NDC, x2NDC, 12-y2NDC, 12-y1NDC);
	}
}

/***** Public functions. *****/

static Widget shell, fileMenu, editMenu, marginsMenu, worldMenu, selectMenu, fontMenu, penMenu, helpMenu;

Widget praat_picture_resolveMenu (const char *menu) {
	return
		strequ (menu, "File") ? fileMenu :
		strequ (menu, "Edit") ? editMenu :
		strequ (menu, "Margins") ? marginsMenu :
		strequ (menu, "World") ? worldMenu :
		strequ (menu, "Select") ? selectMenu :
		strequ (menu, "Font") ? fontMenu :
		strequ (menu, "Pen") ? penMenu :
		strequ (menu, "Help") ? helpMenu :
		editMenu;   /* Default. */
}

void praat_picture_exit (void) {
	Picture_remove (& praat_picture);
}

void praat_picture_open (void) {
	double x1WC, x2WC, y1WC, y2WC;
	if (praatP.inManual) return;
	Graphics_markGroup (GRAPHICS);   /* We start a group of graphics output here. */
	if (! praat.batch) {
		XtMapWidget (shell);
		XMapRaised (XtDisplay (shell), XtWindow (shell)); 
		Picture_unhighlight (praat_picture);
	}
	/* Foregoing drawing routines may have changed some of the output attributes */
	/* that can be set by the user. */
	/* Make sure that they have the right values before every drawing. */
	/* This is especially necessary after an 'erase picture': */
	/* the output attributes that were set by the user before the 'erase' */
	/* must be recorded before copying to a PostScript file. */
	Graphics_setFont (GRAPHICS, praat_font);
	Graphics_setFontSize (GRAPHICS, praat_size);
	Graphics_setLineType (GRAPHICS, praat_lineType);
	Graphics_setLineWidth (GRAPHICS, praat_lineWidth);
	Graphics_setColour (GRAPHICS, praat_colour);
	Graphics_setViewport (GRAPHICS, x1NDC, x2NDC, y1NDC, y2NDC);
	/* The following will dump the axes to the PostScript file after Erase all. BUG: should be somewhere else. */
	Graphics_inqWindow (GRAPHICS, & x1WC, & x2WC, & y1WC, & y2WC);
	Graphics_setWindow (GRAPHICS, x1WC, x2WC, y1WC, y2WC);
}

void praat_picture_close (void) {
	if (praatP.inManual) return;
	if (! praat.batch) Picture_highlight (praat_picture);
}

void praat_picture_init (void) {
	Widget dialog, scrollWindow, menuBar, drawingArea = NULL;
	int margin, width, height, resolution, x;
	static char itemTitle_search [100];
	if (! praat.batch) {
		char pictureWindowTitle [100];
		int screenWidth = WidthOfScreen (DefaultScreenOfDisplay (XtDisplay (praat.topShell)));
		resolution = motif_getResolution (praat.topShell);
		#if defined (macintosh)
			margin = 2, width = 6 * resolution + 20;
			height = 9 * resolution + Machine_getMenuBarHeight () + 24;
			x = screenWidth - width - 14;
			width += margin * 2;
		#elif defined (_WIN32)
			margin = 2, width = 6 * resolution + 22;
			height = 9 * resolution + 24;
			x = screenWidth - width - 17;
		#else
			margin = 0, width = 6 * resolution + 30;
			height = width * 3 / 2 + Machine_getTitleBarHeight ();
			x = screenWidth - width - 10;
			width += margin * 2;
		#endif
		shell = motif_addShell (praat.topShell, 0);
		sprintf (pictureWindowTitle, "%s picture", praatP.title);
		XtVaSetValues (shell, XmNwidth, width, XmNheight, height,
			XmNdeleteResponse, XmUNMAP, XmNiconName, "Picture", XmNtitle, pictureWindowTitle,
			XmNx, x, NULL);
		dialog = XtVaCreateWidget ("picture", xmFormWidgetClass, shell,
			XmNautoUnmanage, False, XmNdialogStyle, XmDIALOG_MODELESS, 0);
		#ifdef UNIX
			XtVaSetValues (dialog, XmNhighlightThickness, 1, 0);
		#endif
		menuBar = motif_addMenuBar (dialog);
	}
	if (! praat.batch) {
		fileMenu = motif_addMenu (menuBar, "File", 0);
		editMenu = motif_addMenu (menuBar, "Edit", 0);
		marginsMenu = motif_addMenu (menuBar, "Margins", 0);
		worldMenu = motif_addMenu (menuBar, "World", 0);
		selectMenu = motif_addMenu (menuBar, "Select", 0);
		penMenu = motif_addMenu (menuBar, "Pen", 0);
		fontMenu = motif_addMenu (menuBar, "Font", 0);
		helpMenu = motif_addMenu (menuBar, "Help", 0);
	}

	praat_addMenuCommand ("Picture", "File", "Read from praat picture file...", 0, 0, DO_Picture_readFromPraatPictureFile);
	praat_addMenuCommand ("Picture", "File", "Read from old praat picture file...", 0, praat_HIDDEN, DO_Picture_readFromOldPraatPictureFile);
	#ifdef _WIN32
	praat_addMenuCommand ("Picture", "File", "Read from old Windows praat picture file...", 0, praat_HIDDEN, DO_Picture_readFromOldWindowsPraatPictureFile);
	#endif
	praat_addMenuCommand ("Picture", "File", "Write to praat picture file...", 0, 0, DO_Picture_writeToPraatPictureFile);
	#ifdef macintosh
	praat_addMenuCommand ("Picture", "File", "Write to Mac PICT file...", 0, 0, DO_Picture_writeToMacPictFile);
	praat_addMenuCommand ("Picture", "File", "Copy to clipboard", 0, 'C', DO_Copy_picture_to_clipboard);
	praat_addMenuCommand ("Picture", "File", "Copy screen image to clipboard", 0, 0, DO_Copy_screen_image_to_clipboard);
	#endif
	#ifdef _WIN32
	praat_addMenuCommand ("Picture", "File", "Write to Windows metafile...", 0, 0, DO_Picture_writeToWindowsMetafile);
	praat_addMenuCommand ("Picture", "File", "Copy to clipboard", 0, 'C', DO_Copy_picture_to_clipboard);
	#endif
	praat_addMenuCommand ("Picture", "File", "-- print --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "File", "PostScript settings...", 0, 0, DO_PostScript_settings);
	praat_addMenuCommand ("Picture", "File", "Write to EPS file...", 0, 'S', DO_Picture_writeToEpsFile);
	praat_addMenuCommand ("Picture", "File", "Write to fontless EPS file...", 0, 0, DO_Picture_writeToFontlessEpsFile);
	#if defined (macintosh)
		praat_addMenuCommand ("Picture", "File", "Page setup...", 0, 0, DO_Page_setup);
	#endif
	praat_addMenuCommand ("Picture", "File", "Print...", 0, 'P', DO_Print);

	praat_addMenuCommand ("Picture", "Edit", "Undo", 0, 'Z', DO_Undo);
	praat_addMenuCommand ("Picture", "Edit", "-- erase --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Edit", "Erase all", 0, 0, DO_Erase_all);

	praat_addMenuCommand ("Picture", "Margins", "Draw inner box", 0, 0, DO_DrawInnerBox);
	praat_addMenuCommand ("Picture", "Margins", "-- text --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Text left...", 0, 0, DO_Text_left);
	praat_addMenuCommand ("Picture", "Margins", "Text right...", 0, 0, DO_Text_right);
	praat_addMenuCommand ("Picture", "Margins", "Text top...", 0, 0, DO_Text_top);
	praat_addMenuCommand ("Picture", "Margins", "Text bottom...", 0, 0, DO_Text_bottom);
	praat_addMenuCommand ("Picture", "Margins", "-- marks every --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Marks left every...", 0, 0, DO_Marks_left_every);
	praat_addMenuCommand ("Picture", "Margins", "Marks right every...", 0, 0, DO_Marks_right_every);
	praat_addMenuCommand ("Picture", "Margins", "Marks bottom every...", 0, 0, DO_Marks_bottom_every);
	praat_addMenuCommand ("Picture", "Margins", "Marks top every...", 0, 0, DO_Marks_top_every);
	praat_addMenuCommand ("Picture", "Margins", "-- one mark --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "One mark left...", 0, 0, DO_Mark_left);
	praat_addMenuCommand ("Picture", "Margins", "One mark right...", 0, 0, DO_Mark_right);
	praat_addMenuCommand ("Picture", "Margins", "One mark bottom...", 0, 0, DO_Mark_bottom);
	praat_addMenuCommand ("Picture", "Margins", "One mark top...", 0, 0, DO_Mark_top);
	praat_addMenuCommand ("Picture", "Margins", "-- marks --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Marks", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Marks left...", 0, 1, DO_Marks_left);
	praat_addMenuCommand ("Picture", "Margins", "Marks right...", 0, 1, DO_Marks_right);
	praat_addMenuCommand ("Picture", "Margins", "Marks bottom...", 0, 1, DO_Marks_bottom);
	praat_addMenuCommand ("Picture", "Margins", "Marks top...", 0, 1, DO_Marks_top);
	praat_addMenuCommand ("Picture", "Margins", "Logarithmic marks", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Logarithmic marks left...", 0, 1, DO_marksLeftLogarithmic);
	praat_addMenuCommand ("Picture", "Margins", "Logarithmic marks right...", 0, 1, DO_marksRightLogarithmic);
	praat_addMenuCommand ("Picture", "Margins", "Logarithmic marks bottom...", 0, 1, DO_marksBottomLogarithmic);
	praat_addMenuCommand ("Picture", "Margins", "Logarithmic marks top...", 0, 1, DO_marksTopLogarithmic);
	praat_addMenuCommand ("Picture", "Margins", "One logarithmic mark", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "One logarithmic mark left...", 0, 1, DO_LogarithmicMark_left);
	praat_addMenuCommand ("Picture", "Margins", "One logarithmic mark right...", 0, 1, DO_LogarithmicMark_right);
	praat_addMenuCommand ("Picture", "Margins", "One logarithmic mark bottom...", 0, 1, DO_LogarithmicMark_bottom);
	praat_addMenuCommand ("Picture", "Margins", "One logarithmic mark top...", 0, 1, DO_LogarithmicMark_top);
	praat_addMenuCommand ("Picture", "Margins", "-- axes --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Margins", "Axes...", 0, 0, DO_Axes);

	praat_addMenuCommand ("Picture", "World", "Text...", 0, 0, DO_Text);
	praat_addMenuCommand ("Picture", "World", "Text special...", 0, 0, DO_Text_special);
	praat_addMenuCommand ("Picture", "World", "-- line --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Draw line...", 0, 0, DO_DrawLine);
	praat_addMenuCommand ("Picture", "World", "Draw arrow...", 0, 0, DO_DrawArrow);
	praat_addMenuCommand ("Picture", "World", "Draw two-way arrow...", 0, 0, DO_DrawDoubleArrow);
	praat_addMenuCommand ("Picture", "World", "-- function --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Draw function...", 0, 0, DO_DrawFunction);
	praat_addMenuCommand ("Picture", "World", "-- rectangle --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Draw rectangle...", 0, 0, DO_DrawRectangle);
	praat_addMenuCommand ("Picture", "World", "Paint rectangle...", 0, 0, DO_PaintRectangle);
	praat_addMenuCommand ("Picture", "World", "Draw rounded rectangle...", 0, 0, DO_DrawRoundedRectangle);
	praat_addMenuCommand ("Picture", "World", "Paint rounded rectangle...", 0, 0, DO_PaintRoundedRectangle);
	praat_addMenuCommand ("Picture", "World", "-- arc --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Draw arc...", 0, 0, DO_DrawArc);
	praat_addMenuCommand ("Picture", "World", "Draw ellipse...", 0, 0, DO_DrawEllipse);
	praat_addMenuCommand ("Picture", "World", "Paint ellipse...", 0, 0, DO_PaintEllipse);
	praat_addMenuCommand ("Picture", "World", "Draw circle...", 0, 0, DO_DrawCircle);
	praat_addMenuCommand ("Picture", "World", "Paint circle...", 0, 0, DO_PaintCircle);
	praat_addMenuCommand ("Picture", "World", "Draw circle (mm)...", 0, 0, DO_DrawCircle_mm);
	praat_addMenuCommand ("Picture", "World", "Paint circle (mm)...", 0, 0, DO_PaintCircle_mm);
	praat_addMenuCommand ("Picture", "World", "-- axes --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Axes...", 0, 0, DO_Axes);
	praat_addMenuCommand ("Picture", "World", "Measure", 0, 0, 0);
	praat_addMenuCommand ("Picture", "World", "Horizontal mm to wc...", 0, 1, DO_dxMMtoWC);
	praat_addMenuCommand ("Picture", "World", "Horizontal wc to mm...", 0, 1, DO_dxWCtoMM);
	praat_addMenuCommand ("Picture", "World", "Vertical mm to wc...", 0, 1, DO_dyMMtoWC);
	praat_addMenuCommand ("Picture", "World", "Vertical wc to mm...", 0, 1, DO_dyWCtoMM);
	praat_addMenuCommand ("Picture", "World", "-- text measure --", 0, 1, 0);
	praat_addMenuCommand ("Picture", "World", "Text width (wc)...", 0, 1, DO_textWidth_wc);
	praat_addMenuCommand ("Picture", "World", "Text width (mm)...", 0, 1, DO_textWidth_mm);
	praat_addMenuCommand ("Picture", "World", "PostScript text width (wc)...", 0, 1, DO_textWidth_ps_wc);
	praat_addMenuCommand ("Picture", "World", "PostScript text width (mm)...", 0, 1, DO_textWidth_ps_mm);

	praatButton_innerViewport = praat_addMenuCommand ("Picture", "Select", "Mouse selects inner viewport", 0, praat_CHECKABLE, DO_MouseSelectsInnerViewport);
	praatButton_outerViewport = praat_addMenuCommand ("Picture", "Select", "Mouse selects outer viewport", 0, praat_CHECKABLE, DO_MouseSelectsOuterViewport);
	praat_addMenuCommand ("Picture", "Select", "-- select --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Select", "Select inner viewport...", 0, 0, DO_SelectInnerViewport);
	praat_addMenuCommand ("Picture", "Select", "Select outer viewport...", 0, 0, DO_SelectOuterViewport);
	praat_addMenuCommand ("Picture", "Select", "Viewport...", 0, praat_HIDDEN, DO_SelectOuterViewport);
	praat_addMenuCommand ("Picture", "Select", "-- viewport drawing --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Select", "Viewport text...", 0, 0, DO_ViewportText);

	praatButton_plainLine = praat_addMenuCommand ("Picture", "Pen", "Plain line", 0, praat_CHECKABLE, DO_Plain_line);
	praatButton_dottedLine = praat_addMenuCommand ("Picture", "Pen", "Dotted line", 0, praat_CHECKABLE, DO_Dotted_line);
	praatButton_dashedLine = praat_addMenuCommand ("Picture", "Pen", "Dashed line", 0, praat_CHECKABLE, DO_Dashed_line);
	praat_addMenuCommand ("Picture", "Pen", "-- line width --", 0, 0, 0);
	praat_addMenuCommand ("Picture", "Pen", "Line width...", 0, 0, DO_Line_width);
	praat_addMenuCommand ("Picture", "Pen", "-- colour --", 0, 0, 0);
	praatButton_black = praat_addMenuCommand ("Picture", "Pen", "Black", 0, praat_CHECKABLE, DO_Black);
	praatButton_white = praat_addMenuCommand ("Picture", "Pen", "White", 0, praat_CHECKABLE, DO_White);
	praatButton_red = praat_addMenuCommand ("Picture", "Pen", "Red", 0, praat_CHECKABLE, DO_Red);
	praatButton_green = praat_addMenuCommand ("Picture", "Pen", "Green", 0, praat_CHECKABLE, DO_Green);
	praatButton_blue = praat_addMenuCommand ("Picture", "Pen", "Blue", 0, praat_CHECKABLE, DO_Blue);
	praatButton_yellow = praat_addMenuCommand ("Picture", "Pen", "Yellow", 0, praat_CHECKABLE, DO_Yellow);
	praatButton_cyan = praat_addMenuCommand ("Picture", "Pen", "Cyan", 0, praat_CHECKABLE, DO_Cyan);
	praatButton_magenta = praat_addMenuCommand ("Picture", "Pen", "Magenta", 0, praat_CHECKABLE, DO_Magenta);
	praatButton_maroon = praat_addMenuCommand ("Picture", "Pen", "Maroon", 0, praat_CHECKABLE, DO_Maroon);
	praatButton_lime = praat_addMenuCommand ("Picture", "Pen", "Lime", 0, praat_CHECKABLE, DO_Lime);
	praatButton_navy = praat_addMenuCommand ("Picture", "Pen", "Navy", 0, praat_CHECKABLE, DO_Navy);
	praatButton_teal = praat_addMenuCommand ("Picture", "Pen", "Teal", 0, praat_CHECKABLE, DO_Teal);
	praatButton_purple = praat_addMenuCommand ("Picture", "Pen", "Purple", 0, praat_CHECKABLE, DO_Purple);
	praatButton_olive = praat_addMenuCommand ("Picture", "Pen", "Olive", 0, praat_CHECKABLE, DO_Olive);
	praatButton_silver = praat_addMenuCommand ("Picture", "Pen", "Silver", 0, praat_CHECKABLE, DO_Silver);
	praatButton_grey = praat_addMenuCommand ("Picture", "Pen", "Grey", 0, praat_CHECKABLE, DO_Grey);

	praatButton_10 = praat_addMenuCommand ("Picture", "Font", "10", 0, praat_CHECKABLE, DO_10);
	praatButton_12 = praat_addMenuCommand ("Picture", "Font", "12", 0, praat_CHECKABLE,  DO_12);
	praatButton_14 = praat_addMenuCommand ("Picture", "Font", "14", 0, praat_CHECKABLE, DO_14);
	praatButton_18 = praat_addMenuCommand ("Picture", "Font", "18", 0, praat_CHECKABLE, DO_18);
	praatButton_24 = praat_addMenuCommand ("Picture", "Font", "24", 0, praat_CHECKABLE, DO_24);
	praat_addMenuCommand ("Picture", "Font", "Font size...", 0, 0, DO_Font_size);
	praat_addMenuCommand ("Picture", "Font", "-- font ---", 0, 0, 0);
	praatButton_times = praat_addMenuCommand ("Picture", "Font", "Times", 0, praat_CHECKABLE, DO_Times);
	praatButton_helvetica = praat_addMenuCommand ("Picture", "Font", "Helvetica", 0, praat_CHECKABLE, DO_Helvetica);
	praatButton_newCenturySchoolbook = praat_addMenuCommand ("Picture", "Font", "New Century Schoolbook", 0, praat_CHECKABLE, DO_New_Century_Schoolbook);
	praatButton_palatino = praat_addMenuCommand ("Picture", "Font", "Palatino", 0, praat_CHECKABLE, DO_Palatino);
	praatButton_courier = praat_addMenuCommand ("Picture", "Font", "Courier", 0, praat_CHECKABLE, DO_Courier);

	praat_addMenuCommand ("Picture", "Help", "Picture window help", 0, '?', DO_PictureWindowHelp);
	praat_addMenuCommand ("Picture", "Help", "About special symbols", 0, 0, DO_AboutSpecialSymbols);
	praat_addMenuCommand ("Picture", "Help", "About text styles", 0, 0, DO_AboutTextStyles);
	praat_addMenuCommand ("Picture", "Help", "Phonetic symbols", 0, 0, DO_PhoneticSymbols);
	praat_addMenuCommand ("Picture", "Help", "-- manual --", 0, 0, 0);
	sprintf (itemTitle_search, "Search %s manual...", praatP.title);
	praat_addMenuCommand ("Picture", "Help", itemTitle_search, 0, 'M', DO_SearchManual);

	if (! praat.batch) {
		XtManageChild (menuBar);
		#if defined (macintosh) || defined (_WIN32)
			scrollWindow = XmCreateScrolledWindow (dialog, "scrolledWindow", NULL, 0);
			XtVaSetValues (scrollWindow,
				XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, margin,
				XmNrightAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight () + margin,
				XmNbottomAttachment, XmATTACH_FORM, 0);
			drawingArea = XmCreateDrawingArea (scrollWindow, "drawingArea", NULL, 0);
			width = height = resolution * 12;
			XtVaSetValues (drawingArea,
				XmNwidth, width, XmNheight, height,
				XmNmarginWidth, 0, XmNmarginHeight, 0, 0);
			XtManageChild (drawingArea);
		#else
			#if ! defined(sun4)
			scrollWindow = XtVaCreateWidget (
				"scrolledWindow", xmScrolledWindowWidgetClass, dialog,
				XmNscrollingPolicy, XmAUTOMATIC, XmNrightAttachment, XmATTACH_FORM,
				XmNbottomAttachment, XmATTACH_FORM, XmNleftAttachment, XmATTACH_FORM,
				XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (), 0);
			drawingArea = XmCreateDrawingArea (scrollWindow, "drawingArea", NULL, 0);
			#else
			drawingArea = XmCreateDrawingArea (dialog, "drawingArea", NULL, 0);
			#endif
			width = height = resolution * 12;
			XtVaSetValues (drawingArea,
				#if defined(sun4)
					XmNleftAttachment, XmATTACH_FORM,
					XmNrightAttachment, XmATTACH_FORM,
					XmNtopAttachment, XmATTACH_FORM, XmNtopOffset, Machine_getMenuBarHeight (),
					XmNbottomAttachment, XmATTACH_FORM,
				#endif
				XmNwidth, width, XmNheight, height,
				XmNmarginWidth, 0, XmNmarginHeight, 0, XmNborderWidth, 1,
				0);
			XtManageChild (drawingArea);
		#endif
		#if ! defined(sun4)
			XtManageChild (scrollWindow);
		#endif
		XtManageChild (dialog);
		XtRealizeWidget (shell);
	}
	praat_picture = Picture_create (drawingArea, ! praat.batch);
	Picture_setSelectionChangedCallback (praat_picture, cb_selectionChanged, NULL);
	updatePenMenu ();
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
	praat.graphics = Picture_getGraphics (praat_picture);
}

void praat_picture_prefs (void) {
	Resources_addInt ("Picture.font", & praat_font);
	Resources_addInt ("Picture.fontSize", & praat_size);
	Resources_addInt ("Picture.mouseSelectsInnerViewport", & praat_mouseSelectsInnerViewport);
}

void praat_picture_prefsChanged (void) {
	updateFontMenu ();
	updateSizeMenu ();
	updateViewportMenu ();
	Graphics_setFontSize (praat.graphics, praat_size);   /* So that the thickness of the selection rectangle is correct. */
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

/* End of file praat_picture.c */
