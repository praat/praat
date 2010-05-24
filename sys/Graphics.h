#ifndef _Graphics_h_
#define _Graphics_h_
/* Graphics.h
 *
 * Copyright (C) 1992-2009 Paul Boersma
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
 * pb 2009/12/20
 */

#ifndef _Thing_h_
	#include "Thing.h"
#endif

#include "Graphics_enums.h"

typedef struct structGraphics *Graphics;
Graphics Graphics_create (int resolution);
Graphics Graphics_create_postscriptjob (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
	enum kGraphicsPostscript_paperSize paperSize, enum kGraphicsPostscript_orientation rotation, double magnification);
Graphics Graphics_create_epsfile (MelderFile file, int resolution, enum kGraphicsPostscript_spots spots,
	double xmin, double xmax, double ymin, double ymax, bool includeFonts, bool useSilipaPS);
Graphics Graphics_create_pdffile (MelderFile file, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches);
Graphics Graphics_create_pdf (void *context, int resolution,
	double x1inches, double x2inches, double y1inches, double y2inches);
Graphics Graphics_create_postscriptprinter (void);
Graphics Graphics_create_screenPrinter (void *display, unsigned long window);
Graphics Graphics_create_screen (void *display, unsigned long window, int resolution);
#ifdef macintosh
	Graphics Graphics_create_port (void *display, unsigned long port, int resolution);
#endif
Graphics Graphics_create_xmdrawingarea (/* Widget */ void *drawingArea);

int Graphics_getResolution (I);

void Graphics_setWsViewport (I, short x1DC, short x2DC, short y1DC, short y2DC);
void Graphics_setWsWindow (I, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
void Graphics_inqWsViewport (I, short *x1DC, short *x2DC, short *y1DC, short *y2DC);
void Graphics_inqWsWindow (I, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_clearWs (I);
void Graphics_flushWs (I);
void Graphics_updateWs (I);
void Graphics_DCtoWC (I, short xDC, short yDC, double *xWC, double *yWC);
void Graphics_WCtoDC (I, double xWC, double yWC, short *xDC, short *yDC);

typedef struct { double x1NDC, x2NDC, y1NDC, y2NDC; } Graphics_Viewport;
void Graphics_setViewport (I, double x1NDC, double x2NDC, double y1NDC, double y2NDC);
Graphics_Viewport Graphics_insetViewport (I, double x1rel, double x2rel, double y1rel, double y2rel);
void Graphics_resetViewport (I, Graphics_Viewport viewport);
void Graphics_setWindow (I, double x1, double x2, double y1, double y2);

void Graphics_polyline (I, long numberOfPoints, double *x, double *y);
void Graphics_text (I, double x, double y, const wchar_t *text);
void Graphics_text1 (I, double x, double y, const wchar_t *s1);
void Graphics_text2 (I, double x, double y, const wchar_t *s1, const wchar_t *s2);
void Graphics_text3 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3);
void Graphics_text4 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4);
void Graphics_text5 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5);
void Graphics_text6 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6);
void Graphics_text7 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7);
void Graphics_text8 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8);
void Graphics_text9 (I, double x, double y, const wchar_t *s1, const wchar_t *s2, const wchar_t *s3, const wchar_t *s4, const wchar_t *s5, const wchar_t *s6, const wchar_t *s7, const wchar_t *s8, const wchar_t *s9);
void Graphics_textRect (I, double x1, double x2, double y1, double y2, const wchar_t *text);
double Graphics_textWidth (I, const wchar_t *text);
double Graphics_textWidth_ps (I, const wchar_t *text, bool useSilipaPS);
double Graphics_textWidth_ps_mm (I, const wchar_t *text, bool useSilipaPS);
void Graphics_fillArea (I, long numberOfPoints, double *x, double *y);
void Graphics_cellArray (I, double **z, long ix1, long ix2, double x1, double x2,
	long iy1, long iy2, double y1, double y2, double minimum, double maximum);
void Graphics_cellArray8 (I, unsigned char **z, long ix1, long ix2, double x1, double x2,
	long iy1, long iy2, double y1, double y2, unsigned char minimum, unsigned char maximum);
void Graphics_image (I, double **z, long ix1, long ix2, double x1, double x2,
	long iy1, long iy2, double y1, double y2, double minimum, double maximum);
void Graphics_image8 (I, unsigned char **z, long ix1, long ix2, double x1, double x2,
	long iy1, long iy2, double y1, double y2, unsigned char minimum, unsigned char maximum);
