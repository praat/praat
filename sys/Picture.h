#ifndef _Picture_h_
#define _Picture_h_
/* Picture.h
 *
 * Copyright (C) 1992-2011,2012,2013,2014,2015 Paul Boersma
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

/* A self-recording picture inside a DrawingArea widget.
	All drawing is kept in a Graphics.
	If the picture is sensitive,
	the user can select the viewport by dragging the mouse across the drawing area.
	If the viewport is smaller than the entire drawing area, it is highlighted.
	Usage:
		You should put highlighting off during drawing.
		Do not use the workstation Graphics routines,
		like clearWs, flushWs, closeWs, updateWs, setWsViewport.
	Example:
		Picture p = Picture_create (myDrawingArea);
		Graphics g = Picture_peekGraphics (p);
		Picture_unhighlight (p);
		Graphics_setTextAlignment (g, Graphics_CENTRE, Graphics_HALF);
		Graphics_text (g, 0.5, 0.7, U"Hello");
		Graphics_text (g, 0.5, 0.6, U"there");
		Picture_highlight (p);
		... (event handling)
		Picture_unhighlight (p);
		Graphics_text (g, 0.5, 0.3, U"Goodbye");
		Picture_highlight (p);
		... (event handling)
		Picture_writeToEpsFile (p, U"HelloGoodbye.eps", false, false);
		Picture_print (p, GraphicsPostscript_FINE);
		Picture_remove (& p);
*/

#include "Gui.h"

Thing_define (Picture, Thing) {
	GuiDrawingArea drawingArea;
	autoGraphics graphics, selectionGraphics;
	bool sensitive;
	double selx1, selx2, sely1, sely2;   // selection in NDC co-ordinates
	void (*selectionChangedCallback) (Picture, void *, double, double, double, double);
	void *selectionChangedClosure;
	bool backgrounding, mouseSelectsInnerViewport;

	void v_destroy () noexcept
		override;
};

autoPicture Picture_create (GuiDrawingArea drawingArea, bool sensitive);
/*
	Function:
		create an empty self-recording picture inside 'drawingArea'.
	Precondition:
		'drawingArea' must have been realized.
	Postconditions:
		a Graphics has been created from Graphics_create_macintosh or Graphics_create_xwindow;
		the workstation viewport of this Graphics is [0, 1] x [0, 1];
		selection is [0, 1] x [0, 1] (NDC), which is invisible;
*/

Graphics Picture_peekGraphics (Picture me);
/*
	Function:
		return the Graphics object.
	Usage:
		send the graphics output that you want to be in the picture to this Graphics,
		bracketed by calls to Picture_startRecording and Picture_stopRecording.
*/

void Picture_unhighlight (Picture me);
/*
	Function:
		hide the viewport.
	Usage:
		call just before sending graphics output.
*/

void Picture_highlight (Picture me);
/*
	Function:
		visualize the viewport.
	Usage:
		call just after sending graphics output.
*/

void Picture_setSelectionChangedCallback (Picture me,
	void (*selectionChangedCallback) (Picture, void *closure,
			double x1NDC, double x2NDC, double y1NDC, double y2NDC),
	void *selectionChangedClosure);

void Picture_setMouseSelectsInnerViewport (Picture me, int mouseSelectsInnerViewport);

void Picture_erase (Picture me);   // clears the screen

void Picture_writeToPraatPictureFile (Picture me, MelderFile file);
void Picture_readFromPraatPictureFile (Picture me, MelderFile file);

void Picture_writeToEpsFile (Picture me, MelderFile file, bool includeFonts, bool useSilipaPS);
void Picture_writeToPdfFile (Picture me, MelderFile file);
void Picture_writeToPngFile_300 (Picture me, MelderFile file);
void Picture_writeToPngFile_600 (Picture me, MelderFile file);

void Picture_print (Picture me);
void Picture_printToPostScriptPrinter (Picture me, int spots, int paperSize, int rotation, double magnification);
#ifdef macintosh
	void Picture_copyToClipboard (Picture me);
#endif
#ifdef _WIN32
	void Picture_copyToClipboard (Picture me);
	void Picture_writeToWindowsMetafile (Picture me, MelderFile file);
#endif

void Picture_setSelection
	(Picture me, double x1NDC, double x2NDC, double y1NDC, double y2NDC, bool notify);
/*
	Preconditions:
		0.0 <= x1NDC < x2NDC <= 1.0;
		0.0 <= y1NDC < y2NDC <= 1.0;
*/

void Picture_background (Picture me);
void Picture_foreground (Picture me);

/* End of file Picture.h */
#endif
