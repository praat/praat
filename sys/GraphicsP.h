#ifndef _GraphicsP_h_
#define _GraphicsP_h_
/* GraphicsP.h
 *
 * Copyright (C) 1992-2011,2012 Paul Boersma, 2013 Tom Naughton
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

#include "Graphics.h"
#include "Gui.h"

#if defined (_WIN32)
	#include <windowsx.h>
#elif defined (macintosh)
	#include "macport_on.h"
    #if useCarbon
        #include <Carbon/Carbon.h>
    #endif
	#include "macport_off.h"
#endif

void Graphics_init (Graphics me);
/*
	Postconditions:
		my font == Graphics_FONT_HELVETICA;
		my fontSize == 9;
		my fontStyle == Graphics_NORMAL;
*/ 

#define kGraphics_font_SYMBOL  (kGraphics_font_MAX + 1)
#define kGraphics_font_IPATIMES  (kGraphics_font_MAX + 2)
#define kGraphics_font_IPAPALATINO  (kGraphics_font_MAX + 3)
#define kGraphics_font_DINGBATS  (kGraphics_font_MAX + 4)

Thing_define (GraphicsScreen, Graphics) {
	// new data:
	public:
		#if defined (UNIX)
			GdkDisplay *d_display;
			GdkDrawable *d_window;
			GdkGC *d_gdkGraphicsContext;
			cairo_t *d_cairoGraphicsContext;
		#elif defined (_WIN32)
			HWND d_winWindow;
			HDC d_gdiGraphicsContext;
			COLORREF d_winForegroundColour;
			HPEN d_winPen;
			HBRUSH d_winBrush;
			bool d_fatNonSolid;
			bool d_useGdiplus;
		#elif defined (macintosh)
            #if useCarbon
                GrafPtr d_macPort;
            #else
                NSView *d_macView;
            #endif
			int d_macFont, d_macStyle;
			int d_depth;
			RGBColor d_macColour;
			CGContextRef d_macGraphicsContext;
		#endif
	// overridden methods:
	protected:
		virtual void v_destroy ();
		virtual void v_polyline (long numberOfPoints, long *xyDC, bool close);
		virtual void v_fillArea (long numberOfPoints, long *xyDC);
		virtual void v_rectangle (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_fillRectangle (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_circle (double xDC, double yDC, double rDC);
		virtual void v_ellipse (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_arc (long xDC, long yDC, long rDC, double fromAngle, double toAngle);
		virtual void v_fillCircle (long xDC, long yDC, long rDC);
		virtual void v_fillEllipse (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_button (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_roundedRectangle (long x1DC, long x2DC, long y1DC, long y2DC, long r);
		virtual void v_arrowHead (long xDC, long yDC, double angle);
		virtual bool v_mouseStillDown ();
		virtual void v_getMouseLocation (double *xWC, double *yWC);
		virtual void v_flushWs ();
		virtual void v_clearWs ();
		virtual void v_updateWs ();
};

#if defined (UNIX)
	#define mac 0
	#define win 0
	#define cairo 1
	#define pango 1
#elif defined (_WIN32)
	#define mac 0
	#define win 1
	#define cairo 0
	#define pango 0
#elif defined (macintosh)
	#define mac 1
	#define win 0
	#define cairo 0
	#define pango 0
#endif

Thing_define (GraphicsPostscript, Graphics) {
	// new data:
	public:
		FILE *d_file;
		int (*d_printf) (void *stream, const char *format, ...);
		int languageLevel;
		int photocopyable, spotsDensity, spotsAngle, landscape, includeFonts, loadedXipa, useSilipaPS;
		double magnification;
		char *fontInfos [1 + kGraphics_font_DINGBATS] [1 + Graphics_BOLD_ITALIC];
		const char *lastFid;
		int job, eps, pageNumber, lastSize;
	// overridden methods:
		virtual void v_destroy ();
		virtual void v_polyline (long numberOfPoints, long *xyDC, bool close);
		virtual void v_fillArea (long numberOfPoints, long *xyDC);
		virtual void v_rectangle (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_fillRectangle (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_circle (double xDC, double yDC, double rDC);
		virtual void v_ellipse (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_arc (long xDC, long yDC, long rDC, double fromAngle, double toAngle);
		virtual void v_fillCircle (long xDC, long yDC, long rDC);
		virtual void v_fillEllipse (long x1DC, long x2DC, long y1DC, long y2DC);
		virtual void v_arrowHead (long xDC, long yDC, double angle);
};

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
	/* 157 */ SET_ARROW_SIZE, DOUBLE_ARROW, SET_RGB_COLOUR, IMAGE_FROM_FILE,
	/* 161 */ POLYLINE_CLOSED
};

void _GraphicsScreen_text_init (GraphicsScreen me);
void _Graphics_fillRectangle (Graphics me, long x1DC, long x2DC, long y1DC, long y2DC);
void _Graphics_setColour (Graphics me, Graphics_Colour colour);
void _Graphics_setGrey (Graphics me, double grey);
void _Graphics_colour_init (Graphics me);
bool _GraphicsMac_tryToInitializeAtsuiFonts (void);

#ifdef macintosh
	void GraphicsQuartz_initDraw (GraphicsScreen me);
	void GraphicsQuartz_exitDraw (GraphicsScreen me);
#endif

/* End of file GraphicsP.h */
#endif