void Graphics_imageFromFile (I, const wchar_t *relativeFileName, double x1, double x2, double y1, double y2);
void Graphics_line (I, double x1, double y1, double x2, double y2);
void Graphics_rectangle (I, double x1, double x2, double y1, double y2);
void Graphics_fillRectangle (I, double x1, double x2, double y1, double y2);
void Graphics_roundedRectangle (I, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_fillRoundedRectangle (I, double x1, double x2, double y1, double y2, double r_mm);
void Graphics_function (I, double y [], long ix1, long ix2, double x1, double x2);   /* y [ix1..ix2] */
void Graphics_function16 (I, short y [], int stagger, long ix1, long ix2, double x1, double x2);
	/* y [ix1..ix2] or y [ix1*2..ix2*2] */
void Graphics_circle (I, double x, double y, double r);
void Graphics_fillCircle (I, double x, double y, double r);
void Graphics_circle_mm (I, double x, double y, double d);
void Graphics_fillCircle_mm (I, double x, double y, double d);
void Graphics_rectangle_mm (I, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_fillRectangle_mm (I, double x, double y, double horizontalSide_mm, double verticalSide_mm);
void Graphics_arc (I, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_fillArc (I, double x, double y, double r, double fromAngle, double toAngle);
void Graphics_ellipse (I, double x1, double x2, double y1, double y2);
void Graphics_fillEllipse (I, double x1, double x2, double y1, double y2);
void Graphics_arrow (I, double x1, double y1, double x2, double y2);
void Graphics_doubleArrow (I, double x1, double y1, double x2, double y2);
void Graphics_arcArrow (I, double x, double y, double r, double fromAngle, double toAngle, int arrowAtStart, int arrowAtEnd);
void Graphics_mark (I, double x, double y, double size_mm, const wchar_t *markString);
void Graphics_button (I, double x1, double x2, double y1, double y2);
void Graphics_innerRectangle (I, double x1, double x2, double y1, double y2);

typedef struct { double red, green, blue; } Graphics_Colour;
extern Graphics_Colour Graphics_BLACK, Graphics_WHITE, Graphics_RED, Graphics_GREEN, Graphics_BLUE,
	Graphics_CYAN, Graphics_MAGENTA, Graphics_YELLOW, Graphics_MAROON, Graphics_LIME, Graphics_NAVY, Graphics_TEAL,
	Graphics_PURPLE, Graphics_OLIVE, Graphics_PINK, Graphics_SILVER, Graphics_GREY;
const wchar_t * Graphics_Colour_name (Graphics_Colour colour);
static inline bool Graphics_Colour_equal (Graphics_Colour colour1, Graphics_Colour colour2) {
	return colour1. red == colour2. red && colour1. green == colour2. green && colour1. blue == colour2. blue;
}
void Graphics_setColour (I, Graphics_Colour colour);
void Graphics_setGrey (I, double grey);

void Graphics_xorOn (I, Graphics_Colour colour);
void Graphics_xorOff (I);
void Graphics_highlight (I, double x1, double x2, double y1, double y2);
void Graphics_unhighlight (I, double x1, double x2, double y1, double y2);
void Graphics_highlight2 (I, double x1, double x2, double y1, double y2,
	double innerX1, double innerX2, double innerY1, double innerY2);
void Graphics_unhighlight2 (I, double x1, double x2, double y1, double y2,
	double innerX1, double innerX2, double innerY1, double innerY2);

#define Graphics_NOCHANGE  -1
#define Graphics_LEFT  kGraphics_horizontalAlignment_LEFT
#define Graphics_CENTRE  kGraphics_horizontalAlignment_CENTRE
#define Graphics_RIGHT  kGraphics_horizontalAlignment_RIGHT
#define Graphics_BOTTOM  0
#define Graphics_HALF  1
#define Graphics_TOP  2
#define Graphics_BASELINE  3
void Graphics_setTextAlignment (I, int horizontal, int vertical);
void Graphics_setFont (I, enum kGraphics_font font);
void Graphics_setFontSize (I, int height);
void Graphics_setFontStyle (I, int style);
void Graphics_setItalic (I, bool onoff);
void Graphics_setBold (I, bool onoff);
void Graphics_setCode (I, bool onoff);
#define Graphics_NORMAL  0
#define Graphics_BOLD  1
#define Graphics_ITALIC  2
#define Graphics_BOLD_ITALIC  3
#define Graphics_CODE  4
void Graphics_setTextRotation (I, double angle);
void Graphics_setTextRotation_vector (I, double dx, double dy);
void Graphics_setWrapWidth (I, double wrapWidth);
void Graphics_setSecondIndent (I, double indent);
double Graphics_inqTextX (I);
double Graphics_inqTextY (I);
typedef struct { double x1, x2, y1, y2; wchar_t *name; } Graphics_Link;
int Graphics_getLinks (Graphics_Link **plinks);
void Graphics_setNumberSignIsBold (I, bool isBold);
void Graphics_setPercentSignIsItalic (I, bool isItalic);
void Graphics_setCircumflexIsSuperscript (I, bool isSuperscript);
void Graphics_setUnderscoreIsSubscript (I, bool isSubscript);
void Graphics_setDollarSignIsCode (I, bool isCode);
void Graphics_setAtSignIsLink (I, bool isLink);

void Graphics_setLineType (I, int lineType);
#define Graphics_DRAWN  0
#define Graphics_DOTTED  1
#define Graphics_DASHED  2
void Graphics_setLineWidth (I, double lineWidth);
void Graphics_setArrowSize (I, double arrorSize);

void Graphics_inqViewport (I, double *x1NDC, double *x2NDC, double *y1NDC, double *y2NDC);
void Graphics_inqWindow (I, double *x1WC, double *x2WC, double *y1WC, double *y2WC);
int Graphics_inqFont (I);
int Graphics_inqFontSize (I);
int Graphics_inqFontStyle (I);
int Graphics_inqLineType (I);
double Graphics_inqLineWidth (I);
double Graphics_inqArrowSize (I);
Graphics_Colour Graphics_inqColour (I);

void Graphics_contour (I, double **z,
	long ix1, long ix2, double x1WC, double x2WC, long iy1, long iy2, double y1WC, double y2WC, double height);
void Graphics_altitude (I, double **z,
	long ix1, long ix2, double x1, double x2, long iy1, long iy2, double y1, double y2, int numberOfBorders, double borders []);
void Graphics_grey (I, double **z,
	long ix1, long ix2, double x1, double x2, long iy1, long iy2, double y1, double y2, int numberOfBorders, double borders []);
#define Graphics_gray Graphics_grey
void Graphics_surface (I, double **z, long ix1, long ix2, double x1, double x2,
	long iy1, long iy2, double y1, double y2, double minimum, double maximum, double elevation, double azimuth);

void Graphics_setInner (I);
void Graphics_unsetInner (I);
void Graphics_drawInnerBox (I);
void Graphics_textLeft (I, bool far, const wchar_t *text);
void Graphics_textRight (I, bool far, const wchar_t *text);
void Graphics_textBottom (I, bool far, const wchar_t *text);
void Graphics_textTop (I, bool far, const wchar_t *text);
void Graphics_marksLeft (I, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRight (I, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottom (I, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTop (I, int numberOfMarks, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksLeftLogarithmic (I, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRightLogarithmic (I, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottomLogarithmic (I, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTopLogarithmic (I, int numberOfMarksPerDecade, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_markLeft (I, double yWC, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markRight (I, double yWC, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markBottom (I, double xWC, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markTop (I, double xWC, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markLeftLogarithmic (I, double y /* > 0 */, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markRightLogarithmic (I, double y, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markBottomLogarithmic (I, double x, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_markTopLogarithmic (I, double x, bool hasNumber, bool hasTick, bool hasDottedLine, const wchar_t *text);
void Graphics_marksLeftEvery (I, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksRightEvery (I, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksBottomEvery (I, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);
void Graphics_marksTopEvery (I, double units, double distance, bool haveNumbers, bool haveTicks, bool haveDottedLines);

void *Graphics_x_getCR (I);
void Graphics_x_setCR (I, void *cr);
void *Graphics_x_getGC (I);

bool Graphics_startRecording (I);
bool Graphics_stopRecording (I);
void Graphics_play (Graphics from, Graphics to);
int Graphics_writeRecordings (I, FILE *f);
int Graphics_readRecordings (I, FILE *f);
#ifdef _WIN32
int Graphics_readRecordings_oldWindows (I, FILE *f);
#endif
void Graphics_markGroup (I);
void Graphics_undoGroup (I);

double Graphics_dxMMtoWC (I, double dx_mm);
double Graphics_dyMMtoWC (I, double dy_mm);
double Graphics_distanceWCtoMM (I, double x1WC, double y1WC, double x2WC, double y2WC);
double Graphics_dxWCtoMM (I, double dxWC);
double Graphics_dyWCtoMM (I, double dyWC);

int Graphics_mouseStillDown (I);
void Graphics_waitMouseUp (I);
void Graphics_getMouseLocation (I, double *xWC, double *yWC);

void Graphics_nextSheetOfPaper (I);

/* End of file Graphics.h */
#endif
