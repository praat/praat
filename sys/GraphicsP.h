#ifndef _GraphicsP_h_
#define _GraphicsP_h_
/* GraphicsP.h
 *
 * Copyright (C) 1992-2010 Paul Boersma
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
 * pb 2010/03/11
 */

#ifndef _Graphics_h_
	#include "Graphics.h"
#endif

typedef struct {
	unsigned char link, rightToLeft;
	short style, size, baseline;
	unsigned long code;
	wchar_t kar;
	double width;
	union { long integer; const char *string; } font;
	int cell, line, run;
} _Graphics_widechar;

#include "Gui.h"

#define Graphics_members Thing_members \
	/* Device constants. */ \
	bool screen; \
		/* A boolean for whether we are a graphic screen (which may include a non-PostScript printer. */ \
	bool postScript; \
		/* A boolean for whether we are a PostScript device. */ \
	bool printer; \
		/* A boolean for whether we are a printer. */ \
	bool metafile; \
		/* A boolean for whether we are a high-resolution metafile or clipboard. */ \
	bool yIsZeroAtTheTop; \
		/* A boolean for whether vertical cooordinates increase from top to bottom (as on most screens, but not PostScript). */ \
	Widget drawingArea; \
		/* Also used as a boolean. */ \
	int resolution; \
		/* Dots per inch. */ \
	short x1DCmin, x2DCmax, y1DCmin, y2DCmax; \
		/* Maximum dimensions of the output device. */ \
		/* x1DCmin < x2DCmax; y1DCmin < y2DCmax; */ \
		/* The point (x1DCmin, y1DCmin) can be either in the top left */ \
		/* or in the bottom left, depending on the yIsZeroAtTheTop flag. */ \
		/* Device variables. */ \
	short x1DC, x2DC, y1DC, y2DC; \
		/* Current dimensions of the output device, or: */ \
		/* device coordinates of the viewport rectangle. */ \
		/* x1DCmin <= x1DC < x2DC <= x2DCmax; */ \
		/* y1DCmin <= y1DC < y2DC <= y2DCmax; */ \
		/* Graphics_create_xxxxxx sets these coordinates to */ \
		/* x1DCmin, x2DCmax, y1DCmin, and y2DCmax. */ \
		/* They can be changed by Graphics_setWsViewport. */ \
	double x1wNDC, x2wNDC, y1wNDC, y2wNDC; \
		/* Normalized device coordinates of */ \
		/* the device viewport rectangle. */ \
		/* The point (x1wNDC, y1wNDC) is always in the bottom left.	*/ \
		/* Graphics_create_xxxxxx sets these coordinates to */ \
		/* 0.0, 1.0, 0.0, and 1.0. */ \
		/* They can be changed by Graphics_setWsWindow. */ \
	double x1NDC, x2NDC, y1NDC, y2NDC; \
		/* Normalized device coordinates of the user output viewport, */ \
		/* which is a part of the device viewport rectangle. */ \
		/* x1wNDC <= x1NDC < x2NDC <= x2wNDC; */ \
		/* y1wNDC <= y1NDC < y2NDC <= y2wNDC; */ \
		/* Graphics_create_xxxxxx sets these coordinates to */ \
		/* 0.0, 1.0, 0.0, and 1.0. */ \
		/* They can be changed by Graphics_setViewport. */ \
	double x1WC, x2WC, y1WC, y2WC; \
		/* World coordinates of the user output viewport rectangle.	*/ \
		/* They bear a relation to the "real" world,			*/ \
		/* and are used in the drawing routines.			*/ \
		/* Graphics_create_xxxxxx sets these coordinates to		*/ \
		/* 0.0, 1.0, 0.0, and 1.0.					*/ \
		/* They can be changed by Graphics_setWindow.			*/ \
	double deltaX, deltaY, scaleX, scaleY; \
		/* Current coordinate transformation. */ \
	/* Graphics state. */ \
	int lineType; \
	Graphics_Colour colour; \
	double lineWidth, arrowSize; \
	int horizontalTextAlignment, verticalTextAlignment; \
	double textRotation, wrapWidth, secondIndent, textX, textY; \
	enum kGraphics_font font; \
	int fontSize, fontStyle; \
	int percentSignIsItalic, numberSignIsBold, circumflexIsSuperscript, underscoreIsSubscript; \
	int dollarSignIsCode, atSignIsLink; \
	int recording; \
	long irecord, nrecord; \
	double *record; \
	Graphics_Viewport outerViewport;   /* For Graphics_(un)setInner (). */ \
	double horTick, vertTick;   /* For Graphics_mark(s)XXX (). */ \
	double paperWidth, paperHeight;

#define Graphics_methods Thing_methods
class_create_opaque (Graphics, Thing);

int Graphics_init (I);
/*
	Postconditions:
		my font == Graphics_FONT_HELVETICA;
		my fontSize == 9;
		my fontStyle == Graphics_NORMAL;
*/ 

#define kGraphics_font_SYMBOL  (kGraphics_font_MAX + 1)
#define kGraphics_font_IPATIMES  (kGraphics_font_MAX + 2)
#define kGraphics_font_DINGBATS  (kGraphics_font_MAX + 3)

#if defined (USE_GTK)
	#define GraphicsScreen_members Graphics_members \
		cairo_t *cr;
	#define mac 0
	#define win 0
	#define xwin 0
	#define cairo 1
	#define pango 1
