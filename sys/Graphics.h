#ifndef _Graphics_h_
#define _Graphics_h_
/* Graphics.h
 *
 * Copyright (C) 1992-2005,2007-2020 Paul Boersma
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

#include "Thing.h"
#include "../kar/longchar.h"

#include "Graphics_enums.h"

typedef struct { double x1NDC, x2NDC, y1NDC, y2NDC; } Graphics_Viewport;

Thing_declare (GuiDrawingArea);

typedef struct {
	unsigned char link, rightToLeft;
	short style, size, baseline;
	double size_real;
	uint32 code;
	char32 kar;
	Longchar_Info karInfo;
	double width;
	union { integer integer_; const char *string; void *pointer; } font;
	int cell, line, run;
} _Graphics_widechar;

Thing_define (Graphics, Thing) {
	/* Device constants. */
	bool screen;
		/* A boolean for whether we are a graphic screen (which may include a non-PostScript printer. */
	bool postScript;
		/* A boolean for whether we are a PostScript device. */
	bool printer;
		/* A boolean for whether we are a printer. */
	bool metafile;
		/* A boolean for whether we are a high-resolution metafile or clipboard. */
	bool yIsZeroAtTheTop;
		/* A boolean for whether vertical cooordinates increase from top to bottom (as on most screens, but not PostScript). */
	GuiDrawingArea d_drawingArea;
		/* Also used as a boolean. */
	int resolution;
		/* Dots per inch. */
	enum kGraphics_resolution resolutionNumber;
	integer d_x1DCmin, d_x2DCmax, d_y1DCmin, d_y2DCmax;
		/* Maximum dimensions of the output device. */
		/* x1DCmin < x2DCmax; y1DCmin < y2DCmax; */
		/* The point (x1DCmin, y1DCmin) can be either in the top left */
		/* or in the bottom left, depending on the yIsZeroAtTheTop flag. */
		/* Device variables. */
	integer d_x1DC, d_x2DC, d_y1DC, d_y2DC;
		/* Current dimensions of the output device, or: */
		/* device coordinates of the viewport rectangle. */
		/* x1DCmin <= x1DC < x2DC <= x2DCmax; */
		/* y1DCmin <= y1DC < y2DC <= y2DCmax; */
		/* Graphics_create_xxxxxx sets these coordinates to */
		/* x1DCmin, x2DCmax, y1DCmin, and y2DCmax. */
		/* They can be changed by Graphics_setWsViewport. */
	double d_x1wNDC, d_x2wNDC, d_y1wNDC, d_y2wNDC;
		/* Normalized device coordinates of */
		/* the device viewport rectangle. */
		/* The point (x1wNDC, y1wNDC) is always in the bottom left.	*/
		/* Graphics_create_xxxxxx sets these coordinates to */
		/* 0.0, 1.0, 0.0, and 1.0. */
		/* They can be changed by Graphics_setWsWindow. */
	double d_x1NDC, d_x2NDC, d_y1NDC, d_y2NDC;
		/* Normalized device coordinates of the user output viewport, */
		/* which is a part of the device viewport rectangle. */
		/* x1wNDC <= x1NDC < x2NDC <= x2wNDC; */
		/* y1wNDC <= y1NDC < y2NDC <= y2wNDC; */
		/* Graphics_create_xxxxxx sets these coordinates to */
		/* 0.0, 1.0, 0.0, and 1.0. */
		/* They can be changed by Graphics_setViewport. */
	double d_x1WC, d_x2WC, d_y1WC, d_y2WC;
		/* World coordinates of the user output viewport rectangle.	*/
		/* They bear a relation to the "real" world, */
		/* and are used in the drawing routines. */
		/* Graphics_create_xxxxxx sets these coordinates to */
		/* 0.0, 1.0, 0.0, and 1.0. */
		/* They can be changed by Graphics_setWindow. */
	double deltaX, deltaY, scaleX, scaleY;
		/* Current coordinate transformation. */
	/* Graphics state. */
	int lineType;
	MelderColour colour;
	double lineWidth, arrowSize, speckleSize;
	kGraphics_colourScale colourScale;
	int horizontalTextAlignment, verticalTextAlignment;
	double textRotation, wrapWidth, secondIndent, textX, textY;
	kGraphics_font font;
	double fontSize;
	int fontStyle;
	bool percentSignIsItalic, numberSignIsBold, circumflexIsSuperscript, underscoreIsSubscript;
	bool dollarSignIsCode, atSignIsLink;
	bool recording, duringXor;
	integer irecord, nrecord;
	double *record;
	Graphics_Viewport outerViewport;   // for Graphics_(un)setInner ()
	double horTick, vertTick;   // for Graphics_mark(s)XXX ()
	double paperWidth, paperHeight;

	void v_destroy () noexcept
		override;

	virtual void v_polyline (integer /* numberOfPoints */, double * /* xyDC */, bool /* close */) { }
	virtual void v_fillArea (integer /* numberOfPoints */, double * /* xyDC */) { }
	virtual void v_rectangle (double /* x1DC */, double /* x2DC */, double /* y1DC */, double /* y2DC */) { }
	virtual void v_fillRectangle (double /* x1DC */, double /* x2DC */, double /* y1DC */, double /* y2DC */) { }
	virtual void v_circle (double /* xDC */, double /* yDC */, double /* rDC */) { }
	virtual void v_ellipse (double /* x1DC */, double /* x2DC */, double /* y1DC */, double /* y2DC */) { }
	virtual void v_arc (double /* xDC */, double /* yDC */, double /* rDC */, double /* fromAngle */, double /* toAngle */) { }
	virtual void v_fillCircle (double /* xDC */, double /* yDC */, double /* rDC */) { }
	virtual void v_fillEllipse (double /* x1DC */, double /* x2DC */, double /* y1DC */, double /* y2DC */) { }
	virtual void v_button (double a_x1DC, double a_x2DC, double a_y1DC, double a_y2DC)
		{
			v_rectangle (a_x1DC, a_x2DC, a_y1DC, a_y2DC);   // the simplest implementation
		}
	virtual void v_roundedRectangle (double x1DC, double x2DC, double y1DC, double y2DC, double r);
	virtual void v_fillRoundedRectangle (double x1DC, double x2DC, double y1DC, double y2DC, double r);
	virtual void v_arrowHead (double xDC, double yDC, double angle);
	virtual void v_clearWs () { }
	virtual void v_updateWs () { }
};

autoGraphics Graphics_create (int resolution);
autoGraphics Graphics_create_postscriptjob (MelderFile file, int resolution, kGraphicsPostscript_spots spots,
	kGraphicsPostscript_paperSize paperSize, kGraphicsPostscript_orientation rotation, double magnification);
autoGraphics Graphics_create_epsfile (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
	double xmin, double xmax, double ymin, double ymax, bool includeFonts, bool useSilipaPS);
autoGraphics Graphics_create_pdffile (MelderFile file, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches);
autoGraphics Graphics_create_pdf (void *context, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches);
autoGraphics Graphics_create_pngfile (MelderFile file, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches);
autoGraphics Graphics_create_postscriptprinter (void);
autoGraphics Graphics_create_screenPrinter (void *display, void *window);
autoGraphics Graphics_create_screen (void *display, void *window, int resolution);
autoGraphics Graphics_create_xmdrawingarea (GuiDrawingArea drawingArea);

int Graphics_getResolution (Graphics me);

void Graphics_setWsViewport (Graphics me, integer x1DC, integer x2DC, integer y1DC, integer y2DC);
void Graphics_resetWsViewport (Graphics me, integer x1DC, integer x2DC, integer y1DC, integer y2DC);
void Graphics_setWsWindow (Graphics me, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
void Graphics_inqWsViewport (Graphics me, integer *x1DC, integer *x2DC, integer *y1DC, integer *y2DC);
void Graphics_inqWsWindow (Graphics me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_clearWs (Graphics me);
void Graphics_updateWs (Graphics me);
void Graphics_beginMovieFrame (Graphics me, MelderColour *colour);
void Graphics_endMovieFrame (Graphics me, double frameDuration);
void Graphics_DCtoWC (Graphics me, integer xDC, integer yDC, double *xWC, double *yWC);
void Graphics_WCtoDC (Graphics me, double xWC, double yWC, integer *xDC, integer *yDC);

void Graphics_setViewport (Graphics me, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
Graphics_Viewport Graphics_insetViewport (Graphics me, double x1rel, double x2rel, double y1rel, double y2rel);
void Graphics_resetViewport (Graphics me, Graphics_Viewport viewport);
void Graphics_setWindow (Graphics me, double x1, double x2, double y1, double y2);

void Graphics_polyline (Graphics me, integer numberOfPoints, const double *x, const double *y);
void Graphics_polyline_closed (Graphics me, integer numberOfPoints, const double *x, const double *y);

void Graphics_text (Graphics me, double xWC, double yWC, conststring32 txt);
template <typename... Args>
void Graphics_text (Graphics me, double xWC, double yWC, const MelderArg& first, Args... rest) {
	Graphics_text (me, xWC, yWC, Melder_cat (first, rest...));
}

void Graphics_textRect (Graphics me, double x1, double x2, double y1, double y2, conststring32 text /* cattable */);
double Graphics_textWidth       (Graphics me, conststring32 text /* cattable */);
double Graphics_textWidth_ps    (Graphics me, conststring32 text /* cattable */, bool useSilipaPS);
double Graphics_textWidth_ps_mm (Graphics me, conststring32 text /* cattable */, bool useSilipaPS);
void Graphics_fillArea (Graphics me, integer numberOfPoints, double const *x, double const *y);
void Graphics_cellArray (Graphics me, constMATVU const& z,
	double x1, double x2, double y1, double y2, double minimum, double maximum);
void Graphics_cellArray_colour (Graphics me, constmatrixview <MelderColour> const& z,
	double x1, double x2, double y1, double y2, double minimum, double maximum);
void Graphics_cellArray8 (Graphics me, constmatrixview <unsigned char> const& z,
	double x1, double x2, double y1, double y2, unsigned char minimum, unsigned char maximum);
void Graphics_image (Graphics me, constMATVU const& z,
	double x1, double x2, double y1, double y2, double minimum, double maximum);
void Graphics_image_colour (Graphics me, constmatrixview <MelderColour> const& z,
	double x1, double x2, double y1, double y2, double minimum, double maximum);
void Graphics_image8 (Graphics me, constmatrixview <unsigned char> const& z,
	double x1, double x2, double y1, double y2, uint8 minimum, uint8 maximum);
void Graphics_imageFromFile (Graphics me, conststring32 relativeFileName, double x1, double x2, double y1, double y2);
void Graphics_line (Graphics me, double x1, double y1, double x2, double y2);
void Graphics_rectangle (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_fillRectangle (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_roundedRectangle (Graphics me, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_fillRoundedRectangle (Graphics me, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_function (Graphics me, const double y [], integer ix1, integer ix2, double x1, double x2);   // y [ix1..ix2]
void Graphics_function16 (Graphics me, const int16 y [], integer stride, integer ix1, integer ix2, double x1, double x2);   // y [ix1..ix2] or y [ix1*2..ix2*2]
void Graphics_circle (Graphics me, double x, double y, double r);
void Graphics_fillCircle (Graphics me, double x, double y, double r);
void Graphics_circle_mm (Graphics me, double x, double y, double d);
void Graphics_fillCircle_mm (Graphics me, double x, double y, double d);
void Graphics_speckle (Graphics me, double x, double y);
void Graphics_rectangle_mm (Graphics me, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_fillRectangle_mm (Graphics me, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_arc (Graphics me, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_fillArc (Graphics me, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_ellipse (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_fillEllipse (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_arrow (Graphics me, double x1, double y1, double x2, double y2);
void Graphics_doubleArrow (Graphics me, double x1, double y1, double x2, double y2);
void Graphics_arcArrow (Graphics me, double x, double y, double r, double fromAngle, double toAngle, int arrowAtStart, int arrowAtEnd);
void Graphics_mark (Graphics me, double x, double y, double size_mm, conststring32 markString /* cattable */);
void Graphics_button (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_innerRectangle (Graphics me, double x1, double x2, double y1, double y2);

void Graphics_setColour (Graphics me, MelderColour colour);
void Graphics_setGrey (Graphics me, double grey);

void Graphics_xorOn (Graphics me, MelderColour colour);
void Graphics_xorOff (Graphics me);
void Graphics_highlight (Graphics me, double x1, double x2, double y1, double y2);
void Graphics_highlight2 (Graphics me, double x1, double x2, double y1, double y2,
	double innerX1, double innerX2, double innerY1, double innerY2);

#define Graphics_NOCHANGE  -1
#define Graphics_LEFT  kGraphics_horizontalAlignment::LEFT
#define Graphics_CENTRE  kGraphics_horizontalAlignment::CENTRE
#define Graphics_RIGHT  kGraphics_horizontalAlignment::RIGHT
#define Graphics_BOTTOM  0
#define Graphics_HALF  1
#define Graphics_TOP  2
#define Graphics_BASELINE  3
void Graphics_setTextAlignment (Graphics me, enum kGraphics_horizontalAlignment horizontal, int vertical);

void Graphics_setFont (Graphics me, enum kGraphics_font font);
void Graphics_setFontSize (Graphics me, double height);

#define Graphics_NORMAL  0
#define Graphics_BOLD  1
#define Graphics_ITALIC  2
#define Graphics_BOLD_ITALIC  3
#define Graphics_CODE  4
void Graphics_setFontStyle (Graphics me, int style);

void Graphics_setItalic (Graphics me, bool onoff);
void Graphics_setBold (Graphics me, bool onoff);
void Graphics_setCode (Graphics me, bool onoff);
void Graphics_setTextRotation (Graphics me, double angle);
void Graphics_setTextRotation_vector (Graphics me, double dx, double dy);
void Graphics_setWrapWidth (Graphics me, double wrapWidth);
void Graphics_setSecondIndent (Graphics me, double indent);
double Graphics_inqTextX (Graphics me);
double Graphics_inqTextY (Graphics me);
typedef struct { double x1, x2, y1, y2; char32 *name; } Graphics_Link;
int Graphics_getLinks (Graphics_Link **plinks);
void Graphics_setNumberSignIsBold (Graphics me, bool isBold);
void Graphics_setPercentSignIsItalic (Graphics me, bool isItalic);
void Graphics_setCircumflexIsSuperscript (Graphics me, bool isSuperscript);
void Graphics_setUnderscoreIsSubscript (Graphics me, bool isSubscript);
void Graphics_setDollarSignIsCode (Graphics me, bool isCode);
void Graphics_setAtSignIsLink (Graphics me, bool isLink);

#define Graphics_DRAWN  0
#define Graphics_DOTTED  1
#define Graphics_DASHED  2
#define Graphics_DASHED_DOTTED  3
void Graphics_setLineType (Graphics me, int lineType);

void Graphics_setLineWidth (Graphics me, double lineWidth);
void Graphics_setArrowSize (Graphics me, double arrowSize);
void Graphics_setSpeckleSize (Graphics me, double speckleSize);

void Graphics_setColourScale (Graphics me, enum kGraphics_colourScale colourScale);

void Graphics_inqViewport (Graphics me, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_inqWindow (Graphics me, double *x1WC, double *x2WC, double *y1WC, double *y2WC);
enum kGraphics_font Graphics_inqFont (Graphics me);
double Graphics_inqFontSize (Graphics me);
int Graphics_inqFontStyle (Graphics me);
int Graphics_inqLineType (Graphics me);
double Graphics_inqLineWidth (Graphics me);
double Graphics_inqArrowSize (Graphics me);
double Graphics_inqSpeckleSize (Graphics me);
MelderColour Graphics_inqColour (Graphics me);
enum kGraphics_colourScale Graphics_inqColourScale (Graphics me);

void Graphics_contour (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, double height);
void Graphics_altitude (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, int numberOfBorders, double borders []);
void Graphics_grey (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC, int numberOfBorders, double borders []);
#define Graphics_gray Graphics_grey
void Graphics_surface (Graphics me, constMATVU const& z,
	double x1WC, double x2WC, double y1WC, double y2WC,
	double minimum, double maximum, double elevation, double azimuth
);

void Graphics_setInner (Graphics me);
void Graphics_unsetInner (Graphics me);
void Graphics_drawInnerBox (Graphics me);
void Graphics_textLeft   (Graphics me, bool farr, conststring32 text /* cattable */);
void Graphics_textRight  (Graphics me, bool farr, conststring32 text /* cattable */);
void Graphics_textBottom (Graphics me, bool farr, conststring32 text /* cattable */);
void Graphics_textTop    (Graphics me, bool farr, conststring32 text /* cattable */);
void Graphics_marksLeft   (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRight  (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottom (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTop    (Graphics me, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksLeftLogarithmic   (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRightLogarithmic  (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottomLogarithmic (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTopLogarithmic    (Graphics me, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_markLeft   (Graphics me, double yWC, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markRight  (Graphics me, double yWC, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markBottom (Graphics me, double xWC, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markTop    (Graphics me, double xWC, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markLeftLogarithmic   (Graphics me, double y, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);   // y > 0
void Graphics_markRightLogarithmic  (Graphics me, double y, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markBottomLogarithmic (Graphics me, double x, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_markTopLogarithmic    (Graphics me, double x, bool hasNumber, bool hasTick, bool hasDottedLine, conststring32 text /* cattable */);
void Graphics_marksLeftEvery   (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRightEvery  (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottomEvery (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTopEvery    (Graphics me, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);

bool Graphics_startRecording (Graphics me);
bool Graphics_stopRecording (Graphics me);
void Graphics_clearRecording (Graphics me);
void Graphics_play (Graphics from, Graphics to);
void Graphics_writeRecordings (Graphics me, FILE *f);
void Graphics_readRecordings (Graphics me, FILE *f);
void Graphics_markGroup (Graphics me);
void Graphics_undoGroup (Graphics me);

double Graphics_dxMMtoWC (Graphics me, double dx_mm);
double Graphics_dyMMtoWC (Graphics me, double dy_mm);
double Graphics_distanceWCtoMM (Graphics me, double x1WC, double y1WC, double x2WC, double y2WC);
double Graphics_dxWCtoMM (Graphics me, double dxWC);
double Graphics_dyWCtoMM (Graphics me, double dyWC);

void Graphics_nextSheetOfPaper (Graphics me);

void Graphics_prefs ();

/* End of file Graphics.h */
#endif