#elif defined (UNIX)
	#define GraphicsScreen_members Graphics_members \
		Display *display; \
		int xscreen; \
		Window rootWindow; \
		Visual *visual; \
		unsigned int depth; \
		int bitsPerPixel, pad; \
		Colormap colourMap; \
		Window window; \
		GC gc; \
		struct { \
			Window window; \
			GC gc; \
		} text;
	#define mac 0
	#define win 0
	#define xwin 1
	#define cairo 0
	#define pango 0
	extern unsigned long xwinColour_BLACK, xwinColour_WHITE, xwinColour_PINK, xwinColour_BLUE, xwinGreys [101];
#elif defined (_WIN32)
	#include <windowsx.h>
	#define GraphicsScreen_members Graphics_members \
		HWND window; \
		HDC dc; \
		COLORREF foregroundColour; \
		HPEN pen; \
		HBRUSH brush; \
		int duringXor, fatNonSolid;
	#define mac 0
	#define win 1
	#define xwin 0
	#define cairo 0
	#define pango 0
#elif defined (macintosh)
	#include "macport_on.h"
	#include <Quickdraw.h>
	#include <MacWindows.h>
	#include "macport_off.h"
	#define GraphicsScreen_members Graphics_members \
		GrafPtr macPort; \
		int macFont, macStyle; \
		int depth, duringXor; \
		RGBColor macColour; \
		bool useQuartz; \
		CGContextRef macGraphicsContext;
	#define mac 1
	#define win 0
	#define xwin 0
	#define cairo 0
	#define pango 0
#endif

#define GraphicsScreen_methods Graphics_methods
class_create (GraphicsScreen, Graphics);

#define GraphicsPostscript_members Graphics_members \
	FILE *file;	 \
	int (*printf) (void *stream, const char *format, ...); \
	int languageLevel; \
	int photocopyable, spotsDensity, spotsAngle, landscape, includeFonts, loadedXipa, useSilipaPS; \
	double magnification; \
	char *fontInfos [1 + kGraphics_font_DINGBATS] [1 + Graphics_BOLD_ITALIC]; \
	const char *lastFid; \
	int job, eps, pageNumber, lastSize;
#define GraphicsPostscript_methods Graphics_methods
class_create (GraphicsPostscript, Graphics);

/* Opcodes for recording. */

double * _Graphics_check (Graphics me, long number);
#define put(f)  * ++ p = (double) (f)
#define op(opcode,number)  double *p = _Graphics_check (me, number); if (! p) return; put (opcode); put (number)
#define mput(n,a)  { double *f = a; for (long l = 0; l < n; l ++) put (f [l]); }
#define sput(s,l)  { put (l); strcpy ((char *) (p + 1), s); p += l; }

/* When adding opcodes in the following list, add them at the end. */
/* Otherwise, old picture files will become incompatible with the current Graphics. */
enum opcode { SET_VIEWPORT = 101, SET_INNER, UNSET_INNER, SET_WINDOW,
	/* 105 */ TEXT, POLYLINE, LINE, ARROW, FILL_AREA, FUNCTION, RECTANGLE, FILL_RECTANGLE,
	/* 113 */ CIRCLE, FILL_CIRCLE, ARC, ARC_ARROW, HIGHLIGHT, CELL_ARRAY,
	/* 119 */ SET_FONT, SET_FONT_SIZE, SET_FONT_STYLE,
	/* 122 */ SET_TEXT_ALIGNMENT, SET_TEXT_ROTATION,
	/* 124 */ SET_LINE_TYPE, SET_LINE_WIDTH,
	/* 126 */ SET_STANDARD_COLOUR, SET_GREY,
	/* 128 */ MARK_GROUP, ELLIPSE, FILL_ELLIPSE, CIRCLE_MM, FILL_CIRCLE_MM, IMAGE8,
	/* 134 */ UNHIGHLIGHT, XOR_ON, XOR_OFF, RECTANGLE_MM, FILL_RECTANGLE_MM,
	/* 139 */ SET_WS_WINDOW, SET_WRAP_WIDTH, SET_SECOND_INDENT,
	/* 142 */ SET_PERCENT_SIGN_IS_ITALIC, SET_NUMBER_SIGN_IS_BOLD,
	/* 144 */ SET_CIRCUMFLEX_IS_SUPERSCRIPT, SET_UNDERSCORE_IS_SUBSCRIPT,
	/* 146 */ SET_DOLLAR_SIGN_IS_CODE, SET_AT_SIGN_IS_LINK,
	/* 148 */ BUTTON, ROUNDED_RECTANGLE, FILL_ROUNDED_RECTANGLE, FILL_ARC,
	/* 152 */ INNER_RECTANGLE, CELL_ARRAY8, IMAGE, HIGHLIGHT2, UNHIGHLIGHT2,
	/* 157 */ SET_ARROW_SIZE, DOUBLE_ARROW, SET_RGB_COLOUR, IMAGE_FROM_FILE
};

void _Graphics_text_init (I);
void _Graphics_fillRectangle (I, short x1DC, short x2DC, short y1DC, short y2DC);
void _Graphics_setColour (I, Graphics_Colour colour);
void _Graphics_setGrey (I, double grey);
void _Graphics_colour_init (I);
bool _GraphicsMac_tryToInitializeAtsuiFonts (void);

#ifdef macintosh
	void GraphicsQuartz_initDraw (GraphicsScreen me);
	void GraphicsQuartz_exitDraw (GraphicsScreen me);
#endif

/* End of file GraphicsP.h */
#endif
